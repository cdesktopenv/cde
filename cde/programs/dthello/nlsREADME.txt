# $XConsortium: nlsREADME.txt /main/2 1996/11/11 11:27:52 drk $
# *                                                                      *
# * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
# * (c) Copyright 1993, 1994 International Business Machines Corp.       *
# * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
# * (c) Copyright 1993, 1994 Novell, Inc.                                *
# *
#############################################################################
#
# Component:  dthello (and dtcolor)
#
############################################################################

=============================================================
_DtMessage catalog source:

   File name:  	dthello.msg

   Target:	/usr/dt/nls/%L/dthello.cat

   There is only ONE message catalog (dthello.cat).  It is
   used by both dtcolor and dthello.

    Look for the lines that say, "Starting the Hewlett-Packard
    Visual User Environment."  These should be localized.

    There is no change from what you did for HP DT 2.01.
=============================================================
Two app-defaults files:

   File name:  	Dtcolor

   Target:	/usr/dt/app-defaults/%L/Dtcolor

   File name:  	Dthello

   Target:	/usr/dt/app-defaults/%L/Dthello

   The one resource in these files that should be changed is the
   default font specification.

    For example:

       Dthello*font:          system17
       --changed to--
       Dthello*font:          jpn.8x18

       Dtcolor*font:          system17
       --changed to--
       Dtcolor*font:          jpn.8x18
    
=============================================================
