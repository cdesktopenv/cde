$ $XConsortium: dticon.msg /main/3 1995/11/08 13:51:02 rswiston $
$ *************************************<+>*************************************
$ *****************************************************************************
$ **
$ **  File:        dticon.msg
$ **
$ **  Project:     HP DT Icon Editor
$ **
$ **  Description:
$ **  -----------
$ **  This file is the source for the message catalog for dticon
$ **
$ **
$ *****************************************************************************
$ **
$ **     (c) Copyright 1992, 1993, 1994 Hewlett-Packard Company
$ **     (c) Copyright 1993, 1994 International Business Machines Corp.
$ **     (c) Copyright 1993, 1994 Sun Microsystems, Inc.
$ **     (c) Copyright 1993, 1994 Unix System Labs, Inc., a subsidiary of
$ **         Novell, Inc.
$ **
$ **     All Rights reserved
$ **
$ **
$ *****************************************************************************
$ **

$set 2
$ *****************************************************************************
$ ** _DtMessage set for fileIODialog.c
$ **
$ ** messages 10-16 are for the FileSelectionBox widget (default labels)
$ *****************************************************************************

2 图标编辑器－文件 I/O
4 图标编辑器－文件 I/O
6 图标编辑器－保存为
8 图标编辑器－打开文件
10 更新
12 文件夹
14 文件
16 输入文件名：
17 输入文件名：
18 更新
20 未命名的
22 打开
24 保存

$set 4
$ *****************************************************************************
$ ** _DtMessage set for newIconDialog.c
$ **
$ *****************************************************************************

2 newIconDialog
4 图标编辑器－缩放
6 确定
8 取消
10 提示
12 新的宽度：
14 新的高度：

$set 6
$ *****************************************************************************
$ ** _DtMessage set for queryDialog.c
$ **
$ *****************************************************************************

2 图标编辑器－警告
6 确定
$ ** DO NOT LOCALIZE - the user should never see this message
8 Do ya really wanna?
10 取消

$set 8
$ *****************************************************************************
$ ** _DtMessage set for stdErrDialog.c
$ **
$ *****************************************************************************

2 图标编辑器－出错
4 继续

$set 10
$ *****************************************************************************
$ ** _DtMessage set for utils.c
$ **
$ *****************************************************************************

$ ** DO NOT LOCALIZE messages 2-48, they appear in the errorlog only
2 Cannot initialize button icon for circle
4 Cannot initialize button icon for solid circle
6 Cannot initialize button icon for polyline
8 Cannot initialize button icon for polygon
10 Cannot initialize button icon for solid polygon
12 Cannot initialize button icon for ellipse
14 Cannot initialize button icon for solid ellipse
16 Cannot initialize button icon for eraser
18 Cannot initialize button icon for line
20 Cannot initialize button icon for flood
22 Cannot initialize button icon for point
24 Cannot initialize button icon for rectangle
26 Cannot initialize button icon for solid rectangle
28 Cannot initialize button icon for select
30 Unable to parse static color no.
32 Unable to allocate static color no.
38 No memory(0)
40 No memory(1)
42 No memory(2)
44 No memory(3)
46 No memory(4)
48 No memory(5)

50 不能初始化应用程序图标存储
52 无效的使用 '-f' 参数
54 无效的使用 '-i' 参数
56 无效的使用 '-session' 参数
58 无效的大小格式
60 捕捉屏幕图像失败
62 操作失败。\n颜色表满了
64 无效的维数参数

66 （没有命名的）

$set 12
$ *****************************************************************************
$ ** _DtMessage set for dtIconShell.c
$ **
$ *****************************************************************************

1 图标编辑器
2 2 颜色
3 图标编辑器－（未命名的）
4 欢迎使用图标编辑器。
6 填实
8 静态颜色
10 动态颜色
12 静态灰度
14 前景
16 背景
18 顶部阴影
20 底部阴影
22 选择
24 透明

$ ** message 26-198 are menu items with corresponding mnemonics
$ ** (for set grouping information, see the comment at the end-of-file)
26 N
28 新建[N]…
30 O
32 打开[O]…
34 S
36 保存[S]
38 A
40 保存为[A]…
42 x
44 退出[x]
46 F
48 文件[F]
50 U
52 回复[U]
54 t
56 剪切区域[t]
58 C
60 复制区域[C]
62 P
64 粘贴区域[P]
68 L
70 左[L]
72 R
74 右[R]
76 R
78 旋转区域[R]
82 V
84 垂直[V]
86 H
88 水平[H]
90 F
92 翻转区域[F]
94 S
96 比例区域[S]
98 z
100 缩放图标[z]…
102 A
104 增加热点[A]
106 D
108 删除热点[D]
110 G
112 抓取屏幕图像[G]
114 e
116 清除图标[e]
118 E
120 编辑[E]
122 V
124 网格可见[V]
128 B
130 X位图[B]
134 P
136 X象素图[P]
138 O
140 输出格式[O]
144 2x
146 3x
148 4x
150 5x
152 6x
154 8x
156 10x
158 12x
160 M
162 放大[M]
164 O
166 选项[O]
168 v
170 概览[v]…
172 T
174 任务[T]…
176 R
178 参考[R]…
180 O
182 针对项[O]
188 U
190 使用提示[U]…
192 A
194 关于图标编辑器[A]…
196 H
198 提示[H]

$ ** message 200-are used in the "instruction" line for current drawing mode
200 当前编辑模式：
202 （填充的）
204 铅笔
206 快速填充
208 直线
210 折线
212 矩形
214 多边形
216 圆形
218 椭圆形
220 擦除
222 选择
224 无效的

$set 14
$ *****************************************************************************
$ ** _DtMessage set for help.c
$ **
$ *****************************************************************************

2 图标编辑器－提示
4 你必须选择一个在此图标编辑器中的项目。

$set 16
$ *****************************************************************************
$ ** _DtMessage set for process.c
$ **
$ *****************************************************************************

2 此文件不能被访问或包含无效的数据
4 不能写数据到文件
6 此功能目前还不可使用
8 没有前一图像可用
10 指定了无效的宽度与/或高度
12 没有选择的的区域
14 没有先前切割或复制的区域
16 当前图标还没有保存。\n\n将会失去你的更改。
20 检查文件权限
22 不能从文件中恢复会话过程
24 不能保存会话过程到文件中
25 文件已经存在。\n\n可以覆盖此文件吗ⅶ

$ *****************************************************************************
$ ** End of _DtMessage Catalog File
$ ** (Comments only below this line)
$ *****************************************************************************
$ **
$ *****************************************************************************
$ ** Unique mnemonic groupings from set 12
$ *****************************************************************************
$ **
$ ** Unique Set 1:  (Main Menu Bar)
$ **
$ **   46 F
$ **   48 File
$ **
$ **   118 E
$ **   120 Edit
$ **
$ **   164 O
$ **   166 Options
$ **
$ **   196 H
$ **   198 Help
$ **
$ **
$ ** Unique Set 2:  (File Menu)
$ **
$ **   26 N
$ **   28 New...
$ **   30 O
$ **   32 Open...
$ **   34 S
$ **   36 Save
$ **   38 A
$ **   40 Save As...
$ **   42 x
$ **   44 Exit
$ **
$ **
$ ** Unique Set 3:  (Edit Menu)
$ **
$ **   50 U
$ **   52 Undo
$ **   54 t
$ **   56 Cut Area
$ **   58 C
$ **   60 Copy Area
$ **   62 P
$ **   64 Paste Area
$ **
$ **        Unique Set 3a:  (Rotate Sub-Menu)
$ **        68 L
$ **        70 Left
$ **        72 R
$ **        74 Right
$ **   76 R
$ **   78 Rotate Area
$ **
$ **        Unique Set 3b:  (Flip Sub-Menu)
$ **        82 V
$ **        84 Vertical
$ **        86 H
$ **        88 Horizontal
$ **   90 F
$ **   92 Flip Area
$ **   94 S
$ **   96 Scale Area
$ **   98 z
$ **   100 Resize Icon...
$ **   102 A
$ **   104 Add Hot Spot
$ **   106 D
$ **   108 Delete Hot Spot
$ **   110 G
$ **   112 Grab Screen Image
$ **   114 e
$ **   116 Clear Icon
$ **
$ **
$ ** Unique Set 4:  (Options Menu)
$ **
$ **   122 V
$ **   124 Visible Grid
$ **
$ **        Unique Set 4a:  (Output Sub-Menu)
$ **        128 B
$ **        130 XBM
$ **        134 P
$ **        136 XPM
$ **   138 O
$ **   140 Output Format
$ **   160 M
$ **   162 Magnification
$ **
$ **
$ ** Unique Set 5:  (Help Menu)
$ **
$ **   168 I
$ **   170 Introduction...
$ **   172 T
$ **   174 Tasks...
$ **   176 R
$ **   178 Reference...
$ **   180 O
$ **   182 On Item
$ **   188 U
$ **   190 Using Help...
$ **   192 V
$ **   194 Version...
$ **
