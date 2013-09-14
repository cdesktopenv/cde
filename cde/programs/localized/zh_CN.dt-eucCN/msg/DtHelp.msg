$ *************************************<+>*************************************
$ *****************************************************************************
$ **
$ **  File:        dthelp.msg
$ **
$ **  Project:     CDE 1.0 Help
$ **
$ **  Description:
$ **  -----------
$ **  This file is the source for the message catalog for DtHelp 
$ **
$ **
$ *****************************************************************************
$ **
$ **     (c) Copyright 1990 Hewlett-Packard Company
$ **         All Rights reserved
$ **
$ **
$ **     There are two types of messages in this file:
$ **
$ **     1) Messages that appear in dialogs or are displayed to the user.
$ **        These messages are the default and they should ALL BE LOCALIZED.
$ **        Note that these messages do NOT have any identification.
$ **
$ **     2) Messages that should not be localized.
$ **        These messages are marked with DO NOT TRANSLATE THESE MESSAGES.
$ **
$ *****************************************************************************
$ **
$ **
$ **  ------------------------- MODIFICATION RECORD --------------------------
$ *   Major Mods, 3/26/92 in preperation for the NLS package release on the 
$ *               27th.  (Mike Wilson).
$ *
$ *   Major Mods, 6/10/94 in prep for Cde Message Catalog Freeze
$ *
$ **  ----------------------- MODIFICATION RECORD END ------------------------
$ *****************************************************************************
$ *************************************<+>*************************************

$set 2
$ *****  Module: HelpDialog.c  and a few from HelpQuickD.c *****
$ 
$ These Messages appear in the pulldown menus for Cde Help General Help
$ dialog widget.
$ 
$ They come in pairs where the first is the menu name (e.g. 1 Print...) and the
$ second is the mnemonic (e.g. 2 P).  
$ 
$ Messages 1 through 6 are the pulldown menus for the File menu.
1 打印...
2 P
3 关闭
4 C
5 新窗口...
6 W
$ Messages 7 through 8 are the pulldown menus for the Edit menu.
7 复制
8 C 
$ Messages 9 through 10 are the pulldown menus for the Search menu.
9 索引...
10 I
$ Messages 11 through 16 are the pulldown menus for the Navigate menu.
11 起始题目
12 T
13 回溯
14 B
15 历史记录...
16 H
$ Messages 17 through 28 are the pulldown menus for the Help menu.
17 概览－使用提示
18 v
19 目录
20 C
21 任务
22 T
23 参考
24 R
25 鼠标与键盘
26 M
27 关于提示
28 A
$ 
$ Messages 29 through 36 are the menu labels and mnemonics for for the 
$ Help Dialog.
29 文件
30 F
31 搜索
32 S
33 导览
34 N
35 提示
36 H
37 编辑
38 E
$ 
$ 
$ >>>>> Skip #39 through #49
$ 
$ Messages 50 through 55 are used to display formatting errors within the
$ help dialog and quick help dialog widgets.
$ 
50 字符串数据不能被格式化。
51 动态字符串数据不能被格式化。
52 手册页不能被格式化。请求的手册页或者不在，或者被毁坏。
53 文本文件数据不能被格式化。请求的文本文件或者不在，或者被毁坏。
54 提示题目不能被格式化。
55 不存在的定位标识符：
$ 
$ Messages 56 and 67 are displayed when a hypertext link type is selected but
$ not supported within the current application.
$ 
56 选择的超文本链接在此应用程序中不被支持。
57 到手册页的链接在此应用程序中不被支持。
$ 
$ Messages 58 through 62  are used when bad or null help volumes or 
$ locationId's are used.
$ 
58 请求的联机提示没有安装，或者不处在合适的提示搜索路径上。 要获取关于安装联机提示的信息，请查看本产品的文档。\n
59 定位标识符没有指定。
60 提示卷没有指定。
61 提示卷：
62 定位标识符：
$ 
$ >>>>> Skip #63 through #69
$ 
$ 
$ Message 70 is used when a null value for DtNtopicTitle is used.
$ 
70 不存在的题目标题。
$ 
$ New messages added after early msg cat freeze; not in order
80 指定的提示类型是非法的。



$set 3
$ ****  Module: History.c ****
$ These messages are used as labels for the History Dialog.
1 提示－历史记录浏览器
2 访问的提示卷：
3 关闭
4 提示
5 提示文件
6 手册页
7 提示信息
8 访问的提示题目：
9 访问的提示文件：
10 访问的手册页：
11 访问的提示信息：


$set 4
$ *****  Module: Print.c  *****
$ These messages are used as labels for the Print Dialog.
$ 1: dialog title
1 提示－打印
$ 10-12: standard printer dialog labels
10 打印机：
11 份数：
12 标题页的标题:
$ 20-25: paper size label and sizes
20 纸张大小:
21 信纸 8.5 x 11 英寸
22 律师纸 8.5 x 14 英寸
23 行政纸 7.25 x 10.5 英寸
24 A4 210 x 297 毫米
25 B5 182 x 257 毫米
$ 30-33: help volume-specific printing options
30 要打印的题目
31 当前题目
32 当前及子题目
33 目录与索引
34 卷中的所有题目

$ 40-42: buttons at bottom of dialog
40 打印
41 取消
42 提示

$ These messages are used to describe the kind of content to be printed
$ 50 "Help Volume: "
50 提示卷：
51 提示信息
52 提示文件

$ 53 "Manual Page: "
53 手册页： 
54 出错消息

$ Don't translate message 60 unless it is not an empty string
$ This is the default value that goes in the Printer field
$ 60 " "
$quote "
60 ""
$quote

$ Don't translate message 61
$ This is the default value that goes in the Copies field
61 1


$set 5 
$ *****  Module: GlobSearch.c  *****
$ Message 1 is the label used to label the dialog
1 提示－索引搜索
2 搜索
3 当前
$ DO NOT TRANSLATE MESSAGE 4; it is no longer used
$ " (%s)"
4  (%s)
5 所有卷
6 选择的
7 选择卷...
8 显示
9 完全索引
10 项具有：
11 转至
12 开始搜索
13 继续搜索
14 停止搜索
15 关闭
16 提示
17 正在扫描卷... 找到 %d
18 正在搜索... 还剩卷数：%d
19 没有找到索引项
20 完全索引
21 带有“%s”的项
22 没有选择的卷
23 选择以上的搜索选项
$ Do NOT translate messages 25-31
$quote "
$ " "
25 " "
26 "+"
27 "-"
$ "  +"
28 "  +"
$ "  -"
29 "  -"
$ 30 "       "
30 "       "
$ 31 "         "
31 "         "
$quote
$ DO translate messages 32 and above
$ kind of topic messages
32 提示信息
33 提示文件
34 手册页
35 未知格式
$ specialized title of Search Volume Selection dialog
40 提示－搜索卷选择
$ additional messages (late addition caused this ordering)
50 没有找到索引项。 请试所有卷。
51 没有找到索引项。 请试另一个词。
$ additional messages (later addition caused this ordering)
$ These messages are the braces that go around the current volume name
$ in the index search dialog.  They are kept in the message catalog
$ in case they need to be localized.
60 (
61 )


$set 6
$ *****  Module: HelpAccess.c  *****
$ These messages are used for the Help on Help dialog in Cde Help widgets.
1 关于提示的提示
2 打印...
3 关闭
4 关于提示的提示没有对此应用程序提供！
5 回溯
6 提示...


$set 7
$ *****  Module: Main.c & Util.c & (dthelpview CDE 1.0  *****
$ These messages are used by the dthelpview program.  They need to be 
$ localized for dthelpview, however, they are not directly used by libDtHelp
$ Messages 1 through 7 are command line usage response messages.
$ Do NOT translate the option name (e.g. -helpVolume).  DO translate
$ the argument descriptor name (e.g. <Help Volume File>)
1 用法：%s...\n
2 \t-helpVolume <提示卷文件>\n
3 \t-locationId  <定位标识符>\n
5 \t-file  <ASCII文本文件>\n
6 \t-man\n
7 \t-manPage <Unix手册页>\n\n\n
$ man page dialog
11 手册页
12 手册页：
13 显示手册页
$ button labels
16 退出
17 关闭
18 回溯
19 打印...
$ title of helpview window
20 提示观看器


$set 8
$ *****  Module: HelpUtil.c  *****
$ Message 1 is the label used in the definition link dialog box 
$ (e.g. Quick help close button)
2 关闭
3 内部出错：不能获得鼠标\nDtHelpReturnSelectedWidget 异常终止。\n
4 内部出错：不能获得键盘\nDtHelpReturnSelectedWidget() 异常终止。\n
5 提示出错
$ Messages 10 and up were added after message catalogs were taken
$ for translation.  They are not in any order or chosen location.
10 无论如何都执行
11 不要执行
12 此提示卷欲执行一个命令别名。\n别名 '%s' 没有定义。
13 此提示卷欲执行一个命令。\n为安全起见，自动命令执行被关闭了。\n该命令是：%s
$ End Message set 8


$set 9
$ *****  Module: PathArea.c  *****
$ These messages are used in the general Help Dialog Widget.
$ Message 1 is the label used to label Topic Tree Area  
1 卷： 
$ Messages 2 though 5 are used in the Button Box area of our general 
$ help dialog.
2 回溯
3 历史记录...
4 索引...
5 顶层


$set 10 
$ *****  Module: VolSelect.c  *****
$ Message 1 is the default label used to label the volume selection dialog.
1 提示－卷选择
2 提示卷
3 选择一个或多个提示卷
4 关闭
5 提示


$set 11
$ *****  Module: HelpQuickD.c  *****
$ These messages are used as labels for the quick help dialog.
1 关闭
2 还有...
3 回溯
4 打印...
5 提示...

$set 99
$    DO NOT TRANSLATE THESE MESSAGES
$    DO NOT CHANGE THESE MESSAGES
$    DO NOT LOCALIZE THESE MESSAGES
$    These messages are used for the version information.
$quote "
1 "@(#)version_goes_here"
2 "\n@(#)Message catalog source $XConsortium: DtHelp.msg /main/3 1995/11/08 13:46:59 rswiston $"

