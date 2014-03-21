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
 *
 *	$TOG: Session.C /main/16 1998/07/24 16:08:39 mgreess $
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

#if defined (__osf__)

// Workaround for CDExc19308
//
// This ifdef was added as a workaround for the
// bug in the OSF1 V3.2 148 /usr/include/sys/localedef.h
// header file, and should be removed as soon as the bug is
// fixed.

#include <iconv.h>
#include <locale.h>
#include <time.h>
#include <DtHelp/LocaleXlate.h>
#include <errno.h>
#include <stdlib.h>
#include <limits.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pwd.h>
#include <sys/socket.h>
#include <ctype.h>

#include <DtMail/DtMail.hh>
#include <EUSDebug.hh>
#include "DynamicLib.hh"
#include <DtMail/Threads.hh>
#include <DtMail/ImplDriver.hh>
#include "ImplConfigTable.hh"
#include "SigChldImpl.hh"
#include <DtMail/IO.hh>
#include <DtMail/Common.h>

#ifndef True
#define True 1
#endif
#ifndef False
#define False 0
#endif

#else

#include <stdlib.h>
#include <unistd.h>
#if defined(USL) && (OSMAJORVERSION == 2)
extern "C" {
#endif
#if defined(USL) && (OSMAJORVERSION == 2)
};
#endif
#include <string.h>
#include <pwd.h>
#include <sys/socket.h>
#include <ctype.h>

#include <DtMail/DtMail.hh>

#include <EUSDebug.hh>
#include "DynamicLib.hh"
#include <DtMail/Threads.hh>
#include <DtMail/ImplDriver.hh>
#include "ImplConfigTable.hh"
#include "SigChldImpl.hh"
#include <DtMail/IO.hh>
#include <DtMail/Common.h>

// For CHARSET
//-------------------------------------
// HACK ALERT
// Any code change within "For CHARSET" should be changed in
// RFCBodyPart and Session because the same methods are duplicated
// in both of these classes.
// See RFCImpl.hh or DtMail/DtMail.hh for more explanation.
//-------------------------------------
#include <locale.h>
#include <time.h>
#include <DtHelp/LocaleXlate.h>
#include <errno.h>
#include <stdlib.h>
#include <limits.h>
#include <ctype.h>
 
#ifndef True
#define True 1
#endif
#ifndef False
#define False 0
#endif
 
#if defined(SunOS) && (SunOS < 55)
extern "C" {
#endif

#if !defined(linux)
#  include <iconv.h>
#else
   // Iconv not defined for linux.  Use the EUSCompat stubs instead.
#  include <EUSCompat.h>
#endif

#if defined(SunOS) && (SunOS < 55)
}
#endif

// End of For CHARSET
#endif	// ! defined(__osf__)

DtVirtArray<SigChldInfo *>	*DtMailSigChldList;
void *				_DtMutex;

static const int SessionSignature = 0xef9421cc;
static const int MaxImpls = 16;

static const int MAXIMUM_PATH_LENGTH = 2048;

DtMail::Session::Session(DtMailEnv & error, const char * app_name)
: _events(16), _valid_keys(2048)
{
    _DtMutex = MutexInit();

    error.clear();

    _object_signature = 0;
    _cur_key = 0;

    // Create the ToolTalk session for managing file locking,
    // if one doesn't exist.
    _tt_channel = tt_default_procid();
    if (tt_pointer_error(_tt_channel) != TT_OK) {
	_tt_channel = ttdt_open(&_tt_fd, app_name, "SunSoft", "%I", 0);
	if (tt_pointer_error(_tt_channel) != TT_OK) {
	    error.setError(DTME_TTFailure);
	    DebugPrintf(1,
			"DtMail::createSession - ttdt_open returns %s\n",
			tt_status_message(tt_pointer_error(_tt_channel)));
	    return;
	}
    }
    else {
	_tt_fd = tt_fd();
    }

    // The event_fd is how we allow async behavior to occur in a
    // compatible way. We use a Unix domain socket as the file descriptor.
    // The client will watch for activity on this file descriptor, and
    // call our event routine when there is activity (either from XtMainLoop,
    // or through some other method).
    //
    pipe(_event_fd);

    _app_name = strdup(app_name);

    DtMailEnv b_error;

    _mail_rc = new MailRc(error, this);

    buildImplTable(error);
    if (error.isSet()) {
	return;
    }

    _obj_mutex = MutexInit();

    // The default implementation is specified via the DEFAULT_BACKEND
    // variable. If this is not set in the .mailrc, then choose entry
    // zero.
    //
    const char * value;
    _mail_rc->getValue(b_error, "DEFAULT_BACKEND", &value);
    if (b_error.isNotSet()) {
	_default_impl = lookupImpl(value);
	if (_default_impl < 0) {
	    _default_impl = 0;
	}
    }
    else {
	b_error.clear();
	_default_impl = 0;
    }

    DtMailSigChldList = new DtVirtArray<SigChldInfo *>(8);

    _busy_cb = NULL;
    _busy_cb_data = NULL;
    _canAutoSave = DTM_TRUE;

    _object_signature = SessionSignature;

    return;
}

DtMail::Session::~Session(void)
{
    if (_object_signature != SessionSignature) { // Been here, did that!
	return;
    }
    
  {
      MutexLock lock_scope(_obj_mutex);
      
      _object_signature = 0;
      
      ttdt_close(_tt_channel, NULL, 1);
      
      DtMailEnv b_error;
      
      // Close off the dynamic libraries and free the impl table.
      for (int tbl = 0; tbl < _num_impls; tbl++) {
	  free(_impls[tbl].impl_name);
	  DynamicLib * dl = (DynamicLib *)_impls[tbl].impl_lib;
	  delete dl;
      }

      free(_impls);

      lock_scope.unlock_and_destroy();
      delete _mail_rc;

      close(_event_fd[0]);
      close(_event_fd[1]);
  }
    
}

const char **
DtMail::Session::enumerateImpls(DtMailEnv & error)
{
    error.clear();
    return(_impl_names);
}

void
DtMail::Session::setDefaultImpl(DtMailEnv & error, const char * impl)
{
    int slot = lookupImpl(impl);

    if (slot < 0) {
	error.setError(DTME_NoSuchImplementation);
	return;
    }

    MutexLock lock_scope(_obj_mutex);

    _default_impl = slot;
    error.clear();
}

const char *
DtMail::Session::getDefaultImpl(DtMailEnv & error)
{
    if (_num_impls == 0) {
	error.setError(DTME_NoImplementations);
	return(NULL);
    }

    error.clear();

    return(_impl_names[_default_impl]);
}

void
DtMail::Session::queryImpl(DtMailEnv & error,
			   const char * impl,
			   const char * capability,
			   ...)
{
    va_list	args;

    va_start(args, capability);
    queryImplV(error, impl, capability, args);
    va_end(args);

    return;
}

void
DtMail::Session::queryImplV(DtMailEnv & error,
			   const char * impl,
			   const char * capability,
			   va_list args)
{
    int slot = lookupImpl(impl);

    if (slot < 0) {
	error.setError(DTME_NoSuchImplementation);
	return;
    }

    error.clear();

    // We need to retrieve the QueryImpl entry point for the implementation.
    //
    QueryImplEntry qie;
    
    qie = (QueryImplEntry)_impls[slot].impl_meta_factory(QueryImplEntryOp);
    if (!qie) {
	error.setError(DTME_ImplFailure);
	return;
    }

    qie(*this, error, capability, args);

    return;
}

DtMail::MailBox *
DtMail::Session::mailBoxConstruct(DtMailEnv & error,
				  DtMailObjectSpace space,
				  void * arg,
				  DtMailCallback open_callback,
				  void * client_data,
				  const char * impl_name)
{
    // If the client specified an implementation of choice, then that
    // is the only thing we use.
    //
    int primary_impl = _default_impl;

    if (impl_name) {
	int sl = lookupImpl(impl_name);
	if (sl < 0) {
	    error.setError(DTME_NoSuchImplementation);
	    return(NULL);
	}

	primary_impl = sl;
    }

    // First thing we will do is see if the default implementation
    // can open this file. If so, then we will create a mail box object
    // based on the default implementation.
    //
    int		const_impl;
    QueryOpenEntry qoe;
    
    qoe = (QueryOpenEntry)
	    _impls[primary_impl].impl_meta_factory(QueryOpenEntryOp);
    if (!qoe) {
	error.setError(DTME_ImplFailure);
	return(NULL);
    }

    if (qoe(*this, error, space, arg) == DTM_FALSE) {
	// Don't go on if the client specified an implementation.
	//
	if (impl_name) {
	    error.setError(DTME_ImplFailure);
	    return(NULL);
	}

	// Oh well, let's walk through the list of impls and see if any of
	// them will take ownership of this file.
	//
	MutexLock lock_scope(_obj_mutex);

	const_impl = -1;
	for(int slot = 0; slot < _num_impls; slot++) {
	    qoe = (QueryOpenEntry)
		    _impls[slot].impl_meta_factory(QueryOpenEntryOp);
	    if (!qoe) {
		// Just skip this implementation.
		continue;
	    }
	    if (qoe(*this, error, space, arg) == DTM_TRUE) {
		const_impl = slot;
		break;
	    }
	}

	// If we didn't find an impl, then we have to give up.
	//
	if (const_impl < 0) {
	    error.setError(DTME_NotMailBox);
	    return(NULL);
	}
    }
    else {
	const_impl = primary_impl;
    }

    // At this point we have an implementation that is willing to work
    // with the path. Get its mail box constructor and build a mailbox
    // from it.
    //
    MailBoxConstructEntry mbce;
    
    mbce = (MailBoxConstructEntry)
	     _impls[const_impl].impl_meta_factory(MailBoxConstructEntryOp);
    if (!mbce) {
	error.setError(DTME_ImplFailure);
	return(NULL);
    }

    return(mbce(*this, error, space, arg, open_callback, client_data));
}

DtMail::Message *
DtMail::Session::messageConstruct(DtMailEnv & error,
				  DtMailObjectSpace space,
				  void * arg,
				  DtMailCallback open_callback,
				  void * client_data,
				  const char * impl_name)
{
    // If the client specified an implementation of choice, then that
    // is the only thing we use.
    //
    int primary_impl = _default_impl;

    if (impl_name) {
	int sl = lookupImpl(impl_name);
	if (sl < 0) {
	    error.setError(DTME_NoSuchImplementation);
	    return(NULL);
	}

	primary_impl = sl;
    }

    int		const_impl;
    QueryMessageEntry qoe;
    
    qoe = (QueryMessageEntry)
	      _impls[primary_impl].impl_meta_factory(QueryMessageEntryOp);
    if (!qoe) {
	error.setError(DTME_ImplFailure);
	return(NULL);
    }

    if (qoe(*this, error, space, arg) == DTM_FALSE) {
	// Don't go on if the client specified an implementation.
	//
	if (impl_name) {
	    error.setError(DTME_ImplFailure);
	    return(NULL);
	}

	// Oh well, let's walk through the list of impls and see if any of
	// them will take ownership of this file.
	//
	MutexLock lock_scope(_obj_mutex);

	const_impl = -1;
	for(int slot = 0; slot < _num_impls; slot++) {
	    qoe = (QueryMessageEntry)
		    _impls[slot].impl_meta_factory(QueryMessageEntryOp);
	    if (!qoe) {
		// Just skip this implementation.
		continue;
	    }
	    if (qoe(*this, error, space, arg) == DTM_TRUE) {
		const_impl = slot;
		break;
	    }
	}

	// If we didn't find an impl, then we have to give up.
	//
	if (const_impl < 0) {
	    error.setError(DTME_ImplFailure);
	    return(NULL);
	}
    }
    else {
	const_impl = primary_impl;
    }

    // At this point we have an implementation that is willing to work
    // with the path. Get its mail box constructor and build a mailbox
    // from it.
    //
    MessageConstructEntry mbce;
    
    mbce = (MessageConstructEntry)
	     _impls[const_impl].impl_meta_factory(MessageConstructEntryOp);
    if (!mbce) {
	error.setError(DTME_ImplFailure);
	return(NULL);
    }

    return(mbce(*this, error, space, arg, open_callback, client_data));
}

DtMail::Transport *
DtMail::Session::transportConstruct(DtMailEnv & error,
				    const char * impl,
				    DtMailStatusCallback call_back,
				    void * client_data)
{
    // We need to find the implementation for starters.
    //
    int slot = lookupImpl(impl);
    if (slot < 0) {
	error.setError(DTME_NoSuchImplementation);
	return(NULL);
    }

    TransportConstructEntry tce;
    tce = (TransportConstructEntry)
	    _impls[slot].impl_meta_factory(TransportConstructEntryOp);

    if (!tce) {
	error.setError(DTME_NotSupported);
	return(NULL);
    }

    return(tce(*this, error, call_back, client_data));
}

DtMail::MailRc *
DtMail::Session::mailRc(DtMailEnv & error)
{
    error.clear();

    return(_mail_rc);
}

//
// NEEDS TO BE DELETED .. SHOULD NO LONGER BE USED...
//
void
DtMail::Session::setError(DtMailEnv & error, const DTMailError_t minor_code)
{
  error.setError(minor_code);
  //DtMail::setError(*this, error, minor_code);
}

DtMailBoolean
DtMail::Session::pollRequired(DtMailEnv & error)
{
    error.clear();

#if defined(POSIX_THREADS)
    return(DTM_FALSE);
#else
    return(DTM_TRUE);
#endif

}

int
DtMail::Session::eventFileDesc(DtMailEnv & error)
{
    error.clear();

    return(_event_fd[0]);
}

void
DtMail::Session::poll(DtMailEnv & error)
{
    error.clear();

#if defined(POSIX_THREADS)
    return; // A thread does this job.
#else

    // We will grab the time and determine what needs to run.
    // Any events that have expired since the last time we were
    // called are automatically ran. If the event returns DTM_TRUE,
    // then reset last_ran to the current time to cause the event
    // to not run for the full wait interval; otherwise, refrain
    // and let the event happen again at the next poll interval
    //
    time_t now = time(NULL);

    for (int ev = 0; ev < _events.length(); ev++) {
	EventRoutine * event = _events[ev];
	if ((now - event->last_ran) > event->interval) {
	    if (event->routine(event->client_data) == DTM_TRUE)
	      event->last_ran = now;
	}
    }

    return;
#endif

}

char *
DtMail::Session::expandPath(DtMailEnv & error, const char * path)
{
    const char * fold_path;

    if (path == NULL) {
	error.setError(DTME_BadArg);
	return(NULL);
    }

    error.clear();

    char * exp_name = (char *)malloc(MAXIMUM_PATH_LENGTH);
    if (exp_name == NULL)  {
	error.setError(DTME_NoMemory);
	return(NULL);
    }
    if (strchr(path, '$') != NULL) {
    	sprintf (exp_name, "echo %s", path);
    	FILE *fp;
    	if ((fp = popen(exp_name, "r")) != NULL) {
          	exp_name[0] = '\0';
          	if (fgets(exp_name, MAXIMUM_PATH_LENGTH, fp) != NULL &&
			exp_name[0] != '\0') 
    	        	// get rid of \n at end of string
	  		exp_name[strlen(exp_name)-1] = '\0';
	  	else
			strcpy(exp_name, path); 
	  	pclose(fp);
    	}
	else 
		strcpy(exp_name, path);
    }
    else 
	strcpy(exp_name, path);

    char * exp_name2 = (char *)malloc(MAXIMUM_PATH_LENGTH);
    exp_name2[0] = '\0';

    switch (exp_name[0]) {
      case '+':
	// This is relative to the folder path. Figure out what that is.
      {
	  _mail_rc->getValue(error, "folder", &fold_path);
	  if (error.isNotSet()) {
               if (*fold_path != '/' && *fold_path != '.' && 
			*fold_path != '~' && *fold_path != '$') 
		  strcpy(exp_name2, "~/");

	  	strcat(exp_name2, fold_path);
	  	strcat(exp_name2, "/");
	  }
	  else  // Use the default folder
	  	strcpy(exp_name2, "~/");
		
	  strcat(exp_name2, &exp_name[1]);
      
          // We need to call ourselves again to deal with 
          // relative paths in the folder directory.
          // 
          char * old_exp = exp_name2;
          exp_name2 = expandPath(error, old_exp);
	  free(old_exp);
	  break;
      }
	
      case '~':
	// This is relative to the user's home directory.
      {
	  passwd pw;
	  const char * start;

	  if (exp_name[1] == '/' || exp_name[1] == '\0') {
	      GetPasswordEntry(pw);
	      start = &exp_name[1];
	  }
	  else {
	      passwd * pw_p;

	      char * slash = strchr(&exp_name[1], '/');
	      if (slash == NULL) {
		   error.clear();
		   error.setError(DTME_NoSuchFile);
		   break;
	      }

	      int len = slash - &exp_name[1];
	      char * name = new char[len + 1];
	      strncpy(name, &exp_name[1], len);
	      name[len] = 0;
	      pw_p = getpwnam(name);

	      if (!pw_p) {
		  error.clear();
		  error.setError(DTME_NoSuchFile);
		  break;
	      }
	      pw = *pw_p;

	      delete [] name;

	      start = slash;
	  }

	  strcpy(exp_name2, pw.pw_dir);
	  strcat(exp_name2, start);
	  break;
      }

      // We have a directory or no specials. Just copy the path and
      // return.
      case '.':
      case '/':
      default:
	strcpy(exp_name2, exp_name);
	break;

    }
    free(exp_name);
    return(exp_name2);
}

// This routine takes a path and checks to see if the path can be
// expressed relative to the "folder" path.  If it can, it returns
// the relative path; otherwise, it returns the original path.
char *
DtMail::Session::getRelativePath(DtMailEnv & error, const char * path)
{
    const char * fold_path;

    if (path == NULL) {
	error.setError(DTME_BadArg);
	return(NULL);
    }

    error.clear();

    char * exp_name = (char *)malloc(MAXIMUM_PATH_LENGTH);
    if (!exp_name) {
	error.setError(DTME_NoMemory);
	return(NULL);
    }
    exp_name[0] = '\0'; // Just for errors.

    switch (path[0]) {
	case '/':
	    // This is an absolute path, so there is a chance that
	    // we can trim it down to a relative path if it goes down
	    // the same way as the folder path.
	{
	    _mail_rc->getValue(error, "folder", &fold_path);
	    if (error.isNotSet()) {
		strcpy(exp_name, fold_path);
	      
		// We need to call ourselves again to deal with 
		// relative paths in the folder directory.
		// 
		char * old_exp = exp_name;
		exp_name = expandPath(error, old_exp);
		free(old_exp);

		// Check to see if the path starts with the folder path.
		char * matched_path = const_cast<char *>(strstr(path, exp_name));
		if (matched_path == path) {
		    // Yes it does, make it a relative path to the folder dir.
		    int folder_path_length = strlen(exp_name);
		    while (path[folder_path_length] == '/')
		      folder_path_length++;
		    strcpy(exp_name, &path[folder_path_length]);
		    break;
		} else {
		    strcpy(exp_name, path);
		    break;
		}
	    }
	    else {
		// There is no folder variable so just fall through to the
		// default.
		error.clear();
	    }
	}
	case '+':
	    // This is relative to the folder path. Leave it alone.
	    // The only time we are likely to see a leading '+' is
	    // when the path was carried over from mailtool in the .mailrc.
	case '~':
	    // This is relative to the user's home directory.  Leave it alone.
	    // The only time we are likely to see a leading '~' is
	    // when the path was carried over from mailtool in the .mailrc.
	case '.':
	    // This is relative to the current directory where dtmail is
	    // running.  Leave it alone.  The only time we are likely to see
	    // a leading '.' is when the path was carried over from mailtool
	    // in the .mailrc.
	default:
	{
	    strcpy(exp_name, path);
	    break;
	}
    }
    return(exp_name);

}


void
DtMail::Session::addEventRoutine(DtMailEnv & error,
				 DtMailEventFunc routine,
				 void * client_data,
				 time_t interval)
{
    error.clear();

    EventRoutine * er = new EventRoutine;

    er->routine = routine;
    er->client_data = client_data;
    er->interval = interval;
    er->last_ran = 0; // This will case this routine to run immediately.

    _events.append(er);
}

void
DtMail::Session::removeEventRoutine(DtMailEnv & error,
				    DtMailEventFunc routine,
				    void * client_data)
{
    error.clear();

    for (int slot = 0; slot < _events.length(); slot++) {
	EventRoutine * event = _events[slot];
	if (event->routine == routine &&
	    event->client_data == client_data) {
	    delete event;
	    _events.remove(slot);
	}
    }
}

void
DtMail::Session::writeEventData(DtMailEnv&,
				const void * buf,
				const unsigned long size)
{
    int status = SafeWrite(_event_fd[1], buf, (int)size);
}

DtMailBoolean
DtMail::Session::validObjectKey(DtMailObjectKey key)
{
    return(_valid_keys.indexof(key) < 0 ? DTM_FALSE : DTM_TRUE);
}

DtMailObjectKey
DtMail::Session::newObjectKey(void)
{
    MutexLock lock_scope(_obj_mutex);

    _cur_key += 1;
    _valid_keys.append(_cur_key);

    return(_cur_key);
}

void
DtMail::Session::removeObjectKey(DtMailObjectKey key)
{
    MutexLock lock_scope(_obj_mutex);

    int slot = _valid_keys.indexof(key);
    if (slot >= 0) {
	_valid_keys.remove(slot);
    }
}

void
DtMail::Session::registerDisableGroupPrivilegesCallback(
				DisableGroupPrivilegesCallback cb,
				void * cb_data)
{
    _disableGroupPrivileges_cb = cb;
    _disableGroupPrivileges_cb_data = cb_data;
}

#ifdef DEAD_WOOD
void
DtMail::Session::unregisterDisableGroupPrivilegesCallback(void)
{
    _disableGroupPrivileges_cb = NULL;
    _disableGroupPrivileges_cb_data = NULL;
}
#endif /* DEAD_WOOD */

void
DtMail::Session::disableGroupPrivileges(void)
{
    if (_disableGroupPrivileges_cb) {
	_disableGroupPrivileges_cb(_disableGroupPrivileges_cb_data);
    }
}

void
DtMail::Session::registerEnableGroupPrivilegesCallback(
					EnableGroupPrivilegesCallback cb,
					void * cb_data)
{
    _enableGroupPrivileges_cb = cb;
    _enableGroupPrivileges_cb_data = cb_data;
}

#ifdef DEAD_WOOD
void
DtMail::Session::unregisterEnableGroupPrivilegesCallback(void)
{
    _enableGroupPrivileges_cb = NULL;
    _enableGroupPrivileges_cb_data = NULL;
}
#endif /* DEAD_WOOD */

void
DtMail::Session::enableGroupPrivileges(void)
{
    if (_enableGroupPrivileges_cb) {
	_enableGroupPrivileges_cb(_enableGroupPrivileges_cb_data);
    }
}

void
DtMail::Session::registerBusyCallback(DtMailEnv&,
				      BusyApplicationCallback cb,
				      void * cb_data)
{
    _busy_cb = cb;
    _busy_cb_data = cb_data;
}

#ifdef DEAD_WOOD
void
DtMail::Session::unregisterBusyCallback(DtMailEnv & error)
{
    _busy_cb = NULL;
    _busy_cb_data = NULL;
}
#endif /* DEAD_WOOD */

void
DtMail::Session::setBusyState(DtMailEnv &error, DtMailBusyState busy_state)
{
    if (_busy_cb) {
	_busy_cb(error, busy_state, _busy_cb_data);
    }
}


void
DtMail::Session::registerLastInteractiveEventTimeCallback(
					LastInteractiveEventTimeCallback cb,
					void * cb_data)
{
    _interactive_time_cb = cb;
    _interactive_time_cb_data = cb_data;
}

long
DtMail::Session::lastInteractiveEventTime(void)
{
    if (_interactive_time_cb) {
      return(_interactive_time_cb(_interactive_time_cb_data));
    }
    else
      return(0);
}

void
DtMail::Session::buildImplTable(DtMailEnv & error)
{
  error.clear();

    // Let's pick a ridiculous number of implementations.
    _impls = (Impls *)malloc(sizeof(Impls) * MaxImpls);
    _impl_names = (const char **)malloc(sizeof(char *) * (MaxImpls + 1));

    // We will simply walk through the default implementations
    // to start, adding them to the impl table.
    int tbl;
    for (tbl = 0, _num_impls = 0; initial_impls[tbl].meta_entry_point; tbl++) {
	// Get the library handle.
	DynamicLib * dl = CreatePlatformDl(initial_impls[tbl].lib_name);

	if (dl) { // We are only interested in libraries we can load.
	    _impls[_num_impls].impl_lib = dl;
	    _impls[_num_impls].impl_meta_factory =
	      (MetaImplFactory)dl->getSym(initial_impls[tbl].meta_entry_point);
	    if (_impls[_num_impls].impl_meta_factory == NULL) {
		delete dl;
		continue;
	    }
	    _impls[_num_impls].impl_name = strdup(initial_impls[tbl].impl_name);
	    _impl_names[_num_impls] = _impls[_num_impls].impl_name;
	    _num_impls += 1;
	}
    }

    _impl_names[_num_impls] = NULL;

    if (_num_impls == 0) {
	error.setError(DTME_NoImplementations);
    }
}

int
DtMail::Session::lookupImpl(const char * impl)
{
    MutexLock lock_scope(_obj_mutex);
    for(int i = 0; i < _num_impls; i++) {
	if (strcmp(_impls[i].impl_name, impl) == 0) {
	    return(i);
	}
    }

    return(-1);
}

void
DtMail::Session::setAutoSaveFlag(
    DtMailBoolean flag
)
{
    _canAutoSave = flag;
}

DtMailBoolean
DtMail::Session::getAutoSaveFlag()
{
    return(_canAutoSave);
}

extern "C" void
ChildExitNotify(const int pid, const int status)
{
    // We need to lookup the child, and set the status of the
    // correct condition variable. We will remove the slot, but
    // the thread will destroy the buffer in the slot.
    //
    for (int slot = 0; slot < DtMailSigChldList->length(); slot++) {
	if ((*DtMailSigChldList)[slot]->pid == pid) {
	    (*DtMailSigChldList)[slot]->cond = status;
	    DtMailSigChldList->remove(slot);
	    return;
	}
    }

    // Not finding the pid is not a problem. Just means it wasn't
    // one of ours.
    //
    return;
}


// For CHARSET
/*
 * Wrapper functions taken from libHelp/CEUtil.c
 *
 * We took these functions and renamed them because
 * 1. Originally these are called _DtHelpCeXlate* and thus they are private
 *    to libHelp and not exported to outside of libHelp.
 * 2. When these functions are moved to another library, then users of these
 *    functions would only need to link with a different library.  The caller
 *    doesn't have to modify code.
 */

static const char *DfltStdCharset = "us-ascii";
static const char *DfltStdLang = "C";

static char       MyPlatform[_DtPLATFORM_MAX_LEN+1];
static _DtXlateDb MyDb = NULL;
static char       MyProcess = False;
static char       MyFirst   = True;
static int        ExecVer;
static int        CompVer;


/******************************************************************************
 * Function:    static int OpenLcxDb ()
 *
 * Parameters:   none
 *
 * Return Value:  0: ok
 *               -1: error
 *
 * errno Values:
 *
 * Purpose: Opens the Ce-private Lcx database
 *
 *****************************************************************************/
int
DtMail::Session::OpenLcxDb (void)
{
    time_t	time1  = 0;
    time_t	time2  = 0;

    while (MyProcess == True) 
      {
        /* if time out, return */
	if (time(&time2) == (time_t)-1)
	    return -1;

        if (time1 == 0)
	    time1 = time2;
	else if (time2 - time1 >= (time_t)30)
	    return -1;
      }

    if (MyFirst == True)
      {
        MyProcess = True;
        if (_DtLcxOpenAllDbs(&MyDb) == 0 &&
	    _DtXlateGetXlateEnv(MyDb,MyPlatform,&ExecVer,&CompVer) != 0)
	  {
	    _DtLcxCloseDb(&MyDb);
	    MyDb = NULL;
	  }
	MyFirst = False;
        MyProcess = False;
      }

    return (MyDb == NULL ? -1 : 0 );
}

/******************************************************************************
 * Function:    int DtXlateOpToStdLocale(char *operation, char *opLocale,
 *                         		char **ret_stdLocale,
 *                         		char **ret_stdLang,
 *                         		char **ret_stdSet)
 *
 * Parameters:
 *              operation       Operation associated with the locale value
 *              opLocale	An operation-specific locale string
 *              ret_locale      Returns the std locale
 *				Caller must free this string.
 *		ret_stdLang        Returns the std language & territory string.
 *				Caller must free this string.
 *              ret_stdSet         Returns the std code set string.
 *				Caller must free this string.
 *
 * Return Value:
 *
 * Purpose:  Gets the standard locale given an operation and its locale
 *
 *****************************************************************************/
void
DtMail::Session::DtXlateOpToStdLocale (
     char       *operation,
     char       *opLocale,
     char       **ret_stdLocale,
     char       **ret_stdLang,
     char       **ret_stdSet)
{
    int result = OpenLcxDb();

    if (result == 0) {
	(void) _DtLcxXlateOpToStd(
				MyDb, MyPlatform, CompVer,
				operation, opLocale,
				ret_stdLocale, ret_stdLang, ret_stdSet, NULL);
    }

    /* if failed, give default values */
    if (ret_stdLocale != NULL && (result != 0 || *ret_stdLocale == NULL))
    {
        *ret_stdLocale =
	  (char *)malloc(strlen(DfltStdLang)+strlen(DfltStdCharset)+3);
	sprintf(*ret_stdLocale,"%s.%s",DfltStdLang,DfltStdCharset);
    }

    if (ret_stdLang != NULL && (result != 0 || *ret_stdLang == NULL))
	*ret_stdLang = (char *)strdup(DfltStdLang);
    if (ret_stdSet != NULL && (result != 0 || *ret_stdSet == NULL))
	*ret_stdSet = (char *)strdup(DfltStdCharset);
}

/******************************************************************************
 * Function:    int DtXlateStdToOpLocale(char *operation,
 *					char *stdLocale,
 *                                      char *dflt_opLocale,
 *					char **ret_opLocale)
 *
 * Parameters:
 *    operation		operation whose locale value will be retrieved
 *    stdLocale		standard locale value
 *    dflt_opLocale	operation-specific locale-value
 *			This is the default value used in error case
 *    ret_opLocale	operation-specific locale-value placed here
 *			Caller must free this string.
 *
 * Return Value:
 *
 * Purpose: Gets an operation-specific locale string given the standard string
 *
 *****************************************************************************/
void
DtMail::Session::DtXlateStdToOpLocale (
     char       *operation,
     char       *stdLocale,
     char       *dflt_opLocale,
     char       **ret_opLocale)
{
    int result = this->OpenLcxDb();

    if (ret_opLocale)
      *ret_opLocale = NULL;

    if (result == 0) {
      (void) _DtLcxXlateStdToOp(
				MyDb, MyPlatform, CompVer,
			  	operation, stdLocale,
				NULL, NULL, NULL,
				ret_opLocale);
    }

    /* if translation fails, use a default value */
    if (ret_opLocale && (result != 0 || *ret_opLocale == NULL))
    {
       if (dflt_opLocale) *ret_opLocale = (char *)strdup(dflt_opLocale);
       else if (stdLocale) *ret_opLocale = (char *)strdup(stdLocale);
    }
}


/******************************************************************************
 * Function:    int DtXlateStdToOpCodeset (
 *				char *operation,
 *				char *stdCodeset,
 *                              char *dflt_opCodeset,
 *				char **ret_opCodeset)
 *
 * Parameters:
 *    operation		operation whose codeset value will be retrieved
 *    stdCodeset	standard codeset value
 *    dflt_opCodeset	operation-specific codeset-value
 *			This is the default value used in error case
 *    ret_opCodeset	operation-specific codeset-value placed here
 *			Caller must free this string.
 *
 * Return Value:
 *
 * Purpose: Gets an operation-specific locale string given the standard string
 *
 *****************************************************************************/
void
DtMail::Session::DtXlateStdToOpCodeset (
     char       *operation,
     char       *stdCodeset,
     char       *dflt_opCodeset,
     char       **ret_opCodeset)
{
    int result = this->OpenLcxDb();

    if (ret_opCodeset)
      *ret_opCodeset = NULL;

    if (result == 0)
    {
        (void) _DtLcxXlateStdToOp(
				MyDb, MyPlatform, CompVer,
			  	operation,
				NULL, NULL, stdCodeset, NULL,
				ret_opCodeset);
    }

    /* if translation fails, use a default value */
    if (ret_opCodeset && (result != 0 || *ret_opCodeset == NULL))
    {
       if (dflt_opCodeset) *ret_opCodeset = (char *)strdup(dflt_opCodeset);
       else if (stdCodeset) *ret_opCodeset = (char *)strdup(stdCodeset);
    }
}

void
DtMail::Session::DtXlateMimeToIconv(
	const char *mimeId,
	const char *defaultCommonCS,
	const char *defaultIconvCS,
	char **ret_commonCS,
	char **ret_platformIconv)
{
    int exists = -1;

    this->OpenLcxDb();
   
    exists = _DtLcxXlateOpToStd(
				MyDb, MyPlatform, CompVer,
				DtLCX_OPER_MIME, mimeId,
				NULL, NULL, ret_commonCS, NULL);

    if (exists == -1)
    {
	exists = _DtLcxXlateOpToStd(
				MyDb, "CDE", 0,
				DtLCX_OPER_MIME, mimeId,
				NULL, NULL, ret_commonCS, NULL);
        if (exists == -1)
	  *ret_commonCS = (char *)strdup(defaultCommonCS);
    }

    exists = _DtLcxXlateStdToOp(
				MyDb, MyPlatform, CompVer,
				DtLCX_OPER_ICONV3,
	   			NULL, NULL, *ret_commonCS, NULL,
				ret_platformIconv);
    if (exists == -1)
      *ret_platformIconv = (char *)strdup(defaultIconvCS);
}

void
DtMail::Session::DtXlateLocaleToMime(
        const char * locale,
        const char * defaultCommonCS,
        const char * defaultMimeCS,
        char ** ret_mimeCS)
{
   char * commonCS = NULL;

   this->OpenLcxDb();

  /* look for platform-specific locale to CDE translation */
  _DtLcxXlateOpToStd(
		MyDb, MyPlatform, CompVer,
		DtLCX_OPER_SETLOCALE, locale,
		NULL, NULL, &commonCS, NULL);
  if (!commonCS)
      commonCS = (char *)strdup(defaultCommonCS);

  /* look for platform-specific MIME types; by default, there is none */
  _DtLcxXlateStdToOp(
		MyDb, MyPlatform, CompVer,
		DtLCX_OPER_MIME,
          	NULL, NULL, commonCS, NULL,
		ret_mimeCS);
  if (!(*ret_mimeCS))
  {
     _DtLcxXlateStdToOp(
		MyDb, "CDE", 0,
		DtLCX_OPER_MIME,
          	NULL, NULL, commonCS, NULL,
		ret_mimeCS);
     if (!(*ret_mimeCS))
        *ret_mimeCS = (char *)strdup(defaultMimeCS);
  }

  if (commonCS)
      free(commonCS);
}

// Return iconv name of the given codeset.
// If iconv name does not exist, return NULL.
char *
DtMail::Session::csToConvName(char *cs)
{
   int exists = -1;
   char *commonCS = NULL;
   char *convName = NULL;
   char *ret_target = NULL;
 
   this->OpenLcxDb();
 
   // Convert charset to upper case first because charset table is
   // case sensitive.
   if (cs)
   {
       int len_cs = strlen(cs);
       for ( int num_cs = 0;  num_cs < len_cs;  num_cs++ )
         *(cs+num_cs) = toupper(*(cs+num_cs));
   }
   exists = _DtLcxXlateOpToStd(
			MyDb, MyPlatform, CompVer,
			DtLCX_OPER_MIME, cs,
			NULL, NULL, &commonCS, NULL);
   if (exists == -1) {
      exists = _DtLcxXlateOpToStd(
			MyDb, "CDE", 0,
			DtLCX_OPER_MIME, cs,
			NULL, NULL, &commonCS, NULL);
      if  (exists == -1)
        return NULL;
   }
 
  DtXlateStdToOpCodeset(DtLCX_OPER_INTERCHANGE_CODESET,
      commonCS,
      NULL,
      &ret_target);
   DtXlateStdToOpCodeset(DtLCX_OPER_ICONV3,
      ret_target,
      NULL,
      &convName);

   if ( ret_target )
     free( ret_target );
   if ( commonCS )
     free( commonCS );
 
   // Workaround for libDtHelp
   // Case of no iconv name for a particular locale, eg. C,
   // check for empty string.
   if ( convName != NULL )
   {
      if ( strlen(convName) > 0 )
        return convName;
      else
        free( convName );
   }
   return NULL;
}

// Return current locale's iconv name.
char *
DtMail::Session::locToConvName()
{
   char *ret_locale = NULL;
   char *ret_lang = NULL;
   char *ret_codeset = NULL;
 
   DtXlateOpToStdLocale(DtLCX_OPER_SETLOCALE,
      setlocale(LC_CTYPE, NULL),
      &ret_locale,
      &ret_lang,
      &ret_codeset);

   if (ret_codeset) {
       free(ret_codeset);
       ret_codeset = NULL;
   }
   
   if (ret_lang) {
       free(ret_lang);
       ret_lang = NULL;
   }
   
   DtXlateStdToOpLocale(DtLCX_OPER_ICONV3,
      ret_locale,
      NULL,
      &ret_codeset);

   if (ret_locale) 
     free(ret_locale);

   // Workaround for libDtHelp
   // Case of no iconv name for a particular locale, eg. C, 
   // check for empty string.
   if ( ret_codeset != NULL )
   {
      if ( strlen(ret_codeset) > 0 )
        return ret_codeset;
      else
        free(ret_codeset);
   }
   return NULL;
}

// Return target codeset's iconv name.
char *
DtMail::Session::targetConvName()
{
   char *ret_locale = NULL;
   char *ret_lang = NULL;
   char *ret_codeset = NULL;
   char *ret_target = NULL;
   char *ret_convName = NULL;
 
   DtXlateOpToStdLocale(DtLCX_OPER_SETLOCALE,
      setlocale(LC_CTYPE, NULL),
      &ret_locale,
      &ret_lang,
      &ret_codeset);
   DtXlateStdToOpLocale(DtLCX_OPER_INTERCHANGE_CODESET,
      ret_locale,
      NULL,
      &ret_target);
   // Or do I call csToConvName() here??
   DtXlateStdToOpCodeset(DtLCX_OPER_ICONV3,
      ret_target,
      NULL,
      &ret_convName);
 
   if (ret_locale)
     free(ret_locale);
   if (ret_lang)
     free(ret_lang);
   if (ret_codeset)
     free(ret_codeset);
   if (ret_target)
     free(ret_target);

   // Workaround for libDtHelp
   // Case of no iconv name for a particular locale, eg. C,
   // check for empty string.
   if ( ret_convName != NULL )
   {
      if ( strlen(ret_convName) > 0 )
        return ret_convName;
      else
        free(ret_convName);
   }
   return NULL;
}

// Return target codeset's MIME (tag) name.
char *
DtMail::Session::targetTagName()
{
   char *ret_locale = NULL;
   char *ret_lang = NULL;
   char *ret_codeset = NULL;
   char *ret_target = NULL;

   DtXlateOpToStdLocale(DtLCX_OPER_SETLOCALE,
	  setlocale(LC_CTYPE, NULL),
	  &ret_locale,
	  &ret_lang,
	  NULL);
   DtXlateStdToOpLocale(DtLCX_OPER_INTERCHANGE_CODESET,
	  ret_locale,
	  NULL,
	  &ret_target);
   DtXlateStdToOpCodeset(DtLCX_OPER_MIME,
	  ret_target,
	  NULL,
	  &ret_codeset);

   if (ret_locale)
     free(ret_locale);
   if (ret_lang)
     free(ret_lang);
   if (ret_target)
     free(ret_target);

   return ret_codeset;
}

// Given an extension to the interchange codeset name.
// Return target codeset's MIME (tag) name.
// The extension is for Sun V3 backward compatibility so that
// reverse mapping of out-going charset tag is the same as
// OpenWindows Mailtool.
char *
DtMail::Session::targetTagName(char *special)
{
   char *ret_locale = NULL;
   char *ret_lang = NULL;
   char *ret_codeset = NULL;
   char *ret_target = NULL;
 
   DtXlateOpToStdLocale(DtLCX_OPER_SETLOCALE,
      setlocale(LC_CTYPE, NULL),
      &ret_locale,
      &ret_lang,
      &ret_codeset);
 
   // Allocate two more bytes for "." and null terminator.
   char *special_locale;

   special_locale = (char *)calloc(
				strlen(ret_locale) + strlen(special) + 2,
        			sizeof(char));
   sprintf(special_locale, "%s%s%s", ret_locale, ".", special);
 
   DtXlateStdToOpLocale(DtLCX_OPER_INTERCHANGE_CODESET,
      special_locale,
      NULL,
      &ret_target);
   DtXlateStdToOpCodeset(DtLCX_OPER_MIME,
      ret_target,
      NULL,
      &ret_codeset);
 
   if (ret_locale)
     free(ret_locale);
   if (ret_lang)
     free(ret_lang);
   if (ret_target)
     free(ret_target);

   return ret_codeset;
}

// Given a message text and codesets
// Convert message text from one codeset to another
// Return 1 if conversion is successful else return 0.

int
DtMail::Session::csConvert(char **bp, unsigned long &bp_len, int free_bp,
char *from_cs, char *to_cs)
{
   DtMailEnv error;
   iconv_t cd;
   size_t ileft = (size_t) bp_len, oleft = (size_t) bp_len, ret = 0;
#if defined(_AIX) || defined(sun) || defined(__FreeBSD__) || defined(__NetBSD__)
   const char *ip = (const char *) *bp;
#else
   char *ip = *bp;
#endif
   char *op = NULL;
   char *op_start = NULL;
   int mb_ret = 0;
   size_t delta;

   if ( *bp == NULL  ||  **bp == '\0'  ||  bp_len <= 0 )
	  return 0;
   if ( to_cs == NULL  ||  from_cs == NULL )
	  return 0;
   if ( (cd = iconv_open(to_cs, from_cs)) == (iconv_t) -1 ) {
	  switch (errno) {
		case EINVAL:
		  error.logError(DTM_FALSE,
		  "DtMail: Conversion from %s to %s is not supported.\n",
		  from_cs, to_cs);
          break;
	  }   // end of switch statement
      return 0;
   }
   // Caller will set _must_free_body to DTM_TRUE if this routine
   // succeeds.  Then this space will be freed appropriately.
   // Add 1 to buffer size for null terminator.
   op_start = op = (char *)calloc((unsigned int) bp_len + 1, sizeof(char));

   // When ileft finally reaches 0, the conversion still might not be
   // complete.  Here's why we also need to check for E2BIG:  Let's
   // say we're converting from eucJP to ISO-2022-JP, and there's just
   // enough room in the output buffer for the last input character,
   // but not enough room for the trailing "ESC ( B" (for switching
   // back to ASCII).  In that case, iconv() will convert the last
   // input character, decrement ileft to zero, and then set errno to
   // E2BIG to tell us that it still needs more room for the "ESC ( B".
   errno = 0;
   while ( ileft > 0 || errno == E2BIG ) {
      errno = 0;
      if ((ret = iconv(cd, &ip, &ileft, &op, &oleft)) == (size_t) -1) {
	     switch (errno) {
		   case E2BIG:   // increase output buffer size
			 delta = ileft ? ileft : 3;
			 bp_len += delta;
			 op_start = (char *)realloc(
						(char *)op_start,
						(unsigned int) bp_len + 1); 
			 op = op_start + bp_len - delta - oleft;
			 oleft += delta;
			 // realloc does not clear out unused space.
			 // Therefore, garbage shows up in output buffer.
			 memset(op, 0, oleft + 1);
			 break;
		   case EILSEQ:  // input byte does not belong to input codeset
		   case EINVAL:  // invalid input
             		 mb_ret = mblen(ip, MB_LEN_MAX);
             		 if ( (mb_ret > 0) && (oleft >= mb_ret) ) {
             		   strncat(op_start, ip, mb_ret);
             		   ip += mb_ret;
             		   op += mb_ret;
             		   oleft -= mb_ret;
             		   ileft -= mb_ret;
			   mb_ret = 0;
             		 } else {
			   //  mb_ret is either 0 or -1 at this point,
			   //  then skip one byte
			   //  and try conversion again.
			   ip++;
			   ileft--;
			 }
			 break;
		   case EBADF:   // bad conversion descriptor
			 break;
	     }   // end of switch statement
	  }
   }  // end of while loop
   iconv_close(cd);

   // Is this necessary??  Is _body_decode_len == strlen(_body)??
   // Or can _body_decode_len contain spaces??

   // Check to see if a body had been allocated by prior decoding.
   if (free_bp) {
      free(*bp);
   }
   *bp = op_start;
   bp_len = strlen(*bp); 

   return 1;
}

// End of For CHARSET
