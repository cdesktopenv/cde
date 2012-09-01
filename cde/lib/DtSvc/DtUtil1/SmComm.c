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
/* $XConsortium: SmComm.c /main/7 1996/06/21 17:26:07 ageorge $ */
/******************************************************************************
 *
 * File Name: SmComm.c
 *
 *  Contains functionality for communication with the session manager via
 *  properties put on its top level window
 *
 *****************************************************************************/
/*                                                               
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1993, 1994, 1996 Hewlett-Packard Company        
 * (c) Copyright 1993, 1994, 1996 International Business Machines Corp.       
 * (c) Copyright 1993, 1994, 1996 Sun Microsystems, Inc.                      
 * (c) Copyright 1993, 1994, 1996 Novell, Inc.                                
 * (c) Copyright 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 */

#include <X11/Xlib.h>
#include <X11/Intrinsic.h>
#include <X11/IntrinsicP.h>
#include <X11/Xatom.h>
#include <Dt/DtP.h>
#include <Dt/SessionP.h>
#include <Dt/SessionM.h>
#include "DtSvcLock.h"

/********    Static Function Declarations    ********/
static int _GetSmWindow(Display *, Window, Window *, Atom);
static Status _GetSmState(Display *, Window, Atom, PropDtSmStateInfo **);
static Atom _GetSmAtom(Display *, int);
static void NotifySm( Display *, Window, Atom, CARD32);
/********    End Static Function Declarations    ********/

/*
 * _GetSmAtom() identifiers.
 */
#define SM_ATOMID_STATE  0
#define SM_ATOMID_SAVER  1


/*************************************<->*************************************
 *
 *  status _DtGetSmWindow (display, root, pSmWindow)
 *
 *
 *  Description:
 *  -----------
 *  Get the Dt session manager window
 *
 *
 *  Inputs:
 *  ------
 *  display		- display 
 *  root		- root window of screen
 *  pSmWindow		- pointer to a window (to be returned)
 *
 *  Outputs:
 *  --------
 *  *pSmWindow		- dt SM window id, if successful
 *  Return		- status from XGetWindowProperty
 *
 *  Comments:
 *  --------
 *  This can fail if dt SM is not running
 * 
 *************************************<->***********************************/
Status 
_DtGetSmWindow(
        Display *display,
        Window root,
        Window *pSmWindow )
{
    Atom xa_DT_SM_WINDOW_INFO;

    xa_DT_SM_WINDOW_INFO = XInternAtom (display, _XA_DT_SM_WINDOW_INFO, False);
    return (_GetSmWindow (display, root, pSmWindow, xa_DT_SM_WINDOW_INFO));
} /* END OF FUNCTION _DtGetSmWindow */


/*************************************<->*************************************
 *
 *  Status _DtGetSmState (display, smWindow, pSmStateInfo)
 *
 *
 *  Description:
 *  -----------
 *  Get the Dt state information from the session manager window
 *
 *
 *  Inputs:
 *  ------
 *  display		- display 
 *  smWindow		- session manager window
 *  pSmStateInfo       	- pointer to a state info (to be returned)
 *
 *  Outputs:
 *  --------
 *  *pSmStateInfo	- SM state info
 *  Return		- status from XGetWindowProperty
 *
 *  Comments:
 *  --------
 *  This can fail if dt SM is not running
 * 
 *************************************<->***********************************/
Status 
_DtGetSmState(
	     Display *display,
	     Window smWindow,
	     SmStateInfo *pSmStateInfo)
{
  int			propStatus;
  PropDtSmStateInfo  *pPropSmStateInfo;

  propStatus = _GetSmState(display, smWindow,
                           _GetSmAtom(display, SM_ATOMID_STATE),
                           &pPropSmStateInfo);
    
  if(propStatus == Success)
  {
    pSmStateInfo->flags = pPropSmStateInfo->flags;
    pSmStateInfo->smStartState = pPropSmStateInfo->smStartState;
    pSmStateInfo->smConfirmMode = pPropSmStateInfo->smConfirmMode;
    pSmStateInfo->smCompatMode = pPropSmStateInfo->smCompatMode;
    pSmStateInfo->smSendSettings = pPropSmStateInfo->smSendSettings;
    pSmStateInfo->smCoverScreen = pPropSmStateInfo->smCoverScreen;
    pSmStateInfo->smLockOnTimeout = pPropSmStateInfo->smLockOnTimeout;
    pSmStateInfo->smLockOnTimeoutStatus = pPropSmStateInfo->smLockOnTimeoutStatus;
    pSmStateInfo->smCycleTimeout = pPropSmStateInfo->smCycleTimeout;
    pSmStateInfo->smLockTimeout = pPropSmStateInfo->smLockTimeout;
    pSmStateInfo->smSaverTimeout = pPropSmStateInfo->smSaverTimeout;
    pSmStateInfo->smRandom = pPropSmStateInfo->smRandom;
    pSmStateInfo->smDisplaySpecific =  pPropSmStateInfo->smDisplaySpecific;
    XFree((char *)pPropSmStateInfo);
  }

  return(propStatus);

} /* END OF FUNCTION _DtGetSmState */


/*************************************<->*************************************
 *
 *  Status _DtSetSmState (display, smWindow, pSmStateInfo)
 *
 *
 *  Description:
 *  -----------
 *  Set the DT Session Manager state property on the provided window
 *  and notify that window of the changes via a ClientMessage.
 *
 *
 *  Inputs:
 *  ------
 *  display		- display 
 *  smWindow		- session manager window
 *  pSmStateInfo       	- pointer to a state info (to be returned)
 *
 *  Outputs:
 *  --------
 *  *pSmStateInfo	- SM state info
 *  Return		- status from XGetWindowProperty
 *
 *  Comments:
 *  --------
 *  This can fail if dt SM is not running
 * 
 *************************************<->***********************************/
Status 
_DtSetSmState(
	     Display *display,
	     Window smWindow,
	     SmStateInfo *pSmStateInfo)
{
  int			propStatus;
  PropDtSmStateInfo    propSmStateInfo;
  PropDtSmStateInfo   *pPropSmStateInfo;
  Atom                  xaDtSmStateInfo;

  xaDtSmStateInfo = _GetSmAtom(display, SM_ATOMID_STATE);

  if (pSmStateInfo->flags != SM_STATE_ALL)
  {
   /*
    * Caller not setting all fields in state, so get the other fields
    * from property.
    */
    propStatus = _GetSmState(display, smWindow,
                             xaDtSmStateInfo, &pPropSmStateInfo);
    if (propStatus != Success)
    {
     /* 
      * Whoa. Property could not be obtained. Do a reasonable thing by
      * setting unspecified fields to zero. 
      */
      pPropSmStateInfo = &propSmStateInfo;
      memset(pPropSmStateInfo, 0x00, sizeof(*pPropSmStateInfo));
    }
  }
  else 
  {
   /*
    * Caller setting all fields in state. No need to attempt to retrieve
    * existing values from property.
    */
    pPropSmStateInfo = &propSmStateInfo;
  }

 /* 
  * Set specified fields.
  */    
  pPropSmStateInfo->flags = pSmStateInfo->flags;

  if (pSmStateInfo->flags & SM_STATE_START)
    pPropSmStateInfo->smStartState = pSmStateInfo->smStartState;
    
  if (pSmStateInfo->flags & SM_STATE_CONFIRM)
    pPropSmStateInfo->smConfirmMode = pSmStateInfo->smConfirmMode;

  if (pSmStateInfo->flags & SM_STATE_COMPAT)
    pPropSmStateInfo->smCompatMode = pSmStateInfo->smCompatMode;

  if (pSmStateInfo->flags & SM_STATE_SEND)
    pPropSmStateInfo->smSendSettings = pSmStateInfo->smSendSettings;

  if (pSmStateInfo->flags & SM_STATE_COVER)
    pPropSmStateInfo->smCoverScreen = pSmStateInfo->smCoverScreen;

  if (pSmStateInfo->flags & SM_STATE_LOTOUT)
    pPropSmStateInfo->smLockOnTimeout = pSmStateInfo->smLockOnTimeout;

  if (pSmStateInfo->flags & SM_STATE_LOTOUTSTAT)
    pPropSmStateInfo->smLockOnTimeoutStatus = pSmStateInfo->smLockOnTimeoutStatus;

  if (pSmStateInfo->flags & SM_STATE_CYCLETIMEOUT)
    pPropSmStateInfo->smCycleTimeout = pSmStateInfo->smCycleTimeout;

  if (pSmStateInfo->flags & SM_STATE_LOCKTIMEOUT)
    pPropSmStateInfo->smLockTimeout = pSmStateInfo->smLockTimeout;

  if (pSmStateInfo->flags & SM_STATE_SAVERTIMEOUT)
    pPropSmStateInfo->smSaverTimeout = pSmStateInfo->smSaverTimeout;

  if (pSmStateInfo->flags & SM_STATE_RANDOM)
    pPropSmStateInfo->smRandom = pSmStateInfo->smRandom;

  if (pSmStateInfo->flags & SM_STATE_DISP_SESSION)
    pPropSmStateInfo->smDisplaySpecific = pSmStateInfo->smDisplaySpecific;

  XChangeProperty (display, smWindow,
                   xaDtSmStateInfo, xaDtSmStateInfo,
                   32, PropModeReplace, (unsigned char *)pPropSmStateInfo,
                   PROP_DT_SM_STATE_INFO_ELEMENTS);

  if (pPropSmStateInfo != &propSmStateInfo)
  {    
   /*
    * Memory must have come from _GetStateInfo(). Deallocate.
    */
    XFree((char *)pPropSmStateInfo);
  }

 /*
  * Notify receiver of which state fields have changed. Now one might
  * ask why the receiver doesn't just check the 'flags' field on a 
  * PropertyNotify event. Well they can, and it will work fine, most of
  * the time. However, it is possible for the sender to change the property
  * multiple times before the receiver get its first PropertyNotify event.
  * By the time the receiver reads the property, the flags field might 
  * have changed. The flags field will only tell the receiver which 
  * fields changed on the last call to _DtSetSmStateInfo(). 
  * 
  * This way the receiver has a record of changes to the state.
  */
  NotifySm(display, smWindow, xaDtSmStateInfo, (CARD32)pSmStateInfo->flags);

  return(propStatus);

} /* END OF FUNCTION _DtSetSmState */


/*************************************<->*************************************
 *
 *  status _DtGetSmSaver (display, smWindow, pSmSaverInfo)
 *
 *
 *  Description:
 *  -----------
 *  Get the Dt screen saver information from the session manager window
 *
 *
 *  Inputs:
 *  ------
 *  display		- display 
 *  smWindow		- session manager window
 *  pSmSaverInfo       	- pointer to a screen saver information (to be returned)
 *
 *  Outputs:
 *  --------
 *  *pSmSaverInfo	- SM screen saver info
 *  Return		- status from XGetWindowProperty
 *
 *  Comments:
 *  --------
 *  Caller needs to deallocate pSmSaverInfo->saverList with XFree().
 * 
 *************************************<->***********************************/
Status 
_DtGetSmSaver(
	     Display *display,
	     Window smWindow,
	     SmSaverInfo *pSmSaverInfo)
{
  int                 propStatus;
  Atom                actualType;
  int                 actualFormat;
  unsigned long       nitems;
  unsigned long       leftover;

  propStatus = XGetWindowProperty(display, smWindow,
                                  _GetSmAtom(display, SM_ATOMID_SAVER), 0L,
                                  100000000L, False,
                                  XA_STRING, &actualType,
                                  &actualFormat, &nitems, &leftover,
                                  (unsigned char **)&pSmSaverInfo->saverList);

  if(propStatus == Success)
  {
    if(actualType != XA_STRING)
    {
      propStatus = BadValue;
    }
  }

  return(propStatus);

} /* END OF FUNCTION _DtGetSmSaver */


/*************************************<->*************************************
 *
 *  status _DtSetSmSaver (display, smWindow, pSmSaverInfo)
 *
 *
 *  Description:
 *  -----------
 *  Set the Dt screen saver information on the session manager window
 *
 *
 *  Inputs:
 *  ------
 *  display		- display 
 *  smWindow		- session manager window
 *  pSmSaverInfo       	- pointer to screen saver information struct (to be set)
 *
 *  Outputs:
 *  --------
 *  *pSmSaverInfo	- SM screen saver info
 *  Return		- status from XGetWindowProperty
 *
 *  Comments:
 *  --------
 * 
 *************************************<->***********************************/
void
_DtSetSmSaver(
	     Display *display,
	     Window smWindow,
	     SmSaverInfo *pSmSaverInfo)
{
  Atom                  xaDtSmSaverInfo;

  xaDtSmSaverInfo = _GetSmAtom(display, SM_ATOMID_SAVER);

  XChangeProperty (display, smWindow,
                   xaDtSmSaverInfo,
                   XA_STRING,
                   8, PropModeReplace,
                   (unsigned char *)pSmSaverInfo->saverList,
                   strlen(pSmSaverInfo->saverList));

  NotifySm(display, smWindow, xaDtSmSaverInfo, (CARD32)0);

} /* END OF FUNCTION _DtSetSmSaver */


/*************************************<->*************************************
 *
 *  status _DtGetSmScreen (display, smWindow, xaDtSmScreenInfo, pSmScreenInfo)
 *
 *
 *  Description:
 *  -----------
 *  Get the Dt screen saver information from the dt session manager window
 *
 *
 *  Inputs:
 *  ------
 *  display		- display 
 *  smWindow		- session manager window
 *  xaDtSmScreenInfo   - Atom for screen property
 *  pSmScreenInfo       - pointer to a screen info (to be returned)
 *
 *  Outputs:
 *  --------
 *  *pSmScreenInfo	- SM screen info
 *  Return		- status from XGetWindowProperty
 *
 *  Comments:
 *  --------
 *  This can fail if dt SM is not running
 * 
 *************************************<->***********************************/
Status 
_DtGetSmScreen(
	      Display *display,
	      Window smWindow,
	      Atom xaDtSmScreenInfo,
	      SmScreenInfo *pSmScreenInfo)
{
    Atom 		actualType;
    int 		actualFormat;
    unsigned long 	nitems;
    unsigned long 	leftover;
    int			propStatus;
    PropDtSmScreenInfo  *pSmScreenRet;


    propStatus = XGetWindowProperty(display, smWindow, xaDtSmScreenInfo, 0L,
				    PROP_DT_SM_SCREEN_INFO_ELEMENTS, False,
				    xaDtSmScreenInfo, &actualType,
				    &actualFormat, &nitems, &leftover,
				    (unsigned char **)&pSmScreenRet);
    
    if(propStatus == Success)
    {
	if(actualType != xaDtSmScreenInfo)
	{
	    propStatus = BadValue;
	}
	else
	{
	    pSmScreenInfo->flags = pSmScreenRet->flags;
	    pSmScreenInfo->smTimeout = pSmScreenRet->smTimeout;
	    pSmScreenInfo->smInterval = pSmScreenRet->smInterval;
	    pSmScreenInfo->smPreferBlank = pSmScreenRet->smPreferBlank;
	    pSmScreenInfo->smAllowExp = pSmScreenRet->smAllowExp;

	    XFree((char *)pSmScreenRet);
	}
    }

    return(propStatus);

} /* END OF FUNCTION _DtGetSmScreen */


/*************************************<->*************************************
 *
 *  status _DtGetSmAudio (display, smWindow, xaDtSmAudioInfo, pSmAudioInfo)
 *
 *
 *  Description:
 *  -----------
 *  Get the Dt audio information from the dt session manager window
 *
 *
 *  Inputs:
 *  ------
 *  display		- display 
 *  smWindow		- session manager window
 *  xaDtSmAudioInfo   - Atom for audio property
 *  pSmAudioInfo       - pointer to bell info (to be returned)
 *
 *  Outputs:
 *  --------
 *  *pSmAudioInfo	- SM bell info
 *  Return		- status from XGetWindowProperty
 *
 *  Comments:
 *  --------
 *  This can fail if dt SM is not running
 * 
 *************************************<->***********************************/
Status 
_DtGetSmAudio(
	     Display *display,
	     Window smWindow,
	     Atom xaDtSmAudioInfo,
	     SmAudioInfo *pSmAudioInfo)
{
    Atom 		actualType;
    int 		actualFormat;
    unsigned long 	nitems;
    unsigned long 	leftover;
    int			propStatus;
    PropDtSmAudioInfo  *pSmAudioRet;


    propStatus = XGetWindowProperty(display, smWindow, xaDtSmAudioInfo, 0L,
				    PROP_DT_SM_AUDIO_INFO_ELEMENTS, False,
				    xaDtSmAudioInfo, &actualType,
				    &actualFormat, &nitems, &leftover,
				    (unsigned char **)&pSmAudioRet);
    
    if(propStatus == Success)
    {
	if(actualType != xaDtSmAudioInfo)
	{
	    propStatus = BadValue;
	}
	else
	{
	    pSmAudioInfo->flags = pSmAudioRet->flags;
	    pSmAudioInfo->smBellPercent = pSmAudioRet->smBellPercent;
	    pSmAudioInfo->smBellPitch = pSmAudioRet->smBellPitch;
	    pSmAudioInfo->smBellDuration = pSmAudioRet->smBellDuration;

	    XFree((char *)pSmAudioRet);
	}
    }

    return(propStatus);

} /* END OF FUNCTION _DtGetSmAudio */


/*************************************<->*************************************
 *
 *  status _DtGetSmKeyboard (display, smWindow,
 *                          xaDtSmKeyboardInfo, pSmKeyboardInfo)
 *
 *
 *  Description:
 *  -----------
 *  Get the Dt keyboard information from the dt session manager window
 *
 *
 *  Inputs:
 *  ------
 *  display		- display 
 *  smWindow		- session manager window
 *  xaDtSmKeyboardInfo   - Atom for keyboard property
 *  pSmKeyboardInfo       - pointer to keyboard info (to be returned)
 *
 *  Outputs:
 *  --------
 *  *pSmKeyboardInfo	- SM keyboard info
 *  Return		- status from XGetWindowProperty
 *
 *  Comments:
 *  --------
 *  This can fail if dt SM is not running
 * 
 *************************************<->***********************************/
Status 
_DtGetSmKeyboard(
		Display *display,
		Window smWindow,
		Atom xaDtSmKeyboardInfo,
		SmKeyboardInfo *pSmKeyboardInfo)
{
    Atom 		actualType;
    int 		actualFormat;
    unsigned long 	nitems;
    unsigned long 	leftover;
    int			propStatus;
    PropDtSmKeyboardInfo  *pSmKeyboardRet;


    propStatus = XGetWindowProperty(display, smWindow, xaDtSmKeyboardInfo, 0L,
				    PROP_DT_SM_KEYBOARD_INFO_ELEMENTS, False,
				    xaDtSmKeyboardInfo, &actualType,
				    &actualFormat, &nitems, &leftover,
				    (unsigned char **)&pSmKeyboardRet);
    
    if(propStatus == Success)
    {
	if(actualType != xaDtSmKeyboardInfo)
	{
	    propStatus = BadValue;
	}
	else
	{
	    pSmKeyboardInfo->flags = pSmKeyboardRet->flags;
	    pSmKeyboardInfo->smKeyClickPercent =
		pSmKeyboardRet->smKeyClickPercent;
	    pSmKeyboardInfo->smGlobalAutoRepeat =
		pSmKeyboardRet->smGlobalAutoRepeat;

	    XFree((char *)pSmKeyboardRet);
	}
    }

    return(propStatus);

} /* END OF FUNCTION _DtGetSmKeyboard */


/*************************************<->*************************************
 *
 *  status _DtGetSmFont (display, smWindow, xaDtSmFontInfo, retFont)
 *
 *
 *  Description:
 *  -----------
 *  Get the Dt font information from the dt session manager window
 *
 *
 *  Inputs:
 *  ------
 *  display		- display 
 *  smWindow		- session manager window
 *  xaDtSmFontInfo   - Atom for font property
 *  retFont		- pointer to font info
 *
 *  Outputs:
 *  --------
 *  retFont		- SM fontinfo
 *  Return		- status from XGetWindowProperty
 *
 *  Comments:
 *  --------
 *  This can fail if dt SM is not running
 * 
 *************************************<->***********************************/
Status 
_DtGetSmFont(
	    Display *display,
	    Window smWindow,
	    Atom xaDtSmFontInfo,
	    char **retFont)
{
    Atom 		actualType;
    int 		actualFormat;
    unsigned long 	nitems;
    unsigned long 	leftover;
    int			propStatus;


    propStatus = XGetWindowProperty(display, smWindow, xaDtSmFontInfo, 0L,
				    100000000L,False,XA_STRING,&actualType,
				    &actualFormat,&nitems,&leftover,
				    (unsigned char**) retFont);
    
    if(propStatus == Success)
    {
	if(actualType != XA_STRING) 
	{
	    propStatus = BadValue;
	}
    }

    return(propStatus);

} /* END OF FUNCTION _DtGetSmFont */


/*************************************<->*************************************
 *
 *  status _DtGetSmPreedit (display, smWindow, xaDtSmPreeditInfo, retPreedit)
 *
 *
 *  Description:
 *  -----------
 *  Get the preeditType information from the dt session manager window
 *
 *
 *  Inputs:
 *  ------
 *  display		- display 
 *  smWindow		- session manager window
 *  xaDtSmPreeditInfo   - Atom for preeditType property
 *  retPreedit		- pointer to preeditType info
 *
 *  Outputs:
 *  --------
 *  retPreedit		- SM preeditType info
 *  Return		- status from XGetWindowProperty
 *
 *  Comments:
 *  --------
 *  This can fail if dt SM is not running
 * 
 *************************************<->***********************************/
Status 
_DtGetSmPreedit(
	    Display *display,
	    Window smWindow,
	    Atom xaDtSmPreeditInfo,
	    char **retPreedit)
{
    Atom 		actualType;
    int 		actualFormat;
    unsigned long 	nitems;
    unsigned long 	leftover;
    int			propStatus;


    propStatus = XGetWindowProperty(display, smWindow, xaDtSmPreeditInfo, 0L,
				    100000000L,False,XA_STRING,&actualType,
				    &actualFormat,&nitems,&leftover,
				    (unsigned char**) retPreedit);
    
    if(propStatus == Success)
    {
	if(actualType != XA_STRING) 
	{
	    propStatus = BadValue;
	}
    }

    return(propStatus);

} /* END OF FUNCTION _DtGetSmPreedit */


/*************************************<->*************************************
 *
 *  status _DtGetSmPointer (display, smWindow, xaDtSmPointerInfo, retFont)
 *
 *
 *  Description:
 *  -----------
 *  Get the Dt pointer information from the dt session manager window
 *
 *
 *  Inputs:
 *  ------
 *  display		- display 
 *  smWindow		- session manager window
 *  xaDtSmPointerInfo   - Atom for pointer property
 *  retPointer		- pointer to pointer info
 *
 *  Outputs:
 *  --------
 *  retPointer		- SM pointer info
 *  Return		- status from XGetWindowProperty
 *
 *  Comments:
 *  --------
 *  This can fail if dt SM is not running
 * 
 *************************************<->***********************************/
Status 
_DtGetSmPointer(
	       Display *display,
	       Window smWindow,
	       Atom xaDtSmPointerInfo,
	       char **retPointer)
{
    Atom 		actualType;
    int 		actualFormat;
    unsigned long 	nitems;
    unsigned long 	leftover;
    int			propStatus;


    propStatus = XGetWindowProperty(display, smWindow, xaDtSmPointerInfo, 0L,
				    100000000L,False,XA_STRING,&actualType,
				    &actualFormat,&nitems,&leftover,
				    (unsigned char**) retPointer);
    
    if(propStatus == Success)
    {
	if(actualType != XA_STRING)
	{
	    propStatus = BadValue;
	}
    }

    return(propStatus);

} /* END OF FUNCTION _DtGetSmPointer */



/*************************************<->*************************************
 *
 *  static Status
 *  _GetSmWindow (display, root, pSmWindow, property)
 *
 *
 *  Description:
 *  -----------
 *  Get the Dt session manager window
 *
 *
 *  Inputs:
 *  ------
 *  display		- display 
 *  root		- root window of screen
 *  pSmWindow		- pointer to a window (to be returned)
 *  property		- the property atom
 *
 *  Outputs:
 *  --------
 *  *pSmWindow		- dt SM  window id, if successful
 *  Return		- status from XGetWindowProperty
 *
 *  Comments:
 *  --------
 *  This can fail if dt SM is not running
 * 
 *************************************<->***********************************/
static int 
_GetSmWindow(
        Display *display,
        Window root,
        Window *pSmWindow,
        Atom property )
{
    Atom actualType;
    int actualFormat;
    unsigned long nitems;
    unsigned long leftover;
    PropDtSmWindowInfo *pSmInfo = NULL;
    int rcode;
    Window wroot, wparent, *pchildren;
    unsigned int nchildren;

    *pSmWindow = 0;
    if ((rcode=XGetWindowProperty(display,root,
			     property,0L, PROP_DT_SM_WINDOW_INFO_ELEMENTS,
			     False,property,
			     &actualType,&actualFormat,
			     &nitems,&leftover,
				  (unsigned char **)&pSmInfo))==Success)
    {

        if (actualType != property)
	{
	    /* wrong type, force failure */
	    rcode = BadValue;
	}
	else
#ifdef BAD
	{
	    rcode = BadWindow;	/* assume the worst */

	    /*
	     * The SM window should be a direct child of root
	     */
	    if (XQueryTree (display, root, &wroot, &wparent,
			    &pchildren, &nchildren))
	    {
		int i;

		for (i = 0; (i < nchildren) && (rcode != Success); i++)
		{
		    if (pchildren[i] == pSmInfo->smWindow)
		    {
			rcode = Success;
		    }
		}
	    }

	    if (rcode == Success);
	    {
		*pSmWindow = pSmInfo->smWindow;
	    }

	    if (pchildren)
	    {
		XFree ((char *)pchildren);
	    }

	}
#else 
        *pSmWindow = pSmInfo->smWindow;

#endif /* BAD */


	if (pSmInfo)
	{
	    XFree ((char *)pSmInfo);
	}
    }
	
    return(rcode);

} /* END OF FUNCTION _GetSmWindow */


/*************************************<->*************************************
 *
 *  Atom _GetSmAtom (display, id)
 *
 *
 *  Description:
 *  -----------
 *  Get a session manager atom
 *
 *
 *  Inputs:
 *  ------
 *  display		- display 
 *  id                  - SM_ATOMID_* value
 *
 *  Outputs:
 *  --------
 *
 *  Return:
 *  -------
 *  atom  		- sm state atom
 *
 *  Comments:
 *  --------
 * 
 *************************************<->***********************************/

static struct {
  char *string;
  Atom atom;
} smAtomList[] = {
  {_XA_DT_SM_STATE_INFO, 0},
  {_XA_DT_SM_SAVER_INFO, 0},
};

static Atom
_GetSmAtom(Display *display, int id)
{
  _DtSvcProcessLock();
  if (!smAtomList[id].atom)
  {
    smAtomList[id].atom = XInternAtom (display, smAtomList[id].string, False);
  }
  _DtSvcProcessUnlock();

  return(smAtomList[id].atom);

} /* END OF FUNCTION _GetSmAtom */


/*************************************<->*************************************
 *
 *  status _GetSmState (display, smWindow, xaDtSmStateInfo, pPropSmStateInfo)
 *
 *
 *  Description:
 *  -----------
 *  Get the Dt state information from the session manager window
 *
 *
 *  Inputs:
 *  ------
 *  display		- display 
 *  smWindow		- session manager window
 *  pSmStateInfo       	- pointer to mem in which to place state info pointer
 *
 *  Outputs:
 *  --------
 *  *pSmStateInfo	- pointer to state info (use XFree() to free)
 *
 *  Return:
 *  -------
 *  status		- status from XGetWindowProperty
 *
 *  Comments:
 *  --------
 *  This can fail if dt SM is not running
 * 
 *************************************<->***********************************/
static Status 
_GetSmState(
	     Display *display,
	     Window smWindow,
	     Atom xaDtSmStateInfo,
	     PropDtSmStateInfo **pPropSmStateInfo)
{
    Atom 		actualType;
    int 		actualFormat;
    unsigned long 	nitems;
    unsigned long 	leftover;
    int			propStatus;

    propStatus = XGetWindowProperty(display, smWindow, xaDtSmStateInfo, 0L,
				    PROP_DT_SM_STATE_INFO_ELEMENTS, False,
				    xaDtSmStateInfo, &actualType,
				    &actualFormat, &nitems, &leftover,
				    (unsigned char **)pPropSmStateInfo);
    
    if(propStatus == Success)
    {
	if(actualType != xaDtSmStateInfo)
	{
	    propStatus = BadValue;
	}
    }

    return(propStatus);

} /* END OF FUNCTION _GetSmState */


/*************************************<->*************************************
 *
 *  static void
 *  _NotifySm (display, smWindow, atom, card32)
 *
 *
 *  Description:
 *  -----------
 *  Notify session manager of change
 *
 *
 *  Inputs:
 *  ------
 *  display		- display 
 *  smWindow		- session manager window
 *  atom                - atom id of property changed
 *  card32              - additional property specific information
 *
 *  Outputs:
 *  --------
 *
 *  Return:
 *  -------
 *
 *  Comments:
 *  --------
 * 
 *************************************<->***********************************/
static void
NotifySm(
  Display *display,
  Window smWindow,
  Atom atom,
  CARD32 md0)
{
  XClientMessageEvent   message;

  message.type = ClientMessage;
  message.window = smWindow;
  message.message_type = atom;
  message.format = 32;
  message.data.l[0] = md0;
  message.data.l[1] = CurrentTime;
  XSendEvent(display, smWindow, False, NoEventMask, (XEvent *)&message);

} /* END OF FUNCTION _NotifySm */
