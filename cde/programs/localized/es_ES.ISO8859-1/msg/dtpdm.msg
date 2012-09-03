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
$ **  $XConsortium: dtpdm.msg /main/2 1996/10/19 14:46:34 rws $
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
1 Vertical
2 Horizontal
3 Vertical invertido
4 Horizontal invertido
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
5 Una cara
6 Dos caras
7 Invertir segunda cara
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
30 Carta
31 Oficio
32 Ejecutivo
33 Folio
34 Factura
35 Libro mayor
36 Libro en cuarto
37 ISO C3
38 ISO C4
39 ISO C5
40 ISO C6
41 Designado largo ISO
42 Sobre 10x13
43 Sobre 9x12
44 Sobre núm. 10
45 Sobre 7x9
46 Sobre 9x11
47 Sobre 10x14
48 Sobre núm. 9
49 Sobre monarca
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
66 Superior
67 Medio
68 Inferior
69 Sobre
70 Manual
71 Gran capacidad
72 Principal
73 Lateral


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
1 La ubicación no está soportada por X
$    /* NL_COMMENT
$	Attention Translator:
$	
$       Message number 2 appears in the tray options menu and
$       indicates no preference as to the printer tray from which
$       media will be obtained.
$ 	
$     */
2 Selección automática
$    /* NL_COMMENT
$	Attention Translator:
$	
$       Message number 3 appears in the page size list box and
$       indicates that the default page size will be used.
$ 	
$     */
3 Predeterminado
$    /* NL_COMMENT
$	Attention Translator:
$	
$       Message numbers 4-6 appear ultimately in the Dt PDM Daemon log
$       file, and indicate that a syntax error was found parsing an
$       attribute. Do not localize the words "medium-source-sizes",
$       "input-trays-medium" or "document format".
$ 	
$     */
4 Error de sintaxis al efectuar el análisis sintáctico de tamaños de medio
5 Error de sintaxis al efectuar el análisis sintáctico de bandeja alimentación de medio
6 Error de sintaxis al efectuar el análisis sintáctico del formato del documento
