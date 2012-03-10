GI:		_Start
StartCode:	source @{_env DBKTCL_DIR}docbook.tcl
		^set TOSS_PATH "@{_env TPT_LIB}"
		^set LOCALE_STRING_DIR "@{_env LOCALE_DIR}"
		^set NO_UNIQUE_ID "@{_env _DTHELPTAG_NO_UNIQUE_ID}"
-
GI:		_End
StartCode:	CloseDocument
-
GI:		PART
StartText:	@{_followrel child PARTINTRO 5}
		@{_action 4}
		@{_followrel child TITLE 12}
		@{_action 13}
		@{_find top gi FOOTNOTE 3}
		@{_find gi APPENDIX 8}
EndCode:	EndPart
SpecID:	1010
-
GI:		DOCINFO
Relation:	parent PART
-
GI:		DOCINFO ARTICLE
GI:		SETINDEX TOC LOT
Ignore:		all
-
GI:		SET BOOK
Relation:	descendant PART
StartText:	@{_followrel descendant PART 1010}
Ignore:		all
-
GI:		SET BOOK
StartCode:	UserError "A @{_gi} was found but it contains no PART" yes
		^exit 1
Ignore:		all
-
######################################################################
#                          TITLES
######################################################################
GI:		TITLE
Relation:	parent DOCINFO
StartCode:	StartNewVirpage TITLE SDL-RESERVED-TITLE
		^Title @{ID ""} TITLE
EndCode:	CloseTitle TITLE
-
GI:		SUBTITLE
Relation:	parent DOCINFO
StartCode:	StartParagraph @{ID ""} "P" ""
-
GI:		TITLE
Relation:	parent TABLE
Ignore:		all
-
GI:		TITLE
Relation:	parent PART
Ignore:		all
-
GI:		TITLE
Relation:	parent REFSYNOPSISDIV
StartCode:	StartManPageDivisionTitle @{ID ""}
EndCode:	EndManPageDivisionTitle
-
GI:		TITLE
Relation:	parent REFSECT1
StartCode:	StartManPageDivisionTitle @{ID ""}
EndCode:	EndManPageDivisionTitle
-
GI:		TITLE
Relation:	parent REFSECT2
StartCode:	StartManPageDivisionTitle @{ID ""}
EndCode:	EndManPageDivisionTitle
-
GI:		TITLE
Relation:	parent REFSECT3
StartCode:	StartManPageDivisionTitle @{ID ""}
EndCode:	EndManPageDivisionTitle
-
GI:		TITLE
Relation:	parent NOTE
Action:		1006
-
GI:		TITLE
Relation:	parent CAUTION
Action:		1006
-
GI:		TITLE
Relation:	parent WARNING
Action:		1006
-
GI:		TITLE
Relation:	parent TIP
Action:		1006
-
GI:		TITLE
Relation:	parent IMPORTANT
StartCode:	Title @{ID ""} ADMONITION
EndCode:	CloseTitle ADMONITION
SpecID:	1006
-
GI:		TITLE
StartCode:	Title @{ID ""} @{_parent}
EndCode:	CloseTitle @{_parent}
SpecID:	1002
-
GI:		TITLEABBREV
Ignore:		all
# titleabbrev should end up as the ABBREV= attribute of <HEAD>
-
######################################################################
#                      TOP LEVEL STUFF
######################################################################
GI:		CHAPTER
GI:		SECT1 SECT2 SECT3 SECT4 SECT5
StartCode:	StartGlossedTopic @{_gi} @{ID ""} @{_nchild GLOSSARY}
EndCode:        EndGlossedTopic @{_nchild GLOSSARY}
-
GI:		APPENDIX
AttValue:	ROLE notoc
Ignore:		all
-
GI:		APPENDIX
StartCode:	StartAppendix @{_gi} @{ID ""} @{ROLE ""}
-
GI:		GLOSSARY
Relation:	parent DOCINFO
StartCode:	StartNewLevel0Virpage @{_gi} @{ID ""}
EndCode:	SortAndEmitGlossary NoPopForm
-
GI:		GLOSSARY
Relation:	parent PART
Relation:	sibling- GLOSSARY
StartCode:	StartNewVirpage @{_gi} @{ID ""}
EndCode:	SortAndEmitGlossary NoPopForm
-
GI:		GLOSSARY
Relation:	parent PART
Relation:	child TITLE
StartCode:	StartNewVirpageWithID @{_gi} @{ID ""} SDL-RESERVED-@{_gi} 1
EndCode:	SortAndEmitGlossary NoPopForm
-
GI:		GLOSSARY
Relation:	parent PART
StartCode:	StartNewVirpageWithID @{_gi} @{ID ""} SDL-RESERVED-@{_gi} 0
EndCode:	SortAndEmitGlossary NoPopForm
-
GI:		GLOSSARY
StartCode:	PushForm "" SUBGLOSSARY @{ID ""}
EndCode:	SortAndEmitGlossary PopForm
-
GI:		PREFACE
GI:		BIBLIOGRAPHY
GI:		REFENTRY
GI:		INDEX
StartCode:	StartNewVirpage @{_gi} @{ID ""}
-
# if we're inside a Reference, the Refentry's are nested 1 level deeper
GI:		REFERENCE
StartCode:	incr virpageLevels(REFENTRY)
		^StartNewVirpage @{_gi} @{ID ""}
EndCode:        incr virpageLevels(REFENTRY) -1
-
GI:		PARTINTRO
Relation:	child TITLE
StartCode:	StartNewVirpage PARTINTRO @{ID ""}
-
GI:		PARTINTRO
StartCode:	StartNewVirpage PARTINTRO @{ID ""}
		^SynthesizeHomeTopicTitle
-
GI:		HIGHLIGHTS
StartCode:	PushForm "" "HIGHLIGHTS" @{ID ""}
EndCode:	PopForm
-
######################################################################
#                          METAINFO (from Part->DocInfo)
######################################################################
GI:		ABSTRACT AUTHORGROUP REVHISTORY LEGALNOTICE
Relation:	parent DOCINFO
Relation:	child TITLE
StartCode:	StartNewVirpageWithID @{_gi} @{ID ""} SDL-RESERVED-@{_gi} 1
-
GI:		ABSTRACT AUTHORGROUP REVHISTORY LEGALNOTICE
Relation:	parent DOCINFO
StartCode:	StartNewVirpageWithID @{_gi} @{ID ""} SDL-RESERVED-@{_gi} 0
-
GI:		AUTHOR EDITOR COLLAB OTHERCREDIT
Relation:	parent AUTHORGROUP
StartCode:	PushForm "" @{_gi} @{ID ""}
EndCode:	PopForm
-
GI:		CORPAUTHOR
Relation:	parent AUTHORGROUP
StartCode:	StartParagraph @{ID ""} CORPAUTHOR LINED
-
GI:		AUTHOR EDITOR CORPAUTHOR
-
GI:		HONORIFIC FIRSTNAME SURNAME LINEAGE OTHERNAME CONTRIB
Context:	((AUTHOR)|(EDITOR)|(OTHERCREDIT)) AUTHORGROUP
StartCode:	StartParagraphMaybe @{ID ""} @{_parent} LINED
-
GI:		AFFILIATION
Context:	((AUTHOR)|(EDITOR)|(COLLAB)|(OTHERCREDIT)) AUTHORGROUP
StartCode:	StartParagraph @{ID ""} AFFILIATION LINED
EndCode:	EndParagraph
-
GI:		AFFILIATION
-
GI:		AUTHORBLURB
Context:	((AUTHOR)|(EDITOR)|(OTHERCREDIT)) AUTHORGROUP
StartCode:	PushForm "" AUTHORBLURB @{ID ""}
EndCode:	PopForm
-
GI:		AUTHORBLURB
Context:	((AUTHOR)|(EDITOR)|(OTHERCREDIT))
Ignore:		all
-
GI:		AUTHORBLURB
StartCode:	PushForm "" AUTHORBLURB @{ID ""}
EndCode:	PopForm
-
GI:		REVISION
Context:	REVHISTORY DOCINFO
StartCode:	StartParagraph @{ID ""} REVISION LINED
EndCode:	EndParagraph
-
GI:		HONORIFIC FIRSTNAME SURNAME OTHERNAME
GI:		JOBTITLE ORGDIV COLLABNAME
GI:		ADDRESS CITY COUNTRY EMAIL FAX
GI:		LINEAGE CONTRIB CORPAUTHOR
GI:		REVHISTORY REVISION
GI:		REVNUMBER DATE AUTHORINITIALS REVREMARK
StartCode:	Anchor @{ID ""}
-
######################################################################
#                      SUPER/SUBSCRIPT
######################################################################
GI:		LINK ULINK SUPERSCRIPT SUBSCRIPT
Relation:	ancestor SUPERSCRIPT
StartCode:	UserError 
		   "Cannot put a @{_gi} anywhere within a SUPERSCRIPT" yes
-
GI:		OLINK
Relation:	ancestor SUPERSCRIPT
StartCode:	UserError 
		   "Cannot put an OLINK anywhere within a SUPERSCRIPT" yes
-
GI:		LINK ULINK SUPERSCRIPT SUBSCRIPT
Relation:	ancestor SUBSCRIPT
StartCode:	UserError 
		   "Cannot put a @{_gi} anywhere within a SUBSCRIPT" yes
-
GI:		OLINK
Relation:	ancestor SUBSCRIPT
StartCode:	UserError 
		   "Cannot put an OLINK anywhere within a SUBSCRIPT" yes
-
GI:		SUPERSCRIPT SUBSCRIPT
StartCode:	StartSPhrase @{ID ""} @{_gi}
EndCode:	EndSPhrase
-
######################################################################
#                          LINKS
######################################################################
GI:		LINK OLINK ULINK
Relation:	parent REFENTRY
# ignore these for now - probably emit them out of flow somewhere
Ignore:		all
-
GI:		LINK OLINK ULINK
Relation:	parent REFNAMEDIV
# ignore these for now - probably emit them out of flow somewhere
Ignore:		all
-
GI:		LINK
Relation:	isfirstcon PARA
Relation:	hasonlycon INLINEGRAPHIC
StartCode:	DeferLink @{ID ""} @{LINKEND} @{TYPE ""}
-
GI:		LINK
StartCode:	StartLink @{ID ""} @{LINKEND} @{TYPE ""}
EndCode:	EndLink
-
GI:		OLINK
Relation:	isfirstcon PARA
Relation:	hasonlycon INLINEGRAPHIC
StartCode:	DeferOLink @{ID ""} "@{LOCALINFO}" @{TYPE ""}
-
GI:		OLINK
StartCode:	StartOLink @{ID ""} "@{LOCALINFO}" @{TYPE ""}
EndCode:	EndLink
-
GI:		ULINK
Relation:	isfirstcon PARA
Relation:	hasonlycon INLINEGRAPHIC
StartCode:	DeferULink @{ID ""}
-
GI:		ULINK
StartCode:	StartParagraphMaybe @{ID ""} "P" ""
-
GI:		ANCHOR
Relation:	parent TITLE
StartCode:	Anchor @{ID}
-
GI:		ANCHOR
StartCode:	AnchorInP @{ID}
-
######################################################################
#                        PARAGRAPHS
######################################################################
GI:		PARA
Relation:	parent FORMALPARA
-
GI:		PARA FORMALPARA
Relation:	descendant GLOSSLIST
Action:	1001
-
GI:		PARA FORMALPARA
Relation:	descendant ITEMIZEDLIST
Action:	1001
-
GI:		PARA FORMALPARA
Relation:	descendant ORDEREDLIST
Action:	1001
-
GI:		PARA FORMALPARA
Relation:	descendant SEGMENTEDLIST
Action:	1001
-
GI:		PARA FORMALPARA
Relation:	descendant SIMPLELIST
Action:	1001
-
GI:		PARA FORMALPARA
Relation:	descendant VARIABLELIST
StartCode:	StartCompoundParagraph @{ID ""} "P" ""
EndCode:	PopForm
SpecID:	1001
-
GI:		PARA FORMALPARA SIMPARA
StartCode:	StartParagraph @{ID ""} "P" ""
EndCode:	EndParagraph
-
######################################################################
#                            ABSTRACT
######################################################################
GI:		ABSTRACT
StartCode:	PushForm "" ABSTRACT @{ID ""}
EndCode:	PopForm
-
######################################################################
#                             LISTS
######################################################################
GI:		ITEMIZEDLIST
StartCode:	ItemizedList @{ID ""} {@{MARK PLAIN}} 
			     @{ROLE LOOSE} @{_parent}
EndCode:	EndList @{_parent}
-
GI:		ORDEREDLIST
StartCode:	OrderedList @{ID ""} @{NUMERATION ""} @{INHERITNUM} 
			@{CONTINUATION} @{ROLE LOOSE} @{_parent}
EndCode:	EndList @{_parent}
-
GI:		VARIABLELIST
StartCode:	VariableList @{ID ""} @{ROLE LOOSE} @{_parent}
EndCode:	EndList @{_parent}
-
GI:		VARLISTENTRY
StartCode:	VarListEntry @{ID ""}
-
GI:		TERM
StartCode:	StartTerm @{ID ""}
EndCode:	EndParagraph
-
GI:		LISTITEM
StartCode:	ListItem @{ID ""} {@{OVERRIDE NO}}
EndCode:	PopForm
-
GI:		SEGMENTEDLIST
Relation:	child SEGTITLE
StartCode:	SegmentedList @{ID ""} @{ROLE LOOSE} @{_parent}
EndCode:	EndList @{_parent}
-
GI:		SEGMENTEDLIST
StartCode:	UserWarning "No SegTitle provided for a SegmentedList" yes
Ignore:		all
-
GI:		SEGTITLE
StartCode:	StartSegTitle @{ID ""}
EndCode:	CloseBlock
-
GI:		SEGLISTITEM
StartCode:	StartSegListItem @{ID ""}
-
GI:		SEG
NthChild:	-1
StartCode:	StartSeg @{ID ""} 1
EndCode:	CloseBlock
-
GI:		SEG
StartCode:	StartSeg @{ID ""} 0
EndCode:	CloseBlock
-
GI:		SIMPLELIST
StartCode:	StartSimpleList @{ID ""} @{TYPE} @{ROLE LOOSE} @{_parent}
EndCode:	EndSimpleList @{COLUMNS 1} @{TYPE} @{_parent}
-
GI:		MEMBER
NthChild:	-1
StartCode:	StartMember @{ID ""} @{_pattr TYPE}
EndCode:	EndMember @{_pattr TYPE} ""
-
GI:		MEMBER
NthChild:	-2
StartCode:	StartMember @{ID ""} @{_pattr TYPE}
EndCode:	EndMember @{_pattr TYPE} " and "
-
GI:		MEMBER
StartCode:	StartMember @{ID ""} @{_pattr TYPE}
EndCode:	EndMember @{_pattr TYPE} ", "
-
######################################################################
#	                   ADMONITIONS
######################################################################
GI:		NOTE CAUTION WARNING TIP IMPORTANT
StartCode:	StartAdmonition @{ID ""} @{_gi} @{_nchild TITLE}
EndCode:	PopForm
-
######################################################################
#			      INDEX
######################################################################
# ignore INDEXTERMs with SPANEND= set for the time being
# (there is no content and I'm not sure it makes sense on-line)
GI:		INDEXTERM
AttValue:	SPANEND .
-
# ignore SEE for the time being
GI:		SEE
Ignore:		all
-
# ignore SEEALSO for the time being
# (where should it go, index or current page?)
GI:		SEEALSO
Ignore:		all
-
GI:		INDEXTERM
StartCode:	StartIndexTerm @{ID ""}
EndCode:	EndIndexTerm
-
GI:		PRIMARY
StartCode:	StartPrimaryIndexEntry @{ID ""} {@{_cdata}}
EndCode:	EndPrimaryIndexEntry
-
GI:		SECONDARY
StartCode:	StartSecondaryIndexEntry @{ID ""} {@{_cdata}}
EndCode:	EndSecondaryIndexEntry
-
GI:		TERTIARY
StartCode:	StartTertiaryIndexEntry @{ID ""} {@{_cdata}}
EndCode:	EndTertiaryIndexEntry
-
######################################################################
#			      GLOSSARY
######################################################################
GI:		GLOSSTERM
Relation:	parent GLOSSENTRY
StartCode:	StartAGlossedTerm
EndCode:	EndATermInAGlossary @{ID ""}
-
GI:		GLOSSTERM
Relation:	parent GLOSSTERM
-
GI:		GLOSSTERM
StartCode:	StartAGlossedTerm
EndCode:	EndAGlossedTerm @{ID ""} "@{ROLE}"
-
GI:		GLOSSENTRY
StartCode:	StartGlossEntry @{ID ""} [expr "@{_nchild ACRONYM} + 
					        @{_nchild ABBREV}"] 
					 @{_nchild GLOSSDEF}
EndCode:	EndGlossEntry "@{SORTAS}"
-
GI:		ACRONYM
Relation:	parent GLOSSENTRY
EndCode:	EndAcronymInAGlossary @{ID ""}
-
GI:		ABBREV
Relation:	parent GLOSSENTRY
EndCode:	EndAbbrevInAGlossary @{ID ""}
-
GI:		GLOSSSEE
AttValue:	OTHERTERM .
StartCode:	StartGlossSee @{ID ""} @{OTHERTERM ""}
		^set glossType GlossSee
		^@{_followlink OTHERTERM 1007}
		^unset glossType
EndCode:	EndGlossSeeOrSeeAlso @{OTHERTERM ""}
-
GI:		GLOSSSEE
StartCode:	StartGlossSee @{ID ""} @{OTHERTERM ""}
EndCode:	EndGlossSeeOrSeeAlso @{OTHERTERM ""}
-
# we always get here via the SpecID
GI:		GLOSSTERM
StartCode:	CheckOtherTerm @{ID} @{_gi} @{_parent}
SpecID:		1007
-
GI:		GLOSSDEF
StartCode:	StartGlossDef @{ID ""}
EndCode:	EndGlossDef
-
GI:		GLOSSSEEALSO
AttValue:	OTHERTERM .
StartCode:	StartGlossSeeAlso @{ID ""} @{OTHERTERM ""}
		^set glossType GlossSeeAlso
		^@{_followlink OTHERTERM 1007}
		^unset glossType
EndCode:	EndGlossSeeOrSeeAlso @{OTHERTERM ""}
-
GI:		GLOSSSEEALSO
StartCode:	StartGlossSeeAlso @{ID ""} @{OTHERTERM ""}
EndCode:	EndGlossSeeOrSeeAlso @{OTHERTERM ""}
-
GI:		GLOSSLIST GLOSSDIV
StartCode:	PushForm "" @{_gi}  @{ID ""}
EndCode:	SortAndEmitGlossary PopForm
-
######################################################################
#			  LITERALS AND SCREENS
######################################################################
GI:		LITERALLAYOUT PROGRAMLISTING SCREEN SCREENSHOT
Relation:	parent PARA
StartCode:	PushForm "" "INSIDE-PARA" ""
		^StartParagraph @{ID ""} @{_gi} LITERAL
EndCode:	ContinueParagraph
-
GI:		LITERALLAYOUT PROGRAMLISTING SCREEN
StartCode:	StartParagraph @{ID ""} @{_gi} LITERAL
EndCode:	EndParagraph
-
GI:		SCREENSHOT
StartCode:	PushForm "" SCREENSHOT @{ID ""}
EndCode:	PopForm
-
GI:		SCREENINFO
StartCode:	StartParagraph @{ID ""} SCREENINFO ""
EndCode:	EndParagraph
-
######################################################################
#                           BIBLIOGRAPHY
######################################################################
GI:		BIBLIODIV BIBLIOENTRY
StartCode:	PushForm "" @{_gi} @{ID ""}
EndCode:	PopForm
-
GI:		BIBLIOMISC
StartCode:	StartParagraph @{ID ""} "P" ""
EndCode:	EndParagraph
-
GI:		ARTHEADER BOOKBLIBLIO SERIESINFO
Ignore:		all
-
######################################################################
#
######################################################################
GI:		EPIGRAPH
StartCode:	PushForm "" @{_gi} @{ID ""}
EndCode:	PopForm
-
GI:		SYNOPSIS
Relation:	parent REFSYNOPSISDIV
StartCode:	StartSynopsis @{ID ""} @{LINESPECIFIC ""}
EndCode:	EndSynopses @{_parent}
-
GI:		CMDSYNOPSIS
StartCode:	StartCmdSynopsis @{ID ""}
EndCode:	EndSynopses @{_parent}
-
GI:		FUNCSYNOPSIS
StartCode:	StartFuncSynopsis @{ID ""}
EndCode:	EndSynopses @{_parent}
-
GI:		INFORMALEQUATION INFORMALEXAMPLE
GI:		BLOCKQUOTE
Relation:	parent PARA
StartCode:	PushForm "" "INSIDE-PARA" ""
		^StartParagraph @{ID ""} @{_gi} ""
EndCode:	ContinueParagraph
-
GI:		INFORMALEQUATION INFORMALEXAMPLE
StartCode:	StartParagraph @{ID ""} @{_gi} ""
EndCode:	EndParagraph
-
GI:		EQUATION
StartCode:	PushForm "" "" ""
EndCode:	PopForm
-
GI:		BLOCKQUOTE
StartCode:	PushForm "" @{_gi} @{ID ""}
EndCode:	PopForm
-
GI:		SYNOPSIS
Relation:	parent PARA
StartCode:	PushForm "" "INSIDE-PARA" ""
		^StartParagraph @{ID ""} @{_gi} LITERAL
EndCode:	ContinueParagraph
-
GI:		SYNOPSIS
StartCode:	StartParagraph @{ID ""} @{_gi} LITERAL
EndCode:	EndParagraph
-
GI:		XREF
AttValue:	ENDTERM .
StartText:	<LINK RID="@{LINKEND}">@{_followlink ENDTERM 8001}</LINK>
-
GI:		XREF
StartText:	<LINK RID="@{LINKEND}">@{_followlink LINKEND 1}</LINK>
-
GI:		FOOTNOTEREF
StartCode:	FootnoteRef @{LINKEND ""}
-
# FOOTNOTE elements were already processed in CHAPTER
GI:		FOOTNOTE
Ignore:		all
-
GI:		FIGURE
StartCode:	StartFigure @{ID ""} @{ROLE ""}
EndCode:	PopForm
-
GI:		GRAPHIC
Relation:	parent PARA
AttValue:	ENTITYREF .
StartCode:	PushForm "" "INSIDE-PARA" ""
		^Graphic @{ID ""} "@{_filename}" "@{FILEREF}" @{_gi}
EndCode:	ContinueParagraph
Ignore:		all
-
GI:		GRAPHIC
Relation:	parent PARA
StartCode:	PushForm "" "INSIDE-PARA" ""
		^Graphic @{ID ""} "" "@{FILEREF}" @{_gi}
EndCode:	ContinueParagraph
Ignore:		all
-
GI:		GRAPHIC
Relation:	parent INLINEEQUATION
AttValue:	ENTITYREF .
StartCode:	Graphic @{ID ""} "@{_filename}" "@{FILEREF}" INLINEGRAPHIC
Ignore:		all
-
GI:		GRAPHIC
Relation:	parent INLINEEQUATION
StartCode:	Graphic @{ID ""} "" "@{FILEREF}"  INLINEGRAPHIC
Ignore:		all
-
GI:		GRAPHIC
AttValue:	ENTITYREF .
StartCode:	Graphic @{ID ""} "@{_filename}" "@{FILEREF}"  @{_gi}
EndCode:	EndParagraph
Ignore:		all
-
GI:		GRAPHIC
StartCode:	Graphic @{ID ""} "" "@{FILEREF}"  @{_gi}
EndCode:	EndParagraph
Ignore:		all
-
GI:		INLINEGRAPHIC
Relation:	isfirstcon PARA
AttValue:	ENTITYREF .
StartCode:	InFlowGraphic @{ID ""} "@{_filename}" "@{FILEREF}" 
			      PARA      @{REMAP ""}    @{ROLE ""}
Ignore:		all
-
GI:		INLINEGRAPHIC
Relation:	isfirstcon LINK
AttValue:	ENTITYREF .
StartCode:	InFlowGraphic @{ID ""} "@{_filename}" "@{FILEREF}" 
			      LINK      @{REMAP ""}    @{ROLE ""}
Ignore:		all
-
GI:		INLINEGRAPHIC
Relation:	isfirstcon OLINK
AttValue:	ENTITYREF .
StartCode:	InFlowGraphic @{ID ""} "@{_filename}" "@{FILEREF}" 
			      OLINK     @{REMAP ""}    @{ROLE ""}
Ignore:		all
-
GI:		INLINEGRAPHIC
Relation:	isfirstcon ULINK
AttValue:	ENTITYREF .
StartCode:	InFlowGraphic @{ID ""} "@{_filename}" "@{FILEREF}" 
			      ULINK     @{REMAP ""}    @{ROLE ""}
Ignore:		all
-
GI:		INLINEGRAPHIC
AttValue:	ENTITYREF .
StartCode:	Graphic @{ID ""} "@{_filename}" @{FILEREF ""}  @{_gi}
Ignore:		all
-
GI:		INLINEGRAPHIC
Relation:	isfirstcon PARA
StartCode:	InFlowGraphic @{ID ""} ""  "@{FILEREF}" 
			      PARA @{REMAP ""} @{ROLE ""}
Ignore:		all
-
GI:		INLINEGRAPHIC
Relation:	isfirstcon LINK
StartCode:	InFlowGraphic @{ID ""} "" "@{FILEREF}" 
			      LINK @{REMAP ""} @{ROLE ""}
Ignore:		all
-
GI:		INLINEGRAPHIC
Relation:	isfirstcon OLINK
StartCode:	InFlowGraphic @{ID ""} "" "@{FILEREF}" 
			      OLINK @{REMAP ""} @{ROLE ""}
Ignore:		all
-
GI:		INLINEGRAPHIC
Relation:	isfirstcon ULINK
StartCode:	InFlowGraphic @{ID ""} "" "@{FILEREF}" 
			      ULINK @{REMAP ""} @{ROLE ""}
Ignore:		all
-
GI:		INLINEGRAPHIC
StartCode:	Graphic @{ID ""} "" @{FILEREF ""}  @{_gi}
Ignore:		all
-
GI:		INLINEEQUATION
-
GI:		EXAMPLE INFORMALEXAMPLE
StartCode:	Example @{ID ""}
EndCode:	CloseExample
-
GI:		CITETITLE
StartCode:	StartKey @{ID ""} PUB-LIT CITETITLE-@{PUBWORK PART}
EndText:	</KEY>
-
GI:		CITATION
StartCode:	StartKey @{ID ""} PUB-LIT CITATION
EndText:	</KEY>
-
GI:		COMMAND
Relation:	parent CMDSYNOPSIS
NthChild:	1
StartCode:	StartCommand @{ID ""} ""
EndText:	</KEY>
-
GI:		COMMAND
Relation:	parent CMDSYNOPSIS
StartCode:	StartKey @{ID ""} NAME "COMMAND"
EndText:	</KEY>
-
GI:		COMMAND FIRSTTERM
StartCode:	StartKey @{ID ""} NAME @{_gi}
EndText:	</KEY>
-
GI:		ACTION FILENAME FUNCTION HARDWARE KEYCAP
GI:		INTERFACEDEFINITION PROPERTY
StartCode:	StartKey @{ID ""} NAME @{_gi}
EndText:	</KEY>
-
GI:		INTERFACE
StartCode:	StartKey @{ID ""} NAME @{_gi}-@{CLASS BUTTON}
EndText:	</KEY>
-
GI:		REPLACEABLE
Relation:	parent GROUP
NthChild:	1
Action:		1004
-
GI:		REPLACEABLE
Relation:	parent GROUP
StartText:	|@{_action 1004}
-
GI:		REPLACEABLE
StartCode:	StartKey @{ID ""} MACH-IN @{_gi}-@{CLASS PARAMETER}
EndText:	</KEY>
SpecID:		1004
-
GI:		QUOTE
StartCode:	StartQuote @{ID ""}
EndCode:	EndQuote
-
GI:		LITERAL
StartCode:	StartKey @{ID ""} QUOTE @{_gi}
EndText:	</KEY>
-
GI:		EMPHASIS
AttValue:       ROLE .
StartCode:	StartHeading @{ID ""} @{ROLE}
EndText:	</KEY>
-
GI:		EMPHASIS SYMBOL
StartCode:	StartKey @{ID ""} EMPH @{_gi}
EndText:	</KEY>
-
GI:		OPTION
Relation:	parent ARG
NthChild:	1
Action:		1005
-
GI:		OPTION
Relation:	parent ARG
StartText:	\s@{_action 1005}
-
GI:		OPTION
StartCode:	StartKey @{ID ""} MACH-IN @{_gi}
EndText:	</KEY>
SpecID:		1005
-
GI:		USERINPUT OPTIONAL
StartCode:	StartKey @{ID ""} MACH-IN @{_gi}
EndText:	</KEY>
-
GI:		SYSTEMITEM
StartCode:	StartKey @{ID ""} MACH-OUT @{CLASS SYSTEMNAME}
EndText:	</KEY>
-
GI:		COMPUTEROUTPUT
StartCode:	StartKey @{ID ""} MACH-OUT @{_gi}
EndText:	</KEY>
-
GI:		PARAMETER RETURNVALUE
StartCode:	StartKey @{ID ""} NAME @{_gi}
EndText:	</KEY>
-
GI:		ERRORNAME ERRORTYPE APPLICATION FOREIGNPHRASE MARKUP
GI:		WORDASWORD
StartCode:	StartKey @{ID ""} EMPH @{_gi}
EndText:	</KEY>
-
GI:		STRUCTNAME STRUCTFIELD TYPE TOKEN
StartCode:	StartKey @{ID ""} NAME @{_gi}
EndText:	</KEY>
-
GI:		SIDEBAR
# should this be done as a <HEAD> of the object containing it?
# that would be impossible, the component containing a <SideBar>
# will have to be emitted as a two column <FORM> with the sidebar
# put in the second column.
Ignore:		all
-
GI:		KEYCODE KEYSYM MEDIALABEL
GI:		ACRONYM ABBREV
StartCode:	Anchor @{ID ""}
-
######################################################################
#			    SGMLTAG
######################################################################
GI:		SGMLTAG
StartCode:	StartSgmlTag @{ID ""} @{CLASS ELEMENT}
EndCode:	EndSgmlTag @{CLASS ELEMENT}
-
######################################################################
#			    MESSAGES
######################################################################
GI:		MSGTEXT MSGSET MSGENTRY MSG MSGMAIN MSGSUB
GI:		MSGREL MSGEXPLAN
StartCode:	PushForm "" @{_gi} @{ID ""}
EndCode:	PopForm
-
GI:		MSGINFO
StartCode:	StartParagraph @{ID ""} MSG LITERAL
-
GI:		MSGORIG MSGAUD MSGLEVEL
-
######################################################################
#			     TABLE
######################################################################
GI:		TABLE
StartText:	@{_action 6}@{_followrel child TITLE 1002}
StartCode:	StartTable @{ID ""}    @{COLSEP 1} @{FRAME ALL} 
			   @{LABEL ""} @{ROWSEP 1}
EndCode:	PopForm; PopForm
-
GI:		INFORMALTABLE
Relation:	parent PARA
StartCode:	PushForm "" "INSIDE-PARA" ""
		^StartTable @{ID ""}    @{COLSEP 1} @{FRAME ALL} 
			   @{LABEL ""} @{ROWSEP 1}
EndCode:	PopForm; ContinueParagraph
-
GI:		INFORMALTABLE
StartCode:	StartTable @{ID ""}    @{COLSEP 1} @{FRAME ALL} 
			   @{LABEL ""} @{ROWSEP 1}
EndCode:	PopForm
-
GI:		TGROUP
StartCode:	StartTGroup	@{ID ""} @{ALIGN}     {@{CHAR}} 
				@{COLS}  @{COLSEP ""} @{ROWSEP ""} 
				@{_nchild COLSPEC}
EndText:	@{_followrel child TFOOT 1003}@{_action 7}
-
GI:		COLSPEC
Relation:	parent THEAD
Ignore:		all
-
GI:		COLSPEC
Relation:	parent TFOOT
Ignore:		all
-
GI:		COLSPEC
StartCode:	ColSpec @{_parent 2}  @{_parent} 
			@{ALIGN LEFT} {@{CHAR}} 
			@{COLNAME ""} @{COLNUM ""} 
			@{COLSEP ""}  @{COLWIDTH ""} 
			@{ROWSEP ""}
SpecID:		1009
-
GI:		SPANSPEC
StartCode:	SpanSpec @{_parent} @{ALIGN}   {@{CHAR}}    @{COLSEP ""} 
			 @{NAMEEND} @{NAMEST}  @{ROWSEP ""} @{SPANNAME}
-
GI:		TFOOT
StartCode:	PrepForTFoot @{_nchild COLSPEC}
Ignore:		all
-
GI:		THEAD TFOOT TBODY
Relation:	sibling- THEAD
StartText:	@{_action 10}@{_followrel child COLSPEC 1009}
StartCode:	StartTHeadTFootTBody @{_parent} @{_gi} 1 
				     @{ID ""} @{VALIGN} 
				     @{_nchild ROW} 
				     @{_nchild COLSPEC}
EndCode:	EndTHeadTFootTBody @{_parent} @{_gi}
-
GI:		THEAD TFOOT TBODY
StartText:	@{_action 10}@{_followrel child COLSPEC 1009}
StartCode:	StartTHeadTFootTBody @{_parent} @{_gi} 0 
				     @{ID ""} @{VALIGN} 
				     @{_nchild ROW} 
				     @{_nchild COLSPEC}
EndCode:	EndTHeadTFootTBody @{_parent} @{_gi}
SpecID:	1003
-
GI:		ROW
StartCode:	StartRow @{_parent 2} @{_parent} @{ID ""} 
			 @{ROWSEP ""} @{VALIGN NONE}
EndCode:	EndRow @{_parent 2} @{_parent}
-
GI:		ENTRY
StartCode:	StartCell @{_parent 3}	@{_parent 2}   @{_gi} 
			  @{ID ""}	@{ALIGN ""}    {@{CHAR}} 
			  @{COLNAME ""} ""	       @{COLSEP ""} 
			  @{MOREROWS}	@{NAMEEND ""}  @{NAMEST ""} 
			  @{ROWSEP ""}	@{SPANNAME ""} @{VALIGN ""} 
			  ""		""
EndCode:	EndEntry
-
GI:		ENTRYTBL
StartCode:	StartCell @{_parent 3}	@{_parent 2}   @{_gi} 
			  @{ID ""}	@{ALIGN ""}    {@{CHAR}} 
			  @{COLNAME ""} @{COLS}        @{COLSEP ""} 
			  0		@{NAMEEND ""}  @{NAMEST ""} 
			  @{ROWSEP ""}	@{SPANNAME ""} {} 
			  @{_nchild COLSPEC} 
			  @{_nchild TBODY}
EndCode:	EndEntryTbl
-
######################################################################
#		             PROCEDURE
######################################################################
GI:		PROCEDURE
StartCode:	StartProcedure @{ID ""}
EndCode:	PopForm
-
GI:		STEP
StartCode:	StartStep @{ID ""}
EndCode:	PopForm
-
GI:		SUBSTEPS
StartCode:	StartSubStep @{ID ""}
EndCode:	PopForm
-
######################################################################
#		             REFENTRY
######################################################################
GI:		REFMETA
-
GI:		COMMENT
Ignore:		all
-
GI:		REFMISCINFO
Ignore:		all
-
GI:		CITEREFENTRY
-
GI:		REFENTRYTITLE
Relation:	parent CITEREFENTRY
-
GI:		REFENTRYTITLE
StartCode:	DivertOutputToManTitle
EndCode:	RestoreOutputStreamFromManTitle
-
GI:		MANVOLNUM
Relation:	parent CITEREFENTRY
StartText:	(
EndText:	)
-
GI:		MANVOLNUM
StartCode:	DivertOutputToManVolNum
EndCode:	RestoreOutputStreamFromManVolNum
-
GI:		REFNAMEDIV
StartCode:	StartRefNameDiv @{_nchild REFNAME}
EndCode:	EndRefNameDiv @{ID ""}
-
GI:		REFDESCRIPTOR
StartCode:	DivertOutputToManDescriptor
EndCode:	RestoreOutputStreamFromManDescriptor
-
GI:		REFNAME
StartCode:	StartAManName
EndCode:	EndAManName
-
GI:		REFPURPOSE
StartCode:	DivertOutputToManPurpose
EndCode:	RestoreOutputStreamFromManPurpose
-
GI:		REFCLASS
Ignore:		all
-
GI:		REFSYNOPSISDIV
StartCode:	StartRefSynopsisDiv @{ID ""} @{_nchild TITLE} 
                           [expr "@{_nchild SYNOPSIS} + 
				  @{_nchild CMDSYNOPSIS} + 
				  @{_nchild FUNCSYNOPSIS}"]
EndCode:	PopForm
-
GI:		SYNOPFRAGMENTREF
StartText:	@{_id @{LINKEND} 11}
StartCode:	StartLink @{ID ""} @{LINKEND} ""
EndCode:	EndLink
-
GI:		SYNOPFRAGMENT
StartCode:	Anchor @{ID ""}
-
GI:		ARG
NthChild:	1
StartCode:	StartArg @{ID ""} @{CHOICE} ""
EndCode:	EndArg @{CHOICE} @{REP}
-
GI:		ARG
Relation:	parent GROUP
StartCode:	StartArg @{ID ""} @{CHOICE} "|"
EndCode:	EndArg @{CHOICE} @{REP}
-
GI:		ARG
StartCode:	StartArg @{ID ""} @{CHOICE} " "
EndCode:	EndArg @{CHOICE} @{REP}
-
GI:		GROUP
NthChild:	1
StartCode:	StartGroup @{ID ""} @{CHOICE} ""
EndCode:	EndGroup @{CHOICE} @{REP}
-
GI:		GROUP
Relation:	parent GROUP
StartCode:	StartGroup @{ID ""} @{CHOICE} "|"
EndCode:	EndGroup @{CHOICE} @{REP}
-
GI:		GROUP
StartCode:	StartGroup @{ID ""} @{CHOICE} " "
EndCode:	EndGroup @{CHOICE} @{REP}
-
GI:		FUNCSYNOPSISINFO
StartCode:	StartFuncSynopsisInfo @{ID ""} @{LINESPECIFIC ""}
-
GI:		FUNCDEF
StartCode:	StartFuncDef @{ID ""}
-
GI:		VOID VARARGS
StartCode:	DoVoidOrVarargs @{_gi} @{ID ""}
-
GI:		PARAMDEF
NthChild:	-1
StartCode:	StartParamDef @{ID ""}
EndCode:	EndParamDef ")"
-
GI:		PARAMDEF
StartCode:	StartParamDef @{ID ""}
EndCode:	EndParamDef ", "
-
GI:		FUNCPARAMS
NthChild:	-1
StartCode:	StartFuncParams @{ID ""}
EndCode:	EndFuncParams ")"
-
GI:		FUNCPARAMS
StartCode:	StartFuncParams @{ID ""}
EndCode:	EndFuncParams ", "
-
GI:		REFSECT1 REFSECT2 REFSECT3
StartCode:	PushForm "" "" @{ID ""}
EndCode:	PopForm; PopForm
-
######################################################################
#		         BRIDGEHEAD
######################################################################
GI:		BRIDGEHEAD
StartCode:	StartBridgeHead @{ID ""} @{RENDERAS OTHER}
EndCode:	EndBridgeHead
-
######################################################################
#		         TRADEMARK
######################################################################
GI:		TRADEMARK
StartCode:	StartKey @{ID ""} PRODUCT TRADEMARK
EndCode:	EndTradeMark @{CLASS TRADE}
-
######################################################################
#                       IGNORED THINGS
######################################################################
GI:		AUTHORGROUP EDITION SUBTITLE BOOKBIBLIO
GI:		RELEASEINFO PUBSNUMBER INVPARTNUMBER ISSN OTHERADDR
GI:		CONFGROUP CONFTITLE PUBLISHERNAME PUBLISHER PUBDATE
GI:		CONTRACTNUM PRINTHISTORY
GI:		INDEXDIV INDEXENTRY PRIMARYIE SEEIE SECONDARYIE
GI:		SEEALSOIE TERTIARYIE INDEXDIV INDEXENTRY PRIMARYIE
GI:		SEEIE SECONDARYIE SEEALSOIE TERTIARYIE
GI:		LINEANNOTATION
Ignore:		all
-
######################################################################
#                        NO-OP THINGS
######################################################################
GI:		POSTCODE BEGINPAGE CLASSNAME DATABASE
GI:		PRODUCTNUMBER PRODUCTNAME
GI:		ORGNAME STREET POB STATE PHONE
GI:		SHORTAFFIL
StartCode:	Anchor @{ID ""}
-
######################################################################
#	            END OF SUPPORTED GI'S
######################################################################
# done with all accepted GIs
StartCode:	UserError "Unrecognized tag \"@{_gi}\"" yes
-
#::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
# The following specifications are only reached via the SpecID ::
#::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
SpecID:		1
StartText:	@{_set xrefdone no}
		@{_attval XREFLABEL 2}
		@{_break xrefdone yes}
		@{_followrel descendant TITLE 8001}
Ignore:		all
-
SpecID: 8001
-
SpecID:		2
StartText:	@{XREFLABEL}
Set:		xrefdone yes
Ignore:		all
-
# used in PART to find FOOTNOTE elements and pre-number them
SpecID:		3
StartCode:	GatherFootnote @{ID ""}
-
SpecID:		4
StartCode:	OpenDocument @{host} @{basename} "@{date}"
Ignore:		all
-
# used in PART to find the PARTINTRO element (if any) and save
# its ID for use as the FIRST-PAGE attribute of SDLDOC
SpecID:		5
StartCode:	set partIntroId @{ID ""}
Ignore:		all
-
# used in TABLE to emit a blank FORM so we have a place to hang
# the TITLE
SpecID:		6
StartCode:	PushForm "" "" ""
Ignore:		all
-
# used in TGROUP, we want to handle TFOOT out of order so we need an
# action that will let us embed tcl-code in a text field (because the
# text fields are processed before and after the code fields
SpecID:		7
EndCode:	EndTGroup
Ignore:		all
-
# used in DOCINFO, we first catch all the appendices and emit the
# level 0 ones so the go out before the level 1 hometopic
SpecID:		8
StartText:	@{_attval ROLE notoc 9}
Ignore:		all
-
SpecID:		9
StartCode:	StartAppendix @{_gi} @{ID ""} @{ROLE ""}
-
SpecID:		10
StartCode:	set numberOfColSpecs @{_nchild COLSPEC}
Ignore:		all
-
SpecID:		11
StartCode:	CheckSynopFragmentRef @{_gi} @{ID ""}
Ignore:		all
-
SpecID:		12
StartCode:	Title @{ID ""} PART
EndCode:	CloseTitle @{_parent}
-
SpecID:		13
StartCode:	StartPart @{ID ""}
Ignore:		all
-
