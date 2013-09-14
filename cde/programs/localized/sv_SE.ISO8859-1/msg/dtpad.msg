$ $XConsortium: dtpad.msg /main/4 1995/12/08 09:39:11 rswiston $
$ *************************************<+>*************************************
$ *****************************************************************************
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
$ *****************************************************************************
$ **
$ **     (c) Copyright 1992, 1993 Hewlett-Packard Company
$ **     All Rights reserved
$ **
$ **
$ *****************************************************************************
$ **
$ *****************************************************************************
$ **  (c) Copyright 1993, 1994 Hewlett-Packard Company
$ **  (c) Copyright 1993, 1994 International Business Machines Corp.
$ **  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
$ **  (c) Copyright 1993, 1994 Novell, Inc.
$ *****************************************************************************
$ **

$set 2
$ **********************************************
$ **  General messages used in multiple files **
$ **********************************************

$ Label used for all "Help" buttons in all DIALOGS.
$ It is not used for the Help menu.
1 Hjälp


$set 4
$ *****************************************************************
$ **                                                    fileCB.c **
$ ** These are the strings which change between the Open dialog  **
$ ** and the Include dialog.  These two operations use the same  **
$ ** Motif dialog, and simply change these two labels.           **
$ *****************************************************************

$ Title of the Open dialog
1 Öppna en fil

$ label for the File text field in the file selection box
$ ***UNUSED***	This label is no longer used and need not be localized.
2 File to Open:

$ Title of the Include dialog
3 Inkludera en fil

$ Label for the File text field in the file selection box
$ ***UNUSED***	This label is no longer used and need not be localized.
4 File to Include:

$ Error when printing a file and a temporary copy can not be saved.
5 Det går inte att skapa en temporär fil.


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
1 Vill du spara ändringarna i %s?

$ This is used as-is, whenever the current file is unnamed.
2 Den aktuella filen saknar namn.\nVill du spara den?

$ This is used to build a message like "Do you want to save the current
$ copy of FILENAME before you Edit NEWFILENAME?" when a new file is droped
$ on the edit window.
$ ***UNUSED***	Currently, dropped files are INSERTED in the text rather than
$		opened for editing so this message is no longer used and need
$		not be localized.
3 Vill du spara den aktuella \nkopian av %s\ninnan du redigerar %s?

$ Used when about to write over an existing file.
$ "%s" is replaced by the file name.
5 Filen %s finns redan.\nVill du skriva över den\nmed innehållet i den redigerade filen?

$ Title of "file already exists" dialog.  Also used for other "Warning" type
$ dialogs.
6 Varning

$ Title of "Save As" dialog.
7 Spara som

$ Labels for add/don't-add newlines radio buttons in the Save As dialog
$ ***UNUSED***	These labels are no longer used and need not be localized.
$		They've been replaced by messages 44 and 45
8 Infoga radmatningstecken i slutet av ombrutna rader.
9 Infoga inte radmatningstecken. Endast sådana radbrytningar som skapas med [Return]\nkommer att behållas.

$ Labels for the Save Warning dialog. Also used in the confirmation, or
$ Editor Return dialog.
14 Nej
15 Ja
$ Title of the dialog - used when user selects "New", "Open", or "Exit" with
$ unsaved changes in the text.
16 Vill du spara?

$ Title of general error dialog
17 Fel

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
20 Information

$ Default file name in Save As dialog box.
21 NAMNLÖS

$ Error message when a file can not be saved.  The "%1$s" gets replaced by
$ the file name and the "%2$s" gets replaced by the reason why the file could
$ not be saved (messages 31-39).
30 Det går inte att spara %1$s.\n%2$s

$ Messages 31-39 are reasons why a file could not be saved.
$ Messages 34-39 are substitued in message 30 for "%2$s".
31 Det går inte att spara filen.\nDu har inte angivit något filnamn.

34 Det går inte att skriva till filen.

35 Filen är en katalog.

36 Filen är en teckenspecifik enhet.

37 Filen är en blockspecifik enhet.

$ ***UNUSED***	This message should never appear since currently there is
$		only one supported file format - text.  And this type is
$		hard coded into dtpad.
38 Invalid file format.

$ This is a "catch all" when the file can not be saved and none of the above
$ errors apply.
39 Okänd anledning. Det kanske inte finns tillräckligt med diskutrymme.

$ This is displayed when a ToolTalk edit request on a buffer is received,
$ a Save operation is done and there are unsaved changes.
40 Spara ändringar?

$ This is displayed when a ToolTalk edit request on a file or buffer is
$ received with a document name specified in the request, a Save operation
$ is done and there are unsaved changes.  The "%s" in the message is
$ replaced by the document name (instead of the filename).
41 Spara ändringar i "%s"?

$ Title of "Copy As" dialog which is substitued for the "Save As" dialog
$ when file name changes are disallowd.  This should match the menu button
$ label specified in message 108 of set 11.
$ ***UNUSED***	This title is no longer used and need not be localized.
42 Kopiera som

$ Title of "Copy To File" dialog which is substitued for the "Save As" dialog
$ when file name changes are disallowd.  This should match the menu button
$ label specified in message 110 of set 11.
43 Kopiera till fil

$ Labels for add/don't-add newlines radio buttons in the SaveAs and Save dialogs
44 Infoga radmatningstecken i slutet av ombrutna rader.
45 Infoga inte radmatningstecken. Endast sådana radbrytningar som\nskapats med [Return] kommer att behållas.
$ 
$ New message as of 5/31/95 (i.e. new since Sample Implementation). This 
$ new message is in response to bug DTS CISlx19530.
$ 
46 Antingen är filen skrivskyddad eller också\nexisterar inte den angivna sökvägen.



$set 6
$ ************************************************************************
$ **                                                           fileIo.c **
$ **                                                                    **
$ ** Error messages generated when there are problems reading a file.   **
$ ************************************************************************

1 Filen har endast öppnats för att läsas.
2 Filen finns inte.
3 Det går inte att redigera en katalog.
4 Det går inte att redigera en teckenspecifik enhetsfil.
5 Det går inte att redigera en blockspecifik enhetsfil.
6 Inbäddade tomma strängar har tagits bort från filen.
7 Det går inte att ladda filen (minnet räcker inte).
8 Det går inte att komma åt filen (filens status kan inte identifieras).
9 Kan inte läsa filen (förmodligen har du inte läsbehörighet till den). 
$ Message placed in the status line message area based on the -viewOnly
$ resource and whether or not the file being edited is read only
10 Visa
11 Skrivskyddad


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
2 Textredigerare - 

$ Default file name used in the main window title.
$ ***UNUSED***	This mesage has been replaced by message 6 of this set and
$		need not be localized.
3 (namnlös)

$ Terminal error message displayed to stdout when the desk top services
$ can not be initialized (via DtInitialize())
4 Det går inte att initiera Desktop services ... avslutar Textredigerare.

$ Terminal error message displayed to stdout when a connection to the
$ display can not be opend (via XtDisplay())
5 Kan inte öppna fönster ... Textredigeraren avslutas.

$ Default file name used in the main window title.
$ This should correspond to the default file name used in the "Save As"
$ dialog box - specified in message 21 of set 5.
6 (NAMNLÖS)
$ 
$ New messages (7,8) since CDE/SI - error gets displayed when user tries to run
$ dtpad -server when and dtpad server process is already running on the
$ $DISPLAY.
7 FEL: En dtpad-server på %s\n\
betjänar redan det här fönstret.
8 FEL: En dtpad-server\n\
betjänar redan det här fönstret.
$ 
$ Message 9 is also new since CDE/SI - new as of 5/22/95.  It is a usage message
$ for dtpad. Don't translate the '-OPTION' just the text explaining the
$ option.
$ 
9 Alternativet '%s' är felaktigt.\n\
Syntax: %s...\n\
   -saveOnClose\n\
\tDtpad sparar automatiskt den aktuella texten om det\n\
\tfinns osparade ändringar när du stänger textredigeraren.\n\n\
   -missingFileWarning\n\
\tVisar en varningsruta om du anger namnet på en fil\n\
\tsom inte finns eller inte går att komma åt.\n\n\
   -noReadOnlyWarning\n\
\tVisar ingen varningsruta även om du anger namnet på en fil\n\
\tsom du inte har skrivbehörighet till.\n\n\
   -noNameChange\n\
\tIndikerar att det standardfilnamn som är associerat med\n\
\tden aktuella texten inte får ändras om texten sparas med\n\
\tett annat namn än den laddades med.\n\n\
   -viewOnly\n\
\tHindrar redigering av texten i redigeringsfönstret, vilket i\n\
\tpraktiken förvandlar textredigeraren till ett textfönster.\n\n\
   -statusLine\n\
\tVisar en statusrad längst ned i redigeringsfönstret. Där\n\
\tkan du se på vilken rad textmarkören befinner sig\n\
\tför tillfället.\n\n\
   -wrapToFit\n\
\tAktiverar läget för ombrutna rader.\n\n\
   -workspaceList WorkspaceList\n\
\tVisar den aktuella textredigerarsessionens redigerings-\n\
\tfönster i den eller de arbetsytor som angivits.\n\n\
   -session SessionFile\n

10 \tÅterställer textredigeraren med de textredigeringsfönster och\n\
\tinställningar som var aktiva innan CDE tidigare stängdes. Alla\n\
\tandra alternativ på kommandraden ignoreras när du anger\n\
\tdetta alternativ.\n\n\
   -standAlone\n\
\tTvingar den nuvarande textredigerarsessionen att själv\n\
\tutföra sin textbehandling i det egna fönstret, oberoende\n\
\tav textredigerarservern.\n\n\
   -noBlocking\n\
\tAvslutar textredigerarens process för redigeringsbegäran\n\
\tså snart textredigerarservern avgör att den kan hantera\n\
\taktuell redigering.\n\n\
   -server\n\
\tTvingar en textredigerarserver att starta (om inte en sådan\n\
\tredan är igång) så att den kan utföra alla efterföljande\n\
\tredigeringar i fönstret.\n\n\
   -exitOnLastClose\n\
\tAnger att textredigerarens serverprocess ska avslutas\n\
\tnär det sista redigeringsfönstret stängs. Alternativet\n\
\tbör endast användas tillsammans med alternativet -server\n\
\teftersom det bara kan tillämpas på serverprocessen.\n\n\
Manualsidan för dtpad innehåller fler detaljer om dessa alternativ.\n\n
$ 


$set 10
$ ************************************************************************
$ **                                                          session.c **
$ ** THESE MESSAGES ARE _ONLY_ WRITTEN TO THE ERRORLOG FILE.
$ **
$ ************************************************************************
$ Strings used to build error messages issued when the session save
$ file cannot be read or written.
1 Kontrollera filens behörigheter.
2  %s är filen som skulle ha använts för att spara din session.


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
1 H
2 Hjälp

$ mnemonic and label for the "Overview" item
3 v
4 Översikt...

$ mnemonic and label for the "Tasks" item
5 t
6 Steg-för-steg-anvisningar...

$ nmemonic and label for the "Table of Contents" item are message nos. 71 & 72

$ mnemonic and label for the "Reference" item
7 R
8 Referens...

$ mnemonic and label for the "On Item" item
9 O
10 Objekthjälp

$ mnemonic and label for "Using Help" item
11 j
12 Om hjälpfunktionen...

$ mnemonic and label for "About Text Editor" item
13 e
14 Om Textredigerare...

$ *
$ * Mnemonics and labels for the "File" pulldown menu and items.
$ *
$ mnemonic and label for the "File" pulldown menu
15 F
16 Fil

$ mnemonic and label for the "New" item
17 N
18 Nytt

$ mnemonic and label for the "Open" item
19 Ö
20 Öppna...

$ mnemonic and label for the "Include" item
21 I
22 Inkludera...

$ mnemonic and label for the "Save" item
$ NOTE: The "Save" label can be either Message 24 or message 73
23 p
24 Spara

$ mnemonic and label for the "Save As" item
25 m
26 Spara som...

$ mnemonic and label for the "Print" item
27 u
28 Skriv ut...

$ mnemonic and label for the "Close" item
29 S
30 Stäng

$ *
$ * Mnemonics and labels for the "Edit" pulldown menu and items.
$ *
$ mnemonic and label for the "Edit" pulldown menu
31 R
32 Redigera

$ mnemonic and label for the "Undo" menu item
33 Å
34 Ångra

$ mnemonic and label for the "Cut" menu item
35 t
36 Klipp ut

$ mnemonic and label for the "Copy" menu item
37 K
38 Kopiera

$ mnemonic and label for the "Paste" menu item
39 n
40 Klistra in

$ mnemonic and label for the "Clear" menu item
41 f
42 Töm fönstret

$ mnemonic and label for the "Delete" menu item
43 b
44 Ta bort

$ ***UNUSED***	Messages 45-50 are no longer used and need not be localized.
$		They have been replaced by messages 101-106 of this set.
$ mnemonic and label for the "Select All" menu item
$ 45 a
$ 46 Markera allt

$ mnemonic and label for the "Find/Change" menu item
$ 47 S
$ 48 Sök/ersätt...

$ mnemonic and label for the Check Spelling menu item
$ 49 S
$ 50 Kontrollera stavning...

$ *
$ * Mnemonics and labels for the "Format" pulldown menu and items.
$ *
$ mnemonic and label for the "Format" pulldown menu
51 r
52 Format

$ mnemonic and label for the "Settings..." menu item
53 I
54 Inställningar...

$ mnemonic and label for the "Paragraph" menu item
55 S
56 Stycke

$ mnemonic and label for the "All" menu item
57 l
58 Hela dokumentet

$ *
$ * Mnemonics and labels for the "Options" pulldown menu and items.
$ *
$ mnemonic and label for the "Format" pulldown menu
59 v
60 Alternativ

$ mnemonic and label for the "Overstrike" toggle button
61 ö
62 Skriv över

$ mnemonic and label for the "Word Wrap" toggle button
$ ***UNUSED***	This label and mnemonic are no longer used and need not be
$		localized.  They've been replaced by messages 111 and 112.
63 b
64 Radbyte

$ mnemonic and label for the "Status Line" toggle button
65 S
66 Statusrad

$ *
$ * Late additions.
$ *

$ nmemonic and label for the "Table of Contents" item of the "Help" menu
$ ***UNUSED*** 	This nmemonic and label are no longer used and need not be
$		localized.  They've been replaced by messages 87 and 88.
71 I
72 Innehållsförteckning...

$ label for the "Save" item in the "File" pulldown menu
$ NOTE: This label is used in place of message 24 when text has been
$       changed but has not been saved
73 Spara (ändringar)


$ *
$ * Mnemonics and labels for the "Help" pulldown menu and items.
$ *
$ Mnemonic and label for the "Help" pulldown menu
81 H
82 Hjälp

$ mnemonic and label for the "Overview" item
83 v
84 Översikt

$ mnemonic and label for the "Tasks" item
85 t
86 Steg-för-steg-anvisningar

$ nmemonic and label for the "Table of Contents" item
87 I
88 Innehållsförteckning

$ mnemonic and label for the "Reference" item
89 R
90 Referens

$ mnemonic and label for the "On Item" item
91 O
92 Objekthjälp

$ mnemonic and label for "Using Help" item
93 h
94 Om hjälpfunktionen

$ mnemonic and label for "About Text Editor" item
95 e
96 Om Textredigerare


$ *
$ * Changes for "Edit" pulldown menu messages 45-50.
$ *

$ mnemonic and label for the "Select All" menu item
101 a
102 Markera allt

$ mnemonic and label for the "Find/Change" menu item
103 S
104 Sök/ersätt...

$ mnemonic and label for the Check Spelling menu item
105 K
106 Kontrollera stavning...

$ mnemonic and label for the "Copy As" File menu item
$ (should match the dialog title specified in message 42 of set 5)
$ ***UNUSED***	This label is no longer used and need not be localized.
107 m
108 Kopiera som...

$ mnemonic and label for the "Copy To File" File menu item
$ (should match the dialog title specified in message 42 of set 5)
109 f
110 Kopiera till fil...

$ mnemonic and label for the "Wrap To Fit" toggle button
111 r
112 Anpassa rader


$ ***********************************************************
$ **                                              helpCB.c **
$ **                                                       **
$ ** Messages and strings associated with online help.     **
$ **                                                       **
$ ***********************************************************
$set 12
$ Message used for "Help - On Item" if the user clicks outside application
1 Du måste markera ett objekt i Textredigeraren.



$ ***********************************************************
$ **                                        ttMsgSupport.c **
$ **                                                       **
$ ** Messages and strings associated with ToolTalk.        **
$ **                                                       **
$ ***********************************************************
$set 13
1 Kan ej spara Checkpoint.
2 Kan ej hantera svar från Checkpoint.

$ NEW message after the SAMPLE IMPLEMENTATION.... more errors/warnings can 
$ be detected while reading in a file via ToolTalk.
3 Det går inte att fastställa innehållet i textredigeraren (okänd orsak).
