
$ "$XConsortium: dtlogin.msg /main/3 1995/11/08 13:34:30 rswiston $"

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

1 Ange användarnamn
2 Ange lösenord
3 OK
4 Börja om
5 Alternativ
6 Hjälp

$ 
$ messages 7 through 11 are options button menu items
$ 

7 Återställ inloggningsskärm
8 Kommandoradsinloggning
9 Version...
10 Session med endast ett fönster
11 Språk

$ 
$ messages 12 through 14 are miscellaneous button labels
$ 
12 Starta DT
13 Avbryt
14 Hjälp


$ 
$ message 15, 17 are more option button menu items 
$ message 16 is the welcome message
$ 
15 DT Ren session
16 Välkommen till
17 Normal session

$ 
$ message 18-24
$ 
18 * Lämnar tillfälligt inloggningen via skrivbordet...\r\n*\r\n
19 * Tryck på [Enter] för att få en ledtext för inloggningen.\r\n*\r\n
20 * Logga in. Inloggningen via skrivbordet kommer att fortsätta så snart du loggar ut.\r\n*\r\n
21 * Fortsätter inloggningen via skrivbordet...\r\n*\r\n
22 * Startar inloggningen via skrivbordet på visningsenheten %1$s...\r\n*\r\n
23 * Vänta på inloggningsskärmen innan du loggar in.\r\n*\r\n
24 * X-hanteraren kan inte startas på visningsenheten %1$s...\r\n*\r\n

$ messages 25 is a options button menu item
25 Session

$ 
$ 26 replaces 16 as the greeting after a user name has been 
$ entered. The %s is replaced by the user name.
$ 
26 Välkommen %s

$set 2
$ set 2 is error messages
$ 

1 Felaktig inloggning. Försök igen.
2 Det går inte att byta till hemkatalogen.
3 Maximalt antal användare har redan loggat in.
$ message 4 is a lengthy message that should be localized to correspond to 
$ the language.  
4 \n\
Antingen måste du välja ett lösenord till ditt kontot för första\n\
gången eller också har ditt aktuella lösenord upphört att gälla.\n\
\n\
När du har angivit lösenordet måste du logga in igen\n\
med det nya lösenordet.
5 Inloggningsfel, ogiltigt användar-ID.
6 Inloggningsfel, ogiltigt grupp-ID.
7 Inloggningsfel, ogiltigt gransknings-ID.
8 Inloggningsfel, ogiltig granskningsflagga.
9 Inloggningarna är avaktiverade för tillfället.
10 Endast root vill köra %1$s\n
11 Bygger om standardspråklistan från %1$s\n
12 Genomsöker både konfigurations- och serverfiler\n
13 Läser om konfigurationsfilen %1$s\n
14 Läser om serverfilen %1$s\n
15 Läser om accessfilen %1$s\n
16 dtlogin: för lite minne i rutinen\n


$set 3
$ set 3 is help text
$ 

$ message 1 is generated when the help button on the login screen is clicked.
1 \n\
Inloggning - Hjälp\n\
\n\
Använd inloggningsskärmen för att identifiera dig i systemet.\n\
\n\
Gör följande när du vill logga in:\n\
       1) Skriv ditt användarnamn och tryck på <Enter> eller välj [OK].\n\
          (om du inte har skapat någon användaren, ange "root" som\n\
          användarnamn)\n\
       2) Skriv ditt lösenord och tryck på <Enter> eller välj [OK].\n\
\n\
[Börja om]\n\
   Raderar det skrivna så att du kan skriva användarnamnet och\n\
   försöka igen.\n\
\n\
[Alternativ]\n\
   Här kan du ändra alternativ för sessionen:\n\
\n\
   Placera muspekaren på knappen [Alternativ] och håll ned\n\
   musknapp 1. Dra pekaren genom alternativen.\n\
   När du släpper upp musknappen markeras det alternativ\n\
   som muspekaren befinner sig på. Du kan välja bland följande:\n\
\n\
   Språk - för att ändra språket i sessionen\n\
   Session - för att välja en grafisk miljö\n\
     Skrivbordssession - för CDE (Common Desktop Environment)\n\
     Kommandoradssession - för X-windows-miljö\n\
   Kommandoradsinloggning - för att växla till kommandoradsmiljö\n\
   Återställ inloggningsskärm - för att starta om X-hanteraren\n\
\n\
Om till exempel systemmeddelanden har gjort skärmen svårläsbar\n\
kan du klicka var som helst i bakgrunden med någon av musknapparna\n\
för att förnya skärmen.\n\
\n\
Om du vill ha mer hjälpinformation loggar du först in och klickar sedan\n\
på hjälphanterarikonen på frontpanelen (böcker med ett frågetecken).\n\
Gå sedan till hjälpvolymen om inloggningshanteraren.\n\

$ message 2 is generated when you attempt to start a system whose id is
$ set to unknown or if it is not configured for networking.
2 \n\
Arbetsstationen har inget unikt namn. Den har tillfälligt\n\
fått namnet "okänd". Du rekommenderas att namnge\n\
arbetsstationen.\n\
\n\

$ message 3 is generated when the help button on the chooser screen is clicked.
3 \n\
Väljaren - Hjälp\n\
\n\
Du använder väljarskärmen för att välja vilken server du ska logga in på.\n\
\n\
Denna skärm innehåller en lista med två delar för varje\n\
post. Den första delen (servernamnet) består av nätverksnamnet\n\
för alla tillgängliga servrar. Den andra delen (serverinformation)\n\
består av olika information beroende på vilken\n\
inloggningshanterare som körs på servrarna. Om en server\n\
kör CDE-skrivbordet bör den t ex tillhandahålla information\n\
om antalet aktuella användare (eller tty-enheter) och\n\
laddningsmedelvärden (CPU-medelvärdet) under de senaste 5, 10 och\n\
15 minuterna.\n\
\n\
Om du vill välja en server som du ska logga in på markerar du\n\
den rad i listan som innehåller namnet på den önskade servern.\n\
\n\
När du har markerat det önskade alternativet väljer du knappen OK.\n\
Då visas inloggningsskärmen för den markerade servern.\n\
\n\
[Uppdatera lista]\n\
   Detta alternativ rensar listan och bygger om den genom att\n\
   granska servrarna på nätverket för att se om de tillåter inloggningar.\n\
\n\
[Alternativ]\n\
   Här kan du ändra alternativ för sessionen:\n\
\n\
   Placera muspekaren på knappen [Alternativ] och håll ned\n\
   musknapp 1. Dra pekaren genom alternativen.\n\
   När du släpper upp musknappen markeras det alternativ\n\
   som muspekaren befinner sig på. Du kan välja bland följande:\n\
\n\
   Språk - för att ändra språket i sessionen\n\
   Återställ inloggningsskärm - för att starta om X-hanteraren\n\
\n\
Om till exempel systemmeddelanden har gjort skärmen svårläsbar\n\
kan du klicka var som helst i bakgrunden med någon av musknapparna\n\
för att förnya skärmen.\n\
\n\


$set 4
$ set 4 is log error messages
$ 

1 Det går inte att hitta accessfilen \"%1$s\" på värddatorn \"%2$s\"\n
2 ReadHostEntry\n
3 Accessfilen %1$2s, okänd visningsenhet %1$2s\n
4 Det går inte att öppna accessinställningsfilen %1$s, XDMCP-förfrågningarna kan inte garanteras\n
5 Verifieringsfel i domän/operativsystem: %1$s (%2$s/%3$s)
6 Det går inte att öppna serverns verifieringsfil %1$s\n
7 Det går inte att skriva serverns verifieringsfil %1$s\n
8 saveAddr
9 Det går inte att hämta konfigurationen för nätverksgränssnittet 
10 setAuthNumber
11 Det går inte att låsa verifieringsfilen %1$s eller säkerhetskopian %2$s\n
12 Det går inte att flytta verifieringen på plats\n
13 Det går inte att skapa/låsa pid-fil %1$s\n
14 Det går inte att låsa pid-filen %1$s. En annan Dtlogin körs (pid %2$d)\n
15 Det går inte att få tillgång till serverfilen %1$s\n
16 NewDisplay
17 Det går inte att öppna errorLogFile %1$s\n
18 TrimErrorLog(): den maximala storleken på felloggfilen är 200 Kb.\n
19 Visningsenhetens namn saknas i serverfilen\n
20 Visningsenhetens typ saknas för %1$s\n
21 Värddatornamnet %1$s är ogiltigt. DISPLAY anges till ':0'\n
22 Otillåten typ %1$s för visningsenheten %2$s\n
23 Pseudoåterställning har nått tidsgränsen.\n
24 GetResource
25 ingen plats för argumentet realloc\n
26 Det går inte att öppna konfigurationsfilen %1$s\n
27 För många argument på kommandoraden:
28 StartServer(): inga argument\n
29 Servern %1$s kan inte exekveras\n
30 Klyvning misslyckades i StartServer() , sover\n
31 Servern slutade oväntat att fungera\n
32 Hängde sig i XOpenDisplay(%1$s) försök #%2$d, avbryter.\n
33 Försök #%1$d att öppna servern misslyckades för %2$s, ger upp\n
34 Kritiskt in-utdatafel %d (%1$s)\n
35 X-fel\n
36 Sessionsstarten misslyckades\n
37 VARNING: det gick inte att säkra visningsenheten %1$s\n
38 VARNING: det gick inte att säkra tangentbordet på visningsenheten %1$s\n
39 BLS - Det gick inte att hämta information om kontot.\n
40 BLS - Det gick inte att hämta skyddad information om kontot.\n
41 BLS - Det gick inte att verifiera känslighetsnivå.\n
42 BLS - Det gick inte att växla till användaren : %1$s.\n
43 AFS - setgroups() misslyckades när sessionen startades\n
44 Setuid misslyckades för användaren  %s, fel nr = %d\n
45 Det finns ingen hemkatalog %1$s för användaren %2$s, använder /\n
46 Sessionsexekveringen misslyckades %1$s\n
47 Sessionen har inga kommandon/argument\n
48 Det går inte att starta en session för %s, klyvningen misslyckades\n
49 Dtlogin kan inte koppla bort klienten\n
50 Det går inte att exekvera %1$s\n
51 Det går inte att klyva för att exekvera %1$s\n
52 Felaktigt verifieringsnamn \"%1$s\". Använd AFS, Kerberos eller standardnamnet.\n
53 Det går inte att exekvera Dtgreet %1$s för %2$s\n
54 Det går inte att klyva för att exekvera Dtgreet %1$s\n
55 Det gick inte att skapa sockel på port %1$d\n
56 Fel vid bindning av sockeladress %1$d, fel nr = %2$d\n
57 makeEnv
58 setEnv
59 parseArgs
60 Genomsökningen av katalogen %1$s överstiger %2$d sekunder. Genomsökningen avbryts...\n
61 %s tillhör mer än %1$d grupper, %2$s ignoreras\n
62 fel i audwrite(): anroparen är inte superanvändare\n
63 fel i audwrite(): ogiltigt händelsenummer i granskningspost.\n
64 fel i audwrite(): fel nr = %1$d\n
65 AFS-verifiering misslyckades: %1$s\n
66 Kontot för %1$s har upphört att gälla. Kontakta systemadministratören.\n
67 Servern har slutat att fungera  %1$s\n
68 Det går inte att öppna en bitmappsfil för logotypen: %1$s\n
69 Det går inte att läsa meddelandeförteckningen \"%1$s\". Standardspråket används.
70 Det gick inte att initiera visningsenheten %1$s. Servern kan ha kopierats.\n
71 (%1$s): Bredden på Login matte överstiger visningsenheten, standardinställningen används i stället. Matte-enheterna är bildpunkter.\n
72 (%1$s) Höjden på Login matte överstiger visningsenheten, standardinställningen används i stället. Matte-enheterna är bildpunkter.\n

$set 5
$ set 5 is for other stuff specific to chooser.
1 Markera den server som du vill logga in på:
$ 1 is the title/instructions for the chooser dialog
2 Servernamn                     Serverinformation
$                                ^
$                                | This needs to start in this column
$ 2 is the headings for the list
3  Uppdatera lista 

$set 99
$    DO NOT TRANSLATE THESE MESSAGES
$    DO NOT CHANGE THESE MESSAGES
$    DO NOT LOCALIZE THESE MESSAGES
$    These messages are used for the version information.
1 @(#)version_goes_here
2 \n@(#)_DtMessage catalog source $XConsortium: dtlogin.msg /main/3 1995/11/08 13:34:30 rswiston $
