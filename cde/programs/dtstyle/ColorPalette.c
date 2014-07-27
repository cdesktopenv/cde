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
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        ColorPalette.c
 **
 **   Project:     DT 3.0
 **
 **   Description: Controls the Dtstyle Color Palette data
 **
 **
 **  (c) Copyright Hewlett-Packard Company, 1990.  
 **
 **
 **
 ****************************************************************************
 ************************************<+>*************************************/
/* $XConsortium: ColorPalette.c /main/6 1995/10/30 13:08:58 rswiston $ */
/*+++++++++++++++++++++++++++++++++++++++*/
/* include files                         */
/*+++++++++++++++++++++++++++++++++++++++*/

#include <X11/Xlib.h>

#include <Xm/Xm.h>

#include "Main.h"
#include "ColorMain.h"

/*+++++++++++++++++++++++++++++++++++++++*/
/* include extern functions              */
/*+++++++++++++++++++++++++++++++++++++++*/
#include "ColorPalette.h"

/*+++++++++++++++++++++++++++++++++++++++*/
/* Internal Variables                    */
/*+++++++++++++++++++++++++++++++++++++++*/
extern Pixmap BGPixmap;

/*************************************************************************
**  Allocate colors for the 8 color sets within a palette, if HIGH_COLOR.
**  Will have to do major checking for low color
**  moitors.  Setting the allocated pixels to pCurrentPalette.
**
**  NOTE: for now I am allocating them all, In future we will want to
**        be more careful about our allocations.
**************************************************************************/
Bool 
AllocatePaletteCells(
        Widget shell )
{
    int i;
    int         colorUse;
    XmPixelSet  pixels[XmCO_MAX_NUM_COLORS];
    int         j=0;
    XColor      colors[XmCO_MAX_NUM_COLORS * 5];

    if(style.count > 9)
       return 0;

    XmeGetPixelData (style.screenNum, &colorUse, pixels,
                                   &(pCurrentPalette->active),
                                   &(pCurrentPalette->inactive),
                                   &(pCurrentPalette->primary),
                                   &(pCurrentPalette->secondary));
	  
    for(i = 0; i < XmCO_MAX_NUM_COLORS; i++)
    {
       pCurrentPalette->color[i].bg.pixel = pixels[i].bg;
       pCurrentPalette->color[i].sc.pixel = pixels[i].sc;
       pCurrentPalette->color[i].fg.pixel = pixels[i].fg;
       pCurrentPalette->color[i].ts.pixel = pixels[i].ts;
       pCurrentPalette->color[i].bs.pixel = pixels[i].bs;

       if (style.dynamicColor)
       {
          pCurrentPalette->color[i].bg.flags = DoRed | DoGreen | DoBlue;
          pCurrentPalette->color[i].fg.flags = DoRed | DoGreen | DoBlue;
          pCurrentPalette->color[i].ts.flags = DoRed | DoGreen | DoBlue;
          pCurrentPalette->color[i].bs.flags = DoRed | DoGreen | DoBlue;
          pCurrentPalette->color[i].sc.flags = DoRed | DoGreen | DoBlue;

          if(i < pCurrentPalette->num_of_colors)
          {
             colors[j++] =  pCurrentPalette->color[i].bg;
             colors[j++] =  pCurrentPalette->color[i].sc;

             if(FgColor == DYNAMIC)
                colors[j++] =  pCurrentPalette->color[i].fg;

             if(!UsePixmaps)
             {
                colors[j++] =  pCurrentPalette->color[i].ts;
                colors[j++] =  pCurrentPalette->color[i].bs;
             }
          }
       } /* if(TypeOfMonitor != XmCO_BLACK_WHITE) */
    }

    if (style.dynamicColor)
       XStoreColors(style.display, style.colormap, colors, j );

    style.count++;
    return(True);

}

/*************************************************************************
**  ReColorPalette changes to RGB values of the already allocated pixels
**  for the 8 color buttons.  Each color button uses 5 pixels (at least
**  for now.)
**
**  The palette passed has the colors the pixels are going to change to.
**
**************************************************************************/
int 
ReColorPalette( void )
{
    register int     n;
    Arg              args[MAX_ARGS];
    int              i;
    int              j=0;
    XColor           colors[XmCO_MAX_NUM_COLORS * 5];

    for(i = 0; i < XmCO_MAX_NUM_COLORS; i++) 
    {
       pCurrentPalette->primary = pOldPalette->primary;
       pCurrentPalette->secondary = pOldPalette->secondary;
       pCurrentPalette->active = pOldPalette->active;
       pCurrentPalette->inactive = pOldPalette->inactive;

       pCurrentPalette->color[i].bg.pixel = 
                         pOldPalette->color[i].bg.pixel; 
       if(TypeOfMonitor != XmCO_BLACK_WHITE)
       {
          pCurrentPalette->color[i].bg.flags = DoRed | DoGreen | DoBlue;
          if(i < pCurrentPalette->num_of_colors)
             colors[j++] =  pCurrentPalette->color[i].bg;
       }

       pCurrentPalette->color[i].sc.pixel =
                         pOldPalette->color[i].sc.pixel; 
       if(TypeOfMonitor != XmCO_BLACK_WHITE)
       {
          pCurrentPalette->color[i].sc.flags = DoRed | DoGreen | DoBlue;
          if(i < pCurrentPalette->num_of_colors)
             colors[j++] =  pCurrentPalette->color[i].sc;
       }

       pCurrentPalette->color[i].fg.pixel =
                         pOldPalette->color[i].fg.pixel; 
       if(TypeOfMonitor != XmCO_BLACK_WHITE)
       {
          if(FgColor == DYNAMIC)
          {
             pCurrentPalette->color[i].fg.flags = DoRed | DoGreen | DoBlue;
             if(i < pCurrentPalette->num_of_colors)
                colors[j++] =  pCurrentPalette->color[i].fg;
          }
       }

       pCurrentPalette->color[i].ts.pixel =
                         pOldPalette->color[i].ts.pixel; 
       if(TypeOfMonitor != XmCO_BLACK_WHITE)
       {
          if(UsePixmaps == FALSE)
          {
             pCurrentPalette->color[i].ts.flags = DoRed | DoGreen | DoBlue;
             if(i < pCurrentPalette->num_of_colors)
                colors[j++] =  pCurrentPalette->color[i].ts;
          }
       }

       pCurrentPalette->color[i].bs.pixel = 
                         pOldPalette->color[i].bs.pixel; 
       if(TypeOfMonitor != XmCO_BLACK_WHITE)
       {
          if(UsePixmaps == FALSE)
          {
             pCurrentPalette->color[i].bs.flags = DoRed | DoGreen | DoBlue;
             if(i < pCurrentPalette->num_of_colors)
                colors[j++] =  pCurrentPalette->color[i].bs;
          }
       }
    }

    if (TypeOfMonitor != XmCO_BLACK_WHITE)
        XStoreColors(style.display, style.colormap, colors, j );

    return(True);
}

/*************************************************************************
**  CheckMonitor - querry color server for monitor type
**************************************************************************/
void 
CheckMonitor(
        Widget shell )
{
    WaitSelection = TRUE;

    XtGetSelectionValue(shell, XA_CUSTOMIZE, XA_TYPE_MONITOR, show_selection,
                                    (XtPointer)GET_TYPE_MONITOR, CurrentTime);

    XFlush(style.display);

    while(WaitSelection)
          XtAppProcessEvent (XtWidgetToApplicationContext(shell), XtIMAll);

}
