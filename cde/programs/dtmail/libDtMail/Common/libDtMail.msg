$ +SNOTICE
$ 
$ 	$TOG: libDtMail.msg /main/17 1999/03/26 16:52:56 mgreess $
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
1 "No error occured."

$ // DTME_AlreadyLocked
2 "The folder is locked by another session."

$ // DTME_BadArg
3 "A bad argument was passed as a parameter to the operation."

$ // DTME_BadMailAddress
4 "The specified mail address could not be processed."

$ // DTME_BadRunGroup
5 "The dtmail program is not running as group \"mail\"."

$ // DTME_FileCreateFailed
6 "The requested file could not be created."

$ // DTME_FolderNotSet
7 "The mailbox to incorporate was not set."

$ // DTME_GetLockRefused
8 "The user refused to take the lock of a locked mailbox."

$ // DTME_ImplFailure
9 "The specified implementation could not perform the requested operation."

$ // DTME_InitializationFailed
10 "The instance could not be initialized."

$ // DTME_InternalFailure
11 "An internal failure occurred while performing the operation."

$ // DTME_InvalidError
12 "The error structure is invalid."

$ // DTME_InvalidOperationSequence
13 "An operation was attempted before the instance was initialized."

$ // DTME_MD5ChecksumFailed
14 "The MD5 signature did not match the message contents."

$ // DTME_MailTransportFailed
15 "Unable to communicate with the mail transport."

$ // DTME_NoDataType
16 "There is no data type that matches the transport type."

$ // DTME_NoImplementations
17 "No implementations were found for the mail library."

$ // DTME_NoMailBox
18 "The mailbox was uninitialized at load time."

$ // DTME_NoMemory
19 "No memory available for operation."

$ // DTME_NoMsgCat
20 "No message catalog exists for DtMail."

$ // DTME_NoNewMail
21 "There is no new mail to incorporate."

$ // DTME_NoObjectValue
22 "No value for the object could be found."

$ // DTME_NoSuchFile
23 "The mailbox does not exist and creation was not requested."

$ // DTME_NoSuchImplementation
24 "The specified implementation does not exist."

$ // DTME_NoSuchType
25 "The data type is not known to the library."

$ // DTME_NoUser
26 "The user for this session could not be identified."

$ // DTME_NotInbox
27 "The file specified is not a mailbox."

$ // DTME_NotLocked
28 "The mailbox is not locked for access."

$ // DTME_NotMailBox
29 "The requested file is not a mailbox in any format recognized by this implementation."

$ // DTME_NotSupported
30 "The operation is not supported by the current implementation."

$ // DTME_ObjectAccessFailed
31 "Unable to access an object required to complete the operation."

$ // DTME_ObjectCreationFailed
32 "Unable to create an object required to complete the operation."

$ // DTME_ObjectInUse
33 "An attempt was made to initialize an object that was already initialized."

$ // DTME_ObjectInvalid
34 "An invalid object was referenced."

$ // DTME_OpenContainerInterrupted
35 "The user interrupted the process of opening a mailbox."

$ // DTME_OperationInvalid
36 "An internal error occurred while performing the operation."

$ // DTME_OtherOwnsWrite
37 "Another mail program owns the write access to the mailbox."

$ // DTME_RequestDenied
38 "The other session denied the request for lock or copy."

$ // DTME_TTFailure
39 "A ToolTalk message could not be processed."

$ // DTME_TransportFailed
40 "The mail delivery transport failed."

$ // DTME_UnknownFormat
41 "The message is not in one of the supported formats."

$ // DTME_UnknownOpenError
42 "An unknown error occurred when opening a mailbox."

$ // DTME_UserAbort
43 "The user aborted the operation."

$ // DTME_UserInterrupted
44 "The user interrupted the operation."

$ // DTME_ObjectReadOnly
45 "The mailbox permissions only allow read access."

$ // DTME_NoPermission
46 "The user does not have access to the mailbox."

$ // DTME_IsDirectory
47 "The specified path is a directory."

$ // DTME_CannotRemoveStaleMailboxLockFile
48 "Cannot lock mailbox (could not remove stale lock file).\nStale lock file: %s\nReason: %s"

$ // DTME_CannotCreateMailboxLockFile
49 "Cannot lock mailbox (could not create lock file).\nMailbox lock file: %s\nReason: %s"

$ // DTME_CannotCreateMailboxLockFile_NoPermission
50 "Cannot create lock file and lock mailbox because the user does not have\naccess to either the mailbox or the directory containing the mailbox."

$ // DTME_CannotCreateMailboxLockFile_IsDirectory
51 "Cannot create lock file and lock mailbox because the name for the\nmailbox lock file already exists and is a directory."

$ // DTME_CannotCreateMailboxLockFile_NoSuchFile
52 "Cannot create lock file and lock mailbox because a component of the\npath name of the lock file is not a directory."

$ // DTME_CannotCreateMailboxLockFile_RemoteAccessLost
53 "Cannot create lock file and lock mailbox because the remote system on\nwhich the lock file was to be created is no longer accessible."

$ // DTME_CannotObtainInformationOnOpenMailboxFile
54 "Cannot obtain information on current mailbox file.\nMailbox file: %s\nReason: %s"

$ // DTME_CannotCreateTemporaryMailboxFile
55 "Cannot create temporary mailbox file.\nTemporary Mailbox file: %s\nReason: %s"

$ // DTME_CannotCreateTemporaryMailboxFile_NoPermission
56 "Cannot create temporary mailbox file because the user does not have\naccess to either the mailbox or the directory containing the mailbox."

$ // DTME_CannotCreateTemporaryMailboxFile_IsDirectory
57 "Cannot create temporary mailbox file because the name for the\ntemporary mailbox already exists and is a directory."

$ // DTME_CannotCreateTemporaryMailboxFile_NoSuchFile
58 "Cannot create temporary mailbox file because a component of the\npath name of the temporary file is not a directory."

$ // DTME_CannotCreateTemporaryMailboxFile_RemoteAccessLost
59 "Cannot create temporary mailbox file because the remote system on\nwhich the file was to be created is no longer accessible."

$ // DTME_CannotSetPermissionsOfTemporaryMailboxFile
60 "Cannot set permissions on temporary mailbox file.\nTemporary Mailbox file: %s\nPermissions requested: %o\nReason: %s"

$ // DTME_CannotSetOwnerOfTemporaryMailboxFile
61 "Cannot set owner of temporary mailbox file.\nTemporary Mailbox file: %s\nOwner uid requested: %d\nReason: %s"

$ // DTME_CannotSetGroupOfTemporaryMailboxFile
62 "Cannot set group of temporary mailbox file.\nTemporary Mailbox file: %s\nGroup gid requested: %d\nReason: %s"

$ // DTME_CannotWriteToTemporaryMailboxFile
63 "Cannot write to temporary mailbox file.\nTemporary Mailbox file: %s\nReason: %s"

$ // DTME_CannotWriteToTemporaryMailboxFile_ProcessLimitsExceeded
64 "Cannot write to temporary mailbox file because the process's file\nsize limit or the maximum file size has been reached."

$ // DTME_CannotWriteToTemporaryMailboxFile_RemoteAccessLost
65 "Cannot write to temporary mailbox file because the remote system on\nwhich the file was created is no longer accessible."

$ // DTME_CannotWriteToTemporaryMailboxFile_NoFreeSpaceLeft
66 "Cannot write to temporary mailbox file because there is no free\nspace left on the device on which the file was created."

$ // DTME_CannotReadNewMailboxFile
67 "Cannot read new mailbox file\nReason: %s"

$ // DTME_CannotReadNewMailboxFile_OutOfMemory
68 "Cannot read new mailbox file because no memory is available for the operation."

$ // DTME_CannotRemoveMailboxLockFile
69 "Cannot unlock mailbox (could not remove lock file).\nMailbox lock file: %s\nReason: %s"

$ // DTME_CannotRenameNewMailboxFileOverOld
70 "Cannot rename new mailbox file over old mailbox file.\nOld mailbox file still exists but complete and correct contents\nof mailbox contents have been saved in the new mailbox file.\nThis problem must be corrected manually as soon as possible.\nOld Mailbox file: %s\nNew Mailbox file: %s\nReason: %s"

$ // DTME_InternalAssertionFailure
71 "An internal error has occurred within this application.\nThere is no way to recover and continue from this error.\nError condition: %s\n"

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
