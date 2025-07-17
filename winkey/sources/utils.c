#include "winkey.h"


void    hideWindow(void)
{
    HWND    stealth;

    stealth = FindWindowA("ConsoleWindowClass", NULL);
    ShowWindow(stealth, SW_HIDE);
}