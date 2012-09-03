
$ "$TOG: dtfile.msg /main/6 1998/04/29 15:13:12 rafi $"

$ *************************************<+>*************************************
$ *****************************************************************************
$ **
$ **  File:        dtfile.msg
$ **
$ **  Project:     Common Desktop Environment dtfile
$ **
$ **  Description:
$ **  -----------
$ **  This file is the source for the message catalog for dtfile
$ **  Any additional messages are to be added to this file by hand,
$ **  and the associated index number placed in the code.
$ **
$ *****************************************************************************
$ **
$ **     (c) Copyright 1990, 1991, 1992 Hewlett-Packard Company
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
$ 03/25/93 H. Amro            removed the guilty $ from message 30
$ 
$ 
$ 
$ *****************************************************************************

$set 2
$ ****** Module: ChangeDir.c ****(developer comment only)********

$ Messages 3 and 10 are titles for the Change Folder dialog, Message 3 is
$   used when the user has specified a title for dtfile windows
$ Messages 2 and 4 are labels for the Change Folder dialog
$ Message 1 is the title for Change Folder errors
$ Messages 7, 8, 9, 11, and 12 are Change Folder error messages

1 更改文件夹出错
2 新建文件夹：
3 更改文件夹
4 宿主机：
7 下列文件夹名是无效的。\n\n%s:%s
8 你不能切换到此文件夹。 只允许你观看\n\
你的当前文件夹下面的那些文件夹。\n\
你不能指定到新文件夹的绝对路径。

9 此显示处于受限的模式。你不能转至\n\
指定的文件夹，因为它不位于该受限文件夹\n\
的相对路径上。\n
10 文件管理程序－更改文件夹
11 不能获得新文件夹名。
12 请输入文件夹名。

13 ‘转至’出错
14 目的文件夹：
15 转至
16 系统名：
17 文件管理程序－转至
18 缺少目的文件夹名。\n\
   输入一个文件夹名，或从列表中选择一个文件夹。


$set 3
$ ****** Module: ChangeDirP.c ****(developer comment only)********

$ Messages 2-6 are labels for the folder Status Line
$ Message 3 is required for SUN systems
$ Message 2-9 are obsolete

2 %d 文件 %d 隐藏
3 %1$d 文件 %2$d 隐藏
4 %3d 文件 ...
5 正在读...
6 %d 隐藏

7 %d 项 %d 隐藏
8 %1$d 项 %2$d 隐藏
9 %3d 项 ...



10 %d 项
11 %d 项 ...
12 %d 项 %d 隐藏



$set 4
$set 5
$set 6
$set 7
$set 8



$set 9
$ ****** Module: File.c ****(developer comment only)********

$ Messages 6 and 7 are the title and text for an action error message
$ Messages 8 and 9 are rename error messages
$ Messages 11-19 are the title and text for warning messages displayed
$   when an object/set of objects is moved/copied/linked to a folder
$   containing object(s) with the same name

6 动作出错
7 没有为 %s 定义的动作。\n

8 此文件在该工作空间表示，而已经有一\n\
个具有此新名的文件存在于该工作空间。\n\
请选另一个名字。
9 已经存在一个此名的文件。\n\
请选另一个名字。

11 文件管理程序－移动/复制/链接警告
$ Messages 12-15 deal with multiple files/folders
12 下列文件（或文件夹）\n\
已经存在于\n\
%s：
13 \n复制操作将覆盖这些文件。\n\
	  你希望继续进行吗ⅶ\n
14 \n移动操作将覆盖这些文件。\n\
	  你希望继续进行吗ⅶ\n
15 \n链接操作将覆盖这些文件。\n\
	  你希望继续进行吗ⅶ\n

$ Messages 16&17 deal with a single file/folder
16 一个称为 \n%s\n 的对象已经存在于该文件夹中\n%s
17 一个称为 \n%s\n 的文件夹已经存在于此文件夹中\n%s

$ Labels in rename/replace and merge/replace dialogs
20 你想做什么ⅶ
21 替换已有对象
22 换名已有对象至：
24 合并两个文件夹的内容
25 替换已有文件夹

27 确定
28 取消
29 提示
30 取消移动
31 取消复制
32 取消链接

$ Window title for overwrite dialogs
33 文件管理程序－移动/复制/链接警告
34 文件管理程序－移动警告
35 文件管理程序－复制警告
36 文件管理程序－链接警告

37 对象换名－出错
38 %s\n已经存在：选择另一个名字。
39 %s\n不能换名至\n%s\n%s
40 对象替换- 出错

51 在复制
52 在移动
53 在链接
54 在更改
56 替换已有对象
57 通过添加一个数字换名已有对象
58 已复制
59 已移动
60 已链接
61 已更改
68 1 个对象 %s。
69 %d 个对象 %s。
70 %s\n%d 个对象与文件夹 %s \n中的对象具有相同的名字
71 %s\n1 个对象与文件夹 %s \n中的对象具有相同的名字
72 复制
73 移动
74 链接
75 操作
76 复制
77 移动
78 链接
79 操作
80 %s 取消了－－此文件夹不是 %s。\n\n你想替换的文件夹不能被删除，\n因为在那个文件夹中的一个对象不能被删除。\n在那个文件夹中的其它对象可能在 %s 进程\n停止前已经被删除。
81 %s\n\n此问题最有可能的原因是你不具有\n对那个不能被删除的对象的写权限。\n更改写权限，或者手工地换名你要 %s\n的文件夹，这样它就不会同名了。
82 %s 取消了\n\n%s\n不能被删除。\n\n此问题最有可能的原因是你不具有\n对那个不能被删除的对象的写权限。\n更改写权限，或者手工地换名你要 %s\n的文件夹，这样它就不会同名了。
83 %s 取消了－－没有一个对象是 %s。\n\n下列对象不能被自动地换名\n%s\n\n%d 个对象在 %s 进程终止前被换名了。
84 %s\n\n此问题最有可能的原因是你不具有\n对那个不能被删除的对象的写权限。\n更改写权限，或者手工地换名你要 %s\n的文件夹，这样它就不会同名了。
85 %s 取消了－－没有一个对象是 %s。\n\n下列对象不能被自动地换名\n%s\n\n1 个对象在 %s 进程终止前被换名了。
86 %s 取消了－－没有一个对象是 %s。\n\n你要替换的文件夹不能被删除。\n   文件夹：%s\n\n在此文件夹中的某些对象可能在 %s 进程\n停止前已经被删除。\n\n%d 个其它对象已被删除。\n\n此问题最有可能的原因是你不具有\n对那个不能被删除的对象的写权限。\n更改写权限，或者手工地换名你要 %s\n的文件夹，这样它就不会同名了。
87 %s 取消了－－没有一个对象是 %s。\n\n你要替换的文件夹不能被删除。\n  文件夹：%s\n\n在此文件夹中的某些对象可能在 %s 进程\n停止前已经被删除。\n\n1 个其它对象已被删除。\n\n此问题最有可能的原因是你不具有\n对那个不能被删除的对象的写权限。\n更改写权限，或者手工地换名你要 %s\n的文件夹，这样它就不会同名了。
88 %s 取消了－－没有一个对象是 %s。\n\n你要替换的文件不能被删除。\n   文件：%s\n\n%d 个其它对象已被删除。\n\n此问题最有可能的原因是你不具有\n对那个不能被删除的对象的写权限。\n更改写权限，或者手工地换名你要 %s\n的文件夹，这样它就不会同名了。
89 %s 取消了－－没有一个对象是 %s。\n\n你要替换的文件不能被删除。\n   文件：%s\n\n1 个其它对象已被删除。\n\n此问题最有可能的原因是你不具有\n对那个不能被删除的对象的写权限。\n更改写权限，或者手工地换名你要 %s\n的文件夹，这样它就不会同名了。
90 名字冲突。\n\
  该对象在工作空间的背景之外。\n\
  另一个在背景中的对象已经有一\n\
  个与你试图输入的相同的名字。\n\
  请选另一个名字。

91 停止复制
92 停止链接
93 停止移动
94 出错

$ "NEW-"
140 Rename new object to:

$ "NEW-"
141 Rename new objects by appending a number


$set 10
$ ****** Module: FileDialog.c ****(developer comment only)********

$ Messages 1, 2, 20, and 21 are titles for error dialogs
$ Message 3 is an error message for dialogs requiring new file names
$ Messages 5 and 27 are titles for the Copy File dialog, Message 5 is used
$   when the user has specified a title for dtfile windows
$ Messages 6-9 are labels for the File Copy, New Dir, and New File dialogs
$ Messages 10-13 and 25 are error messages for the file copy operation
$ Messages 14 and 28 are titles for the New Folder dialog, Message 14 is
$   used when the user has specified a title for dtfile windows
$ Message 16 is a label for the New Folder dialog
$ Messages 22-24 are error messages for the New File/Folder operations
$ Messages 26 and 29 are titles for the New File dialog, Message 26 is used
$   when the user has specified a title for dtfile windows
$ Messages 30 and 31 are titles for the Move File dialog, Message 30 is used
$   when the user has specified a title for dtfile windows
$ Messages 32 and 33 are titles for the Link File dialog, Message 32 is used
$   when the user has specified a title for dtfile windows

1 文件换名出错
2 文件复制出错

3 在文件或文件夹名中不允许有空格。

5 复制文件

6 当前文件名：
7 新建文件名：
8 文件类型
9 显示文件类型

10 原来选择的文件不再存在。
11 文件或文件夹不能有空名字。
13 无效的文件或文件夹名。

14 新建文件夹

16 新建文件夹名：

20 创建文件夹出错
21 创建文件出错

22 不能用空白名字创建一个文件或文件夹。
23 无效的文件夹名。
24 无效的文件名。

25 你不能用复制命令复制一个\n\
文件到工作空间。你必须使用\n\
在文件菜单中的‘放入工作空间’\n\
命令，或将文件拖放到工作空间。

26 新建文件

27 文件管理程序－复制文件
28 文件管理程序－新建文件夹
29 文件管理程序－新建文件

30 移动文件
31 文件管理程序－移动文件

32 链接文件
33 文件管理程序－链接文件
34 目的文件夹：
35 对象名字：
36 显示图标

37 选择的对象：
38 用于拷贝的名字：

39 换名出错
40 此对象是一个动作。\n\
   动作图标的标号不能被直接换名。
41 对此文件类型的正确图标已经显示。



$set 11
$ ****** Module: FileManip.c ****(developer comment only)********

$ Message 6 is the title for an error dialog
$ Messages 9, 14, 16, 28, and 30-36 are file manipulation error messages

6 文件操纵出错

9 你不能从废物箱中将文件\n\
放到工作空间。

14 不能找到文件夹的位置。
16 文件夹不能被移到它自己上。\n%s
28 %s 不能找到。
30 不能移动或换名文件夹 %s。\n\
所有的文件管理程序视窗显示的文件夹或它的子文\n\
件夹必须在一个文件夹可被移动或换名前关闭。
31 读 %s 时出错
32 不能换名 %s
33 不能打开 %s
34 你没有权限移动 %s\n\
然而，你可以复制此文件（用 <CTRL>\n\
同时拖动此文件）。
35 不能复制一个文件夹到它自己。
36 不能创建子进程。

37 你不能从废物箱中将文件\n\
放回工作空间。\n\
要从废物箱中删除一个对象，\n\
   －选择该对象，然后\n\
   －选取‘放回’\n\
从文件菜单或该对象的弹出菜单中。\n\
你接着可以将该对象拖出文件管理程序，\n\
并把它拖到工作空间幕布上。

38 你没有移动 %s 的权限。\n\
然而，你可以复制该对象。\n\
要复制一个对象：\n\
 －按下并按住 <Ctrl> 键，并\n\
 －用你的鼠标拖动该对象。\n\
或者\n\
 －使用在菜单栏的‘选择项’弹出菜单中的‘复制到’。

39 不能创建子进程。\n\
   已经到达本系统的最大进程数。\n\
   停止某些现在运行的进程或程序，\n\
   然后再试此函数。

40 源和目的对象是相同的。\n%s
41 文件夹\n%s\n不存在。
42 在你要复制到的设备上\n\
   没有可用的空间。
43 在你要移动到的设备上\n\
没有可用的空间。
44 没有移动操作作用在对象 %s 上。\n\
   你不能将一个对象移动到它所在的\n\
   文件夹中。在该对象能被移动前，\n\
   你必须输入一个不同的目的文件夹。
45 没有复制操作作用在对象 %s 上。\n\
   在可以建立一个拷贝前，你必须或者\n\
   更改目的文件夹，或者更改复制名。
46 没有链接操作作用在对象 %s 上。\n\
   在可以建立一个链接前，你必须或者\n\
   更改目的文件夹，或者更改复制名。

47 不能移动一个文件夹到它自己。


58 Process Create Error
59 Cannot create a child process to delete the dropped files.

$set 12
$ ****** Module: FileMgr.c ****(developer comment only)********

$ Message 3 is a dialog title
$ Messages 4-6 are movement warning or error messages
$ Message 7 is the title for the folder popup menus

3 移动文件

4 对象 %s 当前在文件夹 %s 中。\n\
你正移动该对象到文件夹 %s。\n\
你是想这样做吗?
5 对象 %1$s 当前在文件夹 %2$s 中。\n\
你正移动该对象到文件夹 %3$s。\n\
你是想这样做吗?
6 至少有一个拖动的文件来自不同的文件夹。\n\
你正移动该文件到文件夹 %s。\n\
你是想这样做吗?

7 文件管理程序

18 文件夹 %s 是空的。
19 文件夹 %s 不包括子目录。


$set 13
$ ****** Module: Filter.c ****(developer comment only)********

$ Messages 11 and 21-22 are button labels for the Modify Filter List dialog
$ Messages 19 and 24 are titles for the Link File dialog, Message 19 is used
$   when the user has specified a title for dtfile windows
$ Message 23 is a text field label for the Modify Filter List dialog
$ Message 25 is a label for the Modify Filter List dialog

11 默认

19 设置过滤器选项

21 选择所有的
22 去除选择所有的

23 过滤器字符串（可选的）：

24 文件管理程序－设置过滤器选项

25 选择要隐藏的文件类型：

26 也隐藏（可选的）：

27 显示

28 隐藏

29 也显示（可选的）：

30 选择要隐藏的数据类型：


31 选择数据类型进行:



$set 14



$set 15
$ ****** Module: Find.c ****(developer comment only)********

$ Message 1 is the title for an error dialog
$ Messages 14 and 15 are button labels for the Find Files dialog
$ Messages 23, 26, and 27 are Find Files error messages
$ Messages 29 and 41 are titles for the Find Files dialog, Message 29 is used
$   when the user has specified a title for dtfile windows
$ Messages 30-40 and 42 are labels for the Find Files dialog

1 查找文件出错

14 开始
15 停止

23 无效的查找信息\n\n\
必须指定一个要搜索的文件夹。
26 无效的查找信息。\n\n\
选择的文件不再存在。
27 没有找到匹配搜索数据的文件。

29 查找文件或文件夹

30 填入若干字段，指定要查找的文件：
31 文件或文件夹名：
32 文件内容：

$ Messages 33-35 are used on hpux systems
33 开
34 关
35 跟随链接：

36 打开新视图
37 放入工作空间
38 找到文件（按内容）：
39 找到文件（按名字和内容）：
40 找到文件（按名字）：

41 文件管理程序－查找文件或文件夹

42 搜索文件夹：

43 查找出错

44 缺少搜索文件夹名参数。\n\
打入你要搜索的文件夹名后开始。

45 选择的文件不再存在。\n\n\
某人在搜索过程结束后删掉了该文件。

46 查找

47 填入若干字段指定要查找的条目：

48 打开文件夹

49 文件管理程序－查找

50 搜索文件夹名或文件内容参数缺少\n\
输入你要搜索的文件夹名后开始。\n\
或输入你要搜索的字符串。


$set 16
$set 17



$set 18
$ ****** Module: Main.c ****(developer comment only)********

$ Message 1 is a blank line
$ Messages 2, 6, 17-18 and 27-28 are error messages which will ONLY APPEAR
$   in the DT ERRORLOG FILE
$ Messages 16 and 32 are titles for the Save Settings dialog, Message 16 is
$   used when the user has specified a title for dtfile windows
$ Messages 19-21 are error messages for invalid directory input.
$ Message 22 is the title for an error dialog
$ Messages 25 and 26 are usage strings
$ Messages 29 and 30 are text for the Save Settings dialog
$ Messages 34 and 35 are drag/drop error messages

$ DO NOT TRANSLATE or CHANGE or LOCALIZE MESSAGE 1
1 

$ MESSAGE 2 WILL ONLY APPEAR IN THE DT ERRORLOG FILE
2 不能创建 .dt 文件夹或子文件夹。

$ MESSAGE 6 WILL ONLY APPEAR IN THE DT ERRORLOG FILE, and stderr
6 X 工具箱出错 ... 退出。\n

16 保存为默认选项

$ MESSSAGES 17-18 WILL ONLY APPEAR IN THE DT ERRORLOG FILE
17 不能创建文件以存储默认选项。
18 不能打开会话过程文件。

19 文件夹规格描述，\n\
%s\n\
 不是一个文件夹。
20 文件夹规格描述，\n\
%s\n\
不存在。
21 无效的文件夹规格描述，%s:%s。

22 拖出错

23 \n用法：%s...\n\n\
   -noview\n\n\
\tDtfile 在服务器模式运行。\n\n\
   -session SessionFile\n\n\
\tDtfile 以在 SessionFile 参数中指定的会话过程\n\
\t文件运行。\n\n\
   -folder 文件夹[,文件夹,文件夹]\n\
   -dir 文件夹[,文件夹,文件夹]\n\n\
\tDtfile 对每一在 -folder 参数中指定的文件夹\n\
\t显示一个窗口。-folder 参数可以包含多个以逗号\n\
\t分隔的文件夹。文件夹可以是这样的形式：\n\
\t‘路径’。\n\n\
   -title 标题\n\n\
\tDtfile 使用在标题参数中指定的字符串作为\n\
\t它的窗口的标题。\n\n\
   -help_volume 提示_卷\n\n\
\tDtfile 使用在‘提示_卷’参数中指定的提示卷。\n\n\
   -restricted\n\n\
\tDtfile 将不显示在受限的文件夹之上的文件夹。\n\
\t如果 -dir 选项使用了，在此选项中指定的文件夹\n\
\t就是受限的文件夹。如果 -dir 选项没有使用，\n\
\t该用户当前的文件夹就是受限的文件夹。\n\n\
   -grid on/off\n\n\
\ton     = 文件以网格模式显示。\n\
\toff    = 文件以正常模式显示。\n\n\
   -tree on/off\n\n\
\ton     = 文件以单文件夹模式显示。\n

24 \toff    = 文件以文件夹树模式显示。\n\n\
   -tree_files /never/always/choose\n\n\
\tnever  = 树模式有两个状态：部分展开的或收缩的。\n\
\talways = 树模式有两个状态：完全展开的或收缩的。\n\
\tchoose = 树模式有三个状态：部分展开的、完全展开\n\
\t的或收缩的。\n\n\
   -order alphabetical/file_type/date/size\n\n\
\t文件以指定的次序显示：字母序、按文件类型、\n\
\t按日期、按大小。\n\n\
   -view no_icon/large_icon/small_icon/attributes\n\n\
\t文件以指定的格式显示：仅文本、文本和大图标、\n\
\t文本和小图标、带有属性。\n\n\
   -direction ascending/descending\n\n\
\t文件以指定的方向显示：升序（ascending）或\n\
\t降序（descending）。\n\n\
   -large_icon_width <大小>\n\n\
   -large_icon_height <大小>\n\n\
   -small_icon_width  <大小>\n\n\
   -small_icon_height <大小>\n\n\
	   文件管理器中图标图像的显示区域大小\n\
	   大于此尺寸的图标图像将被切割到此大小\n\
	   默认的显示区域大小对大的是 38，对小的是 24\n\n
26 \n受限的模式的用法：%s...\n\n\
   -folder 文件夹[,文件夹,文件夹]\n\
   -dir 文件夹[,文件夹,文件夹]\n\n\
\t其中文件夹是用户起始文件夹之下（与/或包含\n\
\t起始文件夹）的文件夹。\n\n

$ MESSSAGES 27-28 WILL ONLY APPEAR IN THE DT ERRORLOG FILE
27 不能恢复保存的默认起始设置文件，将使用默认的。\n
28 不能恢复保存的默认工具设置文件，将使用默认的。\n

$ Messages 29-30 are the strings displayed in a question dialog
29 保存当前文件管理程序视窗的窗口大小，\n\
偏好，以及过滤器设置ⅶ\n\n\
它们将被任一从前端面板启动的新的文件管理程序\n\
视窗窗口所使用。
30 保存当前受限的文件管理程序视窗的窗口大小，\n\
偏好，以及过滤器设置ⅶ\n\n\
它们将被任一从前端面板启动的新的受限的文件\n\
管理程序视窗窗口所使用。

32 文件管理程序－保存为默认选项

34 你不能从废物箱里复制出或链接一个文件\n\
文件夹。
$ 35 你不能从废物箱里复制出或链接一个文件\n\
$ 文件夹。

35 保存当前应用程序管理程序视窗的窗口大小，\n\
偏好，以及过滤器设置ⅶ\n\n\
它们将被任一从前端面板启动的新的应用程序管理程序\n\
视窗窗口所使用。

36 你不能从废物箱中复制出或链接一个文件或文件夹。\n\
将该对象移出废物箱并把它放到文件管理程序中。\n\
你可以从那里复制或链接它。

37 你不能从废物箱里复制出或链接一个文件或文件夹。\n\
将该对象移出废物箱并把它放到文件管理程序中。\n\
你可以从那里复制或链接它。




$set 19



$set 20
$ ****** Module: Menu.c ****(developer comment only)********

$ Messages 1-8, 123 and 9 are the text and mnemonics for the dtfile menu bar
$ Messages 10/11, 128/129, 124/125, 121/122, 126/127, 16/17, 18/19, 84/85,
$   90/91, 22/23, 117/118, are the text and mnemonics for the Fill 
$   pulldown menu 
$ Messages 10/11, 30/33, 92/93, 34/37, 38/41, 42/94, and 97/98 are the text
$   and mnemonics for the Folder pulldown menu
$ Messages 46/47, 99/100, 48/51, 52/55, 101/102, 103/104, 119/120, and 
$   62/63 are the text and mnemonics for the View pulldown menu
$ Messages 105-116 are the text and mnemonics for the Help pulldown menu

1 文件[F]
2 F
3 文件夹[D]
4 D
5 视窗[V]
6 V
7 选择项[S]
8 S

9 H

10 新建[N]...
11 N
16 特性[P]...
17 P
18 查找[F]...
19 F
22 显示废物箱[S]...
23 S
30 更新[U]
33 U
34 变更为[C]
37 C
38 至起始目录[H]
41 H
$ NOTE: The mnemonic for Up is Message 94 in this set.
42 向上[U]
46 新建[N]
47 N
48 选择所有的[S]
51 S
52 去除所有的选择[D]
55 D
62 保存所有的特性[t]...
63 t
84 放入工作空间[W]
85 W
90 删除到废物箱[T]
91 T
92 快速更改至[F]
93 F
$ NOTE: Message 94 is the mnemonic for the "Up" menu item
94 U
97 终端[T]
98 T
99 清理[C]
100 C
101 显示隐藏文件[H]
102 H
103 修改过滤器列表[F] ...
104 F
105 概览[v]
106 v
107 任务[T]
108 T
109 参考[R]
110 R
111 针对项[O]
112 O
113 使用提示[U]
114 U
115 关于文件管理程序[A]
116 A
117 关闭[C]
118 C
119 设置视窗特性[P] ...
120 P
121 复制[y]...
122 y
123 提示
124 移动[M]...
125 M
126 链接[L]...
127 L
128 换名[n]
129 n
130 新建文件夹[N]...
131 N
132 新建文件[w]...
133 w
134 转至[G]...
135 G
136 打开废物箱
137 换名...
138 打开新视窗[O]
139 O
140 作为链接而复制...
141 设置视窗选项...
142 保存为默认选项[S]...
143 S
144 设置过滤器选项[C]...
145 C
146 打开终端[O]
147 O
148 内容目录[C]
149 C
150 更改权限...
151 放进废物箱
152 移到...
153 复制到...
154 作为链接复制...
155 向上
156 显示隐藏的对象
157 关于应用程序管理器

$set 21
$ ****** Module: ModAttr.c ****(developer comment only)********

$ Messages 1 and 21 are titles for the File Properties dialog, Message 1 is
$   used when the user has specified a title for dtfile windows
$ Messages 2-13 and 18 are labels for the File Properties dialog
$ Message 14 is the output of the file modification date; DO NOT LOCALIZE
$ Messages 15 and 16 are file properties error messages
$ Message 17 is the title for an error dialog
$ Message 19 is the default file owner/group, if real one can't be determined
$ Messages 22-28 are error messages that can be generated when dtfile
$   tries to use the dtfile.config configuration file, this file is
$   used to customize the File Properties dialog

1 文件权限

2 文件名：
3 属主名：
4 组名：
5 权限：
6 读
7 写
8 执行
9 属主：
10 组：
11 其它：
12 大小（字节数）：
13 修改的：

$ DO NOT LOCALIZE MESSAGE 14
14 %x %X

15 无效的文件属性信息。\n\n\
指定的用户不存在。
16 无效的文件属性信息。\n\n\
指定的组不存在。

17 特性出错

18 链接到：

19 未知的

21 文件管理程序－文件权限

22 不能打开文件管理程序配置文件：
24 文件管理程序配置文件中未知的字段标号：
25 在文件管理程序配置文件中没有找到文件系统标识符的信息
26 不能为文件系统对话创建进程
27 执行文件系统对话时出错
28 试图执行：

$ do not localize "fsDialog"
29 在 dtfile 的配置文件中没有为 fsDialog 提供值。

30 名字：

31 属主名无效。\n\
指定的用户不存在。

32 组名无效。\n\n\
在你的系统中没有创建此名字的用户组。\n\
如果你想创建此名字的用户组，请使用系统管理软件。


33 权限出错

34 文件管理程序－权限


38 File Manager Error
39 File Open Error

$set 22
$ ****** Module: ModAttrP.c ****(developer comment only)********

$ Messages 1 and 3 are file properties error messages
$ Message 2 is the title for the properties error dialog

1 不能打开文件，操作终止
2 特性出错
3 不能为 %s 更改特性
4 不能打开对象。
5 设置权限出错



$set 23
$ ****** Module: Prefs.c ****(developer comment only)********

$ Messages 1 and 22 are titles for the Set Preferences dialog, Message 1 is
$   used when the user has specified a title for dtfile windows
$ Messages 2-20 and 23-29 are labels for the Set Preferences dialog

1 设置视窗选项

2 视窗
3 按名字
4 按名字和图标
5 按名字和小图标
6 按名字，日期，大小 ...
7 显示
8 按单个文件夹
9 按树
10 次序
11 按文件类型
12 字母的
13 按日期
14 按大小
15 方向
16 升序
17 降序
18 布局
19 分类的网格
20 如所布置的

22 文件管理程序－设置视窗选项

23 首标
24 图标的路径
25 当前文件夹
26 状态行
27 仅文件夹
28 文件夹，然后文件
29 文件夹和文件
30 默认

31 表示
32 仅按名字
33 按大图标
34 按小图标
35 行和列
36 文本路径
37 消息行


$set 24
$set 25
$set 26



$set 27
$ ****** Module: Trash.c ****(developer comment only)********

$ Messages 3, 4, and 81 are titles for error warning dialogs
$ Message 5-8, 56, 58, 78, 82, and 83 are trash error messages
$ Message 9 is the title of the trash container
$ Messages 24/26 and 28/30 are the text and mnemonics for the File pulldown
$   menu
$ Message 73 is an error dialog title
$ Messages 74, 75, 79, and 80 are trash warning messages

3 废物箱出错
4 废物箱警告

5    下列文件或文件夹不能放到废物箱中：    \n
6    下列每一个或者是非空的文件夹，   \n   \
或者是你没有写权限的文件。\n   \
你想接着进行吗ⅶ\n
7    你没有下列文件的写权限。  \n   \
你想接着进行吗ⅶ\n
8    下列文件夹不空。 \n   \
你想接着进行吗ⅶ\n

9 废物箱

24 放回[P]
26 P
28 销毁[h]
30 h

56    下列文件不能放回：\n
$ Message 58 supplements Message 56
58 （加上 %d 个另外的文件）

73 销毁文件

74 下列文件还在工作空间里：\n\
   %s\n\
把文件移到废物箱将会从使它从工作空间中删除。

75 下列文件还在工作空间中：\n\
   %s\n\
把文件移到废物箱将会从使它从工作空间中删除。


78 文件管理程序不支持名字中有空格的文件或\n\
文件夹，所以不能删除它们。\n


79 工作空间文件 %s 还在你的文件系统中，位于：\n\
   %s\n\
把文件移到废物箱将会从使它从文件系统中删除。


80 工作空间对象还在你的文件系统中，位于：\n\
   %s\n\
把文件移到废物箱将会从使它从文件系统中删除。


81 销毁文件/文件夹出错

82 下列文件不能从文件系统中删除：  \n

83 在 $HOME/.dt/Trash 中的 .trashinfo 文件不能被打开。\n\
   你需要访问它才能使废物箱工作。\n\
   检查那个文件和它的文件夹的权限。\n
84 当废物箱对象被销毁，它们就永远地从你的系统中\n\
   删除了。\n\n\
   你确实要销毁吗ⅶ\n
85 你要删除的工作空间对象是一个在文件管理程序里的\n\
   %s\n\
   的链接复制。\n\
   如果你按下‘确定’，该链接和原始对象都将\n\
   被删除。\n\
   如果你只想删除该工作空间对象，\n\
	  1) 按一下此对话上的‘取消’\n\
	  2) 然后从该工作空间对象的弹出菜单\n\
		 选择‘从工作空间中删除’。
86 你要删除的工作空间对象是一个在文件管理程序里的\n\
   %s\n\
   的链接复制。\n\
   如果你按下‘确定’，该链接和原始对象都将\n\
   被删除。\n\
   如果你只想删除该工作空间对象，\n\
	  1) 按一下此对话上的‘取消’\n\
	  2) 然后从该工作空间对象的弹出菜单\n\
		 选择‘从工作空间中删除’。
87 在废物箱中的对象不能被打开。\n\n\
   要打开一个对象，使用‘放回’将它返回到\n\
   文件管理程序中，然后在那儿打开它。

88 不能写到一个临时文件。\n\
   也许你的文件系统满了。

89 不能打开废物箱目录\n\
   %s\n\
  （鉴别，访问权限问题）\n\n\
   所有的废物箱操作将不被执行。

90 ‘废物箱’不能被初始化。\n\
   请检查：\n\
	1)$HOME/.dt/Trash 的权限。\n\
	2)可用的磁盘空间。\n\
		3)鉴别。

$ 91 is not used
91 下列对象的一个链接复制在\n\
工作空间幕布上：\n\
将该对象移到废物箱将使它既从\n\
文件管理程序也从工作空间幕布中删除。

$ 92 is not used
92 下列对象的一个链接复制在\n\
工作空间幕布上：\n\
将该对象移到废物箱将使它既从\n\
文件管理程序也从工作空间幕布中删除。

93 不能访问此废物箱信息文件：\n\
   %s\n\
   所有的废物箱操作将不被执行。\n\
   最可能的原因是：\n\
    －网络鉴别。\n\
	－磁盘空间不足。\n\
	－错误的 $HOME/.dt/Trash 权限。

94 文件管理程序不支持名字中有空格的对象，\n\
所以我们不能删除它们。\n\
要删除这些对象：\n\
 －从文件管理程序的菜单栏中选择‘打开终端’\n\
 －然后发出删除命令删除它们。

95 不能访问废物箱目录：\n\
   %s\n\
   所有的废物箱操作将不被执行。\n\
   最可能的原因是：\n\
    －网络鉴别。\n\
	－磁盘空间不足。\n\
	－错误的权限。

96 下列对象不能从文件系统中删除：  \n

97 下列对象不能放到废物箱中：    \n

98 （加上 %d 个另外的对象）

$ message 99 is not used
99 你没有权限写下列文件：\n\
你要进行吗ⅶ\n

100 下列文件夹不空。\n\
你想接着进行吗ⅶ\n

101 下列对象不能放回：\n

102 下列对象的一个链接复制在\n\
工作空间幕布上：\n\
%s\n\
将该对象移到废物箱将使它既从\n\
文件管理程序也从工作空间幕布中删除。

103 下列对象的一个链接复制在\n\
工作空间幕布上：\n\
%s\n\
将该对象移到废物箱将使它既从\n\
文件管理程序也从工作空间幕布中删除。

104 你没有权限写下列文件：\n\
你想接着进行吗ⅶ\n

105 废物对象的默认动作将不被执行。\n\n\
	要执行此对象的默认动作，使用‘放回’\n\
	操作将它返回到文件管理器，然后在那儿\n\
	执行它。

106 关于废物箱



$set 28
$ ****** Module: Desktop.c ****(developer comment only)********

$ Messages 1-3 Workspace error messages
$ Message 4 is the title for the Workspace popup menus
$ Messages 5-8 are the labels and mnemonics for the Workspace obj popup menu

1 已有一个此名字的文件存在于工作空间上。\n\
请选另一个名字。
2 已有一个此名字的文件存在于原始文件夹上\n\
（此文件出自该文件夹）。\n\
请选另一个名字。
3 一个称为 %s 的文件已经存在于此工作空间的工作空间上。\n\
你不能将另一个同名的文件放到此工作空间上。\n\
要想把两个文件都放到此工作空间，将其一换名。

4 工作空间对象

5 从工作空间中删除[R]
6 R
7 打开父亲文件夹[O]
8 O

9 已经有一个此名字的对象在此工作空间上。\n\
请选另一个名字。

10 已经有一个此名字的对象在原始文件夹中\n\
（此对象出自该文件夹）。\n\
请选另一个名字。

11 一个称为 %s 的文件已经存在于此工作空间的幕布上。\n\
你不能将另一个同名的文件放到此工作空间上。\n\
要把两个文件都放到此工作空间，将其一换名。



$set 29
$ ****** Module: Help.c ****(developer comment only)********

$ Message 1 is the title for the dtfile help dialog
$ Messages 1 and 2 are titles for the Help dialog, Message 2 is
$   used when the user has specified a title for dtfile windows
$ Message 3 is the text for help on the "Open New View" action
$ Message 4 is the text for help on the "Open In Place" action
$ Messages 5-6 are help error messages

1 文件管理程序提示
2 %s 提示

3 ‘%s’动作打开一个请求的文件夹的新文件管理程序视窗。

4 ‘%s’动作显示请求的文件夹于当前文件管理程序视窗中。

5 对 %s 动作无提示。
6 对 %s 文件类型无提示。

7 ‘%s’动作打开一个请求的文件夹的新文件管理程序\n\
视窗。

8 ‘%s’动作显示请求的文件夹于当前文件管理程序\n\
视窗中。

9 废物箱提示

$set 30
$ ****** Module: SharedMsgs.c ****(developer comment only)********

$ The following messages are used in more than one place within the
$ dtfile source.  Unless otherwise specified, they are error messages.

1 不能从 %s 中读取。

$ Message 2 is used to format the file attributes in 'by attributes' mode
2 %b %d %H:%M:%S %Y

3 不能移动或复制文件 %s －从宿主机 %s上。\n\
宿主机 %s 可能没有正确地安装到你的文件系统上。
4 不能移动或复制文件 %1$s －从宿主机 %2$s 上。\n\
宿主机 %3$s 可能没有正确地安装到你的文件系统上。
5 不能读 %s
6 不能在文件夹 %s 中写
7 不能创建 %s
8 不能删除 %s
9 一个具有名字 %s\n\
  的文件夹或文件已经存在 \n\
  并不能被覆盖。\n\
  输入一个不同的名字。

$ Message 10 is the help topic string for help on a file icon or action
10 文件类型/动作的描述

11 你不能指定路径作为文件名的一部分。
12 无效的查找信息。\n\n\
下列文件夹不存在，或不可访问：\n\n\
     %s\n

$ Messages 13-16 are dialog titles
13 %s 概览
14 %s 拓导
15 关于 %s
16 文件管理程序

$ Message 18 is appended to ".." folders
18 （向上）

$ Messages 19 and 20 are messages which appear when a user clicks item help
$ outside the application.  19 is the title, while 20 is the message.
19 针对项提示出错
20 你必须选择文件管理程序中的\n\
一个部件。

$ Messages 21 and 22 are titles for error dialogs
21 文件移动出错
22 文件链接出错
23 不能创建‘%s’\n\n\
最常见的原因是你不具有\n\
正确的权限来引用\n\
文件或文件夹。\n\n\
要观看权限，选择该对象，\n\
然后选择‘更改权限...’

24 你必须选择在此文件管理程序中的一个对象。\n\
‘针对项’提示只在你启动该应用程序的地方\n\
起作用。

25 不能创建‘%s’\n\n\
最常见的原因是你不具有\n\
正确的权限来引用\n\
文件或文件夹 。\n\n\
要查看权限，选择该对象，\n\
然后从‘选择项’或弹出\n\
菜单中选择‘更改权限...’。

26 你必须在应用程序管理器中选择一个对象。\n\
‘针对项’提示只在你启动该应用程序的地方起作用。


$set 31

$set 32
$ ****** Module: dterror.src ****(developer comment only)********
$ Message 1 is a title for dterror
1 dt 出错
2 文件管理器打开目录出错

$set 33
1 多个对象
2 当前文件夹

$set 99
$ ****** Version Information ********
$ DO NOT TRANSLATE THESE MESSAGES
$ DO NOT CHANGE THESE MESSAGES
$ DO NOT LOCALIZE THESE MESSAGES
$ These messages are used for the version information.
1 @(#)version_goes_here
2 \n@(#)_DtMessage catalog source $TOG: dtfile.msg /main/6 1998/04/29 15:13:12 rafi $
