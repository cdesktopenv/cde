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
1 2진
2 8진
3 10진
4 16진
$ 
$ Messages 5 - 7 are used by the "display mode" option button. Eng is an
$  abbreviation for Engineering, Fix is an abbreviation for Fixed, and
$  Sci is an abbreviation for Scientific.  Keep these to 3 characters.
$ 
5 공학 
6 일반 
7 과학 
$ 
$ NOTE: no message 8
$ 
$ Messages 9 - 11 refer to the three modes the Calculator can operate in.
$  They are listed in the "Mode" option button.
$ 
9 재무
10 논리
11 과학
$ 
$ Message 12 - This is the title to the Calculator.
$ 
12 계산기
$ 
$ Messages 13 - 30 are the labels in the menubar pulldown menus and the
$  mnemonic's (the letter that gets underlined for keyboard access) that
$  go with them.
$ 
$ Message 13 is the mnemonic for Message 14
$ 
13 O
14 선택사항O
15 메모리 레지스터R
16 ASCII 변환
$ 
$ Message 17 is the mnemonic for Message 18
$ 
17 H
18 도움말H
$ 
$ Message 19 is the mnemonic for Message 20
$ 
19 v
20 개요v...
$ 
$ Message 21 is the mnemonic for Message 22
$ 
21 T
22 작업T...
$ 
$ Message 23 is the mnemonic for Message 24
$ 
23 R
24 참조R...
$ 
$ Message 25 is the mnemonic for Message 26
$ 
25 O
26 항목O
$ 
$ Message 27 is the mnemonic for Message 28
$ 
27 U
28 도움말 사용법U...
$ 
$ Message 29 is the mnemonic for Message 30
$ 
29 A
30 계산기에 관하여A
$ 
$ Message 31 is an simple error message.
$ 
31 디스플레이를 열 수 없습니다.\n
$ 
$ Messages 32 and 33 are button labels for dialog boxes.
$ 
32 닫기
33 적용
$ 
$ Message 34 is an simple error message.
$ 
34 세션 파일을 열 수 없습니다.
$ 
$ Message 35 - Gets put in the "Functions" popup when no functions have been
$  defined by the users.
$ 
35 정의된 기능이 없습니다.
$ 
$ Message 36 and 37 are titles to two types of dialogs.
$ 
36 알림 - 계속
37 알림 - 확인
$ 
$ Message 38 and 39 are another label for the Help pulldown menu.
$ Message 38 is the mnemonic for Message 39
$ 
38 C
39 차례C
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
46 계산기 돌출 메뉴

$ Used as the title, In Asian languages labels need to be different
$  from the menu label.
$ 
47 도움말
$ 
$set 3
$ ****** Module: text.c ****(developer comment only)********
$ 
$ NOTE: there is no Message 1 - 4.
$ 
$ Messages 5-47 are what gets displayed on each of the calculator keys.
$ 
5 함수
6 상수
7 키
$ 
$ Message 8 - Int is short for integer.
$ 
8 정수
$ 
$ Message 9 - Frac is short for fraction.
$ 
9 소수점이하
$ 
$ Message 10 - Abs is short for absolute value.
$ 
10 절대값
$ 
$ Message 11 - Exp is short for exponent.
$ 
11 지수
12 정확도
13 저장
14 다시 호출
15 교환
16 1/x
17 x^2
18 %
$ 
$ Message 19 - Sqrt is short for square root.
$ 
19 제곱근
20 (
21 )
$ 
$ Message 22 - Bsp is short for backspace.
$ 
22 한칸지우기 
$ 
$ Message 23 - Clr is short for clear.
$ 
23 다시입력 
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
47 종료
$ 
$ Messages 48-51 tell the user what mode they are in.
$ 
48 기본
49 표시
50 모드
51 삼각법
$ 
$ Messages 52-94 are what gets put on the keys when the user presses the 'Keys'
$  key.  These are the keyboard keys which provide the given function. These are
$  also known as keyboard accelerators.  These keys directly correspond to
$  Messages 5-47. (i.e 5 maps to 52, 6 maps to 53, etc.).
$ 
52 F - 함수
53 # - 상수
54 k - 키
55 I - 정수
$ Message 56 is "Control f"
56 ^f-소수점이하
$ Message 57 is "Control u"
57 ^u - 절대값
58 E - 지수
59 A - 정확도
60 S - 저장
61 R - 다시 호출
62 X - 교환
63 r - 1/x
64 @ - x^2
65 %
66 s - 제곱근
67 (
68 )
$ 
$ Message 69 is "Control h"
$ 
69 ^h-한칸지우기
$ 
$ Message 70 is "Delete"
$ 
70 del-다시입력 
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
94 q - 종료
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
107 정확도
108 숫자 기본
109 상수
110 교환
111 함수
113 모드
114 표시 유형
115 검색
116 오른쪽 변형
117 저장
118 삼각법 유형
$ 
$ Messages 119-128 are the messages describing the Constants listed when the
$ Constant popup is brought up.  NOTE: don't translate <=>
$ 
119 시간당 킬로미터 <=> 시간당 마일
120 2의 제곱근
121 e
122 pi
123 센티미터 <=> 인치
124 라디안 각도
125 2 ^ 20
126 그램 <=> 온스
127 킬로주울 <=> 영국의 열 단위
128 입방 센티미터 <=> 입방 인치
$ 
$ Messages 132-143 are labels in the dialogs.
$ 
132 상수 번호:
133 함수 번호:
134 계산기
135 새 상수
136 새 함수
137 메모리 레지스터
138 재무용 레지스터g
139 계산기 특성
140 설명:
141 값:
142 상수 입력C
143 함수 입력F
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
149 %s %1d이(가) 이미 있습니다.
$ 
$ Messages 153-162 are in the accuracy popup menu for the level of accuracy.
$ 
153 0 기수 위치
154 1 기수 위치
155 2 기수 위치
156 3 기수 위치
157 4 기수 위치
158 5 기수 위치
159 6 기수 위치
160 7 기수 위치
161 8 기수 위치
162 9 기수 위치
$ 
$ Messages 163-172 are in the Sto, Rcl, Exch popup menu's listing the registers.
$ 
163 레지스터 0
164 레지스터 1
165 레지스터 2
166 레지스터 3
167 레지스터 4
168 레지스터 5
169 레지스터 6
170 레지스터 7
171 레지스터 8
172 레지스터 9
$ 
$ Messages 188-191 are the numeric base types of operation.
$ 
188 2진
189 8진
190 10진
191 16진
$ 
$ Messages 192-194 are the display types.
$ 
192 공학
193 고정 소수점
194 과학
$ 
$ Messages 195-197 are the trig. types.
$ 
195 각도
196 기울기
197 라디안
$ 
$ Messages 198-201 are the modes of operation of the calculator.
$ 
198 기본
199 재무
200 논리
201 과학
$ 
$ Messages 203-207 are the labels in the Financial Registers dialog.
$ 
203 지불기간:
204 연이율:
205 현재가치:
206 지불액:
207 미래가치:
$ 
$ Messages 208-217 are the button labels of the financial portion of the
$ Calculator when it is in Financial mode.
$ 
208 지불기간 
209 연이율 
$ 
$ Message 210 - Pv stands for Present Value.
$ 
210 현재가치 
$ 
$ Message 211 - Pmt stands for Payment
$ 
211 지불액 
$ 
$ Message 212 - Fv stands for Future Value.
$ 
212 미래가치 
$ 
$ Message 213 means clear financial registers
$ 
213 레지스터지우기
$ 
$ Message 214 means Compounding term
$ 
214 복리기간 
$ 
$ Message 215 means double-declining depreciation
$ 
215 정률법 
$ 
$ Message 216 means Straight-line depreciation
$ 
216 정액법 
$ 
$ Message 217 means Sum-of-the-years-digits depreciation
$ 
217 연수합계법 
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
227 쌍곡선 
$ 
$ Message 228 - Inv is short for Inverse
$ 
228 역 
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
238 난수
$ 
$ Messages 239-248 are the button labels of the financial portion of the
$ Calculator when it is in Financial mode when in "keys" mode. (i.e. button
$ acclerators).
$ 
239 T - 지불기간 
$ Message 240 is 'Control r'
240 ^c - 연이율 
241 p - 현재가치 
242 P - 지불액 
243 v - 미래가치 
244 L-레지스터지우기 
$ Message 245 is 'Control t'
245 ^t - 복리기간 
$ Message 246 is 'Control d'
246 ^d - 정률법 
$ Message 247 is 'Control s'
247 ^s - 정액법
$ Message 248 is 'Control y'
248 ^y-년수합계법 
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
258 h - 쌍곡선 
259 i - 역 
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
269 ? - 난수
$ 
$ Messages 270-272 are capitalized versions of the calculator MODES.
$ 
270 재무
271 논리
272 과학
$ 
$ Messages 273-336 are error messages which will go into $HOME/.errorlog. They
$ are all math errors. Localization of these are optional.
$ 
273 *** MPADD2 호출에서 부호 NOT 0, +1 또는 -1.\n
274 겹쳐쓰기 문제의 가능성 ***\n
275 *** MPART1 호출에서 N .LE. 1 ***\n
276 *** MPASIN 호출에서 ABS(X) .GT. 1 ***\n
277 *** MPATAN에서 오류 발생, 결과가 잘못됨.***
278 *** MPCHK 호출에서 B = %d 이(가) 잘못됨.\n
279 아마도 MP 루틴을 호출하기 전에 설정하지 못함.***\n
280 *** MPCHK 호출에서 T = %d 이(가) 잘못됨.\n
282 *** MPCHK 호출에서 M .LE. T.\n
284 *** MPCHK 호출에서 B가 너무 큽니다. ***\n
285 *** MXR이 너무 작거나 MP 루틴을 호출하기 전에
286 DIM(R)을 설정할 수 없습니다. ***\n
287 *** MXR은 적어도 %d*T + %d = %d (이)어야 합니다.  ***\n
288 *** 실제로 MXR = %d 그리고 T= %d  ***\n
289 *** MPCMD에서 유동 소수점 넘침/아랫자리 넘침.***\n
290 *** MPCMR에서 유동 소수점 넘침/아랫자리 넘침.***\n
291 *** MPCQM 호출에서 J = 0 ***\n
292 *** MPDIV 호출에서 0으로 나누려고 시도했습니다. ***\n
293 *** MPDIV에서 넘침이 발생했습니다. ***\n
294 *** MPDIVI 호출에서 0으로 나누려고 시도했습니다. ***\n
295 *** MPDIVI에서 정수 넘침, B가 너무 큽니다. ***\n
296 *** MPEXP 서브루틴에서 넘침 ***\n
297 *** MPEXP에서 오류 발생, 결과가 잘못되었습니다. ***\n
298 *** MPEXP1 호출에서 ABS(X)가 1 보다 작지 않습니다. ***\n
299 *** MPLN 호출에서 X가 양수가 아닙니다. ***\n
301 *** MPLN에서 오류, 반복이 수렴되지 않습니다. ***\n
302 *** MPLNS 호출에서 ABS(X) .GE. 1/B ***\n
303 *** MPLNS에서 오류가 발생했습니다.\n
304 NEWTON 반복이 제대로 수렴되지 않습니다. ***\n
305 *** MPMUL에서 정수 넘침, B가 너무 큽니다. ***\n
306 *** MPMUL 호출에서 기본 B 숫자가 잘못되었습니다.\n
307 겹쳐쓰기 문제의 가능성 ***\n
308 *** MPMUL2에서 넘침이 발생했습니다. ***\n
309 *** MPMUL2에서 정수 넘침, B가 너무 큽니다. ***\n
310 *** MPMULQ에서 0으로 나누려고 시도했습니다. ***\n
311 겹쳐쓰기 문제의 가능성 ***\n
312 *** MPNZR에서 넘침이 발생했습니다. ***\n
313 *** MPOVFL 호출에서, MP 넘침이 발생했습니다. ***\n
314 *** MPPI에서 오류 발생, 결과가 잘못되었습니다. ***\n
315 *** MPPWR 서브루틴 호출에서 0 올림을 음수 제곱으로 올리려고\n
316 시도했습니다. ***\n
317 *** MPPWR2 호출에서 X 음수 ***\n
318 *** MPPWR2 호출에서 X 0과 양수가 아닌 Y  ***\n
319 *** MPREC 호출에서 0으로 나누려고 시도하였습니다. ***\n
320 *** MPREC에서 오류 발생, NEWTON 반복이\n
321 제대로 수렴되지 않았습니다. ***\n
322 *** MPREC에 오류 발생 ***\n
323 *** MPROOT 호출에서 N = 0 ***\n
324 *** MPROOT 호출에서 ABS(N)이(가) 너무 큽니다. ***\n
325 *** MPROOT 호출에서 X = 0 과 음수 N ***\n
326 *** MPROOT 호출에서 X NEGATIVE와 N EVEN ***\n
327 *** MPROOT에서 오류 발생, NEWTON 반복\n
328 제대로 수렴되지 않습니다. ***\n
329 *** MPSET 호출에서 IDECPL .LE. 0 ***\n
330 MPSET 호출에서 ITMAX2가 너무 작습니다. ***\n
331 *** ITMAX2와 MP 배열의 차원이 증가합니다. \n
332 적어도 %d 까지 ***\n
333 *** MPSIN에서 오류 발생, 결과가 잘못되었습니다. ***\n
334 *** MPSIN1 호출에서 ABS(X) .GT. 1 ***\n
335 *** 서브루틴 MPSQRT 호출에서 X NEGATIVE ***\n
336 *** MPTAN 호출에서 TAN(X)가 너무 큽니다. ***\n
$ 
$ Messages 337-343 are error messages from the command line options.
$ They also will only show up in the errorlog.
$ 
337 계산기: -a 는 0-9의 정확도 값을 요구합니다.\n
$ 
$ Message 338 only goes to the errorlog file
$ 
338 계산기: 정확도는 0-9 사이의 범위이어야 합니다.\n정확도의 기본값 설정:2\n
339 계산기: 기준이 이진, 8진, 10진 또는 16진이어야 합니다.\n기준의 기본값 설정: 10진\n

$ Message 340 - %s is the invalid display mode the user supplied.
$   This goes to $HOME/.dt/errorlog only.
$ 
340 계산기: 잘못된 표시 모드 [%s]\n
$ 
$ Message 341 - %s is the invalid mode the user supplied.
$   This goes to $HOME/.dt/errorlog only.
$ 
341 계산기: 잘못된 모드 [%s]\n모드의 기본값 설정: 과학\n
$ 
$ Message 342 - %s is the invalid trig. mode the user supplied.
$   This goes to $HOME/.dt/errorlog only.
$ 
342 계산기: 잘못된 삼각법 모드 [%s]\n
343 계산기: 잘못된 세션 파일\n
$ 
$ NOTE: no Messages 344 through 350
$ 
$ Message 351 is a label in the ASCII convert dialog.
$ 
351 문자:
$ 
$ Message 352 is the title of the ASCII convert dialog.
$ 
352 ASCII변환값
$ 
$ Messages 353-355 are the Trigonometric types (abbreviations).
$ 
353 각도
354 기울기
355 라디안
$ 
$ Messages 356-358 are the usage messages.
$    Don't translate dtcalc, -a, -m, -b, -no_menu_bar -? -notation -trig or
$     -session.
$ 
356 계산기 (dtcalc) 버전 1.%1d\n\n
357 사용법: dtcalc [-a accuracy] [-m mode] [-b base] [-no_menu_bar] [-?]\n
358 \t[-notation display_notation] [-trig trignometric_type] [-session session_file]\n
$ 
$ Messages 359 and 360 not used
$ 
$ Messages 361 - 363 are button labels
$ 
361 취소
362 확인
363 계속
364 오류
$ 
$ Message 365 - 369 not used
$ 
$ Messages 370 - 372 get put under the Calculator display when the
$  corresponding calculator key is pressed, CLR is short for clear, HYP is
$  short for Hyperbolic, and INV is short for Inverse.
$ 
370 다시입력 
371 쌍곡선 
372 역 
373 잘못된 상수값
374 잘못된 상수 번호
375 잘못된 함수 번호
$ 
$ Message 376 not used.
$ 
377 상수가 바뀌지 않았습니다.
378 숫자 스택 오류
379 연산자 스택 오류
380 겹쳐쓰겠습니까?
381 0 - 9 범위에 있어야 합니다.
$ 
$ Message 382 not used.
$ 
383 상수
384 함수
385 기본값을 저장할 수 없습니다.\n
$ 
$ NOTE: Messages 386 - 402 are not used.
$ 
403 마침x
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
514 지불액/년:
515 연간지불횟수 
516 y-연간지불횟수 
517 y
$ 
$ Used as the title, In Asian languages the dialog title needs to be different
$  from the menu label.
$ 
518 재무용 레지스터
$ 
$ 
$set 4
$ ****** Module: help.c ****(developer comment only)********
$ Message 1 is the title for the calculator help dialogs.
1 계산기 - 도움말
2 계산기에 있는 항목을 선택해야 합니다.
3 오류
4 예

$set 5
$ ****** Module: functions.c ****(developer comment only)********
$ 
1 오류: 계산되지 않았습니다.
2 오류: 해결 방법이 없습니다.
3 오류: 여러 해결 방법이 있습니다.
4 오류: Term<= 0
5 오류:홀수 단계 값이 잘못되었습니다.
6 오류:작동에 대한 숫자가 너무 큽니다.
7  0이 아닌 interest rate,\n0이 아닌 현재 값\n 그리고 0이 아닌 앞으로의 값\n이 없는 'Compound Term'은 계산할 수 없습니다.\n

$set 99
$ ****** Version Information ********
$ DO NOT TRANSLATE THESE MESSAGES
$ DO NOT CHANGE THESE MESSAGES
$ DO NOT LOCALIZE THESE MESSAGES
$ These messages are used for the version information.
1 @(#)version_goes_here
2 \n@(#)_DtMessage catalog source $XConsortium: dtcalc.msg /main/3 1995/11/08 13:14:55 rswiston $


