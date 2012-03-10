# $XConsortium: nlsREADME.txt /main/2 1996/11/11 11:31:32 drk $
# *                                                                      *
# * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
# * (c) Copyright 1993, 1994 International Business Machines Corp.       *
# * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
# * (c) Copyright 1993, 1994 Novell, Inc.                                *
# *
#############################################################################
#
# Component:  /usr/dt/bin/dtsession
#
############################################################################


_DtMessage catalog source:

   File name:  	dtsession.msg

   Target:	/usr/dt/nls/%L/dtsession.cat

---------------------------------------------------------------------------
App-defaults file:

   File name:  	Dtsession

   Target:	/usr/dt/app-defaults/%L/Dtsession
---------------------------------------------------------------------------

Configuration files:

   File Name:  sys.session

   Target:     /usr/dt/config/%L/sys.session

   In this file is the startup command for the Console terminal window.
One of the options for this command is -T Console.  This name needs to
be localized.

   This file starts up the clients in a default HP DT session.

---------------------------------------------------------------------------
   File Name:  sys.resources

   Target:     /usr/dt/config/%L/sys.resources

   There is an include line near the top of this file that looks like:

   #include "/usr/dt/config/sys.font"

   This line needs to be changed to include the language specific
location.  For example:

   #include "/usr/dt/config/japanese/sys.font"

   This file restores resources  in a default HP DT session.

---------------------------------------------------------------------------
   File Name:  sys.font

   Target:     /usr/dt/config/%L/sys.font

   This file contains the specification for the fonts to be used in an
HP DT session.  If you wish to have different fonts for the default
session, they need to be changed here.  Included are fonts for small,
medium and high resolution displays.

   Font resource values must specify all 14 field delimiters.
       Scalable font - must specify point size field
       Bitmap font - must specify pixel size field

   Font resource values must match exactly the default font resources 
   specified in /usr/dt/app-defaults/%L/Dtstyle.

   This file restores font resources in a default HP DT session, or in
a session where the language has been newly changed.


