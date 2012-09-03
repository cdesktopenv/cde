$ "$XConsortium: dthello.msg /main/4 1996/10/16 03:15:01 cde-hit $";
$ *                                                                      *
$ * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
$ * (c) Copyright 1993, 1994 International Business Machines Corp.       *
$ * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
$ * (c) Copyright 1993, 1994 Novell, Inc.                                *
$ *
$ *************************************<+>*************************************
$ *****************************************************************************
$ **
$ **  File:        dthello.msg
$ **
$ **  Project:     HP DT login transition program (dthello) 
$ **
$ **  Description:
$ **  -----------
$ **  This file is the source for the message catalog for dthello
$ **
$ **
$ *****************************************************************************
$ **
$ **     (c) Copyright 1990, 1991 Hewlett-Packard Company
$ **     All Rights reserved
$ **
$ **
$ *****************************************************************************
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

$set 2
$    THIS COMMENT FOR DT TEAM ONLY -- dthello.c --
$    Do not translate. Blank message, please ignore
1 

$    Message #2 is no longer used. It has been replaced with messag #3
2 COSE\n\
ユーザ環境を起動します。\n\n

$    Default startup message (written before the contents
$    of the default file, /etc/copyright).
3 共通デスクトップ環境の起動\n\n\


$set 4
$    THIS COMMENT FOR DT TEAM ONLY -- dthello.c --
$    This entire set is for error messages
$    DO NOT TRANSLATE THESE MESSAGES
1 %s: displayが開けません。
2 %s: timeoutは正数でなくてはならない。
3 %1$s: 最大 %2$d ファイルまで許されます。 %3$s を省きます。

$ Messages 4 and 5 are both used when there is a syntax error on
$ the command line invoking dthello
$ 
4 使用方法: %s [-display <ディスプレイ>] [-fg <フォアグラウンド・カラー>] [-bg <バックグラウンド・カラー>]\\\n
5 [-font <フォント>] [-string <メッセージ>] [-file <ファイル名>] [-timeout <秒数>]\n

$ _DtMessage 6 occurs when an font can't be loaded
$  the likely causes: (1) the font was incorrectly specified
$                     (2) it is not available on this server
6 %1$s: ディスプレイ %2$s は、フォント %3$s を知りません。\n

7 %1$s: ファイル %2$s を開けません。\n
8 %1$s: %3$s を使用してバックグラウンド・カラーを %2$s に設定できません。\n
9 %1$s: %3$s を使用してフォアグラウンド・カラーを %2$s に設定できません。\n
10 %s: メモリが不足しています。(SeparateTextLines)\n

$set 99
$    DO NOT TRANSLATE THESE MESSAGES
$    DO NOT CHANGE THESE MESSAGES
$    DO NOT LOCALIZE THESE MESSAGES
$    These messages are used for the version information.
1 @(#)version_goes_here
2 \n@(#)_DtMessage catalog source $XConsortium: dthello.msg /main/4 1996/10/16 03:15:01 cde-hit $
