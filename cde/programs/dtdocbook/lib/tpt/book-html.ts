# Copyright (c) 1993 Open Software Foundation, Inc.
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
# ______________________________________________________________________
#
# book (and refpage) to HTML
#
# $XConsortium: book-html.ts /main/2 1996/07/18 14:21:17 drk $
# ______________________________________________________________________
#
Var:		Auth	Open Software Foundation

Var:		part	1
Var:		chap	1
Var:		sect	1
Var:		subsect	1
Var:		app	1
# ______________________________________________________________________
GI:		TITLE
Context:	DISPLAY
Relation:	sibling TABLE
StartText:	^<P><B>
EndText:	</B>^\n
-
GI:		DISPLAY
Relation:	child TABLE
StartText:	^<BLOCKQUOTE>^${_attval ID 76}^
EndText:	^${_attval ID 77}</BLOCKQUOTE>^
-
GI:		DISPLAY
AttValue:	WRAP NOWRAP
StartText:	^\n<BLOCKQUOTE>^<PRE>^${_attval ID 76}^
EndText:	^</PRE>^${_attval ID 77}</BLOCKQUOTE>^\n
-
GI:		TITLE
Context:	DISPLAY
Relation:	sibling GRAPHIC
StartText:	^<P><B>
EndText:	</B>^\n
-
# default display
GI:		DISPLAY
StartText:	^<!-- start of display -->^<BLOCKQUOTE>^${_attval ID 76}^
EndText:	^${_attval ID 77}</BLOCKQUOTE>^<!-- end of display -->^\n
-
GI:		TITLE
Context:	DISPLAY
StartText:	^
EndText:	^
-
GI:		TEXT-AS-FIGURE
StartText:	^\n<PRE>^
EndText:	^</PRE>^\n
-
# ______________________________________________________________________
GI:		PROGLANG-SYNOPSIS
StartText:	^\n<P>
EndText:	^\n
-
GI:		INCLUDE
StartText:	^
EndText:	^
-
# GI:		DATA-DECLARATION
# StartText:	^
# EndText:	^
#-
GI:		DATA-DECLARATION
Message:	\nDATA-DECLARATION element not supported\n
-
GI:		FILE-SYNOPSIS
StartText:	^\n<P>
EndText:	^\n
-
GI:		CMD-SYNOPSIS
StartText:	^\n<P>
EndText:	^\n
-
# GI:		CMD-ARGUMENT
# StartText:	\s
# EndText:	\s
# -
GI:		CMD-ARGUMENT
Message:	\nCMD-ARGUMENT element no longer supported\n
-
# ______________________________________________________________________
GI:		LITERAL COMMAND FUNCTION FILE DIRECTORY DATATYPE E-MAIL OPTION-NAME
StartText:	<B>
EndText:	</B>
-
GI:		VARIABLE
StartText:	<VAR>
EndText:	</VAR>
-
GI:		EMPHASIS
StartText:	<EM>
EndText:	</EM>
-
GI:		SUBSCRIPT
StartText:	subscript(
EndText:	)
-
GI:		SUPERSCRIPT
StartText:	superscript(
EndText:	)
-
GI:		TRADEMARK
StartText:	<B>
EndText:	</B>
-
GI:		PERSON PLACE HONORIFIC DATE TIME TELEPHONE NUMBER ACRONYM
-
GI:		KEYBOARD-INPUT
StartText:	<KBD>
EndText:	</KBD>
-
GI:		COMPUTER-OUTPUT
Relation:	ancestor DISPLAY
StartText:	^<LISTING>^
EndText:	^</LISTING>^
-
GI:		COMPUTER-OUTPUT
StartText:	<SAMP>
EndText:	</SAMP>
-
GI:		KEYBOARD-KEY
StartText:	<TT>&lt;
EndText:	&gt;</TT>
-
GI:		INPUT-INSTRUCT
StartText:	<I>
EndText:	</I>
-
GI:		GUI-TEXT
StartText:	<B>
EndText:	</B>
-
GI:		FOREIGN-PHRASE
StartText:	<EM>
EndText:	</EM>
-
GI:		LOGICAL-NEGATION
StartText:	^.ul 1^
EndText:	^
-
GI:		MISC-DATA
-
GI:		MARKUP
AttValue:	LANG sgml
AttValue:	CATEGORY elem.*
StartText:	<B>&lt;
EndText:	&gt;</B>
-
GI:		MARKUP
AttValue:	CATEGORY (attr|entity)
StartText:	<B>
EndText:	</B>
-
GI:		MARKUP
StartText:	<B>&lt;
EndText:	&gt;</B>
-
GI:		SPECIAL-FORMAT
AttValue:	SMALLCAPS SMALLCAPS
StartText:	[
EndText:	]
-
GI:		SPECIAL-FORMAT
AttValue:	HORIZKEEP NOBREAK
StartText:	^.DS^
EndText:	^.DE^
-
# a bit if a hack
GI:		SPECIAL-FORMAT
AttValue:	STYLE .
StartText:	[\\${STYLE}\s
EndText:	]
-
GI:		EXCERPT
StartText:	^<BLOCKQUOTE>
EndText:	</BLOCKQUOTE>^
-
GI:		QUOTE
StartText:	``
StartText:	''
-
# ______________________________________________________________________
GI:		P
Context:	ITEM
StartText:	^
EndText:	^
-
# label/item for labeled list
GI:		LABEL
StartText:	^<DT>
EndText:	^
-
GI:		ITEM
Context:	L-ITEM
StartText:	^<DD>
EndText:	^
-
# item for general list
GI:		ITEM
StartText:	^<LI>
EndText:	^
-
GI:		LIST
Relation:	child L-ITEM
StartText:	^<DL>^
EndText:	^</DL>^
-
GI:		LIST
Relation:	child ITEM
AttValue:	TYPE UNORDERED
StartText:	^<UL>^
EndText:	^</UL>^
-
GI:		LIST
Relation:	child ITEM
AttValue:	TYPE ORDERED
StartText:	^<OL>^
EndText:	^</OL>^
-
# default general list
GI:		LIST
Relation:	child ITEM
StartText:	^<UL>^
EndText:	^</UL>^
-
# procedure list
GI:		LIST
Relation:	child PROCEDURE-STEP
StartText:	^<UL>^
EndText:	^.LE^
-
GI:		PROCEDURE-STEP
StartText:	^<LI>
EndText:	^
-
# note list
GI:		LIST
Relation:	child NOTE
StartText:	^
EndText:	^
-
# hack??  I have not tried this yet.
GI:		BRIDGE-P
StartText:	^.P^
EndText:	^
-
GI:		L-ITEM
EndText:	^
-
# ______________________________________________________________________
GI:		ROW
StartText:	^
EndText:	^
-
GI:		TITLE-ROW
StartText:	^
EndText:	^
-
GI:		CELL
Context:	TITLE-ROW
Relation:	sibling+ CELL
#StartText:	
EndText:	\ |\s
-
GI:		CELL
Context:	TITLE-ROW
#StartText:	
#EndText:	
-
GI:		CELL
Relation:	sibling+ CELL
#StartText:	
EndText:	\ |\s
-
GI:		CELL
#StartText:	
#EndText:	
-
# if you want a table title/caption, put it in a display
GI:		TABLE
StartText:	^\n<!-- Table -->^<PRE>^
		Table Markup omitted.
EndText:	^</PRE>^\n
Ignore:	all
-
# ______________________________________________________________________
GI:		NOTE
Relation:	child TITLE
StartText:	^<BLOCKQUOTE>^
EndText:	^</BLOCKQUOTE>^
-
GI:		NOTE
StartText:	^<BLOCKQUOTE>^<B>Note</B>^
EndText:	^</BLOCKQUOTE>^
-
GI:		TITLE
Context:	NOTE
StartText:	^<B>
EndText:	</B>^
-
GI:		ANNOTATION
StartText:	^<BLOCKQUOTE>^<B>Annotation</B>^
EndText:	^</BLOCKQUOTE>^
-
GI:		FOOTNOTE
StartText:	^<BLOCKQUOTE>^<B>Footnote</B>^
EndText:	^</BLOCKQUOTE>^
-
GI:		NOTEREF
-
# ______________________________________________________________________
GI:		_
SpecID:		98
-
GI:		REF-NAME
Relation:	ancestor BODY
StartText:	<H2>Name</H2>^<P>
EndText:	\ --\s
SpecID:		19
-
GI:		REF-NAME
Relation:	ancestor SUPPLEMENTS
Action:		19
-
GI:		REF-NAME
StartText:	^<TITLE>
EndText:	</TITLE>^\n<H2>Name</H2>^<P>${_action 98} --\s
#EndText:	" ${_pattr CATEGORY}^.SH "Name"^${_action 98} --\s
-
GI:		REF-PURPOSE
#StartText:
EndText:	^\n
-
GI:		TITLE
Context:	RSECTION
StartText:	^<H2>
EndText:	</H2>^
-
GI:		TITLE
Context:	RSUBSECTION
StartText:	^<H3>
EndText:	</H3>^
-
GI:		TITLE
Context:	RDIVISION
StartText:	^<H3>
EndText:	</H3>^
-
GI:		RSECTION
StartText:	^
EndText:	^
-
GI:		RSUBSECTION
StartText:	^
EndText:	^
-
GI:		RDIVISION
StartText:	^
EndText:	^
-
GI:		OSF-REFPAGE
StartText:	^<!-- Start OSF-REFPAGE -->
EndText:	^<!-- End OSF-REFPAGE -->^
-
# ______________________________________________________________________
GI:		MESSAGE
StartText:	^<!-- Message -->^<DL>^
EndText:	^</DL>^
-
GI:		MSG-CODE
Context:	MESSAGE
StartText:	^<DT><B>Code</B></DT>^<DD>
EndText:	^
-
GI:		MSG-SYMBOL
StartText:	^<DT><B>Symbol</B></DT>^<DD>
EndText:	^
-
GI:		MSG-TEXT
StartText:	^<DT><B>Text</B></DT>^<DD>
Context:	MESSAGE
EndText:	^
-
GI:		MSG-TYPE
StartText:	^<DT><B>Type</B></DT>^<DD>
EndText:	^
-
GI:		MSG-DESC
StartText:	^<DT><B>Description</B></DT>^<DD>
EndText:	^
-
GI:		MSG-CAUSE
StartText:	^<DT><B>Cause</B></DT>^<DD>
EndText:	^
-
GI:		MSG-AUDIENCE
StartText:	^<DT><B>Audience</B></DT>^<DD>
EndText:	^
-
GI:		MSG-RESP
StartText:	^<DT><B>Response</B></DT>^<DD>
EndText:	^
-
GI:		MSG-ORIGIN
StartText:	^<DT><B>Origin</B></DT>^<DD>
EndText:	^
-
GI:		P
Context:	MSG-DESC
NthChild:	1
Action:		2
-
GI:		P
Context:	MSG-CAUSE
NthChild:	1
Action:		2
-
GI:		P
Context:	MSG-RESP
NthChild:	1
Action:		2
-
# ______________________________________________________________________
GI:		TITLE
Context:	OSF-BOOK
StartText:	^<TITLE>
EndText:	</TITLE>^
-
# ______________________________________________________________________
# "default" handling of some common and keydata elements
GI:		P
StartText:	^<P>^
EndText:	^\n
-
GI:		MSG-CODE
StartText:	<B>
EndText:	</B>
-
GI:		MSG-TEXT
#StartText:	{
#EndText:	}
-
# ______________________________________________________________________
GI:		TOC
Context:	NAVIGATION AUGMENTUM OSF-BOOK
StartText:	^\n
EndText:	^
-
GI:		INDEX
Replace:	^<!--  This document wants an index -->
Ignore:		all
-
GI:		BIBLIOGRAPHY
Replace:	^<!--  This document wants a bibliography -->
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
StartText:	^
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
StartText:	^.gL "
EndText:	"^
-
# (check for some content to avoud empty tag)
GI:		ENTRY
Content:	.
AttValue:	DAVROLE index
StartText:	${_attval ID 76}
EndText:	${_attval ID 77}
Ignore:		children
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
EndText:	\ <A HREF="#${LINKEND}">go there</A>
-
GI:		XREF
AttValue:	OSFROLE getpage
EndText:	\ <A HREF="#${LINKEND}">go there</A>
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
Replace:	<B>${_chasetogi REF-NAME 1}</B>
-
# title and page number -- page is the input file
GI:		XREF
AttValue:	OSFROLE getshort
Replace:	<B>${_chasetogi TITLE 1}</B>, <A HREF="${_infile}">page</A>
-
# section number, title, and page number
GI:		XREF
AttValue:	OSFROLE getfull
Replace:	${_followlink 87} <A HREF="#${LINKEND}">go there</A>,\s
		<B>${_chasetogi TITLE 1}</B>, <A HREF="${_infile}">page</A>
-
GI:		XREF
-
# ----
GI:		HY-BIB-DESCRIPTION
StartText:	<I>
EndText:	</I>
-
GI:		HY-NMLIST
Ignore:		all
-
# ______________________________________________________________________
GI:		META
Ignore:		all
-
GI:		DOC-ID
Ignore:		all
-
GI:		AUTHOR-INFO
StartText:	^<ADDRESS>^
EndText:	^</ADDRESS>^
-
GI:		PERSON
Context:	AUTHOR-INFO
StartText:	^
EndText:	^
-
GI:		AUTH-AFFILIATION ADDRESS
StartText:	^
EndText:	^
-
GI:		PRODUCT
Context:	MESSAGE
Ignore:		all
-
GI:		VALID-DATE PRODUCT
-
GI:		PRODUCT-NAME PRODUCT-VERSION
StartText:	\s
EndText:	\s
-
GI:		NOTICES DISCLAIMER COPYRIGHTS
StartText:	^\n
EndText:	^\n
-
GI:		OWNER STATEMENT YEAR
Context:	COPYRIGHT-NOTICE
Ignore:		all
-
GI:		COPYRIGHT-NOTICE
StartText:	^Copyright (c) ${_followrel child YEAR 1}\s
		${_followrel child OWNER 1} ${_followrel child STATEMENT 1}
EndText:	^.br^
-
GI:		STATEMENT
StartText:	\s
-
GI:		TRADEMARKS
StartText:	^<P>
-
GI:		TRADEMARK-PAIR
StartText:	^<P>
EndText:	^
-
#GI:		PRODUCT-ID
#StartText:	^
#-
GI:		AUDIENCE
StartText:	^<P>
EndText:	^
-
# ______________________________________________________________________
GI:		REV-BRACKET
StartText:	^<!-- REV bracket -->^
-
GI:		I18N-BRACKET REV-DESCRIPTION OCCURRENCES OCC-ASYNC OCC-SYNC
-
GI:		DESC-POOL
Ignore:		all
-
GI:		REV-DESCRIPTION
Ignore:		all
-
# ______________________________________________________________________
GI:		GRAPHIC
Context:	DISPLAY
#StartText:	^<IMG SRC="${_filename}">^
Replace:	^<P><A HREF="${_filename}">Click for PostScript image.</A><P>^
-
GI:		GRAPHIC
Replace:	^<A HREF="${_filename}">Click for PostScript image.</A>^
-
GI:		EQUATION
Context:	DISPLAY
StartText:	^<BLOCKQUOTE>^
EndText:	^</BLOCKQUOTE>^
-
GI:		EQUATION
StartText:	^<P>TeX equatin:\s
EndText:	^<P>
-
# ______________________________________________________________________
GI:		_anch-title-start
SpecID:		71
Replace:	${_attval ID 76}
-
GI:		_anch-title-end
SpecID:		72
Replace:	${_attval ID 77}
-
GI:		TITLE
Context:	SECTION
SpecID:		12
StartText:	^<H2>${chap}.${sect} ${_followrel parent SECTION 71}
EndText:	${_followrel parent SECTION 72}</H2>^
-
GI:		TITLE
Context:	CHAPTER
SpecID:		11
StartText:	^<H1>Chapter ${chap}: ${_followrel parent CHAPTER 71}
EndText:	${_followrel parent CHAPTER 72}</H1>^
-
GI:		TITLE
Context:	PART
SpecID:		14
StartText:	^<H1>Part ${part}:\s
EndText:	</H1>^
-
GI:		TITLE
Context:	DIVISION CHAPTER
Action:		12
-
GI:		TITLE
Context:	DIVISION APPENDIX
Action:		12
-
GI:		TITLE
Context:	DIVISION BODY
Action:		11
-
GI:		TITLE
Context:	DIVISION SECTION
SpecID:		13
Increment:	subsect
StartText:	^<H3>${chap}.${sect}.${subsect}\s
EndText:	</H3>^
-
GI:		TITLE
Context:	DIVISION DIVISION SECTION
StartText:	^<H4>
EndText:	</H4>^
-
GI:		TITLE
Context:	DIVISION
Context:	DIVISION DIVISION BODY
Action:		13
-
GI:		TITLE
Context:	DIVISION DIVISION BODY
Action:		12
-
GI:		TITLE
Context:	DIVISION BODY
Action:		11
-
GI:		TITLE
Context:	DIVISION DIVISION
StartText:	^<H4>
EndText:	</H4>^
-
GI:		CHAPTER
StartText:	^<!-- Start CHAPTER: ch-${chap}: ${_followrel child TITLE 1} -->^
EndText:	^<!-- End CHAPTER -->^
Increment:	chap
Set:		sect	1
-
GI:		SECTION
StartText:	^<!-- Start SECTION: ${_followrel child TITLE 1} -->^
EndText:	^<!-- End SECTION -->^
Increment:	sect
Set:		subsect	1
-
GI:		DIVISION
StartText:	^
EndText:	^
-
GI:		PART
StartText:	^<!-- Start PART: -: ${_followrel child TITLE 1} -->^
EndText:	^<!-- End PART -->^
Increment:	part
#Var:		chap	1
-

GI:		PREFACE
StartText:	^<!-- Start PREFACE: preface: ${_followrel child TITLE 1} -->^
EndText:	^<!-- End PREFACE -->^
-
GI:		PDIVISION
StartText:	^
EndText:	^
-
GI:		TITLE
Context:	PREFACE
StartText:	^<H1>${_followrel parent PREFACE 71}
EndText:	${_followrel parent PREFACE 72}</H1>^
-
GI:		TITLE
Context:	PDIVISION
StartText:	^<H2>${_followrel parent PDIVISION 71}
EndText:	${_followrel parent PDIVISION 72}</H2>^
-

GI:		TITLE
Context:	APPENDIX
StartText:	^<H1>Appendix ${app}: ${_followrel parent APPENDIX 71}
EndText:	${_followrel parent APPENDIX 72}</H1>^
-
GI:		APPENDIX
StartText:	^<!-- Start PREFACE: app-${app}: ${_followrel child TITLE 1} -->^
EndText:	^<!-- End APPENDIX -->^
Increment:	app
Var:		sect	1
-
GI:		SUPPLEMENTS
StartText:	^<!-- Start SUPPLEMENTS -->
EndText:	^<!-- End SUPPLEMENTS -->^
-
GI:		BODY
StartText:	^
EndText:	^
StartText:	^<!-- Start BODY -->
EndText:	^<!-- End BODY -->^
-
GI:		OSF-BOOK
StartText:	^
EndText:	^
-
# ______________________________________________________________________

GI:		TITLE
#StartText:	"
#EndText:	^
Message:	+++ TITLE??\n${_location}\n
-
GI:		ALT-TITLE
Ignore:		all
#StartText:	^... Alt title:\s
#EndText:	^
-

GI:		AUGMENTUM
Ignore:		all
-
GI:		PRELIMINARIES
StartText:	^<!-- Start PRELIMINARIES -->
EndText:	^<!-- End PRELIMINARIES -->^
-
GI:		YEAR
-
# ______________________________________________________________________
GI:		BRIDGE-TITLE
StartText:	^\n<B>
EndText:	</B>^
-
# ______________________________________________________________________
# Some named specs, used throughout.
GI:		_pass-text
SpecID:		1
-
# 
GI:		_osf-size
Replace:	
-
GI:		_osf-indent
Replace:	
-
GI:		_osf-center
Replace:	
-
GI:		_osf-break
Replace:	
-
GI:		_osf-point-size
Replace:	
-
GI:		_osf-space
Replace:	
-
GI:		_osf-new-page
Replace:	
-
# a "paragraph", with no .P
GI:		_no-dot-P
SpecID:		2
StartText:	^
EndText:	^
-
# Just output the anchor tag and ID.  No content.
GI:		_name
SpecID:		76
StartText:	<A NAME="${ID id}">
Ignore:		all
-
GI:		_name-end
SpecID:		77
StartText:	</A>
Ignore:		all
-
# ______________________________________________________________________
#
GI:		_Start
StartText:	^
-
GI:		_End
EndText:	^
-
# ______________________________________________________________________
