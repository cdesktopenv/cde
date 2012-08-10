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
//%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			
//%%  (c) Copyright 1993, 1994 International Business Machines Corp.	
//%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			
//%%  (c) Copyright 1993, 1994 Novell, Inc. 				
//%%  $TOG: api_api.C /main/9 1999/09/16 13:45:53 mgreess $ 			 				
/* @(#)api_api.C	1.24 93/09/07
 *
 * api_api.cc
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 *
 *
 * This file contains the implementation of functions that are "pure api":
 * that is, functions which exist only to make the api interface easier to
 * use, not to actually perform any ToolTalk function.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>     
#include <memory.h>
#include <errno.h>
#include "mp/mp_c.h"
#include "api/c/tt_c.h"
#include "api/c/api_api.h"
#include "api/c/api_mp.h"
#include "api/c/api_storage.h"
#include "api/c/api_xdr.h"
#include "api/c/api_error.h"
#include "util/tt_audit.h"
#include "util/tt_global_env.h"
#include "util/tt_string.h"
#include "util/tt_gettext.h"
#include "util/tt_xdr_utils.h"
#include "util/tt_threadspecific.h"
#include "tt_options.h"

static _Tt_api_stg_stack* _get_stgstack(void);

// Unfortunately due to sloppy coding, libtt.so.1 exported
// the NetISAM "iserrno" variable, and some clients that used both
// ToolTalk and NetISAM ended up getting the external reference
// resolved from libtt.so.1.  Thus not exposing this symbol
// breaks binary compatibility (in particular docviewer breaks.)

#if defined(OPT_BUG_SUNOS_5)
extern int iserrno = 0;
#endif


// Error pointer values are actually pointers into this array.  The 
// pointers are never dereferenced, so we really just burn a page
// of address space.  (it would be nice if we could put this off on
// a page by itself, but I don't know how to do that.)
char _tt_api_status_page[(int)TT_STATUS_LAST];

// Implementation of ToolTalk C API routines called directly from
// integrated applications.


caddr_t 
tt_malloc(size_t s)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("i", TT_MALLOC, (int) s);
	caddr_t retval;

        if (status != TT_OK) {
		audit.exit((void  *) _tt_error_pointer(status));
                return (caddr_t) _tt_error_pointer(status);
        }

	retval =  _tt_malloc(s);
	audit.exit((void *) retval);
	return retval;
}


void 
tt_free(caddr_t p)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("A", TT_FREE, p);

        if (status != TT_OK) {
		audit.exit();
                return ;
        }

	_tt_free(p);
	audit.exit();
	return;
}


int 
tt_mark(void)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("v", TT_MARK, 0);
	int retval;

        if (status != TT_OK) {
		audit.exit(_tt_error_int(status));
                return _tt_error_int(status);
        }


	retval = _tt_mark();
	audit.exit(retval);
	return retval;
}


void 
tt_release(int mark)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("i", TT_RELEASE, mark);

        if (status != TT_OK) {
		audit.exit();
                return;
        }

	_tt_release(mark);
	audit.exit();
	return;
}


char           *
tt_status_message(Tt_status ttrc)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("t", TT_STATUS_MESSAGE, ttrc);
	char *result;

        if (status != TT_OK) {
		audit.exit((char *)error_pointer(status));
                return (char *)error_pointer(status);
        }

        result =  _tt_status_message(ttrc);
        audit.exit(result);

	return result;
}


Tt_status
tt_pointer_error(void *pointer)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("A", TT_POINTER_ERROR, pointer);

        if (status != TT_OK) {
		audit.exit(status);
                return status;
        }

        status = _tt_pointer_error(pointer);
        audit.exit(status);

	return status;
}


Tt_status
tt_int_error(int return_val)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("i", TT_INT_ERROR, return_val);

        if (status != TT_OK) {
		audit.exit(status);
                return status;
        }

        status =  _tt_int_error(return_val);
        audit.exit(status);

	return status;
}


void *
tt_error_pointer(Tt_status s)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("t", TT_ERROR_POINTER, s);
        void *result;

        if (status != TT_OK) {
		audit.exit((void *)error_pointer(status));
                return (void *)error_pointer(status);
        }

        result = _tt_error_pointer(s);
        audit.exit(result);

        return result;
}


int
tt_error_int(Tt_status s)
{
	_Tt_audit audit;
        Tt_status status = audit.entry("t", TT_ERROR_INT, s);
        int result;

        if (status != TT_OK) {
		audit.exit(error_int(status));
                return error_int(status);
        }

        result = _tt_error_int(s);
        audit.exit(result);

        return result;
}


int
_tt_mark(void)
{
	return _get_stgstack()->mark();
}

void
_tt_release(int mark)
{
	_get_stgstack()->release(mark);
	return;
}

caddr_t
_tt_malloc(size_t s)
{
	return _get_stgstack()->malloc(s);	
}

char *
_tt_strdup(const _Tt_string &s)
{
	return _tt_strdup( (char *)s );
}

char *
_tt_strdup(const char * s)
{
	size_t l;
	if (0==s) return 0;

	l = strlen(s)+1;

	char *result = _get_stgstack()->malloc(l);
	strcpy(result,s);
	return result;
}

char *
_tt_strdup(const char * s, int len)
{
	if ((char *)0==s) return (char *)0;

	char *result = _get_stgstack()->malloc(len+1);
	memcpy(result, s, len);
	result[len] = '\0';
	return result;
}

void
_tt_free(caddr_t p)
{
	_get_stgstack()->free(p);
	return;
}

caddr_t
_tt_take(caddr_t p)
{
	return _get_stgstack()->take(p);
}

char	*
_tt_status_message(Tt_status ttrc)
{
	const char *result;
	switch (ttrc) {
	      case TT_OK:
		result = catgets( _ttcatd, 11, 2,
				  "TT_OK\tRequest successful.");
		break;
	      case TT_WRN_NOTFOUND:
		result = catgets( _ttcatd, 11, 3,
				  "TT_WRN_NOTFOUND\tThe object was not removed because it was not found.");
		break;
	      case TT_WRN_STALE_OBJID:
		result = catgets( _ttcatd, 11, 4,
				  "TT_WRN_STALE_OBJID\tThe object attribute in the message has been replaced with a newer one. Update the place from which the object id was obtained.");
		break;
	      case TT_WRN_STOPPED:
		result = catgets( _ttcatd, 11, 5,
				  "TT_WRN_STOPPED\tThe query was halted by the filter procedure.");
		break;
	      case TT_WRN_SAME_OBJID:
		result = catgets( _ttcatd, 11, 6,
				  "TT_WRN_SAME_OBJID\tThe moved object retains the same objid.");
		break;
	      case TT_WRN_START_MESSAGE:
		result = catgets( _ttcatd, 11, 7,
				  "TT_WRN_START_MESSAGE\tThis message caused this process to be started.  This message should be replied to even if it is a notice.");
		break;
	      case TT_WRN_NOT_ENABLED:
		result = catgets( _ttcatd, 11, 7,
				  "TT_WRN_NOT_ENABLED\tThe ToolTalk feature has not been enabled yet in this process.");
		break;
	      case TT_WRN_APPFIRST:
		result = catgets( _ttcatd, 11, 8,
				  "TT_WRN_APPFIRST\tThis code should be unused.");
		break;
	      case TT_WRN_LAST:
		result = catgets( _ttcatd, 11, 9,
				  "TT_WRN_LAST\tThis code should be unused.");
		break;
	      case TT_ERR_CLASS:
		result = catgets( _ttcatd, 11, 10,
				  "TT_ERR_CLASS\tThe Tt_class value passed is invalid.");
		break;
	      case TT_ERR_DBAVAIL:
		result = catgets( _ttcatd, 11, 11,
				  "TT_ERR_DBAVAIL\tA required database is not available. The condition may be temporary, trying again later may work.");
		break;
	      case TT_ERR_DBEXIST:
		result = catgets( _ttcatd, 11, 12,
				  "TT_ERR_DBEXIST\tA required database does not exist. The database must be created before this action will work.");
		break;
	      case TT_ERR_FILE:
		result = catgets( _ttcatd, 11, 13,
				  "TT_ERR_FILE\tFile object could not be found.");
		break;
	      case TT_ERR_MODE:
		result = catgets( _ttcatd, 11, 14,
				  "TT_ERR_MODE\tThe Tt_mode value is not valid.");
		break;
	      case TT_ERR_ACCESS:
		result = catgets( _ttcatd, 11, 15,
				  "TT_ERR_ACCESS\tAn attempt was made to access a ToolTalk object in a way forbidden by the protection system.");
		break;
	      case TT_ERR_NOMP:
		result = catgets( _ttcatd, 11, 16,
				  "TT_ERR_NOMP\tNo ttsession process is running, probably because tt_open() has not been called yet. If this code is returned from tt_open() it means ttsession could not be started, which generally means ToolTalk is not installed on this system.");
		break;
	      case TT_ERR_NOTHANDLER:
		result = catgets( _ttcatd, 11, 17,
				  "TT_ERR_NOTHANDLER\tOnly the handler of the message can do this.");
		break;
	      case TT_ERR_NUM:
		result = catgets( _ttcatd, 11, 18,
				  "TT_ERR_NUM\tThe integer value passed is not valid.");
		break;
	      case TT_ERR_OBJID:
		result = catgets( _ttcatd, 11, 19,
				  "TT_ERR_OBJID\tThe object id passed does not refer to any existing object spec.");
		break;
	      case TT_ERR_OP:
		result = catgets( _ttcatd, 11, 20,
				  "TT_ERR_OP\tThe operation name passed is not syntactically valid.");
		break;
	      case TT_ERR_OTYPE:
		result = catgets( _ttcatd, 11, 21,
				  "TT_ERR_OTYPE\tThe object type passed is not the name of an installed object type.");
		break;
	      case TT_ERR_ADDRESS:
		result = catgets( _ttcatd, 11, 22,
				  "TT_ERR_ADDRESS\tThe Tt_address value passed is not valid.");
		break;
	      case TT_ERR_PATH:
		result = catgets( _ttcatd, 11, 23,
				  "TT_ERR_PATH\tOne of the directories in the file path passed does not exist or cannot be read.");
		break;
	      case TT_ERR_POINTER:
		result = catgets( _ttcatd, 11, 24,
				  "TT_ERR_POINTER\tThe opaque pointer (handle) passed does not indicate an object of the proper type.");
		break;
	      case TT_ERR_PROCID:
		result = catgets( _ttcatd, 11, 25,
				  "TT_ERR_PROCID\tThe process id passed is not valid.");
		break;
	      case TT_ERR_PROPLEN:
		result = catgets( _ttcatd, 11, 26,
				  "TT_ERR_PROPLEN\tThe property value passed is too long.");
		break;
	      case TT_ERR_PROPNAME:
		result = catgets( _ttcatd, 11, 27,
				  "TT_ERR_PROPNAME\tThe property name passed is syntactically invalid.");
		break;
	      case TT_ERR_PTYPE:
		result = catgets( _ttcatd, 11, 28,
				  "TT_ERR_PTYPE\tThe process type passed is not the name of an installed process type.");
		break;
	      case TT_ERR_DISPOSITION:
		result = catgets( _ttcatd, 11, 29,
				  "TT_ERR_DISPOSITION\tThe Tt_disposition value passed is not valid.");
		break;
	      case TT_ERR_SCOPE:
		result = catgets( _ttcatd, 11, 30,
				  "TT_ERR_SCOPE\tThe Tt_scope value passed is not valid.");
		break;
	      case TT_ERR_SESSION:
		result = catgets( _ttcatd, 11, 31,
				  "TT_ERR_SESSION\tThe session id passed is not the name of an active session.");
		break;
	      case TT_ERR_VTYPE:
		result = catgets( _ttcatd, 11, 32,
				  "TT_ERR_VTYPE\tThe value type name passed is not valid.");
		break;
	      case TT_ERR_NO_VALUE:
		result = catgets( _ttcatd, 11, 33,
				  "TT_ERR_NO_VALUE\tNo property value with the given name and number exists.");
		break;
	      case TT_ERR_INTERNAL:
		result = catgets( _ttcatd, 11, 34,
				  "TT_ERR_INTERNAL\tInternal error (bug)");
		break;
	      case TT_ERR_READONLY:
		result = catgets( _ttcatd, 11, 35,
				  "TT_ERR_READONLY\tThe attribute cannot be changed.");
		break;
	      case TT_ERR_NO_MATCH:
		result = catgets( _ttcatd, 11, 36,
				  "TT_ERR_NO_MATCH\tNo handler could be found for this message, and the disposition was not queue or start.");
		break;
	      case TT_ERR_UNIMP:
		result = catgets( _ttcatd, 11, 37,
				  "TT_ERR_UNIMP\tFunction not implemented.");
		break;
	      case TT_ERR_OVERFLOW:
		result = catgets( _ttcatd, 11, 38,
				  "TT_ERR_OVERFLOW\tToo many active messages (try again later).");
		break;
	      case TT_ERR_PTYPE_START:
		result = catgets( _ttcatd, 11, 39,
				  "TT_ERR_PTYPE_START\tAttempt to launch instance of ptype failed.");
		break;
	      case TT_ERR_CATEGORY:
		result = catgets( _ttcatd, 11, 40,
				  "TT_ERR_CATEGORY\tPattern object has no category set.");
		break;

	      case TT_ERR_DBUPDATE:
		result = catgets( _ttcatd, 11, 41,
				  "TT_ERR_DBUPDATE\tThe database is inconsistent: another tt_spec_write updated object first.");
		break;

	      case TT_ERR_DBFULL:
		result = catgets( _ttcatd, 11, 42,
				  "TT_ERR_DBFULL\tTooltalk database is full.");
		break;

	      case TT_ERR_DBCONSIST:
		result = catgets( _ttcatd, 11, 43,
				  "TT_ERR_DBCONSIST\tDatabase is corrupt or access information is incomplete (run ttdbck).");
		break;

	      case TT_ERR_STATE:
		result = catgets( _ttcatd, 11, 44,
				  "TT_ERR_STATE\tThe Tt_message is in a Tt_state that is invalid for the attempted operation.");
		break;

	      case TT_ERR_NOMEM:
		result = catgets( _ttcatd, 11, 45,
				  "TT_ERR_NOMEM\tNo more memory.");
		break;

	      case TT_ERR_SLOTNAME:
		result = catgets( _ttcatd, 11, 46,
				  "TT_ERR_SLOTNAME\tThe slot name is syntactically invalid.");
		break;

	      case TT_ERR_XDR:
		result = catgets( _ttcatd, 11, 47,
				  "TT_ERR_XDR\tThe XDR proc. passed evaluated to zero len, or failed during invocation.");
		break;

	      case TT_ERR_NETFILE:
		result = catgets( _ttcatd, 11, 48,
				  "TT_ERR_NETFILE\tThe netfilename passed is not syntactically valid.");
		break;

	      case TT_ERR_TOOLATE:
		result = catgets( _ttcatd, 11, 86,
				  "TT_ERR_TOOLATE\tThis must be the first call made into the ToolTalk API and can therefore no longer be performed.");
		break;

	      case TT_ERR_AUTHORIZATION:
		result = catgets( _ttcatd, 11, 87,
				  "TT_ERR_AUTHORIZATION\tThe user is not authorized to connect to this ToolTalk session.");
		break;

	      case TT_ERR_VERSION_MISMATCH:
		result = catgets( _ttcatd, 11, 95,
				  "TT_ERR_VERSION_MISMATCH\tThis client is using a different RPC or program version from the server of this ToolTalk session.");
		break;

	      case TT_DESKTOP_UNMODIFIED:
		result = catgets( _ttcatd, 11, 49,
				  "TT_DESKTOP_UNMODIFIED\tOperation does not apply to unmodified entities");
		break;
	      case TT_MEDIA_ERR_SIZE:
		result = catgets( _ttcatd, 11, 50,
				  "TT_MEDIA_ERR_SIZE\tThe specified size was too big or too small");
		break;
	      case TT_MEDIA_ERR_FORMAT:
		result = catgets( _ttcatd, 11, 51,
				  "TT_MEDIA_ERR_FORMAT\tThe data do not conform to their alleged format");
		break;
	      case TT_ERR_APPFIRST:
		result = catgets( _ttcatd, 11, 52,
				  "TT_ERR_APPFIRST\tThis code should be unused.");
		break;
	      case TT_ERR_LAST:
		result = catgets( _ttcatd, 11, 53,
				  "TT_ERR_LAST\tThis code should be unused.");
		break;
	      case TT_STATUS_LAST:
		result = catgets( _ttcatd, 11, 54,
				  "TT_STATUS_LAST\tThis code should be unused.");
		break;

	      case TT_DESKTOP_EPERM:
		result = catgets( _ttcatd, 11, 56,
				  "TT_DESKTOP_EPERM\tNot super-user");
		break;
	      case TT_DESKTOP_ENOENT:
		result = catgets( _ttcatd, 11, 57,
				  "TT_DESKTOP_ENOENT\tNo such file or directory");
		break;
	      case TT_DESKTOP_EINTR:
		result = catgets( _ttcatd, 11, 58,
				  "TT_DESKTOP_EINTR\tInterrupted system call");
		break;
	      case TT_DESKTOP_EIO:
		result = catgets( _ttcatd, 11, 59,
				  "TT_DESKTOP_EIO\tI/O error");
		break;
	      case TT_DESKTOP_EAGAIN:
		result = catgets( _ttcatd, 11, 60,
				  "TT_DESKTOP_EAGAIN\tNo more processes");
		break;
	      case TT_DESKTOP_ENOMEM:
		result = catgets( _ttcatd, 11, 61,
				  "TT_DESKTOP_ENOMEM\tNot enough space");
		break;
	      case TT_DESKTOP_EACCES:
		result = catgets( _ttcatd, 11, 62,
				  "TT_DESKTOP_EACCES\tPermission denied");
		break;
	      case TT_DESKTOP_EFAULT:
		result = catgets( _ttcatd, 11, 63,
				  "TT_DESKTOP_EFAULT\tBad address");
		break;
	      case TT_DESKTOP_EEXIST:
		result = catgets( _ttcatd, 11, 64,
				  "TT_DESKTOP_EEXIST\tFile exists");
		break;
	      case TT_DESKTOP_ENODEV:
		result = catgets( _ttcatd, 11, 65,
				  "TT_DESKTOP_ENODEV\tNo such device");
		break;
	      case TT_DESKTOP_ENOTDIR:
		result = catgets( _ttcatd, 11, 66,
				  "TT_DESKTOP_ENOTDIR\tNot a directory");
		break;
	      case TT_DESKTOP_EISDIR:
		result = catgets( _ttcatd, 11, 67,
				  "TT_DESKTOP_EISDIR\tIs a directory");
		break;
	      case TT_DESKTOP_EINVAL:
		result = catgets( _ttcatd, 11, 68,
				  "TT_DESKTOP_EINVAL\tInvalid argument");
		break;
	      case TT_DESKTOP_ENFILE:
		result = catgets( _ttcatd, 11, 69,
				  "TT_DESKTOP_ENFILE\tFile table overflow");
		break;
	      case TT_DESKTOP_EMFILE:
		result = catgets( _ttcatd, 11, 70,
				  "TT_DESKTOP_EMFILE\tToo many open files");
		break;
	      case TT_DESKTOP_ETXTBSY:
		result = catgets( _ttcatd, 11, 71,
				  "TT_DESKTOP_ETXTBSY\tText file busy");
		break;
	      case TT_DESKTOP_EFBIG:
		result = catgets( _ttcatd, 11, 72,
				  "TT_DESKTOP_EFBIG\tFile too large");
		break;
	      case TT_DESKTOP_ENOSPC:
		result = catgets( _ttcatd, 11, 73,
				  "TT_DESKTOP_ENOSPC\tNo space left on device");
		break;
	      case TT_DESKTOP_EROFS:
		result = catgets( _ttcatd, 11, 74,
				  "TT_DESKTOP_EROFS\tRead-only file system");
		break;
	      case TT_DESKTOP_EMLINK:
		result = catgets( _ttcatd, 11, 75,
				  "TT_DESKTOP_EMLINK\tToo many links");
		break;
	      case TT_DESKTOP_EPIPE:
		result = catgets( _ttcatd, 11, 76,
				  "TT_DESKTOP_EPIPE\tBroken pipe");
		break;
	      case TT_DESKTOP_ENOMSG:
		result = catgets( _ttcatd, 11, 77,
				  "TT_DESKTOP_ENOMSG\tNo message of desired type");
		break;
	      case TT_DESKTOP_EDEADLK:
		result = catgets( _ttcatd, 11, 78,
				  "TT_DESKTOP_EDEADLK\tDeadlock condition");
		break;
	      case TT_DESKTOP_ENODATA:
		result = catgets( _ttcatd, 11, 79,
				  "TT_DESKTOP_ENODATA\tNo data available");
		break;
	      case TT_DESKTOP_EPROTO:
		result = catgets( _ttcatd, 11, 80,
				  "TT_DESKTOP_EPROTO\tProtocol error");
		break;
	      case TT_DESKTOP_ENOTEMPTY:
		result = catgets( _ttcatd, 11, 81,
				  "TT_DESKTOP_ENOTEMPTY\tDirectory not empty");
		break;
	      case TT_DESKTOP_ETIMEDOUT:
		result = catgets( _ttcatd, 11, 82,
				  "TT_DESKTOP_ETIMEDOUT\tConnection timed out");
		break;
	      case TT_DESKTOP_EALREADY:
		result = catgets( _ttcatd, 11, 83,
				  "TT_DESKTOP_EALREADY\tOperation already in progress");
		break;
	      case TT_DESKTOP_ECANCELED:
		result = catgets( _ttcatd, 11, 84,
				  "TT_DESKTOP_ECANCELED\tOperation canceled");
		break;
	      case TT_DESKTOP_ENOTSUP:
		result = catgets( _ttcatd, 11, 85,
				  "TT_DESKTOP_ENOTSUP\tNot supported");

		break;

	      case TT_AUTHFILE_ACCESS:
		result = catgets( _ttcatd, 11, 88,
				  "TT_AUTHFILE_ACCESS\tThe user is not authorized to access the ToolTalk authority file.");
		break;
	      case TT_AUTHFILE_LOCK:
		result = catgets( _ttcatd, 11, 89,
				  "TT_AUTHFILE_LOCK\tCannot lock the ToolTalk authority file.");
		break;
	      case TT_AUTHFILE_LOCK_TIMEOUT:
		result = catgets( _ttcatd, 11, 90,
				  "TT_AUTHFILE_LOCK_TIMEOUT\tCannot lock the ToolTalk authority file.");
		break;
	      case TT_AUTHFILE_UNLOCK:
		result = catgets( _ttcatd, 11, 91,
				  "TT_AUTHFILE_UNLOCK\tCannot unlock the ToolTalk authority file.");
		break;
	      case TT_AUTHFILE_MISSING:
		result = catgets( _ttcatd, 11, 92,
				  "TT_AUTHFILE_MISSING\tThe ToolTalk authority file is missing.");
		break;
	      case TT_AUTHFILE_ENTRY_MISSING:
		result = catgets( _ttcatd, 11, 93,
				  "TT_AUTHFILE_ENTRY_MISSING\tThe ToolTalk authority file is missing an entry for this session.");
		break;
	      case TT_AUTHFILE_WRITE:
		result = catgets( _ttcatd, 11, 94,
				  "TT_AUTHFILE_WRITE\tCannot write the ToolTalk authority file.");
		break;

	      default:
		result = catgets( _ttcatd, 11, 55,
				  "Unknown Tt_status value.");
		break;
	}
	
	return _tt_strdup(result);
}


Tt_status
_tt_errno_status(
	int err_no
)
{
	switch (err_no) {
	    case EPERM:		return TT_DESKTOP_EPERM;
	    case ENOENT:	return TT_DESKTOP_ENOENT;
	    case EINTR:		return TT_DESKTOP_EINTR;
	    case EIO:		return TT_DESKTOP_EIO;
	    case EAGAIN:	return TT_DESKTOP_EAGAIN;
	    case ENOMEM:	return TT_DESKTOP_ENOMEM;
	    case EACCES:	return TT_DESKTOP_EACCES;
	    case EFAULT:	return TT_DESKTOP_EFAULT;
	    case EEXIST:	return TT_DESKTOP_EEXIST;
	    case ENODEV:	return TT_DESKTOP_ENODEV;
	    case ENOTDIR:	return TT_DESKTOP_ENOTDIR;
	    case EISDIR:	return TT_DESKTOP_EISDIR;
	    case EINVAL:	return TT_DESKTOP_EINVAL;
	    case ENFILE:	return TT_DESKTOP_ENFILE;
	    case EMFILE:	return TT_DESKTOP_EMFILE;
	    case ETXTBSY:	return TT_DESKTOP_ETXTBSY;
	    case EFBIG:		return TT_DESKTOP_EFBIG;
	    case ENOSPC:	return TT_DESKTOP_ENOSPC;
	    case EROFS:		return TT_DESKTOP_EROFS;
	    case EMLINK:	return TT_DESKTOP_EMLINK;
	    case EPIPE:		return TT_DESKTOP_EPIPE;
	    case ENOMSG:	return TT_DESKTOP_ENOMSG;
	    case EDEADLK:	return TT_DESKTOP_EDEADLK;
#ifdef ENODATA
	    case ENODATA:	return TT_DESKTOP_ENODATA;
#endif
#ifdef EPROTO
	    case EPROTO:	return TT_DESKTOP_EPROTO;
#endif
#if !defined(OPT_BUG_AIX)
// on AIX, ENOTEMPTY is the same code as EEXIST, and the switch
// statement fails to compile with duplicate case labels..
	    case ENOTEMPTY:	return TT_DESKTOP_ENOTEMPTY;
#endif
	    case ETIMEDOUT:	return TT_DESKTOP_ETIMEDOUT;
	    case EALREADY:	return TT_DESKTOP_EALREADY;

// These codes are not as widely implemented.  E.g. HP/UX 9 has neither;
// AIX 3.2 does not have ENOTSUP.
#if defined(ECANCELED)
	    case ECANCELED:	return TT_DESKTOP_ECANCELED;
#endif
#if defined(ENOTSUP)		
	    case ENOTSUP:	return TT_DESKTOP_ENOTSUP;
#endif 
	}
	return (Tt_status)(TT_DESKTOP_ + errno);
}


Tt_status
_tt_pointer_error(void *p)
{
	if ((_tt_api_status_page <= (char *)p)
	     && ((char *)p < _tt_api_status_page + sizeof(_tt_api_status_page)))
	{
		return (Tt_status)((char *)p - _tt_api_status_page);
	} else {
		return TT_OK;
	}
}

Tt_status
_tt_int_error(int n)
{
	if (n>=0) {
		return TT_OK;
	} else {
		return (Tt_status) -n;
	}
}

void *
_tt_error_pointer(Tt_status s)
{
	return error_pointer(s);
}

int
_tt_error_int(Tt_status s)
{
	return error_int(s);
}

// The following two "secret" API calls exist because they
// were the only part of Link Manager being used by things
// in OpenWindows v3, so we had to continue supporting them
// even after LM was pulled.  They don't exist in any
// header file, so that we can continue to keep their
// existence a secret. RFM 05/28/91

extern "C" {
	int _tt_string_list_encode(const char **, unsigned char **);
	int _tt_string_list_decode(const unsigned char *, int, char ***);
}

/*
 * Encode this NULL-terminated string list into a byte string, point
 * *opaque_data_ptr at the malloc'd string, and return the length of
 * the byte string, or zero if the encoding failed.
 *
 * Caller is responsible for tt_free()ing *opaque_data_ptr.
 * strings.
 */
int
_tt_string_list_encode(const char **list, unsigned char **opaque_data_ptr)
{
	unsigned long bufsize;
	unsigned char *buf;
	XDR bufstream;
	char ***listaddr = (char ***)&list;
	
	bufsize = _tt_xdr_sizeof((xdrproc_t)_tt_xdr_cstring_list, &list);

	if (0==bufsize) return 0;

	buf = (unsigned char *)_tt_malloc((size_t)bufsize);
	xdrmem_create(&bufstream, (char *)buf, (u_int)bufsize, XDR_ENCODE);
	if (!_tt_xdr_cstring_list(&bufstream, listaddr)) {
		return 0;
	}
	*opaque_data_ptr = buf;
	return (int)bufsize;
}

/*
 * Decode this opaque byte string into a NULL-terminated string list,
 * and return the number of strings put into the list.  Caller should
 * tt_free the string list, and all the strings in it as well.
 */
int
_tt_string_list_decode(const unsigned char *opaque_data, int opaque_data_len,
		       char ***list_ptr)
{
	int count;
	int i;
	char **tar;
	XDR bufstream;

	xdrmem_create(&bufstream, (char *)opaque_data,
		      (u_int)opaque_data_len, XDR_DECODE);
	if (!_tt_xdr_cstring_list(&bufstream, list_ptr)) return 0;

	tar = *list_ptr;
	while (*tar++);
	count = tar-*list_ptr-1; // Don't count the null at the end.
	tar = *list_ptr;

	// The XDR routine used genuine malloc to allocate the storage.
	// Use _tt_take to put all the storage elements in the storage
	// stack so tt_release will get 'em.

	_tt_take((caddr_t)tar);
	for (i=0;i<count;++i) {
		_tt_take(tar[i]);
	}
	return count;
}

// _prepend_P_to_sessid() - Add the P on if isn't already.
// This is the fix for bugid 1105987.  Unfortunately
// we have to fix it here, rather than in ttsession where
// the session is generated, since previous versions of
// ToolTalk have already exposed the 'P'-less session ID.
// This probably isn't correctable until we completely get
// rid of the P from internal code.  Currently it's also
// returned through tt_default_session().
//
void
_prepend_P_to_sessid(const char *sessid, _Tt_string &sessid_with_P)
{
        if (sessid != NULL) {
                if (sessid[0] != 'P' && sessid[0] != 'X') {
                        sessid_with_P = "P ";
			sessid_with_P = sessid_with_P.cat(sessid);
                } else {
			sessid_with_P = sessid;
		}
        }
}

// Given a _Tt_db_results, map it to and return a TT api result.
Tt_status _tt_get_api_error(_Tt_db_results db_result, _Tt_api_context context)
{
	switch (context) {
	    case _TT_API_SPEC:
		switch (db_result) {
			case TT_DB_OK:
				return TT_OK;

			case TT_DB_WRN_FORWARD_POINTER:
				return TT_WRN_STALE_OBJID;

			case TT_DB_WRN_SAME_OBJECT_ID:
			case TT_DB_ERR_SAME_OBJECT:
				return TT_WRN_SAME_OBJID;

			case TT_DB_ERR_RPC_CONNECTION_FAILED:
			case TT_DB_ERR_RPC_FAILED:
			case TT_DB_ERR_DB_CONNECTION_FAILED:
				return TT_ERR_DBAVAIL;

			case TT_DB_ERR_DB_OPEN_FAILED:
				return TT_ERR_DBEXIST;

			// except  if tt_spec_remove() returns TT_WRN_NOTFOUND
			case TT_DB_ERR_NO_SUCH_OBJECT:
				return TT_ERR_OBJID;

			case TT_DB_ERR_NO_SUCH_PROPERTY:
				return TT_ERR_PROPNAME;

			case TT_DB_ERR_NO_OTYPE:
				return TT_ERR_OTYPE;

			case TT_DB_ERR_OTYPE_ALREADY_SET:
				return TT_ERR_READONLY;

			case TT_DB_ERR_UPDATE_CONFLICT:
				return TT_ERR_DBUPDATE;

			case TT_DB_ERR_CORRUPT_DB:
			case TT_DB_ERR_NO_ACCESS_INFO:
				return TT_ERR_DBCONSIST;

			case TT_DB_ERR_DISK_FULL:
				return TT_ERR_DBFULL;

			case TT_DB_ERR_ACCESS_DENIED:
				return TT_ERR_ACCESS;

			case TT_DB_ERR_ILLEGAL_FILE:
				return TT_ERR_PATH;

			case TT_DB_ERR_PROPS_CACHE_ERROR:
			case TT_DB_ERR_NO_SUCH_FILE:
			case TT_DB_ERR_ILLEGAL_OBJECT:
			case TT_DB_ERR_ILLEGAL_PROPERTY:
			case TT_DB_ERR_ILLEGAL_MESSAGE:
			case TT_DB_ERR_DB_LOCKED:
			case TT_DB_ERR_SAME_FILE:
			case TT_DB_ERR_FILE_EXISTS:
			case TT_DB_ERR_OBJECT_EXISTS:
				return TT_ERR_INTERNAL;

			default:
				return TT_ERR_INVALID;
		}
		break;

	    case _TT_API_FILE:
		switch (db_result) {
		    case TT_DB_OK:
			return TT_OK;
		    case TT_DB_ERR_RPC_CONNECTION_FAILED:
		    case TT_DB_ERR_DB_CONNECTION_FAILED:
		    case TT_DB_ERR_DB_LOCKED:
			return TT_ERR_DBAVAIL;
		    case TT_DB_ERR_DISK_FULL:
			return TT_ERR_DBFULL;
		    case TT_DB_ERR_CORRUPT_DB:
			return TT_ERR_DBCONSIST;
		    case TT_DB_ERR_DB_OPEN_FAILED:
			return TT_ERR_DBEXIST;
		    case TT_DB_ERR_ILLEGAL_FILE:
			return TT_ERR_PATH;
		    case TT_DB_ERR_ACCESS_DENIED:
			return TT_ERR_ACCESS;
		    default:
			return TT_ERR_INTERNAL;
		}
		break;

	    case _TT_API_FILE_MAP:
		switch (db_result) {
		    case TT_DB_OK:
			return TT_OK;

		    case TT_DB_ERR_DB_CONNECTION_FAILED:
		    case TT_DB_ERR_RPC_CONNECTION_FAILED:
		    case TT_DB_ERR_DB_LOCKED:
			return TT_ERR_DBAVAIL;

		    case TT_DB_ERR_RPC_UNIMP:
			return TT_ERR_UNIMP;

		    case TT_DB_ERR_DB_OPEN_FAILED:
			return TT_ERR_DBEXIST;

		    case TT_DB_ERR_ILLEGAL_FILE:
			return TT_ERR_PATH;

		    default:
			return TT_ERR_INTERNAL;
		}
		break;
	    default:
		break;
	}

	return TT_ERR_INTERNAL;
}	// end -_tt_get_api_error()-

static _Tt_api_stg_stack*
_get_stgstack(void)
{
	// The static storage stack for non-threaded clients.  If the client
	// is not threaded, this is a pointer to the actual storage stack.
	// If the client *is* threaded, a _Tt_api_stg_stack object
	// from thread-specific storage is grabbed from _Tt_threadspecific

	static _Tt_api_stg_stack *stgstack;

	// Occasionally (like in ttdt_open) the storage stack routines
	// get called before tt_open().  Ensure _tt_global, etc.
	// are initialized.

	_tt_internal_init();

#ifdef OPT_XTHREADS
        if (!_tt_global->multithreaded()) {
		// multithreading not initialized
		if (stgstack == 0) {
			stgstack = new _Tt_api_stg_stack;
		}
		
		return stgstack;
	}
	else {
		// The client is linked with threads, so make the
		// storage stack thread-specific.

		_Tt_threadspecific* tss = (_Tt_threadspecific *) 0;
		xthread_get_specific(_tt_global->threadkey, (void **) &tss);
		if (!tss) {
			// thread-specific storage not yet initialized -- do it
			tss = new _Tt_threadspecific(_tt_c_mp->default_c_session,
						     (_Tt_c_procid *) 0);
			xthread_set_specific(_tt_global->threadkey, tss);
		}

		return tss->thread_stgstack();
	}

#else
	if (stgstack==0) {
		stgstack = new _Tt_api_stg_stack;
	}

	return stgstack;
#endif
}
