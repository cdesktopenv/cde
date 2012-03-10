! $XConsortium: UNIXbindings.src /main/2 1995/07/20 12:05:57 drk $
!######################################################################
!#
!#  UNIXbindings
!#
!#  Common Desktop Environment (CDE)
!#
!#  Provides EMACS style translations for DtEditor widget applications
!#  such as Dtpad and Dtmail.  Also provides alternates for DtEditor
!#  widget application menu accelerators which confict with these
!#  translations.
!#
!#  By default this file is not used by DtEditor widget applications.
!#  To enable these bindings, include this file in your home directory
!#  .Xdefaults file and then restart your session.  Initially, you can
!#  include this file simply by inserting a #include directive in your
!#  .Xdefaults file:
!#
!#	#include "/usr/dt/app-defaults/LANG/UNIXbindings"
!#
!#  where LANG is the value of your $LANG environment variable
!#  (normally "C").
!#
!#  To modifiy these bindings, insert the contents of this file directly
!#  in your .Xdefaults file instead of the above include directive.
!#  Then edit the bindings in your .Xdefaults file (and restart your
!#  session when done).
!#
!#
!#  (c) Copyright 1993, 1994 Hewlett-Packard Company
!#  (c) Copyright 1993, 1994 International Business Machines Corp.
!#  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
!#  (c) Copyright 1993, 1994 Novell, Inc.
!#
!######################################################################


!######################################################################
!# Dtpad overrides for menu accelerators and accelerator text which
!# confict with the UNIX bindings.
!######################################################################
Dtpad*fileMenu.print.acceleratorText:		
Dtpad*fileMenu.print.accelerator:		
Dtpad*editMenu.undo.acceleratorText:		Ctrl+_
Dtpad*editMenu.undo.accelerator:		Ctrl<Key>_
Dtpad*editMenu.paste.acceleratorText:		Shift+Insert
Dtpad*editMenu.paste.accelerator:		Shift<Key>osfInsert
Dtpad*editMenu.findChange.acceleratorText:	Ctrl+S
Dtpad*editMenu.findChange.accelerator:		Ctrl<Key>s


!######################################################################
!# Dtmail compose dialog overrides for menu accelerators and accelerator
!# text which confict with the UNIX bindings.
!######################################################################
Dtmail*ComposeDialog*menubar*Edit.Undo.acceleratorText:		Ctrl+_
Dtmail*ComposeDialog*menubar*Edit.Undo.accelerator: 		Ctrl<Key>_
Dtmail*ComposeDialog*menubar*Edit.Paste.acceleratorText: 	Shift+Insert
Dtmail*ComposeDialog*menubar*Edit.Paste.accelerator: 		Shift<Key>osfInsert
Dtmail*ComposeDialog*menubar*Edit.Find/Change.acceleratorText: 	Ctrl+S
Dtmail*ComposeDialog*menubar*Edit.Find/Change.accelerator: 	Ctrl<Key>s


!######################################################################
!# UNIX Bindings.
!#
!# The following translations provide (GNU style) EMACS control and
!# meta key bindings plus some additional bindings.  When appropriate,
!# they also allow the Shift key to be used in combination with the
!# normal binding to reverse the direction of the operation.  For
!# example, Ctrl-Shift-F will move the cursor backward a character
!# since Ctrl-F normally moves it forward a character.
!#
!# The additional bindings are:
!#
!#    Ctrl-comma:  backward-word       Ctrl-Shift-comma:  forward-word
!#    Ctrl-period: forward-word        Ctrl-Shift-period: backward-word
!#    Ctrl-Return: end-of-file         Ctrl-Shift-Return: beginning-of-file
!#
!# NOTES:
!#
!#  o GNU EMACS binds delete-previous-character() rather than
!#    delete-next-character() to the Delete key.
!#  o Meta-F is normally the mnemonic for the File menu so the binding
!#    to forward-word() will be ignored.
!#
!######################################################################
*DtEditor.textTranslations: \
c ~s		<Key>a:			beginning-of-line()\n\
c s		<Key>a:			end-of-line()\n\
c ~s		<Key>b:			backward-character()\n\
c s		<Key>b:			forward-character()\n\
c ~s		<Key>b:			backward-character()\n\
c s		<Key>b:			backward-word()\n\
m ~s		<Key>b:			backward-word()\n\
m s		<Key>b:			forward-word()\n\
c ~s		<Key>d:			delete-next-character()\n\
c s		<Key>d:			delete-previous-character()\n\
m ~s		<Key>d:			kill-next-word()\n\
m s		<Key>d:			kill-previous-word()\n\
c ~s		<Key>e:			end-of-line()\n\
c s		<Key>e:			beginning-of-line()\n\
c ~s		<Key>f:			forward-character()\n\
c s		<Key>f:			backward-character()\n\
m ~s		<Key>f:			forward-word()\n\
m s		<Key>f:			backward-word()\n\
c		<Key>j:			newline-and-indent()\n\
c ~s		<Key>k:			kill-to-end-of-line()\n\
c s		<Key>k:			kill-to-start-of-line()\n\
c		<Key>l:			redraw-display()\n\
c		<Key>m:			newline()\n\
c ~s		<Key>n:			process-down()\n\
c s		<Key>n:			process-up()\n\
c		<Key>o:			newline-and-backup()\n\
c ~s		<Key>p:			process-up()\n\
c s		<Key>p:			process-down()\n\
c ~s		<Key>u:			kill-to-start-of-line()\n\
c s		<Key>u:			kill-to-end-of-line()\n\
c ~s		<Key>v:			next-page()\n\
c s		<Key>v:			previous-page()\n\
m ~s		<Key>v:			previous-page()\n\
m s		<Key>v:			next-page()\n\
c		<Key>w:			kill-selection()\n\
c ~s		<Key>y:			unkill()\n\
m		<Key>]:			forward-paragraph()\n\
m		<Key>[:			backward-paragraph()\n\
c ~s		<Key>comma:		backward-word()\n\
c s		<Key>comma:		forward-word()\n\
m		<Key>\\<:		beginning-of-file()\n\
c ~s		<Key>period:		forward-word()\n\
c s		<Key>period:		backward-word()\n\
m		<Key>\\>:		end-of-file()\n\
c ~s		<Key>Return:		end-of-file()\n\
c s		<Key>Return:		beginning-of-file()\n\
~c ~s ~m ~a	<Key>osfDelete:		delete-previous-character()\n\
~c s ~m ~a	<Key>osfDelete:		delete-next-character()

!###########################   eof   ###########################
