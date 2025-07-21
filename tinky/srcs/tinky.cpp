#include "tinky.h"

/*#############################################################################
# Class Tinky Implementation
#############################################################################*/

/*#############################################################################
# Public Methods
#############################################################################*/

Tinky::Tinky() : hSCManager(NULL), hService(NULL)
{
    // Initialize the executable path to an empty string
    executablePath[0] = '\0';
}

Tinky::~Tinky() 
{
}

void Tinky::Install() 
{
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

    std::cout << "Successfully opened Service Control Manager" << std::endl;
    CloseServiceHandle(hService);
    CloseServiceHandle(hSCManager);
}

void Tinky::Start() 
{
    // Start the service
}

void Tinky::Stop() 
{
    // Stop the service
}

void Tinky::Delete() 
{
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

/*#############################################################################
# Private Methods
#############################################################################*/

LPCTSTR Tinky::GetErrorMessage() 
{
    static TCHAR errorMessage[256];

    DWORD errorCode = GetLastError();

    FormatMessage(
        FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        errorCode,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        errorMessage,
        sizeof(errorMessage) / sizeof(TCHAR),
        NULL);

    return errorMessage;
}