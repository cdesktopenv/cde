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
/* $XConsortium: SmXrm.c /main/4 1995/10/30 09:39:33 rswiston $ */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */
/*************************************<+>*************************************
 *****************************************************************************
 **
 **  File:        SmXrm.c
 **
 **  Project:     DT Session Manager (dtsession)
 **
 **  Description:
 **  -----------
 **  This file contains routines to manage an Xrm database
 **
 **  SmXrmSubtract() - subtract source_db from target_db and return result_db
 **
 *****************************************************************************
 *************************************<+>*************************************/

#ifdef DEBUG
#include <stdio.h>
#endif

#include <X11/Intrinsic.h>

const XrmQuark empty = NULLQUARK;

struct smSubtractState {
  XrmDatabase source_db;
  XrmDatabase target_db;
  XrmDatabase result_db;
  XrmBindingList target_bindings;
  XrmQuarkList target_quarks;
};


#ifdef DEBUG
static void _PrintDbEntry(
  char *,
  XrmBindingList,
  XrmQuarkList,
  XrmRepresentation *,
  XrmValue *);
#endif

static Bool _CompareBindingQuarkList(
  XrmBindingList bindings1,
  XrmQuarkList   quarks1,
  XrmBindingList bindings2,
  XrmQuarkList   quarks2);
static Bool _SmCompareSourceAndTarget(
  XrmDatabase *db,
  XrmBindingList bindings,
  XrmQuarkList quarks,
  XrmRepresentation *type,
  XrmValue *value,
  XPointer closure);
static Bool _SmEnumerateSource(
  XrmDatabase *db,
  XrmBindingList bindings,
  XrmQuarkList quarks,
  XrmRepresentation *type,
  XrmValue *value,
  XPointer closure);


#ifndef DEBUG
#define _PrintDbEntry(a,b,c,d,e)
#endif /* !DEBUG */


/*************************************<->*************************************
 *
 *  _CompareBindingQuarkList()
 *
 *  Description:
 *  -----------
 *  Compare two binding quark lists and return True if the same
 *
 *  Inputs:
 *  ------
 *  bindings1 - list1 bindings
 *  quarks1 - list1 bindings
 *  bindings2 - list2 bindings
 *  quarks2 - list2 bindings
 *
 *  Outputs:
 *  -------
 *
 *  Return:
 *  ------
 *  same - True if binding quark list is the same, False otherwise
 *
 *  Comments:
 *  --------
 *
 *************************************<->***********************************/

static
Bool _CompareBindingQuarkList(
  XrmBindingList bindings1,
  XrmQuarkList   quarks1,
  XrmBindingList bindings2,
  XrmQuarkList   quarks2)
{
  int i = 0;
  Bool rc = False;

 /* 
  * loop through quarks in list1
  */
  while (quarks1[i] != NULLQUARK)
  {
   /*
    * compare quark in list1 to same element in list2 and
    * break out of loop if they differ.
    */
    if (quarks2[i] == NULLQUARK || quarks2[i] != quarks1[i])
      break;

   /*
    * quarks for this level compare, now compare bindings
    */
    if (bindings1[i] != bindings2[i])
      break;

    i++;
  }

  if (quarks1[i] == NULLQUARK && quarks2[i] == NULLQUARK)
  {
   /*
    * all quarks and bindings in list1 and list2 compare
    */
    rc = True;
  }
  
  return(rc);
}

/*************************************<->*************************************
 *
 *  _PrintDbEntry()
 *
 *  Description:
 *  -----------
 *  Print an Xrm database entry (DEBUG only)
 *
 *  Inputs:
 *  ------
 *  leader - leading string
 *  bindings - binding list
 *  quarks - quark list
 *  type - element type
 *  value - element value
 *
 *  Outputs:
 *  -------
 *
 *  Return:
 *  ------
 *
 *  Comments:
 *  --------
 *
 *************************************<->***********************************/

#ifdef DEBUG
static
void _PrintDbEntry(
       char *leader,
       XrmBindingList bindings,
       XrmQuarkList quarks,
       XrmRepresentation *type,
       XrmValue *   value)
{
   char *   str;
   int i;

   FILE *fp = fopen ("/tmp/dtsession.xrm", "a");

   str = XrmQuarkToString(type);

   fprintf(fp, "%8s ", leader);

   i = 0;
   while ( quarks[i] != NULLQUARK )
   {
      str = XrmQuarkToString(quarks[i]);
      fprintf(fp, "%s", str);
      i++;
      if (quarks[i] != NULLQUARK)
      {
        fprintf(fp, bindings[i] == XrmBindLoosely ? "*" : ".");
      }
   }
   fprintf(fp, ": %s\n",value->addr);
   fclose(fp);
}
#endif /* DEBUG */

/*************************************<->*************************************
 *
 *  _SmCompareSourceAndTarget()
 *
 *  Description:
 *  -----------
 *  Xrm Enum callback that compares the current database element to the
 *  current target element.
 *
 *  Inputs:
 *  ------
 *  db - source database
 *  bindings - binding list
 *  quarks - quark list
 *  type - element type
 *  value - element value
 *  closure - pointer to smSubtractState data
 *
 *  Outputs:
 *  -------
 *
 *  Return:
 *  ------
 *  result - True if same, False if different
 *
 *  Comments:
 *  --------
 *
 *************************************<->***********************************/

static
Bool _SmCompareSourceAndTarget(
  XrmDatabase *db,
  XrmBindingList bindings,
  XrmQuarkList quarks,
  XrmRepresentation *type,
  XrmValue *value,
  XPointer closure)
{
  struct smSubtractState *state = (struct smSubtractState *)closure;
  Bool rc = False;

  _PrintDbEntry("source", bindings, quarks, type, value);

  if (_CompareBindingQuarkList(bindings, quarks,
                              state->target_bindings, state->target_quarks))
  {
    rc = True;
  }

  return rc;
}

/*************************************<->*************************************
 *
 *  _SmEnumerateSource()
 *
 *  Description:
 *  -----------
 *  Xrm Enum callback that enumerates the source database for comparison
 *  against the current target element.
 *
 *  Inputs:
 *  ------
 *  db - target database
 *  bindings - binding list
 *  quarks - quark list
 *  type - element type
 *  value - element value
 *  closure - pointer to smSubtractState data
 *
 *  Outputs:
 *  -------
 *
 *  Return:
 *  ------
 *  result - always False
 *
 *  Comments:
 *  --------
 *
 *************************************<->***********************************/

static
Bool _SmEnumerateSource(
  XrmDatabase *db,
  XrmBindingList bindings,
  XrmQuarkList quarks,
  XrmRepresentation *type,
  XrmValue *value,
  XPointer closure)
{
  struct smSubtractState *state = (struct smSubtractState *)closure;
  Bool rc;

  _PrintDbEntry("target", bindings, quarks, type, value);

 /* 
  * Enumerate source database and compare each element to current
  * target bindings and quarks. 
  */ 
  state->target_bindings = bindings;
  state->target_quarks = quarks;
  if (XrmEnumerateDatabase(state->source_db, &empty, &empty, XrmEnumAllLevels,
                           _SmCompareSourceAndTarget, closure) == False)
  {
   /*
    * Target bindings and quarks don't match any element in source database,
    * so copy target element to result db.
    */
    _PrintDbEntry("nomatch", bindings, quarks, type, value);
    XrmQPutResource(&state->result_db, bindings, quarks, *type, value); 
  }
  return False;
}

/*************************************<->*************************************
 *
 *  SmXrmSubtractDatabase()
 *
 *  Description:
 *  -----------
 *  Subtracts source database from target database and returns
 *  the result database.
 *
 *  Inputs:
 *  ------
 *  source_db - source database
 *  target_db - target database
 *
 *  Outputs:
 *  -------
 *
 *  Return:
 *  ------
 *  result_db - result database
 *
 *  Comments:
 *  --------
 *
 *************************************<->***********************************/

/*
 * Subtracts source database from target database and returns
 * the result database.
 */

XrmDatabase
SmXrmSubtractDatabase(
  XrmDatabase source_db,
  XrmDatabase target_db)
{
  struct smSubtractState state;

 /*
  * return if source or target db not specified
  */
  if (source_db == NULL || target_db == NULL)
    return NULL;

 /*
  * set up state
  */
  state.source_db = source_db;
  state.target_db = target_db;
  state.result_db = NULL;

 /*
  * populate result db by looping through target and
  * copying elements that don't also exist in source db
  */
  XrmEnumerateDatabase(state.target_db, &empty, &empty, XrmEnumAllLevels,
                       _SmEnumerateSource, (XPointer)&state);
  
  return(state.result_db); 
}
