$ $XConsortium: dthelpgen.msg /main/3 1995/11/08 13:50:38 rswiston $
$ *****************************************************************************
$ 
$ ***** NOTE FOR MESSAGE CATALOG TRANSLATORS *****
$ 
$ There may be three types of messages in this file:
$ 
$  1. Messages that appear in dialogs or are displayed to the user.
$ 
$     These messages are the default and they should ALL BE LOCALIZED.
$     Note that these messages do NOT have any identification (see the
$     comments for type 2 and 3 below).
$ 
$  2. Messages that only appear in the DT error log file ($HOME/.dt/errorlog).
$ 
$     The localization of these messages is OPTIONAL.  These messages are
$     identified by the following:
$ 
$  MESSAGES xx-yy IN SET zz WILL ONLY APPEAR IN THE DT ERRORLOG FILE
$ 
$  3. Messages that should not be localized.
$ 
$     These messages are identified by the following:
$ 
$  DO NOT TRANSLATE or CHANGE or LOCALIZE MESSAGES xx-yy from set zz
$ 
$ ***** END (NOTE FOR MESSAGE CATALOG TRANSLATORS) *****
$ 
$ ******************************************************************************
$ 
$set 1
$ 
$ _DtMessage 1 is the usage message
$ 
$ Localize only the words 'directory', 'name', and 'language'.
$ 'directory' means the directory name in which the resulting files
$ will be placed. 'name' mean what base name to give the files placed
$ in 'directory'. 'language' means which localized versions of help files
$ to look for.
$ 
1 %s -dir <目录> [-generate] [-file <名字>] [-lang <语言>]\n

$ 
$ Mesages 2-18 are error messages.
$ 
2 %s: %s 的元素不是一个目录\n
3 %s: 对目录 %s 的访问被拒绝。\n试着作为超级用户运行?\n
4 %s: %s 的元素不存在\n
5 %s: 包含 %s 的文件系统满了\n
6 %s: 不能访问 %s -- 出错状态号 %d\n
7 %s: 包含 %s 的文件系统是只读的\n
8 %s: 需要 root 的权限写到 %s\n
9 %s: 写到 %s 无效\n
10 %s: 查找路径为空\n
11 %s: 缺少‘标题’资源\n
12 %s: 缺少‘抽象’资源\n
13 %s: 缺少‘卷’资源\n
14 %s: 缺少‘字符’集资源\n
15 %s: 缺少目标目录\n
16 %s: 没有找到文件族\n
17 %s: 没有找到文件卷\n
18 %s: 不能访问当前工作目录 - 出错状态号 %d\n
19 %s: 不能分配存储器\n
20 %s: 指定了无效的系统语言 %s\n

$ 
$set 2
$ 
$ Specifies the character set used to create this file
$ 
$ This must be localized for the various languages. That is
$ for Japanese shift JIS, it would be 'ja_JP.SJIS'; for Japanese 
$ EUC it would be 'ja_JP.eucJP'. For files written using
$ HP Roman8 character set it would be '<lang+terr>.HP-ROMAN8', etc.
$ The set of allowable locale strings can be found in
$ /usr/dt/config/svc/CDE.lcx
$ 
1 zh_CN.EUC-CN

$ 
$ Specifies the title for the browser.
$ It is used in the body of text displayed to the user.
$ 
2 欢迎使用提示管理程序

$ 
$ Specifies the body of text displayed in the browser.
$ 
$ When localizing, DO NOT alter any text between the "<" and ">" pairs.
$ That is - DO NOT localize the tags such as <ABBREV> and <PARAGRAPH>.
$           DO     localize the text between the tags.
$ 
3 <ABBREV>欢迎使用提示管理程序</ABBREV>            \
<PARAGRAPH>以下所列的每一标题代表一个已安装并注册了\
联机提示的<ANGLE italic>产品系列</>。每一标题\
（和图标）是一个列出该系列提示的超链接。</>        \
<PARAGRAPH after 0 first 1 left 3 label "<CHAR C.DT-SYMBOL-1><0xB7></>"> \
要列出一个产品系列提示清单，选择\
其标题（划下划线的文本）或图标。</PARAGRAPH>       \
<PARAGRAPH after 0 first 1 left 3 label "<CHAR C.DT-SYMBOL-1><0xB7></>"> \
在一个产品系列内，找出你想看的提示，然后\
选择它的标题。</PARAGRAPH>                         \
<PARAGRAPH first 1 left 3 label "<CHAR C.DT-SYMBOL-1><0xB7></>">        \
如果你在使用提示窗口时需要提示，请按 F1 键。</PARAGRAPH>

$ 
$ Specifies the Volume Title
$ 
4 提示 - 顶层

$ 
$ Specifies the preamble to the help file if no volumes or family files
$ are found.
$ 
$ When localizing, DO NOT alter any text between the "<" and ">" pairs.
$ That is - DO NOT localize the tags such as <ABBREV> and <PARAGRAPH>.
$           DO     localize the text between the tags.
$ 
5 <ABBREV>欢迎使用提示管理程序</ABBREV>                        \
<LINK 0 "Help4Help How-To-Register-Help">                      \
<TYPE serif><WEIGHT bold><SIZE 12><ANGLE italic>               \
注意:    没有已注册的提示                                      \
</SIZE></WEIGHT></TYPE></></LINK>                              \
<PARAGRAPH leftindent 3 firstindent 3>                         \
<WEIGHT bold>没有一个产品系列注册了用于浏览                      \
的联机提示文件。</>  在某些应用程序中可以                      \
直接在应用程序中选择‘提示’命令获得提示。</>

$ 
$ Specifies the title to use in the dthelpgen dtksh dialog.
$ 
6 dthelpgen

$ 
$ Specifies the message to display in the dthelpgen dtksh dialog.
$ This message indicates that dthelpgen is building (or rebuilding)
$ the browser information.
$ 
7 生成浏览器信息。请等待。

$ 
$set 3
$ 
$ ******* Text Formatting Templates ********
$ DO NOT TRANSLATE THESE MESSAGES
$ DO NOT CHANGE THESE MESSAGES
$ DO NOT LOCALIZE THESE MESSAGES
$ 
1 <TOPIC charset %s>
2 <TITLE><TYPE serif><WEIGHT bold><SIZE 14>%s</SIZE></WEIGHT></TYPE></TITLE>
3 <PARAGRAPH before 1 first 1 left 1>
4 <CHARACTERSET %s>
5 <TYPE serif><WEIGHT bold><SIZE 12><ANGLE italic>%s</></></></>
6 <PARAGRAPH before 1 first 1 left 1 graphic %s glink %s gtypelink 0>
7 <PARAGRAPH before 1 first 1 left 1 graphic %s glink "%s %s" gtypelink 1>


