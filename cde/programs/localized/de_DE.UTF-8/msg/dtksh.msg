$ $XConsortium: dtksh.msg /main/3 1995/12/08 09:16:19 rswiston $
$quote "

$set 1        Define initial set#


1 "DtkshCvtStringToPixel: Die Farbe '%s' ist nicht definiert"

$set 5        Redefine set#


1 "Zu viele Ressourcenparameter angegeben; '%s' wird übersprungen"

2 "Fehlerhafte Ressourcenspezifikation; zulässiges Format 'Name:Wert' : %s"

3 "Das Trennzeichen für Ressourcen ist ':', es wurde jedoch '=' verwendet : %s"

4 "Das Toolkit wurde bereits initialisiert"

5 "Verwendung: XtInitialize Variable Anwendungsname Anwendungsklasse [Argumente...]"

6 "Verwendung: %s Variable Name Klasse Parent [Argument:val ...]"

7 "Verwendung: XtCreateApplicationShell Variable Name Klasse [Argument:val ...]"

8 "Verwendung: XtPopup Widget GrabNone|GrabNonexclusive|GrabExclusive"

9 "Grab-Parameter '%s' unbekannt; 'GrabNone' wird verwendet"

10 "Verwendung: XtIsShell Widget"

11 "Verwendung: XtMapWidget Widget"

12 "Verwendung: XtUnmapWidget Widget"

13 "Verwendung: %s Variable Widget"

14 "Verwendung: XtScreen Variable Widget"

15 "Verwendung: XtWindow Variable Widget"

16 "Verwendung: XtCallCallbacks Widget callbackName"

17 "Verwendung: XtHasCallbacks Variable Widget callbackName"

18 "Verwendung: XtAddCallback Widget callbackName ksh-Befehl"

19 "Verwendung: XtRemoveCallback Widget callbackName ksh-Befehl"

20 "Der folgende 'Callback Handle' ist ungültig: %s"

21 "Der angegebene callback ist nicht registriert"

22 "Verwendung: XtAddEventHandler Widget Maske nonMaskable ksh-Befehl"

23 "Verwendung: XtRemoveEventHandler Widget Maske nonMaskable ksh-Befehl"

24 "Der angegebene Ereignisverarbeitungsprozeß (event handler) ist nicht registriert"

25 "Verwendung: XtGetValues Widget Ressource:Variable ..."

26 "Der folgende Ressourcenparameter ist nicht korrekt gebildet: %s"

27 "Verwendung: XtSetValues Widget Argument:val ..."

28 "Verwendung: XtAddWorkProc Variable Befehl"

29 "Verwendung: XtRemoveWorkProc workProcId"

30 "Der Parameter 'workProcId' muß eine Hexadezimalzahl sein: %s"

31 "Verwendung: XtAddTimeOut Variable Millisekunden Befehl"

32 "Der Parameter 'Millisekunden' muß größer als Null sein"

33 "Verwendung: XtRemoveTimeOut Intervall-ID"

34 "Der Parameter 'Intervall-ID' muß eine Hexadezimalzahl sein: %s"

35 "Verwendung: %s Widget ..."

36 "Verwendung: XBell Anzeige Lautstärke"

37 "Verwendung: %s Variable Anzeige"

38 "Der Parameter 'Anzeige' ist ungültig: %s"

39 "Verwendung: XDefineCursor Anzeige Fenster Cursor-Kennung"

40 "Der Parameter 'Cursor-Kennung' ist ungültig: %s"

41 "Verwendung: XUndefineCursor Anzeige Fenster"

42 "Verwendung: XtRemoveAllCallbacks Widget callbackName"

43 "Verwendung: XTextWidth Variable Schriftartname Zeichenfolge"

44 "Verwendung: %s Anzeige drawable [Argumente ...]"

45 "Der Parameter 'drawable' ist ungültig: %s"

46 "Der Parameter 'Ziel' ist ungültig: %s"

47 "Nicht erkannter Grafikfunktionsname: %s"

48 "Liniendarstellung unbekannt: %s"

49 "Nicht erkannte Zeichenoption: %s"

50 "Übrig gebliebene Punkte vorhanden, die ignoriert wurden"

51 "Verwendung: XCopyArea Anzeige Quelle Ziel QuelleX QuelleY Breite Höhe ZielX ZielY [Argumente ...]"

52 "Nicht möglich, den Ressourcentyp '%s' zu 'Zeichenfolge' umzusetzen"

53 "Nicht möglich, den Ressourcentyp 'Zeichenfolge' zu Typ '%s' umzusetzen"

54 "Verwendung: XtAddInput Variable [-r] Dateideskriptor ksh-Befehl"

56 "Verwendung: XtRemoveInput EingabeID"

57 "Der Parameter EingabeID muß eine Hexadezimalzahl sein: %s"

59 "Verwendung: XFlush Anzeige"

60 "Verwendung: XSync Anzeige löschen"

61 "Verwendung: XRaiseWindow Anzeige Fenster"

62 "Verwendung: %s Widget [True|False]"

63 "Verwendung: %s Widget Übersetzungen"

64 "Verwendung: XtUninstallTranslations Widget"

65 "Verwendung: XtParent Variable Widget"

66 "Verwendung: XtLastTimestampProcessed Variable Anzeige"

67 "Verwendung: DtSessionRestorePath Widget Pfadvariable Sicherungsdatei"

68 "Verwendung: DtSessionSavePath Widget Pfadvariable Dateivariable"

69 "Verwendung: DtShellIsIconified Widget"

70 "Verwendung: DtSetStartupCommand Widget Befehl"

71 "Verwendung: DtSetIconifyHint Widget boolean"

73 "Der Parameter 'rootWindow' ist ungültig: %s"

74 "Verwendung: DtWsmSetCurrentWorkspace Widget Atom"

75 "Das Arbeitsbereichsatom ist ungültig: %s"

78 "Verwendung: DtWsmSetWorkspacesOccupied Anzeige Fenster Arbeitsbereichsliste"

79 "Verwendung: DtGetHourGlassCursor Variable Anzeige"

80 "Verwendung: DtWsmAddCurrentWorkspaceCallback Variable Widget ksh-Befehl"

81 "Verwendung: DtWsmRemoveWorkspaceCallback handle"

82 "Verwendung: XtNameToWidget Variable Referenz-Widget Namen"

83 "Verwendung: DtDbLoad"

84 "Verwendung: DtDbReloadNotify ksh-Befehl"

85 "Verwendung: DtActionExists Aktionsname"

86 "Verwendung: DtActionLabel Variable Aktionsname"

87 "Verwendung: DtActionDescription Variable Aktionsname"

88 "Verwendung: DtActionInvoke Widget Aktionsname TermParameter ausführenderHost Kontextverz Indicator_verwenden ksh-Befehl [\"FILE\" Dateiname] ..."

89 "Verwendung: DtDtsLoadDataTypes"

90 "Verwendung: DtDtsFileToDataType Variable Dateiname"

91 "Verwendung: DtDtsFileToAttributeValue Variable Dateiname AttrName"

92 "Verwendung: DtDtsFileToAttributeList Variable Dateiname"

93 "Verwendung: DtDtsDataTypeToAttributeValue Variable Datentyp AttrName OptName"

94 "Verwendung: DtDtsDataTypeToAttributeList Variable Datentyp OptName"

95 "Verwendung: DtDtsFindAttribute Variable Name Wert"

96 "Verwendung: DtDtsDataTypeNames Variable"

97 "Verwendung: DtDtsSetDataType Variable Dateiname Datentyp override"

98 "Verwendung: DtDtsDataTypeIsAction Datentyp"

99 "Verwendung: ttdt_open Variable Status Variable2 Werkzeugname Hersteller Version Sendeanfang"

100 "Verwendung: tttk_Xt_input_handler ProzID Ausgangsobjekt ID"

101 "Der Parameter Ausgangsobjekt muß eine ganze Zahl sein: %s"

102 "Der Parameter ID muß eine Hexadezimalzahl sein: %s"

103 "Verwendung: ttdt_close Status ProzID neueProzID Sendestopp"

104 "Verwendung: ttdt_session_join Variable Status Sess-ID shellWidgetHandle join"

105 "Verwendung: ttdt_session_quit Status Sess-ID SessMuster quit"

106 "Der Parameter Sess-Muster ist ungültig: %s"

107 "Verwendung: ttdt_file_event Status Op Muster send"

108 "Verwendung: ttdt_file_quit Status Muster quit"

109 "Verwendung: ttdt_Get_Modified Pfadname Bereich Zeit_bis_Sperre"

110 "Verwendung: %s Status Pfadname Bereich Zeit_bis_Sperre"

112 "Verwendung: tt_error_pointer Variable ttStatus"

113 "Verwendung: %s Status Nachricht"

114 "Verwendung: %s Status Nachricht NachrStatus NachrStatusZeichenfolge destroy"

115 "Verwendung: ttdt_file_join Variable Status Pfadname Bereich join ksh-Befehl"

116 "Verwendung: tt_file_netfile Variable Status Dateiname"

117 "Verwendung: tt_netfile_file Variable Status Netzdateiname"

118 "Verwendung: tt_host_file_netfile Variable Status Host Dateiname"

119 "Verwendung: tt_host_netfile_file Variable Status Host Netzdateiname"

120 "Verwendung: XtIsSubclass Widget Klasse"

121 "%s ist kein gültiger Widget-Klassenname"

122 "Verwendung: XtClass Variable Widget"

$set 6        Redefine set#


1 "DtkshCvtWindowToString: Die Größe des Wertes 'von' ist ungültig"

2 "DtkshCvtHexIntToString: Die Größe des Wertes 'von' ist ungültig"

3 "DtkshCvtIntToString: Die Größe des Wertes 'von' ist ungültig"

4 "DtkshCvtBooleanToString: Die Größe des Wertes 'von' ist ungültig"

5 "DtkshCvtStringToWidget: Die Größe des Wertes 'von' ist ungültig"

6 "DtkshCvtStringToWidget: Nicht möglich, ein Widget mit dem Namen '%s' zu finden"

7 "DtkshCvtStringToCallback: Die Größe des Wertes 'von' ist ungültig"

8 "DtkshCvtCallbackToString: Die Größe des Wertes 'von' ist ungültig"

9 "DtkshCvtWidgetToString: Die Größe des Wertes 'von' ist ungültig"

10 "DtkshCvtWidgetToString: Nicht möglich, einen Namen für das Widget zu finden"

11 "DtkshCvtWidgetClassToString: Die Größe des Wertes 'von' ist ungültig"

12 "DtkshCvtWidgetClassToString: Unbekannte Widget-Klasse"

13 "DtkshCvtStringToWidgetClass: Unbekannter Widget-Klassenname"

14 "DtkshCvtScreenToString: Die Größe des Wertes 'von' ist ungültig"

$set 11        Redefine set#


1 "Es kann kein Feld mit dem Namen '%s' in der Struktur '%s' gefunden werden"

2 "DtkshCvtCallbackToString: Ein interner Umsetzungspuffer ist übergelaufen"

3 "Hashing-Fehler für Ressource '%s' in der Widget-Klasse '%s'"

4 "Kein Funktionsname zur Verfügung gestellt"

5 "Der Befehl kann nicht ausgeführt werden; das Toolkit wurde nicht initialisiert"

6 "Die Erstellung von Widget '%s' ist fehlgeschlagen"

7 "Der Parameter Anzeige ist ungültig: %s"

8 "Folgender Wert ist ungültig: %s"

9 "Die folgende Ressource ist nicht für Widget '%s' definiert: %s"

10 "Die angegebene Position ist ungültig: %s"

11 "Die angegebene Schriftart ist ungültig: %s"

12 "Das Widget Handle '%s' bezieht sich nicht auf ein bestehendes Widget"

13 "Das Widget muß ein Widget vom Typ 'command' sein"

14 "Das Widget muß ein Widget vom Typ 'mainWindow' sein"

15 "Das Widget muß ein Widget vom Typ 'scale' sein"

16 "Das Widget muß ein Widget vom Typ 'scrollBar' sein"

17 "Das Widget muß ein Widget oder Gadget vom Typ 'toggleButton' sein"

18 "Der Parameter Fenster ist ungültig: %s"

19 "Nicht möglich, den erforderlichen Speicher zuzuordnen; Ende"

20 "Nicht möglich, das übergeordnete Widget zu finden"

21 "Nicht möglich, das Symbol '%s' zu finden"

22 "Nicht möglich, den Typ '%s' zu finden"

23 "Nicht möglich, die Deklaration '%s' zu analysieren; 'unsigned long' wird verwendet"

24 "Unbekannter untergeordneter Typ: %s"

25 "Nicht erkannte Option: %s"

26 "Verwendung: %s Widget Position"

27 "Verwendung: %s Widget Zeit"

28 "Verwendung: %s Widget"

29 "dtksh-Fehler"

30 "dtksh-Warnung"

31 "Verwendung: %s Anzeige Fenster"

32 "Verwendung: %s Anzeige Fenster Variable"

33 "Verwendung: %s Anzeige rootWindow Variable"

34 "Der Parameter 'Zeit bis Sperre' ist ungültig: %s"

35 "Der Parameter 'Nachrichten' ist ungültig: %s"

36 "Der Parameter 'Muster' ist ungültig: %s"

$set 14        Redefine set#


1 "Fehler in interner Hash-Tabelle bei Initialisierung der Widget-Klasse; Ende"

2 "Widget-Klasse mit dem Namen '%s' konnte nicht gefunden werden"

3 "Die Kennung '%s' ist kein gültiges Widget Handle"

4 "Nicht möglich, die Widget-Klasse zu finden"

5 "Verwendung: DtLoadWidget widgetClassName widgetClassRecordName"

6 "Nicht möglich, einen Widget Class Record mit dem Namen '%s' zu finden"

7 "Fehler in interner Hash-Tabelle während der Initialisierung der Widget-Klasse '%s'"

8 "\n\
%sRESSOURCEN FÜR %s%s%s:\n"

9 "VORGABE"

10 "R"

11 "M"

12 "S"

13 "%-15s %-6s %-6s %-18s %-6s %s\n"

14 "UMG.VARIABLE    HANDLE PARENT-KLASSE             STATUSNAME\n"

15 "\n\
Verwendung:\tDtWidgetInfo [widgetHandle]\n\
\tDtWidgetInfo -r <widgetHandle|Klassenname>\n\
\tDtWidgetInfo -R <widgetHandle|Klassenname>\n\
\tDtWidgetInfo -c [Klassenname]\n\
\tDtWidgetInfo -h [widgetHandle]"

$set 15        Redefine set#


1 "Nicht möglich, das Toolkit zu initialisieren"

2 "Die folgende Ressource kann nicht zur Erstellungszeit des Widgets \n\
eingestellt werden. Statt dessen nach der Erstellung XtSetValues verwenden: %s"

3 "Verwendung: %s Variable Parent Name [Argument:Wert ...]"

4 "Verwendung: DtHelpReturnSelectedWidgetId Variable Widget Variable"

5 "Verwendung: DtHelpSetCatalogName Katalogname"

6 "Verwendung: DtHelpQuickDialogGetChild Variable quickHelpWidget Sohn"

7 "Das Widget muß ein Widget vom Typ 'quickHelp' sein"

8 "Das Widget muß ein Widget vom Typ 'list' sein"

9 "Verwendung: %s Widget Position Element"

10 "Verwendung: %s Widget Position Elementliste"

11 "Verwendung: %s Widget Element"

12 "Verwendung: XmListDeleteItemsPos Widget Anzahl Position"

13 "Verwendung: XmListDeleteItems Widget Elementliste"

14 "Verwendung: XmListDeletePositions Widget Positionsliste"

15 "Verwendung: XmListGetKbdItemPos Variable Widget"

16 "Verwendung: XmListItemExists Widget Element"

17 "Verwendung: XmListItemPos Variable Widget Element"

18 "Verwendung: XmListPosSelected Widget Position"

19 "Verwendung: XmListPosToBounds Widget Position Variable Variable Variable Variable"

20 "Verwendung: %s Widget Position notify_Option"

21 "Verwendung: %s Widget Element notify_Option"

22 "Verwendung: XmListSetAddMode Widget boolean"

23 "Verwendung: XmListSetKbdItemPos Widget Position"

24 "Verwendung: XmMainWindowSetAreas Hauptfenster Menü Befehl hscroll vscroll work"

25 "Wert für 'Hauptfenster (mainWindow)' ist NULL"

26 "Verwendung: %s Variable Hauptfenster"

27 "Verwendung: XmProcessTraversal Widget Richtung"

28 "Wert für Traversal Direction unbekannt: %s"

29 "Usage: XmInternAtom variable display name onlyIfExists"

30 "Verwendung: XmGetAtomName Variable Anzeige Atom"

31 "Das angegebene Atom ist ungültig: %s"

32 "Verwendung: XmGetColors Widget Hintergrund Vordergrund obereSchattierung untereSchattierung select"

33 "Das Hintergrundpixel ist ungültig: %s"

34 "Verwendung: XmUpdateDisplay Widget"

35 "Verwendung: %s Widget Protokoll [Protokoll ...]"

36 "Das angegebene Atom ist ungültig: %s"

37 "Verwendung: XmAddWMProtocolCallback Widget Protokoll ksh-Befehl"

38 "Verwendung: XmRemoveWMProtocolCallback Widget Protokoll ksh-Befehl"

39 "Verwendung: XmMenuPosition Menü Ereignis"

40 "Das angegebene Ereignis ist ungültig: %s"

41 "Verwendung: XmCommandAppendValue commandWidget Zeichenfolge"

42 "Verwendung: XmCommandError commandWidget Fehlermeldung"

43 "Verwendung: XmCommandSetValue commandWidget Befehl"

44 "Verwendung: XmCommandGetChild Variable commandWidget Sohn"

45 "Verwendung: XmMessageBoxGetChild Variable commandWidget Sohn"

46 "Das Widget muß ein Widget vom Typ 'messageBox' sein"

47 "Verwendung: XmFileSelectionBoxGetChild Variable Widget Sohn"

48 "Das Widget muß ein Widget vom Typ 'file selection box' sein"

49 "Verwendung: XmSelectionBoxGetChild Variable Widget Sohn"

50 "Das Widget muß ein Widget vom Typ 'selection box' sein"

51 "Verwendung: XmScaleGetValue scaleWidget Variable"

52 "Verwendung: XmScaleSetValue scaleWidget Wert"

53 "Der angegebene Skalierungswert ist ungültig: %s"

54 "Verwendung: XmScrollBarGetValues Bildlaufleiste Variable Variable Variable Variable"

55 "Verwendung: XmScrollBarSetValues Bildlaufleiste Wert Reglergröße Inkrement Seiteninkrement notify"

56 "Der angegebene Wert ist ungültig: %s"

57 "Die angegebene Reglergröße ist ungültig: %s"

58 "Das angegebene Inkrement ist ungültig: %s"

59 "Das angegebene Seiteninkrement ist ungültig: %s"

60 "Verwendung: XmScrollVisible scrolledWin Widget linker_rechter_Rand oberer_unterer_Rand"

61 "Das Widget muß ein Widget vom Typ 'scrolledWindow' sein"

62 "Das Widget, das angezeigt werden soll, existiert nicht. "

63 "Der angegebene linke/rechte Rand ist ungültig: %s"

64 "Der angegebene obere/untere Rand ist ungültig: %s"

65 "Verwendung: %s Widget Status notify"

66 "Verwendung: catopen Variable Katalogname"

67 "Verwendung: catclose Katalog-ID"

68 "Verwendung: catgets Variable Katalog-ID SetNum MsgNum dftMsg"

69 "Das Widget muß ein Widget vom Typ 'text' oder 'textField' sein"

70 "Verwendung: %s Variable Widget"

71 "Verwendung: %s Widget boolean"

72 "Der angegebene Parameter ist ungültig: %s"

73 "Verwendung: %s Widget Zeilen"

74 "Verwendung: %s Widget maxLänge"

75 "Verwendung: %s Widget Zeichenfolge"

76 "Verwendung: %s Widget Variable Variable"

77 "Verwendung: %s Widget Position Zeichenfolge"

78 "Verwendung: %s Widget Position Variable Variable"

79 "Verwendung: %s Widget Position_von Position_an Zeichenfolge"

80 "Die angegebene Position 'von' ist ungültig: %s"

81 "Die angegebene Position 'an' ist ungültig: %s"

82 "Verwendung: %s Widget erstePosition letztePosition Zeit"

83 "Die angegebene erste Position ist ungültig: %s"

84 "Die angegebene letzte Position ist ungültig: %s"

85 "Die angegebene Zeit ist ungültig: %s"

86 "Verwendung: %s Variable Widget x y"

87 "Die angegebene Position x ist ungültig: %s"

88 "Die angegebene Position y ist ungültig: %s"

89 "Verwendung: %s Widget links rechts Modus"

90 "Die angegebene linke Position ist ungültig: %s"

91 "Die angegebene rechte Position ist ungültig: %s"

92 "Verwendung: %s Widget Start Zeichenfolge Richtung Variable"

93 "Die angegebene Startposition ist ungültig: %s"

94 "Verwendung: XmListGetSelectedPos Variable Widget"

95 "Verwendung: XmListGetMatchPos Variable Widget Element"

96 "Verwendung: XmOptionLabelGadget Variable Widget"

97 "Vewendung: XmOptionButtonGadget Variable Widget"

98 "Verwendung: XmGetVisibility Variable Widget"

99 "Verwendung: XmGetTearOffControl Variable Widget"

100 "Verwendung: XmGetTabGroup Variable Widget"

101 "Verwendung: XmGetPostedFromWidget Variable Widget"

102 "Verwendung: XmGetFocusWidget Variable Widget"

103 "Verwendung: XmFileSelectionDoSearch Widget Verzeichnismaske"

$set 16        Redefine set#


1 "DtkshCvtNamedValueToString: Keine Übereinstimmung gefunden"

2 "DtkshCvtStringToNamedValue: Nicht möglich, die Zeichenfolge '%s' umzusetzen"
$ The following messages are for ksh93 itself.  The message numbers must
$ match those specified in the MsgStr array allmsgs in
$ ksh93/src/cmd/ksh93/sh/init.c, as must the set number.

$set 25        Redefine set#


1 "OK"

3 "läuft"

4 "[-n] [Argument...]"

5 "[Argument...]"

6 "[Verz] [Liste]"

7 "[Job...]"

8 "[n]"

9 "[Name [Pfadname] ]"

10 "[Name]"

11 "[oben] [Basis]"

12 "Ausdruck..."

13 "format [Argument...]"

14 "ist eine Funktion"

15 "ist ein Schlüsselwort"

16 "ist eine integrierte Funktion der Shell"

17 "ist eine exportierte Funktion"

18 "ist eine undefinierte Funktion"

19 "name [Argument...]"

20 "optstring name [Argumente...]"

21 "Sekunden"

23 "${HOME:-.}/.profile"

24 "%c: ungültiges Zeichen in Ausdruck - %s"

25 "%c: unbekannte Formatspezifikation"

26 "%d-%d: ungültiger Bereich"

27 "%d: ungültige Version der Binärprozedur"

28 "%s ist ein Alias für"

29 "%s ist ein exportierter Alias für"

30 "%s fehlt"

31 "%s unbekannte Basis"

32 "%s: ':' erwartet für Operator '?' "

33 "%s: Mehrdeutig"

34 "%s: Argumente müssen %job oder Prozeß-IDs sein"

35 "%s: Alias nicht gefunden\n"

36 "%s: arithmetischer Syntaxfehler"

37 "%s: Zuordnung erfordert lvalue"

38 "%s: fehlerhafte Dateieinheitennummer"

39 "%s: fehlerhaftes Format"

40 "%s: fehlerhafte Nummer"

41 "%s: fehlerhafte Option(en)"

42 "%s: fehlerhafte Substitution"

43 "%s: fehlerhafte Hardwareprogrammverzweigung"

44 "%s: kann nicht erstellt werden"

45 "%s: kann nicht ausgeführt werden"

46 "%s: kann nicht geöffnet werden"

47 "%s: Division durch Null"

48 "%s: Ausnahmebedingung bei Domäne"

49 "%s: %s fehlt"

50 "%s: Datei existiert bereits"

51 "%s: unzulässiger Funktionsname"

52 "%s: ungültiger Aliasname"

53 "%s: ungültige Prioritätsfunktion"

54 "%s: ungültiger Exportname"

55 "%s: ungültiger Funktionsname"

56 "%s: ungültiger Name"

57 "%s: ungültiger regulärer Ausdruck"

58 "%s: ungültige Referenz auf sich selbst"

59 "%s: ungültige Verwendung von :"

60 "%s: ungültiger Variablenname"

61 "%s: ist keine Kennung"

62 "%s: nur im Lesezugriff"

63 "%s: Bezeichnung nicht implementiert"

64 "%s: Grenzwert überschritten"

65 "%s: weitere Token erwartet"

66 "%s: kein Parent"

67 "%s: kein Referenzname"

68 "%s: nicht gefunden"

69 "%s: nicht implementiert"

70 "%s: Operanden besitzen nicht kompatible Typen"

71 "%s: Überlaufausnahme"

72 "%s: Parameter nicht gesetzt"

73 "%s: Parameter Null oder nicht gesetzt"

74 "%s: Rekursion zu tief"

75 "%s: Referenzvariable kann kein Bereich sein"

76 "%s: Pfadnamenargument erforderlich"

77 "%s: eingeschränkt"

78 "%s: Ausnahmebedingung bei Singularität"

79 "%s: Subscript außerhalb des Bereichs"

80 "%s: Anzahl der Klammern unausgewogen"

81 "%s: unbekannte Funktion"

82 "%s: unbekannte länderspezifische Angaben"

83 "%s: unbekannter Operator"

84 "%s: unbekannter Signalname"

85 "%s: würde Schleife verursachen"

86 "(coredump)"

87 "-c erfordert Argument"

88 "-e - erfordert einzelnes Argument"

89 "/vpix"

90 "<Befehl unbekannt>"

91 "ACE#?F#?H:[Name]L#?R#?Z#?fi#?[Basis]lnprtux [Name=[Wert]...]"

92 "AE#?F#?HL#?R#?Z#?fi#?[Basis]lnprtux [Name=[Wert]...]"

93 "Abbrechen"

94 "Ad:[Begrenzer]prst#[Zeit_bis_Sperre]u#[Dateinummer] [Name...]"

95 "Alarmruf"

96 "Fehlerhafte Root-Knotenspezifikation"

97 "Fehlerhafter Systemaufruf"

98 "Beschädigte Pipe"

99 "Busfehler"

100 "Jobsteuerung kann nicht gestartet werden"

101 "Aktuelle Optionseinstellungen"

102 "DIL-Signal"

103 "Sohnprozeß abgebrochen"

104 "DircabefhkmnpstuvxCI:[Datei]o:?[Option]A:[Name] [Argument...]"

105 "DircabefhkmnpstuvxCo:?[Option]A:[Name] [Argument...]"

106 "EMT-Trap"

107 "Grenzwert für CPU-Zeit überschritten"

108 "Grenzwert für Dateigröße überschritten"

109 "Floating-Ausnahme"

110 "HSacdfmnstv [Grenzwert]"

111 "nichtprogrammierter Stopp"

112 "IO-Signal"

113 "Unzulässige Anweisung"

114 "Unterbrechung"

115 "Abgebrochen"

116 "LP [Verzeichnis] [Änderung]"

117 "Speicherfehler"

118 "Migrationsprozeß"

119 "Keine Jobsteuerung"

120 "Rufunterbrechung"

121 "Sendeaufrufalarm"

122 "Stromausfall"

123 "Zeitalarm wird definiert"

124 "Verlassen"

125 "Ressourcen verloren"

126 "Alter TTY-Treiber wird zurückgeholt..."

127 "S [Maske]"

129 "Sicherheitsbezeichnung geändert"

130 "Socket-Unterbrechung"

131 "Sound beendet"

132 "Gestoppt (Signal)"

133 "Gestoppt (TTY-Eingabe)"

134 "Gestoppter Prozeß wird fortgesetzt"

135 "Gestoppt"

136 "Gestoppt (TTY-Ausgabe)"

137 "Wechsel zu neuem TTY-Treiber..."

138 "Systemabsturz steht bevor"

139 "Beendet"

140 "Trace/BPT-Trap"

141 "Nicht erkannte Version"

142 "Mit 'Ende' diese Shell beenden"

143 "Benutzersignal 1"

144 "Benutzersignal 2"

145 "Version nicht definiert"

146 "Virtueller Zeitalarm"

147 "Änderung der Fenstergröße"

148 "Laufende Jobs vorhanden"

149 "Gestoppte Jobs vorhanden"

150 "[_[:alpha:]]*([_[:alnum:]])"

151 "\n\
@(#)Version 12/28/93\0\n"

152 "\n\
@(#)Version M-12/28/93\0\n"

153 "\n\
real"

154 "\r\n\
\007Shell überschreitet in 60 Sekunden die Zeitsperre wegen Inaktivität"

155 "a Name..."

156 "a:c [Befehl [Argumente...] ]"

157 "afpv Name..."

158 "alarm %s %.3f\n"

159 "alarm -r %s +%.3g\n"

160 "Argument erwartet"

161 "fehlerhaftes Verzeichnis"

162 "fehlerhafte Dateieinheitennummer"

163 "fehlerhafte Substitution"

164 "Zugriff auf übergeordnete Verzeichnisse nicht möglich"

165 "Pipe kann nicht erstellt werden"

166 "temporäre Datei kann nicht erstellt werden"

167 "fork nicht möglich"

168 "Abruf von %s nicht möglich"

169 "%s kann nicht eingestellt werden"

170 "Alarm kann nicht eingestellt werden"

171 "Bedingung(en) erforderlich"

172 "dsf:[Bibliothek] [Name...]"

173 "e:[Editor]lnrsN# [erster] [letzter]"

174 "Dateiende"

175 "f:[Format]nprsu:[Dateinummer] [Argument...]"

176 "fnv Name..."

177 "hist -e \"${VISUAL:-${EDITOR:-vi}}\" "

178 "Protokolldatei kann nicht geöffnet werden"

179 "falsche Syntax"

180 "ungültiges Argument von Typ %c"

181 "ist eine in die Shell integrierte Version von "

182 "ist ein Alias mit absoluter Pfadangabe für"

183 "abbrechen"

184 "Zeile %d: Vor $ steht kein \\"

185 "Zeile %d: %c innerhalb von ${} muß in Anführungszeichen stehen"

186 "Zeile %d: %s unbekannte Bezeichnung"

187 "Zeile %d: %s innerhalb von [[...]] veraltet; ((...)) verwenden"

188 "Zeile %d: '=' veraltet; '==' verwenden"

189 "Zeile %d: -a veraltet; -e verwenden"

190 "Zeile %d: \\ vor %c ist für die zukünftige Verwendung reserviert"

191 "Zeile %d: `...` veraltet; $(...) verwenden"

192 "Zeile %d: %c mit einem Escape-Zeichen versehen, um Mehrdeutigkeiten zu verhindern"

193 "Zeile %d: Bezeichnung %s wird ignoriert"

194 "Zeile %d: %c in Anführungszeichen setzen, um Mehrdeutigkeiten zu verhindern"

195 "Zeile %d: set %s veraltet"

196 "Zeile %d: Leerzeichen für verschachtelte Subshell erforderlich"

197 "Zeile %d: geschweifte Klammern verwenden, um Mehrdeutigkeiten mit $id[...] zu verhindern"

198 "Zeile %d: Leerzeichen oder Tabulatorzeichen verwenden, um die Operatoren %c und %c zu trennen"

199 "ln#[signum]s:[signame] sig..."

200 "setuid/setgid-Anmelde-Shells nicht zulässig"

201 "Zuordnung"

202 "neue Zeile"

203 "nlp [Job...]"

204 "keine Protokolldatei"

205 "kein Abfrageprozeß"

206 "kein derartiger Job"

207 "kein derartiger Prozeß"

208 "nicht unterstützt"

209 "aus"

210 "ein"

211 "Grenzwert für geöffnete Datei überschritten"

212 "kein Speicherplatz mehr"

213 "p [Aktion Bedingung...]"

214 "p [Name[=Wert]...]"

215 "Parameter nicht gesetzt"

216 "Zugriff verweigert"

217 "Prozeß existiert bereits"

218 "ptx [Name=[Wert]...]"

219 "pvV Name [Argument]..."

220 "r [Variablenname Sekunden]"

221 "Syntaxfehler in Zeile %d: `%s' %s"

222 "Syntaxfehler in Zeile %d: doppelte Bezeichnung %s"

223 "Syntaxfehler: `%s' %s"

224 "sys"

225 "Zeit für Sperre überschritten wegen fehlender Eingabe"

226 "unerwartet"

227 "universe not accessible"

228 "unbegrenzt"

229 "unpaarig"

230 "Benutzer"

231 "Versionen"

232 "Schreiben auf %d fehlgeschlagen"

233 "Post in $_"

234 "Nullbyte"
