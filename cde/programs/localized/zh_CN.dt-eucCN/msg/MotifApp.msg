$ 
$ +SNOTICE
$ 
$  $XConsortium: MotifApp.msg /main/3 1995/11/08 13:48:08 rswiston $
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

$ -------------------------------------
$ MotifApp/Application.C
$ 

$quote "

$ -------------------------------------
$ MotifApp/AskFirstCmd.C
$ 

$quote "


$set 1
1 "你确实想执行此命令吗ⅶ" 
$ -------------------------------------
$ MotifApp/DialogManager.C
$ 

$quote "


2 "确定" 
3 "取消" 
$ -------------------------------------
$ MotifApp/Help.C
$ 

$quote "


4 "DtMail 版本对话" 
5 "DtMail 版本对话" 
6 "你必须选择一个在你的应用内的组成部分。\n" 

$ Messages 7 and 8 are no longer used.  They are both
$ replaced by message 9.

7 "DtMail 提示" 
8 "DtMail 提示" 


$quote "


9 "关闭此文件夹?" 
$ -------------------------------------
$ MotifApp/WarnNoUndoCmd.C
$ 

$quote "


10 "此命令不能被回复。还要继续吗?" 
11 "显示隐藏的文件夹和文件吗?"

$quote "

$ /* NL_COMMENT
$  * This message replaces messages 7 and 8.
$  */
12 "Mailer : 提示"

$set 2
1 "选择出错，无法继续\n" 
2 "选择被用户异常终止。\n" 
$ -------------------------------------
$ MotifApp/QuitCmd.C
$ 

