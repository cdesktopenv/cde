$ $XConsortium: dtterm.msg /main/3 1995/11/08 13:36:18 rswiston $
$ *************************************<+>*************************************
$ *****************************************************************************
$ **
$ **  File:        dtterm.msg
$ **
$ **  Project:     Common Desktop Environment dtterm
$ **
$ **  Description:
$ **  -----------
$ **  This file is the source for the message catalog for dtterm
$ **  Any additional messages are to be added to this file by hand,
$ **  and the associated index number placed in the code.
$ **
$ *****************************************************************************
$ **
$ **     (c) Copyright 1993, 1994 Hewlett-Packard Company
$ **     All Rights reserved
$ **
$ **
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
$ 2. Messages that only appear in the DT error log file ($HOME?.dt/errorlog).
$ 
$     The localization of these messages is OPTIONAL.  These messages are
$     identified by the following:
$ 
$     MESSAGES xx-yy WILL ONLY APPEAR IN THE DT ERRORLOG FILE
$ 
$ 3. Messages that should not be localized.
$ 
$     These messages are identified by the following:
$ 
$     DO NOT TRANSLATE or CHANGE or LOCALIZE MESSAGES xx-yy
$ 
$ ***** END (NOTE FOR MESSAGE CATALOG TRANSLATORS) *****
$ 
$ ********** PROLOGUE ****************
$  Date    Name               Note
$ ======== =================  ===============================================
$ 03/15/94 B. May             Initial rev
$ 
$ *****************************************************************************

$set 1
$ ****** Module: DtTermMain.c 
$ Message 1 is the window menu addition - do not translate f.separator or
$ f.send_msg.  It will be used as the format string for a sprintf() command,
$ so it is necessary to double up on any percent signs ('%') that you
$ want to keep in the string.
$ Message 2 is an error message

1 \
	Avs             f.separator\n\
	Visa/dölj\\ meny\\ rad   f.send_msg %d\
	
2 %s: Tillägget -S kan inte användas tillsammans med tilläggen -server och -serverid\n

$set 2
$ ****** Module: DtTermSyntax.c 
$ These are the usage messages. 
$ DO NOT TRANSLATE Messages 37 or 41 ("End-of-list")

1 -/+132			aktivera/avaktivera Esc-sekvens för växling mellan 80<->132 kolumner
2 -/+aw				aktivera/avaktivera automatiskt radbyte
3 -bg färg			bakgrundsfärg
4 -/+bs				aktivera/avaktivera terminalbakgrunden som markerad färg
5 -display visningsenhet		X-hanterare som ska kontaktas
6 -e kommandoargument		kommando som ska exekveras
7 -fb teckenuppsättning			fetstilt font
8 -fg färg			förgrundsfärg
9 -fn fontuppsättning			normal font
10 -geometry geom			fönstrets storlek (antal tecken) och position
11 -help				skriv ut detta meddelande
12 -/+iconic			starta/starta inte ikonsökväg
13 -/+j				aktivera/avaktivera flerradsbläddring
14 -/+kshMode			aktivera/avaktivera ksh-läge
15 -/+l				aktivera/avaktivera loggning
16 -lf filnamn			logga filnamn
17 -/+ls				aktivera/avaktivera inloggningsskal
18 -/+map				aktivera/avaktivera mappningsfönster för  pty-utdata
19 -/+mb				aktivera/avaktivera signal vid marginal
20 -ms färg			pekarfärg
21 -n sträng			ange ikonnamn
22 -name sträng			strängar för klientrepresentation, ikon och titel
23 -nb avstånd			ange avstånd för signal vid högermarginal
24 -/+rw				aktivera/avaktivera upphävt radbyte
25 -/+sb				aktivera/avaktivera bläddringslist
26 -/+sf				aktivera/avaktivera SUN-funktionstangenter
27 -sl tal[s]			antal bläddrade rader [skärmar] som ska sparas
28 -ti namn			identifikationssträng i programmet
29 -title sträng			titelsträng för fönster
30 -tm sträng			nyckelord och tecken i terminalläge
31 -tn namn			variabelnamn för TERM-miljö
32 -/+vb				aktivera/avaktivera synlig signal 
33 -xrm resurssträng		extra resursspecifikationer
34 -C				console-läge
35 -Sxxd				slavläge på \"ttyxx\"-filbeskrivning \"d\"
36 -Sxxx.d			slavläge på \"ttyxxx\"-filbeskrivning \"d\"
37 End-Of-List
38 Om tillägget -e används måste det placeras i slutet av kommandoraden.
39 I annat fall startas användarens standardskal. Tillägg som inleds 
40 med ett plustecken (+) återställer standardvalet.
41 End-Of-List
47 %s:  felaktigt tillägg på kommandoraden \"%s\"\r\n\n
48 syntax:  %s
49 \r\n   
50  [%s]
51 \r\n\nSkriv \"%s -help\" för att få en fullständig beskrivning.\r\n\n
52 syntax:\n
53 \t%s [-options ...] [-e kommandoargument]\n\n
54 där alternativen omfattar:\n

$set 3
$ ****** Module: TermFunction.c
$ This is a warning message dialog box
1 MINNET ÄR FULLT\nTryck på OK för att ta bort

$set 4
$ ****** Module: TermPrim.c
$ This is the title of a dialog box
1 Terminal - Varning

$set 5
$ ****** Module: TermFunction.c
$ This is a warning message dialog box
1 MINNET ÄR FULLT\nTryck på OK för att ta bort

$set 6
$ ****** Module: TermView.c
$ DO NOT TRANSLATE Message 1
$ Messages 2 and 3 are titles for dialog boxes.
1 80x24 132x24
2 Terminal - Kommandobeskrivning
3 Terminal - Hjälp

$set 7
$ ****** Module: TermViewGlobalDialog.c
$ These are the user-visible labels in the Global Options dialog box.
$ #1 is the title. 
$ The rest are resource types and values - look at the dialog box and 
$ documentation for full specs. 
$ 
$ DO NOT TRANSLATE MESSAGE 15
$ 
1 Terminal - Globala alternativ
2 OK
3 Avbryt
4 Hjälp
5 global
6 Markörinställning
7 Ruta
8 Understrykning
9 Markörtyp
10 Aktiverad
11 Avaktiverad
12 Blinkande markör
13 Blinkningshastighet (millisekunder)
14 Osynlig
15 blinkRate
16 Färg
17 Inverterad
18 Normal
19 Fönsterbakgrund
20 Bläddringsmetod
21 Avaktiverad
22 Aktiverad
23 Steglös bläddring
24 Signalinställning
25 Hörbar
26 Synlig
27 Signaltyp 
28 Aktiverad
29 Avaktiverad
30 Marginalvarning
31 Marginalavstånd 
32 Tillämpa
33 Återställ
$set 8
$ ****** Module: TermViewMenu.c
$ These are the menu bar entries and menu pane choices.
$ NOTE TO TRANSLATORS: The Single letter entries are the menu mnemonic
$ for the entry above them (i.e. message 11 "F" is the mnemonic for
$ message 10 "Font Size"). 
$ The items with a "+" in them are the accellerators for an item.
$ The single-character items are the mnemonics for a menu item, where the
$ text of the menu item is specified in the previous message
$ Do not translate messages 24 and 28.
1 Standard
2 Fönsterstorlek
3 ö
4 -iso8859-1
5 %d punkter
6 %.1f punkter
7 %.1f punkter
8 %.2f punkter
9 Standard
10 Fontstorlek
11 F

$   Window Menu
12 Nytt
13 N
14 Skriv ut
15 u
16 Skriv ut...
17 r
18 Stäng
19 S 
20 Fönster
21 ö

$   Edit Menu
22 Kopiera
23 K
24 Ctrl osfInsert
25 Ctrl+Insert
26 Klistra in
27 i
28 Skift osfInsert
29 Skift+Insert
30 Redigera
31 R

$  Option menu

32 Menyrad
33 M
34 Bläddringslist
35 l
36 Globalt...
37 G
38 Terminal...
39 T
40 Mjuk återställning
41 k
42 Hård återställning
43 d
44 Återställ
45 l
46 Alternativ
47 v

$ Old Help menu (no longer used for CDE)

48 Översikt
49 v
50 Steg-för-steg-anvisningar
51 t
52 Referens
53 R
54 Objekthjälp
55 O
56 Använda Hjälp
57 n
58 Om Terminal
59 m
60 Hjälp
61 H
62 Alt+F4
63 Terminal

$ Current CDE Help menu

64 Översikt
65 v
66 Index
67 I
68 Innehållsförteckning
69 h
70 Steg-för-steg-anvisningar
71 t
72 Referens
73 R
74 Tangentbord
75 T
76 Om hjälpfunktionen
77 h
78 Om Terminal
79 d
80 Hjälp
81 H

$set 9
$ ****** Module: TermTerminalDialog.c
$ These are the messages for the terminal options dialog
1 Terminal - Alternativ
2 OK
3 Avbryt
4 Hjälp
5 Tangentbordsinställning
6 Normal
7 Program
8 Markörtangentläge
9 Numeriskt
10 Program
11 Läge för numeriskt tangentbord
12 Endast vagnretur
13 Vagnretur och radmatning
14 Radmatningssekvens
15 Låst
16 Olåst
17 Användardefinierade funktionstangenter
18 Skärminställning
19 Aktiverad
20 Avaktiverad
21 132 kolumner
22 Radbyte
23 Upphäv radbyte
24 Tillämpa
25 Återställ
