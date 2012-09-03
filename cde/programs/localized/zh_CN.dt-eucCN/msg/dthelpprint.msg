$ $XConsortium: dthelpprint.msg /main/3 1995/11/08 13:50:51 rswiston $
$ *************************************<+>*************************************
$ *****************************************************************************
$ **
$ **  File:        dthelpprint.msg
$ **
$ **  Project:     Cde1 Help
$ **
$ **  Description:
$ **  -----------
$ **  This file is the source for the message catalog for dthelpprint
$ **
$ **
$ *****************************************************************************
$ **
$ ** (c) Copyright 1993, 1994 Hewlett-Packard Company
$ ** (c) Copyright 1993, 1994 International Business Machines Corp.
$ ** (c) Copyright 1993, 1994 Sun Microsystems, Inc.
$ ** (c) Copyright 1993, 1994 Unix System Labs, Inc.,a subsidiary of Novell,Inc.
$ **         All Rights reserved
$ **
$ **
$ **     There are two types of messages in this file:
$ **
$ **     1) Messages that appear in printed output
$ **        These messages are the default and they should all be localized.
$ **        These messages are marked with LOCALIZE THESE MESSAGES.
$ **
$ **     2) Messages that should not be localized.
$ **        These messages are marked with DO NOT LOCALIZE THESE MESSAGES.
$ **
$ *****************************************************************************
$ **
$ **
$ **  ------------------------- MODIFICATION RECORD --------------------------
$ *   Major Mods, 7/11/94 for dthelpprint
$ *
$ **  ----------------------- MODIFICATION RECORD END ------------------------
$ *****************************************************************************
$ *************************************<+>*************************************

$ Use the double quote char around all messages
$quote "

$set 2
$ *****  Module: PrintUtil.c *****
$ 

$set 3
$ *****  Module: PrintTopics.c *****
$ 
$ ** LOCALIZE THESE MESSAGES **
1 "%s 出错: 提示类型是一个题目，但没有指定提示卷。\n"
2 "%s 出错: 不能定位提示卷 %s\n"
3 "%s 出错: 处理提示卷 %s 时有问题\n"
4 "%s 出错: 存储器分配失败\n"
5 "%s 出错: 不能获得题目信息:\n卷 %s，定位标识符 %s\n"
6 "%s 出错: 不能打开临时文件 %s\n"
$ String used to replace $SECTNUM when printing the index section
10 "索引"
$ String used to replace $SECTNUM when printing the table of contents section
11 "目录"
$ ** DO NOT LOCALIZE THESE MESSAGES **
$ index entry, page number
20 "%s，%d\n"
$ index subentry, page number
21 "     %s，%d\n"
$ location ID associated with the index in the TOC maintained by dthelpprint
30 "__GENERATED-索引"
$ string used by can't get a topic title associated with an index subentry
31 ""

$set 4
$ *****  Module: PrintManStrFile.c *****
$ 
$ ** LOCALIZE THESE MESSAGES **
1 "%s 出错: 提示类型是字符串，但没有指定字符串数据。\n"
2 "%s 出错: 提示类型是动态字符串，但没有指定字符串数据。\n"
3 "%s 出错: 提示类型是联机手册，但没有指定联机手册。\n"
4 "%s 出错: 提示类型是文件，但没有指定提示文件。\n"
5 "%s 出错: 存储器分配失败\n"

$set 5
$ *****  Module: Initialize.c *****
$ 
$ ** LOCALIZE THESE MESSAGES **
$ Messages 1 to 29: dthelpprint usage message
$ **DO NOT LOCALIZE** the command line option names (e.g. -copies).
$ **DO LOCALIZE** the option argument (e.g. number) and description.
1 "dthelpprint - 用于提示的打印程序\n\n"
2 "用法: dthelpprint [选项]\n"
3 "控制如何打印的选项:\n"
4 "\t-printer 打印机名     要使用的打印机\n"
5 "\t-copies 数目          要打印的份数\n"
6 "\t-outputFile 文件名    写输出到此文件\n"
7 "\t-paperSize 大小       将内容按此纸张大小排格式\n"
$ Do NOT localize the paper size names help_papersize_xxx
8 "\t\t大小 = { help_papersize_letter|help_papersize_legal|\n"
9 "\t\t         help_papersize_executive|help_papersize_a4|help_papersize_b5}\n"
10 "\t-display 显示名      从中获取资源的显示\n"
11 "\t-name                获取资源时使用的程序名\n"
12 "\t-class               获取资源时使用的类名\n"
13 "\t-xrm 资源字符串      额外的资源\n"
14 "控制打印内容的选项:\n"
15 "\t-helpType 类型       提示数据的类型\n"
16 "\t\t类型 = 0（提示卷），1（字符串），2（联机手册），3（提示文件）\n"
17 "\t-helpVolume 卷       提示卷文件的全路径\n"
18 "\t-locationId 定位     在该卷中的提示题目的名字\n"
19 "\t-all                 打印该提示卷中所有的题目、目录、和索引\n"
20 "\t-sub                 打印题目定位标识符和所有的子题目\n"
21 "\t-one                 打印题目定位标识符\n"
22 "\t-toc                 打印提示卷目录\n"
23 "\t-index               打印提示卷索引\n"
24 "\t-frontMatter         打印提示卷正文前的材料\n"
25 "\t-manPage 联机手册名  联机手册的名字\n"
26 "\t-stringData 字符串   要打印的提示文本\n"
27 "\t-helpFile 文件名     包含提示文本的文件\n"
28 "\t-jobTitle 标题       打印作业的标题字符串\n"
29 "\t-topicTitle 标题     提示文本的标题字符串\n"
$ Warning and error messages
$ Do NOT localize the default size name help_papersize_letter
40 "%s 警告: 非法的纸张大小‘%s’。使用了 help_papersize_letter。\n"
41 "%s 警告: 缺少纸张大小，高度，或者宽度值。使用了 help_papersize_letter。\n"
42 "%s 警告: 不能打开显示%s\n"
45 "出错: 不能为临时文件分配存储器\n"
$ ** DO NOT LOCALIZE THESE MESSAGES **
$ arg order: directory prefix processid filecnt suffix
50 "%1$s/%2$s%3$d_%4$d%5$s"


$set 6
$ *****  Module: Main.c *****
$ 
$ ** LOCALIZE THESE MESSAGES **
1 "%s 出错: 非法的提示类型 %d。\n"

