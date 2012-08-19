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
 *	$TOG: RFCBodyPart.C /main/16 1998/04/06 13:27:40 mgreess $
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

#ifndef I_HAVE_NO_IDENT
#endif

#if defined (__osf__) && OSMAJORVERSION < 4

#include <iconv.h>

// Workaround for CDExc19308
//
// This ifdef was added as a workaround for the
// bug in the OSF1 V3.2 148 /usr/include/sys/localedef.h
// header file, and should be removed as soon as the bug is 
// fixed.
#include <locale.h>
#include <time.h>
#include <DtHelp/LocaleXlate.h>
#include <errno.h>
#include <stdlib.h>
#include <limits.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <Dt/Dts.h>
#include <DtMail/DtMail.hh>
#include "RFCImpl.hh"
#include <DtMail/Threads.hh>

#ifndef True
#define True 1
#endif
#ifndef False
#define False 0
#endif

#else

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <Dt/Dts.h>

#include <DtMail/DtMail.hh>
#include "RFCImpl.hh"
#include <DtMail/Threads.hh>

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
   // Iconv not defined for linux.  Use the EUSCompat stubs instead. */
#  include <iconv.h>
#else
#  include <EUSCompat.h>
#endif


#if defined(SunOS) && (SunOS < 55)
}
#endif

// End of For CHARSET
#endif // defined (__osf__) && OSMAJORVERSION < 4

RFCBodyPart::RFCBodyPart(DtMailEnv & error,
			 DtMail::Message * parent,
			 const char * start,
			 const int len,
			 RFCEnvelope * body_env)
: DtMail::BodyPart(error, parent)
{
    _body_lock = MutexInit();

    _body_start = _body_text = start;
    _body_len = len > 0 ? len : 0;

    // If we have no body to start, then we will want to create
    // a body envelope. We do this to make sure we can always
    // set headers for the body part.
    //
    if (_body_start == NULL) {
	_body_env = new RFCEnvelope(error, parent, NULL, 0);
	_my_env = DTM_TRUE;
    }
    else {
	_body_env = body_env;
	_my_env = _body_env ? DTM_FALSE : DTM_TRUE;
    }

    _body = NULL;
    _body_decoded_len = 0;
    _body_type = NULL;
    _must_free_body = DTM_FALSE;
}

RFCBodyPart::~RFCBodyPart(void)
{
    if (_my_env == DTM_TRUE) {
	delete _body_env;
    }

    if (_must_free_body) {
	free(_body);
    }

    if (_body_type) {
	free(_body_type);
    }
}

void
RFCBodyPart::getContents(DtMailEnv & error,
			  const void ** contents,
			  unsigned long * length,
			  char ** type,
			  char ** name,
			  int * mode,
			  char ** description)
{
  // First things first, for each possible return, zero out the
  // pointer so that if the caller does not check for errors it
  // will be caught by a null dereference
  //
  if (name)
    *name = (char *)0;
  if (description)
    *description = (char *)0;
  if (type)
    *type = (char *)0;
  if (contents)
    *contents = (void *)0;
  
    // The caller can ask for a hodge podge of information.
    // The only real rule here is that you can not ask for
    // the contents without requesting the length.

  if (contents && !length) {
	error.setError(DTME_OperationInvalid);
	return;
    }

// No need to clear the error here because it should be clear already.
//    error.clear();

    // MIME currently doesn't have body part names and RFC
    // doesn't let us at arbitrary message headers. Name
    // therefore can not be retrieved.
    //
    if (name) {
  	*name = getName(error);
	if (error.isSet()) {
	    // don't care about the error condition returned by getName()
	    // because it returns a valid string no matter what error
	    // condition occurs.
	    error.clear();
	}
    }

    // Ditto for mode.
    //
    if (mode) {
	*mode = 0;
    }

    if (description) {
	// getDescription should not be passed a DtMailEnv object.
	// Neither implementation of getDescription (MIME or V3)
	// sets the error before returning.  Besides being unneccessary,
	// it requires the caller of getDescription to check the error
	// status upon its return.
	//
	*description = getDescription(error);
	if (error.isSet()) {
	    // don't care about the error condition returned by getDescription
	    // because it returns a valid string no matter what error
	    // condition occurs.
	    error.clear();
	}
    }

    // Types for DtMail are in the Dt name space. RFC returns
    // mime types. We will have to convert from the RFC name
    // space to the Dt name space.
    //
    if (type) {
	if (!_body_type) {
	    getDtType(error);
	    // Do we want to propogate this error back up to the 
	    // function that called us?  If not, we need to call 
	    // error.clear() before returning.
	    if (error.isSet()) {
		return;
	    }
	}

	*type = strdup(_body_type);
    }

    if (length) {
	*length = getLength(error);
	if (error.isSet()) {
	    // propogate the error back up to the caller
	    return;
	}
    }

    if (contents) {
	*contents = getBody(error);
	if (error.isSet()) {
	    // propogate the error back up to the caller
	    return;
	}
    }
}

void
RFCBodyPart::setContents(DtMailEnv & error,
			  const void * contents,
			  const unsigned long length,
			  const char * type,
			  const char * name,
			  const int, // mode,
			  const char *) // description)
{
    error.clear();

    MutexLock lock_scope(_body_lock);

    if (name) {
	setName(error, name);
    }

    if (contents && !length) {
	error.setError(DTME_OperationInvalid);
	return;
    }
    if (!contents && !length) {
	if (_body && _must_free_body) {
		free(_body);
        	_body = NULL;
		_must_free_body = DTM_FALSE;
	}
        _body_decoded_len = _body_len = 0;
        if (_body_type) {
        	free(_body_type);
        	_body_type = NULL;
	}
//	_must_free_body = DTM_FALSE;
	return;
    }

    if (contents) {
	if (_body && _must_free_body) {
	    free(_body);
	    _body = NULL;
	}

	_body = (char *)malloc((int)length);
	memcpy(_body, contents, (int)length);
	_body_decoded_len = _body_len = (int)length;
	_must_free_body = DTM_TRUE;

	// Reset the type. We don't know what it is!
	if (_body_type) {
	    free(_body_type);
	    _body_type = NULL;
	}
    }

    if (type) {
	if (_body_type) 
	    free(_body_type);
	_body_type = strdup(type);
    }
}

void
RFCBodyPart::lockContents(DtMailEnv & error, const DtMailLock)
{
    error.clear();
}

void
RFCBodyPart::unlockContents(DtMailEnv & error)
{
    error.clear();
}

void
RFCBodyPart::getHeader(DtMailEnv & error,
                      const char * name,
                      const DtMailBoolean abstract,
                      DtMailValueSeq & value)
{
    error.clear();

    // If this is not our envelope, then we will not set the flag.
    //
    if (_my_env == DTM_FALSE) {
       error.setError(DTME_NotSupported);
       return;
    }

    if (_body_env == (RFCEnvelope *)NULL) {
       error.setError(DTME_NoObjectValue);
       return;
    }

    _body_env->getHeader(error, name, abstract, value);
}

void
RFCBodyPart::setFlag(DtMailEnv & error,
		     DtMailBodyPartState state)
{
    error.clear();

    // If this is not our envelope, then we will not set the flag.
    //
    if (_my_env == DTM_FALSE) {
	error.setError(DTME_NotSupported);
	return;
    }

    DtMailEnv my_error;
    time_t now;
    char str_time[40];

    switch (state) {
      case DtMailBodyPartDeletePending:
	now = time(NULL);
	sprintf(str_time, "%08lX", (long)now);
	_body_env->setHeader(my_error, RFCDeleteHeader, DTM_TRUE, str_time);
	break;

      default:
	error.setError(DTME_OperationInvalid);
    }
}

void
RFCBodyPart::resetFlag(DtMailEnv & error,
		       DtMailBodyPartState state)
{
    error.clear();

    // If this is not our envelope, then we will not set the flag.
    //
    if (_my_env == DTM_FALSE) {
	error.setError(DTME_NotSupported);
	return;
    }

    DtMailEnv my_error;

    switch (state) {
      case DtMailBodyPartDeletePending:
	_body_env->removeHeader(my_error, RFCDeleteHeader);
	break;

      default:
	error.setError(DTME_OperationInvalid);
    }
}

DtMailBoolean
RFCBodyPart::flagIsSet(DtMailEnv & error,
		       DtMailBodyPartState state)
{
    error.clear();

    // If this is not our envelope, then we will not set the flag.
    //
    if (_my_env == DTM_FALSE) {
	error.setError(DTME_NotSupported);
	return(DTM_FALSE);
    }

    DtMailEnv my_error;
    DtMailValueSeq value;
    DtMailBoolean answer;

    switch (state) {
      case DtMailBodyPartDeletePending:
	_body_env->getHeader(my_error, RFCDeleteHeader, DTM_FALSE, value);
	if (my_error.isNotSet()) {
	    answer = DTM_TRUE;
	}
	else {
	    answer = DTM_FALSE;
	}
	break;

      default:
	error.setError(DTME_OperationInvalid);
    }

    return(answer);
}

time_t
RFCBodyPart::getDeleteTime(DtMailEnv & error)
{
    time_t	delete_time = 0;

    DtMailValueSeq value;
    _body_env->getHeader(error, RFCDeleteHeader, DTM_FALSE, value);
    if (error.isNotSet()) {
	delete_time = (time_t) strtol(*(value[0]), NULL, 16);
    }

    error.clear();

    return(delete_time);
}


void
RFCBodyPart::adjustBodyPartsLocation(char * start)
{
    MutexLock lock_scope(_body_lock);

    _body_text = (_body_text - _body_start) + start;
    _body_start = start;

    if (_must_free_body == DTM_FALSE) {
	//_body = (char *)_body_text;
	_body = NULL;
    }

    if (_body_env && _my_env == DTM_TRUE) {
	// CMVC bug 2807
	// start points at the body part seperator.  Need to 
	// Skip seperator.  Put in a sanity check until we know
	// this is the right fix
	if (*start != '-' && *(start + 1) != '-') {
	  fprintf(
		stderr,
		"RFCBodyPart::adjustBodyPartLocation(%.20s): Not a seperator\n",
		start);
	} else {
		while (*start != '\n')
			start++;
		start++;
	}
	// End Of fix for 2807
	_body_env->adjustHeaderLocation(start, (int)(_body_text-_body_start));
    }
}

DtMailBoolean
RFCBodyPart::isTerm(const char * start)
{
    if (*start == '\n' || (*start == '\r' && *(start + 1) == '\n')) {
	return(DTM_TRUE);
    }
    else {
	return(DTM_FALSE);
    }
}

const void *
RFCBodyPart::getBody(DtMailEnv & error)
{
    error.clear();
    if (!_body) {
	loadBody(error);
	if (error.isSet()) {
	    return(NULL);
	}
    }

    return(_body);
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
RFCBodyPart::OpenLcxDb (void)
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
 *                         		 char **ret_stdLocale,
 *					 char **ret_stdLang, char **ret_stdSet)
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
RFCBodyPart::DtXlateOpToStdLocale (
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
 * Function:    int DtXlateStdToOpLocale ( char *operation, char *stdLocale,
 *                                    char *dflt_opLocale, char **ret_opLocale)
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
RFCBodyPart::DtXlateStdToOpLocale (
     char       *operation,
     char       *stdLocale,
     char       *dflt_opLocale,
     char       **ret_opLocale)
{
    int result = this->OpenLcxDb();

    if (ret_opLocale)
      *ret_opLocale = NULL;

    if (result == 0)
    {
        (void) _DtLcxXlateStdToOp(
				MyDb, MyPlatform, CompVer,
			  	operation,
				stdLocale, NULL, NULL, NULL,
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
RFCBodyPart::DtXlateStdToOpCodeset (
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
RFCBodyPart::DtXlateMimeToIconv(
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
RFCBodyPart::DtXlateLocaleToMime(
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
RFCBodyPart::csToConvName(char *cs)
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
      for (int num_cs = 0;  num_cs < len_cs;  num_cs++)
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
RFCBodyPart::locToConvName()
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
   if ( ret_codeset != NULL ) {
      if (strlen(ret_codeset) > 0)
        return ret_codeset;
      else
        free(ret_codeset);
   } 
   return NULL;
}

// Return target codeset's iconv name.
char *
RFCBodyPart::targetConvName()
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
      if (strlen(ret_convName) > 0)
        return ret_convName;
      else
        free(ret_convName);
   }
   return NULL;
}

// Return target codeset's MIME (tag) name.
char *
RFCBodyPart::targetTagName()
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

// Given a message text and codesets
// Convert message text from one codeset to another
// Return 1 if conversion is successful else return 0.
int
RFCBodyPart::csConvert(char **bp, unsigned long &bp_len, int free_bp,
char *from_cs, char *to_cs)
{
   DtMailEnv error;
   iconv_t cd;
   size_t ileft = (size_t) bp_len, oleft = (size_t) bp_len, ret = 0;
#if defined(_aix) || defined(sun) || defined(__FreeBSD__)
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
      ret = iconv(cd, &ip, &ileft, &op, &oleft);
      if ( ret == (size_t) -1 ) {
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
