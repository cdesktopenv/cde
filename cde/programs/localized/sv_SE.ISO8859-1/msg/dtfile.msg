
$ "$TOG: dtfile.msg /main/6 1998/04/29 15:12:43 rafi $"

$ *************************************<+>*************************************
$ *****************************************************************************
$ **
$ **  File:        dtfile.msg
$ **
$ **  Project:     Common Desktop Environment dtfile
$ **
$ **  Description:
$ **  -----------
$ **  This file is the source for the message catalog for dtfile
$ **  Any additional messages are to be added to this file by hand,
$ **  and the associated index number placed in the code.
$ **
$ *****************************************************************************
$ **
$ **     (c) Copyright 1990, 1991, 1992 Hewlett-Packard Company
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
$ 03/25/93 H. Amro            removed the guilty $ from message 30
$ 
$ 
$ 
$ *****************************************************************************

$set 2
$ ****** Module: ChangeDir.c ****(developer comment only)********

$ Messages 3 and 10 are titles for the Change Folder dialog, Message 3 is
$   used when the user has specified a title for dtfile windows
$ Messages 2 and 4 are labels for the Change Folder dialog
$ Message 1 is the title for Change Folder errors
$ Messages 7, 8, 9, 11, and 12 are Change Folder error messages

1 Fel i Byt mapp
2 Ny mapp:
3 Byt mapp
4 Värddator:
7 Följande mappnamn är ogiltigt:\n\n%s:%s
8 Du kan inte växla till denna mapp. Det går\n\
endast att visa mapparna under den aktuella mappen.\n\
Du kan inte ange en absolut väg till den nya mappen.\n
9 Detta fönster visas i begränsat läge. Du kan inte\n\
gå till den angivna mappen eftersom den inte finns\n\
i den begränsade mappens relativa sökväg.\n
10 Filhanterare - Byt mapp
11 Det går inte att hämta det nya mappnamnet.
12 Skriv mappens namn.

13 Fel i Gå till
14 Målmapp:
15 Gå till
16 Systemnamn: 
17 Filhanterare - Gå till
18 Målmappens namn saknas.\n\
   Skriv ett mappnamn eller markera en mapp i listan.

19 "Följande mappnamn är ogiltigt.\n\n\
%s"

20 "Du kan inte växla till mappen\n\n\
%s\n\n\
Du har inte behörighet att läsa den här mappen\n\
eftersom det är en mapp med begränsad tillgång."


$set 3
$ ****** Module: ChangeDirP.c ****(developer comment only)********

$ Messages 2-6 are labels for the folder Status Line
$ Message 3 is required for SUN systems
$ Message 2-9 are obsolete

2 %d fil(er) varav %d dold(a)
3 %1$d fil(er) varav %2$d dold(a)
4 %3d fil(er) ...
5 Läser ...
6 %d dold(a)

7 %d objekt varav %d dolt(da)
8 %1$d objekt varav %2$d dolt(da)
9 %3d objekt ...



10 %d objekt
11 %d objekt...
12 %d objekt varav %d dolt(da)



$set 4
$set 5
$set 6
$set 7
$set 8



$set 9
$ ****** Module: File.c ****(developer comment only)********

$ Messages 6 and 7 are the title and text for an action error message
$ Messages 8 and 9 are rename error messages
$ Messages 11-19 are the title and text for warning messages displayed
$   when an object/set of objects is moved/copied/linked to a folder
$   containing object(s) with the same name

6 Fel i funktion
7 Det finns inga definierade funktioner för %s\nm

8 Denna fil finns på arbetsytan och det finns\n\
redan en fil med detta namn där.\n\
Välj ett annat namn.
9 Det finns redan en fil med detta namn.\n\
Välj ett annat namn.

11 Filhanterare - Varning vid flyttning/kopiering/länkning
$ Messages 12-15 deal with multiple files/folders
12 Följande filer (eller mappar)\n\
finns redan i\n\
%s:
13 \nKopieringen kommer att skriva över filerna.\n\
          Vill du verkligen fortsätta?\n
14 \nFlyttningen kommer att skriva över filerna.\n\
          Vill du verkligen fortsätta?\n
15 \nLänkningen kommer att skriva över filerna.\n\
          Vill du verkligen fortsätta?\n

$ Messages 16&17 deal with a single file/folder
16 Det finns redan ett objekt med namnet\n%s\ni mappen\n%s
17 Mappen\n%s\nfinns redan i mappen\n%s

$ Labels in rename/replace and merge/replace dialogs
20 Vad vill du göra?
21 Ersätta ett befintligt objekt
22 Byta namn på ett befintligt objekt till:
24 Slå ihop innehållet i de två mapparna
25 Ersätta en befintlig mapp

27 OK
28 Avbryt
29 Hjälp
30 Avbryt flyttningen
31 Avbryt kopieringen
32 Avbryt länkningen

$ Window title for overwrite dialogs
33 Filhanterare - varning vid flyttning/kopiering/länkning
34 Filhanterare - varning vid flyttning
35 Filhanterare - varning vid kopiering
36 Filhanterare - varning vid länkning

37 Fel vid namnbyte av objekt
38 %s\nfinns redan. Välj ett annat namn.
39 Du kan inte byta namn från %s\ntill\n%s\n%s
40 Fel vid ersättning av objekt

51 kopierar
52 flyttar
53 länkar
54 ändrar
56 Ersätt befintliga objekt
57 Byt namn på befintliga objekt genom att lägga till ett nummer
58 kopierats
59 flyttats
60 länkats
61 ändrats
68 1 objekt %s.
69 %d objekt %s.
70 %s\n%d objekt har samma namn som objekt i mappen\n%s
71 %s\n1 objekt har samma namn som ett objekt i mappen\n%s
72 Kopiera
73 Flytta
74 Länka
75 Operation
76 kopiera
77 flytta
78 länka
79 operation

80 %s har avbrutits. Mappen har inte %s.\n\nMappen som du vill ersätta kunde inte tas bort\neftersom ett objekt i mappen inte kunde tas bort.\nAndra objekt i mappen kan ha tagits bort\ninnan %s-processen avbröts.
81 %s\n\nDet beror antagligen på att du inte\nhar behörighet att ändra objektet.\nÄndra behörighet eller byt namn manuellt på mappen som du\n%s, så att den får ett annat namn.
82 %s har avbrutits. \n\n%s\nkan inte tas bort.\n\nDet beror antagligen på att du inte\nhar behörighet att ändra filen.\nÄndra behörighet eller byt namn manuellt på\nfilen som du %s, så att den får ett annat namn.
83 %s har avbrutits. Inga av objekten har %s.\n\n Följande objekt kunde inte få nya namn automatiskt\n%s.\n\n%d objekt fick nya namn innan %s-processen avbröts.
84 %s\n\nDet beror antagligen på att du inte har\nbehörighet att ändra ovanstående objekt. Ändra behörighet eller byt namn manuellt\npå objektet som du %s, så att det får ett annat namn.
85 %s har avbrutits. Inga av objekten har %s.\n\n Följande objekt kunde inte få ett nytt namn automatiskt\n%s.\n\n1 objekt fick ett nytt namn innan %s-processen avbröts.
86 %s har avbrutits. Inga av objekten har %s.\n\nEn mapp som du vill ersätta kunde inte tas bort.\n Mapp: %s\n\nVissa objekt i mappen kan ha tagits bort\n innan %s-processen avbröts.\n\n%d andra objekt har tagits bort.\n\nDet beror antagligen på att du inte har\nbehörighet att ändra ovanstående mapp. Ändra behörighet eller byt namn manuellt\n på mappen som du %s, så att den får ett annat namn.
87 %s har avbrutits. Inga av objekten har %s.\n\nEn mapp som du vill ersätta kunde inte tas bort.\n Mapp: %s\n\nVissa objekt i mappen kan ha tagits bort\n innan %s-processen avbröts.\n\n1 andra objekt har tagits bort.\n\nDet beror antagligen på att du inte har\nbehörighet att ändra ovanstående mapp. Ändra behörighet eller byt namn manuellt\npå mappen som du %s, så att den får ett annat namn.
88 %s har avbrutits. Inga av objekten har %s.\n\nEn fil som du vill ersätta kunde inte tas bort.\n Fil: %s\n\n%d andra objekt har tagits bort.\n\nDet beror antagligen på att du inte har\nbehörighet att ändra ovanstående fil. Ändra behörighet eller byt namn manuellt\npå filen som du %s, så att den får ett annat namn.
89 %s har avbrutits. Inga av objekten har %s.\n\nEn fil som du vill ersätta kunde inte tas bort.\n Fil: %s\n\n%d andra objekt har tagits bort.\n\nDet beror antagligen på att du inte har\nbehörighet att ändra ovanstående fil. Ändra behörighet eller byt namn manuellt\npå filen som du %s, så att den får ett annat namn.
90 Namnkonflikt.\n\
  Objektet finns på arbetsytans bakgrund.\n\
  Där finns redan ett annat objekt med\n\
  samma namn som du försöker ange.\n\
  Välj ett annat namn.

91 Stoppa kopieringen
92 Stoppa länkningen
93 Stoppa flyttningen
94 Fel

95 "Flytten avbröts - mappen flyttades inte.\n\n\
Den mapp du vill ersätta kan inte tas bort\n\
eftersom ett objekt i mappen inte kan tas bort.\n\
Vissa andra objekt i mappen kanske hann tas bort innan\n\
flyttprocessen avbröts.\n\n\
Den troligaste orsaken till problemet är att du inte har\n\
skrivbehörighet till det objekt som inte kunde tas bort.\n\
Ändra skrivbehörigheten eller döp manuellt om den mapp du\n\
ska flytta."

96 "Kopieringen avbröts - mappen kopierades inte.\n\n\
Den mapp du vill ersätta kunde inte tas bort eftersom\n\
ett objekt i mappen inte kunde tas bort. Vissa andra\n\
objekt i mappen kanske hann tas bort innan\n\
kopieringen avbröts.\n\n\
Den troligaste orsaken till problemet är att du inte har\n\
skrivbehörighet till det objekt som inte kunde tas bort.\n\
Ändra skrivbehörigheten eller döp manuellt om den mapp som\n\
ska kopieras."

97 "Länkningen avbröts - mappen länkades inte.\n\n\
Den mapp du vill ersätta kunde inte tas bort eftersom\n\
ett objekt i mappen inte kunde tas bort. Vissa andra\n\
objekt i mappen kanske hann tas bort innan \n\
länkningen avbröts.\n\n\
Den troligaste orsaken till problemet är att du inte har\n\
skrivbehörighet till det objekt som inte kunde tas bort.\n\
Ändra skrivbehörigheten eller döp manuellt om mappen som \n\
du vill länka."

98 "Åtgärden avbröts - mappen ändrades inte.\n\n\
Den mapp du vill ersätta kunde inte tas bort eftersom\n\
ett objekt i mappen inte kunde tas bort. Vissa andra\n\
objekt kanske redan hann tas bort innan processen avbröts.\n\n\
Den troligaste orsaken till problemet är att du inte har\n\
skrivbehörighet till det objekt som inte kunde tas bort.\n\
Ändra skrivbehörigheten eller döp manuellt om mappen som\n\
du vill ändra."

99 "Flytten avbröts\n\n\
%s\n\
kan inte tas bort.\n\n\
Den troligaste orsaken är att du inte har\n\
skrivbehörighet till filen. Ändra skrivbehörigheten\n\
eller döp manuellt om filen du flyttar."

100 "Kopieringen avbröts\n\n\
%s\n\
kan inte tas bort.\n\n\
Den troligaste orsaken är att du inte har\n\
skrivbehörighet för filen. Ändra skrivbehörigheten\n\
eller döp manuellt om filen du kopierar."

101 "Länkningen avbröts\n\n\
%s\n\
kan inte tas bort.\n\n\
Den troligaste orsaken är att du inte har\n\
skrivbehörighet för filen. Ändra skrivbehörigheten\n\
eller döp manuellt om filen du länkar. "

102 "Åtgärden avbröts\n\n\
%s\n\
kan inte tas bort.\n\n\
Den troligaste orsaken är att du inte har\n\
skrivbehörighet för filen. Ändra skrivbehörighet\n\
eller döp manuellt om filen du ska ändra."

103 "Flytten avbröts - inga objekt flyttades.\n\n\
Följande objekt kunde inte döpas om automatiskt:\n\
%s\n\n\
%d objekt döptes om innan flyttprocessen avbröts.\n\n\
Den troligaste orsaken är att du inte har skrivbehörighet till \n\
objektet. Ändra skrivbehörighet eller döp manuellt om objektet\n\
du ska flytta."

104 "Kopieringen avbröts - inga objekt kopierades.\n\n\
Följande objekt kunde inte döpas om automatiskt\n\
%s\n\n\
%d objekt döptes om innan kopieringen avbröts.\n\n\
Den troligaste orsaken är att du inte har skrivbehörighet till \n\
objektet. Ändra skrivbehörigheten eller döp manuellt om det\n\
objekt du ska kopiera."

105 "Länkningen avbröts - inga objekt länkades.\n\n\
Följande objekt kunde inte döpas om automatiskt\n\
%s\n\n\
%d objekt döptes om innan länkningen avbröts.\n\n\
Den troligaste orsaken är att du inte har skrivbehörighet till\n\
objektet. Ändra skrivbehörigheten eller döp manuellt om det\n\
objekt du ska länka."

106 "Åtgärden avbröts - inga objekt ändrades.\n\n\
Följande objekt kunde inte döpas om automatiskt:\n\
%s\n\n\
%d objekt döptes om innan processen avbröts.\n\n\
Den troligaste orsaken är att du inte har skrivbehörighet till\n\
objektet. Ändra skrivbehörigheten eller döp manuellt om det \n\
objekt du ska ändra."

107 "Flytten avbröts - inga objekt flyttades.\n\n\
Följande objekt kunde inte döpas om automatiskt:\n\
%s\n\n\
1 objekt döptes om innan flyttprocessen avbröts.\n\n\
Den troligaste orsaken är att du inte har skrivbehörighet till\n\
objektet. Ändra skrivbehörigheten eller döp manuellt om det \n\
objekt du ska flytta."

108 "Kopieringen avbröts - inga objekt kopierades.\n\n\
Följande objekt kunde inte döpas om automatiskt:\n\
%s\n\n\
1 objekt döptes om innan kopieringen avbröts.\n\n\
Den troligaste orsaken är att du inte har skrivbehörighet till\n\
objektet. Ändra skrivbehörigheten eller döp manuellt om det\n\
objekt du ska kopiera. "

109 "Länkningen avbröts - inga objekt länkades.\n\n\
Följande objekt kunde inte döpas om automatiskt:\n\
%s\n\n\
1 objekt döptes om innan länkningen avbröts.\n\n\
Den troligaste orsaken är att du inte har skrivbehörighet till\n\
objektet. Ändra skrivbehörigheten eller döp manuellt om det\n\
objekt du ska länka."

110 "Åtgärden avbröts - inga objekt ändrades.\n\n\
Följande objekt kunde inte döpas om automatiskt:\n\
%s\n\n\
1 objekt döptes om innan processen avbröts.\n\n\
Den troligaste orsaken är att du inte har skrivbehörighet till\n\
objektet. Ändra skrivbehörigheten eller döp manuellt om det\n\
objekt du ska ändra."

111 "Flytten avbröts - inga objekt flyttades.\n\n\
En mapp som du vill ersätta kunde inte tas bort.\n\
   Mapp: %s\n\n\
Vissa objekt i mappen kanske redan hann tas bort innan\n\
processen avbröts.\n\n\
%d andra objekt har tagits bort.\n\n\
Den troligaste orsaken är att du inte har skrivbehörighet till\n\
mappen. Ändra skrivbehörigheten eller döp manuellt om den mapp\n\
som ska flyttas."

112 "Kopieringen avbröts - inga objekt kopierades.\n\n\
En mapp som du vill ersätta kunde inte tas bort.\n\
   Mapp: %s\n\n\
Vissa objekt i mappen kanske redan hann tas bort innan \n\
kopieringen avbröts.\n\n\
%d andra objekt har tagits bort.\n\n\
Den troligaste orsaken är att du inte har skrivbehörighet till\n\
mappen. Ändra skrivbehörigheten eller döp manuellt om den mapp\n\
som ska kopieras."

113 "Länkningen avbröts - inga objekt länkades.\n\n\
En mapp som du vill ersätta kunde inte tas bort.\n\
   Mapp: %s\n\n\
Vissa objekt i mappen kanske redan hann tas bort innan \n\
länkningen avbröts.\n\n\
%d andra objekt har tagits bort.\n\n\
Den troligaste orsaken är att du inte har skrivbehörighet till\n\
mappen. Ändra skrivbehörigheten eller döp manuellt om den mapp\n\
som ska ändras."

114 "Åtgärden avbröts - inga objekt ändrades.\n\n\
En mapp som du vill ersätta kunde inte tas bort.\n\
   Mapp: %s\n\n\
Vissa objekt i mappen kanske redan hann tas bort innan\n\
processen avbröts.\n\n\
%d andra objekt har tagits bort.\n\n\
Den troligaste orsaken är att du inte har skrivbehörighet till mappen.\n\
Ändra skrivbehörigheten eller döp manuellt om den mapp som ska ändras."

115 "Flytten avbröts - inga objekt flyttades.\n\n\
En mapp som du vill ersätta kunde inte tas bort.\n\
   Mapp: %s\n\n\
Vissa objekt i mappen kanske redan hann tas bort innan\n\
flyttningsprocessen avbröts.\n\n\
1 annat objekt har tagits bort.\n\n\
Den troligaste orsaken är att du inte har skrivbehörighet till\n\
mappen. Ändra skrivbehörigheten eller döp manuellt om den mapp\n\
som ska flyttas."

116 "Kopieringen avbröts - inga objekt kopierades.\n\n\
En mapp som du vill ersätta kunde inte tas bort.\n\
   Mapp: %s\n\n\
Vissa objekt i mappen kanske redan hann tas bort innan\n\
kopieringen avbröts.\n\n\
1 annat objekt har tagits bort.\n\n\
Den troligaste orsaken är att du inte har skrivbehörighet till\n\
mappen. Ändra skrivbehörigheten eller döp manuellt om den mapp\n\
som ska kopieras."

117 "Länkningen avbröts - inga objekt länkades.\n\n\
En mapp som du vill ersätta kunde inte tas bort.\n\
   Mapp: %s\n\n\
Vissa objekt i mappen kanske redan hann tas bort innan\n\
länkningen avbröts.\n\n\
1 annat objekt har tagits bort.\n\n\
Den troligaste orsaken är att du inte har skrivbehörighet till\n\
mappen. Ändra skrivbehörigheten eller döp manuellt om den mapp\n\
som ska länkas."

118 "Åtgärden avbröts - inga objekt ändrades.\n\n\
En mapp som du vill ersätta kunde inte tas bort.\n\
   Mapp: %s\n\n\
Vissa objekt i mappen kanske redan hann tas bort innan\n\
processen avbröts.\n\n\
1 annat objekt har tagits bort.\n\n\
Den troligaste orsaken är att du inte har skrivbehörighet till\n\
mappen. Ändra skrivbehörigheten eller döp manuellt om den mapp\n\
som ska ändras."

119 "Flytten avbröts - inga objekt flyttades.\n\n\
En fil som du vill ersätta kunde inte tas bort.\n\
    Fil: %s\n\n\
%d andra objekt har tagits bort.\n\n\
Den troligaste orsaken är att du inte har skrivbehörighet till\n\
filen. Ändra skrivbehörigheten eller döp manuellt om den fil som\n\
ska flyttas."

120 "Kopieringen avbröts - inga objekt kopierades.\n\n\
En fil som du ville ersätta kunde inte tas bort.\n\
    Fil: %s\n\n\
%d andra objekt har tagits bort.\n\n\
Den troligaste orsaken är att du inte har skrivbehörighet till\n\
filen. Ändra skrivbehörigheten eller döp manuellt om den fil som\n\
ska kopieras."

121 "Länkningen avbröts - inga objekt länkades.\n\n\
En fil som du vill ersätta kunde inte tas bort.\n\
    Fil: %s\n\n\
%d andra objekt har tagits bort.\n\n\
Den troligaste orsaken är att du inte har skrivbehörighet till\n\
filen. Ändra skrivbehörigheten eller döp manuellt om den fil\n\
som ska länkas."

122 "Åtgärden avbröts - inga objekt ändrades.\n\n\
En fil som du vill ersätta kunde inte tas bort.\n\
    Fil: %s\n\n\
%d andra objekt har tagits bort.\n\n\
Den troligaste orsaken är att du inte har skrivbehörighet till\n\
filen. Ändra skrivbehörigheten eller döp manuellt om den fil\n\
som ska ändras."

123 "Flytten avbröts - inga objekt har flyttats.\n\n\
En fil som du vill ersätta kunde inte tas bort.\n\
\ Fil: %s\n\n\
1 annat objekt har tagits bort.\n\n\
Den troligaste orsaken är att du inte har skrivbehörighet till\n\
filen. Ändra skrivbehörigheten eller döp manuellt om den fil\n\
som ska flyttas."

124 "Kopieringen avbröts - inga objekt kopierades.\n\n\
En fil som du vill ersätta kunde inte tas bort.\n\
    Fil: %s\n\n\
1 annat objekt har tagits bort.\n\n\
Den troligaste orsaken är att du inte har skrivbehörighet till\n\
filen. Ändra skrivbehörigheten eller döp manuellt om den fil som\n\
ska kopieras."

125 "Länkningen avbröts - inga objekt länkades.\n\n\
En fil som du vill ersätta kunde inte tas bort.\n\
    Fil: %s\n\n\
1 annat objekt har tagits bort.\n\n\
Den troligaste orsaken är att du inte har skrivbehörighet till\n\
filen. Ändra skrivbehörigheten eller döp manuellt om den fil\n\
som ska länkas."

126 "Åtgärden avbröts - inga objekt ändrades.\n\n\
En fil som du vill ersätta kunde inte tas bort.\n\
    Fil: %s\n\n\
1 annat objekt har tagits bort.\n\n\
Den troligaste orsaken är att du inte har skrivbehörighet till\n\
filen. Ändra skrivbehörigheten eller döp manuellt om den fil\n\
ska ändras."

127 "1 objekt har flyttats."

128 "%d objekt har flyttats."

129 "1 objekt har kopierats."

130 "%d objekt har kopierats."

131 "1 objekt har länkats."

132 "%d objekt har länkats."

133 "1 objekt har ändrats."

134 "%d objekt har ändrats."

135 "Det går inte att flytta objektet %s till sig själv.  "

136 "Det går inte att kopiera objektet %s till sig själv.  "

137 "Det går inte att länka objektet %s till sig själv.  "

138 "Det gick inte att döpa om följande objekt:\n\n\
%s\n\n\
Den troligaste orsaken är att du inte har\n\
behörighet att döpa om objektet eller objekten."

139 "Det gick inte att ersätta följande objekt:\n\n\
%s\n\n\
Den troligaste orsaken är att du inte har\n\
behörighet att ersätta objektet eller objekten."

$ "NEW-"
140 Rename new object to:

$ "NEW-"
141 Rename new objects by appending a number


$set 10
$ ****** Module: FileDialog.c ****(developer comment only)********

$ Messages 1, 2, 20, and 21 are titles for error dialogs
$ Message 3 is an error message for dialogs requiring new file names
$ Messages 5 and 27 are titles for the Copy File dialog, Message 5 is used
$   when the user has specified a title for dtfile windows
$ Messages 6-9 are labels for the File Copy, New Dir, and New File dialogs
$ Messages 10-13 and 25 are error messages for the file copy operation
$ Messages 14 and 28 are titles for the New Folder dialog, Message 14 is
$   used when the user has specified a title for dtfile windows
$ Message 16 is a label for the New Folder dialog
$ Messages 22-24 are error messages for the New File/Folder operations
$ Messages 26 and 29 are titles for the New File dialog, Message 26 is used
$   when the user has specified a title for dtfile windows
$ Messages 30 and 31 are titles for the Move File dialog, Message 30 is used
$   when the user has specified a title for dtfile windows
$ Messages 32 and 33 are titles for the Link File dialog, Message 32 is used
$   when the user has specified a title for dtfile windows

1 Fel vid namnbyte av fil
2 Fel vid kopiering av fil

3 Du kan inte använda mellanslag i fil- och mappnamn.

5 Kopiera fil

6 Aktuellt filnamn:
7 Nytt filnamn:
8 Filtyp
9 Visa filtyp

10 Den markerade filen finns inte längre.
11 Filer och mappar kan inte ha tomma namn.
13 Ogiltigt fil- eller mappnamn.

14 Ny mapp

16 Nytt mappnamn:

20 Fel när en mapp skapades
21 Fel när en fil skapandes

22 Det går inte att skapa en fil eller mapp med ett tomt namn.
23 Ogiltigt mappnamn.
24 Ogiltigt filnamn.

25 Du kan inte använda kommandot Kopiera för att\n\
kopiera en fil till arbetsytan. Du måste använda\n\
kommandot `Lägg på arbetsytan` på menyn Fil eller\n\
släppa filen på arbetsytan.

26 Ny fil

27 Filhanterare - Kopiera fil
28 Filhanterare - Ny mapp
29 Filhanterare - Ny fil

30 Flytta fil
31 Filhanterare - Flytta fil

32 Länka fil
33 Filhanterare - Länka fil
34 Målmapp: 
35 Objektnamn:
36 Visa ikon

37 Markerade objekt:
38 Namn på kopia:

39 Fel vid namnbyte
40 Detta objekt är en Funktion.\n\
   Funktionsikonens namn kan inte ändras här.
41 Rätt ikon för den här datatypen visas redan.

42 "Flytta objekt"

43 "Filhanteraren - Flytta objekt"

44 "Kopiera objekt"

45 "Filhanteraren - Kopiera objekt"

46 "Länka objekt"

47 "Filhanteraren - Länka objekt"

48 "Nytt objektnamn:"



$set 11
$ ****** Module: FileManip.c ****(developer comment only)********

$ Message 6 is the title for an error dialog
$ Messages 9, 14, 16, 28, and 30-36 are file manipulation error messages

6 Fel vid filmanipulering

9 Du kan inte släppa filer från\n\
papperskorgen till arbetsytan.

14 Det gick inte att hitta mappens placering.
16 Det går inte att flytta mappar in i sig själva.\n%s
28 Det gick inte att hitta %s.
30 Det gick inte att flytta eller byta namn på mappen %s.\n\
Du måste stänga alla filhanterarfönster för en mapp eller dess \n\
undermappar innan du kan flytta eller byta namn på mappen.
31 Ett fel uppstod vid läsning av %s
32 Det gick inte att byta namn på %s
33 Det gick inte att öppna %s
34 Du har inte behörighet att flytta %s.\n\
Du kan dock kopiera filen. Använd <CTRL>\n\
samtidigt som du drar filen.
35 Det gick inte att kopiera mappen till sig själv.
36 Det gick inte att skapa en underprocess.

37 Du kan inte släppa filer från\n\
papperskorgen till arbetsytan.\n\
Gör så här om du vill ta bort ett objekt från papperskorgen,\n\
   -Markera objektet\n\
   -Välj 'Återställ' på menyn Fil eller\n\
på objektets snabbmeny.\n\
Sedan kan du dra objektet från filhanteraren till\n\
arbetsytans bakgrund.

38 Du har inte behörighet att flytta %s.\n\
Du kan dock kopiera objektet.\n\
Gör så här om du vill kopiera ett objekt:\n\
  - Håll ned <Ctrl>tangenten och\n\
  - dra samtidigt objektet med musen.\n\
eller\n\
  - Använd 'Kopiera till' på menyn 'Markera' på menyraden.

39 Det gick inte att skapa en underprocess.\n\
   Du har nått det maximala antalet processer i systemet.\n\
   Avsluta några av de aktiva processerna eller programmen\n\
   och försök igen.

40 Samma käll- och målobjekt.\n%s
41 Mappen\n%s\nfinns inte.
42 Utrymme saknas på\n\
   enheten Du kopierar till.
43 Utrymme saknas på\n\
   enheten Du flyttar till.
44 Det gick inte att flytta objektet %s.\n\
   Det går inte att flytta ett objekt till samma\n\
   mapp. Ge destinationsmappen ett annat namn\n\
   innan objektet kan flyttas.
45 Det fick inte att kopiera objectet %s.\n\
   Ändra namn på destinationsmappen eller namn\n\
   på kopian innan en kopia kan skapas.
46 Det gick inta att länka objektet %s.\n\
   Ändra namn på destinationsmappen eller namn\n\
   på kopian innan en länk kan skapas.

47 Det gick inte att flytta mappen till sig själv.  

48 Det gick inte att kopiera följande objekt till mappen "%s"\n\n\
%s\n\n\
Den troligaste orsaken är att du inte har\n\
skrivbehörighet till målmappen.

49 Det går inte att flytta följande objekt till mappen "%s"\n\n\
%s\n\n\
Den troligaste orsaken är att du inte har\n\
skrivbehörighet till målmappen.

50 Det går inte att flytta följande objekt till mappen "%s"\n\n\
%s\n\n\
Den troligaste orsaken är att du inte har\n\
behörighet att flytta källobjekt.
$  
$ Messages 51 - 56 are new since CDE/SI.    
$  

51 "Kan inte kopiera filen/mappen därför att\n\
utrymmet på disken du kopierar till inte\n\
räcker till."

52 "Kan inte flytta filen/mappen därför att\n\
utrymmet på disken du flyttar till inte\n\
räcker till."

53 "Kopieringen av filen/mappen misslyckades\n\
beroende på ett internt fel. Det interna\n\
felet är:"

54 "Flyttningen av filen/mappen misslyckades\n\
beroende på ett internt fel. Det interna\n\
felet är:"

55 "Kontakta systemadministratören"

56 "(Okänd):"

58 Process Create Error
59 Cannot create a child process to delete the dropped files.

$set 12
$ ****** Module: FileMgr.c ****(developer comment only)********

$ Message 3 is a dialog title
$ Messages 4-6 are movement warning or error messages
$ Message 7 is the title for the folder popup menus

3 Flytta fil(er)

4 Objektet %s finns för närvarande i mappen %s.\n\
Du flyttar objektet till mappen %s.\n\
Vill du verkligen göra detta?
5 Objektet %1$s finns för närvarande i mappen %2$s.\n\
Du flyttar objektet till mappen %3$s.\n\
Vill du verkligen göra detta?
6 Minst en av filerna som du släppte kommer från en annan\n\
mapp. Du flyttar alla dessa filer till %s.\n\
Vill du verkligen göra detta?

7 Filhanterare

18 Mappen %s är tom.
19 Mappen %s innehåller inga underkataloger.

$set 13
$ ****** Module: Filter.c ****(developer comment only)********

$ Messages 11 and 21-22 are button labels for the Modify Filter List dialog
$ Messages 19 and 24 are titles for the Link File dialog, Message 19 is used
$   when the user has specified a title for dtfile windows
$ Message 23 is a text field label for the Modify Filter List dialog
$ Message 25 is a label for the Modify Filter List dialog

11 Standardinställning

19 Ange alternativ för filter

21 Markera allt
22 Avmarkera alla

23 Filtersträng (valfritt):

24 Filhanterare - Ange alternativ för filter

25 Markera filtyper som ska döljas:

26 Dölj också (valfritt):

27 Visas

28 Döljs

29 Visa också (valfritt):

30 Markera datatyper som ska döljas:

31 "Välj datatyper som ska:"



$set 14



$set 15
$ ****** Module: Find.c ****(developer comment only)********

$ Message 1 is the title for an error dialog
$ Messages 14 and 15 are button labels for the Find Files dialog
$ Messages 23, 26, and 27 are Find Files error messages
$ Messages 29 and 41 are titles for the Find Files dialog, Message 29 is used
$   when the user has specified a title for dtfile windows
$ Messages 30-40 and 42 are labels for the Find Files dialog

1 Fel vid filsökning

14 Starta
15 Stoppa

23 Ogiltig sökinformation.\n\n\
Du måste ange en sökmapp.
26 Ogiltig sökinformation.\n\n\
Den markerade filen finns inte längre.
27 Det finns inga filer som matchar sökdata.

29 Sök filer eller mappar

30 Fyll i ett eller flera fält för att ange vilka filer som ska hittas:
31 Namn på fil eller mapp:
32 Filinnehåll:

$ Messages 33-35 are used on hpux systems
33 På
34 Av
35 Följ länkar:

36 Öppna i nytt fönster
37 Lägg på arbetsytan
38 Hittade filer (efter Innehåll):
39 Hittade filer (efter Namn och Innehåll):
40 Hittade filer (efter Namn):

41 Filhanterare - Sök filer eller mappar

42 Sök i mapp:

43 Fel vid sökning

44 Namnargumentet i fältet 'Sök i mapp' saknas.\n\
Ange namnet på den mapp där sökningen ska börja.

45 Den markerade filen finns inte längre.\n\n\
Någon har tagit bort filen efter att sökningen genomfördes.

46 Sök

47 Fyll i ett eller flera fält för att ange vilka objekt som ska hittas:

48 Öppna mapp

49 Filhanterare - Sök

50 Det saknas ett namn i 'Sök i mapp' eller ett argument i 'Filinnehåll'.\n\
Ange namnet på den mapp där sökningen ska börja, eller\n\
ange söksträngen.


$set 16
$set 17



$set 18
$ ****** Module: Main.c ****(developer comment only)********

$ Message 1 is a blank line
$ Messages 2, 6, 17-18 and 27-28 are error messages which will ONLY APPEAR
$   in the DT ERRORLOG FILE
$ above no longer true for message 2 - it will be displayed.
$ Messages 16 and 32 are titles for the Save Settings dialog, Message 16 is
$   used when the user has specified a title for dtfile windows
$ Messages 19-21 are error messages for invalid directory input.
$ Message 22 is the title for an error dialog
$ Messages 25 and 26 are usage strings
$ Messages 29 and 30 are text for the Save Settings dialog
$ Messages 34 and 35 are drag/drop error messages

$ DO NOT TRANSLATE or CHANGE or LOCALIZE MESSAGE 1
$quote "
1 ""
$quote

$ MESSAGE 2 WILL ONLY APPEAR IN THE DT ERRORLOG FILE
2 Could not create the .dt folder or sub folders.

$ MESSAGE 6 WILL ONLY APPEAR IN THE DT ERRORLOG FILE, and stderr
6 An X Toolkit error occurred... Exiting.\n

16 Spara som standard

$ MESSSAGES 17-18 WILL ONLY APPEAR IN THE DT ERRORLOG FILE
17 Unable to create a file to store the default options.
18 Could not open the session file.

19 Mappspecifikationen,\n\
%s\n\
är inte en mapp.
20 Mappspecifikationen,\n\
%s\n\
finns inte.
21 Felaktig mappspecifikation, %s:%s.

22 Fel vid dragning

23 \nSyntax: %s...\n\n\
   -noview\n\n\
\tDtfile körs i serverläge.\n\n\
   -session sessionsfil\n\n\
\tDtfile körs med den sessionsfil som anges i argumentet sessionsfil.\n\
   -folder mapp[,mapp,mapp]\n\
   -dir mapp[,mapp,mapp]\n\n\
\tDtfile visar ett fönster för varje mapp som anges i\n\
\targumentet mapp. Detta argument kan innehålla flera\n\
\tmappar som avskiljs med kommatecken. Mappar kan anges på formen\n\
\t'sökväg'.\n\n\
   -title titel\n\n\
\tDtfile använder strängen i argumentet titel som\n\
\tnamn på fönstren.\n\n\
   -help_volume hjälpvolym\n\n\
\tDtfile använder den hjälpvolym som anges i argumentet hjälpvolym.\n\n\
   -restricted\n\n\
\tDtfile visar inte mappar ovanför den begränsade mappen.\n\
\tOm tillägget -dir används är den mapp som anges\n\
\tden begränsade mappen. Om tillägget -dir inte används är den aktiva mappen\n\
\tden begränsade mappen.\n\n\
   -grid on/off\n\n\
\ton     = Filerna visas i ett rutnät.\n\
\toff    = Filerna visas i den ordning som de placeras.\n\n\
   -tree on/off\n\n\
\ton     = Filerna visas i en mapp.\n

24 \toff    = Filerna visas i ett mappträd.\n\n\
   -tree_files /never/always/choose\n\n\
\tnever  = Trädet har två lägen: delvis utökat eller komprimerat.\n\
\talways = Trädet har två lägen: helt utökat eller komprimerat.\n\
\tchoose = Trädet har tre lägen: delvis utökat, helt\n\
\tutökat eller komprimerat.\n\n\
   -order alphabetical/file_type/date/size\n\n\
\tFilerna visas i angiven ordning: i bokstavsordning eller\n\
\tefter filtyp, datum eller storlek.\n\n\
   -view no_icon/large_icon/small_icon/attributes\n\n\
\tFilerna visas i angivet format: endast text, text och\n\
\tstora ikoner, text och små ikoner, med attribut.\n\n\
   -direction ascending/descending\n\n\
\tFilerna visas i angiven ordning: stigande eller\n\
\tfallande.\n\n\
   -large_icon_width <size>\n\n\
   -large_icon_height <size>\n\n\
   -small_icon_width  <size>\n\n\
   -small_icon_height <size>\n\n\
       Visningsytans storlek för ikonbilderna i Filhanteraren\n\
       Ikonbilder större än denna storlek beskärs till denna storlek\n\
       Standardvisningsstorleken är 38 för large är och 24 för small\n\n
26 \nSyntax i begränsat läge: %s...\n\n\
   -folder mapp[,mapp,mapp]\n\
   -dir mapp[,mapp,mapp]\n\n\
\tdär mapp är en mapp under och/eller inklusive\n\
\themmappen.\n\n

$ MESSSAGES 27-28 WILL ONLY APPEAR IN THE DT ERRORLOG FILE
27 Det går inte att återhämta den sparade standardheminställningsfilen. Standard används.\n
28 Det går inte att återhämta den sparade standardverktygsinställningsfilen. Standard används.\n

$ Messages 29-30 are the strings displayed in a question dialog
29 Vill du spara fönsterstorleken och\n\
filterinställningarna som används i den aktuella filhanteraren?\n\n\
De kommer att användas i alla nya filhanterarfönster\n\
som startas från frontpanelen.
30 Vill du spara fönsterstorleken, filterinställningarna och övriga\n\
inställningar som används i det aktuella begränsade filhanterarfönstret?\n\n\
De kommer att användas i alla nya begränsade filhanterarfönster\n\
som startas från frontpanelen.

32 Filhanterare - Spara som standard

34 Du kan inte kopiera eller länka filer\n\
och mappar till papperskorgen.

$ 35 Du kan inte kopiera eller länka filer\n\
$ och mappar från papperskorgen.

35 Vill du spara fönsterstorleken, filterinställningarna och\n\
övriga inställningar som används i det aktuella programhanterarfönstret?\n\n\
De kommer att användas i alla nya programhanterarfönster\n\
som startas från frontpanelen.

36 Du kan inte kopiera eller länka filer och mappar från papperskorgen.\n\
Flytta objektet från papperskorgen och lägg det i filhanteraren.\n\
Sedan kan du kopiera eller länka det.

37 Du kan inte kopiera eller länka filer och mappar från papperskorgen.\n\
Flytta objektet från papperskorgen och lägg det i filhanteraren.\n\
Sedan kan du kopiera eller länka det.

38 "Ogiltig mappspecifikation, %s"



$set 19



$set 20
$ ****** Module: Menu.c ****(developer comment only)********

$ Messages 1-8, 123 and 9 are the text and mnemonics for the dtfile menu bar
$ Messages 10/11, 128/129, 124/125, 121/122, 126/127, 16/17, 18/19, 84/85,
$   90/91, 22/23, 117/118, are the text and mnemonics for the Fill 
$   pulldown menu 
$ Messages 10/11, 30/33, 92/93, 34/37, 38/41, 42/94, and 97/98 are the text
$   and mnemonics for the Folder pulldown menu
$ Messages 46/47, 99/100, 48/51, 52/55, 101/102, 103/104, 119/120, and 
$   62/63 are the text and mnemonics for the View pulldown menu
$ Messages 105-116 are the text and mnemonics for the Help pulldown menu

1 Fil
2 F
3 Mapp
4 p
5 Visa
6 V
7 Markera
8 M

9 H

10 Nytt...
11 N
16 Egenskaper...
17 E
18 Sök...
19 S
22 Visa innehållet i papperskorgen...
23 V
30 Uppdatera
33 U
34 Byt till...
37 B
38 Gå till hemkatalogen
41 H
$ NOTE: The mnemonic for Up is Message 94 in this set.
42 Uppåt
46 Nytt
47 N
48 Markera allt
51 M
52 Avmarkera alla
55 v
62 Spara alla egenskaper ...
63 e
84 Lägg på arbetsytan
85 y
90 Lägg i papperskorgen
91 p
92 Byt snabbt till
93 t
$ NOTE: Message 94 is the mnemonic for the "Up" menu item
94 U
97 Terminal
98 T
99 Ordna
100 O
101 Visa dolda filer
102 d
103 Ändra filterlista...
104 f
105 Översikt
106 v
107 Steg-för-steg-anvisningar
108 t
109 Referens
110 R
111 Objekthjälp
112 O
113 Om hjälpfunktionen
114 h
115 Om Filhanterare
116 F
117 Stäng
118 S
119 Välj visningsegenskaper...
120 V
121 Kopiera...
122 p
123 Hjälp
124 Flytta...
125 F
126 Länka...
127 L
128 Byt namn
129 n
130 Ny mapp...
131 m
132 Ny fil...
133 N
134 Gå till...
135 G
136 Öppna papperskorgen
137 Byt namn...
138 Öppna i nytt fönster
139 O
140 Kopiera som länk...
141 Välj visningsegenskaper...
142 Spara som standard...
143 S
144 Välj filteregenskaper...
145 V
146 Öppna Terminal
147 T
148 Innehållsförteckning
149 n
150 Ändra behörighet...
151 Lägg i papperskorgen
152 Flytta till...
153 Kopiera till...
154 Kopiera som länk...
155 Gå till överordnad
156 Visa dolda objekt
157 Om programhanteraren


$set 21
$ ****** Module: ModAttr.c ****(developer comment only)********

$ Messages 1 and 21 are titles for the File Properties dialog, Message 1 is
$   used when the user has specified a title for dtfile windows
$ Messages 2-13 and 18 are labels for the File Properties dialog
$ Message 14 is the output of the file modification date; DO NOT LOCALIZE
$ Messages 15 and 16 are file properties error messages
$ Message 17 is the title for an error dialog
$ Message 19 is the default file owner/group, if real one can't be determined
$ Messages 22-28 are error messages that can be generated when dtfile
$   tries to use the dtfile.config configuration file, this file is
$   used to customize the File Properties dialog

1 Filbehörighet

2 Filnamn:
3 Ägarnamn:
4 Gruppnamn:
5 Behörighet:
6 Läsa
7 Skriva
8 Exekvera
9 Ägare:
10 Grupp:
11 Övriga:
12 Storlek (i byte):
13 Ändrad:

$ DO NOT LOCALIZE MESSAGE 14
14 %x %X

15 Ogiltigt filattribut.\n\n\
Den angivna användaren finns inte.
16 Ogiltigt filattribut.\n\n\
Den angivna gruppen finns inte.

17 Fel i behörighet

18 är länkad med:

19 Okänt

21 Filhanterare - Filbehörighet

22 Det går inte att öppna konfigurationsfilen för filhanteraren:
24 Okänt fältnamn i konfigurationsfilen för filhanteraren:
25 Det finns ingen information om filsystemidentifieraren i konfigurationsfilen för filhanteraren
26 Det går inte att skapa en process för filsystemdialog
27 Fel vid exekvering av filsystemdialog
28 Försöker exekvera:

$ do not localize "fsDialog"
29 Det finns inget värde för fältet fsDialog i konfigurationsfilen för dtfile

30 Namn:

31 Ägarnamnet är ogiltigt.\n\
Den angivna användaren finns inte.

32 Gruppnamnet är ogiltigt.\n\n\
Det finns ingen användargrupp med detta namn i systemet.\n\
Använd ett systemadministrationsprogram om du vill skapa\n\
en användargrupp med detta namn.

33 Fel i behörighet

34 Filhanterare - Behörighet

35 "Dialogrutan Behörighet för objektet\n\n\
%s/%s\n\n\
stängdes eftersom objektet redan har tagits bort\n\
från systemet."

36 "Ändra behörigheter - Varning"

37 "Du ändrar behörigheterna för den aktuella mappen\n\
på ett sådant sätt att du inte kommer att kunna\n\
se mappen längre.  Den kommer att stängas\n\
automatiskt och du kommer inte att kunna öppna\n\
den igen.\n\n\
Vill du göra det?"

38 File Manager Error
39 File Open Error

$set 22
$ ****** Module: ModAttrP.c ****(developer comment only)********

$ Messages 1 and 3 are file properties error messages
$ Message 2 is the title for the properties error dialog

1 Det går inte att öppna filen, operationen har avbrutits
2 Fel i behörighet
3 Det går inte att ändra egenskaperna för %s.
4 Det går inte att öppna objektet.
5 Fel vid inställning av behörighet



$set 23
$ ****** Module: Prefs.c ****(developer comment only)********

$ Messages 1 and 22 are titles for the Set Preferences dialog, Message 1 is
$   used when the user has specified a title for dtfile windows
$ Messages 2-20 and 23-29 are labels for the Set Preferences dialog

1 Välj visningsegenskaper

2 Visa
3 Efter namn
4 Efter namn och ikon
5 Efter namn och små ikoner
6 Efter Namn, datum, storlek ...
7 Visa
8 Som enstaka mapp
9 Som träd
10 Ordning
11 Efter filtyp
12 I bokstavsordning
13 Efter datum
14 Efter storlek
15 Ordning
16 Stigande
17 Fallande
18 Placering
19 Sorterat rutnät
20 Enligt placering

22 Filhanterare - Välj visningsegenskaper

23 Huvud
24 Ikonsökväg
25 Aktuell mapp
26 Statusrad
27 Endast mappar
28 Mappar, sedan filer
29 Mappar och filer
30 Standardinställning

31 Visa som
32 Endast namn
33 Stora ikoner
34 Små ikoner
35 Rader och kolumner
36 Sökväg
37 Meddelanderad


$set 24
$set 25
$set 26



$set 27
$ ****** Module: Trash.c ****(developer comment only)********

$ Messages 3, 4, and 81 are titles for error warning dialogs
$ Message 5-8, 56, 58, 78, 82, and 83 are trash error messages
$ Message 9 is the title of the trash container
$ Messages 24/26 and 28/30 are the text and mnemonics for the File pulldown
$   menu
$ Message 73 is an error dialog title
$ Messages 74, 75, 79, and 80 are trash warning messages

3 Fel i papperskorgen
4 Papperskorg - varning

5    Följande filer eller mappar kunde inte placeras i papperskorgen:     \n
6    Följande är någondera en mapp som inte är tom    \n   \
eller en fil som du inte har behörighet att ändra.    \n   \
Vill du fortsätta?\n
7    Du har inte behörighet att ändra följande filer.    \n   \
Vill du fortsätta?\n
8    Följande mappar är inte tomma.    \n   \
Vill du fortsätta?\n

9 Papperskorg

24 Återställ
26 t
28 Förstör
30 F

56    Följande filer kunde inte återställas:\n
$ Message 58 supplements Message 56
58 (plus %d ytterligare filer)

73 Förstör fil(er)

74 Följande fil finns också på arbetsytan:\n\
   %s\n\
Om du flyttar filen till papperskorgen tas den bort\n\
från arbetsytan.
75 Följande filer finns också på arbetsytan:\n\
   %s\n\
Om du flyttar filerna till papperskorgen tas de bort\n\
från arbetsytan.

78 Filer och mappar vars namn innehåller\n\
mellanslag kan inte tas bort, eftersom de\n\
inte stöds i filhanteraren.\n

79 Filen %s på arbetsytan finns också i filsystemet i:\n\
   %s\n\
Om du flyttar filen till papperskorgen tas den bort\n\
från filsystemet.

80 Objekten på arbetsytan finns också i filsystemet i:\n\
   %s\n\
Om du flyttar filerna till papperskorgen tas de bort\n\
från filsystemet.

81 Fel vid förstöring av fil eller mapp

82 Följande fil kunde inte tas bort från filsystemet:   \n

83 Filen .trashinfo i $HOME/.dt/Trash kan inte öppnas.\n\
   Du måste ha tillgång till den för att papperskorgen ska fungera.\n\
   Kontrollera behörigheterna för filen och dess mapp.\n
84 När objekt i papperskorgen förstörs tas de bort permanent\n\
   från systemet.\n\n\
   Vill du verkligen förstöra objekt?\n
85 Objektet som du tar bort är en länk till det\n\
   ursprungliga objektet i filhanteraren i\n\
   %s\n\
   Om du väljer OK tas både länken och det\n\
   ursprungliga objektet bort.\n\
   Gör så här om du bara vill ta bort länken:\n\
      1) Välj Avbryt i denna ruta.\n\
      2) Klicka med MENY på länken.\n\
      3) Välj 'Ta bort från arbetsytan' på menyn som visas.
86 Objekten som du tar bort är länkar till de\n\
   ursprungliga objekten i filhanteraren i\n\
   %s\n\
   Om du väljer OK tas både den länkarna och de\n\
   ursprungliga objekten bort.\n\
   Gör så här om du bara vill ta bort länkarna:\n\
      1) Välj Avbryt i denna ruta.\n\
      2) Klicka med MENY på länken.\n\
      3) Välj 'Ta bort från arbetsytan' på menyn som visas.
87 Det går inte att öppna objektet i papperskorgen.\n\n\
   Välj 'Återställ' om du vill öppna ett objekt för att lägga\n\
   tillbaka det i filhanteraren och öppna det därifrån.

88 Det går inte att skriva till en temporär fil.\n\
   Filsystemet är kanske fullt.

89 Det går inte att öppna papperskorgens katalog (Trash)\n\
   %s\n\
   (Verifiering, behörighetsproblem)\n\n\
   Vissa operationer som rör papperskorgen kommer inte att utföras.

90 'Papperskorg' kan inte startas.\n\
   Kontrollera följande:\n\
        1)behörigheten i $HOME/.dt/Trash\n\
        2)tillgängligt skivutrymme\n\
        3)verifiering.

$ 91 is not used
91 Det finns en länkad kopia av följande objekt\n\
på arbetsytan:\n\
Om du flyttar objektet till papperskorgen tas det bort\n\
både från filhanteraren och arbetsytan.

$ 92 is not used
92 Det finns en länkad kopia av följande objekt\n\
på arbetsytan:\n\
Om du flyttar objekten till papperskorgen tas de bort\n\
både från filhanteraren och arbetsytan.

93 Det går inte att få tillgång till denna\n\
   informationsfil för papperskorgen:\n\
   %s\n\
   Vissa operationer som rör papperskorgen kommer inte att utföras.\n\
   Följande är de vanligaste orsakerna:\n\
     - nätverksverifiering.\n\
     - otillräckligt diskutrymme.\n\
     - felaktig behörighet $HOME/.dt/Trash.

94 Objekt vars namn innehåller mellanslag kan\n\
inte tas bort, eftersom de inte stöds i filhanteraren.\n\
Gör följande om du vill ta bort objekten:\n\
  - Välj 'Öppna terminalfönster' på menyraden i filhanteraren.\n\
  - Välj kommandot Ta bort.

95 Det går inte att få tillgång till katalogen för papperskorgen:\n\
   %s\n\
   Vissa operationer som rör papperskorgen kommer inte att utföras.\n\
   Följande är de vanligaste orsakerna:\n\
     - nätverksverifiering.\n\
     - otillräckligt diskutrymme.\n\
     - felaktig behörighet.

96 Det gick inte att ta bort följande objekt från filsystemet:   \n

97 Det gick inte att placera följande objekt i papperskorgen:     \n

98 (plus %d ytterligare objekt)

$ message 99 is not used
99 Du har inte skrivbehörighet till följande fil(er):\n\
Vill du fortsätta?\n

100 Följande mapp(ar) är inte tom(ma).\n\
Vill du fortsätta?\n

101 Det gick inte att återställa följande objekt:\n

102 På arbetsytan finns en länk till följande objekt:\n\
%s\n\
Om du flyttar objektet till papperskorgen tas det bort\n\
både från filhanteraren och arbetsytan.

103 På arbetsytan finns länkar till följande objekt:\n\
%s\n\
Om du flyttar objekten till papperskorgen tas de bort\n\
både från filhanteraren och arbetsytan.

104 Du har inte behörighet att ändra följande fil(er):\n\
Vill du fortsätta?\n

105 Standardfunktionen för ett 'kastat' objekt körs ej.\n\n\
    Kör den genom att lägga tillbaka objektet på\n\
    filhanteraren, använd 'Återställ', och kasta\n\
    objektet därifrån.

106 Om Papperskorgen

107 "Minst ett av objekten i följande mapp kan inte tas bort:\n\n\
%s\n\n\
Den troligaste orsaken är att du inte har behörighet att \n\
ta bort objektet."

108 "Följande fil kan inte tas bort: \n\n\
%s\n\n\
Den troligaste orsaken är att du inte har behörighet att\n\
ta bort filen."

109 "Återställ - Varning"

110 "Det finns redan ett objekt med namnet\n\n\
    %s\n\n\
Välj OK om du vill ersätta det med\n\
objektet från Papperskorgen.\n\n\
Välj Avbryt om du vill behålla objektet.\n\
Om du sedan döper om objektet kan du göra om\n\
Återställ och på så sätt behålla båda objekten."

111 "Återställ - Fel"

112 "Följande objekt kan inte tas bort.\n\
Den troligaste orsaken är att objektet/objekten\n\
redan har tagits bort.\n\n"

113 "Minst ett objekt i följande mapp(ar) eller \n\
själva mappen kan inte tas bort.\n\
Den troligaste orsaken är att du inte har behörighet\n\
att ta bort objektet.\n\n"

114 "Följande fil(er) kan inte tas bort. Den troligaste orsaken \n\
är att du inte har behörighet att ta bort den eller dem.\n\n"

115 "Följande objekt kan inte tas bort.\n\
Skrivbordet fungerar inte som det ska utan objektet/objekten.\n\n"

$set 28
$ ****** Module: Desktop.c ****(developer comment only)********

$ Messages 1-3 Workspace error messages
$ Message 4 is the title for the Workspace popup menus
$ Messages 5-8 are the labels and mnemonics for the Workspace obj popup menu

1 Det finns redan en fil med detta namn på arbetsytan.\n\
Välj ett annat namn.
2 Det finns redan en fil med detta namn i den ursprungliga mappen\n\
(den mapp där filen hämtades).\n\
Välj ett annat namn.
3 Filen %s finns redan på arbetsytan.\n\
Du kan inte lägga en annan fil med samma namn på arbetsytan.\n\
Om du vill placera bägge filerna på arbetsytan måste du byta\n\
namn på en av dem.

4 Objekt på arbetsytan

5 Ta bort från arbetsytan
6 b
7 Öppna överordnad mapp
8 Ö

9 Det finns redan ett objekt med detta namn på arbetsytan.\n\
Välj ett annat namn.

10 Det finns redan ett objekt med detta namn i den ursprungliga mappen\n\
(den mapp där filen hämtades).\n\
Välj ett annat namn.

11 Objektet %s finns redan på arbetsytan.\n\
Du kan inte lägga ett annat objekt med samma namn på arbetsytan.\n\
Om du vill placera bägge objekten på arbetsytan måste du byta namn på ett av dem.

12 "En ogiltig arbetsyta har använts. Standardarbetsytan kommer att\n\
bli arbetsyta ett och ett objekt kallat:\n\
   %s\n\
finns redan i bakgrunden på den här arbetsytan.\n\
Du kan inte lägga ett objekt till med samma namn på arbetsytan.\n\
Om du vill ha båda objekten på arbetsytan, måste döpa om ett av dem."

13 "Objekt:\n\
   %s\n\
finns inte i det här filsystemet."



$set 29
$ ****** Module: Help.c ****(developer comment only)********

$ Message 1 is the title for the dtfile help dialog
$ Messages 1 and 2 are titles for the Help dialog, Message 2 is
$   used when the user has specified a title for dtfile windows
$ Message 3 is the text for help on the "Open New View" action
$ Message 4 is the text for help on the "Open In Place" action
$ Messages 5-6 are help error messages

1 Filhanterare - Hjälp
2 %s Hjälp

3 Funktionen `%s` öppnar ett nytt filhanterarfönster för den angivna mappen.

4 Funktionen `%s` visar den angivna mappen i det aktuella filhanterarfönstret.

5 Det finns inget hjälpavsnitt för funktionen %s.
6 Det finns inget hjälpavsnitt för filtypen %s.

7 Funktionen `%s` öppnar ett nytt filhanterarfönster\n\
för den angivna mappen.

8 Funktionen `%s` visar den angivna mappen i\n\
det aktuella filhanterarfönstret.

9 Papperskorg - Hjälp

10 "Ingen Hjälp är tillgänglig för flera markerade objekt."

$set 30
$ ****** Module: SharedMsgs.c ****(developer comment only)********

$ The following messages are used in more than one place within the
$ dtfile source.  Unless otherwise specified, they are error messages.

1 Det går inte att läsa från %s

$ Message 2 is used to format the file attributes in 'by attributes' mode
2 %b %d %H:%M:%S %Y

3 Det går inte att flytta eller kopiera filen %s från värddatorn %s.\n\
Värddatorn %s kanske inte är rätt ansluten till filsystemet.
4 Det går inte att flytta eller kopiera filen %1$s från värddatorn %2$s.\n\
Värddatorn %3$s kanske inte är rätt ansluten till filsystemet.
5 Det går inte att läsa %s.
6 Det går inte att skriva i mappen %s.
7 Det går inte att skapa %s.
8 Det går inte att ta bort %s.
9 Det finns redan en mapp eller fil med namnet\n\
  %s\n\
  och den kan inte skrivas över.\n\
  Välj ett annat namn.

$ Message 10 is the help topic string for help on a file icon or action
10 Beskrivning av filtyp/funktion

11 Du kan inte ange en sökväg som en del av filnamnet.
12 Ogiltig sökinformation.\n\n\
Följande mapp finns inte eller går inte att få tillgång till:\n\n\
     %s\n

$ Messages 13-16 are dialog titles
13 %s Översikt
14 %s Självstudier
15 Om %s
16 Filhanterare

$ Message 18 is appended to ".." folders
18 (Gå uppåt)

$ Messages 19 and 20 are messages which appear when a user clicks item help
$ outside the application.  19 is the title, while 20 is the message.
19 Fel i objekthjälpen
20 Du måste markera en komponent\n\
i filhanteraren.

$ Messages 21 and 22 are titles for error dialogs
21 Fel vid flyttning av fil
22 Fel vid länkning av fil
23 Det går inte att skapa '%s'.\n\n\
Den vanligaste orsaken är att du inte har\n\
rätt behörighet för\n\
filerna eller mapparna.\n\n\
Du kan se den aktuella behörigheten genom att markera objektet\n\
och välja 'Ändra behörighet...'

24 Du måste markera ett objekt i filhanteraren.\n\
'Objekthjälp' fungerar bara i det program där den startas.


25 Det går inte att skapa '%s'.\n\n\
Den vanligaste orsaken är att du inte har\n\
rätt behörighet för\n\
filerna eller mapparna.\n\n\
Du kan se den aktuella behörigheten genom att markera objektet\n\
och välja 'Ändra behörighet...' på objektets meny\n\
eller på menyn Markera.

26 Du måste markera ett objekt i programhanteraren.\n\
'Objekthjälp' fungerar bara i det program där den startas.



$set 31

$set 32
$ ****** Module: dterror.src ****(developer comment only)********
$ Message 1 is a title for dterror
1 dt-fel
2 Filhanterare - Fel när katalogen skulle öppnas


$set 33
1 Flera objekt
2 Aktuell mapp

$quote "
3 "Fel vid Flytta objekt"

4 "Fel vid Kopiera objekt"

5 "Fel vid Länka objekt"

6 "Den plats du försöker flytta till:\n\n\
   %s\n\n\
finns inte i filsystemet."

7 "Den plats du försöker kopiera till:\n\n\
   %s\n\n\
finns inte i filsystemet."

8 "Den plats du försöker länka till:\n\n\
   %s\n\n\
finns inte i filsystemet."

9 "Den plats du försöker flytta till:\n\n\
   %s\n\n\
är inte en mapp."

10 "Den plats du försöker kopiera till:\n\n\
   %s\n\n\
är inte en mapp."

11 "Den plats du försöker länka till:\n\n\
   %s\n\n\
är inte en mapp."

12 "Objektet som du försöker flytta:\n\n\
   %s\n\n\
finns inte i filsystemet."

13 "Ett av objekten som du försöker flytta:\n\n\
   %s\n\n\
finns inte i filsystemet.\n\
Flyttar inget av dem."

14 "Det objekt som du försöker kopiera:\n\n\
   %s\n\n\
finns inte i filsystemet."

15 "Ett av objekten som du försöker kopiera:\n\n\
   %s\n\n\
finns inte i filsystemet.\n\
Kopierar inget av dem."

16 "Objektet som du försöker länka:\n\n\
   %s\n\n\
finns inte i filsystemet."

17 "Ett av objeketen som du försöker länka:\n\n\
   %s\n\n\
finns inte i filsystemet.\n\
Länkar inget av dem."


$set 99
$ ****** Version Information ********
$ DO NOT TRANSLATE THESE MESSAGES
$ DO NOT CHANGE THESE MESSAGES
$ DO NOT LOCALIZE THESE MESSAGES
$ These messages are used for the version information.
1 "@(#)version_goes_here"
2 "\n@(#)_DtMessage catalog source $TOG: dtfile.msg /main/6 1998/04/29 15:12:43 rafi $"

