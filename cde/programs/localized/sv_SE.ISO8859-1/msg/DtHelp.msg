$ *************************************<+>*************************************
$ *****************************************************************************
$ **
$ **  File:        dthelp.msg
$ **
$ **  Project:     CDE 1.0 Help
$ **
$ **  Description:
$ **  -----------
$ **  This file is the source for the message catalog for DtHelp 
$ **
$ **
$ *****************************************************************************
$ **
$ **     (c) Copyright 1990 Hewlett-Packard Company
$ **         All Rights reserved
$ **
$ **
$ **     There are two types of messages in this file:
$ **
$ **     1) Messages that appear in dialogs or are displayed to the user.
$ **        These messages are the default and they should ALL BE LOCALIZED.
$ **        Note that these messages do NOT have any identification.
$ **
$ **     2) Messages that should not be localized.
$ **        These messages are marked with DO NOT TRANSLATE THESE MESSAGES.
$ **
$ *****************************************************************************
$ **
$ **
$ **  ------------------------- MODIFICATION RECORD --------------------------
$ *   Major Mods, 3/26/92 in preperation for the NLS package release on the 
$ *               27th.  (Mike Wilson).
$ *
$ *   Major Mods, 6/10/94 in prep for Cde Message Catalog Freeze
$ *
$ **  ----------------------- MODIFICATION RECORD END ------------------------
$ *****************************************************************************
$ *************************************<+>*************************************

$set 2
$ *****  Module: HelpDialog.c  and a few from HelpQuickD.c *****
$ 
$ These Messages appear in the pulldown menus for Cde Help General Help
$ dialog widget.
$ 
$ They come in pairs where the first is the menu name (e.g. 1 Print...) and the
$ second is the mnemonic (e.g. 2 P).  
$ 
$ Messages 1 through 6 are the pulldown menus for the File menu.
1 Skriv ut...
2 u
3 Stäng
4 S
5 Nytt fönster...
6 f
$ Messages 7 through 8 are the pulldown menus for the Edit menu.
7 Kopiera
8 K 
$ Messages 9 through 10 are the pulldown menus for the Search menu.
9 Index...
10 I
$ Messages 11 through 16 are the pulldown menus for the Navigate menu.
11 Startavsnitt
12 t
13 Spåra bakåt
14 b
15 Historik...
16 H
$ Messages 17 through 28 are the pulldown menus for the Help menu.
17 Översikt - Använda Hjälp
18 v
19 Innehållsförteckning
20 h
21 Steg-för-steg-anvisningar
22 a
23 Referens
24 R
25 Mus och tangentbord
26 M
27 Om hjälpfunktionen
28 m
$ 
$ Messages 29 through 36 are the menu labels and mnemonics for for the 
$ Help Dialog.
29 Fil
30 F
31 Sök
32 S
33 Förflytting
34 F
35 Hjälp
36 H
37 Redigera
38 R
$ 
$ 
$ >>>>> Skip #39 through #49
$ 
$ Messages 50 through 55 are used to display formatting errors within the
$ help dialog and quick help dialog widgets.
$ 
50 Strängen kunde inte formateras.
51 Den dynamiska strängen kunde inte formateras.
52 Kommandobeskrivningen kunde inte formateras. Beskrivningen finns inte eller\
så är den skadad.
53 Textfilen kunde inte formateras. Filen finns inte eller så är den\
skadad.
54 Avsnittet kunde inte formateras.
55 Plats-ID finns inte:
$ 
$ Messages 56 and 67 are displayed when a hypertext link type is selected but
$ not supported within the current application.
$ 
56 Hypertextlänken fungerar inte i den tillämpningen.
57 Länkar till kommandobeskrivningarna fungerar inte i den här tillämpningen.
$ 
$ Messages 58 through 62  are used when bad or null help volumes or 
$ locationId's are used.
$ 
58 Hjälpen är inte installerad eller inte installerad på rätt sökväg. I dokumentationen som hör till produkten kan du hitta information om hur hjälpen ska installeras.\n
59 Du har inte angivit något plats-ID.
60 Du har inte angivit någon volym.
61 Volym:
62 Plats-ID:
$ 
$ >>>>> Skip #63 through #69
$ 
$ 
$ Message 70 is used when a null value for DtNtopicTitle is used.
$ 
70 Avsnittet finns inte.
$ 
$ New messages added after early msg cat freeze; not in order
80 Angiven hjälp typ saknas.


$set 3
$ ****  Module: History.c ****
$ These messages are used as labels for the History Dialog.
1 Hjälp - Historik
2 Använda volymer:
3 Stäng
4 Hjälp
5 Hjälpfiler
6 Kommandobeskrivningar
7 Hjälpmeddelanden
8 Använda avsnitt:
9 Använda hjälpfiler:
10 Använda kommandobeskrivningar:
11 Använda hjälpmeddelanden:


$set 4
$ *****  Module: Print.c  *****
$ These messages are used as labels for the Print Dialog.
$ 1: dialog title
1 Hjälp - Skriv ut
$ 10-12: standard printer dialog labels
10 Skrivare:
11 Kopior:
12 Titel på försättsblad:
$ 20-25: paper size label and sizes
20 Pappersformat:
21 Letter 8,5 x 11 tum
22 Legal 8,5 x 14 tum
23 Executive 7,25 x 10,5 tum
24 A4 210 x 297 mm
25 B5 182 x 257 mm
$ 30-33: help volume-specific printing options
30 Avsnitt att skriva ut
31 Aktuellt avsnitt
32 Aktuellt och underavsnitt
33 Innehållsförteckning och hjälpindex
34 Alla avsnitt i volymen

$ 40-42: buttons at bottom of dialog
40 Skriv ut
41 Avbryt
42 Hjälp

$ These messages are used to describe the kind of content to be printed
$ 50 "Help Volume: "
50 Volym: 
51 Hjälpmeddelanden
52 Hjälpfil

$ 53 "Manual Page: "
53 Kommandobeskrivning: 
54 Felmeddelande

$ Don't translate message 60 unless it is not an empty string
$ This is the default value that goes in the Printer field
$quote "
60 ""
$quote

$ Don't translate message 61
$ This is the default value that goes in the Copies field
61 1


$set 5 
$ *****  Module: GlobSearch.c  *****
$ Message 1 is the label used to label the dialog
1 Hjälp - Sök i hjälpindex
2 Sök
3 Aktuell 
$ DO NOT TRANSLATE MESSAGE 4; it is no longer used
$ " (%s)"
4  (%s)
5 Alla volymer
6 Valda
7 Valda volymer...
8 Visa
9 Fullständigt hjälpindex
10 Poster med:
11 Gå till
12 Börja söka
13 Fortsätt söka
14 Sluta söka
15 Stäng
16 Hjälp
17 Söker efter volymer... %d funna
18 Söker... %d volymer återstår
19 Inga sakregisterposter funna
20 Fullständigt hjälpindex
21 Poster med "%s"
22 Inga volymer är valda
23 Välj alternativ för sökningen ovan
$ Do NOT translate messages 25-31
$quote "
$ " "
25 " "
26 "+"
27 "-"
$ "  +"
28 "  +"
$ "  -"
29 "  -"
$ 30 "       "
30 "       "
$ 31 "         "
31 "         "
$quote
$ DO translate messages 32 and above
$ kind of topic messages
32 Hjälpmeddelande
33 Hjälpfil
34 Kommandobeskrivning
35 Okänt format
$ specialized title of Search Volume Selection dialog
40 Hjälp - Välj volymer för sökning
$ additional messages (late addition caused this ordering)
50 Inga sakregisterposter funna. Försök med Alla volymer.
51 Inga sakregisterposter funna. Försök med ett annat ord.
$ additional messages (later addition caused this ordering)
$ These messages are the braces that go around the current volume name
$ in the index search dialog.  They are kept in the message catalog
$ in case they need to be localized.
60 (
61 )


$set 6
$ *****  Module: HelpAccess.c  *****
$ These messages are used for the Help on Help dialog in Cde Help widgets.
1 Hjälp om Hjälp 
2 Skriv ut ...
3 Stäng
4 Hjälp om Hjälp finns inte i den här tillämpningen.
5 Spåra bakåt
6 Hjälp ...


$set 7
$ *****  Module: Main.c & Util.c & (dthelpview CDE 1.0  *****
$ These messages are used by the dthelpview program.  They need to be 
$ localized for dthelpview, however, they are not directly used by libDtHelp
$ Messages 1 through 7 are command line usage response messages.
$ Do NOT translate the option name (e.g. -helpVolume).  DO translate
$ the argument descriptor name (e.g. <Help Volume File>)
1 Syntax: %s...\n
2 \t-helpVolume <hjälpvolym>\n
3 \t-locationId  <ID>\n
5 \t-file  <ASCII-textfil>\n
6 \t-man\n
7 \t-manPage <kommandobeskrivning i Unix>\n\n\n
$ man page dialog
11 Kommandobeskrivning
12 Kommandobeskrivning:
13 Visa kommandobeskrivning
$ button labels
16 Avsluta
17 Stäng
18 Spåra bakåt
19 Skriv ut ...
$ title of helpview window
20 Hjälp


$set 8
$ *****  Module: HelpUtil.c  *****
$ Message 1 is the label used in the definition link dialog box 
$ (e.g. Quick help close button)
2 Stäng
3 Internt fel: Det gick inte att få tag i musen\nDtHelpReturnSelectedWidget avslutad.\n
4 Internt fel: Det gick inte att få tag i tangentbordet\nDtHelpReturnSelectedWidget() avslutad.\n
5 Fel i Hjälp 
$ Messages 10 and up were added after message catalogs were taken
$ for translation.  They are not in any order or chosen location.
10 Kör ändå
11 Kör ej
12 Hjälpvolymen vill köra ett kommandoalias.\n'%s' är inte definierat som ett alias.
13 Hjälpvolymen vill köra ett kommando.\nAv säkerhetsskäl stoppas den automatiska kommandokörningen.\nKommandot är:  %s
$ End Message set 8


$set 9
$ *****  Module: PathArea.c  *****
$ These messages are used in the general Help Dialog Widget.
$ Message 1 is the label used to label Topic Tree Area  
1 Volym: 
$ Messages 2 though 5 are used in the Button Box area of our general 
$ help dialog.
2 Spåra bakåt
3 Historik...
4 Index...
5 Översta nivån


$set 10 
$ *****  Module: VolSelect.c  *****
$ Message 1 is the default label used to label the volume selection dialog.
1 Hjälp - Val av volym
2 Volymer
3 Välj en eller flera volymer
4 Stäng
5 Hjälp


$set 11
$ *****  Module: HelpQuickD.c  *****
$ These messages are used as labels for the quick help dialog.
1 Stäng
2 Mer ...
3 Spåra bakåt
4 Skriv ut ...
5 Hjälp ...

$set 99
$    DO NOT TRANSLATE THESE MESSAGES
$    DO NOT CHANGE THESE MESSAGES
$    DO NOT LOCALIZE THESE MESSAGES
$    These messages are used for the version information.
$quote "
1 "@(#)version_goes_here"
2 "\n@(#)Message catalog source $XConsortium: DtHelp.msg /main/3 1995/11/08 13:30:37 rswiston $"

