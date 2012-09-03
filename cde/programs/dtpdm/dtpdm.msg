$ *************************************<+>*************************************
$ *****************************************************************************
$ **
$ **  File:        dtpdm.msg
$ **
$ **  Project:     DT
$ **
$ **  Description:
$ **  -----------
$ **  This file is the source for the message catalog for the Dt Print
$ **  Dialog Manager.
$ **
$ **
$ *****************************************************************************
$ **
$ ** (c) Copyright 1996 Digital Equipment Corporation.
$ ** (c) Copyright 1996 Hewlett-Packard Company.
$ ** (c) Copyright 1996 International Business Machines Corp.
$ ** (c) Copyright 1996 Sun Microsystems, Inc.
$ ** (c) Copyright 1996 Novell, Inc. 
$ ** (c) Copyright 1996 FUJITSU LIMITED.
$ ** (c) Copyright 1996 Hitachi.
$ **
$ *****************************************************************************
$ **
$ **
$ **  $XConsortium: dtpdm.msg /main/6 1996/08/12 18:43:25 cde-hp $
$ *****************************************************************************
$ *****************************************************************************

$set 1
$ *************************************************************
$ ** 							     ** 
$ ** DtPdm OID messages 				     **
$ ** 							     ** 
$ *************************************************************

$    /* NL_COMMENT
$	Attention Translator:
$	
$ 	Message numbers 1-4 appear in the page orientation option menu
$ 	and indicate how the resulting printed page should be
$ 	oriented.
$ 
$     */
1 Portrait
2 Landscape
3 Reverse Portrait
4 Reverse Landscape
$    /* NL_COMMENT
$	Attention Translator:
$	
$ 	Message numbers 5-7 appear in the page sides option menu
$ 	and indicate which sides of the page should be printed.
$ 	
$	Simplex ->  print one side only
$	Duplex  ->  print both sides such that the resulting output is
$		    intended for the reader to turn pages in a
$		    horizontal direction; left to right (like an
$		    English book) or right to left.
$	Tumble  ->  print both sides such that the resulting output is
$		    intended for the reader to turn pages in a
$		    vertical direction; top to bottom or bottom to top
$ 
$     */
5 Simplex
6 Duplex
7 Tumble
$    /* NL_COMMENT
$	Attention Translator:
$	
$ 	Message numbers 8-65 appear in the page sizes list box
$ 	and indicate the size of the printed page.
$ 	
$     */
8 ISO A0
9 ISO A1
10 ISO A2
11 ISO A3
12 ISO A4
13 ISO A5
14 ISO A6
15 ISO A7
16 ISO A8
17 ISO A9
18 ISO A10
19 ISO B0
20 ISO B1
21 ISO B2
22 ISO B3
23 ISO B4
24 ISO B5
25 ISO B6
26 ISO B7
27 ISO B8
28 ISO B9
29 ISO B10
30 Letter
31 Legal
32 Executive
33 Folio
34 Invoice
35 Ledger
36 Quarto
37 ISO C3
38 ISO C4
39 ISO C5
40 ISO C6
41 ISO Designated Long
42 10x13 Envelope
43 9x12 Envelope
44 #10 Envelope
45 7x9 Envelope
46 9x11 Envelope
47 10x14 Envelope
48 #9 Envelope
49 Monarch Envelope
50 A
51 B
52 C
53 D
54 E
55 JIS B0
56 JIS B1
57 JIS B2
58 JIS B3
59 JIS B4
60 JIS B5
61 JIS B6
62 JIS B7
63 JIS B8
64 JIS B9
65 JIS B10
$    /* NL_COMMENT
$	Attention Translator:
$	
$ 	Message numbers 66-73 appear in the tray options menu
$ 	and indicate the printer tray from which media will be
$	obtained.
$ 	
$     */
66 Top
67 Middle
68 Bottom
69 Envelope
70 Manual
71 Large Capacity
72 Main
73 Side


$set 2
$ *************************************************************
$ ** 							     ** 
$ ** DtPdm general messages 				     **
$ ** 							     ** 
$ *************************************************************
$    /* NL_COMMENT
$	Attention Translator:
$	
$       Message number 1 is an error message indicating that the X
$       window system does not support the current locale.
$ 	
$     */
1 Locale unsupported by X
$    /* NL_COMMENT
$	Attention Translator:
$	
$       Message number 2 appears in the tray options menu and
$       indicates no preference as to the printer tray from which
$       media will be obtained.
$ 	
$     */
2 Auto-select
$    /* NL_COMMENT
$	Attention Translator:
$	
$       Message number 3 appears in the page size list box and
$       indicates that the default page size will be used.
$ 	
$     */
3 Default
$    /* NL_COMMENT
$	Attention Translator:
$	
$       Message numbers 4-6 appear ultimately in the Dt PDM Daemon log
$       file, and indicate that a syntax error was found parsing an
$       attribute. Do not localize the words "medium-source-sizes",
$       "input-trays-medium" or "document format".
$ 	
$     */
4 Syntax error parsing medium-source-sizes
5 Syntax error parsing input-trays-medium
6 Syntax error parsing document format
