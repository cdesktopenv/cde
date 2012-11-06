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
/* $TOG: Control.c /main/18 1997/04/22 11:40:14 mgreess $
 *
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1992,1996 Hewlett-Packard Company.
 * (c) Copyright 1996 International Business Machines Corp.
 * (c) Copyright 1996 Sun Microsystems, Inc.
 * (c) Copyright 1996 Novell, Inc. 
 * (c) Copyright 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 */

/**---------------------------------------------------------------------
***	
***	file:		Control.c
***
***	project:	MotifPlus Widgets
***
***	description:	Source code for DtControl class.
***	
***-------------------------------------------------------------------*/


/*-------------------------------------------------------------
**	Include Files
*/

#include <stdio.h>
#ifdef __apollo
#include <sys/types.h>           /* needed for sys/stat.h on bsd4.3 */
#else
#include <stdlib.h>
#endif
#include <sys/stat.h>
#define X_INCLUDE_STRING_H
#define X_INCLUDE_TIME_H
#define XOS_USE_XT_LOCKING
#include <X11/Xos_r.h>
#include <Xm/GadgetP.h>
#include <Xm/DrawP.h>
#include <Xm/XmP.h>
#include <Dt/Control.h>
#include <Dt/ControlP.h>
#include <Xm/ManagerP.h>
#include <Dt/MacrosP.h>
#include <langinfo.h>
#include "DtWidgetI.h"
#include "DtSvcInternal.h"


/********    Public Function Declarations    ********/

extern Widget _DtCreateControl( 
                        Widget parent,
                        String name,
                        ArgList arglist,
                        int argcount) ;

/********    End Public Function Declarations    ********/

/********    Static Function Declarations    ********/

static void ClickTimeout( 
                        XtPointer client_data,
                        XtIntervalId *id) ;
static void BusyTimeout( 
                        XtPointer client_data,
                        XtIntervalId *id) ;
static void DateTimeout( 
                        XtPointer client_data,
                        XtIntervalId *id) ;
static void PushAnimationTimeout( 
                        XtPointer client_data,
                        XtIntervalId *id) ;
static void DropAnimationTimeout( 
                        XtPointer client_data,
                        XtIntervalId *id) ;
static void FileCheckTimeout( 
                        XtPointer client_data,
                        XtIntervalId *id) ;
static void CheckFile( 
                        DtControlGadget g) ;
static void ClassInitialize( void ) ;
static void ClassPartInitialize(
                        WidgetClass cl) ;

static void Destroy (Widget);

static void Initialize( 
                        Widget request_w,
                        Widget new_w) ;
static Boolean SetValues(
        		Widget current_w,
                        Widget request_w,
                        Widget new_w) ;
static void GetSize( 
                        DtControlGadget g,
                        Dimension *w,
                        Dimension *h) ;
static void GetPositions( 
                        DtControlGadget g,
                        Position w,
                        Position h,
                        Dimension h_t,
                        Dimension s_t,
                        Position *pix_x,
                        Position *pix_y,
                        Position *str_x,
                        Position *str_y) ;
static void Draw( 
                        DtControlGadget g,
                        Drawable drawable,
                        Position x,
                        Position y,
                        Dimension w,
                        Dimension h,
                        Dimension h_t,
                        Dimension s_t,
                        unsigned char s_type,
                        unsigned char fill_mode) ;
static void CallCallback( 
                        DtControlGadget g,
                        XtCallbackList cb,
                        int reason,
                        XEvent *event) ;
static void UpdateGCs( 
                        DtControlGadget g) ;
static void ReplaceJPDate(
                        char *date,
                        char *jpstr,
                        int wday) ;

/********    End Static Function Declarations    ********/


/*-------------------------------------------------------------
**	Public Interface
**-------------------------------------------------------------
*/

WidgetClass	dtControlGadgetClass;


#define UNSPECIFIED_DIMENSION	9999
#define UNSPECIFIED_CHAR	255

#ifdef _SUN_OS
static const char _DtMsgIndicator_0000[] = "/var/mail/";
#elif defined(__osf__)
static const char _DtMsgIndicator_0000[] = "/usr/spool/mail/";
#else
static const char _DtMsgIndicator_0000[] = "/usr/mail/";
#endif

#define	MAIL_DIR	_DtMsgIndicator_0000
#define Min(x, y)    (((x) < (y)) ? (x) : (y))
#define Max(x, y)    (((x) > (y)) ? (x) : (y))

#define G_Width(r)              (r -> rectangle.width)
#define G_Height(r)             (r -> rectangle.height)

/*-------------------------------------------------------------
**	Resource List
*/
#define R_Offset(field) \
	XtOffset (DtControlGadget, control.field)
static XtResource resources[] = 
{
	{
		"_do_update",
		XmCBoolean, XmRBoolean, sizeof (Boolean),
		R_Offset (_do_update), XmRImmediate, (XtPointer) FALSE
	},
	{
		XmNuseEmbossedText,
		XmCBoolean, XmRBoolean, sizeof (Boolean),
		R_Offset (use_embossed_text), XmRImmediate, (XtPointer) TRUE
	},
	{
		XmNpushFunction,
		XmCPushFunction, XmRFunction, sizeof (XtPointer),
		R_Offset (push_function),
		XmRImmediate, (XtPointer) NULL
	},
	{
		XmNpushArgument,
		XmCPushArgument, XmRPointer, sizeof (XtPointer),
		R_Offset (push_argument),
		XmRImmediate, (XtPointer) NULL
	},
	{
		XmNdropAction,
		XmCDropAction, XmRString, sizeof (String),
		R_Offset (drop_action),
		XmRImmediate, (XtPointer) NULL
	},
	{
		"pushAction",
		"PushAction", XmRString, sizeof (String),
		R_Offset (push_action),
		XmRImmediate, (XtPointer) NULL
	},
	{
		XmNformat,
		XmCFormat, XmRString, sizeof (String),
		R_Offset (format),
		XmRImmediate, (XtPointer) "%b%n%e"
	},
        {
                XmNformatJP,
                XmCFormatJP, XmRString, sizeof (String),
                R_Offset (format_jp),
                XmRImmediate, (XtPointer) NULL
        },
	{
		XmNsubpanel,
		XmCSubpanel, XmRWidget, sizeof (Widget),
		R_Offset (subpanel),
		XmRImmediate, (XtPointer) NULL
	},
	{
		XmNcontrolType,
		XmCControlType, XmRControlType, sizeof (unsigned char),
		R_Offset (control_type),
		XmRImmediate, (XtPointer) XmCONTROL_NONE
	},
	{
		XmNfileName,
		XmCString, XmRString, sizeof (XmString),
		R_Offset (file_name), XmRImmediate, (XtPointer) NULL
	},
	{
		XmNalternateImage,
		XmCAlternateImage, XmRString, sizeof (String),
		R_Offset (alt_image), XmRImmediate, (XtPointer) NULL
	},
	{
		XmNchime,
		XmCBoolean, XmRBoolean, sizeof (Boolean),
		R_Offset (chime), XmRImmediate, (XtPointer) FALSE
	},
	{
		XmNclientTimeoutInterval,
		XmCInterval, XmRInt, sizeof (int),
		R_Offset (max_blink_time), XmRImmediate, (XtPointer) 10000
	},
	{
		XmNwaitingBlinkRate,
		XmCInterval, XmRInt, sizeof (int),
		R_Offset (blink_time), XmRImmediate, (XtPointer) 500
	},
	{
		XmNmonitorTime,
		XmCInterval, XmRInt, sizeof (int),
		R_Offset (monitor_time), XmRImmediate, (XtPointer) 30000
	},
	{
		XmNpushButtonClickTime,
		XmCInterval, XmRInt, sizeof (int),
		R_Offset (click_time), 
		XmRImmediate, (XtPointer) 1000
	},
	{
		XmNuseLabelAdjustment,
		XmCBoolean, XmRBoolean, sizeof (Boolean),
		R_Offset (use_label_adj), XmRImmediate, (XtPointer) TRUE
	}
};
#undef	R_Offset


static XmBaseClassExtRec       controlBaseClassExtRec = {
    NULL,                                     /* Next extension       */
    NULLQUARK,                                /* record type XmQmotif */
    XmBaseClassExtVersion,                    /* version              */
    sizeof(XmBaseClassExtRec),                /* size                 */
    XmInheritInitializePrehook,               /* initialize prehook   */
    XmInheritSetValuesPrehook,                /* set_values prehook   */
    XmInheritInitializePosthook,              /* initialize posthook  */
    XmInheritSetValuesPosthook,               /* set_values posthook  */
    (WidgetClass)&dtControlCacheObjClassRec,  /* secondary class      */
    XmInheritSecObjectCreate,                 /* creation proc        */
    XmInheritGetSecResData,                   /* getSecResData        */
    {0},                                      /* fast subclass        */
    XmInheritGetValuesPrehook,                /* get_values prehook   */
    XmInheritGetValuesPosthook,               /* get_values posthook  */
    NULL,                                     /* classPartInitPrehook */
    NULL,                                     /* classPartInitPosthook*/
    NULL,                                     /* ext_resources        */
    NULL,                                     /* compiled_ext_resources*/
    0,                                        /* num_ext_resources    */
    FALSE,                                    /* use_sub_resources    */
    XmInheritWidgetNavigable,                 /* widgetNavigable      */
    XmInheritFocusChange,                     /* focusChange          */
};

/* ext rec static initialization */
externaldef (xmcontrolcacheobjclassrec)
DtControlCacheObjClassRec dtControlCacheObjClassRec =
{
  {
      /* superclass         */    (WidgetClass) &xmExtClassRec,
      /* class_name         */    "DtControl",
      /* widget_size        */    sizeof(DtControlCacheObjRec),
      /* class_initialize   */    NULL,
      /* chained class init */    NULL,
      /* class_inited       */    False,
      /* initialize         */    NULL,
      /* initialize hook    */    NULL,
      /* realize            */    NULL,
      /* actions            */    NULL,
      /* num_actions        */    0,
      /* resources          */    NULL,
      /* num_resources      */    0,
      /* xrm_class          */    NULLQUARK,
      /* compress_motion    */    False,
      /* compress_exposure  */    False,
      /* compress enter/exit*/    False,
      /* visible_interest   */    False,
      /* destroy            */    NULL,
      /* resize             */    NULL,
      /* expose             */    NULL,
      /* set_values         */    NULL,
      /* set values hook    */    NULL,
      /* set values almost  */    NULL,
      /* get values hook    */    NULL,
      /* accept_focus       */    NULL,
      /* version            */    XtVersion,
      /* callback offsetlst */    NULL,
      /* default trans      */    NULL,
      /* query geo proc     */    NULL,
      /* display accelerator*/    NULL,
      /* extension record   */    (XtPointer)NULL,
   },

   {
      /* synthetic resources */   NULL,
      /* num_syn_resources   */   0,
      /* extension           */   NULL,
   }
};
	

/*-------------------------------------------------------------
**	Class Record
*/
DtControlClassRec dtControlClassRec =
{
	/*	Core Part
	*/
	{	
		(WidgetClass) &dtIconClassRec, /* superclass		*/
		"Control",			/* class_name		*/
		sizeof (DtControlRec),  	/* widget_size		*/
		ClassInitialize,		/* class_initialize	*/
		ClassPartInitialize,		/* class_part_initialize*/
		False,				/* class_inited		*/
		(XtInitProc) Initialize,	/* initialize		*/
		NULL,				/* initialize_hook	*/
		NULL,				/* realize		*/
		NULL,				/* actions		*/
		0,				/* num_actions		*/
		resources,			/* resources		*/
		XtNumber (resources),		/* num_resources	*/

		NULLQUARK,			/* xrm_class		*/
		True,				/* compress_motion	*/
		True,				/* compress_exposure	*/
		True,				/* compress_enterleave	*/
		False,				/* visible_interest	*/	
		Destroy,		 	/* destroy		*/	
		XtInheritResize,		/* resize		*/
		XtInheritExpose,		/* expose		*/	
		(XtSetValuesFunc)SetValues,	/* set_values		*/	
		NULL,				/* set_values_hook	*/
		XtInheritSetValuesAlmost,	/* set_values_almost	*/
		NULL,				/* get_values_hook	*/
		NULL,				/* accept_focus		*/	
		XtVersion,			/* version		*/
		NULL,				/* callback private	*/
		NULL,				/* tm_table		*/
		NULL,				/* query_geometry	*/
		NULL,				/* display_accelerator	*/
		(XtPointer)&controlBaseClassExtRec, /* extension	*/

	},

	/*	XmGadget Part
	*/
	{
(XtWidgetProc)	XmInheritBorderHighlight,	/* border_highlight	*/
(XtWidgetProc)	XmInheritBorderUnhighlight,	/* border_unhighlight	*/
(XtActionProc)	XmInheritArmAndActivate,	/* arm_and_activate	*/
(XmWidgetDispatchProc)	XmInheritInputDispatch,	/* input_dispatch	*/
(XmVisualChangeProc)	XmInheritVisualChange,	/* visual_change	*/
		NULL,				/* get_resources	*/
		0,				/* num_get_resources	*/
		XmInheritCachePart,		/* class_cache_part	*/
		NULL,	 			/* extension		*/
	},

	/*	DtIcon Part
	*/
	{
(GetSizeProc)	GetSize,			/* get_size		*/
(GetPositionProc)	GetPositions,			/* get_positions	*/
(DrawProc)	Draw,				/* draw			*/
(CallCallbackProc)	CallCallback,			/* call_callback	*/
(UpdateGCsProc)	UpdateGCs,			/* update_gcs		*/
		True,				/* optimize_redraw	*/
		NULL,				/* class_cache_part	*/
		NULL,				/* extension		*/
	},

	/*	DtControl Part
	*/
	{
		NULL,				/* class_cache_part	*/
		NULL,				/* extension		*/
	}
};


WidgetClass dtControlGadgetClass = (WidgetClass) &dtControlClassRec;



/*-------------------------------------------------------------
**	Private Functions
**-------------------------------------------------------------
*/

/*-------------------------------------------------------------
**	FileCheckTimeout
**		Check for change in file, reset timeout.
**		An XtTimerCallbackProc.
*/
static void
FileCheckTimeout(
        XtPointer client_data,
        XtIntervalId *id )
{
	DtControlGadget	g = (DtControlGadget) client_data;
	XtAppContext		app_context =
			XtWidgetToApplicationContext ((Widget) g);

	CheckFile (g);

	G_MonitorTimer (g) = 
		XtAppAddTimeOut (app_context, G_MonitorTime (g),
				FileCheckTimeout, (XtPointer) g);
}


/*-------------------------------------------------------------
**	CheckFile
**		Check for change in file size
*/
static void 
CheckFile(
        DtControlGadget g )
{
  Boolean		file_changed;
  long		file_size = 0;
  struct stat	stat_buf;
  
  if (stat (G_FileName (g), &stat_buf) == 0)
    {
      file_size = stat_buf.st_size;
    }
  
  switch (G_ControlType (g))
    {
    case XmCONTROL_MONITOR:
      file_changed = (file_size == 0) ? False : True;
      break;

    case XmCONTROL_MAIL:
      if (file_size == 0)
	file_changed = False;
      else if (stat_buf.st_atime > stat_buf.st_mtime)
	file_changed = False;
      else if (file_size == G_FileSize (g))
	file_changed = G_FileChanged (g);
      else if (file_size > G_FileSize (g))
	file_changed = True;
      else
	file_changed = False;
      break;
    }
  
  G_FileSize (g) = file_size;
  if (file_changed != G_FileChanged (g))
    {
      XtExposeProc expose;
      CallCallbackProc call_callback;

      _DtProcessLock();
      expose = XtCoreProc((Widget)g, expose);
      call_callback = C_CallCallback(XtClass(g));
      _DtProcessUnlock();

      G_FileChanged (g) = file_changed;
      (*call_callback) ((DtIconGadget) g, G_Callback (g), XmCR_MONITOR, NULL);
      (*expose)((Widget)g, NULL, NULL);
      XmUpdateDisplay ((Widget) g);
    }
}



/*-------------------------------------------------------------
**	ClickTimeout
**		An XtTimerCallbackProc.
**		
*/
static void
ClickTimeout(
        XtPointer client_data,
        XtIntervalId *id )
{
  DtControlGadget	g = 	(DtControlGadget) client_data;

  G_ClickTimer (g) = 0;
}



/*-------------------------------------------------------------
**	BusyTimeout
**		Switch pixmap.
**              An XtTimerCallbackProc.
*/
static void
BusyTimeout(
        XtPointer client_data,
        XtIntervalId *id )
{
  DtControlGadget	g = 	(DtControlGadget) client_data;
  XtAppContext		app_context = XtWidgetToApplicationContext((Widget) g);
  XtExposeProc		expose;
  CallCallbackProc	call_callback;
  
  _DtProcessLock();
  expose = XtCoreProc((Widget)g, expose);
  call_callback = C_CallCallback(XtClass((Widget)g));
  _DtProcessUnlock();
  
  if ((G_Busy (g) > 0) && (G_BlinkElapsed (g) < G_MaxBlinkTime (g)))
    {
      G_Set (g) = ! G_Set (g);
      G_BlinkTimer (g) = 
	XtAppAddTimeOut (app_context, G_BlinkTime (g),
			 BusyTimeout, (XtPointer) g);
      G_BlinkElapsed (g) += G_BlinkTime (g);
    }
  else
    {
      G_Set (g) = False;
      G_BlinkTimer (g) = 0;
      G_BlinkElapsed (g) = 0;
      G_Busy (g) = 0;
      (*call_callback) ((DtIconGadget) g, G_Callback (g), XmCR_BUSY_STOP, NULL);
    }
  
  (*expose) ((Widget) g, NULL, False);
}



/*-------------------------------------------------------------
**	DateTimeout
**		Update date strings.
**              An XtTimerCallbackProc.
*/
static void
DateTimeout(
        XtPointer client_data,
        XtIntervalId *id )
{
  DtControlGadget	g = 	(DtControlGadget) client_data;
  XtAppContext		app_context =
    XtWidgetToApplicationContext ((Widget) g);
  char			sTime[128];
  struct tm		*timeptr;
  time_t	 	tse;
  String	 	s1, s2, s3;
  unsigned long		tilMidnight;
  XtExposeProc		expose;
  
  /*	Get time string.
   */
  tse = time (NULL);
  timeptr = _XLocaltime(&tse, localtime_buf);
  strftime ((char*) sTime, 128, G_Format (g), timeptr);
  if (g->control.format_jp != NULL && strlen(g->control.format_jp) != 0)
    ReplaceJPDate(sTime, g->control.format_jp, timeptr->tm_wday);

  /*	Break time string between date and day of week.
   */
  s1 = sTime;
  s2 = strchr (sTime, '\n');
  
  if (s2 == NULL)
    s3 = NULL;
  else
    {
      s3 = strchr (s2+1, '\n');

      if (s3 != NULL)
	{
	  s2[0] = ' ';
	  s2 = s3;
	}
	
      s2[0] = '\0';
      s2++;
    }
  
  /*	Create date string.
   */
  if (s1 != NULL)
    {
      if (s2 == NULL)
	{
	  XmStringFree(G_String(g));
	  G_String (g) = XmStringCreate (s1, XmFONTLIST_DEFAULT_TAG);
	  XmStringExtent (G_FontList (g), G_String (g),
			  &(G_StringWidth (g)), &(G_StringHeight (g)));

	  if (G_AltString (g) != NULL)
	    {
	      XmStringFree (G_AltString (g));
	      G_AltString (g) = NULL;
	      G_AltStringHeight (g) = 0;
	      G_AltStringWidth (g) = 0;
	    }
	}
      else
	{
	  if (!G_UseLabelAdjustment (g))
	    {
	      /* swap s1 and s2 */
	      s3 = s1;
	      s1 = s2;
	      s2 = s3;
	    }
	  if (G_AltString (g) != NULL)
	    XmStringFree (G_AltString (g));
	  G_AltString (g) =  XmStringCreate (s1, XmFONTLIST_DEFAULT_TAG);
	  XmStringExtent (G_FontList (g), G_AltString (g),
			  &(G_AltStringWidth (g)), &(G_AltStringHeight (g)));
	  XmStringFree(G_String(g));
	  G_String (g) = XmStringCreate (s2, XmFONTLIST_DEFAULT_TAG);
	  XmStringExtent (G_FontList (g), G_String (g),
			  &(G_StringWidth (g)), &(G_StringHeight (g)));
	}
    }

  /*	Update display.
   */
  _DtProcessLock();
  expose = XtCoreProc((Widget)g, expose);
  _DtProcessUnlock();
  (*expose) ((Widget) g, NULL, False);
  XmUpdateDisplay ((Widget) g);
  
  /*	Compute milliseconds until midnight:
   *	    a. compute current time in seconds
   *	    b. subtract that from number of seconds in a day.
   *	    c. multiply by 1000 to get milliseconds.
   *	Add timeout.
   */
  tilMidnight = 1000 * (86400 - ((3600 * timeptr->tm_hour) +
				 (60 * timeptr->tm_min) +
				 (timeptr->tm_sec)));
  
  if (G_Format (g) != NULL)
    {
      G_DateTimer (g) = 
	XtAppAddTimeOut (app_context, tilMidnight, 
			 DateTimeout, (XtPointer) g);
    }
  else
    {
      G_DateTimer (g) = 0;
    }
}



/*-------------------------------------------------------------
**	Action Procs
**-------------------------------------------------------------
*/



/*-------------------------------------------------------------
**	Core Procs
**-------------------------------------------------------------
*/


/*-------------------------------------------------------------
**	ClassInitialize
*/

static void
ClassInitialize( void )
{
  controlBaseClassExtRec.record_type = XmQmotif;
}


/*-------------------------------------------------------------
**	ClassPartInitialize
*/

static void
ClassPartInitialize(
        WidgetClass cl )
{
    dtControlCacheObjClassRec.object_class.resources =
				 dtIconCacheObjClassRec.object_class.resources;

    dtControlCacheObjClassRec.object_class.num_resources =
			 dtIconCacheObjClassRec.object_class.num_resources;
}



/*-------------------------------------------------------------
**	Initialize
**		Initialize a new gadget instance.
*/
static void 
Initialize(
        Widget request_w,
        Widget new_w )
{
  DtControlGadget	request = (DtControlGadget) request_w,
  new = (DtControlGadget) new_w;
  XtAppContext	app_context =
    XtWidgetToApplicationContext (new_w);
  String		str;			
  UpdateGCsProc	update_gcs;
  
  G_MonitorTimer (new) = 0;
  G_ClickTimer (new) = 0;
  G_DateTimer (new) = 0;
  G_BlinkTimer (new) = 0;
  G_PushAnimationTimer (new) = 0;
  G_DropAnimationTimer (new) = 0;
  
  G_FileSize (new) = 0;
  G_FileChanged (new) = False;
  
  G_Busy (new) = 0;
  G_AltString (new) = NULL;
  G_AltStringWidth (new) = 0;
  G_AltStringHeight (new) = 0;
  
  /*	Initialize animation data.
   */
  G_NumPushImages (new) = 0;
  G_MaxPushImages (new) = 0;
  G_PushImagePosition (new) = 0;
  G_PushDelays (new) = NULL;
  G_PushPixmaps (new) = NULL;
  G_PushMasks (new) = NULL;
  
  G_NumDropImages (new) = 0;
  G_MaxDropImages (new) = 0;
  G_DropImagePosition (new) = 0;
  G_DropDelays (new) = NULL;
  G_DropPixmaps (new) = NULL;
  G_DropMasks (new) = NULL;
  
  /*	Copy drop argument.
   */
  if (G_DropAction (new) != NULL)
    {
      String	str = G_DropAction (new);
      G_DropAction (new) = XtMalloc (strlen (str) + 1);
      strcpy (G_DropAction (new), str);
    }
  
  if (G_PushAction (new) != NULL)
    {
      String    str = G_PushAction (new);
      G_PushAction (new) = XtMalloc (strlen (str) + 1);
      strcpy (G_PushAction (new), str);
    }
  
  /*	Validate control type.
   */
  if (G_ControlType (new) != XmCONTROL_NONE &&
      G_ControlType (new) != XmCONTROL_SWITCH &&
      G_ControlType (new) != XmCONTROL_BUTTON &&
      G_ControlType (new) != XmCONTROL_BLANK &&
      G_ControlType (new) != XmCONTROL_DATE &&
      G_ControlType (new) != XmCONTROL_BUSY &&
      G_ControlType (new) != XmCONTROL_CLIENT &&
      G_ControlType (new) != XmCONTROL_MAIL &&
      G_ControlType (new) != XmCONTROL_MONITOR)
    {
      G_ControlType (new) = XmCONTROL_NONE;
    }
  
  if (G_ControlType (new) == XmCONTROL_SWITCH)
    G_ShadowType (new) = (G_Set (new)) ? XmSHADOW_IN : XmSHADOW_OUT;
  
  if ( ((G_ControlType (new) == XmCONTROL_MONITOR) ||
	(G_ControlType (new) == XmCONTROL_MAIL) ||
	(G_ControlType (new) == XmCONTROL_BUSY)) &&
      (G_AltImage (new) != NULL))
    {

      /*	Copy string or default to user mail directory.
       */
      if ((G_ControlType (new) == XmCONTROL_MONITOR) ||
	  (G_ControlType (new) == XmCONTROL_MAIL))
	{
	  if (G_FileName (new) == NULL)
	    {
	      if (G_ControlType (new) == XmCONTROL_MAIL)
		{
		  if ((str = getenv ("MAIL")) == NULL)
		    {
		      str = getenv ("LOGNAME");
		      G_FileName (new) = 
			XtMalloc (strlen (MAIL_DIR) + strlen (str) + 1);
		      strcpy (G_FileName (new), MAIL_DIR);
		      strcat (G_FileName (new), str);
		    }
		  else
		    {
		      G_FileName (new) = XtMalloc (strlen (str) + 1);
		      strcpy (G_FileName (new), str);
		    }
		}
	    }
	  else
	    {
	      str = G_FileName (new);
	      G_FileName (new) = XtMalloc (strlen (str) + 1);
	      strcpy (G_FileName (new), str);
	    }

	  G_FileChanged (new) = False;
	}
	
      /*	Get alternate pixmap and mask.
       */
      G_AltPix (new) =
	XmGetPixmap (XtScreen (new), G_AltImage (new),
		     G_PixmapForeground (new),
		     G_PixmapBackground (new));
      G_AltMask (new) =
	_DtGetMask (XtScreen (new), G_AltImage (new));
      G_AltImage (new) = NULL;

      if ((G_AltPix (new) != XmUNSPECIFIED_PIXMAP) &&
	  ((G_ControlType (new) == XmCONTROL_MONITOR) ||
	   (G_ControlType (new) == XmCONTROL_MAIL)))
	{
	  CheckFile (new);
	  G_MonitorTimer (new) =
	    XtAppAddTimeOut (app_context, G_MonitorTime (new),
			     FileCheckTimeout, (XtPointer) new);
	}

      /*	Check for Control state change.
       */
    }
  else
    {
      G_AltPix (new) = XmUNSPECIFIED_PIXMAP;
      G_AltMask (new) = XmUNSPECIFIED_PIXMAP;
      G_MonitorTimer (new) = 0;
    }
  
  if (G_ControlType (new) == XmCONTROL_DATE)
    {
      Dimension	w, h;

      DateTimeout ((XtPointer) new_w, NULL);

      if (G_Width (request) == 0 || G_Height (request) == 0)
        {
	  GetSizeProc get_size;

	  _DtProcessLock();
	  get_size = C_GetSize(XtClass(new));
	  _DtProcessUnlock();
	  (*get_size) ((DtIconGadget) new, &w, &h);
	  if (G_Width (request) == 0)
	    G_Width (new) = w;
	  if (G_Height (request) == 0)
	    G_Height (new) = h;
        }
    }
  
  G_ArmedGC (new) = NULL;
  
  G_TopShadowGC (new) = NULL;
  G_BottomShadowGC (new) = NULL;
  G__DoUpdate (new) = True;
  _DtProcessLock();
  update_gcs = C_UpdateGCs(XtClass(new));
  _DtProcessUnlock();
  (*update_gcs) ((DtIconGadget) new);
  G__DoUpdate (new) = False;
}



/*-------------------------------------------------------------
**	Destroy
**		Release resources allocated for gadget.
*/

static void 
Destroy(
        Widget w )
{
  DtControlGadget	g =	(DtControlGadget) w;
  
  if (G_FileName (g) != NULL)
    XtFree (G_FileName (g));
  
  if (G_NumPushImages (g) > 0)
    {
      XtFree ((char*) G_PushDelays (g));
      XtFree ((char*) G_PushPixmaps (g));
      XtFree ((char*) G_PushMasks (g));
    }
  if (G_NumDropImages (g) > 0)
    {
      XtFree ((char*) G_DropDelays (g));
      XtFree ((char*) G_DropPixmaps (g));
      XtFree ((char*) G_DropMasks (g));
    }
  if (G_DropAction (g) != NULL)
    XtFree (G_DropAction (g));
  if (G_PushAction (g) != NULL)
    XtFree (G_PushAction (g));
  
  if (G_AltString (g) != NULL)
    XmStringFree (G_AltString (g));
  
  if (G_ClickTimer (g))
    XtRemoveTimeOut (G_ClickTimer (g));
  if (G_BlinkTimer (g))
    XtRemoveTimeOut (G_BlinkTimer (g));
  if (G_MonitorTimer (g))
    XtRemoveTimeOut (G_MonitorTimer (g));
  if (G_DateTimer (g))
    XtRemoveTimeOut (G_DateTimer (g));
  if (G_PushAnimationTimer (g))
    XtRemoveTimeOut (G_PushAnimationTimer (g));
  if (G_DropAnimationTimer (g))
    XtRemoveTimeOut (G_DropAnimationTimer (g));
}



/*-------------------------------------------------------------
**	SetValues
**		
*/
static Boolean 
SetValues(
        Widget current_w,
        Widget request_w,
        Widget new_w )
{


    DtControlGadget	current_c =	(DtControlGadget) current_w,
			new_c =		(DtControlGadget) new_w;
#if 0
	String		file_name =	G_FileName (new);
	int		file_name_size;				
	Boolean		redraw_flag =	False;
#endif /*  0 */

    if (strcmp(G_Format(new_c),G_Format(current_c)) &&
        G_ControlType (new_c) == XmCONTROL_DATE)
    {
       if (G_DateTimer (new_c))
    	  XtRemoveTimeOut (G_DateTimer (new_c));

       if (G_Format(new_c) != NULL)
          DateTimeout ((XtPointer) new_w, NULL);
    }

    if (G_ImageName (new_c) && (G_ImageName (current_c) != G_ImageName (new_c)))
       return True;

    return False;

#if 0

/*	Copy drop argument.
*/
if (G_DropAction (new) != G_DropAction (current))
    {
    String	str = G_DropAction (new);
    if (G_DropAction (current) != NULL)
	XtFree (G_DropAction (current));
    G_DropAction (new) = XtMalloc (strlen (str) + 1);
    strcpy (G_DropAction (new), str);
    }

if (G_PushAction (new) != G_PushAction (current))
    {
    String    str = G_PushAction (new);
    if (G_PushAction (current) != NULL)
      XtFree (G_PushAction (current));
    G_PushAction (new) = XtMalloc (strlen (str) + 1);
    strcpy (G_PushAction (new), str);
    }

/*	Copy string or default to user mail directory.
*/
if ((G_ControlType (new) == XmCONTROL_MONITOR) ||
    (G_ControlType (new) == XmCONTROL_MAIL))
    {
	if (G_FileName (new) != G_FileName (current))
	{
		if (G_FileName (current) != NULL)
			XtFree (G_FileName (current));
		if (G_FileName (new) != NULL)
		{
			file_name_size = strlen (G_FileName (new));
			G_FileName (new) = XtMalloc (file_name_size);
			strcpy (G_FileName (new), file_name);
		}
	}
    }

	return (redraw_flag);
#endif /* 0 */
}


/*-------------------------------------------------------------
**	Gadget Procs
**-------------------------------------------------------------
*/


/*-------------------------------------------------------------
**	Icon Procs
**-------------------------------------------------------------
*/

/*-------------------------------------------------------------
**	GetSize
**		Compute size.
*/
static void 
GetSize(
        DtControlGadget g,
        Dimension *w,
        Dimension *h )
{
  Dimension	s_t = G_ShadowThickness (g);
  Dimension	h_t = G_HighlightThickness (g);
  Dimension	p_w = G_PixmapWidth (g);
  Dimension	p_h = G_PixmapHeight (g);
  Dimension	m_w = G_MarginWidth (g);
  Dimension	m_h = G_MarginHeight (g);
  Dimension	s_w = G_StringWidth (g);
  Dimension	s_h = G_StringHeight (g);
  Dimension	v_pad = 2 * (s_t + h_t + m_h);
  Dimension	h_pad = 2 * (s_t + h_t + m_w);
  Dimension	spacing = G_Spacing (g);
  
  if (((p_w == 0) && (p_h == 0)) || ((s_w == 0) && (s_h == 0)))
    spacing = 0;
  
  if (G_ControlType (g) == XmCONTROL_DATE)
    {
      /*    Adjust size based on second date string.
       */
      if ( (G_StringWidth (g) > 0) &&
	  (G_AltStringWidth (g) > G_StringWidth (g)) )
	s_w = G_AltStringWidth (g);
      if ( (G_StringWidth (g) > 0) && (G_AltStringWidth (g) > 0) )
	s_h += spacing + G_AltStringHeight (g);
    }
  else
    {
      /*    Add left and right spacing for string.
       */
      if (s_w > 0)
	s_w += 4;
    }
  
  /*	Get width and height.
   */
  switch ((int) G_PixmapPosition (g))
    {
    case XmPIXMAP_TOP:
    case XmPIXMAP_BOTTOM:
      *w = Max (p_w, s_w) + h_pad;
      *h = p_h + s_h + v_pad + spacing;
      break;
    case XmPIXMAP_LEFT:
    case XmPIXMAP_RIGHT:
      *w = p_w + s_w + h_pad + spacing;
      *h = Max (p_h, s_h) + v_pad;
      break;
    case XmPIXMAP_MIDDLE:
      *w = Max (p_w, s_w) + h_pad;
      *h = Max (p_h, s_h) + v_pad;
      break;
    }
}



/*-------------------------------------------------------------
**	GetPositions
**		Get positions of string and pixmap.
*/
static void 
GetPositions(
        DtControlGadget g,
        Position w,
        Position h,
        Dimension h_t,
        Dimension s_t,
        Position *pix_x,
        Position *pix_y,
        Position *str_x,
        Position *str_y )
{
	Dimension	p_w =		G_PixmapWidth (g),
			p_h =		G_PixmapHeight (g),
			s_w =		G_StringWidth (g),
			s_h =		G_StringHeight (g),
			m_w =		G_MarginWidth (g),
			m_h =		G_MarginHeight (g),
			spacing =	G_Spacing (g),
			h_pad =		s_t + h_t + m_w,
			v_pad =		s_t + h_t + m_h,
			width =		w - 2 * h_pad,
			height =	h - 2 * v_pad;
	Position	p_x =		h_pad,
			p_y =		v_pad,
			s_x =		h_pad,
			s_y =		v_pad;
	unsigned char	align =		G_Alignment (g);

	if (((p_w == 0) && (p_h == 0)) || ((s_w == 0) && (s_h == 0)))
		spacing = 0;

/*	Set positions
*/
	switch ((int) G_PixmapPosition (g))
	{
		case XmPIXMAP_TOP:
			if (align == XmALIGNMENT_CENTER)
			{
				if (p_w && width > p_w)
					p_x += (width - p_w)/2U;
				if (s_w && width > s_w)
					s_x += (width - s_w)/2U;
			}
			else if (align == XmALIGNMENT_END)
			{
				if (p_w && width > p_w)
					p_x += width - p_w;
				if (s_w && width > s_w)
					s_x += width - s_w;
			}
			if (p_h && ((unsigned)height > (p_h + s_h + spacing)))
				p_y += (height - p_h - s_h - spacing)/2U;
			if (p_h)
				s_y = p_y + p_h + spacing;
			else
				s_y += (height - s_h)/2U;
			break;
		case XmPIXMAP_BOTTOM:
			if (align == XmALIGNMENT_CENTER)
			{
				if (p_w && width > p_w)
					p_x += (width - p_w)/2U;
				if (s_w && width > s_w)
					s_x += (width - s_w)/2U;
			}
			else if (align == XmALIGNMENT_END)
			{
				if (p_w && width > p_w)
					p_x += width - p_w;
				if (s_w && width > s_w)
					s_x += width - s_w;
			}
			if (s_h && ((unsigned)height > (p_h + s_h + spacing)))
				s_y += (height - p_h - s_h - spacing)/2U;
			if (s_h)
				p_y = s_y + s_h + spacing;
			else
				p_y += (height - p_h)/2U;
			break;
		case XmPIXMAP_LEFT:
			if (p_h && height > p_h)
				p_y += (height - p_h)/2U;
			s_x += p_w + spacing;
			if (s_h && height > s_h)
				s_y += (height - s_h)/2U;
/*
			if (p_w && width > p_w + spacing + s_w)
				p_x += (width - p_w - spacing - s_w)/2;
			if (s_w && width > p_w + spacing + s_w)
				s_x += (width - p_w - spacing - s_w)/2;
*/
			break;
		case XmPIXMAP_RIGHT:
			if (s_h && height > s_h)
				s_y += (height - s_h)/2U;
			p_x += s_w + spacing;
			if (p_w && ((unsigned)width > (p_w + spacing + s_w)))
				p_x += (width - p_w - spacing - s_w)/2U;
			if (s_w && ((unsigned)width > (p_w + spacing + s_w)))
				s_x += (width - p_w - spacing - s_w)/2U;
			if (p_h && height > p_h)
				p_y += (height - p_h)/2U;
			break;
		case XmPIXMAP_MIDDLE:
			if (p_w && width > p_w)
		 	   p_x += (width - p_w)/2U;
			if (s_w && width > s_w)
			   s_x += (width - s_w)/2U;
			if (s_h && height > s_h)
				s_y += (height - s_h)/2U;
			if (p_h && height > p_h)
				p_y += (height - p_h)/2U;
                        break;
	}

	*pix_x = p_x;
	*pix_y = p_y;
	*str_x = s_x;
	*str_y = s_y;
}



/*-------------------------------------------------------------
**	Draw
**		Draw gadget to drawable.
*/
static void 
Draw(
        DtControlGadget g,
        Drawable drawable,
        Position x,
        Position y,
        Dimension w,
        Dimension h,
        Dimension h_t,
        Dimension s_t,
        unsigned char s_type,
        unsigned char fill_mode )
{
  XmManagerWidget	mgr =	(XmManagerWidget) XtParent (g);
  Display *		d = 	XtDisplay (g);
  GC			gc, gcTS, gcBS;
  XRectangle		clip;
  Position		p_x, p_y, s_x, s_y;
  Dimension		width = 0, height = 0;
  Pixmap		pix;
  Pixmap		mask;
  int			index;
  Dimension		left = 0, right = 0, top = 0, bottom = 0;
  Dimension		v_pad;
  Boolean		bClearArea = False;
  Boolean		bClearShadow = False;
  Boolean		bMono;
  GetPositionProc	get_positions;
  
  bMono = (((G_Foreground (g) == BlackPixelOfScreen (XtScreen (g))) ||
	    (G_Foreground (g) == WhitePixelOfScreen (XtScreen (g)))) &&
	   ((G_Background (g) == BlackPixelOfScreen (XtScreen (g))) ||
	    (G_Background (g) == WhitePixelOfScreen (XtScreen (g)))) );
  
  /*	Select gc to fill background.
   */
  if (G_ControlType (g) == XmCONTROL_SWITCH)
    gc = bMono ? M_BackgroundGC (mgr) : G_BackgroundGC (g);
  else if ((G_Armed (g)) && (fill_mode != XmFILL_PARENT))
    gc = G_ArmedBackgroundGC (g);
  else
    gc = M_BackgroundGC (mgr); 
  
  if ((fill_mode != XmFILL_NONE) && (fill_mode != XmFILL_TRANSPARENT))
    bClearArea = True;
  
  /*	Select pixmap and mask.
   */
  if (G_Pixmap (g))
    {
      /*	Terminate animation sequence.
       */
      if (G_PushImagePosition (g) > G_NumPushImages (g))
	G_PushImagePosition (g) = 0;
      if (G_DropImagePosition (g) > G_NumDropImages (g))
	G_DropImagePosition (g) = 0;

      /*	Use animation image.
       */
      if (G_PushImagePosition (g) > 0)
	{
	  index = (G_PushImagePosition (g)) - 1;    
	  if ((index == 0) || (bMono))
	    bClearArea = True;
	  else
	    bClearArea = False;
	  pix = G_PushPixmaps (g)[index];
	  mask = G_PushMasks (g)[index];
	  ++G_PushImagePosition (g);
	}
      else if (G_DropImagePosition (g) > 0)
	{
	  index = G_DropImagePosition (g) - 1;
	  if (bMono)
	    bClearArea = True;
	  else
	    bClearArea = False;
	  pix = G_DropPixmaps (g)[index];
	  mask = G_DropMasks (g)[index];
	  ++G_DropImagePosition (g);
	}

      /*	Use alternate image.
       */
      else if ( ( ((G_ControlType (g) == XmCONTROL_MONITOR) ||
		   (G_ControlType (g) == XmCONTROL_MAIL)) &&
		 (G_AltPix (g) != XmUNSPECIFIED_PIXMAP) &&
		 G_FileChanged (g) ) ||
	       ( (G_ControlType (g) == XmCONTROL_BUSY) &&
		(G_AltPix (g) != XmUNSPECIFIED_PIXMAP) &&
		G_Busy (g) && G_Set (g)) )
	{
	  pix = G_AltPix (g);
	  mask = G_AltMask (g);
	}

      /*	Use normal image.
       */
      else
	{
	  pix = G_Pixmap (g);
	  mask = G_Mask (g);
	}
    }
  
  if (bClearShadow)
    {
      /*	Erase shadow - top, left, right, bottom.
       */
      XFillRectangle (d, drawable, gc, x + h_t, y + h_t,
		      w - 2 * h_t, s_t);
      XFillRectangle (d, drawable, gc, x + h_t, y + h_t + s_t,
		      s_t, h - 2 * (h_t + s_t));
      XFillRectangle (d, drawable, gc, x + w - h_t - s_t, y + h_t + s_t,
		      s_t, h - 2 * (h_t + s_t));
      XFillRectangle (d, drawable, gc, x + h_t, y + h - h_t - s_t,
		      w - 2 * h_t, s_t);
    }
  else if (bClearArea)
    /*	Fill background.
     */
    XFillRectangle (d, drawable, gc, x + h_t, y + h_t,
		    w - 2 * h_t, h - 2 * h_t);
  
  
  /*	Get pixmap and string positions.
   */
  _DtProcessLock();
  get_positions = C_GetPositions(XtClass(g));
  _DtProcessUnlock();
  (*get_positions) ((DtIconGadget) g, w, h, h_t, s_t, &p_x, &p_y, &s_x, &s_y);
  
  /*	Select and display pixmap.
   */
  if (G_Pixmap (g))
    {

      /*	Compute display region.
       */
      if ((width == 0) && (height == 0))
	{	   
	  width = ((unsigned)(p_x + s_t + h_t) >= G_Width (g))
	    ? 0 : Min ((unsigned)G_PixmapWidth (g),
		       G_Width (g) - p_x - s_t - h_t);
	  height = ((unsigned)(p_y + s_t + h_t) >= G_Height (g))
	    ? 0 : Min ((unsigned)G_PixmapHeight (g),
		       G_Height (g) - p_y - s_t - h_t);
	}
      /*	Update clip gc.
       */
      if (mask != XmUNSPECIFIED_PIXMAP)
	{
	  gc = G_ClipGC (g);
	  XSetClipMask (XtDisplay(g), gc, mask);
	  XSetClipOrigin (XtDisplay(g), gc, x + p_x, y + p_y);
	}
      else
	gc = G_NormalGC (g);
	
      /*	Paint pixmap.
       */
      if ((width > 0 && height > 0) &&
	  (gc != NULL) && (pix != XmUNSPECIFIED_PIXMAP))
	XCopyArea (d, pix, drawable, gc, 0, 0,
		   width, height, x + p_x, y + p_y);
    }
  
  /*	Draw string.
   */
  clip.x = x + h_t + s_t;
  clip.y = y + h_t + s_t;
  clip.width = G_Width (g) - (2 * (h_t + s_t));
  clip.height = G_Height (g) - (2 * (h_t + s_t));
  
  if ((G_String (g)) && (clip.width > 0 && clip.height > 0))
    {
      if (G_ControlType (g) == XmCONTROL_DATE)
	{
	  if ( (G_StringHeight (g) > 0) && (G_AltStringHeight (g) > 0) )
            {
	      unsigned pad_factor;

	      if (G_UseLabelAdjustment (g))
		pad_factor = 3;
	      else
		pad_factor = 4;

	      v_pad = ((unsigned)G_Height(g) >
		       (G_StringHeight(g) + G_AltStringHeight(g))) ?
			 (G_Height (g) - G_StringHeight (g) - 
			  G_AltStringHeight (g)) / pad_factor : 0;
	      s_y = (2 * v_pad);
            }

	  if (bMono)
	    {
	      XmStringDrawImage (d, drawable, G_FontList (g), G_String (g),
				 G_NormalGC (g), x + s_x, y + s_y,
				 clip.width, XmALIGNMENT_BEGINNING,
				 XmSTRING_DIRECTION_L_TO_R, &clip);
	    }
	  else
	    {
	      if (G_UseEmbossedText (g))
		XmStringDraw (d, drawable, G_FontList (g), G_String (g),
			      G_BottomShadowGC (g), x + s_x + 1, y + s_y + 1,
			      clip.width, XmALIGNMENT_BEGINNING,
			      XmSTRING_DIRECTION_L_TO_R, &clip);
	      XmStringDraw (d, drawable, G_FontList (g), G_String (g),
			    G_NormalGC (g), x + s_x, y + s_y,
			    clip.width, XmALIGNMENT_BEGINNING,
			    XmSTRING_DIRECTION_L_TO_R, &clip);
	    }

	  if (G_Width (g) > G_AltStringWidth (g))
	    s_x = (G_Width (g) - G_AltStringWidth (g)) / 2U;

	  if (G_UseLabelAdjustment (g))
	    s_y = ((G_Height (g) - G_AltStringWidth (g)) + 5);
	  else
	    s_y += G_StringHeight (g);

	  if (bMono)
	    {
	      XmStringDrawImage (d, drawable, G_FontList (g),
				 G_AltString (g),
				 G_NormalGC (g), x + s_x, y + s_y,
				 clip.width, XmALIGNMENT_BEGINNING,
				 XmSTRING_DIRECTION_L_TO_R, &clip);
	    }
	  else
	    {
	      if (G_UseEmbossedText (g))
		XmStringDraw (d, drawable, G_FontList (g),
			      G_AltString (g),
			      G_BottomShadowGC (g), x + s_x + 1, y + s_y + 1,
			      clip.width, XmALIGNMENT_BEGINNING,
			      XmSTRING_DIRECTION_L_TO_R, &clip);
	      XmStringDraw (d, drawable, G_FontList (g),
			    G_AltString (g),
			    G_NormalGC (g), x + s_x, y + s_y,
			    clip.width, XmALIGNMENT_BEGINNING,
			    XmSTRING_DIRECTION_L_TO_R, &clip);
	    }
	}
      else
	{
	  if (bMono)
	    {
	      if ((s_x - 2U) >= (h_t + s_t))
		XFillRectangle (d, drawable, G_ArmedBackgroundGC (g),
				x + s_x - 2, y + s_y,
				2, G_StringHeight (g));
	      XmStringDrawImage (d, drawable, G_FontList (g), G_String (g),
				 G_NormalGC (g), x + s_x, y + s_y,
				 clip.width, XmALIGNMENT_BEGINNING,
				 XmSTRING_DIRECTION_L_TO_R, &clip);
	      if ((s_x + G_StringWidth (g) + 2U) <= (G_Width (g) - h_t - s_t))
		XFillRectangle (d, drawable, G_ArmedBackgroundGC (g),
				x + s_x + G_StringWidth (g), y + s_y,
				2, G_StringHeight (g));
	    }
	  else
	    {
	      if (G_UseEmbossedText (g))
		XmStringDraw (d, drawable, G_FontList (g), G_String (g),
			      G_BottomShadowGC (g), x + s_x + 1, y + s_y + 1,
			      clip.width, XmALIGNMENT_BEGINNING,
			      XmSTRING_DIRECTION_L_TO_R, &clip);
	      XmStringDraw (d, drawable, G_FontList (g), G_String (g),
			    G_NormalGC (g), x + s_x, y + s_y,
			    clip.width, XmALIGNMENT_BEGINNING,
			    XmSTRING_DIRECTION_L_TO_R, &clip);
	    }
	}
    }
  
  /*	Draw shadow.
   */
  switch (G_ControlType (g))
    {
    case XmCONTROL_BLANK:
    case XmCONTROL_DATE:
      break;

    case XmCONTROL_CLIENT:
      /*	Get insets.
       */
      if (bMono)
	/*	Use black and white.
	 */
	{
	  if (G_Foreground (g) == WhitePixelOfScreen (XtScreen (g)))
	    {
	      gcTS = G_BackgroundGC (g);
	      gcBS = G_NormalGC (g);
	    }
	  else
	    {
	      gcBS = G_BackgroundGC (g);
	      gcTS = G_NormalGC (g);
	    }
	}
      else
	{
	  gcTS = M_BottomShadowGC (mgr);
	  gcBS = M_TopShadowGC (mgr);
	}
      /*	Drop inner shadow if secondary since no fill.
       */
      XmeDrawShadows (d, drawable, gcTS, gcBS, 
		      x + left - s_t, y + top - s_t,
		      w - left - right + (2 * s_t),
		      h - top - bottom + (2 * s_t),
		      s_t, XmSHADOW_ETCHED_OUT);
      break;

    default:
    case XmCONTROL_BUTTON:
    case XmCONTROL_MONITOR:
    case XmCONTROL_MAIL:
      switch (G_Behavior (g))
	{
	case XmICON_BUTTON:
	  if (G_Armed (g))
	    XmeDrawShadows(d, drawable, 
			   M_TopShadowGC(mgr), M_BottomShadowGC(mgr),
			   x + h_t, y + h_t,
			   w - 2*h_t, h - 2*h_t, s_t, XmSHADOW_IN);
	  break;
	case XmICON_TOGGLE:
	  if ( (G_Armed (g) && !G_Set (g)) ||
	      (!G_Armed (g) && G_Set (g)) )
	    XmeDrawShadows(d, drawable, 
			   M_TopShadowGC(mgr), M_BottomShadowGC(mgr),
			   x + h_t, y + h_t,
			   w - 2*h_t, h - 2*h_t, s_t, XmSHADOW_IN);
	  break;
	}
      break;
    case XmCONTROL_SWITCH:
      if (!G_FillOnArm (g))
	{
	  gcTS = G_BottomShadowGC (g);
	  gcBS = G_TopShadowGC (g);
	}
      else
	{
	  if (G_Foreground (g) == WhitePixelOfScreen (XtScreen (g)))
	    {
	      gcTS = G_BackgroundGC (g);
	      gcBS = G_NormalGC (g);
	    }
	  else
	    {
	      gcTS = G_NormalGC (g);
	      gcBS = G_BackgroundGC (g);
	    }
	}
      XmeDrawShadows (d, drawable, gcTS, gcBS,
		      x + h_t, y + h_t, w - 2*h_t, h - 2*h_t, 1,
		      XmSHADOW_OUT);

      ++x;  ++y;  w -= 2;  h -= 2;  --s_t;
      if (s_t > 0)
	{
	  if ( (G_Armed (g) && !G_Set (g)) ||
	      (!G_Armed (g) && G_Set (g)) )
	    {
	      gcTS = G_BottomShadowGC (g);
	      gcBS = G_TopShadowGC (g);
	    }
	  else
	    {
	      gcTS = G_TopShadowGC (g);
	      gcBS = G_BottomShadowGC (g);
	    }
	  XmeDrawShadows (d, drawable, gcTS, gcBS,
			  x + h_t, y + h_t, w - 2*h_t, h - 2*h_t, s_t,
			  XmSHADOW_OUT);
	}
      break;
    }
}


/*-------------------------------------------------------------
**	CallCallback
**		Call callback, if any, with reason and event.
*/
static void 
CallCallback(
        DtControlGadget g,
        XtCallbackList cb,
        int reason,
        XEvent *event )
{
  DtControlCallbackStruct	cb_data;
  XtAppContext	app_context = XtWidgetToApplicationContext ((Widget) g);
  
  if ((reason == XmCR_ACTIVATE) && (G_Behavior (g) == XmICON_BUTTON))
    {
      if (G_ClickTimer (g) != 0)
	return;
      else
	G_ClickTimer (g) = 
	  XtAppAddTimeOut (app_context, G_ClickTime (g),
			   ClickTimeout, (XtPointer) g);
    }
  
  if (cb != NULL)
    {
      cb_data.reason = reason;
      cb_data.event = event;
      cb_data.control_type = G_ControlType (g);
      cb_data.set = G_Set (g);
      cb_data.push_function = G_PushFunction (g);
      cb_data.push_argument = G_PushArgument (g);
      cb_data.subpanel = G_Subpanel (g);
      cb_data.file_size = G_FileSize (g);
      XtCallCallbackList ((Widget) g, cb, &cb_data);

    }
}



/*-------------------------------------------------------------
**	UpdateGCs
**		Get normal and background graphics contexts.
**		Use standard mask to maximize caching opportunities.
*/
static void 
UpdateGCs(
        DtControlGadget g )
{
  XGCValues		values;
  XtGCMask		value_mask;
  XmManagerWidget	mw = (XmManagerWidget) XtParent(g);
  XFontStruct *		font;
  Boolean		font_rtn;
  
  if (!G__DoUpdate (g))
    return;
  
  if (G_NormalGC (g))
    XtReleaseGC ((Widget)mw, G_NormalGC (g));
  if (G_ClipGC (g))
    XtReleaseGC ((Widget)mw, G_ClipGC (g));
  if (G_BackgroundGC (g))
    XtReleaseGC ((Widget)mw, G_BackgroundGC (g));
  if (G_ArmedBackgroundGC (g))
    XtReleaseGC ((Widget)mw, G_ArmedBackgroundGC (g));
  if (G_TopShadowGC (g))
    XtReleaseGC ((Widget)mw, G_TopShadowGC (g));
  if (G_BottomShadowGC (g))
    XtReleaseGC ((Widget)mw, G_BottomShadowGC (g));
  
  /*	Get normal GC.
   */
  font_rtn = XmeRenderTableGetDefaultFont (G_FontList (g), &font);
  value_mask = GCForeground | GCBackground | GCFont | GCFillStyle;
  if (G_UseEmbossedText (g))
    values.foreground = WhitePixelOfScreen (XtScreen (g));
  else
    values.foreground = G_Foreground (g);
  values.background = G_Background (g);
  
  values.fill_style = FillSolid;
  values.font = font->fid;
  G_NormalGC (g) = XtGetGC ((Widget)mw, value_mask, &values);
  
  /*	Get top shadow GC.
   */
  if (G_ControlType (g) == XmCONTROL_SWITCH)
    values.foreground = G_PixmapBackground (g);
  else
    values.foreground = mw -> manager.top_shadow_color;
  values.background = G_Background (g);
  G_TopShadowGC (g) = XtGetGC ((Widget)mw, value_mask, &values);
  
  /*	Get bottom shadow GC.
   */
  if (G_ControlType (g) == XmCONTROL_SWITCH)
    values.foreground = G_PixmapForeground (g);
  else
    values.foreground = mw -> manager.bottom_shadow_color;
  values.background = G_Background (g);
  G_BottomShadowGC (g) = XtGetGC ((Widget)mw, value_mask, &values);
  
  /*	Get background GC.
   */
  values.foreground = G_Background (g);
  values.background = G_Foreground (g);
  G_BackgroundGC (g) = XtGetGC ((Widget)mw, value_mask, &values);
  
  /*	Get armed background GC.
   */
  values.foreground = G_ArmColor (g);
  values.background = G_Background (g);
  G_ArmedBackgroundGC (g) = XtGetGC ((Widget)mw, value_mask, &values);
  
  /*	Get Clip GC
   */
  if (G_Mask(g) != XmUNSPECIFIED_PIXMAP)
    {
      value_mask |= GCClipMask;
      values.clip_mask = G_Mask(g);
    }
  values.foreground = G_Foreground (g);
  values.background = G_Background (g);
  G_ClipGC (g) = XtGetGC ((Widget)mw, value_mask, &values);
}


/*-------------------------------------------------------------
**	Public Entry Points
**-------------------------------------------------------------
*/


/*-------------------------------------------------------------
**	Push Animation Timeout
**              An XtTimerCallbackProc.
*/
static void 
PushAnimationTimeout(
        XtPointer client_data,
        XtIntervalId *id )
{
  DtControlGadget	g = (DtControlGadget) client_data;
  XtAppContext		app_context = XtWidgetToApplicationContext ((Widget) g);
  XtExposeProc		expose;
  
  if ((G_PushImagePosition (g) > 0) &&
      (G_PushImagePosition (g) <= G_NumPushImages (g)))
    {
      G_PushAnimationTimer (g) = 
	XtAppAddTimeOut (app_context,
			 G_PushDelays (g)[G_PushImagePosition (g) - 1],
			 PushAnimationTimeout, (XtPointer) g);
    }
  if (G_PushImagePosition (g) > 1)
    {
      _DtProcessLock();
      expose = XtCoreProc((Widget)g, expose);
      _DtProcessUnlock();
      (*expose) ((Widget) g, NULL, NULL);
    }
}


/*-------------------------------------------------------------
**	_DtControlDoPushAnimation
**		Do Push animation.
**-------------------------------------------------------------
*/
void 
_DtControlDoPushAnimation(
	Widget w )
{
  DtControlGadget	 g = (DtControlGadget) w;
  
  if ((G_NumPushImages (g) > 0) && (G_PushImagePosition (g) == 0))
    {
      G_PushImagePosition (g) = 1;
      PushAnimationTimeout ((XtPointer) g, NULL);
    }
}


/*-------------------------------------------------------------
**	_DtControlAddPushAnimationImage
**		Add Push animation image.
**-------------------------------------------------------------
*/
void 
_DtControlAddPushAnimationImage(
	Widget w,
	String image,
        int delay )
{
  DtControlGadget	 g = (DtControlGadget) w;
  int			 i;
  
  /*	Allocate blocks of animation data.
   */
  if (G_NumPushImages (g) == G_MaxPushImages (g))
    {
      G_MaxPushImages (g) += NUM_LIST_ITEMS;
      G_PushDelays (g) = (int*)
	XtRealloc ((char*) G_PushDelays (g),
		   G_MaxPushImages (g) *sizeof (int));
      G_PushPixmaps (g) = (Pixmap*)
	XtRealloc ((char*) G_PushPixmaps (g),
		   G_MaxPushImages (g) *sizeof (Pixmap));
      G_PushMasks (g) = (Pixmap*)
	XtRealloc ((char*) G_PushMasks (g),
		   G_MaxPushImages (g) *sizeof (Pixmap));
    }
  
  /*	Allocate animation image.
   */
  i = G_NumPushImages (g)++;
  G_PushDelays (g)[i] = delay;
  G_PushPixmaps (g)[i] = XmGetPixmap (XtScreen (g), image,
				      G_PixmapForeground (g), 
				      G_PixmapBackground (g));
  G_PushMasks (g)[i] = _DtGetMask (XtScreen (g), image);
}


/*-------------------------------------------------------------
**	Drop Animation Timeout
**              An XtTimerCallbackProc.
*/
static void
DropAnimationTimeout(
	XtPointer client_data,
        XtIntervalId *id )
{
  DtControlGadget	g = (DtControlGadget) client_data;
  XtAppContext	app_context = XtWidgetToApplicationContext ((Widget) g);
  XtExposeProc	expose;
  
  if ((G_DropImagePosition (g) > 0) &&
      (G_DropImagePosition (g) <= G_NumDropImages (g)))
    {
      G_DropAnimationTimer (g) = 
	XtAppAddTimeOut (app_context,
			 G_DropDelays (g)[G_DropImagePosition (g) - 1],
			 DropAnimationTimeout, (XtPointer) g);
    }
  
  _DtProcessLock();
  expose = XtCoreProc((Widget)g, expose);
  _DtProcessUnlock();
  (*expose) ((Widget) g, NULL, NULL);
}


/*-------------------------------------------------------------
**	_DtControlDoDropAnimation
**		Do drop animation.
**-------------------------------------------------------------
*/
void 
_DtControlDoDropAnimation(
	Widget w )
{
  DtControlGadget	 g = (DtControlGadget) w;
  
  if ((G_NumDropImages (g) > 0) && (G_DropImagePosition (g) == 0))
    {
      G_DropImagePosition (g) = 1;
      DropAnimationTimeout ((XtPointer) g, NULL);
    }
}


/*-------------------------------------------------------------
**	_DtControlAddDropAnimationImage
**		Add drop animation image.
**-------------------------------------------------------------
*/
void 
_DtControlAddDropAnimationImage(
	Widget w,
	String image,
        int delay )
{
  DtControlGadget	 g = (DtControlGadget) w;
  int			 i;
  
  /*	Allocate blocks of animation data.
   */
  if (G_NumDropImages (g) == G_MaxDropImages (g))
    {
      G_MaxDropImages (g) += NUM_LIST_ITEMS;
      G_DropDelays (g) = (int*)
	XtRealloc ((char*) G_DropDelays (g),
		   G_MaxDropImages (g) *sizeof (int));
      G_DropPixmaps (g) = (Pixmap*)
	XtRealloc ((char*) G_DropPixmaps (g),
		   G_MaxDropImages (g) *sizeof (Pixmap));
      G_DropMasks (g) = (Pixmap*)
	XtRealloc ((char*) G_DropMasks (g),
		   G_MaxDropImages (g) *sizeof (Pixmap));
    }
  
  /*	Allocate animation image.
   */
  i = G_NumDropImages (g)++;
  G_DropDelays (g)[i] = delay;
  G_DropPixmaps (g)[i] = XmGetPixmap (XtScreen (g), image,
				      G_PixmapForeground (g), 
				      G_PixmapBackground (g));
  G_DropMasks (g)[i] = _DtGetMask (XtScreen (g), image);
}


/*-------------------------------------------------------------
**	_DtControlSetFileChanged
**		Set file changed.
**-------------------------------------------------------------
*/
void 
_DtControlSetFileChanged(
	Widget w,
	Boolean b )
{
  DtControlGadget	g = (DtControlGadget) w;
  XtExposeProc		expose;
  
  if ((G_ControlType (g) == XmCONTROL_MONITOR) ||
      (G_ControlType (g) == XmCONTROL_MAIL))
    {
      G_FileChanged (g) = b;
      _DtProcessLock();
      expose = XtCoreProc((Widget)g, expose);
      _DtProcessUnlock();
      (*expose) ((Widget) g, NULL, False);
    }
}


/*-------------------------------------------------------------
**	_DtControlSetBusy
**		Set busy.
**-------------------------------------------------------------
*/
void 
_DtControlSetBusy(
	Widget w,
	Boolean b )
{
  DtControlGadget	 g = (DtControlGadget) w;
  
  if (G_ControlType (g) != XmCONTROL_BUSY)
    return;
  
  if (b)
    {
      G_BlinkElapsed (g) = 0;
      ++ G_Busy (g);
      if (G_Busy (g) == 1)
	{
	  CallCallbackProc call_callback;

	  _DtProcessLock();
	  call_callback = C_CallCallback(XtClass(g));
	  _DtProcessUnlock();
	  (*call_callback) ((DtIconGadget) g, G_Callback (g), XmCR_BUSY_START,
			    NULL);
	  BusyTimeout ((XtPointer) w, NULL);
	}
    }
  else
    {
      if (G_Busy (g) > 0)
	--G_Busy (g);
    }
}


/*-------------------------------------------------------------
**	_DtCreateControl
**		Create a new gadget instance.
**-------------------------------------------------------------
*/
Widget 
_DtCreateControl(
        Widget parent,
        String name,
        ArgList arglist,
        int argcount )
{
	return (XtCreateWidget (name, dtControlGadgetClass, 
			parent, arglist, argcount));
}

static void
ReplaceJPDate(char *date, char *jpstr, int wday)
{
    char *s, *rp, *sp;
    char *p = NULL;
    char abday[5];
    char newdate[128];
    int i, j;
    size_t k;
    _Xstrtokparams strtok_buf;

    s = (char *)malloc((strlen(jpstr) + 1) * sizeof(char));
    strcpy(s, jpstr);
    for(p = _XStrtok(s, ",", strtok_buf), i = 0; 
	p != NULL && i < wday ;
        p = _XStrtok(NULL, ",", strtok_buf), i++)
      /* EMPTY */;
    if(p == NULL) {
        free(s);
        return;
    }
    strcpy(abday, nl_langinfo(ABDAY_1 + wday));
    if((rp = strstr(date, abday)) != NULL) {
        for(i = 0, j = 0, sp = date; date[j] != '\0'; sp++) {
            if(sp == rp) {
                for(k = 0; k < strlen(p); k++)
                    newdate[i++] = p[k];
                j += strlen(abday);
            }
            else
                newdate[i++] = date[j++];
        }
        newdate[i] = '\0';
        strcpy(date, newdate);
    }
    free(s);
    return;
}

char
_DtControlGetMonitorState(Widget w)
{
    DtControlGadget	 g = (DtControlGadget) w;

    if (G_FileChanged (g))
       return(DtMONITOR_ON);
    else
       return(DtMONITOR_OFF);
}
