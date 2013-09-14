$ "$TOG: dtsession.msg /main/18 1999/09/20 13:49:48 mgreess $";
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

$set 2
$    THIS COMMENT FOR DTSESSION TEAM ONLY -- SmMain.c --
$ These are errorlog messages
$ DO NOT TRANSLATE THIS SET.

2 Another dtsession is currently running - exiting.
3 Screen lock when the screen saver is turned on is not available on this server.  Your screen will not be locked when the screen saver is turned on. 

$set 4
$    THIS COMMENT FOR DTSESSION TEAM ONLY -- SmGlobals.c --
$ This entire set is for the errorlog.
$ DO NOT TRANSLATE THIS SET.

1 Invalid display name - exiting.
2 No defaults files exist.  No applications will be restarted.
3 No defaults files exist.  No applications will be restarted.
4 Unable to remove session directory.  Make sure write permissions exist \
on $HOME/.dt directory.  Invalid session files will not be removed.
5 Unable to malloc memory for operation.
6 Unable to lock display.  Another application may have the pointer or \
keyboard grabbed.
7 Unable to open session file.  No clients will be restarted.
8 Unable to fork client process.
9 Unable to create DT directories.  Check permissions on home directory.
10 Unable to lock display due to security restrictions.

$ The next message is added to the error log if a required temporary 
$ file can not be created by the session manager. 
$ The insert is the name of the temporary file. 
$quote "
11 "Unable to create the temporary file: %s. Make sure write permissions exist for this file. Exiting from Session Manager."

$set 6
$    THIS COMMENT FOR DTSESSION TEAM ONLY -- SmMsgSrv.c --
$ This entire set is for the errorlog.
$ DO NOT TRANSLATE THIS SET.

$quote
1 "Could not connect to ToolTalk message server:\n%sExiting ...\n"
2 The DT message server has exited.


$set 8
$    THIS COMMENT FOR DTSESSION TEAM ONLY -- SmError.c --
$ This entire set is for the errorlog.
$ DO NOT TRANSLATE THIS SET.

1 Connection to server lost - exiting.


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

1 Unable to exec process /usr/bin/X11/xrdb.  No session resources will be \
restored.
2 Invalid client settings file.  No settings restored.
3 Unable to exec %s.
4 Unable to exec process /usr/dt/bin/dtwm.  No window manager will be started.
5 You have reached the maximum allowed number of remote clients.  No further remote clients will be restored.
6 Invalid display/language independent resource file.  No display/language independent resources will be restored.
7 The wmStartupCommand resource is set to:\n\n\
      %s\n\n\
This file does not exist or is not executable.\n\
/usr/dt/bin/dtwm will be started instead.\n
8 An attempt to restore the following\n\
command (using the DT remote execution process)\n\
on host "%s" failed:\n\n\
      %s\n\n\
The following execution string will be tried:\n\n\
   %s\n\n
9 The following window manager did not start:\n\n\
      %s\n\n\
This message indicates you tried to start a\n\
window manager that is built into an X terminal.\n\
This will only work with X terminals that support this protocol.\n\
/usr/dt/bin/dtwm will be started instead.\n


$set 18
$    THIS COMMENT FOR DTSESSION TEAM ONLY -- SmUI.c -- 
$ LOCALIZE Set 18
$ Deleted Messages:  13, 14, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28,
$						29, 30, 31, 32, 33

$ Message 1 tells the user who currently has the display locked.
1 Schermo bloccato dall'utente %s.

$ Message 2 tells the user to enter the password for the above user to unlock
$ the display.
2 Per sbloccare lo schermo occorre inserire la parola chiave.

$ Message 3 is a prompt for the user to enter a password.
3 Parola chiave: 

$ Message 4 is the logout message that appears when dtsession is going
$ to save the Current session information. 
$ Access this message by pressing the logout button.
$ Message 4 is the logout message that appears when dtsession is going
$ to save the current session information. 
$ Access this message by pressing the logout button.
$ 4 Avvio del processo di logout...\n\n\
$ I dati non salvati verranno probabilmente persi.\n\n\
$ La sessione corrente verrà salvata; tuttavia, dopo il login\n\n\
$ potrà essere necessario riavviare alcune applicazioni.
4 Uscita dalla sessione del desktop...\n\n\
La sessione iniziale verrà ripristinata al prossimo login.\n\
La sessione corrente non verrà salvata.\n\n\
Le modifiche non salvate verranno perse.\n\n\
Continuare il processo di logout?

$ Message 5 is the logout message that appears when dtsession is not going
$ to save the Current session information - but return to the Home session info
$ 5 Avvio del processo di logout...\n\n\
$ I dati non salvati verranno probabilmente persi.\n\n\
$ La sessione corrente non verrà salvata. Al prossimo\n\n\
$ login verrà ripristinata la sessione iniziale.
5 Uscita dalla sessione del desktop...\n\n\
La sessione iniziale verrà ripristinata al prossimo\n\
login. La sessione corrente non verrà salvata.\n\n\
Le modifiche non salvate verranno perse.\n\n\
Continuare il processo di logout?

$ Message 6 is the title of the logout dialog box.
6 Conferma del logout

$ Messages 7,8,9,10 have been OBSOLETED.
$ Do NOT translate messages 7, 8, 9 and 10
$ Message 7,8,9,10 are all part of the logout dialog that appears when
$ the user has requested to have the choice
$ to either save the current session or return to the home session.
$ 7 OBS- Avvio del processo di logout...\n\n\
$ Sono disponibili due opzioni per l'avvio del desktop al prossimo login:\n\n\
$ Salvare la sessione corrente e riavviarla nella sua forma attuale.\n\n\
$ oppure\n\n\
$ Abbandonare la sessione corrente e ripristinare la sessione iniziale salvata.\n\n\
$ In entrambi i casi, i dati non salvati verranno probabilmente persi.
$ 8 OBS- Logout:\nSalvare la sessione corrente
$ 9 OBS- Logout:\nRipristinare la sessione iniziale
$ Message 10 is the title of the logout dialog box.
$ 10 OBS- Conferma del logout

$ Message 11,12 are part of the error dialog that appears when the DT
$ messaging system dies.
$ These messages are generated when you kill the BMS (broadcast Message
$ Server), which is /usr/softbench/bin/softmsgsrv.
$ Message 11 is obsolete.  Message 38 replaces message 11

$ 11 DT Messaging System Inoperative\n\n\
$ To restart:\n\n\
$ 1) Save all open data files.\n\
$ 2) Press the reset key sequence to return to the login screen.\n\
$ 3) Log in again.\n\n\
$ Note: The current session will not be saved.\n\n\
$ When you are ready to begin the restart process, click [OK] and\n\
$ proceed to save your files.

$ 12 Errore Message
12 Errore nel sistema di messaggi

$ Messages 15, 16, 17 are used for identical purpose as messages 1, 2,3
$ in this set.
15 Schermo bloccato dall'utente %s.
16 Per sbloccare lo schermo è necessario inserire la parola chiave.
17 Parola chiave: 

$ Message 34, 35 are messages that appear when the user started up in 
$ compatibility mode, and tries to log out.  You can only exit out of 
$ compatibility mode by using the reset key sequence.
$ To generate this message, start in x11 (not DT) and then run dtsession.
$ Refer to the dtsession manpage.  Then try to log out.
34 Questa sessione è stata avviata da uno script\ndel sistema di visualizzazione delle finestre.\n\n\
Le informazioni della sessione non verranno salvate.\n\nEseguire il logout usando la combinazione di tasti di ripristino.

$ 35 Logout Message
35 Messaggio Logout

$ Messages 36,37 are part of the error dialog that appears when dtsession is
$ unable to start the DT messaging system, and therefore not able to bring
$ up the system at all.
$ To generate this, move the message server from
$ /usr/softbench/bin/softmsgsrv to some other directory, then start Dt.
$ 36 Non è stato possibile avviare il sistema di messaggi DT.\n\n\
$ Per correggere il problema:\n\n\
$ 1.  Scegliere [OK] per ritornare allo schermo di login.\n\n\
$ 2.  Selezionare Sessione protetta dal menu Opzioni dello\n\
$      schermo di login ed eseguire il login.\n\n\
$ 3.  Verificare che il nome host sia corretto nelle seguenti posizioni:\n\
$      /etc/src.sh\n\
$      /etc/hosts\n\
$      /usr/adm/inetd.sec\n\n\
$ Per ulteriori informazioni, vedere il Manuale dell'utente DT.
36 Non è stato possibile avviare il sistema di messaggi DT.\n\n\
Per correggere il problema:\n\n\
1.  Scegliere [OK] per ritornare allo schermo di login.\n\n\
2.  Selezionare Sessione protetta dal menu Opzioni dello\n\
         schermo di login ed eseguire il login.\n\n\
3.  Verificare che il desktop sia installato correttamente,\n\
         che il nome host sia corretto (/etc/hosts) e che la rete\n\
         sia configurata appropriatamente.\n\n\
Per ulteriori informazioni, consultare il Manuale dell'utente del CDE.

37 Azione richiesta

$ Message 38 is part of the error dialog that appears when the DT
$ messaging system dies.
$ This messages are generated when you kill the BMS (broadcast Message
$ Server), which is /usr/softbench/bin/softmsgsrv.

38 Il sistema di messaggi non è operativo.\n\n\
Per riavviarlo:\n\n\
1) Salvare tutti i file di dati aperti.\n\
2) Eseguire il logout.\n\
3) Eseguire nuovamente il login.\n\n\
Nota: La sessione corrente non verrà salvata.\n\n\
Quando si è pronti per iniziare il processo di riavvio,\n\
fare clic su [OK] e procedere al salvataggio dei file.

$ 
$ Messages 39-41 are displayed on the logout and/or session selection
$  confirmation dialogs.  These message are for dialog buttons
$ 
$ 39 "Continuer la déconnexion"
$ 40 "Annuler la déconnexion"
39 OK
40 Annullare
41 Aiuto

$ 
$ Messages 50-53 are for the display-specific session confirmation dialog
$ 
50 Inziale
51 Corrente
$ Message 52 - The first '%s' in message 52 will be replaced with message 
$   50 or 51. The second '%'s in message 52 will be replaced with the 
$   value of the $DISPLAY environment variable (e.g. hostname:0).
52 Una sessione '%s' è stata selezionata per un video\n\
inesistente:'%s'. \n\n\
Se si continua, una nuova sessione specifica per il video\n\
verrà creata.
53 Avvertimento

$ 
$ Message 55-56 are for the Session Save error dialog
$   55 is the dialog title
$   56 is the message in the dialog - do NOT localize 'save_state'
$ 
55 Errore nel salvataggio della sessione
56 Non è possibile salvare la sessione: \n\
il messaggio non contiene il parametro 'save_state'.\n\n\
La sessione non verrà salvata.

$ 
$ Messages 70-71 refer to the name of the System Administration manual
$ 
70 Per ulteriori informazioni, consultare:
71 Manuale del CDE per utenti esperti e amministratori di sistema.

$ 
$ Message 75 is used during logout if the user started a Current
$   session but used the Style Manager to return to a Home session.
$ 
75 Uscita dalla sessione del desktop...\n\n\
La sessione corrente verrà salvata però la sessione iniziale\n\
verrà ripristinata al prossimo login.\n\n\
Le modifiche non salvate verranno perse.\n\n\
Continuare il processo di logout?

$ 
$ Message 76 is used at the same time message 36 is used except that
$   message 76 is for Digital/OSF only and is slightly different than
$   message 36.
$ 
76 Non è stato possibile avviare il sistema di messaggi DT.\n\n\
Per correggere il problema:\n\n\
1.  Scegliere [OK] per ritornare allo schermo di login.\n\n\
2.  Selezionare Sessione protetta dal menu Opzioni dello\n\
         schermo di login ed eseguire il login.\n\n\
3.  Verificare che il vostro hostname sia corretto in /etc/hosts\n\
    se la vostra rete è già stata configurata.\n\
4.  Se la vostra rete non è ancora stata configurata, verificare\n\
    che /etc/hosts contenga la seguente voce:\n\
    127.0.0.1 localhost \n\n\
Per ulteriori informazioni, consultare il Manuale dell'utente del CDE.
$ 
$ Message 99 is displayed when a user logs in for the first time
$ 
99 Il nuovo file '.dtprofile' è stato aggiunto alla vostra\n\
directory iniziale.\n\
\n\
Seguire le istruzioni in questo file in modo da accertarsi\n\
che quando eseguite il prossimo login il file '.login' o\n\
'.profile' sia attivato e che funzioni correttamente con il CDE.\n\
\n\
Per ulteriori informazioni, consultare:\n\
Manuale del CDE per utenti esperti e amministratori di sistema.



$set 20
$    THIS COMMENT FOR DTSESSION TEAM ONLY -- SmWindow.c --
$ This entire set is for the errorlog.
$ DO NOT TRANSLATE THIS SET.

1 Client message failed.  Client information will not be saved.
2 Invalid root window.  Can not save client information.
3 Session restoration information not updated for client .  Invalid \
information may be saved.
4 Session restoration information not updated for client %s.  Invalid \
information may be saved.


$set 22
$    THIS COMMENT FOR DTSESSION TEAM ONLY -- SmLock.c --


$set 24
$    THIS COMMENT FOR DTSESSION TEAM ONLY -- version.c --


$set 26
$    THIS COMMENT FOR DTSESSION TEAM ONLY -- SrvPalette.c --
$ This entire set is for the errorlog.
$ DO NOT TRANSLATE THIS SET.

1 Color Server Warning: couldn't get ownership of the selection:
2 Color Server Warning, losing ownership of the selection:
3 Should never lose the selection.
4 Warning, found more pixels then are available.
5 Warning, can't allocate enough pixels.


$set 28
$    THIS COMMENT FOR DTSESSION TEAM ONLY -- SrvFile_io.c --
$ This entire set is for the errorlog.
$ DO NOT TRANSLATE THIS SET.
$ Messages 1,2,6,7,8,9 are obsolete

$ 1 Color Server Warning, the size of file '
$ 2 ' is invalid\nremoving file and trying again.
$ 6 Color Server FatalError, could not make directory
$ 7 Color Server FatalError: couldn't open or create
$ 8 Color Server FatalError, couldn't open file
$ 9 Color Server FatalError, couldn't open system file

3 Color Server Warning, the size of file is invalid:
4 Color Server Warning, invalid information in '
5 ' removing file and starting again.\n
10 Color Server FatalError, not enough memory to create the structure for \
a palette.\n
11 Warning: Color Server system default file is corrupt\n
12 Color Server Warning, could not read file %s\n


$set 29
$    THIS COMMENT FOR DTSESSION TEAM ONLY -- SmHelp.c -- 
$ LOCALIZE Set 29
1 Aiuto della Gestione delle sessioni


$ $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
$ 
$ Set 40 is for messages related to the X11R6 Session Management Protocol
$ 
$   o Do NOT localize the string '%s' in any of the messages in this set.
$ 
$set 40
$ Messages 1-7 are used when a session cannot be started.
$ Message 1 - this message will preface messages 2-7
1 Impossibile avviare una sessione a causa\n\
del seguente errore:
2 SmsInitialize fallito.
3 IceListenForConnections fallito.

$ 
$ Do not remove the whitespace at the beginning of lines 2 and 3
$   for message number 4
$ 
4 Non è stato possibile creare il file di autenticazione ~/.ICEauthority.\n\
   Se i file ~/.ICEauthority-c e ~/.ICEauthority-l esistono,\n\
   devono essere rimossi prima di avviare la sessione.

5 IceAddConnectionWatch fallito.
6 IceComposeNetworkIds fallito.
$ Message 7 - Do NOT translate "SESSION_MANAGER".
7 Impossibile creare la variabile d'ambiente SESSION_MANAGER.
$ 
$ Messages 8-9 occur when a session database cannot be opened.
$ 
$ Message 8 - Do NOT remove the whitespace before the last '%s'.
8 Impossibile aprire il seguente database della sessione:\n\n\
   '%s'
$ Message 9 - Do NOT remove the whitespace before the last '%s'.
9 I seguenti database non contengono la\n\
  risorsa richiesta '%s':\n\n\
   %s
$ 
$ Messages 10-11 are used to report errors when reading a session database
$ 
$ Message 10 - Do NOT remove the whitespace before the last '%s'.
10 La caratteristica richiesta '%s' è mancante per il client\n\n\
   %s\n\n\
in nel file '%s'.
11 La caratteristica richiesta '%s' è mancante per il client '%d'\n\
nel file '%s'.
$ 
$ Messages 12-14 used if errors occur when a session is restarted
$ 
$ Message 12 - Do NOT remove the whitespace before the last '%s'.
12 La directory '%s'\n\
non è disponibile per l'applicazione '%s'.\n\n\
Si utilizzerà la directory seguente:\n\n\
   %s
$ Message 13 - Do NOT remove the whitespace before the last '%s'.
13 Non è possibile avviare l'applicazione seguente sull'host '%s'.\n\
Non è possibile accedere a questo host dall'host '%s':\n\n\
   %s
$ Message 14 - Do NOT remove the whitespace before the third '%s'.
$ Message 14 - "cannonical form" in this context means an internal
$   format.  
14 Il tentativo di avviare l'applicazione'%s' su\n\
host '%s' è fallito perché non è stato possibile\n\
tradurre in formato interno la seguente directory:\n\n\
   %s\n\n\
[%s]

$ 
$ Message 15 - used if the -session command line option has no value
$ 
15 Non è stato fornito un nome della sessione per l'opzione -session.
$ 
$ Message 16 - used if the Save_Session message contains an unrecognized
$    argument
$ 
16 Il messaggio Save_Session contiene un argomento sconosciuto: '%s'.

17 Non è stato possibile ottenere un elenco dei client dalla 'Gestione finestre'.
$ 
$ Message 18 - the client database cannot be opened for writing.
$ Do NOT remove the whitespace before the last '%s'.
$ 
18 Impossibile aprire il seguente database per la scrittura:\n\n\
   %s\n\n\
La sessione non verrà salvata.
$ 
$ Message 19 - used if an application cannot be saved
$ 
19 Impossibile salvare l'applicazione '%s'.
$ 
$ Message 20 - used if the IceAcceptConnection function fails:
$ 
20 La funzione IceAcceptConnection è fallita.
$ 
$ Message 21 - used if a Save Session is requested and one is already
$    in progress
21 Salvataggio sessione è già in corso. La sessione non verrà salvata.

22 Il processo di logout è stato annullato dall'applicazione '%s'.

$ 
$ Messages 23-25 are used when a session is saved and an application 
$    has not set its required properties
$ 
$ Message 23 - Do NOT remove the whitespace before the last '%s'.
23 L'applicazione seguente non ha impostato la caratteristica\n\
richiesta '%s':\n\n\
   %s\n\n\
Questa applicazione non verrà salvata.

24 L'applicazione '%s' non verrà salvata perché non ha impostato\n\
la caratteristica richiesta '%s'.

25 L'applicazione '%s' non ha impostato la caratteristica richiesta '%s'.

$ 
$ Messages 26-27 are used when the DiscardCommand properties are
$    invoked - before an old session directory is removed.
$ Message 26 - do NOT remove the whitespace before the '%s'.
$ Message 26 - do NOT translate the word 'DiscardCommand' in
$    this message
26 Impossibile aprire il database client:\n\n\
   %s\n\n\
Impossibile eseguire 'DiscardCommand'.

$ 
$ Message 27 - do NOT translate the word 'DiscardCommand' in
$    this message
27 Non è stato possibile eseguire la caratteristica 'DiscardCommand'\n\
per l'applicazione '%s'.

$ 
$ Messages 28-29 - used when an application exists and it wants the
$    Session Manager to post a dialog with a message that describes
$    the reason the client exited.
$ Message 29 is the title for the dialog
$ 
28 L'applicazione '%s' si è interrotta a causa di:\n\n\
%s
29 Cause dell'interruzione dell'applicazione

$set 99
$    DO NOT TRANSLATE THESE MESSAGES
$    DO NOT CHANGE THESE MESSAGES
$    DO NOT LOCALIZE THESE MESSAGES
$    These messages are used for the version information.
$quote "
1 "@(#)version_goes_here"
2 "\n@(#)Message catalog source $TOG: dtsession.msg /main/18 1999/09/20 13:49:48 mgreess $"

