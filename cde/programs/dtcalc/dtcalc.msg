$ *************************************<+>*************************************
$ 
$   COMPONENT_NAME: desktop
$ 
$   FUNCTIONS: Abs
$		Bsp
$		Calculator
$		Ctrm
$		Ddb
$		END
$		Exp
$		Frac
$		Fv
$		Int
$		Pmt
$		Pv
$		Sln
$		Syd
$		YR
$		character
$		clrgs
$		identification
$		key
$		sign
$ 
$   ORIGINS: 27,118,119,120,121
$ 
$   This module contains IBM CONFIDENTIAL code. -- (IBM
$   Confidential Restricted when combined with the aggregated
$   modules for this product)
$   OBJECT CODE ONLY SOURCE MATERIALS
$ 
$   (C) COPYRIGHT International Business Machines Corp. 1995
$   All Rights Reserved
$   US Government Users Restricted Rights - Use, duplication or
$   disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
$ 
$ *************************************<+>*************************************
$ *****************************************************************************
$ **
$ **  File:        dtcalc.msg
$ **
$ **  Project:     CDE dtcalc
$ **
$ **  Description:
$ **  -----------
$ **  This file is the source for the message catalog for dtcalc
$ **  Any additional messages are to be added to this file by hand,
$ **  and the associated index number placed in the code.
$ **
$ *****************************************************************************
$ **
$ **    (c) Copyright 1993, 1994 Hewlett-Packard Company
$ **    (c) Copyright 1993, 1994 International Business Machines Corp.
$ **    (c) Copyright 1993, 1994 Sun Microsystems, Inc.
$ **    (c) Copyright 1993, 1994 Novell, Inc.
$ **
$ **
$ *****************************************************************************
$ *************************************<+>*************************************

$ *************************************<L>*************************************
$ **  -----------------GENERAL LOCALIZATION NOTES SECTION---------------------
$ * Comment lines begin with a $ except $set which indicates the start of a
$ * new set.
$ *
$ * Do not delete any comments; you may add comments for your use.
$ *
$ **  ----------------GENERAL LOCALIZATION NOTES SECTION END------------------
$ *************************************<L>*************************************

$ *****************************************************************************
$ 
$ ***** NOTE FOR MESSAGE CATALOG TRANSLATORS *****
$ 
$ There may be three types of messages in this file:
$ 
$ 1. Messages that appear in dialogs or are displayed to the user.
$ 
$     These messages are the default and they should ALL BE LOCALIZED.
$     Note that these messages do NOT have any identification (see the
$     comments for type 2 and 3 below).
$ 
$ 2. Messages that only appear in the DT error log file ($HOME?.dt/errorlog).
$ 
$     The localization of these messages is OPTIONAL.  These messages are
$     identified by the following:
$ 
$     MESSAGES xx-yy WILL ONLY APPEAR IN THE DT ERRORLOG FILE
$ 
$ 3. Messages that should not be localized.
$ 
$     These messages are identified by the following:
$ 
$     DO NOT TRANSLATE or CHANGE or LOCALIZE MESSAGES xx-yy
$ 
$ ***** END (NOTE FOR MESSAGE CATALOG TRANSLATORS) *****
$ 
$ *****************************************************************************

$set 2
$ ****** Module: motif.c ****(developer comment only)********
$ 
$ Messages 1 - 4 are used by the "numeric base" option button.  Bin is 
$  short for Binary, Oct is short for Octal, Dec is short for Decimal and 
$  Hex is short for Hexidecimal.  Keep these to 3 to 4 characters!
$ 
1 Bin
2 Oct
3 Dec
4 Hex
$ 
$ Messages 5 - 7 are used by the "display mode" option button. Eng is an
$  abbreviation for Engineering, Fix is an abbreviation for Fixed, and
$  Sci is an abbreviation for Scientific.  Keep these to 3 characters.
$ 
5 Eng
6 Fix
7 Sci
$ 
$ NOTE: no message 8
$ 
$ Messages 9 - 11 refer to the three modes the Calculator can operate in.
$  They are listed in the "Mode" option button.
$ 
9 Financial
10 Logical
11 Scientific
$ 
$ Message 12 - This is the title to the Calculator.
$ 
12 Calculator
$ 
$ Messages 13 - 30 are the labels in the menubar pulldown menus and the
$  mnemonic's (the letter that gets underlined for keyboard access) that 
$  go with them. 
$ 
$ Message 13 is the mnemonic for Message 14
$ 
13 O
14 Options
15 Memory Registers
16 ASCII Convert
$ 
$ Message 17 is the mnemonic for Message 18
$ 
17 H
18 Help
$ 
$ Message 19 is the mnemonic for Message 20
$ 
19 v
20 Overview
$ 
$ Message 21 is the mnemonic for Message 22
$ 
21 T
22 Tasks
$ 
$ Message 23 is the mnemonic for Message 24
$ 
23 R
24 Reference
$ 
$ Message 25 is the mnemonic for Message 26
$ 
25 O
26 On Item
$ 
$ Message 27 is the mnemonic for Message 28
$ 
27 U
28 Using Help
$ 
$ Message 29 is the mnemonic for Message 30
$ 
29 A
30 About Calculator
$ 
$ Message 31 is an simple error message.
$ 
31 Could not open display.\n
$ 
$ Messages 32 and 33 are button labels for dialog boxes.
$ 
32 Close
33 Apply
$ 
$ Message 34 is an simple error message.
$ 
34 Could not open the session file.
$ 
$ Message 35 - Gets put in the "Functions" popup when no functions have been
$  defined by the users.
$ 
35 No Functions Defined
$ 
$ Message 36 and 37 are titles to two types of dialogs.
$ 
36 Continue Notice
37 Confirmation Notice
$ 
$ Message 38 and 39 are another label for the Help pulldown menu.
$ Message 38 is the mnemonic for Message 39
$ 
38 C
39 Table of Contents
$ 
$ 40 -  This is the mnemonic for the 'Memory Registers...'  option on 
$  the Option menu pulldown.  (message 15 set 2)
$ 
40 R
$ 
$ 41 -  This is the mnemonic for the 'ASCII Convert...' option on the Option
$   menu pulldown. (message 16 set 2)
$ 
41 A
$ 
$ 42 -  This is the mnemonic for the 'Enter Function...' option on the Option
$   menu pulldown.  (message 143 set 3)
$ 
42 F
$ 
$ 43 -  This is the mnemonic for the 'Enter Constant...' option on the Option
$   menu pulldown. (message 142 set 3)
$ 
43 C
$ 
$ 44 -  This is the mnemonic for the 'Exit' option on the Option
$   menu pulldown. (message 403 set 3)
$ 
44 x
$ 
$ 45 -  This is the mnemonic for the 'Financial Registers' option on the Option
$   menu pulldown. (message 138 set 3)
$ 
45 g
$ 
$ 46 - This is the title for the popup menu.
$ 
46 Calculator Popup
$ 
$ Used as the title, In Asian languages labels need to be different
$  from the menu label.
$ 
47 Help
$ 
$set 3
$ ****** Module: text.c ****(developer comment only)********
$ 
$ NOTE: there is no Message 1 - 4.
$ 
$ Messages 5-47 are what gets displayed on each of the calculator keys.
$ 
5 Functions
6 Constants
7 Keys
$ 
$ Message 8 - Int is short for integer.
$ 
8 Int
$ 
$ Message 9 - Frac is short for fraction.
$ 
9 Frac
$ 
$ Message 10 - Abs is short for absolute value.
$ 
10 Abs
$ 
$ Message 11 - Exp is short for exponent.
$ 
11 Exp
12 Accuracy
13 Store
14 Recall
15 Exchange
16 1/x
17 x^2
18 %
$ 
$ Message 19 - Sqrt is short for square root.
$ 
19 Sqrt
20 (
21 )
$ 
$ Message 22 - Bsp is short for backspace.
$ 
22 Bsp
$ 
$ Message 23 - Clr is short for clear.
$ 
23 Clr
24 D
25 E
26 F
27 +/-
28 A
29 B
30 C
31 X
32 7
33 8
34 9
35 /
36 4
37 5
38 6
39 -
40 1
41 2
42 3
43 +
44 0
45 .
46 =
47 Quit
$ 
$ Messages 48-51 tell the user what mode they are in.
$ 
48 Base
49 Disp
50 Mode
51 Trig
$ 
$ Messages 52-94 are what gets put on the keys when the user presses the 'Keys'
$  key.  These are the keyboard keys which provide the given function. These are
$  also known as keyboard accelerators.  These keys directly correspond to
$  Messages 5-47. (i.e 5 maps to 52, 6 maps to 53, etc.).
$ 
52 F - Funcs
53 # - Cnsts
54 k - Keys
55 I - Int
$ Message 56 is "Control f" 
56 ^f - Frac
$ Message 57 is "Control u"
57 ^u Abs
58 E - Exp
59 A - Acc
60 S - Sto
61 R - Rcl
62 X - Xchg
63 r - 1/x
64 @ - x^2
65 %
66 s - Sqrt
67 (
68 )
$ 
$ Message 69 is "Control h"
$ 
69 ^h - Bsp
$ 
$ Message 70 is "Delete"
$ 
70 del - Clr
71 d
72 e
73 f
74 C - +/-
75 a
76 b
77 c
78 x
79 7
80 8
81 9
82 /
83 4
84 5
85 6
86 -
87 1
88 2
89 3
90 +
91 0
92 .
93 =
94 q - Quit
$ See COSE cmvc 7910
$ Messages 95-98 are keyboard acclerators for Messages 48-51 
95 B
96 D
97 M
98 T
$ 
$ NOTE: Messages 99 - 106 aren't used.
$ 
$ Messages 107-118 are dialog/popup titles.
$ 
107 Accuracy
108 Numeric base
109 Constants
110 Exchange
111 Functions
113 Mode
114 Display type
115 Retrieve
116 Right shift
117 Store
118 Trigonometric type
$ 
$ Messages 119-128 are the messages describing the Constants listed when the
$ Constant popup is brought up.  NOTE: don't translate <=>
$ 
119 kilometers per hour <=> miles per hour.
120 square root of 2.
121 e.
122 pi.
123 centimeters <=> inch.
124 degrees in a radian.
125 2 ^ 20.
126 grams <=> ounce.
127 kilojoules <=> British thermal units.
128 cubic centimeters <=> cubic inches.
$ 
$ Messages 132-143 are labels in the dialogs.
$ 
132 Constant no:
133 Function no:
134 calculator
135 New Constant
136 New Function
137 Memory Registers
138 Financial Registers
139 Calculator properties
140 Description:
141 Value:
142 Enter Constant
143 Enter Function
$ 
$ NOTE: Message 144 - 147 aren't used.
$ 
$ Messages 148 is an error message used when the command line is used 
$  improperly.  %s tells the user what argument should come next.
$ 
148 %s\n
$ 
$ Message 149 is an error message.  %s is either the word "Function" or the
$  word "Constant" and %1d is a number 0-9.  This message gets put in a 
$  dialog when the user defines a new Function or Constant that already 
$  exists and the dialog asks the user wheither to overwrite it. (Message 380)
$ 
149 %s %1d already exists.
$ 
$ Messages 153-162 are in the accuracy popup menu for the level of accuracy.
$ 
153 0 radix places
154 1 radix places
155 2 radix places
156 3 radix places
157 4 radix places
158 5 radix places
159 6 radix places
160 7 radix places
161 8 radix places
162 9 radix places
$ 
$ Messages 163-172 are in the Sto, Rcl, Exch popup menu's listing the registers.
$ 
163 Register 0
164 Register 1
165 Register 2
166 Register 3
167 Register 4
168 Register 5
169 Register 6
170 Register 7
171 Register 8
172 Register 9
$ 
$ Messages 188-191 are the numeric base types of operation.
$ 
188 Binary
189 Octal
190 Decimal
191 Hexadecimal
$ 
$ Messages 192-194 are the display types.
$ 
192 Engineering
193 Fixed point
194 Scientific
$ 
$ Messages 195-197 are the trig. types.
$ 
195 Degrees 
196 Gradients
197 Radians
$ 
$ Messages 198-201 are the modes of operation of the calculator.
$ 
198 Basic
199 Financial
200 Logical
201 Scientific
$ 
$ Messages 203-207 are the labels in the Financial Registers dialog.
$ 
203 Term:
204 i%Yr:
205 Present Value:
206 Payment:
207 Future Value:
$ 
$ Messages 208-217 are the button labels of the financial portion of the
$ Calculator when it is in Financial mode.
$ 
208 Term
209 i%Yr
$ See COSE cmvc 7910
$ 
$ Message 210 - Pv stands for Present Value.
$ 
210 Pv
$ 
$ Message 211 - Pmt stands for Payment
$ 
211 Pmt
$ 
$ Message 212 - Fv stands for Future Value.
$ 
212 Fv
$ 
$ Message 213 means clear financial registers
$ 
213 CLRfregs
$ 
$ Message 214 means Compounding term
$ 
214 Ctrm
$ 
$ Message 215 means double-declining depreciation
$ 
215 Ddb
$ 
$ Message 216 means Straight-line depreciation
$ 
216 Sln
$ 
$ Message 217 means Sum-of-the-years-digits depreciation
$ 
217 Syd
$ 
$ Messages 218-226 are the button labels of the logical portion of the
$ Calculator when it is in Logical mode.
$ 
$ Message 218 means shift left
$ 
218 <
$ 
$ Message 219 means shift right
$ 
219 >
$ 
$ Message 220 means trunctate to the lower 16 bits.
$ 
220 &16
$ 
$ Message 221 means trunctate to the lower 32 bits.
$ 
221 &32
$ 
$ Message 222 means perform a logical OR
$ 
222 Or
$ 
$ Message 223 means perform a logical AND
$ 
223 And
$ 
$ Message 224 means perform a logical NOT
$ 
224 Not
$ 
$ Message 225 means perform a logical XOR
$ 
225 Xor
$ 
$ Message 226 means perform a logical XNOR
$ 
226 Xnor
$ 
$ Messages 227-238 are the button labels of the Scientific portion of the
$ Calculator when it is in Scientific mode. This is the default mode.
$ 
$ 
$ Message 227 - Hyp is short for Hyperbolic
$ 
227 Hyp
$ 
$ Message 228 - Inv is short for Inverse
$ 
228 Inv
229 e^x
230 10^x
231 y^x
232 x!
233 Cos
234 Sin
235 Tan
236 Ln
237 Log
$ 
$ Message 238 - Rand is short for Random
$ 
238 Rand
$ 
$ Messages 239-248 are the button labels of the financial portion of the
$ Calculator when it is in Financial mode when in "keys" mode. (i.e. button
$ acclerators).
$ 
239 T - Term
$ Message 240 is 'Control r'
240 ^r - i%Yr
$ See COSE cmvc 7910

241 p - Pv
242 P - Pmt
243 v - Fv
244 L - Clrgs
$ Message 245 is 'Control t'
245 ^t - Ctrm
$ Message 246 is 'Control d'
246 ^d - Ddb
$ Message 247 is 'Control s'
247 ^s - Sln
$ Message 248 is 'Control y'
248 ^y - Syd
$ 
$ Messages 249-257 are the button labels of the logical portion of the
$ Calculator when it is in Logical mode when in "keys" mode. (i.e. button
$ acclerators).
$ 
249 <
250 >
251 [ - &16
252 ] - &32
253 | - Or
254 & - And
255 ~ - Not
256 ^ - Xor
257 n - Xnor
$ 
$ Messages 258-269 are the button labels of the scientific portion of the
$ Calculator when it is in Scientific mode when in "keys" mode. (i.e. button
$ acclerators).
$ 
258 h - Hyp
259 i - Inv
260 { - e^x
261 } - 10^x
262 y - y^x
263 ! - x!
$ Message 264 is 'Control c'
264 ^c - Cos
$ Message 265 is 'Control s'
265 ^s - Sin
$ Message 266 is 'Control t'
266 ^t - Tan
267 N - Ln
268 G - Log
269 ? - Rand
$ 
$ Messages 270-272 are capitalized versions of the calculator MODES.
$ 
270 FINANCIAL
271 LOGICAL
272 SCIENTIFIC
$ 
$ Messages 273-336 are error messages which will go into $HOME/.errorlog. They
$ are all math errors. Localization of these are optional.
$ 
273 *** SIGN NOT 0, +1 OR -1 IN MPADD2 CALL.\n
274 POSSIBLE OVERWRITING PROBLEM ***\n
275 *** N .LE. 1 IN CALL TO MPART1 ***\n
276 *** ABS(X) .GT. 1 IN CALL TO MPASIN ***\n
277 *** ERROR OCCURRED IN MPATAN, RESULT INCORRECT ***
278 *** B = %d ILLEGAL IN CALL TO MPCHK.\n
279 PERHAPS NOT SET BEFORE CALL TO AN MP ROUTINE ***\n
280 *** T = %d ILLEGAL IN CALL TO MPCHK.\n
282 *** M .LE. T IN CALL TO MPCHK.\n 
284 *** B TOO LARGE IN CALL TO MPCHK ***\n
285 *** MXR TOO SMALL OR NOT SET TO DIM(R) BEFORE CALL
286 TO AN MP ROUTINE ***\n
287 *** MXR SHOULD BE AT LEAST %d*T + %d = %d  ***\n 
288 *** ACTUALLY MXR = %d, AND T= %d  ***\n
289 *** FLOATING-POINT OVER/UNDER-FLOW IN MPCMD ***\n
290 *** FLOATING-POINT OVER/UNDER-FLOW IN MPCMR ***\n
291 *** J = 0 IN CALL TO MPCQM ***\n
292 *** ATTEMPTED DIVISION BY ZERO IN CALL TO MPDIV ***\n
293 *** OVERFLOW OCCURRED IN MPDIV ***\n
294 *** ATTEMPTED DIVISION BY ZERO IN CALL TO MPDIVI ***\n
295 *** INTEGER OVERFLOW IN MPDIVI, B TOO LARGE ***\n
296 *** OVERFLOW IN SUBROUTINE MPEXP ***\n
297 *** ERROR OCCURRED IN MPEXP, RESULT INCORRECT ***\n
298 *** ABS(X) NOT LESS THAN 1 IN CALL TO MPEXP1 ***\n
299 *** X NONPOSITIVE IN CALL TO MPLN ***\n
301 *** ERROR IN MPLN, ITERATION NOT CONVERGING ***\n
302 *** ABS(X) .GE. 1/B IN CALL TO MPLNS ***\n
303 *** ERROR OCCURRED IN MPLNS.\n
304 NEWTON ITERATION NOT CONVERGING PROPERLY ***\n
305 *** INTEGER OVERFLOW IN MPMUL, B TOO LARGE ***\n
306 *** ILLEGAL BASE B DIGIT IN CALL TO MPMUL.\n
307 POSSIBLE OVERWRITING PROBLEM ***\n
308 *** OVERFLOW OCCURRED IN MPMUL2 ***\n
309 *** INTEGER OVERFLOW IN MPMUL2, B TOO LARGE ***\n
310 *** ATTEMPTED DIVISION BY ZERO IN MPMULQ ***\n
311 POSSIBLE OVERWRITING PROBLEM ***\n
312 *** OVERFLOW OCCURRED IN MPNZR ***\n
313 *** CALL TO MPOVFL, MP OVERFLOW OCCURRED ***\n
314 *** ERROR OCCURRED IN MPPI, RESULT INCORRECT ***\n
315 *** ATTEMPT TO RAISE ZERO TO NEGATIVE POWER IN\n
316 CALL TO SUBROUTINE MPPWR ***\n
317 *** X NEGATIVE IN CALL TO MPPWR2 ***\n
318 *** X ZERO AND Y NONPOSITIVE IN CALL TO MPPWR2 ***\n
319 *** ATTEMPTED DIVISION BY ZERO IN CALL TO MPREC ***\n
320 *** ERROR OCCURRED IN MPREC, NEWTON ITERATION\n
321 NOT CONVERGING PROPERLY ***\n
322 *** OVERFLOW OCCURRED IN MPREC ***\n
323 *** N = 0 IN CALL TO MPROOT ***\n
324 *** ABS(N) TOO LARGE IN CALL TO MPROOT ***\n
325 *** X = 0 AND N NEGATIVE IN CALL TO MPROOT ***\n
326 *** X NEGATIVE AND N EVEN IN CALL TO MPROOT ***\n
327 *** ERROR OCCURRED IN MPROOT, NEWTON ITERATION\n
328 NOT CONVERGING PROPERLY ***\n
329 *** IDECPL .LE. 0 IN CALL TO MPSET ***\n
330 ITMAX2 TOO SMALL IN CALL TO MPSET ***\n
331 *** INCREASE ITMAX2 AND DIMENSIONS OF MP ARRAYS \n
332 TO AT LEAST %d ***\n
333 *** ERROR OCCURRED IN MPSIN, RESULT INCORRECT ***\n
334 *** ABS(X) .GT. 1 IN CALL TO MPSIN1 ***\n
335 *** X NEGATIVE IN CALL TO SUBROUTINE MPSQRT ***\n
336 *** TAN(X) TOO LARGE IN CALL TO MPTAN ***\n
$ 
$ Messages 337-343 are error messages from the command line options.
$ They also will only show up in the errorlog.
$ 
337 Calculator: -a needs accuracy value of 0-9.\n
$ 
$ Message 338 only goes to the errorlog file 
$ 
338 Calculator: accuracy should be in the range 0-9\nWill set accuracy to the default: 2.\n
339 Calculator: base should be binary, octal, decimal or hexadecimal.\nWill set base to the default: decimal.\n
$ 
$ Message 340 - %s is the invalid display mode the user supplied.
$   This goes to $HOME/.dt/errorlog only.
$ 
340 Calculator: invalid display mode [%s]\n
$ 
$ Message 341 - %s is the invalid mode the user supplied.
$   This goes to $HOME/.dt/errorlog only.
$ 
341 Calculator: invalid mode [%s]\nWill set mode to its default: scientific\n
$ 
$ Message 342 - %s is the invalid trig. mode the user supplied.
$   This goes to $HOME/.dt/errorlog only.
$ 
342 Calculator: invalid trig. mode [%s].\n
343 Calculator: invalid session file.\n
$ 
$ NOTE: no Messages 344 through 350
$ 
$ Message 351 is a label in the ASCII convert dialog.
$ 
351 Character:
$ 
$ Message 352 is the title of the ASCII convert dialog.
$ 
352 Get ASCII
$ 
$ Messages 353-355 are the Trigonometric types (abbreviations).
$ 
353 Deg
354 Grad
355 Rad
$ 
$ Messages 356-358 are the usage messages.
$    Don't translate dtcalc, -a, -m, -b, -no_menu_bar -? -notation -trig or
$     -session.
$ 
356 Calculator (dtcalc) Version 1.%1d\n\n
357 Usage: dtcalc [-a accuracy] [-m mode] [-b base] [-no_menu_bar] [-?]\n
358 \t [-notation display_notation] [-trig trignometric_type] [-session session_file]\n
$ 
$ Messages 359 and 360 not used
$ 
$ Messages 361 - 363 are button labels
$ 
361 Cancel
362 Confirm
363 Continue
364 Error
$ 
$ Message 365 - 369 not used
$ 
$ Messages 370 - 372 get put under the Calculator display when the 
$  corresponding calculator key is pressed, CLR is short for clear, HYP is
$  short for Hyperbolic, and INV is short for Inverse.
$ 
370 CLR
371 HYP
372 INV
373 Invalid constant value
374 Invalid constant number.
375 Invalid function number.
$ 
$ Message 376 not used.
$ 
377 Constant not changed.
378 Numeric stack error
379 Operand stack error
380 Okay to overwrite?
381 Must be in the range 0 - 9
$ 
$ Message 382 not used.
$ 
383 Constant
384 Function
385 Unable to save defaults.\n
$ 
$ NOTE: Messages 386 - 402 are not used.
$ 
403 Exit
$ 
$ Messages 452-478 are the actual characters that the keyboard accelerators
$  map to.  These are the keyboard keys which provide the given function. 
$  For example: Message 5 is the label of the Functions key, Message 52 is
$  the label of the Functions key when the "keys" button is pressed, and
$  finally Message 452 is the character (i.e. 'F') that the calculator uses
$  to represent the Functions key so if the user had a "function string" which
$  included 'F' the calculator know that the 'F' represents the Function key.
$ 
$ Message 452 - the Functions key.
$ 
452 F
$ 
$ Message 453 - the Constants key.
$ 
453 #
$ 
$ Message 454 - the Keys key.
$ 
454 k
$ 
$ Message 455 - the Int (integer) key.
$ 
455 I
$ 
$ Message 456 - the Frac (fraction) key.
$  \006 is "Control f"
$ 
456 \006
$ 
$ Message 457 - the Abs (absolute value) key.
$  \025 is "Control u"
$ 
457 \025
$ 
$ Message 458 - the Exp (exponent) key.
$ 
458 E
$ 
$ Message 459 - the accuracy key.
$ 
459 A
$ 
$ Message 460 - the Store key (memory register store)
$ 
460 S
$ 
$ Message 461 - the Recall key (memory register recall)
$ 
461 R
$ 
$ Message 462 - the Exchange key (memory register exchange)
$ 
462 X
$ 
$ Message 463 - the 1/x key 
$ 
463 r
$ 
$ Message 464 - the x^2 key 
$ 
464 @
$ 
$ Message 465 - the % key 
$ 
465 %
$ 
$ Message 465 - the square root key 
$ 
466 s
$ 
$ Message 467 - the Bsp (backspace) key.
$  \010 is "Control h"
$ 
467 \010
$ 
$ Message 468 - the clear key.
$ Message 468 is "Delete" (Note: Message 513 is the same)  
$ 
468 \177
$ 
$ Message 469 - hex 'd' key
$ 
469 d
$ 
$ Message 470 - hex 'e' key
$ 
470 e
$ 
$ Message 471 - hex 'f' key
$ 
471 f
$ 
$ Message 472 - Change sign (i.e. +/-) key
$ 
472 C
$ 
$ Message 473 - hex 'a' key
$ 
473 a
$ 
$ Message 474 - hex 'b' key
$ 
474 b
$ 
$ Message 475 - hex 'c' key
$ 
475 c
$ 
$ Message 476 - multiplcation key
$ 
476 x
$ 
$ Message 477 - the quit key
$ 
477 q
$ 
$ NOTE: 478 and 479 are not used.
$ 
$ Messages 480-489 are the actual characters that the keyboard accelerators
$  map to when the calculator is in Financial mode.  These are the keyboard 
$  keys which provide the given function. 
$ 
$ Message 480 - the Term key
$ 
480 T
$ 
$ Message 481 - the i%Yr (interest per year)
$  \022 is 'Control r'
$ 
481 \022
$ 
$ Message 482 - the Pv (present value) key
$ 
482 p
$ 
$ Message 483 - the Pmt (payment) key
$ 
483 P
$ 
$ Message 484 - the Fv (future value) key
$ 
484 v
$ 
$ Message 485 - the clrgs (clear financial registers) key
$ 
485 L
$ 
$ Message 486 - the Ctrm (Compounding term) key
$  \024 is 'Control t'
$ 
486 \024
$ 
$ Message 487 - the Ddb (Double-declining depreciation) key
$  \004 is 'Control d'
$ 
487 \004
$ 
$ Message 488 - the Sln (Straight-line depreciation) key
$  \023 is 'Control s'
$ 
488 \023
$ 
$ Message 489 - the Syd (Sum-of-the-years'-digits depreciation) key
$  \031 is 'Control y'
$ 
489 \031
$ 
$ Messages 490-496 are the actual characters that the keyboard accelerators
$  map to when the calculator is in Logical mode.  These are the keyboard 
$  keys which provide the given function. 
$ 
$ Message 490 - the &16 (truncate to 16 bits) key.
$ 
490 [
$ 
$ Message 491 - the &32 (truncate to 32 bits) key.
$ 
491 ]
$ 
$ Message 492 - the OR key
$ 
492 |
$ 
$ Message 493 - the AND key
$ 
493 &
$ 
$ Message 494 - the NOT key
$ 
494 ~
$ 
$ Message 495 - the XOR key
$ 
495 ^
$ 
$ Message 496 - the XNOR key
$ 
496 n
$ 
$ NOTE 497 - 499 aren't used.
$ 
$ Messages 500-511 are the actual characters that the keyboard accelerators
$  map to when the calculator is in Scientific mode.  These are the keyboard 
$  keys which provide the given function. 
$ 
$ Message 500 - the Hyp (Hyperbolic) key
$ 
500 h
$ 
$ Message 501 - the Inv (Inverse) key
$ 
501 i
$ 
$ Message 502 - the e^x key
$ 
502 {
$ 
$ Message 503 - the 10^x key
$ 
503 }
$ 
$ Message 504 - the y^x key
$ 
504 y
$ 
$ Message 505 - the x! key
$ 
505 !
$ 
$ Message 506 - the Cos (cosine) key
$  \003 is 'Control c'
$ 
506 \003
$ 
$ Message 507 - the Sin (sine) key
$  \023 is 'Control s'
$ 
507 \023
$ 
$ Message 507 - the Tan (tangent) key
$  \024 is 'Control t'
$ 
508 \024
$ 
$ Message 509 - the Ln key
$ 
509 N
$ 
$ Message 510 - the Log key
$ 
510 G
$ 
$ Message 510 - the Rand (random) key
$ 
511 ?
$ 
$ Message 512, 513 are HP's del - Clr when it finds an ITF keyboard.
$ 
512 clr disp
$ 
$ Message 513 is 'Clear Display' on HP boxes
$ 
513 \013
$ 
$ added now capability to Financial regs: change the payment per year
$ The following messages (514 - 517) refect this new button.
$ 
514 Payments/Year:
515 P/YR 
516 y - P/YR
517 y
$ 
$ Used as the title, In Asian languages the dialog title needs to be different
$  from the menu label.
$ 
518 Financial Registers
$ 
$set 4
$ ****** Module: help.c ****(developer comment only)********
$ Message 1 is the title for the calculator help dialogs.
1 Calculator - Help
2 You must select an item within Calculator.
3 Error
4 OK

$set 5
$ ****** Module: functions.c ****(developer comment only)********
$ 
1 ERROR: Computation Failed
2 ERROR: No Solution
3 ERROR: Multiple Solutions
4 ERROR: Term<= 0
5 ERROR:Invalid odd period values
6 ERR:Num too large for operation
7 Can't calculate 'Compound Term'\nwithout a non zero interest rate,\na non zero Present value, and\n a not zero Future value.\n

$set 99
$ ****** Version Information ********
$ DO NOT TRANSLATE THESE MESSAGES
$ DO NOT CHANGE THESE MESSAGES
$ DO NOT LOCALIZE THESE MESSAGES
$ These messages are used for the version information.
$quote "
1 "@(#)version_goes_here"
2 "\n@(#)_DtMessage catalog source $XConsortium: dtcalc.msg /main/3 1995/11/01 12:41:03 rswiston $"

