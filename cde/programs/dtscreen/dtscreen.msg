$ $XConsortium: dtscreen.msg /main/5 1996/08/12 12:02:15 pascale $
$ @(#)25	1.1.1.1  com/XTOP/aixclients/xlock/xlock.msg, xclients, r5gos325, 9333325d 5/6/93 20:13:37
$ 
$ COMPONENT_NAME: dtscreen
$ 
$ FUNCTIONS: dtscreen.msg
$ 
$ 
$ (C) COPYRIGHT International Business Machines Corp. 1987, 1993
$ All Rights Reserved
$ Licensed Materials - Property of IBM
$ 
$ US Government Users Restricted Rights - Use, duplication or
$ disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
$ 
$ 
$ *                                                                      *
$ * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
$ * (c) Copyright 1993, 1994 International Business Machines Corp.       *
$ * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
$ * (c) Copyright 1993, 1994 Novell, Inc.                                *
$ *

$set 2
$ message file created 1993

$quote "
1 "1386-101 %s:  Bad command line option:  %s.\n\n"
2 "1386-102 \nType %s -help for a full description.\n\n"

3 "1386-103 Usage:\n        %s [-options ...]\n\n\where options include:\n"
5 "1386-105    %-28s %s\n\t where mode is one of:\n\", \"-mode mode\", \"animation mode"

7 "1386-107 Unknown mode: "
10 "1386-110 couldn't allocate: %s\n"
11 "1386-111 %d pixels allocated\n"

17 "1386-117 %s: Unable to open display %s.\n"
18 "1386-118 %s: GetResource - couldn't allocate memory"
19 "1386-119 %s: Malformed -display argument, \"%s\"\n"

20 "1386-120 -batchcount argument must be positive."
21 "1386-121 -saturation argument must be between 0.0 and 1.0."

25 "1386-125 %s: can only support %d screens.\n"
30 "1386-130 -delay argument must be positive."
35 "1386-135 couldn't allocate: %s, using %s instead\n"
40 "%s: Cannot locate window in which to draw. Using the -create\n\
option will cause %s to create its own window.\n"
