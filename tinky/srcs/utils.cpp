#include "tinky.h"

LPCTSTR GetErrorMessage() 
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

DWORD GetProcessIdByName(const std::string& processName) 
{
	PROCESSENTRY32 pe32 = {};  // Zero-initialize to avoid padding issues
	pe32.dwSize = sizeof(PROCESSENTRY32);

	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnapshot == INVALID_HANDLE_VALUE) {
		return 0;
	}

	if (Process32First(hSnapshot, &pe32)) {
		do {
			if (_stricmp(pe32.szExeFile, processName.c_str()) == 0) {
				DWORD pid = pe32.th32ProcessID;
				CloseHandle(hSnapshot);
				return pid;
			}
		} while (Process32Next(hSnapshot, &pe32));
	}

	CloseHandle(hSnapshot);
	return 0;
}

void DebugLogToFile(const std::string& message) {
	std::ofstream logFile("C:\\Users\\Leo\\Desktop\\tinky_winkey\\debug.log", std::ios::app);
	if (logFile.is_open()) {
		auto now = std::chrono::system_clock::now();
		auto time_t = std::chrono::system_clock::to_time_t(now);
		std::tm tm;
		localtime_s(&tm, &time_t);

		logFile << std::put_time(&tm, "[%Y-%m-%d %H:%M:%S] ") << message << std::endl;
		logFile.close();
	}
}