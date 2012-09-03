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
9 財務
10 論理
11 科学
$ 
$ Message 12 - This is the title to the Calculator.
$ 
12 電卓
$ 
$ Messages 13 - 30 are the labels in the menubar pulldown menus and the
$  mnemonic's (the letter that gets underlined for keyboard access) that 
$  go with them. 
$ 
$ Message 13 is the mnemonic for Message 14
$ 
13 O
14 オプション(O)
15 メモリ・レジスタ(R)
16 ASCII 変換
$ 
$ Message 17 is the mnemonic for Message 18
$ 
17 H
18 ヘルプ(H)
$ 
$ Message 19 is the mnemonic for Message 20
$ 
19 v
20 概要(v)
$ 
$ Message 21 is the mnemonic for Message 22
$ 
21 T
22 使い方(T)
$ 
$ Message 23 is the mnemonic for Message 24
$ 
23 R
24 リファレンス(R)
$ 
$ Message 25 is the mnemonic for Message 26
$ 
25 O
26 アイテムヘルプ(O)
$ 
$ Message 27 is the mnemonic for Message 28
$ 
27 U
28 ヘルプの使い方(U)
$ 
$ Message 29 is the mnemonic for Message 30
$ 
29 A
30 電卓について(A)
$ 
$ Message 31 is an simple error message.
$ 
31 ディスプレイを開けません。\n
$ 
$ Messages 32 and 33 are button labels for dialog boxes.
$ 
32 閉じる
33 適用
$ 
$ Message 34 is an simple error message.
$ 
34 セッション・ファイルを開けません。
$ 
$ Message 35 - Gets put in the "Functions" popup when no functions have been
$  defined by the users.
$ 
35 関数が定義されていません。
$ 
$ Message 36 and 37 are titles to two types of dialogs.
$ 
36 継続通知
37 確認通知
$ 
$ Message 38 and 39 are another label for the Help pulldown menu.
$ Message 38 is the mnemonic for Message 39
$ 
38 C
39 目次(C)
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
46 電卓ポップアップ
$ 
$ Used as the title, In Asian languages labels need to be different
$  from the menu label.
$ 
47 ヘルプ
$ 
$set 3
$ ****** Module: text.c ****(developer comment only)********
$ 
$ NOTE: there is no Message 1 - 4.
$ 
$ Messages 5-47 are what gets displayed on each of the calculator keys.
$ 
5 Functions
$ 5 関数
6 Constants
$ 6 定数
7 Keys
$ 7 キー
$ 
$ Message 8 - Int is short for integer.
$ 
8 Int
$ 8 整数
$ 
$ Message 9 - Frac is short for fraction.
$ 
9 Frac
$ 9 小数
$ 
$ Message 10 - Abs is short for absolute value.
$ 
10 Abs
$ 10 絶対値
$ 
$ Message 11 - Exp is short for exponent.
$ 
11 Exp
$ 11 指数
12 Accuracy
$ 12 精度
13 Store
$ 13 記憶
14 Recall
$ 14 リコール
15 Exchange
$ 15 交換
16 1/x
$ 16 逆数
17 x^2
$ 17 二乗
18 %
$ 
$ Message 19 - Sqrt is short for square root.
$ 
19 Sqrt
$ 19 平方根
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
$ 23 消去
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
$ 47 終了
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
$ 52 F - 関数
$ 53 # - 定数
$ 54 k - キー
$ 55 I - 整数
$ Message 56 is "Control f" 
56 ^f - Frac
$ 56 ^f - 小数
$ Message 57 is "Control u"
57 ^u Abs
58 E - Exp
59 A - Acc
60 S - Sto
61 R - Rcl
62 X - Xchg
63 r - 1/x
64 @ - x^2
$ 57 ^u 絶対値
$ 58 E - 指数
$ 59 A - 精度
$ 60 S - 記憶
$ 61 R - リコール
$ 62 X - 交換
$ 63 r - 逆数
$ 64 @ - 二乗
65 %
66 s - Sqrt
$ 66 s - 平方根
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
$ 70 del - 消去
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
107 精度
108 基数
109 定数
110 交換
111 関数
113 モード
114 ディスプレイ・タイプ
115 検索
116 右シフト
117 格納
118 三角関数タイプ
$ 
$ Messages 119-128 are the messages describing the Constants listed when the
$ Constant popup is brought up.  NOTE: don't translate <=>
$ 
119 キロメートル毎時 <=> マイル毎時
120 2 の平方根
121 e.
122 pi.
123 センチメートル <=> インチ
124 度(平面角)
125 2 ^ 20.
126 グラム <=> オンス
127 キロジュール <=> 英国熱量単位
128 立方センチメートル <=> 立方インチ
$ 
$ Messages 132-143 are labels in the dialogs.
$ 
132 定数番号:
133 関数番号:
134 電卓
135 新規定数
136 新規関数
137 メモリ・レジスタ
138 財務登録(g)
139 電卓属性
140 説明:
141 値:
142 定数の入力(C)
143 関数の入力(F)
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
149 %s %1d はすでに存在します。
$ 
$ Messages 153-162 are in the accuracy popup menu for the level of accuracy.
$ 
153 小数第 0 位
154 小数第 1 位
155 小数第 2 位
156 小数第 3 位
157 小数第 4 位
158 小数第 5 位
159 小数第 6 位
160 小数第 7 位
161 小数第 8 位
162 小数第 9 位
$ 
$ Messages 163-172 are in the Sto, Rcl, Exch popup menu's listing the registers.
$ 
163 レジスタ 0
164 レジスタ 1
165 レジスタ 2
166 レジスタ 3
167 レジスタ 4
168 レジスタ 5
169 レジスタ 6
170 レジスタ 7
171 レジスタ 8
172 レジスタ 9
$ 
$ Messages 188-191 are the numeric base types of operation.
$ 
188 2 進
189 8 進
190 10 進
191 16 進
$ 
$ Messages 192-194 are the display types.
$ 
192 エンジニアリング
193 固定小数点
194 科学
$ 
$ Messages 195-197 are the trig. types.
$ 
195 度
196 グラジエント
197 ラジアン
$ 
$ Messages 198-201 are the modes of operation of the calculator.
$ 
198 基本
199 財務
200 論理
201 科学
$ 
$ Messages 203-207 are the labels in the Financial Registers dialog.
$ 
203 期間:
204 年利:
205 現在の値:
206 支払:
207 将来の値:
$ 
$ Messages 208-217 are the button labels of the financial portion of the
$ Calculator when it is in Financial mode.
$ 
208 Term
209 i%Yr
$ 208 期間
$ 209 年利
$ 
$ Message 210 - Pv stands for Present Value.
$ 
210 Pv
$ 210 現在の値
$ 
$ Message 211 - Pmt stands for Payment
$ 
211 Pmt
$ 211 支払
$ 
$ Message 212 - Fv stands for Future Value.
$ 
212 Fv
$ 212 将来の値
$ 
$ Message 213 means clear financial registers
$ 
213 CLRfregs
$ 213 財務登録の消去
$ 
$ Message 214 means Compounding term
$ 
214 Ctrm
$ 214 複合期間
$ 
$ Message 215 means double-declining depreciation
$ 
215 Ddb
$ 215 倍額定率法
$ 
$ Message 216 means Straight-line depreciation
$ 
216 Sln
$ 216 定額法
$ 
$ Message 217 means Sum-of-the-years-digits depreciation
$ 
217 Syd
$ 217 算術級数法

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
$ 222 論理和
$ 
$ Message 223 means perform a logical AND
$ 
223 And
$ 223 論理積
$ 
$ Message 224 means perform a logical NOT
$ 
224 Not
$ 224 ノット
$ 
$ Message 225 means perform a logical XOR
$ 
225 Xor
$ 225 排他的論理和
$ 
$ Message 226 means perform a logical XNOR
$ 
226 Xnor
$ 226 排他的ノア
$ 
$ Messages 227-238 are the button labels of the Scientific portion of the
$ Calculator when it is in Scientific mode. This is the default mode.
$ 
$ 
$ Message 227 - Hyp is short for Hyperbolic
$ 
227 Hyp
$ 227 双曲線
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
$ 228 逆関数
$ 229 e のべき乗
$ 230 10 のべき乗
$ 231 べき乗
$ 232 階乗
$ 233 コサイン
$ 234 サイン
$ 235 タンジェント
$ 236 自然対数
$ 237 対数
$ 
$ Message 238 - Rand is short for Random
$ 
238 Rand
$ 238 乱数
$ 
$ Messages 239-248 are the button labels of the financial portion of the
$ Calculator when it is in Financial mode when in "keys" mode. (i.e. button
$ acclerators).
$ 
239 T - Term
$ 239 T - 期間
$ Message 240 is 'Control r'
240 ^r - i%Yr
$ See COSE cmvc 7910

241 p - Pv
242 P - Pmt
243 v - Fv
244 L - Clrgs
$ 240 ^c - 年利
$ 241 p - 現在の値
$ 242 P - 支払
$ 243 v - 将来の値
$ 244 L - 財務登録の消去
$ Message 245 is 'Control t'
245 ^t - Ctrm
$ 245 ^t - 複合期間
$ Message 246 is 'Control d'
246 ^d - Ddb
$ 246 ^d - 倍額定率法
$ Message 247 is 'Control s'
247 ^s - Sln
$ 247 ^s - 定額法
$ Message 248 is 'Control y'
248 ^y - Syd
$ 248 ^y - 算術級数法
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
$ 253 | - 論理和
$ 254 & - 論理積
$ 255 ~ - ノット
$ 256 ^ - 排他的論理和
$ 257 n - 排他的ノア
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
$ 258 h - 双曲線
$ 259 i - 逆関数
$ 260 { - e のべき乗
$ 261 } - 10 のべき乗
$ 262 y - べき乗
$ 263 ! - 階乗
$ Message 264 is 'Control c'
264 ^c - Cos
$ 264 ^c - コサイン 
$ Message 265 is 'Control s'
265 ^s - Sin
$ 265 ^s - サイン 
$ Message 266 is 'Control t'
266 ^t - Tan
267 N - Ln
268 G - Log
269 ? - Rand
$ 266 ^t - タンジェント
$ 267 N - 自然対数
$ 268 G - 対数
$ 269 ? - 乱数
$ 
$ Messages 270-272 are capitalized versions of the calculator MODES.
$ 
270 財務
271 論理
272 科学
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
337 電卓: -a オプションには、精度の値 0-9 が必要です。\n
$ 
$ Message 338 only goes to the errorlog file 
$ 
338 電卓: 精度は 0-9 の範囲でなければなりません。\n精度をデフォルトの 2 に設定します。\n
339 電卓: 基数は 2 進、8 進、10 進、または 16 進でなければなりません。\n\
基数をデフォルトの 10 進に設定します。\n
$ 
$ Message 340 - %s is the invalid display mode the user supplied.
$   This goes to $HOME/.dt/errorlog only.
$ 
340 電卓: 表示モード [%s] が無効です。\n
$ 
$ Message 341 - %s is the invalid mode the user supplied.
$   This goes to $HOME/.dt/errorlog only.
$ 
341 電卓: モード [%s] が無効です。\nモードをデフォルト(科学)に設定します。\n
$ 
$ Message 342 - %s is the invalid trig. mode the user supplied.
$   This goes to $HOME/.dt/errorlog only.
$ 
342 電卓: 三角関数モード [%s] は無効です。\n
343 電卓: セッション・ファイルが無効です。\n
$ 
$ NOTE: no Messages 344 through 350
$ 
$ Message 351 is a label in the ASCII convert dialog.
$ 
351 文字:
$ 
$ Message 352 is the title of the ASCII convert dialog.
$ 
352 ASCII を取得
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
356 電卓(dtcalc)バージョン 1.%1d\n\n
357 使用方法: dtcalc [-a 精度] [-m モード] [-b 基数] [-no_menu_bar] [-?]\n
358 \t [-notation 表示通知] [-trig 三角関数のタイプ] [-session セッション・ファイル]\n
$ 
$ Messages 359 and 360 not used
$ 
$ Messages 361 - 363 are button labels
$ 
361 取消し
362 確認
363 継続
364 エラー
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
373 定数の値は無効です。
374 定数番号は無効です。
375 関数番号は無効です。
$ 
$ Message 376 not used.
$ 
377 定数を変更しません。
378 数値スタック・エラーが発生しました。
379 オペランド・スタック・エラーが発生しました。
380 上書きしてもいいですか?
381 0 - 9 の範囲でなければなりません。
$ 
$ Message 382 not used.
$ 
383 定数
384 関数
385 デフォルトを保存できません。\n
$ 
$ NOTE: Messages 386 - 402 are not used.
$ 
403 終了(x)
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
514 支払／年:
515 P/YR 
516 y - P/YR
517 y
$ 
$ Used as the title, In Asian languages the dialog title needs to be different
$  from the menu label.
$ 
518 財務登録
$set 4
$ ****** Module: help.c ****(developer comment only)********
$ Message 1 is the title for the calculator help dialogs.
1 電卓 - ヘルプ
2 電卓内で項目を選択しなければなりません。
3 エラー
4 了解

$set 5
$ ****** Module: functions.c ****(developer comment only)********
$ 
1 エラー: 計算に失敗しました。
2 エラー: 解答がありません。
3 エラー: 複数解答があります。
4 エラー: 期間<= 0
5 エラー: 期間の値が無効です。
6 エラー: 操作するには数字が大きすぎます。
7 0 以外の利率、0 以外の現在の値、および 0 以外の将来の値がなければ、\n'複合期間' を計算できません。
$ 1 エラー: 計算に失敗しました。
$ 2 エラー: 解答がありません。
$ 3 エラー: 複数解答があります。
$ 4 エラー: 期間 <= 0
$ 5 エラー: 期間の値が無効です。
$ 6 エラー: 計算するには数字が大きすぎます。
$ 7 0 以外の利率、0 以外の現在の値、および 0 以外の将来の値がなければ、\n[複合期間] を計算できません。

$set 99
$ ****** Version Information ********
$ DO NOT TRANSLATE THESE MESSAGES
$ DO NOT CHANGE THESE MESSAGES
$ DO NOT LOCALIZE THESE MESSAGES
$ These messages are used for the version information.
1 @(#)version_goes_here
2 \n@(#)_DtMessage catalog source $XConsortium: dtcalc.msg /main/4 1996/10/16 03:13:46 cde-hit $
