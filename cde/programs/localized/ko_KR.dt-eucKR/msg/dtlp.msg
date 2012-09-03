
$ "$XConsortium: dtlp.msg /main/3 1995/11/08 13:17:42 rswiston $";

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
10 "기본값"

$  This is the label that appears in the title bar of the print dialog.
20 "인쇄"

$  This is the label for the field indicating the file that will be printed.
30 "파일:"

$  This is the label for the field indicating the name of the printer.
40 "프린터:"

$  This is the message that will appear in the "Printer:" field if no printer
$  has been specified.
50 "기본값"

$  This is the label for the field indicating the number of copies to print.
60 "인쇄매수:"

$  This is the label for the field indicating the title for the banner page.
70 "표지 제목:"

$  This is the label for the field indicating if page numbers are to be added.
80 "페이지 번호 인쇄"

$  This is thef label for the field indicating additional options for the
$  "lp" command.
90 "인쇄 명령 선택사항:"

$  This is the label on the left button. Pushing this button causes the
$  dialog to disappear and the print operation to start.
100 "인쇄"

$  This is the label on the center button. Pushing this button causes the
$  dialog to disappear and cancels the print operation.
110 "취소"

$  This is the label on the right button. Pushing this button causes a
$  help dialog to be posted.
120 "도움말"

$ 
$  -- The following are error messages. --
$ 

$  This message is posted when there is a problem with the file specified.
$  The file name will be appended to the message. One additional message
$  from the list below will also be included in the dialog to provide
$  more details.
130 "죄송합니다. -- 인쇄할 수 없습니다."

$  This message is added to the error dialog when the file is not printable
$  or does not exist.
132 "이것은 없거나 인쇄할 수 없는 파일입니다."

$  This message is added to the error dialog when the file is not readable
$  by the user invoking dtlp.
134 "사용자는 해당 파일을 읽기 위한 사용 권한을 가지고 있지 않습니다."

$  This message is added to the error dialog when there is a general failure
$  in setting up to print.
136 "이 파일이 있는지 점검해 주십시오."

$  This is the label that appears in the title bar of the error dialog.
160 "인쇄 정보"



$set 99
$    DO NOT TRANSLATE THESE MESSAGES
$    DO NOT CHANGE THESE MESSAGES
$    DO NOT LOCALIZE THESE MESSAGES
$    These messages are used for the version information.
1 @(#)version_goes_here
2 \n@(#)_DtMessage catalog source $XConsortium: dtlp.msg /main/3 1995/11/08 13:17:42 rswiston $


