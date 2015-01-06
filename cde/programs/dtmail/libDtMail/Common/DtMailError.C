/*
 * CDE - Common Desktop Environment
 *
 * Copyright (c) 1993-2012, The Open Group. All rights reserved.
 *
 * These libraries and programs are free software; you can
 * redistribute them and/or modify them under the terms of the GNU
 * Lesser General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * These libraries and programs are distributed in the hope that
 * they will be useful, but WITHOUT ANY WARRANTY; without even the
 * implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with these librararies and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
/*
 *+SNOTICE
 *
 *	$TOG: DtMailError.C /main/18 1999/02/08 09:32:25 mgreess $
 *
 *	RESTRICTED CONFIDENTIAL INFORMATION:
 *	
 *	The information in this document is subject to special
 *	restrictions in a confidential disclosure agreement between
 *	HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
 *	document outside HP, IBM, Sun, USL, SCO, or Univel without
 *	Sun's specific written approval.  This document and all copies
 *	and derivative works thereof must be returned or destroyed at
 *	Sun's request.
 *
 *	Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 *
 *+ENOTICE
 */

#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <assert.h>
#include <errno.h>
#include <DtMail/DtMail.hh>
#include <DtMail/DtMailError.hh>
#include <DtMail/Common.h>
#include <Dt/MsgCatP.h>

#include <syslog.h>

#if defined(HPUX) && !defined(hpV4)
// HP-UX 9.* syslog.h does not define these.
//
extern "C" int syslog(int, const char *, ...);
extern "C" int openlog(const char *, int, int);
#endif

#include <EUSDebug.hh>

// Provide interface to the DtSvc function DtSimpleError
// When this interface is better defined, this can be removed
// and replaced with the appropriate include file

typedef enum {
  DtIgnore,
  DtInformation,
  DtWarning,
  DtError,
  DtFatalError,
  DtInternalError
} DtSeverity;

extern "C" void _DtSimpleError( 
                        char *progName,
                        DtSeverity severity,
                        char *help,
                        char *format,
                        ...) ;

int			DtMailDebugLevel = 0;
DtMailBoolean		DtMailEnv::_syslog_open = DTM_FALSE;
nl_catd			DtMailEnv::_errorCatalog = (nl_catd) -1;  
const char *		DtMailEnv::DtMailCatalogDataFile = "libDtMail";
nl_catd 		DtMailMsgCat = DtMailEnv::_errorCatalog;// COMPATIBILITY


//
// NOTE - IMPORTANT -- READ ME
//
// The order of the strings below must match the
// entries in DtMail/MailError.h.
//
static const char * error_strings[] = {

  // - DTME_NoError
  "No error occurred.",

  // - DTME_AlreadyLocked
  "The mailbox is locked by another session.",

  // - DTME_BadArg
  "A bad argument was passed as a parameter to the operation.",

  // - DTME_BadMailAddress
  "The specified mail address could not be processed.",
  
  // - DTME_BadRunGroup
  "The dtmail program is not running as group \"mail\".",

  // - DTME_FileCreateFailed
  "The requested file could not be created.",

  // - DTME_FolderNotSet
  "The mailbox to incorporate was not set.",

  // - DTME_GetLockRefused
  "The user refused to take the lock of a locked mailbox.",

  // - DTME_ImplFailure
  "The specified implementation could not perform the requested operation.",

  // - DTME_InitializationFailed
  "The instance could not be initialized.",

  // - DTME_InternalFailure
  "An internal failure occurred while performing the operation.",

  // - DTME_InvalidError
  "The error structure is invalid.",

  // - DTME_InvalidOperationSequence
  "An operation was attempted before the instance was initialized.",

  // - DTME_MD5ChecksumFailed
  "The MD5 signature did not match the message contents.",

  // - DTME_MailTransportFailed
  "Unable to communicate with the mail transport.",

  // - DTME_NoDataType
  "There is no data type that matches the transport type.",

  // - DTME_NoImplementations
  "No implementations were found for the mail library.",

  // - DTME_NoMailBox
  "The mailbox was uninitialized at load time.",

  // - DTME_NoMemory
  "No memory available for operation.",

  // - DTME_NoMsgCat
  "No message catalog exists for DtMail.",

  // - DTME_NoNewMail
  "There is no new mail to incorporate.",

  // - DTME_NoObjectValue
  "No value for the object could be found.",

  // - DTME_NoSuchFile
  "The mailbox does not exist and creation was not requested.",

  // - DTME_NoSuchImplementation
  "The specified implementation does not exist.",

  // - DTME_NoSuchType
  "The data type is not known to the library.",

  // - DTME_NoUser
  "The user for this session could not be identified.",

  // - DTME_NotInbox
  "The file specified is not a mailbox.",

  // - DTME_NotLocked
  "The mailbox is not locked for access.",

  // - DTME_NotMailBox
  "The requested file is not a mailbox in any format recognized by this implementation.",

  // - DTME_NotSupported
  "The operation is not supported by the current implementation.",

  // - DTME_ObjectAccessFailed
  "Unable to access an object required to complete the operation.",

  // - DTME_ObjectCreationFailed
  "Unable to create an object required to complete the operation.",

  // - DTME_ObjectInUse
  "An attempt was made to initialize an object that was already initialized.",

  // - DTME_ObjectInvalid
  "An invalid object was referenced.",

  // - DTME_OpenContainerInterrupted
  "The user interrupted the process of opening a mailbox.",

  // - DTME_OperationInvalid
  "An internal error occurred while performing the operation.",

  // - DTME_OtherOwnsWrite
  "Another mail program owns the write access to the mailbox.",

  // - DTME_RequestDenied
  "The other session denied the request for lock or copy.",

  // - DTME_TTFailure
  "A ToolTalk message could not be processed.",

  // - DTME_TransportFailed
  "The mail delivery transport failed.",

  // - DTME_UnknownFormat
  "The message is not in one of the supported formats.",  

  // - DTME_UnknownOpenError
  "An unknown error occurred when opening a mailbox.",

  // - DTME_UserAbort
  "The user aborted the operation.",

  // - DTME_UserInterrupted
  "The user interrupted the operation.",

  // - DTME_ObjectReadOnly
  "The mailbox permissions only allow read access.",

  // - DTME_NoPermission,
  "The user does not have access to the mailbox.",

  // - DTME_IsDirectory,
  "The specified path is a directory.",

  // - DTME_CannotRemoveStaleMailboxLockFile
  "Cannot lock mailbox (could not remove stale lock file).\nStale lock file: %s\nReason: %s",

  // - DTME_CannotCreateMailboxLockFile
  "Cannot lock mailbox (could not create lock file).\nMailbox lock file: %s\nReason: %s",

  // - DTME_CannotCreateMailboxLockFile_NoPermission
  "Cannot create lock file and lock mailbox because the user does not have\naccess to either the mailbox or the directory containing the mailbox.",

  // - DTME_CannotCreateMailboxLockFile_IsDirectory
  "Cannot create lock file and lock mailbox because the name for the\nmailbox lock file already exists and is a directory.",
    
  // - DTME_CannotCreateMailboxLockFile_NoSuchFile
  "Cannot create lock file and lock mailbox because a component of the\npath name of the lock file is not a directory.",
  
  // - DTME_CannotCreateMailboxLockFile_RemoteAccessLost
  "Cannot create lock file and lock mailbox because the remote system on\nwhich the lock file was to be created is no longer accessible.",
  
  // - DTME_CannotObtainInformationOnOpenMailboxFile
  "Cannot obtain information on current mailbox file.\nMailbox file: %s\nReason: %s",

  // - DTME_CannotCreateTemporaryMailboxFile
  "Cannot create temporary mailbox file.\nTemporary Mailbox file: %s\nReason: %s",

  // - DTME_CannotCreateTemporaryMailboxFile_NoPermission
  "Cannot create temporary mailbox file because the user does not have\naccess to either the mailbox or the directory containing the mailbox.",

  // - DTME_CannotCreateTemporaryMailboxFile_IsDirectory
  "Cannot create temporary mailbox file because the name for the\ntemporary mailbox already exists and is a directory.",
    
  // - DTME_CannotCreateTemporaryMailboxFile_NoSuchFile
  "Cannot create temporary mailbox file because a component of the\npath name of the temporary file is not a directory.",
  
  // - DTME_CannotCreateTemporaryMailboxFile_RemoteAccessLost
  "Cannot create temporary mailbox file because the remote system on\nwhich the file was to be created is no longer accessible.",
  
  // - DTME_CannotSetPermissionsOfTemporaryMailboxFile
  "Cannot set permissions on temporary mailbox file.\nTemporary Mailbox file: %s\nPermissions requested: %o\nReason: %s",

  // - DTME_CannotSetOwnerOfTemporaryMailboxFile
  "Cannot set owner of temporary mailbox file.\nTemporary Mailbox file: %s\nOwner uid requested: %d\nReason: %s",
  
  // - DTME_CannotSetGroupOfTemporaryMailboxFile
  "Cannot set group of temporary mailbox file.\nTemporary Mailbox file: %s\nGroup gid requested: %d\nReason: %s",

  // - DTME_CannotWriteToTemporaryMailboxFile
  "Cannot write to temporary mailbox file.\nTemporary Mailbox file: %s\nReason: %s",

  // - DTME_CannotWriteToTemporaryMailboxFile_ProcessLimitsExceeded
  "Cannot write to temporary mailbox file because the process's file\nsize limit or the maximum file size has been reached.",
  
  // - DTME_CannotWriteToTemporaryMailboxFile_RemoteAccessLost
  "Cannot write to temporary mailbox file because the remote system on\nwhich the file was created is no longer accessible.",
  
  // - DTME_CannotWriteToTemporaryMailboxFile_NoFreeSpaceLeft
  "Cannot write to temporary mailbox file because there is no free\nspace left on the device on which the file was created.",
  
  // - DTME_CannotReadNewMailboxFile
  "Cannot read new mailbox file\nReason: %s",

  // - DTME_CannotReadNewMailboxFile_OutOfMemory
  "Cannot read new mailbox file because no memory is available for the operation.",
  
  // - DTME_CannotRemoveMailboxLockFile
  "Cannot unlock mailbox (could not remove lock file).\nMailbox lock file: %s\nReason: %s",

  // - DTME_CannotRenameNewMailboxFileOverOld
  "Cannot rename new mailbox file over old mailbox file.\nOld mailbox file still exists but complete and correct contents\nof mailbox contents have been saved in the new mailbox file.\nThis problem must be corrected manually as soon as possible.\nOld Mailbox file: %s\nNew Mailbox file: %s\nReason: %s",
  
  // - DTME_InternalAssertionFailure
"An internal error has occurred within this application.\nThere is no way to recover and continue from this error.\nError condition: %s\n",

  // - DTME_ResourceParsingNoEndif
"An error occurred while parsing the .mailrc resource file.\nThere is a conditional if statement that does not have a corresponding endif statement.\n",
  
  // - DTME_AlreadyOpened,
   "This mail folder is already opened.",

  // - DTME_OutOfSpace,
   "No Space on Temporary Filesystem.",

  // - DTME_CannotCreateMailboxDotDtmailLockFile
  "Mailer has detected a mailbox lockfile:\n%s",

  // - DTME_MailboxInodeChanged
  "Mailer can no longer access this mailbox.\nIt would be best to close and reopen it.",

  // - DTME_MailServerAccess_AuthorizationFailed
  "Cannot retrieve mail for '%s@%s' using '%s'.\nAuthorization failed.",
  
  // - DTME_MailServerAccess_Error
  "Cannot retrieve mail for '%s@%s' using '%s'.\nThe server returned:\n     %s",
  
  // - DTME_MailServerAccess_MissingPassword
  "Please enter a password for '%s@%s' using '%s'",
  
  // - DTME_MailServerAccess_ProtocolViolation
  "Cannot retrieve mail for '%s@%s' using '%s'.\nClient/server protocol error.",
  
  // - DTME_MailServerAccess_ServerTimeoutError
  "Cannot retrieve mail for '%s@%s' using '%s'.\nTimed out waiting for server.",
  
  // - DTME_MailServerAccess_SocketIOError
  "Cannot retrieve mail for '%s@%s' using '%s'.\n%s.",
  
  // - DTME_AppendMailboxFile_Error
  "Cannot append to mailbox",
  
  // - DTME_AppendMailboxFile_FileTooBig
  "Cannot append to mailbox:  %s\nSYSERROR(%d):  %s.",
  
  // - DTME_AppendMailboxFile_LinkLost
  "Cannot append to mailbox:  %s\nSYSERROR(%d):  %s.",
  
  // - DTME_AppendMailboxFile_NoSpaceLeft
  "Cannot append to mailbox:  %s\nSYSERROR(%d):  %s.",
  
  // - DTME_AppendMailboxFile_SystemError
  "Cannot append to mailbox:  %s\nSYSERROR(%d):  %s.",

  // - DTME_GetmailCommandRetrieval_SystemError
  "Getmail command failed:  %s\nSYSERROR(%d):  %s.",

  // - DTME_GetmailCommandRetrieval_AbnormalExit
  "Getmail command exited abnormally:  %s.",

  // - DTME_PathElementPermissions
  "Search permission denied on a component of the path prefix,\n'%s'.",

  // - DTME_PathElementNotDirectory
  "A component of the path prefix is not a directory,\n'%s'.",

  // - DTME_PathElementDoesNotExist
  "A component of the path prefix does not exist,\n'%s'.",

  // - DTME_MailServerAccessInfo_SocketOpen
  "Opening connection for '%s@%s'.",
  
  // - DTME_MailServerAccessInfo_NoMessages
  "No messages for '%s@%s'.",
  
  // - DTME_MailServerAccessInfo_RetrievingMessage
  "Retrieving message %d of %d for '%s@%s'.",
  
  // - DTME_MailServerAccessInfo_MessageTooLarge
  "Skipping oversized message (%d bytes).",
  
  // - DTME_MAXDTME
  NULL
};

DtMailEnv::DtMailEnv()
{
  setCPP(NULL,NULL,NULL);
  _error = DTME_NoError;
  _message = NULL;
  _tt_message = NULL;
  _client = NULL;
  _implClearFunc = NULL;
  _fatal = DTM_FALSE;
}

#ifdef hpV4
#define GETMSG(catd, set, msg, dft) _DtCatgetsCached(catd, set, msg, dft)
#else
#define GETMSG(catd, set, msg, dft) catgets(catd, set, msg, dft)
#endif

char *
DtMailEnv::getMessageText(int set, int msg, char *dft)
{
    static int oneTimeFlag = 0;	// Only attempt to open message catalog once
    char *message;
    
    if ((oneTimeFlag == 0) && (_errorCatalog == (nl_catd) -1))
    {
	oneTimeFlag++;
	_errorCatalog = catopen((char*) DtMailCatalogDataFile, NL_CAT_LOCALE);
    }
    if (_errorCatalog != (nl_catd) -1)
      message = GETMSG(_errorCatalog, set, msg, dft);

    return message;
}

void
DtMailEnv::getErrorMessageText()
{
    if (_message == NULL)
    {
        if (_error_minor_code >= DTME_MAXDTME)
          _error_minor_code = DTME_InvalidError;

	_message = getMessageText(
			MailErrorSet, _error_minor_code + 1,
			(char*) error_strings[_error_minor_code]);
    }
    if (_message == NULL)
    {
	_error = DTME_NoMemory;
	_message = strdup(error_strings[_error_minor_code]);
    }
    else
      _message = strdup(_message);
}

void
DtMailEnv::clear()
{
  _error = DTME_NoError;

  if (_message != NULL) {
    free((char *)_message);
    _message = NULL;
  }

  if (_tt_message != NULL) {
    tt_message_destroy(_tt_message);
    _tt_message = NULL;
  }

  if (_client != NULL && _implClearFunc != NULL) {
    implClear();
  }
  _fatal = DTM_FALSE;
}

void
DtMailEnv::setError(const DTMailError_t minor_code, 
		    DtMailBoolean fatal,
		    Tt_message msg)
{
  clear();				// Clear out and free any old storage.

  _error_minor_code = minor_code;	// Remember original error number
  
  if (minor_code != DTME_NoError ) {
    _error = minor_code;
    if (minor_code >= DTME_MAXDTME) {
      _error = DTME_InvalidError;
    }
  }
  _tt_message = msg;	// Save the new one.

  _fatal = fatal;
  return;
}

// given system error number, include that string into the given message,
// which must have a %s embedded within it
//

// DtMailEnv::errnoMessage -- convert current system error number to readable text
// Description:
//  Take the current system error number (in the global errno), and
//  return a pointer to a readable string that describes the error.
// Arguments:
//  <none>
// Outputs:
//  <none>
// Returns:
//  const char * -- pointer to ascii text describing current system error
//			that is safe to refer to in an MT-hot environment
//
const char *
DtMailEnv::errnoMessage(void)
{
  return(errnoMessage(errno));
}

// DtMailEnv::errnoMessage -- convert system error number to readable text
// Description:
//  Given a valid system error number (as returned in the global errno),
//  return a pointer to a readable string that describes the error.
// Arguments:
//  systemErrorNumber - errno value
// Outputs:
//  <none>
// Returns:
//  const char * -- pointer to ascii text describing system error
//			that is safe to refer to in an MT-hot environment
//
const char *
DtMailEnv::errnoMessage(int systemErrorNumber)
{
  // Get the system error message for the given system error
  //
  const char *syserrstr = strerror(systemErrorNumber);
  if (!syserrstr)
    syserrstr = (const char *)"?";
  return(syserrstr);
}

// allow variable arguments at end and call vsprintf to process the
// error message string
//
void
DtMailEnv::vSetError(const DTMailError_t minor_code, 
		    DtMailBoolean fatal,
		    Tt_message msg,
		    ... )
{
  const int MessageBufferSize = 4096;
  char *messageBuffer = new char[MessageBufferSize+1];
  
  // allow error to come up with the final translated error message
  //
  setError(minor_code, fatal, msg);
  if (_message == NULL)
    getErrorMessageText();

  // Use the current error message as a format to vsprintf to
  // construct the final message
  //
  va_list	var_args;

  va_start(var_args, msg);
  (void) vsprintf(messageBuffer, _message, var_args);
  assert(strlen(messageBuffer) < MessageBufferSize);
  va_end(var_args);
  free((char *)_message);
  _message = (const char *)strdup(messageBuffer);
  delete [] messageBuffer;
}

#ifdef DEAD_WOOD
const char *
DtMailEnv::implGetMessage()
{
  if (_client != NULL && _implMessageFunc != NULL) {
	return((*_implMessageFunc)(_client));
  }
 return(NULL);
}

int
DtMailEnv::implGetError()
{
  if (_client != NULL && _implErrorFunc != NULL) {
	return((*_implErrorFunc)(_client));
  }
 return(0);
}
#endif /* DEAD_WOOD */

void
DtMailEnv::logError(DtMailBoolean criticalError, const char *format, ...) const
{
  const int MessageBufferSize = 8192;
  char *messageBuffer = new char[MessageBufferSize+1];
  
  if (!_syslog_open) {
    //
    // Open the log device and:
    // LOG_PIG - log the pid number of process.
    // LOG_CONS - log to the console, if the log device can not be opened.
    // LOG_NOWAIT - Do not wait for syslog to finish, avoids the use of SIGCHLD
    //
    openlog("libDtMail", LOG_PID | LOG_CONS | LOG_NOWAIT, LOG_MAIL);
  }

  va_list	var_args;

  va_start(var_args, format);
  (void) vsprintf(messageBuffer, format, var_args);
  assert(strlen(messageBuffer) < MessageBufferSize);
  va_end(var_args);

  syslog(criticalError == DTM_TRUE ? LOG_CRIT|LOG_ERR : LOG_ERR,
	 "%s", messageBuffer);
  
  _DtSimpleError("libDtMail", criticalError  == DTM_TRUE ? DtError : DtWarning,
		NULL, messageBuffer);
  delete [] messageBuffer;
}

#ifdef DEAD_WOOD
void
DtMailEnv::logFatalError(DtMailBoolean criticalError, const char *format, ...)
{
  _fatal = DTM_TRUE;

  va_list	var_args;

  va_start(var_args, format);
  logError(criticalError, format, var_args);
  va_end(var_args);
}
#endif /* DEAD_WOOD */
