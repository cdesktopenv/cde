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
/* $XConsortium: UI.c /main/7 1996/11/07 16:35:08 rswiston $ */
/*****************************************************************************
 *
 *   File:         UI.c
 *
 *   Project:	    CDE
 *
 *   Description:  This file contains the user interface creation and
 *                 processing code for the CDE front panel
 *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company
 * (c) Copyright 1993, 1994 International Business Machines Corp.
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 * (c) Copyright 1993, 1994 Novell, Inc.
 *
 ****************************************************************************/


#include <Dt/DtP.h>
#include <Dt/DbReader.h>
#include <Dt/Dnd.h>
#include "PanelSP.h"
#include <Dt/Control.h>
#include "Clock.h"
#include <Dt/ControlP.h>
#include <Dt/IconFile.h>
#include <Dt/Icon.h>
#include "Button.h"
#include <Dt/GetDispRes.h>

#include <Xm/Form.h>
#include <Xm/ToggleBG.h>
#include <Xm/MwmUtil.h>
#include <Xm/AtomMgr.h>
#include <Xm/DrawingA.h>
#include <Xm/RowColumn.h>
#include <Xm/SeparatoG.h>
#include <Xm/DialogS.h>
#include <Xm/ColorObjP.h>
#include <Xm/TextF.h>

#include <X11/Xatom.h>
#include <X11/keysymdef.h>

#include <langinfo.h>

#define MWM_NEED_TIME16
#include "WmBitmap.h"
#include "WmHelp.h"

#include "DataBaseLoad.h"
#include "PopupMenu.h"
#include "UI.h"



/************************************************************************
 *
 *  External and static function declarations.
 *
 ************************************************************************/
 

extern void PushCB (Widget, XtPointer, XtPointer); 
extern void ArrowCB (Widget, XtPointer, XtPointer); 
extern XtTranslations HandleInputTranslations (void);
extern void HandleInputCB (Widget, XtPointer, XtPointer); 
extern void MinimizeInputCB (Widget, XtPointer, XtPointer); 
extern void MenuInputCB (Widget, XtPointer, XtPointer); 
extern void SwitchButtonCB (Widget, XtPointer, XtPointer); 
extern void SubpanelUnmapCB (Widget, XtPointer, XtPointer); 
extern void SubpanelTornEventHandler (Widget, XtPointer, XEvent *, Boolean *);
extern void PushRecallRegister (ControlData *, Boolean);
extern void EmbeddedClientRegister (ControlData *, Boolean);
extern void EmbeddedClientReposition (Widget, Position, Dimension);
extern void DropCB (Widget, XtPointer, XtPointer);
extern void TransferDropCB (Widget, XtPointer, XtPointer);
extern void CustomizeDropCB (Widget, XtPointer, XtPointer);
extern void CustomizeTransferDropCB (Widget, XtPointer, XtPointer);
extern void FrontPanelCreate (Widget);
extern void EmbeddedClientReparent (char *, Widget);
extern void DeleteControlActionList (ControlData *);
extern Boolean CheckOtherMonitorsOn (SubpanelData *);

static void   Initialize (DtPanelShellWidget);
static void   BoxCreate ();
static Widget PanelControlCreate (Widget, char *, String);
static void   MainControlCreate (int);
static void   SwitchCreate (BoxData *);
static void   SubpanelCreate (ControlData *, SubpanelData *);
static void   ControlCreate (Widget, ControlData **, int);
static void   ArrowCreate (Widget, ControlData **, int, Boolean, Boolean);
static void   SetupPushAnimation(ControlData *);
static void   SetupDropAnimation(ControlData *);

void SubpanelControlCreate (SubpanelData *, ControlData *, ControlData *,
				   Widget, Boolean, Boolean);
void SwitchButtonCreate (SwitchData *, Boolean);
void DeleteSubpanelControl (SubpanelData *, ControlData *);



static char DTFP_CLASS_NAME[] = "Frontpanel";
static char DTFP_APP_NAME[] = "frontpanel";

static XtCallbackRec dropCB[] = { {DropCB, NULL}, {NULL, NULL} };
static XtCallbackRec transferDropCB[] = { {TransferDropCB, NULL},{NULL, NULL} };
static XtCallbackRec customizeDropCB[] = { {CustomizeDropCB, NULL}, {NULL, NULL} };
static XtCallbackRec customizeTransferDropCB[] = { {CustomizeTransferDropCB, NULL},{NULL, NULL} };
                                  

/************************************************************************
 *
 *  File local globals.
 *
 ************************************************************************/

String post_arrow_image = NULL;
String unpost_arrow_image = NULL;
String post_monitor_arrow_image = NULL;
String unpost_monitor_arrow_image = NULL;
String blank_arrow_image = NULL;
String dropzone_image = NULL;
String indicator_on_image = NULL;
String indicator_off_image = NULL;
String minimize_normal_image = NULL;
String minimize_selected_image = NULL;
String menu_normal_image = NULL;
String menu_selected_image = NULL;
String handle_image = NULL;

Pixmap minimize_normal_pixmap;
Pixmap minimize_selected_pixmap;
Pixmap menu_normal_pixmap;
Pixmap menu_selected_pixmap;

#define HARD_CODED_PRIMARY 3
#define _WS_HIGH_COLOR_COUNT 4

static int _ws_high_color_map[] = { 3, 5, 6, 7 };
static	Dimension	switch_height = 0;

/************************************************************************
 *
 *  FrontPanelCreate
 *
 ************************************************************************/
 
void
FrontPanelCreate (Widget toplevel)


{
   DtPanelShellWidget panel_shell;
   char * panel_name = (char *) panel.element_values[PANEL_NAME].parsed_value;
   unsigned int display_height;

   Arg al[20];
   int ac;


   display_height = DisplayHeight (XtDisplay (toplevel), 
                                   DefaultScreen (XtDisplay (toplevel)));

   /*   Create panel shell.  */
   
   ac = 0;
   XtSetArg (al[ac], XmNallowShellResize, True); ac++;
   XtSetArg (al[ac], XmNiconY, display_height); ac++;
   XtSetArg (al[ac], XmNmwmDecorations, MWM_DECOR_BORDER); ac++;
   panel.shell =  XtCreatePopupShell (panel_name, dtPanelShellWidgetClass, 
                                      toplevel, al, ac);

   panel_shell = (DtPanelShellWidget) panel.shell;


   /*   Initialize the general data into the panel structure  */

   Initialize (panel_shell);


   /*  Set pixel resources.  */

   ac = 0;
   XtSetArg (al[ac], XmNforeground, panel.inactive_pixel_set->fg);  ac++;
   XtSetArg (al[ac], XmNbackground, panel.inactive_pixel_set->bg);  ac++;
   XtSetArg (al[ac], XmNtopShadowColor, panel.inactive_pixel_set->ts);  ac++;
   XtSetArg (al[ac], XmNbottomShadowColor, panel.inactive_pixel_set->bs);  ac++;
   XtSetArg (al[ac], XmNselectColor, panel.inactive_pixel_set->sc);  ac++;


   /*  Remove all of the tab groups  */

   XtSetArg (al[ac], XmNnavigationType, XmNONE);  ac++;

   /*  Create the outer form widget that will contain the entire panel  */
   
   panel.form = XmCreateForm (panel.shell, panel_name, al, ac);
   XtManageChild (panel.form);
   
   /*  Create the handles, menu and iconify  */
   
   if ((Boolean) panel.element_values[PANEL_DISPLAY_HANDLES].parsed_value)
   {
      XtTranslations handle_translations = HandleInputTranslations();

      panel.left_handle = PanelControlCreate (panel.form, "handle", handle_image);
      panel.right_handle = PanelControlCreate (panel.form, "handle", handle_image);

      XtOverrideTranslations(panel.left_handle, handle_translations);
      XtAddCallback (panel.left_handle, XmNinputCallback,
                     (XtCallbackProc) HandleInputCB, NULL);
      XtAddCallback (panel.left_handle, XmNhelpCallback,
                     (XtCallbackProc) GeneralTopicHelpCB, PANEL_HANDLE);

      XtOverrideTranslations(panel.right_handle, handle_translations);
      XtAddCallback (panel.right_handle, XmNinputCallback,
                     (XtCallbackProc) HandleInputCB, NULL);
      XtAddCallback (panel.right_handle, XmNhelpCallback,
                     (XtCallbackProc) GeneralTopicHelpCB, PANEL_HANDLE);

      if ((Boolean) panel.element_values[PANEL_DISPLAY_MENU].parsed_value)
      {
         panel.menu = PanelControlCreate (panel.form, "menu", menu_normal_image);

         XtAddCallback (panel.menu, XmNinputCallback,
                        (XtCallbackProc) MenuInputCB, NULL);
         XtAddCallback (panel.menu, XmNhelpCallback,
                     (XtCallbackProc) GeneralTopicHelpCB, PANEL_MENU);

         ac = 0;
         XtSetArg (al[ac], XmNtopAttachment, XmATTACH_FORM);  ac++;
         XtSetArg (al[ac], XmNtopOffset, 1);  ac++;
         XtSetArg (al[ac], XmNleftAttachment, XmATTACH_FORM);  ac++;
         XtSetArg (al[ac], XmNleftOffset, 1);  ac++;
         if (panel.resolution == HIGH || panel.resolution == MEDIUM) {
            XtSetArg (al[ac], XmNwidth, 21);  ac++;
            XtSetArg (al[ac], XmNheight, 15);  ac++;
         } else {
            XtSetArg (al[ac], XmNwidth, 17);  ac++;
            XtSetArg (al[ac], XmNheight, 13);  ac++;
         }
         XtSetValues (panel.menu, al, ac);

         ac = 0;
         XtSetArg (al[ac], XmNtopAttachment, XmATTACH_WIDGET);  ac++;
         XtSetArg (al[ac], XmNtopWidget, panel.menu);  ac++;
         XtSetArg (al[ac], XmNleftAttachment, XmATTACH_FORM);  ac++;
         XtSetArg (al[ac], XmNleftOffset, 1);  ac++;
         XtSetArg (al[ac], XmNbottomAttachment, XmATTACH_FORM);  ac++;
         XtSetArg (al[ac], XmNbottomOffset, 1);  ac++;
         if (panel.resolution == HIGH || panel.resolution == MEDIUM) {
            XtSetArg (al[ac], XmNwidth, 20);  ac++;
         } else {
            XtSetArg (al[ac], XmNwidth, 16);  ac++;
         }
         XtSetValues (panel.left_handle, al, ac);
      }
      else
      {
         panel.menu = NULL;

         ac = 0;
         XtSetArg (al[ac], XmNtopAttachment, XmATTACH_FORM);  ac++;
         XtSetArg (al[ac], XmNtopOffset, 1);  ac++;
         XtSetArg (al[ac], XmNleftAttachment, XmATTACH_FORM);  ac++;
         XtSetArg (al[ac], XmNleftOffset, 1);  ac++;
         XtSetArg (al[ac], XmNbottomAttachment, XmATTACH_FORM);  ac++;
         XtSetArg (al[ac], XmNbottomOffset, 1);  ac++;
         if (panel.resolution == HIGH || panel.resolution == MEDIUM) {
            XtSetArg (al[ac], XmNwidth, 20);  ac++;
         } else {
            XtSetArg (al[ac], XmNwidth, 16);  ac++;
         }
         XtSetValues (panel.left_handle, al, ac);
      }

      if ((Boolean) panel.element_values[PANEL_DISPLAY_MINIMIZE].parsed_value)
      {
         panel.iconify = 
	    PanelControlCreate (panel.form, "minimize", minimize_normal_image);

         XtAddCallback (panel.iconify, XmNinputCallback,
                        (XtCallbackProc) MinimizeInputCB, NULL);
         XtAddCallback (panel.iconify, XmNhelpCallback,
                     (XtCallbackProc) GeneralTopicHelpCB, PANEL_ICONIFY);

         ac = 0;
         XtSetArg (al[ac], XmNtopAttachment, XmATTACH_FORM);  ac++;
         XtSetArg (al[ac], XmNtopOffset, 1);  ac++;
         XtSetArg (al[ac], XmNrightAttachment, XmATTACH_FORM);  ac++;
         XtSetArg (al[ac], XmNrightOffset, 1);  ac++;
         if (panel.resolution == HIGH || panel.resolution == MEDIUM) {
            XtSetArg (al[ac], XmNwidth, 21);  ac++;
            XtSetArg (al[ac], XmNheight, 15);  ac++;
         } else {
            XtSetArg (al[ac], XmNwidth, 17);  ac++;
            XtSetArg (al[ac], XmNheight, 13);  ac++;
         }
         XtSetValues (panel.iconify, al, ac);

         ac = 0;
         XtSetArg (al[ac], XmNtopAttachment, XmATTACH_WIDGET);  ac++;
         XtSetArg (al[ac], XmNtopWidget, panel.iconify);  ac++;
         XtSetArg (al[ac], XmNrightAttachment, XmATTACH_FORM);  ac++;
         XtSetArg (al[ac], XmNrightOffset, 1);  ac++;
         XtSetArg (al[ac], XmNbottomAttachment, XmATTACH_FORM);  ac++;
         XtSetArg (al[ac], XmNbottomOffset, 1);  ac++;
         if (panel.resolution == HIGH || panel.resolution == MEDIUM) {
            XtSetArg (al[ac], XmNwidth, 20);  ac++;
         } else {
            XtSetArg (al[ac], XmNwidth, 16);  ac++;
         }
         XtSetValues (panel.right_handle, al, ac);
      }
      else
      {
         panel.iconify = NULL;

         ac = 0;
         XtSetArg (al[ac], XmNtopAttachment, XmATTACH_FORM);  ac++;
         XtSetArg (al[ac], XmNtopOffset, 1);  ac++;
         XtSetArg (al[ac], XmNrightAttachment, XmATTACH_FORM);  ac++;
         XtSetArg (al[ac], XmNrightOffset, 1);  ac++;
         XtSetArg (al[ac], XmNbottomAttachment, XmATTACH_FORM);  ac++;
         XtSetArg (al[ac], XmNbottomOffset, 1);  ac++;
         if (panel.resolution == HIGH || panel.resolution == MEDIUM) {
            XtSetArg (al[ac], XmNwidth, 20);  ac++;
         } else {
            XtSetArg (al[ac], XmNwidth, 16);  ac++;
         }
         XtSetValues (panel.right_handle, al, ac);
      }
   }
   else
   {
      panel.left_handle = NULL;
      panel.right_handle = NULL;
   }


   /*  Create all of the boxes.  This function, in turn, creates  */
   /*  all of the controls, switch, subpanels.                    */
   
   BoxCreate ();   


   /*   Manage the front panel to get it created and layed out  */

   XtSetMappedWhenManaged (panel.shell, False);
   XtManageChild (panel.shell);
}




/************************************************************************
 *
 *  Initialize
 *	Get all of the default data needed for the panel and put it into
 *	the panel structure.
 *
 ************************************************************************/
 
static void 
Initialize (DtPanelShellWidget panel_shell)


{
   Screen  * screen = XtScreen (panel_shell);
   Display * display = XtDisplay (panel_shell);

   Pixmap busy_pixmap;
   Pixmap busy_pixmap_mask;
   XColor xcolors[2];

   Pixel black_pixel = BlackPixelOfScreen (screen);
   Pixel white_pixel = WhitePixelOfScreen (screen);

   Boolean use_mask_return;
   Boolean use_icon_file_cache_return;

   short active, inactive, primary, secondary;

   int color_use, resolution;
   unsigned int size;
    

   /*  Create busy cursor.  */

   xcolors[0].pixel = black_pixel;
   xcolors[1].pixel = white_pixel;

   XQueryColors (display, DefaultColormapOfScreen (screen), xcolors, 2);
   busy_pixmap = 
      XCreateBitmapFromData (display, RootWindowOfScreen (screen),
                             (char *)time16_bits, time16_width, time16_height);
   busy_pixmap_mask = 
      XCreateBitmapFromData (display, RootWindowOfScreen (screen),
                             (char *)time16m_bits, time16_width, time16_height);

   panel.busy_cursor = 
      XCreatePixmapCursor (display, busy_pixmap, busy_pixmap_mask,
                           &(xcolors[0]), &(xcolors[1]),
                           time16_x_hot, time16_y_hot);

   XFreePixmap (display, busy_pixmap);
   XFreePixmap (display, busy_pixmap_mask);

   
   panel.resolution = (int) panel.element_values[PANEL_RESOLUTION].parsed_value;

   /*   Extract and validate resolution.  */
   
   if (panel.resolution == MATCH_DISPLAY ||
       (panel.resolution != HIGH &&
	panel.resolution != MEDIUM &&
	panel.resolution != LOW))
   {
      resolution =
          _DtGetDisplayResolution (display, XScreenNumberOfScreen(screen));

      switch (resolution)
      {
      	case HIGH_RES_DISPLAY:
           	panel.resolution = HIGH;
      	break;
     	 
      	case MED_RES_DISPLAY:
           	panel.resolution = MEDIUM;
      	break;
     	 
      	case LOW_RES_DISPLAY:
      	case NO_RES_DISPLAY:
           	panel.resolution = LOW;
      	break;
      }	
   }

   if (panel.resolution == HIGH || panel.resolution == MEDIUM)
   {
      panel.main_icon_size = DtLARGE;
      panel.sub_icon_size = DtMEDIUM;
      panel.switch_icon_size = DtSMALL;
   }
   else
   {
      panel.main_icon_size = DtMEDIUM;
      panel.sub_icon_size = DtTINY;
      panel.switch_icon_size = DtTINY;
   }


   /*  Set the font list based on the screen resolution  */

   switch(panel.resolution) {
   	case HIGH:
      		panel.font_list = S_HighResFontList (panel_shell);
      		panel.date_font_list = S_MediumResFontList (panel_shell);
		break;

	case MEDIUM:
      		panel.font_list = S_MediumResFontList (panel_shell);
      		panel.date_font_list = S_MediumResFontList (panel_shell);
	 	break;

	case LOW:
      		panel.font_list = S_LowResFontList (panel_shell);
      		panel.date_font_list = S_LowResFontList (panel_shell);
		break;
   }

   /* initialize popup_data to NULL */
   panel.popup_data = NULL; 

   /*  See if using bitonal or multicolor icons.  If bitonal, set the  */
   /*  pixel set to black and white based on the colors of the parent  */
   /*  If color, use the color obj to get the color pixel set.         */

   XmeGetIconControlInfo (screen,
			  &use_mask_return, &panel.use_color_icons,
			  &use_icon_file_cache_return);

   panel.pixel_set = (XmPixelSet *) 
     XtMalloc (sizeof(XmPixelSet) * XmCO_NUM_COLORS);

   if (XmeGetPixelData (XScreenNumberOfScreen (screen), 
			&color_use, panel.pixel_set, 
			&active, &inactive, &primary, &secondary))
   {
      panel.color_use = color_use;
      panel.active_pixel_set = &(panel.pixel_set[active]);
      panel.inactive_pixel_set = &(panel.pixel_set[inactive]);
      panel.primary_pixel_set = &(panel.pixel_set[primary]);
      panel.secondary_pixel_set = &(panel.pixel_set[secondary]);
   }
   else
   {			
      XtFree ((char *) panel.pixel_set);
   
      panel.pixel_set = (XmPixelSet *) XtMalloc (sizeof (XmPixelSet));
      panel.active_pixel_set = &(panel.pixel_set[0]);
      panel.pixel_set_count = 1;

      panel.active_pixel_set->bg = panel_shell->core.background_pixel;

      if (panel.active_pixel_set->bg == black_pixel)
      {
         panel.active_pixel_set->fg = white_pixel;
         panel.active_pixel_set->bg = black_pixel;
         panel.active_pixel_set->ts = white_pixel;
         panel.active_pixel_set->bs = white_pixel;
         panel.active_pixel_set->sc = black_pixel;
	 panel.color_use = XmCO_BLACK_WHITE;
      }
      else
      {
	 if (panel.active_pixel_set->bg == white_pixel)
         {
	    panel.active_pixel_set->fg = black_pixel;
	    panel.active_pixel_set->bg = white_pixel;
	    panel.active_pixel_set->ts = black_pixel;
	    panel.active_pixel_set->bs = black_pixel;
	    panel.active_pixel_set->sc = white_pixel;
	    panel.color_use = XmCO_BLACK_WHITE;
         }
         else
         {
           /*  Get the Motif defaults and assign into a single allocated  */
           /*  pixel set which is then referenced throught the other 3    */
           /*  pixel set datas.                                           */

	    XmGetColors (screen, DefaultColormapOfScreen (screen), 
			 panel.active_pixel_set->bg,
			 &(panel.active_pixel_set->fg), 
			 &(panel.active_pixel_set->ts),
			 &(panel.active_pixel_set->bs), 
			 &(panel.active_pixel_set->sc));

	    panel.color_use = XmCO_LOW_COLOR;
         }
      }

      panel.inactive_pixel_set = panel.active_pixel_set;
      panel.primary_pixel_set = panel.active_pixel_set;
      panel.secondary_pixel_set = panel.active_pixel_set;
   }


   /*  Initialize other panel specific information  */
   
   panel.busy_light_data = NULL;

   panel.push_recall_list = NULL;
   panel.push_recall_count = 0;
   panel.max_push_recall_count = 0;

   panel.embedded_client_list = NULL;
   panel.embedded_client_count = 0;
   panel.max_embedded_client_count = 0;
   
   panel.dynamic_data_list = NULL;
   panel.dynamic_data_count = 0;
   panel.max_dynamic_data_count = 0;


   /*  Get the names of the predefined images  */

   /*  Get icon size.  */


  /* main panel icons */
   size = panel.main_icon_size;

   post_arrow_image = GetIconName (UP_ARROW_IMAGE_NAME, size);
   unpost_arrow_image = GetIconName (DOWN_ARROW_IMAGE_NAME, size);
   post_monitor_arrow_image = GetIconName (UP_MONITOR_ARROW_IMAGE_NAME, size);
   unpost_monitor_arrow_image= GetIconName(DOWN_MONITOR_ARROW_IMAGE_NAME, size);
   blank_arrow_image = GetIconName (BLANK_ARROW_IMAGE_NAME, size);
   minimize_normal_image = GetIconName (MINIMIZE_NORMAL_IMAGE_NAME, size);
   minimize_selected_image = GetIconName (MINIMIZE_SELECTED_IMAGE_NAME, size);
   menu_normal_image = GetIconName (MENU_NORMAL_IMAGE_NAME, size);
   menu_selected_image = GetIconName (MENU_SELECTED_IMAGE_NAME, size);
   handle_image = GetIconName (HANDLE_IMAGE_NAME, DtLARGE);

  /* subpanel icons */
   size = panel.sub_icon_size;

   indicator_off_image = GetIconName (INDICATOR_OFF_IMAGE_NAME, size);
   indicator_on_image = GetIconName (INDICATOR_ON_IMAGE_NAME, size);
   dropzone_image = GetIconName (DROPZONE_IMAGE_NAME, size);
}




/************************************************************************
 *
 *  PanelControlCreate
 *	Create the handles, menu or iconify buttons attached to the panel.
 *
 ************************************************************************/

static Widget
PanelControlCreate (Widget parent, 
                    char * control_name,
		    String image_name)


{
   Widget w;
   Pixmap pixmap;

   Pixel fg, bg;

   Arg al[15];
   int ac;

   if (panel.color_use != XmCO_BLACK_WHITE)
   {
     /*      Use background for color set matching.
      *      Use top shadow for clock hand color.
      */
      fg = panel.inactive_pixel_set->ts;
      bg = panel.inactive_pixel_set->bg;
   }
   else
   {
      fg = BlackPixelOfScreen (XtScreen (panel.form));
      bg = WhitePixelOfScreen (XtScreen (panel.form));
   }

   pixmap = XmGetPixmap (XtScreen (parent), image_name, fg, bg);

   if (strcmp (control_name, "minimize") == 0)
   {
      minimize_normal_pixmap = pixmap;
      minimize_selected_pixmap = 
         XmGetPixmap (XtScreen (parent), minimize_selected_image, fg, bg);
   }
   else if (strcmp (control_name, "menu") == 0)
   {
      menu_normal_pixmap = pixmap;
      menu_selected_pixmap = 
         XmGetPixmap (XtScreen (parent), menu_selected_image, fg, bg);
   }


   ac = 0;
   XtSetArg (al[ac], XmNforeground, fg);		ac++;
   XtSetArg (al[ac], XmNbackground, bg);		ac++;
   XtSetArg (al[ac], XmNwidth, 11);			ac++;
   XtSetArg (al[ac], XmNshadowThickness, 0);		ac++;
   XtSetArg (al[ac], XmNborderWidth, 0);		ac++;
   XtSetArg (al[ac], XmNbackgroundPixmap, pixmap);	ac++;
   XtSetArg (al[ac], XmNtraversalOn, False);		ac++;
   XtSetArg (al[ac], XmNnavigationType, XmNONE);	ac++;


   w = XmCreateDrawingArea (parent, control_name, al, ac);
   XtManageChild (w);

   return (w);
}




/************************************************************************
 *
 *  BoxCreate
 *	Create all of the boxes defined within the panel data structure.
 *
 ************************************************************************/

static void
BoxCreate ()


{
   int i, j;
   BoxData * box_data;
   BoxData * switch_box_data = NULL;
   Widget    prev_form;
   int switch_position = POSITION_FIRST;

   Pixmap pixmap;

   Arg al[40];
   int ac;


   /*  Loop through the panels boxes, creating the set outer forms    */
   /*  necessary to hold the inner forms for the controls, arrows...  */

   prev_form = NULL;

   for (i = 0; i < panel.box_data_count; i++)
   {
      box_data = panel.box_data[i];


      /*  Create the outer box form which will contain the inner box set  */
      
      ac = 0;
      XtSetArg (al[ac], XmNshadowThickness, 0);  ac++;

      if (panel.left_handle != NULL)
      {
	 XtSetArg (al[ac], XmNrightAttachment, XmATTACH_WIDGET);  ac++;
	 XtSetArg (al[ac], XmNrightWidget, panel.right_handle);  ac++;
	 XtSetArg (al[ac], XmNleftAttachment, XmATTACH_WIDGET);  ac++;
	 XtSetArg (al[ac], XmNleftWidget, panel.left_handle);  ac++;
      }
      else
      {
	 XtSetArg (al[ac], XmNrightAttachment, XmATTACH_FORM);  ac++;
	 XtSetArg (al[ac], XmNleftAttachment, XmATTACH_FORM);  ac++;
      }

      if (prev_form != NULL)
      {
	 XtSetArg (al[ac], XmNtopAttachment, XmATTACH_WIDGET);  ac++;
	 XtSetArg (al[ac], XmNtopWidget, prev_form);  ac++;
      }
      else
      {
	 XtSetArg (al[ac], XmNtopAttachment, XmATTACH_FORM);  ac++;
	 XtSetArg (al[ac], XmNtopOffset, 1);  ac++;
      }

      if (i == panel.box_data_count - 1)
      {
	 XtSetArg (al[ac], XmNbottomAttachment, XmATTACH_FORM);  ac++;
	 XtSetArg (al[ac], XmNbottomOffset, 1);  ac++;
      }

      if (panel.color_use == XmCO_BLACK_WHITE)
      {
	 XtSetArg (al[ac], XmNbottomShadowColor,
                           BlackPixelOfScreen (XtScreen (panel.shell))); ac++;
      }

      /*  Remove all of the tab groups  */

      XtSetArg (al[ac], XmNnavigationType, XmNONE);  ac++;


      XtSetArg (al[ac], XmNbackground, panel.primary_pixel_set->bg);	ac++;
      XtSetArg (al[ac], XmNforeground, panel.primary_pixel_set->fg);	ac++;

      /*  Create the outer form widget that will contain the entire panel  */
   
      box_data->form = 
         XmCreateForm (panel.form, 
                       (char *) box_data->element_values[BOX_NAME].parsed_value,
                        al, ac);
      XtManageChild (box_data->form);

      prev_form = box_data->form;
   }


   /*  Loop through the panels boxes, creating the set of forms needed    */
   /*  to contain the controls, arrows, or switch within the front panel  */

   for (i = 0; i < panel.box_data_count; i++)
   {
      box_data = panel.box_data[i];

      if (box_data->switch_data != NULL)
      {
         switch_box_data = box_data;
	 
         ac = 0;
	 XtSetArg (al[ac], XmNshadowThickness, 1);  ac++;
	 XtSetArg (al[ac], XmNtopAttachment, XmATTACH_FORM);  ac++;
	 XtSetArg (al[ac], XmNtopOffset, 0);  ac++;
	 XtSetArg (al[ac], XmNbottomAttachment, XmATTACH_FORM);  ac++;
	 XtSetArg (al[ac], XmNbottomOffset, 0);  ac++;

         switch_position = 
	    (int) box_data->switch_data->element_values[SWITCH_POSITION_HINTS].parsed_value;


         /*  Determine if the defined switch position is actually the  */
         /*  first or last position.                                   */

         if (switch_position != POSITION_FIRST || switch_position != POSITION_LAST)
         {
	    for (j = 0; j < box_data->control_data_count; j++)
            {
	       if (switch_position < (int) ((box_data->control_data[j])->element_values[CONTROL_POSITION_HINTS].parsed_value))
                  break;
            }

            if (j == 0)
               switch_position = POSITION_FIRST;
            else if (j == box_data->control_data_count)
               switch_position = POSITION_LAST;
         }

         if (switch_position == POSITION_FIRST)
	 {
            XtSetArg (al[ac], XmNleftAttachment, XmATTACH_FORM);  ac++;
            XtSetArg (al[ac], XmNleftOffset, 0);  ac++;
	 }

         if (switch_position == POSITION_LAST ||
	     box_data->control_data_count == 0)
	 {
            XtSetArg (al[ac], XmNrightAttachment, XmATTACH_FORM);  ac++;
            XtSetArg (al[ac], XmNrightOffset, 0);  ac++;
	 }

         XtSetArg (al[ac], XmNuserData, SWITCH); ac++;


         /*  Remove all of the tab groups  */

         XtSetArg (al[ac], XmNnavigationType, XmNONE);  ac++;

         if (panel.color_use == XmCO_BLACK_WHITE)
         {
            XtSetArg (al[ac], XmNbottomShadowColor,
                              BlackPixelOfScreen (XtScreen (panel.form))); ac++;

            pixmap = XmGetPixmap (XtScreen (panel.form), "50_foreground",
                                  panel.inactive_pixel_set->fg,
                                  panel.inactive_pixel_set->bg);

            XtSetArg (al[ac], XmNbackgroundPixmap, pixmap);  ac++;
         }

         XtSetArg (al[ac], XmNbackground, panel.primary_pixel_set->bg);	ac++;
         XtSetArg (al[ac], XmNforeground, panel.primary_pixel_set->fg);	ac++;

         box_data->switch_form =  
	    XmCreateForm (box_data->form, "switch", al, ac);
         XtManageChild (box_data->switch_form);

         XtAddCallback (box_data->switch_form, XmNhelpCallback,
                        (XtCallbackProc) SwitchTopicHelpCB,
                        box_data->switch_data);

         if (box_data->control_data_count == 0) 
	    continue;
      }


      /*  Create the arrow boxes  */

      if (box_data->switch_form != NULL)
      {
	 if (switch_position == POSITION_FIRST)
	 {
	    ac = 0;
	    XtSetArg (al[ac], XmNleftAttachment, XmATTACH_WIDGET);  ac++;
	    XtSetArg (al[ac], XmNleftWidget, box_data->switch_form);  ac++;
	    XtSetArg (al[ac], XmNleftOffset, -4);  ac++;
	    XtSetArg (al[ac], XmNtopAttachment, XmATTACH_FORM);  ac++;
	    XtSetArg (al[ac], XmNtopOffset, 0);  ac++;
	    XtSetArg (al[ac], XmNrightAttachment, XmATTACH_FORM);  ac++;
	    XtSetArg (al[ac], XmNrightOffset, 0);  ac++;

	 }
	 else if (switch_position == POSITION_LAST)
	 {
	    ac = 0;
	    XtSetArg (al[ac], XmNleftAttachment, XmATTACH_FORM);  ac++;
	    XtSetArg (al[ac], XmNleftOffset, 0);  ac++;
	    XtSetArg (al[ac], XmNtopAttachment, XmATTACH_FORM);  ac++;
	    XtSetArg (al[ac], XmNtopOffset, 0);  ac++;
	    XtSetArg (al[ac], XmNrightAttachment, XmATTACH_WIDGET);  ac++;
	    XtSetArg (al[ac], XmNrightWidget, box_data->switch_form);  ac++;
	    XtSetArg (al[ac], XmNrightOffset, 0);  ac++;

	 }
	 else
	 {
	    ac = 0;
	    XtSetArg (al[ac], XmNleftAttachment, XmATTACH_WIDGET);  ac++;
	    XtSetArg (al[ac], XmNleftWidget, box_data->switch_form);  ac++;
	    XtSetArg (al[ac], XmNleftOffset, -6);  ac++;
	    XtSetArg (al[ac], XmNtopAttachment, XmATTACH_FORM);  ac++;
	    XtSetArg (al[ac], XmNtopOffset, 0);  ac++;
	    XtSetArg (al[ac], XmNrightAttachment, XmATTACH_FORM);  ac++;
	    XtSetArg (al[ac], XmNrightOffset, 0);  ac++;

	    XtSetArg (al[ac], XmNbackground, panel.inactive_pixel_set->bg);	ac++;
	    XtSetArg (al[ac], XmNforeground, panel.inactive_pixel_set->fg);	ac++;
	    XtSetArg (al[ac], XmNtopShadowColor, panel.inactive_pixel_set->ts);	ac++;
	    XtSetArg (al[ac], XmNselectColor, panel.inactive_pixel_set->sc);	ac++;

	    if (panel.color_use == XmCO_BLACK_WHITE)
	    {
	       XtSetArg (al[ac], XmNbottomShadowColor,
			 BlackPixelOfScreen (XtScreen (panel.form))); ac++;

	       pixmap = XmGetPixmap (XtScreen (panel.form), "50_foreground",
				     panel.inactive_pixel_set->fg,
				     panel.inactive_pixel_set->bg);

	       XtSetArg (al[ac], XmNbackgroundPixmap, pixmap);  ac++;
	    }
	    else
	    {
	       XtSetArg (al[ac], XmNbottomShadowColor,
				 panel.inactive_pixel_set->bs); ac++;
	    }


	    /*  Remove all of the tab groups  */

	    XtSetArg (al[ac], XmNnavigationType, XmNONE);  ac++;


            if (i == 0)
            {
                XtSetArg (al[ac], XmNshadowThickness, 0);  ac++;
            }
            else
            {
               XtSetArg (al[ac], XmNshadowThickness, 1);  ac++;  
            }

	    box_data->right_arrow_form = 
	       XmCreateForm (box_data->form, "right_arrow_form", al, ac);

	    if (box_data->subpanel_count != 0)
	       XtManageChild (box_data->right_arrow_form);

	    XtAddCallback (box_data->right_arrow_form, XmNhelpCallback,
			   (XtCallbackProc) GeneralTopicHelpCB,
			   SUBPANEL_ACCESS_AREA);

	    ac = 0;
	    XtSetArg (al[ac], XmNleftAttachment, XmATTACH_FORM);  ac++;
	    XtSetArg (al[ac], XmNleftOffset, 0);  ac++;
	    XtSetArg (al[ac], XmNtopAttachment, XmATTACH_FORM);  ac++;
	    XtSetArg (al[ac], XmNtopOffset, 0);  ac++;
	    XtSetArg (al[ac], XmNrightAttachment, XmATTACH_FORM);  ac++;
	    XtSetArg (al[ac], XmNrightOffset, -2);  ac++;
	 }
      }
      else
      {
	 ac = 0;
	 XtSetArg (al[ac], XmNleftAttachment, XmATTACH_FORM);  ac++;
	 XtSetArg (al[ac], XmNtopAttachment, XmATTACH_FORM);  ac++;
	 XtSetArg (al[ac], XmNrightAttachment, XmATTACH_FORM);  ac++;

         if (i == 0)
         {
            XtSetArg (al[ac], XmNtopOffset, 1);  ac++;
            XtSetArg (al[ac], XmNleftOffset, 1);  ac++;
            XtSetArg (al[ac], XmNrightOffset, 1);  ac++;
         }
         else
         {
            XtSetArg (al[ac], XmNtopOffset, 0);  ac++;
            XtSetArg (al[ac], XmNleftOffset, 0);  ac++;
            XtSetArg (al[ac], XmNrightOffset, 0);  ac++;
         }
      }

      XtSetArg(al[ac], XmNbackground, panel.inactive_pixel_set->bg); ac++;
      XtSetArg(al[ac], XmNforeground, panel.inactive_pixel_set->fg); ac++;
      XtSetArg(al[ac], XmNtopShadowColor, panel.inactive_pixel_set->ts);ac++;
      XtSetArg(al[ac], XmNselectColor, panel.inactive_pixel_set->sc); ac++;

      if (panel.color_use == XmCO_BLACK_WHITE)
      {
	 XtSetArg (al[ac], XmNbottomShadowColor,
		   BlackPixelOfScreen (XtScreen (panel.form))); ac++;

	 pixmap = XmGetPixmap (XtScreen (panel.form), "50_foreground",
			       panel.inactive_pixel_set->fg,
			       panel.inactive_pixel_set->bg);

	 XtSetArg (al[ac], XmNbackgroundPixmap, pixmap);  ac++;
      }
      else
      {
	 XtSetArg (al[ac], XmNbottomShadowColor,
			   panel.inactive_pixel_set->bs); ac++;
      }


      /*  Remove all of the tab groups  */

      XtSetArg (al[ac], XmNnavigationType, XmNONE);  ac++;

      if (i == 0)
      {
         XtSetArg (al[ac], XmNshadowThickness, 0);  ac++;
      }
      else
      {
         XtSetArg (al[ac], XmNshadowThickness, 1);  ac++;  
      }


      box_data->left_arrow_form = 
	 XmCreateForm (box_data->form, "left_arrow_form", al, ac);

      if (box_data->subpanel_count != 0)
	 XtManageChild (box_data->left_arrow_form);

      XtAddCallback (box_data->left_arrow_form, XmNhelpCallback,
		     (XtCallbackProc) GeneralTopicHelpCB,
		     SUBPANEL_ACCESS_AREA);


      /*  Now create the control boxes  */
      
      if (box_data->switch_form != NULL)
      {
         if (switch_position == POSITION_FIRST ||
             switch_position == POSITION_LAST)
         {
            ac = 0;
            XtSetArg (al[ac], XmNshadowThickness, 1);  ac++;

            if (switch_position == POSITION_FIRST)
            {
               XtSetArg (al[ac], XmNleftAttachment, XmATTACH_WIDGET);  ac++;
               XtSetArg (al[ac], XmNleftWidget, box_data->switch_form);  ac++;
               XtSetArg (al[ac], XmNleftOffset, -4);  ac++;
               XtSetArg (al[ac], XmNrightAttachment, XmATTACH_FORM);  ac++;
               XtSetArg (al[ac], XmNrightOffset, 0);  ac++;
            }
            else
            {
               XtSetArg (al[ac], XmNleftAttachment, XmATTACH_FORM);  ac++;
               XtSetArg (al[ac], XmNleftOffset, 0);  ac++;
               XtSetArg (al[ac], XmNrightAttachment, XmATTACH_WIDGET);  ac++;
               XtSetArg (al[ac], XmNrightWidget, box_data->switch_form);  ac++;
               XtSetArg (al[ac], XmNrightOffset, 0);  ac++;
            }

            if (XtIsManaged(box_data->left_arrow_form))
            {
               XtSetArg (al[ac], XmNtopAttachment, XmATTACH_WIDGET);  ac++;
               XtSetArg (al[ac], XmNtopWidget, box_data->left_arrow_form);  ac++;
            }
            else
            {
               XtSetArg (al[ac], XmNtopAttachment, XmATTACH_FORM);  ac++;
               XtSetArg (al[ac], XmNtopOffset, 0);  ac++;
            }

            XtSetArg (al[ac], XmNbottomAttachment, XmATTACH_FORM);  ac++;
            XtSetArg (al[ac], XmNbottomOffset, 0);  ac++;

            XtSetArg (al[ac], XmNuserData, CONTROL); ac++;


            /*  Remove all of the tab groups  */

            XtSetArg (al[ac], XmNnavigationType, XmNONE);  ac++;

            if (panel.color_use == XmCO_BLACK_WHITE)
            {
               pixmap = XmGetPixmap (XtScreen (panel.form), "50_foreground",
                                     panel.inactive_pixel_set->fg,
                                     panel.inactive_pixel_set->bg);

               XtSetArg (al[ac], XmNbackgroundPixmap, pixmap);  ac++;
            }

            XtSetArg (al[ac], XmNbackground, panel.primary_pixel_set->bg); ac++;
            XtSetArg (al[ac], XmNforeground, panel.primary_pixel_set->fg); ac++;

            box_data->left_control_form = 
               XmCreateForm (box_data->form, "left_control_form", al, ac);
            XtManageChild (box_data->left_control_form);
         }
         else
         {
            ac = 0;
            XtSetArg (al[ac], XmNshadowThickness, 1);  ac++;
            XtSetArg (al[ac], XmNleftAttachment, XmATTACH_FORM);  ac++;
            XtSetArg (al[ac], XmNleftOffset, 0);  ac++;

            if (XtIsManaged (box_data->left_arrow_form))
            {
               XtSetArg (al[ac], XmNtopAttachment, XmATTACH_WIDGET);  ac++;
               XtSetArg (al[ac], XmNtopWidget, box_data->left_arrow_form);  ac++;
            }
            else
            {
               XtSetArg (al[ac], XmNtopAttachment, XmATTACH_FORM);  ac++;
               XtSetArg (al[ac], XmNtopOffset, 0);  ac++;
            }

            XtSetArg (al[ac], XmNbottomAttachment, XmATTACH_FORM);  ac++;
            XtSetArg (al[ac], XmNbottomOffset, 0);  ac++;

            XtSetArg (al[ac], XmNuserData, CONTROL); ac++;


            /*  Remove all of the tab groups  */

            XtSetArg (al[ac], XmNnavigationType, XmNONE);  ac++;

            if (panel.color_use == XmCO_BLACK_WHITE)
            {
               pixmap = XmGetPixmap (XtScreen (panel.form), "50_foreground",
                                     panel.inactive_pixel_set->fg,
                                     panel.inactive_pixel_set->bg);

               XtSetArg (al[ac], XmNbackgroundPixmap, pixmap);  ac++;
            }

            XtSetArg (al[ac], XmNbackground, panel.primary_pixel_set->bg); ac++;
            XtSetArg (al[ac], XmNforeground, panel.primary_pixel_set->fg); ac++;

            box_data->left_control_form =
	       XmCreateForm (box_data->form, "left_control_form", al, ac);
            XtManageChild (box_data->left_control_form);
	       
            ac = 0;
            XtSetArg (al[ac], XmNleftAttachment, XmATTACH_WIDGET);         ac++;
            XtSetArg (al[ac], XmNleftWidget, box_data->left_control_form); ac++;
            XtSetArg (al[ac], XmNleftOffset, -3);                          ac++;
            XtSetValues (box_data->switch_form, al, ac);


            ac = 0;
            XtSetArg (al[ac], XmNshadowThickness, 1);  ac++;
            XtSetArg (al[ac], XmNleftAttachment, XmATTACH_WIDGET);  ac++;
            XtSetArg (al[ac], XmNleftWidget, box_data->switch_form);  ac++;
            XtSetArg (al[ac], XmNleftOffset, -4);  ac++;
            XtSetArg (al[ac], XmNrightAttachment, XmATTACH_FORM);  ac++;
            XtSetArg (al[ac], XmNrightOffset, 0);  ac++;

            if (XtIsManaged(box_data->right_arrow_form))
            {
               XtSetArg (al[ac], XmNtopAttachment, XmATTACH_WIDGET);  ac++;
               XtSetArg (al[ac], XmNtopWidget, box_data->right_arrow_form);  ac++;
            }
            else
            {
               XtSetArg (al[ac], XmNtopAttachment, XmATTACH_FORM);  ac++;
               XtSetArg (al[ac], XmNtopOffset, 0);  ac++;
            }

            XtSetArg (al[ac], XmNbottomAttachment, XmATTACH_FORM);  ac++;
            XtSetArg (al[ac], XmNbottomOffset, 0);  ac++;

            XtSetArg (al[ac], XmNuserData, CONTROL); ac++;


            /*  Remove all of the tab groups  */

            XtSetArg (al[ac], XmNnavigationType, XmNONE);  ac++;

            if (panel.color_use == XmCO_BLACK_WHITE)
            {
               pixmap = XmGetPixmap (XtScreen (panel.form), "50_foreground",
                                     panel.inactive_pixel_set->fg,
                                     panel.inactive_pixel_set->bg);

               XtSetArg (al[ac], XmNbackgroundPixmap, pixmap);  ac++;
            }

            XtSetArg (al[ac], XmNbackground, panel.primary_pixel_set->bg); ac++;
            XtSetArg (al[ac], XmNforeground, panel.primary_pixel_set->fg); ac++;

            box_data->right_control_form = 
               XmCreateForm (box_data->form, "right_control_form", al, ac);
            XtManageChild (box_data->right_control_form);
         }
      }
      else
      {
         ac = 0;
         XtSetArg (al[ac], XmNshadowThickness, 1);  ac++;
         XtSetArg (al[ac], XmNleftAttachment, XmATTACH_FORM);  ac++;
         XtSetArg (al[ac], XmNleftOffset, 0);  ac++;
         XtSetArg (al[ac], XmNrightAttachment, XmATTACH_FORM);  ac++;
         XtSetArg (al[ac], XmNrightOffset, 0);  ac++;
         XtSetArg (al[ac], XmNbottomAttachment, XmATTACH_FORM);  ac++;
         XtSetArg (al[ac], XmNbottomOffset, 0);  ac++;

         if (XtIsManaged(box_data->left_arrow_form))
         {
            XtSetArg (al[ac], XmNtopAttachment, XmATTACH_WIDGET);  ac++;
            XtSetArg (al[ac], XmNtopWidget, box_data->left_arrow_form);  ac++;
         }
         else
         {
            XtSetArg (al[ac], XmNtopAttachment, XmATTACH_FORM);  ac++;
            XtSetArg (al[ac], XmNtopOffset, 0);  ac++;
         }

         XtSetArg (al[ac], XmNuserData, CONTROL); ac++;


         /*  Remove all of the tab groups  */

         XtSetArg (al[ac], XmNnavigationType, XmNONE);  ac++;

         if (panel.color_use == XmCO_BLACK_WHITE)
         {
            pixmap = XmGetPixmap (XtScreen (panel.form), "50_foreground",
                                  panel.inactive_pixel_set->fg,
                                  panel.inactive_pixel_set->bg);

            XtSetArg (al[ac], XmNbackgroundPixmap, pixmap);  ac++;
         }

         XtSetArg (al[ac], XmNbackground, panel.primary_pixel_set->bg);	ac++;
         XtSetArg (al[ac], XmNforeground, panel.primary_pixel_set->fg);	ac++;

         box_data->left_control_form = 
            XmCreateForm (box_data->form, "left_control_form", al, ac);
         XtManageChild (box_data->left_control_form);
      }

      if (box_data->left_control_form != NULL)
      {
         if (panel.popup_data == NULL || panel.popup_data->popup == NULL)
	    CreatePopupMenu(box_data->left_control_form);
         else
	    XmAddToPostFromList(panel.popup_data->popup,
				box_data->left_control_form);


         /* Event handler for posting popup menu */

         XtAddEventHandler(box_data->left_control_form, ButtonPressMask, False,
                           (XtEventHandler) PostPopupMenu, (XtPointer) NULL);
      }

      if (box_data->right_control_form != NULL)
      {
         if (panel.popup_data == NULL || panel.popup_data->popup == NULL)
	    CreatePopupMenu(box_data->right_control_form);
         else
	    XmAddToPostFromList(panel.popup_data->popup,
				box_data->right_control_form);


         /* Event handler for posting popup menu */

         XtAddEventHandler(box_data->right_control_form, ButtonPressMask,False,
                           (XtEventHandler) PostPopupMenu, (XtPointer) NULL);
      }
   }


   /*  Once all of the boxes and switch form have been created within  */
   /*  the main panel, call the function to create the main panel      */
   /*  controls.  This function also creates the arrows.               */
   
   MainControlCreate (switch_position);
   
   
   /*  Call a function to create the control and button set for the switch  */
   
   if (switch_box_data != NULL)
      SwitchCreate (switch_box_data);

   for (i = 0; i < panel.box_data_count; i++)
   {
      box_data = panel.box_data[i];

      if (box_data->switch_data != NULL)
      {
         if (box_data->switch_form != NULL)
         {
            if (box_data->switch_data->popup_data == NULL ||
                box_data->switch_data->popup_data->popup == NULL)
               CreateWorkspacePopupMenu (box_data->switch_form,
				         box_data->switch_data);
            else
	       XmAddToPostFromList (box_data->switch_data->popup_data->popup,
			            box_data->switch_form);

             /* Event handler for posting popup menu */

             XtAddEventHandler (box_data->switch_form, ButtonPressMask,
			        False,
				(XtEventHandler) PostWorkspacePopupMenu,
			        (XtPointer) NULL);
         }
      }
   }
}




/************************************************************************
 *
 *  MainControlCreate
 *
 ************************************************************************/
 
static void
MainControlCreate (int switch_position)


{
   int i;
   BoxData * box_data;
   Boolean   first_box;
   Arg al[1];


   /*  Set up a loop to go through each box and create the set of controls  */
   /*  subpanel arrows, and subpanels for each box.                         */

   for (i = 0; i < panel.box_data_count; i++)
   {
      box_data = panel.box_data[i];
      if (i == 0) first_box = True;
      else first_box = False;

      if (box_data->control_data_count == 0)
         continue;


      /*  See if we need to split the control create for the box  */
      /*  because the switch is centered between the controls.    */

      if (switch_position == POSITION_FIRST ||
          switch_position == POSITION_LAST  || 
          box_data->switch_form == NULL)
      {
         ControlCreate (box_data->left_control_form, box_data->control_data,
	                box_data->control_data_count);

         ArrowCreate (box_data->left_arrow_form, box_data->control_data,
	              box_data->control_data_count, False, first_box);
      }
      else
      {
         int j;
	 
	 
         /*  Find the position within the control list that is less than  */
	 /*  the switch position.                                         */
	 
	 for (j = 0; j < box_data->control_data_count; j++)
	 {
	    if (switch_position <
 	        (int) ((box_data->control_data[j])->element_values[CONTROL_POSITION_HINTS].parsed_value))
            {
               ControlCreate (box_data->left_control_form, 
		              &box_data->control_data[0], j);

               ControlCreate (box_data->right_control_form, 
		              &box_data->control_data[j],
                              box_data->control_data_count - j);

               ArrowCreate (box_data->left_arrow_form, 
                            &box_data->control_data[0], j, False, first_box);

               ArrowCreate (box_data->right_arrow_form, 
                            &box_data->control_data[j],
                            box_data->control_data_count - j, True, first_box);
               break;
	    }
	 }
      }
   }
}




/************************************************************************
 *
 *  ControlSetVisualData
 *	Set up the arg list elements for a controls color and font
 *	resources.
 *
 *  Inputs: control_data - a pointer to the control to be created
 *	    al - a pointer to the arg list to contain the resource data
 *          ac - a pointer to the arg list count
 *
 ************************************************************************/
 
static void
ControlSetVisualData (ControlData * control_data,
		      ArgList   al,
                      int     * ac)


{
   XtSetArg (al[*ac], XmNbackground, panel.primary_pixel_set->bg);	(*ac)++;
   XtSetArg (al[*ac], XmNforeground, panel.primary_pixel_set->fg);	(*ac)++;

   if (panel.color_use == XmCO_BLACK_WHITE)
   {
      XtSetArg (al[*ac], XmNuseEmbossedText, False);  (*ac)++;
      XtSetArg (al[*ac], XmNarmColor, panel.primary_pixel_set->bg);   (*ac)++;
   }
   else
   {
      XtSetArg (al[*ac], XmNarmColor, panel.primary_pixel_set->sc);   (*ac)++;
   }

   if (panel.use_color_icons)
   {
     /*      Use background for color set matching.
      *      Use top shadow for clock hand color.
      */
      XtSetArg (al[*ac], XmNpixmapBackground, panel.primary_pixel_set->bg);
      (*ac)++;
      XtSetArg (al[*ac], XmNpixmapForeground, panel.primary_pixel_set->ts);
      (*ac)++;
   }
   else
   {
      if (control_data != NULL &&
        (char) control_data->element_values[CONTROL_TYPE].parsed_value ==
                                                               CONTROL_CLOCK)
      {
         if (panel.color_use != XmCO_BLACK_WHITE)
         {
           XtSetArg (al[*ac], XmNpixmapForeground, panel.primary_pixel_set->fg);
           (*ac)++;
           XtSetArg(al[*ac], XmNpixmapBackground, panel.primary_pixel_set->bg);
           (*ac)++;
         }
         else
         {
           XtSetArg (al[*ac], XmNpixmapForeground,
                        BlackPixelOfScreen (XtScreen (panel.form)));  (*ac)++;
           XtSetArg (al[*ac], XmNpixmapBackground,
                        WhitePixelOfScreen (XtScreen (panel.form)));  (*ac)++;
         }
      }
      else
      {
         XtSetArg (al[*ac], XmNpixmapBackground,
                        WhitePixelOfScreen (XtScreen (panel.form)));  (*ac)++;
         XtSetArg (al[*ac], XmNpixmapForeground,
                        BlackPixelOfScreen (XtScreen (panel.form)));  (*ac)++;
      }
   }

   XtSetArg (al[*ac], XmNtopShadowColor, panel.primary_pixel_set->ts);	(*ac)++;
   XtSetArg (al[*ac], XmNbottomShadowColor, panel.primary_pixel_set->bs);(*ac)++;
   XtSetArg (al[*ac], XmNselectColor, panel.primary_pixel_set->sc);	(*ac)++;

   if ((char) control_data->element_values[CONTROL_TYPE].parsed_value ==
								 CONTROL_DATE)
   {
/*
      XtSetArg (al[*ac], XmNfontList, panel.date_font_list);	(*ac)++;
      XtSetArg (al[*ac], XmNuseEmbossedText, False);		(*ac)++;
      XtSetArg (al[*ac], XmNforeground, 
                         BlackPixelOfScreen (XtScreen (panel.form)));  (*ac)++;
*/
      if (panel.color_use == XmCO_BLACK_WHITE)
      {
         XtSetArg (al[*ac], XmNbackground, 
                         WhitePixelOfScreen (XtScreen (panel.form)));  (*ac)++;
      }
   }
   else
   {
      XtSetArg (al[*ac], XmNfontList, panel.font_list);			(*ac)++;
   }

   XtSetArg (al[*ac], XmNbehavior, XmICON_DRAG);  			(*ac)++;
}




/************************************************************************
 *
 *  ControlSetIconData
 *	Set up the arg list elements for a controls icon and label
 *	resources.
 *
 *  Inputs: parent - the widget to be the parent of the control
 *          control_data - a pointer to the control to be created
 *          icon_label   - a return for an XmString for the control's label
 *          container_type - the type of parent the control is going into
 *          al - a pointer to the arg list to contain the resource data
 *          ac - a pointer to the arg list count
 *	    icon_name - a pointer to the icon name (free after widget
 *                                                  is created)
 *	    alternate_icon_name - a pointer to the icon name (free after widget
 *                                                            is created)
 *
 ************************************************************************/
 
static void
ControlSetIconData (Widget        parent,
                    ControlData * control_data,
                    XmString    * icon_label,
                    int           container_type,
                    ArgList       al,
                    int         * ac,
		    char       ** icon_name,
   		    char       ** alternate_icon_name)


{
   char * control_label;
   int    icon_size;
   

   /*  Set up the icon and alternate icon resources for the control  */
   
   if (container_type == BOX)
      icon_size = panel.main_icon_size;
   else if (container_type == SUBPANEL)
      icon_size = panel.sub_icon_size;
   else
      icon_size = panel.switch_icon_size;


   *icon_name = 
      (char *) control_data->element_values[CONTROL_NORMAL_ICON].parsed_value;

   if (*icon_name != NULL)
   {
      *icon_name = GetIconName (*icon_name, icon_size);
      XtSetArg (al[*ac], XmNimageName, *icon_name);			(*ac)++;
   }


   *alternate_icon_name = 
    (char *) control_data->element_values[CONTROL_ALTERNATE_ICON].parsed_value;

   if (*alternate_icon_name != NULL)
   {
      *alternate_icon_name = GetIconName (*alternate_icon_name, icon_size);
      XtSetArg (al[*ac], XmNalternateImage, *alternate_icon_name); (*ac)++;
   }


   /*  If this is not a subpanel control and the keyword is is set so that  */
   /*  labels are not to be displayed, set the string resource to NULL.     */

   if (container_type != SUBPANEL && (Boolean) 
       panel.element_values[PANEL_DISPLAY_CONTROL_LABELS].parsed_value == False)
   {
      XtSetArg (al[*ac], XmNstring, NULL);				(*ac)++;
   }
   else
   {
      control_label = 
         (char *) control_data->element_values[CONTROL_LABEL].parsed_value;


      if ((int) control_data->element_values[CONTROL_TYPE].parsed_value
							 != CONTROL_BLANK)
      {
         if (control_label != NULL)
         {
            *icon_label = XmStringCreateLocalized (control_label);
            XtSetArg (al[*ac], XmNstring, *icon_label);			(*ac)++;
         }
         else if (icon_name == NULL)
         {
            control_label = 
               (char *) control_data->element_values[CONTROL_NAME].parsed_value;

            *icon_label = XmStringCreateLocalized (control_label);
            XtSetArg (al[*ac], XmNstring, *icon_label);			(*ac)++;
         }
      }
      else
      {
         *icon_label = XmStringCreateLocalized ("");
         XtSetArg (al[*ac], XmNstring, *icon_label);			(*ac)++;
      }
   }
}




/************************************************************************
 *
 *  ControlSetBehavior
 *      Use the control type value to set the behavior and some visual
 *      arg list data to be used to create or set values on a control.
 *
 *  Inputs: control_data - a pointer to the control to be created
 *          al - the arg list to be set
 *          ac - a pointer to the count of the arg list elements
 *          in_subpanel - a boolean denoting the parent type of the control
 *
 ************************************************************************/

static void
ControlSetBehavior (ControlData * control_data,
                    ArgList       al,
                    int         * ac,
                    Boolean       in_subpanel,
		    String      * expanded_file_name)

{
   Boolean sensitive;
   Dimension shadow_thickness;
   Dimension highlight_thickness;
   Dimension margin_width;


   /*  Set local shadow thickness and sensitive values to be used  */
   /*  to set the resources for the various control types.         */

   if (control_data->element_values[CONTROL_DROP_ACTION].parsed_value != NULL)
   {
      if (panel.resolution == LOW) 
         shadow_thickness = 1;
      else
         shadow_thickness = 2;
   }
   else
      shadow_thickness = 1;

   if (control_data->element_values[CONTROL_PUSH_ACTION].parsed_value != NULL)
   {
      sensitive = True;
      highlight_thickness = 1;
   }
   else
   {
      sensitive = False;
      highlight_thickness = 0;
   }

   if (panel.resolution == HIGH &&
       (char) control_data->element_values[CONTROL_CONTAINER_TYPE].parsed_value == BOX &&
       in_subpanel == False)
      margin_width = 5;
   else
      margin_width = 2;



   /*  All controls get their user data set to their control data  */
   /*  to be used out of callback functions.                       */
   
   XtSetArg (al[*ac], XmNuserData, control_data);                (*ac)++;


   /*  Switch on the controls control type to set the behaviors  */
   /*  specific to the individual control types.                 */
   
   switch ((char) control_data->element_values[CONTROL_TYPE].parsed_value)
   {
      case CONTROL_BLANK:
      {
         XtSetArg (al[*ac], XmNshadowThickness, 0);             (*ac)++;
         XtSetArg (al[*ac], XmNhighlightThickness, 0);          (*ac)++;
         XtSetArg (al[*ac], XmNcontrolType, XmCONTROL_BLANK);   (*ac)++;
         XtSetArg (al[*ac], XmNbehavior, XmICON_LABEL);         (*ac)++;
         XtSetArg (al[*ac], XmNsensitive, False);               (*ac)++;
      }
      break;

      case CONTROL_BUSY:
      {
         XtSetArg (al[*ac], XmNshadowThickness, 0);             (*ac)++;
         XtSetArg (al[*ac], XmNcontrolType, XmCONTROL_BUSY);    (*ac)++;
         XtSetArg (al[*ac], XmNhighlightThickness, 0);          (*ac)++;
         XtSetArg (al[*ac], XmNsensitive, False);               (*ac)++;
         XtSetArg (al[*ac], XmNmarginWidth, margin_width);      (*ac)++;
         panel.busy_light_data = control_data;
      }
      break;

      case CONTROL_ICON:
      case CONTROL_FILE:
      {
         XtSetArg (al[*ac], XmNshadowThickness, shadow_thickness); (*ac)++;
         XtSetArg (al[*ac], XmNsensitive, sensitive);              (*ac)++;
         XtSetArg (al[*ac], XmNhighlightThickness,highlight_thickness); (*ac)++;
         XtSetArg (al[*ac], XmNmarginWidth, margin_width);         (*ac)++;


         if ((char) control_data->element_values[CONTROL_MONITOR_TYPE].parsed_value == MONITOR_NONE)
         {
	    XtSetArg (al[*ac], XmNcontrolType, XmCONTROL_BUTTON);  (*ac)++;
	 }
         else if ((char) control_data->element_values[CONTROL_MONITOR_TYPE].parsed_value == MONITOR_FILE)
         {
            String file_name;
	 
            file_name = (String) control_data->element_values[CONTROL_FILE_NAME].parsed_value;
            *expanded_file_name = (String) _DtWmParseFilenameExpand ((unsigned char*) file_name);

            XtSetArg (al[*ac], XmNfileName, *expanded_file_name);    (*ac)++;
	    XtSetArg (al[*ac], XmNcontrolType, XmCONTROL_MONITOR);  (*ac)++;
	 }
         else if ((char) control_data->element_values[CONTROL_MONITOR_TYPE].parsed_value == MONITOR_MAIL)
         {
            String file_name;
	 
            file_name = (String) control_data->element_values[CONTROL_FILE_NAME].parsed_value;

	    /*
	     * Set fileName resource if specified; otherwise, leave
	     * unset and it will default to the user's mail file.
	     */
	    if ((file_name != (String)NULL) &&
		(*file_name != '\0'))
	    {
		*expanded_file_name = (String)
		    _DtWmParseFilenameExpand ((unsigned char*) file_name);
		XtSetArg (al[*ac], XmNfileName, *expanded_file_name); (*ac)++;
	    }

	    XtSetArg (al[*ac], XmNcontrolType, XmCONTROL_MAIL);  (*ac)++;
	 }
      }
      break;

      case CONTROL_CLIENT:
      {
         GeometryData * geometry_data;

         XtSetArg (al[*ac], XmNcontrolType, XmCONTROL_CLIENT);      (*ac)++;
         XtSetArg (al[*ac], XmNshadowThickness, shadow_thickness);  (*ac)++;
         XtSetArg (al[*ac], XmNsensitive, sensitive);               (*ac)++;
         XtSetArg (al[*ac], XmNhighlightThickness,highlight_thickness); (*ac)++;


         geometry_data = (GeometryData *) control_data->
  	                  element_values[CONTROL_CLIENT_GEOMETRY].parsed_value;

         if (geometry_data)
	 {
            if (geometry_data->flags & WidthValue && geometry_data->width > 0)
	    {
               XtSetArg (al[*ac], XmNwidth, geometry_data->width + 6);  (*ac)++;
            }

            if (geometry_data->flags & HeightValue && geometry_data->height > 0)
	    {
               XtSetArg (al[*ac], XmNheight, geometry_data->height + 6);(*ac)++;
            }
         }
      }
      break;

      case CONTROL_CLOCK:
      {
         int  inset;


         XtSetArg (al[*ac], XmNshadowThickness, shadow_thickness); (*ac)++;
         XtSetArg (al[*ac], XmNsensitive, sensitive);              (*ac)++;
         XtSetArg (al[*ac], XmNhighlightThickness,highlight_thickness); (*ac)++;
         XtSetArg (al[*ac], XmNmarginWidth, margin_width);         (*ac)++;


         /*  Adjust the size of the clock hands to match the resolution  */

         if ((panel.resolution == HIGH || panel.resolution == MEDIUM) &&
	     in_subpanel == False)
            inset = 8;
         else
            inset = 6;

         XtSetArg (al[*ac], XmNleftInset, inset);            (*ac)++;
         XtSetArg (al[*ac], XmNrightInset, inset);           (*ac)++;
         XtSetArg (al[*ac], XmNtopInset, inset);             (*ac)++;
         XtSetArg (al[*ac], XmNbottomInset, inset);          (*ac)++;
      }
      break;

      case CONTROL_DATE:
      {
         int size;
	 
         XtSetArg (al[*ac], XmNcontrolType, XmCONTROL_DATE);         (*ac)++;
         XtSetArg (al[*ac], XmNshadowThickness, shadow_thickness);   (*ac)++;
         XtSetArg (al[*ac], XmNsensitive, sensitive);                (*ac)++;
         XtSetArg (al[*ac], XmNhighlightThickness,highlight_thickness); (*ac)++;
         XtSetArg (al[*ac], XmNmarginWidth, margin_width);           (*ac)++;
         XtSetArg (al[*ac], XmNuseLabelAdjustment, False);           (*ac)++;


         if (!control_data->element_values[CONTROL_DATE_FORMAT].use_default)
	 {
            XtSetArg (al[*ac], XmNformat, 
               control_data->element_values[CONTROL_DATE_FORMAT].parsed_value);
            (*ac)++;
         }


         if (!in_subpanel)
         {
            XtSetArg (al[*ac], XmNpixmapPosition, XmPIXMAP_MIDDLE);  (*ac)++;
            XtSetArg (al[*ac], XmNfontList, panel.date_font_list);  (*ac)++;
            XtSetArg (al[*ac], XmNuseEmbossedText, False);	    (*ac)++;
            XtSetArg (al[*ac], XmNforeground,
                      BlackPixelOfScreen (XtScreen (panel.form)));  (*ac)++;
         }
         else
         {
            XtSetArg (al[*ac], XmNuseEmbossedText, True);	     (*ac)++;
         }
      }
      break;
   }
}




/************************************************************************
 *
 *  ControlCreateAndRegister
 *	Create the control, add it callbacks, register it, as needed, 
 *      as dropable, as an embedded client, as a push recall client, its
 *      animations.
 *
 *  Inputs: parent - the widget to be the parent of the control
 *          control_data - a pointer to the control to be created
 *          main_copy - this is a boolean indicating that the control
 *                      being created is a copy of the main panel control
 *                      that will be contained within a subpanel
 *          al - a pointer to the arg list to contain the resource data
 *          ac - the arg list count
 *
 ************************************************************************/
 
static Widget
ControlCreateAndRegister (Widget        parent,
                          ControlData * control_data,
                          Boolean       main_copy,
                          ArgList       al,
                          int           ac)


{
   Widget icon;
   unsigned char operations = NULL;
   char *format, * next_seg;
   Arg al2[4];


   /*  Create either a normal control or a clock  */
   
   if ((int) control_data->element_values[CONTROL_TYPE].parsed_value
							 == CONTROL_CLOCK)
      icon = DtCreateClock (parent, "icon", al, ac);
   else
      icon = _DtCreateControl (parent, "icon", al, ac);

   if ((int)control_data->element_values[CONTROL_TYPE].parsed_value
 	                                             == CONTROL_DATE &&
       (control_data->subpanel_data == NULL || main_copy) &&
       (main_copy ||
	(int)control_data->element_values[CONTROL_CONTAINER_TYPE].parsed_value
 	                                             == SUBPANEL))
   {
         format = nl_langinfo(D_FMT);
         XtSetArg (al2[0], XmNformat, format);
         XtSetValues(icon, al2, 1);
   }

   XtManageChild (icon);
   XtAddCallback (icon, XmNcallback, (XtCallbackProc) PushCB, control_data);

   XtAddCallback (icon, XmNhelpCallback,
                     (XtCallbackProc) ControlTopicHelpCB, control_data);

   if (!main_copy)
      control_data->icon = icon;


   /*  Register the animations  */
   
   if (!main_copy)
   {
      SetupPushAnimation (control_data);
      SetupDropAnimation (control_data);
   }
   
	 
   /*  Set the drop zone for the icon  */
      
   /*  Since all file controls have CONTROL_DROP_ACTIONS by default, only    */
   /*  register the ones that are either a data type that is an action or    */
   /*  a file type the has either move, copy or link actions associated with */
   /*  it.  Also register non-file controls that have drop actions as copy.  */

   if (control_data->element_values[CONTROL_DROP_ACTION].parsed_value
                                                                   != NULL)
   {
      if ((int)control_data->element_values[CONTROL_TYPE].parsed_value ==
                                                             CONTROL_FILE)
      {
         if (control_data->is_action)
         {
	    operations = XmDROP_COPY;
         }
         else
         {
	    if (control_data->move_action != NULL)
	       operations |= XmDROP_MOVE;
	    if (control_data->copy_action != NULL)
	       operations |= XmDROP_COPY;
	    if (control_data->link_action != NULL)
	       operations |= XmDROP_LINK;
         }
      }
      else
      {
	 operations = XmDROP_COPY;
      }

      if (operations != NULL)
      {
	 XtSetArg (al2[0], DtNdropAnimateCallback, dropCB);
	 XtSetArg (al2[1], DtNtextIsBuffer, True);
	 XtSetArg (al2[2], XmNanimationStyle, XmDRAG_UNDER_SHADOW_IN);
	 XtSetArg (al2[3], DtNpreserveRegistration, True);

         /* Temporarily hard-coded to reject buffer drops on the trash
            until the desktop clients have trash actions which can
            take appropriate action on calendar appointments, mail
            attachments, selected text, etc.  File Manager is also
            hard-coded to reject buffer drops on the trash window.
            Any changes here should be coordinated with File Manager.
          */
         if (strcmp(control_data->element_values[CONTROL_NAME].parsed_value,
                    "Trash") == 0)
           DtDndDropRegister(icon,
                             DtDND_FILENAME_TRANSFER,
                             operations, transferDropCB, al2, 4);
         else
	   DtDndDropRegister(icon,
                             DtDND_FILENAME_TRANSFER|DtDND_BUFFER_TRANSFER, 
		             operations, transferDropCB, al2, 4);
      }
   }


   /*  Install the control into the push recall and embedded client list.  */
   
   if (!main_copy)
   {
      if ((Boolean) control_data->element_values[CONTROL_PUSH_RECALL].parsed_value)
         PushRecallRegister (control_data, True);

      if ((char) control_data->element_values[CONTROL_TYPE].parsed_value == CONTROL_CLIENT)
         EmbeddedClientRegister (control_data, True);
   }

   return (icon);
}




/************************************************************************
 *
 *  SwitchCreate
 *	Create the workspace switch area.  This consists of three steps.
 *	First, call the window manager to get the set of workspace names.
 *	Next, create a row column and switch button set for the workspace
 *	switch functionality.  Last, create the set of controls that 
 *	surround the switch buttons.
 *
 ************************************************************************/
 
static void
SwitchCreate (BoxData * box_data)


{
   SwitchData * switch_data = box_data->switch_data;

   int     switch_count = 1;
   Atom  * atom_names;
   Atom    current_workspace_atom;
   int     current_workspace;

   Widget switch_button;

   XmString label_string;
   
   XmPixelSet * pixel_set;

   Pixmap pixmap;

   Widget prev_left = NULL;
   Widget prev_right = NULL;

   char * icon_name = NULL;
   char * alt_icon_name = NULL;

   String exp_file_name = NULL;

   int ac;
   Arg al[40];
   Arg al2[4];
   int i;


   /*  Pre create the text field to be used for renaming the workspaces.  */

   box_data->switch_edit = 
      (Widget) XmCreateTextField (box_data->switch_form, "switch_edit", al, 0);


   /*  Get current workspace from the window manager.  */

   DtWsmGetCurrentWorkspace (XtDisplay (box_data->switch_form),
                             RootWindowOfScreen (XtScreen (box_data->switch_form)),
			     &current_workspace_atom);


   /*  Get current workspace list from the window manager.  */

   if (DtWsmGetWorkspaceList (XtDisplay (box_data->switch_form),
                          RootWindowOfScreen (XtScreen (box_data->switch_form)),
                          &atom_names, &switch_count) == 0)
   {
      switch_data->atom_names = atom_names;
      switch_data->switch_count = switch_count;
      
      switch_data->switch_names = 
         (char **) XtMalloc (sizeof(char *) * switch_count);

      for (i = 0; i < switch_count; i++)
      {
         DtWsmWorkspaceInfo * workspace_info;
	 
         DtWsmGetWorkspaceInfo (XtDisplay (box_data->switch_form),
                                RootWindowOfScreen (XtScreen (box_data->switch_form)),
                                atom_names[i], &workspace_info);
									 
         switch_data->switch_names[i] = XtNewString (workspace_info->pchTitle);

         DtWsmFreeWorkspaceInfo (workspace_info);

         if (atom_names[i] == current_workspace_atom)
	    current_workspace = i;
      }
   }
   else
   {
      switch_data->switch_names = NULL;
      switch_data->switch_count = 0;
   }

   panel.switch_row_count =
      (int) (switch_data->element_values[SWITCH_NUMBER_OF_ROWS].parsed_value);
   switch_data->buttons = 
      (Widget *) XtMalloc (sizeof (Widget *) * switch_count);

   switch_data->popup_data = NULL;


   /*  Create the row column within the switch form to contain the  */
   /*  workspace buttons.                                           */
   
   ac = 0;
   XtSetArg (al[ac], XmNtopAttachment, XmATTACH_FORM);  ac++;
   XtSetArg (al[ac], XmNtopOffset, 1);  ac++;
   XtSetArg (al[ac], XmNleftAttachment, XmATTACH_FORM);  ac++;
   XtSetArg (al[ac], XmNleftOffset, 1);  ac++;
   XtSetArg (al[ac], XmNrightAttachment, XmATTACH_FORM);  ac++;
   XtSetArg (al[ac], XmNrightOffset, 1);  ac++;
   XtSetArg (al[ac], XmNbottomAttachment, XmATTACH_FORM);  ac++;
   XtSetArg (al[ac], XmNbottomOffset, 1);  ac++;

   XtSetArg (al[ac], XmNnumColumns, panel.switch_row_count);  ac++;
   XtSetArg (al[ac], XmNpacking, XmPACK_COLUMN);  ac++;
   XtSetArg (al[ac], XmNorientation, XmHORIZONTAL);  ac++;
   XtSetArg (al[ac], XmNadjustLast, False);  ac++;


   /*  Remove all of the tab groups  */

   XtSetArg (al[ac], XmNnavigationType, XmNONE);  ac++;

   if (panel.color_use == XmCO_BLACK_WHITE)
   {
      pixmap = XmGetPixmap (XtScreen (panel.form), "50_foreground",
                            panel.inactive_pixel_set->bg,
                            panel.inactive_pixel_set->fg);
      XtSetArg (al[ac], XmNbackgroundPixmap, pixmap);  ac++;
   }

   switch_data->rc = 
      XmCreateRowColumn (box_data->switch_form, "switch_rc", al, ac);
   XtManageChild (switch_data->rc);


   /*  Create the switch buttons  */
   
   SwitchButtonCreate (switch_data, False);


   /*  Set the active switch button to the active workspace  */
   
   XtSetArg (al[0], XmNset, True);
   switch_data->active_switch = current_workspace;
   XtSetValues (switch_data->buttons[current_workspace], al, 1);
   

   /* Initialize time stamp for catching multi-click events */

   switch_data->time_stamp = 0;


   /*  Create each of the switch controls and adjust the switch_rc's  */
   /*  constraints to position it properly.                           */

   /*  Set up a loop and create each control within the switch  */

   for (i = 0; i < switch_data->control_data_count; i++)
   {
      ControlData * control_data = switch_data->control_data[i];
      XmString icon_label = NULL;

      ac = 0;
      ControlSetVisualData (control_data, al, &ac);

      XtSetArg (al[ac], XmNpixmapPosition, XmPIXMAP_TOP);	ac++;

      if (i % 2)
      {
         if (prev_right == NULL)
	 {
            XtSetArg (al[ac], XmNtopAttachment, XmATTACH_FORM);	ac++;
            XtSetArg (al[ac], XmNtopOffset, 1);			ac++;
         }
	 else
	 {
            XtSetArg (al[ac], XmNtopAttachment, XmATTACH_WIDGET);	ac++;
            XtSetArg (al[ac], XmNtopWidget, prev_right);		ac++;
	 }

         XtSetArg (al[ac], XmNleftAttachment, XmATTACH_WIDGET);		ac++;
         XtSetArg (al[ac], XmNleftWidget, switch_data->rc);		ac++;
         XtSetArg (al[ac], XmNrightAttachment, XmATTACH_FORM);		ac++;
         XtSetArg (al[ac], XmNrightOffset, 1);				ac++;
      }
      else
      {
         if (prev_left == NULL)
	 {
            XtSetArg (al[ac], XmNtopAttachment, XmATTACH_FORM);	ac++;
            XtSetArg (al[ac], XmNtopOffset, 1);			ac++;
         }
	 else
	 {
            XtSetArg (al[ac], XmNtopAttachment, XmATTACH_WIDGET);	ac++;
            XtSetArg (al[ac], XmNtopWidget, prev_left);			ac++;
	 }

         XtSetArg (al[ac], XmNleftAttachment, XmATTACH_FORM);	ac++;
         XtSetArg (al[ac], XmNleftOffset, 1);			ac++;
      }
      
      if (i >= switch_data->control_data_count - 2)
      {
         XtSetArg (al[ac], XmNbottomAttachment, XmATTACH_FORM);		ac++;
         XtSetArg (al[ac], XmNbottomOffset, 1);				ac++;
      }
     

      /*  Call the function used to set the control's icon and label data  */
      
      ControlSetIconData (box_data->switch_form, control_data, 
                          &icon_label, SWITCH, al, &ac, &icon_name,
                          &alt_icon_name);


      /*  Call the function used to set the control's behavioral aspects  */

      ControlSetBehavior (control_data, al, &ac, False, &exp_file_name);


      /*  Call the function used to create and register the control  */
      
      control_data->icon = 
         ControlCreateAndRegister (box_data->switch_form, 
                                   control_data, False, al, ac);

      if (icon_label != NULL)
         XmStringFree (icon_label);

      if (icon_name != NULL)
         XtFree(icon_name);

      if (alt_icon_name != NULL)
         XtFree(alt_icon_name);

      if (exp_file_name != NULL)
         XtFree(exp_file_name);

      if (i % 2)
         prev_right = control_data->icon;
      else
         prev_left = control_data->icon;

      if (i == 0)
      {
         XtSetArg (al2[0], XmNleftAttachment, XmATTACH_WIDGET);
         XtSetArg (al2[1], XmNleftWidget, prev_left);
         XtSetArg (al2[2], XmNrightAttachment, XmATTACH_NONE);
         XtSetValues (switch_data->rc, al2, 3);
      }
   }
}




/************************************************************************
 *
 *  SwitchButtonCreate
 *      Create either the full set of switch button or one new one.
 *
 *************************************************************************/
 
void
SwitchButtonCreate (SwitchData * switch_data,
                    Boolean      one)


{
   BoxData    * box_data;
   XmPixelSet * pixel_set;
   int          switch_count;
   XmString     label_string;
		 
   Arg al[40];
   int ac;
   int ac_save;

   int i;


   /*  Find the box data which contains the switch.  This is needed  */
   /*  for the switch callback processing.                           */
   
   box_data = switch_data->box_data;

   /* Calculate switch height */
   if (!switch_height) {
	Dimension	ignore_width;

	label_string = XmStringCreateLocalized (switch_data->switch_names[0]);
	XmStringExtent(panel.font_list,label_string,
			&ignore_width,&switch_height);
	XmStringFree(label_string);
	if (panel.resolution == HIGH || panel.resolution == MEDIUM) {
		switch_height += 14;
		if (switch_height < SWITCH_HIGH_BUTTON_HEIGHT) {
			switch_height = SWITCH_HIGH_BUTTON_HEIGHT;
		}
	} else {
		switch_height += 8;
		if (switch_height < SWITCH_LOW_BUTTON_HEIGHT) {
			switch_height = SWITCH_LOW_BUTTON_HEIGHT;
		}
	}
   }

   /*  Create Switch Toggles  */

   ac = 0;

   XtSetArg (al[ac], XmNfillOnArm, False);			ac++;

   if (panel.resolution == HIGH || panel.resolution == MEDIUM)
   {
      XtSetArg (al[ac], XmNwidth, SWITCH_HIGH_BUTTON_WIDTH);	ac++;
      XtSetArg (al[ac], XmNshadowThickness, 3);			ac++;
      XtSetArg (al[ac], XmNmarginWidth, 4);			ac++;
	 
   }
   else
   {
      XtSetArg (al[ac], XmNwidth, SWITCH_LOW_BUTTON_WIDTH);	ac++;
      XtSetArg (al[ac], XmNshadowThickness, 2);			ac++;
      XtSetArg (al[ac], XmNmarginWidth, 2);			ac++;

   }

   XtSetArg (al[ac], XmNheight, switch_height);  ac++;
   XtSetArg (al[ac], XmNbehavior, XmICON_TOGGLE);		ac++;
   XtSetArg (al[ac], XmNfillMode, XmFILL_SELF);			ac++;
   XtSetArg (al[ac], XmNalignment, XmALIGNMENT_BEGINNING);	ac++;
   XtSetArg (al[ac], XmNpixmapPosition, XmPIXMAP_BOTTOM);	ac++;
   XtSetArg (al[ac], XmNhighlightThickness, 1);			ac++;
   XtSetArg (al[ac], XmNmarginHeight, 0);			ac++;
   XtSetArg (al[ac], XmNspacing, 0);				ac++;
   XtSetArg (al[ac], XmNcontrolType, XmCONTROL_SWITCH);		ac++;
   XtSetArg (al[ac], XmNfontList, panel.font_list);		ac++;
   XtSetArg (al[ac], XmNrecomputeSize, False);			ac++;
   XtSetArg (al[ac], XmNuserData, box_data);			ac++;
   ac_save = ac;


   /*  Set up a loop to either create a full set of switch buttons  */
   /*  or just one, depending on the input parameters.              */
   
   switch_count = switch_data->switch_count;

   for (i = 0; i < switch_count; i++)
   {
      int pixel_set_index, map_index;

      if (one == True)
         i = switch_count - 1;

      ac = ac_save;


      /*  Set pixel resources.  */
      switch (panel.color_use)
      {
	  case XmCO_BLACK_WHITE:
	  case XmCO_LOW_COLOR:
              pixel_set_index = 1;
              break;

          case XmCO_MEDIUM_COLOR:
              pixel_set_index = HARD_CODED_PRIMARY;
              break;

          case XmCO_HIGH_COLOR:
              map_index = i % _WS_HIGH_COLOR_COUNT;
              pixel_set_index = _ws_high_color_map[map_index];
              break;
      }

      pixel_set = &panel.pixel_set[pixel_set_index - 1];

      XtSetArg (al[ac], XmNforeground, pixel_set->fg);		ac++;
      XtSetArg (al[ac], XmNbackground, pixel_set->bg);		ac++;
      XtSetArg (al[ac], XmNarmColor, pixel_set->bg);		ac++;
      XtSetArg (al[ac], XmNfillOnArm, False);			ac++;
     
      if (panel.color_use == XmCO_BLACK_WHITE)
      {
         XtSetArg (al[ac], XmNuseEmbossedText, False);  ac++;
         XtSetArg (al[ac], XmNpixmapBackground,
                   WhitePixelOfScreen (XtScreen (panel.form)));  ac++;
         XtSetArg (al[ac], XmNpixmapForeground,
                   BlackPixelOfScreen (XtScreen (panel.form)));  ac++;
      }
      else
      {
         XtSetArg (al[ac], XmNpixmapBackground, pixel_set->ts);       ac++;
         XtSetArg (al[ac], XmNpixmapForeground, pixel_set->bs);       ac++;
      }

      
      /*  Set label.  */

      label_string = XmStringCreateLocalized (switch_data->switch_names[i]);
      XtSetArg (al[ac], XmNstring, label_string);		ac++;


      switch_data->buttons[i] = 
         _DtCreateControl (switch_data->rc, "toggle", al, ac);

      XtManageChild (switch_data->buttons[i]);

      XtAddCallback (switch_data->buttons[i], XmNcallback,
                     (XtCallbackProc) SwitchButtonCB, NULL);

      XtAddCallback (switch_data->buttons[i], XmNhelpCallback,
                  (XtCallbackProc) GeneralTopicHelpCB, SWITCH_BUTTON);

      XmStringFree (label_string);
   }
}




/************************************************************************
 *
 *  ControlCreate
 *
 ************************************************************************/
 
static void
ControlCreate (Widget         parent,
               ControlData ** control_data,
               int            control_count)


{
   int i;
   Arg al[40];
   int ac;
   Widget prev_icon = NULL;
   
   char * icon_name = NULL;
   char * alt_icon_name = NULL;
   String exp_file_name = NULL;
   XmString icon_label = NULL;


   /*  Set up a loop and create each control within the box  */

   for (i = 0; i < control_count; i++)
   {
      ac = 0;
      ControlSetVisualData (control_data[i], al, &ac);
   
      XtSetArg (al[ac], XmNpixmapPosition, XmPIXMAP_TOP);	ac++;

      XtSetArg (al[ac], XmNtopAttachment, XmATTACH_FORM);	ac++;
      XtSetArg (al[ac], XmNtopOffset, 1);			ac++;
      XtSetArg (al[ac], XmNbottomAttachment, XmATTACH_FORM);	ac++;
      XtSetArg (al[ac], XmNbottomOffset, 1);			ac++;

      if (prev_icon == NULL)
      {
         XtSetArg (al[ac], XmNleftAttachment, XmATTACH_FORM);	ac++;
         XtSetArg (al[ac], XmNleftOffset, 1);			ac++;
      }
      else
      {
         XtSetArg (al[ac], XmNleftAttachment, XmATTACH_WIDGET);	ac++;	 
         XtSetArg (al[ac], XmNleftWidget, prev_icon);		ac++;	 
      }

      if (i == control_count - 1)
      {
	 XtSetArg (al[ac], XmNrightAttachment, XmATTACH_FORM);	ac++;
         XtSetArg (al[ac], XmNrightOffset, 1);			ac++;
      }
     

      /*  Call the function used to set the control's icon and label data  */
      
      ControlSetIconData (parent, control_data[i], &icon_label, BOX, al, &ac,
			  &icon_name, &alt_icon_name);


      /*  Call the function used to set the control's behavioral aspects  */

      ControlSetBehavior (control_data[i], al, &ac, False, &exp_file_name);


      /*  Call the function used to create and register the control  */

      control_data[i]->icon = 
         ControlCreateAndRegister (parent, control_data[i], False, al, ac);

      if (icon_label != NULL)
      {
         XmStringFree (icon_label);
         icon_label = NULL;
      }
	 
      if (icon_name != NULL)
      {
         XtFree(icon_name);
         icon_name = NULL;
      }

      if (alt_icon_name != NULL)
      {
         XtFree(alt_icon_name);
         alt_icon_name = NULL;
      }

      if (exp_file_name != NULL)
      {
         XtFree(exp_file_name);
         exp_file_name = NULL;
      }

      prev_icon = control_data[i]->icon;

      if (control_data[i]->subpanel_data != NULL)
         SubpanelCreate (control_data[i], control_data[i]->subpanel_data);
   }
}




/************************************************************************
 *
 *  ArrowCreate
 *
 ************************************************************************/
 
static void
ArrowCreate (Widget         parent,
             ControlData ** control_data,
	     int            arrow_count,
             Boolean        right_side,
             Boolean        first_box)


{
   int i;
   Widget prev_separator = NULL;
   Arg al[40];
   Arg al2[30];
   Arg al3[1];
   int ac;
   int ac2;
   int ac_save;
   int ac2_save;
   Dimension control_width;
   Pixmap pixmap;


   /*  If there is no arrow form (there are no subpanels for the controls)  */
   /*  then the parent widget will be null so just return.                  */
   
   if (parent == NULL)
      return;

   ac = 0;
   if (first_box)
   {
      XtSetArg (al[ac], XmNtopOffset, 0);				ac++;
      XtSetArg (al[ac], XmNbottomOffset, 0);				ac++;
   }
   else
   {
      XtSetArg (al[ac], XmNtopOffset, 1);				ac++;
      XtSetArg (al[ac], XmNbottomOffset, 1);				ac++;
   }


   XtSetArg (al[ac], XmNtopAttachment, XmATTACH_FORM);			ac++;
   XtSetArg (al[ac], XmNbottomAttachment, XmATTACH_FORM);		ac++;
   XtSetArg (al[ac], XmNbackground, panel.inactive_pixel_set->bg);	ac++;
   XtSetArg (al[ac], XmNforeground, panel.inactive_pixel_set->fg);	ac++;
   XtSetArg (al[ac], XmNtopShadowColor, panel.inactive_pixel_set->ts);	ac++;
   XtSetArg (al[ac], XmNbottomShadowColor, panel.inactive_pixel_set->bs); ac++;
   XtSetArg (al[ac], XmNselectColor, panel.inactive_pixel_set->sc);	ac++;
   XtSetArg (al[ac], XmNhighlightThickness, 1);				ac++;
   XtSetArg (al[ac], XmNshadowThickness, 0);				ac++;

   if (panel.color_use == XmCO_BLACK_WHITE)
   {
      pixmap = XmGetPixmap (XtScreen (panel.form), "50_foreground",
                            panel.inactive_pixel_set->fg,
                            panel.inactive_pixel_set->bg);

      XtSetArg (al[ac], XmNbackgroundPixmap, pixmap);  ac++;
   }

   ac_save = ac;

   ac2 = 0;
   XtSetArg (al2[ac2], XmNtopAttachment, XmATTACH_FORM);		ac2++;
   XtSetArg (al2[ac2], XmNtopOffset, 0);				ac2++;
   XtSetArg (al2[ac2], XmNbottomAttachment, XmATTACH_FORM);		ac2++;
   XtSetArg (al2[ac2], XmNbottomOffset, 0);				ac2++;
   XtSetArg (al2[ac2], XmNbackground, panel.inactive_pixel_set->bg);	ac2++;
   XtSetArg (al2[ac2], XmNforeground, panel.inactive_pixel_set->fg);	ac2++;
   XtSetArg (al2[ac2], XmNtopShadowColor, panel.inactive_pixel_set->ts);ac2++;
   XtSetArg (al2[ac2], XmNbottomShadowColor, panel.inactive_pixel_set->bs); ac2++;
   XtSetArg (al2[ac2], XmNshadowThickness, 2);				ac2++;
   XtSetArg (al2[ac2], XmNorientation, XmVERTICAL);			ac2++;
   ac2_save = ac2;

   
   for (i = 0; i < arrow_count; i++)
   {
      ac = ac_save;
      ac2 = ac2_save;

      if (prev_separator == NULL)
      {
         XtSetArg (al[ac], XmNleftAttachment, XmATTACH_FORM);	ac++;
         if (first_box)
         {
            XtSetArg (al[ac], XmNleftOffset, 0);			ac++;
         }
         else
         {
            XtSetArg (al[ac], XmNleftOffset, 1);			ac++;
         }
      }
      else
      {
         XtSetArg (al[ac], XmNleftAttachment, XmATTACH_WIDGET);	ac++;
         XtSetArg (al[ac], XmNleftWidget, prev_separator);	ac++;
      }

      control_width = XtWidth (control_data[i]->icon);
      control_width += 2;

      if (i == 0 && right_side == True)
         control_width -= 2;
      else if (i == 0 && right_side == False)
         control_width -= 2;
      else if (i != 0)
         control_width -= 4;

      if (i == 0 && first_box == False)
         control_width -= 2;

      XtSetArg (al[ac], XmNwidth, control_width);            	ac++;

      if (control_data[i]->subpanel_data == NULL)
      {
         XtSetArg (al[ac], XmNimageName, blank_arrow_image);	ac++;
      }
      else
      {
         if ((int) (control_data[i]-> 
             element_values[CONTROL_MONITOR_TYPE].parsed_value) != MONITOR_NONE)
         {
            if (CheckOtherMonitorsOn(control_data[i]->subpanel_data))
            {
               XtSetArg (al[ac], XmNimageName, post_monitor_arrow_image); ac++;
            }
            else
            {
               XtSetArg (al[ac], XmNimageName, post_arrow_image); 	ac++;
            }
         }
         else
         {
            XtSetArg (al[ac], XmNimageName, post_arrow_image); 	ac++;
         }
      }

      XtSetArg (al[ac], XmNmultiClick, XmMULTICLICK_DISCARD); ac++;

      control_data[i]->arrow = 
         DtCreateButtonGadget (parent, "arrow", al, ac);

      XtManageChild (control_data[i]->arrow);

      if (control_data[i]->subpanel_data == NULL)
         XtSetSensitive (control_data[i]->arrow, False);
      
      XtAddCallback (control_data[i]->arrow, XmNcallback, 
	             ArrowCB,
                     (XtPointer) control_data[i]);

      if (i < arrow_count - 1)
      {
         XtSetArg (al2[ac2], XmNleftAttachment, XmATTACH_WIDGET);       ac2++;
         XtSetArg (al2[ac2], XmNleftWidget, control_data[i]->arrow);    ac2++;

         if (i == 0 && right_side == False)
         {
            XtSetArg (al2[ac2], XmNleftOffset, 1);                      ac2++;
         }
         else if (i == 0 && right_side == True)
         {
            XtSetArg (al2[ac2], XmNleftOffset, 3);                      ac2++;
         }
         else
         {
            XtSetArg (al2[ac2], XmNleftOffset, 0);                      ac2++;
         }

         control_data[i]->arrow_separator = 
            XmCreateSeparatorGadget (parent, "arrow_separator", al2, ac2);
         XtManageChild (control_data[i]->arrow_separator);

         prev_separator = control_data[i]->arrow_separator;
      }
   }
}




/************************************************************************
 *
 *  SubpanelCreate
 *
 ************************************************************************/
 
static void
SubpanelCreate (ControlData  * main_control_data,
                SubpanelData * subpanel_data)


{
   DtPanelShellWidget subpanel_shell;
   char * subpanel_name = (char *) subpanel_data->element_values[SUBPANEL_TITLE].parsed_value;
   XmString icon_label;

   ControlData * control_data;
   Widget attach_widget;
   Boolean control_install;
   Boolean monitor = False;
   Boolean main = False;

   Pixmap pixmap;

   int i;

   Arg al[40];
   Arg al2[3];
   int ac;
   int ac_save;


   /*   Create the subpanel shell.  */
   
   ac = 0;
   XtSetArg (al[ac], XmNallowShellResize, True); ac++;
   XtSetArg (al[ac], XmNmwmDecorations, MWM_DECOR_MENU | MWM_DECOR_TITLE); ac++;
   XtSetArg (al[ac], XmNtitle, subpanel_name); ac++;

   subpanel_data->shell =  
      XtCreatePopupShell (subpanel_name, xmDialogShellWidgetClass,
                          panel.shell, al, ac);

   XtSetMappedWhenManaged (subpanel_data->shell, False);


   /*  Set pixel resources.  */

   ac = 0;
   XtSetArg (al[ac], XmNforeground, panel.primary_pixel_set->fg);       ac++;
   XtSetArg (al[ac], XmNbackground, panel.primary_pixel_set->bg);       ac++;
   XtSetArg (al[ac], XmNtopShadowColor, panel.primary_pixel_set->ts);   ac++;
   XtSetArg (al[ac], XmNbottomShadowColor, panel.primary_pixel_set->bs);ac++;
   XtSetArg (al[ac], XmNselectColor, panel.primary_pixel_set->sc);      ac++;

   if (panel.color_use == XmCO_BLACK_WHITE)
   {
      XtSetArg (al[ac], XmNuseEmbossedText, False);                   	ac++;
      XtSetArg (al[ac], XmNarmColor, panel.primary_pixel_set->bg);    	ac++;
      XtSetArg (al[ac], XmNpixmapBackground,
                        WhitePixelOfScreen (XtScreen (panel.form)));  	ac++;
      XtSetArg (al[ac], XmNpixmapForeground,
                        BlackPixelOfScreen (XtScreen (panel.form)));  	ac++;
   }

   if (panel.color_use == XmCO_LOW_COLOR)
   {
      XtSetArg (al[ac], XmNpixmapBackground,
                        WhitePixelOfScreen (XtScreen (panel.form)));  	ac++;
      XtSetArg (al[ac], XmNpixmapForeground, panel.primary_pixel_set->bg); ac++;
      XtSetArg (al[ac], XmNpixmapForeground, panel.primary_pixel_set->bg); ac++;
   }

   ac_save = ac;

   XtSetArg (al[ac], XmNshadowThickness, 1);  ac++;


   /*  Create the outer form widget that will contain the entire panel  */
   
   XtSetArg (al[ac], XmNuserData, SUBPANEL); ac++;

   if (panel.color_use == XmCO_BLACK_WHITE)
   {
      pixmap = XmGetPixmap (XtScreen (panel.form), "50_foreground",
                            panel.inactive_pixel_set->fg,
                            panel.inactive_pixel_set->bg);

      XtSetArg (al[ac], XmNbackgroundPixmap, pixmap);  ac++;
   }

   subpanel_data->form = 
      XmCreateForm (subpanel_data->shell, subpanel_name, al, ac);

   XtManageChild (subpanel_data->form);

   XtAddCallback (subpanel_data->form, XmNhelpCallback,
                  (XtCallbackProc) SubpanelTopicHelpCB, subpanel_data);

   if (panel.popup_data == NULL || panel.popup_data->popup == NULL)
      CreatePopupMenu (subpanel_data->form);
   else
      XmAddToPostFromList (panel.popup_data->popup, subpanel_data->form);


   /* Event handler for posting popup menu */

   XtAddEventHandler (subpanel_data->form, ButtonPressMask, False,
                      (XtEventHandler) PostPopupMenu, (XtPointer) NULL);


   /*  If the value for subpanel control install is true, create  */
   /*  the drop zone as the top child of the form                 */

   control_install = 
      (Boolean) subpanel_data->element_values[SUBPANEL_CONTROL_INSTALL].parsed_value;

   if (control_install)
   {
      icon_label = XmStringCreateLocalized (GETMESSAGE (84, 1, "Install Icon"));

      ac = ac_save;
      XtSetArg (al[ac], XmNtopAttachment, XmATTACH_FORM);  		ac++;
      XtSetArg (al[ac], XmNtopOffset, 5);  				ac++;
      XtSetArg (al[ac], XmNleftAttachment, XmATTACH_FORM);  		ac++;
      XtSetArg (al[ac], XmNleftOffset, 5);  				ac++;
      XtSetArg (al[ac], XmNrightAttachment, XmATTACH_FORM);  		ac++;
      XtSetArg (al[ac], XmNrightOffset, 5);  				ac++;
      XtSetArg (al[ac], XmNshadowThickness, 2);				ac++;
      XtSetArg (al[ac], XmNhighlightThickness, 1);			ac++;
      XtSetArg (al[ac], XmNmarginWidth, 1);				ac++;
      XtSetArg (al[ac], XmNmarginHeight, 1);				ac++;
      XtSetArg (al[ac], XmNstring, icon_label);				ac++;
      XtSetArg (al[ac], XmNpixmapPosition, XmPIXMAP_LEFT);		ac++;
      XtSetArg (al[ac], XmNbehavior, XmICON_LABEL);         		ac++;
      XtSetArg (al[ac], XmNimageName, dropzone_image);			ac++;
      XtSetArg (al[ac], XmNuserData, subpanel_data);			ac++;
      XtSetArg (al[ac], XmNfontList, panel.font_list);		        ac++;
      XtSetArg (al[ac], XmNtraversalOn, False);		        	ac++;

      if (panel.color_use == XmCO_BLACK_WHITE)
      {
	 pixmap = XmGetPixmap (XtScreen (panel.form), "50_foreground",
			       panel.inactive_pixel_set->fg,
 			       panel.inactive_pixel_set->bg);

	 XtSetArg (al[ac], XmNbackgroundPixmap, pixmap);  ac++;
      }

      subpanel_data->dropzone = 
         _DtCreateControl (subpanel_data->form, "dropzone", al, ac);
      XtManageChild (subpanel_data->dropzone);
      XmStringFree (icon_label);

      XtAddCallback (subpanel_data->dropzone, XmNhelpCallback,
                     (XtCallbackProc) GeneralTopicHelpCB, INSTALL_ZONE);


      /*  Set the drop zone for the install area.  */

      XtSetArg (al2[0], DtNdropAnimateCallback, customizeDropCB);
      XtSetArg (al2[1], XmNanimationStyle, XmDRAG_UNDER_SHADOW_IN);
      XtSetArg (al2[2], DtNpreserveRegistration, True);
      
      DtDndDropRegister (subpanel_data->dropzone, DtDND_FILENAME_TRANSFER,
                         XmDROP_COPY, customizeTransferDropCB, al2, 3);


      /*  Create the separator that goes between the dropzone and controls  */
   
      ac = ac_save;
      XtSetArg (al[ac], XmNtopAttachment, XmATTACH_WIDGET);  		ac++;
      XtSetArg (al[ac], XmNtopWidget, subpanel_data->dropzone);  	ac++;
      XtSetArg (al[ac], XmNtopOffset, 5);  				ac++;
      XtSetArg (al[ac], XmNleftAttachment, XmATTACH_FORM);  		ac++;
      XtSetArg (al[ac], XmNleftOffset, 1);  				ac++;
      XtSetArg (al[ac], XmNrightAttachment, XmATTACH_FORM);  		ac++;
      XtSetArg (al[ac], XmNrightOffset, 1);  				ac++;
      XtSetArg (al[ac], XmNshadowThickness, 2);				ac++;
      XtSetArg (al[ac], XmNorientation, XmHORIZONTAL);			ac++;

      subpanel_data->separator = 
         XmCreateSeparatorGadget (subpanel_data->form, 
                                  "subpanel_separator", al, ac);
      XtManageChild (subpanel_data->separator);
   }
   

   /*  Loop through the control set and see if any of the controls   */
   /*  have monitor files attached to them.  If so, the constraints  */
   /*  for the form need to be set appropriately.  		     */

   if ((int) (main_control_data->
       element_values[CONTROL_MONITOR_TYPE].parsed_value) != MONITOR_NONE)
      monitor = True;

   if (monitor == False)
   {
      for (i = 0; i < subpanel_data->control_data_count; i++)
      {
         if ((int) (subpanel_data->control_data[i]->
             element_values[CONTROL_MONITOR_TYPE].parsed_value) != MONITOR_NONE)
         {
            monitor = True;
            break;
         }
      }
   }


   if (control_install)
      attach_widget = subpanel_data->separator;
   else
      attach_widget = subpanel_data->form;


   /*  Set up a loop and create each control within the subpanel  */
   
   for (i = -1; i < subpanel_data->control_data_count; i++)
   {
      if (i == -1)
      {
         control_data = main_control_data;
         main = True;
      }
      else
      {
         control_data = subpanel_data->control_data[i];
         main = False;
      }

      SubpanelControlCreate (subpanel_data, main_control_data, control_data,
                             attach_widget, main, monitor);

      if (main)
         attach_widget = subpanel_data->main_panel_icon_copy;
      else
         attach_widget = control_data->icon;
   }


   /*  Pad the bottom of the last control in the subpanel  */
   
   ac = 0;
   XtSetArg (al[ac], XmNbottomAttachment, XmATTACH_FORM);        ac++;
   XtSetArg (al[ac], XmNbottomOffset, 5);                        ac++;
   XtSetValues (attach_widget, al, ac);


   XtUnmanageChild (subpanel_data->form);
   XtSetMappedWhenManaged (subpanel_data->shell, True);


   /*  Add the callback for handling subpanel unmaps from the system menu  */

   XtAddCallback (subpanel_data->form, XmNunmapCallback,
                  (XtCallbackProc) SubpanelUnmapCB,
                  (XtPointer) main_control_data);


   /*  Set subpanel posting state variables  */

   subpanel_data->torn = False;


   /*  Initialize the subpanels default control setting to the main  */
   /*  panel control.                                                */
   
   subpanel_data->default_control = main_control_data;

   /*  Set the initial XmNcancelButton resource to the default control  */
   /*  within the subpanel.                                             */

   if (subpanel_data->control_data)
      XtSetArg (al[0], XmNcancelButton, subpanel_data->control_data[0]->icon);
   else
      XtSetArg (al[0], XmNcancelButton, subpanel_data->main_panel_icon_copy);

   XtSetValues (subpanel_data->form, al, 1);

}




/************************************************************************
 *
 *  SubpanelControlCreate
 *      Create a control within the subpanel
 *
 ************************************************************************/

void
SubpanelControlCreate (SubpanelData * subpanel_data,
                       ControlData  * main_control_data,
                       ControlData  * control_data,
                       Widget         attach_widget,
                       Boolean        main,
                       Boolean        monitor)

{
   XmString icon_label = NULL;
   Widget control_icon;

   Arg al[40];
   int ac = 0;
   int ac_save;

   char m_state;

   char * icon_name = NULL;
   char * alt_icon_name = NULL;
   String exp_file_name = NULL;


   /*  Call the function used to set the control's pixel and font data  */

   ControlSetVisualData (control_data, al, &ac);
   ac_save = ac;


   /*  Set the control's positioning resources  */

   if (monitor)
   {
      XtSetArg (al[ac], XmNleftOffset, 20);                     ac++;
   }
   else
   {
      XtSetArg (al[ac], XmNleftOffset, 5);                      ac++;
   }


   XtSetArg (al[ac], XmNleftAttachment, XmATTACH_FORM);         ac++;
   XtSetArg (al[ac], XmNtopAttachment, XmATTACH_WIDGET);        ac++;
   XtSetArg (al[ac], XmNtopWidget, attach_widget);              ac++;
   XtSetArg (al[ac], XmNtopOffset, 5);                          ac++;
   XtSetArg (al[ac], XmNrightAttachment, XmATTACH_FORM);        ac++;
   XtSetArg (al[ac], XmNrightOffset, 5);                        ac++;
   XtSetArg (al[ac], XmNmarginWidth, 1);                        ac++;
   XtSetArg (al[ac], XmNmarginHeight, 1);                       ac++;


   /*  Call the function used to set the control's icon and label data  */
      
   ControlSetIconData (subpanel_data->form, control_data, 
                       &icon_label, SUBPANEL, al, &ac,
		       &icon_name, &alt_icon_name);


   /*  Call the function used to set the control's behavioral aspects  */

   ControlSetBehavior (control_data, al, &ac, True, &exp_file_name);


   /*  Call the function used to create and register the control  */

   control_icon = 
      ControlCreateAndRegister(subpanel_data->form, control_data, main, al, ac);

   if (main)
      subpanel_data->main_panel_icon_copy = control_icon;
   else
      control_data->icon = control_icon;


   if (icon_label != NULL)
      XmStringFree (icon_label);

   if (icon_name != NULL)
      XtFree(icon_name);

   if (alt_icon_name != NULL)
      XtFree(alt_icon_name);

   if (exp_file_name != NULL)
      XtFree(exp_file_name);

   /*  If there is a monitor file, create the indicator  */

   if ((int) (control_data->
       element_values[CONTROL_MONITOR_TYPE].parsed_value) != MONITOR_NONE)
   {
      ac = ac_save;

      m_state = _DtControlGetMonitorState(control_icon);

      XtSetArg (al[ac], XmNleftOffset, 3);                              ac++;
      XtSetArg (al[ac], XmNleftAttachment, XmATTACH_FORM);              ac++;
      XtSetArg (al[ac], XmNtopOffset, 6);                               ac++;
      XtSetArg (al[ac], XmNtopAttachment, XmATTACH_OPPOSITE_WIDGET);    ac++;
      XtSetArg (al[ac], XmNtopWidget, control_icon);                    ac++;
      XtSetArg (al[ac], XmNhighlightThickness, 0);                      ac++;
      XtSetArg (al[ac], XmNmarginWidth, 1);                             ac++;
      XtSetArg (al[ac], XmNmarginHeight, 1);                            ac++;
      XtSetArg (al[ac], XmNstring, NULL);                               ac++;

      if (m_state == DtMONITOR_ON)
      {
         XtSetArg (al[ac], XmNimageName, indicator_on_image);           ac++;
      }
      else /* DtMONITOR_OFF */
      {
         XtSetArg (al[ac], XmNimageName, indicator_off_image);          ac++;
      }

      control_data->indicator =
         _DtCreateControl (subpanel_data->form, "indicator", al, ac);
      XtManageChild (control_data->indicator);

      XtAddCallback (control_data->indicator, XmNhelpCallback,
                  (XtCallbackProc) GeneralTopicHelpCB, MONITOR_INDICATOR);

      XtSetSensitive (control_data->indicator, False);
   }
}




/************************************************************************
 *
 *  UpdateSwitchGeometry
 *	Updates the geometry of the switch area to layout the switch
 *      buttons and switch controls appropriately.
 *      
 *	Inputs:
 *	   box_data:  The box that contains the switch.
 *
 ************************************************************************/


void
UpdateSwitchGeometry (BoxData * box_data)


{
   int new_margin;
   Dimension switch_rc_height, switch_button_height;
   Arg al[2];

   if (box_data->switch_data == NULL) return;

   XtSetArg (al[0], XmNheight, &switch_rc_height);
   XtGetValues (box_data->switch_data->rc, al, 1);

   XtSetArg (al[0], XmNheight, &switch_button_height);
   XtGetValues (box_data->switch_data->buttons[0], al, 1);
              
   new_margin = ((int)switch_rc_height -
                 (int)switch_button_height * panel.switch_row_count) /
                 (panel.switch_row_count + 1);

   if (new_margin < 1) new_margin = 1;

   XtSetArg (al[0], XmNmarginHeight, new_margin);
   XtSetArg (al[1], XmNspacing, new_margin);
   XtSetValues (box_data->switch_data->rc, al, 2);
}




/************************************************************************
 *
 *  AddSubpanel
 *	Create and initialize a new subpanel and attach it to control_data
 *      
 *	Inputs:
 *	   control_data:  The main panel control to which the subpanel
 *	                  will be attached.
 *
 ************************************************************************/
 
void
AddSubpanel (ControlData * control_data)


{
   ElementValue * element_values;
   SubpanelData * subpanel_data;
   BoxData * box_data = (BoxData *)control_data->parent_data;

   DtWmHints vHints;

   Arg al[2];
   int i;


   if (control_data->subpanel_data == NULL)
   {
      control_data->subpanel_data =
            (SubpanelData *) XtMalloc (sizeof (SubpanelData));   

      subpanel_data = control_data->subpanel_data;
      
      element_values = (ElementValue *) XtMalloc (sizeof(ElementValue) *
                                                  SUBPANEL_KEYWORD_COUNT);
      
      for (i = 0; i < SUBPANEL_KEYWORD_COUNT; i++)
      {
         (element_values)[i].use_default = True;
         (element_values)[i].string_value = NULL;
         (element_values)[i].parsed_value = NULL;
      }

      element_values[SUBPANEL_NAME].string_value = XtNewString(
		control_data->element_values[CONTROL_NAME].parsed_value);
      element_values[SUBPANEL_CONTAINER_NAME].string_value = XtNewString(
		control_data->element_values[CONTROL_NAME].parsed_value);
      if (control_data->element_values[CONTROL_LABEL].parsed_value != NULL)
         element_values[SUBPANEL_TITLE].string_value = XtNewString(
                control_data->element_values[CONTROL_LABEL].parsed_value);
      else
         element_values[SUBPANEL_TITLE].string_value = XtNewString(
                control_data->element_values[CONTROL_NAME].parsed_value);
      
      subpanel_data->control_data = NULL;
      subpanel_data->control_data_count = 0;
      subpanel_data->parent_control_data = control_data;

      InitializeSubpanelFields (element_values);

      subpanel_data->element_values = element_values;
   }

   if (!XtIsManaged(XtParent(control_data->arrow)))
   {

      if (box_data->left_arrow_form)
      {
         XtSetArg (al[0], XmNtopAttachment, XmATTACH_WIDGET);
         XtSetArg (al[1], XmNtopWidget, box_data->left_arrow_form);
         XtSetValues(box_data->left_control_form, al, 2);

         XtManageChild(box_data->left_arrow_form);
      }

      if (box_data->right_arrow_form)
      {
         XtManageChild(box_data->right_arrow_form);

         XtSetArg (al[0], XmNtopAttachment, XmATTACH_WIDGET);
         XtSetArg (al[1], XmNtopWidget, box_data->right_arrow_form);
         XtSetValues(box_data->right_control_form, al, 2);
      }

      UpdateSwitchGeometry(box_data);

   }

   box_data->subpanel_count++;

   SubpanelCreate (control_data, subpanel_data);


   XtSetArg (al[0], XmNimageName, post_arrow_image);
   XtSetArg (al[1], XmNsensitive, True);
   XtSetValues (control_data->arrow, al, 2);

   /*  Set the subpanels hint and transient behavior  */

   vHints.flags = DtWM_HINTS_BEHAVIORS | DtWM_HINTS_ATTACH_WINDOW;
   vHints.behaviors = DtWM_BEHAVIOR_PANEL | DtWM_BEHAVIOR_SUBPANEL;
   vHints.attachWindow = XtWindow (panel.shell);

   XtSetArg (al[0], XmNtransientFor, panel.shell);
   XtSetValues (subpanel_data->shell, al, 1);

   XtRealizeWidget (subpanel_data->form);
   _DtWsmSetDtWmHints (XtDisplay (panel.shell), 
                      XtWindow (subpanel_data->shell), &vHints);

   WriteSubpanelComponentFile(control_data->subpanel_data);
}




/************************************************************************
 *
 *  DeleteControl
 *	Remove a control upon a menu request for deletion.
 *
 *  Inputs: control_data - a pointer to the control to be deleted.
 *
 ************************************************************************/


void
DeleteControl (ControlData * control_data)


{
   int i;


   /*  Remove the control from the push recall and embedded client list.  */
   
   PushRecallRegister (control_data, False);

   EmbeddedClientRegister (control_data, False);


   /*  Destroy the widgets allocated by the control  */

   if (control_data->indicator)
      XtDestroyWidget (control_data->indicator);

   if (control_data->icon)
   {
      if (control_data->element_values[CONTROL_DROP_ACTION].parsed_value
                                                                    != NULL)
         DtDndDropUnregister(control_data->icon);
      XtDestroyWidget (control_data->icon);
   }


   /*  Remove the copy of the action set and delete the element values  */

   DeleteControlActionList (control_data);

   RemoveEntry (control_data, CONTROL);
}




/************************************************************************
 *
 *  DeleteSubpanel
 *	Delete the subpanel attached to the control pointed to by
 *	control_data.
 *
 *  Inputs: control_data - a pointer to the main panel control that the
 *          subpanel is attached to.
 *
 ************************************************************************/
 

void
DeleteSubpanel (ControlData * control_data)


{
   SubpanelData * subpanel_data = control_data->subpanel_data;
   BoxData * box_data = (BoxData *) control_data->parent_data;
   Arg al[2];
   int i;


   /*  If the current main control is not within the main panel, call  */
   /*  the DeleteSubpanelControl function to delete the main control   */
   /*  This will cause the current subpanel control within the main    */
   /*  panel to be promoted to a main panel control.                   */
   
   if (subpanel_data->parent_control_data != control_data)
   {
      subpanel_data->torn = True;
      DeleteSubpanelControl (subpanel_data, subpanel_data->parent_control_data);
      subpanel_data->parent_control_data->subpanel_data = NULL;
   }


   /*  Set the main panel control's arrow to blank and insensitive  */
   
   XtSetArg (al[0], XmNimageName, blank_arrow_image);
   XtSetValues (subpanel_data->parent_control_data->arrow, al, 1);
   XtSetSensitive (subpanel_data->parent_control_data->arrow, False);


   /*  Either remove a dynamic .fp or write out the subpanel's .fp  */
   /*  with the DELETE keyword set                                  */
   
   RemoveSubpanelComponentFile (subpanel_data);


   /*  Delete the controls within the subpanel, destroy the remaining  */
   /*  widgets and free up the remaining data.                         */
   
   for (i = 0; i < subpanel_data->control_data_count; i++)
   {
      RemoveControlComponentFile (subpanel_data->control_data[i]);
      DeleteControl (subpanel_data->control_data[i]);
      XtFree ((char *) subpanel_data->control_data[i]);
   }

  /* Remove install zones */
   if (subpanel_data->element_values[SUBPANEL_CONTROL_INSTALL].parsed_value)
   {
      DtDndDropUnregister (subpanel_data->dropzone);
      XtDestroyWidget (subpanel_data->dropzone);
   }

   XtDestroyWidget (subpanel_data->shell);
   XtFree ((char *) subpanel_data->control_data);

   RemoveEntry (subpanel_data, SUBPANEL);

   XtFree ((char *) subpanel_data);
   control_data->subpanel_data = NULL;

   box_data->subpanel_count--;

   if (box_data->subpanel_count == 0)
   {
      if (box_data->left_arrow_form)
      {
         XtSetArg (al[0], XmNtopAttachment, XmATTACH_FORM);
         XtSetArg (al[1], XmNtopOffset, 0);
         XtSetValues(box_data->left_control_form, al, 2);

         XtUnmanageChild(box_data->left_arrow_form);
      }

      if (box_data->right_arrow_form)
      {
         XtUnmanageChild(box_data->right_arrow_form);

         XtSetArg (al[0], XmNtopAttachment, XmATTACH_FORM);
         XtSetArg (al[1], XmNtopOffset, 0);
         XtSetValues(box_data->right_control_form, al, 2);
      }

      UpdateSwitchGeometry(box_data);
   }
}



 
/************************************************************************
 *
 *  SwapElementValue
 *	Exchange the values between two element value structures.
 *
 *  Inputs: to_value, from_value - pointer to the two element value 
 *          structures in which to exchange the values.
 *
 ************************************************************************/
  

static void
SwapElementValue (ElementValue * to_value,
                  ElementValue * from_value)


{
   Boolean   use_default;
   char    * string_value;
   void    * parsed_value;

   use_default = from_value->use_default;
   string_value = from_value->string_value;
   parsed_value = from_value->parsed_value;

   from_value->use_default = to_value->use_default;
   from_value->string_value = to_value->string_value;
   from_value->parsed_value = to_value->parsed_value;

   to_value->use_default = use_default;
   to_value->string_value = string_value;
   to_value->parsed_value = parsed_value;
}




/************************************************************************
 *
 *  DeleteSubpanelControl
 *	This function removes a control contained in control_data from
 *	a subpanel.
 *
 *	Inputs:
 *	   subpanel_data:  The subpanel the control is being deleted from
 *	   control_data:   The control to be deleted
 *
 ************************************************************************/


void
DeleteSubpanelControl (SubpanelData * subpanel_data,
                       ControlData  * control_data)


{
   ControlData * main_control_data = subpanel_data->parent_control_data;
   int           control_data_count;

   Widget  * child_list;
   int       num_children;
   Widget    attach_widget = NULL;
   Widget    target_widget = NULL;
   Boolean   last = False;
   Dimension height;

   Boolean control_monitor;

   Arg al[2];
   int i, j;


   /*  Put the subpanel down before mucking with the widget hierarchy  */

   if (subpanel_data->torn == False)
      XtUnmanageChild (subpanel_data->shell);        


   /*  There are 3 deletion cases.                                     */
   /*    1. Deletion of the main panel copy when there are 0 subpanel  */
   /*       controls.  This involves destroying the copy and deleting  */
   /*       the subpanel.                                              */
   /*                                                                  */
   /*    2. Deletion of the main panel copy when there is subpanel     */
   /*       controls.  This involves the full deletion of the main     */
   /*       panel control and the promotion of a subpanel control to   */
   /*       a full main panel control.                                 */
   /*                                                                  */
   /*    3. Deletion of a subpanel control.  This involves the full    */
   /*       deletion of the subpanel control and if it has been        */
   /*       toggled to the main panel another control is selected      */

   control_data_count = subpanel_data->control_data_count;

   if (main_control_data == control_data && control_data_count == 0)
   {
      DeleteSubpanel (main_control_data);
      return;
   }


   /*  Look through the controls parent (form) child list to get    */
   /*  the position of the control to be deleted and the previous   */
   /*  and next control.  This is needed for resetting constraints  */

   if (main_control_data == control_data)
   {
      height = XtHeight (subpanel_data->main_panel_icon_copy);
      XtSetArg (al[0], XmNtopWidget, &target_widget);
      XtGetValues (subpanel_data->main_panel_icon_copy, al, 1);
   }
   else
   {
      height = XtHeight (control_data->icon);
      XtSetArg (al[0], XmNtopWidget, &target_widget);
      XtGetValues (control_data->icon, al, 1);
   }
   
   XtSetArg (al[0], XmNchildren, &child_list);
   XtSetArg (al[1], XmNnumChildren, &num_children);
   XtGetValues (subpanel_data->form, al, 2);
   
   for (i = 0; i < num_children; i++)
   {
      XtSetArg (al[0], XmNtopWidget, &attach_widget);
      XtGetValues (child_list[i], al, 1);

      if ((main_control_data == control_data && 
          attach_widget == subpanel_data->main_panel_icon_copy) ||
          (main_control_data != control_data && 
           attach_widget == control_data->icon))
      {
         attach_widget = child_list[i];
         break;
      }
   }

   if (i == num_children)
   {
      attach_widget = NULL;
      last = True;
   }


   if (main_control_data == control_data)
   {
      ControlData * subpanel_control_data;
      int           subpanel_control_position;
      

      if (main_control_data == subpanel_data->default_control)
         ToggleDefaultControl (main_control_data, subpanel_data,
                               subpanel_data->control_data[0]);

      subpanel_control_data = subpanel_data->default_control;

      
      /*  Remove the .fp or write a Delete .fp for both the main panel  */
      /*  control and subpanel control.  The subpanel data will then    */
      /*  be moved the the main panel data and the new control will be  */
      /*  written.                                                      */
   
      /*  For the main panel control, if a dynamic .fp exists, remove  */
      /*  it otherwize, do nothing since a new one will be written.    */
      
      if (SessionFileNameLookup ((char *) main_control_data->element_values[CONTROL_NAME].parsed_value,
                                 (int) CONTROL,
                                 (char *) main_control_data->element_values[CONTROL_CONTAINER_NAME].parsed_value,
                                 (int) BOX) != NULL)
         RemoveControlComponentFile (main_control_data);

      RemoveControlComponentFile (subpanel_control_data);


      /*  Find the position of the subpanel control data    */
      /*  within their parent list, for latter processing   */
      
      for (subpanel_control_position = 0; 
           subpanel_control_position < subpanel_data->control_data_count;
	   subpanel_control_position++)
      {
         if (subpanel_data->control_data[subpanel_control_position] == subpanel_control_data)
            break;
      }


      /*  Reset the subpanel values that were switch from the deleted  */
      /*  main control when ToggleDefaultControl was called above.     */
      
      XtDestroyWidget (subpanel_data->main_panel_icon_copy);
      subpanel_data->main_panel_icon_copy = subpanel_control_data->icon;
      subpanel_data->default_control = main_control_data;

      
      /*  Move the necessary data from the MP control to the SP control  */
      /*  and write out the new control file.                            */

      SwapElementValue (&subpanel_control_data->element_values[CONTROL_NAME],
                        &main_control_data->element_values[CONTROL_NAME]);
      SwapElementValue (&subpanel_control_data->element_values[CONTROL_CONTAINER_NAME],
                        &main_control_data->element_values[CONTROL_CONTAINER_NAME]);
      SwapElementValue (&subpanel_control_data->element_values[CONTROL_CONTAINER_TYPE],
                        &main_control_data->element_values[CONTROL_CONTAINER_TYPE]);
      SwapElementValue (&subpanel_control_data->element_values[CONTROL_POSITION_HINTS],
                        &main_control_data->element_values[CONTROL_POSITION_HINTS]);

      WriteControlComponentFile (subpanel_control_data);

      
      /*  Delete the main panel control.  Null out the icon field   */
      /*  since it is being used within the subpanel_control_data.  */
      
      subpanel_control_data->icon = main_control_data->icon;
      main_control_data->icon = NULL;
      DeleteControl (main_control_data);


      /*  Set the main control to the subpanel control values  */

      main_control_data->element_values = subpanel_control_data->element_values;
      main_control_data->icon = subpanel_control_data->icon;
      main_control_data->indicator = subpanel_control_data->indicator;
      main_control_data->actions = subpanel_control_data->actions;


      /*  Remove and readd the callbacks from the main control and  */
      /*  the main panel icon copy.                                 */
      
      XtRemoveCallback (main_control_data->icon, XmNcallback, 
                        (XtCallbackProc) PushCB, subpanel_control_data);

      XtRemoveCallback (main_control_data->icon, XmNhelpCallback,
                        (XtCallbackProc) ControlTopicHelpCB, subpanel_control_data);
      
      XtAddCallback (main_control_data->icon, XmNcallback, 
                     (XtCallbackProc) PushCB, main_control_data);

      XtAddCallback (main_control_data->icon, XmNhelpCallback,
                     (XtCallbackProc) ControlTopicHelpCB, main_control_data);

      
      XtRemoveCallback (subpanel_data->main_panel_icon_copy, XmNcallback, 
                        (XtCallbackProc) PushCB, subpanel_control_data);

      XtRemoveCallback (subpanel_data->main_panel_icon_copy, XmNhelpCallback,
                        (XtCallbackProc) ControlTopicHelpCB, subpanel_control_data);
      
      XtAddCallback (subpanel_data->main_panel_icon_copy, XmNcallback, 
                     (XtCallbackProc) PushCB, main_control_data);

      XtAddCallback (subpanel_data->main_panel_icon_copy, XmNhelpCallback,
                     (XtCallbackProc) ControlTopicHelpCB, main_control_data);



      /*  Move the subpanel control data's up one position within the    */
      /*  subpanel's list to denote the removal of the subpanel control  */
      /*  and free up the remaining subpanel data.                       */

      for (i = subpanel_control_position; i < subpanel_data->control_data_count - 1; i++)
         subpanel_data->control_data[i] = subpanel_data->control_data[i + 1];

      subpanel_data->control_data_count--;

      XtFree ((char *) subpanel_control_data);


      if (subpanel_data->control_data_count == 0)
      {
	 XtFree ((char *) subpanel_data->control_data);
	 subpanel_data->control_data = NULL;
      }
   }
   else
   {
      /*  Call a function which either unlinks the dynamic .fp file or  */
      /*  writes a Delete = True dynamic .fp file.  The function also   */
      /*  updates the session tables.                                   */
   
      RemoveControlComponentFile (control_data);


      for (i = 0; i < subpanel_data->control_data_count; i++)
      {
          if (control_data == subpanel_data->control_data[i]) break;
      }

      if (subpanel_data->default_control == control_data)
      {
         if (i < subpanel_data->control_data_count - 1)
         {
            ToggleDefaultControl (main_control_data, subpanel_data,
                                  subpanel_data->control_data[i + 1]);
         }
         else
         {
            if (i > 0)
               ToggleDefaultControl (main_control_data, subpanel_data,
                                     subpanel_data->control_data[i - 1]);
            else if (subpanel_data->main_panel_icon_copy != NULL)
               ToggleDefaultControl (main_control_data, subpanel_data,
                                     main_control_data);
         }
      }

      DeleteControl (control_data);

      /*  Loop through the control list and move the controls down  */
      /*  one position.                                             */
      
      for (j = i + 1 ; j < subpanel_data->control_data_count; j++)
      {
         subpanel_data->control_data[j - 1] = subpanel_data->control_data[j];
      }

      subpanel_data->control_data_count--;
   }


   /*  If the subpanel is empty, delete it.  */
   
   if (subpanel_data->main_panel_icon_copy == NULL &&
       subpanel_data->control_data_count == 0)
   {
      DeleteSubpanel (subpanel_data->parent_control_data);
      return;
   }


   /*  Get the control positioning constraints reset  */
   
   if (attach_widget != NULL && target_widget != NULL)
   {
      XtSetArg (al[0], XmNtopWidget, target_widget);
      XtSetValues (attach_widget, al, 1);
   }

   if (last)
   {
      XtSetArg (al[0], XmNbottomAttachment, XmATTACH_FORM);
      XtSetArg (al[1], XmNbottomOffset, 5);
      XtSetValues (target_widget, al, 2);

      XtSetArg (al[0], XmNheight, XtHeight (subpanel_data->form) -
                                  (XtHeight (target_widget) - height));
      XtSetValues (subpanel_data->form, al, 1);
   }


   /*  Loop through the remaining controls and see if any left are  */
   /*  monitors.  If not, adjust the left pad of the controls       */
   
   control_monitor = False;

   for (j = 0; j < subpanel_data->control_data_count; j++)
   {
      if ((int) subpanel_data->control_data[j]->
          element_values[CONTROL_MONITOR_TYPE].parsed_value != MONITOR_NONE)
      {
         control_monitor = True;
         break;
      }
   }

   if ((int) main_control_data->
       element_values[CONTROL_MONITOR_TYPE].parsed_value != MONITOR_NONE)
   {
      control_monitor = True;
   }

   if (control_monitor == False)
   {
      XtSetArg (al[0], XmNleftOffset, 5);

      for (j = 0; j < subpanel_data->control_data_count; j++)
         XtSetValues (subpanel_data->control_data[j]->icon, al, 1);

      if (subpanel_data->main_panel_icon_copy != NULL)
         XtSetValues (subpanel_data->main_panel_icon_copy, al, 1);
   }


   /*  Pop the subpanel back up and resume processing  */

   if (subpanel_data->torn == False)
       ArrowCB (main_control_data->arrow,
		(XtPointer)main_control_data, (XtPointer)NULL);

   /*  Set the XmNcancelButton resource to the default control  */
   /*  within the subpanel.                                     */

   if (subpanel_data->control_data)
      XtSetArg (al[0], XmNcancelButton, subpanel_data->control_data[0]->icon);
   else
      XtSetArg (al[0], XmNcancelButton, subpanel_data->main_panel_icon_copy);

   XtSetValues (subpanel_data->form, al, 1);

}




/************************************************************************
 *
 *  ToggleDefaultControl
 *      This is used to switch the main panel control to the control
 *      attached to the selected toggle.
 *
 *  Inputs:
 *      main_control_data - a pointer to the main panel's control data.
 *      subpanel_data - a pointer to the subpanel where the control to
 *                      be put in the main panel is coming from.
 *      control_data - a pointer to the control in the subpanel to be 
 *                     toggled into the main panel.
 *
 ************************************************************************/


void
ToggleDefaultControl (ControlData  * main_control_data,
                      SubpanelData * subpanel_data,
                      ControlData  * control_data)


{
   BoxData * box_data;

   Widget prev_icon = NULL;
   Widget next_icon = NULL;
   Widget old_main_icon = main_control_data->icon;
   Widget main_subpanel_icon;
   Widget parent = XtParent (old_main_icon);
   Widget control_icon;

   char * icon_name = NULL;
   char * alt_icon_name = NULL;
   String exp_file_name = NULL;


   XmString icon_label = NULL;

   Dimension width;
   Dimension new_width;

   Arg al[40];
   int ac;
   int i;


   /*  If the selection occured on the currently selected toggle   */
   /*  do nothing.                                                 */

   if (subpanel_data->default_control == control_data)
      return;


   /*  Update the main panel control's push recall and embedded client  */
   /*  lists, and reparent the embedded client if necessary.            */

   if (main_control_data == subpanel_data->default_control)
   {
      control_icon = main_control_data->icon;
      main_control_data->icon = subpanel_data->main_panel_icon_copy;

      if ((Boolean) main_control_data->element_values[CONTROL_PUSH_RECALL].parsed_value)
         PushRecallRegister (main_control_data, True);

      if ((char) main_control_data->element_values[CONTROL_TYPE].parsed_value == CONTROL_CLIENT)
         EmbeddedClientRegister (main_control_data, True);

      main_control_data->icon = control_icon;

      if ((char) main_control_data->
          element_values[CONTROL_TYPE].parsed_value == CONTROL_CLIENT)
      {
         EmbeddedClientReparent ((char *) main_control_data->
                      element_values[CONTROL_CLIENT_NAME].parsed_value,
                      subpanel_data->main_panel_icon_copy);
      }
   }
   else
   {
      if ((Boolean) subpanel_data->default_control->element_values[CONTROL_PUSH_RECALL].parsed_value)
         PushRecallRegister (subpanel_data->default_control, True);

      if ((char) subpanel_data->default_control->element_values[CONTROL_TYPE].parsed_value == CONTROL_CLIENT)
         EmbeddedClientRegister (subpanel_data->default_control, True);

      if ((char) subpanel_data->default_control->
          element_values[CONTROL_TYPE].parsed_value == CONTROL_CLIENT)
      {
         EmbeddedClientReparent ((char *) subpanel_data->default_control->
                     element_values[CONTROL_CLIENT_NAME].parsed_value,
                     subpanel_data->default_control->icon);
      }
   }


   subpanel_data->default_control = control_data;
   control_data->subpanel_data = subpanel_data;
   main_control_data->is_action = control_data->is_action;


   /*  Get the width of the current main control to be used to  */
   /*  possible adjust the new control when it is created.      */

   width = XtWidth (main_control_data->icon);


   /*  Disallow geometry changes of the main control's parent until  */
   /*  after the controls have been destroyed and created            */

   XtSetArg (al[0], XmNresizePolicy, XmRESIZE_NONE);
   XtSetValues (parent, al, 1);


   /*  Create the new control using the control data from the  */
   /*  control within the subpanel                             */

   /*  Loop through the main controls parent data (box_data) to  */
   /*  find its position relative to controls around it          */
   
   box_data = (BoxData *) main_control_data->parent_data;
   
   for (i = 0; i < box_data->control_data_count; i++)
   {
      if (box_data->control_data[i] == main_control_data)
      {
	 if (i != 0)
	    prev_icon = box_data->control_data[i - 1]->icon;

         if (i != box_data->control_data_count - 1)
	    next_icon = box_data->control_data[i + 1]->icon;

         break;
      }
   }


   /*  Call the function used to set the control's pixel and font data  */

   ac = 0;
   ControlSetVisualData (control_data, al, &ac);

   XtSetArg (al[ac], XmNpixmapPosition, XmPIXMAP_TOP);	ac++;


   /*  Set up the attachment constraints for the control  */

   XtSetArg (al[ac], XmNtopAttachment, XmATTACH_FORM);		ac++;
   XtSetArg (al[ac], XmNtopOffset, 1);				ac++;
   XtSetArg (al[ac], XmNbottomAttachment, XmATTACH_FORM);	ac++;
   XtSetArg (al[ac], XmNbottomOffset, 1);			ac++;

   if (prev_icon == NULL)
   {
      XtSetArg (al[ac], XmNleftAttachment, XmATTACH_FORM);	ac++;
      XtSetArg (al[ac], XmNleftOffset, 1);			ac++;
   }
   else
   {
      XtSetArg (al[ac], XmNleftAttachment, XmATTACH_WIDGET);	ac++;	 
      XtSetArg (al[ac], XmNleftWidget, prev_icon);		ac++;	 
   }

   if (next_icon == NULL)
   {
      XtSetArg (al[ac], XmNrightAttachment, XmATTACH_FORM);	ac++;
      XtSetArg (al[ac], XmNrightOffset, 1);			ac++;
   }
     

   /*  Call the function used to set the control's icon and label data  */
      
   ControlSetIconData (parent, control_data, &icon_label, BOX, al, &ac,
		       &icon_name, &alt_icon_name);


   /*  Call the function used to set the control's behavioral aspects  */

   ControlSetBehavior (control_data, al, &ac, False, &exp_file_name);


   /*  Call the function used to create and register the control  */

   control_icon = control_data->icon;

   main_control_data->icon = 
      ControlCreateAndRegister (parent, control_data, False, al, ac);


   /*  If this in not the main control copy then restore the subpanel icon  */

   if (main_control_data != control_data)
      control_data->icon = control_icon;


   /*  If this is a toggle of a client control from the subpanel to  */
   /*  the main panel, call the reparenting function.                */

   if ((char) control_data->
       element_values[CONTROL_TYPE].parsed_value == CONTROL_CLIENT)
   {
      EmbeddedClientReparent ((char *) control_data->
                              element_values[CONTROL_CLIENT_NAME].parsed_value,
                              main_control_data->icon);
   }


   if (icon_label != NULL)
      XmStringFree (icon_label);

   if (icon_name != NULL)
      XtFree(icon_name);

   if (alt_icon_name != NULL)
      XtFree(alt_icon_name);

   if (exp_file_name != NULL)
      XtFree(exp_file_name);

   /*  If this is not the right most control within its parent,  */
   /*  reset the poistioning constraints of the control to the  */
   /*  right of this one.                                        */
   
   if (next_icon != NULL)
   {
      ac = 0;
      XtSetArg (al[ac], XmNleftAttachment, XmATTACH_WIDGET);		ac++;
      XtSetArg (al[ac], XmNleftWidget, main_control_data->icon);	ac++;
      XtSetValues (next_icon, al, ac);
   }


   /*  Now destroy the original main panel icon  */

   XtDestroyWidget (old_main_icon);

   
   /*  Get the width of the new icon and increase its size to minimize  */
   /*  the degree of resizing of the main panel.                        */
   
   new_width = XtWidth (main_control_data->icon);

   if (new_width < width)
   {
      Dimension normal_size;
      Dimension margin_width;
      
      if (panel.resolution == HIGH)
         normal_size = ICON_HIGH_WIDTH;
      else if (panel.resolution == MEDIUM)
         normal_size = ICON_MEDIUM_WIDTH;
      else
         normal_size = ICON_LOW_WIDTH;

      if (new_width < normal_size)
      {
         XtSetArg (al[0], XmNwidth, normal_size);
         XtSetArg (al[1], XmNmarginWidth, (Dimension)(normal_size-new_width)/2);
         XtSetValues (main_control_data->icon, al, 2);
      }
   }


   /*  Get the width of the icon again for arrow and embedded client  */
   /*  processing.                                                    */

   new_width = XtWidth (main_control_data->icon);


   /*  Call a function to reposition embedded clients to the right  */
   /*  of the main panel control.                                   */
   
   if (new_width != width)
      EmbeddedClientReposition (main_control_data->icon,
                                XtX (main_control_data->icon) + 3, 
				new_width - width);


   /*  Reset the geometry policy for the main control parent  */

   XtSetArg (al[0], XmNresizePolicy, XmRESIZE_ANY);
   XtSetValues (parent, al, 1);


   /*  Set the width of the arrow  */

   new_width += 2;
   if (prev_icon != NULL) new_width -= 2;
   if (next_icon != NULL) new_width -= 2;

   XtSetArg (al[0], XmNwidth, new_width);
   XtSetValues (main_control_data->arrow, al, 1);
}




/************************************************************************
 *
 *  SetupPushAnimation
 *	Add the sequence of images that make up a push animation for a
 *      control.
 *
 *      Inputs: control_data - a pointer to the control to which to add
 *              the animation.
 *
 ************************************************************************/
 

static void
SetupPushAnimation (ControlData * control_data)


{
   char * animation_name = control_data->
                           element_values[CONTROL_PUSH_ANIMATION].parsed_value;
   int    count;
   int    delay;
   String image_name;

   int i, j; 


   count = panel.animation_count;
   
   if (animation_name == NULL) return;

   for (i = 0; i < count; i++)
   {
       if (!strcmp (panel.animation_data[i].name, animation_name))
	  break;
   }

   if (i < count)
   {
      for (j = 0; j < panel.animation_data[i].item_count; j++)
      {
         image_name = GetIconName (panel.animation_data[i].items[j].image_name,
                                   panel.main_icon_size);

         delay = panel.animation_data[i].items[j].delay;

         _DtControlAddPushAnimationImage (control_data->icon, image_name, delay);
         XtFree(image_name);
      }
   }
}




/************************************************************************
 *
 *  SetupDropAnimation
 *	Add the sequence of images that make up a drop animation for a
 *      control.
 *
 *      Inputs: control_data - a pointer to the control to which to add
 *              the animation.
 *
 ************************************************************************/
 

static void
SetupDropAnimation (ControlData * control_data)


{
   char * animation_name = control_data->
                            element_values[CONTROL_DROP_ANIMATION].parsed_value;
   int    count;
   int    delay;
   String image_name;

   int i, j; 


   count = panel.animation_count;
   
   if (animation_name == NULL) return;

   for (i = 0; i < count; i++)
   {
       if (!strcmp (panel.animation_data[i].name, animation_name))
	  break;
   }

   if (i < count)
   {
      for (j = 0; j < panel.animation_data[i].item_count; j++)
      {
         image_name = GetIconName (panel.animation_data[i].items[j].image_name,
                                   panel.main_icon_size);

         delay = panel.animation_data[i].items[j].delay;

         _DtControlAddDropAnimationImage (control_data->icon, image_name, delay);
         XtFree(image_name);
      }
   }
}





/************************************************************************
 *
 *  GetIconName
 *      Get the file name for an icon by extracting the panel resolution
 *      and then looking up the image name.
 *
 *      Inputs: image_name - the base name of the image to be found
 *              icon_size - the size requested (t, s, m, l) for the
 *              icon.
 *
 ************************************************************************/


String
GetIconName (String       image_name, 
             unsigned int icon_size)  
	     


{
   String       return_name = NULL;
   Screen     * screen = XtScreen (panel.shell);
   unsigned int next_size;


   /*  Get name.  */

   next_size = icon_size;


   /*  Loop through until all sizes are exhausted  */

   while (return_name == NULL)
   {
     return_name = 
        _DtGetIconFileName (screen, image_name, NULL, NULL, next_size);

     if (next_size == DtLARGE)
        next_size = DtMEDIUM;
     else if (next_size == DtMEDIUM)
        next_size = DtSMALL;
     else if (next_size == DtSMALL)
        next_size = DtTINY;
     else
        break; /* tried all sizes, exit loop */
   }


   if (return_name == NULL)
      return_name = 
         _DtGetIconFileName (screen, image_name, NULL, NULL, DtUNSPECIFIED);

   if (return_name == NULL)
      return_name = 
         _DtGetIconFileName (screen, DEFAULT_IMAGE_NAME, NULL, NULL, icon_size);

   if (return_name == NULL)
      return_name = XtNewString (image_name);


   /*  Return value to be freed by caller.  */

   return (return_name);
}
