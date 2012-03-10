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
# $XConsortium: ref-tex.ts /main/2 1996/07/18 14:23:28 drk $
# ______________________________________________________________________
#
Var:		User	!printenv USER
#Var:		Auth	!egrep "^$USER:" /etc/passwd | sed -e 's/,.*//' -e 's/^.*://'
Var:		Auth	Open Software Foundation
#Var:		Date	!date

# ______________________________________________________________________

GI:		AUGMENTUM
StartText:	^\\setcounter{page}{2}^
EndText:	^% recall ${_action 21}^
#Message:	AUGMENTUM\n
-

# ______________________________________________________________________

GI:		OSF-REFPAGE
StartText:	^\n\\begin{document}^
	\\lhead{\\Pname} \\rhead{\\Pname} \\chead{OSF} \\rfoot{\\today}
EndText:	^\n\\end{document}^\n
-
GI:		BODY
StartText:	^\n% start body^\\pagebreak^\\cleardoublepage^
		\\pagenumbering{arabic}^\\setcounter{page}{1}
EndText:	^% end body^
-
GI:		SECTION
StartText:	^\n\\section
EndText:	^
-
GI:		CHAPTER
StartText:	^\n\\chapter
EndText:	^
-
GI:		DIVISION
Context:	SECTION
StartText:	^\n\\subsection
EndText:	^
-
GI:		DIVISION
Context:	DIVISION SECTION
StartText:	^\n\\subsubsection
EndText:	^
-
GI:		DIVISION
Context:	DIVISION
Message:	^Caught deep division (beyond subsubsection)!!\n${_location}
StartText:	^\n\\subsubsection*
EndText:	^
-
GI:		TITLE
Context:	CHAPTER
StartText:	{
EndText:	\\label{${_pattr ID}}}
-
GI:		TITLE
Context:	SECTION
StartText:	{
EndText:	\\label{${_pattr ID}}}
-
# ______________________________________________________________________
GI:		ROW
StartText:	^
EndText:	\\\\ \\hline^
-
GI:		TITLE-ROW
StartText:	^
EndText:	\\\\ \\hline\\hline^
-
GI:		CELL
Context:	TITLE-ROW
SibFollow:	CELL
StartText:	{\\bf\s
EndText:	} &\s
-
GI:		CELL
Context:	TITLE-ROW
StartText:	{\\bf\s
EndText:	} \s
-
GI:		CELL
SibFollow:	CELL
StartText:	{
EndText:	} &\s
-
GI:		CELL
StartText:	{
EndText:	}\s
-
# if you want a table title/caption, put it in a display
GI:		TABLE
StartText:	^\n\\begin{center}^\\begin{tabular}{${HALIGN}} \\hline^
EndText:	^\\end{tabular}^\\end{center}^\n
-
# ______________________________________________________________________
GI:		TEXT-AS-FIGURE
StartText:	^\n\\begin{verbatim}^
EndText:	^\\end{verbatim}^\n
-
GI:		TITLE
Context:	DISPLAY
Sibling:	TABLE
StartText:	^\\caption{
EndText:	}^
-
GI:		DISPLAY
AttValue:	IN-FLOW FLOAT
Child:		TABLE
StartText:	^\n\\begin{table}^
EndText:	^\\end{table}^\n
-
GI:		DISPLAY
Child:		TABLE
StartText:	^\n\\begin{table}[h]^
EndText:	^\\end{table}^\n
-
GI:		DISPLAY
AttValue:	WRAP NOWRAP
StartText:	^\\begin{quote}^\\begingroup \\obeylines\\obeyspaces\\tt\\scriptsize^
EndText:	^\\endgroup^\\end{quote}^
-
GI:		DISPLAY
AttValue:	TYPE FIGURE
StartText:	^\\begin{figure}^
EndText:	^\\end{figure}^
-
GI:		TITLE
Context:	DISPLAY
Sibling:	GRAPHIC
StartText:	^\\caption{
EndText:	}^
-
# default display
GI:		DISPLAY
StartText:	^\\begin{quote}^
EndText:	^\\end{quote}^
-
GI:		TITLE
Context:	DISPLAY
StartText:	\\centerline{\\bf\\large\s
EndText:	}
-
# ______________________________________________________________________
GI:		PROGLANG-SYNOPSIS
StartText:	^\n% Prog lang synopsis^
EndText:	^\n
-
GI:		INCLUDE
StartText:	^
EndText:	\\vspace{2ex}\\\\^
-
GI:		DATA-DECLARATION
StartText:	^{
EndText:	}^
-
GI:		FILE-SYNOPSIS
StartText:	^\n% File synopsis^
EndText:	^\n
-
GI:		CMD-SYNOPSIS
StartText:	^\n% Command synopsis^
EndText:	^\n
-
GI:		CMD-ARGUMENT
StartText:	{
EndText:	}
-
# ______________________________________________________________________
GI:		LITERAL
StartText:	{\\bf\s
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
GI:		ACRONYM
-
GI:		TRADEMARK
StartText:	{\\bf\s
EndText:	}
-
GI:		PERSON
-
GI:		PLACE
-
GI:		HONORIFIC
-
GI:		DATE
-
GI:		TIME
-
GI:		TELEPHONE
-
# yes, a hack...
GI:		YEAR
Context:	COPYRIGHT-NOTICE
StartText:	\\def\\copYr{
EndText:	}
-
GI:		YEAR
-
GI:		E-MAIL
StartText:	{\\bf\s
EndText:	}
-
GI:		NUMBER
-
GI:		VARIABLE
StartText:	{\\it\s
EndText:	}
-
GI:		COMMAND
StartText:	{\\bf\s
EndText:	}
-
GI:		FUNCTION
StartText:	{\\bf\s
EndText:	}
-
GI:		FILE
StartText:	{\\bf\s
EndText:	}
-
GI:		DIRECTORY
StartText:	{\\bf\s
EndText:	}
-
GI:		DATATYPE
StartText:	{\\bf\s
EndText:	}
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
StartText:	{\$<\$\\bf\s
EndText:	\$>\$}
-
GI:		INPUT-INSTRUCT
StartText:	{\\sl\s
EndText:	}
-
GI:		GUI-TEXT
StartText:	{\\bf\s
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
StartText:	{\\bf\s
EndText:	}
-
GI:		MISC-DATA
StartText:	{
EndText:	}
-
GI:		MARKUP
AttValue:	LANG sgml
AttValue:	CATEGORY elem.*
StartText:	{\\bf\s\$<\$
EndText:	\$>\$}
-
GI:		MARKUP
AttValue:	CATEGORY (attr|entity)
StartText:	{\\bf\s
EndText:	}
-
GI:		MARKUP
StartText:	{\\bf\s\$<\$
EndText:	\$>\$}
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
StartText:	''
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
# item for general list
GI:		ITEM
StartText:	^\n\\item\s
-
GI:		LIST
Child:		L-ITEM
StartText:	^\n\\begin{description}^
EndText:	^\\end{description}^\n
#Message:	-> LABELED LIST\n
-
GI:		LIST
Child:		ITEM
AttValue:	TYPE UNORDERED
StartText:	^\n\\begin{itemize}^
EndText:	^\\end{itemize}^\n
-
GI:		LIST
Child:		ITEM
AttValue:	TYPE ORDERED
StartText:	^\n\\begin{enumerate}^
EndText:	^\\end{enumerate}^\n
-
# default general list
GI:		LIST
Child:		ITEM
StartText:	^\n\\begin{itemize}^
EndText:	^\\end{itemize}^\n
-
# procedure list
GI:		LIST
Child:		PROCEDURE-STEP
StartText:	^\n\\begin{itemize}^
EndText:	^\\end{itemize}^\n
-
GI:		PROCEDURE-STEP
StartText:	^\\item\s
-
# hack??  I have not tried this yet.
GI:		BRIDGE-P
StartText:	^\\item{}\s
EndText:	^
-
GI:		L-ITEM
-
# ______________________________________________________________________
GI:		NOTE
Child:		TITLE
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
Context:	OSF-REFPAGE
Ignore:		all
-
# ______________________________________________________________________
GI:		BRIDGE-TITLE
StartText:	^\n\\leftline{\\bf
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
StartText:	^\n
EndText:	^
-
GI:		MSG-CODE
StartText:	{\\bf\s
EndText:	}
-
GI:		MSG-TEXT
StartText:	{
EndText:	}
-
GI:		TITLE
StartText:	{
EndText:	}^
-
GI:		ALT-TITLE
Ignore:		all
-
# ______________________________________________________________________
GI:		TOC
Context:	NAVIGATION AUGMENTUM OSF-BOOK
StartText:	^\n\\begingroup^\\tightenlist^\\tableofcontents^\\endgroup^
EndText:	^
-
GI:		INDEX
#StartText:	^\n\\makeindex^
StartText:	^%   This document wants an index:\s
EndText:	^
-
GI:		BIBLIOGRAPHY
StartText:	^\n\\makebib^
EndText:	^
-
GI:		GLOSSARY
StartText:	^%   This document wants a glossary:\s
EndText:	^
-
# ______________________________________________________________________
# Set page number to 1 (roman) for TOC and friends
GI:		AUGMENTUM
Context:	OSF-BOOK
StartText:	^\\setcounter{page}{1}^
EndText:	^
-
GI:		NAVIGATION
-
GI:		HY-LOCATOR
Ignore:		all
-
# 				ignore until we know how to process these
GI:		HY-CONCEPT
Context:	META
Ignore:		all
-
GI:		HY-CONCEPT
Ignore:		all
-
GI:		HY-DEF
-
GI:		HY-TERM
-
# ----
GI:		ENTRY
AttValue:	DAVROLE glossary
StartText:	{
EndText:	}
-
GI:		ENTRY
AttValue:	DAVROLE index
StartText:	{
EndText:	}
#StartText:	\\index{
#EndText:	}
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
EndText:	\ \\ref{${LINKEND}}
-
GI:		XREF
AttValue:	OSFROLE getpage
EndText:	\ \\pageref{${LINKEND}}
-
GI:		XREF
-
# ----
GI:		HY-BIB-DESCRIPTION
StartText:	{\\it\s
EndText:	}
-
# ______________________________________________________________________
GI:		META
StartText:	^% start meta^
EndText:	^% end meta^
-
GI:		DOC-ID
#StartText:	^.ad r^.S 20^.SP 1.5i^.\\" start doc id^
#EndText:	^.ad b^.br^.\\" end doc id^
-
GI:		AUTHOR-INFO
StartText:	^\\begin{flushright}^
EndText:	^\\end{flushright}^
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
StartText:	^% PRODUCT...\s
EndText:	^
-
GI:		PRODUCT-NAME
StartText:	^% PRODUCT-NAME...\s
EndText:	^
-
GI:		PRODUCT-VERSION
StartText:	^%\s
-
GI:		NOTICES
StartText:	^
-
GI:		DISCLAIMER
StartText:	^\n
EndText:	^\n
-
GI:		COPYRIGHTS
StartText:	^
-
GI:		COPYRIGHT-NOTICE
EndText:	^
-
GI:		OWNER
StartText:	\\def\\copOwn{
EndText:	}
-
# yes, a hack...
GI:		STATEMENT
Context:	COPYRIGHT-NOTICE
StartText:	^Copyright \\copyright{} \\copYr{} \\copOwn\\hfill\\\\
EndText:	^
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
StartText:	^\n
EndText:	^
-
# ______________________________________________________________________
GI:		REV-BRACKET
StartText:	\\osfrev{}
-
GI:		I18N-BRACKET
-
GI:		REV-DESCRIPTION
-
GI:		OCCURRENCES
-
GI:		OCC-ASYNC
-
GI:		OCC-SYNC
-
GI:		DESC-POOL
Ignore:		all
-
GI:		REV-DESCRIPTION
-
# ______________________________________________________________________
#GI:		GRAPHIC
#StartText:	${NOTATION} graphic from file ${NAME} goes here.
#EndText:	^
#-
GI:		GRAPHIC
Context:	DISPLAY
StartText:	^\\begin{center}\\ ^\\psfig{figure=${_filename}}
EndText:	^\\end{center}^
-
GI:		GRAPHIC
StartText:	\\psfig{figure=${_filename}}
#EndText:	}
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
#
#  For the "book.sty" LaTeX style
#
GI:		_Start
StartText:	^\n\\documentstyle[11pt,osf-extras,osf-ref]{article}^
-
GI:		_End
EndText:	^
-
GI:		_Comment
StartText:	^%\s
EndText:	^
-
# ______________________________________________________________________
