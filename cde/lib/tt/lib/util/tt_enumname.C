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
//%%  $TOG: tt_enumname.C /main/8 1999/09/16 13:47:14 mgreess $ 			 				
/*
 *
 * @(#)tt_enumname.C	1.12 30 Jul 1993
 *
 * Copyright (c) 1990-1992 by Sun Microsystems, Inc.
 *
 * Implementations of _tt_enumname() functions for each enumeration type.
 *
 * A "switch" statement is used for each function.  This is somewhat
 * harder to type than an array declaration or a chain of conditionals,
 * but it has the advantage that if the enums are reordered no change 
 * need be made here; also, cfront warns if there are fewer cases in 
 * a switch on an enum variable than there are elements in the enum;
 * therefore, we'll get a warning if we add a enum element but forget
 * to update this code.
 * 
 */

#include <string.h>
#include "tt_options.h"
#ifdef OPT_CLASSING_ENGINE
#include "../slib/ce_err.h"
#endif
#include "util/tt_enumname.h"
     
/* following macros save a lot of typing */
#if defined(__STDC__) || defined(__cplusplus)
#define STRINGIFY(s) #s
#else
#define STRINGIFY(s) "s"
#endif

#define TTC(x) case x: return STRINGIFY(x)
#define TTBADC(x) case x: return STRINGIFY(x) " (! Tt_status)"

const char *
_tt_enumname(Tt_status x)
{
	switch(x) {
		TTC(TT_OK);
		TTC(TT_WRN_NOTFOUND);
		TTC(TT_WRN_STALE_OBJID);
		TTC(TT_WRN_STOPPED);
		TTC(TT_WRN_SAME_OBJID);
		TTC(TT_WRN_START_MESSAGE);
		TTC(TT_ERR_CLASS);
		TTC(TT_ERR_DBAVAIL);
		TTC(TT_ERR_DBEXIST);
		TTC(TT_ERR_FILE);
		TTC(TT_ERR_INVALID);
		TTC(TT_ERR_MODE);
		TTC(TT_ERR_ACCESS);
		TTC(TT_ERR_NOMP);
		TTC(TT_ERR_NOTHANDLER);
		TTC(TT_ERR_NUM);
		TTC(TT_ERR_OBJID);
		TTC(TT_ERR_OP);
		TTC(TT_ERR_OTYPE);
		TTC(TT_ERR_ADDRESS);
		TTC(TT_ERR_PATH);
		TTC(TT_ERR_POINTER);
		TTC(TT_ERR_PROCID);
		TTC(TT_ERR_PROPLEN);
		TTC(TT_ERR_PROPNAME);
		TTC(TT_ERR_PTYPE);
		TTC(TT_ERR_DISPOSITION);
		TTC(TT_ERR_SCOPE);
		TTC(TT_ERR_SESSION);
		TTC(TT_ERR_VTYPE);
		TTC(TT_ERR_NO_VALUE);
		TTC(TT_ERR_INTERNAL);
		TTC(TT_ERR_READONLY);
		TTC(TT_ERR_NO_MATCH);
		TTC(TT_ERR_UNIMP);
		TTC(TT_ERR_OVERFLOW);
		TTC(TT_ERR_PTYPE_START);
		TTC(TT_ERR_CATEGORY);
		TTC(TT_ERR_DBUPDATE);
		TTC(TT_ERR_DBFULL);
		TTC(TT_ERR_DBCONSIST);
		TTC(TT_ERR_STATE);
		TTC(TT_ERR_NOMEM);
		TTC(TT_ERR_SLOTNAME);
		TTC(TT_ERR_XDR);
		TTC(TT_ERR_NETFILE);
		TTC(TT_ERR_TOOLATE);
		TTC(TT_ERR_AUTHORIZATION);
		TTC(TT_ERR_VERSION_MISMATCH);
		TTC(TT_DESKTOP_EPERM);
		TTC(TT_DESKTOP_ENOENT);
		TTC(TT_DESKTOP_EINTR);
		TTC(TT_DESKTOP_EIO);
		TTC(TT_DESKTOP_EAGAIN);
		TTC(TT_DESKTOP_ENOMEM);
		TTC(TT_DESKTOP_EACCES);
		TTC(TT_DESKTOP_EFAULT);
		TTC(TT_DESKTOP_EEXIST);
		TTC(TT_DESKTOP_ENODEV);
		TTC(TT_DESKTOP_ENOTDIR);
		TTC(TT_DESKTOP_EISDIR);
		TTC(TT_DESKTOP_EINVAL);
		TTC(TT_DESKTOP_ENFILE);
		TTC(TT_DESKTOP_EMFILE);
		TTC(TT_DESKTOP_ETXTBSY);
		TTC(TT_DESKTOP_EFBIG);
		TTC(TT_DESKTOP_ENOSPC);
		TTC(TT_DESKTOP_EROFS);
		TTC(TT_DESKTOP_EMLINK);
		TTC(TT_DESKTOP_EPIPE);
		TTC(TT_DESKTOP_ENOMSG);
		TTC(TT_DESKTOP_EDEADLK);
		TTC(TT_DESKTOP_ECANCELED);
		TTC(TT_DESKTOP_ENOTSUP);
		TTC(TT_DESKTOP_ENODATA);
		TTC(TT_DESKTOP_EPROTO);
		TTC(TT_DESKTOP_ENOTEMPTY);
		TTC(TT_DESKTOP_ETIMEDOUT);
		TTC(TT_DESKTOP_EALREADY);
		TTC(TT_DESKTOP_UNMODIFIED);
		TTC(TT_MEDIA_ERR_SIZE);
		TTC(TT_MEDIA_ERR_FORMAT);
		TTC(TT_AUTHFILE_ACCESS);
		TTC(TT_AUTHFILE_LOCK);
		TTC(TT_AUTHFILE_LOCK_TIMEOUT);
		TTC(TT_AUTHFILE_UNLOCK);
		TTC(TT_AUTHFILE_MISSING);
		TTC(TT_AUTHFILE_ENTRY_MISSING);
		TTC(TT_AUTHFILE_WRITE);
		TTBADC(TT_DESKTOP_);
		TTBADC(TT_WRN_APPFIRST);
		TTBADC(TT_WRN_LAST);
		TTBADC(TT_ERR_APPFIRST);
		TTBADC(TT_ERR_LAST);
		TTBADC(TT_STATUS_LAST);
	    default:
		return "! Tt_status";
	}
}



const char *
_tt_enumname(Tt_filter_action x) {
      switch (x) {
	      TTC(TT_FILTER_CONTINUE); 
	      TTC(TT_FILTER_STOP); 
	    case TT_FILTER_LAST:
	    default:
	      return "! Tt_filter_action";
      }
}

const char *
_tt_enumname(Tt_mode x) {
	switch (x) {
		TTC(TT_MODE_UNDEFINED);
		TTC(TT_IN);
		TTC(TT_OUT);
		TTC(TT_INOUT);
	      case TT_MODE_LAST:
	      default:
	      return "! Tt_mode";	
	}
}

const char *
_tt_enumname(Tt_scope x) {
	switch (x) {
		TTC(TT_SCOPE_NONE);
		TTC(TT_SESSION);
		TTC(TT_FILE);
		TTC(TT_BOTH);
		TTC(TT_FILE_IN_SESSION);
	      default:
		return "! Tt_scope";	
	}
}

const char *
_tt_enumname(Tt_class x) {
	switch (x) {
		TTC(TT_CLASS_UNDEFINED);
		TTC(TT_NOTICE);
		TTC(TT_REQUEST);
		TTC(TT_OFFER);
	      case TT_CLASS_LAST:
	      default:
		return "! Tt_class";
	}
}

const char *
_tt_enumname(Tt_category x) {
	switch (x) {
		TTC(TT_CATEGORY_UNDEFINED);
		TTC(TT_OBSERVE);
		TTC(TT_HANDLE);
		TTC(TT_HANDLE_PUSH);
		TTC(TT_HANDLE_ROTATE);
	      case TT_CATEGORY_LAST:
	      default:
		return "! Tt_category";
	}
}

const char *
_tt_enumname(Tt_address x) {
	switch (x) {
		TTC(TT_PROCEDURE);
		TTC(TT_OBJECT);
		TTC(TT_HANDLER);
		TTC(TT_OTYPE);
	      case TT_ADDRESS_LAST:
	      default:
		return "! Tt_address";
	}
}

const char *
_tt_enumname(Tt_disposition x) {
	switch ((int)x) {
		/* 
		 * Flag bits, not enumerated values. Cast to int to avoid 
		 * warnings from cfront.
		 */
	      case (int)TT_DISCARD:
		return "TT_DISCARD";
	      case (int)TT_QUEUE:
		return "TT_QUEUE";
	      case (int)TT_START:
		return "TT_START";
	      case (int)TT_QUEUE+(int)TT_START:
		return "TT_QUEUE+TT_START";
	      default:
		return "! Tt_disposition";
	}
}

const char *
_tt_enumname(Tt_state x) {
	switch (x) {
		TTC(TT_CREATED);
		TTC(TT_SENT);
		TTC(TT_HANDLED);
		TTC(TT_FAILED);
		TTC(TT_QUEUED);
		TTC(TT_STARTED);
		TTC(TT_REJECTED);
		TTC(TT_RETURNED);
		TTC(TT_ACCEPTED);
		TTC(TT_ABSTAINED);
	      case TT_STATE_LAST:
	      default:
		return "! Tt_state";
	}
}

const char *
_tt_enumname(Tt_feature x) {
	switch (x) {
		TTC(TT_FEATURE_MULTITHREADED);
	      case _TT_FEATURE_LAST:
	      default:
		return "! Tt_feature";
	}
}

const char *
_tt_enumname(Tt_callback_action x) {
        switch (x) {
                TTC(TT_CALLBACK_CONTINUE); 
                TTC(TT_CALLBACK_PROCESSED);
                TTC(TT_CALLBACK_LAST); 
              default:
                return "! Tt_callback_action";
        }
}

#ifdef OPT_CLASSING_ENGINE

const char *
_tt_enumname(_Tt_ce_status ce_err)
{
	switch (ce_err) {
	      TTC(CE_ERR_INTERNAL_ERROR);
	      TTC(CE_ERR_ERROR_READING_DB);
	      TTC(CE_ERR_WRITE_IN_PROGRESS);
	      TTC(CE_ERR_DB_NOT_LOADED);
	      TTC(CE_ERR_WRITE_NOT_STARTED);
	      TTC(CE_ERR_NAMESPACE_EXISTS);
	      TTC(CE_ERR_NAMESPACE_NOT_EMPTY);
	      TTC(CE_ERR_NAMESPACE_DOES_NOT_EXIST);
	      TTC(CE_ERR_NS_ENTRY_EXISTS);
	      TTC(CE_ERR_ENTRY_NOT_ALLOCED);
	      TTC(CE_ERR_ATTRIBUTE_EXISTS);
	      TTC(CE_ERR_NO_MEMORY);
	      TTC(CE_ERR_NO_PERMISSION_TO_WRITE);
	      TTC(CE_ERR_WRONG_ARGUMENTS);
	      TTC(CE_ERR_ERROR_WRITING_DB);
	      TTC(CE_ERR_OPENING_DB);
	      TTC(CE_ERR_DB_LOCKED);
	      TTC(CE_ERR_ERROR_OPENING_FILE);
	      TTC(CE_ERR_WRONG_DATABASE_VERSION);
	      TTC(CE_ERR_UNKNOWN_DATABASE_NAME);
	      TTC(CE_ERR_BAD_DATABASE_FILE);
	      default:
	      return "! _Tt_ce_status";
	}
}
#endif
