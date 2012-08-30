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
/* $XConsortium: vglogo.c /main/4 1995/10/27 16:17:33 rswiston $ */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        vglogo.c
 **
 **   Project:     HP Visual User Environment (DT)
 **
 **   Description: Routines to create the Corporate logo
 **
 **
 **
 **   (c) Copyright 1987, 1988, 1989 by Hewlett-Packard Company
 **
 **
 **
 ****************************************************************************
 ************************************<+>*************************************/


/***************************************************************************
 *
 *  Includes
 *
 ***************************************************************************/

#include	<stdio.h>
#include 	<X11/Intrinsic.h>
#include 	<X11/StringDefs.h>
#include 	<Xm/Xm.h>
#include	<Xm/DrawingA.h>
#include	<Xm/Frame.h>
#include	<Xm/Label.h>
#include    <Dt/IconFile.h>
#include    <Dt/Icon.h>

#include	"vg.h"
#include	"vglogo.h"	/* pixmap and bitmap for default Login logo   */
# include	"vgmsg.h"





/***************************************************************************
 *
 *  External declarations
 *
 ***************************************************************************/


/***************************************************************************
 *
 *  Procedure declarations
 *
 ***************************************************************************/


/***************************************************************************
 *
 *  Global variables
 *
 ***************************************************************************/

        LogoInfo logoInfo;	/* logo resources			   */

static	XtResource logoResources[] = {
    {"bitmapFile", "BitmapFile",
	XtRString, sizeof(char *), XtOffset(LogoInfoPtr, bitmapFile),
#ifdef __osf__
        XtRString, "DECDtlogo"                                          },
#else
	XtRString, "Dtlogo"						},
#endif
};

 
/***************************************************************************
 *
 *  MakeLogo
 *
 *  make DT logo
 *
 *  Widgets: logo, logo_pixmap
 ***************************************************************************/

void 
MakeLogo( void )
{
    register int i;

    char	*logoFile;		/* name of logo bitmap file	   */
    
    Pixmap	logoPixmap;		/* logo pixmap			   */
    char	*logoName;		/* logo name			   */

    int		logoWidth, logoHeight;	/* width, height of logo	   */
    Pixel	fg, bg;			/* foreground, background colors   */

    Pixmap	dsPixmap;		/* drop shadow pixmap		   */
    int		dsWidth, dsHeight;	/* width, height of drop shadow    */

    Pixmap		pixmap;			/* scratch pixmap	   */
    GC		gc;			/* scratch GC		   */
    XGCValues	gcval;			/* GC values		   */
    unsigned int	width, height;		/* width, height of bitmap */
    int		x_hot, y_hot;		/* bitmap hot spot (if any)*/

    
    /*
     *  get the user's logo preferences...
     */
     
    XtGetSubresources(table, &logoInfo, "logo", "Logo",
	logoResources, XtNumber(logoResources), NULL, 0);

    /*
     *  create the logo frame...
     */

    i = InitArg(Frame);
	XtSetArg(argt[i], XmNshadowType, XmSHADOW_OUT); i++;
    XtSetArg(argt[i], XmNshadowThickness, 2); i++; 
    XtSetArg(argt[i], XmNtopAttachment, XmATTACH_FORM); i++;
    XtSetArg(argt[i], XmNtopOffset, 15); i++;
    XtSetArg(argt[i], XmNbottomAttachment, XmATTACH_FORM); i++;
    XtSetArg(argt[i], XmNbottomOffset, 15); i++;
	XtSetArg(argt[i], XmNrightAttachment, XmATTACH_FORM); i++;
	XtSetArg(argt[i], XmNrightOffset, 15); i++;
    XtSetArg(argt[i], XmNleftAttachment, XmATTACH_WIDGET); i++;
    XtSetArg(argt[i], XmNleftWidget, matteFrame); i++;
    logo1 = XmCreateFrame(matte, "logo", argt, i); 
    XtManageChild(logo1);


    /*
     *  get the colors of the frame...
     */

    XtSetArg(argt[0], XmNforeground, &fg);
    XtSetArg(argt[1], XmNbackground, &bg);
    XtGetValues(logo1, argt, 2);
    

    /*
     *  create the logo pixmap...
     */

    logoFile = logoInfo.bitmapFile;

#if defined (_AIX) && defined (_POWER)
/*
 * On AIX4 we have a Dtlogo.s.pm
 */
# define LOGO_TYPE (LOWRES ? DtSMALL : 0)
#else 
# define LOGO_TYPE 0
#endif
    
    logoName = _DtGetIconFileName(DefaultScreenOfDisplay(dpyinfo.dpy), 
        logoFile, NULL, NULL, LOGO_TYPE);

    if (logoName == NULL)
    {
	LogError(
		ReadCatalog(MC_LOG_SET,MC_LOG_NO_LOGOBIT,MC_DEF_LOG_NO_LOGOBIT),
		logoFile);
	logoFile = NULL;
    }


    /*
     *  create the logo control...
     */

    i = InitArg(LabelG);
    XtSetArg(argt[i], XmNmarginWidth, 0); i++;
    XtSetArg(argt[i], XmNmarginHeight, 0); i++;
    XtSetArg(argt[i], XmNhighlightThickness, 0); i++;
    XtSetArg(argt[i], XmNbehavior, XmICON_LABEL); i++;
    XtSetArg(argt[i], XmNfillMode, XmFILL_TRANSPARENT); i++;
    XtSetArg(argt[i], XmNstring, NULL); i++;
    if (logoName != NULL)
    {
        XtSetArg(argt[i], XmNpixmapForeground, fg); i++;
        XtSetArg(argt[i], XmNpixmapBackground, bg); i++;
        XtSetArg(argt[i], XmNimageName, logoName); i++;
    }
    else
    {
        /*
         *  use built-in logo if no logo was not found
         */
     
        logoPixmap = XCreatePixmapFromBitmapData(
                                dpyinfo.dpy,		/* display	   */
                                dpyinfo.root,		/* drawable	   */
                                (char *)dt_logo_bits,	/* data		   */
				dt_logo_width,          /* width	   */
                                dt_logo_height,	        /* height	   */
                                fg,			/* foreground	   */
                                bg,			/* background	   */
                                dpyinfo.depth);		/* depth	   */

        logoWidth  = dt_logo_width;
        logoHeight = dt_logo_height;

        XtSetArg(argt[i], XmNpixmap, logoPixmap); i++;
    }

    	logo_pixmap = _DtCreateIcon(logo1, "logo_pixmap", argt, i);
    	XtManageChild(logo_pixmap);

        XtSetArg(argt[0], XmNheight,  245); /* keeps dialog a consistent height and width */
        XtSetArg(argt[1], XmNwidth,   245);
        XtSetValues(logo1, argt, 2);
}
