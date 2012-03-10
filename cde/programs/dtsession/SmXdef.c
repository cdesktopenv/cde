/* $XConsortium: SmXdef.c /main/4 1995/10/30 09:39:16 rswiston $ */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */
/*************************************<+>*************************************
 *****************************************************************************
 **
 **  File:        SmXdef.c
 **
 **  Project:     DT Session Manager (dtsession)
 **
 **  Description:
 **  -----------
 **  This file contains routines to manage the Xdefaults file.
 **
 **  SmXdefMerge() - merge .Xdefaults file into RESOURCE_MANAGER
 **  SmXdefSubtract() - subtract .Xdefaults file from given database
 **
 *****************************************************************************
 *************************************<+>*************************************/

#include <stdio.h>
#include <X11/Intrinsic.h>
#include "Sm.h"
#include "SmXrm.h"

/*
 * State data
 *   dbXdefaults - copy of .Xdefaults in Xrm database form
 */
 static XrmDatabase dbXdefaults = NULL;


/*
 * Note:
 * 
 * The memory for dbXdefaults is freed only upon dtsession termination
 *
 * This code is currently restricted to handling the .Xdefaults file,
 * but can easily be extended to handle other default resource files.
 *
 */


/*************************************<->*************************************
 *
 *  SmXdefMerge(display)
 *
 *  Description:
 *  -----------
 *  Merge the .Xdefaults file into the RESOURCE_MANAGER database
 *
 *  Inputs:
 *  ------
 *  display - display connection
 *
 *  Outputs:
 *  -------
 *
 *
 *  Comments:
 *  --------
 *
 *************************************<->***********************************/
void
SmXdefMerge(Display *display)
{
  char *xdefaults;

 /*
  * Load .Xdefaults
  */
  if( (xdefaults = (char *)malloc(strlen(getenv("HOME"))+12)) != NULL)  
  {
    sprintf(xdefaults,"%s/%s",getenv("HOME"),".Xdefaults");
    if(access(xdefaults,R_OK) == 0)
    {
      FILE *fp;
      char *b = NULL;
      int size;
      struct stat statinfo; 

     /*
      * Determine size of file.
      */
      if (stat(xdefaults, &statinfo) == -1)
      {
        statinfo.st_size = 0;
      }

     /*
      * Get some memory.
      */
      if (statinfo.st_size > 0)
      {
        b = (char *)SM_MALLOC(statinfo.st_size + 1);
      }

      if (b != NULL)
      {
       /*
        * Read file into memory.
        */
        if ((fp = fopen(xdefaults, "r")) != NULL)
        {
          size = fread(b, 1, statinfo.st_size, fp);
          fclose(fp);
        }

        if (size == statinfo.st_size)
        {
         /*
          * Merge .Xdefaults string into RESOURCE_MANAGER database, and
          * also convert to Xrm database form for later subtraction.
          */
          b[size] = '\0';
          _DtAddToResource(display, b);
          dbXdefaults = XrmGetStringDatabase(b);  
          SM_FREE(b);
        }
      }
    }
    free(xdefaults);
  }
}


/*************************************<->*************************************
 *
 *  SmXdefSubtract(db)
 *
 *  Description:
 *  -----------
 *  Subract prior merged .Xdefaults file from given database
 *
 *  Inputs:
 *  ------
 *  db - Xrm database from which to subtract .Xdefaults
 *
 *  Outputs:
 *  -------
 *
 *  Return: 
 *  ------
 *  dbResult - result database
 *
 *  Comments:
 *  --------
 *  Caller is responsible for freeing dbResult using XrmDestroyDatabase()
 *
 *
 *************************************<->***********************************/
XrmDatabase
SmXdefSubtract(XrmDatabase db)
{
  XrmDatabase dbResult;

  if (dbXdefaults)
  {
    dbResult = SmXrmSubtractDatabase(dbXdefaults, db);
  }
  else
  {
    dbResult = NULL;
  }
  return(dbResult);
}
