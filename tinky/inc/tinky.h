#pragma once

/*#############################################################################
# Includes
#############################################################################*/

#include <iostream>
#include <string>
#include <cstdlib>   // For malloc/free
#include <unordered_map>
#include <functional> // For std::function
#include <string_view> // For std::string_view and std::wstring_view
#include <vector>    // For std::vector
#include <sstream>   // For std::istringstream

// Windows API includes
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsvc.h>  // For Service Control Manager functions
#include <tchar.h>   // For TCHAR string functions
#include <libloaderapi.h>
#include <shellapi.h> // For CommandLineToArgvW>

/*#############################################################################
# Defines
#############################################################################*/

#define SERVICE_NAME _T("tinky")  // Name of the service
#define SERVICE_DISPLAY_NAME _T("tinky")  // Display name of the service

/*#############################################################################
# Service Management Methods / tinky.cpp
#############################################################################*/

void Install();
void Start();
void Stop();
void Delete();

/*#############################################################################
# Service Implementation Methods / svc.cpp
#############################################################################*/

void MainService(DWORD argc, LPTSTR *argv);

/*#############################################################################
# Utils Methods / Utils.cpp
#############################################################################*/

LPCTSTR GetErrorMessage();