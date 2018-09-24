$ "$XConsortium: dthello.msg /main/3 1995/11/08 11:52:40 rswiston $";    
$ *************************************<+>*************************************    
$ *****************************************************************************    
$ **    
$ **  File:        dthello.msg    
$ **    
$ **  Project:     HP DT login transition program (dthello)     
$ **    
$ **  Description:    
$ **  -----------    
$ **  This file is the source for the message catalog for dthello    
$ **    
$ **    
$ *****************************************************************************    
$ **    
$ **     (c) Copyright 1990, 1991 Hewlett-Packard Company    
$ **     All Rights reserved    
$ **    
$ **    
$ *****************************************************************************    
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
$quote "

$set 2
$    THIS COMMENT FOR DT TEAM ONLY -- dthello.c --    
$    Do not translate. Blank message, please ignore    

1 ""
$    Message #2 is no longer used. It has been replaced with messag #3    

2 "Starten der\n\
COSE\n\
Benutzerumgebung\n\n"
$    Default startup message (written before the contents    
$    of the default file, /etc/copyright).    

3 "Starten des\n\
Common Desktop Environment\n\n"

$set 4        Redefine set#

$    THIS COMMENT FOR DT TEAM ONLY -- dthello.c --    
$    This entire set is for error messages    
$    DO NOT TRANSLATE THESE MESSAGES    

1 "%s: Anzeige kann nicht geöffnet werden\n"

2 "%s: Timeout-Wert muß positiv sein\n"

3 "%1$s: Maximum von %2$d Dateien erlaubt, %3$s nicht berücksichtigt\n"
$ Messages 4 and 5 are both used when there is a syntax error on    
$ the command line invoking dthello    
$     

4 "Verwendung: %s [-display <Display>] [-fg <Farbe>] [-bg <Farbe>]\\\n"

5 "[-font <Schriftart>] [-string <Meldung>] [-file <Name>] [-timeout <Sekunden>]\n"
$ _DtMessage 6 occurs when an font can't be loaded    
$  the likely causes: (1) the font was incorrectly specified    
$                     (2) it is not available on this server    

6 "%1$s: Bildschirm %2$s kennt Schriftart %3$s nicht\n"

7 "%1$s: Datei %2$s kann nicht geöffnet werden\n"

8 "%1$s: Hintergrund kann nicht auf %2$s gesetzt werden, %3$s verwendet.\n"

9 "%1$s: Vordergrund kann nicht auf %2$s gesetzt werden, %3$s verwendet.\n"

10 "%s: Nicht genügend Speicher (SeparateTextLines)\n"

$set 99        Redefine set#

$    DO NOT TRANSLATE THESE MESSAGES    
$    DO NOT CHANGE THESE MESSAGES    
$    DO NOT LOCALIZE THESE MESSAGES    
$    These messages are used for the version information.    

1 "@(#)version_goes_here"

2 "\n\
@(#)_DtMessage catalog source $XConsortium: dthello.msg /main/3 1995/11/08 11:52:40 rswiston $"
