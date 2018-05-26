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

/*
 *	$XConsortium: lib_funcs.c /main/3 1995/11/06 18:07:31 rswiston $
 *
 *	@(#)lib_funcs.c	1.9 11 Apr 1994	cde_app_builder/src/abmf
 *
 * 	RESTRICTED CONFIDENTIAL INFORMATION:
 *	
 *	The information in this document is subject to special
 *	restrictions in a confidential disclosure agreement between
 *	HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
 *	document outside HP, IBM, Sun, USL, SCO, or Univel without
 *	Sun's specific written approval.  This document and all copies
 *	and derivative works thereof must be returned or destroyed at
 *	Sun's request.
 *
 *	Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 *
 */


/*
 *  lib_funcs.c
 *
 *  This file contains the "library" functions that get put into
 *  a generated application.  This file is converted into strings
 *  that are included in the abmf executable.  This file is NOT
 *  linked into abmf.
 *
 *  The comment preceding each function is preserved in the final
 *  version (in the generated code).  
 */


/****************************************************************************
 * 
 * END TEST CODE
 *
 * This code exists so that this file may be compiled to test its 
 * syntactic correctness. None of this gets pulled in into dtcodegen.
 */

#undef _POSIX_SOURCE	/* tooltalk headers won't compile with this set */

#include <sys/param.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <Tt/tttk.h>
#include <X11/Intrinsic.h>
#include <Xm/XmStrDefs.h>
#include <Xm/Xm.h>
#include <Xm/Form.h>
#include <Xm/MessageB.h>
#include <Xm/PanedW.h>
#include <Xm/PushB.h>
#include <Dt/Help.h>
#include <Dt/HelpDialog.h>
#include <Dt/HelpQuickD.h>
#include <Dt/Session.h>
#include <Dt/xpm.h>		/* doesn't exist in CDE, but OK for testing */


/*
 * File: dtb_utils.h
 * CDE Application Builder General Utility Functions
 *
 * This file was generated from project dtbuilder by dtcodegen
 *
 *    ** DO NOT MODIFY BY HAND - ALL MODIFICATIONS WILL BE LOST **
 *
 */
#ifndef _DTB_UTILS_H_
#define _DTB_UTILS_H_

#include <stdlib.h>
#include <X11/Intrinsic.h>

/*
 * Function type for client session save callback
 */
typedef Boolean (*DtbClientSessionSaveCB) (
    Widget,
    char *,
    char ***,
    int *
);

/*
 * Function type for Application-defined ToolTalk Message callbacks
 */
typedef Boolean (*DtbTTMsgHandlerCB) (
    Tt_message	msg,
    void	*calldata
);

/*
 * Function type for client session restore callback
 */
typedef Boolean (*DtbClientSessionRestoreCB) (
    Widget,
    char *
);
typedef struct {
    char	*help_text;
    char	*help_volume;
    char	*help_locationID;
} DtbObjectHelpDataRec, *DtbObjectHelpData;

/*
 * Returns answer value for modal MessageBox
 */
typedef enum {
    DTB_ANSWER_NONE,
    DTB_ANSWER_ACTION1,
    DTB_ANSWER_ACTION2,
    DTB_ANSWER_ACTION3,
    DTB_ANSWER_CANCEL,
    DTB_ANSWER_HELP
} DTB_MODAL_ANSWER;

/*
 * Values for MessageBox default button
 */
typedef enum {
    DTB_ACTION1_BUTTON,
    DTB_ACTION2_BUTTON,
    DTB_ACTION3_BUTTON,
    DTB_CANCEL_BUTTON,
    DTB_NONE
} DTB_BUTTON;

/*
 * Types/ways of centering an object
 */
typedef enum {
    DTB_CENTER_NONE,
    DTB_CENTER_POSITION_VERT,
    DTB_CENTER_POSITION_HORIZ,
    DTB_CENTER_POSITION_BOTH
} DTB_CENTERING_TYPES;

/*
 * Structure to store values for Messages
 */
typedef struct {
    unsigned char       type;
    String              title;
    String              message;
    String              action1_label;
    XtCallbackProc      action1_callback;
    String              action2_label;
    XtCallbackProc      action2_callback;
    String              action3_label;
    XtCallbackProc      action3_callback;
    Boolean             cancel_button;
    XtCallbackProc      cancel_callback;
    Boolean             help_button;
    DTB_BUTTON          default_button;
} DtbMessageDataRec, *DtbMessageData;

/*
 * Application Builder utility functions
 */
int dtb_cvt_file_to_pixmap(
    String	fileName,
    Widget	widget,
    Pixmap	*pixmapReturnPtr
);
int dtb_set_label_pixmaps(
    Widget	widget,
    Pixmap	labelPixmap,
    Pixmap	labelInsensitivePixmap
);
int dtb_set_label_from_bitmap_data(
    Widget		widget,
    int			width,
    int			height,
    unsigned char	*bitmapData
);
Boolean dtb_file_has_extension(
    String	fileName,
    String	extension
);
int dtb_cvt_filebase_to_pixmap(
    Widget      widget,
    String      fileBase,
    String      extension,
    Pixmap      *pixmap_ptr
);

int dtb_cvt_image_file_to_pixmap(
    Widget	widget,
    String	fileName,
    Pixmap	*pixmap
);
int dtb_set_label_from_image_file(
    Widget	widget,
    String	fileName
);
unsigned long dtb_cvt_resource_from_string(
    Widget		parent,
    String		res_type,
    unsigned int	size_of_type,
    String		res_str_value,
    unsigned long	error_value
);
Pixmap dtb_create_greyed_pixmap(
    Widget	widget,
    Pixmap	pixmap
);
void dtb_save_toplevel_widget(
    Widget 	toplevel
);

Widget dtb_get_toplevel_widget();

void dtb_remove_sash_focus(
    Widget	panedwindow
);

void dtb_save_command(
    char *command
);

char *  dtb_get_command();

void dtb_help_dispatch(
    Widget 	widget,
    XtPointer	clientData,
    XtPointer 	callData
);
void dtb_help_back_hdlr(
    Widget 	widget,
    XtPointer	clientData,
    XtPointer	callData
);
void dtb_more_help_dispatch(
    Widget	widget,
    XtPointer	clientData,
    XtPointer	callData
);
void dtb_do_onitem_help();
void dtb_show_help_volume_info(
    char	*volume_name,
    char	*location_id
);
Widget dtb_create_message_dlg(
    Widget		parent,
    DtbMessageData	mbr,
    String		override_msg
);
static void destroyCB(
    Widget      widget,
    XtPointer   client_data,
    XtPointer   call_data
);
Widget dtb_MessageBoxGetActionButton(
    Widget	msg_dlg,
    DTB_BUTTON	which_btn
);
void dtb_show_message(
    Widget		parent,
    DtbMessageData	mbr,
    String		override_msg,
    DtbObjectHelpData	override_help
);
DTB_MODAL_ANSWER dtb_show_modal_message(
    Widget		parent,
    DtbMessageData	mbr,
    String		override_msg,
    DtbObjectHelpData	override_help,
    Widget         	*modal_dlg_pane_out_ptr
);
static void modal_dlgCB(
    Widget      widget,
    XtPointer   client_data,
    XtPointer   call_data
);
void dtb_children_center(
    Widget		form,
    DTB_CENTERING_TYPES	type
);
void dtb_children_uncenter(
    Widget		form,
    DTB_CENTERING_TYPES	type
);
void dtb_center(
    Widget		form_child,
    DTB_CENTERING_TYPES	type
);
void dtb_uncenter(
    Widget		form_child,
    DTB_CENTERING_TYPES	type
);
void dtb_session_save(
    Widget	widget,
    XtPointer	clientData,
    XtPointer	callData
);
DtbClientSessionSaveCB dtb_get_client_session_saveCB();

void dtb_set_client_session_saveCB(
    DtbClientSessionSaveCB session_saveCB
);
void dtb_session_restore(
    Widget	widget,
    char *session_file
);
void dtb_set_client_session_restoreCB(
    DtbClientSessionRestoreCB session_restoreCB
);

DtbClientSessionRestoreCB dtb_get_client_session_restoreCB();

void dtb_set_tt_msg_quitCB(
    DtbTTMsgHandlerCB msg_quitCB
);
void dtb_set_tt_msg_do_commandCB(
    DtbTTMsgHandlerCB msg_do_commandCB
);
void dtb_set_tt_msg_get_statusCB(
    DtbTTMsgHandlerCB msg_get_statusCB
);
void dtb_set_tt_msg_pause_resumeCB(
    DtbTTMsgHandlerCB msg_pause_resumeCB
);
Tt_message dtb_tt_contractCB(
    Tt_message		msg,
    void 		*client_data,
    Tt_message 		contract
);

void dtb_tt_close();


#define dtb_cvt_string_to_pixel(parent, str) \
        ((Pixel)dtb_cvt_resource_from_string( \
                   (parent), XtRPixel, sizeof(Pixel), (str), 0))

#endif /* _DTB_UTILS_H_ */

#ifndef min
#define min(a,b) ((a) < (b)? (a):(b))
#endif

/*
 * Static functions used for dynamic centering of objects
 */
static void  center_widget(
    Widget		form_child,
    DTB_CENTERING_TYPES	type
);
static void  uncenter_widget(
    Widget		form_child,
    DTB_CENTERING_TYPES	type
);
static void centering_handler(
    Widget	widget,
    XtPointer	client_data,
    XEvent	*event,
    Boolean	*cont_dispatch
);

/*
 * Private functions for determining the directory the executable was
 * loaded from.
 */
static char	*dtb_get_exe_dir(void);

static int	determine_exe_dir(
			char *argv0, 
			char *buf, 
			int bufSize
		);
static int	determine_exe_dir_from_argv(
			char *argv0, 
			char *buf, 
			int bufSize
		);
static int	determine_exe_dir_from_path(
			char *argv0, 
			char *buf, 
			int bufSize
		);
static Boolean	path_is_executable(
			char *path, 
			uid_t euid, 
			gid_t egid
		);

static void	get_widest_label(
		    WidgetList	list,
		    int		count,
		    Widget	*child_widget,
		    Dimension	*label_width
		);

static Position get_offset_from_ancestor(
		    Widget	ancestor,
		    Widget	w
		);

static void	get_widest_value(
		    WidgetList	list,
		    int		count,
		    Widget	*child_widget,
		    Dimension	*value_width
		);

static void	get_widget_rect(
		    Widget widget,
		    XRectangle *rect
		);

static void	get_greatest_size(
		    Widget	*list,
		    int		count,
		    int		*width,
		    int		*height,
		    Widget	*tallest,
		    Widget	*widest
		);

static void	get_group_cell_size(
		    Widget		parent,
		    DtbGroupInfo	*group_info,
		    int			*cell_width,
		    int			*cell_height
		);

static void	get_group_row_col(
		    Widget		parent,
		    DtbGroupInfo	*group_info,
		    int			*rows,
		    int			*cols
		);

/*
 * Variable for storing client session save callback
 */
static DtbClientSessionSaveCB		dtb_client_session_saveCB = NULL;
/*
 * Variable for storing client session restore callback
 */
static DtbClientSessionRestoreCB	dtb_client_session_restoreCB = NULL;

/*
 * Variable for storing top level widget
 */
static Widget		dtb_project_toplevel_widget = (Widget)NULL;

/*
 * Variable for storing command used to invoke application
 */
static char		*dtb_save_command_str = (char *)NULL;

/*
 * Directory binary for this process was loaded from.
 */
static char		*dtb_exe_dir = (char *)NULL;


/*
 *  END TEST CODE
 * 
 ****************************************************************************/

/****************************************************************************
 ****************************************************************************
 *****									*****
 *****		LIBRARY FUNCTIONS BEGIN 				*****
 *****									*****
 ****************************************************************************
 ****************************************************************************/

/*
 * Create/load a Pixmap given an XPM or Bitmap files
 * NOTE: this allocates a server Pixmap;  it is the responsibility
 * of the caller to free the Pixmap
 */
int
dtb_cvt_file_to_pixmap(
    String	fileName,
    Widget	widget,
    Pixmap	*pixmapReturnPtr
)
{
#define pixmapReturn (*pixmapReturnPtr)
    Pixmap	pixmap = NULL;
    Screen	*screen = NULL;
    Pixel	fgPixel = 0;
    Pixel	bgPixel = 0;
    char	image_path[MAXPATHLEN+1];
    Boolean	pixmap_found = False;

    /*
     * Get default values
     */
    screen = XtScreenOfObject(widget);
    fgPixel = WhitePixelOfScreen(screen);
    bgPixel = BlackPixelOfScreen(screen);

    /*
     * Get proper colors for widget
     */
    XtVaGetValues(widget,
	XmNforeground, &fgPixel,
	XmNbackground, &bgPixel,
	NULL);

    /*
     * In CDE, XmGetPixmap handles .xpm files, as well.
     */
    if (!pixmap_found)
    {
        pixmap = XmGetPixmap(screen, fileName, fgPixel, bgPixel);
    }
    pixmap_found = ((pixmap != NULL) && (pixmap != XmUNSPECIFIED_PIXMAP));

    if (!pixmap_found)
    {
        sprintf(image_path, "%s/%s", dtb_get_exe_dir(), fileName);
        pixmap = XmGetPixmap(screen, image_path, fgPixel, bgPixel);
    }
    pixmap_found = ((pixmap != NULL) && (pixmap != XmUNSPECIFIED_PIXMAP));

    if (!pixmap_found)
    {
        sprintf(image_path, "%s/bitmaps/%s", dtb_get_exe_dir(), fileName);
        pixmap = XmGetPixmap(screen, image_path, fgPixel, bgPixel);
    }
    pixmap_found = ((pixmap != NULL) && (pixmap != XmUNSPECIFIED_PIXMAP));

    if (!pixmap_found)
    {
	return -1;
    }

    pixmapReturn = pixmap;
    pixmapReturn = pixmap;
    return 0;
#undef pixmapReturn
}


/*
 * For a given pixmap, create a 50% greyed version.  Most likely this will
 * be used where the source pixmap is the labelPixmap for a widget and an 
 * insensitivePixmap is needed so the widget will look right when it is 
 * "not sensitive" ("greyed out" or "inactive").
 * 
 * NOTE: This routine creates a Pixmap, which is an X server resource.  The
 *       created pixmap must be freed by the caller when it is no longer
 *	 needed.
 */
Pixmap
dtb_create_greyed_pixmap(
    Widget	widget,
    Pixmap	pixmap
)
{
    Display	 *dpy;
    Window	 root;
    Pixmap       insensitive_pixmap;
    Pixel	 background;
    unsigned int width, height, depth, bw;
    int		 x,y;
    XGCValues    gcv;
    XtGCMask     gcm;
    GC           gc;


    dpy = XtDisplayOfObject(widget);

    if(pixmap == XmUNSPECIFIED_PIXMAP || pixmap == (Pixmap)NULL) {
	return((Pixmap)NULL);
    }

    XtVaGetValues(widget,
	XmNbackground, &background,
	NULL);

    /* Get width/height of source pixmap */
    if (!XGetGeometry(dpy,pixmap,&root,&x,&y,&width,&height,&bw,&depth)) {
	    return((Pixmap)NULL);
    }
    gcv.foreground = background;
    gcv.fill_style = FillStippled;
    gcv.stipple = XmGetPixmapByDepth(XtScreenOfObject(widget),
			"50_foreground", 1, 0, 1);
    gcm = GCForeground | GCFillStyle | GCStipple;
    gc = XtGetGC(widget, gcm, &gcv);

    /* Create insensitive pixmap */
    insensitive_pixmap = XCreatePixmap(dpy, pixmap, width, height, depth);
    XCopyArea(dpy, pixmap, insensitive_pixmap, gc, 0, 0, width, height, 0, 0);
    XFillRectangle(dpy, insensitive_pixmap, gc, 0, 0, width, height);

    XtReleaseGC(widget, gc);
    return(insensitive_pixmap);
}

/*
 * Sets the label and insensitive label pixmaps of the widget.  
 *
 * If either (or both) pixmap is NULL, it is ignored.
 */
int
dtb_set_label_pixmaps(
    Widget	widget,
    Pixmap	labelPixmap,
    Pixmap	labelInsensitivePixmap
)
{
    if (   (widget == NULL)
	|| ((labelPixmap == NULL) && (labelInsensitivePixmap == NULL)) )
    {
	return -1;
    }

    /*
     * Set the appropriate resources.
     */
    XtVaSetValues(widget, XmNlabelType,	XmPIXMAP, NULL);
    if (labelPixmap != NULL)
    {
	XtVaSetValues(widget, XmNlabelPixmap, labelPixmap, NULL);
    }
    if (labelInsensitivePixmap != NULL)
    {
	XtVaSetValues(widget, XmNlabelInsensitivePixmap, 
				labelInsensitivePixmap, NULL);
    }

    return 0;
}

/*
 * Returns True if the fileName has the extension
 */
Boolean
dtb_file_has_extension(
    String	fileName, 
    String	extension
)
{
    Boolean        hasExt = False;

    if (extension == NULL)
    {
        hasExt = ( (fileName == NULL) || (strlen(fileName) == 0) );
    }   
    else
    {   
        if (fileName == NULL)
            hasExt = False;
        else
        {
            char *dotPtr= strrchr(fileName, '.');
            if (dotPtr == NULL)
                hasExt= False;
            else if (strcmp(dotPtr+1, extension) == 0)
	 	hasExt = True;
        }
    }
    return hasExt;
} 

/*
 * Appends the extension to fileBase and attempts to load in
 * the Pixmap 
 */
int
dtb_cvt_filebase_to_pixmap(
    Widget	widget,
    String	fileBase,
    String	extension,
    Pixmap	*pixmap_ptr
)
{
    char        fileName[512];  
    int		rc = 0;

    strcpy(fileName, fileBase);
    strcat(fileName, extension);
    rc = dtb_cvt_file_to_pixmap(fileName, widget, pixmap_ptr);
    return rc;
}


int
dtb_cvt_image_file_to_pixmap(
		Widget	widget,
		String	fileName,
		Pixmap	*pixmap
)
{
    int		rc = 0;		/* return code */
    Pixmap	tmpPixmap = NULL;
    int		depth;

    if (dtb_file_has_extension(fileName, "pm") ||
 	dtb_file_has_extension(fileName, "xpm") ||	
 	dtb_file_has_extension(fileName, "bm") ||	
 	dtb_file_has_extension(fileName, "xbm"))
    {
    	/* If explicit filename requested, use it directly */
	rc = dtb_cvt_file_to_pixmap(fileName, widget, &tmpPixmap);
    }
    else /* Append extensions to locate best graphic match */
    {
    	XtVaGetValues(XtIsSubclass(widget, xmGadgetClass)? XtParent(widget) : widget, 
		XmNdepth, &depth, NULL);

    	if (depth > 1) /* Look for Color Graphics First */
    	{
    	    rc = dtb_cvt_filebase_to_pixmap(widget, fileName, ".pm", &tmpPixmap);
    	    if (rc < 0)
	    	rc = dtb_cvt_filebase_to_pixmap(widget, fileName, ".xpm", &tmpPixmap);
	    if (rc < 0)
            	rc = dtb_cvt_filebase_to_pixmap(widget, fileName, ".bm", &tmpPixmap);
            if (rc < 0) 
            	rc = dtb_cvt_filebase_to_pixmap(widget, fileName, ".xbm", &tmpPixmap); 
    	}
    	else /* Look for Monochrome First */
    	{
            rc = dtb_cvt_filebase_to_pixmap(widget, fileName, ".bm", &tmpPixmap);
            if (rc < 0)
            	rc = dtb_cvt_filebase_to_pixmap(widget, fileName, ".xbm", &tmpPixmap);
            if (rc < 0) 
            	rc = dtb_cvt_filebase_to_pixmap(widget, fileName, ".pm", &tmpPixmap); 
            if (rc < 0) 
            	rc = dtb_cvt_filebase_to_pixmap(widget, fileName, ".xpm", &tmpPixmap);  
    	}
    }

    if (rc < 0)
    {
	*pixmap = NULL;
	return rc;
    }

    *pixmap = tmpPixmap;
    return 0;
}


/* 
 * Sets the XmNlabel from the image file (either xbitmap or xpixmap format).
 *
 * returns negative on error.
 */
int
dtb_set_label_from_image_file(
		Widget	widget,
		String	fileName
)
{
    int		rc = 0;		/* return code */
    Pixmap	labelPixmap = NULL;
    Pixmap	insensitivePixmap = NULL;
    int		depth;

    rc = dtb_cvt_image_file_to_pixmap(widget, fileName, &labelPixmap);
    if (rc < 0)
    {
	return rc;
    }

    insensitivePixmap = dtb_create_greyed_pixmap(widget,labelPixmap);
    rc = dtb_set_label_pixmaps(widget, labelPixmap, insensitivePixmap);
    if (rc < 0)
    {
	return rc;
    }

    return 0;
}


/*
 * Sets both the sensitive and insensitive pixmaps
 */
int
dtb_set_label_from_bitmap_data(
    Widget		widget,
    int			width,
    int			height,
    unsigned char	*bitmapData
)
{
    Display		*display = NULL;
    Screen		*screen = NULL;
    Drawable		window = NULL;
    long		bgPixel = 0;
    long		fgPixel = 0;
    unsigned int	depth = 0;
    Pixmap		labelPixmap = NULL;

    if (   (widget == NULL)
	|| (width < 1)
	|| (height < 1)
	|| (bitmapData == NULL) )
    {
        return -1;
    }

    /*
     * Get a whole slew of information X needs
     */
    {
	Pixel		widgetBg = 0;
	Pixel		widgetFg = 0;
	int		widgetDepth = 0;

        display = XtDisplay(widget);
        screen = XtScreen(widget);
        window = XtWindow(widget);
	if (window == NULL)
	{
	    /* Widget has not been realized, yet */
	    window = RootWindowOfScreen(screen);
	}

	XtVaGetValues(XtIsSubclass(widget, xmGadgetClass)? XtParent(widget) : widget,
		XmNbackground,	&widgetBg,
		XmNforeground,	&widgetFg,
		XmNdepth,	&widgetDepth,
		NULL);
	bgPixel = widgetBg;
	fgPixel = widgetFg;
	depth = widgetDepth;
    }

    /*
     * Create the pixmap
     */
    labelPixmap = XCreatePixmapFromBitmapData(
		display,
		window,
		(char *) bitmapData,
		width, height,
		fgPixel, bgPixel,
		depth);
    if (labelPixmap == NULL)
    {
	return -1;
    }

    dtb_set_label_pixmaps(widget, labelPixmap, NULL);

    return 0;
}


/*
 * Sets both the sensitive and insensitive pixmaps
 */
int
dtb_set_label_from_xpm_data(
    Widget	widget,
    char	*xpmData[]
)
{
    int		status = 0;
    Display	*display = NULL;
    Screen	*screen = NULL;
    Drawable	window = NULL;
    Pixmap	labelPixmap = NULL;

    if (   (widget == NULL)
	|| (xpmData == NULL) )
    {
	return -1;
    }

    display = XtDisplay(widget);
    screen = XtScreen(widget);
    window = XtWindow(widget);
    if (window == NULL)
    {
        /* Widget has not been realized, yet */
        window = RootWindowOfScreen(screen);
    }

    status = _DtXpmCreatePixmapFromData(
            display,
            window,
            xpmData,
            &labelPixmap,
            (Pixmap *)NULL,
            (XpmAttributes *)NULL);
    if (status != XpmSuccess)
        return -1;

    dtb_set_label_pixmaps(widget, labelPixmap, NULL);

    return 0;
}


unsigned long
dtb_cvt_resource_from_string(
    Widget		parent,
    String		res_type,
    unsigned int	size_of_type,
    String		res_str_value,
    unsigned long	error_value
)
{
    unsigned long	cvt_value_return = error_value;
    unsigned char	cvt_value1 = 0;
    unsigned short	cvt_value2 = 0;
    unsigned int	cvt_value3 = 0;
    unsigned long	cvt_value4 = 0;
    XtPointer		cvt_value_ptr = NULL;
    int			which_cvt_value = -1;
    XrmValue		source;
    XrmValue		dest;

    if (size_of_type > sizeof(cvt_value_return))
    {
	/* Type we are converting to is too large */
	return cvt_value_return;
    }

    /*
     * Get a data object of the appropriate size
     */
    if (size_of_type == sizeof(cvt_value1))
    {
	which_cvt_value = 1;
	cvt_value_ptr = (XtPointer)&cvt_value1;
    }
    else if (size_of_type == sizeof(cvt_value2))
    {
	which_cvt_value = 2;
	cvt_value_ptr = (XtPointer)&cvt_value2;
    }
    else if (size_of_type == sizeof(cvt_value3))
    {
	which_cvt_value = 3;
	cvt_value_ptr = (XtPointer)&cvt_value3;
    }
    else if (size_of_type == sizeof(cvt_value4))
    {
	which_cvt_value = 4;
	cvt_value_ptr = (XtPointer)&cvt_value4;
    }
    else
    {
	return cvt_value_return;
    }

    /*
     * Actually do the conversion
     */
    source.size = strlen(res_str_value) + 1;
    source.addr = res_str_value;

    dest.size   = size_of_type;
    dest.addr   = (char *)cvt_value_ptr;

    if (XtConvertAndStore(parent, XtRString, &source,
		res_type, &dest) != 0)
    {
	switch (which_cvt_value)
	{
	    case 1:
		cvt_value_return = (unsigned long)cvt_value1;
	    break;

	    case 2:
		cvt_value_return = (unsigned long)cvt_value2;
	    break;

	    case 3:
		cvt_value_return = (unsigned long)cvt_value3;
	    break;

	    case 4:
		cvt_value_return = (unsigned long)cvt_value4;
	    break;
	}
    }

    return cvt_value_return;
}

/*
** Routines to save and access the toplevel widget for an application.
** This is useful in dtb_ convenience functions, and also probably by
** developers in routines they provide in their _stubs.c files.
** static Widget dtb_project_toplevel_widget = (Widget) NULL;
*/
void
dtb_save_toplevel_widget(
    Widget	toplevel
)
{
	dtb_project_toplevel_widget = toplevel;
}

Widget
dtb_get_toplevel_widget()
{
	return(dtb_project_toplevel_widget);
}

/*
** Function to turn off traversal on the invisible sash within a
** PanedWindow.  This is primarily used for the PanedWindow within
** a Custom Dialog object.
*/
void
dtb_remove_sash_focus(
    Widget	widget
)
{
    WidgetList	children;
    int		numChildren, i;

    if (widget == NULL || !XtIsSubclass(widget, xmPanedWindowWidgetClass))
	return;

    XtVaGetValues(widget,
	XmNchildren,	&children,
	XmNnumChildren, &numChildren,
	NULL);

    for(i = 0; i < numChildren; i++)
	if (XtIsSubclass(children[i], xmSashWidgetClass))
	    XtVaSetValues(children[i], XmNtraversalOn, False, NULL);
}


/*
 ** Routines to save and access the command used to invoke the application. 
 */
void
dtb_save_command(
    char	*argv0
)
{
    char	exe_dir[MAXPATHLEN+1];
    dtb_save_command_str = argv0; 

    /*
     * Save the path to the executable
     */
    if (determine_exe_dir(argv0, exe_dir, MAXPATHLEN+1) >= 0)
    {
	dtb_exe_dir = (char *)malloc(strlen(exe_dir)+1);
	if (dtb_exe_dir != NULL)
	{
	    strcpy(dtb_exe_dir, exe_dir);
	}
    }
}


char * 
dtb_get_command() 
{
    return(dtb_save_command_str); 
}

/* 
** Generic callback function to be attached as XmNhelpCallback and
** provide support for on-object and Help-key help.  The help text to
** be displayed is provided via a specialized data structure passed in
** as client data.
*/

void
dtb_help_dispatch(
    Widget 	widget,
    XtPointer 	clientData,
    XtPointer 	callData
)
{
    DtbObjectHelpData 	help_data = (DtbObjectHelpData)clientData;
    int             	i;
    Arg             	wargs[10];
    char		buffer[100];
    Widget		back_button;
    static Widget	Quick_help_dialog = (Widget)NULL;
    static Widget	MoreButton;

    /* 
    ** In order to save the more-help info (help volume & location ID) as part
    ** of the quick help dialog's backtrack mechanism, we have to splice the 
    ** volume & ID strings together and save them as the help volume field.
    ** If there isn't supplemental help information, we save a null string.
    **
    ** Checking the status of the more-help info also lets us decide whether
    ** the "More..." button should be enabled on the dialog.
    */
    if( help_data->help_volume     ==0 || *(help_data->help_volume) == 0 ||
	help_data->help_locationID ==0 || *(help_data->help_locationID)== 0){
		buffer[0] = '\0';
    }
    else {
	sprintf(buffer,"%s/%s",help_data->help_volume,help_data->help_locationID);
    }

    /* 
    ** If this is our first time to post help, create the proper dialog and
    ** set its attributes to suit the current object.  If not, then just
    ** update the attributes.
    **
    ** (You have to be careful about gratuitous SetValues on the dialog because
    ** its internal stack mechanism takes repeated settings as separate items
    ** and updates the stack for each.)
    */
    if(Quick_help_dialog == (Widget)NULL) {
        /* Create shared help dialog */
        i = 0;
	XtSetArg(wargs[i],XmNtitle, "Application Help");            i++;
	XtSetArg(wargs[i],DtNhelpType, DtHELP_TYPE_DYNAMIC_STRING); i++;
	XtSetArg(wargs[i],DtNstringData,help_data->help_text);      i++;
        XtSetArg(wargs[i],DtNhelpVolume,buffer);		    i++;
	Quick_help_dialog = DtCreateHelpQuickDialog(dtb_get_toplevel_widget(),
		"Help",wargs,i);

	/* 
	** Fetch out the Dialog's More button child and hook the 'more help'
	** handler to its activateCallback.  Set it's current status to
	** indicate whether this object has supplemental help data. 
	*/
	MoreButton = DtHelpQuickDialogGetChild(Quick_help_dialog,
		DtHELP_QUICK_MORE_BUTTON);
	XtManageChild(MoreButton);
	XtAddCallback(MoreButton,XmNactivateCallback,dtb_more_help_dispatch,
		(XtPointer)Quick_help_dialog);
	if(buffer[0] == '\0') XtSetSensitive(MoreButton,False);

	/* 
	** Fetch out the Dialog's Backtrack button child & hook a callback
	** that will control button sensitivity based on the presence of more
	** help data.
	*/
	back_button = DtHelpQuickDialogGetChild(Quick_help_dialog,
		DtHELP_QUICK_BACK_BUTTON);
	XtAddCallback(back_button,XmNactivateCallback,dtb_help_back_hdlr,
		(XtPointer)Quick_help_dialog);
    }
    /* Otherwise the dialog already exists so we just set the attributes. */
    else {
	/* 
	** If we have supplemental help info, enable the more button.
	** Also save this info for later use in the backtrack handler.
	*/
	if(buffer[0] == '\0') {
	    XtSetSensitive(MoreButton,False);
	}
	else {
	    XtSetSensitive(MoreButton,True);
	}

        XtVaSetValues(Quick_help_dialog,
    	    DtNhelpType, DtHELP_TYPE_DYNAMIC_STRING,
            DtNhelpVolume,buffer,
            DtNstringData,help_data->help_text,
	    NULL);
    }

    /* Now display the help dialog */
    XtManageChild(Quick_help_dialog);
}
/*
** Callback that is added to the QuickHelpDialog widget's "Backtrack" button
** and is used to control the "More.." button.  At each step in the backtrack,
** this routine checks to see if there is help volume & location info stored
** in the dialog's helpVolume resource.  If so, then the "More..." button is
** enabled.  If not, then it is disabled.
*/
void
dtb_help_back_hdlr(
    Widget 	widget,
    XtPointer 	clientData,
    XtPointer 	callData
)
{
    String		buffer, text, vol, loc;
    char		*cp;
    Widget		more_button;
    Widget		help_dialog = (Widget)clientData;

    /* Fetch the saved volume/locationID information from the dialog widget */
    XtVaGetValues(help_dialog,
	DtNhelpVolume,&buffer,
	DtNstringData,&text,
	NULL);

    /* Get a handle to the "More..." button */
    more_button = DtHelpQuickDialogGetChild(help_dialog,
		DtHELP_QUICK_MORE_BUTTON);
    /* 
    ** Parse the combined volume/locationID string.  Disable the "More..."
    ** button if there isn't any help info, and enable it if there is.
    */
    if( buffer == 0 || (*buffer == NULL) ||
	(cp=strrchr(buffer,'/')) == (char *)NULL) {
		XtSetSensitive(more_button,False);
    }
    else {
		XtSetSensitive(more_button,True);
    }
}
/*
** This callback is invoked when the user presses "More..." on the
** QuickHelpDialog.  It figures out whether a help volume entry is associated
** with the displayed help text, and if so it brings up a GeneralHelpDialog
** to display the appropriate help volume information.
*/
void
dtb_more_help_dispatch(
    Widget 	widget,
    XtPointer 	clientData,
    XtPointer 	callData
)
{
    int             	i;
    Arg             	wargs[10];
    String		buffer, vol, loc;
    char		*cp;
    static Widget	GeneralHelpDialog = (Widget) NULL;
    Widget		help_dialog = (Widget)clientData;
    Widget		more_button;

    /* Fetch the saved volume/locationID information from the dialog widget */
    XtVaGetValues(help_dialog,
	DtNhelpVolume,&buffer,
	NULL);

    /* 
    ** Parse the combined volume/locationID string.  If that fails there
    ** must be no data, so don't bother displaying the GeneralHelpDialog.
    ** (We shouldn't be in this callback routine if that happens, though...)
    */
    if( (cp=strrchr(buffer,'/')) != (char *)NULL) {
	*cp++ = 0;
	vol = buffer;
	loc = cp; 
    }

    if(GeneralHelpDialog == (Widget)NULL) {
	/* Create General Help Dialog */
        i = 0;
	XtSetArg(wargs[i],XmNtitle, "Application Help");        i++;
	XtSetArg(wargs[i],DtNhelpType, DtHELP_TYPE_TOPIC);      i++;
	XtSetArg(wargs[i],DtNhelpVolume, vol);			i++;
	XtSetArg(wargs[i],DtNlocationId,loc);			i++;

	GeneralHelpDialog = DtCreateHelpDialog(dtb_get_toplevel_widget(),
		"GeneralHelp",wargs,i);
    }
    else {
        i = 0;
        XtSetArg(wargs[i],DtNhelpType, DtHELP_TYPE_TOPIC);  	i++;
        XtSetArg(wargs[i],DtNhelpVolume,vol);			i++;
        XtSetArg(wargs[i],DtNlocationId,loc);			i++;
        XtSetValues(GeneralHelpDialog,wargs,i);
    }

    /* Now take down the quick help dialog and display the full help one */
    XtManageChild(GeneralHelpDialog);
    XtUnmanageChild(help_dialog);
}
/*
** Utility function used to provide support for on-item help.
** It is typically invoked via a callback on the "On Item" item in the
** main menubar's "Help" menu.
*/
void
dtb_do_onitem_help()
{
    Widget	target;

    /* Call the DtHelp routine that supports interactive on-item help. */
    if(DtHelpReturnSelectedWidgetId(dtb_get_toplevel_widget(),
	(Cursor)NULL,&target) != DtHELP_SELECT_VALID) return;
	
    /*
    ** Starting at the target widget, wander up the widget tree looking
    ** for one that has an XmNhelpCallback, and call the first one we
    ** find.
    */
    while(target != (Widget)NULL) {
	if( XtHasCallbacks(target,XmNhelpCallback) == XtCallbackHasSome) {
	    XtCallCallbacks(target,XmNhelpCallback,(XtPointer)NULL);
	    return;
	}
	else {
	    target = XtParent(target);
	}
    }
    return;
}
/*
** Utility function called to display help volume information.
** It needs the name of the help volume and the location ID (both as
** strings) so it can configure the full help dialog widget properly.
*/
int
dtb_show_help_volume_info(
    char	*volume_name,
    char	*location_id
)
{
    int             	i;
    Arg             	wargs[10];
    static Widget	GeneralHelpDialog = (Widget) NULL;
    
    if(GeneralHelpDialog == (Widget)NULL) {
	/* Create General Help Dialog */
        i = 0;
	XtSetArg(wargs[i],XmNtitle, "Application Help");        i++;
	XtSetArg(wargs[i],DtNhelpType, DtHELP_TYPE_TOPIC);      i++;
	XtSetArg(wargs[i],DtNhelpVolume, volume_name);		i++;
	XtSetArg(wargs[i],DtNlocationId,location_id);		i++;

	GeneralHelpDialog = DtCreateHelpDialog(dtb_get_toplevel_widget(),
		"GeneralHelp",wargs,i);
    }
    else {
        i = 0;
        XtSetArg(wargs[i],DtNhelpType, DtHELP_TYPE_TOPIC);  	i++;
        XtSetArg(wargs[i],DtNhelpVolume,volume_name);		i++;
        XtSetArg(wargs[i],DtNlocationId,location_id);		i++;
        XtSetValues(GeneralHelpDialog,wargs,i);
    }

    /* Now display the full help dialog */
    XtManageChild(GeneralHelpDialog);

    return(0);
}
/*
 * dtb_session_save()
 * Callback that is called when the application (top level
 * widget of application) gets a WM_SAVE_YOURSELF ClientMessage
 * This callback will call the client/application's session
 * save callback.
 */
void
dtb_session_save(
    Widget 	widget,
    XtPointer 	clientData,
    XtPointer 	callData
)
{
    int				new_argc,
    				client_argc = 0,
    				new_argc_counter,
    				i;
    char			**new_argv,
    				**client_argv = NULL,
    				*session_file_path,
    				*session_file_name,
    				*app_name = NULL;
    Boolean			status = False;
    DtbClientSessionSaveCB	session_saveCB;

    /*
     * Return if no widget passed in.
     */
    if (!widget)
        return;

    /*
     * Get session file path/name to store application's state
     */
    if (DtSessionSavePath(widget, &session_file_path, &session_file_name) == False)
        return;

    /*
     * Get client session save callback
     */
    session_saveCB = dtb_get_client_session_saveCB();

    /*
     * Call client session save callback
     */
    if (session_saveCB)
        /*
         * client_argv and client_argc are the variables that
         * will contain any extra command line options
         * that need to be used when invoking the application
         * to bring it to the current state.
         */
        status = session_saveCB(widget, session_file_path,
        			&client_argv, &client_argc);

    /*
     * Generate the reinvoking command and add it as the property value
     */

    /*  
     * Fetch command used to invoke application
     */
    app_name = dtb_get_command();

    /*
     * new_argc and new_argc are the variables used to reconstruct
     * the command to re-invoke the application
     */

    /*
     * Start new_argc with:
     *	1	for argv[0], normally the application
     *	client_argc	any extra command line options as
     *		returned from client session save
     *		callback
     */
    new_argc = 1 + client_argc;

    /*
     * If the status returned from session save callback is 'True',
     * the session file was actually used. This means we need to
     * add:
     * 	-session <session file name>
     * to the command saved, which is 2 more strings.
     */
    if (status)
        new_argc += 2;

    /*
     * Allocate vector
     */
    new_argv = (char **)XtMalloc((sizeof(char **) * new_argc));

    /*
     * Set new_argv[0] to be the application name
     */
    new_argc_counter = 0;
    new_argv[new_argc_counter] = app_name;
    new_argc_counter++;

    /*
     * Proceed to copy every command line option from
     * client_argv. Skip -session, if found.
     */
    for (i=0; i < client_argc;)
    {
        if (strcmp(client_argv[i], "-session"))
        {
            new_argv[new_argc_counter] = client_argv[i];
            new_argc_counter++;
        }
        else
        {
            /*
             * Skip "-session"
             * The next increment below will skip the session file.
             */
            ++i;
        }

        ++i;
        
    }

    /*
     * If session file used, add
     *	-session <session file name>
     */
    if (status)
    {
        new_argv[new_argc_counter] = "-session";
        new_argc_counter++;
        new_argv[new_argc_counter] = session_file_name;
    }
    else
    {
        /*
         * otherwise, destroy session file
         */
        (void)unlink(session_file_path);
    }

    /*
     * Set WM_COMMAND property with vector constructed
     */
    XSetCommand(XtDisplay(widget), XtWindow(widget),
    		new_argv, new_argc);

    /*
     * Free argument vector
     */
    XtFree ((char *)new_argv);

    /*
     * CDE Sessioning API states that the path/name
     * strings have to be free'd by the application.
     */
    XtFree ((char *)session_file_path);
    XtFree ((char *)session_file_name);
}

/*
 * dtb_get_client_session_saveCB()
 */
DtbClientSessionSaveCB
dtb_get_client_session_saveCB()
{
    return(dtb_client_session_saveCB);

}

/*
 * dtb_set_client_session_saveCB()
 */
void
dtb_set_client_session_saveCB(
    DtbClientSessionSaveCB 	session_saveCB
)
{
    dtb_client_session_saveCB = session_saveCB;

}

/*
 * dtb_session_restore()
 * Callback that is called during session restore (application
 * startup). It is called only if the application was invoked
 * with:
 *	-session <session file name>
 * It calls the client session restore callback.
 */
void
dtb_session_restore(
    Widget 	widget,
    char 	*session_file
)
{
    DtbClientSessionRestoreCB	session_restoreCB;
    char			*session_file_path;
    Boolean			status;

    /*
     * If parameters are NULL, return.
     */
    if (!widget || !session_file)
        return;

    /*
     * Get path of session file to read
     */
    if (DtSessionRestorePath(widget, &session_file_path, session_file) == False)
        return;

    /*
     * Get client session restore callback
     */
    session_restoreCB = dtb_get_client_session_restoreCB();

    /*
     * Call client session restore callback
     */
    if (session_restoreCB)
        status = session_restoreCB(widget, session_file_path);

    /*
     * CDE Sessioning API states that the path
     * string has to be free'd by the application.
     */
    if (!session_file_path)
        XtFree((char *)session_file_path);

}

/*
 * dtb_set_client_session_restoreCB()
 */
void
dtb_set_client_session_restoreCB(
    DtbClientSessionRestoreCB 	session_restoreCB
)
{
    dtb_client_session_restoreCB = session_restoreCB;

}

/*
 * dtb_get_client_session_restoreCB()
 */
DtbClientSessionRestoreCB
dtb_get_client_session_restoreCB()
{
    return(dtb_client_session_restoreCB);

}

/*
 * Create a Message Dialog.
 */
Widget
dtb_create_message_dlg(
    Widget		parent,
    DtbMessageData	mbr,
    XmString		override_msg,
    DtbObjectHelpData	override_help
)
{
    Widget      	msg_dlg = (Widget) NULL;
    Widget      	shell = (Widget) NULL;
    Widget      	button = (Widget) NULL;
    Widget		action_button = (Widget) NULL;
    unsigned char	default_btn = XmDIALOG_NONE;
    Arg         	arg[12];
    int         	n = 0;

    /* The dialog should be parented off of a Shell,
     * so walk up the tree to find the parent's shell
     * ancestor...
     */
    shell = parent;
    while(!XtIsSubclass(shell, shellWidgetClass))
        shell = XtParent(shell);
 
    msg_dlg = XmCreateMessageDialog(shell,"dtb_msg_dlg", NULL, 0);
 
    if (!mbr->cancel_button)
    {
        button = XmMessageBoxGetChild(msg_dlg, XmDIALOG_CANCEL_BUTTON);
        XtUnmanageChild(button);
    }
    if (!mbr->help_button)
    {
        button = XmMessageBoxGetChild(msg_dlg, XmDIALOG_HELP_BUTTON);
        XtUnmanageChild(button);
    }
    if (mbr->action1_label == (XmString) NULL)
    {
        button = XmMessageBoxGetChild(msg_dlg, XmDIALOG_OK_BUTTON);
        XtUnmanageChild(button);
    }
    else
    {
        XtSetArg(arg[n], XmNokLabelString, mbr->action1_label); n++;
    }
    /* Create an extra button for the MessageBox */
    if (mbr->action2_label != (XmString) NULL)
    {
	button = XtVaCreateManagedWidget("action2_button",
			xmPushButtonWidgetClass,
			msg_dlg,
			XmNlabelString,	mbr->action2_label,
			XmNuserData,	DTB_ACTION2_BUTTON,
			NULL);
    }
    /* Create an extra button for the MessageBox */
    if (mbr->action3_label != (XmString) NULL)
    {
        button = XtVaCreateManagedWidget("action3_button",
                        xmPushButtonWidgetClass,
                        msg_dlg,
                        XmNlabelString, mbr->action3_label,
                        XmNuserData,    DTB_ACTION3_BUTTON,
                        NULL);
    }

    XtSetArg(arg[n], XmNdialogType,       mbr->type);            n++;
    XtSetValues(msg_dlg, arg, n);

    if (override_msg != (XmString) NULL)
    {
	XtVaSetValues(msg_dlg, XmNmessageString, override_msg, NULL);
    }
    else if (mbr->message != (XmString) NULL)
    {
	XtVaSetValues(msg_dlg, XmNmessageString, mbr->message, NULL);
    }

    if (mbr->title != (XmString) NULL)
    {
	XtVaSetValues(msg_dlg, XmNdialogTitle, mbr->title, NULL);
    }
    else
    {
        XmString        null_str;

        null_str = XmStringCreateLocalized(" ");
        XtVaSetValues(msg_dlg, XmNdialogTitle, null_str, NULL);
        XmStringFree(null_str);
    }


    switch (mbr->default_button)
    {
	case DTB_ACTION1_BUTTON:
	    default_btn = XmDIALOG_OK_BUTTON;
	    break;
	case DTB_ACTION2_BUTTON:
	case DTB_ACTION3_BUTTON:
	case DTB_NONE:
	    default_btn = XmDIALOG_NONE;
	    break;
	case DTB_CANCEL_BUTTON:
	    default_btn = XmDIALOG_CANCEL_BUTTON;
	    break;
	default:
	    break;
    }
    XtVaSetValues(msg_dlg, XmNdefaultButtonType, default_btn, NULL);

    if (mbr->default_button == DTB_ACTION2_BUTTON)
    {
	action_button = dtb_MessageBoxGetActionButton(msg_dlg,
				DTB_ACTION2_BUTTON);
    }
    else if (mbr->default_button == DTB_ACTION3_BUTTON)
    {
        action_button = dtb_MessageBoxGetActionButton(msg_dlg,
                                DTB_ACTION3_BUTTON);
    }

    if (action_button != (Widget) NULL)
    {
	XtVaSetValues(action_button, 
                        XmNdefaultButtonShadowThickness, 2,
			XmNshowAsDefault, True, 
			NULL);
	XtVaSetValues(msg_dlg, XmNdefaultButton, action_button, NULL);
    }

    XtAddCallback(XtParent(msg_dlg), XtNpopdownCallback, destroyCB,
		 (XtPointer) override_help);

    return(msg_dlg);
}

/*
 * popdownCallback for MessageBox.
 */
static void
destroyCB(
    Widget      widget,
    XtPointer   client_data,
    XtPointer   call_data
)
{
    DtbObjectHelpData 	help_data = (DtbObjectHelpData)client_data;

    if (help_data != (DtbObjectHelpData) NULL)
    {
	if (help_data->help_text)
	    XtFree((char *)help_data->help_text);
	if (help_data->help_volume)
	    XtFree((char *)help_data->help_volume);
	if (help_data->help_locationID)
	    XtFree((char *)help_data->help_locationID);
	XtFree((char *)help_data);
    }

    XtDestroyWidget(widget);
}

/* 
 * Get handle to Action2 button.
 */
Widget
dtb_MessageBoxGetActionButton(
    Widget	msg_dlg,
    DTB_BUTTON	which_btn
)
{
    int			i, numChildren = 0;
    WidgetList		children = NULL;
    Widget		action_button = NULL;
    int			button = -1;
    Boolean		Found = False;

    XtVaGetValues(msg_dlg,
			XmNnumChildren, &numChildren,
			XmNchildren, &children,
			NULL);
    for (i = 0; i < numChildren && !Found; i++)
    {
	XtVaGetValues(children[i], XmNuserData, &button, NULL);
	if (which_btn == (DTB_BUTTON) button)
	{
	    Found = True;
	    action_button = children[i];
	}
    }
    return (action_button);
}

/*
 * Use this routine to post a non-modal message. It should
 * be used to post Information and Working messages.
 */
void
dtb_show_message(
    Widget		parent,
    DtbMessageData	mbr,
    XmString		override_msg,
    DtbObjectHelpData	override_help
)
{
    DtbObjectHelpData	help_data_copy = (DtbObjectHelpData)NULL;
    Widget      	msg_dlg = (Widget) NULL, 
			action_btn = (Widget) NULL;

    if (override_help != (DtbObjectHelpData) NULL)
    {
	help_data_copy = (DtbObjectHelpData)XtMalloc(sizeof(DtbObjectHelpDataRec));

	help_data_copy->help_text = override_help->help_text ? 
		XtNewString(override_help->help_text) : 
		NULL;
	help_data_copy->help_volume = override_help->help_volume ? 
		XtNewString(override_help->help_volume) :
		NULL;
	help_data_copy->help_locationID = override_help->help_locationID ?
		XtNewString(override_help->help_locationID) :
		NULL;
    }

    msg_dlg = dtb_create_message_dlg(parent, 
		mbr, override_msg, help_data_copy);

    if (msg_dlg == (Widget) NULL)
	return;

    /* Add Callbacks if necessary */
    if (mbr->action1_callback != (XtCallbackProc) NULL)
        XtAddCallback(msg_dlg, XmNokCallback, mbr->action1_callback, NULL);
    if (mbr->cancel_callback != (XtCallbackProc) NULL)
        XtAddCallback(msg_dlg, XmNcancelCallback, mbr->cancel_callback, NULL);
    if (mbr->action2_callback != (XtCallbackProc) NULL)
    {
	action_btn = dtb_MessageBoxGetActionButton(msg_dlg, DTB_ACTION2_BUTTON);
	if (action_btn != NULL)
            XtAddCallback(action_btn, XmNactivateCallback,
			  mbr->action2_callback, NULL);
    }
    if (mbr->action3_callback != (XtCallbackProc) NULL)
    {
        action_btn = dtb_MessageBoxGetActionButton(msg_dlg, DTB_ACTION3_BUTTON);        if (action_btn != NULL)
            XtAddCallback(action_btn, XmNactivateCallback,
                          mbr->action3_callback, NULL);
    }
    if (mbr->help_button)
    {
	if (help_data_copy != (DtbObjectHelpData) NULL)
	{
            XtAddCallback(msg_dlg,
                        XmNhelpCallback, dtb_help_dispatch,
                        (XtPointer) help_data_copy);
	}
	else if (mbr->help_data.help_text != (char *) NULL)
	{
	    XtAddCallback(msg_dlg, 
			XmNhelpCallback, dtb_help_dispatch,
			(XtPointer) &(mbr->help_data));
	}
    }

    XtManageChild(msg_dlg);
    XRaiseWindow(XtDisplay(msg_dlg), XtWindow(XtParent(msg_dlg)));
}

/*
 * Use this routine to post a modal message.
 * It should be used to post Error, Question, and Warning messages.
 * It returns the information on which button was pressed. A switch
 * statement can then be done to process the answer.
 */
DTB_MODAL_ANSWER
dtb_show_modal_message(
    Widget		parent,
    DtbMessageData	mbr,
    XmString		override_msg,
    DtbObjectHelpData	override_help,
    Widget         	*modal_dlg_pane_out_ptr
)                                 
{
    XtAppContext		app;
    Widget			modal_dlg_pane = (Widget) NULL;
    Widget			action2_button = (Widget) NULL;
    Widget			action3_button = (Widget) NULL;
    DtbObjectHelpData		help_data_copy = (DtbObjectHelpData)NULL;
    DTB_MODAL_ANSWER		answer = DTB_ANSWER_NONE;
    XtCallbackRec ok_callback[] = {
            {(XtCallbackProc)modal_dlgCB, (XtPointer) DTB_ANSWER_ACTION1},
            {(XtCallbackProc) NULL, (XtPointer) NULL}
    };
    XtCallbackRec cancel_callback[] = {
            {(XtCallbackProc)modal_dlgCB, (XtPointer) DTB_ANSWER_CANCEL},
            {(XtCallbackProc) NULL, (XtPointer) NULL}
    };

    if (override_help != (DtbObjectHelpData) NULL)
    {
	help_data_copy = (DtbObjectHelpData)XtMalloc(sizeof(DtbObjectHelpDataRec));

	help_data_copy->help_text = override_help->help_text ? 
		XtNewString(override_help->help_text) : 
		NULL;
	help_data_copy->help_volume = override_help->help_volume ? 
		XtNewString(override_help->help_volume) :
		NULL;
	help_data_copy->help_locationID = override_help->help_locationID ?
		XtNewString(override_help->help_locationID) :
		NULL;
    }

    modal_dlg_pane = dtb_create_message_dlg(parent, mbr, 
				override_msg, help_data_copy);

    if (modal_dlg_pane == (Widget) NULL)
	return (answer);
 
    XtVaSetValues(modal_dlg_pane,
                XmNdialogStyle,    XmDIALOG_FULL_APPLICATION_MODAL,
                XmNokCallback,     &ok_callback,
                XmNcancelCallback, &cancel_callback,
                XmNuserData,       &answer,
                NULL);

    action2_button = dtb_MessageBoxGetActionButton(modal_dlg_pane,
				DTB_ACTION2_BUTTON);
    if (action2_button != (Widget) NULL)
    {
	XtVaSetValues(action2_button,
                        XmNuserData, (XtPointer) &answer,
                        NULL);
        XtAddCallback(action2_button,
                        XmNactivateCallback, modal_dlgCB,
                        (XtPointer) DTB_ANSWER_ACTION2);
    }

    action3_button = dtb_MessageBoxGetActionButton(modal_dlg_pane,
                                DTB_ACTION3_BUTTON);
    if (action3_button != (Widget) NULL)
    {
        XtVaSetValues(action3_button,
                        XmNuserData, (XtPointer) &answer,
                        NULL);
        XtAddCallback(action3_button,
                        XmNactivateCallback, modal_dlgCB,
                        (XtPointer) DTB_ANSWER_ACTION3);
    }

    if (mbr->help_button) 
    { 
        if (help_data_copy != (DtbObjectHelpData) NULL)
        {
            XtAddCallback(modal_dlg_pane, 
                        XmNhelpCallback, dtb_help_dispatch, 
                        (XtPointer) help_data_copy); 
	}
        else if (mbr->help_data.help_text != (char *) NULL) 
        { 
            XtAddCallback(modal_dlg_pane, 
                        XmNhelpCallback, dtb_help_dispatch, 
                        (XtPointer) &(mbr->help_data)); 
        }
    }   

    /* Popup Modal MessageDialog and wait for answer */
    XtManageChild(modal_dlg_pane);
    XRaiseWindow(XtDisplay(modal_dlg_pane), XtWindow(XtParent(modal_dlg_pane)));

    app = XtDisplayToApplicationContext(XtDisplay(modal_dlg_pane));
    while (answer == DTB_ANSWER_NONE)
        XtAppProcessEvent(app, XtIMAll);
 
    if (modal_dlg_pane_out_ptr != NULL)
    {
        (*modal_dlg_pane_out_ptr) = modal_dlg_pane;
    }
    return(answer);
}

/*
 * This is the activateCallback for the MessageBox buttons.
 * It returns the button which was pressed.
 */
static void
modal_dlgCB(
    Widget      widget,
    XtPointer   client_data,
    XtPointer   call_data
)
{
    DTB_MODAL_ANSWER     op = (DTB_MODAL_ANSWER) client_data;
    DTB_MODAL_ANSWER     *answerp = (DTB_MODAL_ANSWER) NULL;
 
    XtVaGetValues(widget, XmNuserData, &answerp, NULL);
 
    /* Will cause Modal dialog to return */
    *answerp = op;
}

/*
 * This function will center all the passed form's children.
 * The type of centering depends on what 'type' is.
 */
void
dtb_children_center(
    Widget		form,
    DTB_CENTERING_TYPES	type
)
{
    WidgetList		children_list;
    int			i, 
			num_children;

    if (!form || (type == DTB_CENTER_NONE))
	return;

    /*
     * Get children list
     */
    XtVaGetValues(form,
            XmNnumChildren, &num_children,
            XmNchildren, &children_list,
            NULL);

    /*
     * Center all children
     */
    for (i=0; i < num_children; ++i)
    {
	dtb_center(children_list[i], type);
    }
}

/*
 * This function 'uncenters' the children of the passed
 * form widget.
 */
void
dtb_children_uncenter(
    Widget		form,
    DTB_CENTERING_TYPES	type
)
{
    WidgetList		children_list;
    int			i;
    int			num_children;

    if (!form || (type == DTB_CENTER_NONE))
	return;

    /*
     * Get children list
     */
    XtVaGetValues(form,
            XmNnumChildren, &num_children,
            XmNchildren, &children_list,
            NULL);

    /*
     * Center all children
     */
    for (i=0; i < num_children; ++i)
    {
	dtb_uncenter(children_list[i], type);
    }
}

/*
 * This function centers the passed widget.
 * This is done by setting the proper offsets.
 * Dynamic centering is accomplished by attaching an event handler
 * which detect resizes and recomputes and sets the appropriate offset.
 */
void
dtb_center(
    Widget		form_child,
    DTB_CENTERING_TYPES	type
)
{
    if (!form_child || (type == DTB_CENTER_NONE))
	return;

   center_widget(form_child, type);

   XtAddEventHandler(form_child,
            StructureNotifyMask, False,
            centering_handler, (XtPointer)type);
}

/*
 * This function 'uncenters' the passed widget.
 * This involves resetting the attachment offsets
 * and removing the resize event handler.
 */
void
dtb_uncenter(
    Widget		form_child,
    DTB_CENTERING_TYPES	type
)
{
    if (!form_child || (type == DTB_CENTER_NONE))
	return;

   uncenter_widget(form_child, type);

   XtRemoveEventHandler(form_child,
            StructureNotifyMask, False,
            centering_handler, (XtPointer)type);
}

/*
 * This function centers the passed widget.
 * This is done by making the appropriate offset equal 
 * to the negative half of it's width/height (depending
 * on whether horizontal or vertical centering was chosen.
 */
static void 
center_widget(
    Widget		form_child,
    DTB_CENTERING_TYPES	type
)
{
    Widget		parent;
    Dimension		width = 0,
			height = 0;
    int			center_offset;
    unsigned char	left_attach = XmATTACH_NONE,
			top_attach = XmATTACH_NONE;

    if (!form_child || !XtIsManaged(form_child) || !XtIsRealized(form_child))
	return;

    parent = XtParent(form_child);

    if (!parent || !XtIsSubclass(parent, xmFormWidgetClass))
	return;

    XtVaGetValues(form_child,
		XmNwidth, &width,
		XmNheight, &height,
		XmNleftAttachment, &left_attach,
		XmNtopAttachment, &top_attach,
                NULL);

    switch (type) {
	case DTB_CENTER_POSITION_VERT:

            if (left_attach != XmATTACH_POSITION)
	        return;

            center_offset = -(width/2);

            XtVaSetValues(form_child,
		XmNleftOffset, center_offset,
		NULL);

	break;

	case DTB_CENTER_POSITION_HORIZ:

            if (top_attach != XmATTACH_POSITION)
	        return;

            center_offset = -(height/2);

            XtVaSetValues(form_child,
		XmNtopOffset, center_offset,
		NULL);
	break;

	case DTB_CENTER_POSITION_BOTH:
	{
            int		left_offset,
			top_offset;

            if ((left_attach != XmATTACH_POSITION) &&
	        (top_attach != XmATTACH_POSITION))
	        return;
	    
	    left_offset = -(width/2);
	    top_offset = -(height/2);

            XtVaSetValues(form_child,
		XmNleftOffset, left_offset,
		XmNtopOffset, top_offset,
		NULL);
	}
	break;
    }
}

/*
 * This function 'uncenters' the passed widget.
 * It merely resets the offsets of the top/left attachments to 0.
 */
static void 
uncenter_widget(
    Widget		form_child,
    DTB_CENTERING_TYPES	type
)
{
    Widget		parent;
    unsigned char	left_attach = XmATTACH_NONE,
			top_attach = XmATTACH_NONE;

    if (!form_child || !XtIsManaged(form_child) || !XtIsRealized(form_child))
	return;

    parent = XtParent(form_child);

    if (!parent || !XtIsSubclass(parent, xmFormWidgetClass))
	return;

    XtVaGetValues(form_child,
		XmNleftAttachment, &left_attach,
		XmNtopAttachment, &top_attach,
                NULL);

    switch (type) {
	case DTB_CENTER_POSITION_VERT:

            if (left_attach != XmATTACH_POSITION)
	        return;

            XtVaSetValues(form_child,
		XmNleftOffset, 0,
		NULL);

	break;

	case DTB_CENTER_POSITION_HORIZ:

            if (top_attach != XmATTACH_POSITION)
	        return;

            XtVaSetValues(form_child,
		XmNtopOffset, 0,
		NULL);
	break;

	case DTB_CENTER_POSITION_BOTH:

            if ((left_attach != XmATTACH_POSITION) &&
	        (top_attach != XmATTACH_POSITION))
	        return;

            XtVaSetValues(form_child,
		XmNleftOffset, 0,
		XmNtopOffset, 0,
		NULL);
	break;
    }
}

/*
 * Event handler to center a widget
 * The type of centering needed is passed in as client_data
 */
static void
centering_handler(
    Widget	widget,
    XtPointer	client_data,
    XEvent	*event,
    Boolean	*cont_dispatch
)
{
    XConfigureEvent	*xcon = &event->xconfigure;
    Widget		resized_child;
    DTB_CENTERING_TYPES	type = (DTB_CENTERING_TYPES)client_data;


    if ((event->type != ConfigureNotify) && (event->type != MapNotify))
        return;

    resized_child = XtWindowToWidget(XtDisplay(widget), xcon->window);

    if (!resized_child)
	return;

    center_widget(resized_child, type);
}


/*
 * Given a widget, return it's label widget.
 */
static Widget
get_label_widget(
    Widget	widget
)
{
    WidgetList	children_list;
    Widget	label_widget = NULL;
    int		i,
		num_children = 0;
    char	*subobj_name = NULL,
		*label_name = NULL;
    char	*underscore_ptr = NULL;

    if (XtIsSubclass(widget, xmLabelWidgetClass))  {
	return(widget);
    }

    subobj_name = XtName(widget);
    label_name = (char *)XtMalloc(1 + strlen(subobj_name) + strlen("_label") + 5);
    label_name[0] = '*';
    strcpy(label_name+1, subobj_name);
    if ((underscore_ptr = strrchr(label_name, '_')) != NULL)
    {
	strcpy(underscore_ptr, "_label");
        label_widget = XtNameToWidget(widget, label_name);
    }
    if (label_widget == NULL)
    {
	strcpy(label_name+1, subobj_name);
	strcat(label_name, "_label");
        label_widget = XtNameToWidget(widget, label_name);
    }

    XtFree((char *)label_name);

    if (label_widget)
	return(label_widget);

    /*
     * How to look for 1st child of group object ??
     * How do we know if 'widget' is a group object ??
     * For now, just check if it is a form
     */
    if (XtIsSubclass(widget, xmFormWidgetClass) || 
        XtIsSubclass(widget, xmFrameWidgetClass))
        XtVaGetValues(widget,
            XmNnumChildren, &num_children,
            XmNchildren, &children_list,
            NULL);

    if (num_children > 0)
        return(get_label_widget(children_list[0]));

    return (NULL);
}


static Position
get_offset_from_ancestor(
    Widget	ancestor,
    Widget	w
)
{
    Widget	cur = w;
    Widget	cur_parent = NULL;
    Position	offset = 0;

    if (!ancestor || !w || (w == ancestor))
	return (0);

    XtVaGetValues(cur, XmNx, &offset, NULL);

    cur_parent = XtParent(cur);

    while (cur_parent != ancestor)
    {
        Position	tmp_offset = 0;

	cur = cur_parent;
        XtVaGetValues(cur, XmNx, &tmp_offset, NULL);
	
	offset += tmp_offset;
        cur_parent = XtParent(cur);
    }

    return (offset);
}

static Dimension
get_label_width(
    Widget	widget
)
{
    WidgetList	children_list;
    Widget	lbl_widget = NULL;
    Dimension	lbl_width = 0;

    lbl_widget = get_label_widget(widget);

    if (lbl_widget)
    {
	Position	offset = 0;

        XtVaGetValues(lbl_widget,
            XmNwidth, &lbl_width,
            NULL);

	offset = get_offset_from_ancestor(widget, lbl_widget);

	lbl_width += (Dimension)offset;
    }
    
    return (lbl_width);
}

static void
get_widest_label(
    WidgetList	list,
    int		count,
    Widget	*child_widget,
    Dimension	*label_width
)
{
    Widget	cur_widest = NULL;
    Dimension	cur_width = 0;
    int		i;

    for (i = 0; i < count; ++i)
    {
	Dimension	tmp;

	tmp = get_label_width(list[i]);

	if (tmp > cur_width)
	{
	    cur_width = tmp;
	    cur_widest = list[i];
	}
    }
    
    *child_widget = cur_widest;
    *label_width = cur_width;
}


static void
get_widest_value(
    WidgetList	list,
    int		count,
    Widget	*child_widget,
    Dimension	*value_width
)
{
    Widget	cur_widest = NULL;
    Dimension	cur_width = 0;
    int		i;

    for (i = 0; i < count; ++i)
    {
	Dimension	tmp, label_width, obj_width = 0;

	label_width = get_label_width(list[i]);
	XtVaGetValues(list[i], XmNwidth, &obj_width, NULL);

	tmp = obj_width - label_width;

	if (tmp > cur_width)
	{
	    cur_width = tmp;
	    cur_widest = list[i];
	}
    }
    
    *child_widget = cur_widest;
    *value_width = cur_width;
}


static void
get_widget_rect(
    Widget widget,
    XRectangle *rect
)
{
    if (!rect)
	return;

    XtVaGetValues(widget,
        XtNwidth,       (XtArgVal)&(rect->width),
        XtNheight,      (XtArgVal)&(rect->height),
        XtNx,           (XtArgVal)&(rect->x),
        XtNy,           (XtArgVal)&(rect->y),
        NULL);
}

static void
get_greatest_size(
    Widget	*list,
    int		count,
    int		*width,
    int		*height,
    Widget	*tallest,
    Widget	*widest
)
{
    XRectangle  w_rect;
    int         i;
    int		previous_width, previous_height;

    if (!list || (count < 0))
	return;

    get_widget_rect(list[0], &w_rect);

    *width = w_rect.width;
    previous_width = *width;

    *height = w_rect.height;
    previous_height = *height;

    if (tallest != NULL)
        *tallest = list[0];

    if (widest != NULL)
    	*widest = list[0];

    for (i=0; i < count; i++)
    {
        get_widget_rect(list[i], &w_rect);

        *width = max((int) w_rect.width, (int) *width); 
	if (widest != NULL && *width > previous_width)
		*widest = list[i];

        *height = max((int) w_rect.height, (int)*height);
	if (tallest != NULL && *height > previous_height)
		*tallest = list[i];
    }
}

static void
get_group_cell_size(
    Widget		parent,
    DtbGroupInfo	*group_info,
    int			*cell_width,
    int			*cell_height
)
{
    WidgetList	children_list = NULL;
    int		i,
		num_children = 0;

    /*
     * Get children list
     */
    XtVaGetValues(parent,
            XmNnumChildren, &num_children,
            XmNchildren, &children_list,
            NULL);

    get_greatest_size(children_list, num_children,
		cell_width, cell_height,
		(Widget *)NULL, (Widget *)NULL);
}

static void
get_group_row_col(
    Widget		parent,
    DtbGroupInfo	*group_info,
    int			*rows,
    int			*cols
)
{
    WidgetList	children_list = NULL;
    int		num_rows,
		num_cols,
    		num_children;

    if (!parent || !group_info)
    {
	*rows = *cols = -1;

	return;
    }

    /*
     * Get children list
     */
    XtVaGetValues(parent,
            XmNnumChildren, &num_children,
            XmNchildren, &children_list,
            NULL);

    num_rows = group_info->num_rows;
    num_cols = group_info->num_cols;

    if ((num_rows <= 0) && (num_cols <= 0))
    {
	*rows = *cols = -1;

	return;
    }

    if (num_cols <= 0)
        num_cols = (num_children/num_rows) + ((num_children % num_rows) ? 1 : 0);

    if (num_rows <= 0)
        num_rows = (num_children/num_cols) + ((num_children % num_cols) ? 1 : 0);

    *rows = num_rows;
    *cols = num_cols;
}

static Widget
get_group_child(
    Widget		parent,
    DtbGroupInfo	*group_info,
    int			x_pos,
    int			y_pos
)
{
    DTB_GROUP_TYPES	group_type;
    WidgetList		children_list = NULL;
    Widget		ret_child = NULL;
    int			num_children = 0,
			num_rows,
			num_columns,
    			i = -1;

    if (!parent || !group_info || 
       (x_pos < 0) || (y_pos < 0))
	return (NULL);

    group_type = group_info->group_type;
    num_rows = group_info->num_rows;
    num_columns = group_info->num_cols;

    /*
     * Get number of children
     */
    XtVaGetValues(parent,
            XmNnumChildren, &num_children,
            XmNchildren, &children_list,
            NULL);

    if (num_children <= 0)
	return (NULL);

    switch (group_type)
    {
	case DTB_GROUP_NONE:
	break;

	case DTB_GROUP_ROWS:
	    /*
	     * num_rows = 1
	     * y_pos is ignored
	     */
	    i = x_pos;
	break;

	case DTB_GROUP_COLUMNS:
	    /*
	     * num_columns = 1
	     * x_pos is ignored
	     */
	    i = y_pos;
	break;

	case DTB_GROUP_ROWSCOLUMNS:
	    if (!num_rows && !num_columns)
		break;

	    if (num_rows > 0)
	    {
		/*
		 * ROWFIRST
		 */
		if (y_pos < num_rows)
		    i = (x_pos * num_rows) + y_pos;
	    }
	    else
	    {
		/*
		 * COLFIRST
		 */
		if (x_pos < num_columns)
		    i = x_pos + (y_pos * num_columns);
	    }
	break;

	default:
	break;
    }

    if ((i >= 0) && (i < num_children))
    {
        ret_child = children_list[i];
    }

    return (ret_child);
}

void
dtb_children_align(
    Widget		parent,
    DTB_GROUP_TYPES	group_type,
    DTB_ALIGN_TYPES	row_align,
    DTB_ALIGN_TYPES	col_align,
    int			margin,
    int			num_rows,
    int			num_cols,
    int			hoffset,
    int			voffset
)
{
    DtbGroupInfo		*group_info;

    switch (group_type)
    {
	case DTB_GROUP_COLUMNS:
	    num_rows = 0;
	    num_cols = 1;
	break;

	case DTB_GROUP_ROWS:
	    num_rows = 1;
	    num_cols = 0;
	break;

    }

    group_info = (DtbGroupInfo *)XtMalloc(sizeof(DtbGroupInfo));

    group_info->group_type = group_type;
    group_info->row_align = row_align;
    group_info->col_align = col_align;
    group_info->margin = margin;
    group_info->num_rows = num_rows;
    group_info->num_cols = num_cols;
    group_info->hoffset = hoffset;
    group_info->voffset = voffset;
    group_info->ref_widget = NULL;

    align_children(parent, group_info, True);

    /*
     * Register expose handler
     * Some group objects depend on it's members' sizes for their layout.
     * Unfortunately, some group members have invalid sizes prior to
     * XtRealize(), so the group layout has to be recalculated after the
     * group is realized or exposed in this case, since there is no realize
     * callback.
     */
    switch(group_info->group_type)
    {
	case DTB_GROUP_NONE:
	break;

	case DTB_GROUP_ROWS:
	    if (group_info->row_align == DTB_ALIGN_HCENTER)
		XtAddEventHandler(parent,
	                ExposureMask, False,
	                expose_handler, (XtPointer)group_info);
	break;

	case DTB_GROUP_COLUMNS:
	    if ((group_info->col_align == DTB_ALIGN_LABELS) || 
		(group_info->col_align == DTB_ALIGN_VCENTER))
		XtAddEventHandler(parent,
	                ExposureMask, False,
	                expose_handler, (XtPointer)group_info);
	break;

	case DTB_GROUP_ROWSCOLUMNS:
	    if ((group_info->row_align == DTB_ALIGN_HCENTER) ||
	        (group_info->col_align == DTB_ALIGN_LABELS) || 
		(group_info->col_align == DTB_ALIGN_VCENTER))
		XtAddEventHandler(parent,
	                ExposureMask, False,
	                expose_handler, (XtPointer)group_info);
	break;

    }

    XtAddCallback(parent, XtNdestroyCallback, 
		free_group_info, (XtPointer)group_info);
}

static void
align_children(
    Widget		parent,
    DtbGroupInfo	*group_info,
    Boolean		init
)
{
    if (!parent || !group_info)
	return;

    switch(group_info->group_type)
    {
        case DTB_GROUP_NONE:
        break;

        case DTB_GROUP_ROWS:
	    align_rows(parent, group_info, init);
	    align_left(parent, group_info);
        break;

        case DTB_GROUP_COLUMNS:
	    align_cols(parent, group_info, init);
	    align_top(parent, group_info);
        break;

        case DTB_GROUP_ROWSCOLUMNS:
	    align_rows(parent, group_info, init);
	    align_cols(parent, group_info, init);
        break;

    }

}

static void
align_handler(
    Widget	widget,
    XtPointer	client_data,
    XEvent	*event,
    Boolean	*cont_dispatch
)
{
    DtbGroupInfo	*group_info = (DtbGroupInfo *)client_data;
    WidgetList	children_list;
    int		i,
		num_children = 0;
    Boolean	relayout_all = False;


    /*
     * Get children list
     */
    XtVaGetValues(widget,
            XmNnumChildren, &num_children,
            XmNchildren, &children_list,
            NULL);

    if (num_children <= 0)
	return;

    XtRemoveEventHandler(widget,
                SubstructureNotifyMask, False,
                align_handler, (XtPointer)client_data);

    if (event->type == ConfigureNotify) {
        XConfigureEvent	*xcon = &event->xconfigure;
	Widget		resized_child;

	if (xcon->window != xcon->event)
	{
            resized_child = XtWindowToWidget(XtDisplay(widget), xcon->window);

            switch(group_info->group_type)
            {
                case DTB_GROUP_NONE:
                break;

                case DTB_GROUP_ROWS:
                    if (group_info->row_align == DTB_ALIGN_HCENTER)
                        relayout_all = True;
                break;

                case DTB_GROUP_COLUMNS:
                    if ((group_info->col_align == DTB_ALIGN_LABELS) ||
                        (group_info->col_align == DTB_ALIGN_VCENTER))
                            relayout_all = True;
                break;

                case DTB_GROUP_ROWSCOLUMNS:
                    if ((group_info->row_align == DTB_ALIGN_HCENTER) ||
                        (group_info->col_align == DTB_ALIGN_LABELS) ||
                        (group_info->col_align == DTB_ALIGN_VCENTER))
                        relayout_all = True;
                break;

            }
        }
    }

    /*
     * Relayout when new widgets are created
     */
    if (event->type == CreateNotify) {
        XCreateWindowEvent	*xcreate = &event->xcreatewindow;

	relayout_all = True;
    }

    /*
     * Relayout when widgets are destroyed
     */
    if (event->type == DestroyNotify) {
        XDestroyWindowEvent	*xdestroy = &event->xdestroywindow;
	Widget			destroyed_child;

        destroyed_child = XtWindowToWidget(XtDisplay(widget), 
			xdestroy->window);

	relayout_all = True;
    }

    if (relayout_all)
    {
        align_children(widget, group_info, False);
    }

    XtAddEventHandler(widget,
                SubstructureNotifyMask, False,
                align_handler, (XtPointer)client_data);
}


static void
expose_handler(
    Widget	widget,
    XtPointer	client_data,
    XEvent	*event,
    Boolean	*cont_dispatch
)
{
    DtbGroupInfo	*group_info = (DtbGroupInfo *)client_data;
    WidgetList	children_list;
    int		i,
		num_children = 0;
    Boolean	relayout_all = False,
		register_align_handler = False;


    if (event->type != Expose) 
	return;
    
    if (!group_info)
	return;

    /*
     * Get children list
     */
    XtVaGetValues(widget,
            XmNnumChildren, &num_children,
            XmNchildren, &children_list,
            NULL);

    if (num_children <= 0)
	return;

    XtRemoveEventHandler(widget,
                ExposureMask, False,
                expose_handler, (XtPointer)client_data);

    switch(group_info->group_type)
    {
        case DTB_GROUP_NONE:
        break;

        case DTB_GROUP_ROWS:
        if (group_info->row_align == DTB_ALIGN_HCENTER)
        {
            relayout_all = True;

            register_align_handler = True;
        }
        break;

        case DTB_GROUP_COLUMNS:
        if ((group_info->col_align == DTB_ALIGN_LABELS) ||
            (group_info->col_align == DTB_ALIGN_VCENTER))
        {
            relayout_all = True;

            register_align_handler = True;
        }
        break;

        case DTB_GROUP_ROWSCOLUMNS:
        if ((group_info->row_align == DTB_ALIGN_HCENTER) ||
            (group_info->col_align == DTB_ALIGN_LABELS) ||
            (group_info->col_align == DTB_ALIGN_VCENTER))
        {
            relayout_all = True;

            register_align_handler = True;
        }
        break;
    }

    if (relayout_all)
    {
        align_children(widget, group_info, False);
    }

    if (register_align_handler)
    {
	/*
	 * Register align handler to relayout group if/when
	 * any of it's members resize
	 */
	XtAddEventHandler(widget,
            SubstructureNotifyMask, False,
            align_handler, (XtPointer)group_info);
    }
}



static void		
free_group_info(
    Widget	widget,
    XtPointer	client_data,
    XtPointer	call_data
)
{
    DtbGroupInfo	*group_info = (DtbGroupInfo *)client_data;;

    XtFree((char *)group_info);
}

static void
align_rows(
    Widget		parent,
    DtbGroupInfo	*group_info,
    Boolean		init
)
{
    if (!parent || !group_info || (group_info->group_type == DTB_GROUP_COLUMNS))
	return;

    switch (group_info->row_align)
    {
        case DTB_ALIGN_TOP:
            align_top(parent, group_info);
        break;

        case DTB_ALIGN_HCENTER:
            align_hcenter(parent, group_info, init);
        break;

        case DTB_ALIGN_BOTTOM:
            align_bottom(parent, group_info);
        break;

        default:
        break;
    }
}

static void
align_cols(
    Widget		parent,
    DtbGroupInfo	*group_info,
    Boolean		init
)
{
    if (!parent || !group_info || (group_info->group_type == DTB_GROUP_ROWS))
	return;

    switch (group_info->col_align)
    {
        case DTB_ALIGN_LEFT:
            align_left(parent, group_info);
        break;

        case DTB_ALIGN_LABELS:
            align_labels(parent, group_info);
        break;

        case DTB_ALIGN_VCENTER:
            align_vcenter(parent, group_info, init);
        break;

        case DTB_ALIGN_RIGHT:
            align_right(parent, group_info);
        break;

        default:
        break;
    }
}

static void
align_left(
    Widget		parent,
    DtbGroupInfo	*group_info
)
{
    WidgetList	children_list;
    Widget	child, 
		previous_child;
    int		num_children = 0,
		num_columns,
		num_rows,
		cell_width,
		cell_height,
		i,
		j;
  
    if (!parent || !group_info)
	return;

    /*
     * Get children list
     */
    XtVaGetValues(parent,
            XmNnumChildren, &num_children,
            XmNchildren, &children_list,
            NULL);

    if (num_children <= 0)
	return;
    
    get_group_cell_size(parent, group_info, &cell_width, &cell_height);
    get_group_row_col(parent, group_info, &num_rows, &num_columns);

    for (j = 0; j < num_rows; j++)
    {
        for (i = 0; i < num_columns; i++)
        {
            Arg		args[12];
	    int		n = 0;

	    child = get_group_child(parent, group_info, i, j);

	    if (!child)
		continue;

	    if ((i == 0) && (j == 0))
	    {
	        XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM);	n++;
	        XtSetArg(args[n], XmNleftOffset, 0);			n++;
		XtSetArg(args[n], XmNrightAttachment, XmATTACH_NONE);	n++;

	        XtSetValues(child, args, n);

		continue;
	    }

	    if (j == 0)
	    {
	        int			offset = group_info->hoffset;
		DTB_GROUP_TYPES		group_type = group_info->group_type;

	        previous_child = get_group_child(parent, group_info, i-1, j);

		if (!previous_child)
		    continue;

                if (group_type == DTB_GROUP_ROWSCOLUMNS)
		{
		    Dimension	width = 0;

		    XtVaGetValues(previous_child, XmNwidth, &width, NULL);
		    offset += (cell_width - (int)(width));
		}

	        XtSetArg(args[n], XmNleftAttachment, 
				XmATTACH_WIDGET);			n++;
	        XtSetArg(args[n], XmNleftWidget, previous_child);	n++;
	        XtSetArg(args[n], XmNleftOffset, offset);		n++;
		XtSetArg(args[n], XmNrightAttachment, XmATTACH_NONE);	n++;

	        XtSetValues(child, args, n);

		continue;
	    }

	    previous_child = get_group_child(parent, group_info, i, j-1);

	    if (previous_child)
	    {
	        XtSetArg(args[n], XmNleftAttachment, 
				    XmATTACH_OPPOSITE_WIDGET);		n++;
	        XtSetArg(args[n], XmNleftWidget, previous_child);	n++;
	        XtSetArg(args[n], XmNleftOffset, 0);			n++;
		XtSetArg(args[n], XmNrightAttachment, XmATTACH_NONE);	n++;

	        XtSetValues(child, args, n);
	    }
        }
    }
}

static void
align_right(
    Widget		parent,
    DtbGroupInfo	*group_info
)
{
    WidgetList	children_list;
    Widget	child, 
		previous_child;
    int		num_children = 0,
		num_columns,
		num_rows,
		cell_width,
		cell_height,
	        offset,
		i,
		j;

    if (!parent || !group_info)
	return;

    /*
     * Get children list
     */
    XtVaGetValues(parent,
            XmNnumChildren, &num_children,
            XmNchildren, &children_list,
            NULL);

    if (num_children <= 0)
	return;
    
    get_group_cell_size(parent, group_info, &cell_width, &cell_height);
    get_group_row_col(parent, group_info, &num_rows, &num_columns);

    for (j = 0; j < num_rows; j++)
    {
        for (i = 0; i < num_columns; i++)
        {
            Arg		args[12];
	    int		n = 0;

	    child = get_group_child(parent, group_info, i, j);

	    if (!child)
		continue;

	    if ((i == 0) && (j == 0))
	    {
		Dimension	width = 0;

		XtVaGetValues(child, XmNwidth, &width, NULL);

		offset = (cell_width - width);

	        XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM);	n++;
	        XtSetArg(args[n], XmNleftOffset, offset);		n++;
		XtSetArg(args[n], XmNrightAttachment, XmATTACH_NONE);	n++;

	        XtSetValues(child, args, n);

		continue;
	    }

	    if (j == 0)
	    {
	        previous_child = get_group_child(parent, group_info, i-1, j);

		if (!previous_child)
		    continue;

	        offset = group_info->hoffset;

                if (group_info->group_type == DTB_GROUP_ROWSCOLUMNS)
		{
		    Dimension	width = 0;

		    XtVaGetValues(child, XmNwidth, &width, NULL);
		    offset += (cell_width - width);
		}

	        XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET);	n++;
	        XtSetArg(args[n], XmNleftWidget, previous_child);	n++;
	        XtSetArg(args[n], XmNleftOffset, offset);		n++;
		XtSetArg(args[n], XmNrightAttachment, XmATTACH_NONE);	n++;

	        XtSetValues(child, args, n);

		continue;
	    }

	    previous_child = get_group_child(parent, group_info, i, j-1);

	    if (previous_child)
	    {
	        XtSetArg(args[n], XmNrightAttachment, 
				XmATTACH_OPPOSITE_WIDGET);		n++;
	        XtSetArg(args[n], XmNrightWidget, previous_child);	n++;
	        XtSetArg(args[n], XmNrightOffset, 0);			n++;
		XtSetArg(args[n], XmNleftAttachment, XmATTACH_NONE);	n++;

	        XtSetValues(child, args, n);
	    }
        }
    }
}

static void
align_labels(
    Widget		parent,
    DtbGroupInfo	*group_info
)
{
    WidgetList	children_list = NULL,
		one_col;
    Widget	previous_child = NULL,
		child,
		ref_widget,
		previous_ref_widget = NULL;
    Dimension	ref_lbl_width = 0,
		max_label_width = 0,
		max_value_width = 0;
    int		num_children = 0,
		num_rows,
		num_columns,
		cell_width,
		cell_height,
		offset,
		i,
		j,
		ref_x;

    if (!parent || !group_info)
	return;

    /*
     * Get children list
     */
    XtVaGetValues(parent,
            XmNnumChildren, &num_children,
            XmNchildren, &children_list,
            NULL);

    if (num_children <= 0)
	return;
    
    get_group_cell_size(parent, group_info, &cell_width, &cell_height);

    get_widest_label(children_list, num_children, &child, &max_label_width);
    get_widest_value(children_list, num_children, &child, &max_value_width);

    if (cell_width < (int)(max_label_width + max_value_width))
        cell_width = (int)(max_label_width + max_value_width);

    get_group_row_col(parent, group_info, &num_rows, &num_columns);

    if (num_rows > 0)
	one_col = (WidgetList)XtMalloc(num_rows * sizeof(WidgetList));

    for (i = 0; i < num_columns; i++)
    {
	Widget		ref_widget;
	Dimension	ref_width;
        Arg		args[12];
	int		n = 0;

        for (j = 0; j < num_rows; j++)
	    one_col[j] = get_group_child(parent, group_info, i, j);

	get_widest_label(one_col, num_rows, &ref_widget, &ref_width);

	if (!ref_widget)
	    continue;

	if (previous_ref_widget)
	    offset = (i * (group_info->hoffset + cell_width));
	else
	    offset = 0;

	XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM);	n++;
	XtSetArg(args[n], XmNleftOffset, offset);		n++;
        XtSetArg(args[n], XmNrightAttachment, XmATTACH_NONE);	n++;

	XtSetValues(ref_widget, args, n);

        for (j = 0; j < num_rows; j++)
        {
	    child = get_group_child(parent, group_info, i, j);

	    if (!child || (child == ref_widget))
		continue;

	    offset = (i * (group_info->hoffset + cell_width));
	    offset += (int)(ref_width - get_label_width(child));

	    n = 0;
	    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM);	n++;
	    XtSetArg(args[n], XmNleftOffset, offset);			n++;
	    XtSetArg(args[n], XmNrightAttachment, XmATTACH_NONE);	n++;

	    XtSetValues(child, args, n);
        }

	previous_ref_widget = ref_widget;
    }

    if (num_rows > 0)
        XtFree((char*)one_col);
}

static void
align_vcenter(
    Widget		parent,
    DtbGroupInfo	*group_info,
    Boolean		init
)
{
    WidgetList	children_list;
    Widget	child, 
		previous_child;
    DTB_GROUP_TYPES group_type;
    int		num_children = 0,
		num_columns,
		num_rows,
		cell_width,
		cell_height,
		group_width,
		group_height,
		offset,
		gridline,
		i,
		j;

    if (!parent || !group_info)
	return;

    /*
     * Get children list
     */
    XtVaGetValues(parent,
            XmNnumChildren, &num_children,
            XmNchildren, &children_list,
            NULL);

    if (num_children <= 0)
	return;
    
    get_group_cell_size(parent, group_info, &cell_width, &cell_height);
	 
    get_group_row_col(parent, group_info, &num_rows, &num_columns);
	      
    offset = group_info->hoffset;
		   
    group_type = group_info->group_type;

    if (group_type == DTB_GROUP_ROWSCOLUMNS)
    {
        group_width = (num_columns * cell_width) + ((num_columns-1) * offset);
    }

    for (i = 0; i < num_columns; i++)
    {
        if (group_type == DTB_GROUP_ROWSCOLUMNS)
	    gridline = (((i * (cell_width + offset)) + (cell_width/2)) * 100)/group_width;
	else
	    gridline = 50;

        for (j = 0; j < num_rows; j++)
        {
            Arg		args[12];
	    int		n = 0;
	    Dimension	width = 0;

	    child = get_group_child(parent, group_info, i, j);

	    if (!child)
		continue;

	    XtVaGetValues(child, XmNwidth, &width, NULL);

	    if (init)
	    {
		int	offset = 0;

		if (!XtIsSubclass(child, compositeWidgetClass))
		{
	            offset = (cell_width - (int)width)/2;

                    if (group_type == DTB_GROUP_ROWSCOLUMNS)
	                offset += (i * (cell_width + group_info->hoffset));
		}

	        XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM);	n++;
	        XtSetArg(args[n], XmNleftOffset, offset);		n++;
		XtSetArg(args[n], XmNrightAttachment, XmATTACH_NONE);	n++;
	    }
	    else
	    {
	        XtSetArg(args[n], XmNleftAttachment, 
				XmATTACH_POSITION);			n++;
	        XtSetArg(args[n], XmNleftPosition, gridline);		n++;
	        XtSetArg(args[n], XmNleftOffset, (int)(-(width/2)));	n++;
		XtSetArg(args[n], XmNrightAttachment, XmATTACH_NONE);	n++;
	    }

	    XtSetValues(child, args, n);
        }
    }
}

static void
align_top(
    Widget		parent,
    DtbGroupInfo	*group_info
)
{
    WidgetList	children_list;
    Widget	previous_child = NULL,
		child,
		cur_child;
    int		num_children = 0,
		num_columns,
		num_rows,
		cell_width,
		cell_height,
		i,
		j;

    if (!parent || !group_info)
	return;

    /*
     * Get children list
     */
    XtVaGetValues(parent,
            XmNnumChildren, &num_children,
            XmNchildren, &children_list,
            NULL);

    if (num_children <= 0)
	return;
    
    get_group_cell_size(parent, group_info, &cell_width, &cell_height);
    get_group_row_col(parent, group_info, &num_rows, &num_columns);

    for (j = 0; j < num_rows; j++)
    {
        for (i = 0; i < num_columns; i++)
        {
	    Arg args[12];
	    int n = 0;

	    child = get_group_child(parent, group_info, i, j);

	    if (!child)
		continue;

	    if ((i == 0) && (j == 0))
	    {
	        XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM);	n++;
	        XtSetArg(args[n], XmNtopOffset, 0);			n++;
	        XtSetArg(args[n], XmNbottomAttachment, XmATTACH_NONE);	n++;

	        XtSetValues(child, args, n);

		continue;
	    }

	    if (i == 0)
	    {
	        previous_child = get_group_child(parent, group_info, 0, j-1);

	        if (previous_child)
	        {
                    DTB_GROUP_TYPES	group_type = group_info->group_type;
	            int			offset = group_info->voffset;
    
                    if (group_type == DTB_GROUP_ROWSCOLUMNS)
		    {
			Dimension	height = 0;

			XtVaGetValues(previous_child, XmNheight, &height, NULL);

		        offset += (cell_height - (int)(height));
		    }

	            XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET);	n++;
	            XtSetArg(args[n], XmNtopWidget, previous_child);		n++;
	            XtSetArg(args[n], XmNtopOffset, offset);			n++;
	            XtSetArg(args[n], XmNbottomAttachment, XmATTACH_NONE);	n++;

	            XtSetValues(child, args, n);
	        }
		continue;
	    }

	    previous_child = get_group_child(parent, group_info, i-1, j);

	    if (previous_child)
	    {
	        XtSetArg(args[n], XmNtopAttachment, XmATTACH_OPPOSITE_WIDGET);	n++;
	        XtSetArg(args[n], XmNtopWidget, previous_child);		n++;
	        XtSetArg(args[n], XmNtopOffset, 0);				n++;
	        XtSetArg(args[n], XmNbottomAttachment, XmATTACH_NONE);		n++;

	        XtSetValues(child, args, n);
	    }

        }
    }
}

static void
align_bottom(
    Widget		parent,
    DtbGroupInfo	*group_info
)
{
    WidgetList	children_list;
    Widget	child, 
		previous_child;
    DTB_GROUP_TYPES group_type;
    int		num_children = 0,
		num_columns,
		num_rows,
		cell_height,
		cell_width,
		offset,
		i,
		j;
  
    if (!parent || !group_info)
	return;

    /*
     * Get children list
     */
    XtVaGetValues(parent,
            XmNnumChildren, &num_children,
            XmNchildren, &children_list,
            NULL);

    if (num_children <= 0)
	return;
    
    get_group_cell_size(parent, group_info, &cell_width, &cell_height);
    get_group_row_col(parent, group_info, &num_rows, &num_columns);

    for (j = 0; j < num_rows; j++)
    {
        for (i = 0; i < num_columns; i++)
        {
            Arg		args[12];
	    int		n = 0;

	    child = get_group_child(parent, group_info, i, j);

	    if (!child)
		continue;

	    if ((i == 0) && (j == 0))
	    {
		Dimension	height = 0;

		XtVaGetValues(child, XmNheight, &height, NULL);

		offset = cell_height - height;

	        XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM);	n++;
	        XtSetArg(args[n], XmNtopOffset, offset);		n++;
	        XtSetArg(args[n], XmNbottomAttachment, XmATTACH_NONE);	n++;

	        XtSetValues(child, args, n);

		continue;
	    }

	    if (i == 0)
	    {
	        previous_child = get_group_child(parent, group_info, 0, j-1);

	        if (previous_child)
	        {
		    Dimension	height = 0;

		    XtVaGetValues(child, XmNheight, &height, NULL);

	            offset = group_info->voffset;

                    if (group_info->group_type == DTB_GROUP_ROWSCOLUMNS)
		        offset += (cell_height - height);

	            XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET);	n++;
	            XtSetArg(args[n], XmNtopWidget, previous_child);		n++;
	            XtSetArg(args[n], XmNtopOffset, offset);			n++;
	            XtSetArg(args[n], XmNbottomAttachment, XmATTACH_NONE);	n++;

	            XtSetValues(child, args, n);
	        }
		continue;
	    }

	    previous_child = get_group_child(parent, group_info, i-1, j);

	    if (child && previous_child)
	    {
	        XtSetArg(args[n], XmNbottomAttachment, 
				XmATTACH_OPPOSITE_WIDGET);		n++;
	        XtSetArg(args[n], XmNbottomWidget, previous_child);	n++;
	        XtSetArg(args[n], XmNbottomOffset, 0);			n++;
	        XtSetArg(args[n], XmNtopAttachment, XmATTACH_NONE);	n++;

	        XtSetValues(child, args, n);
	    }
        }
    }
}

static void
align_hcenter(
    Widget		parent,
    DtbGroupInfo	*group_info,
    Boolean		init
)
{
    WidgetList	children_list = NULL;
    Widget	child, 
		previous_child;
    DTB_GROUP_TYPES group_type;
    int		num_children = 0,
		num_columns,
		num_rows,
		cell_width,
		cell_height,
		group_width,
		group_height,
		offset,
		gridline,
		i,
		j;
  
    if (!parent || !group_info)
	return;

    /*
     * Get children list
     */
    XtVaGetValues(parent,
            XmNnumChildren, &num_children,
            XmNchildren, &children_list,
            NULL);

    if (num_children <= 0)
	return;
    
    group_type = group_info->group_type;

    get_group_cell_size(parent, group_info, &cell_width, &cell_height);
    get_group_row_col(parent, group_info, &num_rows, &num_columns);

    offset = group_info->voffset;

    if (group_type == DTB_GROUP_ROWSCOLUMNS)
    {
        group_height = (num_rows * cell_height) + ((num_rows-1) * offset);
    }

    for (j = 0; j < num_rows; j++)
    {
        if (group_type == DTB_GROUP_ROWSCOLUMNS)
	    gridline = 
		(((j * (cell_height + offset)) + (cell_height/2)) * 100)/group_height;
	else
	    gridline = 50;

        for (i = 0; i < num_columns; i++)
        {
            Arg		args[12];
	    int		n = 0;
	    Dimension	height = 0;

	    child = get_group_child(parent, group_info, i, j);

	    if (!child)
		continue;

	    XtVaGetValues(child, XmNheight, &height, NULL);

	    if (init)
	    {
		int	init_offset = 0;

                if (!XtIsSubclass(child, compositeWidgetClass))
		{
		    init_offset = (cell_height - (int)height)/2;

		    if (group_type == DTB_GROUP_ROWSCOLUMNS)
		    init_offset += (j * (cell_height + group_info->voffset));
		}

	        XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM);	n++;
	        XtSetArg(args[n], XmNtopOffset, init_offset);		n++;
	        XtSetArg(args[n], XmNbottomAttachment, XmATTACH_NONE);	n++;
	    }
	    else
	    {
	        XtSetArg(args[n], XmNtopAttachment, 
				XmATTACH_POSITION);			n++;
	        XtSetArg(args[n], XmNtopPosition, gridline);		n++;
	        XtSetArg(args[n], XmNtopOffset, (int)(-(height/2)));	n++;
	        XtSetArg(args[n], XmNbottomAttachment, XmATTACH_NONE);	n++;
	    }

	    XtSetValues(child, args, n);
        }
    }
}


/*************************************************************************
 *									**
 *  Determine the directory the executable for this process was		**
 *  loaded from. This is used find data files associated with the 	**
 *  executable.								**
 *									**
 *************************************************************************/

/*
 * Returns the directory that the executable for this process was loaded 
 * from.
 */
String
dtb_get_exe_dir(void)
{
    return dtb_exe_dir;
}


/*
 * Determines the directory the executable for this process was loaded from.
 */
static int 
determine_exe_dir(
    char 	*argv0,
    char 	*buf,
    int 	bufSize
)
{
    Boolean	foundDir= False;

    if ((buf == NULL) || (bufSize < 1))
    {
	return -1;
    }
    
    if (determine_exe_dir_from_argv(argv0, buf, bufSize) >= 0)
    {
	foundDir = True;
    }

    if (!foundDir)
    {
	if (determine_exe_dir_from_path(argv0, buf, bufSize) >= 0)
	{
	    foundDir = True;
	}
    }

    /*
     * Convert relative path to absolute, so that directory changes will
     * not affect us.
     */
    if (foundDir && (buf[0] != '/'))
    {
	char	cwd[MAXPATHLEN+1];
	char	*env_pwd = NULL;
	char	*path_prefix = NULL;
	char	abs_exe_dir[MAXPATHLEN+1];

	if (getcwd(cwd, MAXPATHLEN+1) != NULL)
	{
	    path_prefix = cwd;
	}
	else
	{
	    env_pwd = getenv("PWD");
	    if (env_pwd != NULL)
	    {
		path_prefix = env_pwd;
	    }
	}

	if (path_prefix != NULL)
	{
	    strcpy(abs_exe_dir, path_prefix);
	    if (strcmp(buf, ".") != 0)
	    {
		strcat(abs_exe_dir, "/");
		strcat(abs_exe_dir, buf);
	    }
	    strcpy(buf, abs_exe_dir);
	}
    }

    return foundDir? 0:-1;
}



/*
 *  Looks for absolute path in arv[0].
 */
static int
determine_exe_dir_from_argv(
    char 	*argv0,
    char 	*buf,
    int 	bufSize
)
{
    int		i= 0;
    Boolean	foundit= False;

    for (i= strlen(argv0)-1; (i >= 0) && (argv0[i] != '/'); --i)
    {
    }

    if (i >= 0)
    {
	int	maxStringSize= min(i, bufSize);
	strncpy(buf, argv0, maxStringSize);
	buf[min(maxStringSize, bufSize-1)]= 0;
	foundit = True;
    }

    return foundit? 0:-1;
}


/*
 * Assumes: bufSize > 0
 */
static int
determine_exe_dir_from_path (
    char 	*argv0,
    char 	*buf,
    int 	bufSize
)
{
    Boolean	foundDir= False;
    Boolean	moreDirs= True;
    char	*szExeName= argv0;
    int		iExeNameLen= strlen(szExeName);
    char	*szTemp= NULL;
    char	szPathVar[MAXPATHLEN+1];
    int		iPathVarLen= 0;
    char	szCurrentPath[MAXPATHLEN+1];
    int		iCurrentPathLen= 0;
    int		iCurrentPathStart= 0;
    int		i = 0;
    uid_t	euid= geteuid();
    uid_t	egid= getegid();

    szTemp= getenv("PATH");
    if (szTemp == NULL)
    {
	moreDirs= False;
    }
    else
    {
	strncpy(szPathVar, szTemp, MAXPATHLEN);
	szPathVar[MAXPATHLEN]= 0;
	iPathVarLen= strlen(szPathVar);
    }

    while ((!foundDir) && (moreDirs))
    {
	/* find the current directory name */
	for (i= iCurrentPathStart; (i < iPathVarLen) && (szPathVar[i] != ':'); 
	    )
    	{
	    ++i;
	}
	iCurrentPathLen= i - iCurrentPathStart;
	if ((iCurrentPathLen + iExeNameLen + 2) > MAXPATHLEN)
	{
	    iCurrentPathLen= MAXPATHLEN - (iExeNameLen + 2);
	}

	/* create a possible path to the executable */
	strncpy(szCurrentPath, &szPathVar[iCurrentPathStart], iCurrentPathLen);
	szCurrentPath[iCurrentPathLen]= 0;
	strcat(szCurrentPath, "/");
	strcat(szCurrentPath, szExeName);

	/* see if the executable exists (and we can execute it) */
	if (path_is_executable(szCurrentPath, euid, egid))
	{
	    foundDir= True;
	}

	/* skip past the current directory name */
	if (!foundDir)
	{
	    iCurrentPathStart+= iCurrentPathLen;
	    while (   (iCurrentPathStart < iPathVarLen) 
		   && (szPathVar[iCurrentPathStart] != ':') )
	    {
		++iCurrentPathStart;	/* find : */
	    }
	    if (iCurrentPathStart < iPathVarLen) 
	    {
		++iCurrentPathStart;	/* skip : */
	    }
	    if (iCurrentPathStart >= iPathVarLen)
	    {
		moreDirs= False;
	    }
	}
    } /* while !foundDir */

    if (foundDir)
    {
	szCurrentPath[iCurrentPathLen]= 0;
	strncpy(buf, szCurrentPath, bufSize);
	buf[bufSize-1]= 0;
    }
    return foundDir? 0:-1;
} /* determine_exe_dir_from_path */


/*
 * returns False is path does not exist or is not executable
 */
static Boolean
path_is_executable(
    char 	*path,
    uid_t	euid,
    gid_t 	egid
)
{
    Boolean	bExecutable= False;
    struct stat	sStat;

    if (stat(path, &sStat) == 0)
    {
	Boolean	bDetermined= False;

	if (!bDetermined)
	{
	    if (!S_ISREG(sStat.st_mode))
	    {
		/* not a regular file */
		bDetermined= True;
		bExecutable= False;
	    }
	}

	if (!bDetermined)
	{
	    if (   (euid == 0) 
	        && (   ((sStat.st_mode & S_IXOTH) != 0)
		    || ((sStat.st_mode & S_IXGRP) != 0)
		    || ((sStat.st_mode & S_IXUSR) != 0) )
	       )
	    {
		bDetermined= True;
		bExecutable= True;
	    }
	}

	if (!bDetermined)
	{
	    if (   (((sStat.st_mode & S_IXOTH) != 0)    )
		|| (((sStat.st_mode & S_IXGRP) != 0) && (sStat.st_gid == egid))
		|| (((sStat.st_mode & S_IXUSR) != 0) && (sStat.st_gid == euid))
	       )
	    {
		bDetermined= True;
	        bExecutable= True;
	    }
	}
    } /* if stat */

    return bExecutable;
} /* path is executable */

/*
 * dtb_set_tt_msg_quitCB()
 */
void
dtb_set_tt_msg_quitCB(
    DtbTTMsgHandlerCB 	msg_quitCB
)
{
    dtb_tt_msg_quitCB = msg_quitCB;
}

/*
 * dtb_get_tt_msg_quitCB()
 */
static DtbTTMsgHandlerCB
dtb_get_tt_msg_quitCB()
{
    return(dtb_tt_msg_quitCB);
}

/*
 * dtb_tt_msg_quit()
 * Handle the ToolTalk Desktop "Quit" message.
 * Call Application-defined callback if it exists.
 */
static Boolean
dtb_tt_msg_quit(
    Tt_message	msg
)
{
    DtbTTMsgHandlerCB	msg_quitCB = NULL;
    Boolean		handled = True;

    msg_quitCB = dtb_get_tt_msg_quitCB();
    if (msg_quitCB)
	handled = msg_quitCB(msg, (void *)NULL);

    return handled;
}

/*
 * dtb_set_tt_msg_do_commandCB()
 */
void
dtb_set_tt_msg_do_commandCB(
    DtbTTMsgHandlerCB 	msg_do_commandCB
)
{
    dtb_tt_msg_do_commandCB = msg_do_commandCB;
}

/* 
 * dtb_get_tt_msg_do_commandCB()
 */ 
static DtbTTMsgHandlerCB
dtb_get_tt_msg_do_commandCB()
{ 
    return(dtb_tt_msg_do_commandCB);
}
 
/* 
 * dtb_tt_msg_do_command()
 * Handle the ToolTalk Desktop "Do Command" message. 
 * Call Application-defined callback if it exists. 
 */ 
static Boolean
dtb_tt_msg_do_command(
    Tt_message	msg
)
{
    DtbTTMsgHandlerCB 	msg_do_commandCB = NULL;
    Boolean             handled = True;
 
    msg_do_commandCB = dtb_get_tt_msg_do_commandCB();
    if (msg_do_commandCB)
        handled = msg_do_commandCB(msg, (void *)NULL);
 
    return handled;
}

/*
 * dtb_set_tt_msg_get_statusCB()
 */
void
dtb_set_tt_msg_get_statusCB(
    DtbTTMsgHandlerCB 	msg_get_statusCB
)
{
    dtb_tt_msg_get_statusCB = msg_get_statusCB;
}

/* 
 * dtb_get_tt_msg_get_statusCB()
 */ 
static DtbTTMsgHandlerCB
dtb_get_tt_msg_get_statusCB()
{ 
    return(dtb_tt_msg_get_statusCB);
}
 
/* 
 * dtb_tt_msg_get_status()
 * Handle the ToolTalk Desktop "Get Status" message. 
 * Call Application-defined callback if it exists. 
 */ 
static Boolean
dtb_tt_msg_get_status(
    Tt_message  msg
)
{
    DtbTTMsgHandlerCB 	msg_get_statusCB = NULL;
    Boolean             handled = True;

    msg_get_statusCB = dtb_get_tt_msg_get_statusCB();
    if (msg_get_statusCB)
        handled = msg_get_statusCB(msg, (void *)NULL);

    return handled;
}

/*
 * dtb_set_tt_msg_pause_resumeCB()
 */
void
dtb_set_tt_msg_pause_resumeCB(
    DtbTTMsgHandlerCB 	msg_pause_resumeCB
)
{
    dtb_tt_msg_pause_resumeCB = msg_pause_resumeCB;
}

/* 
 * dtb_get_tt_msg_pause_resumeCB()
 */ 
static DtbTTMsgHandlerCB
dtb_get_tt_msg_pause_resumeCB()
{ 
    return(dtb_tt_msg_pause_resumeCB);
}
 
/* 
 * dtb_tt_msg_pause_resume()
 * Handle the ToolTalk Desktop "Pause/Resume" messages. 
 * Call Application-defined callback if it exists. 
 */ 
static Boolean
dtb_tt_msg_pause_resume(
    Tt_message	msg,
    Boolean	sensitive
)
{
    DtbTTMsgHandlerCB 	msg_pause_resumeCB = NULL;
    Boolean             handled = True;

    msg_pause_resumeCB = dtb_get_tt_msg_pause_resumeCB();
    if (msg_pause_resumeCB)
        handled = msg_pause_resumeCB(msg, (void *)sensitive);

    return handled;
}

/* 
 * dtb_tt_contractCB()
 * Receives any ToolTalk Desktop messages sent to application
 * and dispatches them to the appropriate Application-defined callbacks
 * if they exist.
 */ 
Tt_message
dtb_tt_contractCB(
    Tt_message	msg,
    void	*client_data,
    Tt_message	contract
)
{        
    char	*op_string;
    Tttk_op	op;
    Boolean	sensitive = True;

    if (contract != 0) 
    {
        tttk_message_fail( msg, TT_DESKTOP_ENOMSG, 0, 1 );
        return 0;
    }
    op_string = tt_message_op(msg);
    op = tttk_string_op(op_string);
    tt_free(op_string);

    switch(op)
    {
	case TTDT_QUIT:
	    if (dtb_tt_msg_quit(msg) == True)
	        return 0;
	    break;

	case TTDT_GET_STATUS:
	    if (dtb_tt_msg_get_status(msg) == True)
                return 0; 
	    break;

        case TTDT_PAUSE:
            sensitive = False;
        case TTDT_RESUME:
	    if (dtb_tt_msg_pause_resume(msg, sensitive) == True)
	        return 0;
	    break;

	case TTDT_DO_COMMAND:
	    if (dtb_tt_msg_do_command(msg) == True)
	        return 0;
	    break;
	}
	return msg;

}

/*
 * dtb_tt_close()
 * Close ToolTalk Connection
 */
void
dtb_tt_close()
{
    ttdt_close(NULL, NULL, True);
}

/* 
** dtb_call_help_callback()
** Utility routine to call the help callbacks on a target widget.  This
** is predominantly used to display help data on a dialog by having this
** function as the activate callback on the dialog's help button.
*/
void 
dtb_call_help_callback(
    Widget widget,
    XtPointer clientData,
    XtPointer callData
)
{
	Widget target = (Widget)clientData;

	XtCallCallbacks(target,XmNhelpCallback,(XtPointer)NULL);
}

static void     dtb_drag_button1_motion_handler(
                        Widget          dragInitiator,
                        XtPointer       clientData,
                        XEvent         *event
		);
static int	dtb_drag_start(
			DtbDragSite dragSite, 
			XEvent *event
		);
static void	dtb_drag_convertCB(
			Widget		dragContext,
			XtPointer	clientData,
			XtPointer	callData
		);
static void	dtb_drag_to_rootCB(
			Widget		dragContext,
			XtPointer	clientData,
			XtPointer	callData
		);
static void	dtb_drag_finishCB(
			Widget		dragContext,
			XtPointer	clientData,
			XtPointer	callData
		);

static int	dtb_drag_terminate(DtbDragSite dragSite);


/*
 * Registers a popup menu to be brought by button three on the parent
 */
int 
dtb_popup_menu_register(Widget popupMenu, Widget parent)
{
    static XtTranslations	popupMenuTrans = NULL;
    static XtActionsRec		menuActions[] =
    {
	{"DtbPopupMenu", (XtActionProc)dtb_popup_menu }
    };
    int		i = 0;
    Boolean	foundEntry = False;

    if (popupMenuTrans == NULL)
    {
	XtAppContext	appContext = 
		XtWidgetToApplicationContext(dtb_get_toplevel_widget());
	XtAppAddActions(appContext, menuActions, XtNumber(menuActions));
	popupMenuTrans = XtParseTranslationTable(
				"<Btn3Down>: DtbPopupMenu()");

    }
    XtOverrideTranslations(parent, popupMenuTrans);

    /*
     * Save the reference from this widget to the menu
     */
    /* see if an entry already exists */
    for (i = 0; i < numPopupMenus; ++i)
    {
	if (   (popupMenus[i].widget == parent)
	    || (popupMenus[i].menu == popupMenu) )
	{
	    foundEntry = True;
	    break;
	}
    }
    /* look for an empty slot */
    if (!foundEntry)
    {
	/* look for an empty slot */
	for (i = 0; i < numPopupMenus; ++i)
	{
	    if (popupMenus[i].widget == NULL)
	    {
		foundEntry = True;
		break;
	    }
	}
    }
    /* make a new slot, if necessary */
    if (!foundEntry)
    {
	/* It's not in the list - add it */
	DtbMenuRef newPopupMenus = (DtbMenuRef)
		realloc(popupMenus, sizeof(DtbMenuRefRec)*(numPopupMenus+1));
	if (newPopupMenus != NULL)
	{
	    popupMenus = newPopupMenus;
	    ++numPopupMenus;
	    i = numPopupMenus-1;
	    foundEntry = True;
	}
    }

    /* we have a slot; fill it in */
    if (foundEntry)
    {
        popupMenus[i].widget = parent;
        popupMenus[i].menu = popupMenu;
	XtAddCallback(popupMenus[i].widget,
	    XmNdestroyCallback, dtb_popup_menu_destroyCB, (XtPointer)NULL);
	XtAddCallback(popupMenus[i].menu,
	    XmNdestroyCallback, dtb_popup_menu_destroyCB, (XtPointer)NULL);
    }

    return 0;
}


static void
dtb_popup_menu(
    Widget	widget,
    XEvent	*event,
    String	*params,
    Cardinal	*num_params
)
{
    int		i = 0;
    Widget	menu = NULL;

    if (event->type == ButtonPress)
    {
        for (i = 0 ; i < numPopupMenus; ++i)
        {
	    if (popupMenus[i].widget == widget)
	    {
		menu = popupMenus[i].menu;
	    }
	}
    }

    if (menu != NULL)
    {
	XmMenuPosition(menu, (XButtonPressedEvent*)event);
	XtManageChild(menu);
    }
}


/*
 * This keeps the list of popup menus up-to-date, if widgets are destroyed
 */
static void 
dtb_popup_menu_destroyCB(
			Widget		widget,
			XtPointer	clientData,
			XtPointer	callData
)
{
    int		i = 0;
    for (i = 0; i < numPopupMenus; ++i)
    {
	if (   (popupMenus[i].widget == widget) 
	    || (popupMenus[i].menu == widget) )
	{
	    popupMenus[i].widget = NULL;
	    popupMenus[i].menu = NULL;
	    break;
	}
    }
}


/*
 * Returns non-negative if successful.
 */
int
dtb_drag_site_register(
                        Widget 			widget,
			DtbDndDragCallback	callback,
			DtDndProtocol		protocol,
			unsigned char		operations,
			Boolean			bufferIsText,
			Boolean			allowDropOnRootWindow,
			Pixmap			cursor,
			Pixmap			cursorMask,
			DtbDragSiteHandle	*dragSiteHandleOut
)
{
    DtbDragSite dragSite = (DtbDragSite)XtCalloc(1,sizeof(DtbDragSiteRec));
    Widget	sourceIcon = NULL;

    if (dragSite != NULL)
    {
        dragSite->widget = widget;
    	dragSite->protocol = protocol;
    	dragSite->operations = operations;
	dragSite->bufferIsText = bufferIsText;
	dragSite->allowDropOnRootWindow = allowDropOnRootWindow;
	if ((cursor != NULL) && (cursorMask != NULL))
	{
	    dragSite->sourceIcon = 
			DtDndCreateSourceIcon(widget, cursor, cursorMask);
	}
    	dragSite->callback = callback;
        dragSite->convertCBRec[0].callback = dtb_drag_convertCB;
        dragSite->dragToRootCBRec[0].callback = dtb_drag_to_rootCB;
        dragSite->dragFinishCBRec[0].callback = dtb_drag_finishCB;
    
        XtAddEventHandler(widget, Button1MotionMask, False,
            (XtEventHandler)dtb_drag_button1_motion_handler, 
	    (XtPointer)dragSite);
    }
    
    /*
     * Pass back a handle, so that this can be freed, later. Unregistering
     * drag sites is not currently implemented, but this gives the ability
     * to provide this functionality in the future.
     */
    if (dragSiteHandleOut != NULL)
    {
        *dragSiteHandleOut = (DtbDragSiteHandle)dragSite;
    }
    return 0;
}


int
dtb_drop_site_register(
                        Widget                  widget,
                        DtbDndDropCallback      callback,
                        DtDndProtocol		protocols,
                        unsigned char           operations,
                        Boolean                 dropsOnChildren,
                        Boolean                 preservePreviousRegistration,
                        DtbDropSiteHandle       *dropSiteHandleOut
)
{
    DtbDropSite dropSite = (DtbDropSite)NULL;

    if (   (callback != NULL)
	&& ((dropSite = (DtbDropSite)XtCalloc(1,sizeof(DtbDropSiteRec))) 
								!= NULL) 
       )
    {
	DtbDndDropRegisterInfoRec	regInfo;

	/* initialize the data */
        dropSite->widget = widget;
        dropSite->callback = callback;
        dropSite->protocols = protocols;
        dropSite->operations = operations;
        dropSite->dropsOnChildren = dropsOnChildren;
        dropSite->preservePreviousRegistration = preservePreviousRegistration;
        dropSite->animateCBRec[0].callback = dtb_drop_animateCB;
        dropSite->animateCBRec[0].closure = (XtPointer)dropSite;
        dropSite->transferCBRec[0].callback = dtb_drop_transferCB;
        dropSite->transferCBRec[0].closure = (XtPointer)dropSite;

        /* Let the client modify the drop site initialization */
        regInfo.protocols = dropSite->protocols;
        regInfo.operations = dropSite->operations;
        regInfo.textIsBuffer = dropSite->textIsBuffer;
        regInfo.preservePreviousRegistration = 
				dropSite->preservePreviousRegistration;
        regInfo.respondToDropsOnChildren = dropSite->dropsOnChildren;
        dropSite->callback(widget, DTB_DND_REGISTER, &regInfo, NULL, NULL);

        /* actually register it! */
        DtDndVaDropRegister(
	    widget, regInfo.protocols, regInfo.operations,
	    dropSite->transferCBRec,
	    DtNregisterChildren, regInfo.respondToDropsOnChildren,
	    DtNtextIsBuffer, regInfo.textIsBuffer,
#ifdef DtNpreserveRegistration
	    DtNpreserveRegistration, regInfo.preservePreviousRegistration,
#endif
	    DtNdropAnimateCallback, dropSite->animateCBRec,
	    NULL);
    }

    if (dropSiteHandleOut != NULL)
    {
	*dropSiteHandleOut = (DtbDropSiteHandle)dropSite;
    }

    return ((dropSite == NULL)? -1:0);
}



static int
dtb_drag_terminate(DtbDragSite dragSite)
{
    dragInProgress = False;
    dragInitialX = -1;
    dragInitialY = -1;
    dragSite->convertCBRec[0].closure = NULL;
    dragSite->dragToRootCBRec[0].closure = NULL;
    dragSite->dragFinishCBRec[0].closure = NULL;
    return 0;
}


/*
 * dragMotionHandler
 *
 * Determine if the pointer has moved beyond the drag threshold while button 1
 * was being held down.
 */
static void
dtb_drag_button1_motion_handler(
        Widget          dragInitiator,
        XtPointer       clientData,
        XEvent         *event
)
{
    int             	diffX, diffY;
    DtbDragSite		dragSite = (DtbDragSite)clientData;

    if (!dragInProgress) {
        /*
         * If the drag is just starting, set initial button down coords
         */
        if (dragInitialX == -1 && dragInitialY == -1) {
                dragInitialX = event->xmotion.x;
                dragInitialY = event->xmotion.y;
        }

        /*
         * Find out how far pointer has moved since button press
         */
        diffX = dragInitialX - event->xmotion.x;
        diffY = dragInitialY - event->xmotion.y;
        
        if ((ABS(diffX) >= DRAG_THRESHOLD) ||
            (ABS(diffY) >= DRAG_THRESHOLD)) 
	{
		dtb_drag_start(dragSite, event);
                dragInitialX = -1;
                dragInitialY = -1;
        }
    }
}


static int
dtb_drag_start(DtbDragSite dragSite, XEvent *event)
{
    DtbDndDragStartInfoRec	dragStart;
    Arg				args[3];
    int				n = 0;

    dragInProgress = True;

    dragSite->convertCBRec[0].closure = (XtPointer)dragSite;
    dragSite->dragToRootCBRec[0].closure = (XtPointer)dragSite;
    dragSite->dragFinishCBRec[0].closure = (XtPointer)dragSite;

    /*
     * Call the client callback to update values for drag start
     */
    memset((void*)&dragStart, 0, sizeof(DtbDndDragStartInfoRec));
    dragStart.protocol= dragSite->protocol;
    dragStart.operations = dragSite->operations;
    dragStart.cursor = dragSite->sourceIcon;
    dragStart.bufferIsText = dragSite->bufferIsText;
    dragStart.allowDropOnRootWindow = dragSite->allowDropOnRootWindow;
    dragStart.numItems = 1;
    if (dragSite->callback != NULL)
    {
        dragSite->callback(DTB_DND_DRAG_START, &dragStart, NULL,NULL,NULL,NULL);
    }

    n = 0;
    if (dragStart.cursor != NULL)
    {
	XtSetArg(args[n], DtNsourceIcon, dragStart.cursor); ++n;
    }
    if (   (dragStart.bufferIsText) 
	&& (dragStart.protocol == DtDND_BUFFER_TRANSFER) )
    {
	XtSetArg(args[n], DtNbufferIsText, True); ++n;
    }
    if (dragStart.allowDropOnRootWindow)
    {
	XtSetArg(args[n], DtNdropOnRootCallback, (XtPointer)dragSite->dragToRootCBRec); ++n;
    }
    if (DtDndDragStart(dragSite->widget, event, 
	dragStart.protocol,
	dragStart.numItems,
	dragStart.operations,
	dragSite->convertCBRec, dragSite->dragFinishCBRec,
	args, n) == NULL)
    {
	/* drag start failed */
	dtb_drag_terminate(dragSite);
	return -1;
    }
    return 0;
}


static void
dtb_drag_convertCB(
			Widget		dragContext,
			XtPointer	clientData,
			XtPointer	callData
)
{
    DtbDragSite			dragSite = (DtbDragSite)clientData;
    DtDndConvertCallback	convert = (DtDndConvertCallback)callData;
    DtDndContext		*dragData = convert->dragData;
    int				i = 0;

    switch (convert->reason)
    {
	case DtCR_DND_CONVERT_DATA:
	    for (i = 0; i < dragData->numItems; ++i)
	    {
		switch (dragData->protocol)
		{
		    case DtDND_BUFFER_TRANSFER:
			dragData->data.buffers[i].bp = NULL;
			dragData->data.buffers[i].size = 0;
		    break;
		    case DtDND_FILENAME_TRANSFER:
			dragData->data.files[i] = "/etc/passwd";
		    break;
		    case DtDND_TEXT_TRANSFER:
			dragData->data.strings[i] = XmStringCreateLocalized("Hello, world!\n");
		    break;
		}
	    }
	    dragSite->callback(DTB_DND_CONVERT,NULL,convert,NULL,NULL,NULL);
	break;

	case DtCR_DND_CONVERT_DELETE:
	    dragSite->callback(DTB_DND_DELETE,NULL,NULL,NULL,convert,NULL);
	break;
    }
}


static void
dtb_drag_to_rootCB(
			Widget		dragContext,
			XtPointer	clientData,
			XtPointer	callData
)
{
    DtbDragSite			dragSite = (DtbDragSite)clientData;
    DtbDndDroppedOnRootWindowInfoRec	dropInfo;
    memset(&dropInfo, 0, sizeof(DtbDndDroppedOnRootWindowInfoRec));

    dropInfo.droppedOnRootWindow = True;
    dragSite->callback(DTB_DND_DROPPED_ON_ROOT_WINDOW,
			NULL,NULL,&dropInfo,NULL,NULL);
}


static void
dtb_drag_finishCB(
			Widget		dragContext,
			XtPointer	clientData,
			XtPointer	callData
)
{
    DtbDragSite			dragSite = (DtbDragSite)clientData;
    DtDndDragFinishCallback	finish = (DtDndDragFinishCallback)callData;

    dragSite->callback(DTB_DND_FINISH,NULL,NULL,NULL,NULL,finish);
    dtb_drag_terminate((DtbDragSite)clientData);
}


void 
dtb_default_dragCB(
			DTB_DND_REQUEST			request,
			DtbDndDragStartInfo		dragStart,
			DtDndConvertCallback		convert,
			DtbDndDroppedOnRootWindowInfo	dropOnRootWindow,
			DtDndConvertCallback		deleteSource,
			DtDndDragFinishCallback		finish
)
{
    /*** DTB_USER_CODE_START vvv Add C variables and code below vvv ***/
    /*** DTB_USER_CODE_END   ^^^ Add C variables and code above ^^^ ***/

    switch (request)
    {
	case DTB_DND_DRAG_START:
        /** DTB_USER_CODE_START 
	 **
	 ** The drag will start normally, using the defaults for this
	 ** drag site, and one item to transfer assumed.
	 **
	 ** To override the default protocol, operations, cursor, or 
	 ** number of of items to transfer, add code after this comment.
	 **/
        printf("action: dtb_default_dragCB(DTB_DND_DRAG_START)\n");
        /*** DTB_USER_CODE_END   ^^^ Add C code above ^^^ ***/
	break;

	case DTB_DND_CONVERT:
        /** DTB_USER_CODE_START
	 **
	 ** Here, the data must be supplied to the dropped-on object.
	 **/
        printf("action: dtb_default_dragCB(DTB_DND_CONVERT)\n");
        /*** DTB_USER_CODE_END   ^^^ Add C code above ^^^ ***/
	break;

	case DTB_DND_DELETE:
        /** DTB_USER_CODE_START
	 **
	 ** A move has been completed. Add code to delete the source
	 ** object, after this comment.
	 **/
        printf("action: dtb_default_dragCB(DTB_DND_DELETE)\n");
        /*** DTB_USER_CODE_END   ^^^ Add C code above ^^^ ***/
	break;

	case DTB_DND_DROPPED_ON_ROOT_WINDOW:
        /** DTB_USER_CODE_START
	 **
	 ** The dragged data was dropped on the root window.
	 **/
        printf("action: dtb_default_dragCB(DTB_DND_DROPPED_ON_ROOT_WINDOW)\n");
        /*** DTB_USER_CODE_END   ^^^ Add C code above ^^^ ***/
	break;

	case DTB_DND_FINISH:
        /** DTB_USER_CODE_START
	 **
	 ** The drag and drop operation has completed (successfully or
	 ** unsuccessfully). Clean up and allocated memory or other
	 ** resources allocated during this drag.
	 **/
        printf("action: dtb_default_dragCB(DTB_DND_FINISH)\n");
        /*** DTB_USER_CODE_END   ^^^ Add C code above ^^^ ***/
	break;
    }
    
    /*** DTB_USER_CODE_START vvv Add C code below vvv ***/
    /*** DTB_USER_CODE_END   ^^^ Add C code above ^^^ ***/
}


/*
 * NOTE: THE PARAMETERS OF TYPE *Callback ARE NOT CALLBACKS! THEY ARE
 *       STANDARD DATA STRUCTURES.
 */
void 
dtb_default_dropCB(
    Widget			widget,
    DTB_DND_REQUEST		request,
    DtbDndDropRegisterInfo	registerInfo,
    DtDndTransferCallback	receiveInfo,
    DtDndDropAnimateCallback	animateInfo
)
{
    /*** DTB_USER_CODE_START vvv Add C variables and code below vvv ***/
    /*** DTB_USER_CODE_END   ^^^ Add C variables and code above ^^^ ***/

    /*
     * The cases in this switch statement have been written in the order
     * that these requests will normally be received.
     */
    switch (request)
    {
	case DTB_DND_REGISTER:
        /** DTB_USER_CODE_START 
	 **
	 ** The drag site will be automatically be registered with the 
	 ** options chosen in the Application Builder, with default values
	 ** for any implementation-specific options.
	 **
	 ** To override the options chosen for this drop site, or to 
	 ** affect implementation-specific options not available from
	 ** the Application Builder, add code after this comment.
	 **/
        printf("action: dtb_default_dropCB(DTB_DND_REGISTER)\n");
        /*** DTB_USER_CODE_END   ^^^ Add C code above ^^^ ***/
	break;

	case DTB_DND_RECEIVE_DATA:
        /** DTB_USER_CODE_START
	 **
	 ** The data has been supplied by the dragged-from object. The
	 ** dragged data will be freed after this function returns, so
	 ** any data needed in the future must be copied.
	 **/
        printf("action: dtb_default_dropCB(DTB_DND_RECEIVE_DATA)\n");
        /*** DTB_USER_CODE_END   ^^^ Add C code above ^^^ ***/
	break;

	case DTB_DND_ANIMATE:
        /** DTB_USER_CODE_START
	 **
	 ** Give visual feedback on the acceptability of the provided
	 ** data.
	 **/
        printf("action: dtb_default_dropCB(DTB_DND_ANIMATE)\n");
        /*** DTB_USER_CODE_END   ^^^ Add C code above ^^^ ***/
	break;
    }
    
    /*** DTB_USER_CODE_START vvv Add C code below vvv ***/
    /*** DTB_USER_CODE_END   ^^^ Add C code above ^^^ ***/
}


static void
dtb_drop_animateCB(
			Widget		dragContext,
			XtPointer	clientData,
			XtPointer	callData
)
{
    DtbDropSite		dropSite = (DtbDropSite)clientData;
    DtDndDropAnimateCallback 	animateInfo = 
					(DtDndDropAnimateCallback)callData;
    dropSite->callback(
	dropSite->widget, DTB_DND_ANIMATE, NULL, NULL, animateInfo);
}


static void
dtb_drop_transferCB(
			Widget		dragContext,
			XtPointer	clientData,
			XtPointer	callData
)
{
    DtbDropSite			dropSite = (DtbDropSite)clientData;
    DtDndTransferCallback	transferInfo = (DtDndTransferCallback)callData;

    dropSite->callback(
	dropSite->widget, DTB_DND_RECEIVE_DATA, NULL, transferInfo, NULL);
}

