$ $XConsortium: dthelpprint.msg /main/3 1995/11/07 13:18:21 rswiston $
$ *************************************<+>*************************************
$ *****************************************************************************
$ **
$ **  File:        dthelpprint.msg
$ **
$ **  Project:     Cde1 Help
$ **
$ **  Description:
$ **  -----------
$ **  This file is the source for the message catalog for dthelpprint
$ **
$ **
$ *****************************************************************************
$ **
$ ** (c) Copyright 1993, 1994 Hewlett-Packard Company
$ ** (c) Copyright 1993, 1994 International Business Machines Corp.
$ ** (c) Copyright 1993, 1994 Sun Microsystems, Inc.
$ ** (c) Copyright 1993, 1994 Unix System Labs, Inc.,a subsidiary of Novell,Inc.
$ **         All Rights reserved
$ **
$ **
$ **     There are two types of messages in this file:
$ **
$ **     1) Messages that appear in printed output
$ **        These messages are the default and they should all be localized.
$ **        These messages are marked with LOCALIZE THESE MESSAGES.
$ **
$ **     2) Messages that should not be localized.
$ **        These messages are marked with DO NOT LOCALIZE THESE MESSAGES.
$ **
$ *****************************************************************************
$ **
$ **
$ **  ------------------------- MODIFICATION RECORD --------------------------
$ *   Major Mods, 7/11/94 for dthelpprint
$ *
$ **  ----------------------- MODIFICATION RECORD END ------------------------
$ *****************************************************************************
$ *************************************<+>*************************************

$ Use the double quote char around all messages
$quote "

$set 2
$ *****  Module: PrintUtil.c *****
$ 

$set 3
$ *****  Module: PrintTopics.c *****
$ 
$ ** LOCALIZE THESE MESSAGES **
1 "%s Error: helpType is topic, but no helpVolume specified.\n"
2 "%s Error: unable to locate help volume %s\n"
3 "%s Error: problem processing help volume %s\n"
4 "%s Error: memory allocation failed\n"
5 "%s Error: unable to get topic information:\nvolume %s, locationId %s\n"
6 "%s Error: unable to open temporary file %s\n"
$ String used to replace $SECTNUM when printing the index section
10 "Index"
$ String used to replace $SECTNUM when printing the table of contents section
11 "Table of Contents"
$ ** DO NOT LOCALIZE THESE MESSAGES **
$ index entry, page number
20 "%s, %d\n"
$ index subentry, page number
21 "     %s, %d\n"
$ location ID associated with the index in the TOC maintained by dthelpprint
30 "__GENERATED-INDEX"
$ string used by can't get a topic title associated with an index subentry
31 ""

$set 4
$ *****  Module: PrintManStrFile.c *****
$ 
$ ** LOCALIZE THESE MESSAGES **
1 "%s Error: helpType is string, but no stringData specified.\n"
2 "%s Error: helpType is dynamic string, but no stringData specified.\n"
3 "%s Error: helpType is man page, but no manPage specified.\n"
4 "%s Error: helpType is file, but no helpFile specified.\n"
5 "%s Error: memory allocation failed\n"

$set 5
$ *****  Module: Initialize.c *****
$ 
$ ** LOCALIZE THESE MESSAGES **
$ Messages 1 to 29: dthelpprint usage message
$ **DO NOT LOCALIZE** the command line option names (e.g. -copies).
$ **DO LOCALIZE** the option argument (e.g. number) and description.
1 "dthelpprint - Print program for Help\n\n"
2 "Usage: dthelpprint [options]\n"
3 "Options controlling how to print:\n"
4 "\t-printer printername  printer to use\n"
5 "\t-copies number        number of copies to print\n"
6 "\t-outputFile filename  write output to this file\n"
7 "\t-paperSize size       format content to this paper size\n"
$ DO NOT LOCALIZE THE PAPER SIZE NAMES help_papersize_xxx
8 "\t\tsize = { help_papersize_letter|help_papersize_legal|\n"
9 "\t\t         help_papersize_executive|help_papersize_a4|help_papersize_b5}\n"
10 "\t-display displayname  display from which to get resources\n"
11 "\t-name                 program name used when getting resources\n"
12 "\t-class                class name used when getting resources\n"
13 "\t-xrm resourcestring   additional resources\n"
14 "Options controlling what to print:\n"
15 "\t-helpType type        type of Help data\n"
16 "\t\ttype = 0 (help volume), 1 (string), 2 (man page), 3 (help file)\n"
17 "\t-helpVolume volume    full path of help volume file\n"
18 "\t-locationId location  name of Help topic in the volume\n"
19 "\t-all                  print all topics, toc, & index in the help volume\n"
20 "\t-sub                  print topic locationId and all subtopics\n"
21 "\t-one                  print topic locationId\n"
22 "\t-toc                  print help volume table of contents\n"
23 "\t-index                print help volume index\n"
24 "\t-frontMatter          print help volume front matter\n"
25 "\t-manPage manpagename  name of man page\n"
26 "\t-stringData string    Help text to print\n"
27 "\t-helpFile filename    file containing Help text\n"
28 "\t-jobTitle title       title string for print job\n"
29 "\t-topicTitle title     title string for Help text\n"
$ Warning and error messages
$ Do NOT localize the default size name help_papersize_letter
40 "%s Warning: Illegal paper size '%s'.  help_papersize_letter used.\n"
41 "%s Warning: Missing paper size, height, or width value.  help_papersize_letter used.\n"
42 "%s Warning: Unable to open display %s\n"
45 "Error: unable to allocate memory for temporary file\n"
$ ** DO NOT LOCALIZE THESE MESSAGES **
$ arg order: directory prefix processid filecnt suffix
50 "%1$s/%2$s%3$d_%4$d%5$s"


$set 6
$ *****  Module: Main.c *****
$ 
$ ** LOCALIZE THESE MESSAGES **
1 "%s Error: Illegal helpType %d.\n"



