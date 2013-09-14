$ "$XConsortium: dtstyle.msg /main/10 1996/11/19 18:08:39 rws $"  
$ *************************************<+>*************************************  
$ *****************************************************************************  
$ **  
$ **  File:        dtstyle.msg  
$ **  
$ **  Project:     DT Style Manager  
$ **  
$ **  Description:  
$ **  -----------  
$ **  This file is the source for the message catalog for dtstyle  
$ **  
$ **  
$ *****************************************************************************  
$ **  
$ **     (c) Copyright 1990 Hewlett-Packard Company  
$ **     All Rights reserved  
$ **  
$ **  
$ *****************************************************************************  
$ *****************************************************************************  
$   
$ ***** NOTE FOR MESSAGE CATALOG TRANSLATORS *****  
$   
$ There may be three types of messages in this file:  
$   
$ 1. Messages that appear in dialogs or are displayed to the user.  
$   
$     These messages are the default and they should ALL BE LOCALIZED.  
$     Note that these messages do NOT have any identification (see the  
$     comments for type 2 and 3 below).  
$   
$ 2. Messages that only appear in the DT error log file ($HOME/.dt/errorlog).  
$   
$     The localization of these messages is OPTIONAL.  These messages are  
$     identified by the following:  
$   
$  MESSAGES xx-yy IN SET zz WILL ONLY APPEAR IN THE DT ERRORLOG FILE  
$   
$ 3. Messages that should not be localized.    
$   
$     These messages are identified by the following:  
$   
$  DO NOT TRANSLATE or CHANGE or LOCALIZE MESSAGES xx-yy from set zz  
$   
$ ***** END (NOTE FOR MESSAGE CATALOG TRANSLATORS) *****  
$   
$ ******************************************************************************  
$ *************************************<L>*************************************  
$ **  -----------------GENERAL LOCALIZATION NOTES SECTION---------------------  
$ * Comment lines begin with a $ except $set which indicates the start of a   
$ * new set.  
$ *  
$ * Do not delete any comments; you may add comments for your use.  
$ *  
$ *   
$ **  ----------------GENERAL LOCALIZATION NOTES SECTION END------------------  
$ *************************************<L>*************************************  
$quote "

$set 2
$ Message 1 from set 2 was deleted from the message catalog  
$ _DtMessage 2 is the title of all Information dialog boxes  

2 "Hinweis"
$ _DtMessage 3 is the title of all Error dialog boxes  

3 "Fehler"
$ _DtMessage 4 WILL ONLY APPEAR IN THE DT ERRORLOG FILE  

4 "Keine Kommunikation mit Sitzungssteuerung...\n\
Anforderung wird beendet."
$  MESSAGES 5-6 IN SET 2 WILL ONLY APPEAR IN THE DT ERRORLOG FILE  

5 "Umgebungsverwaltung bereits aktiv,\n\
zweiter Versuch abgebrochen."

6 "Fehler bei X Toolkit aufgetreten...\n\
Anforderung wird beendet."
$ _DtMessage 7 is the title of all Help dialog boxes  

7 "Umgebungsverwaltung - Hilfe"
$ _DtMessage 8 is an error message that appears when you are using Itemh Help  
$ and select on something outside of the Style Manager  

8 "Es muß ein Element innerhalb der \n\
Umgebungsverwaltung ausgewählt werden."
$ _DtMessage 9 WILL ONLY APPEAR IN THE DT ERRORLOG FILE  

9 "Es konnten keine Informationen zum \n\
Bildschirmschoner von der \n\
Sitzungsverwaltung abgerufen werden.\n\
Die Starteinstellungen sind \n\
möglicherweise falsch."

$set 4        Redefine set#

$ _DtMessage set 4 is for the Style Manager main window  
$ Messages 6,8-26,34,39 from set 4 were deleted from the message catalog  
$ _DtMessage 1 is the title of the main dtstyle window  

1 "Umgebungsverwaltung"
$ mnemonic and label for the File menubar item.  

2 "D"

3 "Datei"
$ mnemonic and label for the Help menubar item.  

4 "H"

5 "Hilfe"
$ _DtMessage 7 is the label for the Exit button in the File pulldown  
$ the mnenomic for the Exit button is set 4 _DtMessage 40  

7 "Beenden"
$ Messages 27-33,35  are the labels on the buttons in the dtstyle main window.  

27 "Farbe"

28 "Schrift"

29 "Hintergrund"

30 "Tastatur"

31 "Maus"

32 "Signalton"

33 "Anzeige"

35 "Neustart"
$  MESSAGE 36 IN SET 4 WILL ONLY APPEAR IN THE DT ERRORLOG FILE  

36 "No Dtstyle restore information available for current session"
$ _DtMessage 37 is the label on the dtstyle icon  

37 "Umgebungsverwaltung"
$ Message 38 is the label on the Window button in the dtstyle main window.  

38 "Fenster"
$ _DtMessage 40 is replaced with message 53  
$ 40 E  
$ *  
$ * mnemonic and label for the Help menu items.  
$ *   
$ Messages 41-42 have been replaced with messages 54-55  
$ Overview... menu item  
$ 41 I  
$ 42 Introduction...  
$ Tasks... menu item   

43 "A"
$ Message 44 has been replaced with messages 56  
$ 44 Tasks...  
$ Reference... menu item  
$ 45 R  
$ Message 46 has been replaced with messages 57  
$ 46 Reference...  
$ On Item menu item  
$ Keybaord menu item  

47 "Z"

48 "Zu Element..."
$ Using Help... menu item  

49 "H"
$ Message 50 has been replaced with messages 58  
$ 50 Using Help...   
$ Messages 51-52 have been replaced with messages 59-60  
$ Overview menu item  
$ 51 V  
$ 52 Version...  
$ Message 53 is replaced with message 40  

53 "x"
$ Messages 54-55 are replacements for messages 41-42  

54 "v"

55 "Übersicht"
$ Message 56 is a replacement for messages 44  

56 "Aufgaben"
$ Message 57 is a replacement for messages 46  

57 "Referenzinformation"
$ Message 58 is a replacement for messages 50  

58 "Zu Hilfe"
$ Messages 59-60 are replacements for messages 51-52  

59 "A"

60 "Informationen zu Umgebungsverwaltung"

$set 5        Redefine set#

$ _DtMessage set 5 is for the Font dialog  
$ Messages 1-6, 7-16 from set 5 were deleted from the message catalog  
$ _DtMessage 17 is the label of the titlebox of the preview font area  

17 "Voranzeige"
$ _DtMessage 18 appears in a Notice dialog after the user selects a new font  
$ and presses Ok - when the resource *WriteXrdbImmediate is True  

18 "Die Schriftartänderungen sind bei einigen Anwendungen \n\
beim nächsten Starten wirksam.\n\
In anderen Anwendungen wie dem Datei-Manager und\n\
dem Anwendungs-Manager werden die Schriftartänderungen\n\
erst nach Verlassen des Desktop und anschließendem \n\
erneutem Anmelden wirksam."
$ _DtMessage 19 appears in a Notice dialog after the user selects a new font  
$ and presses Ok - when the resource *WriteXrdbImmediate is False  

19 "Die gewählte Schrift wird verwendet, wenn\n\
diese Sitzung neu gestartet wird."
$ _DtMessage 20 and 21 is the text that initially appears in the Font Preview   
$ area displaying sample user and system fonts.  

20 "AaBbCcDdEeFfGg0123456789"

21 "AaBbCcDdEeFfGg0123456789"
$ _DtMessage 22 is the label of the titlebox containing the list of font sizes  

22 "Größe"
$ _DtMessage 23 is the title of the Fonts dialog  

23 "Umgebungsverwaltung - Schrift"
$ _DtMessage 24 is an informative message used by the Fonts dialog 

24 "Die momentan fM-|r das Desktop verwendete Schriftart steht\n\
nicht in der Liste 'GrM-vM-_e' zur VerfM-|gung. Wird eine neue Schriftart\n\
ausgewM-dhlt und angewendet, ist es nicht mM-vglich, mit Hilfe des\n\
Dialogfensters 'Umgebungsmanager - Schrift' zur aktuellen Schriftart\n\
zurM-|ckzukehren."

$set 6        Redefine set#

$ _DtMessage set 6 is for the Beep dialog  
$ _DtMessage 1,6 from set 6 have been deleted from the message catalog  
$ Messages 2 through 5 are the default buttons and labels of the scales.    

2 "Standard"

3 "Stärke"

4 "Höhe"

5 "Dauer"
$ _DtMessage 7 is the title of the Beep dialog box  

7 "Umgebungsverwaltung - Signalton"

$set 7        Redefine set#

$ _DtMessage set 7 is for the Startup dialog  
$ Messages 1-2 from set 7 have been deleted from the message catalog  
$ Messages 3 through 9 are labels of the buttons and text visible in the   
$ Session Startup dialog box.   

3 "Beim Anmelden:"

4 "Aktuelle Umgebung wiederaufnehmen"

5 "Zu Standardsitzung zurückkehren"

$ Message 6 is obsolete - DO NOT localize message #6 from this set
$ 6 "OBS- Beim Abmelden fragen"

7 "Dialog zur Abmeldungsbestätigung:"

8 "Ein"

9 "Aus"

10 "Standardsitzung festlegen ..."
$ _DtMessage 11 is a warning message when the user pushes Set Home Session...  
$ Notice the spaces before Continue .. the spaces make it centered.  

11 "Hiermit wird die Standardsitzung durch \n\
die aktuelle Sitzung ersetzt.\n\n\
                 Fortfahren?"
$ _DtMessage 12 is the title of the warning dialog box when Set Home Session is   
$ pushed.   

12 "Warnung"
$ _DtMessage 13 is an error message  

13 "Diese Sitzung wurde durch eine Startprozedur für \n\
X Window System gestartet. Die Starteinstellungen sind \n\
nicht gültig, da die Sitzung nicht wiederhergestellt \n\
werden kann."
$ _DtMessage 14 is the title of the Session Startup dialog box.  

14 "Umgebungsverwaltung - Neustart"
$ _DtMessage 15 is an error message  

15 "Die Sitzungsverwaltung ist nicht aktiv.\n\
Starteinstellungen sind nicht gültig."

$ Message 16 is used in the Sessin Startup dialog.  It is the
$   string that appears to right of the Startup dialog icon IF
$   the user selects a display-specfic session at the login screen.
16 "Bildschirmspezifische Session:"

$set 8        Redefine set#

$ All of the messages from set 8 have been deleted from the message catalog  

$set 9        Redefine set#

$ _DtMessage set 9 is for the Mouse dialog  
$ Message 7 from set 9 have been deleted from the message catalog  
$ Messages 1-4 are labels of toggle buttons in the Mouse Dialog.   

1 "Links"

2 "Rechts"

3 "Links-/Rechtshänder:"

4 "Taste 2:"
$ _DtMessage 5 is the label of default button.  

5 "Standard"
$ Messages 6,8,9 are labels of the scales.  

6 "Doppelklickzeit"

8 "Geschwindigkeit"

9 "Distanz"
$  MESSAGE 10 IN SET 9 WILL ONLY APPEAR IN THE DT ERRORLOG FILE  

10 "ERROR: setting pointer mapping\n"

11 "Übertragen"

12 "Anpassen"
$ _DtMessage 13 appears in a Notice dialog after the user selects the   
$ left-handed or right-handed toggle  

13 "Nach Klicken auf OK wird die Funktion von\n\
linker und rechter Maustaste vertauscht.\n\
Die Grafik 'Doppelklicken testen' im Mausfenster\n\
zeigt die aktuelle Mauskonfiguration."
$ _DtMessage 14 appears in a Notice dialog after the user selects the   
$ transfer toggle to change the function of the middle mouse button.  

14 "Bei Klicken auf OK wird Maustaste 2 in der nächsten\n\
Sitzung auf `Übertragen' (ziehen) gesetzt."
$ double-click time and presses Ok (and the resource   
$ *WriteXrdbImmediate is True)  

15 "Die neue Doppelklickzeit gilt, sobald die Anwendungen \n\
neu gestartet werden."
$ _DtMessage 16 appears in a Notice dialog after the user selects a new   
$ double-click time and presses Ok (and the resource   
$ *WriteXrdbImmediate is False)  

16 "Die neue Doppelklickzeit gilt ab der nächsten\n\
Sitzung."
$ _DtMessage 17 is the text below the mouse icon. Notice the two lines.  

17 "Doppelklicken\n\
testen"
$ _DtMessage 18 is the Mouse Dialog title  

18 "Umgebungsverwaltung - Maus"
$ _DtMessage 19 appears in a Notice dialog after the user selects the   
$ adjust toggle to change the function of the middle mouse button.  

19 "Bei Klicken auf OK wird Maustaste 2 bei der \n\
nächsten Sitzung auf 'anpassen' gesetzt. \n\
Taste 2 kann zum Anpassen von Text- und \n\
Listenauswahlmöglichkeiten verwendet werden.\n\
Übertragungsaktionen für Text und Listen \n\
erfordern die Verwendung von Taste 1."

$set 10        Redefine set#

$ _DtMessage set 10 is for the SaveRestore functionality  
$ Messages 2,4 from set 10 have been deleted from the message catalog  
$  MESSAGE 1,3 IN SET 10 WILL ONLY APPEAR IN THE DT ERRORLOG FILE  
$ Messages 1,3 make up one error message.  Put together, they say   
$ "<filename> <_DtMessage 2> <_DtMessage 1>  

1 "Dateiberechtigungen prüfen."

3 "%s ist die Datei, in der die Sitzung gespeichert worden wäre.\n"

$set 11        Redefine set#

$ _DtMessage set 11 is for the Backdrop dialog  
$  MESSAGE 1 IN SET 11 WILL ONLY APPEAR IN THE DT ERRORLOG FILE  
$ _DtMessage 1 - could not open a backdrop directory  

1 "Verzeichnis %s konnte nicht geöffnet werden. "
$ _DtMessage 2 - error opening the backdrop files  

2 "Der Hintergrundteil der Umgebungsverwaltung\n\
funktioniert nicht, da keine Hintergrunddateien\n\
zur Verfügung stehen. $HOME/.dt/errorlog überprüfen."
$ _DtMessage 12 is the title of the Backdrop dialog box  

12 "Umgebungsverwaltung - Hintergrund"

$set 12        Redefine set#

$ _DtMessage set 12 is for the Screen dialog  
$ Message 2 from set 12 has been deleted from the message catalog  
$ _DtMessage 1 is the title of the Screen dialog box.   

1 "Umgebungsverwaltung - Anzeige"
$ Messages 3-14 are the labels and buttons for the Screen dialog  

3 "Standard"

4 "Bildschirmschoner"

5 "Bildschirmschoner starten"

6 "Zeit zwischen Wechsel"

7 "Minuten"

8 "Bildschirmsperrung"
$ Message 9 has been removed   

10 "Ein"

11 "Aus"

12 "Bildschirmschoner:"

13 "Anzeigensperre:"

14 "Sperre starten"

15 "Automatische Bildschirmverdunklung"

16 "Bildschirmverdunklung:"

17 "Verdunklung starten"

18 "Sperre des Bedienfelds"
$ Message 19 has been replaced by message 21  

19 "Hintergrund der Bildschirmsperre verwenden"

20 "Zeit pro Hintergrund"
$ Message 21 is a replacement for message 19  

21 "Hintergründe für Sperre verwenden"

22 "Transparente Sperre"

$set 13        Redefine set#

$ _DtMessage set 13 is for the Keyboard dialog  
$ _DtMessage 1,5 from set 13 have been deleted from the message catalog  
$ _DtMessage 2 through 4 are labels of scales, buttons, and text visible in the  
$ Keyboard dialog box.  

2 "Standard"

3 "Zeichenwiederholung"

4 "Tastaturgeräusch"
$ _DtMessage 6 is the title of the Keyboard dialog box  

6 "Umgebungsverwaltung - Tastatur"

$set 14        Redefine set#

$ _DtMessage set 14 is for the Color dialog  
$ _DtMessage 1-4,9,14-15,17,19,20,22,24 from set 14 have been deleted from the message catalog  
$ Messages 5 through 8 are labels and buttons visible in the Color dialog  

5 "Paletten"

6 "Hinzufügen"

7 "Löschen"

8 "Ändern..."
$ _DtMessage 10 is the title above the text area in the Add Palette dialog box  

10 "Name der neuen Palette:"
$ _DtMessage 11 is the title of the Add Palette dialog box.  

11 "Palette hinzufügen"
$ Messages 12 through 14 are error messages.  

12 "Der Palettenname darf folgende Zeichen\n\
nicht enthalten:\n\n\
<space> * : ( ) [ ] { } < > ! | \" / \\"
$ LOCALIZERS: FOR 8 AND 16 BIT LANGUAGES YOU MAY WANT TO CHANGE THE MESSAGE
$ TO SAY THE PALETTE NAME MUST BE 10 BYTES OR LESS  

13 "Der Palettenname darf\n\
höchstens 10 Zeichen lang sein.\n"
$ _DtMessage 16 is the title of the Delete Palette dialog box.  See screen 4a (2).  

16 "Palette löschen"
$ _DtMessage 18 appears in an information dialog.  

18 "Die letzte Palette kann nicht gelöscht werden.\n"
$ _DtMessage 21 is an error message  

21 "Warnung"
$ _DtMessage 23 appears in an information dialog  

23 "Palette '%s' löschen?\n"
$ _DtMessage 25 is an error message that appears when you press the color  
$ button and the color server is not running for some reason.  

25 "Die Farbfunktionen der Umgebungsverwaltung\n\
sind inaktiv, weil der Farb-Server nicht aktiviert \n\
ist. $HOME/.dt/errorlog überprüfen."
$ _DtMessage 26 can be caused by setting the resource *useColorObj to False  
$ and then running dtstyle and trying to post the Color Dialog.  

26 "Die Farbfunktionen der Umgebungsverwaltung\n\
sind inaktiv, da die Ressource 'useColorObj'\n\
auf \"False\" gesetzt ist."
$ _DtMessage 27 is for the new ColorUse dialog when you press ok.  

27 "Die neuen Farbwerte sind ab der nächsten\n\
Sitzung wirksam."
$ _DtMessage 28 is a warning message that appears on Black and White systems  

28 "Die gewählte Palette ist ab der\n\
nächsten Sitzung wirksam."
$ _DtMessage 29 appears in a warning dialog  

29 "Eine Palette mit dem Namen '%s' existiert bereits.\n\
Durch die neue Palette wird die alte überschreiben.\n\
Soll dieser Schritt durchgeführt werden?"
$ _DtMessage 30 is the title of the Color dialog box.  

30 "Umgebungsverwaltung - Farbe"
$ _DtMessages 31-33 are replaced by Dtmessages 41-43  
$ _DtMessage 31-33 are the ColorUse options  
$ 31 High Color  
$ 32 Medium Color  
$ 33 Low Color  

34 "Schwarz/weiß"

35 "Standard"
$ Dt Message 36 has been replaced by message 39  
$ _DtMessage 36 is the title of the Color Use dialog  
$ 36 DT Color Use  
$ Dt Message 37 has been replaced by message 40  
$ _DtMessage 37 is the Color Use button in the main Color Dialog   
$ 37 DT Color Use...  
$ _DtMessage 38 is an error message that appears when you press the color  
$ button and there are no palette files to be found  

38 "Die Farbfunktionen der Umgebungsverwaltung sind\n\
inaktiv, weil keine Paletten-Dateien verfügbar\n\
sind. $HOME/.dt/errorlog überprüfen."
$ DtMessage 39 is a replacement for DtMessage 36  
$ DtMessage 39 is the title of the Color Use dialog  

39 "Anzahl der zu verwendenen Farben:"
$ DtMessage 40 is a replacement for DtMessage 37  
$ DtMessage 40 is the title of the Color Use dialog  

40 "Anzahl der Farben..."
$ _DtMessages 41-43 are replacements for Dtmessages 31-33  
$ _DtMessage 41-43 are the ColorUse options  

41 "Maximal"

42 "Mittel"

43 "Minimal"

$set 15        Redefine set#

$ All of the messages from set 15 have been deleted from the message catalog  

$set 16        Redefine set#

$ _DtMessage set 16 is for the Color dialog  
$ _DtMessage 1-10,13-17 from set 16 have been deleted from the message catalog  
$ _DtMessage 11-12 form one error message that appears in an error dialog  

11 "'%s' kann nicht gelöscht werden.\n"

12 "Weitere Hinweise in $HOME/.dt/errorlog."
$  MESSAGE 18-21 IN SET 16 WILL ONLY APPEAR IN THE DT ERRORLOG FILE  

18 "Warning, Too many directories listed in the resource paletteDirectories,\n\
Maximum number is %d. "
$ _DtMessage 19 - could not open a palette directory  

19 "Could not open directory %s. "
$ _DtMessage 20 - error opening the palette file  

20 "Could not open %s. "
$ _DtMessage 21 - error reading a palette file  

21 "%s is an invalid palette file."

$set 17        Redefine set#

$ _DtMessage set 17 is for the Modify Color dialog  
$ Messages 1-3,13,16 from set 17 have been deleted from the message catalog  
$ Messages 4 through 12 are labels, buttons, and text visible in the  
$ Modify Color dialog box  

4 "Farbprobe"

5 "Alt"

6 "Neu"

7 "Farbe wählen"

8 "Farb-Editor"

9 "Farbton"
$ We recommend that you don't translate R,G,B which stands for Red, Green,  
$ Blue unless there are standard single letter acronyms in your language.  

10 "R"

11 "G"

12 "B"
$  MESSAGE 14-15 IN SET 17 WILL ONLY APPEAR IN THE DT ERRORLOG FILE  

14 "Warning, couldn't grab pointer.\n"

15 "Warning, couldn't grab keyboard.\n"
$ _DtMessage 16 is the title of the Modify Color dialog box.  

16 "Umgebungsverwaltung - Farbe ändern"

$set 18        Redefine set#

$ _DtMessage set 18 is for the Window dialog  
$ _DtMessage 1 is the title of the Window dialog box  

1 "Umgebungsverwaltung - Fenster"
$ Messages 2 through 5 are labels on the Window dialog.  

2 "Standard"

3 "Fensterfunktionen"

4 "Symbole für Fenster"
$ Messages 5-8 have been replaced by messages 13-16   
$ 5 Focus Follows Mouse  
$ 6 Click In Window For Focus  
$ 7 Raise Window With Focus  
$ 8 Opaque Move  

9 "Symbolfenster verwenden"

10 "In Arbeitsbereich stellen"

11 "Primärfenster oben zulassen"

12 "Die Arbeitsbereichsverwaltung muß neu gestartet\n\
werden, damit die Änderungen wirksam werden.\n\
OK: Arbeitsbereichsverwaltung neu starten.\n\
Abbrechen: Änderungen zurücksetzen."
$ Messages 13-16 are replacements for messages 5-8  

13 "Zum Aktivieren auf Fenster zeigen"

14 "Zum Aktivieren im Fenster klicken"

15 "Aktiviertes Fenster in Vordergrund stellen"

16 "Inhalt beim Verschieben anzeigen"

$set 19
$ The set 19 contains messages for the I18N dialog

$ 
$ Messsages 1 to 11 are labels displayed in the dialog.
$ 

$quote

$ Message 1 is the title of the Dialog.
1 Umgebungsparameter - Internationalisierung

$ Message 2 appears on the button used to reset the defailt values.
2 Standard

$ Message 3 appears as the title of the title box containing Input
$ Methods information.
3 Eingabemethode

$ Message 4 appears as the title of the title box containing Input
$ Method Start Mode choice.
4 Startmodus der Eingabemethode

$ Message 5 appears as the title of the title box containing PreeditType
$ information.
5 Vorbearbeitungstyp

$ Message 6 is the label presenting the server host names
6 Server-Rechner :

$ Message 7 is the label presenting the Input Method list
7 Eingabemethode

$ Message 8 and 9 are the toggle buttons for the start mode selection:
$ "Ask at login" will cause dtimsstart to present the list of Input
$ Methods at next login; "Resume Current Input Method" will cause
$ dtimsstart to use the selected IM at next login, without asking the
$ user.
8 Bei Anmeldung fragen

9 Aktuelle Eingabemethode wiederaufnehmen

$ Message 10 is the label displayed on the button used to move up a
$ selected element in the preedit Type list (meaning at the previous
$ position).
10 Nach oben verschieben

$ Message 11 is the label displayed on the button used to move down a
$ selected element in the preedit Type list (meaning at the next
$ position).
11 Nach unten verschieben

$ 
$ Messages 20-21 are displayed in an warning dialog when the user
$ hits the OK button.
$ 

$ Message 20 appears when the user modifies the XmNpreeditType
$ resource value, and the resource *writeXrdbImmediate is True.
$ Do NOT localize the word XmNpreeditType.
20 Der neue Wert XmNpreeditType tritt in Kraft,\n\
wenn die Anwendungen neu gestartet werden.

$ Message 21 appears when the userselects a new Input Method Server,
$ or modifies the IMS Start Mode, or modifies the XmNpreeditType
$ resource value, and the resource *writeXrdbImmediate is False.
21 Ihre Auswahl tritt bei Ihrer\n\
nächsten Session in Kraft.

$ 
$ Messages 30 to 41 appear as errors in Error Dialog.
$ 

$ Do NOT localize the word HOME.
30 Die Umgebungsvariable HOME wurde nicht definiert.

$ Do NOT localize the word LANG.
31 Die Umgebungsvariable LANG wurde nicht definiert.

32 Das Desktop unterstützt diese Sprachumgebung nicht.

33 Kann die Benutzereingabemethode-Auswahldatei nicht erstellen.

34 Kann die Benutzereingabemethode-Auswahldatei nicht finden.

35 Kann die Benutzereingabemethode-Auswahldatei nicht speichern.

36 Unbekannter Rechner

$ Do NOT translate dtimsstart.
37 dtimsstart nicht gefunden.\n\
Möglicherweise ist die Desktop-Umgebung\n\
nicht richtig installiert.

38 Entfernte Ausführung auf dem\n ausgewählten Rechner fehlgeschlagen.

39 Kann entfernte Ausführung nicht initialisieren.

40 Kann keine Eingabemethode-Informationen vom ausgewählten Rechner holen.

41 Unbekannter Fehler



$set 99        Redefine set#

$    DO NOT TRANSLATE THESE MESSAGES  
$    DO NOT CHANGE THESE MESSAGES  
$    DO NOT LOCALIZE THESE MESSAGES  
$    These messages are used for the version information.  

$quote "
1 "@(#)version_goes_here"

2 "\n\
@(#)_DtMessage catalog source $XConsortium: dtstyle.msg /main/10 1996/11/19 18:08:39 rws $"
