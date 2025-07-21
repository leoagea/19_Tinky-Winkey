#include "tinky.h"

/*#############################################################################
# Service Management Methods
#############################################################################*/

void Install() 
{
	SC_HANDLE hSCManager = NULL;
	SC_HANDLE hService = NULL;
	TCHAR executablePath[MAX_PATH];

	if (!GetModuleFileName(NULL, executablePath, MAX_PATH)) {
		std::cout << "GetModuleFileName failed: " << GetErrorMessage() << std::endl;
		exit(1);
	}
	// std::cout << "Executable Path: " << executablePath << std::endl;

	hSCManager = OpenSCManager(
		NULL,                    // local computer
		NULL,                    // ServicesActive database 
		SC_MANAGER_ALL_ACCESS);  // full access rights 
	
	if (hSCManager == NULL) {
		std::cout << "OpenSCManager failed: " << GetErrorMessage() << std::endl;
		exit(1);
	}

	hService = OpenService(hSCManager, SERVICE_NAME, SERVICE_ALL_ACCESS);
	 if (hService != NULL) {
		std::cout << "Service already exists." << std::endl;
		CloseServiceHandle(hService);
		CloseServiceHandle(hSCManager);
		exit(1);
	} else if (GetLastError() != ERROR_SERVICE_DOES_NOT_EXIST) {
		// Some other error occurred
		std::cout << "OpenService failed: " << GetErrorMessage() << std::endl;
		CloseServiceHandle(hSCManager);
		exit(1);
	}

	hService = CreateService(
		hSCManager,              // SCManager database
		SERVICE_NAME,            // name of service
		SERVICE_DISPLAY_NAME,     // service name to display
		SERVICE_ALL_ACCESS,      // desired access
		SERVICE_WIN32_OWN_PROCESS, // service type
		SERVICE_DEMAND_START,    // start type
		SERVICE_ERROR_NORMAL,    // error control type
		executablePath, // path to service binary
		NULL,                    // no load ordering group
		NULL,                    // no tag identifier
		NULL,                    // no dependencies
		NULL,                    // LocalSystem account
		NULL);                   // no password
	
	if (hService == NULL) {
		std::cout << "CreateService failed: " << GetErrorMessage() << std::endl;
		CloseServiceHandle(hSCManager);
		exit(1);
	}

	std::cout << "Service successfully installed." << std::endl;
	CloseServiceHandle(hService);
	CloseServiceHandle(hSCManager);
}

void Start() 
{
	SC_HANDLE hSCManager = NULL;
	SC_HANDLE hService = NULL;

	hSCManager = OpenSCManager(
		NULL,                    // local computer
		NULL,                    // ServicesActive database 
		SC_MANAGER_ALL_ACCESS);  // full access rights

	if (hSCManager == NULL) {
		std::cout << "OpenSCManager failed: " << GetErrorMessage() << std::endl;
		exit(1);
	}

	hService = OpenService(hSCManager, SERVICE_NAME, SERVICE_START);
	if (hService == NULL) {
		std::cout << "OpenService failed: " << GetErrorMessage() << std::endl;
		CloseServiceHandle(hSCManager);
		exit(1);
	}

	if (!StartService(hService, 0, NULL))
		std::cout << "StartService failed: " << GetErrorMessage() << std::endl;
	else
		std::cout << "Service started successfully." << std::endl;

	CloseServiceHandle(hService);
	CloseServiceHandle(hSCManager);
}

void Stop() 
{
	SC_HANDLE hSCManager = NULL;
	SC_HANDLE hService = NULL;
	SERVICE_STATUS serviceStatus;

	hSCManager = OpenSCManager(
		NULL,                    // local computer
		NULL,                    // ServicesActive database 
		SC_MANAGER_ALL_ACCESS);  // full access rights

	if (hSCManager == NULL) {
		std::cout << "OpenSCManager failed: " << GetErrorMessage() << std::endl;
		exit(1);
	}

	hService = OpenService(hSCManager, SERVICE_NAME, SERVICE_STOP | SERVICE_QUERY_STATUS);
	if (hService == NULL) {
		std::cout << "OpenService failed: " << GetErrorMessage() << std::endl;
		CloseServiceHandle(hSCManager);
		exit(1);
}

	if (!ControlService(hService, SERVICE_CONTROL_STOP, &serviceStatus)) {
		std::cout << "ControlService failed: " << GetErrorMessage() << std::endl;
	}

	std::cout << "Service stopped successfully." << std::endl;
	CloseServiceHandle(hService);
	CloseServiceHandle(hSCManager);
}

void Delete() 
{
	SC_HANDLE hSCManager = NULL;
	SC_HANDLE hService = NULL;

	hSCManager = OpenSCManager(
		NULL,                    // local computer
		NULL,                    // ServicesActive database 
		SC_MANAGER_ALL_ACCESS);  // full access rights

	if (hSCManager == NULL) {
		std::cout << "OpenSCManager failed: " << GetErrorMessage() << std::endl;
		exit(1);
	}

	hService = OpenService(hSCManager, SERVICE_NAME, SERVICE_ALL_ACCESS);

	if (hService == NULL) {
		std::cout << "OpenService failed: " << GetErrorMessage() << std::endl;
		CloseServiceHandle(hSCManager);
		exit(1);
	}

	if (!DeleteService(hService))
		std::cout << "DeleteService failed: " << GetErrorMessage() << std::endl;
	else
		std::cout << "Service deleted successfully." << std::endl;

	CloseServiceHandle(hService);
	CloseServiceHandle(hSCManager);
}

