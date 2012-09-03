 
$ "$XConsortium: dtstyle.msg /main/4 1995/12/08 09:39:47 rswiston $"

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

$set 2
$ Message 1 from set 2 was deleted from the message catalog

$ _DtMessage 2 is the title of all Information dialog boxes
2 Meddelande
$ _DtMessage 3 is the title of all Error dialog boxes
3 Fel
$ _DtMessage 4 WILL ONLY APPEAR IN THE DT ERRORLOG FILE
4 Cannot communicate with the session\n\
manager... Exiting.
 
$  MESSAGES 5-6 IN SET 2 WILL ONLY APPEAR IN THE DT ERRORLOG FILE
5 Style Manager is already running,\n\
second attempt aborted.
6 An X Toolkit error occurred...\n\
Exiting.

$ _DtMessage 7 is the title of all Help dialog boxes
7 Arbetsinställningar - Hjälp

$ _DtMessage 8 is an error message that appears when you are using Itemh Help
$ and select on something outside of the Style Manager
8 Du måste markera ett objekt\n\
i Arbetsinställningar.

$ _DtMessage 9 WILL ONLY APPEAR IN THE DT ERRORLOG FILE
9 Could not obtain screen saver information\n\
from the session manager. Start up settings\n\
may be incorrect.


$set 4
$ _DtMessage set 4 is for the Style Manager main window
$ Messages 6,8-26,34,39 from set 4 were deleted from the message catalog

$ _DtMessage 1 is the title of the main dtstyle window
1 Arbetsinställningar

$ mnemonic and label for the File menubar item.
2 F
3 Fil
$ mnemonic and label for the Help menubar item.
4 H
5 Hjälp

$ _DtMessage 7 is the label for the Exit button in the File pulldown
$ the mnenomic for the Exit button is set 4 _DtMessage 40
7 Avsluta

$ Messages 27-33,35  are the labels on the buttons in the dtstyle main window.
27 Färg
28 Font
29 Bakgrund
30 Tangentbord
31 Mus
32 Ljudsignal
33 Skärm
35 Startinställningar

$  MESSAGE 36 IN SET 4 WILL ONLY APPEAR IN THE DT ERRORLOG FILE
36 No Dtstyle restore information available for current session

$ _DtMessage 37 is the label on the dtstyle icon
37 Arbetsinställningar 

$ Message 38 is the label on the Window button in the dtstyle main window.
38 Fönster

$ _DtMessage 40 is replaced with message 53
$ 40 E

$ *
$ * mnemonic and label for the Help menu items.
$ * 

$ Messages 41-42 have been replaced with messages 54-55
$ Overview... menu item
$ 41 I
$ 42 Introduktion...

$ Tasks... menu item 
43 t
$ Message 44 has been replaced with messages 56
$ 44 Steg-för-steg-anvisningar...

$ Reference... menu item
$ 45 R
$ Message 46 has been replaced with messages 57
$ 46 Referens...

$ On Item menu item
$ Keybaord menu item
47 O
48 Objekthjälp

$ Using Help... menu item
49 j
$ Message 50 has been replaced with messages 58
$ 50 Om hjälpfunktionen... 

$ Message 50 has been replaced with messages 58
$ Version... menu item
$ 51 V
$ 52 Version...

$ Message 53 is replaced with message 40
53 x

$ Messages 54-55 are replacements for messages 41-42
54 v
55 Översikt

$ Message 56 is a replacement for messages 44
56 Steg-för-steg-anvisningar...

$ Message 57 is a replacement for messages 46
57 Referens

$ Message 58 is a replacement for messages 50
58 Om hjälpfunktionen

$ Messages 59-60 are replacements for messages 51-52
59 A
60 Om Arbetsinställningar

$set 5
$ _DtMessage set 5 is for the Font dialog
$ Messages 1-6, 7-16 from set 5 were deleted from the message catalog

$ _DtMessage 17 is the label of the titlebox of the preview font area
17 Förgranskning

$ _DtMessage 18 appears in a Notice dialog after the user selects a new font
$ and presses Ok - when the resource *WriteXrdbImmediate is True
18 Fontändringarna kommer att synas i vissa\n\
program nästa gång de startas.\n\
I andra program, som Filhanteraren och\n\
Programhanteraren, syns inte fontändringarna\n\
förrän du avslutar skrivbordet och sedan loggar in igen.

$ _DtMessage 19 appears in a Notice dialog after the user selects a new font
$ and presses Ok - when the resource *WriteXrdbImmediate is False
19 Den markerade fonten kommer att användas\n\
när du startar om denna session.

$ _DtMessage 20 and 21 is the text that initially appears in the Font Preview 
$ area displaying sample user and system fonts.
20 AaBbCcDdEeFfGg0123456789
21 AaBbCcDdEeFfGg0123456789

$ _DtMessage 22 is the label of the titlebox containing the list of font sizes
22 Storlek

$ _DtMessage 23 is the title of the Fonts dialog
23 Arbetsinställningar - Font

$ _DtMessage 24 is an informative message used by the Fonts dialog 

24 Den font som anvM-dnds pM-e skrivbordet finns inte tillgM-dnglig\n\
i listan Storlek. Om du vM-dljer en ny font och tillM-dmpar den,\n\
kan du sedan inte M-etergM-e till den aktuella fonten med\n\
dialogrutan ArbetsinstM-dllningar - Font.


$set 6
$ _DtMessage set 6 is for the Beep dialog
$ _DtMessage 1,6 from set 6 have been deleted from the message catalog

$ Messages 2 through 5 are the default buttons and labels of the scales.  
2 Standard
3 Volym
4 Ton
5 Längd

$ _DtMessage 7 is the title of the Beep dialog box
7 Arbetsinställningar - Ljudsignal  


$set 7
$ _DtMessage set 7 is for the Startup dialog
$ Messages 1-2 from set 7 have been deleted from the message catalog

$ Messages 3 through 9 are labels of the buttons and text visible in the 
$ Session Startup dialog box. 
3 Vid inloggning:
4 Fortsätt aktiv session
5 Återgå till hemsession
6 Fråga vid utloggning
7 Bekräftelse av utloggning:
8 På
9 Av
10 Spara inställningar för hemsession ...

$ _DtMessage 11 is a warning message when the user pushes Set Home Session...
$ Notice the spaces before Continue .. the spaces make it centered.
11 Detta ersätter hemsessionen med\n\
den aktiva sessionen.\n\n \
                 Vill du fortsätta?

$ _DtMessage 12 is the title of the warning dialog box when Set Home Session is 
$ pushed. 
12 Varning

$ _DtMessage 13 is an error message
13 Denna session startades från ett startskript för X Window System.\n\
Startinställningarna är inte giltiga,\n\
eftersom sessionen inte kan återställas.

$ _DtMessage 14 is the title of the Session Startup dialog box.
14 Arbetsinställningar - Startinställningar

$ _DtMessage 15 is an error message
15 Sessionshanteraren körs inte.\n\
Startinställningarna är inte giltiga.


$set 8
$ All of the messages from set 8 have been deleted from the message catalog


$set 9
$ _DtMessage set 9 is for the Mouse dialog
$ Message 7 from set 9 have been deleted from the message catalog

$ Messages 1-4 are labels of toggle buttons in the Mouse Dialog. 
1 Vänster
2 Höger
3 Höger-/vänsterhänt:
4 Knapp 2:
$ _DtMessage 5 is the label of default button.
5 Standard

$ Messages 6,8,9 are labels of the scales.
6 Dubbelklickning
8 Acceleration
9 Tröskelvärde

$  MESSAGE 10 IN SET 9 WILL ONLY APPEAR IN THE DT ERRORLOG FILE
10 ERROR: setting pointer mapping\n

11 Överföring
12 Ändra

$ _DtMessage 13 appears in a Notice dialog after the user selects the 
$ left-handed or right-handed toggle
13 Funktionerna i vänster och höger musknapp kommer att byta\n\
plats omedelbart efter att du väljer OK.\n\
I bilden `Testa dubbelklickning' i musfönstret\n\
visar placeringen av knapp 1 och 3 de aktuella\n\
inställningarna för vänster- /högerhänthet.\n\

$ _DtMessage 14 appears in a Notice dialog after the user selects the 
$ transfer toggle to change the function of the middle mouse button.
14 Om du väljer OK kommer musknapp 2 att ställas in för\n\
`överföring' (dvs att dra med musen) vid nästa session.

$ double-click time and presses Ok (and the resource 
$ *WriteXrdbImmediate is True)
15 Den nya dubbelklickningstiden kommer att börja\n\
gälla när programmen startas om.

$ _DtMessage 16 appears in a Notice dialog after the user selects a new 
$ double-click time and presses Ok (and the resource 
$ *WriteXrdbImmediate is False)
16 Den nya dubbelklickningstiden kommer att börja\n\
gälla nästa session.

$ _DtMessage 17 is the text below the mouse icon. Notice the two lines.
17 Testa\ndubbelklickning

$ _DtMessage 18 is the Mouse Dialog title
18 Arbetsinställningar - Mus

$ _DtMessage 19 appears in a Notice dialog after the user selects the 
$ adjust toggle to change the function of the middle mouse button.
19 Om du väljer OK kommer musknapp 2 att ställas in för\n\
`ändra' vid nästa session. Du kan då använda\n\
knapp 2 för att ändra markeringar i text och listor.\n\
Du måste använda knapp 1 för alla överföringshandlingar\n\
(dvs att dra med musen) på text och listor.

$set 10
$ _DtMessage set 10 is for the SaveRestore functionality
$ Messages 2,4 from set 10 have been deleted from the message catalog

$  MESSAGE 1,3 IN SET 10 WILL ONLY APPEAR IN THE DT ERRORLOG FILE
$ Messages 1,3 make up one error message.  Put together, they say 
$ "<filename> <_DtMessage 2> <_DtMessage 1>
1 Check file permissions.
3 %s is the file that would have been used to save your session\n


$set 11
$ _DtMessage set 11 is for the Backdrop dialog

$  MESSAGE 1 IN SET 11 WILL ONLY APPEAR IN THE DT ERRORLOG FILE

$ _DtMessage 1 - could not open a backdrop directory
1 Could not open directory %s. 
$ _DtMessage 2 - error opening the backdrop files
2 Bakgrunden i Arbetsinställningar kommer\n\
inte att fungera eftersom det inte finns några\n\
tillgängliga bakgrundsfiler. Kontrollera $HOME/.dt/errorlog.


$ _DtMessage 12 is the title of the Backdrop dialog box
12 Arbetsinställningar - Bakgrund


$set 12
$ _DtMessage set 12 is for the Screen dialog
$ Message 2 from set 12 has been deleted from the message catalog

$ _DtMessage 1 is the title of the Screen dialog box. 
1 Arbetsinställningar - Skärm

$ Messages 3-14 are the labels and buttons for the Screen dialog
3 Standard
4 Skärmsläckare
5 Tid innan skärmsläckaren startar
6 Visa skärmsläckare i
7 minuter
8 Skärmlås
$ Message 9 has been removed 
10 På
11 Av
12 Skärmsläckare:
13 Skärmlås:
14 Tid innan skärmen låses
15 Automatisk skärmsläckning
16 Skärmsläckning:
17 Tid före skärmsläckning
18 Frontpanellås
$ Message 19 has been replaced by message 21
19 Använd Släck skärmen
20 Visa bakgrund i:
$ Message 21 is a replacement for message 19
21 Använd bakgrunder vid skärmsläckning
22 Genomskinlig skärmsläckning

$set 13
$ _DtMessage set 13 is for the Keyboard dialog
$ _DtMessage 1,5 from set 13 have been deleted from the message catalog

$ _DtMessage 2 through 4 are labels of scales, buttons, and text visible in the
$ Keyboard dialog box.
2 Standard
3 Upprepa automatiskt
4 Tangentljudsvolym

$ _DtMessage 6 is the title of the Keyboard dialog box
6 Arbetsinställningar - Tangentbord


$set 14
$ _DtMessage set 14 is for the Color dialog
$ _DtMessage 1-4,9,14-15,17,19,20,22,24 from set 14 have been deleted from the message catalog

$ Messages 5 through 8 are labels and buttons visible in the Color dialog
5 Paletter
6 Lägg till...
7 Ta bort...
8 Ändra färg...

$ _DtMessage 10 is the title above the text area in the Add Palette dialog box
10 Nytt palettnamn:

$ _DtMessage 11 is the title of the Add Palette dialog box.
11 Lägg till palett

$ Messages 12 through 14 are error messages.
12 Palettnamnet får inte innehålla\n\
följande tecken:\n\n\
<mellanslag> * : ( ) [ ] { } < > ! | \" / \\
$ LOCALIZERS: FOR 8 AND 16 BIT LANGUAGES YOU MAY WANT TO CHANGE THE MESSAGE 
$ TO SAY THE PALETTE NAME MUST BE 10 BYTES OR LESS
13 Palettnamnet kan vara högst\n10 tecken långt.\n

$ _DtMessage 16 is the title of the Delete Palette dialog box.  See screen 4a (2).
16 Ta bort palett

$ _DtMessage 18 appears in an information dialog.
18 Det går inte att ta bort den sista paletten.\n

$ _DtMessage 21 is an error message
21 Varning

$ _DtMessage 23 appears in an information dialog
23 Vill du ta bort paletten '%s'?\n

$ _DtMessage 25 is an error message that appears when you press the color
$ button and the color server is not running for some reason.
25 Färgdelen i Arbetsinställningar\n\
kommer inte att fungera eftersom färghanteraren\n\
inte körs. Kontrollera $HOME/.dt/errorlog.

$ _DtMessage 26 can be caused by setting the resource *useColorObj to False
$ and then running dtstyle and trying to post the Color Dialog.
26 Färgdelen av Arbetsinställningar\n\
kommer inte att fungera eftersom resursen\n\
'useColorObj' har angivits till False.

$ _DtMessage 27 is for the new ColorUse dialog when you press ok.
27 Det nya värdet för antal färger kommer att börja gälla\n\
nästa session.

$ _DtMessage 28 is a warning message that appears on Black and White systems
28 Den markerade paletten kommer att börja gälla\n\
nästa session.

$ _DtMessage 29 appears in a warning dialog
29 Det finns redan en palett med namnet '%s'.\n\
Den nya paletten kommer att skriva över den gamla.\n\
Vill du verkligen skriva över den?

$ _DtMessage 30 is the title of the Color dialog box.
30 Arbetsinställningar - Färg

$ _DtMessage 31-33 are the ColorUse options
$ 31 Kraftig färg
$ 32 Medelkraftig färg
$ 33 Svag färg
34 Svartvit
35 Standard

$ Dt Message 36 has been replaced by message 39
$ _DtMessage 36 is the title of the Color Use dialog
$ 36 DT Ange antal färger

$ Dt Message 37 has been replaced by message 40
$ _DtMessage 37 is the Color Use button in the main Color Dialog 
$ 37 DT Ange antal färger...

$ _DtMessage 38 is an error message that appears when you press the color
$ button and there are no palette files to be found
38 Färgdelen i Arbetsinställningar\n\
kommer inte att fungera eftersom det inte finns\n\
några tillgängliga palettfiler. Kontrollera $HOME/.dt/errorlog.

$ DtMessage 39 is a replacement for DtMessage 36
$ DtMessage 39 is the title of the Color Use dialog
39  Ange antal färger:

$ DtMessage 40 is a replacement for DtMessage 37
$ DtMessage 40 is the title of the Color Use dialog
40 Ange antal färger...

$ _DtMessages 41-43 are replacements for Dtmessages 31-33
$ _DtMessage 41-43 are the ColorUse options
41 Fler färger för skrivbordet
42 Fler färger för program
43 Flest färger för program


$set 15
$ All of the messages from set 15 have been deleted from the message catalog


$set 16
$ _DtMessage set 16 is for the Color dialog
$ _DtMessage 1-10,13-17 from set 16 have been deleted from the message catalog

$ _DtMessage 11-12 form one error message that appears in an error dialog
11 Det går inte att ta bort '%s'.\n
12 Kontrollera $HOME/.dt/errorlog.

$  MESSAGE 18-21 IN SET 16 WILL ONLY APPEAR IN THE DT ERRORLOG FILE
18 Warning, Too many directories listed in the resource paletteDirectories,\n\
Maximum number is %d. 
$ _DtMessage 19 - could not open a palette directory
19 Could not open directory %s. 
$ _DtMessage 20 - error opening the palette file
20 Could not open %s. 
$ _DtMessage 21 - error reading a palette file
21 %s is an invalid palette file.


$set 17
$ _DtMessage set 17 is for the Modify Color dialog
$ Messages 1-3,13,16 from set 17 have been deleted from the message catalog

$ Messages 4 through 12 are labels, buttons, and text visible in the
$ Modify Color dialog box
4 Exempel
5 Gammal
6 Ny
7 Kopiera färg
8 Färgredigerare
9 Nyans
$ We recommend that you don't translate R,G,B which stands for Red, Green,
$ Blue unless there are standard single letter acronyms in your language.
10 R
11 G
12 B

$  MESSAGE 14-15 IN SET 17 WILL ONLY APPEAR IN THE DT ERRORLOG FILE
14 Warning, couldn't grab pointer.\n
15 Warning, couldn't grab Keyboard.\n

$ _DtMessage 16 is the title of the Modify Color dialog box.
16 Arbetsinställningar - Ändra färg


$set 18
$ _DtMessage set 18 is for the Window dialog
$ _DtMessage 1 is the title of the Window dialog box
1 Arbetsinställningar - Fönster

$ Messages 2 through 5 are labels on the Window dialog.
2 Standard
3 Fönsterinställningar
4 Fönsterikoner
$ Messages 5-8 have been replaced by messages 13-16 
$ 5 Fokus följer musen
$ 6 Aktivera fönstret med musen
$ 7 Fokuserat fönster överst
$ 8 Flytta fönster med innehåll
9 Använd ikonruta
10 Lägg på arbetsytan
11 Lägg huvudfönster överst
12 Arbetsytehanteraren måste startas om\n\
för att ändringarna ska träda i kraft.\n\
Välj OK för att starta om arbetsytehanteraren\n\
eller välj Avbryt för att återställa ändringarna.
$ Messages 13-16 are replacements for messages 5-8
13 Peka i fönster för att aktivera
14 Klicka i fönster för att aktivera
15 Aktiverat fönster överst
16 Flytta fönster med innehåll

$set 99
$    DO NOT TRANSLATE THESE MESSAGES
$    DO NOT CHANGE THESE MESSAGES
$    DO NOT LOCALIZE THESE MESSAGES
$    These messages are used for the version information.
1 @(#)version_goes_here
2 \n@(#)_DtMessage catalog source $XConsortium: dtstyle.msg /main/4 1995/12/08 09:39:47 rswiston $


