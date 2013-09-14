$ *************************************<+>*************************************
$ *****************************************************************************
$ **
$ **  File:        dtpdmd.msg
$ **
$ **  Description:
$ **  -----------
$ **  This file is the source for the message catalog for the Dt Print
$ **  Dialog Manager Daemon.
$ **
$ **
$ *****************************************************************************
$ **
$ ** (c) Copyright 1996 Digital Equipment Corporation.
$ ** (c) Copyright 1996 Hewlett-Packard Company.
$ ** (c) Copyright 1996 International Business Machines Corp.
$ ** (c) Copyright 1996 Sun Microsystems, Inc.
$ ** (c) Copyright 1996 Novell, Inc. 
$ ** (c) Copyright 1996 FUJITSU LIMITED.
$ ** (c) Copyright 1996 Hitachi.
$ **
$ *****************************************************************************
$ **
$ **
$ **  $XConsortium: dtpdmd.msg /main/2 1996/11/19 19:28:51 rws $
$ *****************************************************************************
$ *****************************************************************************

$set 1

$    /* NL_COMMENT
$ 
$	General Translator Comments
$ 
$       - The acronyms "pdm" and "PDM" are used in several
$         messages, and were derived from the english phrase
$         "Print Dialog Manager". The acronyms should not be
$         localized.
$ 
$       - The acronym "pdmd" is used in several messages,
$         and was derived from the english phrase "Print
$         Dialog Manager Daemon". The acronym should not be
$         localized.
$ 
$     */
1 Sintassi: %1$s [opzioni]\n\
\n\
dove [opzioni] includono:\n\
   -d <display>        host:display[.screen] del server di stampa X\n\
   -a <selezione alt>  un altro nome di X-selezione (default "%2$s")\n\
   -p <pdm predef.>    un pdm di default per avviare (default "%3$s")\n\
   -P <il pdm>         il pdm da avviare, ignorare tutti gli altri\n\
   -s                  attivare il protocollo di sicurezza\n\
   -f <file autorizz>  un insieme iniziale di voci di autorizzazione\n\
   -l <logfile>        un file per la registrazione di errori, ecc.

2 %1$s: errore, impossibile aprire il video %2$s\n

3 %1$s: errore, un altro pdm é già in esecuzione (%2$s)\n

4 %s: errore, impossibile creare Xt signal pipe\n

$    /* NL_COMMENT
$ 
$       - Do not translate the word "SelectionRequest" in
$         messages 5, 6 and 7.
$ 
$       - Do not translate the word "XmbTextPropertyToTestList"
$         in message 7.
$ 
$     */
5 %s: errore, impossibile ottenere la caratteristica SelectionRequest\n

6 %s: errore, formato non valido per la caratteristica SelectionRequest\n

7 %s: errore, XmbTextPropertyToTestList fallito nella caratteristica SelectionRequest\n

8 %s: errore, impossibile creare pipe di messaggi\n

$    /* NL_COMMENT
$ 
$       - Do not translate the word "fork" in message 9.
$ 
$     */
9 %s: errore, fork impossibile\n

10 %1$s: errore, impossibile eseguire "%2$s" \n

$    /* NL_COMMENT
$ 
$       - Do not translate the word "exec" in message 11.
$ 
$       - Do not translate the words "-display" or
$         "-vdisplay" in message 11.
$ 
$     */
11 %1$s: messaggio exec registrato a %2$s\n\
   messaggio di errore exec = "%3$s"\n\
   codice di errore exec    = %4$s\n\
   pdm                      = "%5$s"\n\
   -display                 = "%6$s"\n\
   -vdisplay          = "%7$s" \n

12 %1$s: errore, impossibile spedire il messaggio finale\n\
%2$s da parte del PDM al client che l'ha richiesto.\n\
Causa: errore generato X.\n

13 %1$s: errore, impossibile spedire il messaggio finale\n\
%2$s da parte del PDM al cliente cha l'ha richiesto.\n\
Causa: errore XIO.\n

$    /* NL_COMMENT
$ 
$       - Do not translate the word "stderr" in message 14.
$ 
$       - Do not translate the words "-display" or
$         "-vdisplay" in message 14.
$ 
$     */
14 %1$s: messaggio pdm registrato a %2$s\n\
   pdm              = "%3$s"\n\
   -display         = "%4$s"\n\
   -vdisplay        = "%5$s"\n\
   codice di uscita = "%6$d"\n\
   stderr           = "%7$s" \n

15 errore pdmd allegato= \n"%s" \n

16 %1$s: errore, impossibile aprire il logfile "%2$s".  È in corso la chiusura della registrazione dei messaggi stderr\n

