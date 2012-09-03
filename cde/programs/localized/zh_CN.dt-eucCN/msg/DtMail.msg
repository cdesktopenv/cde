$ 
$ +SNOTICE
$ 
$  $TOG: DtMail.msg /main/5 1998/05/22 17:07:43 rafi $
$ 
$  RESTRICTED CONFIDENTIAL INFORMATION:
$ 
$  The information in this document is subject to special
$  restrictions in a confidential disclosure agreement between
$  HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
$  document outside HP, IBM, Sun, USL, SCO, or Univel without
$  Sun's specific written approval.  This document and all copies
$  and derivative works thereof must be returned or destroyed at
$  Sun's request.
$ 
$  Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
$ 
$ +ENOTICE
$ 

$quote "

$ 
$ Strings from dtmail/RoamApp.C

$set 1
1 "确定" 
$           /* NL_COMMENT
$            * This is a menu label, please refer to the resource file "Dtmail" for
$            * appropriate mnemonics tranlation
$            */  
2 "邮件选项..." 
3 "休假" 
4 "正在修改特性..." 


$ 
$ Strings from dtmail/RoamMenuWindow.C

5 "取消" 
6 "邮件程序" 
7 "邮件程序" 
8 "邮件程序" 
9 "删除" 
10 "下一个" 
11 "上一个" 
12 "向发信者回信" 
13 "打印" 
14 "发信者" 
15 "主题" 
16 "日期和时间" 
17 "大小" 
$           /* NL_COMMENT
$            * Menu labels in Main Mailer (from RoamMenuWindow.C).
$            * The following menu labels require translation plus parenthesized
$            * mnemonics for all Asian locales. Please refer to the resource file
$            * "Dtmail" for each mnemonics.
$            * Message 18-22,25,28,29 are menu labels for "Mailbox" pull down menu.
$            * Message 30,31,33-40 are menu labels for "Message" pull down menu.
$            * Message 43-45 are menu labels for "Edit" pull down menu.
$            * Message 47--55 are menu labels for "View" pull down menu.
$            * Message 56-63 are menu labels for "Compose" pull down menu.
$            * Message 64,65,221 are menu labels for "Move" pull down menu.
$            * Message 66,68,69 are menu labels for "Attachment" pull down menu.
$            * Message 71-77 are menu labels for "Help" pull down menu.
$            */  
18 "信箱" 
19 "检查新邮件" 
20 "打开收件箱" 
21 "打开收件箱" 
22 "新建..." 
23 "邮件程序 - 新建" 
24 "新建" 
25 "打开..." 
26 "邮件程序 - 打开" 
27 "打开" 
28 "销毁被删除的消息" 
29 "关闭" 
30 "打开" 
31 "存为文本..." 
32 "邮件程序 - 消息 - 存为文本" 
33 "复制到" 
34 "打印..." 
35 "查找..." 
36 "选择所有的" 
37 "删除" 
38 "恢复最后一个删除" 
39 "从列表中恢复删除..." 
40 "消息" 
41 "移动" 
42 "邮件程序 - 消息" 
43 "复制" 
44 "选择所有的" 
45 "编辑" 
46 "邮件程序 - 文本" 
47 "下一个" 
48 "上一个" 
49 "缩写的标头" 
50 "按日期/时间" 
51 "按发信者" 
52 "按主题" 
53 "按大小" 
54 "按状态" 
55 "显示" 
56 "新消息" 
57 "新建，包括所有的" 
58 "转发邮件" 
59 "向发信者回信" 
60 "向所有人回信" 
61 "向发信者回信，包括" 
62 "向所有人回信，包括" 
63 "书写" 
64 "移入" 
65 "其它邮箱..." 
66 "保存为..." 
67 "邮件程序 - 附件 - 保存为" 
68 "选择所有的" 
69 "附件" 
70 "邮件程序 - 附件" 
71 "概览" 
72 "任务" 
73 "参考" 
74 "针对项" 
75 "使用提示" 
76 "提示" 
77 "关于邮件程序..." 
78 "邮件程序不理解此信箱的状态。\n选择〈确定〉来关闭此信箱，然后重新启动邮件程序。" 
79 "正在转换... 完成了百分之%d" 
80 "邮件程序" 

$ 
$ Strings from dtmail/AttachArea.C

81 "邮件程序" 
82 "邮件程序" 
83 "邮件程序" 


$ 
$ Strings from dtmail/Attachment.C

84 "%s是可执行的附件。你要执行它吗?" 
85 "邮件程序" 
86 "邮件程序" 
87 "邮件程序" 
88 "邮件程序" 


$ 
$ Strings from dtmail/RoamCmds.C

89 "邮件程序 - 其它邮箱" 
90 "移动" 
91 "正在打印..." 
92 "确定" 
93 "保存" 
94 "保存" 
95 "保存" 
96 "空" 
97 "换名" 
98 "邮件程序 - 换名" 
99 "邮件程序" 
100 "确定" 
101 "取消" 
102 "你已经对除休假外的其它某些事情使用转发设施。\n当休假程序运行时，休假将会被附加到此转发\n活动中。是否还要启动休假程序?" 
103 "邮件程序" 
104 "你已经在 .forward 文件中运行了休假程序。\n查看文档如何从你的 .forward 文件中停止和\n删掉它。在解决问题后使用此命令。" 
105 "不能打开 .vacation.msg 文件 - 没有写权限。" 
106 ".vacation.msg 文件已经存在。用新文本替换?" 
107 "不能打开 .vacation.msg 文件 - 没有写权限。" 
108 "我在休假" 
$ 	    /* NL_COMMENT
$ 	     * This is the default value of the subject field in the
$ 	     * message that gets returned to the sender when vacation
$ 	     * is turned on.
$ 	     */
109 "我在休假。\n你致 \"$SUBJECT\" 的邮件在我回来后会被阅读。\n" 


$ 
$ Strings from dtmail/MsgScrollingList.C

110 "N" 
$ /* NL_COMMENT
$  * In a mailer container window's message scrolling list, a "N" appears
$  * to the left of a mail message header indicating that the mail message
$  * is "new" (just arrived and not yet viewed by the user).
$  * There is only space to display 1 character.  If "N" needs to be translated,
$  * please make sure the translation is only 1 character.
$  */
111 "N" 
$ /* NL_COMMENT
$  * In a mailer container window's message scrolling list, a "N" appears
$  * to the left of a mail message header indicating that the mail message
$  * is "new" (just arrived and not yet viewed by the user).
$  * There is only space to display 1 character.  If "N" needs to be translated,
$  * please make sure the translation is only 1 character.
$  */
112 "N" 
$ /* NL_COMMENT
$  * In a mailer container window's message scrolling list, a "N" appears
$  * to the left of a mail message header indicating that the mail message
$  * is "new" (just arrived and not yet viewed by the user).
$  * There is only space to display 1 character.  If "N" needs to be translated,
$  * please make sure the translation is only 1 character.
$  */
113 "N" 
$ /* NL_COMMENT
$  * In a mailer container window's message scrolling list, a "N" appears
$  * to the left of a mail message header indicating that the mail message
$  * is "new" (just arrived and not yet viewed by the user).
$  * There is only space to display 1 character.  If "N" needs to be translated,
$  * please make sure the translation is only 1 character.
$  */
114 "N" 


$ 
$ Strings from dtmail/Undelete.C

115 "恢复删除" 
116 "关闭" 

$ 
$ Strings from dtmail/SendMsgDialog.C

117 "发送" 
118 "关闭" 
119 "新建消息" 
120 "邮件程序" 
121 "文件" 
122 "包括..." 
123 "邮件程序 - 包括" 
124 "包括" 
125 "存为文本..." 
126 "邮件程序 - 存为文本" 
127 "注册消息" 
128 "发送为" 
129 "关闭" 
130 "编辑" 
131 "回复" 
132 "剪切" 
133 "复制" 
134 "粘贴" 
135 "特殊粘贴" 
136 "加括号的" 
137 "缩格的" 
138 "清除" 
139 "删除" 
140 "选择所有的" 
141 "附件" 
142 "增加文件..." 
143 "邮件程序 - 增加" 
144 "增加" 
145 "保存为..." 
146 "邮件程序 - 附件 - 保存为" 
147 "删除" 
148 "恢复删除" 
149 "换名" 
150 "选择所有的" 
$       /* NL_COMMENT
$        * This is an obsolete message.  It is replaced by message 226
$        * in set 1.
$        */
151 "显示窗格" 
152 "格式" 
153 "断行" 
154 "设定..." 
155 "查找/更改..." 
156 "检查拼写..." 
157 "样板" 
158 "邮件程序 - 附件" 
159 "邮件程序 - 书写" 
160 "新消息" 
$       /* NL_COMMENT
$        * This is the label for a toggle item in a menu.  When the item
$        * is set to "Show List", the Attachment List is mapped in the
$        * Compose Window.  This message replaces message 151 in set 1.
$        */


$ 
$ Strings from dtmail/ViewMsgDialog.C

161 "复制" 
162 "选择所有的" 
163 "编辑" 
164 "保存为..." 
165 "邮件程序 - 附件 - 保存为" 
166 "选择所有的" 
167 "附件" 
168 "邮件程序 - 附件" 
169 "邮件程序 - 文本" 
170 "概览" 
171 "任务" 
172 "参考" 
173 "针对项" 
174 "使用提示" 
175 "提示" 
176 "关于邮件程序..." 
177 "邮件程序" 


$ 
$ Strings from dtmail/DtMailGenDialog.C

178 "邮件程序 - 关于Dtmail" 
179 "Dtmail 版本1.0" 
180 "确定" 
181 "确定" 
182 "取消" 

$ 
$ Strings from dtmail/FindDialog.C

183 "查找" 
$  /* NL_COMMENT
$   * This is an obsolete message.  Replaced by message 220 in set 1
$   */
184 "查找 & 选择所有的"
185 "清除" 
186 "关闭" 
187 "提示" 
188 "至：" 
189 "自：" 
190 "主题：" 
191 "抄送：" 
192 "邮件程序 - 查找"
193 "向前" 
194 "向后" 
$  /* NL_COMMENT
$   * This message replaces message 184 in set 1
$   */



$ 
$ Strings from dtmail/Editor.C

195 "-------------- 开始转发消息 --------------\n\n" 
196 "-------------- 开始包括消息 --------------\n\n" 
197 "-------------- 结束转发消息 --------------\n\n" 
198 "-------------- 结束包括消息 --------------\n\n" 

$ 
$ Strings from dtmail/XmTextEditor.C

199 "\n-------------- 开始包括消息 --------------\n" 
200 "\n-------------- 结束包括消息 --------------\n" 

$ 
$ Strings from dtmail/DtEditor.C

201 "\n-------------- 开始包括消息 --------------\n" 
202 "\n-------------- 结束包括消息 --------------\n" 

$ 
$ Strings from dtmail/ComposeCmds.C

203 "邮件程序"
204 "不能创建书写窗口。" 
205 "邮件程序" 
206 "不能创建书写窗口。" 
207 "邮件程序" 
208 "不能创建书写窗口。" 
209 "邮件程序" 
210 "不能创建书写窗口。" 
211 "邮件程序"
212 "该样板不存在。" 
213 "邮件程序" 
214 "该样板似乎被毁坏了。" 
215 "邮件程序" 
216 "无足够存储器装入此样板。" 
217 "邮件程序" 
218 "该样板看起来被毁坏了。" 

$ 
$ Strings from dtmail/SortCmd.C

219 "正在排序..." 

220 "选择所有的" 
221 "收件盒" 

222 "%a %b %d %k:%M" 
$ /* NL_COMMENT
$  * The "%a %b %d %k:%M" is the time and date format, please refer to 
$  * strftime man page for explanation of each format.
$  */
223 "%a %b %d %H:%M" 
$ /* NL_COMMENT
$  * The "%a %b %d %H:%M" is the time and date format, please refer to 
$  * strftime man page for explanation of each format.
$  */
224 "%a %b %d %k:%M" 
$ /* NL_COMMENT
$  * The "%a %b %d %k:%M" is the time and date format, please refer to 
$  * strftime man page for explanation of each format.
$  */
225 "%a %b %d %H:%M" 
$ /* NL_COMMENT
$  * The "%a %b %d %H:%M" is the time and date format, please refer to 
$  * strftime man page for explanation of each format.
$  */
226 "显示列表" 

$ String from Undelete.C, Deleted Messages window title
227 "邮件程序 - 删除消息"

$ Strings from SendMsgDialog.C, Add Bcc, Delete Bcc menu buttons
228 "增加"
229 "删除"
230 "发出"

$  /* NL_COMMENT
$   * This string is displayed on the find dialog status line
$   * when searching for a matching message.
$   */
231 "查找..."

$        /* NL_COMMENT
$         * These strings are displayed on the find dialog status line
$         * when one or more matching messages are found.  The first
$         * string is displayed when there is one matching message,
$         * and the second string is displayed when there is more than
$         * one.  The %d is the number of messages that matched.
$         */
232 "选了1条消息"
233 "选了%d条消息"

$  /* NL_COMMENT
$   * This string is displayed on the find dialog status line when
$   * no matching messages were found.
$   */
234 "未找到匹配的消息"

$  /* NL_COMMENT
$   * These strings replace strings 178 and 179.  They are used in
$   * the "About Mailer" dialog box.
$   */
235 "邮件程序 - 关于邮件程序"
236 "邮件程序 版本 1.0"


$set 2
1 "邮件程序" 
2 "ToolTalk 没有初始化。 邮件程序在没有 TooTalk 时不能运行。\n请启动 /usr/dt/bin/dtsession，或与系统管理员联系。" 
3 "不能初始化窗口。退出。\n" 
4 "邮件程序没有正确地安装，\n而且由于执行组没有正确地设置\n所以不能运行。" 
5 "你没有察看 %s 的权限" 
$ 	    /* NL_COMMENT
$ 	     * The %s is the name of the mailbox the user doesn't have
$ 	     * permission to view.
$ 	     */
6 "信箱 %s 是一个目录，不能被打开。" 
7 "不能清空 %s。它不存在。" 
8 "容器 %s 被清空了" 
9 "出错 - 不能清空 %s" 
10 "邮件程序" 
11 "%s" 
12 "邮件程序" 
13 "邮件程序" 
14 "在发出“保存为”命令来存文件前，必须选择一个附件。" 

15 "出错 - 不能获得 mailrc。" 
16 "没有选择的邮件消息。" 
$ 	/* NL_COMMENT
$ 	 * No mail message has been selected by the user.
$ 	 */


17 "你已经编辑了“%s”。 还要删除吗?" 
18 "出错：不能包括文件 %s" 
19 "在发出“保存为”命令来存文件前必须选择一个附件。" 
20 "在发出“保存为”命令来存文件前必须选择一个附件" 
21 "邮件程序" 






$set 3
1 "自动保存..." 

2 "正在初始化..." 
3 "信箱 %s 不存在。\n以此名字创建一个信箱?" 
4 "邮件程序" 
5 "确定" 
6 "信箱 %s 被锁着。\n你可以手动将此信箱开锁，再试一次\n或与你的系统管理员联系。"
7 "邮件程序" 
8 "确定" 
9 "确定" 
10 "确定" 
11 "确定" 
12 "%s" 
13 "第%d条消息, 共%d条消息, %d条新消息, 删除了%d条消息"
14 "第%d条消息, 共%d条消息, %d条新消息, 删除了%d条消息"
$ 	/* NL_COMMENT
$ 	 * The user will see the following message display as:
$ 	 * "Message 3 of 10, 2 new, 6 deleted"
$ 	 * This means ??? -- Explain to translator.
$ 	 */
15 "正在保存..." 
16 "邮件程序" 
$ 		/* NL_COMMENT
$ 		 * This dialog comes up when the user tries to quit the
$ 		 * mailbox and the user is asked if he wants to destroy
$ 		 * the messages marked for deletion.
$ 		 */
17 "销毁此信箱中你标记了\n删除的消息吗?" 
18 "确定" 
19 "取消" 
20 "正在打开信箱..." 
21 "%s 已经存在。\n覆盖吗?" 
22 "邮件程序" 
23 "不能覆盖%s.\n检查文件的权限，然后再重做。" 
24 "邮件程序" 
25 "正在装入容器..." 
26 "文件夹提要信息" 
27 "正在装入..." 
28 "空的容器" 
29 "确定" 
30 "确定" 
31 "确定" 
32 "正在销毁要删除的消息..." 
33 "确定" 

34 "%s 不存在。" 
35 "不能打开 %s。" 
36 "不能分配存储器。" 
37 "附件的提要" 
38 "附件" 
39 "附件" 
$ 	/* NL_COMMENT
$ 	 * "attachments" is the plural form of "attachment".
$ 	 */
40 "附件" 
41 "删除的" 

42 "%s已经存在。 替换它?" 
43 "不能替换 %s。" 
44 "不能创建 %s。" 
45 "不能创建 %s。" 

46 "空容器" 
47 "%s 已经存在。\n覆盖它?" 
48 "邮件程序" 
49 "不能覆盖%s。\n检查文件权限，然后再试。" 
50 "邮件程序" 
51 "不能创建%s。" 
52 "邮件程序" 
53 "不能写到%s。" 
54 "邮件程序" 
55 "不能创建%s。" 
56 "邮件程序" 
57 "附件换名为" 
58 "该书写窗口含有在\n窗口关闭时将会丢失\n的文本或附件。\n关闭此书写窗口?" 
59 "存储器不够" 
60 "没有足够的存储器来装入现有的 .vacation.msg 文件。" 
61 "邮件程序" 
62 "现有的 .vacation.msg 文件似乎被毁坏了。" 


63 "选择一个要移动的消息。"
64 "选择一个要复制的消息。"
65 "%d 个消息移动到 %s"
66 "%d 个消息移动到 %s" 
67 "%d 个消息复制到 %s" 
68 "%d 个消息复制到 %s" 
69 "视窗已经存在。 把它移到前面来。" 
$ 	/* NL_COMMENT
$ 	* The current mail message selected is already displayed in a separate
$ 	* window.  Therefore this 'separate' window will be raised in front
$ 	* of existing windows so the user can see it.
$ 	*/



70 "正在存死信..."
71 "邮件程序" 
72 "确定" 
73 "取消" 
74 "确定" 

75 "确定"
76 "确定" 

77 "You have an attachment open that may have unsaved changes.\nSending this message will break the connection to the open\n attachment. Any unsaved changes will not be part of the\n message. You can use Save As to save Changes after the\n connection is broken, but the changes will not be part of\n the attachment."
78 "初始化邮件选项..."
79 "销毁并退出"
80 "不销毁, 退出"
81 "This attachment may contain commands that can cause serious\n\damage.  It is recommended that you only execute it after you are certain it is safe to do so.  Press OK if you are certain it is safe, Cancel to cancel execution."
82 "Someone else is using this mailbox.\nWould you like to demand exclusive access?"
83 "Mailer is unable to obtain exclusive access to this mailbox.\nWould you like to open this mailbox read-only?"

$	
$	These messages have been commented out to allow compilation.
$	The english messages need to be translated.
$	The conflicts in the Chinese messages need to be resolved.
$ 
$ 84 "Mailer is unable to obtain exclusive access to this\nmailbox because the system is not responding.\n\nFor this time only, you can choose to open this mailbox\nread-only, or to open it read-write without exclusive\naccess (use only if no one else is using this mailbox)."
$ 85 "只读"
$ 86 "读写"

$            /* NL_COMMENT
$             * The following sentence means %d number of mail messages have
$             * been deleted from the mail folder.  This is the plural form
$             * of the message that gets printed if more than one message
$             * is moved.
$             */
84 "删除了%d条消息"

$            /* NL_COMMENT
$             * The following sentence means %d number of mail messages have
$             * been deleted from the mail folder.  This is the singular
$             * form of the message that gets printed if only one message
$             * is moved.
$             */
85 "删除了%d条消息"
86 "检查新邮件..."
$		/* NL_COMMENT
$		 * This dialog comes up when the user tries to quit the
$		 * mailbox.  The user is asked if they want to destroy
$		 * the deleted messages.
$		 */
87 "邮件程序 - 关闭"
88 "销毁删除的消息并关闭此信箱?"
89 "销毁并关闭"
90 "保留并关闭"
$        /* NL_COMMENT
$         * Post a dialog explaining that the action was invalid
$         */
91 "不能执行无效的动作"
$        /* NL_COMMENT
$         * Post a dialog explaining that the action failed.
$         */
92 "执行动作失败!"
93 "选择的图符%s可执行.\n要运行, 选[确定], 否则选[取消]."


$set 5
1 "邮件程序" 
2 "邮件程序" 
3 "这样发送此消息将使你丢失对此附件的任何更改。"
$    /* NL_COMMENT
$     * The user tried to send a messages without saving changes in
$     * some open attachments.  This warning makes sure that is what
$     * the user intended.
$     */
4 "只选择一个附件\n	   然后选择‘换名’" 
$       /* NL_COMMENT
$        * Mailer ran out of memory.  Ask the user to quit some other
$        * applications so there will be more memory available.
$        */
5 "在此消息中的某些地址不正确，它们没有引用此系统中的\n任何已知用户。请保证所有的地址是有效的然后再试一次。"
$       /* NL_COMMENT
$        * Mailer ran out of memory.  Ask the user to quit some other
$        * applications so there will be more memory available.
$        */
6 "邮件程序不能获得足够的内存发送此消息。\n请退出其它应用程序然后再发送此消息。"
$       /* NL_COMMENT
$        * There was an error from the mail transport (sendmail).
$        */
7 "在试图发送你的消息时出现错误。核实该消息是否被收到，\n如果没有，你必须重新发送此消息。"
$       /* NL_COMMENT
$        * User trying to send a message that has no addressed recipient.
$        */
8 "Try Send after specifying  recipient(s) of the message in \n\the To:, Cc:, or Bcc: fields."


$set 12
1 "附件" 
2 "附件" 
3 "显示计数" 
4 "不能附加 FIFO 文件：%s" 
5 "不能附加字符特殊文件：%s" 
6 "不能附加目录：%s" 
7 "不能附加块特殊文件：%s" 
8 "不能附加套接字文件：%s" 


$set 13
1 "不能创建" 
2 "不能写" 

$ 
$ Strings from dtmail/AttachCmds.C

$set 14
1 "增加附件" 
2 "不能打开 %s" 
3 "不能附加 FIFO 文件：%s" 
4 "不能附加字符特殊文件：%s" 
5 "不能附加目录：%s"
6 "不能附加块特殊文件：%s" 
7 "不能附加套接字文件：%s" 
8 "不能打开 %s" 
9 "邮件程序 - 附件换名" 
10 "描述" 
11 "保存附件为" 

$set 15
1 "你做过修改, 但未保存.\n你可以保存, 放弃这些修改, \n或回到对话的前一个位置."
2 "保存"
3 "放弃"
