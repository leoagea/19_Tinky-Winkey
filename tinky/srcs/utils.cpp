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