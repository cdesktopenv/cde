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
/* $XConsortium: SmProperty.c /main/7 1996/02/08 11:27:32 barstow $ */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */
/*************************************<+>*************************************
 *****************************************************************************
 **
 **  File:        SmProperty.c
 **
 **  Project:     HP DT Session Manager (dtsession)
 **
 **  Description:
 **  -----------
 **  This file contains routines that deal with the properties used by the
 **  session manager to save and restore client information.
 **
 **
 **
 *******************************************************************
 **  (c) Copyright Hewlett-Packard Company, 1990.  All rights are  
 **  reserved.  Copying or other reproduction of this program      
 **  except for archival purposes is prohibited without prior      
 **  written consent of Hewlett-Packard Company.		     
 ********************************************************************
 **
 **
 **
 *****************************************************************************
 *************************************<+>*************************************/

#include <stdio.h>
#include <X11/Intrinsic.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include "Sm.h"
#include "SmProtocol.h"
#include "SmXSMP.h"


/*************************************<->*************************************
 *
 *  GetStandardProperties -
 *
 *
 *  Description: returns information about the specified window
 *  -----------
 *
 *  Inputs:
 *  ------
 *  window = window for which we are getting properties
 * 
 *  Outputs:
 *  -------
 *  argv = data returned from WM_COMMAND property (to restart client)
 *  argc = number of arguments returned from WM_COMMAND property
 *  clientMachine = which machine is client running on
 *  xsmpClient = True if the client is XSMP and False otherwise
 *  screen = window's screen number
 *
 *  Comments:
 *  --------
 *  All X (except GetWMHints) were not available until R4 and therefore have
 *  R4 or greater dependencies.
 *
 *  BEWARE OF THESE ROUTINES:  The XGetWindowProperty routine returns 0 if
 *  it succeeds.  These routines (which were derived from XGetWindowProperty
 *  return 0 if they FAIL.
 * 
 *************************************<->***********************************/
Status GetStandardProperties(
        Window 			window,
	int			screen,
        int 			*argc,			/* RETURNED */
        char 			***argv,		/* RETURNED */
        char 			**clientMachine,	/* RETURNED */
	Boolean 		*xsmpClient)		/* RETURNED */
{
  int 				cc;
  long				suppliedRet;
  XTextProperty			sessProp;
  Atom				actType;
  int				actFormat;
  unsigned long			bytesAfter;
  unsigned long 		nitems;
  unsigned char 		*data = NULL;

  /*
   * If this client is participating in the XSMP, then don't save
   * it as a proxy (pre-XSMP) client.  However, do cache its
   * screen number before returning.
   */
  if (XGetWindowProperty(smGD.display, window, XaSmClientId, 0L,
			 (long) BUFSIZ, False, XA_STRING, &actType,
			 &actFormat, &nitems, &bytesAfter, &data) == Success) 
  {
      if (data && actType == XA_STRING)
      {
	  ClientRecPtr		pClient;

	  for (pClient = connectedList; pClient != NULL; 
	       pClient = pClient->next) {
	       if (!strcmp ((char *) data, pClient->clientId)) {
		   pClient->screenNum = screen;
		   break;
	       }
	  }
          *xsmpClient = True;
          SM_FREE ((char *) data);
  	  return (0);
      }
      SM_FREE ((char *) data);
  }
  *xsmpClient = False;

  /*
   * Get WM_COMMAND property
   */
  if ((cc=XGetCommand(smGD.display,window,argv,argc))==0)
      return(cc);

  /*
   * If there is no argc or argv - don't bother going on.  We're not
   * going to save anything anyway
   */
  if(*argc == 0)
      return(0);


  /*
   * Get WM_CLIENT_MACHINE property
   */
  if ((cc=XGetWMClientMachine(smGD.display,window,&sessProp))==0)
  {
      *clientMachine = NULL;
  }
  else
  {
      *clientMachine =  (char *) sessProp.value;
  }

  return(1);
}
