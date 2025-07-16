🧠 BUT GLOBAL DU PROJET

Créer deux programmes en C/C++ :

    tinky.exe → Un service Windows SYSTEM

    winkey.exe → Un keylogger discret

L’objectif est que le service tinky contrôle entièrement le keylogger winkey, de sa création à sa destruction, avec privilèges SYSTEM, en toute autonomie, comme le ferait un logiciel espion professionnel (dans un cadre éducatif).
🟩 tinky — Le Service Windows (en C++)
🎯 Objectif

Gérer le cycle de vie complet du keylogger winkey.exe via le mécanisme de service Windows, en utilisant les API du Service Control Manager (SCM).
Ce service tourne en arrière-plan et doit :

    s’installer/désinstaller

    se démarrer/s’arrêter

    lancer le keylogger avec les privilèges SYSTEM

    empêcher plusieurs instances

    tuer le keylogger quand on stoppe/supprime le service

🛠 Fonctionnalités attendues
🎛 Commandes disponibles

Tu dois pouvoir appeler tinky (svc.exe) depuis la console avec les commandes suivantes :

svc.exe install   -> crée le service {tinky}
svc.exe start     -> démarre le service
svc.exe stop      -> arrête le service
svc.exe delete    -> supprime le service

🔄 Fonctionnement technique détaillé
1. svc.exe install → Création du service

    OpenSCManager() → accès au SCM

    CreateService() → enregistre un service nommé tinky

        Type : SERVICE_WIN32_OWN_PROCESS

        Démarrage manuel

        Lien vers le binaire svc.exe

➡️ Résultat : le service tinky est visible dans services.msc et peut être démarré.
2. svc.exe start → Démarrage du service

    StartService() → lance ServiceMain, le point d'entrée du service

Dans ServiceMain :

    Le service :

        Impersonne un token SYSTEM :

            Il cherche un processus SYSTEM (winlogon.exe)

            Ouvre son token avec OpenProcessToken

            Crée un jeton duplicable avec DuplicateTokenEx, de type Primary

        Lance winkey.exe avec ce token SYSTEM :

            Utilise CreateProcessAsUser() pour exécuter winkey en SYSTEM

            Le keylogger tourne en arrière-plan, avec tous les privilèges

        Garde son état actif (boucle ou attente d’un stop signal)

        Peut gérer une PID globale ou un mutex pour empêcher plusieurs winkey

3. svc.exe stop → Arrêt du service

    Reçoit un SERVICE_CONTROL_STOP via ControlService()

    Appelle un handler (ServiceCtrlHandler) pour le nettoyage :

        Tuer winkey.exe s’il tourne (via TerminateProcess ou FindProcess)

        Libérer les handles, token, etc.

    Le service se désenregistre proprement

4. svc.exe delete → Suppression

    Appelle DeleteService() via OpenService()

    S’assure que winkey.exe est bien tué si encore en cours

    Supprime toute trace du service

🟩 winkey — Le Keylogger bas niveau (en C)
🎯 Objectif

Capturer les frappes clavier de tous les utilisateur·ices en lien avec :

    La fenêtre au premier plan

    Le nom du processus actif

    La date/heure

    Le tout dans un fichier winkey.log, en texte lisible

🔍 Fonctionnement technique
1. Installation d’un hook clavier bas niveau

    SetWindowsHookEx(WH_KEYBOARD_LL, HookProc, NULL, 0)

    Cette fonction intercepte toutes les frappes clavier, même hors processus

2. À chaque frappe détectée (HookProc) :

    Vérifie l’état (key down ou up)

    Récupère la fenêtre active avec GetForegroundWindow()

    Récupère le PID de cette fenêtre avec GetWindowThreadProcessId()

    Ouvre le processus → récupère le nom du programme

    Utilise ToUnicodeEx() pour convertir le code clavier en caractère lisible

        Selon la locale active (ex : AZERTY, QWERTY)

3. Écrit dans un fichier winkey.log :

[24.06.2025 17:43:12] - 'Visual Studio Code'
CtrlShiftP

➡️ Le fichier est en texte brut, lisible, et peut être lu/analysé plus tard ou envoyé à distance (bonus).
🔒 Pourquoi il doit être lancé par tinky ?

    Pour fonctionner, le keylogger doit être injecté dans le contexte SYSTEM

    Sinon, il n’aura pas accès à certaines frappes, notamment :

        des utilisateurs non administrateurs

        des sessions protégées

        des fenêtres en élévation UAC

🔗 🔄 Comment tinky et winkey sont liés
Élément	tinky.exe	winkey.exe
Type	Service SYSTEM	Programme espion clavier
Exécution	En tant que service	En tant que processus enfant SYSTEM
Lancement	Lance winkey.exe avec CreateProcessAsUser()	Démarre sa propre boucle de hook
Contrôle	Installe/stoppe/tue winkey.exe	Ne tourne que s’il est lancé
Privilèges	SYSTEM	SYSTEM, hérités du parent
Communication	Indirecte via logs, mutex, PID	Peut écrire dans winkey.log
🗂 Exemple de cycle d'exécution complet
1. Installation

svc.exe install

➡️ Le service tinky est enregistré
2. Démarrage

svc.exe start

➡️ tinky :

    Impersonne SYSTEM (à partir de winlogon.exe)

    Lance winkey.exe avec les droits SYSTEM

    winkey commence à logger toutes les frappes

3. Consultation du log

type winkey.log

➡️ Affiche les frappes par timestamp et programme
4. Arrêt et nettoyage

svc.exe stop
svc.exe delete

➡️ winkey.exe est tué automatiquement
➡️ Le service est supprimé du système
🧠 Concepts Windows que tu manipules dans ce projet
Concept	Pourquoi important ?
Service Windows	Permet de tourner en SYSTEM et en arrière-plan
Token d’accès (access token)	Permet d’exécuter un processus sous une autre identité
DuplicateTokenEx	Crée un nouveau token SYSTEM utilisable
CreateProcessAsUser	Lance winkey avec privilèges SYSTEM
WH_KEYBOARD_LL hook	Capture des frappes clavier à bas niveau
GetForegroundWindow	Permet de savoir quelle application est utilisée