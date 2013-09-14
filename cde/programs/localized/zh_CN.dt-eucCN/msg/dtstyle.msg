 
$ "$XConsortium: dtstyle.msg /main/3 1995/11/08 13:52:58 rswiston $"

$ *************************************<+>*************************************
$ *****************************************************************************
$ **
$ **  File:        dtstyle.msg
$ **
$ **  Project:     DT Style Manager
$ **
$ **  Description:
$ **  -----------
$ **  This file is the source for the message catalog for dtstyle
$ **
$ **
$ *****************************************************************************
$ **
$ **     (c) Copyright 1990 Hewlett-Packard Company
$ **     All Rights reserved
$ **
$ **
$ *****************************************************************************
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
$ 2. Messages that only appear in the DT error log file ($HOME/.dt/errorlog).
$ 
$     The localization of these messages is OPTIONAL.  These messages are
$     identified by the following:
$ 
$  MESSAGES xx-yy IN SET zz WILL ONLY APPEAR IN THE DT ERRORLOG FILE
$ 
$ 3. Messages that should not be localized.  
$ 
$     These messages are identified by the following:
$ 
$  DO NOT TRANSLATE or CHANGE or LOCALIZE MESSAGES xx-yy from set zz
$ 
$ ***** END (NOTE FOR MESSAGE CATALOG TRANSLATORS) *****
$ 
$ ******************************************************************************
$ *************************************<L>*************************************
$ **  -----------------GENERAL LOCALIZATION NOTES SECTION---------------------
$ * Comment lines begin with a $ except $set which indicates the start of a 
$ * new set.
$ *
$ * Do not delete any comments; you may add comments for your use.
$ *
$ * 
$ **  ----------------GENERAL LOCALIZATION NOTES SECTION END------------------
$ *************************************<L>*************************************

$set 2
$ Message 1 from set 2 was deleted from the message catalog

$ _DtMessage 2 is the title of all Information dialog boxes
2 通告
$ _DtMessage 3 is the title of all Error dialog boxes
3 出错
$ _DtMessage 4 WILL ONLY APPEAR IN THE DT ERRORLOG FILE
4 不能与会话过程管理程序通讯...\n\
退出。

$  MESSAGES 5-6 IN SET 2 WILL ONLY APPEAR IN THE DT ERRORLOG FILE
5 式样管理程序已经在运行，\n\
第二次尝试异常终止。
6 X 工具箱出错...\n\
退出。

$ _DtMessage 7 is the title of all Help dialog boxes
7 式样管理程序 - 提示

$ _DtMessage 8 is an error message that appears when you are using Itemh Help
$ and select on something outside of the Style Manager
8 你必须在式样管理程序中\n\
选择一个项目。

$ _DtMessage 9 WILL ONLY APPEAR IN THE DT ERRORLOG FILE
9 不能从会话过程管理程序中获得屏\n\
幕保护程序的信息。初起设定可能\n\
不正确。


$set 4
$ _DtMessage set 4 is for the Style Manager main window
$ Messages 6,8-26,34,39 from set 4 were deleted from the message catalog

$ _DtMessage 1 is the title of the main dtstyle window
1 式样管理程序

$ mnemonic and label for the File menubar item.
2 F
3 文件[F]
$ mnemonic and label for the Help menubar item.
4 H
5 提示[H]

$ _DtMessage 7 is the label for the Exit button in the File pulldown
$ the mnenomic for the Exit button is set 4 _DtMessage 40
7 退出[E]

$ Messages 27-33,35  are the labels on the buttons in the dtstyle main window.
27 颜色
28 字体
29 幕布
30 键盘
31 鼠标
32 响铃
33 屏幕
35 启动

$  MESSAGE 36 IN SET 4 WILL ONLY APPEAR IN THE DT ERRORLOG FILE
36 对当前会话过程没有 Dtstyle 恢复信息可用

$ _DtMessage 37 is the label on the dtstyle icon
37 式样

$ Message 38 is the label on the Window button in the dtstyle main window.
38 窗口

$ _DtMessage 40 is replaced with message 53
$ 40 E

$ *
$ * mnemonic and label for the Help menu items.
$ *

$ Messages 41-42 have been replaced with messages 54-55
$ Overview... menu item
$ 41 I
$ 42 介绍[I]...

$ Tasks... menu item 
43 T
$ Message 44 has been replaced with messages 56
$ 44 任务[T]...

$ Reference... menu item
$ 45 R
$ Message 46 has been replaced with messages 57
$ 46 参考[R]...

$ On Item menu item
$ Keybaord menu item
47 O
48 针对项[O]

$ Using Help... menu item
49 U
$ Message 50 has been replaced with messages 58
$ 50 使用提示...

$ Messages 51-52 have been replaced with messages 59-60
$ Overview menu item
$ 51 V
$ 52 版本...

$ Message 53 is replaced with message 40
53 x

$ Messages 54-55 are replacements for messages 41-42
54 v
55 概览[v]

$ Message 56 is a replacement for messages 44
56 任务[T]

$ Message 57 is a replacement for messages 46
57 参考[R]

$ Message 58 is a replacement for messages 50
58 使用提示[U]

$ Messages 59-60 are replacements for messages 51-52
59 A
60 关于式样管理器[A]


$set 5
$ _DtMessage set 5 is for the Font dialog
$ Messages 1-6, 7-16 from set 5 were deleted from the message catalog

$ _DtMessage 17 is the label of the titlebox of the preview font area
17 预演

$ _DtMessage 18 appears in a Notice dialog after the user selects a new font
$ and presses Ok - when the resource *WriteXrdbImmediate is True
18 字体的改变对某些应用程序将会在下次它\n\
们启动时显示出来。其它应用程序，例如文件\n\
管理程序和应用程序管理程序，在它们退出桌\n\
面并重新登录进入前是不会显示这些改变的字\n\
体的。


$ _DtMessage 19 appears in a Notice dialog after the user selects a new font
$ and presses Ok - when the resource *WriteXrdbImmediate is False
19 选择的字体将在你重新启动此会话过程\n\
时使用。

$ _DtMessage 20 and 21 is the text that initially appears in the Font Preview 
$ area displaying sample user and system fonts.
20 AaBbCcDdEeFfGg0123456789
21 AaBbCcDdEeFfGg0123456789 

$ _DtMessage 22 is the label of the titlebox containing the list of font sizes
22 大小

$ _DtMessage 23 is the title of the Fonts dialog
23 式样管理程序 - 字体


$set 6
$ _DtMessage set 6 is for the Beep dialog
$ _DtMessage 1,6 from set 6 have been deleted from the message catalog

$ Messages 2 through 5 are the default buttons and labels of the scales.  
2 默认
3 音量
4 音调
5 持续时间

$ _DtMessage 7 is the title of the Beep dialog box
7 式样管理程序 - 响铃


$set 7
$ _DtMessage set 7 is for the Startup dialog
$ Messages 1-2 from set 7 have been deleted from the message catalog

$ Messages 3 through 9 are labels of the buttons and text visible in the 
$ Session Startup dialog box. 
3 在登录时：
4 恢复当前会话过程
5 返回到起始会话过程
6 在注销时询问我
7 注销确认对话：
8 开
9 关
10 设置起始会话过程...

$ _DtMessage 11 is a warning message when the user pushes Set Home Session...
$ Notice the spaces before Continue .. the spaces make it centered.
11 这将以当前会话过程代替你的\n\
起始会话过程。\n\n \
		继续吗?

$ _DtMessage 12 is the title of the warning dialog box when Set Home Session is 
$ pushed. 
12 警告

$ _DtMessage 13 is an error message
13 此会话过程从一个 X 窗口系统初起\n\
脚本中启动的。由于该会话过程不能被\n\
恢复，因此初起设定不合法。

$ _DtMessage 14 is the title of the Session Startup dialog box.
14 式样管理程序 - 初起

$ _DtMessage 15 is an error message
15 会话过程管理程序不在运行。\n\
初起设定不合法。


$set 8
$ All of the messages from set 8 have been deleted from the message catalog


$set 9
$ _DtMessage set 9 is for the Mouse dialog
$ Message 7 from set 9 have been deleted from the message catalog

$ Messages 1-4 are labels of toggle buttons in the Mouse Dialog. 
1 左
2 右
3 惯用手：
4 按钮２：
$ _DtMessage 5 is the label of default button.
5 默认

$ Messages 6,8,9 are labels of the scales.
6 双击键
8 加速
9 域值  

$  MESSAGE 10 IN SET 9 WILL ONLY APPEAR IN THE DT ERRORLOG FILE
10 出错：设置指针映射\n

11 传送
12 调整 

$ _DtMessage 13 appears in a Notice dialog after the user selects the 
$ left-handed or right-handed toggle
13 鼠标的左右按钮将在你按下确定后\n\
立即交换功能。鼠标窗口中的‘测试\n\
双击键’图形通过按钮 1 和按钮 3\n\
的位置显示了当前的惯用手情况。


$ _DtMessage 14 appears in a Notice dialog after the user selects the 
$ transfer toggle to change the function of the middle mouse button.
14 如果你按一下【确定】，鼠标的按钮２将\n\
在你的下一会话过程中设置为‘传送’（拖动）。

$ double-click time and presses Ok (and the resource 
$ *WriteXrdbImmediate is True)
15 新的双击键时间将在应用程序重新启动时\n\
起作用。

$ _DtMessage 16 appears in a Notice dialog after the user selects a new 
$ double-click time and presses Ok (and the resource 
$ *WriteXrdbImmediate is False)
16 新的双击键时间将在你的下一会话过程中\n\
起作用。

$ _DtMessage 17 is the text below the mouse icon. Notice the two lines.
17 测试\n双击键

$ _DtMessage 18 is the Mouse Dialog title
18 式样管理程序 - 鼠标

$ _DtMessage 19 appears in a Notice dialog after the user selects the 
$ adjust toggle to change the function of the middle mouse button.
19 如果你按一下【确定】，鼠标的按钮２\n\
将在你的下一会话过程时设置成‘调整’。\n\
按钮２可以被用来调整文本和列表选择。\n\
在文本和列表上的传送动作（拖动）将要求\n\
使用按钮１。

$set 10
$ _DtMessage set 10 is for the SaveRestore functionality
$ Messages 2,4 from set 10 have been deleted from the message catalog

$  MESSAGE 1,3 IN SET 10 WILL ONLY APPEAR IN THE DT ERRORLOG FILE
$ Messages 1,3 make up one error message.  Put together, they say 
$ "<filename> <_DtMessage 2> <_DtMessage 1>
1 检查文件权限。
3 %s 是应该用来保存你的会话过程的文件。\n


$set 11
$ _DtMessage set 11 is for the Backdrop dialog

$  MESSAGE 1 IN SET 11 WILL ONLY APPEAR IN THE DT ERRORLOG FILE

$ _DtMessage 1 - could not open a backdrop directory
1 不能打开目录 %s。
$ _DtMessage 2 - error opening the backdrop files
2 由于没有可用的幕布文件，式样管理程序的幕布\n\
部分将不能工作。请检查 $HOME/.dt/errorlog。


$ _DtMessage 12 is the title of the Backdrop dialog box
12 式样管理程序 - 幕布


$set 12
$ _DtMessage set 12 is for the Screen dialog
$ Message 2 from set 12 has been deleted from the message catalog

$ _DtMessage 1 is the title of the Screen dialog box.
1 式样管理程序 - 屏幕

$ Messages 3-14 are the labels and buttons for the Screen dialog
3 默认
4 屏幕保护程序
5 启动保护程序
6 每个保护程序的时间
7 分钟
8 锁屏
$ Message 9 has been removed 
10 开
11 关
12 屏幕保护程序：
13 锁屏：
14 开始锁定
15 自动清屏
16 清屏程序：
17 清屏启动
18 前面板锁定
$ Message 19 has been replaced by message 21
19 使用手动保护屏幕
20 每个背景时间
$ Message 21 is a replacement for message 19
21 锁定时使用背景
22 透明锁定

$set 13
$ _DtMessage set 13 is for the Keyboard dialog
$ _DtMessage 1,5 from set 13 have been deleted from the message catalog

$ _DtMessage 2 through 4 are labels of scales, buttons, and text visible in the
$ Keyboard dialog box.
2 默认
3 自动重复
4 击键音量

$ _DtMessage 6 is the title of the Keyboard dialog box
6 式样管理程序 - 键盘


$set 14
$ _DtMessage set 14 is for the Color dialog
$ _DtMessage 1-4,9,14-15,17,19,20,22,24 from set 14 have been deleted from the message catalog

$ Messages 5 through 8 are labels and buttons visible in the Color dialog
5 调色板
6 增加...
7 删除...
8 修改...

$ _DtMessage 10 is the title above the text area in the Add Palette dialog box
10 新的调色板名：

$ _DtMessage 11 is the title of the Add Palette dialog box.
11 增加调色板

$ Messages 12 through 14 are error messages.
12 调色板名不能包含这些字符\n\
\n\
<space> * : ( ) [ ] { } < > ! | " / \\
$ LOCALIZERS: FOR 8 AND 16 BIT LANGUAGES YOU MAY WANT TO CHANGE THE MESSAGE 
$ TO SAY THE PALETTE NAME MUST BE 10 BYTES OR LESS
13 调色板名必须少于或等于 10 个字节。\n

$ _DtMessage 16 is the title of the Delete Palette dialog box.  See screen 4a (2).
16 删除调色板

$ _DtMessage 18 appears in an information dialog.
18 不能删除最后的调色板。\n

$ _DtMessage 21 is an error message
21 警告

$ _DtMessage 23 appears in an information dialog
23 删除调色板‘%s’?\n

$ _DtMessage 25 is an error message that appears when you press the color
$ button and the color server is not running for some reason.
25 由于颜色服务器没有运行，式样管理程序的颜\n\
色部分不能工作。请检查 $HOME/.dt/errorlog。


$ _DtMessage 26 can be caused by setting the resource *useColorObj to False
$ and then running dtstyle and trying to post the Color Dialog.
26 由于资源‘useColorObj’设置成 False，\n\
式样管理程序的颜色部分将不能工作。


$ _DtMessage 27 is for the new ColorUse dialog when you press ok.
27 新的‘颜色使用’值将在你的下次\n\
会话过程中起作用。

$ _DtMessage 28 is a warning message that appears on Black and White systems
28 选择的调色板将在你的下次\n\
会话过程中起作用。

$ _DtMessage 29 appears in a warning dialog
29 调色板名‘%s’已经存在。\n\
此新的调色板将覆盖旧的。\n\
你想这样做吗?

$ _DtMessage 30 is the title of the Color dialog box.
30 式样管理程序 - 颜色

$ _DtMessage 31-34 are the ColorUse options
$ 31 颜色丰富
$ 32 颜色中等
$ 33 颜色稀少
34 黑白
35 默认

$ Dt Message 36 has been replaced by message 39
$ _DtMessage 36 is the title of the Color Use dialog
$ 36 DT 颜色使用

$ Dt Message 37 has been replaced by message 40
$ _DtMessage 37 is the Color Use button in the main Color Dialog
$ 37 DT 颜色使用...

$ _DtMessage 38 is an error message that appears when you press the color
$ button and there are no palette files to be found
38 由于没有调色板文件可用，式样管理程序的颜\n\
色部分不能工作。请检查 $HOME/.dt/errorlog。

$ DtMessage 39 is a replacement for DtMessage 36
$ DtMessage 39 is the title of the Color Use dialog
39 要使用的颜色数量:

$ DtMessage 40 is a replacement for DtMessage 37
$ DtMessage 40 is the title of the Color Use dialog
40 颜色数量...

$ _DtMessages 41-43 are replacements for Dtmessages 31-33
$ _DtMessage 41-43 are the ColorUse options
41 更多的颜色用于桌面
42 更多的颜色用于应用程序
43 最多的颜色用于应用程序



$set 15
$ All of the messages from set 15 have been deleted from the message catalog


$set 16
$ _DtMessage set 16 is for the Color dialog
$ _DtMessage 1-10,13-17 from set 16 have been deleted from the message catalog

$ _DtMessage 11-12 form one error message that appears in an error dialog
11 不能删除‘%s’。\n
12 检查 $HOME/.dt/errorlog 以获得提示。

$  MESSAGE 18-21 IN SET 16 WILL ONLY APPEAR IN THE DT ERRORLOG FILE
18 警告，在资源 paletteDirectories 列出太多的目录，\n\
最大数目是 %d。
$ _DtMessage 19 - could not open a palette directory
19 不能打开目录 %s。
$ _DtMessage 20 - error opening the palette file
20 不能打开 %s。
$ _DtMessage 21 - error reading a palette file
21 %s 是无效的调色板文件。


$set 17
$ _DtMessage set 17 is for the Modify Color dialog
$ Messages 1-3,13,16 from set 17 have been deleted from the message catalog

$ Messages 4 through 12 are labels, buttons, and text visible in the
$ Modify Color dialog box
4 颜色采样
5 旧
6 新建
7 抓取颜色
8 颜色编辑器
9 色调
$ We recommend that you don't translate R,G,B which stands for Red, Green,
$ Blue unless there are standard single letter acronyms in your language.
10 R
11 G
12 B

$  MESSAGE 14-15 IN SET 17 WILL ONLY APPEAR IN THE DT ERRORLOG FILE
14 警告，不能获得指针。\n
15 警告，不能获得键盘。\n

$ _DtMessage 16 is the title of the Modify Color dialog box.
16 式样管理程序 - 修改颜色


$set 18
$ _DtMessage set 18 is for the Window dialog
$ _DtMessage 1 is the title of the Window dialog box
1 式样管理程序 - 窗口

$ Messages 2 through 5 are labels on the Window dialog.
2 默认
3 窗口行为
4 窗口图标
$ Messages 5-8 have been replaced by messages 13-16
$ 5 焦点跟随在鼠标后
$ 6 在窗口内按一下以得到焦点
$ 7 提升焦点窗口
$ 8 整体移动
9 使用图标盒
10 放到工作空间
11 允许主窗口在顶层
12 要使你的更改起作用, 工作空间管理程序必须重新启动.\n\
按【确定】重新启动工作空间管理程序\n\
或【取消】以恢复这些更改。
$ Messages 13-16 are replacements for messages 5-8
13 指到窗口内使之激活
14 在窗口内按一下使之激活
15 当窗口激活时提升它
16 在窗口移动过程中显示其内容

$set 99
$    DO NOT TRANSLATE THESE MESSAGES
$    DO NOT CHANGE THESE MESSAGES
$    DO NOT LOCALIZE THESE MESSAGES
$    These messages are used for the version information.
$quote "
1 "@(#)version_goes_here"
2 "\n@(#)_DtMessage catalog source $XConsortium: dtstyle.msg /main/3 1995/11/08 13:52:58 rswiston $"

