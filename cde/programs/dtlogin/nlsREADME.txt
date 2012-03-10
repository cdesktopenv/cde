# $XConsortium: nlsREADME.txt /main/2 1996/11/11 11:30:53 drk $
# *                                                                      *
# * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
# * (c) Copyright 1993, 1994 International Business Machines Corp.       *
# * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
# * (c) Copyright 1993, 1994 Novell, Inc.                                *
# *
#############################################################################
#
# Component:  Dtlogin
#
############################################################################

_DtMessage catalog source:

   File name:  	dtlogin.msg

   Target:	/usr/lib/nls/%L/dtlogin.cat


If additional files are to be localized, for each file include:

   File name:	Xresources

   Target:	/usr/dt/config/Xresources

   Purpose:

     This file contains the appearance resources for the login screen.  The
     traditional 'app-defaults' technology is not used since these resources
     are specified on a per-display rather than a per-user basis.  When the
     login screen is drawn, the user that will eventually log in is not yet
     known.

     There is only one file containing the login appearance resources to
     service all languages, rather than an individual file per language.
     Before the login screen is drawn, this file is scanned (with the xrdb
     program) and the appropriate resources are used.  The current value of
     the $LANG variable is available so that language specified resources
     (such as font) can be selected.


   Instructions:

     If the font to be used for the localized login screen is different from
     the default, then an entry must be made in this file to indicate the
     desired font.
   
     At the end of the section labeled...

        !!#################################################################
	!!
	!!  FONTS
	!!


	1. add...

		#ifdef <LANG>

			where <LANG> is replaced by the value of the $LANG
			variable appropriate for this language.

	2. add...

		Dtlogin.labelFont:	<font for button labels>
		Dtlogin.textFont:	<font for dialog & help text>
		Dtlogin.greeting.fontList: <font for greeting text>		

	3. add...

		#endif
		

   ex.  for LANG=japanese
   

	#ifdef japanese
	  Dtlogin*labelFont:		jpn.16x32
	  Dtlogin*textFont:		jpn.12x24
	  Dtlogin.greeting.fontList:   <italic font of equivalent size>	
	#endif




     The message catalog contains the text for the first part of the
     greeting message displayed below the HP logo.  This text is "Welcome
     to" (set 1,16).  Dtlogin appends the system name to the text to
     generate the complete greeting message.

     This text can be overridden by the user via a resource in the
     Xresources file.  The file is shipped with the resource commented out
     so that the message catalog takes precedence.  The resource value
     should be localized identically to the message catalog.

     At the end of the section labeled...

        !!#################################################################
	!!
	!!  GREETING
	!!


	1. add...

		#ifdef <LANG>

			where <LANG> is replaced by the value of the $LANG
			variable appropriate for this language.

	2. add...

		!! Dtlogin*greeting.labelString:   <Welcome to> %LocalHost%
						     ^^^^^^^^^^
						     localized value
	3. add...

		#endif
		

   ex.  for LANG=japanese
   

	#ifdef japanese
	!! Dtlogin*greeting.labelString: <Welcome to> %LocalHost%
	#endif


     To test this text, remove the comment characters "!!" from the line. To
     test the text in the message catalog, insert the comment characters.


     

     The localized Xresources file along with the message catalog should be
     returned to the DT development team.  In addition, it may be necessary
     for the customize script (run after installing a language) to contain
     instructions that would append the above lines to an existing
     Xresources.
