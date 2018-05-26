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
 * License along with these libraries and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
/*
 *+SNOTICE
 *
 *
 *	$TOG: DtMailError.hh /main/15 1999/03/26 16:59:10 mgreess $
 *
 *	RESTRICTED CONFIDENTIAL INFORMATION:
 *	
 *	The information in this document is subject to special
 *	restrictions in a confidential disclosure agreement bertween
 *	HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
 *	document outside HP, IBM, Sun, USL, SCO, or Univel wihtout
 *	Sun's specific written approval.  This documment and all copies
 *	and derivative works thereof must be returned or destroyed at
 *	Sun's request.
 *
 *	Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 *
 *+ENOTICE
 */

#ifndef _DTMAILERROR_H
#define _DTMAILERROR_H

#include <DtMail/DtMailTypes.h>
#include <stdlib.h>
#include <stdarg.h>
#include <Tt/tt_c.h>
#include <nl_types.h>

//
// Name of the messaging database file.
//
extern	const char *	DtMailCatalogDataFile;
static const int	MailErrorSet = 1;
static const int	SockErrorSet = 2;
static const int	ValuesErrorSet = 2;
static const int	FileShareMsgSet = 2;

//
// NOTE - IMPORTANT -- READ ME
//
// The order of the definitions below must match the
// strings in libDtMail/Common/DtMailError.C
//
typedef enum {
  DTME_NoError,
  DTME_AlreadyLocked,
  DTME_BadArg,
  DTME_BadMailAddress,
  DTME_BadRunGroup,
  DTME_FileCreateFailed,
  DTME_FolderNotSet,
  DTME_GetLockRefused,
  DTME_ImplFailure,
  DTME_InitializationFailed,
  DTME_InternalFailure,
  DTME_InvalidError,
  DTME_InvalidOperationSequence,
  DTME_MD5ChecksumFailed,
  DTME_MailTransportFailed,
  DTME_NoDataType,
  DTME_NoImplementations,
  DTME_NoMailBox,
  DTME_NoMemory,
  DTME_NoMsgCat,
  DTME_NoNewMail,
  DTME_NoObjectValue,
  DTME_NoSuchFile,
  DTME_NoSuchImplementation,
  DTME_NoSuchType,
  DTME_NoUser,
  DTME_NotInbox,
  DTME_NotLocked,
  DTME_NotMailBox,
  DTME_NotSupported,
  DTME_ObjectAccessFailed,
  DTME_ObjectCreationFailed,
  DTME_ObjectInUse,
  DTME_ObjectInvalid,
  DTME_OpenContainerInterrupted,
  DTME_OperationInvalid,
  DTME_OtherOwnsWrite,
  DTME_RequestDenied,
  DTME_TTFailure,
  DTME_TransportFailed,
  DTME_UnknownFormat,
  DTME_UnknownOpenError,
  DTME_UserAbort,
  DTME_UserInterrupted,
  DTME_ObjectReadOnly,
  DTME_NoPermission,
  DTME_IsDirectory,
  DTME_CannotRemoveStaleMailboxLockFile,
  DTME_CannotCreateMailboxLockFile,
  DTME_CannotCreateMailboxLockFile_NoPermission,
  DTME_CannotCreateMailboxLockFile_IsDirectory,
  DTME_CannotCreateMailboxLockFile_NoSuchFile,
  DTME_CannotCreateMailboxLockFile_RemoteAccessLost,
  DTME_CannotObtainInformationOnOpenMailboxFile,
  DTME_CannotCreateTemporaryMailboxFile,
  DTME_CannotCreateTemporaryMailboxFile_NoPermission,
  DTME_CannotCreateTemporaryMailboxFile_IsDirectory,
  DTME_CannotCreateTemporaryMailboxFile_NoSuchFile,
  DTME_CannotCreateTemporaryMailboxFile_RemoteAccessLost,
  DTME_CannotSetPermissionsOfTemporaryMailboxFile,
  DTME_CannotSetOwnerOfTemporaryMailboxFile,
  DTME_CannotSetGroupOfTemporaryMailboxFile,
  DTME_CannotWriteToTemporaryMailboxFile,
  DTME_CannotWriteToTemporaryMailboxFile_ProcessLimitsExceeded,
  DTME_CannotWriteToTemporaryMailboxFile_RemoteAccessLost,
  DTME_CannotWriteToTemporaryMailboxFile_NoFreeSpaceLeft,
  DTME_CannotReadNewMailboxFile,
  DTME_CannotReadNewMailboxFile_OutOfMemory,
  DTME_CannotRemoveMailboxLockFile,
  DTME_CannotRenameNewMailboxFileOverOld,
  DTME_InternalAssertionFailure,
  DTME_ResourceParsingNoEndif,
  DTME_AlreadyOpened,
  DTME_OutOfSpace,
  DTME_CannotCreateMailboxDotDtmailLockFile,
  DTME_MailboxInodeChanged,
  DTME_MailServerAccess_AuthorizationFailed,
  DTME_MailServerAccess_Error,
  DTME_MailServerAccess_MissingPassword,
  DTME_MailServerAccess_ProtocolViolation,
  DTME_MailServerAccess_ServerTimeoutError,
  DTME_MailServerAccess_SocketIOError,
  DTME_AppendMailboxFile_Error,
  DTME_AppendMailboxFile_FileTooBig,
  DTME_AppendMailboxFile_LinkLost,
  DTME_AppendMailboxFile_NoSpaceLeft,
  DTME_AppendMailboxFile_SystemError,
  DTME_GetmailCommandRetrieval_SystemError,
  DTME_GetmailCommandRetrieval_AbnormalExit,
  DTME_PathElementPermissions,
  DTME_PathElementNotDirectory,
  DTME_PathElementDoesNotExist,
  DTME_MailServerAccessInfo_SocketOpen,
  DTME_MailServerAccessInfo_NoMessages,
  DTME_MailServerAccessInfo_RetrievingMessage,
  DTME_MailServerAccessInfo_MessageTooLarge,
  DTME_MAXDTME
} DTMailError_t;

//
// Make a class wrapper for errors so that we can set/clear
// errors easier.
//
class DtMailEnv {
public:

  void  clear();        // Sets everything to no error, and frees memory.

  //
  // logError() and logFatalError():
  //
  // This will print out in a formatted way all of the error information
  // that it can. it also sends the results to the system log device for
  // bug tracking.
  //
  // First it will format the error message to be returned.
  // Then if the _client message is set it will be printed.
  //
  // %m is used to insert the system supplied message into the supplied
  // format (see syslog(3)):
  //
  //	error.logError(DTM_FALSE, "I got an error: number %d\n %m , error_code);
  //
  // The format may be in a system specific way.
  //
  void		logError(DtMailBoolean criticalError,
			 const char *format, ...) const;

#ifdef DEAD_WOOD
  //
  // This one is the same as logError() plus it sets _fatal.
  //
  void		logFatalError(DtMailBoolean criticalError,
			      const char *format, ...);
#endif /* DEAD_WOOD */
  
  //
  // Set the error code. Tt_message is optional.
  //
  // Fatal here is within the scope of the function called,
  // not necessarily the program. If the program can recover - it should.
  //
  void		setError(const DTMailError_t,
			 DtMailBoolean fatal,
			 Tt_message = NULL);

  void		vSetError(const DTMailError_t minor_code,
			 DtMailBoolean fatal,
			 Tt_message msg,
			 ... );
  
  void		setError(const DTMailError_t,
			 const Tt_message = NULL);

  const char *	errnoMessage(int systemErrorNumber);
  const char *	errnoMessage();
  void		setClient(void *);
  void *	getClient(void) { return _client; }
  DtMailBoolean	isSet() const;	// Returns DTM_TRUE if there was an error.
  DtMailBoolean	isNotSet() const;	// !IsSet()
  DtMailBoolean	isFatal() const;// Returns DTM_TRUE if the error was fatal.
  DtMailBoolean	isNotFatal() const;	// !IsFatal()
  
  //
  // Cast operators.			While:
  //
  //	DtMailEnv	 	 error;
  //				
  //	// You can say....
  //
  //	const Tt_message msg	= (const Tt_message)error;
  //
  //	const CMEnv env		= (const CMEnv*)error;
  //
  //	printf("%s %d", (const char*)error, (DTMailError_t)error);
  //
  operator const char*();
  operator const Tt_message() const;
  operator DTMailError_t() const;

  //
  // Constructor/Destructor
  //
  DtMailEnv();            // The constructor will set everything to no error.

  ~DtMailEnv();		// Sets everything to no error, and frees memory.
 
  //
  // Message catalog.
  //
  static nl_catd	_errorCatalog;
  static char		*getMessageText(int set, int msg, char *dft);

protected:

  //
  // The implementation specific function pointer and the _client
  // variable below was designed for use C++ API.
  //
  //
  // NOTES:
  //	1)	CPP == C++.
  // 	2)	THESE FUNCTON POINTERS >>>EXPECT<<< TO CALL MT-SAFE FUNCTIONS.
  //
  // Implmentation specific error messages are here.
  // The pointer to the client data is in _client.
  //
  // Pointer to function for C++ 'clear', 'message', and
  // 'code' functions provided to this class from the implementation.
  // They are set using setCPP(...) function.
  //
  // This is for functions that use the C++ calling conventions.
  //
  // The CPPclearF points to a function provided by
  // the implementation to clear the error and free any
  // data assoicated with the _client specific data.
  //
  // The CPPmessageF points to a function provided by
  // the implementation to return the error 'message' assoicated with
  // the _client specific data.
  // --THE PROVIDED FUNCTION MUST RETURN NULL WHEN THERE IS NO ERROR.
  //
  // The CPPerrorF points to a function provided by
  // the implementation to return the error 'code' assoicated with
  // the _client specific data.
  // --THE PROVIDED FUNCTION MUST RETURN ZERO (0) WHEN THERE IS NO ERROR.
  //
  // void setClient(void *), sets the value of _client.
  //
  // C
  typedef void		(*CPPclearF)(void *);
  typedef const char  *	(*CPPmessageF)(void *);
  typedef int		(*CPPerrorF)(void *);

  void			setCPP(CPPclearF, CPPmessageF, CPPerrorF);

  void			implClear();
#ifdef DEAD_WOOD
  const char *		implGetMessage();
  int			implGetError();
#endif /* DEAD_WOOD */
  
private:
  DTMailError_t		_error;		// The corrected error number.
  DTMailError_t		_error_minor_code;	// The original error number
  const char		*_message;	// The error message.
  Tt_message		_tt_message;	// ToolTalk error message.
  DtMailBoolean		_fatal;		// DTM_TRUE if this is a fatal error.
  void			getErrorMessageText(void);
  
  //
  // Function pointers for implementation specific clear,
  // get message, and get error functions.
  //
  CPPclearF	_implClearFunc;   // C function that will clear the error.
  CPPmessageF	_implMessageFunc; // C function that will get the error msg.
  CPPerrorF	_implErrorFunc;   // C function that will get the error code.

  //
  // Where the implementation stores the data.
  //
  void		*_client;	// Implmentation specific message.

  //
  // Has syslog been opened?
  //
  static DtMailBoolean	_syslog_open;

  //
  // Message catalog.
  //
  static const char *	DtMailCatalogDataFile;
};

// Inline member functions.

inline  void
DtMailEnv::setCPP(CPPclearF clear, CPPmessageF message, CPPerrorF code)
{
  _implClearFunc = clear;
  _implMessageFunc = message;
  _implErrorFunc = code;
}

inline
DtMailEnv::~DtMailEnv()
{
  clear();
  if (_client)
      free(_client);
}

inline void
DtMailEnv::setError(const DTMailError_t err, const Tt_message msg)
{
  setError(err, DTM_FALSE, msg);
}

inline DtMailBoolean
DtMailEnv::isSet() const
{
  return(_error == DTME_NoError ? DTM_FALSE : DTM_TRUE);
}

inline DtMailBoolean
DtMailEnv::isNotSet() const
{
  return(_error == DTME_NoError ? DTM_TRUE : DTM_FALSE);
}

inline DtMailBoolean
DtMailEnv::isFatal() const
{
  return(_fatal);
}

inline DtMailBoolean
DtMailEnv::isNotFatal() const
{
  return(_fatal == DTM_TRUE ? DTM_FALSE : DTM_TRUE);
}

inline	void
DtMailEnv::setClient(void *client)
{
  _client = client;
}

inline
DtMailEnv::operator const char*()
{
    if (_message == NULL)
	getErrorMessageText();
    return(_message == NULL ? "Unknown" : _message);
}

inline
DtMailEnv::operator const Tt_message() const
{
  return(_tt_message);
}

inline
DtMailEnv::operator DTMailError_t() const
{
  return(_error);
}

inline void
DtMailEnv::implClear()
{
  if (_client != NULL && _implClearFunc != NULL) {
	(*_implClearFunc)(_client);
  }
}

#ifdef DEAD_WOOD
inline Tt_message
DtMailError_getTTmsg(DtMailEnv & error)
{
  return((const Tt_message)error);	// The prefered way.
}
#endif /* DEAD_WOOD */

#endif	// _DTCM_HH
 
