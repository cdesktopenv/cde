$ $TOG: dticon.msg /main/7 1999/09/17 12:17:55 mgreess $
$ *************************************<+>************************************* 
$ ***************************************************************************** 
$ ** 
$ **  File:        dticon.msg 
$ ** 
$ **  Project:     HP DT Icon Editor 
$ ** 
$ **  Description: 
$ **  ----------- 
$ **  This file is the source for the message catalog for dticon 
$ ** 
$ ** 
$ ***************************************************************************** 
$ ** 
$ **     (c) Copyright 1992, 1993, 1994 Hewlett-Packard Company 
$ **     (c) Copyright 1993, 1994 International Business Machines Corp. 
$ **     (c) Copyright 1993, 1994 Sun Microsystems, Inc. 
$ **     (c) Copyright 1993, 1994 Unix System Labs, Inc., a subsidiary of 
$ **         Novell, Inc. 
$ ** 
$ **     All Rights reserved 
$ ** 
$ ** 
$ ***************************************************************************** 
$ ** 
$quote "

$set 2
$ ***************************************************************************** 
$ ** _DtMessage set for fileIODialog.c 
$ ** 
$ ** messages 10-16 are for the FileSelectionBox widget (default labels) 
$ ***************************************************************************** 

2 "Symbol-Editor - Datei-E/A"

4 "Symbol-Editor - Datei-E/A"

6 "Symbol-Editor - Speichern als"

8 "Symbol-Editor - Datei öffnen"

10 "Aktualisieren"

12 "Ordner"

14 "Dateien"

16 "Dateinamen eingeben:"

17 "Dateinamen eingeben:"

18 "Aktualisieren"

20 "OHNE_TITEL"

22 "Öffnen"

24 "Sichern"

$set 4        Redefine set#

$ ***************************************************************************** 
$ ** _DtMessage set for newIconDialog.c 
$ ** 
$ ***************************************************************************** 

2 "newIconDialog"

4 "Symbol-Editor - Größe ändern"

6 "OK"

8 "Abbruch"

10 "Hilfe"

12 "Neue Breite:"

14 "Neue Höhe:"

$set 6        Redefine set#

$ ***************************************************************************** 
$ ** _DtMessage set for queryDialog.c 
$ ** 
$ ***************************************************************************** 

2 "Symbol-Editor - Warnung"

6 "OK"
$ ** DO NOT LOCALIZE - the user should never see this message 

8 "Do ya really wanna?"

10 "Abbruch"

$set 8        Redefine set#

$ ***************************************************************************** 
$ ** _DtMessage set for stdErrDialog.c 
$ ** 
$ ***************************************************************************** 

2 "Symbol-Editor - Fehler"

4 "Fortsetzung"

$ 
$ "-NEW"
6 OK

$ 
$ "-NEW"
8 "Could not connect to ToolTalk:\n%s\nExiting ..."

$set 10        Redefine set#

$ ***************************************************************************** 
$ ** _DtMessage set for utils.c 
$ ** 
$ ***************************************************************************** 
$ ** DO NOT LOCALIZE messages 2-48, they appear in the errorlog only 

2 "Cannot initialize button icon for circle"

4 "Cannot initialize button icon for solid circle"

6 "Cannot initialize button icon for polyline"

8 "Cannot initialize button icon for polygon"

10 "Cannot initialize button icon for solid polygon"

12 "Cannot initialize button icon for ellipse"

14 "Cannot initialize button icon for solid ellipse"

16 "Cannot initialize button icon for eraser"

18 "Cannot initialize button icon for line"

20 "Cannot initialize button icon for flood"

22 "Cannot initialize button icon for point"

24 "Cannot initialize button icon for rectangle"

26 "Cannot initialize button icon for solid rectangle"

28 "Cannot initialize button icon for select"

30 "Unable to parse static color no."

32 "Unable to allocate static color no."

38 "No memory(0)"

40 "No memory(1)"

42 "No memory(2)"

44 "No memory(3)"

46 "No memory(4)"

48 "No memory(5)"

50 "Speichern des Symbols für die Anwendung nicht initialisierbar "

52 "Ungültige Verwendung des Parameters '-f'"

54 "Ungültige Verwendung der Option '-i'"

56 "Ungültige Verwendung des Parameters '-session'"

58 "Ungültige Größenangabe"

60 "Bilderfassung ist fehlgeschlagen"

62 "Vorgang fehlgeschlagen.\n\
Colormap ist voll"

64 "Ungültiger Maßparameter"

66 "(UNBENANNT)"

$set 12        Redefine set#

$ ***************************************************************************** 
$ ** _DtMessage set for dtIconShell.c 
$ ** 
$ ***************************************************************************** 

1 "Symbol-Editor"

2 "2-farbig"

3 "Symbol-Editor - (UNBENANNT)"

4 "Willkommen im Symbol-Editor."

6 "Flächen ausfüllen"

8 "Statische Farben"

10 "Dynamische Farben"

12 "Statische Graustufen"

14 "Vordergrund"

16 "Hintergrund"

18 "Schatten oben"

20 "Schatten unten"

22 "Wählen"

24 "Transparent"
$ ** message 26-198 are menu items with corresponding mnemonics 
$ ** (for set grouping information, see the comment at the end-of-file) 

26 "E"

28 "Erstellen..."

30 "f"

32 "Öffnen..."

34 "p"

36 "Speichern"

38 "a"

40 "Speichern als..."

42 "B"

44 "Beenden "

46 "D"

48 "Datei"

50 "R"

52 "Rückgängig"

54 "a"

56 "Bereich ausschneiden"

58 "k"

60 "Bereich kopieren"

62 "e"

64 "Bereich einfügen"

68 "L"

70 "Links"

72 "R"

74 "Rechts"

76 "d"

78 "Bereich drehen"

82 "V"

84 "Vertikal"

86 "H"

88 "Horizontal"

90 "w"

92 "Bereich wenden"

94 "s"

96 "Bereich skalieren"

98 "g"

100 "Symbolgröße ändern..."

102 "h"

104 "Brennpunkt hinzufügen"

106 "l"

108 "Brennpunkt löschen"

110 "B"

112 "Bild erfassen"

114 "y"

116 "Symbol leeren "

118 "B"

120 "Bearbeiten "

122 "G"

124 "Sichtbares Gitter"

128 "B"

130 "XBM"

134 "P"

136 "XPM"

138 "A"

140 "Ausgabeformat"

144 "2x"

146 "3x"

148 "4x"

150 "5x"

152 "6x"

154 "8x"

156 "10x"

158 "12x"

160 "V"

162 "Vergrößerung"

164 "O"

166 "Optionen"

168 "b"

170 "Überblick..."

172 "A"

174 "Aufgaben..."

176 "R"

178 "Referenz..."

180 "Z"

182 "Zu Element"

188 "f"

190 "Hilfe für Hilfe..."

192 "E"

194 "Symboleditor: Info..."

196 "H"

198 "Hilfe"
$ ** message 200-are used in the "instruction" line for current drawing mode 

200 "Aktueller Änderungsmodus:"

202 "(ausgefüllt)"

204 "Bleistift"

206 "Einfärben"

208 "Linie"

210 "Linienzug"

212 "Rechteck"

214 "Vieleck"

216 "Kreis"

218 "Ellipse"

220 "Löschen"

222 "Auswählen"

224 "Ungültig"

$set 14        Redefine set#

$ ***************************************************************************** 
$ ** _DtMessage set for help.c 
$ ** 
$ ***************************************************************************** 

2 "Hilfe für Symbol-Editor"

4 "Es muß ein Element innerhalb des \n\
Symbol-Editors ausgewählt werden."

$set 16        Redefine set#

$ ***************************************************************************** 
$ ** _DtMessage set for process.c 
$ ** 
$ ***************************************************************************** 

2 "Auf die Datei kann nicht zugegriffen\n\
werden oder sie enthält ungültige Daten"

4 "Daten können nicht in Datei \n\
geschrieben werden"

6 "Diese Funktion ist noch nicht verfügbar"

8 "Kein vorheriges Bild verfügbar"

10 "Angegebene Breite und/oder Höhe ungültig"

12 "Kein Bereich ausgewählt"

14 "Kein Bereich zuvor ausgeschnitten \n\
oder kopiert"

16 "Das aktuelle Symbol wurde nicht gesichert.\n\n\
Die Änderungen gehen verloren."

20 "Dateiberechtigungen überprüfen"

22 "Sitzung konnte aus Datei nicht wiederhergestellt werden"

24 "Sitzung konnte nicht als Datei gespeichert werden"

25 "Datei bereits vorhanden.\n\n\
Soll Datei überschrieben werden?"

$set 18
$ *****************************************************************************
$ ** _DtMessage set for event.c
$ **
$ *****************************************************************************

$quote

2 Größe des übernommenen Bildes = %dx%d


$ ***************************************************************************** 
$ ** End of _DtMessage Catalog File 
$ ** (Comments only below this line) 
$ ***************************************************************************** 
$ ** 
$ ***************************************************************************** 
$ ** Unique mnemonic groupings from set 12 
$ ***************************************************************************** 
$ ** 
$ ** Unique Set 1:  (Main Menu Bar) 
$ ** 
$ **   46 F 
$ **   48 File 
$ ** 
$ **   118 E 
$ **   120 Edit 
$ ** 
$ **   164 O 
$ **   166 Options 
$ ** 
$ **   196 H 
$ **   198 Help 
$ ** 
$ ** 
$ ** Unique Set 2:  (File Menu) 
$ ** 
$ **   26 N 
$ **   28 New... 
$ **   30 O 
$ **   32 Open... 
$ **   34 S 
$ **   36 Save 
$ **   38 A 
$ **   40 Save As... 
$ **   42 x 
$ **   44 Exit 
$ ** 
$ ** 
$ ** Unique Set 3:  (Edit Menu) 
$ ** 
$ **   50 U 
$ **   52 Undo 
$ **   54 t 
$ **   56 Cut Area 
$ **   58 C 
$ **   60 Copy Area 
$ **   62 P 
$ **   64 Paste Area 
$ ** 
$ **        Unique Set 3a:  (Rotate Sub-Menu) 
$ **        68 L 
$ **        70 Left 
$ **        72 R 
$ **        74 Right 
$ **   76 R 
$ **   78 Rotate Area 
$ ** 
$ **        Unique Set 3b:  (Flip Sub-Menu) 
$ **        82 V 
$ **        84 Vertical 
$ **        86 H 
$ **        88 Horizontal 
$ **   90 F 
$ **   92 Flip Area 
$ **   94 S 
$ **   96 Scale Area 
$ **   98 z 
$ **   100 Resize Icon... 
$ **   102 A 
$ **   104 Add Hot Spot 
$ **   106 D 
$ **   108 Delete Hot Spot 
$ **   110 G 
$ **   112 Grab Screen Image 
$ **   114 e 
$ **   116 Clear Icon 
$ ** 
$ ** 
$ ** Unique Set 4:  (Options Menu) 
$ ** 
$ **   122 V 
$ **   124 Visible Grid 
$ ** 
$ **        Unique Set 4a:  (Output Sub-Menu) 
$ **        128 B 
$ **        130 XBM 
$ **        134 P 
$ **        136 XPM 
$ **   138 O 
$ **   140 Output Format 
$ **   160 M 
$ **   162 Magnification 
$ ** 
$ ** 
$ ** Unique Set 5:  (Help Menu) 
$ ** 
$ **   168 I 
$ **   170 Introduction... 
$ **   172 T 
$ **   174 Tasks... 
$ **   176 R 
$ **   178 Reference... 
$ **   180 O 
$ **   182 On Item 
$ **   188 U 
$ **   190 Using Help... 
$ **   192 V 
$ **   194 Version... 
$ ** 
