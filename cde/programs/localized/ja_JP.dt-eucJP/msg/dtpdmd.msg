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
$ **  $XConsortium: dtpdmd.msg /main/1 1996/11/01 17:49:31 cde-hp $
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
$ "NEW -"
1 usage: %1$s [options]\n\
\n\
where [options] includes:\n\
   -d <display>        host:display[.screen] of X Print Server\n\
   -a <alt selection>  an alternate X-selection name ("%2$s" default)\n\
   -p <default pdm>    a default pdm to launch ("%3$s" default)\n\
   -P <the pdm>        the pdm to launch, overrides all others\n\
   -s                  turn on security protocol\n\
   -f <auth file>      an initial set of authorization cookies\n\
   -l <log file>       a file for the logging of errors, etc

$ "NEW -"
2 %1$s: error, cannot open display %2$s\n

$ "NEW -"
3 %1$s: error, another pdm is already running (%2$s)\n

$ "NEW -"
4 %s: error, cannot create Xt signal pipe\n

$    /* NL_COMMENT
$ 
$       - Do not translate the word "SelectionRequest" in
$         messages 5, 6 and 7.
$ 
$       - Do not translate the word "XmbTextPropertyToTestList"
$         in message 7.
$ 
$     */
$ "NEW -"
5 %s: error, unable to get SelectionRequest property\n

$ "NEW -"
6 %s: error, invalid format for SelectionRequest property\n

$ "NEW -"
7 %s: error, XmbTextPropertyToTestList failed on SelectionRequest property\n

$ "NEW -"
8 %s: error, unable to create message pipe\n

$    /* NL_COMMENT
$ 
$       - Do not translate the word "fork" in message 9.
$ 
$     */
$ "NEW -"
9 %s: error, unable to fork\n

$ "NEW -"
10 %1$s: error, cannot execute "%2$s" \n

$    /* NL_COMMENT
$ 
$       - Do not translate the word "exec" in message 11.
$ 
$       - Do not translate the words "-display" or
$         "-vdisplay" in message 11.
$ 
$     */
$ "NEW -"
11 %1$s: exec message logged at %2$s\n\
   exec error message = "%3$s"\n\
   exec error code    = %4$s\n\
   pdm                = "%5$s"\n\
   -display           = "%6$s"\n\
   -vdisplay          = "%7$s" \n

$ "NEW -"
12 %1$s: error, unable to send the final %2$s\n\
message to the requesting client on behalf of\n\
the PDM because of a generated X error\n

$ "NEW -"
13 %1$s: error, unable to send the final %2$s\n\
message to the requesting client on behalf of\n\
the PDM because of an XIO error\n

$    /* NL_COMMENT
$ 
$       - Do not translate the word "stderr" in message 14.
$ 
$       - Do not translate the words "-display" or
$         "-vdisplay" in message 14.
$ 
$     */
$ "NEW -"
14 %1$s: pdm message logged at %2$s\n\
   pdm       = "%3$s"\n\
   -display  = "%4$s"\n\
   -vdisplay = "%5$s"\n\
   exit code = "%6$d"\n\
   stderr    = "%7$s" \n

$ "NEW -"
15 attached pdmd error= \n"%s" \n

$ "NEW -"
16 %1$s: error, cannot open log file "%2$s". Turning off stderr message logging\n

