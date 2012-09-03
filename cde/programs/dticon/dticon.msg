$ $TOG: dticon.msg /main/4 1999/09/17 12:17:03 mgreess $
$ *************************************<+>*************************************
$ *****************************************************************************
$ **
$ **  File:        dticon.msg
$ **
$ **  Project:     HP DT Icon Editor
$ **
$ **  Description:
$ **  -----------
$ **  This file is the source for the message catalog for dticon
$ **
$ **
$ *****************************************************************************
$ **
$ **     (c) Copyright 1992, 1993, 1994 Hewlett-Packard Company
$ **     (c) Copyright 1993, 1994 International Business Machines Corp.
$ **     (c) Copyright 1993, 1994 Sun Microsystems, Inc.
$ **     (c) Copyright 1993, 1994 Unix System Labs, Inc., a subsidiary of
$ **         Novell, Inc.
$ **
$ **     All Rights reserved
$ **
$ **
$ *****************************************************************************
$ **

$set 2
$ *****************************************************************************
$ ** _DtMessage set for fileIODialog.c
$ **
$ ** messages 10-16 are for the FileSelectionBox widget (default labels)
$ *****************************************************************************

2 Icon Editor - File I/O
4 Icon Editor - File I/O
6 Icon Editor - Save As
8 Icon Editor - Open File
10 Update
12 Folders
14 Files
16 Enter file name:
17 Enter file name:
18 Update
20 UNTITLED
22 Open
24 Save

$set 4
$ *****************************************************************************
$ ** _DtMessage set for newIconDialog.c
$ **
$ *****************************************************************************

2 newIconDialog
4 Icon Editor - Resize
6 OK
8 Cancel
10 Help
12 New Width:
14 New Height:

$set 6
$ *****************************************************************************
$ ** _DtMessage set for queryDialog.c
$ **
$ *****************************************************************************

2 Icon Editor - Warning
6 OK
$ ** DO NOT LOCALIZE - the user should never see this message
8 Do ya really wanna?
10 Cancel

$set 8
$ *****************************************************************************
$ ** _DtMessage set for stdErrDialog.c
$ ** _DtMessage set for DieFromToolTalkError in main.c
$ **
$ *****************************************************************************

2 Icon Editor - Error
4 Continue
$ 
$ "-NEW"
6 OK
$ 
$ "-NEW"
8 "Could not connect to ToolTalk:\n%s\nExiting ..."

$set 10
$ *****************************************************************************
$ ** _DtMessage set for utils.c
$ **
$ *****************************************************************************

$ ** DO NOT LOCALIZE messages 2-48, they appear in the errorlog only
2 Cannot initialize button icon for circle
4 Cannot initialize button icon for solid circle
6 Cannot initialize button icon for polyline
8 Cannot initialize button icon for polygon
10 Cannot initialize button icon for solid polygon
12 Cannot initialize button icon for ellipse
14 Cannot initialize button icon for solid ellipse
16 Cannot initialize button icon for eraser
18 Cannot initialize button icon for line
20 Cannot initialize button icon for flood
22 Cannot initialize button icon for point
24 Cannot initialize button icon for rectangle
26 Cannot initialize button icon for solid rectangle
28 Cannot initialize button icon for select
30 Unable to parse static color no.
32 Unable to allocate static color no.
38 No memory(0)
40 No memory(1)
42 No memory(2)
44 No memory(3)
46 No memory(4)
48 No memory(5)

50 Cannot initialize application icon storage
52 Invalid use of the '-f' parameter
54 Invalid use of the '-i' parameter
56 Invalid use of the '-session' parameter
58 Invalid size format
60 Screen image capture failed
62 Operation failed.\nColormap is full
64 Invalid dimension parameter

66 (UNTITLED)

$set 12
$ *****************************************************************************
$ ** _DtMessage set for dtIconShell.c
$ **
$ *****************************************************************************

1 Icon Editor
2 2 Color
3 Icon Editor - (UNTITLED)
4 Welcome to Icon Editor.
6 Fill Solids
8 Static Colors
10 Dynamic Colors
12 Static Grays
14 Foreground
16 Background
18 TopShadow
20 BottomShadow
22 Select
24 Transparent

$ ** message 26-198 are menu items with corresponding mnemonics
$ ** (for set grouping information, see the comment at the end-of-file)
26 N
28 New
30 O
32 Open...
34 S
36 Save
38 A
40 Save As...
42 x
44 Exit
46 F
48 File
50 U
52 Undo
54 t
56 Cut Area
58 C
60 Copy Area
62 P
64 Paste Area
68 L
70 Left
72 R
74 Right
76 R
78 Rotate Area
82 V
84 Vertical
86 H
88 Horizontal
90 F
92 Flip Area
94 S
96 Scale Area
98 z
100 Resize Icon...
102 A
104 Add Hotspot
106 D
108 Delete Hotspot
110 G
112 Grab Screen Image
114 e
116 Clear Icon
118 E
120 Edit
122 V
124 Visible Grid
128 B
130 XBM
134 P
136 XPM
138 O
140 Output Format
144 2x
146 3x
148 4x
150 5x
152 6x
154 8x
156 10x
158 12x
160 M
162 Magnification
164 O
166 Options
168 v
170 Overview...
172 T
174 Tasks...
176 R
178 Reference...
180 O
182 On Item
188 U
190 Using Help...
192 A
194 About the Icon Editor...
196 H
198 Help

$ ** message 200-are used in the "instruction" line for current drawing mode
200 Current Edit Mode:
202 (Filled)
204 Pencil
206 Flood
208 Line
210 Polyline
212 Rectangle
214 Polygon
216 Circle
218 Ellipse
220 Erase
222 Select
224 Invalid

$set 14
$ *****************************************************************************
$ ** _DtMessage set for help.c
$ **
$ *****************************************************************************

2 Icon Editor - Help
4 You must select an item\nwithin the Icon Editor.

$set 16
$ *****************************************************************************
$ ** _DtMessage set for process.c and fileIO.c
$ **
$ *****************************************************************************

2 The file cannot be accessed\nor contains invalid data
4 Unable to write data to file
6 This feature is not available yet
8 There is no previous\nimage available
10 Invalid width and/or\nheight specified
12 No area has been selected
14 No area was previously\ncut or copied
16 The current icon has not been saved.\n\nYour changes will be lost.
20 Check file permissions
22 Couldn't restore session from file
24 Couldn't save session to file
25 File already exists.\n\nOK to overwrite the file?

$set 18
$ *****************************************************************************
$ ** _DtMessage set for event.c
$ **
$ *****************************************************************************
2 Grabbed Image Size = %dx%d

$ *****************************************************************************
$ ** End of _DtMessage Catalog File
$ ** (Comments only below this line)
$ *****************************************************************************
$ **
$ *****************************************************************************
$ ** Unique mnemonic groupings from set 12
$ *****************************************************************************
$ **
$ ** Unique Set 1:  (Main Menu Bar)
$ **
$ **   46 F
$ **   48 File
$ **
$ **   118 E
$ **   120 Edit
$ **
$ **   164 O
$ **   166 Options
$ **
$ **   196 H
$ **   198 Help
$ **
$ **
$ ** Unique Set 2:  (File Menu)
$ **
$ **   26 N
$ **   28 New
$ **   30 O
$ **   32 Open...
$ **   34 S
$ **   36 Save
$ **   38 A
$ **   40 Save As...
$ **   42 x
$ **   44 Exit
$ **
$ **
$ ** Unique Set 3:  (Edit Menu)
$ **
$ **   50 U
$ **   52 Undo
$ **   54 t
$ **   56 Cut Area
$ **   58 C
$ **   60 Copy Area
$ **   62 P
$ **   64 Paste Area
$ **
$ **        Unique Set 3a:  (Rotate Sub-Menu)
$ **        68 L
$ **        70 Left
$ **        72 R
$ **        74 Right
$ **   76 R
$ **   78 Rotate Area
$ **
$ **        Unique Set 3b:  (Flip Sub-Menu)
$ **        82 V
$ **        84 Vertical
$ **        86 H
$ **        88 Horizontal
$ **   90 F
$ **   92 Flip Area
$ **   94 S
$ **   96 Scale Area
$ **   98 z
$ **   100 Resize Icon...
$ **   102 A
$ **   104 Add Hot Spot
$ **   106 D
$ **   108 Delete Hot Spot
$ **   110 G
$ **   112 Grab Screen Image
$ **   114 e
$ **   116 Clear Icon
$ **
$ **
$ ** Unique Set 4:  (Options Menu)
$ **
$ **   122 V
$ **   124 Visible Grid
$ **
$ **        Unique Set 4a:  (Output Sub-Menu)
$ **        128 B
$ **        130 XBM
$ **        134 P
$ **        136 XPM
$ **   138 O
$ **   140 Output Format
$ **   160 M
$ **   162 Magnification
$ **
$ **
$ ** Unique Set 5:  (Help Menu)
$ **
$ **   168 I
$ **   170 Introduction...
$ **   172 T
$ **   174 Tasks...
$ **   176 R
$ **   178 Reference...
$ **   180 O
$ **   182 On Item
$ **   188 U
$ **   190 Using Help...
$ **   192 V
$ **   194 Version...
$ **
