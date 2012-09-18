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
/* $XConsortium: IconicPath.c /main/9 1996/10/15 10:08:03 mustafa $ */
/************************************<+>*************************************
 ****************************************************************************
 *
 *   FILE:           IconicPath.c
 *
 *   COMPONENT_NAME: Desktop File Manager (dtfile)
 *
 *   Description:    Contains routines to handle the Iconic Path.
 *
 *   FUNCTIONS: ButtonCallback
 *		ChangeManaged
 *		ClassInitialize
 *		ClassPartInitialize
 *		DESIRED_HEIGHT
 *		DESIRED_WIDTH
 *		Destroy
 *		DtUpdateIconicPath
 *		GeometryManager
 *		ICON_HT
 *		IconicPathRedraw
 *		Initialize
 *		MHT
 *		MIN_WD
 *		MWD
 *		QueryGeometry
 *		Redisplay
 *		Resize
 *		SPC
 *		SetValues
 *		Update
 *		WidgetNavigable
 *		_DtCreateIconicPath
 *		externaldef
 *
 *   (c) Copyright 1993, 1994, 1995 Hewlett-Packard Company
 *   (c) Copyright 1993, 1994, 1995 International Business Machines Corp.
 *   (c) Copyright 1993, 1994, 1995 Sun Microsystems, Inc.
 *   (c) Copyright 1993, 1994, 1995 Novell, Inc.
 *
 ****************************************************************************
 ************************************<+>*************************************/

/* Copied from Xm/XmI.h */
#define GMode(g)	    ((g)->request_mode)
#define IsWidth(g)	    (GMode (g) & CWWidth)
#define IsHeight(g)	    (GMode (g) & CWHeight)
#define IsBorder(g)	    (GMode (g) & CWBorderWidth)
#define IsQueryOnly(g)      (GMode (g) & XtCWQueryOnly)

#include <Xm/TransltnsP.h>
#include <Xm/LabelG.h>
#include <Xm/PushBG.h>
#include <Xm/DragDrop.h>
#include <Dt/Icon.h>
#include <Dt/IconP.h>
#include <Dt/DtNlUtils.h>
#include <Dt/Connect.h>
#include <Dt/FileM.h>
#include <Dt/Dts.h>
#include "Encaps.h"
#include "SharedProcs.h"

#include <Xm/XmPrivate.h> /* _XmShellIsExclusive */

#include "Desktop.h"
#include "FileMgr.h"
#include "Main.h"
#include "ChangeDir.h"
#include "Prefs.h"
#include "IconicPathP.h"


#define defaultTranslations	_XmManager_defaultTranslations
#define traversalTranslations	_XmManager_managerTraversalTranslations


/********    Static Function Declarations    ********/

static void ClassInitialize( void ) ;
static void ClassPartInitialize( 
                        WidgetClass w_class) ;
static void Initialize(
                        Widget rw,
                        Widget nw,
                        ArgList args,
                        Cardinal *num_args) ;
static void Destroy(
                        Widget w) ;
static void Redisplay( 
                        Widget wid,
                        XEvent *event,
                        Region region) ;
static void Resize( 
                        Widget wid) ;
static XtGeometryResult GeometryManager( 
                        Widget w,
                        XtWidgetGeometry *request,
                        XtWidgetGeometry *reply) ;
static void ChangeManaged( 
                        Widget wid) ;
static Boolean SetValues( 
                        Widget cw,
                        Widget rw,
                        Widget nw,
                        ArgList args,
                        Cardinal *num_args) ;
static XtGeometryResult QueryGeometry( 
                        Widget wid,
                        XtWidgetGeometry *intended,
                        XtWidgetGeometry *desired) ;
static XmNavigability WidgetNavigable( 
                        Widget wid) ;

/********    End Static Function Declarations    ********/


/*--------------------------------------------------------------------
 * Convenience macros
 *------------------------------------------------------------------*/

#define MWD(ip)        ((ip)->iconic_path.margin_width)
#define MHT(ip)        ((ip)->iconic_path.margin_height)
#define SPC(ip)        ((ip)->iconic_path.spacing)
#define MIN_WD(ip)     ((ip)->iconic_path.large_icons? \
                            (ip)->iconic_path.large_min_width: \
                            (ip)->iconic_path.small_min_width)
#define ICON_HT(ip)    ((ip)->iconic_path.large_icons? 32: 16)

#define DESIRED_WIDTH(ip) \
   (MWD(ip) + (ip)->iconic_path.dotdot_button->core.width + MWD(ip) \
    + ((ip)->iconic_path.dropzone? \
         ((ip)->iconic_path.dropzone_icon->core.width + MWD(ip)): 0) \
    + ((ip)->iconic_path.status_msg? \
         ((ip)->iconic_path.status_label->core.width + MWD(ip)): 0))

#define DESIRED_HEIGHT(ip) \
   (MHT(ip) + ICON_HT(ip) + SPC(ip) + \
    (ip)->iconic_path.dotdot_button->core.height + MHT(ip))


/*--------------------------------------------------------------------
 * Resource definitions for IconicPath
 *------------------------------------------------------------------*/

static XmSyntheticResource syn_resources[] =
{
	{	XmNmarginWidth,
		sizeof (Dimension),
		XtOffsetOf( struct _DtIconicPathRec, iconic_path.margin_width),
		XmeFromHorizontalPixels,
		XmeToHorizontalPixels
	},

	{	XmNmarginHeight,
		sizeof (Dimension),
		XtOffsetOf( struct _DtIconicPathRec, iconic_path.margin_height),
		XmeFromVerticalPixels,
		XmeToVerticalPixels
	},
};


static XtResource resources[] =
{
	{	XmNmarginWidth,
		XmCMarginWidth, XmRHorizontalDimension, sizeof (Dimension),
		XtOffsetOf( struct _DtIconicPathRec, iconic_path.margin_width),
		XmRImmediate, (XtPointer) 5
	},

	{	XmNmarginHeight,
		XmCMarginHeight, XmRVerticalDimension, sizeof (Dimension),
		XtOffsetOf( struct _DtIconicPathRec, iconic_path.margin_height),
		XmRImmediate, (XtPointer) 2
	},

	{	XmNspacing,
		XmCSpacing, XmRVerticalDimension, sizeof (Dimension),
		XtOffsetOf( struct _DtIconicPathRec, iconic_path.spacing),
		XmRImmediate, (XtPointer) 4
	},

	{	DtNsmallMinWidth,
		XmCMinWidth, XmRHorizontalDimension, sizeof (Dimension),
		XtOffsetOf( struct _DtIconicPathRec,
		            iconic_path.small_min_width),
		XmRImmediate, (XtPointer) 25
	},

	{	DtNlargeMinWidth,
		XmCMinWidth, XmRHorizontalDimension, sizeof (Dimension),
		XtOffsetOf( struct _DtIconicPathRec,
		            iconic_path.large_min_width),
		XmRImmediate, (XtPointer) 45
	},

	{	DtNforceSmallIcons,
		DtCForceSmallIcons, XmRBoolean, sizeof (Boolean),
		XtOffsetOf( struct _DtIconicPathRec,
		iconic_path.force_small_icons),
		XmRImmediate, (XtPointer) False
	},

	{	DtNforceLargeIcons,
		DtCForceLargeIcons, XmRBoolean, sizeof (Boolean),
		XtOffsetOf( struct _DtIconicPathRec,
		            iconic_path.force_large_icons),
		XmRImmediate, (XtPointer) False
	},

	{	"buttons",
		"Buttons", XmRBoolean, sizeof (Boolean),
		XtOffsetOf( struct _DtIconicPathRec, iconic_path.buttons),
		XmRImmediate, (XtPointer) False
	},

	{	"dropZone",
		"DropZone", XmRBoolean, sizeof (Boolean),
		XtOffsetOf( struct _DtIconicPathRec, iconic_path.dropzone),
		XmRImmediate, (XtPointer) False
	},

	{	"statusMsg",
		"StatusMsg", XmRBoolean, sizeof (Boolean),
		XtOffsetOf( struct _DtIconicPathRec, iconic_path.status_msg),
		XmRImmediate, (XtPointer) True
	},

	{	DtNfileMgrRec,
		DtCfileMgrRec, XmRString, sizeof (char *),
		XtOffsetOf( struct _DtIconicPathRec, iconic_path.file_mgr_rec),
		XmRImmediate, (XtPointer) NULL
	},

	{	DtNcurrentDirectory,
		DtCCurrentDirectory, XmRString, sizeof (char *),
		XtOffsetOf( struct _DtIconicPathRec,
		            iconic_path.current_directory),
		XmRImmediate, (XtPointer) NULL
	},

	{	DtNlargeIcons,
		DtCLargeIcons, XmRBoolean, sizeof (Boolean),
		XtOffsetOf( struct _DtIconicPathRec, iconic_path.large_icons),
		XmRImmediate, (XtPointer) True
	},

	{	DtNiconsChanged,
		DtCIconsChanged, XmRBoolean, sizeof (Boolean),
		XtOffsetOf( struct _DtIconicPathRec, iconic_path.icons_changed),
		XmRImmediate, (XtPointer) True
	},

};



/*--------------------------------------------------------------------
 *
 * Full class record constant
 *
 *------------------------------------------------------------------*/

static XmBaseClassExtRec baseClassExtRec = {
    NULL,
    NULLQUARK,
    XmBaseClassExtVersion,
    sizeof(XmBaseClassExtRec),
    NULL,				/* InitializePrehook	*/
    NULL,				/* SetValuesPrehook	*/
    NULL,				/* InitializePosthook	*/
    NULL,				/* SetValuesPosthook	*/
    NULL,				/* secondaryObjectClass	*/
    NULL,				/* secondaryCreate	*/
    NULL,               		/* getSecRes data	*/
    { 0 },      			/* fastSubclass flags	*/
    NULL,				/* getValuesPrehook	*/
    NULL,				/* getValuesPosthook	*/
    NULL,                               /* classPartInitPrehook */
    NULL,                               /* classPartInitPosthook*/
    NULL,                               /* ext_resources        */
    NULL,                               /* compiled_ext_resources*/
    0,                                  /* num_ext_resources    */
    FALSE,                              /* use_sub_resources    */
    WidgetNavigable,                    /* widgetNavigable      */
    NULL                                /* focusChange          */
};

externaldef( dticonicpathclassrec) DtIconicPathClassRec dtIconicPathClassRec =
{
   {			/* core_class fields      */
      (WidgetClass) &xmManagerClassRec,		/* superclass         */
      "DtIconicPath",				/* class_name         */
      sizeof(DtIconicPathRec),			/* widget_size        */
      ClassInitialize,	        		/* class_initialize   */
      ClassPartInitialize,			/* class_part_init    */
      FALSE,					/* class_inited       */
      Initialize,       			/* initialize         */
      NULL,					/* initialize_hook    */
      XtInheritRealize,				/* realize            */
      NULL,					/* actions	      */
      0,					/* num_actions	      */
      resources,				/* resources          */
      XtNumber(resources),			/* num_resources      */
      NULLQUARK,				/* xrm_class          */
      TRUE,					/* compress_motion    */
      FALSE,					/* compress_exposure  */
      TRUE,					/* compress_enterlv   */
      FALSE,					/* visible_interest   */
      Destroy,			                /* destroy            */
      Resize,           			/* resize             */
      Redisplay,	        		/* expose             */
      SetValues,                		/* set_values         */
      NULL,					/* set_values_hook    */
      XtInheritSetValuesAlmost,	        	/* set_values_almost  */
      NULL,					/* get_values_hook    */
      NULL,					/* accept_focus       */
      XtVersion,				/* version            */
      NULL,					/* callback_private   */
      defaultTranslations,			/* tm_table           */
      QueryGeometry,                    	/* query_geometry     */
      NULL,             	                /* display_accelerator*/
      (XtPointer)&baseClassExtRec,              /* extension          */
   },
   {		/* composite_class fields */
      GeometryManager,    	                /* geometry_manager   */
      ChangeManaged,	                	/* change_managed     */
      XtInheritInsertChild,			/* insert_child       */
      XtInheritDeleteChild,     		/* delete_child       */
      NULL,                                     /* extension          */
   },

   {		/* constraint_class fields */
      NULL,					/* resource list        */   
      0,					/* num resources        */   
      0,					/* constraint size      */   
      NULL,					/* init proc            */   
      NULL,					/* destroy proc         */   
      NULL,					/* set values proc      */   
      NULL,                                     /* extension            */
   },

   {		/* manager_class fields */
      traversalTranslations,			/* translations           */
      syn_resources,				/* syn_resources      	  */
      XtNumber (syn_resources),			/* num_get_resources 	  */
      NULL,					/* syn_cont_resources     */
      0,					/* num_get_cont_resources */
      XmInheritParentProcess,                   /* parent_process         */
      NULL,					/* extension           */    
   },

   {		/* drawingArea class - none */     
      0						/* mumble */
   }	
};

externaldef( dticonicpathwidgetclass) WidgetClass dtIconicPathWidgetClass
                                       = (WidgetClass) &dtIconicPathClassRec ;


/*--------------------------------------------------------------------
 * Activate Callback for iconic path buttons and double click on icons
 *------------------------------------------------------------------*/

static void
ButtonCallback(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{
   DtIconicPathWidget ip = (DtIconicPathWidget)client_data;
   FileMgrRec *file_mgr_rec = (FileMgrRec *) ip->iconic_path.file_mgr_rec;
   DialogData  *dialog_data;
   FileMgrData *file_mgr_data;
   char host_name[MAX_PATH];
   int i;

   if (XtClass(w) == dtIconGadgetClass)
   {
      if (((XmAnyCallbackStruct *)call_data)->reason != XmCR_DEFAULT_ACTION)
         return;
   }

   if ((dialog_data = _DtGetInstanceData ((XtPointer)file_mgr_rec)) == NULL)
      return;
   file_mgr_data = (FileMgrData *) dialog_data->data;
   strcpy(host_name, file_mgr_data->host);

   if (w == ip->iconic_path.dotdot_button)
      i = ip->iconic_path.left_component - 1;
   else
   {
      for (i = 0; i < ip->iconic_path.num_components; i++)
         if (w == ip->iconic_path.components[i].button ||
             w == ip->iconic_path.components[i].icon)
         {
            break;
         }
   }

   if (i == ip->iconic_path.num_components - 1)
      FileMgrReread (file_mgr_rec);

   else if (i < ip->iconic_path.num_components)
      ShowNewDirectory (file_mgr_data, host_name,
                        ip->iconic_path.components[i].path);
}


/*--------------------------------------------------------------------
 * Iconic path update function
 *------------------------------------------------------------------*/

static int
Update(
	DtIconicPathWidget ip,
	FileMgrRec *file_mgr_rec,
	FileMgrData *file_mgr_data)
{
   static Pixmap change_view_pixmap = XmUNSPECIFIED_PIXMAP;
   char *fileLabel;
   Widget *manage;
   int nmanage;
   Dimension iwidth, iheight;
   int twidth;
   Pixel foreground, background;
   Arg args[35];
   char *ptr, *path, *name;
   XmString xm_string;
   int restricted_len;
   int path_len;
   Boolean forbidden;
   int i, j, n;
   int x, y;
   int n_changes = 0;
   PixmapData *pixmapData;
   char msg_buf[21+MAX_PATH];

   /* macro that updates the change count */
# define INC_N_CHANGES() \
   if (n_changes++ == 0) \
   {  /* first change: unmanage all children and clear the window */ \
      XtUnmanageChildren(ip->composite.children, ip->composite.num_children); \
      if (XtIsRealized((Widget)ip)) \
         XClearWindow(XtDisplay(ip), XtWindow(ip)); \
   } else

   /* macro that moves a child and updates the change count */
# define MOVE_OBJECT(w,_x,_y) \
   if ((w)->core.x != (_x) || (w)->core.y != (_y)) \
   { \
      INC_N_CHANGES(); \
      XmeConfigureObject(w, _x, _y, (w)->core.width, (w)->core.height, (w)->core.border_width); \
   } else


   DPRINTF2((
     "IconicPath.Update: cur_dir '%s', dir_shown '%s', icons_changed %d\n",
     ip->iconic_path.current_directory,
     ip->iconic_path.directory_shown? ip->iconic_path.directory_shown: "(nil)",
     ip->iconic_path.icons_changed));

   /* enforce the forceSmallIconsor forceLargeIcons resources */
   if (ip->iconic_path.force_small_icons)
      ip->iconic_path.large_icons = False;
   else if (ip->iconic_path.force_large_icons)
      ip->iconic_path.large_icons = True;

   /* create "..." button, if necessary */
   if (ip->iconic_path.dotdot_button == NULL)
   {
      xm_string = XmStringCreateLocalized("...");
      n = 0;
      XtSetArg (args[n], XmNlabelString, xm_string);            n++;
      XtSetArg (args[n], XmNalignment, XmALIGNMENT_BEGINNING);  n++;
      XtSetArg (args[n], XmNmarginHeight, 0);                   n++;
      XtSetArg (args[n], XmNhighlightThickness, 0);             n++;
      if (ip->iconic_path.buttons)
      {
         XtSetArg (args[n], XmNshadowThickness, 1);                n++;
         XtSetArg (args[n], XmNtraversalOn, False);                n++;
         ip->iconic_path.dotdot_button =
            XmCreatePushButtonGadget((Widget)ip, "iconic_path_button", args, n);
         XtAddCallback(ip->iconic_path.dotdot_button,
                       XmNactivateCallback, ButtonCallback, ip);
      }
      else
      {
         ip->iconic_path.dotdot_button =
            XmCreateLabelGadget((Widget)ip, "iconic_path_label", args, n);
      }
      XmStringFree(xm_string);
      if (ip->iconic_path.dotdot_button->core.width < (Dimension)((Dimension)MIN_WD(ip)/(Dimension)2))
      {
         XmeConfigureObject(ip->iconic_path.dotdot_button,
			    ip->iconic_path.dotdot_button->core.x,
			    ip->iconic_path.dotdot_button->core.y,
			    (Dimension)MIN_WD(ip)/(Dimension)2,
			    ip->iconic_path.dotdot_button->core.height, 0);
      }
   }
   else if (ip->iconic_path.large_shown != ip->iconic_path.large_icons)
   {
      xm_string = XmStringCreateLocalized("...");
      XtSetArg (args[0], XmNlabelString, xm_string);
      XtSetValues(ip->iconic_path.dotdot_button, args, 1);
      XmStringFree(xm_string);
      if (ip->iconic_path.dotdot_button->core.width < (Dimension)((Dimension)MIN_WD(ip)/(Dimension)2))
      {
         XmeConfigureObject(ip->iconic_path.dotdot_button,
			    ip->iconic_path.dotdot_button->core.x,
			    ip->iconic_path.dotdot_button->core.y,
			    (Dimension)MIN_WD(ip)/(Dimension)2,
			    ip->iconic_path.dotdot_button->core.height, 0);
      }
   }

   /* create StatuMsg, if necessary */
   if (ip->iconic_path.status_msg && ip->iconic_path.status_label == NULL)
   {
      /* create StatuMsg */
      n = 0;
      XtSetArg (args[n], XmNmarginHeight, 0);                   n++;
      XtSetArg (args[n], XmNhighlightThickness, 0);             n++;
      ip->iconic_path.status_label =
            XmCreateLabelGadget((Widget)ip, "status_label", args, n);
   }

   foreground = ip->manager.foreground;
   background = ip->core.background_pixel;

   /* create DropZone, if necessary */
   if (ip->iconic_path.dropzone && ip->iconic_path.dropzone_icon == NULL)
   {
      /* create DropZone */
      if (change_view_pixmap == XmUNSPECIFIED_PIXMAP)
      {
         foreground = ip->manager.foreground;
         background = ip->core.background_pixel;
         change_view_pixmap = _DtGetPixmap(XtScreen(ip), CHANGE_VIEW_ICON_M,
                                           foreground, background);
      }
      n = 0;
      XtSetArg (args[n], XmNstring, NULL);                      n++;
      XtSetArg (args[n], XmNshadowThickness, 2);                n++;
      XtSetArg (args[n], XmNfillOnArm, False);                  n++;
      XtSetArg (args[n], XmNhighlightThickness, 0);             n++;
      XtSetArg (args[n], XmNpixmap, change_view_pixmap);        n++;
      XtSetArg (args[n], XmNtraversalOn, False);                n++;
      XtSetArg (args[n], XmNdropSiteOperations,
                   XmDROP_MOVE | XmDROP_COPY | XmDROP_LINK);    n++;
      ip->iconic_path.dropzone_icon =
         _DtCreateIcon ((Widget)ip, "change_view", args, n);

      XtAddCallback (ip->iconic_path.dropzone_icon, XmNdropCallback,
                     DropOnChangeView, (XtPointer) file_mgr_rec);
      XtAddCallback (ip->iconic_path.dropzone_icon, XmNcallback,
                     CurrentDirDropCallback, file_mgr_rec);
   }

   /* if the current directory changed, update component list */
   if (file_mgr_data != NULL && ip->iconic_path.current_directory != NULL &&
       ip->iconic_path.current_directory[0] == '/' &&
       (ip->iconic_path.directory_shown == NULL ||
        strcmp(ip->iconic_path.directory_shown,
               ip->iconic_path.current_directory) != 0 ||
        ip->iconic_path.large_shown != ip->iconic_path.large_icons ||
        ip->iconic_path.icons_changed))
   {
      /* store the new directory */
      XtFree(ip->iconic_path.directory_shown);
      ip->iconic_path.directory_shown =
         XtNewString(ip->iconic_path.current_directory);

      /* for restricted directory: compute length of unshown path */
      if (file_mgr_data->restricted_directory)
      {
         ptr = strrchr(file_mgr_data->restricted_directory, '/');
         if( ptr == file_mgr_data->restricted_directory )
           restricted_len = 1;
         else
           restricted_len = ptr? ptr - file_mgr_data->restricted_directory: 0;
      }
      else
         restricted_len = 0;


      /* get all path components */
      i = 0;
      ptr = ip->iconic_path.directory_shown;
      for (;;)
      {
         /* extract the next path component */
         if (ptr != NULL)
            *ptr = '\0';
         if (ip->iconic_path.directory_shown[0] == '\0')
            path = name = "/";
         else
         {
            path = ip->iconic_path.directory_shown;
            name = strrchr(path, '/') + 1;
         }

         /* don't show path components above a restricted directory */
         path_len = strlen(path);
         if (restricted_len && path_len <= restricted_len)
            goto next_component;

         /* in restricted mode: check if this component is above $HOME */
         forbidden = restrictMode
             && strncmp(path, users_home_dir, path_len) == 0
             && (path_len == 1 ||
                 users_home_dir[path_len] == '/' &&
                 users_home_dir[path_len + 1] != '\0');

         /* check if we need to add or update the path component */
         if (i >= ip->iconic_path.num_components)
         {
            /* create new component */
            INC_N_CHANGES();

            ip->iconic_path.components = (struct _IconicPathComponent *)
               XtRealloc((char *)ip->iconic_path.components,
                         (i + 1)*sizeof(struct _IconicPathComponent));

            ip->iconic_path.components[i].path = XtNewString(path);
            pixmapData = GetPixmapData(file_mgr_rec, 
                                       file_mgr_data,
                                       path,
                                       ip->iconic_path.large_icons);

            n = 0;
            XtSetArg (args[n], XmNstring, NULL);                      n++;
            if (pixmapData)
            {
              XtSetArg (args[n], XmNimageName, pixmapData->iconFileName);
              ip->iconic_path.components[i].icon_name =
                                        XtNewString(pixmapData->iconFileName);
            }
            else
            {
              XtSetArg (args[n], XmNimageName, NULL); 
              ip->iconic_path.components[i].icon_name = NULL;
            }
            n++;


            if ( background == white_pixel )
            {
              XtSetArg (args[n], XmNbackground, white_pixel);         n++;
              XtSetArg (args[n], XmNpixmapBackground, white_pixel);   n++;
              XtSetArg (args[n], XmNpixmapForeground, black_pixel);   n++;
            }
            else if ( background == black_pixel )
            {
              XtSetArg (args[n], XmNbackground, black_pixel);         n++;
              XtSetArg (args[n], XmNpixmapBackground, white_pixel);   n++;
              XtSetArg (args[n], XmNpixmapForeground, black_pixel);   n++;
            }
            else
            {
              XtSetArg (args[n], XmNbackground, background);          n++;
            }


            XtSetArg (args[n], XmNhighlightThickness, 0);             n++;
            XtSetArg (args[n], XmNmarginHeight, 0);                   n++;
            XtSetArg (args[n], XmNmarginWidth, 0);                    n++;
            XtSetArg (args[n], XmNtraversalOn, False);                n++;
            ip->iconic_path.components[i].icon =
               _DtCreateIcon ((Widget)ip, "iconic_path_icon", args, n);

            _DtCheckAndFreePixmapData(
                           GetDirectoryLogicalType(file_mgr_data, path),
                           file_mgr_rec->shell,
                           (DtIconGadget) ip->iconic_path.components[i].icon,
                           pixmapData);

            XtAddCallback (ip->iconic_path.components[i].icon, XmNcallback,
                           ButtonCallback, ip);

            if (fileLabel = DtDtsDataTypeToAttributeValue(
                            GetDirectoryLogicalType(file_mgr_data, path),
                            DtDTS_DA_LABEL,
                            NULL))
            {
               xm_string = XmStringCreateLocalized(fileLabel);
               DtDtsFreeAttributeValue(fileLabel);
            }
            else
            {
               xm_string = XmStringCreateLocalized(name);
            }

            n = 0;
            XtSetArg (args[n], XmNlabelString, xm_string);            n++;
            XtSetArg (args[n], XmNalignment, XmALIGNMENT_BEGINNING);  n++;
            XtSetArg (args[n], XmNmarginHeight, 0);                   n++;
            XtSetArg (args[n], XmNhighlightThickness, 0);             n++;
            if (ip->iconic_path.buttons)
            {
               XtSetArg (args[n], XmNshadowThickness, 1);             n++;
               XtSetArg (args[n], XmNtraversalOn, False);             n++;
               XtSetArg (args[n], XmNsensitive, !forbidden);          n++;
               ip->iconic_path.components[i].button =
                  XmCreatePushButtonGadget ((Widget)ip, "iconic_path_button",
                                            args, n);
               XtAddCallback(ip->iconic_path.components[i].button,
                             XmNactivateCallback, ButtonCallback, ip);
            }
            else
            {
               ip->iconic_path.components[i].button =
                  XmCreateLabelGadget((Widget)ip, "iconic_path_label", args, n);
            }
            XmStringFree(xm_string);

         }
         else
         {
            /* check if the existing component needs to be updated */

            /* check if the path has changed */
            if (strcmp(ip->iconic_path.components[i].path, path) != 0 ||
                ip->iconic_path.large_shown != ip->iconic_path.large_icons)
            {
               INC_N_CHANGES();

               XtFree(ip->iconic_path.components[i].path);
               ip->iconic_path.components[i].path = XtNewString(path);
            }

            if (fileLabel = DtDtsDataTypeToAttributeValue(
                            GetDirectoryLogicalType(file_mgr_data, path),
                            DtDTS_DA_LABEL,
                            NULL))
            {
               xm_string = XmStringCreateLocalized(fileLabel);
               DtDtsFreeAttributeValue(fileLabel);
            }
            else
            {
               xm_string = XmStringCreateLocalized(name);
            }
            XtSetArg (args[0], XmNlabelString, xm_string);
            XtSetArg (args[1], XmNsensitive, !forbidden);
            XtSetValues(ip->iconic_path.components[i].button, args, 2);
            XmStringFree(xm_string);

            /* check if the icon has changed */
            pixmapData = GetPixmapData(file_mgr_rec,
                                       file_mgr_data,
                                       path,
                                       ip->iconic_path.large_icons);

            if (pixmapData)
            {
               if ((pixmapData->iconFileName == NULL) !=
                          (ip->iconic_path.components[i].icon_name == NULL) ||
                   pixmapData->iconFileName != NULL &&
                   strcmp(pixmapData->iconFileName,
                          ip->iconic_path.components[i].icon_name) != 0)
               {
                  INC_N_CHANGES();

                  XtFree(ip->iconic_path.components[i].icon_name);
                  ip->iconic_path.components[i].icon_name =
                                  XtNewString(pixmapData->iconFileName);

                  XtSetArg (args[0], XmNimageName, pixmapData->iconFileName);
                  XtSetValues(ip->iconic_path.components[i].icon, args, 1);
                   
                  _DtCheckAndFreePixmapData(
                          GetDirectoryLogicalType(file_mgr_data, path),
                          file_mgr_rec->shell,
                          (DtIconGadget) ip->iconic_path.components[i].icon,
                          pixmapData);
               }
            }
            else
            {
               XtFree(ip->iconic_path.components[i].icon_name);
               ip->iconic_path.components[i].icon_name = NULL;
               XtSetArg (args[0], XmNimageName, NULL);
               XtSetValues(ip->iconic_path.components[i].icon, args, 1);
            }
         }

         /* update component count */
         i++;

next_component:
         /* go to the next path component */
         if (ptr == NULL)
           break;

         /* restore '/' */
         *ptr = '/';

         /* find next component */
         if (strcmp(ptr, "/") == 0)
            break;
         ptr = DtStrchr(ptr + 1, '/');
      }

      /* free any leftover components */
      for (j = i; j < ip->iconic_path.num_components; j++)
      {
         INC_N_CHANGES();

         XtFree(ip->iconic_path.components[j].path);
         ip->iconic_path.components[j].path = NULL;
         XtFree(ip->iconic_path.components[j].icon_name);
         ip->iconic_path.components[j].icon_name = NULL;
         XtDestroyWidget(ip->iconic_path.components[j].icon);
         XtDestroyWidget(ip->iconic_path.components[j].button);
      }

      ip->iconic_path.num_components = i;
      ip->iconic_path.large_shown = ip->iconic_path.large_icons;
   }

   /* update component widths */
   for (i = 0; i < ip->iconic_path.num_components; i++)
   {
      /* determine width for this component */
      twidth = MIN_WD(ip);
      if (ip->iconic_path.components[i].icon->core.width > (Dimension)twidth)
         twidth = ip->iconic_path.components[i].icon->core.width;
      if (ip->iconic_path.components[i].button->core.width > (Dimension)twidth)
         twidth = ip->iconic_path.components[i].button->core.width;

      if (ip->iconic_path.components[i].button->core.width < (Dimension)twidth)
      {
         /* increment the change count */
         INC_N_CHANGES();

         /* resize */
         XmeConfigureObject(ip->iconic_path.components[i].button,
			    ip->iconic_path.components[i].button->core.x,
			    ip->iconic_path.components[i].button->core.y,
			    twidth,
			    ip->iconic_path.components[i].button->core.height,
			    0);
      }

      ip->iconic_path.components[i].width = twidth;
   }

   /* update the status message */
   if (file_mgr_data && ip->iconic_path.status_msg)
      GetStatusMsg(file_mgr_data, msg_buf);
   else
      strcpy(msg_buf, "");

   if (ip->iconic_path.msg_text == NULL ||
       strcmp(msg_buf, ip->iconic_path.msg_text) != 0)
   {
      /* remember the new status message text */
      XtFree(ip->iconic_path.msg_text);
      ip->iconic_path.msg_text = XtNewString(msg_buf);

      if (ip->iconic_path.status_label)
      {
         /* clear the area under the old status message text */
         if (XtIsRealized((Widget)ip) &&
	     ip->iconic_path.status_label->core.x > 0)
            XClearArea(XtDisplay(ip), XtWindow(ip),
                       ip->iconic_path.status_label->core.x,
                       ip->iconic_path.status_label->core.y,
                       ip->iconic_path.status_label->core.width,
                       ip->iconic_path.status_label->core.height, False);

         /* set a new status message text */
         xm_string = XmStringCreateLocalized(msg_buf);
         XtSetArg (args[0], XmNlabelString, xm_string);
         XtSetValues(ip->iconic_path.status_label, args, 1);
         XmStringFree(xm_string);
      }
   }

   /* set widget size, if necessary */
   if (ip->core.width == 0)
      ip->core.width = DESIRED_WIDTH(ip);
   if (ip->core.height == 0)
      ip->core.height = DESIRED_HEIGHT(ip);

   /* for the layout: check how much of the path will fit */
   twidth = MWD(ip) + ip->iconic_path.dotdot_button->core.width + MWD(ip);
   if (ip->iconic_path.status_msg && ip->iconic_path.dropzone)
   {
      if (ip->iconic_path.status_label->core.width
           >= ip->iconic_path.dropzone_icon->core.width)
         twidth += ip->iconic_path.status_label->core.width + MWD(ip);
      else
         twidth += ip->iconic_path.dropzone_icon->core.width + MWD(ip);
   }
   else if (ip->iconic_path.status_msg)
      twidth += ip->iconic_path.status_label->core.width + MWD(ip);
   else if (ip->iconic_path.dropzone)
      twidth += ip->iconic_path.dropzone_icon->core.width + MWD(ip);


   for (i = ip->iconic_path.num_components; i > 0; i--)
   {
      twidth += SPC(ip) + ip->iconic_path.components[i - 1].width;
      if ((Dimension)twidth > ip->core.width)
         break;
   }

   if (i == 1 && (Dimension)(twidth - ip->iconic_path.dotdot_button->core.width - SPC(ip))
                    <= ip->core.width)
   {
      i--;
   }

   if (i != ip->iconic_path.left_component)
   {
      INC_N_CHANGES();
      ip->iconic_path.left_component = i;
   }

   /* allocate list of to-be-managed children */
   manage = (Widget *)
      XtMalloc((2*(ip->iconic_path.num_components - i) + 2)*sizeof(Widget));
   nmanage = 0;

   /* position & manage the path components */
   x = MWD(ip);
   y = ip->core.height - MHT(ip) - ip->iconic_path.dotdot_button->core.height
        - SPC(ip);

   if (i > 0)
   {
      MOVE_OBJECT(ip->iconic_path.dotdot_button, x, y + SPC(ip));
      manage[nmanage++] = ip->iconic_path.dotdot_button;
      x += ip->iconic_path.dotdot_button->core.width + SPC(ip);
   }

   for (j = i; j < ip->iconic_path.num_components; j++)
   {
      MOVE_OBJECT(ip->iconic_path.components[j].icon,
                  x + SPC(ip),
                  y - ip->iconic_path.components[j].icon->core.height);
      MOVE_OBJECT(ip->iconic_path.components[j].button, x, y + SPC(ip));

      x += ip->iconic_path.components[j].width + SPC(ip);

      manage[nmanage++] = ip->iconic_path.components[j].icon;
      manage[nmanage++] = ip->iconic_path.components[j].button;
   }

   /* position & manage the status message label */
   if (ip->iconic_path.status_msg)
   {
      x = ip->core.width - MWD(ip) -
          (int)ip->iconic_path.status_label->core.width;
      if (x < 0)
         x = 0;
      y = ip->core.height - MHT(ip) -
          (int)ip->iconic_path.status_label->core.height;
      if (y < 0)
         y = 0;

      if (ip->iconic_path.status_label->core.x != x ||
          ip->iconic_path.status_label->core.y != y)
      {
          XmeConfigureObject(ip->iconic_path.status_label, x, y,
			     ip->iconic_path.status_label->core.width,
			     ip->iconic_path.status_label->core.height,
			     ip->iconic_path.status_label->core.border_width);
      }

      manage[nmanage++] = ip->iconic_path.status_label;

      y -= SPC(ip);
   }
   else
      y = ip->core.height;

   /* position & manage the drop zone icon */
   if (ip->iconic_path.dropzone)
   {
      x = ip->core.width - MWD(ip) -
          (int)ip->iconic_path.dropzone_icon->core.width;
      if (x < 0)
         x = 0;
      y = (y - (int)ip->iconic_path.dropzone_icon->core.height + 1)/2;
      if (y < 0)
         y = 0;

      MOVE_OBJECT(ip->iconic_path.dropzone_icon, x, y);

      manage[nmanage++] = ip->iconic_path.dropzone_icon;
   }


   /* if all icons are there, manage the children */
   for (i = 0; i < ip->iconic_path.num_components; i++)
      if (ip->iconic_path.components[i].icon_name == NULL)
         break;
   if (file_mgr_data != NULL && i == ip->iconic_path.num_components)
   {
      DPRINTF2(("IconicPath.Update: manage %d children\n", nmanage));
      XtManageChildren(manage, nmanage);
   }
   XtFree((char *)manage);

   /* reset icons_changed flag */
   ip->iconic_path.icons_changed = False;

   return n_changes;
}


/*--------------------------------------------------------------------
 * Iconic path redraw function
 *------------------------------------------------------------------*/

static void
IconicPathRedraw(
	DtIconicPathWidget ip)
{
   FileMgrRec *file_mgr_rec = (FileMgrRec *)ip->iconic_path.file_mgr_rec;
   DialogData  * dialog_data;
   FileMgrData *file_mgr_data;
   int i;
   int x, y, l;

   if ((dialog_data = _DtGetInstanceData ((XtPointer)file_mgr_rec)) == NULL)
      return;
   file_mgr_data = (FileMgrData *) dialog_data->data;

   if (ip->iconic_path.gc == None)
   {
      XGCValues gc_values;

      gc_values.foreground = ip->manager.foreground;
      gc_values.line_width = 2;

      ip->iconic_path.gc = XCreateGC(XtDisplay(ip), XtWindow(ip),
                                     GCForeground | GCLineWidth, &gc_values);
   }

   x = MWD(ip);
   y = ip->core.height - MHT(ip) - ip->iconic_path.dotdot_button->core.height
        - SPC(ip) - ICON_HT(ip)/2;

   if (ip->iconic_path.left_component > 0)
   {
      XSetLineAttributes(XtDisplay(ip), ip->iconic_path.gc,
                         2, LineOnOffDash, CapNotLast, JoinMiter);
      XSetDashes(XtDisplay(ip), ip->iconic_path.gc, 0, "\04", 1);

      l = ip->iconic_path.dotdot_button->core.width + SPC(ip) - 3;
      l = (l/4) * 4;
      XDrawLine(XtDisplay(ip), XtWindow(ip), ip->iconic_path.gc,
                x + 2 + l, y, x + 2, y);
      x += ip->iconic_path.dotdot_button->core.width + SPC(ip);
      XSetLineAttributes(XtDisplay(ip), ip->iconic_path.gc,
                         2, LineSolid, CapNotLast, JoinMiter);
   }

   for (i = ip->iconic_path.left_component;
        i < ip->iconic_path.num_components - 1;
        i++)
   {
      if (ip->iconic_path.components[i].icon_name != NULL &&
          ip->iconic_path.components[i+1].icon_name != NULL)
      {
         XDrawLine(XtDisplay(ip), XtWindow(ip), ip->iconic_path.gc,
                   x + SPC(ip) +
                       ip->iconic_path.components[i].icon->core.width + 1,
                   y,
                   x + SPC(ip) +
                       ip->iconic_path.components[i].width + SPC(ip) - 1,
                   y);
      }
      x += ip->iconic_path.components[i].width + SPC(ip);
   }
}



/*--------------------------------------------------------------------
 * Class initialize
 *------------------------------------------------------------------*/

static void
ClassInitialize( void )
{   
  baseClassExtRec.record_type = XmQmotif ;
}


static void
ClassPartInitialize(
        WidgetClass w_class )
{   
    _XmFastSubclassInit( w_class, XmDRAWING_AREA_BIT) ;
    return ;
}


/*--------------------------------------------------------------------
 * Instance initialize
 *------------------------------------------------------------------*/

static void
Initialize(
        Widget rw,
        Widget nw,
        ArgList args,
        Cardinal *num_args )
{
    DtIconicPathWidget new_w = (DtIconicPathWidget) nw ;
    FileMgrRec *file_mgr_rec = (FileMgrRec *)new_w->iconic_path.file_mgr_rec;

    new_w->iconic_path.msg_text = NULL;
    new_w->iconic_path.current_directory = NULL;
    new_w->iconic_path.directory_shown = NULL;
    new_w->iconic_path.large_shown = False;
    new_w->iconic_path.status_label = NULL;
    new_w->iconic_path.dotdot_button = NULL;
    new_w->iconic_path.dropzone_icon = NULL;
    new_w->iconic_path.num_components = 0;
    new_w->iconic_path.components = NULL;
    new_w->iconic_path.left_component = 0;
    new_w->iconic_path.gc = None;

    Update(new_w, file_mgr_rec, NULL);

    return;
}


/*--------------------------------------------------------------------
 * Instance destroy
 *------------------------------------------------------------------*/

static void
Destroy(
        Widget w )
{
    DtIconicPathWidget ip = (DtIconicPathWidget) w;
    int i;

    /*
    XtFree(ip->iconic_path.current_directory);
    */
    ip->iconic_path.current_directory = NULL;
    XtFree(ip->iconic_path.msg_text);
    ip->iconic_path.msg_text = NULL;
    XtFree(ip->iconic_path.directory_shown);
    ip->iconic_path.directory_shown = NULL;

    for (i = 0; i < ip->iconic_path.num_components; i++)
    {
       XtFree(ip->iconic_path.components[i].path);
       ip->iconic_path.components[i].path = NULL;
    }

    XtFree((char *)ip->iconic_path.components);
    ip->iconic_path.components = NULL;

    return;
}


/*--------------------------------------------------------------------
 * General redisplay function called on exposure events
 *------------------------------------------------------------------*/

static void
Redisplay(
        Widget wid,
        XEvent *event,
        Region region )
{
    DtIconicPathWidget ip = (DtIconicPathWidget) wid ;

    IconicPathRedraw (ip);

    XmeRedisplayGadgets( (Widget) ip, event, region);
    return ;
}


/*--------------------------------------------------------------------
 * Resize
 *------------------------------------------------------------------*/

static void
Resize(
        Widget wid )
{
    DtIconicPathWidget ip = (DtIconicPathWidget) wid ;
    FileMgrRec *file_mgr_rec = (FileMgrRec *)ip->iconic_path.file_mgr_rec;
    DialogData  *dialog_data;
    FileMgrData *file_mgr_data;

    if ((dialog_data = _DtGetInstanceData ((XtPointer)file_mgr_rec)) == NULL)
       return;
    file_mgr_data = (FileMgrData *) dialog_data->data;

    Update(ip, file_mgr_rec, file_mgr_data);
    return ;
}



/*--------------------------------------------------------------------
 * GeometryManager
 *------------------------------------------------------------------*/

static XtGeometryResult
GeometryManager(
        Widget w,
        XtWidgetGeometry *request,
        XtWidgetGeometry *reply )
{
    DtIconicPathWidget ip;

    ip = (DtIconicPathWidget) w->core.parent;

    if (IsQueryOnly(request)) return XtGeometryYes;

    if (IsWidth(request)) w->core.width = request->width;
    if (IsHeight(request)) w->core.height = request->height;
    if (IsBorder(request)) w->core.border_width = request->border_width;

    /* @@@ adjust layout ? */

    return XtGeometryYes;
}


/*--------------------------------------------------------------------
 * Re-layout children
 *------------------------------------------------------------------*/

static void
ChangeManaged(
        Widget wid )
{
    DtIconicPathWidget ip = (DtIconicPathWidget) wid ;
    XtWidgetProc manager ;

    XmeNavigChangeManaged((Widget) ip) ;

    return;
}


/*--------------------------------------------------------------------
 * SetValues
 *------------------------------------------------------------------*/

static Boolean
SetValues(
        Widget cw,
        Widget rw,
        Widget nw,
        ArgList args,
        Cardinal *num_args )
{
    DtIconicPathWidget current = (DtIconicPathWidget) cw ;
    DtIconicPathWidget ip = (DtIconicPathWidget) nw ;
    FileMgrRec *file_mgr_rec = (FileMgrRec *)ip->iconic_path.file_mgr_rec;
    DialogData *dialog_data;
    FileMgrData *file_mgr_data;
    Boolean redisplay;

    dialog_data = _DtGetInstanceData ((XtPointer)file_mgr_rec);
    file_mgr_data = dialog_data? (FileMgrData *) dialog_data->data: NULL;

    redisplay = (Update(ip, file_mgr_rec, file_mgr_data) > 0);

    if (XtHeight(ip) != DESIRED_HEIGHT(ip)
        || (Dimension)XtWidth(ip) < (Dimension)DESIRED_WIDTH(ip))
    {
       XtWidgetGeometry request;
       XtWidgetGeometry reply;

       request.request_mode = 0;
       if (XtHeight(ip) != DESIRED_HEIGHT(ip))
       {
          request.request_mode |= CWHeight;
          request.height = DESIRED_HEIGHT(ip);
       }
       if ((Dimension)XtWidth(ip) < (Dimension)DESIRED_WIDTH(ip))
       {
          request.request_mode |= CWWidth;
          request.width = DESIRED_WIDTH(ip);
       }

       if (XtMakeGeometryRequest(nw, &request, &reply) != XtGeometryNo)
          Update(ip, file_mgr_rec, file_mgr_data);

       redisplay = True;
    }

    return redisplay;
}


/*--------------------------------------------------------------------
 * QueryGeometry
 *------------------------------------------------------------------*/

static XtGeometryResult
QueryGeometry(
        Widget wid,
        XtWidgetGeometry *intended,
        XtWidgetGeometry *desired )
{
    DtIconicPathWidget ip = (DtIconicPathWidget) wid ;

    desired->width = DESIRED_WIDTH(ip);
    desired->height = DESIRED_HEIGHT(ip);

    /* deal with user initial size setting */
    if (!XtIsRealized(wid))
    {
	if (XtWidth(wid) != 0) desired->width = XtWidth(wid) ;
	if (XtHeight(wid) != 0) desired->height = XtHeight(wid) ;
    }	

    return XmeReplyToQueryGeometry(wid, intended, desired) ;
}

static XmNavigability
WidgetNavigable(
        Widget wid)
{
  if(    wid->core.sensitive
     &&  wid->core.ancestor_sensitive
     &&  ((XmManagerWidget) wid)->manager.traversal_on    )
    {
      XmNavigationType nav_type
	                   = ((XmManagerWidget) wid)->manager.navigation_type ;

      if(    (nav_type == XmSTICKY_TAB_GROUP)
	 ||  (nav_type == XmEXCLUSIVE_TAB_GROUP)
         ||  (    (nav_type == XmTAB_GROUP)
	      &&  !_XmShellIsExclusive( wid))    )
	{
	  return XmDESCENDANTS_TAB_NAVIGABLE ;
	}
    }
  return XmNOT_NAVIGABLE ;
}


/*--------------------------------------------------------------------
 * _DtCreateIconicPath: creates and returns a IconicPath widget.
 *------------------------------------------------------------------*/

Widget
_DtCreateIconicPath(
        Widget p,
        String name,
        ArgList args,
        Cardinal n )
{
    return( XtCreateWidget( name, dtIconicPathWidgetClass, p, args, n)) ;
}


/*--------------------------------------------------------------------
 * UpdateIconicPath: update IconicPath widget.
 *------------------------------------------------------------------*/

void
DtUpdateIconicPath(
	FileMgrRec *file_mgr_rec,
	FileMgrData *file_mgr_data,
	Boolean icons_changed)
{
   Arg args[8];

   XtSetArg (args[0], DtNfileMgrRec, file_mgr_rec);
   XtSetArg (args[1], DtNcurrentDirectory, file_mgr_data->current_directory);
   XtSetArg (args[2], DtNlargeIcons, file_mgr_data->view == BY_NAME_AND_ICON);
   XtSetArg (args[3], DtNiconsChanged, icons_changed);
   XtSetArg (args[4], "statusMsg", !file_mgr_data->show_status_line);
   XtSetValues(file_mgr_rec->iconic_path_da, args, 5);
}

