$ $TOG: dtpad.msg /main/20 1999/09/15 14:19:25 mgreess $
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
$ **  (c) Copyright 1993, 1994 Hewlett-Packard Company  
$ **  (c) Copyright 1993, 1994 International Business Machines Corp.  
$ **  (c) Copyright 1993, 1994 Sun Microsystems, Inc.  
$ **  (c) Copyright 1993, 1994 Unix System Labs, Inc., a subsidiary of Novell, Inc.  
$ *****************************************************************************  
$ **  
$quote "

$set 2
$ **********************************************
$ **  General messages used in multiple files **
$ **********************************************
$ Label used for all "Help" buttons in all DIALOGS.
$ It is not used for the Help menu.

1 "Hilfe"

$quote

$ Unrecognized arguments
2 dtpad: Ungültige Option: %s 
$ Extra arguments
3 dtpad: Zusätzliches Argument: %s


$set 4        Redefine set#

$ *****************************************************************
$ **                                                    fileCB.c **
$ ** These are the strings which change between the Open dialog  **
$ ** and the Include dialog.  These two operations use the same  **
$ ** Motif dialog, and simply change these two labels.           **
$ *****************************************************************
$ Title of the Open dialog

$quote "
1 "Datei öffnen"
$ label for the File text field in the file selection box
$ ***UNUSED***\tThis label is no longer used and need not be localized.

2 "Zu öffnende Datei:"
$ Title of the Include dialog

3 "Datei einfügen"
$ Label for the File text field in the file selection box
$ ***UNUSED***\tThis label in no longer used and need not be localized.

4 "Einzufügende Datei:"
$ Error when printing a file and a temporary copy can not be saved.

5 "Temporäre Datei kann nicht erstellt werden."

$set 5        Redefine set#

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

1 "Änderungen in %s speichern?"
$ This is used as-is, whenever the current file is unnamed.

2 "Die aktuelle Datei hat keinen Namen. Soll die Datei gespeichert werden?"
$ This is used to build a message like "Do you want to save the current
$ copy of FILENAME before you Edit NEWFILENAME?" when a new file is droped
$ on the edit window.
$ ***UNUSED***\tCurrently, dropped files are INSERTED in the text rather than
$ \topened for editing so this message is no longer used and need
$ \tnot be localized.

3 "Soll die aktuelle Kopie \n\
von %s gesichert \n\
werden, bevor %s bearbeitet wird?"
$ Used when about to write over an existing file.
$ The "%s" is replaced by the file name.

5 "Die Datei %s ist bereits vorhanden.\n\
Soll diese Datei durch die derzeit bearbeitete überschrieben werden?"
$ Title of "file already exists" dialog.  Also used for other "Warning" type
$ dialogs.

6 "Warnung"
$ Title of "Save As" dialog.

7 "Speichern als"
$ Labels for add/don't-add newlines radio buttons in the Save As dialog

8 "Zeilenumbruchzeichen an das Ende von Zeilen mit Worttrennung stellen."

9 "Keine Zeilenumbrüche einfügen. Es werden nur die \n\
mit der Rücklauftaste erzeugten Zeilenumbrüche übernommen."
$ Labels for the Save Warning dialog. Also used in the confirmation, or
$ Editor Return dialog.

14 "Nein"

15 "Ja"
$ Title of the dialog - used when user selects "New", "Open", or "Exit" with
$ unsaved changes in the text.

16 "Speichern?"
$ Title of general error dialog

17 "Fehler"
$ Title of the resource-specified "confirmation" dialog
$ This dialog is generally specified by programs such as mailers, which
$ want to know if the user wants to send the message.
$ There is no default message for the dialog, since it is _only_ posted if
$ the message string is specified via a resource (dtpad*confirmationString:)
$ This message is only used if the "dtpad*mainTitle:" resource is not set.
$ If this resource _is_ set, the the mainTitle is also used as the title
$ for the exit confirmation dialog.
$ DO NOT LOCALIZE - the user should never see this message

18 "Exit Confirmation"
$ Title for general information dialog. e.g. Find dialog can't find word in
$ file.

20 "Information"
$ Default file name in Save As dialog box.

21 "UNBENANNT"
$ Error message when a file can not be saved.  The "%1$s" gets replaced by
$ the file name and the "%2$s" gets replaced by the reason why the file could
$ not be saved (messages 31-39).

30 "%1$s kann nicht gespeichert werden.\n\
%2$s"
$ Messages 31-39 are reasons why a file could not be saved.
$ Messages 34-39 are substitued in message 30 for "%2$s".

31 "Datei kann nicht gespeichert werden.\n\
Es wurde kein Name angegeben."

34 "Diese Datei kann von keinem Benutzer geändert werden."

35 "Diese Datei ist ein Verzeichnis."

36 "Diese Datei ist eine zeichenorientierte Gerätedatei."

37 "Diese Datei ist eine blockorientierte Gerätedatei."
$ ***UNUSED***\tThis message should never appear since currently there is
$ \tonly one supported file format - text.  And this type is
$ \thard coded into dtpad.

38 "Ungültiges Dateiformat."
$ This is a "catch all" when the file can not be saved and none of the above
$ errors apply.

39 "Ursache unbekannt - nicht genügend Speicherplatz verfügbar?"
$ This is displayed when a ToolTalk edit request on a buffer is received,
$ a Save operation is done and there are unsaved changes.

40 "Änderungen speichern?"
$ This is displayed when a ToolTalk edit request on a file or buffer is
$ received with a document name specified in the request, a Save operation
$ is done and there are unsaved changes.  The "%s" in the message is
$ replaced by the document name (instead of the filename).

41 "Änderungen speichern in \"%s\"?"
$ Title of "Copy As" dialog which is substitued for the "Save As" dialog
$ when file name changes are disallowd.  This should match the menu button
$ label specified in message 108 of set 11.

42 "Kopieren als"
$ Title of "Copy To File" dialog which is substitued for the "Save As" dialog
$ when file name changes are disallowd.  This should match the menu button
$ label specified in message 110 of set 11.

43 "In Datei kopieren"
$ Labels for add/don't-add newlines radio buttons in the SaveAs and Save dialogs

44 "Zeilenumbruchzeichen am Ende von Zeilen einfügen, für die mit\n\
der Option 'Zeilenumbruch an Fenster anpassen' ein Umbruch durchgeführt wurde."

45 "Keine Zeilenumbruchzeichen hinzufügen. Es bleiben nur Zeilenumbrüche\n\
erhalten, die durch Drücken der Rücklauftaste eingegeben wurden."

$ 
$ New message as of 5/31/95 (i.e. new since Sample Implementation). This
$ new message is in response to bug DTS CISlx19530.
$ 

46 "Entweder ist die Datei nicht beschreibbar, oder der Pfad zu der Datei\nexistiert nicht."

$ 
$ "-NEW"
$ 
47 "Operation failed due to insufficient memory.\nTry increasing swap space."

$ 
$ "-NEW"
$ 
48 "Format operation failed."

$set 6        Redefine set#

$ ************************************************************************
$ **                                                           fileIo.c **
$ **                                                                    **
$ ** Error messages generated when there are problems reading a file.   **
$ ************************************************************************

1 "Diese Datei kann nicht editiert werden. Nur Lesezugriff."

2 "Datei existiert nicht."

3 "Ein Verzeichnis kann nicht editiert werden."

4 "Eine zeichenorientierte Gerätedatei kann nicht editiert werden."

5 "Eine blockorientierte Gerätedatei kann nicht editiert werden."

6 "Eingebettete Nullen aus Datei entfernt."

7 "Datei kann nicht geladen werden (nicht genügend Hauptspeicher)."

8 "Kein Zugriff auf Datei (Status kann nicht ermittelt werden)."

9 "Lesen aus Datei nicht möglich (wahrscheinlich keine Leseberechtigung)."

$ Message placed in the status line message area based on the -viewOnly  
$ resource and whether or not the file being edited is read only  

10 "Nur anzeigen"

11 "Nur lesen"

$quote

12 Kann nicht von Datei lesen (Grund unbekannt).


$set 7        Redefine set#

$ ************************************
$ **                         main.c **
$ **                        dtpad.c **
$ ** Prefix string for all dialogs. **
$ ************************************
$ bogus?? blank string gotten but otherwise unused in main()
$ DO NOT LOCALIZE string 1

$quote "
1 ""
$ The name used in the title bar of every window.
$ Be sure to end this message with a space character.

2 "Texteditor - "
$ Default file name used in the main window title.
$ ***UNUSED***\tThis mesage has been replaced by message 6 of this set and
$ \tneed not be localized.

3 "(unbenannt)"
$ Terminal error message displayed to stdout when the desk top services
$ can not be initialized (via DtInitialize())

4 "Desktop-Services können nicht initialisiert werden ... Texteditor wird verlassen."
$ Terminal error message displayed to stdout when a connection to the
$ display can not be opend (via XtDisplay())

5 "Anzeige kann nicht geöffnet werden ... Texteditor wird verlassen."
$ Default file name used in the main window title.
$ This should correspond to the default file name used in the "Save As"
$ dialog box - specified in message 21 of set 5.

6 "(UNBENANNT)"
$ 
$ New messages (7,8) since CDE/SI - error gets displayed when user tries to run
$ dtpad -server when and dtpad server process is already running on the
$ $DISPLAY.
7 "FEHLER: Ein dtpad-Server auf %s\n\
arbeitet bereits als Server für diese Anzeige."
8 "FEHLER: Ein dtpad-Server\n\
arbeitet bereits als Server für diese Anzeige."
$ 
$ Message 9 is also new since CDE/SI - new as of 5/22/95.  It is a usage message
$ for dtpad. Don't translate the '-OPTION' just the text explaining the
$ option.
$ 
9 "'%s' ist eine ungültige Option.\n\
Syntax: %s...\n\
   -saveOnClose\n\
\tDtpad speichert automatisch ohne zusätzliche Abfrage den aktuellen\n\
\tText, wenn beim Schließen des Texteditors nicht gespeicherte Änderungen\n\
\tvorhanden sind.\n\n\
   -missingFileWarning\n\
\tDurch diese Option wird eine Warnung angezeigt, wenn ein angegebener\n\
\tDateiname nicht vorhanden ist oder auf die Datei nicht zugegriffen\n\
\twerden kann.\n\n\
   -noReadOnlyWarning\n\
\tDurch diese Option wird eine Warnung angezeigt, wenn eine Datei\n\
\tangegeben wird, für die der Benutzer keine Schreibberechtigung besitzt.\n\n\
   -noNameChange\n\
\tDurch diese Option wird angezeigt, daß der Standarddateiname, der dem\n\
\taktuellen Text zugeordnet ist, nicht geändert werden darf, wenn der\n\
\tText unter einem anderen Namen gespeichert wird als dem, unter dem\n\
\tdie Datei eingelesen wurde.\n\n\
   -viewOnly\n\
\tDurch diese Option wird das Bearbeiten von Text im Editierfenster\n\
\tverhindert (d. h. der Texteditor fungiert bei Angabe dieser Option\n\
\tnur als Anzeigeprogramm).\n\n\
   -statusLine\n\
\tDurch diese Option wird eine Statuszeile am unteren Rand des\n\
\tEditorfenster angezeigt. In der Statuszeile wird die Nummer der\n\
\tZeile angezeigt, in der sich der Cursor momentan befindet.\n\n\
   -wrapToFit\n\
\tDurch diese Option wird der Modus 'Zeilenumbruch an Fenster anpassen'\n\
\tbeim Starten aktiviert.\n\n\
   -workspaceList Arbeitsbereichsliste\n\
\tDurch diese Option wird das Editorfenster beim aktuellen Aufruf des\n\
\tTexteditors im/in den angegebenen Arbeitsbereich/en angezeigt.\n\n\
   -session Session-Datei\n"
10 "\tDurch diese Option werden alle Editorfenster und Einstellungen,\n\
\tdie beim Beenden einer vorherigen CDE-Sitzung wirksam waren,\n\
\twiederhergestellt. Bei Angabe dieser Option werden alle anderen\n\
\tBefehlszeilenoptionen ignoriert.\n\n\
   -standAlone\n\
\tDurch diese Option führt der Texteditor seine eigene Textverarbeitung\n\
\tin seinem eigenen Fenster aus, unabhängig vom Server des Texteditors.\n\n\
   -noBlocking\n\
\tDurch diese Option wird der Anforderungsprozeß des Texteditors\n\
\tbeendet, sobald vom Texteditor festgestellt wird, daß die\n\
\tAnforderung zum Bearbeiten verarbeitet werden kann.\n\n\
   -server\n\
\tDurch diese Option wird ein Texteditor-Server gestartet (falls\n\
\tnicht bereits ein Server aktiv ist), um alle nachfolgenden\n\
\tEditieranforderungen für die Anzeige zu verarbeiten.\n\n\
   -exitOnLastClose\n\
\tDurch diese Option wird angegeben, daß der Server-Prozeß des\n\
\tTexteditors beendet werden soll, wenn das letzte Editierfenster\n\
\tfür die Anzeige geschlossen wird. Diese Option sollte nur mit der\n\
\tOption -server verwendet werden, da sie nur auf den Server-Prozeß\n\
\tanwendbar ist.\n\n\
\tNähere Einzelheiten zu diesen Optionen enthält die Handbuchseite zu dtpad.\n\n"
$ 

$ "NEW-"
11 "Exiting text editor ..."
$ "NEW-"
12 "Running in standalone mode ..."
$ "NEW-"
13 "Could not connect to ToolTalk service:\n%s\n%s\n"

$set 10        Redefine set#

$ ************************************************************************
$ **                                                          session.c **
$ ** THESE MESSAGES ARE _ONLY_ WRITTEN TO THE ERRORLOG FILE.
$ **
$ ************************************************************************
$ Strings used to build error messages issued when the session save
$ file cannot be read or written.

1 "Zugriffsberechtigungen prüfen."

2 "%s ist die Datei, in der die Session gespeichert worden wäre."

$set 11        Redefine set#

$ ***********************************************************
$ **                                              dtpad.c **
$ **                                                       **
$ ** These are all of the menu button labels and mnemonics **
$ ***********************************************************
$ ***UNUSED*** \tMessages 1-14 are no longer used and need not be localized.
$ \tAll of the mnemonics and labels relating to the "Help" menu
$ \thave been replaced by messages 81-96.
$ *
$ * Mnemonics and labels for the "Help" pulldown menu and items.
$ *
$ Mnemonic and label for the "Help" pulldown menu

1 "H"

2 "Hilfe"
$ mnemonic and label for the "Overview" item

3 "E"

4 "Einführung..."
$ mnemonic and label for the "Tasks" item

5 "A"

6 "Aufgaben..."
$ nmemonic and label for the "Table of Contents" item are message nos. 71 & 72
$ mnemonic and label for the "Reference" item

7 "R"

8 "Referenzinformationen..."
$ mnemonic and label for the "On Item" item

9 "Z"

10 "Kontexthilfe"
$ mnemonic and label for "Using Help" item

11 "H"

12 "Hilfe für Hilfe..."
$ mnemonic and label for "About Text Editor" item

13 "f"

14 "Texteditor - Info..."
$ *
$ * Mnemonics and labels for the "File" pulldown menu and items.
$ *
$ mnemonic and label for the "File" pulldown menu

15 "D"

16 "Datei"
$ mnemonic and label for the "New" item

17 "r"

18 "Erstellen "
$ mnemonic and label for the "Open" item

19 "f"

20 "Öffnen..."
$ mnemonic and label for the "Include" item

21 "i"

22 "Einfügen..."
$ mnemonic and label for the "Save" item
$ NOTE: The "Save" label can be either Message 24 or message 73

23 "S"

24 "Speichern"
$ mnemonic and label for the "Save As" item

25 "a"

26 "Speichern als..."
$ mnemonic and label for the "Print" item

27 "D"

28 "Drucken..."
$ mnemonic and label for the "Close" item

29 "c"

30 "Schließen"
$ *
$ * Mnemonics and labels for the "Edit" pulldown menu and items.
$ *
$ mnemonic and label for the "Edit" pulldown menu

31 "B"

32 "Bearbeiten "
$ mnemonic and label for the "Undo" menu item

33 "R"

34 "Rückgängig machen "
$ mnemonic and label for the "Cut" menu item

35 "A"

36 "Ausschneiden"
$ mnemonic and label for the "Copy" menu item

37 "K"

38 "Kopieren"
$ mnemonic and label for the "Paste" menu item

39 "i"

40 "Einfügen"
$ mnemonic and label for the "Clear" menu item

41 "e"

42 "Leeren"
$ mnemonic and label for the "Delete" menu item

43 "L"

44 "Löschen "
$ ***UNUSED***\tMessages 45-50 are no longer used and need not be localized.
$ \tThey have been replaced by messages 101-106 of this set.
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

51 "F"

52 "Formatieren"
$ mnemonic and label for the "Settings..." menu item

53 "E"

54 "Einstellungen... "
$ mnemonic and label for the "Paragraph" menu item

55 "A"

56 "Absatz"
$ mnemonic and label for the "All" menu item

57 "l"

58 "Alle"
$ *
$ * Mnemonics and labels for the "Options" pulldown menu and items.
$ *
$ mnemonic and label for the "Format" pulldown menu

59 "O"

60 "Optionen"
$ mnemonic and label for the "Overstrike" toggle button

61 "b"

62 "Überschreibmodus"
$ mnemonic and label for the "Word Wrap" toggle button

63 "Z"

64 "Zeilenumbruch"
$ mnemonic and label for the "Status Line" toggle button

65 "S"

66 "Statuszeile"
$ *
$ * Late additions.
$ *
$ nmemonic and label for the "Table of Contents" item of the "Help" menu
$ ***UNUSED*** \tThis nmemonic and label are no longer used and need not be
$ \tlocalized.  They've been replaced by messages 87 and 88.

71 "I"

72 "Inhalt..."
$ label for the "Save" item in the "File" pulldown menu
$ NOTE: This label is used in place of message 24 when text has been
$       changed but has not been saved

73 "Speichern (erforderlich)"
$ *
$ * Mnemonics and labels for the "Help" pulldown menu and items.
$ *
$ Mnemonic and label for the "Help" pulldown menu

81 "H"

82 "Hilfe"
$ mnemonic and label for the "Overview" item

83 "b"

84 "Überblick"
$ mnemonic and label for the "Tasks" item

85 "A"

86 "Aufgaben"
$ nmemonic and label for the "Table of Contents" item

87 "I"

88 "Inhaltsverzeichnis"
$ mnemonic and label for the "Reference" item

89 "R"

90 "Referenzinformationen"
$ mnemonic and label for the "On Item" item

91 "K"

92 "Kontexthilfe"
$ mnemonic and label for "Using Help" item

93 "H"

94 "Hilfe für Hilfe"
$ mnemonic and label for "About Text Editor" item

95 "z"

96 "Informationen zu Texteditor"
$ *
$ * Changes for "Edit" pulldown menu messages 45-50.
$ *
$ mnemonic and label for the "Select All" menu item

101 "w"

102 "Alles auswählen"
$ mnemonic and label for the "Find/Change" menu item

103 "S"

104 "Suchen/Ersetzen..."
$ mnemonic and label for the Check Spelling menu item

105 "p"

106 "Rechtschreibprüfung..."
$ mnemonic and label for the "Copy As" File menu item
$ (should match the dialog title specified in message 42 of set 5)

107 "K"

108 "Kopieren als..."
$ mnemonic and label for the "Copy To File" File menu item
$ (should match the dialog title specified in message 42 of set 5)

109 "D"

110 "In Datei kopieren..."
$ mnemonic and label for the "Wrap To Fit" toggle button

111 "F"

112 "Zeilenumbruch an Fenster anpassen"

$ Label for the "Print" item
113 Drucken

$ ***********************************************************
$ **                                              helpCB.c **
$ **                                                       **
$ ** Messages and strings associated with online help.     **
$ **                                                       **
$ ***********************************************************

$set 12        Redefine set#

$ Message used for "Help - On Item" if the user clicks outside application

1 "Es muß ein Element innerhalb des Texteditors ausgewählt werden."
$ ***********************************************************
$ **                                        ttMsgSupport.c **
$ **                                                       **
$ ** Messages and strings associated with ToolTalk.        **
$ **                                                       **
$ ***********************************************************

$set 13        Redefine set#


1 "Speichern als Prüfpunktversion gescheitert."

2 "Keine Antwort bei Prüfpunktanforderung."

$ more errors/warnings can
$ be detected while reading in a file via ToolTalk.
3 "Inhalt des Texteditors kann nicht festgelegt werden (unbekannte Ursache)."


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
1 "Die Syntax einer der folgenden Randangaben\n ist nicht korrekt: \n %s \n %s \n %s \n %s \nStattdessen Standardränder benutzen?"
$ 
$	Messages 2 - 7 are messages for errors which can occur in the
$	process of loading a dtpad document file for printing.	
$ 
2 "Datei existiert nicht."
3 "Angegebene Datei ist ein Verzeichnis."
4 "Dateityp-Fehler."
5 "Datei enthält NULL-Zeichen."
6 "Kann Datei nicht laden (nicht genügend Speicher)."
7 "Datei hat kein Lese-Zugriffsrecht."

$	Messages 8 - 11 are format strings used to construct the header
$	and footer strings which appear in the printing output.
$ 
$	Message 8 is the format string used to make a Date header/footer.
$	Message 9 is the format string used to make a DocName header/footer.
$	Message 10 is the format string used to make a Page# header/footer.
$	Message 11 is the format string used to make a UserName header/footer.
8 "Datum:  %s"
9 "Dokument:  %s"
10 "Seite %d von %d"
11 "Dokument für:  %s"

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
12 "Dokumentname: "
13 "Zeilenumbruch an Fenster anpassen."


$	Messages 14 - 15 are status messages for the PrintToFile function.
$ 
$	Message 14 indicates that the PrintToFile function was unsuccessful.
$	Message 15 indicates that the PrintToFile function was successful.
$	Messages 16 - 19 are status messages for the popup PDM function.
$       NOTE:  'Print to File' should be translated as in DtPrint.msg
$               Set 1, Message 3.
14 "'In Datei drucken' fehlgeschlagen."
15 "'In Datei drucken' erfolgreich beendet."

$          Messages 16 - 19 are obsolete, and have been replaced by 24
$ 
$	Message 16 indicates that there was no pdm deamon running.
$	Message 17 indicates that the pdm deamon was unable to start a pdm.
$	Message 18 indicates that the pdm couldnt connect to the video display.
$	Message 19 indicates that the pdm couldnt connect to the print display.
16 "Keinen PDM in der Umgebung gefunden."
17 "Kann PDM nicht starten."
18 "PDM ist nicht zum Anschluß an einen Video-Bildschirm berechtigt."
19 "PDM ist nicht zum Anschluß an einen Druckbildschirm berechtigt."

$	Messages 20 - 22 are status messages displayed while printing
$ 
$	Message 20 indicates that the user has asked to close the editor
$		   window.  Dtpad is prevented from doing so until the
$		   active print jobs have completed.
$	Message 21 status message displayed when the print job is started.
$	Message 22 status message displayed as the print job progresses.
20 "Schließen anstehend: wartet auf Beendung der Aufgabe..."
21 "Druckt %s ..."
22 "Druckt %s: Seite %d von %d ..."

$       Message 23 is used in the construction of a title string for the
$                  Print Setup dialog.
23 " Druckereinstellungen - "

$	Message 24 is used to alert the user to the fact that the X Print
$		   Server cannot complete the print request due to a lack
$		   of resources.
24 "Dem X-Druckserver fehlen vorübergehend die notwendigen Ressourcen"

$       Messages 25 is used to handle errors from the PDM
25 "Druckdialogmanager (PDM)-Fehler - Einstellungen fehlgeschlagen."

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
1 "Leer"
2 "Datum"
3 "Dokumentname"
4 "Seitenzahl"
5 "Benutzername"

$	Messages 6-9 are used to construct error messages informing users
$	that they have input an invalid print page margin specification for 
$ 
6 "Obere Randangabe ist ungültig:  "
7 "Rechte Randangabe ist ungültig:  "
8 "Untere Randangabe ist ungültig:  "
9 "Linke Randangabe ist ungültig:  "
