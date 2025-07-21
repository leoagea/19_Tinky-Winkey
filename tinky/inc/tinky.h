#pragma once

/*#############################################################################
# Includes
#############################################################################*/

#include <iostream>
#include <string>
#include <cstdlib>   // For malloc/free
#include <unordered_map>
#include <functional> // For std::function
#include <string_view> // For std::string_view

// Windows API includes
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsvc.h>  // For Service Control Manager functions
#include <tchar.h>   // For TCHAR string functions
#include <libloaderapi.h>

/*#############################################################################
# Defines
#############################################################################*/

#define SERVICE_NAME _T("tinky")  // Name of the service
#define SERVICE_DISPLAY_NAME _T("tinky")  // Display name of the service

/*#############################################################################
# Class Tinky
#############################################################################*/

#pragma pack(push, 1)  // Ensure 1-byte packing for compatibilit
class Tinky {
public:
	Tinky();
	~Tinky();

	void Install();
	void Start();
	void Stop();
	void Delete();

private:
	LPCTSTR GetErrorMessage();

	SC_HANDLE hSCManager;           // Handle to Service Control Manager
	SC_HANDLE hService;             // Handle to the service (if needed in future operations)
	TCHAR executablePath[MAX_PATH];  // Path to the service executable (allocated dynamically)
};
