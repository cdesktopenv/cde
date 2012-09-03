$ $XConsortium: dtterm.msg /main/3 1995/11/08 13:53:10 rswiston $
$ *************************************<+>*************************************
$ *****************************************************************************
$ **
$ **  File:        dtterm.msg
$ **
$ **  Project:     Common Desktop Environment dtterm
$ **
$ **  Description:
$ **  -----------
$ **  This file is the source for the message catalog for dtterm
$ **  Any additional messages are to be added to this file by hand,
$ **  and the associated index number placed in the code.
$ **
$ *****************************************************************************
$ **
$ **     (c) Copyright 1993, 1994 Hewlett-Packard Company
$ **     All Rights reserved
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
$ ********** PROLOGUE ****************
$  Date    Name               Note
$ ======== =================  ===============================================
$ 03/15/94 B. May             Initial rev
$ 
$ *****************************************************************************

$set 1
$ ****** Module: DtTermMain.c 
$ Message 1 is the window menu addition - do not translate f.separator or
$ f.send_msg.  It will be used as the format string for a sprintf() command,
$ so it is necessary to double up on any percent signs ('%') that you
$ want to keep in the string.
$ Message 2 is an error message

1 \
	分隔符            f.separator\n\
	切换\\菜单\\栏  f.send_msg %d\

2 %s: -S 选项不能与 -server 或 -serverid 选项一起使用\n
3 终端

$set 2
$ ****** Module: DtTermSyntax.c 
$ These are the usage messages. 
$ DO NOT TRANSLATE Messages 37 or 41 ("End-of-list")

1 -/+132                        启用/关闭 80<->132 列换码序列
2 -/+aw                         启用/关闭自动回行
3 -bg 颜色                      背景颜色
4 -/+bs                         开关终端背景是选择颜色
5 -display 显示名               要连接的 X 服务器
6 -e 命令 参数                  要执行的命令
7 -fb 字体                      黑体文本字体
8 -fg 颜色                      前景颜色
9 -fn 字体                      正常文本字体
10 -geometry 几何数据           大小（按字符算）和窗口的位置
11 -help                        打印出此信息
12 -/+iconic                    启动/不启动图标
13 -/+j                         启用/关闭跳动滚屏
14 -/+kshMode                   启用/关闭 ksh 模式
15 -/+l                         启用/关闭记录日志
16 -lf 文件名                   记录日志文件名
17 -/+ls                        启用/关闭登录 shell
18 -/+map                       启用/关闭将窗口映射在 pty 输出上
19 -/+mb                        启用/关闭边界响铃
20 -ms 颜色                     指针颜色
21 -n 字符串                    指定图标名
22 -name 字符串                 客户实例，图标，和标题字符串
23 -nb 距离                     指定距右边界响铃的距离
24 -/+rw                        启用/关闭反向回行
25 -/+sb                        启用/关闭滚动条
26 -/+sf                        启用/关闭 SUN 功能键
27 -sl 数字                     保存的滚动行数[屏幕]
28 -ti 名字                     用于程序识别的字符串
29 -title 字符串                窗口标题字符串
30 -tm 字符串                   终端模式关键字和字符
31 -tn 名字                     TERM 环境变量名
32 -/+vb                        启用/关闭 视觉响铃
33 -xrm 资源字符串              额外的资源描述
34 -C                           监控台模式
35 -Sxxd                        在 \"ttyxx\" 文件描述符 \"d\" 上的从模式
36 -Sxxx.d                      在 \"ttyxxx\" 文件描述符 \"d\" 上的从模式
37 End-Of-List
38 -e 选项，如果给出的话必须出现在命令行末尾,
39 否则将启动用户的默认 shell。以加号（＋）开始
40 的选项恢复默认值。
41 End-Of-List
47 %s:  错误的命令行选项 \"%s\"\r\n\n
48 用法: %s
49 \r\n
50  [%s]
51 \r\n\n输入 \"%s -help\" 得到完整的描述。\r\n\n
52 用法: \n
53 \t%s [-选项 ...] [-e 命令 参数]\n\n
54 其中选项包括:\n

$set 3
$ ****** Module: TermFunction.c
$ This is a warning message dialog box
1 存储器满\n按【确定】清除

$set 4
$ ****** Module: TermPrim.c
$ This is the title of a dialog box
1 Dtterm - 警告

$set 5
$ ****** Module: TermFunction.c
$ This is a warning message dialog box
1 存储器满\n按【确定】清除

$set 6
$ ****** Module: TermView.c
$ DO NOT TRANSLATE Message 1
$ Messages 2 and 3 are titles for dialog boxes.
1 80x24 132x24
2 Dtterm - 联机手册
3 Dtterm - 提示

$set 7
$ ****** Module: TermViewGlobalDialog.c
$ These are the user-visible labels in the Global Options dialog box.
$ #1 is the title. 
$ The rest are resource types and values - look at the dialog box and 
$ documentation for full specs. 
$ 
$ DO NOT TRANSLATE MESSAGE 15
$ 
2 确定
3 取消
4 提示
5 全局
6 光标控制
7 框
8 下划线
9 光标式样
10 启用
11 关闭
12 闪烁光标
13 闪烁速率（毫秒）
14 不可见
15 blinkRate
16 颜色控制
17 反白
18 正常
19 窗口背景
20 滚动行为
21 关闭
22 启用
23 平滑滚动
24 响铃控制
25 可听
26 可视
27 响铃类型
28 启用
29 关闭
30 页边警告
31 页边距离
32 应用
33 复位
34 终端 - 全局选项
$set 8
$ ****** Module: TermViewMenu.c
$ These are the menu bar entries and menu pane choices.
$ NOTE TO TRANSLATORS: The Single letter entries are the menu mnemonic
$ for the entry above them (i.e. message 11 "F" is the mnemonic for
$ message 10 "Font Size"). 
$ The items with a "+" in them are the accellerators for an item.
$ The single-character items are the mnemonics for a menu item, where the
$ text of the menu item is specified in the previous message
$ Do not translate messages 24 and 28.
1 默认
2 窗口大小[W]
3 W
4 -iso8859-1
5 %d 点
6 %.1f 点
7 %.1f 点
8 %.2f 点
9 默认
10 字体大小[F]
11 F

$   Window menu
12 新建[N]
13 N
14 打印[P]
15 P
16 打印[R]...
17 r
18 关闭[C]
19 C
20 窗口[W]
21 W

$   Edit menu
22 复制[C]
23 C
24 Ctrl osfInsert
25 Ctrl+Insert
26 粘贴[P]
27 P
28 Shift osfInsert
29 Shift+Insert
30 编辑[E]
31 E

$  Option menu

32 菜单栏[M]
33 M
34 滚动条[S]
35 S
36 全局[G]...
37 G
38 终端[T]...
39 T
40 软复位[S]
41 S
42 硬复位[H]
43 H
44 复位[R]
45 R
46 选项[O]
47 O

$ Old Help menu (no longer used for CDE)

48 概述[O]
49 O
50 任务[T]
51 T
52 参考[R]
53 R
54 针对项[O]
55 O
56 使用提示[U]
57 U
58 关于 dtterm[A]
59 A
60 提示[H]
61 H

62 Alt+F4

63 dtterm

$ Current CDE Help menu

64 概述[v]
65 v
66 索引[I]
67 I
68 内容目录[C]
69 C
70 任务[T]
71 T
72 参考[R]
73 R
74 键盘[K]
75 K
76 使用提示[U]
77 U
78 关于 dtterm[A]
79 A
80 提示[H]
81 H

$set 9
$ ****** Module: TermTerminalDialog.c
$ These are the messages for the terminal options dialog
2 确定
3 取消
4 提示
5 键盘控制
6 正常
7 应用程序
8 光标键方式
9 数值
10 应用程序
11 小键盘方式
12 仅回车
13 回车/换行
14 换行序列
15 锁定
16 未锁定
17 用户功能键
18 屏幕控制
19 启用
20 关闭
21 132 列切换
22 行末回行
23 反向回行
24 应用
25 复位
26 dtterm－终端选项
