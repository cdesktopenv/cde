$ $XConsortium: dtudcfonted.msg
$ *************************************<+>*************************************
$ *****************************************************************************
$ **
$ **  File:        dtudcfonted.msg
$ **
$ **  Project:     Fujitsu DT UDC Font Editor
$ **
$ **  Description:
$ **  -----------
$ **  This file is the source for the message catalog for dtudcfonted
$ **
$ **
$ *****************************************************************************
$ **
$ **     (c) Copyright 1996 Fujitsu Ltd.
$ **
$set 2
$ *****************************************************************************
$ ** _DtMessage set for ufont.c
$ *****************************************************************************

$ This is footer message when registration of the character.
2 Registration processing inside.

$ Message4-8 are error messages when registration of the character.
4 Failed in the registration of the character.
6 Illegal code.
8 Because the memory allocation cannot be done, it is not possible to add.

$ Message10-16 are error messages when copying of the character.
10 There is no character of the specified code.
12 The mistake is found in the specified copy origin code.
14 The mistake is found in the specified copy target code.
16 There is no character in the specified area.

$ This is footer message.
18 Please wait for a while.

$ This is error message when copying of the character.
20 No copy origin code file is specified.

$ Message22-24 are error message of user defined character
22 Informations of user defined character cannot collect.\nThere is no font specified.
24 Informations of user defined character cannot collect.\nThere are plural fonts specified.

$ UDC is User Defined Character.
26 The specified font do not have any UDCs areas.

28 There are no memories any more.
30 The information of UDCs area is illegal.
32 The font file cannot read.\nPattern area was broken.

$ Dtudcfonted is name of this application.
34 Dtudcfonted cannot edit proportional character font.

36 There are unsaved edits.\nDo you want to save them before opening?
38 There are unsaved edits.\nDo you want to save them before exiting?
40 When the copy processing is done, the glyph\nimage within the following ranges is changed.\nDo you want to do the copy processing?\n\n
42 No write permission for this font. You should change the file access mode before saving.\n

$ There are user-visible labels in font-open dialog.
100 Select Items
102 -        CodeSet:
104 -          Style:
106 -          Width:
108 -         Height:
110 Open
112 Quit

$ There are user-visible labels in menubar.
114 Font
116 Edit
118 Character
120 Information

$ There are user-visible labels in File menu.
122 Open...
124 Save
126 Exit

$ There are user-visible labels in Edit menu.
$ Message135 is copy of cut&copy
128 Clear
130 Set
132 Reverse
134 Cut
135 Copy
136 Paste
138 Rotate
140 Vertical Roll
142 Horizontal Roll
144 Undo

$ There are user-visible labels in Character menu.
$ Message148 is copy of character
146 Add/Delete...
148 Copy...

$ There are user-visible labels in Information menu.
150 XLFD name...
152 Code Area...

$ There are user-visible labels in add/delete dialog.
154 Add
156 Delete
158 Cancel

$ There are user-visible labels in copy dialog.
$ Message166 is label of execute button
160 Code(hex):
162 Copy origin code(hex):
164 Copy target code(hex):
166 Copy
168 Overlay
170 OK

$ There are user-visible labels in unsaved edits.
172 Save
174 Do not Save
176 Save and Exit
178 Do not Save and Exit

$ There are user-visible labels in delete notice dialog.
$ This format is `Message180 (?? Message182)' and `Message184 (?? Message186)'.
$ These Message180 and Message182 are used in only one character.
$ These Message184 and Message186 are used in plural characters.
180 The following character will be deleted.
182 character
184 The following characters will be deleted.
186 characters
188 Register completed
190 Code :
192 Code range :

$ There are footer message label.
194 Message :

$ There are user-visible labels in code dialog.
196 code...
198 Page up
200 Page down
202 Apply

$ There are title.
204 dtudcfonted - Question
206 dtudcfonted - Warning
208 dtudcfonted - Error
210 dtudcfonted - Copy
212 dtudcfonted - Open
214 dtudcfonted - Add/Delete
216 dtudcfonted - XLFD
218 dtudcfonted - CodeArea

$ There are information message.
230 File name
232 XLFD name
234 Code area

$ There are error message of libfal.
300 Fatal error occurred.
301 Cannot open the font file.
302 Cannot read the font file.
303 There is no more memory.
304 Fatal error occurred.
305 The specified font file does not exist.
306 This font is not a pcf or snf font.
307 Cannot open fonts.list file.
308 The format of fonts.list file is illegal.
309 The descriptions of the fonts.list file are incorrect.
310 The format of fonts.list file is illegal.
311 Cannot open fonts.dir file.
312 Cannot read fonts.dir file.
313 Cannot read font properties.
314 Cannot get FONT property.
315 Cannot get FAMILY_NAME property.
316 This font file is already opened by other application.
317 Cannot lock font file.
318 Cannot unlock font file.
319 Cannot get lock information from the font file.
320 Cannot find the specified font file.
321 Cannot read NLS database.
322 Cannot get charset names from NLS database.
323 Charset name not defined in NLS database.
324 The specified font has not been opened.
325 Fatal error occurred.
