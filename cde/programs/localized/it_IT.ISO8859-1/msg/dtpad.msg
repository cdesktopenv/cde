$ $TOG: dtpad.msg /main/22 1999/09/15 14:21:27 mgreess $
$ ***********************************<+>*************************************
$ ***************************************************************************
$ **
$ **  File:        dtpad.msg
$ **
$ **  Project:     DT Text Editor client (sometimes called dtpad)
$ **
$ **  Description:
$ **  -----------
$ **  This file is the source for the message catalog for dtpad
$ **
$ **
$ ***************************************************************************
$ **
$ **     (c) Copyright 1992, 1993 Hewlett-Packard Company
$ **     All Rights reserved
$ **
$ **
$ ***************************************************************************
$ **
$ ***************************************************************************
$ **  (c) Copyright 1993, 1994 Hewlett-Packard Company
$ **  (c) Copyright 1993, 1994 International Business Machines Corp.
$ **  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
$ **  (c) Copyright 1993, 1994 Novell, Inc.
$ ***************************************************************************
$ **

$quote "
$set 2
$ **********************************************
$ **  General messages used in multiple files **
$ **********************************************

$ Label used for all "Help" buttons in all DIALOGS.
$ It is not used for the Help menu.
$quote
1 Aiuto

$ Unrecognized arguments
2 dtpad: opzione non ammessa: %s 
$ Extra arguments
3 dtpad: troppi argomenti: %s



$set 4
$ *****************************************************************
$ **                                                    fileCB.c **
$ ** These are the strings which change between the Open dialog  **
$ ** and the Include dialog.  These two operations use the same  **
$ ** Motif dialog, and simply change these two labels.           **
$ *****************************************************************

$ Title of the Open dialog
1 Apertura file

$ label for the File text field in the file selection box
$ ***UNUSED***	This label is no longer used and need not be localized.
2 File to Open:

$ Title of the Include dialog
3 Inclusione file

$ Label for the File text field in the file selection box
$ ***UNUSED***	This label is no longer used and need not be localized.
4 File to Include:

$ Error when printing a file and a temporary copy can not be saved.
5 Impossibile creare il file temporaneo.


$set 5
$ ***************************************************************************
$ **                                                               dtpad.c **
$ **                                                             fileDlg.c **
$ **                                                                       **
$ ** Messages and strings used in dialogs for all operations involving the **
$ ** "File" menu items.                                                    **
$ ***************************************************************************
$ These compose the dialog which appears if the user attempts to exit, open
$ a different file, drop a file, or start a "New" session with unsaved
$ changes to the current file.

$ This is used to build: "Save changes to FILENAME?"
1 Salvare le modifiche a %s?

$ This is used as-is, whenever the current file is unnamed.
2 Il file corrente non ha nome.\nSi desidera salvarlo?

$ This is used to build a message like "Do you want to save the current
$ copy of FILENAME before you Edit NEWFILENAME?" when a new file is droped
$ on the edit window.
$ ***UNUSED***	Currently, dropped files are INSERTED in the text rather than
$		opened for editing so this message is no longer used and need
$		not be localized.
3 Si desidera salvare la copia\ncorrente di %s\nprima di editare %s?

$ Used when about to write over an existing file.
$ The "%s" is replaced by the file name.
5 Il file %s esiste già.\nSi desidera sovrascrivere il file esistente\ncon il contenuto di questa sessione?

$ Title of "file already exists" dialog.  Also used for other "Warning" type
$ dialogs.
6 Avvertenza

$ Title of "Save As" dialog.
7 Salvataggio come

$ Labels for add/don't-add newlines radio buttons in the Save As dialog
$ ***UNUSED***	These labels are no longer used and need not be localized.
$		They've been replaced by messages 44 and 45
8 Aggiungere caratteri di avanzamento riga alla fine di ogni riga.
9 Non aggiungere caratteri di avanzamento riga. Verranno mantenuti\nsolo i ritorni a capo creati con [Return].

$ Labels for the Save Warning dialog. Also used in the confirmation, or
$ Editor Return dialog.
14 No
15 Sì
$ Title of the dialog - used when user selects "New", "Open", or "Exit" with
$ unsaved changes in the text.
16 Salvare?

$ Title of general error dialog
17 Errore

$ Title of the resource-specified "confirmation" dialog
$ This dialog is generally specified by programs such as mailers, which
$ want to know if the user wants to send the message.
$ There is no default message for the dialog, since it is _only_ posted if
$ the message string is specified via a resource (dtpad*confirmationString:)
$ This message is only used if the "dtpad*mainTitle:" resource is not set.
$ If this resource _is_ set, the the mainTitle is also used as the title
$ for the exit confirmation dialog.
$ DO NOT LOCALIZE - the user should never see this message
18 Exit Confirmation

$ Title for general information dialog. e.g. Find dialog can't find word in
$ file.
20 Informazione

$ Default file name in Save As dialog box.
21 SENZA_NOME

$ Error message when a file can not be saved.  The "%1$s" gets replaced by
$ the file name and the "%2$s" gets replaced by the reason why the file could
$ not be saved (messages 31-39).
30 Impossibile salvare %1$s.\n%2$s

$ Messages 31-39 are reasons why a file could not be saved.
$ Messages 34-39 are substitued in message 30 for "%2$s".
31 Impossibile salvare il file.\nNon è stato specificato il nome.

34 Il file non è accessibile in scrittura.

35 È stata specificata una directory.

36 È stata specificata un'unità a caratteri.

37 È stata specificata un'unità a blocchi.

$ ***UNUSED***	This message should never appear since currently there is
$		only one supported file format - text.  And this type is
$		hard coded into dtpad.
38 Invalid file format.

$ This is a "catch all" when the file can not be saved and none of the above
$ errors apply.
39 Causa sconosciuta - probabilmente lo spazio su disco è insufficiente.

$ This is displayed when a ToolTalk edit request on a buffer is received,
$ a Save operation is done and there are unsaved changes.
40 Salvare le modifiche?

$ This is displayed when a ToolTalk edit request on a file or buffer is
$ received with a document name specified in the request, a Save operation
$ is done and there are unsaved changes.  The "%s" in the message is
$ replaced by the document name (instead of the filename).
41 Salvare le modifiche a "%s"?

$ Title of "Copy As" dialog which is substitued for the "Save As" dialog
$ when file name changes are disallowd.  This should match the menu button
$ label specified in message 108 of set 11.
$ ***UNUSED***	This title is no longer used and need not be localized.
42 Copia come

$ Title of "Copy To File" dialog which is substitued for the "Save As" dialog
$ when file name changes are disallowd.  This should match the menu button
$ label specified in message 110 of set 11.
43 Copia su file

$ Labels for add/don't-add newlines radio buttons in the SaveAs and Save dialogs
44 Aggiungere caratteri di ritorno a capo al termine delle righe.
45 Non aggiungere ritorni a capo. Verranno conservate solo \nle interruzioni di riga create con [Return].

46 Errore: impossibile scrivere sul file o il percorso non e' corretto."

$ 
$ "-NEW"
$ 
47 "Operation failed due to insufficient memory.\nTry increasing swap space."

$ 
$ "-NEW"
$ 
48 "Format operation failed."

$set 6
$ ************************************************************************
$ **                                                           fileIo.c **
$ **                                                                    **
$ ** Error messages generated when there are problems reading a file.   **
$ ************************************************************************

1 Questo file è stato aperto in modalità di sola lettura.
2 Questo file non esiste.
3 Impossibile editare una directory.
4 Impossibile editare un'unità a caratteri.
5 Impossibile editare un'unità a blocchi.
6 Dal file sono stati eliminati caratteri nulli.
7 Impossibile caricare il file (memoria insufficiente).
8 Impossibile accedere al file (impossibile definirne lo stato).
9 Impossibile leggere il file (manca autorizzazione in lettura).

$ Message placed in the status line message area based on the -viewOnly
$ resource and whether or not the file being edited is read only
10 Solo visualizzazione
11 Sola lettura

12 Impossibile leggere il file (causa sconosciuta).



$set 7
$ ************************************
$ **                         main.c **
$ **                        dtpad.c **
$ ** Prefix string for all dialogs. **
$ ************************************
$ bogus?? blank string gotten but otherwise unused in main()
$ DO NOT LOCALIZE string 1
$quote "
1 ""
$quote

$ The name used in the title bar of every window.
$ Be sure to end this message with a space character.
2 Editor di testo - 

$ Default file name used in the main window title.
$ ***UNUSED***	This mesage has been replaced by message 6 of this set and
$		need not be localized.
3 (senza nome)

$ Terminal error message displayed to stdout when the desk top services
$ can not be initialized (via DtInitialize())
4 Impossibile inizializzare i servizi del desktop... uscita dall'Editor di testo.

$ Terminal error message displayed to stdout when a connection to the
$ display can not be opend (via XtDisplay())
5 Impossibile aprire il display ... uscita dall'Editor di testo.

$ Default file name used in the main window title.
$ This should correspond to the default file name used in the "Save As"
$ dialog box - specified in message 21 of set 5.
6 (SENZA_NOME)
$ 
$ New messages (7,8) since CDE/SI - error gets displayed when user tries to run
$ dtpad -server when and dtpad server process is already running on the
$ $DISPLAY.
7 ERRORE: Il server dtpad su %s\n\
è già in esecuzione per questo schermo.
8 ERRORE: Il server dtpad è già\n\
in esecuzione per questo schermo.
$ 
$ Message 9 is also new since CDE/SI - new as of 5/22/95.  It is a usage message
$ for dtpad. Don't translate the '-OPTION' just the text explaining the
$ option.
$ 
9 '%s' non è un'opzione corretta.\n\
Sintassi: %s...\n\
   -saveOnClose\n\
\tDtpad salva automaticamente il testo corrente modificato\n\
\tquando si chiude l'Editor di testo. Non chiede conferma.\n\n\
   -missingFileWarning\n\
\tVisualizza un avvertimento quando si specifica un nome di file\n\
\te il file non esiste oppure non è accessibile.\n\n\
   -noReadOnlyWarning\n\
\tDisattiva la finestra con il messaggio di avvertimento che compare quando\n\
\tl'utente non ha l'autorizzazione in scrittura per il file specificato.\n\n\
   -noNameChange\n\
\tSegnala che il nome di file predefinito associato al testo corrente\n\
\tnon deve essere cambiato. Questo accade quando il nome indicato è\n\
\tdiverso da quello originario.\n\n\
   -viewOnly\n\
\tDisattiva la funzione di modifica del testo nella finestra di modifica,\n\
\ttrasformando l'Editor di testo in un programma di visualizzazione dei testi.\n\n\
   -statusLine\n\
\tVisualizza la riga di stato nella parte inferiore della finestra\n\
\tdi modifica, dove è riportato il numero della riga sulla quale\n\
\tsi trova attualmente il cursore.\n\n\
   -wrapToFit\n\
\tAttiva il ritorno a capo al termine delle righe.\n\n\
   -workspaceList WorkspaceList\n\
\tDopo aver richiamato l'Editor di testo, visualizza la finestra di modifica\n\
\tnegli spazi di lavoro specificati.\n\n\
   -session SessionFile\n

10 \tRipristina le stesse impostazioni e finestre dell'Editor di testo attive nella\n\
\tprecedente sessione di CDE, prima della disattivazione. Questa opzione prevale\n\
\tsu tutte le opzioni specificate sulla riga di comando, che verranno pertanto\n\
\tignorate.\n\n\
   -standAlone\n\
\tRichiama l'Editor di testo facendo in modo che esso elabori\n\
\til testo in una finestra autonoma e indipendentemente dal\n\
\tserver dell'Editor di testo.\n\n\
   -noBlocking\n\
\tConclude l'elaborazione delle richieste non appena il\n\
\tserver dell'Editor di testo conferma di poter gestire\n\
\tle richieste di modifica.\n\n\
   -server\n\
\tAvvia il server dell'Editor di testo (se non è già in\n\
\tfunzione) per elaborare tutte le successive richieste di\n\
\tmodifiche relative allo schermo.\n\n\
   -exitOnLastClose\n\
\tSpecifica che il processo del server dell'Editor di testo dovrà\n\
\tterminare alla chiusura dell'ultima finestra di modifica\n\
\trelativa allo schermo. Da utilizzare solo con l'opzione\n\
\t-server perché applicabile ai processi del server.\n\n\
Consultare la pagina di spiegazioni su dtpad per i dettagli sulle opzioni.\n\n
$ 

$ "NEW-"
11 "Exiting text editor ..."
$ "NEW-"
12 "Running in standalone mode ..."
$ "NEW-"
13 "Could not connect to ToolTalk service:\n%s\n%s\n"

$set 10
$ ************************************************************************
$ **                                                          session.c **
$ ** THESE MESSAGES ARE _ONLY_ WRITTEN TO THE ERRORLOG FILE.
$ **
$ ************************************************************************
$ Strings used to build error messages issued when the session save
$ file cannot be read or written.
1 Controllare le autorizzazioni del file.
2 Il file specificato per il salvataggio è %s.


$set 11
$ ***********************************************************
$ **                                              dtpad.c **
$ **                                                       **
$ ** These are all of the menu button labels and mnemonics **
$ ***********************************************************

$ ***UNUSED*** 	Messages 1-14 are no longer used and need not be localized.
$		All of the mnemonics and labels relating to the "Help" menu
$		have been replaced by messages 81-96.

$ *
$ * Mnemonics and labels for the "Help" pulldown menu and items.
$ *
$ Mnemonic and label for the "Help" pulldown menu
1 A
2 Aiuto

$ mnemonic and label for the "Overview" item
3 P
4 Panoramica...

$ mnemonic and label for the "Tasks" item
5 A
6 Attività...

$ nmemonic and label for the "Table of Contents" item are message nos. 71 & 72

$ mnemonic and label for the "Reference" item
7 R
8 Riferimenti...

$ mnemonic and label for the "On Item" item
9 S
10 Sull'elemento

$ mnemonic and label for "Using Help" item
11 U
12 Uso dell'aiuto...

$ mnemonic and label for "About Text Editor" item
13 f
14 Informazioni sull'Editor di testo...

$ *
$ * Mnemonics and labels for the "File" pulldown menu and items.
$ *
$ mnemonic and label for the "File" pulldown menu
15 F
16 File

$ mnemonic and label for the "New" item
17 N
18 Nuovo

$ mnemonic and label for the "Open" item
19 A
20 Aprire...

$ mnemonic and label for the "Include" item
21 I
22 Includere...

$ mnemonic and label for the "Save" item
$ NOTE: The "Save" label can be either Message 24 or message 73
23 S
24 Salvare

$ mnemonic and label for the "Save As" item
25 v
26 Salvare come...

$ mnemonic and label for the "Print" item
27 t
28 Stampare...

$ mnemonic and label for the "Close" item
29 C
30 Chiudere

$ *
$ * Mnemonics and labels for the "Edit" pulldown menu and items.
$ *
$ mnemonic and label for the "Edit" pulldown menu
31 E
32 Editare

$ mnemonic and label for the "Undo" menu item
33 A
34 Annullare

$ mnemonic and label for the "Cut" menu item
35 T
36 Tagliare

$ mnemonic and label for the "Copy" menu item
37 C
38 Copiare

$ mnemonic and label for the "Paste" menu item
39 I
40 Incollare

$ mnemonic and label for the "Clear" menu item
41 p
42 Ripulire

$ mnemonic and label for the "Delete" menu item
43 n
44 Cancellare

$ ***UNUSED***	Messages 45-50 are no longer used and need not be localized.
$		They have been replaced by messages 101-106 of this set.
$ mnemonic and label for the "Select All" menu item
$ 45 A
$ 46 Select All

$ mnemonic and label for the "Find/Change" menu item
$ 47 F
$ 48 Find/Change...

$ mnemonic and label for the Check Spelling menu item
$ 49 S
$ 50 Check Spelling...

$ *
$ * Mnemonics and labels for the "Format" pulldown menu and items.
$ *
$ mnemonic and label for the "Format" pulldown menu
51 r
52 Formato

$ mnemonic and label for the "Settings..." menu item
53 I
54 Impostazioni...

$ mnemonic and label for the "Paragraph" menu item
55 P
56 Paragrafo

$ mnemonic and label for the "All" menu item
57 T
58 Tutto

$ *
$ * Mnemonics and labels for the "Options" pulldown menu and items.
$ *
$ mnemonic and label for the "Format" pulldown menu
59 O
60 Opzioni

$ mnemonic and label for the "Overstrike" toggle button
61 S
62 Sovrascrivere

$ mnemonic and label for the "Word Wrap" toggle button
$ ***UNUSED***	This label and mnemonic are no longer used and need not be
$		localized.  They've been replaced by messages 111 and 112.
63 A
64 A capo automatico

$ mnemonic and label for the "Status Line" toggle button
65 R
66 Riga di stato

$ *
$ * Late additions.
$ *

$ nmemonic and label for the "Table of Contents" item of the "Help" menu
$ ***UNUSED*** 	This nmemonic and label are no longer used and need not be
$		localized.  They've been replaced by messages 87 and 88.
71 I
72 Indice...

$ label for the "Save" item in the "File" pulldown menu
$ NOTE: This label is used in place of message 24 when text has been
$       changed but has not been saved
73 Salvare (necessario)


$ *
$ * Mnemonics and labels for the "Help" pulldown menu and items.
$ *
$ Mnemonic and label for the "Help" pulldown menu
81 A
82 Aiuto

$ mnemonic and label for the "Overview" item
83 P
84 Panoramica

$ mnemonic and label for the "Tasks" item
85 A
86 Attività

$ nmemonic and label for the "Table of Contents" item
87 I
88 Indice

$ mnemonic and label for the "Reference" item
89 R
90 Riferimenti

$ mnemonic and label for the "On Item" item
91 S
92 Sull'elemento

$ mnemonic and label for "Using Help" item
93 U
94 Uso dell'aiuto

$ mnemonic and label for "About Text Editor" item
95 f
96 Informazioni sull'Editor di testo


$ *
$ * Changes for "Edit" pulldown menu messages 45-50.
$ *

$ mnemonic and label for the "Select All" menu item
101 S
102 Selezionare tutto

$ mnemonic and label for the "Find/Change" menu item
103 R
104 Ricercare/sostituire...

$ mnemonic and label for the Check Spelling menu item
105 o
106 Controllare ortografia...

$ mnemonic and label for the "Copy As" File menu item
$ (should match the dialog title specified in message 42 of set 5)
$ ***UNUSED***	This label is no longer used and need not be localized.
107 C
108 Copiare come...

$ mnemonic and label for the "Copy To File" File menu item
$ (should match the dialog title specified in message 42 of set 5)
109 C
110 Copiare su file...

$ mnemonic and label for the "Wrap To Fit" toggle button
111 A
112 A capo automatico

$ Label for the "Print" item
113 Stampare



$ ***********************************************************
$ **                                              helpCB.c **
$ **                                                       **
$ ** Messages and strings associated with online help.     **
$ **                                                       **
$ ***********************************************************
$set 12
$ Message used for "Help - On Item" if the user clicks outside application
1 Selezionare un elemento all'interno dell'Editor di testo.


$ ***********************************************************
$ **                                        ttMsgSupport.c **
$ **                                                       **
$ ** Messages and strings associated with ToolTalk.        **
$ **                                                       **
$ ***********************************************************
$set 13
1 Salvataggio del checkpoint non riuscito.
2 Risposta al checkpoint non riuscita.

$ more errors/warnings can
$ be detected while reading in a file via ToolTalk.
3 Impossibile definire il contenuto dell'Editor di testo (motivo sconosciuto).



$ ****************************************************************************
$ ****************************************************************************
$ **
$ **	SETS 14 - 15 WERE ADDED TO SUPPORT CDEnext PRINTING
$ **
$ **	The following set was added to support CDEnext printing based on
$ **	the Xp Print Server.
$ **
$ ****************************************************************************
$ ****************************************************************************

$ ****************************************************************************
$ ****************************************************************************
$ **
$ **	SET 14
$ **
$ ****************************************************************************
$ ****************************************************************************
$set 14
$ 
$	Messages 1 - 11 are used in printJob.
$ 
$	This is warning message is displayed when the user has provided
$	a margin specifier in the PrintSetup dialog which cannot be parsed.
$	
$quote "

1 "Errore di sintassi nella specificazione dei margini:\n %s \n %s \n %s \n %s \nUsare i margini predefiniti?"
$ 
$	Messages 2 - 7 are messages for errors which can occur in the
$	process of loading a dtpad document file for printing.	
$ 
2 "Il file non esiste."
3 "Il file specificato è una directory."
4 "Errore nel tipo di file."
5 "Il file contiene caratteri NULLI."
6 "Impossibile caricare il file (memoria insufficiente)."
7 "Manca l'autorizzazione in lettura."

$	Messages 8 - 11 are format strings used to construct the header
$	and footer strings which appear in the printing output.
$ 
$	Message 8 is the format string used to make a Date header/footer.
$	Message 9 is the format string used to make a DocName header/footer.
$	Message 10 is the format string used to make a Page# header/footer.
$	Message 11 is the format string used to make a UserName header/footer.
8 "Data:  %s"
9 "Documento:  %s"
10 "Pagina %d di %d"
11 "Documento per:  %s"

$ 
$	Messages 12 - 13 are used in printSetup to construct the
$	app-specific printing options for DtMail which will be included
$	in the DtPrintSetupBox.
$ 
$	Message 12 is the string labeling the field in the GUI which
$	contains the name of the document.
$	Message 13 is the label used on the Toggle Button to indicate that
$	lines which would extend beyond the edge of the printed page be 
$	"wrapped" to the next line.
12 "Nome del documento:"
13 "A capo automatico"


$	Messages 14 - 15 are status messages for the PrintToFile function.
$ 
$	Message 14 indicates that the PrintToFile function was unsuccessful.
$	Message 15 indicates that the PrintToFile function was successful.
$       NOTE:  'Print to File' should be translated as in DtPrint.msg
$               Set 1, Message 3.
14 "L'operazione 'Stampare su file' non è riuscita."
15 "L'operazione 'Stampare su file' è riuscita."

$          Messages 16 - 19 are obsolete, and have been replaced by 25
$ 
$	Messages 16 - 19 are status messages for the popup PDM function.
$ 
$	Message 16 indicates that there was no pdm deamon running.
$	Message 17 indicates that the pdm deamon was unable to start a pdm.
$	Message 18 indicates that the pdm couldnt connect to the video display.
$	Message 19 indicates that the pdm couldnt connect to the print display.
16 "Impossibile trovare la Gestione del dialogo di stampa PDM nell'ambiente corrente."
17 "Impossibile avviare la Gestione del dialogo di stampa PDM."
18 "Gestione del dialogo di stampa non è autorizzata a collegare al display Video."
19 "Gestione del dialogo di stampa non è autorizzata a collegare al display Stampa."


$	Messages 20 - 22 are status messages displayed while printing
$ 
$	Message 20 indicates that the user has asked to close the editor
$		   window.  Dtpad is prevented from doing so until the
$		   active print jobs have completed.
$	Message 21 status message displayed when the print job is started.
$	Message 22 status message displayed as the print job progresses.
20 "Close pending:  waiting for task to terminate ..."
21 "È in corso la stampa di %s ..."
22 "È in corso la stampa di %s: pagina %d di %d ..."

$       Message 23 is used in the construction of a title string for the
$                  Print Setup dialog.
23 "Impostazione della stampa - "

$	Message 24 is used to alert the user to the fact that the X Print
$		   Server cannot complete the print request due to a lack
$		   of resources.
24 "Il server di stampa X è temporaneamente senza risorse."

$       Messages 25 is used to handle errors from the PDM
25	"Errore Gestione del dialogo di stampa (PDM) - Impostazione non riuscita."

$ ****************************************************************************
$ ****************************************************************************
$ **
$ **	SET 15
$ **
$ ****************************************************************************
$ ****************************************************************************
$ 
$set 15
$ 
$	Messages 1-5 are used in printSetup.c in the pulldown menu for the
$	header/footer options in the print setup dialog.
$ 
$	Message 1 specifies that the page header/footer field be empty.
$	Message 2 put the Date in the page header/footer field.
$	Message 3 put the Document Name in the page header/footer field.
$	Message 4 put the Page Nmbr msg header in the page header/footer field.
$	Message 5 put the User Name msg header in the page header/footer field.
1 "Nessun testo"
2 "Data"
3 "Nome del documento"
4 "Numero di pagina"
5 "Nome dell'utente"

$	Messages 6-9 are used to construct error messages informing users
$	that they have input an invalid print page margin specification for 
$ 
6 "Il valore del margine superiore non è valido:"
7 "Il valore del margine destro non è valido:"
8 "Il valore del margine inferiore non è valido:"
9 "Il valore del margine sinistro non è valido:"
