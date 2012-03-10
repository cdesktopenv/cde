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
# $XConsortium: book-toc.ts /main/2 1996/07/18 14:22:29 drk $
# ______________________________________________________________________
#
#	This is for printing a TOC/outline of an instance as text.
#	Only division titles will be printed.
# ______________________________________________________________________
Var:		part	1
Var:		chap	1
Var:		sect	1
Var:		subsect	1
Var:		app	1
#
# ______________________________________________________________________
GI:		DIVISION
Context:	SECTION
SpecID:		12
StartText:	^        ${subsect}.\s
EndText:	^
Increment:	subsect
-
GI:		SECTION
SpecID:		11
StartText:	^    ${sect}.\s
EndText:	^
Set:		subsect	1
Increment:	sect
-
GI:		CHAPTER
SpecID:		10
StartText:	^Chapter ${chap}.\s
EndText:	^
Set:		sect	1
Increment:	chap
-
GI:		PART
StartText:	^\nPart ${part}:\s
EndText:	^
Increment:	part
-
GI:		APPENDIX
SpecID:		15
StartText:	^${app}.\s
EndText:	^
Set:		sect	1
Increment:	app
-
GI:		DIVISION
Context:	APPENDIX
Action:		11
-
# ______________________________________________________________________
GI:		DIVISION
Context:	DIVISION DIVISION BODY
Action:		12
-
GI:		DIVISION
Context:	DIVISION BODY
Action:		11
-
GI:		DIVISION
Context:	CHAPTER
Action:		11
-
GI:		DIVISION
Context:	BODY
Action:		10
-
# ______________________________________________________________________
GI:		PREFACE
StartText:	^
-
GI:		PDIVISION
StartText:	^   \s
-
# ______________________________________________________________________
GI:		OSF-REFPAGE
StartText:	\n
-
GI:		REF-NAME
StartText:	^
EndText:	\ --\s
-
GI:		REF-PURPOSE
#StartText:
EndText:	^\n
-
GI:		SUB-NAME
StartText:	^   \s
-
GI:		SUB-PURPOSE
StartText:	\t
-
GI:		RSECTION
SpecID:		21
StartText:	^   \s
EndText:	^
-
GI:		RSUBSECTION
SpecID:		22
StartText:	^       \s
EndText:	^
-
GI:		RDIVISION
Context:	RSUBSECTION
SpecID:		23
StartText:	^           \s
EndText:	^
-
GI:		RDIVISION
Context:	RDIVISION RDIVISION REFPAGE
Action:		23
-
GI:		RDIVISION
Context:	RDIVISION REFPAGE
Action:		22
-
GI:		RDIVISION
Context:	REFPAGE
Action:		21
-
# ______________________________________________________________________
GI:		OSF-BOOK
-
GI:		BODY
StartText:	\n
-
GI:		SUPPLEMENTS
StartText:	\nAppendixes^
-
GI:		PRELIMINARIES
StartText:	\n
-
# ______________________________________________________________________
GI:		TITLE
EndText:	^
-
GI:		ALT-TITLE
StartText:	^    [
EndText:	]^
-
# ______________________________________________________________________
# Pass through text of things inside a title
Relation:	ancestor TITLE
-
Relation:	ancestor REF-NAME
-
Relation:	ancestor REF-PURPOSE
-
#
# Wildcard - skip all other elements...
GI:	*
Ignore:	all
-
# ______________________________________________________________________
