
$ "$XConsortium: dtlogin.msg /main/10 1996/12/20 15:13:33 rws $"

$ *************************************<+>*************************************
$ *****************************************************************************
$ **
$ **  File:        dtlogin.msg
$ **
$ **  Project:     CDE dtlogin
$ **
$ **  Description:
$ **  -----------
$ **  This file is the source for the message catalog for dtlogin
$ **
$ **
$ *****************************************************************************
$ **
$ **     (c) Copyright 1990 Hewlett-Packard Company
$ **     All Rights reserved
$ **
$ **
$ *****************************************************************************
$ **
$ **
$ **  ------------------------- MODIFICATION RECORD --------------------------
$ *
$ * 
$ **  ----------------------- MODIFICATION RECORD END ------------------------
$ *****************************************************************************
$ *************************************<+>*************************************

$ *************************************<L>*************************************
$ **  -----------------GENERAL LOCALIZATION NOTES SECTION---------------------
$ * Comment lines begin with a $ except $set which indicates the start of a 
$ * new set.
$ *
$ * Do not delete any comments; you may add comments for your use.
$ *
$ **  ----------------GENERAL LOCALIZATION NOTES SECTION END------------------
$ *************************************<L>*************************************

$set 1
$ set 1 is labels
$ 
$ messages 1 through 6 are button labels on the login screen

1 Inserire il nome utente
2 Inserire la parola chiave
3 OK
4 Ricominciare
5 Opzioni
6 Aiuto

$ 
$ messages 7 through 11 are options button menu items
$ 

7 Ripristinare schermo di login
8 Login della riga comandi
9 Informazioni su...
10 Sessione a finestra singola
11 Lingua

$ 
$ messages 12 through 14 are miscellaneous button labels
$ 
12 Avviare DT
13 Annullare
14 Aiuto


$ 
$ message 15, 17 are more option button menu items 
$ message 16 is the welcome message
$ 
15 Sessione DT Lite
16 Benvenuti in
17 Scrivania normale

$ 
$ message 18-24
$ 
18 * Sospensione del login del CDE...\r\n*\r\n
19 * Premere [Enter] per visualizzare la richiesta di login.\r\n*\r\n
20 * Eseguire il login. Il login del CDE riapparirà dopo il logout.\r\n*\r\n
21 * Ripresa del login del CDE in corso...\r\n*\r\n
22 * Avvio del login del CDE sullo schermo %1$s...\r\n*\r\n
23 * Attendere lo schermo di login del CDE.\r\n*\r\n
24 * Il server X delle finestre non può essere avviato sullo schermo %1$s...\r\n*\r\n

$ messages 25 is a options button menu item
25 Sessione

$ 
$ 26 replaces 16 as the greeting after a user name has been 
$ entered. The %s is replaced by the user name.
$ 
26 Benvenuto, %s

$ 
$ 27 replaces 10 as the label for the failsafe option menu item
$ 
27 Sessione protetta

$ 
$  Sun specific message catalog id numbers in MC_LABEL_SET messages.
$  (Starting at a random uneven higher number, 73, to allow some room  
$  for some shared sample source message additions. 
$ 
73 Desktop più recente
$ 
$  Following used in Language menu.  
$  "to" used in phrases like "ja to zh" locales
74 a


$set 2
$ set 2 is error messages
$ 

1 Login errato; riprovare.
2 Impossibile passare alla directory iniziale.
3 Spiacente. È già stato effettuato il numero massimo di login consentiti.
$ message 4 is a lengthy message that should be localized to correspond to 
$ the language.  
4 \n\
Non è stata impostata una parola chiave iniziale,\n\
oppure la parola chiave corrente è scaduta.\n\
\n\
Impostare la parola chiave, quindi ripetere \n\
il login usando la nuova parola chiave.
5 Errore di login: id utente non valido.
6 Errore di login: id del gruppo non valido.
7 Errore di login: id di revisione non valido.
8 Errore di login: flag di revisione non valido.
9 I login sono correntemente disabilitati.
10 Solo il superutente può eseguire %1$s\n
11 Ricaricamento delle lingue predefinite da %1$s\n
12 Rilettura dei file di configurazione e dei server\n
13 Rilettura del file di configurazione %1$s\n
14 Rilettura del file dei server %1$s\n
15 Rilettura del file delle autorizzazioni %1$s\n
16 dtlogin: memoria esaurita nella routine\n


$set 3
$ set 3 is help text
$ Comments for messsage #1:
$ 
$ Under [Options] there is an item called "Session".  Under
$ Session there are five items and these items have been
$ translated in other files.  To be consistent with the other
$ translations, the translations used below should be the same
$ as the other translations.  The items and their file are
$ listed below:
$ 
$   1. Current   - _common.session.tmsg ; set 1, message 2
$ 
$   2. Home      - _common.session.tmsg ; set 1, message 1
$ 
$   3. <display-name> - Current   - <display-name> on the options
$         menu will be replaced with the workstations display
$         name so does not need to be translated.  "Current" should
$         be the same as _common.session.tmsg ; set 1, message 2
$ 
$   4. <display-name> - Home      - <display-name> on the options
$         menu will be replaced with the workstations display
$         name so does not need to be translated.  "Home" should
$         be the same as _common.session.tmsg ; set 1, message 1
$ 
$ 
1 \n\
Aiuto del login:\n\
\n\
Lo schermo di login consente al computer di identificare l'utente.\n\
\n\
Per eseguire il login:\n\
\n\
     1) Digitare il nome utente e premere <Enter> o fare clic su [OK].\n\
        (se non è stato creato nessun utente, digitare "root")\n\
     2) Digitare la parola chiave e premere <Enter> o fare clic su [OK].\n\
\n\
 [Ricominciare]\n\
   Cancella i dati inseriti permettendo di ricominciare la\n\
   procedura di login.\n\
\n\
[Opzioni]\n\
   Consente di modificare le opzioni per la sessione:\n\
\n\
   Con il puntatore su [Opzioni], premere il pulsante\n\
   del mouse 1. Trascinare il puntatore sull'opzione\n\
   desiderata e rilasciare il pulsante del mouse per\n\
   selezionarla. Le opzioni disponibili sono:\n\
\n\
   Lingua - per cambiare la lingua della sessione\n\
   Sessione - per selezionare l'ambiente grafico:\n\
      Corrente - avvia la sessione più recente\n\
      Iniziale - avvia la sessione iniziale\n\
                 (se impostata in precedenza)\n\
      <nome-video> - Corrente - avvia la sessione più\n\
                 recente per il video specificato\n\
      <nome-video> - Iniziale - avvia la sessione iniziale\n\
                 per il video specificato\n\
      Sessione protetta - Avvia una sessione protetta.\n\
   Login della riga comandi - per un'interfaccia a caratteri\n\
   Ripristinare schermo di login - per riavviare il server X\n\
\n\
Per ridisegnare lo schermo, ad esempio quando i messaggi del\n\
sistema disturbano la visualizzazione, fare clic in un punto \n\
qualsiasi dello sfondo con un pulsante del mouse.\n\
\n\
Per maggiori informazioni di aiuto, eseguire il login, quindi fare\n\
clic sull'icona della Gestione dell'aiuto nel menu della Gestione\n\
delle informazioni (l'icona che raffigura alcuni libri con un\n\
punto interrogativo).\n

$ message 2 is generated when you attempt to start a system whose id is
$ set to unknown or if it is not configured for networking.
2 \n\
La workstation non ha un nome identificativo. Attualmente è impostata\n\
come "sconosciuta", ma si consiglia di assegnarle un nome.\n\
\n\

$ message 3 is generated when the help button on the chooser screen is clicked.
3 \n\
Aiuto sul selettore:\n\
\n\
Usare lo schermo del selettore per scegliere su quale server eseguire il login.\n\
\n\
Lo schermo del selettore contiene una lista con 2 parti per ogni\n\
riga. La prima parte (Nome del server) contiene il nome di rete di\n\
tutti i server disponibili. La seconda parte (Informazioni sul\n\
server) conterrà informazioni diverse a seconda della gestione\n\
del login utilizzata su ciascun server. Se un server utilizza\n\
il CDE, esso deve fornire informazioni sul numero di utenti \n\
(o di unità tty) correntemente in uso e sui valori medi di\n\
carico (uso medio della CPU) degli ultimi 5, 10 e 15 minuti.\n\
\n\
Per scegliere il server su cui effettuare il login, \n\
selezionare la riga con il nome del server desiderato.\n\
\n\
Una volta effettuata la selezione, premere il pulsante ok.\n\
Comparirà uno schermo di login dal server selezionato.\n\
\n\
[Aggiornare la lista]\n\
   Questo comando cancella la lista dei server e la ricrea \n\
   in base ai server della rete che accettano il login.\n\
\n\
[Opzioni]\n\
   Permette di cambiare le opzioni della sessione:\n\
\n\
   Con il puntatore su [Opzioni], premere il pulsante del\n\
   mouse 1. Trascinare il puntatore sull'opzione desiderata\n\
   e rilasciare il pulsante del mouse per selezionarla.\n\
   Le opzioni disponibili sono:\n\
\n\
   Lingua - per cambiare la lingua della sessione\n\
   Ripristinare schermo di login - per riavviare il server delle finestre X\n\
\n\
Per ridisegnare lo schermo, ad esempio quando i messaggi del\n\
sistema disturbano la visualizzazione, fare clic in un punto \n\
qualsiasi dello sfondo con un pulsante del mouse.\n\
\n\


$set 4
$ set 4 is log error messages
$ 

1 File di controllo degli accessi \"%1$s\", host \"%2$s\" non trovato\n
2 ReadHostEntry\n
3 File di controllo degli accessi %1$s, schermo %2$s sconosciuto\n
4 Impossibile aprire il file di controllo degli accessi %1$s; non verrà accettata alcuna richiesta XDMCP\n
5 Errore di autenticazione del dominio/OS: %1$s (%2$s/%3$s)
6 Impossibile aprire il file delle autorizzazioni del server %1$s\n
7 Impossibile scrivere nel file delle autorizzazioni del server %1$s\n
8 saveAddr
9 Problemi nell'ottenere la configurazione dell'interfaccia di rete
10 setAuthNumber
11 Impossibile bloccare il file delle autorizzazioni %1$s o il backup %2$s\n
12 Impossibile posizionare le autorizzazioni\n
13 Impossibile creare/bloccare il file dei pid %1$s\n
14 Impossibile bloccare il file dei pid %1$s: è in esecuzione un altro Dtlogin (pid %2$d)\n
15 Impossibile accedere al file dei server %1$s\n
16 NewDisplay
17 Impossibile aprire il file di log degli errori %1$s\n
18 TrimErrorLog(): la dimensione massima del file di log degli errori è 200Kb.\n
19 Il nome dello schermo non è specificato nel file dei server\n
20 Il tipo di schermo non è specificato per %1$s\n
21 Il nome host %1$s non è valido. La variabile DISPLAY verrà impostata su ':0'\n
22 Il tipo di schermo %1$s non è valido per lo schermo %2$s\n
23 Tempo scaduto per lo pseudo-ripristino.\n
24 GetResource
25 Spazio insufficiente per la riallocazione degli argomenti\n
26 Impossibile aprire il file di configurazione %1$s\n
27 Troppi argomenti nella riga comandi:
28 StartServer(): nessun argomento\n
29 Impossibile avviare il server %1$s\n
30 fork di StartServer() fallito, processo inattivo\n
31 Il server si è inaspettatamente interrotto\n
32 Blocco in XOpenDisplay(%1$s), tentativo #%2$d, operazione abbandonata.\n
33 Il tentativo di apertura del server #%1$d è fallito per %2$s; operazione abbandonata\n
34 Errore di I/O fatale %d (%1$s)\n
35 Errore delle finestre X\n
36 Avvio della sessione fallito\n
37 AVVERTENZA: Non è stato possibile proteggere lo schermo %1$s\n
38 AVVERTENZA: Non è stato possibile proteggere la tastiera dello schermo %1$s\n
39 BLS - Impossibile ottenere informazioni sui profili.\n
40 BLS - Impossibile ottenere informazioni sui profili protetti.\n
41 BLS - Impossibile verificare il livello di sensibilità.\n
42 BLS - Impossibile passare all'utente: %1$s.\n
43 AFS - Errore di setgroups() all'avvio della sessione\n
44 Setuid fallito per l'utente %s, n. errore = %d\n
45 Nessuna directory iniziale %1$s per l'utente %2$s; verrà usata la directory /\n
46 Esecuzione della sessione fallita %1$s\n
47 La sessione non ha comandi/argomenti\n
48 Impossibile avviare la sessione per %s, fork fallito\n
49 Dtlogin non può interrompere il client\n
50 Impossibile eseguire %1$s\n
51 Impossibile eseguire un fork per eseguire %1$s\n
52 L'autenticazione \"%1$s\" non è corretta. Usare AFS, Kerberos o il nome predefinito.\n
53 Impossibile eseguire Dtgreet %1$s per %2$s\n
54 Impossibile eseguire un fork per eseguire Dtgreet %1$s\n
55 Creazione del socket fallita sulla porta %1$d\n
56 Errore nel collegamento dell'indirizzo del socket %1$d; n. errore = %2$d\n
57 makeEnv
58 setEnv
59 parseArgs
60 La lettura della directory %1$s ha richiesto più di %2$d secondi. Abbandono dell'operazione...\n
61 %s appartiene a più di %1$d gruppi, %2$s ignorato\n
62 Errore di audwrite(): il chiamante non è superutente\n
63 Errore di audwrite(): numero di evento non valido nel record di revisione.\n
64 Errore di audwrite(): n. errore = %1$d\n
65 Errore nell'autenticazione AFS: %1$s\n
66 Il profilo per %1$s è scaduto. Rivolgersi all'amministratore del sistema.\n
67 Server interrotto  %1$s\n
68 Impossibile aprire il file a mappa di bit del logo: %1$s\n
69 Impossibile leggere il catalogo dei messaggi \"%1$s\". Verrà usata la lingua predefinita.
70 Impossibile inizializzare lo schermo %1$s; forse il server è bloccato.\n
71 (%1$s): La larghezza del riquadro di login supera quella dello schermo; verrà usato il valore predefinito. I riquadri vengono impostati in pixel.\n
72 (%1$s) L'altezza del riquadro di login supera quella dello schermo; verrà usato il valore predefinito. I riquadri vengono impostati in pixel.\n

$set 5
$ set 5 is for other stuff specific to chooser.
1 Selezionare il server su cui eseguire il login:
$ 1 is the title/instructions for the chooser dialog
2 Nome del server                Informazioni sul server
$                                ^
$                                | This needs to start in this column
$ 2 is the headings for the list
3  Aggiornare l'elenco 

$set 99
$    DO NOT TRANSLATE THESE MESSAGES
$    DO NOT CHANGE THESE MESSAGES
$    DO NOT LOCALIZE THESE MESSAGES
$    These messages are used for the version information.
1 @(#)version_goes_here
2 \n@(#)_DtMessage catalog source $XConsortium: dtlogin.msg /main/10 1996/12/20 15:13:33 rws $
