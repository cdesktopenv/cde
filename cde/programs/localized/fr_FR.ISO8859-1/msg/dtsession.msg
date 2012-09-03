$ "$TOG: dtsession.msg /main/17 1999/09/20 13:49:20 mgreess $";  
$ *                                                                      *
$ * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
$ * (c) Copyright 1993, 1994 International Business Machines Corp.       *
$ * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
$ * (c) Copyright 1993, 1994 Novell, Inc.                                *
$ *
$ *************************************<+>*************************************  
$ *****************************************************************************  
$ **  
$ **  File:        dtsession.msg  
$ **  
$ **  Project:     HP DT  Session Manager (dtsession)  
$ **  
$ **  Description:  
$ **  -----------  
$ **  This file is the source for the message catalog for dtsession  
$ **  
$ **  
$ *****************************************************************************  
$ **  
$ **     (c) Copyright 1990 Hewlett-Packard Company  
$ **     All Rights reserved  
$ **  
$ **  
$ *****************************************************************************  
$ *************************************<L>*************************************  
$ **  -----------------GENERAL LOCALIZATION NOTES SECTION---------------------  
$ * Comment lines begin with a $ except $set which indicates the start of a   
$ * new set.  
$ *  
$ * Do not delete any comments; you may add comments for your use.  
$ *  
$ * Do not localize sets 2, 4, 6, 8, 10, 12, 14, 16.  
$ * Do not localize sets 20, 22, 24, 26, 28, 99.    
$ * They only appear in the errorlog.  
$ *  
$ * DO localize sets 18 and 29.   
$ **  ----------------GENERAL LOCALIZATION NOTES SECTION END------------------  
$ *************************************<L>*************************************  
$quote "

$set 2
$    THIS COMMENT FOR DTSESSION TEAM ONLY -- SmMain.c --  
$ These are errorlog messages  
$ DO NOT TRANSLATE THIS SET.  

2 "Another dtsession is currently running - exiting."

3 "Screen lock when the screen saver is turned on is not available on this server.  Your screen will not be locked when the screen saver is turned on. "

$set 4

$    THIS COMMENT FOR DTSESSION TEAM ONLY -- SmGlobals.c --  
$ This entire set is for the errorlog.  
$ DO NOT TRANSLATE THIS SET.  

1 "Invalid display name - exiting."

2 "No defaults files exist.  No applications will be restarted."

3 "No defaults files exist.  No applications will be restarted."

4 "Unable to remove session directory.  Make sure write permissions exist on $HOME/.dt directory.  Invalid session files will not be removed."

5 "Unable to malloc memory for operation."

6 "Unable to lock display.  Another application may have the pointer or keyboard grabbed."

7 "Unable to open session file.  No clients will be restarted."

8 "Unable to fork client process."

9 "Unable to create DT directories.  Check permissions on home directory."

10 "Unable to lock display due to security restrictions."

$ The next message is added to the error log if a required temporary 
$ file can not be created by the session manager. 
$ The insert is the name of the temporary file. 
11 "Impossible de créer le fichier temporaire: %s. Assurez-vous que vous disposez du droit d'écriture sur ce fichier. Sortie du Gestionnaire de sessions..."

$set 6

$    THIS COMMENT FOR DTSESSION TEAM ONLY -- SmMsgSrv.c --  
$ This entire set is for the errorlog.  
$ DO NOT TRANSLATE THIS SET.  

1 "Could not connect to ToolTalk message server:\n%sExiting ...\n"

2 "The DT message server has exited."

$set 8

$    THIS COMMENT FOR DTSESSION TEAM ONLY -- SmError.c --  
$ This entire set is for the errorlog.  
$ DO NOT TRANSLATE THIS SET.  

1 "Connection to server lost - exiting."

$set 10

$    THIS COMMENT FOR DTSESSION TEAM ONLY -- SmProperty.c --  

$set 12

$    THIS COMMENT FOR DTSESSION TEAM ONLY -- SmProtocol.c --  

$set 14

$    THIS COMMENT FOR DTSESSION TEAM ONLY -- SmSave.c --  

$set 16

$    THIS COMMENT FOR DTSESSION TEAM ONLY -- SmRestore.c --  
$ This entire set is for the errorlog.  
$ DO NOT TRANSLATE THIS SET.  

1 "Unable to exec process /usr/bin/X11/xrdb.  No session resources will be restored."

2 "Invalid client settings file.  No settings restored."

3 "Unable to exec %s."

4 "Impossible d'exécuter /usr/bin/X11/dtwm. Pas de lancement du gestionnaire de fenêtres."

5 "You have reached the maximum allowed number of remote clients.  No further remote clients will be restored."

6 "Invalid display/language independent resource file.  No display/language independent resources will be restored."

7 "The wmStartupCommand resource is set to:\n\n\
      %s\n\n\
This file does not exist or is not executable.\n\
/usr/dt/bin/dtwm will be started instead.\n"

8 "An attempt to restore the following\n\
command (using the DT remote execution process)\n\
on host \"%s\" failed:\n\n\
      %s\n\n\
The following execution string will be tried:\n\n\
   %s\n\n"

9 "The following window manager did not start:\n\n\
      %s\n\n\
This message indicates you tried to start a\n\
window manager that is built into an X terminal.\n\
This will only work with X terminals that support this protocol.\n\
/usr/dt/bin/dtwm will be started instead.\n"

$set 18

$    THIS COMMENT FOR DTSESSION TEAM ONLY -- SmUI.c --   
$ LOCALIZE Set 18  
$ Deleted Messages:  13, 14, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28,  
$ \t\t\t\t\t29, 30, 31, 32, 33  
$ Message 1 tells the user who currently has the display locked.  

1 "Ecran verrouillé par l'utilisateur %s."
$ Message 2 tells the user to enter the password for the above user to unlock  
$ the display.  

2 "Entrez le mot de passe pour déverrouiller."
$ Message 3 is a prompt for the user to enter a password.  

3 "Mot de passe:"
$ Message 4 is the logout message that appears when dtsession is going  
$ to save the current session information.   
$ Access this message by pressing the logout button.  

$ Message 4 is the logout message that appears when dtsession is going
$ to save the Current session information. 
$ Access this message by pressing the logout button.
$ 4 "Processus de déconnexion amorcé...\n\n\
$ 4 Les données non sauvegardées risquent d'être perdues.\n\n\
$ 4 La session en cours va être sauvegardée. Cependant, vous devrez\n\n\
$ 4 peut-être relancer certaines applications après la reconnexion."
$ 4 $ Message 5,6 is the logout message that appears when dtsession is not going  
$ 4 $ to save the current session information - but return to the home session info  
$quote
4 Processus de déconnexion amorcé...\n\n\
La session en cours sera restaurée lors de la prochaine connexion.\n\n\
Les données non sauvegardées risquent d'être perdues.\n\n\
Continuer la déconnexion ?

$ Message 5 is the logout message that appears when dtsession is not going
$ to save the Current session information - but return to the Home session info
$ 5 "Processus de déconnexion amorcé...\n\n\
$ 5 Les données non sauvegardées risquent d'être perdues.\n\n\
$ 5 La session en cours n'est pas sauvegardée. La session initiale\n\n\
$ 5 sera restaurée lors de la prochaine connexion."
5 Processus de déconnexion amorcé...\n\n\
La session initiale sera restaurée lors de la prochaine connexion.\n\
La session en cours ne sera pas sauvegardée.\n\n\
Les données non sauvegardées risquent d'être perdues.\n\n\
Continuer la déconnexion?

$ Message 6 is the title of the logout dialog box.  

$quote "
6 "Confirmation de déconnexion"
$ Message 7,8,9,10 are all part of the logout dialog that appears when  
$ the user has requested to have the choice  
$ to either save the current session or return to the home session.  

$ Messages 7,8,9,10 have been OBSOLETED.
$ Do NOT translate messages 7, 8, 9 and 10
$ 7 OBS- "Processus de déconnexion amorcé...\n\n\
$ Lors de la prochaine connexion, vous pourrez:\n\n\
$ sauvegarder la session en cours et la relancer en l'état,\n\n\
$ ou\n\n\
$ supprimer la session en cours afin de restaurer la session initiale.\n\n\
$ Les données non sauvegardées risquent d'être perdues."

$ 8 OBS- "Déconnexion:\n\
$ Sauveg. session en cours"

$ 9 OBS- "Déconnexion:\n\
$ Restaur. session initiale"
$ Message 10 is the title of the logout dialog box.  

$ 10 OBS- "Confirmation de déconnexion"

$ Message 11,12 are part of the error dialog that appears when the DT  
$ messaging system dies.  
$ These messages are generated when you kill the BMS (broadcast Message  
$ Server), which is /usr/softbench/bin/softmsgsrv.  
$ Message 11 is obsolete.  Message 38 replaces message 11  
$ 11 DT Messaging System Inoperative 
$  
$ \

$ To restart: 
$  
$ \

$ 1) Save all open data files. 
$ \

$ 2) Press the reset key sequence to return to the login screen. 
$ \

$ 3) Log in again. 
$  
$ \

$ Note: The current session will not be saved. 
$  
$ \

$ When you are ready to begin the restart process, click [OK] and 
$ \

$ proceed to save your files.  

$quote

$ 12 "Message Failure"
12 Panne de message

$ Messages 15, 16, 17 are used for identical purpose as messages 1, 2,3  
$ in this set.  

$quote "
15 "Ecran verrouillé par l'utilisateur %s."

16 "Entrez le mot de passe pour déverrouiller."

17 "Mot de passe:"
$ Message 34, 35 are messages that appear when the user started up in   
$ compatibility mode, and tries to log out.  You can only exit out of   
$ compatibility mode by using the reset key sequence.  
$ To generate this message, start in x11 (not DT) and then run dtsession.  
$ Refer to the dtsession manpage.  Then try to log out.  

34 "Cette session a été lancée à partir d'un fichier de lancement\n\
du système X Window.\n\n\
Les données de la session ne seront pas sauvegardées.\n\n\
Utilisez la séquence de touches de restauration pour sortir."

$quote

$ 35 "Logout Message"
35 Message de déconnexion

$ Messages 36,37 are part of the error dialog that appears when dtsession is  
$ unable to start the DT messaging system, and therefore not able to bring  
$ up the system at all.  
$ To generate this, move the message server from  
$ /usr/softbench/bin/softmsgsrv to some other directory, then start Dt.  

$ 36 "Le système de messagerie du Bureau CDE n'a pas pu être lancé.\n\n\
$ Pour remédier à cet incident:\n\n\
$ 1.  Sélectionnez [OK] pour revenir à l'écran de connexion.\n\n\
$ 2.  Dans le menu d'ouverture, sélectionnez Session monofenêtre\n\
$     sous Options et ouvrez une session.\n\n\
$ 3.  Vérifiez que le nom d'hôte est exact dans les fichiers:\n\
$      /etc/src.sh\n\
$      /etc/hosts\n\
$      /usr/adm/inetd.sec\n\n\
$ Pour plus de détails, reportez-vous au Guide d'utilisation du Bureau CDE."
36 Le système de messagerie du Bureau n'a pas pu être lancé.\n\n\
Pour remédier à cet incident :\n\n\
1. Sélectionnez [OK] pour revenir à l'écran de connexion.\n\n\
2. Dans le menu d'ouverture, sélectionnez Session monofenêtre \n\
 sous Options et ouvrez une session.\n\n\
3.  Vérifiez que le bureau est installé correctement,\n\
 que le nom d'hôte est exact (/etc/hosts) et que la\n\
         configuration du réseau est correcte.\n\n\
Pour plus de détails, reportez-vous au Guide de l'utilisateur CDE.

$quote "
37 "Intervention nécessaire"
$ Message 38 is part of the error dialog that appears when the DT  
$ messaging system dies.  
$ This messages are generated when you kill the BMS (broadcast Message  
$ Server), which is /usr/softbench/bin/softmsgsrv.  

38 "Système de messages du Bureau CDE hors fonction.\n\n\
Pour le relancer:\n\n\
1) Sauvegardez tous les fichiers de données ouverts.\n\
2) Déconnectez-vous.\n\
3) Reconnectez-vous.\n\n\
Remarque: La session en cours ne sera pas sauvegardée.\n\n\
Quand vous êtes prêt à exécuter le processus de relance,\n\
cliquez sur [OK] et commencez à sauvegarder vos fichiers."

$ 
$ Messages 39-41 are displayed on the logout and/or session selection
$  confirmation dialogs.  These message are for dialog buttons
$ 
$quote

$ 39 "Continuer la déconnexion"
$ 40 "Annuler la déconnexion"
39 OK
40 Annuler
41 Aide

$ 
$ Messages 50-53 are for the display-specific session confirmation dialog
$ 
50 Initiale
51 En cours
$ Message 52 - The first '%s' in message 52 will be replaced with message 
$   50 or 51. The second '%'s in message 52 will be replaced with the 
$   value of the $DISPLAY environment variable (e.g. hostname:0).
52 Une session display-specific '%s' a été sélectionnée pour l'écran \n\
'%s' mais elle n'existe pas.\n\n\
Si vous continuez, une nouvelle session display-specific sera créée.
53 Avertissement

$ 
$ Message 55-56 are for the Session Save error dialog
$   55 is the dialog title
$   56 is the message in the dialog - do NOT localize 'save_state'
$ 
55 Sauvegarder la panne de session
56 Impossible de sauvegarder la session parce que le paramètre\n\
'save_state'  ne fai pas partie du message.\n\n\
La session sera perdue.

$ 
$ Messages 70-71 refer to the name of the System Administration manual
$ 
70 Pour plus de détails, consultez :
71 Le guide de l'administrateur du bureau CDE et le guide de l'utilisateur

$ 
$ Message 75 is used during logout if the user started a Current
$   session but used the Style Manager to return to a Home session.
$ 
75 Processus de déconnexion amorcé...\n\n\
La session en cours sera sauvegardée mais votre session initiale\n\n\
sera restaurée lors de la prochaine connexion.\n\n\
Les données non sauvegardées risquent d'être perdues.\n\n\
Continuer la déconnexion ?

$ 
$ Message 76 is used at the same time message 36 is used except that
$   message 76 is for Digital/OSF only and is slightly different than
$   message 36.
$ 
76 Le système de messagerie DT n'a pas pu être lancé.\n\n\
Pour remédier à cet incident :\n\n\
1. Sélectionnez [OK] pour revenir à l'écran de connexion.\n\n\
2. Dans le menu d'ouverture, sélectionnez Session monofenêtre \n\
 sous Options et ouvrez une session.\n\n\
3.  Vérifiez que le bureau est installé correctement,\n\
 que le nom d'hôte est exact (/etc/hosts) si le réseau \n\
         a été configuré.\n\n\
4.  Si votre réseau n'a pas été configuré, assurez-vous que/etc/hosts\n\
     y est installé sous le nom suivant :\n\
127.0.0.1 localhost \n\n\
Pour plus de détails, reportez-vous au Guide de l'utilisateur CDE.


$ 
$ Message 99 is displayed when a user logs in for the first time
$ 
99 Le nouveau fichier '.dtprofile' a été ajouté dans votre répertoire home.\n\
\n\
Suivez les instructions données dans ce fichier pour vous assurer que \n\
lors de la prochaine connexion votre fichier '.login' our '.profile' sera activé\n\
tet qu'il fonctionnera correctement au sein de CDE. \n\
\n\
Pour plus de détails, consultez le Guide de l'utilisateur CDE\n\
et le Guide de l'administrateur.


$set 20

$    THIS COMMENT FOR DTSESSION TEAM ONLY -- SmWindow.c --  
$ This entire set is for the errorlog.  
$ DO NOT TRANSLATE THIS SET.  

$quote "
1 "Client message failed.  Client information will not be saved."

2 "Invalid root window.  Can not save client information."

3 "Session restoration information not updated for client .  Invalid information may be saved."

4 "Session restoration information not updated for client %s.  Invalid information may be saved."

$set 22

$    THIS COMMENT FOR DTSESSION TEAM ONLY -- SmLock.c --  

$set 24

$    THIS COMMENT FOR DTSESSION TEAM ONLY -- version.c --  

$set 26

$    THIS COMMENT FOR DTSESSION TEAM ONLY -- SrvPalette.c --  
$ This entire set is for the errorlog.  
$ DO NOT TRANSLATE THIS SET.  

1 "Color Server Warning: couldn't get ownership of the selection:"

2 "Color Server Warning, losing ownership of the selection:"

3 "Should never lose the selection."

4 "Warning, found more pixels then are available."

5 "Warning, can't allocate enough pixels."

$set 28

$    THIS COMMENT FOR DTSESSION TEAM ONLY -- SrvFile_io.c --  
$ This entire set is for the errorlog.  
$ DO NOT TRANSLATE THIS SET.  
$ Messages 1,2,6,7,8,9 are obsolete  
$ 1 Color Server Warning, the size of file '  
$ 2 ' is invalid 
$ removing file and trying again.  
$ 6 Color Server FatalError, could not make directory  
$ 7 Color Server FatalError: couldn't open or create  
$ 8 Color Server FatalError, couldn't open file  
$ 9 Color Server FatalError, couldn't open system file  

3 "Color Server Warning, the size of file is invalid:"

4 "Color Server Warning, invalid information in '"

5 "' removing file and starting again.\n"

10 "Color Server FatalError, not enough memory to create the structure for a palette.\n"

11 "Warning: Color Server system default file is corrupt\n"

12 "Color Server Warning, could not read file %s\n"

$set 29

$    THIS COMMENT FOR DTSESSION TEAM ONLY -- SmHelp.c --   
$ LOCALIZE Set 29  

1 "Gestionnaire de sessions - Aide"


$ $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
$ 
$ Set 40 is for messages related to the X11R6 Session Management Protocol
$ 
$   o Do NOT localize the string '%s' in any of the messages in this set.
$ 
$set 40
$ Messages 1-7 are used when a session cannot be started.
$ Message 1 - this message will preface messages 2-7
$quote

1 Impossible de lancer une session à cause \n\
de l'erreur suivante :
2 SmsInitialize a échoué.
3 IceListenForConnections a échoué.

$ 
$ Do not remove the whitespace at the beginning of lines 2 and 3
$   for message number 4
$ 
4 Impossible de créer le fichier d'authentication ~/.ICEauthority.\n\
   Si les fichiers ~/.ICEauthority-c et~/.ICEauthority-l existent,\n\
   vous devez les supprimer avant de lancer votre session.

5 IceAddConnectionWatch a échoué.
6 IceComposeNetworkIds a échoué.
$ Message 7 - Do NOT translate "SESSION_MANAGER".
7 Impossible de créer la variable d'environnement SESSION_MANAGER
$ 
$ Messages 8-9 occur when a session database cannot be opened.
$ 
$ Message 8 - Do NOT remove the whitespace before the last '%s'.
8 Impossible d'ouvrir la base de données de session suivante :\n\n\
   '%s'
$ Message 9 - Do NOT remove the whitespace before the last '%s'.
9 La base de données de session suivante ne contient pas\n\
la ressource exigée '%s':\n\n\
   %s
$ 
$ Messages 10-11 are used to report errors when reading a session database
$ 
$ Message 10 - Do NOT remove the whitespace before the last '%s'.
10 La propriété obligatoire '%s' est manquante pour le client\n\n\
   %s\n\n\
dans le fichier '%s'.
11 La ressource obligatoire '%s' est manquante pour le client '%d'\n\
dans le fichier '%s'.
$ 
$ Messages 12-14 used if errors occur when a session is restarted
$ 
$ Message 12 - Do NOT remove the whitespace before the last '%s'.
12 Le répertoire'%s'\n\
n'existe pas dans l'application '%s'.\n\n\
Vous utiliserez le répertoire suivant :\n\n\
   %s
$ Message 13 - Do NOT remove the whitespace before the last '%s'.
13 Impossible de lancer l'application suivante sur l'hôte '%s'\n\
parce que vous ne pouvez pas joindre ce dernier à partir de l'hôte '%s':\n\n\
   %s
$ Message 14 - Do NOT remove the whitespace before the third '%s'.
$ Message 14 - "cannonical form" in this context means an internal
$   format.  
14 Tentative de lancement de l'application '%s sur l'hôte'\n\
'%s' a échoué parce qu'il est impossible de traduire le répertoire \n\
suivant en format interne :\n\n\
   %s\n\n\
[%s]

$ 
$ Message 15 - used if the -session command line option has no value
$ 
15 Aucun nom de session pour l'option de la ligne de commande de session.
$ 
$ Message 16 - used if the Save_Session message contains an unrecognized
$    argument
$ 
16 Le message Save_Session contient un argument '%s' inconnu.

17 La tentative d'obtenir la liste des clients dans la fenêtre 'Gestionnaire de fenêtres' a échoué.
$ 
$ Message 18 - the client database cannot be opened for writing.
$ Do NOT remove the whitespace before the last '%s'.
$ 
18 Impossible d'ouvrir le fichier de de base de données suivant en écriture :\n\n\
   %s\n\n\
Impossible de sauvegarder la session.
$ 
$ Message 19 - used if an application cannot be saved
$ 
19 Impossible de sauvegarder l'application '%s'.
$ 
$ Message 20 - used if the IceAcceptConnection function fails:
$ 
20 IceAcceptConnection a échoué.
$ 
$ Message 21 - used if a Save Session is requested and one is already
$    in progress
21 Inpossible de sauvegarder la session parce qu'une session Sauvegarder est en cours.

22 L'application '%s'. a annulé la fermeture de la session

$ 
$ Messages 23-25 are used when a session is saved and an application 
$    has not set its required properties
$ 
$ Message 23 - Do NOT remove the whitespace before the last '%s'.
23 L'application suivante n'a pas défini la propriétéd\n\
obligatoire '%s':\n\n\
   %s\n\n\
Impossible de sauvegarder cette application.

24 Impossible de sauvegarder l'application '%s' parce qu'elle n'a pas défini \n\
la propriété obligatoire '%s'.

25 L'application '%s' n;a pas défini la propriété obligatoire '%s'.

$ 
$ Messages 26-27 are used when the DiscardCommand properties are
$    invoked - before an old session directory is removed.
$ Message 26 - do NOT remove the whitespace before the '%s'.
$ Message 26 - do NOT translate the word 'DiscardCommand' in
$    this message
26 Impossible d'ouvrir la base de données suivante :\n\n\
   %s\n\n\
Impossible d'exécuter les propriétés 'DiscardCommand'.

$ 
$ Message 27 - do NOT translate the word 'DiscardCommand' in
$    this message
27 Une tentative d'exécution de la propriété 'DiscardCommand'\n\
pour l'application '%s' a échoué.

$ 
$ Messages 28-29 - used when an application exists and it wants the
$    Session Manager to post a dialog with a message that describes
$    the reason the client exited.
$ Message 29 is the title for the dialog
$ 
28 L'application '%s' a fermé pour la raison suivante :\n\n\
%s
29 Raisons de la fermeture de l'application

$set 99

$    DO NOT TRANSLATE THESE MESSAGES  
$    DO NOT CHANGE THESE MESSAGES  
$    DO NOT LOCALIZE THESE MESSAGES  
$    These messages are used for the version information.  

$quote "
1 "@(#)version_goes_here"

2 "\n\
@(#)Message catalog source $TOG: dtsession.msg /main/17 1999/09/20 13:49:20 mgreess $"
