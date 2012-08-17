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
/* $XConsortium: SmScreen.c /main/4 1995/10/30 09:38:03 rswiston $ */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */
/*************************************<+>*************************************
 *****************************************************************************
 **
 **  File:        SmScreen.c
 **
 **  Project:     DT Session Manager (dtsession)
 **
 **  Description:
 **  -----------
 **  This file contains all routines needed to manage external
 **  screen savers.
 **
 *****************************************************************************
 *************************************<+>*************************************/

#include <stdio.h>
#include <signal.h>
#include <X11/Intrinsic.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <Dt/Wsm.h>
#include <Dt/UserMsg.h>
#include <Dt/SaverP.h>
#include "Sm.h"
#include "SmCommun.h"
#include "SmUI.h"                  /* smDD.* */
#include "SmError.h"
#include "SmWindow.h"
#include "SmProtocol.h"
#include "SmGlobals.h"
#include "SmScreen.h"

/*
 * Structures visible to this module only.
 */
typedef struct {
  int count;
  char *saver[1];   
  /* variable length saver[] array */
  /* saver command strings */
} SmSaverParseStruct;

/*
 * Variables global to this module only
 */
static int savernum;             /* current screen saver number */
static void *saverstate = NULL;  /* current running screen saver state */
static int firsttime = 1;        /* first call to StartScreenSaver */

/*
 * Local Function declarations
 */
static void ParseSaverList(char *, int *, int *, SmSaverParseStruct *);



/*************************************<->*************************************
 *
 *  StartScreenSaver ()
 *
 *
 *  Description:
 *  -----------
 *  Start an external screen saver.
 *
 *  Inputs:
 *  ------
 *
 * 
 *  Outputs:
 *  -------
 *
 *  Comments:
 *  --------
 * 
 *************************************<->***********************************/

void
StartScreenSaver( void )
{
  int i;
  SmSaverParseStruct *parse;

  if (!smGD.saverListParse)
  {
   /*
    * Parse the screen saver list.
    */
    smGD.saverListParse = SmSaverParseSaverList(smGD.saverList);

    if (!smGD.saverListParse)
    {
      return;
    }
    savernum = -1;
  }

  parse = (SmSaverParseStruct *)smGD.saverListParse;

  if (parse->count == 0)
  {
    return;
  }

 /*
  * Decide which saver number to use.
  */
  savernum = (savernum + 1) % parse->count;

  if (firsttime)
  {
   /*
    * Load actions database.
    */
    ProcessReloadActionsDatabase();
    firsttime = 0;
  }

 /*
  * Start screen saver. _DtSaverStop() must be called to terminate the
  * screen saver.
  */
  saverstate = _DtSaverStart(smGD.display, smDD.coverDrawing,
                  smGD.numSavedScreens, parse->saver[savernum],
                  smGD.topLevelWid);

}


/*************************************<->*************************************
 *
 *  StopScreenSaver ()
 *
 *
 *  Description:
 *  -----------
 *  Stop an external screen saver.
 *
 *  Inputs:
 *  ------
 *
 * 
 *  Outputs:
 *  -------
 *
 *  Comments:
 *  --------
 * 
 *************************************<->***********************************/
void
StopScreenSaver( void )
{
  if (saverstate)
  {
   /*
    * Terminate screen saver.
    */
    _DtSaverStop(smGD.display, saverstate);
    saverstate = NULL;
  }
}

/*************************************<->*************************************
 *
 *  SmSaverParseSaverList()
 *
 *
 *  Description:
 *  -----------
 *  Parse screen saver list into allocated buffer.
 *
 *  SaverLine = {SaverSpec|WhiteSpace}
 *  SaverSpec = WhiteSpace Command WhiteSpace
 *  Command = <valid action name>
 *  WhiteSpace = {<space>|<horizontal tab>|<line feed>}
 *
 *  For example, a saverList resource might be specified as:
 *    *saverList:   \n \
 *       StartDtscreenSwarm      \n\
 *       StartDtscreenQix        \n\
 *       StartDtscreenLife
 *
 *  And be represented in memory as:
 *    "StartDtscreenSwarm   \n StartDtscreenQix\n  StartDtscreenLife"
 *
 *
 *  Inputs:
 *  ------
 *  saverList - pointer to screen saver list. This memory is not changed.
 *
 *  Outputs:
 *  -------
 *  none
 *
 *  Return:
 *  -------
 *  pointer to allocated memory containing parsed saver list
 *
 *  Comments:
 *  --------
 *
 *************************************<->***********************************/

void *
SmSaverParseSaverList(
  char *saverList)
{
   char tokenSep[] = " \n\t";
   char * token;
   int i = 0;
   char * tmpStr;
   int len = strlen(saverList);
   int bytes = sizeof(int);
   char *p;
   SmSaverParseStruct *pstruct;

   tmpStr = (char *)XtMalloc(len + 1);
   memcpy(tmpStr, saverList, len+1);
   token = strtok(tmpStr, tokenSep);
   while(token != NULL)
   {
     i++;
     bytes += sizeof(char *) + strlen(token) + 1;
     token = strtok(NULL, tokenSep);
   }

   pstruct = (SmSaverParseStruct *)XtMalloc(bytes);

   if (pstruct)
   {
     memcpy(tmpStr, saverList, len+1);
     token = strtok(tmpStr, tokenSep);
     pstruct->count = 0;
     p = (char *)(pstruct->saver + i);

     while(token != NULL)
     {
       pstruct->saver[pstruct->count] = p;
       strcpy(pstruct->saver[pstruct->count], token);
       p += strlen(token) + 1;
       token = strtok(NULL, tokenSep);
       pstruct->count++;
     }
   }
   XtFree ((char *) tmpStr);
   return((void *)pstruct);
}
