$ +SNOTICE
$ 
$ 	$TOG: libDtMail.msg /main/19 1999/03/26 16:48:27 mgreess $
$ 
$ 	RESTRICTED CONFIDENTIAL INFORMATION:
$ 	
$ 	The information in this document is subject to special
$ 	restrictions in a confidential disclosure agreement bertween
$ 	HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
$ 	document outside HP, IBM, Sun, USL, SCO, or Univel wihtout
$ 	Sun's specific written approval.  This documment and all copies
$ 	and derivative works thereof must be returned or destroyed at
$ 	Sun's request.
$ 
$ 	Copyright 1993, 1994, 1995 Sun Microsystems, Inc.  All rights reserved.
$ 
$ +ENOTICE
$ @(#)libDtMail.msg	1.9 23 May 1995

$quote "

$set 1 libDtMail API set
$ // DTME_NoError
1 "エラーは発生しませんでした。"

$ // DTME_AlreadyLocked
2 "フォルダは別のセッションによりロックされています。"

$ // DTME_BadArg
3 "操作にパラメータとして不正な引き数が渡されました。"

$ // DTME_BadMailAddress
4 "指定のメールアドレスを処理できません。"

$ // DTME_BadRunGroup
5 "メールプログラムをグループ \"mail\" として実行していません。"

$ // DTME_FileCreateFailed
6 "要求されたファイルを作成できません。"

$ // DTME_FolderNotSet
7 "取り入れようとしたメールボックスは設定されていません。"

$ // DTME_GetLockRefused
8 "ユーザはロックされているメールボックスのロック解除を拒否しました。"

$ // DTME_ImplFailure
9 "指定の実装は要求した操作を実行できません。"

$ // DTME_InitializationFailed
10 "インスタンスを初期化できません。"

$ // DTME_InternalFailure
11 "操作を実行中に内部障害が発生しました。"

$ // DTME_InvalidError
12 "エラー構造が無効です。"

$ // DTME_InvalidOperationSequence
13 "インスタンスを初期化する前に操作を実行しようとしました。"

$ // DTME_MD5ChecksumFailed
14 "MD5 シグニチャーがメッセージ内容と一致しません。"

$ // DTME_MailTransportFailed
15 "メールトランスポートと通信できません。"

$ // DTME_NoDataType
16 "トランスポート型と一致するデータ型がありません。"

$ // DTME_NoImplementations
17 "メールライブラリのための実装が見つかりません。"

$ // DTME_NoMailBox
18 "メールボックスが読み込み時に初期化されません。"

$ // DTME_NoMemory
19 "操作に使用できるメモリがありません。"

$ // DTME_NoMsgCat
20 "メールプログラムのメッセージカタログが存在しません。"

$ // DTME_NoNewMail
21 "新規メールで取り入れていないものはありません。"

$ // DTME_NoObjectValue
22 "オブジェクトの値が見つかりません。"

$ // DTME_NoSuchFile
23 "メールボックスは存在せず、生成を要求していません。"

$ // DTME_NoSuchImplementation
24 "指定の実装は存在しません。"

$ // DTME_NoSuchType
25 "データ型はライブラリにありません。"

$ // DTME_NoUser
26 "このセッションのユーザを特定できません。"

$ // DTME_NotInbox
27 "指定のファイルはメールボックスではありません。"

$ // DTME_NotLocked
28 "メールボックスはアクセスをロックしていません。"

$ // DTME_NotMailBox
29 "要求ファイルは、この実装が認識する形式のメールボックスではありません。"

$ // DTME_NotSupported
30 "現在の実装では操作をサポートしていません。"

$ // DTME_ObjectAccessFailed
31 "操作を完了するのに必要なオブジェクトにアクセスできません。"

$ // DTME_ObjectCreationFailed
32 "操作を完了するのに必要なオブジェクトを作成できません。"

$ // DTME_ObjectInUse
33 "初期化済みのオブジェクトを初期化しようとしました。"

$ // DTME_ObjectInvalid
34 "無効なオブジェクトを参照しました。"

$ // DTME_OpenContainerInterrupted
35 "メールボックスを開くプロセスにユーザが割り込みました。"

$ // DTME_OperationInvalid
36 "操作を実行中に内部エラーが発生しました。"

$ // DTME_OtherOwnsWrite
37 "別のメールプログラムがメールボックスへの書き込み権を持っています。"

$ // DTME_RequestDenied
38 "他のセッションは、ロックまたはコピー要求を拒否しました。"

$ // DTME_TTFailure
39 "ToolTalk メッセージを処理できません。"

$ // DTME_TransportFailed
40 "メールデリバリのトランスポートに失敗しました。"

$ // DTME_UnknownFormat
41 "メッセージがサポートされている形式ではありません。"

$ // DTME_UnknownOpenError
42 "メールボックスを開いている時に未知のエラーが発生しました。"

$ // DTME_UserAbort
43 "ユーザが操作を中止しました。"

$ // DTME_UserInterrupted
44 "ユーザが操作に割り込みました。"

$ // DTME_ObjectReadOnly
45 "メールボックスへのアクセス権は、読み取りのみ許可されています。"

$ // DTME_NoPermission
46 "ユーザはメールボックスへのアクセス権を持っていません。"

$ // DTME_IsDirectory
47 "指定のパスはディレクトリです。"

$ // DTME_CannotRemoveStaleMailboxLockFile
48 "メールボックスをロックできません (旧ロックファイルを削除できません)。\n旧ロックファイル: %s\n理由: %s"

$ // DTME_CannotCreateMailboxLockFile
49 "メールボックスをロックできません (ロックファイルを作成できません)。\メールボックスロックファイル: %s\n理由: %s"

$ // DTME_CannotCreateMailboxLockFile_NoPermission
50 "ユーザがメールボックスまたはメールボックスを含むディレクトリにアクセス権を\n持っていないので、ロックファイルおよびロックメールボックスを作成できません。"

$ // DTME_CannotCreateMailboxLockFile_IsDirectory
51 "メールボックスのロックファイル名が既に存在し、ディレクトリであるため、\nロックファイルおよびロックメールボックスを作成できません。"

$ // DTME_CannotCreateMailboxLockFile_NoSuchFile
52 "ロックファイルのパス名のコンポーネントがディレクトリではないので、\nロックファイルおよびロックメールボックスを作成できません。"

$ // DTME_CannotCreateMailboxLockFile_RemoteAccessLost
53 "ロックファイルを作成しようとしていたリモートシステムにアクセスできなくなった\nため、ロックファイルおよびロックメールボックスを作成できません。"

$ // DTME_CannotObtainInformationOnOpenMailboxFile
54 "現在のメールボックスファイルにある情報を取得できません。\nメールボックスファイル: %s\n理由: %s"

$ // DTME_CannotCreateTemporaryMailboxFile
55 "一時メールボックスファイルを作成できません。\nメールボックスファイル: %s\n理由: %s"

$ // DTME_CannotCreateTemporaryMailboxFile_NoPermission
56 "ユーザがメールボックスまたはメールボックスを含むディレクトリにアクセス権を\n持~っていないので、一時メールボックスファイルを作成できません。"

$ // DTME_CannotCreateTemporaryMailboxFile_IsDirectory
57 "一時メールボックス名が既に存在し、ディレクトリであるため、\n一時メールボックスファイルを作成できません。"

$ // DTME_CannotCreateTemporaryMailboxFile_NoSuchFile
58 "一時ファイルのパス名のコンポーネントがディレクトリでないため、\n一時メールボックス~ファイルを作成できません。"

$ // DTME_CannotCreateTemporaryMailboxFile_RemoteAccessLost
59 "ファイルを作成しようとしていたリモートシステムにアクセスできなくなったため、\n一時メールボックスファイルを作成できません。"

$ // DTME_CannotSetPermissionsOfTemporaryMailboxFile
60 "一時メールボックスファイルにアクセス権を設定できません。\n一時メールボックスファイル: %s\n要求したアクセス権: %o\n理由: %s"

$ // DTME_CannotSetOwnerOfTemporaryMailboxFile
61 "一時メールボックスファイルの所有者を設定できません。\n一時メールボックスファイル: %s\n要求した所有者のユーザ ID: %d\n理由: %s"

$ // DTME_CannotSetGroupOfTemporaryMailboxFile
62 "一時メールボックスファイルのグループを設定できません。\n一時メールボックスファイル: %s\n要求したグループのグループ ID: %d\n理由: %s"

$ // DTME_CannotWriteToTemporaryMailboxFile
63 "一時メールボックスファイルに書き込めません。\n一時メールボックスファイル: %s\n理由: %s"

$ // DTME_CannotWriteToTemporaryMailboxFile_ProcessLimitsExceeded
64 "プロセスのファイルサイズの制限またはファイルの最大サイズに達したため、\n一時メールボックスファイルに書き込めません。"

$ // DTME_CannotWriteToTemporaryMailboxFile_RemoteAccessLost
65 "ファイルを作成しようとしていたリモートシステムにアクセスできなくなったため、\n~一時メールボックスファイルに書き込めません。"

$ // DTME_CannotWriteToTemporaryMailboxFile_NoFreeSpaceLeft
66 "ファイルを作成しているデバイス上に空き領域がないため、\n一時メールボックスファイルに書き込めません。"

$ // DTME_CannotReadNewMailboxFile
67 "新規メールボックスファイルを読めません。\n理由: %s"

$ // DTME_CannotReadNewMailboxFile_OutOfMemory
68 "操作に使用できるメモリがないので、新規メールボックスファイルを読めません。"

$ // DTME_CannotRemoveMailboxLockFile
69 "メールボックスをロック解除できません (ロックファイルを削除できません)。\nメールボックスロックファイル: %s\n理由: %s"

$ // DTME_CannotRenameNewMailboxFileOverOld
70 "旧メールボックスファイルを新規メールボックスファイルに置き換え、\n名前を変更することはできません。\n旧メールボックスファイルは存在しますが、メールボックスの内容はすべて\n新規メールボックスファイルに正しく保存されています。\n早急にこの問題を手動で修正する必要があります。\n旧メールボックスファイル: %s\n新規メールボックスファイル: %s\n理由: %s"

$ // DTME_InternalAssertionFailure
71 "このアプリケーション内で内部エラーが発生しました。\nこのエラーを回復し、ここから継続する方法はありません。\nエラー条件: %s\n"

$ "-NEW"
$ // - DTME_ResourceParsingNoEndif
$ 
72 "An error occurred while parsing the .mailrc resource file.\nThere is a conditional if statement that does not have a corresponding endif statement.\n"
  
$ "-NEW"
$ // - DTME_AlreadyOpened
$ 
73 "This mail folder is already opened."

$ "-NEW"
$ // - DTME_OutOfSpace
$ 
74 "No Space on Temporary Filesystem."

$ "-NEW"
$ // - DTME_CannotCreateMailboxDotDtmailLockFile
75 "Mailer has detected a mailbox lockfile:\n%s"

$ "-NEW"
$ // - DTME_MailboxInodeChanged
76 "Mailer can no longer access this mailbox.\nIt would be best to close and reopen it."

$ "-NEW"
$ // - DTME_MailServerAccess_AuthorizationFailed
77 "Cannot retrieve mail for '%s@%s' using '%s'.\nAuthorization failed.  The server returned:\n     %s\n\nPlease enter a new password"
  
$ "-NEW"
$ // - DTME_MailServerAccess_Error
78 "Cannot retrieve mail for '%s@%s' using '%s'.\nThe server returned:\n     %s"

$ "-NEW"
$ // - DTME_MailServerAccess_MissingPassword
79 "Please enter a password for '%s@%s' using '%s'"

$ "-NEW"
$ // - DTME_MailServerAccess_ProtocolViolation
80 "Cannot retrieve mail for '%s@%s' using '%s'.\nClient/server protocol error."
  
$ "-NEW"
$ // - DTME_MailServerAccess_ServerTimeoutError
81 "Cannot retrieve mail for '%s@%s' using '%s'.\nTimed out waiting for server."
  
$ "-NEW"
$ // - DTME_MailServerAccess_SocketIOError
82 "Cannot retrieve mail for '%s@%s' using '%s'.\n%s."
  
$ "-NEW"
$ // - DTME_AppendMailboxFile_Error
83 "Cannot append to mailbox"
  
$ "-NEW"
$ // - DTME_AppendMailboxFile_FileTooBig
84 "Cannot append to mailbox:  %s\nSYSERROR(%d):  %s."
  
$ "-NEW"
$ // - DTME_AppendMailboxFile_LinkLost
85 "Cannot append to mailbox:  %s\nSYSERROR(%d):  %s."
  
$ "-NEW"
$ // - DTME_AppendMailboxFile_NoSpaceLeft
86 "Cannot append to mailbox:  %s\nSYSERROR(%d):  %s."
  
$ "-NEW"
$ // - DTME_AppendMailboxFile_SystemError
87 "Cannot append to mailbox:  %s\nSYSERROR(%d):  %s."

$ "-NEW"
$ // - DTME_GetmailCommandRetrieval_SystemError
88 "Getmail command failed:  %s\nSYSERROR(%d):  %s."

$ "-NEW"
$ // - DTME_GetmailCommandRetrieval_AbnormalExit
89 "Getmail command exited abnormally:  %s."
  
$ "-NEW"
$ // - DTME_PathElementPermissions
90 "Search permission denied on a component of the path prefix,\n'%s'."

$ "-NEW"
$ // - DTME_PathElementNotDirectory
91 "A component of the path prefix is not a directory,\n'%s'."

$ "-NEW"
$ // - DTME_PathElementDoesNotExist
92 "A component of the path prefix does not exist,\n'%s'."

$ "-NEW"
$ // - DTME_MailServerAccessInfo_SocketOpen
93 "Opening connection for '%s@%s'."
  
$ "-NEW"
$ // - DTME_MailServerAccessInfo_NoMessages
94 "No messages for '%s@%s'."
  
$ "-NEW"
$ // - DTME_MailServerAccessInfo_RetrievingMessage
95 "Retrieving message %d of %d for '%s@%s'."
 
$ "-NEW"
$ // - DTME_MailServerAccessInfo_MessageTooLarge
96 "Skipping oversized message (%d bytes)."
  

$ 
$ Strings from libDtMail/Common/DtMailValuesBuiltin.C

$set 2
$ /* NL_COMMENT
$  * The %C is the time and date format, please refer to strftime man page for 
$  * explanation of each format.
$  */
1 "%C"
  

$ 
$ SockOpen error strings from libDtMail/Common/IO.C

$ "-NEW"
2 "Unknown host:  %s"

$ "-NEW"
3 "Error creating socket: %s"

$ "-NEW"
4 "Error connecting to socket:  %s"


$ 
$ FileShare error strings from libDtMail/Common/FileShare.C

$ "-NEW"
5 "Another user would like your lock."

$ "-NEW"
6 "Another session has this mailbox locked. Request access?"

$ "-NEW"
7 "Unable to obtain lock, open this mailbox as read only?"

$ "-NEW"
8 "Unable to obtain lock because system not responding, open this mailbox as read only, read write, or cancel?"

$ "-NEW"
9 "Another user has taken your lock."
