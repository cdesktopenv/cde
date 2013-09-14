
$ "$XConsortium: dtlp.msg /main/3 1995/11/08 13:51:47 rswiston $";

$ *************************************<+>*************************************
$ *****************************************************************************
$ **
$ **  File:        dtlp.msg
$ **
$ **  Project:     DT printer utility (dtlp)
$ **
$ **  Description:
$ **  -----------
$ **  This file is the source for the message catalog for dtlp
$ **
$ **
$ *****************************************************************************
$ **
$ ** (c) Copyright 1990, 1992, 1993, 1994 Hewlett-Packard Company
$ ** (c) Copyright 1994 International Business Machines Corp.
$ ** (c) Copyright 1994 Sun Microsystems, Inc.
$ ** (c) Copyright 1994 Novell, Inc.
$ **
$ **     All Rights reserved
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
$ * You don't need to localize the following sets:
$ *
$ * Please localize the following sets:
$ *  sets 1
$ **  ----------------GENERAL LOCALIZATION NOTES SECTION END------------------
$ *************************************<L>*************************************

$quote "

$set 1
$ 
$  -- The first group of messages all appear on the Print dialog --
$ 


$  This is the message that appears in the "Printer:" field when no
$  printer name has been specified.
10 "默认"

$  This is the label that appears in the title bar of the print dialog.
20 "打印"

$  This is the label for the field indicating the file that will be printed.
30 "文件："

$  This is the label for the field indicating the name of the printer.
40 "打印机："

$  This is the message that will appear in the "Printer:" field if no printer
$  has been specified.
50 "默认"

$  This is the label for the field indicating the number of copies to print.
60 "份数："

$  This is the label for the field indicating the title for the banner page.
70 "标题页标题："

$  This is the label for the field indicating if page numbers are to be added.
80 "打印页数"

$  This is thef label for the field indicating additional options for the
$  "lp" command.
90 "打印命令选项："

$  This is the label on the left button. Pushing this button causes the
$  dialog to disappear and the print operation to start.
100 "打印"

$  This is the label on the center button. Pushing this button causes the
$  dialog to disappear and cancels the print operation.
110 "取消"

$  This is the label on the right button. Pushing this button causes a
$  help dialog to be posted.
120 "提示"

$ 
$  -- The following are error messages. --
$ 

$  This message is posted when there is a problem with the file specified. 
$  The file name will be appended to the message. One additional message
$  from the list below will also be included in the dialog to provide
$  more details.
130 "抱歉－不能打印："

$  This message is added to the error dialog when the file is not printable 
$  or does not exist.
132 "这不是一个可打印文件或文件不存在。"

$  This message is added to the error dialog when the file is not readable 
$  by the user invoking dtlp.
134 "你没有权限读此文件。"

$  This message is added to the error dialog when there is a general failure 
$  in setting up to print.
136 "请检查此文件是否存在。"

$  This is the label that appears in the title bar of the error dialog.
160 "打印信息"



$set 99
$    DO NOT TRANSLATE THESE MESSAGES
$    DO NOT CHANGE THESE MESSAGES
$    DO NOT LOCALIZE THESE MESSAGES
$    These messages are used for the version information.
1 "@(#)version_goes_here"
2 "\n@(#)_DtMessage catalog source $XConsortium: dtlp.msg /main/3 1995/11/08 13:51:47 rswiston $"

