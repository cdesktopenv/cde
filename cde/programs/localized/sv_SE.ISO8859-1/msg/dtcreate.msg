$ $XConsortium: dtcreate.msg /main/3 1995/11/08 13:32:50 rswiston $
$ %Z%%M%        %I%  %W% %G% %U%
$ 
$   COMPONENT_NAME: dtcreate
$ 
$   FUNCTIONS: none
$ 
$   ORIGINS: 27
$ 
$ 
$   (C) COPYRIGHT International Business Machines Corp. 1993
$   All Rights Reserved
$   Licensed Materials - Property of IBM
$   US Government Users Restricted Rights - Use, duplication or
$   disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
$ 
$ **   Description:
$ **   ------------
$ **   This is the source message catalog file for dtcreate.

$set 2
$ *****************************************************************************
$ **
$ ** Message set for menu options on main window
$ **
$ *****************************************************************************
10 Fil
11 F
15 Öppna...
16 O
20 Nytt
21 N
25 Spara
26 S
30 Avsluta
31 x
35 Alternativ
36 O
40 Visa specialfunktioner
41 A
42 Visa ikoner
43 I
44 Färg
46 C
47 Monokrom
48 M
49 Visa monokroma ikoner
50 M
53 Hjälp
54 H
60 Översikt
61 v
65 Innehållsförteckning
66 C
70 Steg-för-steg-anvisningar
71 T
75 Referens
76 R
80 Om hjälpfunktionen
81 U
85 Om Skapa funktion
86 A

$set 3
$ *****************************************************************************
$ **
$ ** Message set for  main Create Action window
$ **
$ *****************************************************************************
10 Skapa funktion
12 Funktionsnamn (ikonnamn):
13 Funktionsikoner:
20 Kommando när funktionsikonen dubbelklickas:
21 Kommando att exekvera vid dubbelklickning:
22 Kommando när funktionsikonen öppnas (dubbelklickas):
30 Hjälptext om funktionsikonen:
32 Fönstertyp:
34 Grafisk (X-fönster)
36 Terminal (Stäng automatiskt)
38 Terminal (Stäng manuellt)
40 Inget fönster
50 Special
52 Grundläggande

$set 4
$ *****************************************************************************
$ **
$ ** Message set for  main Create Action window advanced function area
$ **
$ *****************************************************************************
10 Specialfunktioner
11 Fråga användarna efter följande när funktionen öppnas:
12 Datatyper som använder funktionen:
13 Lägg till...
14 Ta bort
15 Redigera...
16 Släppbara datatyper:
17 Alla datatyper
18 Endast ovanstående lista

$set 5
$ *****************************************************************************
$ **
$ ** Error and Information message set for  main Create Action window
$ **
$ *****************************************************************************
10 Funktionsnamnet saknas.\n\
Skriv ett namn i fältet 'Funktionsnamn'.
$ 
15 Exekveringskommandot saknas.\n\
Skriv ett kommando i fältet\n\
'Kommando när funktionsikonen öppnas (dubbelklickas)'.
$ 
20 Listan 'Datatyper som använder funktionen' är tom.\n\
Du har markerat 'Endast ovanstående lista' i\n\
fältet 'Släppbara datatyper'.\n\
Ändra alternativet till 'Alla datatyper'\n\
eller lägg till minst en datatyp i listan.
$ 
25 Det finns redan en definitionsfil för funktioner och\n\
datatyper för detta funktionsnamn.\n\
Du kan avbryta Spara och ändra funktionsnamnet\n\
eller skriva över den befintliga definitionsfilen.\n\
Vill du skriva över den befintliga filen?
$ 
30 Det saknas ett variabelnamn i fältet\n\
'Kommando när funktionsikonen öppnas (dubbelklickas)'.\n\
Lägg till ett variabelnamn för det namn du har angivit\n\
i fältet 'Fråga användarna efter följande när funktionen öppnas'.\n\
Giltiga variabelnamn är $*, $1, $2, ..., $9.
$ 
35 Dessa ändringar har inte sparats.\n\
Vill du spara ändringarna innan du avslutar?
$ 
40 De aktuella ändringarna har inte sparats.\n\
Vill du spara funktionen innan du tar bort\n\
fälten och anger en ny funktion?
$ 
45 De aktuella ändringarna har inte sparats.\n\
Vill du spara ändringarna innan du tar bort\n\
fälten för att ladda en annan definitionsfil för funktioner?
$ 
50 Funktionsnamnet innehåller ett eller flera\n\
ogiltiga tecken.\n\
Följande tecken får inte användas i namn:\n\
  / \ " ' ;  (  ) ~ ! $ { } < > # @ | &
$ 
55 Det gick inte att skapa följande fil:
$ 
60 Det gick inte att öppna följande fil:
$ 
65 Det gick inte att ladda följande fil eftersom den\n\
inte har skapats av verktyget Skapa funktion:
$ 
66 Det gick inte att ladda filen eftersom den har ändrats\n\
på något annat sätt än med Skapa funktion. Filen kan ha\n\
ändrats med en textredigerare eller något annat verktyg.
$ 
67 Skapa funktion kan bara redigera filer som har skapats\n\
och redigerats med Skapa funktion.
$ 
68 Det gick inte att ladda följande fil:
$ 
$  The following messages are all part of the same message dialog
70 Den nya funktionen
71 har placerats i hemmappen.
73 Den nya definitionsfilen för funktioner heter:
$ 
80 Det gick inte att hitta alla storlekar av ikonen\n\
som du har markerat.
$ 
85 Det gick inte att starta ikonredigeraren.
$ 
90 Det gick inte att analysera följande fil:
$ 
95 Det gick inte att hitta följande ikonfil i ikonsökvägen:\n\
$ 
98 Ikonen som du har markerat finns inte i ikonsökvägen.\n\
Du måste flytta ikonen till en katalog i ikonsökvägen\n\
så att den kan visas på rätt sätt på skrivbordet.\n\
Välj 'OK' om du vill använda ikonen.\n\
Välj 'Avbryt' om du vill markera en annan ikon.
$ 
$ ONLY TRANSLATE  "Usage"  and  "filename"
110 Syntax: dtcreate [<filnamn>] [-?]
$ 
120 Du har inte markerat någon datatyp\n\
i listan 'Datatyper som använder funktionen'.
125 Markera den datatyp som du vill ta bort.
130 Markera den datatyp som du vill redigera.
$ 
150 Det aktuella arbetet har inte sparats.
152 Spara inte
154 Avbryt Stäng
$ 
160 Det går inte att spara sessionen i filen:
162 Det går inte att återställa sessionen från filen:
$ 
$ TRANSLATORS: DO NOT TRANSLATE the word "HOME" in the following message
170 HOME miljövariabeln är inte angiven.\n\
HOME miljövariabel måste anges för att den\n\
här funktionen skall sparas.


$set 6
$ *****************************************************************************
$ **
$ ** General message set for dialogs
$ **
$ *****************************************************************************
10 OK
11 Tillämpa
12 Avbryt
13 Hjälp
14 Ta bort
18 Sök uppsättning
19 Sök uppsättning...
20 Redigera ikon...
31 Skapa funktion - Fel
41 Skapa funktion - Bekräftelse
46 Skapa funktion - Information
51 Skapa funktion - Fråga

$set 7
$ *****************************************************************************
$ **
$ ** Message set for Add Datatype and Edit Datatype dialogs
$ **
$ *****************************************************************************
10 Lägg till datatyp
11 Redigera datatyp
14 Namn på datatypsfamilj:
15 Särskilda egenskaper:
16 Redigera...
17 Hjälptext om datatypsikonen:
18 Datatypsikoner:
21 Kommandon
22 Kommando för att öppna denna datatyp:
23 Kommando för att skriva ut denna datatyp:
30 Öppna
31 Skriv ut

$set 8
$ *****************************************************************************
$ **
$ ** Error and Information message set for
$ **    Add Datatype and Edit Datatype dialogs
$ **
$ *****************************************************************************
10 Namnet på datatypsfamiljen saknas.\n\
Skriv ett namn i fältet 'Namn på datatypsfamilj'.
$ 
15 De särskilda egenskaperna saknas.\n\
Du måste ange egenskaperna innan datatypen kan skapas.\n\
Du anger egenskaperna genom att välja knappen Redigera\n\
bredvid listan Särskilda egenskaper.
$ 
20 Fältet 'Namn på datatypsfamilj' innehåller ett eller flera\n\
ogiltiga tecken.\n\
Följande tecken får inte användas i namn:\n\
  / \ " ' ;  (  ) ~ ! $ { } < > # @ | &
$ 
25 Det finns redan en datatypsfamilj med detta namn.\n\
Ändra namnet i fältet 'Namn på datatypsfamilj'.\n\

$set 9
$ *****************************************************************************
$ **
$ ** Message set for  Identifying Characteristics dialog
$ **
$ *****************************************************************************
10 Särskilda egenskaper
11 Inkludera alla
12 Filer
13 Mappar
14 som matchar följande:
20 Namnmönster:
21 Behörighetsmönster:
22 Kan läsas
24 Kan ändras
25 Kan köras
27 OCH
28 Innehåll:
29 Mönster:
30 Typ:
31 String
32 Byte (Hex)
33 Kort (Decimal)
34 Lång (Decimal)
35 Start-byte:
40 Läs:
41 Skriv:
42 Exekvera:
43 På
44 Av
45 Endera

$set 10
$ *****************************************************************************
$ **
$ ** Error and Information message set for Identifying Characteristics dialog
$ **
$ *****************************************************************************
10 Namnmönstret saknas.\n\
Om du har markerat Namnmönster måste du ange ett namnmönster.\n\
$ 
15 Alternativet Behörighet saknas.\n\
Om du har markerat Behörighetsmönster måste\n\
du markera ett eller flera av alternativen\n\
under Behörighetsmönster.
$ 
20 Innehållsmönstret saknas.\n\
Om du har markerat Innehåll måste du\n\
fylla i fältet Mönster under Innehåll.
$ 
25 Värdet på start-byte saknas.\n\
Om du har markerat Innehåll måste du\n\
ange ett värde i fältet 'Start-byte'.
$ 
30 Du har inte markerat någon egenskap.\n\
Markera en eller flera av egenskaperna\n\
Namnmönster, Behörighetsmönster eller Innehåll.
$ 
45 Namnmönstret innehåller ett eller flera\n\
ogiltiga tecken.\n\
Följande tecken får inte användas\n\
i namnmönster:\n\
  / \ " ' ;  (  ) ~ ! $ { } < > # @ | &
$ 
46 Innehållsmönstret matchar inte formatet på\n\
den innehållstyp som du har markerat.\n\
Ändra formatet på innehållsmönstret eller\n\
ändra typen så att den matchar innehållsmönstret.
$ 
47 Fältet 'Start-byte' innehåller ogiltiga tecken.\n\
Detta fält får bara innehålla tal i decimalform.
$ 
48 När du anger innehållsmönstret kan det hända att\n\
filhanteraren blir betydligt långsammare.\n\
Filhanteraren måste nämligen söka igenom innehållet\n\
i alla filerna i en mapp innan den kan öppna\n\
mappen. Detta innebär att det tar längre tid att\n\
öppna mapparna.\n\
Av denna anledning rekommenderas du att endast använda\n\
namn- och behörighetsmönster.

$set 11
$ *****************************************************************************
$ **
$ ** Message set for  Icon Selection Dialog
$ **
$ *****************************************************************************
10 Kataloger
11 Ikonfiler
12 Ange ikonfilnamn
20 Laddar ikoner. Vänta...
30 Ikonmappar
50 Det angivna ikonfilnamnet är ogiltigt.\n\
Skriv ett giltigt ikonfilnamn i fältet\n\
'Ange ikonfilnamn'.
$ ONLY TRANSLATE  "Empty".   Do not translate the brackets "[" and "]".
60 [Tom]

$set 12
$ *****************************************************************************
$ **
$ ** Message set for  Open File Dialog
$ **
$ *****************************************************************************
10 Filter
11 Kataloger
12 Filer
13 Markering
20 Öppna fil
25 Skapa funktion - Öppna
