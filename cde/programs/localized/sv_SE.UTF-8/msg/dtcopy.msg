$ $XConsortium: dtcopy.msg /main/3 1996/07/24 18:59:18 drk $
$ *****************************************************************************
$ **
$ **  File:        dtcopy.msg
$ **
$ **  Project:     CDE File Manager
$ **
$ *****************************************************************************

$set 2
$ ****** Module: main_dtcopy.c ******
3 fil
4 filer
5 undermapp
6 undermappar
7 Fel: inga
8 Fel: %d
10 Syntax: %s [alternativ ...] källa mål\n
11 Filhanterare - Flytta mapp
12 Filhanterare - Kopiera mapp
13 %s har inte tagits bort eftersom ett fel inträffade.
14 Klart.
15 avbröts i
$ ** Message 20 - 32
$ ** LongFileOpNames ... message number = op + 20.
20 bearbetar
21 öppnar
22 öppnar
23 öppnar
24 läser länk
25 tar bort
26 kopierar
27 skapar katalog
28 kopierar länk
29 skapar länk
30 arbetar
31 är en kopia
32 är en länk
33 byter namn

$set 3
$ ****** Module: copydialog.c ******
1 Fortsätt
2 Paus
3 Flyttar:
4 Kopierar:
5 Till mapp:
7 Detta avslutar kopieringen. Alla filer som har\n\
kopierats behålls i målmappen.
10 Detta avslutar flyttningen. Alla filer som har\n\
flyttats behålls i målmappen.
11 Kopiera mapp - varning
12 Flytta mapp - varning
13 Avbryt kopiering
14 Fortsätt kopiering
15 Avbryt flyttning
16 Fortsätt flyttning
17 Flyttade
18 Kopierade

$set 4
$ ****** Module: errordialog.c ******
1 Fel %s\n%s
2 Stanna vid fel
3 Fel vid kopiering av mapp
4 Fel vid flyttning av mapp

5 "Objektflytt - fel"

6 "Objektkopiering - fel"

7 "Objektet Papperskorg - fel"

8 "Du har inte behörighet att lägga objektet \n\n\
    %s\n\n\
i papperskorgen.\n\n\
Använd alternativet Ändra behörighet på objektets \n\
meny eller från menyn Markerade objekt om du vill\n\
aktivera din läsbehörighet för objektet.\n\n\n\
Obs: Om objektet är en mapp måste du också ha\n\
läsbehörighet för vart och ett av objekten i\n\
mappen innan du kan lägga mappen i papperskorgen."


$set 5
$ ****** Module: overwrtdialog.c ******
1 Det finns redan en fil med namnet\n%s\ni mappen\n%s
2 Flytta mapp - filer skrivs över
3 Ersätt befintlig fil
4 Byt namn på befintlig fil till:
5 Byt namn på nytt objekt till:
6 Hoppa över denna fil
7 Gör på samma sätt vid kommande namnkonflikter
8 Vad ska göras:
9 Kopiera mapp - filer skrivs över
10 %s\nfinns redan. Välj ett annat namn.
11 Fel vid överskrivning av filer
12 %s\nkan inte byta namn till\n%s\n%s

$set 6
$ ****** Module: utils.c ******
$ ** Button labels.
1 OK
2 Paus
3 Avbryt
4 Hjälp
5 Kopiera katalog - Hjälp
6 Fortsätt
7 Avbryt kopieringen
8 Avbryt flyttningen
