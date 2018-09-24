$ "$XConsortium: dthello.msg /main/3 1995/11/08 12:27:03 rswiston $";  
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

2 "Bureau AIXwindows.\n\
Lancement en cours...\n\n"
$    Default startup message (written before the contents  
$    of the default file, /etc/copyright).  

3 "Démarrage \n\
du Bureau CDE\n\n"

$set 4        Redefine set#

$    THIS COMMENT FOR DT TEAM ONLY -- dthello.c --  
$    This entire set is for error messages  
$    DO NOT TRANSLATE THESE MESSAGES  

1 "%s: can't open display\n"

2 "%s: timeout must be positive\n"

3 "%1$s: Maxiumum of %2$d files allowed, skipping %3$s\n"
$ Messages 4 and 5 are both used when there is a syntax error on  
$ the command line invoking dthello  
$   

4 "Syntaxe: %s [-display <écran>] [-fg <couleur>] [-bg <couleur>]\\\n"

5 "[-font <police>] [-string <message>] [-file <nom>] [-timeout <secondes>]\n"
$ _DtMessage 6 occurs when an font can't be loaded  
$  the likely causes: (1) the font was incorrectly specified  
$                     (2) it is not available on this server  

6 "%1$s: L'écran %2$s ne reconnaît pas la police %3$s.\n"

7 "%1$s: Impossible d'ouvrir le fichier %2$s.\n"

8 "%1$s: Impossible de définir l'arrière-plan à %2$s, utilisation de %3$s.\n"

9 "%1$s: Impossible de définir le premier plan à %2$s, utilisation de %3$s.\n"

10 "%s: Mémoire insuffisante (SeparateTextLines).\n"

$set 99        Redefine set#

$    DO NOT TRANSLATE THESE MESSAGES  
$    DO NOT CHANGE THESE MESSAGES  
$    DO NOT LOCALIZE THESE MESSAGES  
$    These messages are used for the version information.  

1 "@(#)version_goes_here"

2 "\n\
@(#)_DtMessage catalog source $XConsortium: dthello.msg /main/3 1995/11/08 12:27:03 rswiston $"
