#  Copyright (c) 1993 Open Software Foundation, Inc.
#  All rights reserved.
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
# ______________________________________________________________________
#
# refpage-to-man translation spec.
#
# $XConsortium: ref-man.ts /main/2 1996/07/18 14:23:11 drk $
# ______________________________________________________________________
#
Var:		User	!printenv USER
Var:		Auth	Open Software Foundation

# ______________________________________________________________________

GI:		OSF-REFPAGE
StartText:	^
EndText:	^
-
# ______________________________________________________________________
GI:		ROW
StartText:	^
EndText:	^${_osftable tbl rowend}^
-
GI:		TITLE-ROW
StartText:	^
EndText:	^${_osftable tbl rowend}^
-
GI:		CELL
Context:	TITLE-ROW
Relation:	sibling+ CELL
StartText:	\\fB
EndText:	\\fR@
-
GI:		CELL
Context:	TITLE-ROW
StartText:	\\fB
EndText:	\\fR
-
GI:		CELL
Relation:	sibling+ CELL
EndText:	@
-
GI:		CELL
-
GI:		C-CELL
Relation:	sibling+ C-CELL
StartText:	T{^
EndText:	^T}@
-
GI:		C-CELL
StartText:	T{^
EndText:	^T}
-
# if you want a table title/caption, put it in a display
GI:		TABLE
StartText:	^.TS^
		${_osftable tbl}^${_osftable tbl top}^
EndText:	^${_osftable tbl bottom}^.TE^
-
# ______________________________________________________________________
#GI:		TEXT-AS-FIGURE
#StartText:	^\n\\begin{verbatim}^
#EndText:	^\\end{verbatim}^\n
#-
GI:		TITLE
Context:	DISPLAY
Relation:	sibling TABLE
StartText:	^.TB "
EndText:	"^
-
GI:		DISPLAY
Relation:	child TABLE
#StartText:	^
#EndText:	^
-
GI:		DISPLAY
AttValue:	WRAP NOWRAP
StartText:	^.DS^.nf^
EndText:	^.ad^.DE^
-
GI:		DISPLAY
AttValue:	TYPE FIGURE
StartText:	^.DS^
EndText:	^.DE^
-
GI:		TITLE
Context:	DISPLAY
Relation:	sibling GRAPHIC
StartText:	^.C 1^
EndText:	^
-
# default display
GI:		DISPLAY
StartText:	^.DS^
EndText:	^.DE^
-
GI:		TITLE
Context:	DISPLAY
StartText:	^.C 1^
EndText:	^
-
# ______________________________________________________________________
GI:		CMD-SYNOPSIS PROGLANG-SYNOPSIS FILE-SYNOPSIS
StartText:	^.na^.PP^
EndText:	^.ad^
-
GI:		INCLUDE
StartText:	^.br^
EndText:	^.sp 1.5^
-
GI:		DATA-DECLARATION
StartText:	^
EndText:	^
-
GI:		CMD-ARGUMENT
AttValue:	PRESENCE OPTIONAL
AttValue:	REPEATABILITY REPEATABLE
StartText:	\ [
EndText:	\ ...]\s
-
GI:		CMD-ARGUMENT
AttValue:	REPEATABILITY REPEATABLE
StartText:	\s
EndText:	\ ...
-
GI:		CMD-ARGUMENT
AttValue:	PRESENCE OPTIONAL
StartText:	\ [
EndText:	]\s
-
GI:		CMD-ARGUMENT
StartText:	\s
EndText:	\s
-
# hack - should check value of parent's TYPE attr (for OR)
GI:		OPTION-NAME
Context:	CMD-ARGUMENT
PAttSet:	TYPE
NthChild:	1
StartText:	\ \\fB
EndText:	\\fP\s
-
GI:		OPTION-NAME
Context:	CMD-ARGUMENT
PAttSet:	TYPE
StartText:	\ | \\fB
EndText:	\\fP\s
-
GI:		OPTION-NAME
Context:	CMD-ARGUMENT
StartText:	\ \\fB
EndText:	\\fP\s
-
# ______________________________________________________________________
GI:		LITERAL COMMAND FUNCTION FILE DIRECTORY DATATYPE E-MAIL
StartText:	\\fB
EndText:	\\fP
-
GI:		VARIABLE EMPHASIS
StartText:	\\fI
EndText:	\\fP
-
GI:		SUBSCRIPT
StartText:	\\d\\s-2
EndText:	\\s+2\\u
-
GI:		SUPERSCRIPT
StartText:	\\u\\s-2
EndText:	\\s+2\\d
-
GI:		TRADEMARK
StartText:	\\fB
EndText:	\\fP
-
GI:		NUMBER DATE TIME PERSON PLACE HONORIFIC TELEPHONE YEAR ACRONYM
-
# change this if you have font C (Courier) instead of CW.
GI:		KEYBOARD-INPUT
#StartText:	\\fC
StartText:	\\f(CW
EndText:	\\fP
-
GI:		COMPUTER-OUTPUT
Context:	DISPLAY
#StartText:	^.DS^.nf^.ft CW^
#EndText:	^.ft R^.fi^.DE^
StartText:	^.nf^.ft CW^
EndText:	^.ft R^.fi^
-
GI:		COMPUTER-OUTPUT
#StartText:	\\fC
StartText:	\\f(CW
EndText:	\\fP
-
GI:		KEYBOARD-KEY
StartText:	<\\fB
EndText:	\\fP>
-
GI:		INPUT-INSTRUCT FOREIGN-PHRASE
StartText:	\\fI
EndText:	\\fP
-
GI:		GUI-TEXT
StartText:	\\fB
EndText:	\\fP
-
GI:		LOGICAL-NEGATION
StartText:	^.ul 1^
EndText:	^
-
GI:		OPTION-NAME
StartText:	\\fB
EndText:	\\fP
-
GI:		MISC-DATA
#StartText:	{
#EndText:	}
-
GI:		MARKUP
AttValue:	LANG sgml
AttValue:	CATEGORY elem.*
StartText:	\\fB<
EndText:	>\\fP
-
GI:		MARKUP
AttValue:	CATEGORY (attr|entity)
StartText:	\\fB
EndText:	\\fP
-
GI:		MARKUP
StartText:	\\fB<
EndText:	>\\fP
-
GI:		SPECIAL-FORMAT
AttValue:	SMALLCAPS SMALLCAPS
StartText:	\\s-1
EndText:	\\s+1
-
GI:		SPECIAL-FORMAT
AttValue:	HORIZKEEP NOBREAK
StartText:	^.DS^
EndText:	^.DE^
-
# a bit if a hack
GI:		SPECIAL-FORMAT
AttValue:	STYLE .
StartText:	{\\${STYLE}\s
EndText:	}
-
GI:		EXCERPT
StartText:	^.br^.in +.7i^
EndText:	^.in^.br^
-
GI:		QUOTE
StartText:	``
StartText:	''
-
# ______________________________________________________________________
# label/item for labeled list
GI:		LABEL
StartText:	^.IP "
EndText:	"^
-
GI:		ITEM
Context:	L-ITEM
StartText:	^
EndText:	^
-
# item for general list
GI:		ITEM
StartText:	^.IP \\(bu^
EndText:	^
-
GI:		_nested-list
SpecID:		881
#Replace:	^.RS 1cm^
Replace:	^.RS +\\n(INu^
-
GI:		_nested-list
SpecID:		882
Replace:	^.RE^
-
GI:		LIST
Relation:	child L-ITEM
StartText:	^'br\\" labeled list^${_relation ancestor LIST 881}
EndText:	${_relation ancestor LIST 882}^'br\\" labeled list end^
-
GI:		LIST
Relation:	child ITEM
AttValue:	TYPE UNORDERED
StartText:	^'br\\" unordered list^${_relation ancestor LIST 881}
EndText:	${_relation ancestor LIST 882}^^'br\\" unordered list end^
-
GI:		LIST
Relation:	child ITEM
AttValue:	TYPE ORDERED
StartText:	^'br\\" ordered list^${_relation ancestor LIST 881}
EndText:	${_relation ancestor LIST 882}^^'br\\" ordered list end^
-
# default general list
GI:		LIST
Relation:	child ITEM
StartText:	^'br\\" unordered list^${_relation ancestor LIST 881}
EndText:	${_relation ancestor LIST 882}^^'br\\" unordered list end^
-
# procedure list
GI:		LIST
Relation:	child PROCEDURE-STEP
StartText:	^'br\\" procedure list^${_relation ancestor LIST 881}
EndText:	${_relation ancestor LIST 882}^^'br\\" procedure list end^
Set:		stepnum	1
-
GI:		PROCEDURE-STEP
StartText:	^.IP ${stepnum}. 2cm^
EndText:	^
Increment:	stepnum
-
# note list
GI:		LIST
Relation:	child NOTE
StartText:	^
EndText:	^
-
# hack??  I have not tried this yet.
GI:		BRIDGE-P
StartText:	^.PP^
EndText:	^
-
GI:		L-ITEM
EndText:	^
-
GI:		P
Context:	ITEM
NthChild:	1
StartText:	^
EndText:	^
-
GI:		P
Context:	ITEM
StartText:	^.IP^
EndText:	^
-
# ______________________________________________________________________
GI:		NOTE
Relation:	child TITLE
StartText:	^.IP\s
EndText:	^
-
GI:		NOTE
StartText:	^.IP Note: 2cm
EndText:	^
-
GI:		TITLE
Context:	NOTE
StartText:	"
EndText:	" 2 cm^
-
GI:		ANNOTATION
StartText:	^.FS^
EndText:	^.FE^
Message:	WARNING: Found ${_gi} in a manpage:\n${_location}\n
-
GI:		FOOTNOTE
StartText:	^.FS^
EndText:	^.FE^
Message:	WARNING: Found ${_gi} in a manpage:\n${_location}\n
-
GI:		NOTEREF
-
# ______________________________________________________________________
# plain text...
GI:		_
SpecID:		98
-
GI:		REF-NAME
StartText:	^.TH "
EndText:	" "${_pattr CATEGORY}"^.SH "Name"^${_action 98} -\s
-
GI:		REF-PURPOSE
#StartText:
EndText:	^
-
GI:		RSECTION
StartText:	^.SH\s
EndText:	^
-
GI:		RSUBSECTION
StartText:	^.SS\s
EndText:	^
-
GI:		RDIVISION
StartText:	^.SS\s
EndText:	^
-
GI:		META
Ignore:		all
-
GI:		AUGMENTUM
Ignore:		all
-
# ______________________________________________________________________
GI:		BRIDGE-TITLE
StartText:	^.B "
EndText:	"^
-
# ______________________________________________________________________
GI:		MESSAGE
StartText:	^... Message^
-
GI:		MSG-CODE
Context:	MESSAGE
StartText:	^.LI "Code"^
EndText:	^
-
GI:		MSG-SYMBOL
StartText:	^.LI "Symbol"^
EndText:	^
-
GI:		MSG-TEXT
StartText:	^.LI "Text"^
Context:	MESSAGE
EndText:	^
-
GI:		MSG-TYPE
StartText:	^.LI "Type"^
EndText:	^
-
GI:		MSG-DESC
StartText:	^.LI "Description"^
EndText:	^
-
GI:		MSG-CAUSE
StartText:	^.LI "Cause"^
EndText:	^
-
GI:		MSG-AUDIENCE
StartText:	^.LI "Audience"^
EndText:	^
-
GI:		MSG-RESP
StartText:	^.LI "Response"^
EndText:	^
-
GI:		MSG-ORIGIN
StartText:	^.LI "Origin"^
EndText:	^
-
# ______________________________________________________________________
# "default" handling of some common and keydata elements
GI:		P
StartText:	^.PP^
EndText:	^
-
GI:		MSG-CODE
StartText:	\\fB
EndText:	\\fP
-
GI:		MSG-TEXT
#StartText:	{
#EndText:	}
-
GI:		TITLE
StartText:	"
EndText:	"^
-
GI:		ALT-TITLE
StartText:	^... Alt title:\s
EndText:	^
-
# ______________________________________________________________________
GI:		TOC INDEX BIBLIOGRAPHY GLOSSARY NAVIGATION HY-LOCATOR HY-CONCEPT
Ignore:		all
-
GI:		HY-DEF HY-TERM
-
# ----
GI:		ENTRY
AttValue:	DAVROLE glossary
StartText:	\\fB
EndText:	\\fP
-
GI:		ENTRY
AttValue:	DAVROLE index
#	no processing for manpage
-
GI:		ENTRY
-
# ----
GI:		XREF
AttValue:	OSFROLE gettitle
StartText:	Get-Title
-
GI:		XREF
AttValue:	OSFROLE getnumber
#StartText:	Get-Division-Number
#EndText:	\ \\ref{${LINKEND}}
-
GI:		XREF
AttValue:	OSFROLE getpage
#EndText:	\ \\pageref{${LINKEND}}
-
GI:		XREF
-
# ----
GI:		HY-BIB-DESCRIPTION
StartText:	\\fI
EndText:	\\fP
-
# ______________________________________________________________________
GI:		REV-BRACKET
StartText:	^'br rev-bracket^
-
GI:		I18N-BRACKET REV-DESCRIPTION OCCURRENCES OCC-ASYNC OCC-SYNC
-
GI:		DESC-POOL
Ignore:		all
-
# ______________________________________________________________________
GI:		GRAPHIC
Context:	DISPLAY
StartText:	^'br\\" Graphic from file: ${_filename}^
EndText:	^
Message:	WARNING: Found ${_gi} in a manpage:\n${_location}\n
-
GI:		GRAPHIC
StartText:	^'br\\" Graphic from file: ${_filename}^
Message:	WARNING: Found ${_gi} in a manpage:\n${_location}\n
-
GI:		EQUATION
StartText:	^.EQ^
EndText:	^.EN^
-
# ______________________________________________________________________
#
GI:		*
Message:	Found element without a matching spec:\n${_location}\n
-
GI:		_Start
StartText:	^'br Translated with ${transpec} by ${user} on ${host}, ${date}^
-
GI:		_End
EndText:	^
-
# ______________________________________________________________________
#
# Processing Instructions
GI:		_osf-break
Replace:	^.br^
-
GI:		_osf-new-page
Replace:	^.bp^
-
GI:		_osf-need
StartText:	^.ne\s
EndText:	^
-
GI:		_osf-hyphen
Replace:	-
-
# ______________________________________________________________________
