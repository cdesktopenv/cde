$ $XConsortium: dthelpprint.msg /main/3 1995/11/08 13:33:52 rswiston $
$ *************************************<+>*************************************
$ *****************************************************************************
$ **
$ **  File:        dthelpprint.msg
$ **
$ **  Project:     Cde1 Help
$ **
$ **  Description:
$ **  -----------
$ **  This file is the source for the message catalog for dthelpprint
$ **
$ **
$ *****************************************************************************
$ **
$ ** (c) Copyright 1993, 1994 Hewlett-Packard Company
$ ** (c) Copyright 1993, 1994 International Business Machines Corp.
$ ** (c) Copyright 1993, 1994 Sun Microsystems, Inc.
$ ** (c) Copyright 1993, 1994 Unix System Labs, Inc.,a subsidiary of Novell,Inc.
$ **         All Rights reserved
$ **
$ **
$ **     There are two types of messages in this file:
$ **
$ **     1) Messages that appear in printed output
$ **        These messages are the default and they should all be localized.
$ **        These messages are marked with LOCALIZE THESE MESSAGES.
$ **
$ **     2) Messages that should not be localized.
$ **        These messages are marked with DO NOT LOCALIZE THESE MESSAGES.
$ **
$ *****************************************************************************
$ **
$ **
$ **  ------------------------- MODIFICATION RECORD --------------------------
$ *   Major Mods, 7/11/94 for dthelpprint
$ *
$ **  ----------------------- MODIFICATION RECORD END ------------------------
$ *****************************************************************************
$ *************************************<+>*************************************

$ Use the double quote char around all messages
$quote "

$set 2
$ *****  Module: PrintUtil.c *****
$ 

$set 3
$ *****  Module: PrintTopics.c *****
$ 
$ ** LOCALIZE THESE MESSAGES **
1 "%s Fel: Du har angivit ett ämne, men inte i vilken volym du vill söka.\n"
2 "%s Fel: Det går inte att hitta volymen %s\n"
3 "%s Fel: Det har uppstått problem under bearbetningen av volymen %s\n"
4 "%s Fel: Minnestilldelningen misslyckades\n"
5 "%s Fel: Det går inte att få information om ämnet:\nvolym %s, plats %s\n"
6 "%s Fel: Det går inte att öppna den temporära filen %s\n"
$ String used to replace $SECTNUM when printing the index section
10 "Sakregister"
$ String used to replace $SECTNUM when printing the table of contents section
11 "Innehållsförteckning"
$ ** DO NOT LOCALIZE THESE MESSAGES **
$ index entry, page number
20 "%s, %d\n"
$ index subentry, page number
21 "     %s, %d\n"
$ location ID associated with the index in the TOC maintained by dthelpprint
30 "__GENERATED-INDEX"
$ string used by can't get a topic title associated with an index subentry
31 ""

$set 4
$ *****  Module: PrintManStrFile.c *****
$ 
$ ** LOCALIZE THESE MESSAGES **
1 "%s Fel: Du söker efter en söksträng, men du har inte angivt någon.\n"
2 "%s Fel: Du söker efter en dynamisk sträng, men du har inte angivt någon.\n"
3 "%s Fel: Du söker efter en kommandobeskrivning, men du har inte angivt vilken.\n"
4 "%s Fel: Du söker efter en fil, men du har inte angivt vilken.\n"
5 "%s Fel: Minnestilldelningen misslyckades\n"

$set 5
$ *****  Module: Initialize.c *****
$ 
$ ** LOCALIZE THESE MESSAGES **
$ Messages 1 to 29: dthelpprint usage message
$ **DO NOT LOCALIZE** the command line option names (e.g. -copies).
$ **DO LOCALIZE** the option argument (e.g. number) and description.
1 "dthelpprint - Print program for Help\n\n"
2 "Syntax: dthelpprint [tillägg]\n"
3 "Tillägg som styr utskriften:\n"
4 "\t-printer skrivarnamn     skrivaren som ska användas\n"
5 "\t-copies antal            antalet kopior som ska skrivas ut\n"
6 "\t-outputFile filnamn      skriv till den här filen\n"
7 "\t-paperSize pappersformat formatera utskriften efter pappersformatet\n"
$ DO NOT LOCALIZE THE PAPER SIZE NAMES help_papersize_xxx
8 "\t\tpappersformat = { help_papersize_letter|help_papersize_legal|\n"
9 "\t\t         help_papersize_executive|help_papersize_a4|help_papersize_b5}\n"
10 "\t-display visningsenhet  enhet som tillhandahåller resurser\n"
11 "\t-name                   program som används för att få resurser\n"
12 "\t-class                  klassnamn som används för att få resurser\n"
13 "\t-xrm resurssträng       ytterliga resurser\n"
14 "Tillägg som styr vad som ska skrivas ut:\n"
15 "\t-helpType typ           typ av hjälpinformation\n"
16 "\t\ttype = 0 (volym), 1 (söksträng), 2 (kommandobeskrivning), 3 (hjälpfil)\n"
17 "\t-helpVolume volym       hela sökvägen till volymfilen\n"
18 "\t-locationId plats       namn på ämnet i volymen\n"
19 "\t-all                  skriv ut alla ämnen, i.fört. & sakreg. i volymen\n"
20 "\t-sub                  skriv ut ämnet locationId och alla underämnen\n"
21 "\t-one                  skriv ut ämnet locationId\n"
22 "\t-toc                  skriv ut volymens innehållsförteckning\n"
23 "\t-index                skriv ut volymens sakregister\n"
24 "\t-frontMatter          skriv ut volymens omslag\n"
25 "\t-manPage mansidnamn   kommandobeskrivningens namn\n"
26 "\t-stringData sträng    texts som ska skrivas ut\n"
27 "\t-helpFile filnamn     fil som innehåller hjälptext\n"
28 "\t-jobTitle titel       titel på utskriften\n"
29 "\t-topicTitle titel     titel på hjälptexten\n"
$ Warning and error messages
$ Do NOT localize the default size name help_papersize_letter
40 "%s Varning: Pappersformatet '%s' är felaktigt.  help_papersize_letter används.\n"
41 "%s Varning: Pappersformat-, höjd- eller breddvärde saknas.  help_papersize_letter används.\n"
42 "%s Varning: Det går inte att öppna visningsenheten %s\n"
45 "Fel: Det går inte att tilldela minne för den temporära filen\n"
$ ** DO NOT LOCALIZE THESE MESSAGES **
$ arg order: directory prefix processid filecnt suffix
50 "%1$s/%2$s%3$d_%4$d%5$s"


$set 6
$ *****  Module: Main.c *****
$ 
$ ** LOCALIZE THESE MESSAGES **
1 "%s Fel: Felaktig hjälp-typ %d.\n"



