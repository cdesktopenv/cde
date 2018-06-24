$ $XConsortium: dtksh.msg /main/3 1996/07/24 19:01:16 drk $
$set 1
1 DtkshCvtStringToPixel: 颜色 '%s' 没有定义

$set 5
1 指定了太多的资源参数; 跳过 '%s' 
2 错误的资源规格描述; 必须以形式 '名字:值' :%s
3 资源分隔符是 ':' ，但您使用的是 '=' : %s
4 工具箱已经初始化了
5 用法: RXtInitialize variable applicationName applicationClass [args ...]
6 用法: %s variable name class parent [arg:val ...]
7 用法: XtCreateApplicationShell variable name class [arg:val ...]
8 用法: XtPopup widget GrabNone|GrabNonexclusive|GrabExclusive
9 抓取类型 '%s' 不识别; 请用 'GrabNone'
10 用法: XtIsShell widget
11 用法: XtMapWidget widget
12 用法: XtUnmapWidget widget
13 用法: %s variable widget
14 用法: XtScreen variable widget
15 用法: XtWindow variable widget
16 用法: XtCallCallbacks widget callbackName
17 用法: XtHasCallbacks variable widget callbackName
18 用法: XtAddCallback widget callbackName ksh-command
19 用法: XtRemoveCallback widget callbackName ksh-command
20 下面是一个无效的回调句柄: %s
21 指定的回调没有注册
22 用法: XtAddEventHandler widget mask nonMaskable ksh-command
23 用法: XtRemoveEventHandler widget mask nonMaskable ksh-command
24 指定的事件处理程序没有注册
25 用法: XtGetValues widget resource:variable ...
26 下列资源参数不是正确形式: %s
27 用法: XtSetValues widget arg:val ...
28 用法: XtAddWorkProc variable command
29 用法: XtRemoveWorkProc workProcId
30 'workProcId' 参数必须是一个十六进制数: %s
31 用法: XtAddTimeOut variable milliseconds command
32 'milliseconds' 参数必须大于零
33 用法: XtRemoveTimeOut intervalId
34 'intervalId' 参数必须是一个十六进制数: %s
35 用法: %s widget ...
36 用法: XBell display volume
37 用法: %s variable screen
38 'screen' 参数是无效的: %s
39 用法: XDefineCursor display window cursorId
40 'cursorId' 参数是无效的: %s
41 用法: XUndefineCursor display window
42 用法: XtRemoveAllCallbacks widget callbackName
43 用法: XTextWidth variable fontName string
44 用法: %s display drawable [args ...]
45 'drawable' 参数是无效的: %s
46 目标参数是无效的: %s
47 未识别的图形函数名字: %s
48 未识别的行式样: %s
49 未识别的绘画选项: %s
50 还有剩下的点被忽略了
51 用法: XCopyArea display source dest sourceX sourceY width height destX destY [args ...]
52 不能转换资源类型 '%s' 到 '字符串' 
53 不能转换资源类型 '字符串' 到类型 '%s' 
54 用法: XtAddInput variable [-r] fileDescriptor kshCommand
56 用法: XtRemoveInput inputId
57 'inuputId' 参数必须是一个十六进制数: %s
59 用法: XFlush display
60 用法: XSync display discard
61 用法: XRaiseWindow display window
62 用法: %s widget [True|False]
63 用法: %s widget translations
64 用法: XtUninstallTranslations widget
65 用法: XtParent variable widget
66 用法: XtLastTimestampProcessed variable display
67 用法: DtSessionRestorePath widget pathVariable saveFile
68 用法: DtSessionSavePath widget pathVariable fileVariable
69 用法: DtShellIsIconified widget
70 用法: DtSetStartupCommand widget command
71 用法: DtSetIconifyHint widget boolean
73 rootWindow 参数是无效的: %s
74 用法: DtWsmSetCurrentWorkspace widget atom
75 工作空间原子是无效的: %s
78 用法: DtWsmSetWorkspacesOccupied display window workspaceList
79 用法: DtGetHourGlassCursor variable display
80 用法: DtWsmAddCurrentWorkspaceCallback variable widget ksh-command
81 用法: DtWsmRemoveWorkspaceCallback handle
82 用法: XtNameToWidget variable referenceWidget names
83 用法: DtDbLoad
84 用法: DtDbReloadNotify ksh-command
85 用法: DtActionExists actionName
86 用法: DtActionLabel variable actionName
87 用法: DtActionDescription variable actionName
88 用法: DtActionInvoke widget actionName termParms execHost contextDir useIndicator ksh-command ["FILE" fileName] ...
89 用法: DtDtsLoadDataTypes
90 用法: DtDtsFileToDataType variable fileName
91 用法: DtDtsFileToAttributeValue variable fileName attrName
92 用法: DtDtsFileToAttributeList variable fileName
93 用法: DtDtsDataTypeToAttributeValue variable dataType attrName optName
94 用法: DtDtsDataTypeToAttributeList variable dataType optName
95 用法: DtDtsFindAttribute variable name value
96 用法: DtDtsDataTypeNames variable
97 用法: DtDtsSetDataType variable fileName dataType override
98 用法: DtDtsDataTypeIsAction dataType
99 用法: ttdt_open variable status variable2 toolname vendor version sendStarted
100 用法: tttk_Xt_input_handler procId source id
101 'source' 参数必须是一个整数: %s
102 'id' 参数必须是一个十六进制数: %s
103 用法: ttdt_close status procId newProcId sendStopped
104 用法: ttdt_session_join variable status sessId shellWidgetHandle join
105 用法: ttdt_session_quit status sessId sessPatterns quit
106 'sessPatterns' 参数是无效的: %s
107 用法: ttdt_file_event status op patterns send
108 用法: ttdt_file_quit status patterns quit
109 用法: ttdt_Get_Modified pathName scope timeout
110 用法: %s status pathName scope timeout
112 用法: tt_error_pointer variable ttStatus
113 用法: %s status msg
114 用法: %s status msg msgStatus msgStatusString destroy
115 用法: ttdt_file_join variable status pathName scope join ksh-command
116 用法: tt_file_netfile variable status filename
117 用法: tt_netfile_file variable status netfilename
118 用法: tt_host_file_netfile variable status host filename
119 用法: tt_host_netfile_file variable status host netfilename
120 用法: XtIsSubclass widget class
121 %s 不是一个有效的类名字
122 用法: XtClass variable widget

$set 6
1 DtkshCvtWindowToString: '自' 值的大小无效
2 DtkshCvtHexIntToString: '自' 值的大小无效
3 DtkshCvtIntToString: '自' 值的大小无效
4 DtkshCvtBooleanToString: '自' 值的大小无效
5 DtkshCvtStringToWidget: '自' 值的大小无效
6 DtkshCvtStringToWidget: 不能找到称为 '%s' 的 widget 名
7 DtkshCvtStringToCallback: '自' 值的大小无效
8 DtkshCvtCallbackToString: '自' 值的大小无效
9 DtkshCvtWidgetToString: '自' 值的大小无效
10 DtkshCvtWidgetToString: 不能为此 widget 找到名字
11 DtkshCvtWidgetClassToString: '自' 值的大小无效
12 DtkshCvtWidgetClassToString: 未知的 widget 类
13 DtkshCvtStringToWidgetClass: 未知的 widget 类名字
14 DtkshCvtScreenToString: '自' 值的大小无效

$set 11
1 不能找到称为 '%s' 的字段名在 '%s' 结构中
2 DtkshCvtCallbackToString: 内部转换缓冲区溢出
3 对资源 '%s' （在 widget 类 '%s' 中）散列失败
4 没有提供函数名
5 此命令不能执行; 工具箱还没有初始化
6 创建 widget '%s' 失败
7 '显示' 参数无效: %s
8 下列特性原子无效: %s
9 下列资源没有为 widget '%s' 定义: %s
10 指定的位置无效: %s
11 指定的字体无效: %s
12 widget 句柄 '%s' 没有引用一个已有的 widget
13 该 widget 必须是一个 '命令' widget
14 该 widget 必须是一个 '主窗口' widget
15 该 widget 必须是一个 '比例' widget
16 该 widget 必须是一个 '滚动条' widget
17 该 widget 必须是一个 '切换按钮' widget 或 gadget
18 '窗口' 参数无效: %s
19 不能分配所需的存储; 退出
20 不能查找该父 widget
21 不能找到符号 '%s' 
22 不能找到类型 '%s' 
23 不能分析声明 '%s' ; 使用 'unsigned long'
24 未知的子类型: %s
25 未识别的选项标志: %s
26 用法: %s widget position
27 用法: %s widget time
28 用法: %s widget
29 dtksh 出错
30 dtksh 警告
31 用法: %s display window
32 用法: %s display window variable
33 用法: %s display rootWindow variable
34 '超时' 参数无效: %s
35 '信息' 参数无效: %s
36 '模式' 参数无效: %s

$set 14
1 在初始化 widget 类时内部散列表故障; 退出
2 不能找到称为 '%s' 的 widget 类
3 标识符 '%s' 不是一个合法的 widget 句柄
4 不能查找该 widget 类
5 用法: DtLoadWidget widgetClassName widgetClassRecordName
6 不能找到称为 '%s' 的类记录名
7 在初始化类 '%s' 时内部散列表故障
8 \n%sRESOURCES FOR %s%s%s: \n
9 CONSTRAINT
10 R
11 M
12 S
13 %-15s %-6s %-6s %-18s %-6s %s\n
14 环境 变量       句柄   父     类                 状态   名字\n
15 \n用法:\tDtWidgetInfo [widgetHandel]\n\tDtWidgetInfo -r <widgetHandle|className>\n\tDtWidgetInfo -R <widgetHandle|className>\n\tDtWidgetInfo -c [className]\n\tDtWidgetInfo -h [widgetHandle]

$set 15
1 不能初始化工具集
2 下列资源不能在 widget 创建时设置。\n在创建后使用 XtSetValues而不是: %s
3 用法: %s variable parent name [argument:value ...]
4 用法: DtHelpReturnSelectedWidgetId variable widget variable
5 用法: DtHelpSetCatalogName catalogName
6 用法: DtHelpQuickDialogGetChild variable quickHelpWidget child
7 该 widget 必须是一个 'quickHelp' widget
8 该 widget 必须是一个 '列表' widget
9 用法: %s widget position item
10 用法: %s widget position itemList
11 用法: %s widget item
12 用法: XmListDeleteItemsPos widget count position
13 用法: XmListDeleteItems widget itemList
14 用法: XmListDeletePositions widget positionList
15 用法: XmListGetKbdItemPos variable widget
16 用法: XmListItemExists widget item
17 用法: XmListItemPos variable widget item
18 用法: XmListPosSelected widget position
19 用法: XmListPosToBounds widget position variable variable variable variable
20 用法: %s widget position notifyFlag
21 用法: %s widget item notifyFlag
22 用法: XmListSetAddMode widget boolean
23 用法: XmListSetKbdItemPos widget position
24 用法: XmMainWindowSetAreas mainwindow menu command hscroll vscroll work
25 'mailWindow' 句柄为 NULL
26 用法: %s variable mainwindow
27 用法: XmProcessTraversal widget direction 
28 未知的遍历方向: %s
29 用法: XmInternAtom variable display name onlyIfExists
30 用法: XmGetAtomName variable display atom
31 指定的原子无效: %s
32 用法: XmGetColors widget background foreground topshadow bottomshadow select
33 背景象素无效: %s
34 用法: XmUpdateDisplay widget
35 用法: %s widget protocol [protocol ...]
36 指定的原子无效: %s
37 用法: XmAddWMProtocolCallback widget protocol ksh-command
38 用法: XmRemoveWMProtocolCallback widget protocol ksh-command
39 用法: XmMenuPosition menu event
40 指定的事件无效: %s
41 用法: XmCommandAppendValue commandWidget string
42 用法: XmCommandError commandWidget errorMessage
43 用法: XmCommandSetValue commandWidget command
44 用法: XmCommandGetChild variable commandWidget child
45 用法: XmMessageBoxGetChild variable commandWidget child
46 该 widget 必须是一个 'messageBox' widget
47 用法: XmFileSelectionBoxGetChild variable widget child
48 该 widget 必须是一个 'file selection box' widget
49 用法: XmSelectionBoxGetChild variable widget child
50 该 widget 必须是一个 'selection box' widget
51 用法: XmScaleGetValue scaleWidget variable
52 用法: XmScaleSetValue scaleWidget value
53 指定的比例值无效: %s
54 用法: XmScrollBarGetValues scrollbar variable variable variable variable
55 用法: XmScrollBarSetValues scrollbar value sliderSize increment pageIncrement notify
56 指定的值无效: %s
57 指定的滑尺大小无效: %s
58 指定的增量无效: %s
59 指定的页增量无效: %s
60 用法: XmScrollVisible scrolledWin widget leftRightMargin topBottomMargin
61 该 widget 必须是一个 'scrolledWindow' widget
62 不存在可见的 widget。
63 指定的左/右页边无效: %s
64 指定的上/下页边无效: %s
65 用法: %s widget state notify
66 用法: catopen variable catName
67 用法: catclose catId
68 用法: catgets variable catId setNum msgNum dftMsg
69 该 widget 必须是一个 '文本' 或 '文本字段' widget
70 用法: %s variable  widget
71 用法: %s widget boolean 
72 指定的参数无效: %s
73 用法: %s widget lines
74 用法: %s widget maxLength
75 用法: %s widget string
76 用法: %s widget variable variable
77 用法: %s widget position string
78 用法: %s widget position variable variable
79 用法: %s widget fromPosition toPosition string
80 指定的 '自' 位置无效: %s
81 指定的 '至' 位置无效: %s
82 用法: %s widget firstPosition lastPosition time
83 指定的首位置无效: %s
84 指定的末位置无效: %s
85 指定的时间无效: %s
86 用法: %s variable widget x y
87 指定的 x 位置无效: %s
88 指定的 y 位置无效: %s
89 用法: %s widget left right mode
90 指定的左位置无效: %s
91 指定的右位置无效: %s
92 用法: %s widget start string direction variable
93 指定的起始位置无效: %s
94 用法: XmListGetSelectedPos variable widget
95 用法: XmListGetMatchPos variable widget item
96 用法: XmOptionLabelGadget variable widget
97 用法: XmOptionButtonGadget variable widget
98 用法: XmGetVisibility variable widget
99 用法: XmGetTearOffControl variable widget
100 用法: XmGetTabGroup variable widget
101 用法: XmGetPostedFromWidget variable widget
102 用法: XmGetFocusWidget variable widget
103 用法: XmFileSelectionDoSearch widget directoryMask

$set 16
1 DtkshCvtNamedValueToString: 不匹配
2 DtkshCvtStringToNamedValue: 不能转换字符串 '%s' 


$ The following messages are for ksh93 itself.  The message numbers must
$ match those specified in the MsgStr array allmsgs in 
$ ksh93/src/cmd/ksh93/sh/init.c, as must the set number.
$set 25

1  执行完
3  正在运行
4  [-n] [arg...]
5  [arg...]
6  [dir] [list]
7  [job...]
8  [n]
9  [name [pathname] ]
10  [name]
11  [top] [base]
12  expr...
13  format [arg...]
14  是一个函数
15  是一个关键字
16  是一个 shell 内部的
17  是一个输出函数
18  是一个未定义的函数
19  name [arg...]
20  optstring name [args...]
21  秒
23 ${HOME:-.}/.profile
24 %c: 表达式中无效的字符 - %s
25 %c: 未知的格式区分符
26 %d-%d: 无效的范围
27 %d: 无效的二进制脚本版本
28 %s 是一个别名，用于
29 %s 是一个输出的别名，用于
30 %s 缺少
31 %s 未知的基
32 %s:  ':' 期待 '?' 操作符
33 %s: 二义性
34 %s: 参数必须是 %job 或进程标识符
35 %s: 别名没有找到\n
36 %s: 算术语法错
37 %s: 赋值需要左值
38 %s: 错误的文件单位数
39 %s: 错误的格式
40 %s: 错误的数字
41 %s: 错误的选项
42 %s: 错误的替换
43 %s: 错误的俘获
44 %s: 不能创建
45 %s: 不能执行
46 %s: 不能打开
47 %s: 被零整除
48 %s: 定义域例外
49 %s: %s 失败
50 %s: 文件已经存在
51 %s: 非法的函数名
52 %s: 无效的别名名字
53 %s: 无效的规程函数
54 %s: 无效的输出名字
55 %s: 无效的函数名
56 %s: 无效的名字
57 %s: 无效的正规表达式
58 %s: 无效的自引用
59 %s: 无效的使用: 
60 %s: 无效的变量名
61 %s: 不是一个标识符
62 %s: 是只读的
63 %s: 标号没有实现
64 %s: 超过极限
65 %s: 期待更多的单词
66 %s: 没有父名
67 %s: 没有引用名
68 %s: 没有找到
69 %s: 没有实现
70 %s: 操作数类型不兼容
71 %s: 溢出例外
72 %s: 参数没有设置
73 %s: 参数为空或没有设置
74 %s: 递归层次太深
75 %s: 引用变量不能是一个数组
76 %s: 要求路径参数
77 %s: 受限的
78 %s: 奇异性例外
79 %s: 下标出界
80 %s: 不对称的括弧
81 %s: 未知的函数
82 %s: 未知的本地环境
83 %s: 未知的操作符
84 %s: 未知的信号名字
85 %s: 将引起循环
86 （信息转储）
87 -c 要求实参
88 -e - 要求单个实参
89 /vpix
90 <未知的命令>
91 ACE#?F#?H:[name]L#?R#?Z#?fi#?[base]lnprtux [name=[value]...]
92 AE#?F#?HL#?R#?Z#?fi#?[base]lnprtux [name=[value]...]
93 异常终止
94 Ad:[delim]prst#[timeout]u#[filenum] [name...]
95 报警调用
96 错误的根结点规格描述
97 错误的系统调用
98 管道断裂
99 总线出错
100 不能起动作业控制
101 当前选项设定
102 DIL 信号
103 子进程中断
104 DircabefhkmnpstuvxCI:[file]o:?[option]A:[name] [arg...]
105 DircabefhkmnpstuvxCo:?[option]A:[name] [arg...]
106 EMT 俘获
107 超过 CPU 时间限制
108 超过文件大小限制
109 浮点例外
110 HSacdfmnstv [界限]
111 挂起
112 IO 信号
113 非法指令
114 中断
115 中断
116 LP [dir] [change]
117 存储器出错
118 迁移进程
119 没有作业控制
120 电话中断
121 轮转报警
122 电源失效
123 概析时间报警
124 退出
125 资源丢失
126 回复到旧的 tty 驱动程序...
127 S [屏蔽]
129 安全性标号更改
130 Socket 中断
131 声音完成
132 停止了（信号)
133 停止了（tty 输入)
134 停止了的进程继续
135 停止了
136 停止了(tty 输出)
137 切换到新的 tty 驱动程序 ...
138 系统马上瘫痪
139 终止了
140 Trace/BPT 俘获
141 未识别的版本
142 使用 'exit' 终止此 shell
143 用户 信号 1
144 用户 信号 2
145 版本没有定义
146 虚拟时间报警
147 窗口大小更改
148 您运行了作业
149 您停止了作业
150 [_[:alpha:]]*([_[:alnum:]])
151 \n@(#)Version 12/28/93\0\n
152 \n@(#)Version M-12/28/93\0\n
153 \nreal
154 \r\n\007shell 由于不活动将在 60 秒内到时
155 a 名字...
156 a:c [command [args...] ]
157 afpv 名字...
158 alarm %s %.3f\n
159 alarm -r %s +%.3g\n
160 期待参数
161 错误的目录
162 错误的文件单元数
163 错误的替换
164 不能访问父图标
165 不能创建管道
166 不能创建临时文件
167 不能生成子进程
168 不能获得 %s
169 不能设置 %s
170 不能设置报警
171 需要的条件
172 dsf:[库] [名字...]
173 e:[编辑器]lnrsN# [first] [last]
174 文件结束
175 f:[format]nprsu:[filenum] [arg...]
176 fnv 名字...
177 hist -e “${VISUAL:-${EDITOR:-vi}}”
178 历史记录文件不能打开
179 不正确的文法
180 无效的实参类型 %c
181 是一个 shell 的内嵌的版本
182 是一个跟踪的别名 - 用于
183 kill
184 行 %d: $ 没有被 \\ 处理
185 行 %d: 在 ${} 中的 %c 必须被括号括起
186 行 %d: %s 未知的标号
187 行 %d: 在 [[...]] 中的 %s 废弃了，使用((...))
188 行 %d: '=' 废弃了，使用 '=='
189 行 %d: -a 废弃了，使用 -e
190 行 %d: 在 %c 前的 \\ 为将来使用保留
191 行 %d: `...` 废弃了，使用 $(...)
192 行 %d: 转义 %c 以避免二义性
193 行 %d: 忽略标号 %s
194 行 %d: 括起 %c 以避免二义性
195 行 %d: 设置 %s 废弃了
196 行 %d: 对嵌套的子 shell 需要空格
197 行 %d: 在 $id[...] 中使用花括号以避免二义性
198 行 %d: 使用空格或 Tab 键隔离操作符 %c 和 %c
199 ln#[signum]s:[signame] sig...
200 禁用登录 setuid/setgid shell
201 映射
202 换行
203 nlp [作业...]
204 没有历史记录文件
205 无查询进程
206 无此作业
207 无此进程
208 不支持
209 关
210 开
211 打开的文件超过限制
212 存储器不够
213 p [action condition...]
214 p [name[=value]...]
215 参数没有设置
216 权限被拒绝
217 进程已经存在
218 ptx [name=[value]...]
219 pvV name [arg]...
220 r [varname seconds]
221 语法错在行 %d: `%s' %s
222 语法错在行 %d: 重复标号 %s
223 语法错: `%s' %s
224 sys
225 等待输入时间到
226 意外的
227 全域不可访问
228 不限制的
229 不匹配的
230 用户
231 版本
232 写到 %d 失败
233 您有邮件在 $_
234 零字节
