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
$ **  $XConsortium: dtpdm.msg /main/2 1996/12/01 23:39:03 cde-hit $
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
$ "NEW-"
1 ポートレイト
$ "NEW-"
2 ランドスケープ
$ "NEW-"
3 ポートレイトの反転
$ "NEW-"
4 ランドスケープの反転
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
$ "NEW-"
5 シンプレックス
$ "NEW-"
6 デュプレックス
$ "NEW-"
7 タンブル
$    /* NL_COMMENT
$	Attention Translator:
$	
$ 	Message numbers 8-65 appear in the page sizes list box
$ 	and indicate the size of the printed page.
$ 	
$     */
$ "NEW-"
8 ISO A0
$ "NEW-"
9 ISO A1
$ "NEW-"
10 ISO A2
$ "NEW-"
11 ISO A3
$ "NEW-"
12 ISO A4
$ "NEW-"
13 ISO A5
$ "NEW-"
14 ISO A6
$ "NEW-"
15 ISO A7
$ "NEW-"
16 ISO A8
$ "NEW-"
17 ISO A9
$ "NEW-"
18 ISO A10
$ "NEW-"
19 ISO B0
$ "NEW-"
20 ISO B1
$ "NEW-"
21 ISO B2
$ "NEW-"
22 ISO B3
$ "NEW-"
23 ISO B4
$ "NEW-"
24 ISO B5
$ "NEW-"
25 ISO B6
$ "NEW-"
26 ISO B7
$ "NEW-"
27 ISO B8
$ "NEW-"
28 ISO B9
$ "NEW-"
29 ISO B10
$ "NEW-"
30 レター
$ "NEW-"
31 リーガル
$ "NEW-"
32 エグゼクティブ
$ "NEW-"
33 フォリオ
$ "NEW-"
34 インボイス
$ "NEW-"
35 レッジャー
$ "NEW-"
36 クァトロ
$ "NEW-"
37 ISO C3
$ "NEW-"
38 ISO C4
$ "NEW-"
39 ISO C5
$ "NEW-"
40 ISO C6
$ "NEW-"
41 ISO 指定長
$ "NEW-"
42 10x13 エンベロープ
$ "NEW-"
43 9x12 エンベロープ
$ "NEW-"
44 #10 エンベロープ
$ "NEW-"
45 7x9 エンベロープ
$ "NEW-"
46 9x11 エンベロープ
$ "NEW-"
47 10x14 エンベロープ
$ "NEW-"
48 #9 エンベロープ
$ "NEW-"
49 Monarch エンベロープ
$ "NEW-"
50 A
$ "NEW-"
51 B
$ "NEW-"
52 C
$ "NEW-"
53 D
$ "NEW-"
54 E
$ "NEW-"
55 JIS B0
$ "NEW-"
56 JIS B1
$ "NEW-"
57 JIS B2
$ "NEW-"
58 JIS B3
$ "NEW-"
59 JIS B4
$ "NEW-"
60 JIS B5
$ "NEW-"
61 JIS B6
$ "NEW-"
62 JIS B7
$ "NEW-"
63 JIS B8
$ "NEW-"
64 JIS B9
$ "NEW-"
65 JIS B10
$    /* NL_COMMENT
$	Attention Translator:
$	
$ 	Message numbers 66-73 appear in the tray options menu
$ 	and indicate the printer tray from which media will be
$	obtained.
$ 	
$     */
$ "NEW-"
66 トップ
$ "NEW-"
67 ミドル
$ "NEW-"
68 ボトム
$ "NEW-"
69 エンベロープ
$ "NEW-"
70 マニュアル
$ "NEW-"
71 ラージ・キャパシティ
$ "NEW-"
72 メイン
$ "NEW-"
73 サイド


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
$ "NEW-"
1 X でサポートしていないロケール
$    /* NL_COMMENT
$	Attention Translator:
$	
$       Message number 2 appears in the tray options menu and
$       indicates no preference as to the printer tray from which
$       media will be obtained.
$ 	
$     */
$ "NEW-"
2 自動選択
$    /* NL_COMMENT
$	Attention Translator:
$	
$       Message number 3 appears in the page size list box and
$       indicates that the default page size will be used.
$ 	
$     */
$ "NEW-"
3 デフォルト
$    /* NL_COMMENT
$	Attention Translator:
$	
$       Message numbers 4-6 appear ultimately in the Dt PDM Daemon log
$       file, and indicate that a syntax error was found parsing an
$       attribute. Do not localize the words "medium-source-sizes",
$       "input-trays-medium" or "document format".
$ 	
$     */
$ "NEW-"
4 medium-source-size 解析時の構文エラー
$ "NEW-"
5 input-tray-medium 解析時の構文エラー
$ "NEW-"
6 ドキュメント・フォーマット解析時の構文エラー
