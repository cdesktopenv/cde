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
# $XConsortium: book-toc_ps.ts /main/2 1996/07/18 14:22:43 drk $
# ______________________________________________________________________
#
#	This is for printing a TOC/outline of an instance in PostScript.
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
StartText:	^${_followrel child TITLE 1000}\s
		(        ${chap}.${sect}.${subsect}) 10 DivTitle^
Increment:	subsect
-
GI:		SECTION
SpecID:		11
StartText:	^${_followrel child TITLE 1000} (    ${chap}.${sect}) 12 DivTitle^
Set:		subsect	1
Increment:	sect
-
GI:		CHAPTER
SpecID:		10
StartText:	^${_followrel child TITLE 1000} (Chapter ${chap}:) 14 DivTitle^
Set:		sect	1
Increment:	chap
-
GI:		PART
StartText:	^${_followrel child TITLE 1000} (Part ${part}:) 16 DivTitle^
Increment:	part
-
GI:		APPENDIX
SpecID:		15
StartText:	^${app}.\s
StartText:	^${_followrel child TITLE 1000} (Appendix ${app}:) 14 DivTitle^
Set:		sect	1
Increment:	app
-
GI:		DIVISION
Context:	APPENDIX
StartText:	^${_followrel child TITLE 1000} (    ${app}.${sect}) 14 DivTitle^
Set:		subsect	1
Increment:	sect
-
# ______________________________________________________________________
GI:		DIVISION
Context:	DIVISION DIVISION BODY
Action:		12
-
GI:		DIVISION
Context:	CHAPTER
Action:		11
-
GI:		DIVISION
Context:	DIVISION BODY
Action:		11
-
GI:		DIVISION
Context:	BODY
Action:		10
-
# ______________________________________________________________________
GI:		PREFACE
StartText:	^${_followrel child TITLE 1000} () 14 DivTitle^
-
GI:		PDIVISION
StartText:	^${_followrel child TITLE 1000} (    ) 12 DivTitle^
-
# ______________________________________________________________________
GI:		OSF-REFPAGE
StartText:	\n(${_followrel child REF-PURPOSE 1001})\s
		(Refpage: ${_followrel child REF-NAME 1001} \\267) 14 DivTitle
-
GI:		SUB-NAME
StartText:	^   \s
-
GI:		SUB-PURPOSE
StartText:	\t
-
GI:		RSECTION
SpecID:		21
StartText:	^${_followrel child TITLE 1000} (    ) 12 DivTitle^
-
GI:		RSUBSECTION
SpecID:		22
StartText:	^${_followrel child TITLE 1000} (        ) 10 DivTitle^
-
GI:		RDIVISION
Context:	RSUBSECTION
SpecID:		23
StartText:	^${_followrel child TITLE 1000} (            ) 10 DivTitle^
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
StartText:	^${_followrel child TITLE 1000} 20 BkTitle^
-
GI:		BODY
StartText:	\n
-
GI:		SUPPLEMENTS
StartText:	\n
-
GI:		PRELIMINARIES
-
# ______________________________________________________________________
# This just descends tree...
GI:		_TITLE
SpecID:		1000
StartText:	(
EndText:	)
-
# Same, but no parens...
GI:		_TITLE
SpecID:		1001
-
GI:		ALT-TITLE
Ignore:		all
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
# PostScript prolog stuff
GI:	_Start
StartText:	%!^
	/in {72 mul} bind def^
	/Home {/X 1 in def  /Y 10 in def} def^
	/BkTitle {	% (title) ps^
	\  /ps exch def^
	\  /Times-Bold findfont ps scalefont setfont^
	\  X Y moveto  0 ps neg rmoveto^
	\  dup stringwidth pop  /w exch def  w 2 div  neg 3.25 in add 0 rmoveto^
	\  show^
	\  0 -3 rmoveto  w neg 0 rlineto  stroke^
	\  /Y Y ps 1.1 mul sub def^
	} def^
	/DivTitle {	% (title) (divname) ps^
	\  /ps exch def^
	\  /Times-Roman findfont ps scalefont setfont^
	\  X Y moveto  0 ps neg rmoveto^
	\  show  ( ) show  show^
	\  /Y Y ps 1.1 mul sub def^
	\  % check for end of page^
	\  Y 1 in lt { showpage Home } if^
	} def^
	\nHome^
-
GI:		_End
EndText:	^showpage^\n
-
# ______________________________________________________________________
