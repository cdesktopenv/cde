$ "$XConsortium: dthello.msg /main/3 1995/11/08 12:41:50 rswiston $";

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

$set 2
$    THIS COMMENT FOR DT TEAM ONLY -- dthello.c --
$    Do not translate. Blank message, please ignore
1 


$    Message #2 is no longer used. It has been replaced with messag #3
2 Avvio\n\
dell'ambiente utente\n\
COSE\n\n

$    Default startup message (written before the contents
$    of the default file, /etc/copyright).
3 Avvio dell'ambiente CDE\n\n\n


$set 4
$    THIS COMMENT FOR DT TEAM ONLY -- dthello.c --
$    This entire set is for error messages
$    DO NOT TRANSLATE THESE MESSAGES
1 %s: can't open display\n
2 %s: timeout must be positive\n
3 %1$s: Maxiumum of %2$d files allowed, skipping %3$s\n

$ Messages 4 and 5 are both used when there is a syntax error on
$ the command line invoking dthello
$ 
4 Sintassi: %s [-display <schermo>] [-fg <colore>] [-bg <colore>]\\\n
5 [-font <font>] [-string <messaggio>] [-file <nome>] [-timeout <secondi>]\n

$ _DtMessage 6 occurs when an font can't be loaded
$  the likely causes: (1) the font was incorrectly specified
$                     (2) it is not available on this server
6 %1$s: Lo schermo %2$s non conosce il font %3$s\n

7 %1$s: Impossibile aprire il file %2$s\n
8 %1$s: Impossibile impostare il colore di sfondo %2$s; verrà usato il colore %3$s.\n
9 %1$s: Impossibile impostare il colore di primo piano %2$s; verrà usato il colore %3$s.\n
10 %s: Memoria insufficiente (SeparateTextLines)\n

$set 99
$    DO NOT TRANSLATE THESE MESSAGES
$    DO NOT CHANGE THESE MESSAGES
$    DO NOT LOCALIZE THESE MESSAGES
$    These messages are used for the version information.
1 @(#)version_goes_here
2 \n@(#)_DtMessage catalog source $XConsortium: dthello.msg /main/3 1995/11/08 12:41:50 rswiston $
