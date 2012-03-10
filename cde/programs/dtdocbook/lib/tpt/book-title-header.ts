# ______________________________________________________________________
# Copyright 1993 Open Software Foundation, Inc., Cambridge, Massachusetts.
# All rights reserved.
#
# Copyright (c) 1994  
# Open Software Foundation, Inc. 
#  
# Permission is hereby granted to use, copy, modify and freely distribute 
# the software in this file and its documentation for any purpose without 
# fee, provided that the above copyright notice appears in all copies and 
# that both the copyright notice and this permission notice appear in 
# supporting documentation.  Further, provided that the name of Open 
# Software Foundation, Inc. ("OSF") not be used in advertising or 
# publicity pertaining to distribution of the software without prior 
# written permission from OSF.  OSF makes no representations about the 
# suitability of this software for any purpose.  It is provided "as is" 
# without express or implied warranty. 
#
#
# This will return the book's title, marked up for SML's Headers/* files
# Just the text is returned.
#
# $XConsortium: book-title-header.ts /main/2 1996/07/18 14:22:12 drk $
# ______________________________________________________________________
#
GI:		TITLE
Context:	OSF-BOOK
StartText:	^.ds !@\s
EndText:	^
-
# Ignore all other child elements of the book
GI:		META BODY AUGMENTUM PRELIMINARIES SUPPLEMENTS
Ignore:		all
-
GI:		ALT-TITLE
Ignore:		all
Message:	Warning: Ignoring ALT-TITLE within TITLE.\n
-
# Pass characters of child elements of book's TITLE.
GI:		*
-
