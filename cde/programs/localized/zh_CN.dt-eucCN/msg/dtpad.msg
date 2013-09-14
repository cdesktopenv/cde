$ $XConsortium: dtpad.msg /main/4 1995/12/08 09:46:14 rswiston $
$ *************************************<+>*************************************
$ *****************************************************************************
$ **
$ **  File:        dtpad.msg
$ **
$ **  Project:     DT Text Editor client (sometimes called dtpad)
$ **
$ **  Description:
$ **  -----------
$ **  This file is the source for the message catalog for dtpad
$ **
$ **
$ *****************************************************************************
$ **
$ **     (c) Copyright 1992, 1993 Hewlett-Packard Company
$ **     All Rights reserved
$ **
$ **
$ *****************************************************************************
$ **
$ *****************************************************************************
$ **  (c) Copyright 1993, 1994 Hewlett-Packard Company
$ **  (c) Copyright 1993, 1994 International Business Machines Corp.
$ **  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
$ **  (c) Copyright 1993, 1994 Novell, Inc.
$ *****************************************************************************
$ **

$set 2
$ **********************************************
$ **  General messages used in multiple files **
$ **********************************************

$ Label used for all "Help" buttons in all DIALOGS.
$ It is not used for the Help menu.
1 提示


$set 4
$ *****************************************************************
$ **                                                    fileCB.c **
$ ** These are the strings which change between the Open dialog  **
$ ** and the Include dialog.  These two operations use the same  **
$ ** Motif dialog, and simply change these two labels.           **
$ *****************************************************************

$ Title of the Open dialog
1 打开文件

$ label for the File text field in the file selection box
$ ***UNUSED***	This label is no longer used and need not be localized.
2 File to Open:

$ Title of the Include dialog
3 包含文件

$ Label for the File text field in the file selection box
$ ***UNUSED***	This label is no longer used and need not be localized.
4 File to Include:

$ Error when printing a file and a temporary copy can not be saved.
5 不能创建临时文件。


$set 5
$ ***************************************************************************
$ **                                                               dtpad.c **
$ **                                                             fileDlg.c **
$ **                                                                       **
$ ** Messages and strings used in dialogs for all operations involving the **
$ ** "File" menu items.                                                    **
$ ***************************************************************************
$ These compose the dialog which appears if the user attempts to exit, open
$ a different file, drop a file, or start a "New" session with unsaved
$ changes to the current file.

$ This is used to build: "Save changes to FILENAME?"
1 保存更改到 %s?

$ This is used as-is, whenever the current file is unnamed.
2 当前文件没有名字。\n您要保存它吗?

$ This is used to build a message like "Do you want to save the current
$ copy of FILENAME before you Edit NEWFILENAME?" when a new file is droped
$ on the edit window.
$ ***UNUSED***	Currently, dropped files are INSERTED in the text rather than
$		opened for editing so this message is no longer used and need
$		not be localized.
3 您要保存 %s 的当前复制 - 在您编缉 %s 前?

$ Used when about to write over an existing file.
$ The "%s" is replaced by the file name.
5 文件 %s 已经存在。\n您想用此编缉会话过程里的\n内容该文件吗?

$ Title of "file already exists" dialog.  Also used for other "Warning" type
$ dialogs.
6 警告

$ Title of "Save As" dialog.
7 保存为

$ Labels for add/don't-add newlines radio buttons in the Save As dialog
$ ***UNUSED***	These labels are no longer used and need not be localized.
$		They've been replaced by messages 44 and 45
8 增加换行符到断行行尾。
9 不增加换行符。只保留由【回车】产生的断行。

$ Labels for the Save Warning dialog. Also used in the confirmation, or
$ Editor Return dialog.
14 否
15 是
$ Title of the dialog - used when user selects "New", "Open", or "Exit" with
$ unsaved changes in the text.
16 保存?

$ Title of general error dialog
17 出错

$ Title of the resource-specified "confirmation" dialog
$ This dialog is generally specified by programs such as mailers, which
$ want to know if the user wants to send the message.
$ There is no default message for the dialog, since it is _only_ posted if
$ the message string is specified via a resource (dtpad*confirmationString:)
$ This message is only used if the "dtpad*mainTitle:" resource is not set.
$ If this resource _is_ set, the the mainTitle is also used as the title
$ for the exit confirmation dialog.
$ DO NOT LOCALIZE - the user should never see this message
18 Exit Confirmation

$ Title for general information dialog. e.g. Find dialog can't find word in
$ file.
20 信息

$ Default file name in Save As dialog box.
21 未命名的

$ Error message when a file can not be saved.  The "%1$s" gets replaced by
$ the file name and the "%2$s" gets replaced by the reason why the file could
$ not be saved (messages 31-39).
30 不能保存到 %1$s。\n%2$s

$ Messages 31-39 are reasons why a file could not be saved.
$ Messages 34-39 are substitued in message 30 for "%2$s".
31 不能保存文件。\n文件名没有指定。

34 文件不允许被任何人写。

35 文件是一个目录。

36 文件是一个字符特殊设备。

37 文件是一个块模式设备。

$ ***UNUSED***	This message should never appear since currently there is
$		only one supported file format - text.  And this type is
$		hard coded into dtpad.
38 无效的文件格式。

$ This is a "catch all" when the file can not be saved and none of the above
$ errors apply.
39 原因不明 - 可能磁盘空间不够。

$ This is displayed when a ToolTalk edit request on a buffer is received,
$ a Save operation is done and there are unsaved changes.
40 保存更改?

$ This is displayed when a ToolTalk edit request on a file or buffer is
$ received with a document name specified in the request, a Save operation
$ is done and there are unsaved changes.  The "%s" in the message is
$ replaced by the document name (instead of the filename).
41 保存更改到 "%s"?

$ Title of "Copy As" dialog which is substitued for the "Save As" dialog
$ when file name changes are disallowd.  This should match the menu button
$ label specified in message 108 of set 11.
$ ***UNUSED***	This title is no longer used and need not be localized.
42 复制成

$ Title of "Copy To File" dialog which is substitued for the "Save As" dialog
$ when file name changes are disallowd.  This should match the menu button
$ label specified in message 110 of set 11.
43 复制到文件

$ Labels for add/don't-add newlines radio buttons in the SaveAs and Save dialogs
44 在折行的行尾增加新行字符（newline）。
45 不增加新行字符。只有用 [Return] 键建立的行分隔符被保留。
$ 
$ New message as of 5/31/95 (i.e. new since Sample Implementation). This
$ new message is in response to bug DTS CISlx19530.
$ 
46 文件不能写或文件不存在。

$set 6
$ ************************************************************************
$ **                                                           fileIo.c **
$ **                                                                    **
$ ** Error messages generated when there are problems reading a file.   **
$ ************************************************************************

1 此文件已经为只读打开。
2 此文件不存在。
3 不能编辑一个目录。
4 不能编辑一个字符特殊设备文件。
5 不能编辑一个块模式设备文件。
6 Embedded Nulls stripped from file.
7 Unable to load file (insufficient memory).
8 Unable to access file (can't determine its status).
9 Unable to read from the file (probably no read permission).
$ Message placed in the status line message area based on the -viewOnly
$ resource and whether or not the file being edited is read only
10 只看
11 只读

$set 7
$ ************************************
$ **                         main.c **
$ **                        dtpad.c **
$ ** Prefix string for all dialogs. **
$ ************************************
$ bogus?? blank string gotten but otherwise unused in main()
$ DO NOT LOCALIZE string 1
$quote "
1 ""
$quote

$ The name used in the title bar of every window.
$ Be sure to end this message with a space character.
2 文本编辑器 -

$ Default file name used in the main window title.
$ ***UNUSED***	This mesage has been replaced by message 6 of this set and
$		need not be localized.
3 (unnameed)

$ Terminal error message displayed to stdout when the desk top services
$ can not be initialized (via DtInitialize())
4 不能初始化桌面服务... 退出文本编辑器。

$ Terminal error message displayed to stdout when a connection to the
$ display can not be opend (via XtDisplay())
5 不能打开显示... 退出文本编辑器。

$ Default file name used in the main window title.
$ This should correspond to the default file name used in the "Save As"
$ dialog box - specified in message 21 of set 5.
6 (未命名的)
$ 
$ New messages (7,8) since CDE/SI - error gets displayed when user tries to run
$ dtpad -server when and dtpad server process is already running on the
$ $DISPLAY.
7 错误: %s 上的 dtpad 服务程序\n\
已经用于该显示。
8 错误: 一个 dtpad 服务程序已经\n\
用于该显示。
$ 
$ Message 9 is also new since CDE/SI - new as of 5/22/95.  It is a usage message
$ for dtpad. Don't translate the '-OPTION' just the text explaining the
$ option.
$ 
9 '%s' 是一个错误的选项。\n\
用法: %s...\n\
   -saveOnClose\n\
\t当修改过的文本还未保存而要关闭文本编辑器时, dtpad 会自动地\n\
\t保存当前文件。\n\n\
   -missingFileWarning\n\
\t每当指定一个文件, 而这个文件不存在或不能存取时, 显示一个\n\
\t警告对话框。\n\n\
   -noReadOnlyWarning\n\
\t每当对指定的文件用户没有写权限时停用这个警告对话框。\n\n\
   -noNameChange\n\
\t表明当文本保存在不同于它读出的目录时与当前文本相关的\n\
\t默认文件名不要改变。\n\n\
   -viewOnly\n\
\t不允许在编辑窗口中进行文本编辑, 实质上是将文本编辑器\n\
\t转变成观看器。\n\n\
   -statusLine\n\
\t在编辑窗口的底行显示一状态行。状态行显示文本光标当前位置\n\
\t所在行的行号。\n\n\
   -wrapToFit\n\
\t启用折行模式。\n\n\
   -workspaceList WorkspaceList\n\
\t在指定的工作空间里显示文本编辑器当前调用的编辑窗口。\n\n\
   -session SessionFile\n

10 \t恢复文本编辑器到所有的文本编辑窗口, 以及曾在先前 CDE 关\n\
\t闭前生效的设置。当这个选项指定后其它的命令行选项都被忽\n\
\t略。\n\n\
   -standAlone\n\
\t强制文本编辑器当前的调用在它自己的窗口做它自己的文本处理,\n\
\t独立于文编辑器服务程序。\n\n\
   -noBlocking\n\
\t一旦文本编辑器服务程序决定它能处理请求程序的编辑请求, \n\
\t就中断文本编辑器请求程序进程。\n\n\
   -server\n\
\t强制文本编辑器服务程序启动 (如果还没有运行的话) 去处理所\n\
\t有一系列的用于显示的编辑请求。\n\n\
   -exitOnLastClose\n\
\t当最后的用于显示的编辑窗口关闭时, 指定要中断的文本编辑器服务\n\
\t程序进程。它只是在使用 -server 选项时才有用, 因为它只用于服\n\
\t务程序进程。\n\n\
有关这些选项的详细说明, 请参阅 dtpad 联机手册。\n\n
$ 


$set 10
$ ************************************************************************
$ **                                                          session.c **
$ ** THESE MESSAGES ARE _ONLY_ WRITTEN TO THE ERRORLOG FILE.
$ **
$ ************************************************************************
$ Strings used to build error messages issued when the session save
$ file cannot be read or written.
1 检查文件权限。
2  %s 是一个用来保存您的会话过程的文件。


$set 11
$ ***********************************************************
$ **                                              dtpad.c **
$ **                                                       **
$ ** These are all of the menu button labels and mnemonics **
$ ***********************************************************

$ ***UNUSED*** 	Messages 1-14 are no longer used and need not be localized.
$		All of the mnemonics and labels relating to the "Help" menu
$		have been replaced by messages 81-96.

$ *
$ * Mnemonics and labels for the "Help" pulldown menu and items.
$ *
$ Mnemonic and label for the "Help" pulldown menu
1 H
2 提示[H]

$ mnemonic and label for the "Overview" item
3 V
4 概述[V]...

$ mnemonic and label for the "Tasks" item
5 T
6 任务[T]...

$ nmemonic and label for the "Table of Contents" item are message nos. 71 & 72

$ mnemonic and label for the "Reference" item
7 R
8 参考[R]...

$ mnemonic and label for the "On Item" item
9 O
10 针对项[O]

$ mnemonic and label for "Using Help" item
11 U
12 使用提示[U]...

$ mnemonic and label for "About Text Editor" item
13 A
14 关于文本编缉器[A]...

$ *
$ * Mnemonics and labels for the "File" pulldown menu and items.
$ *
$ mnemonic and label for the "File" pulldown menu
15 F
16 文件[F]

$ mnemonic and label for the "New" item
17 N
18 新建[N]

$ mnemonic and label for the "Open" item
19 O
20 打开[O]...

$ mnemonic and label for the "Include" item
21 I
22 包括[I]...

$ mnemonic and label for the "Save" item
$ NOTE: The "Save" label can be either Message 24 or message 73
23 S
24 保存[S]

$ mnemonic and label for the "Save As" item
25 A
26 保存为[A]...

$ mnemonic and label for the "Print" item
27 P
28 打印[P]...

$ mnemonic and label for the "Close" item
29 C
30 关闭[C]

$ *
$ * Mnemonics and labels for the "Edit" pulldown menu and items.
$ *
$ mnemonic and label for the "Edit" pulldown menu
31 E
32 编辑[E]

$ mnemonic and label for the "Undo" menu item
33 U
34 回复[U]

$ mnemonic and label for the "Cut" menu item
35 t
36 剪切[t]

$ mnemonic and label for the "Copy" menu item
37 C
38 复制[C]

$ mnemonic and label for the "Paste" menu item
39 P
40 粘贴[P]

$ mnemonic and label for the "Clear" menu item
41 e
42 清除[e]

$ mnemonic and label for the "Delete" menu item
43 D
44 删除[D]

$ ***UNUSED***	Messages 45-50 are no longer used and need not be localized.
$		They have been replaced by messages 101-106 of this set.
$ mnemonic and label for the "Select All" menu item
$ 45 A
$ 46 Select All

$ mnemonic and label for the "Find/Change" menu item
$ 47 F
$ 48 查找/更改[F]...

$ mnemonic and label for the Check Spelling menu item
$ 49 S
$ 50 检查拼写[S]...

$ *
$ * Mnemonics and labels for the "Format" pulldown menu and items.
$ *
$ mnemonic and label for the "Format" pulldown menu
51 r
52 格式[r]

$ mnemonic and label for the "Settings..." menu item
53 S
54 设置[S]...

$ mnemonic and label for the "Paragraph" menu item
55 P
56 段落[P]

$ mnemonic and label for the "All" menu item
57 l
58 所有[l]

$ *
$ * Mnemonics and labels for the "Options" pulldown menu and items.
$ *
$ mnemonic and label for the "Format" pulldown menu
59 O
60 选项[O]

$ mnemonic and label for the "Overstrike" toggle button
61 O
62 覆盖[O]


63 W
$ ***UNUSED***	This label mnemonic are no longer used and need not be
$		localized.  They've been replaced by messages 111 and 112.
64 断行[W]

$ mnemonic and label for the "Status Line" toggle button
65 S
66 状态行[S]

$ *
$ * Late additions.
$ *

$ nmemonic and label for the "Table of Contents" item of the "Help" menu
$ ***UNUSED*** 	This nmemonic and label are no longer used and need not be
$		localized.  They've been replaced by messages 87 and 88.
71 C
72 内容目录[C]...

$ label for the "Save" item in the "File" pulldown menu
$ NOTE: This label is used in place of message 24 when text has been
$       changed but has not been saved
73 保存（必需的）

$ *
$ * Mnemonics and labels for the "Help" pulldown menu and items.
$ *
$ Mnemonic and label for the "Help" pulldown menu
81 H
82 提示[H]

$ mnemonic and label for the "Overview" item
83 v
84 概览[v]

$ mnemonic and label for the "Tasks" item
85 T
86 任务[T]

$ nmemonic and label for the "Table of Contents" item
87 C
88 内容目录[C]

$ mnemonic and label for the "Reference" item
89 R
90 参考[R]

$ mnemonic and label for the "On Item" item
91 O
92 针对项[O]

$ mnemonic and label for "Using Help" item
93 U
94 使用提示[U]

$ mnemonic and label for "About Text Editor" item
95 A
96 关于文本编辑器[A]


$ *
$ * Changes for "Edit" pulldown menu messages 45-50.
$ *

$ mnemonic and label for the "Select All" menu item
101 S
102 选择所有的[S]

$ mnemonic and label for the "Find/Change" menu item
103 F
104 查找/更改[F]...

$ mnemonic and label for the Check Spelling menu item
105 k
106 检查拼写[k]...

$ mnemonic and label for the "Copy As" File menu item
$ (should match the dialog title specified in message 42 of set 5)
107 A
$ ***UNUSED***	This nmemonic label is no longer used and need not localized.
108 复制成[A]...
$ mnemonic and label for the "Copy To File" File menu item
$ (should match the dialog title specified in message 42 of set 5)
109 F
110 复制到文件...

$ mnemonic and label for the "Wrap To Fit" toggle button
111 W
112 折行




$ ***********************************************************
$ **                                              helpCB.c **
$ **                                                       **
$ ** Messages and strings associated with online help.     **
$ **                                                       **
$ ***********************************************************
$set 12
$ Message used for "Help - On Item" if the user clicks outside application
1 您必须选择文本编缉器内的一个项。


$ ***********************************************************
$ **                                        ttMsgSupport.c **
$ **                                                       **
$ ** Messages and strings associated with ToolTalk.        **
$ **                                                       **
$ ***********************************************************
$set 13
1 检查点保存失败。
2 检查点回答失败。
$ NEW message after the SAMPLE IMPLEMENTATION.... more errors/warnings can
$ be detected while reading in a file via ToolTalk.
3 不能设置文本编辑器的内容目录 (未知原因)。
