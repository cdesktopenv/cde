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
# $XConsortium: book-sml.ts /main/2 1996/07/18 14:21:36 drk $
# ______________________________________________________________________
#
#Var:		logo2cm	/project/tools/libdata/graphics/osflogo/OSF-2cm.eps
Var:		Auth		Open Software Foundation
Var:		OSFaddr1	11 Cambridge Center
Var:		OSFaddr2	Cambridge, MA 02142-1405
#Var:		link_atts	LINKEND LINKENDS
# ______________________________________________________________________
#
GI:		OSF-BOOK
#StartText:	^${_find gi REV-DESCRIPTION 270}^
EndText:	^
-
GI:		TITLE
Context:	OSF-BOOK
StartText:	^'br Start TITLE^
		^.ad r^.PH ""^.PF ""^.S 20^.SP 1.5i^.B "
EndText:	"^.S^.S 12^.SP 2^
		${Auth}^.br^${OSFaddr1}^.br^${OSFaddr2}^.br^
		Doc version: ${_find top gi DOC-ID 1092}^.br^\\*(DT^
		.S^.ad b^.nP^'br End TITLE^
-
GI:		BODY
StartText:	^.bp^'br Start BODY^.pn 1^.af H1 1^
EndText:	^'br End BODY^
-
GI:		SECTION
SpecID:		12
StartText:	^'br section id ${_attval ID 76}^.H 2\s
EndText:	^
-
GI:		CHAPTER
SpecID:		11
StartText:	^'br Start CHAPTER^'br chapter id ${_attval ID 76}^.H 1\s
EndText:	^'br End CHAPTER^
-
# Just output the ID and label tag.  No content.
GI:		_label
SpecID:		76
Replace:	${ID}
-
GI:		DIVISION
Context:	CHAPTER
Action:		12
-
GI:		DIVISION
Context:	APPENDIX
Action:		12
-
GI:		DIVISION
Context:	BODY
Action:		11
-
GI:		DIVISION
Context:	SECTION
SpecID:		13
StartText:	^'br subsection id ${_attval ID 76}^.H 3\s
EndText:	^
-
GI:		DIVISION
Context:	DIVISION SECTION
StartText:	^'br subsubsection id ${_attval ID 76}^.H 4\s
EndText:	^
-
GI:		DIVISION
Context:	DIVISION DIVISION BODY
Action:		13
-
GI:		DIVISION
Context:	DIVISION BODY
Action:		12
-
GI:		DIVISION
Context:	DIVISION
Message:	^Caught deep division (beyond subsubsection)!!\n${_location}
StartText:	^'br subsubsection id ${_attval ID 76}^.H 5\s
EndText:	^
-
GI:		PART
StartText:	^.pA\s
EndText:	^
-
GI:		TITLE
Context:	CHAPTER
StartText:	"
EndText:	"^
-
# let all the other titles default - enclose in quotes, end on newline
#
GI:		APPENDIX
StartText:	^'br appendix id ${_attval ID 76}^
		.pn 1^.af H1 A^.ds |P^.H 1\s
EndText:	^
-
# ______________________________________________________________________
GI:		ROW
StartText:	^${_osftable tbl rowstart}
EndText:	^${_osftable tbl rowend}^
-
GI:		TITLE-ROW
StartText:	^${_osftable tbl rowstart}
EndText:	^.TH^${_osftable tbl rowend}^
-
GI:		CELL
Context:	TITLE-ROW
StartText:	${_osftable tbl cellstart}\\fB
EndText:	\\fR${_osftable tbl cellend}
-
GI:		CELL
StartText:	${_osftable tbl cellstart}
EndText:	${_osftable tbl cellend}
-
GI:		C-CELL
Relation:	sibling+ C-CELL
StartText:	T{^${_osftable tbl cellstart}
EndText:	${_osftable tbl cellend}^T}@
-
# if you want a table title/caption, put it in a display
GI:		TABLE
StartText:	^.TS^${_osftable tbl}^${_osftable tbl top}^
EndText:	^${_osftable tbl bottom}^.TE^
-
#
# ______________________________________________________________________
#
# Synopses
#
# GI:		PROGLANG-SYNOPSIS
# Relation:	child FUNCTION
# StartText:	^'br Proglang synopsis^.sS^
# EndText:	)^.sE^
# -
# GI:		PROGLANG-SYNOPSIS
# StartText:	^'br Proglang synopsis^.sS^
# EndText:	^.sE^
# -
GI:		PROGLANG-SYNOPSIS
StartText:	^.P^
EndText:	^
-
GI:		INCLUDE
StartText:	^
EndText:	^.SP 0.5^
-
# within function
GI:		DATA-DECLARATION
Relation:	sibling- FUNCTION
Relation:	sibling+1 DATA-DECLARATION
StartText:	^\ \ \ \ \ \s
EndText:	,^.br^
-
# within function, last one
GI:		DATA-DECLARATION
Relation:	sibling- FUNCTION
StartText:	^\ \ \ \ \ \s
EndText:	^.br^
-
# GI:		DATA-DECLARATION
# StartText:	^
# EndText:	^
# -
GI:		DATA-DECLARATION
Message:	\nDATA-DECLARATION element no longer supported\n
-
GI:		DATATYPE
Context:	PROGLANG-SYNOPSIS
StartText:	\\fB
EndText:	\\fR
-
GI:		DATATYPE
Context:	DATA-DECLARATION
StartText:	\\fB
EndText:	\\fR
-
GI:		FUNCTION
Context:	PROGLANG-SYNOPSIS
StartText:	\\fB
EndText:	\\fR(^.br^
-
# just on a line by itself
# GI:		FILE-SYNOPSIS
# StartText:	^'br File synopsis^.sS^.P^
# EndText:	^.sE^
# -
GI:		FILE-SYNOPSIS
StartText:	^.P^
EndText:	^
-
# GI:		CMD-SYNOPSIS
# StartText:	^'br Command synopsis^.sS^.P^
# EndText:	^.sE^
# -
GI:		CMD-SYNOPSIS
StartText:	^.P^
EndText:	^
-
# GI:		CMD-ARGUMENT
# AttValue:	PRESENCE OPTIONAL
# AttValue:	REPEATABILITY REPEATABLE
# StartText:	\ [
# EndText:	\ ...]\s
# -
# GI:		CMD-ARGUMENT
# AttValue:	REPEATABILITY REPEATABLE
# StartText:	\s
# EndText:	\ ...
# -
# GI:		CMD-ARGUMENT
# AttValue:	PRESENCE OPTIONAL
# StartText:	\ [
# EndText:	]\s
# -
# GI:		CMD-ARGUMENT
# StartText:	\s
# EndText:	\s
# -
GI:		CMD-ARGUMENT
Message:	\nCMD-ARGUMENT element no longer supported\n
-
# hack - should check value of parent's TYPE attr (for OR)
GI:		OPTION-NAME
Context:	CMD-ARGUMENT
PAttSet:	TYPE
NthChild:	1
StartText:	\ \\fB
EndText:	\\fR\s
-
GI:		OPTION-NAME
Context:	CMD-ARGUMENT
PAttSet:	TYPE
StartText:	\ | \\fB
EndText:	\\fR\s
-

GI:		OPTION-NAME
Context:	CMD-ARGUMENT
StartText:	\ \\fB
EndText:	\\fR\s
-
# ______________________________________________________________________
GI:	COMMAND LITERAL FUNCTION FILE DIRECTORY DATATYPE E-MAIL OPTION-NAME
StartText:	\\*L
EndText:	\\*O
-
GI:		VARIABLE
StartText:	\\*E
EndText:	\\*O
-
GI:		EMPHASIS
StartText:	\\*E
EndText:	\\*O
-
GI:		SUBSCRIPT
StartText:	\\d\\s-4
EndText:	\\s+4\\u
-
GI:		SUPERSCRIPT
StartText:	\\u\\s-4
EndText:	\\s+4\\d
-
GI:		TRADEMARK
StartText:	\\*L
EndText:	\\*O
-
# these are in meta
GI:		OWNER STATEMENT YEAR
Context:	COPYRIGHT-NOTICE
Ignore:		all
-
GI:		NUMBER DATE TIME PERSON PLACE HONORIFIC TELEPHONE YEAR ACRONYM
-
GI:		KEYBOARD-INPUT
StartText:	\\*U
EndText:	\\*O
-
GI:		COMPUTER-OUTPUT
Context:	DISPLAY
#StartText:	^.DS^.nf^.ft CW^
#EndText:	^.ft R^.fi^.DE^
StartText:	^.oS^
EndText:	^.oE^
-
GI:		COMPUTER-OUTPUT
StartText:	\\*C
EndText:	\\*O
-
GI:		KEYBOARD-KEY
StartText:	<\\*L
EndText:	\\*O>
-
GI:		INPUT-INSTRUCT
StartText:	\\*E
EndText:	\\*O
-
GI:		GUI-TEXT
StartText:	\\*L
EndText:	\\*O
-
GI:		FOREIGN-PHRASE
StartText:	\\*E
EndText:	\\*O
-
GI:		LOGICAL-NEGATION
StartText:	__
EndText:	__
-
GI:		MISC-DATA
#StartText:	{
#EndText:	}
-
GI:		MARKUP
AttValue:	LANG sgml
AttValue:	CATEGORY elem.*
SpecID:		619
StartText:	\\*L<
EndText:	>\\*O
-
GI:		MARKUP
AttValue:	CATEGORY (attr|entity)
StartText:	\\*L
EndText:	\\*O
-
GI:		MARKUP
Action:		619
-
GI:		SPECIAL-FORMAT
AttValue:	SMALLCAPS SMALLCAPS
StartText:	{sc\s
EndText:	}
-
GI:		SPECIAL-FORMAT
AttValue:	HORIZKEEP NOBREAK
-
# this will probably never work
GI:		SPECIAL-FORMAT
AttValue:	STYLE
StartText:	^${STYLE}\s
EndText:	^
-
GI:		EXCERPT
StartText:	^.in +1cm^
EndText:	^.in^
-
GI:		QUOTE
StartText:	``
EndText:	''
-
# ______________________________________________________________________
GI:		TEXT-AS-FIGURE
StartText:	^.DS^.na^
EndText:	^.ad^.DE^
-
GI:		TITLE
Context:	DISPLAY
PAttSet:	TYPE FIGURE
StartText:	^.FG "
EndText:	"^
-
GI:		TITLE
Context:	DISPLAY
PAttSet:	TYPE EXAMPLE
StartText:	^.EX "
EndText:	"^
-
GI:		TITLE
Context:	DISPLAY
Relation:	sibling TABLE
StartText:	^.TB "
EndText:	"^
-
GI:		DISPLAY
AttValue:	IN-FLOW FLOAT
Relation:	child TABLE
-
GI:		DISPLAY
Relation:	child COMPUTER-OUTPUT
StartText:	^
EndText:	^
-
GI:		DISPLAY
AttValue:	TYPE TABLE
-
GI:		DISPLAY
AttValue:	WRAP NOWRAP
StartText:	^.DS^.na^
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
StartText:	^.FG "
EndText:	"^
#Message:	\tGraphic - Title - Label\n
-
GI:		DISPLAY
Context:	COMPUTER-OUTPUT
-
GI:		DISPLAY
Context:	KEYBOARD-INPUT
-
# default display
GI:		DISPLAY
StartText:	^.DS^
EndText:	^.DE^
-
GI:		TITLE
Context:	DISPLAY
StartText:	^.ce 1^
EndText:	^
-
# ______________________________________________________________________
# label/item for labeled list
GI:		LABEL
StartText:	^.LI "
EndText:	"^
-
GI:		ITEM
Context:	L-ITEM
StartText:	^
EndText:	^
-
# lablel list title
GI:		LABEL
Context:	L-ITEM-TITLE
StartText:	^.LI "\\fB
EndText:	\\fR"^
-
GI:		ITEM
Context:	L-ITEM-TITLE
StartText:	^\\fB
EndText:	\\fR^
-
# item for general list
GI:		ITEM
SpecID:		343
StartText:	^.LI^
EndText:	^
-
GI:		P
Context:	ITEM
NthChild:	1
StartText:	^
EndText:	^
-
GI:		LIST
Relation:	child L-ITEM
AttValue:	INDENT .*I
StartText:	^.VL ${INDENT:l}^
EndText:	^.LE^
#Message:	-> LABELED LIST\n
-
GI:		LIST
Relation:	child L-ITEM
StartText:	^.VL
EndText:	^.LE^
#Message:	-> LABELED LIST\n
-
# unordered list, but tightened up
GI:		LIST
Relation:	child ITEM
AttValue:	TYPE UNORDERED
AttValue:	VTIGHTNESS TIGHT
SpecID:		341
StartText:	^.ML^
EndText:	^.LE^
-
# default unordered list
GI:		LIST
Relation:	child ITEM
AttValue:	TYPE UNORDERED
SpecID:		340
StartText:	^.ML^
EndText:	^.LE^
-
# default ordered list
GI:		LIST
Relation:	child ITEM
AttValue:	TYPE ORDERED
SpecID:		342
StartText:	^.AL^
EndText:	^.LE^
-
# default general list, but tight
GI:		LIST
Relation:	child ITEM
AttValue:	VTIGHTNESS TIGHT
Action:		341
-
# default general list
GI:		LIST
Relation:	child ITEM
Action:		341
-
# procedure list
GI:		LIST
Relation:	child PROCEDURE-STEP
Action:		342
-
# note list
GI:		LIST
Relation:	child NOTE
StartText:	^
EndText:	^
-
# treat as just items
GI:		PROCEDURE-STEP
Action:		343
-
# not really the right thing...
GI:		BRIDGE-P
StartText:	^.LI^
EndText:	^
Message:	BRIDGE-P...not sure what to do with it.\n
-
GI:		L-ITEM
-
# ______________________________________________________________________
GI:		GRAPHIC
Context:	DISPLAY
StartText:	^.pI ${_filename}^
EndText:	^
-
# can't do in-line graphics in troff...
GI:		GRAPHIC
StartText:	^.pI ${_filename}^
EndText:	^
-
GI:		EQUATION
Context:	DISPLAY
StartText:	^.EQ^
EndText:	^.EN^
-
GI:		EQUATION
#StartText:	$
#EndText:	$
StartText:	^\\fI[skip TeX equation]\\fP^'br\s
EndText:	^
-
# ______________________________________________________________________
GI:		NOTE
Relation:	child TITLE
StartText:	^.nS\s
EndText:	^.nE^
-
GI:		NOTE
StartText:	^.nS Note^
EndText:	^.nE^
-
GI:		TITLE
Context:	NOTE
StartText:	"
EndText:	"^
-
GI:		ANNOTATION
StartText:	^.FS^
EndText:	^.FE^
-
GI:		FOOTNOTE
StartText:	^.FS^
EndText:	^.FE^
-
GI:		NOTEREF
StartText:	\\*F
-
# ______________________________________________________________________
GI:		_
SpecID:		98
-
GI:		REF-NAME
StartText:	^.TH "
EndText:	" ${_pattr CATEGORY}^.SH "Name"^${#action 98} --\s
-
GI:		REF-PURPOSE
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
GI:		OSF-REFPAGE
StartText:	^'br Start OSF-REFPAGE^
EndText:	^'br End OSF-REFPAGE^
-
GI:		META
Context:	OSF-REFPAGE
Ignore:		all
-
# ______________________________________________________________________
#  hopefully, this will be removed from the DTD (NOTE->BRIDGE-TITLE)
GI:		BRIDGE-TITLE
Context:	NOTE
Ignore:		all
Message:	BRIDGE-TITLE inside a note. Not a good thing...\n
-
GI:		BRIDGE-TITLE
StartText:	^.sp 1^.B "
EndText:	"^.sp 1^
-
# ______________________________________________________________________
GI:		MESSAGE
StartText:	^'br Start MESSAGE^.VL^
EndText:	^.LE^'br End MESSAGE^
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
GI:		P
Context:	MSG-DESC
NthChild:	1
Action:		2
-
GI:		MSG-CAUSE
StartText:	^.LI "Cause"^
EndText:	^
-
GI:		P
Context:	MSG-CAUSE
NthChild:	1
Action:		2
-
GI:		MSG-AUDIENCE
StartText:	^.LI "Audience"^
EndText:	^
-
GI:		MSG-RESP
StartText:	^.LI "Response"^
EndText:	^
-
GI:		P
Context:	MSG-RESP
NthChild:	1
Action:		2
-
GI:		MSG-ORIGIN
StartText:	^.LI "Origin"^
EndText:	^
-
# ______________________________________________________________________
# "default" handling of some common and keydata elements
GI:		P
Context:	HY-BIB-DESCRIPTION
-
GI:		P
StartText:	^.P^
EndText:	^
-
GI:		TITLE
StartText:	"
EndText:	"^
-
GI:		ALT-TITLE
Ignore:		all
Message:	Warning: found an ALT-TITLE at:\n${_location}\n
-
GI:		MSG-CODE
StartText:	\\*L
EndText:	\\*O
-
GI:		MSG-TEXT
-
# ______________________________________________________________________
GI:		SUPPLEMENTS
StartText:	^'br Start SUPPLEMENTS^.nr H1 0^
EndText:	^'br End SUPPLEMENTS^
-
# Set page style and page number
# (user should reset page number to agree with the length of the TOC)
GI:		PRELIMINARIES
StartText:	^'br Start PRELIMINARIES^.af P i^.nr P 1^
#		.bp^.af H1 i^.nr N 0^
#		.PH ""^.EH ""^.OH ""^.PF ""^.OF ""^.EF ""^
#		.am TP^.nr !_ 1^..^
EndText:	^'br End PRELIMINARIES^
-
# start of a preface.  the markright thing is a bit of a hack
GI:		PREFACE
StartText:	^'br preface^.H 1\s
EndText:	^
-
GI:		PDIVISION
Context:	PRELIMINARIES
StartText:	^.H 1\s
EndText:	^
-
GI:		PDIVISION
Context:	PDIVISION
StartText:	^.H 2\s
EndText:	^
-
GI:		PDIVISION
StartText:	^.H 2\s
EndText:	^
-
GI:		FOREWARD
StartText:	^.H 1\s
EndText:	^
-
# ______________________________________________________________________
GI:		TOC
AttValue:	CAPHROLE toc.*collect
Context:	NAVIGATION AUGMENTUM OSF-BOOK
Ignore:		all
-
GI:		TOC
AttValue:	CAPHROLE lof.*collect
Context:	NAVIGATION AUGMENTUM OSF-BOOK
Ignore:		all
-
GI:		TOC
AttValue:	CAPHROLE lot.*collect
Context:	NAVIGATION AUGMENTUM OSF-BOOK
Ignore:		all
-
GI:		TOC
Message:	Got unknown type of TOC. CAPHROLE="${CAPHROLE}"\n${_location}
-

GI:		INDEX
StartText:	^`br This document wants an index:\s
EndText:	^
#Message:	INDEX not yet implemented\n
-
GI:		BIBLIOGRAPHY
Ignore:		all
Message:	BIBLIOGRAPHY not yet implemented\n
-
GI:		GLOSSARY
StartText:	^`br This document wants a glossary:\s
EndText:	^
# Message:	GLOSSARY not yet implemented\n
-
# ______________________________________________________________________
GI:		META
StartText:	^'br Start META^.ad l^.fi^
	.P^
	The information contained within this document is subject to change
	without notice.^
	.SP 1^.S 9^
EndText:	^'br End META^
-
GI:		DOC-ID
Ignore:		all
-
#	ignore for now, til we have a good way to present it
GI:		AUTHOR-INFO
Ignore:		all
-
GI:		PERSON
Context:	AUTHOR-INFO
StartText:	^
EndText:	^.br^
-
GI:		AUTH-AFFILIATION
StartText:	^
EndText:	^.br^
-
GI:		ADDRESS
StartText:	^
EndText:	^.br^
-
GI:		VALID-DATE
-
GI:		PRODUCT
StartText:	^'br Product...\s
EndText:	^
-
GI:		PRODUCT-NAME
StartText:	'br Name:\s
EndText:	^
-
GI:		PRODUCT-VERSION
StartText:	^'br Version:\s
EndText:	^
-
GI:		NOTICES COPYRIGHTS
StartText:	^.SP 2^'br ${_gi}^
-
GI:		DISCLAIMER
StartText:	^.P^
EndText:	^
-
GI:		COPYRIGHT-NOTICE
StartText:	^Copyright \\(co ${_followrel child YEAR 1}\s
		${_followrel child OWNER 1} ${_followrel child STATEMENT 1}
EndText:	^.br^
-
GI:		STATEMENT
StartText:	\s
EndText:	^
-
GI:		TRADEMARKS
StartText:	^.SP 2^
-
GI:		TRADEMARK-PAIR
StartText:	^.P^
EndText:	^.br^
-
GI:		PRODUCT-ID
StartText:	^
-
GI:		AUDIENCE
Ignore:		all
-
GI:		REQ-SOFTWARE
-
# ______________________________________________________________________
#
GI:		_Start
StartText:	^'br Translated with ${transpec} by ${user} on ${host}, ${date}^
-
GI:		_End
EndText:	^'br end --------^
-
# ______________________________________________________________________
# Set page number to 1 (roman) for TOC and friends
GI:		AUGMENTUM
StartText:	^'br start augmentum^.bp^.nr x 1^.nr x 1^
-
GI:		NAVIGATION
-
# ______________________________________________________________________
GI:		HY-LOCATOR
Ignore:		all
-
GI:		HY-CONCEPT
Context:	META
Ignore:		all
-
GI:		HY-CONCEPT
Ignore:		all
-
GI:		HY-DEF
Ignore:		all
-
GI:		HY-TERM
-
# ----
GI:		ENTRY
AttValue:	CAPHROLE glossary
StartText:	^.gL\s
		${_chasetogi HY-TERM 18} ${_chasetogi HY-NMLIST 17}^
-
GI:		ENTRY
AttValue:	CAPHROLE index
#Ignore:		all
StartText:	^.iX\s
		${_attval RANGESTART 1206}${_attval RANGEEND 1207}\s
		${_chasetogi HY-TERM 18} ${_chasetogi HY-NMLIST 17}^
#Message:	INDEX ENTRY ==== ${_path}\n
-
GI:		ENTRY
AttValue:	CAPHROLE bib.*
StartText:	\\fI
EndText:	\\fR
-
GI:		ENTRY
-
# ______________________________________________________________________
GI:		XREF
AttValue:	OSFROLE gettitle
Ignore:		all
EndText:	\\fB${_chasetogi TITLE 1}\\fR
-
GI:		XREF
AttValue:	OSFROLE getnumber
EndText:	\ \\s-2xref{${LINKEND}}\\s+2
-
GI:		XREF
AttValue:	OSFROLE getpage
EndText:	\ \\s-2pageref{${LINKEND}}\\s+2
-
# Print GI name, in mixed case
GI:		_pr_gi_name
SpecID:		87
Ignore:		all
EndText:	${_gi M}
-
# GI name
GI:		XREF
AttValue:	OSFROLE gettype
EndText:	${_followlink 87}
-
# name of refpage
GI:		XREF
AttValue:	OSFROLE getrefname
Ignore:		all
EndText:	\\fB${_chasetogi REF-NAME 1}\\fR
-
# title and page number
GI:		XREF
AttValue:	OSFROLE getshort
Ignore:		all
EndText:	\\fB${_chasetogi TITLE 1}\\fR, page ??
-
# section number, title, and page number
GI:		XREF
AttValue:	OSFROLE getfull
#Ignore:	children
StartText:	${_followlink 87}\s
EndText:	\\fB${_chasetogi TITLE 1}\\fR, page ??
#EndText:	${_followlink 87} ref{${LINKEND}},\s
#		\\fB${_chasetogi TITLE 1}\\fR, page ??
-
# Still to do OSFROLE = "getpagerange", "related" (no real action for paper).
#
# For unknown Roles:
GI:		XREF
Message:	Unknown XREF element. Role: ${OSFROLE ???}\n${_path}\n
-
# ______________________________________________________________________
GI:		HY-BIB-DESCRIPTION
StartText:	\\fI
EndText:	\\fR
-
GI:		HY-NMLIST
Ignore:		all
-
# ______________________________________________________________________
#
# index range start and end
GI:		_xx
SpecID:		1206
Replace:	"-["
-
GI:		_xx
SpecID:		1207
Replace:	"-]"
-
GI:		_xx
SpecID:		15
#Message:	Doing _namelist - 1 ${_path}\n
StartText:	\ "
EndText:	"
-
GI:		_xx
SpecID:		16
StartText:	\ "
EndText:	"
#Message:	Doing _namelist - n ${_path}\n
-
GI:		_hy-nmlist
SpecID:		17
Ignore:		all
StartText:	${_namelist 15 16}
#Message:	Done _hy-nmlist element- ${_path}\n
-
GI:		_hy-term
SpecID:		18
StartText:	"
EndText:	"
#Message:	Done _hy-term\n
-
# ______________________________________________________________________
#
# Some named specs, used throughout.
GI:		_pass-text
SpecID:		1
-
# a "paragraph", with no .P
GI:		_no-dot-P
SpecID:		2
StartText:	^
EndText:	^
-
# ______________________________________________________________________
GI:		_rev-desc
SpecID:		270
Ignore:		all
StartText:	^.P^${_find gi OCCURRENCES 271}^
EndText:	^
#Message:	++++ REV DESC done\n
-
GI:		_revs
SpecID:		271
-
GI:		_revs
SpecID:		272
Ignore:		all
StartText:	^'br osfrevstart{}\t'br Start REV - id=${ID}^
-
GI:		_revs
SpecID:		273
Ignore:		all
EndText:	^'br osfrevend{}\t'br End REV - id=${ID}^
-
GI:		_revs
SpecID:		274
Ignore:		all
StartText:	${_insertnode S 272}${_insertnode E 273}
-
# start of changed region
GI:		_st
SpecID:		11275
Ignore:		all
StartText:	${_insertnode S 272}
-
GI:		_st
SpecID:		275
Ignore:		all
StartText:	${_insertnode S 272}
-
#GI:		_st
#SpecID:		275
#Ignore:		all
#StartText:	${_insertnode S 272}
#-
# end of changed region
GI:		_en
SpecID:		276
Ignore:		all
StartText:	${_insertnode E 273}
-
GI:		OCC-ASYNC
StartText:	${_followlink START 275}${_followlink END 276}
-
GI:		OCC-SYNC
StartText:	${_followlink IDREF 274}
-
GI:		REV-BRACKET
StartText:	^'br REV bracket^
-
GI:		I18N-BRACKET REV-DESCRIPTION
-
GI:		OCCURRENCES DESC-POOL
Ignore:		all
-
# ______________________________________________________________________
# Some more named specs...
#
# Print doc version number (doc-version attr of doc-id elem)
GI:		_version-text
SpecID:		1092
StartText:	${DOC-VERSION ??}
Ignore:		all
-
# ______________________________________________________________________
#
#	hacks
#
GI:		_set_chapter
SpecID:		10005
Replace:	^\\setcounter{chapter}{${chapter 1}}^
-
GI:		_set_section
SpecID:		10006
Replace:	^\\setcounter{section}{${section 1}}^
-
# ______________________________________________________________________
#
# Processing Instructions
GI:		_osf-size
Replace:	
-
GI:		_osf-indent
Replace:	
-
GI:		_osf-center
Replace:	
-
GI:		_osf-point-size
StartText:	^.S 
EndText:	^
-
GI:		_osf-space
StartText:	^.sp 
EndText:	^
-
GI:		_osf-break
Replace:	^.br^
-
GI:		_osf-new-page
Replace:	^.nP^
-
GI:		_osf-need
StartText:	^.ne\s
EndText:	
-
GI:		_osf-hyphen
Replace:	\\-
-
# ______________________________________________________________________
