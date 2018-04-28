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
/* $XConsortium: Resource.c /main/6 1996/07/19 10:21:08 pascale $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        Resource.c
 **
 **   Project:     DT 3.0
 **
 **   Description: Controls the Dtstyle resources
 **
 **
 ****************************************************************************
 ************************************<+>*************************************/
/*
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1990, 1996 Hewlett-Packard Company.
 * (c) Copyright 1996 International Business Machines Corp.
 * (c) Copyright 1996 Sun Microsystems, Inc.
 * (c) Copyright 1996 Novell, Inc. 
 * (c) Copyright 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 */

/*+++++++++++++++++++++++++++++++++++++++*/
/* include files                         */
/*+++++++++++++++++++++++++++++++++++++++*/

#include <Xm/Xm.h>
#include <Xm/XmP.h>

#include "Main.h"

/*+++++++++++++++++++++++++++++++++++++++*/
/* include extern functions              */
/*+++++++++++++++++++++++++++++++++++++++*/
#include "Resource.h"

/*+++++++++++++++++++++++++++++++++++++++*/
/* Local #defines                        */
/*+++++++++++++++++++++++++++++++++++++++*/
#define DEF_FONT "Fixed"

#ifndef CDE_INSTALLATION_TOP
#define CDE_INSTALLATION_TOP "/opt/dt"
#endif
/*+++++++++++++++++++++++++++++++++++++++*/
/* Internal Functions                    */
/*+++++++++++++++++++++++++++++++++++++++*/


/*+++++++++++++++++++++++++++++++++++++++*/
/* Internal Variables                    */
/*+++++++++++++++++++++++++++++++++++++++*/


/*++++++++++++++++++++++++++++++++++++++*/
/* Application Resources                */
/*++++++++++++++++++++++++++++++++++++++*/

XtResource sysFont_resources[] = {

  {"systemFont1", "SystemFont1", XmRFontList, sizeof (XmFontList), 
      XtOffset(ApplicationDataPtr, fontChoice[0].sysFont), XmRString, 
      "-adobe-helvetica-medium-r-normal--10-*-iso8859-1"
  },
  {"systemFont2", "SystemFont2", XmRFontList, sizeof (XmFontList), 
      XtOffset(ApplicationDataPtr, fontChoice[1].sysFont), XmRString, 
      "-adobe-helvetica-medium-r-normal--12-*-iso8859-1"
  },
  {"systemFont3", "SystemFont3", XmRFontList, sizeof (XmFontList), 
      XtOffset(ApplicationDataPtr, fontChoice[2].sysFont), XmRString,
      "-adobe-helvetica-medium-r-normal--14-*-iso8859-1"
  },
  {"systemFont4", "SystemFont4", XmRFontList, sizeof (XmFontList), 
      XtOffset(ApplicationDataPtr, fontChoice[3].sysFont), XmRString,
      "-adobe-helvetica-medium-r-normal--17-*-iso8859-1"
  },
  {"systemFont5", "SystemFont5", XmRFontList, sizeof (XmFontList), 
      XtOffset(ApplicationDataPtr, fontChoice[4].sysFont), XmRString,
      "-adobe-helvetica-medium-r-normal--18-*-iso8859-1"
  },
  {"systemFont6", "SystemFont6", XmRFontList, sizeof (XmFontList), 
      XtOffset(ApplicationDataPtr, fontChoice[5].sysFont), XmRString,
      "-adobe-helvetica-medium-r-normal--20-*-iso8859-1"
  },
  {"systemFont7", "SystemFont7", XmRFontList, sizeof (XmFontList), 
      XtOffset(ApplicationDataPtr, fontChoice[6].sysFont), XmRString,
      "-adobe-helvetica-medium-r-normal--24-*-iso8859-1"
  },
};

XtResource userFont_resources[] = {

  {"userFont1", "UserFont1", XmRFontList, sizeof (XmFontList), 
      XtOffset(ApplicationDataPtr, fontChoice[0].userFont), XmRString, 
      "-adobe-courier-medium-r-normal--10-*-iso8859-1"
  },
  {"userFont2", "UserFont2", XmRFontList, sizeof (XmFontList), 
      XtOffset(ApplicationDataPtr, fontChoice[1].userFont), XmRString,
      "-adobe-courier-medium-r-normal--12-*-iso8859-1"
  },
  {"userFont3", "UserFont3", XmRFontList, sizeof (XmFontList), 
      XtOffset(ApplicationDataPtr, fontChoice[2].userFont), XmRString,
      "-adobe-courier-medium-r-normal--14-*-iso8859-1"
  },
  {"userFont4", "UserFont4", XmRFontList, sizeof (XmFontList), 
      XtOffset(ApplicationDataPtr, fontChoice[3].userFont), XmRString,
      "-adobe-courier-medium-r-normal--17-*-iso8859-1"
  },
  {"userFont5", "UserFont5", XmRFontList, sizeof (XmFontList), 
      XtOffset(ApplicationDataPtr, fontChoice[4].userFont), XmRString,
      "-adobe-courier-medium-r-normal--18-*-iso8859-1"
  },
  {"userFont6", "UserFont6", XmRFontList, sizeof (XmFontList), 
      XtOffset(ApplicationDataPtr, fontChoice[5].userFont), XmRString,
      "-adobe-courier-medium-r-normal--20-*-iso8859-1"
  },
  {"userFont7", "UserFont7", XmRFontList, sizeof (XmFontList), 
      XtOffset(ApplicationDataPtr, fontChoice[6].userFont), XmRString,
      "-adobe-courier-medium-r-normal--24-*-iso8859-1"
  },
};

XtResource sysStr_resources[] = {

  {"systemFont1", "SystemFont1", XmRString, sizeof (XmString), 
      XtOffset(ApplicationDataPtr, fontChoice[0].sysStr), XmRString,
      "-adobe-helvetica-medium-r-normal--10-*-iso8859-1"
  },
  {"systemFont2", "SystemFont2", XmRString, sizeof (XmString), 
      XtOffset(ApplicationDataPtr, fontChoice[1].sysStr), XmRString,
      "-adobe-helvetica-medium-r-normal--12-*-iso8859-1"
  },
  {"systemFont3", "SystemFont3", XmRString, sizeof (XmString), 
      XtOffset(ApplicationDataPtr, fontChoice[2].sysStr), XmRString,
      "-adobe-helvetica-medium-r-normal--14-*-iso8859-1"
  },
  {"systemFont4", "SystemFont4", XmRString, sizeof (XmString), 
      XtOffset(ApplicationDataPtr, fontChoice[3].sysStr), XmRString,
      "-adobe-helvetica-medium-r-normal--17-*-iso8859-1"
  },
  {"systemFont5", "SystemFont5", XmRString, sizeof (XmString), 
      XtOffset(ApplicationDataPtr, fontChoice[4].sysStr), XmRString,
      "-adobe-helvetica-medium-r-normal--18-*-iso8859-1"
  },
  {"systemFont6", "SystemFont6", XmRString, sizeof (XmString), 
      XtOffset(ApplicationDataPtr, fontChoice[5].sysStr), XmRString,
      "-adobe-helvetica-medium-r-normal--20-*-iso8859-1"
  },
  {"systemFont7", "SystemFont7", XmRString, sizeof (XmString), 
      XtOffset(ApplicationDataPtr, fontChoice[6].sysStr), XmRString,
      "-adobe-helvetica-medium-r-normal--24-*-iso8859-1"
  },
};

XtResource userStr_resources[] = {

  {"userFont1", "UserFont1", XmRString, sizeof (XmString), 
      XtOffset(ApplicationDataPtr, fontChoice[0].userStr), XmRString,
      "-adobe-courier-medium-r-normal--10-*-iso8859-1"
  },
  {"userFont2", "UserFont2", XmRString, sizeof (XmString), 
      XtOffset(ApplicationDataPtr, fontChoice[1].userStr), XmRString,
      "-adobe-courier-medium-r-normal--12-*-iso8859-1"
  },
  {"userFont3", "UserFont3", XmRString, sizeof (XmString), 
      XtOffset(ApplicationDataPtr, fontChoice[2].userStr), XmRString,
      "-adobe-courier-medium-r-normal--14-*-iso8859-1"
  },
  {"userFont4", "UserFont4", XmRString, sizeof (XmString), 
      XtOffset(ApplicationDataPtr, fontChoice[3].userStr), XmRString,
      "-adobe-courier-medium-r-normal--17-*-iso8859-1"
  },
  {"userFont5", "UserFont5", XmRString, sizeof (XmString), 
      XtOffset(ApplicationDataPtr, fontChoice[4].userStr), XmRString,
      "-adobe-courier-medium-r-normal--18-*-iso8859-1"
  },
  {"userFont6", "UserFont6", XmRString, sizeof (XmString), 
      XtOffset(ApplicationDataPtr, fontChoice[5].userStr), XmRString,
      "-adobe-courier-medium-r-normal--20-*-iso8859-1"
  },
  {"userFont7", "UserFont7", XmRString, sizeof (XmString), 
      XtOffset(ApplicationDataPtr, fontChoice[6].userStr), XmRString,
      "-adobe-courier-medium-r-normal--24-*-iso8859-1"
  },
};

XtResource resources[] = {

  {"numFonts", "NumFonts", XmRInt, sizeof (int), 
      XtOffset(ApplicationDataPtr, numFonts), XmRImmediate, (caddr_t) 7
  },
  {"systemFont", "SystemFont", XmRFontList, sizeof (XmFontList), 
      XtOffset(ApplicationDataPtr, systemFont), XmRString, "Fixed"
  },
  {"userFont", "UserFont", XmRFontList, sizeof (XmFontList), 
      XtOffset(ApplicationDataPtr, userFont), XmRString, "Fixed"
  },
  {"systemFont", "SystemFont", XmRString, sizeof (XmString), 
      XtOffset(ApplicationDataPtr, systemFontStr), XmRString, DEF_FONT
  },
  {"userFont", "UserFont", XmRString, sizeof (XmString), 
      XtOffset(ApplicationDataPtr, userFontStr), XmRString, DEF_FONT
  },

  {"session", "Session", XmRString, sizeof (char *),
      XtOffset (ApplicationDataPtr, session), XmRImmediate, (XtPointer)NULL,
  },
  {"backdropDirectories", "BackdropDirectories", XmRString, sizeof(char *),
        XtOffset(ApplicationDataPtr, backdropDir), XmRString, NULL 
  },
  {"paletteDirectories", "PaletteDirectories", XmRString, sizeof(char *),
        XtOffset(ApplicationDataPtr, paletteDir), XmRString, NULL
  },
  {"timeoutScale", "TimeoutScale", XmRString, sizeof (String),
        XtOffset(ApplicationDataPtr, timeoutScale), XmRString, "10" 
  },
  {"lockoutScale", "LockoutScale", XmRString, sizeof (String),
        XtOffset(ApplicationDataPtr, lockoutScale), XmRString, "30" 
  },
  {"writeXrdbImmediate", "WriteXrdbImmediate", XmRBoolean, sizeof(Boolean) ,
        XtOffset(ApplicationDataPtr, writeXrdbImmediate), XmRImmediate, (XtPointer)True
  },
  {"writeXrdbColors", "WriteXrdbColors", XmRBoolean, sizeof(Boolean) ,
        XtOffset(ApplicationDataPtr, writeXrdbColors), XmRImmediate, (XtPointer)True
  },
  {"componentList", "ComponentList", XtRString, sizeof(String) ,
        XtOffset(ApplicationDataPtr, componentList), XmRImmediate, 
        "Color Font Backdrop Keyboard Mouse Audio Screen Dtwm Startup"
  },
  {"imServerHosts", "ImServerHosts", XmRXmStringTable, sizeof(XmStringTable) ,
        XtOffset(ApplicationDataPtr, imServerHosts), XmRImmediate, 
        NULL
  },
  {"preeditType", "PreeditType", XmRXmStringTable, sizeof(XmStringTable) ,
        XtOffset(ApplicationDataPtr, preeditType), XmRString, 
        "OnTheSpot,OverTheSpot,OffTheSpot,Root"
  },
  {"pipeTimeOut", "PipeTimeOut", XmRInt, sizeof (int), 
      XtOffset(ApplicationDataPtr, pipeTimeOut), XmRImmediate, (caddr_t) 100
  },
};

/************************************************************************
 * GetSysFontResources
 *
 *  Description:
 *  -----------
 *  This function is used to retrieve the Dtstyle System Font resources 
 ************************************************************************/
void
GetSysFontResource(int i)
{
    XtGetApplicationResources(style.shell, &style.xrdb,
                            &sysFont_resources[i],
                              1, NULL, 0);
}

/************************************************************************
 * GetUserFontResources
 *
 *  Description:
 *  -----------
 *  This function is used to retrieve the Dtstyle User Font resources 
 ************************************************************************/
void
GetUserFontResource(int i)
{
    XtGetApplicationResources(style.shell, &style.xrdb,
                            &userFont_resources[i],
                              1, NULL, 0);
}

/************************************************************************
 * GetFontStrResources
 *
 *  Description:
 *  -----------
 ************************************************************************/
static void
GetFontStrResources( void )
{
    int i;

    XtGetApplicationResources(style.shell, &style.xrdb, sysStr_resources,
                              XtNumber(sysStr_resources), NULL, 0);
    XtGetApplicationResources(style.shell, &style.xrdb, userStr_resources,
                              XtNumber(userStr_resources), NULL, 0);
    for (i=0; i<style.xrdb.numFonts; i++) {
      style.xrdb.fontChoice[i].userFont = NULL;
      style.xrdb.fontChoice[i].sysFont = NULL;
    }
}

/************************************************************************
 * GetApplicationResources
 *
 *  Description:
 *  -----------
 *  This function is used to retrieve Dtstyle resources that are 
 * not component-specific.
 ************************************************************************/
void 
GetApplicationResources( void )
{
    XtGetApplicationResources(style.shell, &style.xrdb, resources,
                              XtNumber(resources), NULL, 0);
    GetFontStrResources();

}



