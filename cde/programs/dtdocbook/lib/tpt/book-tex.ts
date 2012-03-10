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
# $XConsortium: book-tex.ts /main/2 1996/07/18 14:21:53 drk $
# ______________________________________________________________________
#
Var:		logo2cm	/project/tools/libdata/graphics/osflogo/OSF-2cm.eps
Var:		Auth		Open Software Foundation
Var:		OSFaddr1	11 Cambridge Center
Var:		OSFaddr2	Cambridge, MA 02142-1405
#Var:		link_atts	LINKEND LINKENDS
# ______________________________________________________________________

GI:		OSF-BOOK
StartText:	^${_find gi REV-DESCRIPTION 270}^
#StartText:	^\n\\begin{document}^\\pagenumbering{roman}^\n
EndText:	
-
GI:		TITLE
Context:	OSF-BOOK
StartText:	^\\pagestyle{empty}^\\null^
		\\vskip 6cm^\\title{
EndText:	}^\\centerline{\\huge\\bf{\\booktitle}}^\\vskip 7cm^
		\\begin{flushright}^${Auth}\\\\^	
		${OSFaddr1}\\\\^${OSFaddr2}\\\\^
		\\vspace{1ex}^
		Doc version: ${_find top gi DOC-ID 1092}\\\\^\\today^
		\\end{flushright}^
		\\vfil^\\leftline{\\epsfbox{${logo2cm}}}^\\pagebreak^\n
-
GI:		BODY
StartText:	^\n\\pagebreak^\\cleardoublepage^\\pagestyle{osfchapheadings}^
		\\pagenumbering{arabic}^\\setcounter{page}{1}^
EndText:	^\n
-
GI:		SECTION
SpecID:		12
StartText:	^\n% --------^\\osfsection{${_attval ID 76}
EndText:	^
#Message:	--------- Process Section ${_path}\n
-
GI:		CHAPTER
SpecID:		11
StartText:	^\n% ----------------^\\chapter{${_attval ID 76}
EndText:	^
#Message:	--------- Process Chapter ${_path}\n
-
# Just output the ID and label tag.  No content.
GI:		_label
SpecID:		76
StartText:	\\label{${ID}}
Ignore:		all
#Message:	--------- Process 76 ${_path}\n
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
StartText:	^\n\\osfsubsection{${_attval ID 76}
EndText:	^
-
GI:		DIVISION
Context:	DIVISION SECTION
StartText:	^\n\\osfsubsubsection{${_attval ID 76}
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
StartText:	^\n\\subsubsection*{
EndText:	^
-
GI:		PART
StartText:	^\n\\part
EndText:	^
-
# chapter title, followed by the per-chap table of contents
GI:		_plain
SpecID:		2
StartText:	^\\item^
-
GI:		_chap-toc
SpecID:		3
		In this chapter:
		^\\begin{itemize}^\\tightenlist^
		${_find gi-parent TITLE SECTION 2}^
		\\end{itemize}^\n
-
GI:		_chap-tocxx
SpecID:		4
		${_followrel parent CHAPTER 3}
-
GI:		TITLE
Context:	CHAPTER
EndText:	}^\n
#		${_relation cousin SECTION 4}
-
GI:		TITLE
Context:	SECTION
EndText:	}
-
GI:		TITLE
Context:	DIVISION
EndText:	}
-
GI:		TITLE
Context:	PDIVISION
EndText:	}
-
GI:		TITLE
Context:	APPENDIX
EndText:	}
-
GI:		TITLE
Context:	PREFACE
Replace:
-
GI:		APPENDIX
StartText:	^\n\\chapter{${_attval ID 76}
EndText:	^
-
#
# ______________________________________________________________________
GI:		ROW
StartText:	^${_osftable tex rowstart}
EndText:	${_osftable tex rowend}^
#EndText:	\\\\ \\hline^
-
GI:		CELL
StartText:	${_osftable tex cellstart}
EndText:	${_osftable tex cellend}
-
GI:		TABLE
Context:	DISPLAY
# The following invocation of 'osftable check' is necessary to initialize
# the 'number of columns' global variable for the TITLE-ROW action.
StartText:	^\\begin{center}^${_osftable check}
		${_followrel cousin TITLE 2199}
		${_followrel descendant TITLE-ROW 2200}
		^\\tabletail{\\hline}
		^\\begin{supertabular}{${_osftable tex}}
		^
EndText:	^\\end{supertabular}^\\end{center}^\n
-
GI:		TABLE
StartText:	^\\begin{center}^${_osftable check}
		${_followrel descendant TITLE-ROW 2200}
		^\\tabletail{${_osftable tex bottom}}
		^\\begin{supertabular}{${_osftable tex}} ${_osftable tex top}^
EndText:	^\\end{supertabular}^\\end{center}^\n
-
GI:		_tabdispID
SpecID:		2198
Replace:	${_attval ID 76}
-
GI:		_tabtitle
SpecID:		2199
StartText:	^\\tablehead{\\hline}^\\tablecaption{
EndText:	}${_followrel parent DISPLAY 2198}^
-
GI:		_tabhead
SpecID:		2200
StartText:	^\\tablehead{\\hline ${_osftable tex rowstart}
EndText:	 \\\\ \\hline }
-
#
# should do something with \tablehead{ ... }, \tabletail{ ... }
#
# ______________________________________________________________________
GI:		DISPLAY
AttValue:	IN-FLOW FLOAT
Relation:	child TABLE
##StartText:	^\n\\begin{table}^
##EndText:	^\\end{table}^\n
-
GI:		DISPLAY
Relation:	child TABLE
##StartText:	^\n\\begin{table}[h]^
##EndText:	^\\end{table}^\n
-
GI:		DISPLAY
AttValue:	WRAP NOWRAP
StartText:	^\\begin{quote}^\\begingroup \\obeylines\\obeyspaces\\tt^
EndText:	^\\endgroup^\\end{quote}^
-
GI:		DISPLAY
AttValue:	TYPE EXAMPLE
StartText:	^\\begin{quote}^\\obeylines \\obeyspaces \\tt^
EndText:	^\\end{quote}^
-
GI:		DISPLAY
AttValue:	TYPE FIGURE
StartText:	^\\begin{figure}^
EndText:	^\\end{figure}^
-
GI:		TITLE
Context:	DISPLAY
Relation:	sibling GRAPHIC
PAttSet:	ID
StartText:	^\\caption{\\label{${_pattr ID}}
EndText:	}^
#Message:	\tGraphic - Title - Label\n
-
GI:		TITLE
Context:	DISPLAY
Relation:	sibling GRAPHIC
StartText:	^\\caption{
EndText:	}^
#Message:	\tGraphic - Title\n
-
# table titles are already taken care of
GI:		TITLE
Context:	DISPLAY
Relation:	cousin TABLE
Ignore:		all
-
# do not display title-rows twice
GI:		TITLE-ROW
Context:	TABLE
Ignore:		all
-
# default display
GI:		DISPLAY
StartText:	^\\begin{quote}^
EndText:	^\\end{quote}^
-
GI:		TITLE
Context:	DISPLAY
StartText:	\\centerline{\\bf\\large{}
EndText:	}
-
GI:		TEXT-AS-FIGURE
StartText:	^\n\\begin{verbatim}^
EndText:	^\\end{verbatim}^\n
-
# ______________________________________________________________________
#
# Synopses
#
# GI:		PROGLANG-SYNOPSIS
# Relation:	child FUNCTION
# StartText:	^\n
# EndText:	)^\n
# -
# -
# GI:		PROGLANG-SYNOPSIS
# StartText:	^\n
# EndText:	^\n
GI:		PROGLANG-SYNOPSIS
StartText:	^\n
EndText:	^
-
GI:		INCLUDE
StartText:	^
EndText:	\\vspace{2ex}\\\\^
-
# within function
# GI:		DATA-DECLARATION
# Relation:	sibling- FUNCTION
# Relation:	sibling+1 DATA-DECLARATION
# StartText:	^\\hspace{1cm}
# EndText:	,\\\\^
# -
# # within function, last one
# GI:		DATA-DECLARATION
# Relation:	sibling- FUNCTION
# StartText:	^\\hspace{1cm}
# EndText:	\\\\^
# -
# GI:		DATA-DECLARATION
# StartText:	^
# EndText:	^
GI:		DATA-DECLARATION
Message:	\nDATA-DECLARATION element not supported\n
-
GI:		DATATYPE
Context:	PROGLANG-SYNOPSIS
StartText:	{\\bf{}
EndText:	}\s
-
GI:		DATATYPE
Context:	DATA-DECLARATION
StartText:	{\\bf{}
EndText:	}
-
GI:		FUNCTION
Context:	PROGLANG-SYNOPSIS
StartText:	{\\bf{}
EndText:	}(\\\\^
-
# just on a line by itself
# GI:		FILE-SYNOPSIS
# StartText:	^\n% File synopsis^
# EndText:	^\n
# -
GI:		FILE-SYNOPSIS
StartText:	^\n
EndText:	^
-
# GI:		CMD-SYNOPSIS
# StartText:	^\n% Command synopsis^
# EndText:	^\n
# -
GI:		CMD-SYNOPSIS
StartText:	^\n
EndText:	^
-
# GI:		CMD-ARGUMENT
# AttValue:	PRESENCE OPTIONAL
# AttValue:	REPEATABILITY REPEATABLE
# StartText:	{ \$\\lbrack\$
# EndText:	\ ...\$\\rbrack\$ }
# -
# GI:		CMD-ARGUMENT
# AttValue:	REPEATABILITY REPEATABLE
# StartText:	{\s
# EndText:	\ ...}
# -
# GI:		CMD-ARGUMENT
# AttValue:	PRESENCE OPTIONAL
# StartText:	{ \$\\lbrack\$
# EndText:	\$\\rbrack\$ }
# -
# GI:		CMD-ARGUMENT
# StartText:	{\s\$\\lbrace\$
# EndText:	\ \$\\rbrace\$}
GI:		CMD-ARGUMENT
Message:	\nCMD-ARGUMENT element no longer supported\n
# -
# hack - should check value of parent's TYPE attr (for OR)
GI:		OPTION-NAME
Context:	CMD-ARGUMENT
PAttSet:	TYPE
NthChild:	1
StartText:	{ \\bf{}
EndText:	\ }
-
GI:		OPTION-NAME
Context:	CMD-ARGUMENT
PAttSet:	TYPE
StartText:	{ \$\\vert\$ \\bf{}
EndText:	\ }
-

GI:		OPTION-NAME
Context:	CMD-ARGUMENT
StartText:	{ \\bf{}
EndText:	\ }
-
# ______________________________________________________________________
GI:		COMMAND LITERAL FUNCTION FILE DIRECTORY DATATYPE E-MAIL
StartText:	{\\bf{}
EndText:	}
-
GI:		VARIABLE
StartText:	{\\it\s
EndText:	}
-
GI:		EMPHASIS
StartText:	{\\em\s
EndText:	}
-
GI:		SUBSCRIPT
StartText:	{\$\\sb{
EndText:	}\$}
-
GI:		SUPERSCRIPT
StartText:	{\$\\sp{
EndText:	}\$}
-
GI:		TRADEMARK
StartText:	{\\bf{}
EndText:	}
-
# these are in meta
GI:		OWNER STATEMENT YEAR
Context:	COPYRIGHT-NOTICE
Ignore:		all
-
GI:		NUMBER DATE TIME PERSON PLACE HONORIFIC TELEPHONE YEAR ACRONYM
-
GI:		KEYBOARD-INPUT
StartText:	{\\tt\s
EndText:	}
-
GI:		COMPUTER-OUTPUT
Context:	DISPLAY
StartText:	^\\begin{verbatim}^
EndText:	^\\end{verbatim}^
-
GI:		COMPUTER-OUTPUT
StartText:	{\\tt\s
EndText:	}
-
GI:		KEYBOARD-KEY
StartText:	{\$<\$\\bf{}
EndText:	\$>\$}
-
GI:		INPUT-INSTRUCT
StartText:	{\\sl\s
EndText:	}
-
GI:		GUI-TEXT
StartText:	{\\bf{}
EndText:	}
-
GI:		FOREIGN-PHRASE
StartText:	{\\it\s
EndText:	}
-
GI:		LOGICAL-NEGATION
StartText:	\$\\overline{
EndText:	}\$
-
GI:		OPTION-NAME
StartText:	{\\bf{}
EndText:	}
-
GI:		MISC-DATA
StartText:	{
EndText:	}
-
GI:		MARKUP
AttValue:	LANG sgml
AttValue:	CATEGORY elem.*
SpecID:		619
StartText:	{\\bf{}\$<\$
EndText:	\$>\$}
-
GI:		MARKUP
AttValue:	CATEGORY (attr|entity)
StartText:	{\\bf{}
EndText:	}
-
GI:		MARKUP
Action:		619
-
GI:		SPECIAL-FORMAT
AttValue:	SMALLCAPS SMALLCAPS
StartText:	{\\sc\s
EndText:	}
-
GI:		SPECIAL-FORMAT
AttValue:	HORIZKEEP NOBREAK
StartText:	\\mbox{
EndText:	}
-
# a bit if a hack
GI:		SPECIAL-FORMAT
AttValue:	STYLE
StartText:	{\\${STYLE}\s
EndText:	}
-
GI:		EXCERPT
StartText:	^\n\\begin{quote}^
EndText:	^\\end{quote}^
-
GI:		QUOTE
StartText:	``
EndText:	''
-
# ______________________________________________________________________
# label/item for labeled list
GI:		LABEL
StartText:	^\\item[{
EndText:	}] \\ ^
-
GI:		ITEM
Context:	L-ITEM
StartText:	^\n
EndText:	^\n
-
# lablel list title
GI:		LABEL
Context:	L-ITEM-TITLE
StartText:	^\\item[{\\bf{}
EndText:	}] \\ ^
-
GI:		ITEM
Context:	L-ITEM-TITLE
StartText:	^\n{\\bf{}
EndText:	}^\n
-
# item for general list
GI:		ITEM
SpecID:		343
StartText:	^\n\\item\s
-
GI:		LIST
Relation:	child L-ITEM
StartText:	^\n\\begin{description}^
EndText:	^\\end{description}^\n
#Message:	-> LABELED LIST\n
-
# unordered list, but tightened up
GI:		LIST
Relation:	child ITEM
AttValue:	TYPE UNORDERED
AttValue:	VTIGHTNESS TIGHT
SpecID:		341
StartText:	^\n\\begin{itemize}^\\tightenlist^
EndText:	^\\end{itemize}^\n
-
# default unordered list
GI:		LIST
Relation:	child ITEM
AttValue:	TYPE UNORDERED
SpecID:		340
StartText:	^\n\\begin{itemize}^
EndText:	^\\end{itemize}^\n
-
# default ordered list
GI:		LIST
Relation:	child ITEM
AttValue:	TYPE ORDERED
SpecID:		342
StartText:	^\n\\begin{enumerate}^
EndText:	^\\end{enumerate}^\n
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
StartText:	^\n
EndText:	^\n
-
# treat as just items
GI:		PROCEDURE-STEP
Action:		343
-
# hack??  I have not tried this yet.
GI:		BRIDGE-P
StartText:	^\\item{}\s
EndText:	^
-
GI:		L-ITEM
-
# ______________________________________________________________________
GI:		GRAPHIC
Context:	DISPLAY
StartText:	^\\begin{center}\\ ^\\epsfbox{${_filename}}
EndText:	^\\end{center}^
-
GI:		GRAPHIC
StartText:	\\epsfbox{${_filename}}
-
GI:		EQUATION
Context:	DISPLAY
StartText:	\\begin{center}^\$\$
EndText:	\$\$\\end{center}^
-
GI:		EQUATION
StartText:	{\$
EndText:	\$}
-
# ______________________________________________________________________
GI:		NOTE
Relation:	child TITLE
StartText:	^\n\\osfnote{\\textwidth}{
EndText:	}^\n
-
GI:		NOTE
StartText:	^\n\\osfnote{\\textwidth}{Note:}{
EndText:	}^\n
-
GI:		TITLE
Context:	NOTE
#StartText:	{
EndText:	}{
-
GI:		P
Context:	NOTE
-
GI:		ANNOTATION
#StartText:	^\\osfannotation{
#EndText:	}^\n
StartText:	\\footnote{
EndText:	}
-
GI:		FOOTNOTE
StartText:	\\footnote{
EndText:	}
-
GI:		P
Context:	ANNOTATION
-
GI:		P
Context:	FOOTNOTE
EndText:	^
-
GI:		NOTEREF
-
# ______________________________________________________________________
GI:		REF-NAME
StartText:	^\\def\\Pname{
EndText:	}^\\section*{Name}^{\\Pname} --\s
-
GI:		REF-PURPOSE
#StartText:
EndText:	^
-
GI:		RSECTION
StartText:	^\n\\section*
EndText:	^
-
GI:		RSUBSECTION
StartText:	^\n\\subsection*
EndText:	^
-
GI:		RDIVISION
StartText:	^\n\\subsection*
EndText:	^
-
GI:		OSF-REFPAGE
-
GI:		META
# Context:	OSF-REFPAGE
Ignore:		all
-
# ______________________________________________________________________
#  hopefully, this will be removed from the DTD (NOTE->BRIDGE-TITLE)
GI:		BRIDGE-TITLE
Context:	NOTE
Ignore:		all
Message:	bridge-title inside a note. Not a good thing...\n
-
GI:		BRIDGE-TITLE
StartText:	^\n\\leftline{\\bf{}
EndText:	}^
-
# ______________________________________________________________________
GI:		MESSAGE
StartText:	^% Message^\\begin{description}^
EndText:	^\\end{description}^% Message end^
-
GI:		MSG-CODE
Context:	MESSAGE
StartText:	^\\item[{Code}]^
EndText:	^
-
GI:		MSG-SYMBOL
StartText:	^\\item[{Symbol}]^
EndText:	^
-
GI:		MSG-TEXT
StartText:	^\\item[{Text}]^
Context:	MESSAGE
EndText:	^
-
GI:		MSG-TYPE
StartText:	^\\item[{Type}]^
EndText:	^
-
GI:		MSG-DESC
StartText:	^\\item[{Description}]^
EndText:	^
-
GI:		MSG-CAUSE
StartText:	^\\item[{Cause}]^
EndText:	^
-
GI:		MSG-AUDIENCE
StartText:	^\\item[{Audience}]^
EndText:	^
-
GI:		MSG-RESP
StartText:	^\\item[{Response}]^
EndText:	^
-
GI:		MSG-ORIGIN
StartText:	^\\item[{Origin}]^
EndText:	^
-
# ______________________________________________________________________
# "default" handling of some common and keydata elements
GI:		P
Context:	HY-BIB-DESCRIPTION
-
GI:		P
StartText:	^\n
EndText:	^
#Message:		PARAGRAPH - ${_path}\n
-
GI:		TITLE
StartText:	{
EndText:	}^
#Message:	Generic TITLE =============== ${_path}\n
-
GI:		ALT-TITLE
Ignore:		all
-
GI:		MSG-CODE
StartText:	{\\bf{}
EndText:	}
-
GI:		MSG-TEXT
StartText:	{
EndText:	}
-
# ______________________________________________________________________
GI:		SUPPLEMENTS
StartText:	^\n\\appendix^
-
GI:		PRELIMINARIES
-
# start of a preface.  the markright thing is a bit of a hack
GI:		PREFACE
StartText:	^\n^\\unnumchapter*{Preface}^\\markright{Preface}^
		\\addtocontents{toc}{\\protect{\\cline {chapter}{Preface}{\\thepage}}}^
		\\thispagestyle{osfunnumheadings}^
EndText:	^
-
GI:		PDIVISION
Context:	PRELIMINARIES
StartText:	^\n\\chapter*
EndText:	^
-
GI:		PDIVISION
Context:	PDIVISION
StartText:	^\n\\prefacesection{
EndText:	^
-
GI:		PDIVISION
StartText:	^\n\\prefacesection{
EndText:	^
-
GI:		FOREWARD
StartText:	^\n\\chapter*
EndText:	^
-
# ______________________________________________________________________
GI:		TOC
AttValue:	CAPHROLE toc.*collect
Context:	NAVIGATION AUGMENTUM OSF-BOOK
StartText:	^\n\\begingroup^\\tightenlist^\\osftableofcontents^
EndText:	^\\endgroup^
-
GI:		TOC
AttValue:	CAPHROLE lof.*collect
Context:	NAVIGATION AUGMENTUM OSF-BOOK
StartText:	^\n\\begingroup^\\tightenlist^\\listoffigures^
EndText:	^\\endgroup^
-
GI:		TOC
AttValue:	CAPHROLE lot.*collect
Context:	NAVIGATION AUGMENTUM OSF-BOOK
StartText:	^\n\\begingroup^\\tightenlist^\\listoftables^
EndText:	^\\endgroup^
-
GI:		TOC
Message:	Got unknown type of TOC. CAPHROLE="${CAPHROLE}"\n${_location}
-

GI:		INDEX
#StartText:	^\n\\makeindex^
StartText:	^%   This document wants an index:\s
EndText:	^
#Message:	INDEX not yet implemented\n
-
GI:		BIBLIOGRAPHY
StartText:	^\n\\makebib^
EndText:	^
Message:	BIBLIOGRAPHY not yet implemented\n
-
GI:		GLOSSARY
StartText:	^%   This document wants a glossary:\s
EndText:	^
Message:	GLOSSARY not yet implemented\n
-
GI:		META
StartText:	^% start meta^\\pagenumbering{roman}^\\pagestyle{osfheadings}
EndText:	^% end meta^
-
GI:		DOC-ID
#StartText:	^.ad r^.S 20^.SP 1.5i^.\\" start doc id^
#EndText:	^.ad b^.br^.\\" end doc id^
-
#	ignore for now, til we have a good way to present it
GI:		AUTHOR-INFO
Ignore:		all
#StartText:	^\\begin{flushright}^
#EndText:	^\\end{flushright}^
-
GI:		PERSON
Context:	AUTHOR-INFO
StartText:	^\\author{
EndText:	}^
-
GI:		AUTH-AFFILIATION
StartText:	^\\author{
EndText:	}^
-
GI:		ADDRESS
StartText:	^
EndText:	\\\\^
-
GI:		VALID-DATE
-
GI:		PRODUCT
StartText:	^% Product...\s
EndText:	^
-
GI:		PRODUCT-NAME
StartText:	% Name...\s
EndText:	^
-
GI:		PRODUCT-VERSION
StartText:	^% Version\s
EndText:	^
-
GI:		NOTICES COPYRIGHTS
StartText:	^% ${_gi}...^
-
GI:		DISCLAIMER
StartText:	^\n
EndText:	^\n
-
GI:		COPYRIGHT-NOTICE
StartText:	^Copyright \\copyright{} ${_followrel child YEAR 1}\s
		${_followrel child OWNER 1} ${_followrel child STATEMENT 1}
EndText:	^.br^
-
GI:		STATEMENT
StartText:	\s
-
GI:		TRADEMARKS
StartText:	^\\vskip 2em
-
GI:		TRADEMARK-PAIR
StartText:	^
EndText:	\\\\^
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
#  For the "book.sty" LaTeX style
#
GI:		_Start
StartText:	^% Translated with ${transpec} by ${user} on ${host}, ${date}^
		^\n\\documentstyle[11pt,osf-book]{book}^
		\\def\\fps@table{h}\\def\\fps@figure{h}	% default placements^
		\\makeindex^
		^\n\\begin{document}^\n
		${_isset chapter 10005}${_isset chapter 10006}^
-
GI:		_End
EndText:	^\\tightenlist^\\input{\\jobname.ind}^\n\\end{document}^\n
-
# ______________________________________________________________________
# Set page number to 1 (roman) for TOC and friends
GI:		AUGMENTUM
StartText:	^% start augmentum^\\cleardoublepage^\\pagenumbering{roman}^
		\\setcounter{page}{1}^\\pagestyle{osfheadings}^
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
StartText:	{
EndText:	}
-
GI:		ENTRY
AttValue:	CAPHROLE index
#Ignore:		all
StartText:	^\\index{${_chasetogi HY-TERM 18}${_chasetogi HY-NMLIST 17}
		${_attval RANGESTART 1206}${_attval RANGEEND 1207}}^
#Message:	INDEX ENTRY ==== ${_path}\n
-
GI:		ENTRY
AttValue:	CAPHROLE bib.*
StartText:	{\\it\s
EndText:	}
-
GI:		ENTRY
-
# ______________________________________________________________________
GI:		XREF
AttValue:	OSFROLE gettitle
Ignore:		all
EndText:	{\\bf ${_chasetogi TITLE 1}}
-
GI:		XREF
AttValue:	OSFROLE getnumber
EndText:	\ \\ref{${LINKEND}}
-
GI:		XREF
AttValue:	OSFROLE getpage
EndText:	\ \\pageref{${LINKEND}}
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
Replace:	{\\bf ${_chasetogi REF-NAME 1}}
-
# title and page number
GI:		XREF
AttValue:	OSFROLE getshort
Replace:	{\\bf ${_chasetogi TITLE 1}}, page \\pageref{${LINKEND}}
-
# section number, title, and page number
GI:		XREF
AttValue:	OSFROLE getfull
Replace:	${_followlink 87} \\ref{${LINKEND}},\s
		{\\bf ${_chasetogi TITLE 1}}, page \\pageref{${LINKEND}}
-
# Still to do OSFROLE = "getpagerange", "related" (no real action for paper).
#
# For unknown Roles:
GI:		XREF
Message:	Unknown XREF element. Role: ${OSFROLE ???}\n${_path}\n
-
# ______________________________________________________________________
GI:		HY-BIB-DESCRIPTION
StartText:	{\\it\s
EndText:	}
-
GI:		HY-NMLIST
Ignore:		all
-
# ______________________________________________________________________
#
# index range start and end
GI:		_xx
Ignore:		all
SpecID:		1206
StartText:	|(
-
GI:		_xx
Ignore:		all
SpecID:		1207
StartText:	|)
-
GI:		_xx
SpecID:		15
#Message:	Doing _namelist - 1 ${_path}\n
-
GI:		_xx
SpecID:		16
StartText:	\!
#Message:	Doing _namelist - n ${_path}\n
-
GI:		_hy-nmlist
SpecID:		17
Ignore:		all
StartText:	${_namelist 15 16}
#StartText:	\\index{${_namelist 15 16}
#EndText:	${_pattval RANGESTART 1206}${_pattval RANGEEND 1207}}
#Message:	Done _hy-nmlist element- ${_path}\n
-
GI:		_hy-term
SpecID:		18
#StartText:	^\\index{
#EndText:	${_pattval RANGESTART 1206}${_pattval RANGEEND 1207}}
#Message:	Done _hy-term\n
-
# ______________________________________________________________________
#
# Some named specs, used throughout.
GI:		_pass-text
SpecID:		1
-
# ______________________________________________________________________
GI:		_rev-desc
SpecID:		270
Ignore:		all
StartText:	^\n${_find gi OCCURRENCES 271}^\n
#Message:	++++ REV DESC done\n
-
GI:		_revs
SpecID:		271
-
GI:		_revs
SpecID:		272
Ignore:		all
StartText:	\\osfrevstart{}\t% start of REV - id=${ID}^
-
GI:		_revs
SpecID:		273
Ignore:		all
EndText:	\\osfrevend{}\t% end of REV - id=${ID}^
-
GI:		_revs
SpecID:		274
Ignore:		all
StartText:	${_insertnode S 272}${_insertnode E 273}
-
GI:		_st
SpecID:		275
Ignore:		all
StartText:	${_insertnode S 272}
-
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
StartText:	^% REV bracket^
#StartText:	\\osfrev{}
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
Replace:	
-
GI:		_osf-point-Ssize
Replace:	
-
GI:		_osf-break
Context:	ITEM
Replace:	
-
GI:		_osf-break
Replace:	\\\\
-
GI:		_osf-space
Context:	ROW
Replace:	
-
GI:		_osf-new-page
Replace:	\\pagebreak^
-
GI:		_osf-need
StartText:	^% NEED
EndText:	^
-
GI:		_osf-hyphen
Replace:	\\-
-
# ______________________________________________________________________
