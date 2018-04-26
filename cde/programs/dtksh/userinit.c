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
/* $TOG: userinit.c /main/6 1998/04/20 12:55:18 mgreess $ */
#include "defs.h"
#include "name.h"
#include "variables.h"
#include <Dt/DtNlUtils.h>
#include <Dt/EnvControlP.h>
#include <stdio.h>
#include <nl_types.h>
#include <X11/X.h>
#include <X11/Intrinsic.h>
#include <X11/IntrinsicP.h>
#include <X11/CoreP.h>
#include <X11/StringDefs.h>
#include <Xm/XmStrDefs.h>
#include <setjmp.h>
#include <string.h>
#include <ctype.h>
#include <Xm/Xm.h>
#include <Xm/Protocols.h>
#include "hash.h"
#include "stdio.h"
#define NO_AST
#include "dtksh.h"
#undef NO_AST
#include "xmksh.h"
#include "dtkcmds.h"
#include "xmcvt.h"
#include "widget.h"
#include "extra.h"
#include "xmwidgets.h"
#include "msgs.h"
#include <locale.h>


/*
 * LocaleChanged is defined in ksh93/src/cmd/ksh93/sh/init.c
 */

extern void LocaleChanged (
		Namval_t * np,
		const char * val,
		int flags,
		Namfun_t * fp );


static Namdisc_t localeDisc = { 0, LocaleChanged, NULL, NULL, NULL, NULL, NULL, NULL };
static Namfun_t localeFun = {NULL, NULL };

extern char *savedNlsPath; /* in ./ksh93/src/cmd/ksh93/sh/init.c */


void
SyncEnv(
    char *name)
{
  char *value, *buf;

  value = getenv(name);
  if(value != (char *)NULL)
  {
    buf = malloc(strlen(name) + strlen(value) + 2);
    strcpy(buf, name);
    strcat(buf, "=");
    strcat(buf, value);
    ksh_putenv(buf);
    free(buf);  /* I hope it's legal to free this! */
  }
}

/*
 *  This is a hook for an additional initialization routine
 *  A function of this name is called in main after sh_init().
 */

void
sh_userinit( void )

{
   int * lockedFds;

   lockedFds = LockKshFileDescriptors();
   (void) XtSetLanguageProc((XtAppContext)NULL, (XtLanguageProc)NULL,
				(XtPointer)NULL);
   setlocale(LC_ALL, "");
   DtNlInitialize();
   _DtEnvControl(DT_ENV_SET);
   localeFun.disc = &localeDisc;
   nv_stack(LANGNOD, &localeFun);
   UnlockKshFileDescriptors(lockedFds);

   /*
    * Save the current setting of NLSPATH.  The user/script may want to
    * set its own NLSPATH to access its message catalog, so we need to
    * remember where to find our own catalog(s).  This saved path is used
    * in ksh93/src/cmd/ksh93/sh/init.c: _DtGetMessage().  We don't mess
    * with the user/script's setting of LANG as we want to track changes
    * in LANG.
    */
   savedNlsPath = strdup(getenv("NLSPATH"));

   /*
    * Sync the libc environment (set up by DtEnvControl) with our internal
    * hash table environment.
    */
  SyncEnv("NLSPATH");
  SyncEnv("LANG");
}

/*
 * The following routines are used to query a CDE database to determine
 * if the current character encoding requires special care in the ksh
 * parser.  They are used in updateShSpecialParse().  These are copied
 * from the DtHelp code.
 */
#include <XlationSvc.h>
#include <LocaleXlate.h>

static const char *DfltStdCharset = "ISO-8859-1";
static const char *DfltStdLang = "C";

static char       MyPlatform[_DtPLATFORM_MAX_LEN+1];
static int        CompVer;

/******************************************************************************
 * Function:    static _DtXlateDb OpenLcxDb ()
 *
 * Parameters:   none
 *
 * Return Value:  NULL: error, else a _DtXlateDb
 *
 * errno Values:
 *
 * Purpose: Opens the Ce-private Lcx database
 *
 *****************************************************************************/
static _DtXlateDb
OpenLcxDb (void)
{
    static _DtXlateDb MyDb;
    static Boolean  MyFirst   = True;
    static Boolean  MyProcess = False;
    static int        ExecVer;
    time_t      time1  = 0;
    time_t      time2  = 0;

    /*
     * wait up to 30 sec. until another thread or enter is done 
     * modifying the table.
     */
    while (MyProcess == True)
      {
        /* if time out, return */
        if (time(&time2) == (time_t)-1)
            return (_DtXlateDb)NULL;

        if (time1 == 0)
            time1 = time2;
        else if (time2 - time1 >= (time_t)30)
            return (_DtXlateDb)NULL;
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

    return MyDb;
}

/******************************************************************************
 * Function:    static void XlateOpToStdLocale(char *operation, char *opLocale,
 *                                             char **ret_stdLocale, 
 *                                             char **ret_stdLang, 
 *                                             char **ret_stdSet)
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
static void
XlateOpToStdLocale (
     char       *operation,
     char       *opLocale,
     char       **ret_stdLocale,
     char       **ret_stdLang,
     char       **ret_stdSet)
{
    _DtXlateDb MyDb;

    MyDb = OpenLcxDb();

    if (MyDb != NULL)
      {
        (void) _DtLcxXlateOpToStd(MyDb, MyPlatform, CompVer,
                                operation,opLocale,
                                ret_stdLocale, ret_stdLang, ret_stdSet, NULL);
      }

    /* if failed, give default values */
    if (ret_stdLocale != NULL && *ret_stdLocale == NULL)
    {
        *ret_stdLocale = malloc(strlen(DfltStdLang)+strlen(DfltStdCharset)+3);
        sprintf(*ret_stdLocale,"%s.%s",DfltStdLang,DfltStdCharset);
    }
    if (ret_stdLang != NULL && *ret_stdLang == NULL)
        *ret_stdLang = strdup(DfltStdLang);
    if (ret_stdSet != NULL && *ret_stdSet == NULL)
        *ret_stdSet = strdup(DfltStdCharset);
}


/******************************************************************************
 * Function:    static void XlateStdToOpLocale(char *operation, 
 *                              char *stdLocale, char *dflt_opLocale, 
 *                              char **ret_opLocale)
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
static void
XlateStdToOpLocale (
     char       *operation,
     char       *stdLocale,
     char       *dflt_opLocale,
     char       **ret_opLocale)
{
    _DtXlateDb MyDb;

    MyDb = OpenLcxDb();

    if (MyDb != NULL)
        (void) _DtLcxXlateStdToOp(MyDb, MyPlatform, CompVer,
                        operation, stdLocale, NULL, NULL, NULL, ret_opLocale);
}

extern int shSpecialParse; /* in ksh93/src/cmd/ksh93/sh/lex.c */

/*
 * updateShellSpecialParse uses the libXvh database to determine if the
 * current character encoding requires special care in the ksh parser.
 * It sets or clears a global flag (shSpecialParse) based on the value
 * from the database.  This flag is declared and inspected in sh_lex() in 
 * ksh93/src/cmd/ksh93/sh/lex.c.  This routine is stubbed in the 
 * file .../sh/userinit.c to allow ksh93 to compile & run, albeit
 * without any knowledge of when to do special parsing.
 */

void
updateShSpecialParse( void )
{
    char *locale = (char *)NULL, *parseVal = (char *)NULL;
    int * lockedFds;

    lockedFds = LockKshFileDescriptors();

    XlateOpToStdLocale(DtLCX_OPER_SETLOCALE, setlocale(LC_CTYPE,NULL),
		       &locale, NULL, NULL);
    XlateStdToOpLocale("dtkshSpecialParse", locale, NULL, &parseVal);
    XtFree(locale);

    UnlockKshFileDescriptors(lockedFds);

    if(parseVal != (char *)NULL)
    {
        shSpecialParse = 1;
	XtFree(parseVal);
    }
    else
        shSpecialParse = 0;
}
