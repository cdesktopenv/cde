#!/usr/bin/tclsh
# set the global variables
set nextId        0  ;# gets incremented before each use

set errorCount    0  ;# number of user errors
set warningCount  0  ;# number of user warnings

set havePartIntro 0  ;# need to emit a hometopic

set firstPInBlock 0  ;# allows a different SSI for first P
set inBlock       "" ;# holds "</BLOCK>\n" when in a BLOCK
set inVirpage     "" ;# holds "</VIRPAGE>\n" when in a VIRPAGE
set needFData     "" ;# holds "<FDATA>\n" if needed (starting a FORM)
set inP           0  ;# flag that we're in an SDL paragraph

set formStack     {} ;# need to stack FORMs when they contain others

set listStack     {} ;# holds type of list and spacing for ListItem

# create some constants for converting list count to ordered label
set ROMAN0   [list "" I II III IV V VI VII VIII IX]
set ROMAN10  [list "" X XX XXX XL L LX LXX LXXX XC]
set ROMAN100 [list "" C CC CCC CD D DC DCC DCCC CM]
set roman0   [list "" i ii iii iv v vi vii viii ix]
set roman10  [list "" x xx xxx xl l lx lxx lxxx xc]
set roman100 [list "" c cc ccc cd d dc dcc dccc cm]
set ALPHABET [list "" A B C D E F G H I J K L M N O P Q R S T U V W X Y Z]
set alphabet [list "" a b c d e f g h i j k l m n o p q r s t u v w x y z]
set DIGITS   [list  0 1 2 3 4 5 6 7 8 9]
set NZDIGITS [list "" 1 2 3 4 5 6 7 8 9]

# specify the "level" value to be given to VIRPAGEs (based on SSI);
# the indexes for this associative array are also used to determine
# whether the closing of a DocBook Title should re-position the
# snbLocation (because the SNB follows HEADs, if any)
set virpageLevels(FOOTNOTE)     0
set virpageLevels(TITLE)        0
set virpageLevels(AUTHORGROUP)  0
set virpageLevels(ABSTRACT)     0
set virpageLevels(REVHISTORY)   0
set virpageLevels(LEGALNOTICE)  0
set virpageLevels(PARTINTRO)    1
set virpageLevels(CHAPTER)      2
set virpageLevels(APPENDIX)     2
set virpageLevels(BIBLIOGRAPHY) 2
set virpageLevels(GLOSSARY)     2
set virpageLevels(INDEX)        2
set virpageLevels(LOT)          2
set virpageLevels(PREFACE)      2
set virpageLevels(REFENTRY)     2
set virpageLevels(REFERENCE)    2
set virpageLevels(TOC)          2
set virpageLevels(SECT1)        3
set virpageLevels(SECT2)        4
set virpageLevels(SECT3)        5
set virpageLevels(SECT4)        6
set virpageLevels(SECT5)        7

# assume the first ID used is SDL-RESERVED1 - if we get a INDEXTERM
# before anything has started, default to the assumed ID
set mostRecentId "SDL-RESERVED1"

# a counter for use in pre-numbering footnotes - will create an
# associative array indexed by "FOOTNOTE ID=" values to hold
# the number of the FOOTNOTE for use by FOOTNOTEREF
set footnoteCounter 0

# the absolute byte offset into the output file where the SNB should be
# inserted by the second pass - the location and snb get saved at
# the end of each VIRPAGE with a little special handling for the
# SDLDOC SNB, the entire snb gets written to the .snb file at
# the close of the document after the saved locations get incremented
# by the size of the index
set snbLocation 0

# normally, we dafault paragraphs to no TYPE= attribute; when in an
# EXAMPLE, for instance, we need to default to TYPE="LITERAL"
set defaultParaType ""


# print internal error message and exit
proc InternalError {what} {
    global errorInfo

    error $what
}


# print a warning message
proc UserWarning {what location} {
    global warningCount

    puts stderr "DtDocBook User Warning: $what"
    if {$location} {
	PrintLocation
    }
    incr warningCount
}


# print an error message plus the location in the source file of the
# error; if we get more than 100 errors, quit
proc UserError {what location} {
    global errorCount

    puts stderr "DtDocBook User Error: $what"
    if {$location} {
	PrintLocation
    }
    if {[incr errorCount] >= 100} {
	puts stderr "Too many errors - quitting"
	exit 1
    }
}


# set up a default output string routine so everything works even
# if run outside of instant(1)
if {[info commands OutputString] == ""} {
    proc OutputString {string} {
	puts -nonewline "$string"
    }
}


# emit a string to the output stream
proc Emit {string} {
    OutputString $string
}


# push an item onto a stack (a list); return item pushed
proc Push {stack item} {
    upvar $stack s
    lappend s $item
    return $item
}


# pop an item from a stack (i.e., a list); return the popped item
proc Pop {stack} {
    upvar $stack s
    set top [llength $s]
    if {!$top} {
	InternalError "Stack underflow in Pop"
    }
    incr top -1
    set item [lindex $s $top]
    incr top -1
    set s [lrange $s 0 $top]
    return $item
}


# return the top of a stack (the stack is a list)
proc Peek {stack} {
    upvar $stack s
    set top [llength $s]
    incr top -1
    set item [lindex $s $top]
}


# replace the top of the stack with the new item; return the item
proc Poke {stack item} {
    upvar $stack s
    set top [llength $s]
    incr top -1
    set s [lreplace $s $top $top $item]
    return $item
}


# emit an ID and save it for reference as the most recently emitted ID;
# the saved value will be used to mark locations for index entries
proc Id {name} {
    global mostRecentId

    set mostRecentId $name
    return "ID=\"$name\""
}


# emit an ANCHOR into the SDL stream; if the passed id is empty, don't
# emit the anchor
proc Anchor {id} {
    if {$id != ""} {
	Emit "<ANCHOR [Id $id]>"
    }
}


# emit an ANCHOR into the SDL stream; if the passed id is empty, don't
# emit the anchor; if we're not in an SDL P yet, start one and use
# the id there rather than emitting an SDL ANCHOR
proc AnchorInP {id} {
    global inP

    if {$id != ""} {
	if {!$inP} {
	    StartParagraph $id "P" ""
	} else {
	    Emit "<ANCHOR [Id $id]>"
	}
    }
}


# set up containers for the IDs of the blocks holding marks - clear
# on entry to each <virpage> but re-use within the <virpage> as much as
# possible; we need two each of the regular and loose versions because
# we need to alternate to avoid the <form> runtime code thinking we're
# trying to span columns
#
# specify a routine to (re-)initialize all the variables for use
# in ListItem
proc ReInitPerMarkInfo {} {
    global validMarkArray

    foreach mark [array names validMarkArray] {
	global FIRSTTIGHT${mark}Id
	set    FIRSTTIGHT${mark}Id  ""

	global FIRSTLOOSE${mark}Id
	set    FIRSTLOOSE${mark}Id  ""

	global TIGHT${mark}Id0
	set    TIGHT${mark}Id0 ""

	global TIGHT${mark}Id1
	set    TIGHT${mark}Id1 ""

	global LOOSE${mark}Id0
	set    LOOSE${mark}Id0 ""

	global LOOSE${mark}Id1
	set    LOOSE${mark}Id1 ""

	global TIGHT${mark}num
	set    TIGHT${mark}num 1

	global LOOSE${mark}num
	set    LOOSE${mark}num 1
    }
}


# add a new mark to the mark array and initialize all the variables
# that depend on the mark; the index for the mark is just the mark
# itself with the square brackets removed and whitespace deleted;
# we've already guaranteed that the mark will be of the form
# "[??????]" (open-square, 6 characters, close-square) and that this
# mark isn't in the array already
proc AddToMarkArray {mark} {
    global validMarkArray

    set m [string range $mark 1 6]
    set m [string trim $m]

    set validMarkArray($m) $mark

    global FIRSTTIGHT${m}Id
    set    FIRSTTIGHT${m}Id  ""

    global FIRSTLOOSE${m}Id
    set    FIRSTLOOSE${m}Id  ""

    global TIGHT${m}Id0
    set    TIGHT${m}Id0 ""

    global TIGHT${m}Id1
    set    TIGHT${m}Id1 ""

    global LOOSE${m}Id0
    set    LOOSE${m}Id0 ""

    global LOOSE${m}Id1
    set    LOOSE${m}Id1 ""

    global TIGHT${m}num
    set    TIGHT${m}num 1

    global LOOSE${m}num
    set    LOOSE${m}num 1

    return $m
}


# start a new paragraph; start a block if necessary
proc StartParagraph {id ssi type} {
    global inBlock firstPInBlock inP defaultParaType

    # close any open paragraph
    if {$inP} { Emit "</P>\n" }

    # if not in a BLOCK, open one
    if {$inBlock == ""} { StartBlock "" "" "" 1 }

    Emit "<P"
    if {$id != ""} { Emit " [Id $id]" }

    # don't worry about whether we're the first para if there's no SSI
    if {$ssi != ""} {
	set firstString ""
	if {$firstPInBlock} {
	    if {$ssi == "P"} {
		set firstString 1
	    }
	    set firstPInBlock 0
	}
	Emit " SSI=\"$ssi$firstString\""
    }

    if {$type == ""} {
	Emit $defaultParaType
    } else {
	Emit " TYPE=\"$type\""
    }

    Emit ">"

    set inP 1
    set inBlock "</P>\n</BLOCK>\n"
}


# conditionally start a paragraph - that is, only start a new
# paragraph if we aren't in one already
proc StartParagraphMaybe {id ssi type} {
    global inP

    if {$inP} {
	Anchor $id
    } else {
	StartParagraph $id $ssi $type
    }
}


# start a compound paragraph - a compound paragraph is when a Para
# contains some other element that requires starting its own SDL
# BLOCK or FORM, e.g., VariableList; we need to create a FORM to hold
# the Para and its parts - put the id and ssi on the FORM rather than
# the contained Ps.
proc StartCompoundParagraph {id ssi type} {
    global firstPInBlock

    if {$ssi != ""} {
	if {$firstPInBlock} {
	    set firstString 1
	} else {
	    set firstString ""
	}
	PushForm "" $ssi$firstString $id
    } else {
	PushForm "" "" $id
    }

    StartParagraph "" "" ""
}


# given the path of parentage of an element, return its n'th ancestor
# (parent == 1), removing the child number (if any); e.g., convert
# "PART CHAPTER(0) TITLE" into "CHAPTER" if level is 2
proc Ancestor {path level} {
    if {$level < 0} { return "_UNDERFLOW_" }

    set last [llength $path]
    incr last -1

    if {$level > $last} { return "_OVERFLOW_" }

    # invert "level" about "last" so we count from the end
    set level [expr "$last - $level"]

    set parent [lindex $path $level]
    set parent [lindex [split $parent "("] 0] ;# remove child #
}


# start a HEAD element for the DocBook Title - use the parent's
# GI in the SSI= of the HEAD except that all titles to things in
# their own topic (VIRPAGE) will use an SSI of CHAPTER-TITLE;
# if we are in a topic with a generated id (e.g., _glossary or
# _title), we might have saved an id or two in savedId to be
# emitted in the HEAD
proc Title {id parent} {
    global virpageLevels partID inP savedId

    Emit "<HEAD"

    if {$id != ""} {
	Emit " ID=\"$id\""
    }

    # if we are the Title of a PartIntro, we'd like to emit the
    # partID as an anchor so linking to the volume will succeed;
    # add it to the list of saved ids to be emitted
    if {$parent == "PARTINTRO"} {
	lappend savedId $partID
    }

    # make the HEAD for all topics (VIRPAGE) have an SSI of
    # "CHAPTER-HEAD", use LEVEL to distinguish between them
    set topicNames [array names virpageLevels]
    foreach name $topicNames {
	if {$parent == $name} {
	    set parent CHAPTER
	    break
	}
    }

    Emit " SSI=\"$parent-TITLE\">"

    # being in a HEAD is equivalent to being in a P for content model
    # but we use "incr" instead of setting inP directly so that if we
    # are in a P->HEAD, we won't prematurely clear inP when leaving
    # the HEAD
    incr inP

    if {[info exists savedId]} {
	foreach id $savedId {
	    Anchor $id
	}
	unset savedId
    }
}


# close a HEAD element for a DocBook Title - if the Title is one for
# a DocBook element that gets turned into an SDL VIRPAGE, set the
# location for the insertion of an SNB (if any) to follow the HEAD
proc CloseTitle {parent} {
    global snbLocation virpageLevels inP

    Emit "</HEAD>\n"

    # we incremented inP on entry to the HEAD so decrement it here
    incr inP -1

    # get a list of DocBook elements that start VIRPAGEs
    set names [array names virpageLevels]

    # add the start of the help volume, PART, to the list
    lappend names PART

    # if our parent is a VIRPAGE creator or the start of the document,
    # we must be dealing with the heading of a VIRPAGE or with the
    # heading of the SDLDOC so move the spot where we want to include
    # the SNB to immediately after this HEAD
    foreach name $names {
	if {$name == $parent} {
	    set snbLocation [tell stdout]
	    break
	}
    }
}


# open an SGML tag - add punctuation as guided by the class attribute
proc StartSgmlTag {id class} {
    switch $class {
	ELEMENT     {set punct "&<"}
	ATTRIBUTE   {set punct ""}
	GENENTITY   {set punct "&&"}
	PARAMENTITY {set punct "%"}
    }
    Emit $punct
}


# close an SGML tag - add punctuation as guided by the class attribute
proc EndSgmlTag {class} {
    switch $class {
	ELEMENT     {set punct ">"}
	ATTRIBUTE   {set punct ""}
	GENENTITY   {set punct ";"}
	PARAMENTITY {set punct ";"}
    }
    Emit $punct
}


# end a trademark, append a symbol if needed
proc EndTradeMark {class} {
    switch $class {
	SERVICE    {set punct ""}
	TRADE      {set punct "<SPC NAME=\"\[trade \]\">"}
	REGISTERED {set punct "<SPC NAME=\"\[reg   \]\">"}
	COPYRIGHT  {set punct "<SPC NAME=\"\[copy  \]\">"}
    }
    Emit "</KEY>$punct"
}


# handle the BridgeHead tag; emit a FORM to hold a HEAD and put the
# BridgeHead there - use the procedure Title to do all the work, the
# renderas attributre simply become the parent to Title
proc StartBridgeHead {id renderas} {
    PushForm "" "" ""

    # default renderas to CHAPTER - arbitrarily
    if {$renderas == "OTHER"} {
	set renderas CHAPTER
    }
    Title $id $renderas
}


# end a BridgeHead; we need to close out the SDL HEAD and close the
# FORM - use CloseTitle to close out the HEAD but give it a null
# parent so it doesn't try to save the SNB now
proc EndBridgeHead {} {
    CloseTitle ""
    PopForm
}


# end a paragraph
proc EndParagraph {} {
    global inP inBlock

    if {$inP} {
	Emit "</P>\n"
    }

    # we set inBlock to </P></BLOCK> in StartParagraph so we need
    # to remove the </P> here; if we're continuing a paragraph
    # inBlock will have been set to "" when we closed the BLOCK to
    # open the embedded FORM so we need to leave it empty to cause
    # a new BLOCK to be opened
    if {$inBlock != ""} {
	set inBlock "</BLOCK>\n"
    }

    # and flag that we're not in a paragraph anymore
    set inP 0
}


# continue a PARA that was interrupted by something from %object.gp;
# first pop the FORM that held the indent attributes for the object
# then start a new paragraph with an SSI that indicates we are
# continuing
proc ContinueParagraph {} {
    PopForm
    StartParagraph "" "P-CONT" ""
}


# start a new BLOCK element; close the old one, if any;
# return the ID in case we allocated one and someone else wants it
proc StartBlock {class ssi id enterInForm} {
    global needFData inBlock formStack nextId firstPInBlock inP

    # if we are the first BLOCK in a FORM, emit the FDATA tag
    Emit $needFData; set needFData ""

    # close any open block and flag that we're opening one
    # but that we haven't seen a paragraph yet
    Emit $inBlock
    set inBlock "</BLOCK>\n"
    set inP 0

    # if a FORM is in progress, add our ID to the row vector,
    # FROWVEC - create an ID if one wasn't provided
    if {$enterInForm && [llength $formStack] != 0} {
	if {$id == ""} { set id "SDL-RESERVED[incr nextId]" }
	AddRowVec $id
    }

    # open the BLOCK
    Emit "<BLOCK"
    if {$id    != ""} { Emit " [Id $id]" }
    if {$class != ""} { Emit " CLASS=\"$class\"" }
    if {$ssi   != ""} { Emit " SSI=\"$ssi\"" }
    Emit ">\n"

    # and flag that the next paragraph is the first in a block
    set firstPInBlock 1

    return $id
}


# close any open BLOCK - no-op if not in a BLOCK otherwise emit the
# BLOCK etag or both BLOCK and P etags if there's an open paragraph
proc CloseBlock {} {
    global inBlock inP

    if {$inBlock != ""} {
	Emit $inBlock   ;# has been prefixed with </P> if needed
	set inBlock ""
	set inP 0
    }
}


# add another FROWVEC element to the top of the form stack
proc AddRowVec {ids} {
    global formStack

    Push formStack "[Pop formStack]<FROWVEC CELLS=\"$ids\">\n"
}


# start a new FORM element within a THead, TBody or TFoot ("push"
# because they're recursive); return the ID in case we allocated one;
# do not enter the ID in the parent's FROWVEC, we'll do that later
# from the rowDope that we build to compute horizontal spans and
# vertical straddles
proc PushFormCell {ssi id} {
    global needFData formStack nextId

    Emit $needFData     ;# in case we're the first in an old FORM
    set needFData "<FDATA>\n" ;# and were now starting a new FORM

    # close any open BLOCK
    CloseBlock

    # make sure we have an ID
    if {$id == ""} { set id "SDL-RESERVED[incr nextId]" }

    # add a new (empty) string to the formStack list (i.e., push)
    Push formStack {}

    Emit "<FORM"
    if {$id  != ""} { Emit " [Id $id]" }
    Emit " CLASS=\"CELL\""
    if {$ssi != ""} { Emit " SSI=\"$ssi\"" }
    Emit ">\n"

    return $id
}


# start a new FORM element ("push" because they're recursive);
# return the ID in case we allocated one
proc PushForm {class ssi id} {
    global needFData formStack nextId

    Emit $needFData     ;# in case we're the first in an old FORM
    set needFData "<FDATA>\n" ;# and were now starting a new FORM

    # close any open BLOCK
    CloseBlock

    if {[llength $formStack] != 0} {
	# there is a <form> in progress
	if {$id == ""} { set id "SDL-RESERVED[incr nextId]" }
	AddRowVec $id
    }

    # add a new (empty) string to the formStack list (i.e., push)
    Push formStack {}

    Emit "<FORM"
    if {$id    != ""} { Emit " [Id $id]" }
    if {$class != ""} { Emit " CLASS=\"$class\"" }
    if {$ssi   != ""} { Emit " SSI=\"$ssi\"" }
    Emit ">\n"

    return $id
}


# start a new FORM element to hold a labeled list item ("push"
# because they're recursive), adding it to an already open two
# column FORM, if any; we assume the first ID is the block holding
# the label and always defined on entry but we return the second
# ID in case we allocated one
proc PushFormItem {ssi id1 id2} {
    global needFData formStack nextId

    Emit $needFData ;# in case we're the first in an old FORM
    set needFData "<FDATA>\n"  ;# and were now starting a new FORM

    # close any open BLOCK
    CloseBlock

    if {$id2 == ""} { set id2 "SDL-RESERVED[incr nextId]" }

    if {[llength $formStack] != 0} {
	# there is a <form> in progress
	if {$id2 == ""} { set id2 "SDL-RESERVED[incr nextId]" }
	AddRowVec "$id1 $id2"
    }

    # add a new (empty) string to the formStack list (i.e., push)
    Push formStack {}

    Emit "<FORM [Id $id2] CLASS=\"ITEM\""
    if {$ssi   != ""} { Emit " SSI=\"$ssi\"" }
    Emit ">\n"

    return $id2
}


# close out a THead, TBody or TFoot; create the FROWVEC from the
# rowDope - save it if we aren't popping the FORM yet (which happens
# if no ColSpec elements were given at the THead or TFoot level and
# we're merging one, the other or both with the TBody), emit the
# saved ROWVEC, if any, and newly created one if we are popping the
# FORM in which case we also want to blow away the top of the
# formStack; we can also blow away the current rowDope here since
# we write or save the FROWVEC and we're done with the dope vector
proc PopTableForm {parent gi popForm} {
    global formStack

    # get the proper row descriptor(s) and number of columns
    if {$parent == "ENTRYTBL"} {
	upvar #0 entryTableRowDope      rowDope
	upvar #0 entryTableSavedFRowVec fRowVec
	global entryTableAttributes
	set nCols $entryTableAttributes(cols)
    } else {
	upvar #0 tableGroupRowDope      rowDope
	upvar #0 tableGroupSavedFRowVec fRowVec
	global tableGroupAttributes
	set nCols $tableGroupAttributes(cols)
    }

    # flush the unused formStack entry if we're actually popping
    if {$popForm} {
	Pop formStack
    }

    # determine whether we are a "header", i.e., inside a TFoot or
    # THead
    if {$gi == "TBODY"} {
	set hdr ""
    } else {
	set hdr " HDR=\"YES\""
    }

    # if actually popping the FORM here (i.e., writing the FSTYLE),
    # emit the FSTYLE wrapper
    if {$popForm} {
	Emit "</FDATA>\n<FSTYLE"
	if {$nCols > 1} {
	    Emit " NCOLS=\"$nCols\""
	}
	Emit ">\n"
    }
    set currentRow 1
    set nRows $rowDope(nRows)
    while {$currentRow <= $nRows} {
	append fRowVec "<FROWVEC$hdr CELLS=\""
	append fRowVec $rowDope(row$currentRow)
	append fRowVec "\">\n"
	incr currentRow
    }
    unset rowDope
    # if actually popping the FORM here (i.e., writing the FSTYLE),
    # emit the FROWVEC elements, zero out the saved fRowVec and close
    # the FSTYLE wrapper
    if {$popForm} {
	Emit $fRowVec
	set fRowVec ""
	Emit "</FSTYLE>\n</FORM>\n"
    }
}


# close out one FORM on the stack; if there hasn't been a block added
# to the FORM, create an empty one to make it legal SDL
proc PopForm {} {
    global formStack

    if {[Peek formStack] == ""} {
	# oops, empty FROWVEC means empty FORM so add an empty BLOCK
	StartBlock "" "" "" 1
    }

    # close any open BLOCK
    CloseBlock

    # write out the saved FROWVEC information wrapped in an FSTYLE
    set openStyle "</FDATA>\n<FSTYLE>\n"
    set closeStyle "</FSTYLE>\n</FORM>"
    Emit "$openStyle[Pop formStack]$closeStyle\n"
}


# close out one N columned FORM on the stack; if there hasn't been a
# block added to the FORM, create an empty one to make it legal SDL
proc PopFormN {nCols} {
    global formStack

    if {[Peek formStack] == ""} {
	# oops, empty FROWVEC means empty FORM so add an empty BLOCK
	# and bring this down to a single column FORM containing only
	# the new BLOCK
	StartBlock "" "" "" 1
	set nCols 1
    }

    # close any open BLOCK
    CloseBlock

    # write out the saved FROWVEC information wrapped in an FSTYLE
    set openStyle "</FDATA>\n<FSTYLE NCOLS=\"$nCols\">\n"
    set closeStyle "</FSTYLE>\n</FORM>"
    Emit "$openStyle[Pop formStack]$closeStyle\n"
}


# check the Role attribute on lists to verify that it's either "LOOSE"
# or "TIGHT"; return upper cased version of verified Role
proc CheckSpacing {spacing} {
    set uSpacing [string toupper $spacing]
    switch $uSpacing {
	LOOSE   -
	TIGHT   {return $uSpacing}
    }
    UserError "Bad value (\"$role\") for Role attribute in a list" yes
    return LOOSE
}


# start a simple list - if Type is not INLINE, we need to save the
# Ids of the BLOCKs we create and lay them out in a HORIZONTAL or
# VERTICAL grid when we have them all
proc StartSimpleList {id type spacing parent} {
    global listStack firstString

    if {$type == "INLINE"} {
	StartParagraphMaybe $id P ""
    } else {
	# if we are inside a Para, we need to issue a FORM to hang the
	# indent attributes on
	if {$parent == "PARA"} {
	    PushForm "" "INSIDE-PARA" ""
	}

	# insure "spacing" is upper case and valid (we use it in the SSI)
	set spacing [CheckSpacing $spacing]

	# save the list type and spacing for use by <Member>;
	set listDope(type)     simple
	set listDope(spacing) $spacing
	Push listStack [array get listDope]

	PushForm LIST SIMPLE-$spacing $id
	set firstString "FIRST-"
    }
}


# end a simple list - if Type was INLINE, we're done, otherwise, we
# need to lay out the grid based on Type and Columns
proc EndSimpleList {columns type parent} {
    global listStack lastList listMembers

    if {$columns == 0} {
	UserWarning "must have at least one column in a simple list" yes
	set columns 1
    }

    if {$type != "INLINE"} {
	# get the most recently opened list and remove it from the stack
	array set lastList [Pop listStack]

	# calculate the number of rows and lay out the BLOCK ids
	# as per the type attribute
	set length [llength $listMembers]
	set rows   [expr ($length + $columns - 1) / $columns]
	set c 0
	set r 0
	set cols $columns
	if {$type == "HORIZ"} {
	    incr cols -1
	    while {$r < $rows} {
		set ids [lrange $listMembers $c [incr c $cols]]
		AddRowVec $ids
		incr c
		incr r
	    }
	} else {
	    set lastRowLength [expr $cols - (($rows * $cols) - $length)]
	    incr rows -1
	    while {$r <= $rows} {
		set i   $r
		set ids ""
		set c   0
		if {$r == $rows} {
		    set cols $lastRowLength
		}
		while {$c < $cols} {
		    lappend ids [lindex $listMembers $i]
		    incr i $rows
		    if {$c < $lastRowLength} {
			incr i
		    }
		    incr c
		}
		AddRowVec $ids
		incr r
	    }
	}
	unset listMembers

	# close the open FORM using the newly generated ROWVECs
	PopFormN $columns

	# if we are inside a Para, we need to close the FORM we issued for
	# hanging the indent attributes
	if {$parent == "PARA"} {
	    ContinueParagraph
	}
    }
}


# collect another Member of a SimpleList; if we're a Vert(ical) or
# Horiz(ontal) list, don't put the BLOCK's id on the list's FORM
# yet - we need to collect them all and lay them out afterward in
# EndSimpleList; if we're an Inline list, don't create a BLOCK, we'll
# add punctuation to separate them in EndMember
proc StartMember {id type} {
    global nextId listStack firstString listMembers

    if {$type == "INLINE"} {
	Anchor $id
    } else {
	# put it in a BLOCK, make sure we have an id and add it to
	# the list of members
	if {$id == ""} {
	    set id SDL-RESERVED[incr nextId]
	}
        lappend listMembers $id

	# get the current list info
	array set listTop [Peek listStack]
	set spacing $listTop(spacing)

	# use an SSI of, e.g., FIRST-LOOSE-SIMPLE
	StartBlock ITEM $firstString$spacing-SIMPLE $id 0
	StartParagraph "" P ""
	set firstString ""
    }
}


# end a SimpleList Member; if it's an Inline list, emit the
# punctuation ("", ", " or "and") based on the position of the
# Member in the list - otherwise, do nothing and the StartBlock from
# the next Member or the PopFormN in EndSimpleList will close the
# current one out
proc EndMember {type punct} {
    if {$type == "INLINE"} {
	Emit $punct
    }
}


# check the value of a ITEMIZEDLIST MARK - issue warning and default 
# it to BULLET if it's unrecognized
proc ValidMark {mark} {
    global validMarkArray

    if {[string toupper $mark] == "PLAIN"} { return PLAIN }

    # if an SDATA entity was used, it'll have spurious "\|" at the
    # beginning and the end added by [n]sgmls
    if {[string match {\\|????????\\|} $mark]} {
	set mark [string range $mark 2 9]
    }

    if {![string match {\[??????\]} $mark]} {
	UserError "Unknown list mark \"$mark\" specified, using PLAIN" yes
	return PLAIN
    } else {
	foreach m [array names validMarkArray] {
	    if {$validMarkArray($m) == $mark} {return $m}
	}
	return [AddToMarkArray $mark]
    }
}


# start an itemized list
proc ItemizedList {id mark spacing parent} {
    global listStack firstString

    # if we are inside a Para, we need to issue a FORM to hang the
    # indent attributes on
    if {$parent == "PARA"} {
	PushForm "" "INSIDE-PARA" ""
    }

    # make sure we recognize the mark
    set mark [ValidMark $mark]

    # insure "spacing" is upper case and valid (we use it in the SSI)
    set spacing [CheckSpacing $spacing]

    # save the list type, mark and spacing for use by <ListItem>
    set listDope(type)   itemized
    set listDope(spacing) $spacing
    set listDope(mark)    $mark
    Push listStack [array get listDope]

    # create a FORM to hold the list items
    if {$mark == "PLAIN"} {
	PushForm LIST "PLAIN-$spacing" $id
    } else {
	PushForm LIST "MARKED-$spacing" $id
    }

    set firstString "FIRST-"
}


# turn absolute item count into proper list number e.g., 2, B, or II
proc MakeOrder {numeration count} {
    global ROMAN0 ROMAN10 ROMAN100
    global roman0 roman10 roman100
    global ALPHABET alphabet
    global NZDIGITS DIGITS

    if {$count == ""} { return "" }

    if {$count > 999} { set count 999 } ;# list too big - cap it

    # initialize the 3 digits of the result value
    set c1 0
    set c2 0
    set c3 0

    # first get the 3 digits in the proper base (26 or 10)
    switch -exact $numeration {
	UPPERALPHA -
	LOWERALPHA {
	    set c3 [expr "$count % 26"]
	    if {$c3 == 0} { set c3 26 }
	    if {[set count [expr "$count / 26"]]} {
		set c2 [expr "$count % 26"]
		if {$c2 == 0} { set c2 26 }
		set c1 [expr "$count / 26"]
	    }
	}
	UPPERROMAN -
	LOWERROMAN -
	default {
	    set c3 [expr "$count % 10"]
	    if {[set count [expr "$count / 10"]]} {
		set c2 [expr "$count % 10"]
		if {[set count [expr "$count / 10"]]} {
		    set c1 [expr "$count % 10"]
		}
	    }
	}
    }

    # then point to proper conversion list(s)
    switch -exact $numeration {
	UPPERALPHA {
	    set c1List $ALPHABET
	    set c2List $ALPHABET
	    set c3List $ALPHABET
	}
	LOWERALPHA {
	    set c1List $alphabet
	    set c2List $alphabet
	    set c3List $alphabet
	}
	UPPERROMAN {
	    set c3List $ROMAN0
	    set c2List $ROMAN10
	    set c1List $ROMAN100
	}
	LOWERROMAN {
	    set c3List $roman0
	    set c2List $roman10
	    set c1List $roman100
	}
	default {
	    set c1List $DIGITS
	    set c2List $DIGITS
	    set c3List $DIGITS
	    if {$c1 == 0} {
		set c1List $NZDIGITS
		if {$c2 == 0} {
		    set c2List $NZDIGITS
		}
	    }
	}
    }

# and do the conversion
set    string [lindex $c1List $c1]
append string [lindex $c2List $c2]
append string [lindex $c3List $c3]
append string .

return $string
}


# start an ordered list
proc OrderedList {id numeration inheritNum continue spacing parent} {
    global listStack lastList firstString

    # if we are inside a Para, we need to issue a FORM to hang the
    # indent attributes on
    if {$parent == "PARA"} {
	PushForm "" "INSIDE-PARA" ""
    }

    # make sure the INHERIT param is compatible with enclosing list
    if {$inheritNum == "INHERIT"} {
	if {[llength $listStack] > 0} {
	    array set outerList [Peek listStack]
	    if {$outerList(type) != "ordered"} {
		UserError "Can only inherit numbering from an ordered list" yes
		set inheritNum IGNORE
	    }
	} else {
	    UserError \
		  "Attempt to inherit a list number with no previous list" yes
	    set inheritNum IGNORE
	}
    }

    # make sure the CONTINUE param is compatible with previous list;
    # also inherit numeration here if unset (else error if different)
    # and we're continuing
    if {$continue == "CONTINUES"} {
	if {![array exists lastList]} {
	    # nothing to inherit from
	    UserError "Attempt to continue a list with no previous list" yes
	    set continue RESTARTS
	} elseif {$lastList(type) != "ordered"} {
	    UserError "Only ordered lists can be continued" yes
	    set continue RESTARTS
	} elseif {$numeration == ""} {
	    set numeration $lastList(numeration)
	}  elseif {$lastList(numeration) != $numeration} {
	    UserError "Can't continue a list with different numeration" yes
	    set continue RESTARTS
	}
    }

    # if no numeration specified, default to Arabic
    if {$numeration == ""} {
	set numeration ARABIC
    }

    set count 0          ;# assume we are restarting the item count
    set inheritString "" ;# fill in later if set

    if {$continue == "CONTINUES"} {
	# continuing means use the old inherit string (if any) and
	# pick up with the old count
	set count $lastList(count)
	if {($lastList(inheritString) != "") && ($inheritNum != "INHERIT")} {
	    UserError \
	       "Must continue inheriting if continuing list numbering" yes
	    set inheritNum INHERIT
	}
    }

    if {$inheritNum == "INHERIT"} {
	# inheriting a string to preface the current number - e.g., "A.1."
	set inheritString $outerList(inheritString)
	append inheritString \
	    [MakeOrder $outerList(numeration) $outerList(count)]
    }

    # insure "spacing" is upper case and valid (we use it in the SSI)
    set spacing [CheckSpacing $spacing]

    # save the list type and spacing for use by <ListItem>
    set listDope(type)           ordered
    set listDope(spacing)       $spacing
    set listDope(numeration)    $numeration
    set listDope(inheritString) $inheritString
    set listDope(count)         $count
    Push listStack [array get listDope]

    # create a FORM to hold the list items
    PushForm LIST "ORDER-$spacing" $id

    set firstString "FIRST-"
}


# start a variable list (i.e., labeled list)
proc VariableList {id role parent} {
    global listStack firstString

    # if we are inside a Para, we need to issue a FORM to hang the
    # indent attributes on
    if {$parent == "PARA"} {
	PushForm "" "INSIDE-PARA" ""
    }

    # parse out the possible role values (loose/tight and
    # wrap/nowrap)
    set role [split [string toupper $role]]
    set role1 [lindex $role 0]
    set role2 ""
    set length [llength $role]
    if {$length > 1} {
	set role2 [lindex $role 1]
    }
    if {$length > 2} {
	UserError "Too many values (> 2) for Role in a VARIABLELIST" yes
    }
    set spacing ""
    set wrap ""
    switch $role1 {
	LOOSE   -
	TIGHT   {set spacing $role1}
	WRAP    -
	NOWRAP  {set wrap $role1}
	default {UserError "Bad value for Role ($role1) in a VARIABLELIST" yes
		}
    }
    switch $role2 {
	""      {#}
	LOOSE   -
	TIGHT   {if {$spacing == ""} {
		     set spacing $role2
		 } else {
		     UserError "Only specify LOOSE/TIGHT once per Role" yes
		 }
		}
	WRAP    -
	NOWRAP  {if {$wrap == ""} {
		     set wrap $role2
		 } else {
		     UserError "Only specify WRAP/NOWRAP once per Role" yes
		 }
		}
	default {UserError "Bad value for Role ($role2) in a VARIABLELIST" yes
		}
    }
    if {$spacing == ""} {
	set spacing "LOOSE"
    }
    if {$wrap == ""} {
	set wrap "NOWRAP"
    }

    # insure "spacing" is upper case and valid (we use it in the SSI)
    set spacing [CheckSpacing $spacing]

    # save the list type and spacing for use by <ListItem>;
    # also save a spot for the current label ID
    set listDope(type)     variable
    set listDope(spacing) $spacing
    set listDope(labelId) $id
    set listDope(wrap)    $wrap
    Push listStack [array get listDope]

    # create a FORM to hold the list items
    PushForm LIST "VARIABLE-$spacing" $id

    set firstString "FIRST-"
}


# open a variable list entry - create a BLOCK to hold the term(s)
proc VarListEntry {id} {
    global firstString listStack nextId

    # get the list spacing, i.e., TIGHT or LOOSE
    array set listDope [Peek listStack]
    set spacing $listDope(spacing)

    # make sure we have an ID for the label (it goes in a FORM)
    # save the ID for use in PushFormItem
    if {$id == ""} {
	set id SDL-RESERVED[incr nextId]
    }
    array set listDope [Pop listStack]
    set listDope(labelId) $id
    Push listStack [array get listDope]

    StartBlock ITEM "$firstString$spacing-TERMS" $id 0
}

# process a term in a variablelist
proc StartTerm {id} {
    global listStack

    # get the current list info
    array set listTop [Peek listStack]
    set wrap $listTop(wrap)

    set lined ""
    if {$wrap == "NOWRAP"} {
	set lined "LINED"
    }

    StartParagraph $id "P" $lined
}


# process an item in an ordered, variable or itemized list
proc ListItem {id override} {
    global listStack firstString nextId needFData validMarkArray

    # get the current list info
    array set listTop [Peek listStack]
    set spacing $listTop(spacing)

    # if it's an itemized list, are we overriding the mark?
    if {$listTop(type) == "itemized"} {
	if {$override == "NO"} {
	    set mark $listTop(mark)
	} elseif {$override == ""} {
	    set mark PLAIN
	} else {
	    set mark [ValidMark $override]
	}
    }

    if {($listTop(type) == "itemized") && ($mark != "PLAIN")} {
	# marked itemized list, try to reuse an existing mark <BLOCK>
	if {$firstString == ""} {
	    # not a FIRST, calculate the next id index - we flip
	    # between 0 and 1 to avoid column span in viewer
	    set numName $spacing${mark}num  ;# get index name
	    upvar #0 $numName idNum
	    set idNum [expr  "-$idNum + 1"]  ;# flip it
	}
	if {$firstString != ""} {
	    set idName FIRST$spacing${mark}Id
	} else {
	    set idName $spacing${mark}Id$idNum
	}
	upvar #0 $idName labelId
	if {$labelId == ""} {
	    # need to create a <BLOCK> and save the id
	    set labelId "SDL-RESERVED[incr nextId]"
	    Emit $needFData; set needFData ""
	    Emit "<BLOCK [Id $labelId] CLASS=\"ITEM\""
	    Emit " TIMING=\"ASYNC\" "
	    Emit "SSI=\"$firstString$spacing-MARKED\""
	    Emit ">\n<P SSI=\"P1\"><SPC NAME=\"$validMarkArray($mark)\""
	    Emit "></P>\n</BLOCK>\n"
	}
    }

    # emit the SSI and label for an ordered list
    if {$listTop(type) == "ordered"} {
	#  start a block for the label
	set labelId "SDL-RESERVED[incr nextId]"
	Emit $needFData; set needFData ""
	Emit "<BLOCK [Id $labelId] CLASS=\"ITEM\" SSI=\""

	# create, e.g., FIRST-LOOSE-ORDERED
	Emit "$firstString$spacing-ORDERED\">\n"

	# emit the label (inherit string followed by order string)
	# and close the block
	Emit "<P SSI=\"P1\">"
	Emit $listTop(inheritString)
	Emit [MakeOrder $listTop(numeration) [incr listTop(count)]]
	Emit "</P>\n</BLOCK>\n"

	# then update the top of the list stack
	Poke listStack [array get listTop]
    }

    # or just get the label id for a variable (labeled) list - the
    # label was emitted in another production
    if {$listTop(type) == "variable"} {
	set labelId $listTop(labelId)
    }

    # emit a one (for PLAIN) or two column FORM to wrap this list item
    set ssi "$firstString$spacing"
    if {($listTop(type) == "itemized") && ($mark == "PLAIN")} {
	PushForm ITEM $ssi $id
    } else {
	PushFormItem $ssi $labelId $id
    }
    set firstString ""
}


# start a segmented list, e.g.,
#   foo:  fooItem1
#   bar:  barItem1
#
#   foo:  fooItem2
#   bar:  barItem2
proc SegmentedList {id spacing parent} {
    global listStack firstString

    # if we are inside a Para, we need to issue a FORM to hang the
    # indent attributes on
    if {$parent == "PARA"} {
	PushForm "" "INSIDE-PARA" ""
    }

    # insure "spacing" is upper case and valid (we use it in the SSI)
    set spacing [CheckSpacing $spacing]

    # save the list type and spacing for use by <ListItem>;
    set listDope(type)     segmented
    set listDope(spacing) $spacing
    Push listStack [array get listDope]

    # create a FORM to hold the list items
    PushForm LIST "SEGMENTED-$spacing" $id

    set firstString "FIRST-"
}

# emit the SegTitle elements, each in its own BLOCK - we'll reuse
# them on each Seg of each SegListItem
proc StartSegTitle {id} {
    global firstString listStack segTitleList nextId

    # get the list spacing, i.e., TIGHT or LOOSE
    array set listDope [Peek listStack]
    set spacing $listDope(spacing)

    # make sure we have an ID for the label (it goes in a FORM)
    # save the ID for use in PushFormItem
    if {$id == ""} {
	set id SDL-RESERVED[incr nextId]
    }
    lappend segTitleList $id

    # start the block but don't put in on the FORM, we'll put this
    # BLOCK and the one containing the SegListItem.Seg into a two
    # column form later
    StartBlock ITEM "$firstString$spacing-SEGTITLE" $id 0
    set firstString ""

    StartParagraph "" SEGTITLE ""
}


# start a SegListItem - save the id (if any) of the SegListItem to
# be emitted as an anchor in the first Seg
proc StartSegListItem {id} {
    global segListItemNumber segListItemId firstString

    set segListItemId     $id
    set segListItemNumber 0
    set firstString       "FIRST-"
}


# process a Seg in a SegListItem - get the corresponding SegTitle ID
# and create a BLOCK for the item then put the pair into the FORM that
# was created back in SegmentedList
proc StartSeg {id isLastSeg} {
    global segTitleList segListItemNumber segListItemId firstString
    global listStack nextId

    set nTitles [llength $segTitleList]
    if {$segListItemNumber >= $nTitles} {
	UserError \
	   "More Seg than SegTitle elements in a SegmentedList" yes
	return
    }
    if {$isLastSeg} {
	if {[expr "$segListItemNumber" + 1] != $nTitles} {
	    UserError \
	       "More SegTitle than Seg elements in a SegmentedList" yes
	}
    }

    # get the current list info
    array set listTop [Peek listStack]
    set spacing $listTop(spacing)

    # open a BLOCK and P to hold the Seg content; put any user
    # supplied Id on the BLOCK and the saved segListItem Id (if
    # any) on the P.
    set itemId $id
    if {$id == ""} {
	set itemId "SDL-RESERVED[incr nextId]"
    }
    StartBlock ITEM $firstString$spacing $itemId 0
    set firstString ""
    StartParagraph $segListItemId P ""
    set segListItemId ""

    # we've already guaranteed that we don't overflow the list
    set titleId [lindex $segTitleList $segListItemNumber]
    incr segListItemNumber

    # add the title and item to a row vector (FROWVEC)
    AddRowVec "$titleId $itemId"
}


# close a list
proc EndList {parent} {
    global listStack lastList

    # get the most recently opened list and remove it from the stack
    array set lastList [Pop listStack]

    if {($lastList(type) == "itemized") && ($lastList(mark) == "PLAIN") } {
	PopForm
    } else {
	PopFormN 2
    }

    # if we are inside a Para, we need to close the FORM we issued for
    # hanging the indent attributes
    if {$parent == "PARA"} {
	ContinueParagraph
    }
}


# start a super- or sub-scripted phrase; if there's an ID, emit the
# anchor before the SPHRASE
proc StartSPhrase {id gi} {
    Anchor $id
    switch $gi {
	SUPERSCRIPT {set type SUPER}
	SUBSCRIPT   {set type SUB}
    }

    Emit "<KEY CLASS=\"EMPH\" SSI=\"SUPER-SUB\"><SPHRASE CLASS=\"$type\">"
}

# end a super- or sub-scripted phrase
proc EndSPhrase {} {
    Emit "</SPHRASE></KEY>"
}


# start an admonition (note/caution/warning/tip/important),
# emit a FORM to hold it and the HEAD for the icon (if any);
# if the admonition has no Title, emit one using the GI of the
# admonition; map Tip to Note and Important to Caution
proc StartAdmonition {id gi haveTitle} {
    PushForm "" ADMONITION $id

    # select the icon
    switch $gi {
	NOTE      -
	TIP       {set icon "graphics/noteicon.pm"}
	CAUTION   -
	IMPORTANT {set icon "graphics/cauticon.pm"}
	WARNING   {set icon "graphics/warnicon.pm"}
    }
    set snbId [AddToSNB GRAPHIC $icon]

    # emit the icon wrapped in a head for placement
    Emit "<HEAD SSI=\"ADMONITION-ICON\"><SNREF>"
    Emit "<REFITEM RID=\"$snbId\" CLASS=\"ICON\"></REFITEM>\n"
    Emit "</SNREF></HEAD>"

    # emit a title if none provided
    if {!$haveTitle} {
	Emit "<HEAD SSI=\"ADMONITION-TITLE\">$gi</HEAD>\n"
    }
}


# start a Procedure - emit a <FORM> to hold it
proc StartProcedure {id} {
    PushForm "" PROCEDURE $id
}


# start a Step inside a Procedure, emit another FORM to hold it
proc StartStep {id} {
    PushForm "" STEP $id
}


# start a SubStep inside a Stop, emit yet another FORM to hold it
proc StartSubStep {id} {
    PushForm "" SUBSTEP $id
}


# start a Part; make the PARTGlossArray be the current glossary array
proc StartPart {id} {
    global partID glossStack

    set partID $id

    # make sure the glossary array exists but is empty
    Push glossStack PARTGlossArray
    upvar #0 [Peek glossStack] currentGlossArray
    set currentGlossArray(foo) ""
    unset currentGlossArray(foo)
}


# end a Part; check for definitions for all glossed terms
proc EndPart {} {
    global glossStack

    # get a convenient handle on the glossary array
    upvar #0 [Peek glossStack] currentGlossArray

    # check that all the glossed terms have been defined
    foreach name [array names currentGlossArray] {
        if {[info exists currentGlossArray($name)]} {
            if {[lindex $currentGlossArray($name) 1] != "defined"} {
                set glossString [lindex $currentGlossArray($name) 2]
                UserError "No glossary definition for \"$glossString\"" no
            }
        } else {
            puts stderr "EndPart: currentGlossArray: index does not exist: '$name'"
        }
    }

    # delete this glossary array
    unset currentGlossArray
}


# create and populate a dummy home page title - if no Title was
# specified we want it to be "Home Topic"
proc SynthesizeHomeTopicTitle {} {
    global partID
    global localizedAutoGeneratedStringArray

    Title $partID PARTINTRO
    set message "Home Topic"
    Emit $localizedAutoGeneratedStringArray($message)
    CloseTitle PARTINTRO
}


# create and populate a dummy home page because there was no
# PartIntro in this document
proc SynthesizeHomeTopic {} {
    global partID
    global localizedAutoGeneratedStringArray


    StartNewVirpage PARTINTRO ""
    SynthesizeHomeTopicTitle
    StartParagraph $partID P ""
    set message "No home topic (PartIntro) was specified by the author."
    Emit $localizedAutoGeneratedStringArray($message)
    EndParagraph
}


# start a virpage for, e.g., a SECTn - close the previous first;
# compute the level rather than specifying it in the transpec to allow
# one specification to do for all SECTn elements; if level=2 and we
# haven't emitted a PartIntro (aka HomeTopic), emit one
proc StartNewVirpage {ssi id} {
    global nextId virpageLevels inVirpage firstPInBlock
    global indexLocation snbLocation savedSNB currentSNB
    global lastList language charset docId havePartIntro partIntroId
    global emptyCells
    global manTitle manVolNum manDescriptor manNames manPurpose

    # get the LEVEL= value for this VIRPAGE (makes for a shorter
    # transpec to not have to specify level there)
    if {[info exists virpageLevels($ssi)]} {
	set level $virpageLevels($ssi)
    } else {
	set level 0
    }

    # if we are opening the PartIntro, use the generated ID (which
    # may be the assigned ID, if present) and flag that we've seen
    # the home topic
    if {$ssi == "PARTINTRO"} {
	set ssi CHAPTER
	set id  $partIntroId
	set havePartIntro 1
    }

    # if we haven't seen a PartIntro but we're trying to create a
    # level 2 VIRPAGE, emit a dummy PartInto
    if {($level == 2) && !$havePartIntro} {
	SynthesizeHomeTopic
    }

    if {[string match {SECT[1-5]} $ssi]} {
	# make Chapter and all Sect? have an SSI of "CHAPTER", use LEVEL
	# to distinguish between them
	set ssi CHAPTER
    } else {
	# make Reference, RefEntry and all RefSect? have an SSI of
	# "REFERENCE", use LEVEL to distinguish between them
	if {$ssi == "REFENTRY"} {
	    set $ssi REFERENCE
	} else {
	    if {[string match {REFSECT[1-3]} $ssi]} { set ssi REFERENCE }
	}
    }
    if {($ssi == "REFERENCE") || ($ssi == "REFENTRY")} {
	# assume no section, we'll get one in RefMeta.ManVolNum, if any
	set manTitle      ""
	set manVolNum     ""
	set manDescriptor ""
	set manNames      ""
	set manPurpose    ""
    }

    # close an open BLOCK, if any
    CloseBlock
    
    # close any open VIRPAGE
    Emit $inVirpage; set inVirpage "</VIRPAGE>\n"

    # if the first paragraph on the page is a compound para, we want
    # to emit a FORM with an SSI="P1" so set the first P flag
    set firstPInBlock 1

    # stash away the SNB for this VIRPAGE (or SDLDOC) - make an
    # associative array of the file location and the SNB data so
    # we can update the file location by adding the INDEX size before
    # writing the .snb file
    set names [array names currentSNB]
    if {[llength $names] != 0} {
	foreach name $names {
	    # split the name into the GI and xid of the SNB entry
	    set colonLoc [string first "::" $name]
	    set type [string range $name 0 [incr colonLoc -1]]
	    set data [string range $name [incr colonLoc 3] end]

	    # emit the entry
	    append tempSNB "<$type ID=\"$currentSNB($name)\" "
	    switch $type {
		GRAPHIC   -
		AUDIO     -
		VIDEO     -
		ANIMATE   -
		CROSSDOC  -
		MAN-PAGE  -
		TEXTFILE  { set command "XID" }
		SYS-CMD   { set command "COMMAND" }
		CALLBACK  { set command "DATA" }
	    }
	    append tempSNB "$command=\"$data\">\n"
	}
	set savedSNB($snbLocation) $tempSNB
	unset currentSNB
    }

    if {[array exists lastList]} {
	unset lastList ;# don't allow lists to continue across virpage
    }

    # delete the list of empty cells used for indefined Entries in
    # tables - we can only re-use them on the same virpage
    if {[array exists emptyCells]} {
	unset emptyCells
    }

    # we have to create new BLOCKs to hold the marks on the new page
    ReInitPerMarkInfo

    if {$id == ""} { set id "SDL-RESERVED[incr nextId]" }
    Emit "<VIRPAGE [Id $id] LEVEL=\"$level\" "
    Emit "LANGUAGE=\"$language\" "
    Emit "CHARSET=\"$charset\" "
    Emit "DOC-ID=\"$docId\" "
    Emit "SSI=\"$ssi\">\n"

    set snbLocation [tell stdout] ;# assume no HEAD for now
}


# save the virpageLevels setting for this ssi (if any) and unset it
# then call StartNewVirpage; on return, restore the virpagelevels
# setting and continue - this will force the virpage to be a level 0
# virpage and not show up in the TOC
proc StartNewLevel0Virpage {ssi id} {
    global virpageLevels

    if {[info exists virpageLevels($ssi)]} {
	set savedLevel $virpageLevels($ssi)
	unset virpageLevels($ssi)
    }

    StartNewVirpage $ssi $id

    if {[info exists savedLevel]} {
	set virpageLevels($ssi) $savedLevel
    }
}


# call StartNewVirpage, then if the user supplied ID is not same as
# the default ID for that topic, emit an empty paragragh to contain
# the user supplied ID; also, convert the ID of
# SDL-RESERVED-LEGALNOTICE to SDL-RESERVED-COPYRIGHT for backwards
# compatibility, preserve the original default ID so we're consistent
# on this release too
proc StartNewVirpageWithID {ssi id defaultID haveTitle} {
    global savedId

    # do we need to replace LEGALNOTICE with COPYRIGHT?
    set legalNotice 0
    if {[string toupper $defaultID] == "SDL-RESERVED-LEGALNOTICE"} {
	set defaultID SDL-RESERVED-COPYRIGHT
	set legalNotice 1
    }

    StartNewVirpage $ssi $defaultID

    # if no user supplied ID but we changed the default, emit the
    # original default on the empty paragraph
    if {($id == "") && $legalNotice} {
	set id SDL-RESERVED-LEGALNOTICE
	set legalNotice 0
    }

    # id is either user supplied or the original default (if changed);
    # if the VIRPAGE has a HEAD (Title), save this id (these ids) and
    # emit it (them) there, otherwise, emit an empty paragraph with
    # the id as its id
    if {$id != ""} {
	if {[string toupper $id] != [string toupper $defaultID]} {
	    if {$haveTitle} {
		set savedId $id
		if {$legalNotice} {
		    # had both a user supplied ID and we changed the default
		    lappend savedId SDL-RESERVED-LEGALNOTICE
		}
	    } else {
		StartParagraph $id "" ""
		if {$legalNotice} {
		    # had both a user supplied ID and we changed the default
		    Anchor SDL-RESERVED-LEGALNOTICE
		}
		EndParagraph
	    }
	}
    }
}


# start a VIRPAGE for an appendix; if there's no ROLE=NOTOC, use the
# virpage level from the level array, otherwise, use level 0
proc StartAppendix {ssi id role} {
    global virpageLevels
 
    set uRole [string toupper $role]

    if {$uRole == "NOTOC"} {
	set saveAppendixLevel $virpageLevels(APPENDIX)
	set virpageLevels(APPENDIX) 0
    } elseif {$role != ""} {
	UserError "Bad value (\"$role\") for Role attribute in Appendix" yes
    }

    StartNewVirpage $ssi $id

    if {$uRole == "NOTOC"} {
	set virpageLevels(APPENDIX) $saveAppendixLevel
    }
}


# start a new VIRPAGE for a topic that may contain a glossary; if
# there is a glossary, start a new one and make it the current glossary,
# otherwise, make the parent's glossary the current one.
proc StartGlossedTopic {gi id haveGlossary} {
    global glossStack

    if {$haveGlossary} {
	# save the glossary array name so we can get back here
	# when this topic is done
	Push glossStack ${gi}GlossArray

	# start a new (empty) glossary array for this glossary
	upvar #0 ${gi}GlossArray currentGlossArray
	set currentGlossArray(foo) ""
	unset currentGlossArray(foo)
    }

    StartNewVirpage $gi $id
}


# end a topic that may contain a glossary; if it did, check that all
# glossed terms have been defined and remove the array - restore the
# previous glossary array
proc EndGlossedTopic {haveGlossary} {
    global glossStack

    # get a convenient handle on the glossary array
    upvar #0 [Peek glossStack] currentGlossArray

    if {$haveGlossary} {
	# check that all the glossed terms have been defined
	foreach name [array names currentGlossArray] {
	    if {[lindex $currentGlossArray($name) 1] != "defined"} {
		set glossString [lindex $currentGlossArray($name) 2]
		UserError "No glossary definition for \"$glossString\"" no
	    }
	}

	# delete this glossary array and restore the previous one
	unset currentGlossArray
	Pop glossStack
    }
}


# alternate OutputString routine for when in a glossed term - merely
# buffer the output rather than sending to the output stream; we'll
# emit the SDL when the whole term has been seen
proc GlossOutputString {string} {
    global glossBuffer

    append glossBuffer $string
}


# prepare to link a glossed term to its definition in the glossary -
# at this point, we simply divert the output into a buffer
proc StartAGlossedTerm {} {
    global glossBuffer

    set glossBuffer ""
    rename OutputString SaveGlossOutputString
    rename GlossOutputString OutputString
}


# strip any SDL markup from the string, upper case it  and return
# the result;  takes advantage of the fact that we never split
# start or end tags across lines (operates a line at a time)
proc StripMarkup {markup} {
    set exp {(^|([^&]*))</?[A-Z]+[^>]*>}
    set stripped ""
    set mList [split $markup "\n"];      # split into a list of lines
    set listLen [llength $mList]
    while {[incr listLen -1] >= 0} {
	set mString [lindex $mList 0];   # get the first line from the
	set mList [lreplace $mList 0 0]; # list and delete it
	if {[string length $mString] == 0} {
	    # empty line of pcdata (no markup)
	    append stripped "\n"
	    continue
	}
	# force to upper case and delete all start and end tags
	set mString [string toupper $mString]
	while {[regsub -all $exp $mString {\1} mString]} {#}
	if {[string length $mString] == 0} {
	    # empty line after removing markup; skip it
	    continue
	}
	append stripped $mString "\n";   # concat this line to result
    }
    return $stripped
}


# done collecting a glossed term - if we're not NOGLOSS, emit the SDL
# wrapped in a LINK; save the term, baseform (if any) and the ID
# used in the link - we'll define the ID in the glossary itself
proc EndAGlossedTerm {id role} {
    global glossBuffer nextId glossStack

    # get a convenient handle on the glossary array
    upvar #0 [Peek glossStack] currentGlossArray

    # get the original output routine back
    rename OutputString GlossOutputString
    rename SaveGlossOutputString OutputString

    set qualifier [string toupper [string range $role 0 8]]
    if {$qualifier == "NOGLOSS"} {
	Emit "<KEY CLASS=\"TERM\" SSI=\"GLOSSARY\">"
	Emit $glossBuffer
	Emit "</KEY>"
    } else {
	if {$qualifier == "BASEFORM="} {
	    set glossString [string range $role 9 end]
	} else {
	    set glossString $glossBuffer
	}

	# trim whitespace from the front and back of the string to be
	# glossed, also turn line feeds into spaces and compress out
	# duplicate whitespace
	set glossString [string trim $glossString]
	set glossString [split $glossString '\n']
	set tmpGlossString $glossString
	set glossString [lindex $tmpGlossString 0]
	foreach str [lrange $tmpGlossString 1 end] {
	    if {$str != ""} {
		append glossString " " [string trim $str]
	    }
	}

	# upper case the glossary entry and strip it of markup to
	# use as an index so we get a case insensitive match - we'll
	# save the original string too for error messages; if there's
	# no glossary entry yet, issue an ID - the second entry in
	# the list will be set to "defined" when we see the definition
	set glossIndex [StripMarkup $glossString]
	if {[info exists currentGlossArray($glossIndex)]} {
	    set refId [lindex $currentGlossArray($glossIndex) 0]
	} else {
	    set refId SDL-RESERVED[incr nextId]
	    set currentGlossArray($glossIndex) [list $refId "" $glossString]
	}

	# now we can emit the glossed term wrapped in a popup link
	Emit "<LINK WINDOW=\"POPUP\" RID=\"$refId\">"
	Emit "<KEY CLASS=\"TERM\" SSI=\"GLOSSARY\">"
	Emit $glossBuffer
	Emit "</KEY></LINK>"
    }
}


# done collecting a term in a glossary - emit the anchor, if not
# already done; if we are to be followed by alternate names (i.e.,
# Abbrev and/or Acronym), emit the opening paren, otherwise, close
# the open KEY
proc EndATermInAGlossary {id} {
    global glossBuffer nextId nGlossAlts glossStack
    global strippedGlossIndex

    # get a convenient handle on the glossary array
    upvar #0 [Peek glossStack] currentGlossArray

    # get the original output routine back
    rename OutputString GlossOutputString
    rename SaveGlossOutputString OutputString

    # emit the user supplied ID
    Anchor $id

    # trim whitespace from the front and back of the string to be
    # placed in the glossary, also turn line feeds into spaces and
    # compress out duplicate whitespace
    set glossString [split $glossBuffer '\n']
    set tmpGlossString $glossString
    set glossString [lindex $tmpGlossString 0]
    foreach str [lrange $tmpGlossString 1 end] {
	if {$str != ""} {
	    append glossString " " [string trim $str]
	}
    }

    # create an upper cased version of the glossed string with markup
    # removed to use as a case insensitive index to the array
    set strippedGlossIndex [StripMarkup $glossString]

    # get or create the generated ID; update the glossary array to
    # reflect that we now have a definition
    if {[info exists currentGlossArray($strippedGlossIndex)]} {
	set id [lindex $currentGlossArray($strippedGlossIndex) 0]
	set defined [lindex $currentGlossArray($strippedGlossIndex) 1]
	if {$defined == "defined"} {
	    UserError \
		"multiple definitions for glossary term \"$glossBuffer\"" yes
	    set id SDL-RESERVED[incr nextId]
	}
    } else {
	set id SDL-RESERVED[incr nextId]
    }
    set currentGlossArray($strippedGlossIndex) \
	[list $id defined $glossString "" ""]

    # emit the generated ID
    Anchor $id
    Emit "<KEY CLASS=\"TERM\" SSI=\"GLOSSARY\">"
    Emit "$glossBuffer"
    if {$nGlossAlts != 0} {
	Emit " ("
    } else {
	Emit "</KEY>"
	unset nGlossAlts
    }
}


proc EndAcronymInGlossary {id} {
    global nGlossAlts

    if {[incr nGlossAlts -1] != 0} {
	Emit ", "
    } else {
	Emit ")</KEY>"
	unset nGlossAlts
    }
}


proc EndAbbrevInGlossary {id} {
    global nGlossAlts

    Emit ")"</KEY"
    unset nGlossAlts
}


# start an entry in a glossary or glosslist; divert the output - we
# need to sort the terms before emitting them
proc StartGlossEntry {id nAlternates nDefs} {
    global nGlossAlts nGlossDefs currentGlossDef
    global glossEntryBuffer

    # this helps when determining if a comma is needed after an alt
    # (either an Abbrev or an Acronym)
    set nGlossAlts $nAlternates

    # this lets us know when to close the FORM holding the GlossDef+
    set nGlossDefs $nDefs
    set currentGlossDef 0

    set glossEntryBuffer ""
    rename OutputString SaveGlossEntryOutputString
    rename GlossEntryOutputString OutputString

    PushForm "" GLOSSENTRY $id
    StartParagraph "" "" ""
}


# alternate OutputString routine for when in a GlossEntry - merely
# buffer the output rather than sending to the output stream; we'll
# save this text for emission when the entire GlossDiv, Glossary or
# GlossList has been processed and we've sorted the entries.
proc GlossEntryOutputString {string} {
    global glossEntryBuffer

    append glossEntryBuffer $string
}


# end an entry in a glossary or glosslist; save the entry in the
# glossarray so we can later sort it for output
proc EndGlossEntry {sortAs} {
    global glossEntryBuffer strippedGlossIndex glossStack

    PopForm

    # get the original output routine back
    rename OutputString GlossEntryOutputString
    rename SaveGlossEntryOutputString OutputString

    # get a convenient handle on the glossary array and element
    upvar #0 [Peek glossStack] currentGlossArray
    upvar  0 currentGlossArray($strippedGlossIndex) currentEntryList

    # save any user supplied sort key and the content of this glossary
    # entry for use when all entries are defined to sort them and emit
    # them in the sorted order
    set currentEntryList \
	[lreplace $currentEntryList 3 4 $sortAs $glossEntryBuffer]

}


# the current batch of glossary entries (to a Glossary, GlossList or
# GlossDiv has been saved in the glossArray - we need to sort them
# based on the sortAs value if given (list index 3) or the index into
# the glossArray of no sortAs was provided; when sorted, we can emit
# entries (list index 4) in the new order and delete the emitted text
# so that we don't try to emit it again (we want to save the
# glossArray until, e.g., all GlossDiv elements are processed so we
# can tell if all glossed terms have been defined); do a PopForm
# when we're done if requested (for, e.g., GlossList)
proc SortAndEmitGlossary {popForm} {
    global glossStack

    # get a convenient handle on the glossary array
    upvar #0 [Peek glossStack] currentGlossArray

    # start with an empty sortArray
    set sortArray(foo) ""
    unset sortArray(foo)

    set names [array names currentGlossArray]
    foreach name $names {
        # puts stderr "JET0: name: $name"
	upvar 0 currentGlossArray($name) glossEntryList

	# skip this array entry if we've already emitted it; mark as
	# emitted after we've extracted the content for emission
	if {[set content [lindex $glossEntryList 4]] == ""} {
	    continue; # already been processed
	}
	set glossEntryList [lreplace $glossEntryList 4 4 ""]

	# sort by the GlossTerm content or sortAs, if provided
	if {[set sortAs [lindex $glossEntryList 3]] == ""} {
            set sortAs $name
	}

	# append the content in case we have equal sort values
	append sortArray($sortAs) $content
    }

    set idxnames [lsort -dictionary [array names sortArray]]

    foreach name $idxnames {
        # puts stderr "JET1: name: $name"
        if {[info exists sortArray($name)]} {
            Emit $sortArray($name)
        } else {
            puts stderr "SortAndEmitGlossary: sortArray index does not exist: '$name'"
        }
    }

    if {[string toupper $popForm] == "POPFORM"} {
	PopForm
    }
}


# start a "See ..." in a glossary; if there was an otherterm, duplicate
# its content and wrap it in a link to the GlossTerm holding the content
proc StartGlossSee {id otherterm} {
    global localizedAutoGeneratedStringArray

    StartBlock "" GLOSSSEE $id 1
    StartParagraph "" "" ""
    set message "See"
    Emit $localizedAutoGeneratedStringArray($message)
    Emit " "
    if {$otherterm != ""} {
	Emit "<LINK RID=\"$otherterm\">"
    }
}


# check the target of an OtherTerm attribute in a GlossSee to verify
# that it is indeed the ID of a GlossTerm inside a GlossEntry
proc CheckOtherTerm {id gi parent} {
    global glossType

    set errorMess "Other term (\"$id\") referenced from a"

    if {$gi != "GLOSSTERM"} {
	UserError "$errorMess $glossType must be a GlossTerm" yes
    } elseif {$parent != "GLOSSENTRY"} {
	UserError "$errorMess GlossSee must be in a GlossEntry" yes
    }
}


# start a definition in a glossary; we wrap a FORM around the whole
# group of GlossDef elements in the GlossEntry
proc StartGlossDef {id} {
    global nGlossDefs currentGlossDef

    if {$currentGlossDef == 0} {
	PushForm "" GLOSSDEF $id
    }
    StartBlock "" "" $id 1
}


# end a definition in a glossary; if this is the last definition,
# close the FORM that holds the group
proc EndGlossDef {} {
    global nGlossDefs currentGlossDef

    if {[incr currentGlossDef] == $nGlossDefs} {
	PopForm
	unset nGlossDefs currentGlossDef
    }
}


# start a "See Also ..." in a glossary definition; if there was an
# otherterm, duplicate its content and wrap it in a link to the
# GlossTerm holding the content
proc StartGlossSeeAlso {id otherterm} {
    global localizedAutoGeneratedStringArray

    StartBlock "" GLOSSSEE $id 1
    StartParagraph "" "" ""
    set message "See Also"
    Emit $localizedAutoGeneratedStringArray($message)
    Emit " "
    if {$otherterm != ""} {
	Emit "<LINK RID=\"$otherterm\">"
    }
}


# end a "See ..." or a "See Also ..." in a glossary definition; if there
# was an otherterm, end the link to it
proc EndGlossSeeOrSeeAlso {otherterm} {
    if {$otherterm != ""} {
	Emit "</LINK>"
    }
}


# alternate OutputString routine for when in IndexTerm - merely
# buffer the output rather than sending to the output stream (index
# entries get emitted into the index, not where they are defined)
proc IndexOutputString {string} {
    global indexBuffer

    append indexBuffer $string
}


# alternate Id routine for when in IndexTerm - merely
# return the string rather than also setting the "most recently used"
# variable.  The markup inside the IndexTerm goes into the index
# not the current virpage so we don't want to use the ids here
proc IndexId {name} {
    return "ID=\"$name\""
}


# start an index entry 
proc StartIndexTerm {id} {
    global indexBuffer inP inBlock

    if {$id != ""} {
	if {$inP} {
	    Anchor $id
	} elseif {$inBlock != ""} {
	    StartParagraph "" "P" ""
	    Anchor $id
	    EndParagraph
	}

    }

    # prepare to buffer the output while in IndexTerm
    set indexBuffer ""
    rename OutputString DefaultOutputString
    rename IndexOutputString OutputString
    rename Id DefaultId
    rename IndexId Id
}


# add an index sub-entry 
proc AddIndexEntry {loc} {
    global indexBuffer indexVals indexArray

    # trim superfluous whitespace at the beginning and end of the
    # indexed term
    set indexBuffer [string trim $indexBuffer]

    # get an array index and determine whether 1st, 2nd or 3rd level
    set index [join $indexVals ", "]
    set level [llength $indexVals]
    set value [lindex $indexVals [expr "$level - 1"]]

    # look for the string we want to put into the index; if the string
    # isn't there, add it - if it's there, verify that the content
    # being indexed is marked up the same as the last time we saw it
    # and that the primary/secondary/tertiary fields are split the
    # same way (bad check for now, we really need to save the
    # individual values) and add the location ID to the list of locs.
    set names [array names indexArray]
    if {$names == ""} {
	set indexArray($index) [list $level $value $loc $indexBuffer]
    } else {
	foreach i $names {
	    set found 0
	    if {$i == $index} {
		set thisIndex $indexArray($index)
		if {$indexBuffer != [lindex $thisIndex 3]} {
		    UserError "Indexing same terms with different markup" yes
		}
		if {$level != [lindex $thisIndex 0]} {
		    UserError "Index botch: levels don't match" yes
		}
		if {$loc != ""} {
		    set locs [lindex $thisIndex 2]
		    if {$locs != ""} { append locs " " }
		    append locs "$loc" 
		    set thisIndex [lreplace $thisIndex 2 2 $locs]
		    set indexArray($index) $thisIndex
		}
		set found 1
		break
	    }
	}
	if {!$found} {
	    set indexArray($index) [list $level $value $loc $indexBuffer]
	}
    }
    set indexBuffer ""
}


# end an index entry 
proc EndIndexTerm {} {
    global mostRecentId

    AddIndexEntry $mostRecentId

    # start emitting to output stream again
    rename OutputString        IndexOutputString
    rename DefaultOutputString OutputString
    rename Id        IndexId
    rename DefaultId Id
}


# start a primary index term
proc StartPrimaryIndexEntry {id cdata} {
    global indexVals

    set indexVals [list [string trim $cdata]]
}


# end a primary index term
proc EndPrimaryIndexEntry {} {
}


# start a secondary index term
proc StartSecondaryIndexEntry {id cdata} {
    global indexVals

    AddIndexEntry "" ;# make sure our primary is defined
    lappend indexVals [string trim $cdata]
}


# end a secondary index term
proc EndSecondaryIndexEntry {} {
}


# start a tertiary index term
proc StartTertiaryIndexEntry {id cdata} {
    global indexVals

    AddIndexEntry "" ;# make sure our secondary is defined
    lappend indexVals [string trim $cdata]
}


# end a tertiary index term
proc EndTertiaryIndexEntry {} {
}


# compute the proper string for LOCS= in an index entry - primarily,
# we want to avoid emitting the LOCS= if there are no locations
# defined for this entry
proc Locs {entry} {
    set locs [lindex $entry 2]
    if {$locs != ""} {
	return " LOCS=\"$locs\""
    }
    return ""
}


# open a .idx file and write the index into it
proc WriteIndex {} {
    global baseName indexArray

    set file [open "${baseName}.idx" w]

    # sort the index

    set idxnames [lsort -dictionary [array names indexArray]]

    if {[set length [llength $idxnames]]} {
	set oldLevel 0
	puts $file "<INDEX COUNT=\"$length\">"
	foreach name $idxnames {
            if {[info exists indexArray($name)]} {
                set thisEntry $indexArray($name)
                switch [lindex $thisEntry 0] {
                    1 { switch $oldLevel {
                        1 { puts $file "</ENTRY>" }
                        2 { puts $file "</ENTRY>\n</ENTRY>" }
                        3 { puts $file "</ENTRY>\n</ENTRY>\n</ENTRY>" }
		    }
                    }
                    2 { switch $oldLevel {
                        2 { puts $file "</ENTRY>" }
                        3 { puts $file "</ENTRY>\n</ENTRY>" }
		    }
                    }
                    3 { if {$oldLevel == 3} { puts $file "</ENTRY>" } }
                }
                puts -nonewline $file "<ENTRY[Locs $thisEntry]>"
                puts -nonewline $file [lindex $thisEntry 3]
                set oldLevel [lindex $thisEntry 0]
            } else {
                puts stderr "WriteIndex: index does not exist: '$name'"
            }
	}

	switch $oldLevel {
	    1 { puts $file "</ENTRY>" }
	    2 { puts $file "</ENTRY>\n</ENTRY>" }
	    3 { puts $file "</ENTRY>\n</ENTRY>\n</ENTRY>" }
	}
	puts $file "</INDEX>"
    }

    close $file
}


# called at the beginning of CHAPTER on each FOOTNOTE element - save
# their numbering for use by FOOTNOTEREF and emit a VIRPAGE for each
# note
proc GatherFootnote {id} {
    global footnoteArray footnoteCounter nextId

    incr footnoteCounter
    if {$id != ""} {
	set footnoteArray($id) $footnoteCounter
    } else {
	set id SDL-RESERVED[incr nextId]
    }

    StartNewVirpage FOOTNOTE $id
}


# emit the footnote number of the id surrounded by a <LINK> so we can
# get to it; skip out if there's no id to reference
proc FootnoteRef {idref} {
    global footnoteArray

    if {$idref != ""} {
	if {[info exists footnoteArray($idref)]} {
	    Emit "<LINK RID=\"$idref\" WINDOW=\"popup\">"
	    Emit "<KEY CLASS=\"EMPH\" SSI=\"FOOTNOTE\">"
	    Emit "$footnoteArray($idref)</KEY></LINK>"
	}
    }
}


# add an element to the current SNB - try to reuse an entry if
# possible
proc AddToSNB {stype data} {
    global currentSNB nextId

    set index "${stype}::${data}"

    if {[info exists currentSNB($index)]} {
	set snbId $currentSNB($index)
    } else {
	set snbId "SDL-RESERVED[incr nextId]"
	set currentSNB($index) $snbId
    }
return $snbId
}


# emit a DocBook Graphic element - create an SNB entry and point to
# it here
proc Graphic {id entityref fileref gi} {
    global inP

    if {$gi == "GRAPHIC"} {
	set class FIGURE
    } else {
	set class IN-LINE
    }

    # if "entityref" is present, it overrides "fileref"
    if {$entityref != ""} {
	# need to remove "<OSFILE ASIS>" (or equivalent for different
	# system identifiers) from the beginning of the entity name
	# if nsgmls was used for the original parse; the regular
	# expression below should work by simply ignoring any leading
	# angle bracket delimited string
	regsub {^(<.*>)(.*)$} $entityref {\2} entityref
	set file $entityref
    } else {
	set file $fileref
    }

    if {$file == ""} {
	UserError "No file name or entity specified for $gi" yes
    }

    # if not in a paragraph, start one
    if {($gi == "GRAPHIC") && (!$inP)} { StartParagraph "" "P" "" }
 
    set snbId [AddToSNB GRAPHIC $file]

    Emit "<SNREF>"
    Emit "<REFITEM RID=\"$snbId\" CLASS=\"$class\"></REFITEM>\n"
    Emit "</SNREF>"
}


# emit a deferred link; we deferred it when we saw that it was first
# in a Para and that it contained only an InlineGraphic - we had
# to wait for the InlineGraphic to come along to see if it not only
# met the contextual constraints but also had a Remap=Graphic
# attribute
proc EmitDeferredLink {} {
    global deferredLink

    if {![array exists deferredLink]} return

    switch $deferredLink(gi) {
	LINK  {StartLink  "" $deferredLink(linkend)   $deferredLink(type)}
	OLINK {StartOLink "" $deferredLink(localinfo) $deferredLink(type)}
    }

    unset deferredLink
}


# emit an InlineGraphic that might be remapped to a Graphic (via
# Remap=) and might have text wrapped around it (if it's first in
# a Para or first in a [OU]Link that is itself first in a Para)
proc InFlowGraphic {id entityref fileref parent remap role} {
    global deferredLink

    # we only map InlineGraphic to Graphic if we're either the first
    # thing in a Para or the only thing in a link which is itself
    # the first thing in a Para
    set ok 0
    set haveDeferredLink [array exists deferredLink]
    switch $parent {
	PARA      {set ok 1}
	LINK      -
	OLINK     -
	ULINK     {set ok $haveDeferredLink}
    }
    if {!$ok} {
	Graphic $id $entityref $fileref INLINEGRAPHIC
	return
    }

    set uRemap [string toupper $remap]
    if {$uRemap == "GRAPHIC"} {
	set uRole [string toupper $role]
	switch $uRole {
	    LEFT  -
	    ""    {set role "LEFT"}
	    RIGHT {set role "RIGHT"}
	    default {
		set badValMess "Bad value (\"$role\") for Role attribute"
		UserError "$badValMess in InlineGraphic" yes
		set role "LEFT"
		}
	}
	if {$haveDeferredLink} {
	    set linkID " ID=\"$deferredLink(id)\""
	    if {$deferredLink(gi) == "ULINK"} {
		unset deferredLink
		set haveDeferredLink 0
	    }
	} else {
	    set linkID ""
	}
	Emit "<HEAD$linkID SSI=\"GRAPHIC-$role\">"
	if {$haveDeferredLink} {
	    EmitDeferredLink
	}
        Graphic $id $entityref $fileref GRAPHIC
	if {$haveDeferredLink} {
	    EndLink
	}
	Emit "</HEAD>"
	return
    } elseif {$remap != ""} {
	set badValMess "Bad value (\"$remap\") for Remap attribute"
	UserError "$badValMess in InlineGraphic" yes
    }

    Graphic $id $entityref $fileref INLINEGRAPHIC
}


# start a figure; for now, ignore Role (as it was ignored in HelpTag)
# but make sure Role contains only legal values
proc StartFigure {id role} {
    if {$role != ""} {
	set uRole [string toupper $role]
	switch $uRole {
	    LEFT     -
	    CENTER   -
	    RIGHT   {set i 0}
	    default {
		set badValMess "Bad value for Role (\"$role\") attribute"
	        UserError "$badValMess in Figure" yes
	    }
	}
    }

    PushForm "" "FIGURE" $id
}


# emit a CiteTitle in a KEY with the SSI set to the PubWork attr.
proc CiteTitle {id type} {
    Emit "<KEY CLASS=\"PUB-LIT\""
    if {$id != ""} {
	Emit " ID=\"$id\""
    }
    Emit " SSI=\"$type\">"
}


# start a KEY element - each parameter is optional (i.e, may be "")
proc StartKey {id class ssi} {
    Emit "<KEY"
    if {$id != ""} {
	Emit " ID=\"$id\""
    }
    if {$class != ""} {
	Emit " CLASS=\"$class\""
    }
    if {$ssi != ""} {
	Emit " SSI=\"$ssi\""
    }
    Emit ">"
}

# start an emphasis with role=heading; want want a different ssi
# so we can make it bold rather than italic for use as a list
# heading
proc StartHeading {id role} {
    set role [string toupper $role]
    if {$role != "HEADING"} {
	if {$role != ""} {
	    UserWarning "Bad value for Role (!= \"Heading\") in EMPHASIS" yes
	}
	set ssi EMPHASIS
    } else {
	set ssi LIST-HEADING
    }
    StartKey $id EMPH $ssi
}


# start an Example or InformalExample - we need to put ourselves
# in a mode where lines and spacing are significant
proc Example {id} {
    global defaultParaType

    set defaultParaType " TYPE=\"LITERAL\""
    PushForm "" "EXAMPLE" $id
}


# close an Example or InformalExample - put ourselves back in
# the normal (non-literal) mode
proc CloseExample {} {
    global defaultParaType

    set defaultParaType ""
    PopForm
}


# start a Table or InformalTable - save the global attributes and
# open a FORM to hold the table
proc StartTable {id colSep frame label rowSep} {
    global tableAttributes

    set tableAttributes(colSep) $colSep
    set tableAttributes(label)  $label
    set tableAttributes(rowSep) $rowSep

    PushForm TABLE "TABLE-$frame" $id

    # create a list of ids of empty blocks to be used to fill in
    # undefined table cells
}


# check the "char" attribute - we only support "." at this time;
# return "." if char="." and "" otherwise; issue warning if char
# is some character other than "."
proc CheckChar {char} {
    if {($char != "") && ($char != ".")} {
	UserError "Only \".\" supported for character alignment" yes
	return ""
    }
    return $char
}


# start a TGROUP - prepare to build a list of column specifications
# and an array of span specifications to be accessed by name; a column
# specification may be numbered, in which case default (all #IMPLIED)
# column specifications will be inserted to come up to the specified
# number - if there are already more column specifications than the
# given number, it's an error; open a FORM to hold the TGroup
proc StartTGroup {id align char cols colSep rowSep nColSpecs} {
    global tableGroupAttributes tableAttributes
    global tableGroupColSpecs tableGroupSpanSpecs
    global numberOfColSpecs colNames haveTFoot
    global needTGroupTHeadForm needTFootForm
    global tableGroupSavedFRowVec

    set numberOfColSpecs $nColSpecs

    # do a sanity check on the number of columns, there must be
    # at least 1
    if {$cols <= 0} {
	UserError "Unreasonable number of columns ($cols) in TGroup" yes
	set cols 1
    }

    # check for more COLSPECs than COLS - error if so
    if {$nColSpecs > $cols} {
	UserError "More ColSpecs defined than columns in the TGroup" yes
    }

    set tableGroupAttributes(align) $align
    set tableGroupAttributes(char)  [CheckChar $char]
    set tableGroupAttributes(cols)  $cols
    if {$colSep == ""} {
	set tableGroupAttributes(colSep) $tableAttributes(colSep)
    } else {
	set tableGroupAttributes(colSep) $colSep
    }
    if {$rowSep == ""} {
	set tableGroupAttributes(rowSep) $tableAttributes(rowSep)
    } else {
	set tableGroupAttributes(rowSep) $rowSep
    }

    # make sure we have a blank colName array so we don't get errors
    # if we try to read or delete it when there have been no named
    # ColSpecs in this tableGroup - use a numeric key since that is
    # not a NMTOKEN and so can never be a colName - note that all
    # colNames share a common name space within each tGroup.
    set colNames(0) ""

    # create an empty column specification list for this TGroup;
    # if no ColSpec definitions at this level, set them all to the
    # defaults - take advantage of the fact that the function ColSpec
    # will create default column specifications to fill out up to an
    # explicitly set ColNum
    set tableGroupColSpecs ""
    if {$nColSpecs == 0} {
	ColSpec "" TGROUP "" "" "" $cols "" "" ""
    }

    PushForm TABLE TGROUP $id

    # set a flag to indicate that we haven't seen a TFoot yet; this
    # flag is used in EndRow and StartCell to determine if a Row is
    # the last row in this TGroup (the last row will be in the TFoot,
    # if present, otherwise it will be in the TBody)
    set haveTFoot 0

    # initialize variables used to determine if we need separate FORM
    # elements for THead or TFoot - if ColSpec elements are not given
    # at those levels, they can go in the same FORM as the TBody and
    # we can guarantee that the columns will line up
    set needTGroupTHeadForm 0
    set needTFootForm       0

    # and initialize a variable to hold saved FROWVEC elements across
    # THead, TBody and TFoot in case we are merging them into one or
    # two FORM elements rather than putting each in its own
    set tableGroupSavedFRowVec ""
}


# close a table group; delete the info arrays and lists and close the
# FORM
proc EndTGroup {} {
    global tableGroupAttributes tableGroupColSpecs tableGroupSpanSpecs
    global haveTFoot

    unset tableGroupAttributes
    unset tableGroupColSpecs
    if {[info exists tableGroupSpanSpecs]} {
	unset tableGroupSpanSpecs
    }
    PopForm

    # see the explanation for this variable under StartTGroup
    unset haveTFoot
}


# process one of a series of column specifications - use the parent GI
# to determine which column specifications we're dealing with; fill up
# to the specified column number with default COLSPECs, using the
# TGROUP, THEAD or TFOOT values as defaults; any COLSPEC values not
# specified in the parameter list should also be defaulted
proc ColSpec {grandparent parent align char colName colNum
					colSep colWidth rowSep} {
    # the number of currently defined colSpecs in this context
    global numberOfColSpecs
    global colNames

    # get the proper list of ColSpecs for the current context
    if {$grandparent == "ENTRYTBL"} {
	set gpName entryTable
    } else {
	set gpName tableGroup
    }
    switch  $parent {
	THEAD    { upvar #0 ${gpName}HeadColSpecs colSpecs }
	TGROUP   { upvar #0 tableGroupColSpecs colSpecs    }
	TFOOT    { upvar #0 tableFootColSpecs  colSpecs    }
	ENTRYTBL { upvar #0 entryTableColSpecs colSpecs    }
    }

    # get the proper number of columns (either from TGroup or EntryTbl);
    # a THead could be in either a TGroup or EntryTbl so we need
    # to check the grandparent if we aren't at the top level
    if {$parent == "TGROUP"} {
	upvar #0 tableGroupAttributes attributes
    } elseif {$parent == "ENTRYTBL"} {
	upvar #0 entryTableAttributes attributes
    } elseif {$grandparent == "ENTRYTBL"} {
	upvar #0 entryTableAttributes attributes
    } else {
	upvar #0 tableGroupAttributes attributes
    }
    set nCols $attributes(cols)

    # check for more COLSPECs than COLS - we've already issued an error if so
    append colSpecs ""
    set currentLength [llength $colSpecs]
    if {$currentLength >= $nCols} {
	return
    }

    # create a default ColSpec
    set thisColSpec(align)    $attributes(align)
    set thisColSpec(char)     $attributes(char)
    set thisColSpec(colName)  ""
    set thisColSpec(colSep)   $attributes(colSep)
    set thisColSpec(colWidth) "1*"
    set thisColSpec(rowSep)   $attributes(rowSep)

    # back fill with default COLSPECs if given an explicit COLNUM and
    # it's greater than our current position
    incr currentLength
    if {($colNum != "")} {
	if {($colNum < $currentLength)} {
	    set badValMess1 "Explicit colNum ($colNum) less than current"
	    set badValMess2 "number of ColSpecs ($currentLength)"
	    UserError "$badValMess1 $badValMess2" yes
	    return
	} else {
	    while {$currentLength < $colNum} {
		set thisColSpec(colNum) $currentLength
		lappend colSpecs [array get thisColSpec]
		incr currentLength
	    }
	}
    }
    set colNum $currentLength

    # set this COLSPEC, we've already set the defaults
    if {$align != ""} {
	set thisColSpec(align)    $align
    }
    if {$char != ""} {
	set thisColSpec(char)     [CheckChar $char]
    }
    set thisColSpec(colName)      $colName
    if {$colName != ""} {
	# save name to num mapping for later lookup by Entry
	set colNames($colName) $colNum
    }
    set thisColSpec(colNum)       $colNum
    if {$colSep != ""} {
	set thisColSpec(colSep)   $colSep
    }
    if {$colWidth != ""} {
	set thisColSpec(colWidth) $colWidth
    }
    if {$rowSep != ""} {
	set thisColSpec(rowSep)   $rowSep
    }
    if {$colNum == $nCols} {
	set thisColSpec(colSep) 0; # ignore COLSEP on last column
    }
    lappend colSpecs [array get thisColSpec]

    # fill out to the number of columns if we've run out of COLSPECs
    if {[incr numberOfColSpecs -1] <= 0} {
	# restore the default COLSPEC
	set thisColSpec(align)    $attributes(align)
	set thisColSpec(char)     $attributes(char)
	set thisColSpec(colName)  ""
	set thisColSpec(colSep)   $attributes(colSep)
	set thisColSpec(colWidth) "1*"
	set thisColSpec(rowSep)   $attributes(rowSep)

	while {$colNum < $nCols} {
	    incr colNum
	    set thisColSpec(colNum) $colNum
	    if {$colNum == $nCols} {
		set thisColSpec(colSep) 0; # ignore on last column
	    }
	    lappend colSpecs [array get thisColSpec]
	}
    }
}


# process a SpanSpec - we can't take defaults yet because the Namest
# and Nameend attributes may refer to ColSpecs that don't get defined
# until a TFoot or THead
proc SpanSpec {parent align char colSep nameEnd nameSt rowSep spanName} {
    if {$parent == "TGROUP"} {
	upvar #0 tableGroupSpanSpecs spanSpecs
    } else {
	upvar #0 entryTableSpanSpecs spanSpecs
    }

    set thisSpanSpec(align)   $align
    set thisSpanSpec(char)    [CheckChar $char]
    set thisSpanSpec(colSep)  $colSep
    set thisSpanSpec(nameEnd) $nameEnd
    set thisSpanSpec(nameSt)  $nameSt
    set thisSpanSpec(rowSep)  $rowSep

    if {[info exists spanSpecs($spanName)]} {
	UserError "duplicate span name \"$spanName\"" yes
	return
    }

    set spanSpecs($spanName) [array get thisSpanSpec]
}


# make a list of empty strings for use as an empty Row
proc MakeEmptyRow {nCols} {
    set thisList ""
    while {$nCols > 0} {
	lappend thisList ""
	incr nCols -1
    }
    return $thisList
}


# given a ColSpec list, compute a COLW= vector for SDL;
# the idea is to assume the page is 9360 units wide - that's
# 6.5 inches in points at approximately 1/72 in. per point,
# subtract all the absolute widths and divide the remnant by
# the number of proportional width values then re-add the absolute
# widths back in to the proper columns;  this technique should
# make pages that are exactly 6.5 in. in printing surface look just
# right and then go proportional from there
proc ComputeCOLW {colSpecList} {

    set nCols [llength $colSpecList]

    # build lists of just the ColWidth specs - one for the proporional
    # values and one for the absolutes
    set index 0
    set totalProps 0
    set totalAbs   0
    while {$index < $nCols} {
	array set thisColSpec [lindex $colSpecList $index]
	set colWidth $thisColSpec(colWidth)
	set colWidth [string trimleft $colWidth]
	set colWidth [string trimright $colWidth]
	set colWidth [string tolower $colWidth]
	set widths [split $colWidth '+']
	set nWidths [llength $widths]
	set propWidth 0
	set absWidth  0
	set wIndex    0
	while {$wIndex < $nWidths} {
	    set thisWidth [lindex $widths $wIndex]
	    if {[scan $thisWidth "%f%s" val qual] != 2} {
		UserError "Malformed ColWidth \"$thisWidth\"" yes
		incr wIndex
		continue
	    }
	    set thisProp 0
	    set thisAbs  0
	    switch -exact $qual {
		*  {set thisProp $val}
		pt {set thisAbs [expr "$val *  1 * 20"]}
		pi {set thisAbs [expr "$val * 12 * 20"]}
		cm {set thisAbs [expr "$val * 28 * 20"]}
		mm {set thisAbs [expr "$val *  3 * 20"]}
		in {set thisAbs [expr "$val * 72 * 20"]}
	    }
	    set propWidth [expr "$propWidth + $thisProp"]
	    set absWidth  [expr "$absWidth  + $thisAbs"]
	    incr wIndex
	}
	lappend propWidths $propWidth
	lappend absWidths  $absWidth
	set totalProps [expr "$totalProps + $propWidth"]
	set totalAbs   [expr "$totalAbs   + $absWidth"]
	incr index
    }
    if {$totalProps == 0} {
	# we need at least some proportionality; assume each cell
	# had been set to 1* to distribute evenly
	set totalProps $nCols
	set index 0
	if {[info exists propWidths]} {
	    unset propWidths
	}
	while {$index < $nCols} {
	    lappend propWidths 1
	    incr index
	}
    }
    set tableWidth 9360
    if {$totalAbs > $tableWidth} {
	set tableWidth $totalAbs
    }
    set propAvail [expr "$tableWidth - $totalAbs"]
    set oneProp   [expr "$propAvail / $totalProps"]

    # now we know what a 1* is worth and we know the absolute size
    # requests, create a ColWidth by adding the product of the
    # proportional times a 1* plus any absolute request; we'll allow
    # 20% growth and shrinkage
    set index 0
    set space ""
    while {$index < $nCols} {
	set thisAbs  [lindex $absWidths  $index]
	set thisProp [lindex $propWidths $index]
	set thisWidth [expr "$thisAbs + ($thisProp * $oneProp)"]
	set thisSlop [expr "$thisWidth * 0.2"]
	# make thisWidth an integer
	set dotIndex [string last "." $thisWidth]
	if {$dotIndex == 0} {
	    set thisWidth 0
	} elseif {$dotIndex > 0} {
	    incr dotIndex -1
	    set thisWidth [string range $thisWidth 0 $dotIndex]
	}
	# make thisSlop an integer
	set dotIndex [string last "." $thisSlop]
	if {$dotIndex == 0} {
	    set thisSlop 0
	} elseif {$dotIndex > 0} {
	    incr dotIndex -1
	    set thisSlop [string range $thisSlop 0 $dotIndex]
	}
	append returnValue "$space$thisWidth,$thisSlop"
	set space " "
	incr index
    }

    return $returnValue
}



# given a ColSpec list, compute a COLJ= vector for SDL;
proc ComputeCOLJ {colSpecList} {

    set nCols [llength $colSpecList]

    set space ""
    set index 0
    while {$index < $nCols} {
	array set thisColSpec [lindex $colSpecList $index]
	switch -exact $thisColSpec(align) {
	    LEFT    -
	    JUSTIFY -
	    ""      { set thisColJ l}
	    CENTER  { set thisColJ c}
	    RIGHT   { set thisColJ r}
	    CHAR    { set thisColJ d}
	}
	append returnValue "$space$thisColJ"

	set space " "
	incr index
    }

    return $returnValue
}


# given a ColSpec, create the COLW= and COLJ= attributes; check the
# list of current TOSS entries to see if one matches - if so, return
# its SSI= else add it and create an SSI= to return
proc CreateOneTOSS {ssi vAlign colSpec} {
    global newTOSS nextId

    set colW [ComputeCOLW $colSpec]
    set colJ [ComputeCOLJ $colSpec]
    set names [array names newTOSS]
    foreach name $names {
	array set thisTOSS $newTOSS($name)
	if {[string compare $colW $thisTOSS(colW)]} {
	    if {[string compare $colJ $thisTOSS(colJ)]} {
		if {[string compare $vAlign $thisTOSS(vAlign)]} {
		    return $name
		}
	    }
	}
    }

    # no matching colW,colJ, add an entry
    if {$ssi == ""} {
	set ssi HBF-SDL-RESERVED[incr nextId]
    }
    set thisTOSS(colW)   $colW
    set thisTOSS(colJ)   $colJ
    set thisTOSS(vAlign) $vAlign
    set newTOSS($ssi) [array get thisTOSS]
    return $ssi
}


# save values from TFoot, we'll actually process TFoot after TBody
# but we need to know whether we have a TFoot and whether that TFoot
# has ColSpec elements in order to push/pop a FORM for the TBody if
# so
proc PrepForTFoot {nColSpecs} {
    global haveTFoot needTFootForm

    set haveTFoot 1
    set needTFootForm [expr "$nColSpecs > 0"]
}


# start a table header, footer or body - create a FORM to hold the rows;
# create an empty row to be filled in by the Entry elements - set the
# current row and number of rows to 1
proc StartTHeadTFootTBody {parent gi haveTHead id vAlign nRows nColSpecs} {
    global numberOfColSpecs haveTFoot
    global needTFootForm

    if {$parent == "ENTRYTBL"} {
	upvar #0 entryTableRowDope     rowDope
	upvar #0 needEntryTblTHeadForm needTHeadForm
	global entryTableAttributes
	set nCols $entryTableAttributes(cols)
	set entryTableAttributes(vAlign) $vAlign
	set entryTableAttributes(rows)   $nRows
    } else {
	upvar #0 tableGroupRowDope   rowDope
	upvar #0 needTGroupTHeadForm needTHeadForm
	global tableGroupAttributes
	set nCols $tableGroupAttributes(cols)
	set tableGroupAttributes(vAlign) $vAlign
	set tableGroupAttributes(rows)   $nRows
    }

    set numberOfColSpecs $nColSpecs

    # get the proper list of ColSpecs for the current context
    if {$parent == "ENTRYTBL"} {
	set parentName entryTable
    } else {
	set parentName tableGroup
    }
    switch  $gi {
	THEAD {upvar #0 ${parentName}HeadColSpecs colSpecs}
	TBODY {upvar #0 ${parentName}ColSpecs colSpecs}
	TFOOT {upvar #0 tableFootColSpecs colSpecs }
    }

    # if no ColSpec definitions at this level, copy the parent's
    # ColSpec definition to here
    if {$nColSpecs == 0} {
	switch  $gi {
	    THEAD   {upvar #0 ${parentName}ColSpecs parentColSpecs}
	    TFOOT   {upvar #0 tableGroupColSpecs parentColSpecs}
	}
	if {$gi != "TBODY"} {
	    set colSpecs $parentColSpecs
	}
    }

    # if we have ColSpec elements on a THead, we'll need to put it
    # in its own FORM; we saved this value for TFoot earlier
    # because TFoot precedes TBody in the content model but doesn't
    # get processed until after TBody (as EndText: to TGroup)
    if {$gi == "THEAD"} {
	set needTHeadForm [expr "$nColSpecs > 0"]
    }

    # determine whether we need to push a new FORM here - we always
    # have to push a FORM for a THead, we only push one for TBody
    # if THead needed its own or there was no THead and we only push
    # one for TFoot if it needs its own
    if {!$haveTHead} {
	set needTBodyForm 1
    } else {
	set needTBodyForm $needTHeadForm
    }
    set doit 0
    switch $gi {
	THEAD {set doit 1}
	TBODY {set doit $needTBodyForm}
	TFOOT {set doit $needTFootForm}
    }

    # and push it, if so
    if {$doit} {
	set ssi [CreateOneTOSS $id "" $colSpecs]
	PushForm TABLE "$ssi" $id
    }

    set rowDope(nRows)      0
    set rowDope(currentRow) 0
}


# end a table header footer or body - delete the global row
# information and close the FORM; also delete the ColSpec info for
# this THead or TFoot (TBody always uses the parent's)
proc EndTHeadTFootTBody {parent gi} {
    global numberOfColSpecs needTFootForm haveTFoot

    if {$parent == "ENTRYTBL"} {
	upvar #0 needEntryTblTHeadForm needTHeadForm
    } else {
	upvar #0 needTGroupTHeadForm needTHeadForm
    }

    # determine whether we want to terminate this FORM here - we
    # only terminate the THead FORM if it needed its own, we only
    # terminate the TBody FORM if the TFoot needs its own or there
    # is no TFoot and we always terminate the FORM for TFoot
    if {($parent == "ENTRYTBL") || !$haveTFoot} {
	set needTBodyForm 1
    } else {
	set needTBodyForm $needTFootForm
    }
    set doit 0
    switch $gi {
	THEAD {set doit $needTHeadForm}
	TBODY {set doit $needTBodyForm}
	TFOOT {set doit 1}
    }
    PopTableForm $parent $gi $doit

    # blow away the list of ColSpecs for the current context
    switch  $gi {
	THEAD    { if {$parent == "ENTRYTBL"} {
		       global entryTableHeadColSpecs
		       unset entryTableHeadColSpecs
		   } else {
		       global tableGroupHeadColSpecs
		       unset tableGroupHeadColSpecs
		   }
		 }
	TFOOT    { global tableFootColSpecs
		   unset tableFootColSpecs
		 }
    }
}


# start a table row - save the attribute values for when we
# actually emit the entries of this row; when we emit the first
# entry we'll emit the ID on the rowSep FORM that we create for each
# Entry and set the ID field to "" so we only emit the ID once
proc StartRow {grandparent parent id rowSep vAlign} {
    if {$grandparent == "ENTRYTBL"} {
	upvar #0 entryTableRowDope rowDope
	global entryTableAttributes
	set nCols $entryTableAttributes(cols)
	if {$vAlign == ""} {
	    set vAlign $entryTableAttributes(vAlign)
	}
    } else {
	upvar #0 tableGroupRowDope rowDope
	global tableGroupAttributes
	set nCols $tableGroupAttributes(cols)
	if {$vAlign == ""} {
	    set vAlign $tableGroupAttributes(vAlign)
	}
    }
    upvar 0 rowDope(currentRow) currentRow
    upvar 0 rowDope(nRows)      nRows

    set rowDope(id)     $id
    set rowDope(rowSep) $rowSep
    set rowDope(vAlign) $vAlign

    incr currentRow
    if {![info exists rowDope(row$currentRow)]} {
	set rowDope(row$currentRow) [MakeEmptyRow $nCols]
	incr nRows
    }
}

# a debugging procedure
proc DumpRowDope {rowDopeName} {
    upvar 1 $rowDopeName rowDope

    puts stderr "rowDope:"
    set index 0
    while {[incr index] <= $rowDope(nRows)} {
	puts stderr \
	    "    $index: ([llength $rowDope(row$index)]) $rowDope(row$index)"
    }
}


# end a table row
proc EndRow {grandparent parent} {
    global emptyCells nextId haveTFoot

    # this row could be in a TGroup or an EntryTbl
    if {$grandparent == "ENTRYTBL"} {
	upvar #0 entryTableRowDope rowDope
	global entryTableAttributes
	set nCols    $entryTableAttributes(cols)
	set nRowDefs $entryTableAttributes(rows)
    } else {
	upvar #0 tableGroupRowDope rowDope
	global tableGroupAttributes
	set nCols    $tableGroupAttributes(cols)
	set nRowDefs $tableGroupAttributes(rows)
    }

    # get the proper list of ColSpecs for the current context
    switch  $parent {
	THEAD    { if {$grandparent == "ENTRYTBL"} {
		       upvar #0 entryTableHeadColSpecs colSpecs
		   } else {
		       upvar #0 tableGroupHeadColSpecs colSpecs
		   }
		 }
	TBODY    { if {$grandparent == "ENTRYTBL"} {
		       upvar #0 entryTableColSpecs colSpecs
		   } else {
		       upvar #0 tableGroupColSpecs colSpecs
		   }
		 }
	TFOOT    { upvar #0 tableFootColSpecs  colSpecs }
    }

    # go over the row filing empty cells with an empty FORM containing
    # an empty BLOCK.  The FORM SSI= is chosen to give a RowSep based
    # upon the current ColSpec and rowDope, if we are on the last row
    # we want to set the RowSep to 0 unless there were more rows
    # created via the MoreRows attribute of Entry or EntryTbl forcing
    # the table to be longer than the number of Rows specified in which
    # case we want to fill in all those rows too and only force RowSep
    # to 0 on the last one; the inner BLOCK SSI= is chosen to give a
    # ColSep based upon the current ColSpec and Row definition - if
    # the column is the last one in the row, the ColSep is set to 0
    set currentRow $rowDope(currentRow)
    if {$currentRow == $nRowDefs} {
	set moreRows [expr "$rowDope(nRows) - $nRowDefs"]
    } else {
	set moreRows 0
    }
    upvar 0 rowDope(row$currentRow) thisRow
    upvar 0 rowDope(row[expr "$currentRow - 1"]) prevRow
    while {$moreRows >= 0} {
	set colIndex 0
	while {$colIndex < $nCols} {
	    set thisCellId [lindex $thisRow $colIndex]
	    if {$thisCellId == ""} {
		array set thisColSpec [lindex $colSpecs $colIndex]
		set desiredCell(colSep) $thisColSpec(colSep)
		set desiredCell(rowSep) $thisColSpec(rowSep)
		if {$rowDope(rowSep) != ""} {
		    set desiredCell(rowSep) $rowDope(rowSep)
		}
		if {$colIndex == $nCols} {
		    set desiredCell(colSep) 0
		}
		if {($moreRows == 0) && ($currentRow == $nRowDefs)} {
		    if {($parent == "TFOOT") ||
			(($parent == "TBODY") && (!$haveTFoot))} {
			set desiredCell(rowSep) 0
		    }
		}
		if {$desiredCell(colSep) == ""} {
		    set desiredCell(colSep) 1
		}
		if {$desiredCell(rowSep) == ""} {
		    set desiredCell(rowSep) 1
		}
		set found 0
		foreach id [array names emptyCells] {
		    array set thisCell $emptyCells($id)
		    if {$thisCell(colSep) != $desiredCell(colSep)} {
			continue
		    }
		    if {$thisCell(rowSep) != $desiredCell(rowSep)} {
			continue
		    }
		    if {$currentRow > 1} {
			if {[lindex $prevRow $colIndex] == $id} {
			    continue
			}
		    }
		    if {$colIndex > 0} {
			if {$lastCellId == $id} {
			    continue
			}
		    }
		    set thisCellId $id
		    set found 1
		    break
		}
		if {!$found} {
		    if {$desiredCell(rowSep)} {
			set ssi BORDER-BOTTOM
		    } else {
			set ssi BORDER-NONE
		    }
		    set id [PushFormCell $ssi ""]
		    if {$desiredCell(colSep)} {
			set ssi ENTRY-NONE-YES-NONE
		    } else {
			set ssi ENTRY-NONE-NO-NONE
		    }
		    StartBlock CELL $ssi "" 1
		    PopForm
		    set emptyCells($id) [array get desiredCell]
		    set thisCellId $id
		}
		Replace thisRow $colIndex 1 $thisCellId
	    }
	set lastCellId $thisCellId
	incr colIndex
	}
	incr moreRows -1
	incr currentRow 1
	upvar 0 thisRow prevRow
	upvar 0 rowDope(row$currentRow) thisRow
    }

    # blow away the variables that get reset on each row
    unset rowDope(id)
    unset rowDope(rowSep)
    unset rowDope(vAlign)
}


# given a row list, an id and start and stop columns, replace the
# entries in the list from start to stop with id - use "upvar" on
# the row list so we actually update the caller's row
proc Replace {callersRow start length id} {
    upvar $callersRow row

    # length will be 0 if there was an error on the row
    if {$length <= 0} {
	return
    }

    # make a list of ids long enough to fill the gap
    set i 1
    set ids $id; # we pad all the others with a starting space
    while {$i < $length} {
	append ids " " $id
	incr i
    }

    # do the list replacement - need to "eval" because we want the
    # ids to be seen a individual args, not a list so we need to
    # evaluate the command twice
    set stop [expr "$start + $length - 1"]
    set command "set row \[lreplace \$row $start $stop $ids\]"
    eval $command
}


# process a table cell (Entry or EntryTbl); attributes are inherited
# in the following fashion:
#
#	ColSpec
#	SpanSpec
#	Row
#	Entry/EntryTbl
#
# with later values (going down the list) overriding earlier ones;
# Table, TGroup, etc., values have already been propagated to the
# ColSpecs
proc StartCell {ancestor grandparent gi id align char colName cols
		    colSep moreRows nameEnd nameSt rowSep spanName
			vAlign nColSpecs nTBodies} {
    global colNames tableGroupAttributes entryTableAttributes
    global numberOfColSpecs entryTableColSpecs nextId haveTFoot
    global needEntryTblTHeadForm entryTableSavedFRowVec

    # get the appropriate SpanSpec list, if any; also get the row
    # row dope vector which also contains the current row number
    # and number of rows currently allocated (we might get ahead
    # of ourselves due to a vertical span via MOREROWS=)
    if {$ancestor == "TGROUP"} {
	upvar #0 tableGroupSpanSpecs spanSpecs
	upvar #0 tableGroupRowDope   rowDope
	set nCols $tableGroupAttributes(cols)
	set nRowDefs $tableGroupAttributes(rows)
    } else {
	upvar #0 entryTableSpanSpecs spanSpecs
	upvar #0 entryTableRowDope   rowDope
	set nCols $entryTableAttributes(cols)
	set nRowDefs $entryTableAttributes(rows)
    }

    # get the proper list of ColSpecs for the current context
    switch  $grandparent {
	THEAD    { if {$ancestor == "ENTRYTBL"} {
		       upvar #0 entryTableHeadColSpecs colSpecs
		   } else {
		       upvar #0 tableGroupHeadColSpecs colSpecs
		   }
		 }
	TBODY    { if {$ancestor == "ENTRYTBL"} {
		       upvar #0 entryTableColSpecs colSpecs
		   } else {
		       upvar #0 tableGroupColSpecs colSpecs
		   }
		 }
	TFOOT    { upvar #0 tableFootColSpecs  colSpecs }
    }

    # check for a span
    if {$spanName != ""} {
	if {[info exists spanSpecs($spanName)]} {
	    array set thisSpan $spanSpecs($spanName)
	    # SpanSpec column names win over explicit ones
	    set nameSt  $thisSpan(nameSt)
	    set nameEnd $thisSpan(nameEnd)
	} else {
	    UserError "Attempt to use undefined SpanSpec \"$spanName\"" yes
	}
    }

    # nameSt, whether explicit or from a span, wins over colName
    if {$nameSt != ""} {
	set colName $nameSt
    }

    # get the row information - use upvar so we can update rowDope
    upvar 0 rowDope(currentRow)     currentRow
    upvar 0 rowDope(row$currentRow) thisRow
    upvar 0 rowDope(nRows)          nRows

    # by now, if no colName we must have neither colName, nameSt nor
    # a horizontal span - find the next open spot in this row
    if {$colName != ""} {
	if {[info exists colNames($colName)]} {
	    set startColNum $colNames($colName)
	    if {$startColNum > $nCols} {
		UserError "Attempt to address column outside of table" yes
		set colName ""
	    } else {
		incr startColNum -1 ;# make the column number 0 based
	    }
	} else {
	    UserError "Attempt to use undefined column name \"$colName\"" yes
	    set colName ""
	}
    }
    if {$colName == ""} {
        set index 0
	while {[lindex $thisRow $index] != ""} {
	    incr index
	}
	if {$index == $nCols} {
	    UserError "More entries defined than columns in this row" yes
	    set index -1
	}
	set startColNum $index
    }

    # if we have a nameEnd, it was either explicit or via a span -
    # get the stop column number; else set the stop column to the
    # start column, i.e., a span of 1
    if {$nameEnd == ""} {
	set stopColNum $startColNum
    } else {
	if {[info exists colNames($nameEnd)]} {
	    set stopColNum $colNames($nameEnd)
	    if {$stopColNum > $nCols} {
		UserError "Attempt to address column outside of table" yes
		set stopColNum $nCols
	    }
	    incr stopColNum -1 ;# make the column number 0 based
	    if {$startColNum > $stopColNum} {
		UserError "End of column span is before the start" yes
		set stopColNum $startColNum
	    }
	} else {
	    UserError "Attempt to use undefined column name \"$nameEnd\"" yes
	    set stopColNum $startColNum
	}
    }

    # create an empty set of attributes for the cell - we'll fill
    # them in from the ColSpec, SpanSpec, Row and Entry or EntryTbl
    # defined values, if any, in that order
    set cellAlign  ""
    set cellColSep 1
    set cellRowSep 1
    set cellVAlign ""

    # initialize the cell description with the ColSpec data
    # Table, TGroup and EntryTable attributes have already
    # percolated to the ColSpec
    if {$startColNum >= 0} {
	array set thisColSpec [lindex $colSpecs $startColNum]
	if {$thisColSpec(colSep) != ""} {
	    set cellColSep $thisColSpec(colSep)
	}
	if {$thisColSpec(rowSep) != ""} {
	    set cellRowSep $thisColSpec(rowSep)
	}
    }

    # overlay any attributes defined on the span, that is, SpanSpec
    # attributes win over ColSpec ones
    if {[info exists thisSpan]} {
	if {$thisSpan(align) != ""} {
	    set cellAlign $thisSpan(align)
	}
	if {$thisSpan(colSep) != ""} {
	    set cellColSep $thisSpan(colSep)
	}
	if {$thisSpan(rowSep) != ""} {
	    set cellRowSep $thisSpan(rowSep)
	}
    }

    # overlay any attributes defined on the Row
    if {$rowDope(rowSep) != ""} {
	set cellRowSep $rowDope(rowSep)
    }
    if {$rowDope(vAlign) != ""} {
	set cellVAlign $rowDope(vAlign)
    }

    # check for a char other than "" or "."; just a check, we don't
    # do anything with char
    set char [CheckChar $char]

    # overlay any attributes defined on the Entry or EntryTbl - these
    # win over all
    if {$align != ""} {
	set cellAlign $align
    }
    if {$colSep != ""} {
	set cellColSep $colSep
    }
    if {$rowSep != ""} {
	set cellRowSep $rowSep
    }
    if {$vAlign != ""} {
	set cellVAlign $vAlign
    }
    
    # if this cell is the first on the row, feed it the (possible)
    # Row ID and set the Row ID to ""
    if {[set cellId $rowDope(id)] == ""} {
	set cellId SDL-RESERVED[incr nextId]
    } else {
	set rowDope(id) ""
    }

    # now put the cell into the rowDope vector - if there's a
    # span, we'll put the cell in several slots; if there's a
    # vertical straddle, we may need to add more rows to rowDope
    if {$startColNum >= 0} {
	set stopRowNum [expr "$currentRow + $moreRows"]
	set spanLength [expr "($stopColNum - $startColNum) + 1"]
	set rowIndex $currentRow
	while {$rowIndex <= $stopRowNum} {
	    if {![info exists rowDope(row$rowIndex)]} {
		set rowDope(row$rowIndex) [MakeEmptyRow $nCols]
		incr nRows
	    }
	    upvar 0 rowDope(row$rowIndex) thisRow
	    set colIndex $startColNum
	    while {$colIndex <= $stopColNum} {
		if {[lindex $thisRow $colIndex] != ""} {
		    set badValMess1 "Multiple definitions for column"
		    set badValMess2 "of row $rowIndex"
		    UserError \
			"$badValMess1 [expr $colIndex + 1] $badValMess2" yes
		    set stopColNum  0
		    set stopRowNum  0
		    set spanLength  0
		}
		incr colIndex
	    }
	    Replace thisRow $startColNum $spanLength $cellId
	    incr rowIndex
	}
    }

    # on the last column, the column separator should be 0; on the
    # last row, the row separator should be 0 - the table frame will
    # set the border on the right and bottom sides
    if {$stopColNum == $nCols} {
	set cellColSep 0
    }
    if {$currentRow == $nRowDefs} {
	if {($grandparent == "TFOOT") ||
	    (($grandparent == "TBODY") && (!$haveTFoot))} {
	    set cellRowSep 0
	}
    }

    # push a form to hold the RowSep
    if {$cellRowSep == 1} {
	set ssi "BORDER-BOTTOM"
    } else {
	set ssi "BORDER-NONE"
    }
    PushFormCell $ssi $cellId

    # build the SSI= for the cell and push a form to hold it
    if {$gi == "ENTRY"} {
	set ssi "ENTRY-"
    } else {
	set ssi "ENTRYTBL-"
    }
    switch $cellAlign {
	""      { append ssi "NONE-" }
	LEFT    { append ssi "LEFT-" }
	RIGHT   { append ssi "RIGHT-" }
	CENTER  { append ssi "CENTER-" }
	JUSTIFY { append ssi "LEFT-" }
	CHAR    { append ssi "CHAR-" }
    }
    switch $cellColSep {
	0 { append ssi "NO-" }
	1 { append ssi "YES-" }
    }
    switch $cellVAlign {
	""     -
	NONE   { append ssi "NONE" }
	TOP    { append ssi "TOP" }
	MIDDLE { append ssi "MIDDLE" }
	BOTTOM { append ssi "BOTTOM" }
    }
    PushForm CELL $ssi $id

    # if we are in an Entry, open a paragraph in case all that's in
    # the Entry are inline objects - this may end up in an empty P
    # if the Entry contains paragraph level things, e.g., admonitions,
    # lists or paragraphs; if we are an EntryTbl, set up the defaults
    # for the recursive calls to, e.g., THead or TBody
    if {$gi == "ENTRY"} {
	StartParagraph "" "" ""
    } else {
	# the syntax would allow multiple TBODY in an ENTRYTBL but
	# we (and the rest of the SGML community, e.g., SGML/Open)
	# don't allow more than one - the transpec will keep us from
	# seeing the extras but we need to flag the error to the user
        if {$nTBodies != 1} {
	    UserError "More than one TBODY in an ENTRYTBL" yes
	}

	set entryTableAttributes(align) $align
	set entryTableAttributes(char)  [CheckChar $char]

	# do a sanity check on the number of columns, there must be
	# at least 1
	if {$cols <= 0} {
	    UserError "Unreasonable number of columns ($cols) in EntryTbl" yes
	    set cols 1
	}
	set entryTableAttributes(cols)  $cols

	if {$colSep == ""} {
	    set entryTableAttributes(colSep) 1
	} else {
	    set entryTableAttributes(colSep) $colSep
	}
	if {$rowSep == ""} {
	    set entryTableAttributes(rowSep) 1
	} else {
	    set entryTableAttributes(rowSep) $rowSep
	}

	# check for more COLSPECs than COLS - error if so
	if {$nColSpecs > $cols} {
	    UserError \
		"More ColSpecs defined than columns in an EntryTbl" yes
	}

	set numberOfColSpecs $nColSpecs

	set entryTableColSpecs ""

	# if no ColSpec definitions at this level, set them all to the
	# defaults - take advantage of the fact that the function ColSpec
	# will create default column specifications to fill out up to an
	# explicitly set ColNum
	if {$nColSpecs == 0} {
	    ColSpec "" ENTRYTBL "" "" "" $cols "" "" ""
	}

	# initialize a variable used to determine if we need a separate
	# FORM element for THead - if ColSpec elements are not given
	# at that level, it can go in the same FORM as the TBody and
	# we can guarantee that the columns will line up
	set needEntryTblTHeadForm 0

	# and initialize a variable to hold saved FROWVEC elements
	# across THead into TBody in case we are merging them into
	# one FORM element rather than putting each in its own
	set entryTableSavedFRowVec ""
    }
}


# end a table Entry - pop the form holding the cell
# attributes and the form holding the RowSep
proc EndEntry {} {
    PopForm
    PopForm
}


# end a table EntryTbl - pop the form holding the cell
# attributes and the form holding the RowSep and clean up the
# global variables
proc EndEntryTbl {} {
    global entryTableSpanSpecs numberOfColSpecs entryTableColSpecs

    PopForm
    PopForm

    if {[info exists entryTableSpanSpecs]} {
	unset entryTableSpanSpecs
    }

    unset entryTableColSpecs
}

######################################################################
######################################################################
#
#                        RefEntry
#
######################################################################
######################################################################

# change the OutputString routine into one that will save the content
# of this element for use as the man-page title, e.g., the "cat"
# in "cat(1)"; this name may be overridden by RefDescriptor in
# RefNameDiv if the sort name is different (e.g., "memory" for
# "malloc")
proc DivertOutputToManTitle {} {
    rename OutputString SaveManTitleOutputString
    rename ManTitleOutputString OutputString
}


# change the output stream back to the OutputString in effect at the
# time of the call to DivertOutputToManTitle
proc RestoreOutputStreamFromManTitle {} {
    rename OutputString ManTitleOutputString
    rename SaveManTitleOutputString OutputString
}


# a routine to buffer the output into the string "manTitle" for later
# use in the top corners of man-pages
proc ManTitleOutputString {string} {
    global manTitle

    append manTitle $string
}


# change the OutputString routine into one that will save the content
# of this element for use as the man-page volume number, e.g., the "1"
# in "cat(1)"
proc DivertOutputToManVolNum {} {
    rename OutputString SaveManVolNumOutputString
    rename ManVolNumOutputString OutputString
}


# change the output stream back to the OutputString in effect at the
# time of the call to DivertOutputToManVolNum
proc RestoreOutputStreamFromManVolNum {} {
    rename OutputString ManVolNumOutputString
    rename SaveManVolNumOutputString OutputString
}


# a routine to buffer the output into the string "manVolNum" for later
# use in the top corners of man-pages
proc ManVolNumOutputString {string} {
    global manVolNum

    append manVolNum $string
}


# start a reference name division; nothing to emit now, just save
# the number of names defined in this division and initialize the
# current name count to 1
proc StartRefNameDiv {nNames} {
    global numManNames currentManName

    set numManNames $nNames
    set currentManName 1
}


# end a reference name division; we can now emit the HEAD elements to
# create the titles in the upper corners and the "NAME" section of the
# man-page
proc EndRefNameDiv {id} {
    global manTitle manVolNum manDescriptor manNames manPurpose
    global localizedAutoGeneratedStringArray

    set manPageName $manTitle
    if {$manDescriptor != ""} {
	set manPageName $manDescriptor
    }

    # emit the titles in the upper left and right corners
    Emit "<HEAD TYPE=\"LITERAL\" SSI=\"MAN-PAGE-TITLE-LEFT\">"
    Emit "${manPageName}($manVolNum)"
    Emit "</HEAD>\n"
    Emit "<HEAD TYPE=\"LITERAL\" SSI=\"MAN-PAGE-TITLE-RIGHT\">"
    Emit "${manPageName}($manVolNum)"
    Emit "</HEAD>\n"

    # and the NAME section
    PushForm "" "" $id
    Emit "<HEAD TYPE=\"LITERAL\" SSI=\"MAN-PAGE-DIVISION-NAME\">"
    set message "NAME"
    Emit $localizedAutoGeneratedStringArray($message)
    Emit "</HEAD>\n"
    StartBlock "" "MAN-PAGE-DIVISION" "" 1
    StartParagraph "" "" ""
    Emit "$manNames - $manPurpose"
    PopForm
}


# change the OutputString routine into one that will save the content
# of this element for use as the man-page descriptor, e.g., the
# "string" in "string(3C)"
proc DivertOutputToManDescriptor {} {
    rename OutputString SaveManDescriptorOutputString
    rename ManDescriptorOutputString OutputString
}


# change the output stream back to the OutputString in effect at the
# time of the call to DivertOutputToManDescriptor
proc RestoreOutputStreamFromManDescriptor {} {
    rename OutputString ManDescriptorOutputString
    rename SaveManDescriptorOutputString OutputString
}


# a routine to buffer the output into the string "manDescriptor" for
# later use in the top corners of man-pages
proc ManDescriptorOutputString {string} {
    global manDescriptor

    append manDescriptor $string
}


# change the OutputString routine into one that will save the content
# of this element for use as the man-page command or function name,
# e.g., the "cat" in "cat(1)"
proc DivertOutputToManNames {} {
    rename OutputString SaveManNamesOutputString
    rename ManNamesOutputString OutputString
}


# change the output stream back to the OutputString in effect at the
# time of the call to DivertOutputToManNames
proc RestoreOutputStreamFromManNames {} {
    rename OutputString ManNamesOutputString
    rename SaveManNamesOutputString OutputString
}


# a routine to buffer the output into the string "manNames" for
# later use in the top corners of man-pages
proc ManNamesOutputString {string} {
    global manNames

    append manNames $string
}


# collect RefName elements into a single string; start diversion to
# the string on the first man name
proc StartAManName {} {
    global numManNames currentManName

    if {$currentManName == 1} {
	DivertOutputToManNames
    }
}


# end diversion on the last man name; append "(), " to each name but
# the last to which we only append "()"
proc EndAManName {} {
    global numManNames currentManName manDescriptor manNames

    if {($currentManName == 1) && ($manDescriptor == "")} {
	set manDescriptor $manNames
    }

    if {$currentManName < $numManNames} {
	Emit ", "
    } elseif {$currentManName == $numManNames} {
	RestoreOutputStreamFromManNames
    }

    incr currentManName
}


# change the OutputString routine into one that will save the content
# of this element for use as the man-page purpose; this string will
# follow the function or command name(s) separated by a "-"
proc DivertOutputToManPurpose {} {
    rename OutputString SaveManPurposeOutputString
    rename ManPurposeOutputString OutputString
}


# change the output stream back to the OutputString in effect at the
# time of the call to DivertOutputToManPurpose
proc RestoreOutputStreamFromManPurpose {} {
    rename OutputString ManPurposeOutputString
    rename SaveManPurposeOutputString OutputString
}


# a routine to buffer the output into the string "manPurpose" for
# later use in the NAME section of man-pages
proc ManPurposeOutputString {string} {
    global manPurpose

    append manPurpose $string
}


# start a reference synopsis division - create a FORM to hold the
# division and, potentially, any RefSect2-3; if there is a Title on
# RefSynopsisDiv, use it, else default to "SYNOPSIS"
proc StartRefSynopsisDiv {id haveTitle nSynopses} {
    global remainingSynopses
    global localizedAutoGeneratedStringArray

    set remainingSynopses $nSynopses
    PushForm "" "" $id
    if {!$haveTitle} {
	StartManPageDivisionTitle ""
	set message "SYNOPSIS"
	Emit $localizedAutoGeneratedStringArray($message)
	EndManPageDivisionTitle
    }
}


# the user provided a title for this section, use it
proc StartManPageDivisionTitle {id} {
    if {$id != ""} {
	set id " ID=\"$id\""
    }
    Emit "<HEAD$id TYPE=\"LITERAL\" SSI=\"MAN-PAGE-DIVISION-NAME\">"
}


# the user provided a title for this section, we need to open a form
# to hold the section now
proc EndManPageDivisionTitle {} {
    Emit "</HEAD>\n"
    PushForm "" "MAN-PAGE-DIVISION" ""
}

# begin a Synopsis - if this is the first of any of the synopses, emit
# a FORM to hold them all
proc StartSynopsis {id linespecific} {
    if {$linespecific == ""} {
	set type LINED
    } else {
	set type ""
    }
    StartParagraph id "" $type
}


# end any of Synopsis, CmdSynopsis or FuncSynopsis - close out the
# form if it's the last one
proc EndSynopses {parent} {
    global remainingSynopses

    Emit "\n"

    if {($parent == "REFSYNOPSISDIV") && ([incr remainingSynopses -1] == 0)} {
	PopForm
    }
}


# begin a CmdSynopsis
proc StartCmdSynopsis {id} {
    StartParagraph id "" ""
}


# start a man-page argument - surround the arg in a KEY element
proc StartArg {id choice separator} {
    # mark this spot if there's a user supplied ID
    Anchor $id

    # emit nothing at start of list, v-bar inside of Group else space
    Emit $separator

    Emit "<KEY CLASS=\"NAME\" SSI=\"MAN-PAGE-ARG\">"
    if {$choice == "OPT"} {
	Emit "\["
    } elseif {$choice == "REQ"} {
	Emit "\{"
    }
}


# end a man-page argument - if choice is not "plain", emit the proper
# close character for the choice; if repeat is "repeat", emit an
# ellipsis after the arg
proc EndArg {choice repeat} {
    if {$choice == "OPT"} {
	Emit "\]"
    } elseif {$choice == "REQ"} {
	Emit "\}"
    }
    if {$repeat == "REPEAT"} {
	Emit "<SPC NAME=\"\[hellip\]\">"
    }
    Emit "</KEY>"
}


# start an argument, filename, etc., group in a man-page command
# synopsis
proc StartGroup {id choice separator} {
    # mark this spot if there's a user supplied ID
    Anchor $id

    # emit nothing at start of list, v-bar inside of Group else space
    Emit $separator

    # clean up optmult/reqmult since, for example, req+repeat == reqmult,
    # optmult and reqmult are redundant
    if {$choice == "OPTMULT"} {
	set choice OPT
    } elseif {$choice == "REQMULT"} {
	set choice REQ
    }

    if {$choice == "OPT"} {
	Emit "\["
    } elseif {$choice == "REQ"} {
	Emit "\{"
    }
}


# end an argument, filename, etc., group in a man-page command
# synopsis
proc EndGroup {choice repeat} {
    # clean up optmult/reqmult since, for example, req+repeat == reqmult,
    # optmult and reqmult are redundant
    if {$choice == "OPTMULT"} {
	set choice OPT
	set repeat REPEAT
    } elseif {$choice == "REQMULT"} {
	set choice "REQ"
	set repeat REPEAT
    }
    if {$choice == "OPT"} {
	Emit "\]"
    } elseif {$choice == "REQ"} {
	Emit "\}"
    }
    if {$repeat == "REPEAT"} {
	Emit "<SPC NAME=\"\[hellip\]\">"
    }
}


# start a command name in a man-page command synopsis
proc StartCommand {id separator} {
    # mark this spot if there's a user supplied ID
    Anchor $id

    # emit nothing at start of synopsis else space
    Emit $separator

    Emit "<KEY CLASS=\"NAME\" SSI=\"MAN-PAGE-COMMAND\">"
}


# begin a FuncSynopsis
proc StartFuncSynopsis {id} {
}


# check that the GI of the element pointed to by a SynopFragmentRef
# is really a SynopFragment
proc CheckSynopFragmentRef {gi id} {
    if {$gi != "SYNOPFRAGMENT"} {
	set badValMess1 "SynopFragmentRef LinkEnd=$id"
	set badValMess2 "must refer to a SynopFragment"
	UserError "$badValMess1 $badValMess2" yes
    }
}


# begin a FuncSynopsisInfo - emit a P to hold it
proc StartFuncSynopsisInfo {id linespecific} {
    if {$linespecific == "LINESPECIFIC"} {
	set type " TYPE=\"LINED\""
    } else {
	set type ""
    }

    StartParagraph $id "FUNCSYNOPSISINFO" $type
}


# begin a FuncDef - emit a P to hold it
proc StartFuncDef {id} {
    StartParagraph $id "FUNCDEF" ""
}


# end a FuncDef, emit the open paren in preparation for the args
proc EndFuncDef {} {
    Emit "("
}


# handle Void or Varargs in a FuncSynopsis - wrap it in a KEY and
# emit the string "VOID" or "VARARGS"
proc DoVoidOrVarargs {gi id} {
    # mark this spot if there's a user supplied ID
    Anchor $id

    Emit "<KEY CLASS=\"NAME\" SSI=\"FUNCDEF-ARGS\">"
    Emit $gi
    Emit "</KEY>"
    Emit ")"
}


# start a ParamDef - just emit an anchor, if needed, for now
proc StartParamDef {id} {
    # mark this spot if there's a user supplied ID
    Anchor $id
}


# end of a ParamDef - emit either the ", " for the next one or, if the
# last, emit the closing ")"
proc EndParamDef {separator} {
    Emit $separator
}


# start a FuncParams - just emit an anchor, if needed, for now
proc StartFuncParams {id} {
    # mark this spot if there's a user supplied ID
    Anchor $id
}


# end of a FuncParams - emit either the ", " for the next one or, if the
# last, emit the closing ")"
proc EndFuncParams {separator} {
    Emit $separator
}


######################################################################
######################################################################
#
#                             links
#
######################################################################
######################################################################
# open an intradocument link
proc StartLink {id linkend type} {
    StartParagraphMaybe "" "P" $id

    Emit "<LINK RID=\"$linkend\""
    if {$type != ""} {
	set type [string toupper $type]
	switch $type {
	    JUMPNEWVIEW {Emit " WINDOW=\"NEW\""}
	    DEFINITION  {Emit " WINDOW=\"POPUP\""}
	}
    }
    Emit ">"

    Anchor $id
}


# defer a Link at the start of a Para until we see if the following 
# InlineGraphic has Role=graphic and we want it in a HEAD
proc DeferLink {id linkend type} {
    global deferredLink

    set deferredLink(gi)      LINK
    set deferredLink(id)      $id
    set deferredLink(linkend) $linkend
    set deferredLink(type)    $type
}


# open an interdocument link; this link will require an SNB entry
proc StartOLink {id localInfo type} {
    StartParagraphMaybe "" "P" $id

    set type [string toupper $type]

    set linkType CURRENT
    switch $type {
	JUMP        {set linkType CURRENT}
	JUMPNEWVIEW {set linkType NEW}
	MAN         -
        DEFINITION  {set linkType POPUP}
    }

    set snbType CROSSDOC
    switch $type {
	EXECUTE     {set snbType SYS-CMD}
	APP-DEFINED {set snbType CALLBACK}
	MAN         {set snbType MAN-PAGE}
    }

    set snbId [AddToSNB $snbType $localInfo]

    Emit "<LINK RID=\"$snbId\""
    if {$linkType != "CURRENT"} {
	Emit " WINDOW=\"$linkType\""
    }
    Emit ">"
}


# defer an OLink at the start of a Para until we see if the following 
# InlineGraphic has Role=graphic and we want it in a HEAD
proc DeferOLink {id localInfo type} {
    global deferredLink

    set deferredLink(gi)        OLINK
    set deferredLink(id)        $id
    set deferredLink(localinfo) $localinfo
    set deferredLink(type)      $type
}


# defer a ULink at the start of a Para until we see if the following 
# InlineGraphic has Role=graphic and we want it in a HEAD
proc DeferULink {id} {
    global deferredLink

    set deferredLink(gi)        ULINK
    set deferredLink(id)        $id
}


# close a link
proc EndLink {} {
    Emit "</LINK>"
}


######################################################################
######################################################################
#
#                       character formatting
#
######################################################################
######################################################################
# open a Quote; we'll emit two open single quotes wrapped in a
# key with a style that will put them in a proportional font so they
# fit together and look like an open double quote
proc StartQuote {id} {
  Emit "<KEY CLASS=\"QUOTE\" SSI=\"PROPORTIONAL\">"
  Anchor $id
  Emit "``</KEY>"
}

# close a Quote; we'll emit two close single quotes wrapped in a
# key with a style that will put them in a proportional font so they
# fit together and look like a close double quote
proc EndQuote {} {
  Emit "<KEY CLASS=\"QUOTE\" SSI=\"PROPORTIONAL\">''</KEY>"
}

######################################################################
######################################################################
#
#                      end of document stuff
#
######################################################################
######################################################################

# write out the .snb file - first update the file location for
# insertion of the SNB by the second pass to reflect the addition
# of the INDEX; also incorporate the INDEX and update the TOSS to
# reflect any additions necessary to support tables
proc WriteSNB {} {
    global savedSNB indexLocation tossLocation baseName

    # get a handle for the index file and the existing .sdl file;
    # prepare to write the updated .sdl file and the .snb file by
    # blowing away the current names so the second open of the .sdl
    # file is creating a new file and we don't have leftover .snb
    # or .idx files laying around
    close stdout
    set sdlInFile [open "${baseName}.sdl" r]
    set sdlSize   [file size "${baseName}.sdl"]
    #
    set idxFile   [open "${baseName}.idx" r]
    set idxSize   [file size "${baseName}.idx"]
    #
    exec rm -f ${baseName}.sdl ${baseName}.idx ${baseName}.snb
    set sdlOutFile [open "${baseName}.sdl" w]

    # create any additional TOSS entries made necessary by COLW and
    # COLJ settings for TGroup or EntryTbl elements.
    set toss [CreateTableTOSS]
    set tossSize [string length $toss]

    # get a list of the byte offsets into the .sdl file for the
    # .snb entries
    set snbLocations [lsort -integer [array names savedSNB]]

    # and write out the .snb file updating the locations as we go
    if {[llength $snbLocations] > 0} {
	set snbFile [open "${baseName}.snb" w]
	foreach location $snbLocations {
	    puts $snbFile [expr "$location + $idxSize + $tossSize"]
	    puts -nonewline $snbFile $savedSNB($location)
	}
	close $snbFile
    }

    # now update the toss and include the index file into the sdl file
    # by copying the old .sdl file to the new up to the location of
    # the first FORMSTYLE in the TOSS and emitting the new TOSS
    # entries then continue copying the old .sdl file up to the index
    # location and copying the .idx file to the new .sdl file followed
    # by the rest of the old .sdl file (the old .sdl and .idx files
    # have already been deleted from the directory), finally, close
    # the output file
    #
    # 1: copy the sdl file up to the first FORMSTYLE element or, if
    #    none, to just after the open tag for the TOSS
    set location $tossLocation
    set readSize 1024
    while {$location > 0} {
	if {$location < $readSize} { set readSize $location }
	puts -nonewline $sdlOutFile [read $sdlInFile $readSize]
	incr location -$readSize
    }
    # 2: emit the TOSS updates, if any
    puts -nonewline $sdlOutFile $toss
    # 3: copy the sdl file up to the index location
    set location [expr "$indexLocation - $tossLocation"]
    set readSize 1024
    while {$location > 0} {
	if {$location < $readSize} { set readSize $location }
	puts -nonewline $sdlOutFile [read $sdlInFile $readSize]
	incr location -$readSize
    }
    # 4: copy over the index file
    set location $idxSize
    set readSize 1024
    while {$location > 0} {
	if {$location < $readSize} { set readSize $location }
	puts -nonewline $sdlOutFile [read $idxFile $readSize]
	incr location -$readSize
    }
    # 5: and copy over the rest of the sdl file
    set location [expr "$sdlSize - $indexLocation"]
    set readSize 1024
    while {$location > 0} {
	if {$location < $readSize} { set readSize $location }
	puts -nonewline $sdlOutFile [read $sdlInFile $readSize]
	incr location -$readSize
    }
    # 6: close the output
    close $sdlOutFile
}


# read the global variable newTOSS and use the information to create
# TOSS entries for THead, TBody and TFoot; these entries will contain
# the justification and width information for the table sub-components;
# return the new TOSS elements
proc CreateTableTOSS {} {
    global newTOSS

    set returnValue ""
    foreach ssi [array names newTOSS] {
	array set thisTOSSdata $newTOSS($ssi)
	set vAlign $thisTOSSdata(vAlign)
	switch $vAlign {
	    NONE -
	    ""      { set vJust "" }
	    TOP     { set vJust "TOP" }
	    MIDDLE  { set vJust "CENTER" }
	    BOTTOM  { set vJust "BOTTOM" }
	}

	append returnValue "<FORMSTYLE\n"
	append returnValue "    CLASS=\"TABLE\"\n"
	append returnValue "    SSI=\"$ssi\"\n"
	append returnValue \
		    "    PHRASE=\"TGroup, THead or TBody specification\"\n"
	append returnValue "    COLW=\"$thisTOSSdata(colW)\"\n"
	append returnValue "    COLJ=\"$thisTOSSdata(colJ)\"\n"
	if {$vJust != ""} {
	    append returnValue "    VJUST=\"${vJust}-VJUST\"\n"
	}
	append returnValue ">\n"
    }

    return $returnValue
}


# try to open a file named docbook.tss either in our current
# directory or on TOSS_PATH - if it exists, copy it to
# the output file as the TOSS - when the first line containing
# "<FORMSTYLE" is seen, save the location so we can include the
# updates to the TOSS necessary due to needing FORMSTYLE entries for
# tables with the appropriate COLJ and COLW values
proc IncludeTOSS {} {
    global tossLocation TOSS_PATH

    set tossLocation -1
    set foundToss     0

    # look for docbook.tss in the current directory first, then on the path
    set path ". [split $TOSS_PATH :]"
    foreach dir $path {
	set tssFileName $dir/docbook.tss
	if {[file exists $tssFileName]} {
	    set foundToss 1
	    break;
	}
    }

    if {$foundToss} {
	if {[file readable $tssFileName]} {
	    set tssFile [open $tssFileName r]
	    set eof [gets $tssFile line]
	    while {$eof != -1} {
		if {[string match "*<FORMSTYLE*" [string toupper $line]]} {
		    set tossLocation [tell stdout]
		}
		puts $line
		set eof [gets $tssFile line]
	    }
	    close $tssFile
	} else {
	    UserError "$tssFileName exists but is not readable" no
	}
    } else {
	UserWarning "Could not find docbook.tss - continuing with null TOSS" no
    }

    if {$tossLocation == -1} {
	set tossLocation [tell stdout]
    }
}

proc GetLocalizedAutoGeneratedStringArray {filename} {
    global localizedAutoGeneratedStringArray

    set buffer [ReadLocaleStrings $filename]

    set regExp {^(".*")[	 ]*(".*")$} ;# look for 2 quoted strings

    set stringList [split $buffer \n]
    set listLength [llength $stringList]
    set index 0
    while {$listLength > 0} {
	set line [lindex $stringList $index]
	set line [string trim $line]
	if {([string length $line] > 0) && ([string index $line 0] != "#")} {
	    if {[regexp $regExp $line match match1 match2]} {
		set match1 [string trim $match1 \"]
		set match2 [string trim $match2 \"]
		set localizedAutoGeneratedStringArray($match1) $match2
	    } else {
		UserError \
		    "Malformed line in $filename line [expr $index + 1]" no
	    }
	}
	incr index
	incr listLength -1
    }

    set message "Home Topic"
    if {![info exists localizedAutoGeneratedStringArray($message)]} {
	set localizedAutoGeneratedStringArray($message) $message
    }
    set message "No home topic (PartIntro) was specified by the author."
    if {![info exists localizedAutoGeneratedStringArray($message)]} {
	set localizedAutoGeneratedStringArray($message) $message
    }
    set message "See"
    if {![info exists localizedAutoGeneratedStringArray($message)]} {
	set localizedAutoGeneratedStringArray($message) $message
    }
    set message "See Also"
    if {![info exists localizedAutoGeneratedStringArray($message)]} {
	set localizedAutoGeneratedStringArray($message) $message
    }
    set message "NAME"
    if {![info exists localizedAutoGeneratedStringArray($message)]} {
	set localizedAutoGeneratedStringArray($message) $message
    }
    set message "SYNOPSIS"
    if {![info exists localizedAutoGeneratedStringArray($message)]} {
	set localizedAutoGeneratedStringArray($message) $message
    }
}


# start - initialize variables and write the preamble
proc OpenDocument {host base date} {
    global docId baseName indexLocation snbLocation
    global validMarkArray partIntroId nextId
    global NO_UNIQUE_ID LOCALE_STRING_DIR
    global language charset

    # NO_UNIQUE_ID will be set to YES for test purposes so we don't
    # get spurious mismatches from the timestamp of from the system on
    # which the document was processed.
    if {[string toupper $NO_UNIQUE_ID] == "YES"} {
	set docId TEST
	set timeStamp 0
    } else {
	set docId $host
	set timeStamp $date
    }

    GetLocalizedAutoGeneratedStringArray ${LOCALE_STRING_DIR}/strings

    # split out the language and charset info from LOCALE_STRING_DIR
    #   first, remove any directory information
    set languageAndCharset [lindex [split $LOCALE_STRING_DIR /] end]
    #   then split the language and charset at the dot
    set languageAndCharset [split $languageAndCharset .]
    #   and extract the values from the resulting list
    set language [lindex $languageAndCharset 0]
    set charset  [lindex $languageAndCharset 1]

    set baseName $base

    # set up the validMarkArray values
    ReInitPerMarkInfo

    # if we have a PartIntro element, use its ID as the first-page
    # attribute - if no ID, assign one; if no PartIntro, assign an
    # ID and we'll dummy in a hometopic when we try to emit the first
    # level 1 virpage
    if {![info exists partIntroId]} {
	set partIntroId ""
    }
    if {$partIntroId == ""} {
	# set partIntroId SDL-RESERVED[incr nextId]
	set partIntroId SDL-RESERVED-HOMETOPIC
    }
    
    # open the document
    Emit "<SDLDOC PUB-ID=\"CDE 2.1\""
    Emit " DOC-ID=\"$docId\""
    Emit " LANGUAGE=\"$language\""
    Emit " CHARSET=\"$charset\""
    Emit " FIRST-PAGE=\"$partIntroId\""
    Emit " TIMESTMP=\"$timeStamp\""
    Emit " SDLDTD=\"1.1.1\">\n"

    # and create the VSTRUCT - the INDEX goes in it, the SNB goes after
    # it; if there's a Title later, it'll reset the SNB location;
    # we also need to read in docbook.tss (if any) and to create an
    # empty TOSS to cause the second pass to replace docbook.tss with
    # <src file name>.tss (if any) in the new .sdl file
    Emit "<VSTRUCT DOC-ID=\"$docId\">\n"
    Emit "<LOIDS>\n</LOIDS>\n<TOSS>\n"
    IncludeTOSS
    Emit "</TOSS>\n"
    set indexLocation [tell stdout]
    Emit "</VSTRUCT>\n"
    set snbLocation [tell stdout]
}


# done - write the index and close the document
proc CloseDocument {} {
    global inVirpage errorCount warningCount
    global snbLocation savedSNB currentSNB

    # close any open block and the current VIRPAGE
    CloseBlock
    Emit $inVirpage; set inVirpage ""

    # if the last VIRPAGE in the document had any system notation
    # block references, we need to add them to the saved snb array
    # before writing it out
    set names [array names currentSNB]
    if {[llength $names] != 0} {
	foreach name $names {
	    # split the name into the GI and xid of the SNB entry
	    set colonLoc [string first "::" $name]
	    set type [string range $name 0 [incr colonLoc -1]]
	    set data [string range $name [incr colonLoc 3] end]

	    # emit the entry
	    append tempSNB "<$type ID=\"$currentSNB($name)\" "
	    switch $type {
		GRAPHIC   -
		AUDIO     -
		VIDEO     -
		ANIMATE   -
		CROSSDOC  -
		MAN-PAGE  -
		TEXTFILE  { set command "XID" }
		SYS-CMD   { set command "COMMAND" }
		CALLBACK  { set command "DATA" }
	    }
	    append tempSNB "$command=\"$data\">\n"
	}
	set savedSNB($snbLocation) $tempSNB
	unset currentSNB
    }

    # close the document and write out the stored index and system
    # notation block
    Emit "</SDLDOC>\n"
    WriteIndex
    WriteSNB

    if {$errorCount || $warningCount} {
	puts stderr "DtDocBook total user errors:   $errorCount"
	puts stderr "DtDocBook total user warnings: $warningCount"
    }

    if {$errorCount > 0} {
	exit 1
    }

    if {$warningCount > 0} {
	exit -1
    }
}
