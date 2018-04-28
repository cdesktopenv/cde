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
/* $XConsortium: HelpXlate.c /main/1 1996/08/22 09:16:03 rswiston $ */
/****************************************************************************
$FILEBEG$:    HelpXlate.c
$PROJECT$:    Cde 1.0
$COMPONENT$:  DtXlate service
$1LINER$:     Implements a translation service using tables and regex search
$COPYRIGHT$:
 (c) Copyright 1993, 1994 Hewlett-Packard Company
 (c) Copyright 1993, 1994 International Business Machines Corp.
 (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 (c) Copyright 1993, 1994 Unix System Labs, Inc., a subsidiary of Novell, Inc.
$END$
 ****************************************************************************
 ************************************<+>*************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>        /* MAXPATHLEN */
#include <time.h>

/* for Xrm */
#include <X11/Intrinsic.h>

/*=================================================================
$SHAREDBEG$:  This header appears in all appropriate DtXlate topics
=======================================================$SKIP$======*/
/*$INCLUDE$*/ 
#include "HelpXlate.h"
#include "Lock.h"
/*$END$*/

static char       MyPlatform[_DtPLATFORM_MAX_LEN+1];
static _DtXlateDb MyDb = NULL;
static int        ExecVer;
static int        CompVer;

static const char *DfltStdCharset = "ISO-8859-1";
static const char *DfltStdLang = "C";

/*========================================================*/
/*================== Private routines ====================*/
/*========================================================*/
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
    time_t      time1  = 0;
    time_t      time2  = 0;
    static short	  MyProcess = False;
    static short	  MyFirst   = True;

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

    _DtHelpProcessLock();
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
    _DtHelpProcessUnlock();

    return (MyDb == NULL ? -1 : 0 );
}

/******************************************************************************
 * Function:    int _DtHelpCeXlateStdToOpLocale ( char *operation, char *stdLoc
ale,
 *                                      char *dflt_opLocale, char **ret_opLocale
)
 *
 * Parameters:
 *    operation         operation whose locale value will be retrieved
 *    stdLocale         standard locale value
 *    dflt_opLocale     operation-specific locale-value
 *                      This is the default value used in error case
 *    ret_opLocale      operation-specific locale-value placed here
 *                      Caller must free this string.
 *
 * Return Value:
 *
 * Purpose: Gets an operation-specific locale string given the standard string
 *
 *****************************************************************************/
void
_DtHelpCeXlateStdToOpLocale (
     char       *operation,
     char       *stdLocale,
     char       *dflt_opLocale,
     char       **ret_opLocale)
{
    int result = OpenLcxDb();

    _DtHelpProcessLock();
    if (result == 0)
      {
        (void) _DtLcxXlateStdToOp(MyDb, MyPlatform, CompVer,
                        operation, stdLocale, NULL, NULL, NULL, ret_opLocale);
      }
    _DtHelpProcessUnlock();

    /* if translation fails, use a default value */
    if (ret_opLocale && (result != 0 || *ret_opLocale == NULL))
      {
        if (dflt_opLocale) *ret_opLocale = strdup(dflt_opLocale);
        else if (stdLocale) *ret_opLocale = strdup(stdLocale);
      }
}


/******************************************************************************
 * Function:    int _DtHelpCeXlateOpToStdLocale (char *operation, char *opLocale
,
 *                         char **ret_stdLocale, char **ret_stdLang, char **ret_
stdSet)
 *
 * Parameters:
 *              operation       Operation associated with the locale value
 *              opLocale        An operation-specific locale string
 *              ret_locale      Returns the std locale
 *                              Caller must free this string.
 *              ret_stdLang        Returns the std language & territory string.
 *                              Caller must free this string.
 *              ret_stdSet         Returns the std code set string.
 *                              Caller must free this string.
 *
 * Return Value:
 *
 * Purpose:  Gets the standard locale given an operation and its locale
 *
 *****************************************************************************/
void
_DtHelpCeXlateOpToStdLocale (
     char       *operation,
     char       *opLocale,
     char       **ret_stdLocale,
     char       **ret_stdLang,
     char       **ret_stdSet)
{
    int result = OpenLcxDb();

    _DtHelpProcessLock();
    if (result == 0)
      {
        (void) _DtLcxXlateOpToStd(MyDb, MyPlatform, CompVer,
                                operation,opLocale,
                                ret_stdLocale, ret_stdLang, ret_stdSet, NULL);
      }
    _DtHelpProcessUnlock();

    /* if failed, give default values */
    if (ret_stdLocale != NULL && (result != 0 || *ret_stdLocale == NULL))
      {
        *ret_stdLocale = malloc(
                                strlen(DfltStdLang)+strlen(DfltStdCharset)+3);
        sprintf(*ret_stdLocale,"%s.%s",DfltStdLang,DfltStdCharset);
      }
    if (ret_stdLang != NULL && (result != 0 || *ret_stdLang == NULL))
        *ret_stdLang = strdup(DfltStdLang);
    if (ret_stdSet != NULL && (result != 0 || *ret_stdSet == NULL))
        *ret_stdSet = strdup(DfltStdCharset);
}

/******************************************************************************
 * Function:    int _DtHelpCeGetMbLen (char *lang, char *char_set)
 *
 * Parameters:
 *              lang            Represents the language. A NULL value
 *                              defaults to "C".
 *              char_set        Represents the character set. A NULL
 *                              value value defaults to "ISO-8859-1"
 *
 * Return Value:        Returns the MB_CUR_MAX for the combination
 *                      lang.charset.
 *
 * errno Values:
 *
 * Purpose: To determine the maximum number of bytes required to display
 *          a character if/when the environment is set to 'lang.charset'
 *
 *****************************************************************************/
int
_DtHelpCeGetMbLen (
     char       *lang,
     char       *char_set)
{
    int           retLen = 1;

    if (lang == NULL)
        lang = (char *)DfltStdLang;

    if (char_set == NULL)
        char_set = (char *)DfltStdCharset;

    _DtHelpProcessLock();
    if (OpenLcxDb() == 0)
      {
        /* if translation is present, lang.charset are a multibyte locale */
        if (_DtLcxXlateStdToOp(MyDb, MyPlatform, CompVer, DtLCX_OPER_MULTIBYTE,
                                NULL, lang, char_set, NULL, NULL) == 0)
            retLen = MB_CUR_MAX;
      }
    _DtHelpProcessUnlock();

    return retLen;
}

