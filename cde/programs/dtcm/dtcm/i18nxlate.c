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
/* $XConsortium: i18nxlate.c /main/5 1995/11/09 13:00:30 rswiston $ */
/*
 *  (c) Copyright 1993, 1994 Hewlett-Packard Company
 *  (c) Copyright 1993, 1994 International Business Machines Corp.
 *  (c) Copyright 1993, 1994 Novell, Inc.
 *  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <time.h>

#include <LocaleXlate.h>	/* under DtHelp, for Operation constants */

#ifndef True
#define True 1
#endif
#ifndef False
#define False 0
#endif


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

static const char *DfltStdCharset = "ISO-8859-1";
static const char *DfltStdLang = "C";

static char       MyPlatform[_DtPLATFORM_MAX_LEN+1];
static _DtXlateDb MyDb = NULL;
char       MyProcess = False;
char       MyFirst   = True;
int        ExecVer;
int        CompVer;


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
static int
OpenLcxDb (void)
{
    time_t	time1  = 0;
    time_t	time2  = 0;

    /*
     * wait 30 sec. until another thread or enter is done modifying the table
     */
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
 * Function:    int DtI18NXlateOpToStdLocale (char *operation, char *opLocale,
 *                         char **ret_stdLocale, char **ret_stdLang, char **ret_stdSet)
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
_DtI18NXlateOpToStdLocale (
     char       *operation,
     char       *opLocale,
     char       **ret_stdLocale,
     char       **ret_stdLang,
     char       **ret_stdSet)
{
    OpenLcxDb();

    if (MyDb != NULL)
      {
	(void) _DtLcxXlateOpToStd(MyDb, MyPlatform, CompVer,
				operation,opLocale,
				ret_stdLocale, ret_stdLang, ret_stdSet, NULL);
      }

    /* if failed, give default values */
    if (ret_stdLocale != NULL && *ret_stdLocale == NULL)
    {
        *ret_stdLocale = (char *)malloc(strlen(DfltStdLang)+strlen(DfltStdCharset)+3);
	sprintf(*ret_stdLocale,"%s.%s",DfltStdLang,DfltStdCharset);
    }
    if (ret_stdLang != NULL && *ret_stdLang == NULL)
	*ret_stdLang = (char *)strdup(DfltStdLang);
    if (ret_stdSet != NULL && *ret_stdSet == NULL)
	*ret_stdSet = (char *)strdup(DfltStdCharset);
}

/******************************************************************************
 * Function:    int DtI18NXlateStdToOpLocale ( char *operation, char *stdLocale,
 *                                      char *dflt_opLocale, char **ret_opLocale)
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
_DtI18NXlateStdToOpLocale (
     char       *operation,
     char       *stdLocale,
     char       *dflt_opLocale,
     char       **ret_opLocale)
{
    OpenLcxDb();

    if (MyDb != NULL)
    {
	(void) _DtLcxXlateStdToOp(MyDb, MyPlatform, CompVer,
			operation, stdLocale, NULL, NULL, NULL, ret_opLocale);
    }

    /* if translation fails, use a default value */
    if (ret_opLocale && *ret_opLocale == NULL)
    {
       if (dflt_opLocale) *ret_opLocale = (char *)strdup(dflt_opLocale);
       else if (stdLocale) *ret_opLocale = (char *)strdup(stdLocale);
    }
}

