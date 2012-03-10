#if DOC
/*===================================================================
$FILEBEG$:   PrintManStrFile.c
$COMPONENT$: dthelpprint
$PROJECT$:   Cde1
$SYSTEM$:    HPUX 9.0; AIX 3.2; SunOS 5.3
$REVISION$:  $XConsortium: PrintManStrFile.c /main/4 1996/10/30 11:35:27 drk $
$CHGLOG$:    
$COPYRIGHT$:
   (c) Copyright 1993, 1994 Hewlett-Packard Company
   (c) Copyright 1993, 1994 International Business Machines Corp.
   (c) Copyright 1993, 1994 Sun Microsystems, Inc.
   (c) Copyright 1993, 1994 Unix System Labs, Inc., a subsidiary of Novell, Inc.
==$END$==============================================================*/
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#if defined(sun)
#include <locale.h>
#else
#include <langinfo.h>
#endif

#include "HelpPrintI.h"

/*======== boundary values ==============*/
#define MAX_COMMAND_LENGTH 5120		 /* max system can handle */

/*======== dthelpprint.sh options ==============*/

/*======== helper values ===============*/
#define EOS           '\0'
#define EMPTY_STR     s_EmptyStr

#define PMSET  4        /* message catalog set */

/*======== helper variables ===============*/
static char s_EmptyStr[1] = { EOS };

/*======== data structs ==============*/

/*======== static variables ===============*/

/*======== functions ==============*/

#if DOC
===================================================================
$FUNBEG$:  _DtHPrPrintStringData()
$1LINER$:  Writes string data to file and then sends to printer
$DESCRIPT$:
Writes string data to file and then sends to printer
$RETURNS$:
$ARGS$:
========================================================$SKIP$=====*/
#endif /*DOC*/

int _DtHPrPrintStringData(
   Display *dpy,
   _DtHPrOptions * options)
{       /*$CODE$*/
   char *printCommand;
   char cmdFormat[100];
   char prOffsetArg[30];
   int status;

    if ( NULL == options->stringData )
    {
         fprintf(stderr, _DTGETMESSAGE(PMSET,1,
                            "%s: Error: helpType is string, "
                            "but no stringData specified.\n"),
                             options->programName);

         return 1;                      /* RETURN */
    }

   /* Alloc max shell command line len */
   printCommand = malloc(MAX_COMMAND_LENGTH*sizeof(char));
   if (printCommand == NULL)
   {
         fprintf(stderr, _DTGETMESSAGE(PMSET,5,
                            "%s: Error: memory allocation failed\n"),
                             options->programName );

         return 2;                      /* RETURN error */
   }
   
   /** generate the command **/
   _DtHPrGetPrOffsetArg(options,prOffsetArg);
   sprintf(cmdFormat, "%s %s|%s %s|%s %s %s", /* echo | fold | pr */
             options->echoCommand, options->echoArgs,
             options->foldCommand, options->foldArgs,
             options->prCommand, prOffsetArg, options->prArgs);
   sprintf(printCommand, cmdFormat,
             options->stringData,                            /* echo */
             options->colsTextWidth, EMPTY_STR,		     /* fold */
             options->topicTitle, options->rowsHeight, EMPTY_STR); /* pr */

   return _DtHPrGenFileOrPrint(options,"String",printCommand);
} /*$END$*/



#if DOC
===================================================================
$FUNBEG$:  _DtHPrPrintDynamicStringData()
$1LINER$:  Writes string data to file, formats it, then sends to printer
$DESCRIPT$:
Writes string data to file, formats it, then sends to printer
$RETURNS$:
$ARGS$:
========================================================$SKIP$=====*/
#endif /*DOC*/

int _DtHPrPrintDynamicStringData(
   Display *dpy,
   _DtHPrOptions * options)
{       /*$CODE$*/
   char *printCommand;
   char cmdFormat[100];
   char prOffsetArg[30];
   int status;

    if ( NULL == options->stringData )
    {
         fprintf(stderr, _DTGETMESSAGE(PMSET,2,
                            "%s: Error: helpType is dynamic string, "
                            "but no stringData specified.\n"),
                             options->programName);

         return 1;                      /* RETURN */
    }

   /* Alloc max shell command line len */
   printCommand = malloc(MAX_COMMAND_LENGTH*sizeof(char));
   if (printCommand == NULL)
   {
         fprintf(stderr, _DTGETMESSAGE(PMSET,5,
                            "%s: Error: memory allocation failed\n"),
                             options->programName );

         return 2;                      /* RETURN error */
   }
   
   /** generate the command **/
   _DtHPrGetPrOffsetArg(options,prOffsetArg);
    sprintf(cmdFormat, "%s %s|%s %s|%s %s %s", /* echo | fold | pr */
             options->echoCommand, options->echoArgs,
             options->foldCommand, options->foldArgs,
             options->prCommand, prOffsetArg, options->prArgs);
    sprintf(printCommand, cmdFormat,
             options->stringData,                            /* echo */
             options->colsTextWidth, EMPTY_STR,		     /* fold */
             options->topicTitle, options->rowsHeight, EMPTY_STR); /* pr */


   return _DtHPrGenFileOrPrint(options,"String",printCommand);
} /*$END$*/


#if DOC
===================================================================
$FUNBEG$:  _DtHPrPrintManPage()
$1LINER$:  Dumps formatted man page to file and then sends to printer
$DESCRIPT$:
Dumps formatted man page to file and then sends to printer
$RETURNS$:
$ARGS$:
========================================================$SKIP$=====*/
#endif /*DOC*/

int _DtHPrPrintManPage(
   Display *dpy,
   _DtHPrOptions * options)
{       /*$CODE$*/
   char *printCommand;
   char cmdFormat[100];
   int status;

    if ( NULL == options->manPage )
    {
         fprintf(stderr, _DTGETMESSAGE(PMSET,3,
                            "%s: Error: helpType is man page, "
                            "but no manPage specified.\n"),
                             options->programName);

         return 1;                      /* RETURN */
    }

   /* Alloc max shell command line len */
   printCommand = malloc(MAX_COMMAND_LENGTH*sizeof(char));
   if (printCommand == NULL)
   {
         fprintf(stderr, _DTGETMESSAGE(PMSET,5,
                            "%s: Error: memory allocation failed\n"),
                             options->programName );

         return 2;                      /* RETURN error */
   }
   
   /** generate the command **/
    sprintf(cmdFormat, "%s %s",         /* man */
             options->manCommand, options->manArgs);
    sprintf(printCommand, cmdFormat,
             options->manPage);         /* man */


   return _DtHPrGenFileOrPrint(options,options->manPage,printCommand);
} /*$END$*/
   


#if DOC
===================================================================
$FUNBEG$:  _DtHPrPrintHelpFile()
$1LINER$:  Format file and then sends to printer
$DESCRIPT$:
Format file and then sends to printer
$RETURNS$:
$ARGS$:
========================================================$SKIP$=====*/
#endif /*DOC*/

int _DtHPrPrintHelpFile(
   Display *dpy,
  _DtHPrOptions * options)
{       /*$CODE$*/
   char *printCommand;
   char  cmdFormat[100];
   char prOffsetArg[30];
   int status;

    if ( NULL == options->helpFile )
    {
         fprintf(stderr, _DTGETMESSAGE(PMSET,4,
                            "%s: Error: helpType is file, "
                            "but no helpFile specified.\n"),
                             options->programName);

         return 1;                      /* RETURN error */
    }

   /* Alloc max shell command line len */
   printCommand = malloc(MAX_COMMAND_LENGTH*sizeof(char));
   if (printCommand == NULL)
   {
         fprintf(stderr, _DTGETMESSAGE(PMSET,5,
                            "%s: Error: memory allocation failed\n"),
                             options->programName );

         return 2;                      /* RETURN error */
   }
   
   /** generate the command **/
   _DtHPrGetPrOffsetArg(options,prOffsetArg);
    sprintf(cmdFormat, "%s %s|%s %s %s", /* fold | pr */
             options->foldCommand, options->foldArgs,
             options->prCommand, prOffsetArg, options->prArgs);
    sprintf(printCommand, cmdFormat,
             options->colsTextWidth, options->helpFile,	     /* fold */
             options->topicTitle, options->rowsHeight, EMPTY_STR); /* pr */

   return _DtHPrGenFileOrPrint(options,options->helpFile,printCommand);
} /*$END$*/
   
