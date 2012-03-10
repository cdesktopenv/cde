#!/bin/sh
# *                                                                      *
# * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
# * (c) Copyright 1993, 1994 International Business Machines Corp.       *
# * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
# * (c) Copyright 1993, 1994 Novell, Inc.                                *
# *
#######################################################################
#
#     convertVS.sh
#
#     The shell-script which converts a DT 2.0 dt.session file
#     to a DT 3.0 dt.session file.  This file is intended to 
#     only be executed by dtsession.
#
#     Hewlett-Packard Visual User Environment
#
#     Copyright (c) 1991, 1992 Hewlett-Packard Company
#
#     @(#) $XConsortium: convertVS.sh /main/3 1995/10/30 09:40:46 rswiston $
#
######################################################################
# Usage: convertVS.sh  <session_file>

if [ $# -eq 1 ]
then
    if [  -r $1  ]
    then

#       ----   get the local clients

	grep -v host $1 >$1T1


#       ----  get the remote clients ----

	grep    host $1 >$1T2


#       ----  change /usr/bin/X11/<client> to /usr/dt/bin/<client> ----
#             for local clients  $1T1                                   ----

        sed 's/\/usr\/bin\/X11\/hpterm/\/usr\/dt\/bin\/hpterm/g
             s/\/usr\/bin\/X11\/xload/\/usr\/dt\/bin\/xload/g'  $1T1  > $1T3


#       ----  preserve old dt.session file ----

        cp $1 $1.20


#       ----  put local clients first in file ----

	cat $1T3 $1T2 >$1TMP


#       ----  remove the temporary files ----

	rm -f $1T1 $1T2 $1T3


#       ----  make this our new dt.session ----

        mv -f $1TMP $1


        exit 0
    else
        exit 1
    fi
else
    exit 1
fi
