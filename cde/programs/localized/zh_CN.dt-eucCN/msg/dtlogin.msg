
$ "$XConsortium: dtlogin.msg /main/3 1995/11/08 13:51:31 rswiston $"

$ *************************************<+>*************************************
$ *****************************************************************************
$ **
$ **  File:        dtlogin.msg
$ **
$ **  Project:     CDE dtlogin
$ **
$ **  Description:
$ **  -----------
$ **  This file is the source for the message catalog for dtlogin
$ **
$ **
$ *****************************************************************************
$ **
$ **     (c) Copyright 1990 Hewlett-Packard Company
$ **     All Rights reserved
$ **
$ **
$ *****************************************************************************
$ **
$ **
$ **  ------------------------- MODIFICATION RECORD --------------------------
$ *
$ * 
$ **  ----------------------- MODIFICATION RECORD END ------------------------
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

$set 1
$ set 1 is labels
$ 
$ messages 1 through 6 are button labels on the login screen

1 请输入你的用户名
2 请输入你的口令
3 确定
4 重新开始
5 选项
6 提示

$ 
$ messages 7 through 11 are options button menu items
$ 

7 复位登录屏幕
8 命令行登录
9 版本...
10 单窗口会话过程
11 语言

$ 
$ messages 12 through 14 are miscellaneous button labels
$ 
12 启动桌面环境
13 取消
14 提示


$ 
$ message 15, 17 are more option button menu items 
$ message 16 is the welcome message
$ 
15 DT Lite 会话过程
16 欢迎来
17 正规桌面环境

$ 
$ message 18-24
$ 
18 * 挂起桌面登录...\r\n*\r\n
19 * 请按回车键出现登录提示。\r\n*\\rn
20 * 登录。桌面登录将在你注消后很快恢复。\r\n*\r\n
21 * 正在恢复桌面登录...\r\n*\r\n
22 * 正在启动桌面登录于显示 %1$s 上...\r\n*\r\n
23 * 在登录前等待桌面登录屏幕。\r\n*\\rn
24 * X-服务器不能在显示 %1$s 上启动...\r\n*\r\n

$ messages 25 is a options button menu item
25 会话过程

$ 
$ 26 replaces 16 as the greeting after a user name has been 
$ entered. The %s is replaced by the user name.
$ 
26 欢迎 %s

$ 
$  Sun specific message catalog id numbers in MC_LABEL_SET messages.
$  (Starting at a random uneven higher number, 73, to allow some room
$  for some shared sample source message additions.
$ 
73 用户的上一个桌面环境

$set 2
$ set 2 is error messages
$ 

1 登录不正确；请再试一次。
2 不能改变到起始目录。
3 对不起。已经有最大数量的用户登录了。
$ message 4 is a lengthy message that should be localized to correspond to 
$ the language.  
4 \n\
你需要为你的帐户选择一个初始口令，\n\
或者你当前的口令已经过期。\n\
\n\
在设置你的口令后，你需要用新的口令\n\
重新登录。
5 登录出错，无效的用户标识符。
6 登录出错，无效的组标识符。
7 登录出错，无效的审计标识符。
8 登录出错，无效的审计标识符。
9 登录当前被禁止。
10 只有根能运行 %1$s\n
11 从 %1$s 中重建默认语言表\n
12 重新扫描配置和服务器文件\n
13 重读配置文件 %1$s\n
14 重读服务器文件 %1$s\n
15 重读访问文件 %1$s\n
16 dtlogin：在例程中存储器不够\n


$set 3
$ set 3 is help text
$ 

$ message 1 is generated when the help button on the login screen is clicked.
1 \n\
登录提示：\n\
\n\
使用登录屏幕使计算机识别你。\n\
\n\
要登录：\n\
	   1) 输入用户名，然后按 <Enter> 键或按一下【确定】。\n\
 	      （如果你还没有创建用户，输入“root”作为用户名）\n\
	   2) 输入口令，然后按 <Enter> 键或按一下【确定】。\n\
\n\
[重新开始]\n\
   擦除输入以便重新输入用户名，然后\n\
   再试一次。\n\
\n\
[选项]\n\
   允许你改变会话过程选项：\n\
\n\
   用鼠标指针指向【选项】按钮，按下并保持\n\
   住鼠标按钮１。在选择项之间拖动指针。\n\
   在某个项上放开鼠标按钮将会选择该项。\n\
   你可以选择：\n\
\n\
   语言－－更改会话过程语言\n\
   会话过程－－选择你喜欢的桌面环境\n\
     OpenWindows 桌面环境 - OpenWindows \n\
     公用桌面环境 - CDE \n\
     用户的上一个桌面环境 - 上一次退出登录时的环境 \n\
     安全会话过程 - 单终端窗口环境\n\
   命令行登录－－切换到命令行环境\n\
   复位登录屏幕－－重新启动 X-服务器\n\
\n\
在背景的任何地方按一下鼠标按钮就可以\n\
刷新屏幕，例如，如果系统消息使屏幕变\n\
得不可读。\n\
\n\
要获得更多的提示信息，首先登录，然后在前端面板\n\
中的提示管理程序图标上按一下（带有问号的书）。\n\
然后查找登录管理程序提示卷。\n\

$ message 2 is generated when you attempt to start a system whose id is
$ set to unknown or if it is not configured for networking.
2 \n\
你的工作站没有唯一的名字。\n\
它当前被设置成“unknown”。\n\
建议你命名此工作站。\n\
\n\

$ message 3 is generated when the help button on the chooser screen is clicked.
3 \n\
选择器提示：\n\
\n\
使用选择器屏幕选择你要登录进入的服务器。\n\
\n\
选择器屏幕包含一个列表，其中每一项有两部分。\n\
第一部分（服务器名）包含每一可用的服务器的\n\
网络名。第二部分（服务器信息）将包含不同的\n\
信息，取决于每一服务器上运行的登录管理程序。\n\
如果服务器运行的是 CDE 桌面，它将提供有关\n\
当前使用的用户数（或 tty 设备），以及最近\n\
5、10、15分钟的平均负载（平均 CPU 使用率）。\n\
\n\
要选择一个要登录进入的服务器，只需选择该列表中包含\n\
你希望的服务器的行。\n\
\n\
一旦你做出选择，按一下【确定】按钮。\n\
这将带给你一个你所选择的服务器的登录屏幕。\n\
\n\
[更新列表]\n\
   这将清除该列表，并查询网络上的服务器\n\
   它们是否允许登录。\n\
\n\
[选项]\n\
   允许你更改会话过程选项：\n\
\n\
   用鼠标指针指向【选项】按钮，按住\n\
   鼠标按钮１。在选择项之间拖动指针。\n\
   在某个项上放开鼠标按钮将会选择该项。\n\
   你可以选择：\n\
\n\
   语言－更改会话过程语言\n\
   复位登录屏幕－重新启动 X-服务器\n\
\n\
在背景的任何地方按一下鼠标按钮就可以\n\
刷新屏幕，例如，如果系统消息使屏幕\n\
变得不可读。\n\
\n\


$set 4
$ set 4 is log error messages
$ 

1 访问文件“%1$s”，宿主机“%2$s”没有找到\n
2 ReadHostEntry\n
3 访问文件 %1$2s，显示 %1$2s 未知\n
4 不能打开访问控制文件 %1%s，将没有 XDMCP 请求被许可\n
5 Domain/OS 鉴定出错：%1$s (%2$s/%3$s)
6 不能打开服务器特许文件 %1$s\n
7 不能写服务器特许文件 %1$s\n
8 saveAddr
9 获取网络接口配置困难
10 setAuthNumber
11 不能锁定特许文件 %1$s 或备份 %2$s\n
12 不能移动特许\n
13 不能 创建/锁定 进程标识符文件 %1$s\n
14 不能锁定进程标识符文件 %1$s，另一个 Dtlogin 正在运行（pid %2$d）\n
15 不能访问服务器文件 %1$s\n
16 NewDisplay
17 不能打开出错登记文件 %1$s\n
18 TrimErrorLog()：出错登记文件最大的大小为 200Kb。\n
19 在服务器文件中缺少显示名\n
20 缺少 %1$s 的显示类型\n
21 宿主机名 %1$s 是无效的。 将显示设置为‘:0’\n
22 不可接收的显示类型 %1$s－用于显示 %2$s\n
23 伪复位时间到。\n
24 GetResource
25 没有空间用于参数重新分配\n
26 不能打开配置文件 %1$s\n
27 命令行中额外的参数：
28 StartServer()：没有参数\n
29 服务器 %1$s 不能执行\n
30 StartServer() fork 失败，睡眠\n
31 服务器意外死亡\n
32 XOpenDisplay(%1$s) 挂起尝试 #%2$d 次，异常退出。\n
33 服务器打开尝试 #%1$d 次，由于 %2$s 而失败，放弃\n
34 致命的 IO 出错 %d（%1$s）\n
35 X 出错\n
36 会话过程启动失败\n
37 警告：不能得到显示 %1$s \n
38 警告：不能得到在显示 %1$s 上的键盘\n
39 BLS－不能获得帐户信息。\n
40 BLS－不能获得受保护的帐户信息。\n
41 BLS－不能验证灵敏度级别。\n
42 BLS－不能改变到用户：%1$s。\n
43 AFS－setgroups() 在启动会话过程时失败\n
44 Setuid 对用户 %s 失败，errno = %d\n
45 没有起始目录 %1$s 用于用户 %2$s，使用/\n
46 会话过程执行失败 %1$s\n
47 会话过程没有命令/参数\n
48 不能为 %s 启动会话过程，fork 失败\n
49 Dtlogin 不能杀死客户\n
50 不能执行 %1$s\n
51 不能产生子进程执行 %1$s\n
52 不合适的鉴定名“%1$s”。使用 AFS，Kerberos 或默认值。\n
53 不能执行 Dtgreet %1$s－为 %2$s\n
54 不能产生子进程执行 Dtgreet %1$s\n
55 在端口 %1$d 创建 Socket 失败\n
56 绑定套接字地址 %1$d 出错，errno = %2$d\n
57 makeEnv
58 setEnv
59 parseArgs
60 扫描 %1$s 目录超过 %2$d 秒。终止扫描...\n
61 %s 属于超过 %1$d 组，%2$s 被忽略\n
62 audwrite() 出错：调用者不是超级用户\n
63 audwrite() 出错：审计记录中无效的事件号。\n
64 audwrite() 出错：errno = %1$d\n
65 AFS 鉴定失败：%1$s\n
66 %1$s 的帐户已经过期。请查问你的系统管理员。\n
67 服务器死亡 %1$s\n
68 不能打开标志位图文件：%1$s\n
69 不能读“%1$s”消息目录。使用默认语言。
70 显示 %1$s 不能被初始化，服务器可能被占有。\n
71 (%1$s)：登录边框宽度超过显示，使用默认的。 边框的单位是象素。\n
72 (%1$s) 登录边框高度超过显示，使用默认的。 边框的单位是象素。\n

$set 5
$ set 5 is for other stuff specific to chooser.
1 选择一个要登录进入的服务器：
$ 1 is the title/instructions for the chooser dialog
2 服务器名                       服务器信息
$                                ^
$                                | This needs to start in this column
$ 2 is the headings for the list
3  更新列表

$set 99
$    DO NOT TRANSLATE THESE MESSAGES
$    DO NOT CHANGE THESE MESSAGES
$    DO NOT LOCALIZE THESE MESSAGES
$    These messages are used for the version information.
$quote "
1 "@(#)version_goes_here"
2 "\n@(#)_DtMessage catalog source $XConsortium: dtlogin.msg /main/3 1995/11/08 13:51:31 rswiston $"

