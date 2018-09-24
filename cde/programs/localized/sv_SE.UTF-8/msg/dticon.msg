$ $XConsortium: dticon.msg /main/3 1995/11/08 13:34:04 rswiston $
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

$set 2
$ *****************************************************************************
$ ** _DtMessage set for fileIODialog.c
$ **
$ ** messages 10-16 are for the FileSelectionBox widget (default labels)
$ *****************************************************************************

2 Ikonredigerare - In- och utmatning av fil
4 Ikonredigerare - In- och utmatning av fil
6 Ikonredigerare - Spara som
8 Ikonredigerare - Öppna fil
10 Uppdatera
12 Mappar
14 Filer
16 Ange filnamn:
17 Ange filnamn:
18 Uppdatera
20 NAMNLÖS
22 Öppna
24 Spara

$set 4
$ *****************************************************************************
$ ** _DtMessage set for newIconDialog.c
$ **
$ *****************************************************************************

2 newIconDialog
4 Ikonredigerare - Ändra storlek
6 OK
8 Avbryt
10 Hjälp
12 Ny bredd:
14 Ny höjd:

$set 6
$ *****************************************************************************
$ ** _DtMessage set for queryDialog.c
$ **
$ *****************************************************************************

2 Ikonredigerare - varning
6 OK
$ ** DO NOT LOCALIZE - the user should never see this message
8 Do ya really wanna?
10 Avbryt

$set 8
$ *****************************************************************************
$ ** _DtMessage set for stdErrDialog.c
$ **
$ *****************************************************************************

2 Ikonredigerare - Fel
4 Fortsätt

$set 10
$ *****************************************************************************
$ ** _DtMessage set for utils.c
$ **
$ *****************************************************************************

$ ** DO NOT LOCALIZE messages 2-48, they appear in the errorlog only
2 Cannot initialize button icon for circle
4 Cannot initialize button icon for solid circle
6 Cannot initialize button icon for polyline
8 Cannot initialize button icon for polygon
10 Cannot initialize button icon for solid polygon
12 Cannot initialize button icon for ellipse
14 Cannot initialize button icon for solid ellipse
16 Cannot initialize button icon for eraser
18 Cannot initialize button icon for line
20 Cannot initialize button icon for flood
22 Cannot initialize button icon for point
24 Cannot initialize button icon for rectangle
26 Cannot initialize button icon for solid rectangle
28 Cannot initialize button icon for select
30 Unable to parse static color no.
32 Unable to allocate static color no.
38 No memory(0)
40 No memory(1)
42 No memory(2)
44 No memory(3)
46 No memory(4)
48 No memory(5)

50 Det går inte att initiera lagringsstället för programikonerna 
52 Ogiltig användning av tillägget '-f'
54 Ogiltig användning av tillägget '-i'
56 Ogiltig användning av tillägget '-session'
58 Ogiltigt storleksformat
60 Det gick inte att kopiera skärmbilden
62 Operationen misslyckades.\nFärgpaletten är full.
64 Ogiltigt dimensionsargument

66 (Namnlös)

$set 12
$ *****************************************************************************
$ ** _DtMessage set for dtIconShell.c
$ **
$ *****************************************************************************

1 Ikonredigerare
2 Två färger
3 Ikonredigerare - (Namnlös)
4 Välkommen till ikonredigeraren.
6 Ifyllda
8 Statiska färger
10 Dynamiska färger
12 Statisk gråskala
14 Förgrund
16 Bakgrund
18 Övre skugga
20 Undre skugga
22 Markeringsfärg
24 Genomskinlig

$ ** message 26-198 are menu items with corresponding mnemonics
$ ** (for set grouping information, see the comment at the end-of-file)
26 N
28 Nytt...
30 p
32 Öppna...
34 S
36 Spara
38 m
40 Spara som...
42 x
44 Avsluta
46 F
48 Fil
50 n
52 Ångra
54 t
56 Klipp ut område 
58 p
60 Kopiera område
62 i
64 Klistra in område
68 V
70 Vänster
72 H
74 Höger
76 R
78 Rotera område
82 L
84 Lodrätt
86 V
88 Vågrätt
90 d
92 Vänd område
94 S
96 Skala område
98 s
100 Ikonstorlek...
102 t
104 Lägg till aktiveringspunkt
106 b
108 Ta bort aktiveringspunkt
110 k
112 Kopiera skärmbild
114 T
116 Ta bort ikon
118 R
120 Redigera
122 r
124 Synligt rutnät
128 B
130 XBM
134 P
136 XPM
138 O
140 Utfilsformat
144 2x
146 3x
148 4x
150 5x
152 6x
154 8x
156 10x
158 12x
160 F
162 Förstoring
164 l
166 Alternativ
168 v
170 Översikt...
172 t
174 Steg-för-steg-anvisningar...
176 R
178 Referens...
180 O
182 Objekthjälp
188 h
190 Om hjälpfunktionen...
192 k
194 Om Ikonredigerare...
196 H
198 Hjälp

$ ** message 200-are used in the "instruction" line for current drawing mode
200 Aktuellt redigeringsläge:
202 (Fylld)
204 Penna
206 Fyll
208 Linje
210 Öppen polygon
212 Rektangel
214 Polygon
216 Cirkel
218 Ellips
220 Sudda
222 Markera
224 Ogiltig

$set 14
$ *****************************************************************************
$ ** _DtMessage set for help.c
$ **
$ *****************************************************************************

2 Ikonredigerare - Hjälp
4 Du måste markera ett objekt\ni Ikonredigeraren.

$set 16
$ *****************************************************************************
$ ** _DtMessage set for process.c
$ **
$ *****************************************************************************

2 Det går inte att få tillgång till filen\neller så innehåller filen ogiltiga data
4 Det går inte att skriva data till filen
6 Programfunktionen finns inte tillgänglig än
8 Det finns ingen föregående\nbild tillgänglig
10 Ogiltig bredd och/eller\nhöjd
12 Du har inte markerat något område
14 Du har inte klippt ut\neller kopierat något område
16 Den aktuella ikonen har inte sparats.\n\nÄndringarna kommer att gå förlorade.
20 Kontrollera filbehörigheten
22 Det gick inte att återställa sessionen från filen
24 Det gick inte att spara sessionen till filen
25 Filen finns redan.\n\nErsätt den befintliga filen?

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
$ **   48 Fil
$ **
$ **   118 E
$ **   120 Redigera
$ **
$ **   164 O
$ **   166 Alternativ
$ **
$ **   196 H
$ **   198 Hjälp
$ **
$ **
$ ** Unique Set 2:  (File Menu)
$ **
$ **   26 N
$ **   28 Nytt...
$ **   30 O
$ **   32 Öppna...
$ **   34 S
$ **   36 Spara
$ **   38 A
$ **   40 Spara som...
$ **   42 x
$ **   44 Avsluta
$ **
$ **
$ ** Unique Set 3:  (Edit Menu)
$ **
$ **   50 U
$ **   52 Ångra
$ **   54 t
$ **   56 Klipp ut område
$ **   58 C
$ **   60 Kopiera område
$ **   62 P
$ **   64 Klistra in område
$ **
$ **        Unique Set 3a:  (Rotate Sub-Menu)
$ **        68 L
$ **        70 Vänster
$ **        72 R
$ **        74 Höger
$ **   76 R
$ **   78 Rotera område
$ **
$ **        Unique Set 3b:  (Flip Sub-Menu)
$ **        82 V
$ **        84 Lodrätt
$ **        86 H
$ **        88 Vågrätt
$ **   90 F
$ **   92 Vänd område
$ **   94 S
$ **   96 Skala område
$ **   98 z
$ **   100 Ikonstorlek...
$ **   102 A
$ **   104 Lägg till aktiveringspunkt
$ **   106 D
$ **   108 Ta bort aktiveringspunkt
$ **   110 G
$ **   112 Kopiera skärmbild
$ **   114 e
$ **   116 Ta bort ikon
$ **
$ **
$ ** Unique Set 4:  (Options Menu)
$ **
$ **   122 V
$ **   124 Synligt rutnät
$ **
$ **        Unique Set 4a:  (Output Sub-Menu)
$ **        128 B
$ **        130 XBM
$ **        134 P
$ **        136 XPM
$ **   138 O
$ **   140 Utfilsformat
$ **   160 M
$ **   162 Förstoring
$ **
$ **
$ ** Unique Set 5:  (Help Menu)
$ **
$ **   168 I
$ **   170 Introduktion...
$ **   172 T
$ **   174 Steg-för-steg-anvisningar...
$ **   176 R
$ **   178 Referens...
$ **   180 O
$ **   182 Objekthjälp
$ **   188 U
$ **   190 Om hjälpfunktionen...
$ **   192 V
$ **   194 Version...
$ **
