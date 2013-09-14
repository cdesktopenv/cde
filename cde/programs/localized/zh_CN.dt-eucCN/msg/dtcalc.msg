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
1 2 进制
2 8 进制
3 10进制
4 16进制
$ 
$ Messages 5 - 7 are used by the "display mode" option button. Eng is an
$  abbreviation for Engineering, Fix is an abbreviation for Fixed, and
$  Sci is an abbreviation for Scientific.  Keep these to 3 characters.
$ 
5 工程
6 定点
7 科学
$ 
$ NOTE: no message 8
$ 
$ Messages 9 - 11 refer to the three modes the Calculator can operate in.
$  They are listed in the "Mode" option button.
$ 
9 金融
10 逻辑
11 科学
$ 
$ Message 12 - This is the title to the Calculator.
$ 
12 计算器
$ 
$ Messages 13 - 30 are the labels in the menubar pulldown menus and the
$  mnemonic's (the letter that gets underlined for keyboard access) that 
$  go with them. 
$ 
$ Message 13 is the mnemonic for Message 14
$ 
13 O
14 选项
15 记忆寄存器
16 ASCII 变换
$ 
$ Message 17 is the mnemonic for Message 18
$ 
17 H
18 提示
$ 
$ Message 19 is the mnemonic for Message 20
$ 
19 v
20 概览
$ 
$ Message 21 is the mnemonic for Message 22
$ 
21 T
22 任务
$ 
$ Message 23 is the mnemonic for Message 24
$ 
23 R
24 参考
$ 
$ Message 25 is the mnemonic for Message 26
$ 
25 O
26 针对项
$ 
$ Message 27 is the mnemonic for Message 28
$ 
27 U
28 使用提示
$ 
$ Message 29 is the mnemonic for Message 30
$ 
29 A
30 关于计算器
$ 
$ Message 31 is an simple error message.
$ 
31 不能打开显示。\n
$ 
$ Messages 32 and 33 are button labels for dialog boxes.
$ 
32 关闭
33 应用
$ 
$ Message 34 is an simple error message.
$ 
34 不能打开会话过程文件。
$ 
$ Message 35 - Gets put in the "Functions" popup when no functions have been
$  defined by the users.
$ 
35 没有定义函数
$ 
$ Message 36 and 37 are titles to two types of dialogs.
$ 
36 继续通知
37 确认通知
$ 
$ Message 38 and 39 are another label for the Help pulldown menu.
$ Message 38 is the mnemonic for Message 39
$ 
38 C
39 目录
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
46 计算器弹出菜单
$ 
$ Used as the title, In Asian languages labels need to be different
$  from the menu label.
$ 
47 提示
$ 
$set 3
$ ****** Module: text.c ****(developer comment only)********
$ 
$ NOTE: there is no Message 1 - 4.
$ 
$ Messages 5-47 are what gets displayed on each of the calculator keys.
$ 
5 函数
6 常量
7 键
$ 
$ Message 8 - Int is short for integer.
$ 
8 整数
$ 
$ Message 9 - Frac is short for fraction.
$ 
9 小数
$ 
$ Message 10 - Abs is short for absolute value.
$ 
10 绝对值
$ 
$ Message 11 - Exp is short for exponent.
$ 
11 指数
12 精度
13 储存
14 调出
15 交换
16 1/x
17 x^2
18 %
$ 
$ Message 19 - Sqrt is short for square root.
$ 
19 平方根
20 (
21 )
$ 
$ Message 22 - Bsp is short for backspace.
$ 
22 退格
$ 
$ Message 23 - Clr is short for clear.
$ 
23 清除
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
47 退出
$ 
$ Messages 48-51 tell the user what mode they are in.
$ 
48 基本
49 显示
50 模式
51 三角
$ 
$ Messages 52-94 are what gets put on the keys when the user presses the 'Keys'
$  key.  These are the keyboard keys which provide the given function. These are
$  also known as keyboard accelerators.  These keys directly correspond to
$  Messages 5-47. (i.e 5 maps to 52, 6 maps to 53, etc.).
$ 
52 F - 函数
53 # - 常量
54 k - 键
55 I - 整数
$ Message 56 is "Control f"
56 ^f - 小数
$ Message 57 is "Control u"
57 ^u 绝对值
58 E - 指数
59 A - 精度
60 S - 储存
61 R - 调出
62 X - 交换
63 r - 1/x
64 @ - x^2
65 %
66 s - 平方根
67 (
68 )
$ 
$ Message 69 is "Control h"
$ 
69 ^h - 退格
$ 
$ Message 70 is "Delete"
$ 
70 del - 清除
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
94 q - 退出
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
109 常量
110 交换
111 函数
113 模式
114 显示类型
115 检索
116 右移
117 储存
118 三角类型
$ 
$ Messages 119-128 are the messages describing the Constants listed when the
$ Constant popup is brought up.  NOTE: don't translate <=>
$ 
119 每小时公里 <=> 每小时英里。
120 2 的平方根。
121 e。
122 pi。
123 厘米 <=> 英寸。
124 弧度。
125 2 ^ 20。
126 克 <=> 盎司。
127 千焦 <=> 英国热量单位。
128 立方厘米 <=> 立方英寸。
$ 
$ Messages 132-143 are labels in the dialogs.
$ 
132 常量号:
133 函数号:
134 计算器
135 新常量
136 新函数
137 记忆寄存器
138 金融寄存器
139 计算器特性
140 描述:
141 值:
142 输入常量
143 输入函数
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
149 %s %1d 已经存在。
$ 
$ Messages 153-162 are in the accuracy popup menu for the level of accuracy.
$ 
153 ０个小数位置
154 １个小数位置
155 ２个小数位置
156 ３个小数位置
157 ４个小数位置
158 ５个小数位置
159 ６个小数位置
160 ７个小数位置
161 ８个小数位置
162 ９个小数位置
$ 
$ Messages 163-172 are in the Sto, Rcl, Exch popup menu's listing the registers.
$ 
163 寄存器０
164 寄存器１
165 寄存器２
166 寄存器３
167 寄存器４
168 寄存器５
169 寄存器６
170 寄存器７
171 寄存器８
172 寄存器９
$ 
$ Messages 188-191 are the numeric base types of operation.
$ 
188 二进制
189 八进制
190 十进制
191 十六进制
$ 
$ Messages 192-194 are the display types.
$ 
192 工程
193 定点
194 科学
$ 
$ Messages 195-197 are the trig. types.
$ 
195 角度
196 梯度
197 弧度
$ 
$ Messages 198-201 are the modes of operation of the calculator.
$ 
198 基本
199 金融
200 逻辑
201 科学
$ 
$ Messages 203-207 are the labels in the Financial Registers dialog.
$ 
203 方式:
204 年利%:
205 现值:
206 支付:
207 未来值:
$ 
$ Messages 208-217 are the button labels of the financial portion of the
$ Calculator when it is in Financial mode.
$ 
208 方式
209 年利%
$ 
$ Message 210 - Pv stands for Present Value.
$ 
210 现值
$ 
$ Message 211 - Pmt stands for Payment
$ 
211 支付
$ 
$ Message 212 - Fv stands for Future Value.
$ 
212 未来值
$ 
$ Message 213 means clear financial registers
$ 
213 清金融寄存器
$ 
$ Message 214 means Compounding term
$ 
214 复合方式
$ 
$ Message 215 means double-declining depreciation
$ 
215 双倍递减折旧
$ 
$ Message 216 means Straight-line depreciation
$ 
216 直线折旧
$ 
$ Message 217 means Sum-of-the-years-digits depreciation
$ 
217 年数总和折旧
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
222 或
$ 
$ Message 223 means perform a logical AND
$ 
223 与
$ 
$ Message 224 means perform a logical NOT
$ 
224 非
$ 
$ Message 225 means perform a logical XOR
$ 
225 异或
$ 
$ Message 226 means perform a logical XNOR
$ 
226 异非或
$ 
$ Messages 227-238 are the button labels of the Scientific portion of the
$ Calculator when it is in Scientific mode. This is the default mode.
$ 
$ 
$ Message 227 - Hyp is short for Hyperbolic
$ 
227 双曲
$ 
$ Message 228 - Inv is short for Inverse
$ 
228 逆
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
238 随机数
$ 
$ Messages 239-248 are the button labels of the financial portion of the
$ Calculator when it is in Financial mode when in "keys" mode. (i.e. button
$ acclerators).
$ 
239 T - 方式
$ Message 240 is 'Control r'
240 ^c - 年利%
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
253 | - 或
254 & - 与
255 ~ - 非
256 ^ - 异或
257 n - 异非或
$ 
$ Messages 258-269 are the button labels of the scientific portion of the
$ Calculator when it is in Scientific mode when in "keys" mode. (i.e. button
$ acclerators).
$ 
258 h - 双曲
259 i - 逆
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
269 ? - 随机数
$ 
$ Messages 270-272 are capitalized versions of the calculator MODES.
$ 
270 金融
271 逻辑
272 科学
$ 
$ Messages 273-336 are error messages which will go into $HOME/.errorlog. They
$ are all math errors. Localization of these are optional.
$ 
273 *** 在 MPADD2 调用中符号不是 0，+1 或 -1。\n
274 可能覆盖问题 ***\n
275 *** 在 MPART1 调用中 N .LE. 1  ***\n
276 *** 在 MPASIN 调用中 ABS(X) .GT. 1  ***\n
277 *** MPATAN 中发生出错，结果不正确 ***
278 *** 在 MPCHK 调用中 B = %d 非法。\n
279 可能在调用一个 MP 例程前没有设置 ***\n
280 *** 在 MPCHK 调用中 T = %d 非法。\n
282 *** 在 MPCHK 调用中 M .LE. T 。\n
284 *** 在 MPCHK 调用中 B 太大 ***\n
285 *** MXR 太小，或调用一个 MP 例程前没有
286 设置为 DIM(R) ***\n
287 *** MXR 必须至少 %d*T + %d = %d  ***\n
288 *** 实际 MXR = %d，和 T= %d  ***\n
289 *** 在 MPCMD 中浮点上/下溢 ***\n
290 *** 在 MPCMR 中浮点上/下溢 ***\n
291 *** 在调用 MPCQM 中 J = 0 ***\n
292 *** 在调用 MPDIV 中试图除以零 ***\n
293 *** MPDIV 中上溢 ***\n
294 *** 在调用 MPDIVI 中试图除以零 ***\n
295 *** 在 MPDIVI 中整数上溢，B 太大 ***\n
296 *** 在 MPEXP 子例程中上溢 ***\n
297 *** 在 MPEXP 中发生错误， 结果不正确 ***\n
298 *** 在 MPEXP1 调用中 ABS(X) 不小于 1 ***\n
299 *** 在 MPLN 调用中 X 非负 ***\n
301 *** 在 MPLN 中出错， 迭代不收敛 ***\n
302 *** 在 MPLNS 调用中 ABS(X) .GE. 1/B ***\n
303 *** 在 MPLNS 中出错。\n
304 牛顿迭代不收敛 ***\n
305 *** 在 MPMUL 中整数上溢，B 太大 ***\n
306 *** 在 MPMUL 调用中非法的基数 B 。\n
307 可能覆盖问题 ***\n
308 *** 在 MPMUL2 中出现上溢 ***\n
309 *** 在 MPMUL2 中整数上溢，B 太大 ***\n
310 *** 在 MPMULQ 中试图除以零 ***\n
311 可能覆盖问题 ***\n
312 *** 在 MPNZR 中出现上溢 ***\n
313 *** 在 MPOVFL 调用中， 出现 MP 上溢 ***\n
314 *** 在 MPPI 中出错，结果不正确 ***\n
315 *** 在 MPPWR 调用中，试图做零的\n
316 负数次方 ***\n
317 *** 在 MPPWR2 调用中 X 负数 ***\n
318 *** 在 MPPWR2 调用中 X 为零以及 Y 非负 ***\n
319 *** 在 MPREC 调用中试图除以零 ***\n
320 *** 在 MPREC 中出错，牛顿迭代\n
321 不正确地收敛 ***\n
322 *** 在 MPREC 中上溢 ***\n
323 *** 在 MPROOT 调用中 N = 0 ***\n
324 *** 在 MPROOT 调用中 ABS(N) 太大 ***\n
325 *** 在 MPROOT 调用中 X = 0 及 N 为负 ***\n
326 *** 在 MPROOT 调用中 X 为负，N 为偶数 ***\n
327 *** 在 MPROOT 中出错，牛顿迭代\n
328 不正确地收敛 ***\n
329 *** 在 MPSET 调用中 IDECPL .LE. 0 ***\n
330 *** 在 MPSET 调用中 ITMAX2 太小 ***\n
331 *** 增加 ITMAX2 和 MP 数组的维数到\n
332 至少 %d ***\n
333 *** 在 MPSIN 中出错，结果不正确 ***\n
334 *** 在 MPSIN1 调用中 ABS(X) .GT. 1 ***\n
335 *** 在 MPSQRT 调用中 X 为负 ***\n
336 *** 在 MPTAN 调用中 TAN(X) 太大 ***\n
$ 
$ Messages 337-343 are error messages from the command line options.
$ They also will only show up in the errorlog.
$ 
337 计算器: -a 需要 0-9 的精度值。\n
$ 
$ Message 338 only goes to the errorlog file 
$ 
338 计算器: 精度必须在 0-9 的范围内\n将设置精度值为默认值: 2。\n
339 计算器: 基数必须是二进制、八进制、十进制或十六进制。\n将设置成基数的默认值: 十进制。\n
$ 
$ Message 340 - %s is the invalid display mode the user supplied.
$   This goes to $HOME/.dt/errorlog only.
$ 
340 计算器: 无效的显示模式 [%s]\n
$ 
$ Message 341 - %s is the invalid mode the user supplied.
$   This goes to $HOME/.dt/errorlog only.
$ 
341 计算器: 无效的模式 [%s]\n将设置成默认的模式: 科学\n
$ 
$ Message 342 - %s is the invalid trig. mode the user supplied.
$   This goes to $HOME/.dt/errorlog only.
$ 
342 计算器: 无效的三角模式 [%s]。\n
343 计算器: 无效的会话过程文件。\n
$ 
$ NOTE: no Messages 344 through 350
$ 
$ Message 351 is a label in the ASCII convert dialog.
$ 
351 字符:
$ 
$ Message 352 is the title of the ASCII convert dialog.
$ 
352 得到 ASCII 码
$ 
$ Messages 353-355 are the Trigonometric types (abbreviations).
$ 
353 角度
354 梯度
355 弧度
$ 
$ Messages 356-358 are the usage messages.
$    Don't translate dtcalc, -a, -m, -b, -no_menu_bar -? -notation -trig or
$     -session.
$ 
356 计算器（dtcalc）版本 1.%1d\n\n
357 用法: dtcalc [-a 精度] [-m 模式] [-b 基数] [-no_menu_bar] [-?]\n
358 \t [-notation 显式记号] [-trig 三角类型] [-session 会话过程文件]\n
$ 
$ Messages 359 and 360 not used
$ 
$ Messages 361 - 363 are button labels
$ 
361 取消
362 确认
363 继续
364 出错
$ 
$ Message 365 - 369 not used
$ 
$ Messages 370 - 372 get put under the Calculator display when the 
$  corresponding calculator key is pressed, CLR is short for clear, HYP is
$  short for Hyperbolic, and INV is short for Inverse.
$ 
370 清除
371 双曲
372 逆
373 无效的常量值
374 无效的常量号。
375 无效的函数号。
$ 
$ Message 376 not used.
$ 
377 常量没有更改。
378 数值栈出错
379 操作数栈出错
380 覆盖吗?
381 必须在０ - ９的范围内
$ 
$ Message 382 not used.
$ 
383 常量
384 函数
385 不能保存默认值。\n
$ 
$ NOTE: Messages 386 - 402 are not used.
$ 
403 退出
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
512 清除显示
$ 
$ Message 513 is 'Clear Display' on HP boxes
$ 
513 \013
$ 
$ added now capability to Financial regs: change the payment per year
$ The following messages (514 - 517) refect this new button.
$ 
514 付款/年:
515 付款/年
516 y - 付款/年
517 y
$ 
$ Used as the title, In Asian languages the dialog title needs to be different
$  from the menu label.
$ 
518 金融寄存器
$ 
$set 4
$ ****** Module: help.c ****(developer comment only)********
$ Message 1 is the title for the calculator help dialogs.
1 计算器 - 提示
2 您必须选择计算器中的一个条目。
3 出错
4 确定

$set 5
$ ****** Module: functions.c ****(developer comment only)********
$ 
1 出错: 计算失败
2 出错: 无解
3 出错: 多解
4 出错: 条目<= 0
5 出错: 无效的奇周期值
6 出错: 数对操作太大
7 在没有一个非零的利率、一个非零现值、\n一个非零的未来值时，不能计算 '复合方式'。\n

$set 99
$ ****** Version Information ********
$ DO NOT TRANSLATE THESE MESSAGES
$ DO NOT CHANGE THESE MESSAGES
$ DO NOT LOCALIZE THESE MESSAGES
$ These messages are used for the version information.
$quote "
1 "@(#)version_goes_here"
2 "\n@(#)_DtMessage catalog source $XConsortium: dtcalc.msg /main/3 1995/11/08 13:49:17 rswiston $"

