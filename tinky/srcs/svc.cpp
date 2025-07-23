#include "tinky.h"

SERVICE_STATUS g_ServiceStatus = {0};
SERVICE_STATUS_HANDLE g_ServiceStatusHandle = NULL;
HANDLE g_ServiceStopEvent = INVALID_HANDLE_VALUE;

/*#############################################################################
# Service Implementation Methods
#############################################################################*/

static void WorkerThreadRoutine()
{
	DebugLogToFile("Worker thread started.");

	DWORD pid = GetProcessIdByName("winlogon.exe");
	if (pid == 0) {
		DebugLogToFile("Failed to get process ID for winlogon.exe");
		return;
	}

	DebugLogToFile("Winlogon.exe, PID: " + std::to_string(pid));

	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
	if (hProcess == NULL) {
		DebugLogToFile("Failed to open process for winlogon.exe");
		return;
	}

	HANDLE hToken = NULL;
	OpenProcessToken(hProcess, TOKEN_ALL_ACCESS, &hToken);
	if (hToken == NULL) {
		DebugLogToFile("Failed to open process token for winlogon.exe");
		CloseHandle(hProcess);
		return;
	}

	HANDLE hDuplicatedToken = NULL;
	if (DuplicateTokenEx(hToken, TOKEN_ALL_ACCESS, NULL, SecurityImpersonation, TokenPrimary, &hDuplicatedToken) == 0) {
		std::stringstream errorMessage;
		errorMessage << "Failed to duplicate token for winlogon.exe: " << GetErrorMessage();
		DebugLogToFile(errorMessage.str());
		CloseHandle(hToken);
		CloseHandle(hProcess);
		return;
	}

	if (!ImpersonateLoggedOnUser(hDuplicatedToken)) {
		std::stringstream errorMessage;
		errorMessage << "Failed to impersonate logged on user: " << GetErrorMessage();
		DebugLogToFile(errorMessage.str());
		CloseHandle(hDuplicatedToken);
		CloseHandle(hToken);
		CloseHandle(hProcess);
		return;
	}

	
	CloseHandle(hDuplicatedToken);
	CloseHandle(hToken);
	CloseHandle(hProcess);
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
			// TODO: perform cleanup threads and processes here


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
		return;
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
	WorkerThreadRoutine(); // Start worker thread or perform service tasks

	WaitForSingleObject(g_ServiceStopEvent, INFINITE);

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
