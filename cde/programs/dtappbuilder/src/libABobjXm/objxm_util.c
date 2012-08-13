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

/*
 *    $XConsortium: objxm_util.c /main/4 1995/11/06 18:47:59 rswiston $
 *
 * @(#)objxm_util.c	1.23 15 Feb 1994    cde_app_builder/src/libABobjXm
 *
 *     RESTRICTED CONFIDENTIAL INFORMATION:
 *    
 *    The information in this document is subject to special
 *    restrictions in a confidential disclosure agreement between
 *    HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
 *    document outside HP, IBM, Sun, USL, SCO, or Univel without
 *    Sun's specific written approval.  This document and all copies
 *    and derivative works thereof must be returned or destroyed at
 *    Sun's request.
 *
 *    Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 *
 */


/*
 ***********************************************************************
 * objxm_util.c - general Motif utility routines
 *
 *
 ***********************************************************************
 */

#include <stdio.h>
#include <sys/stat.h>
#include <sys/param.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <Xm/MenuShell.h>
#include <Dt/xpm.h>
#include <ab_private/obj.h>
#include <ab_private/trav.h>
#include "objxmP.h"

#define	BUF_SIZE	512
#define	LG_BUF_SIZE	1024

/*
 * Public symbols
 */
ObjxmFilenameToPixmapFunc	ObjxmP_filename_to_pixmap_cb = NULL;

/*
 * Post instantiate func, which can be registered via
 */
ObjxmPostInstantiateFunc	ObjxmP_post_instantiate_cb = NULL;


/*************************************************************************
**                                                                      **
**       Private Function Declarations                                  **
**                                                                      **
*************************************************************************/ 

static int 	indent(
		    FILE *file, int spaces
		);

static int	convert_filebase_to_pixmap(
   		    Widget	widget,
    		    STRING	filebase,
    		    Pixmap	*pixmap_p
		);
static int	convert_file_to_pixmap(
    		    Widget	widget,
    		    STRING      filename,
    		    Pixmap      *pixmap_p
		);
static int	load_pixmap(
    		    Widget      widget,
    		    STRING      filebase,
    		    STRING	ext,
    		    Pixmap      *pixmap_p
		);


/*************************************************************************
**                                                                      **
**       Function Definitions                                           **
**                                                                      **
*************************************************************************/ 

/*
 * Hook to add routine to convert filenames to callbacks.
 * NULL callback reinstates default conversion method.
 */
extern int	
objxm_set_cvt_filename_to_pixmap_callback(
    ObjxmFilenameToPixmapFunc	filename_to_pixmap_cb
)
{
    ObjxmP_filename_to_pixmap_cb = filename_to_pixmap_cb;
    return 0;
}

/*
 * Hook to add post instantiate routine
 * NULL callback reinstates default conversion method.
 */
extern int	
objxm_set_post_instantiate_callback(
    ObjxmPostInstantiateFunc	post_instantiate_cb
)
{
    ObjxmP_post_instantiate_cb = post_instantiate_cb;
    return 0;
}

/*
 * Determine if a color name exists in the rgb database
 */
BOOL
objxm_color_exists(
    String	colorname
)
{
    static Colormap     colormap = 0;
    static Display	*display = NULL;
    XColor              color;
    XColor              exact_color;
 
    if (display == NULL) /* dtbuilder doesn't handle multiple displays */
    {
	display = XtDisplay(ObjxmP_toplevel);
        colormap = XDefaultColormap(display, XDefaultScreen(display));
    }
    return(XLookupColor(display, colormap, colorname, &exact_color, &color));
}


/*
 * Use the Xt Resource converter to convert a colorname to a pixel.
 * This has the advantage of caching the value and automatically
 * taking care to Free the pixel when the associated widget is
 * destroyed.
 */
int
objxm_name_to_pixel(
    Widget   	widget,
    String   	colorname,
    Pixel	*pixel_p
)
{
    XrmValue src, dst;

    src.size = strlen(colorname) + 1;
    src.addr = (XPointer)colorname;
    dst.size = sizeof(Pixel);
    dst.addr = (XPointer)pixel_p;

    if (XtConvertAndStore(widget, XtRString, &src, XtRPixel, &dst))
        return OK;
    else
        return ERR;
}

int
objxm_filebase_to_pixmap(
    Widget      widget,
    STRING      filebase,
    Pixmap      *pixmap_p
)
{
    int         status = 0;                 /* return code */

    if (util_strempty(filebase))
        return ERR_BAD_PARAM2;

    *pixmap_p = 0;

    /* Use Pixmap conversion callback if it exists */
    if (ObjxmP_filename_to_pixmap_cb != NULL)
    {
        return(ObjxmP_filename_to_pixmap_cb(widget, filebase, pixmap_p));
    }
    else /* Use internal conversion routine */
	return (convert_filebase_to_pixmap(widget, filebase, pixmap_p));

}


STRING
objxm_pixmap_conversion_error_msg(
    ABObj	obj,
    STRING	filebase,
    int		status
)
{
    char	static buf[LG_BUF_SIZE];
    char	buf2[BUF_SIZE];
    char	file_buf[BUF_SIZE];

    if (obj != NULL)
    	sprintf(buf, catgets(OBJXM_MESSAGE_CATD, 203, 14,
		"Warning: %s :\n"), util_strsafe(obj_get_name(obj)));
    else
	strcpy(buf, "");

    sprintf(buf2, catgets(OBJXM_MESSAGE_CATD, 203, 15,
		"Graphic File base '%s'\ncould not be converted to a Pixmap because\n"), filebase);
    strcat(buf, buf2);

    if (status == ERR_BAD_PARAM2)
	strcat(buf, catgets(OBJXM_MESSAGE_CATD, 203, 16,
		"File base is an empty string."));
    else
    {
	strcat(buf, catgets(OBJXM_MESSAGE_CATD, 203, 17,
		"None of the following files could be converted to a Pixmap:\n"));

	if ((int)(strlen(buf) + (4 * (strlen(filebase) + 4))) > LG_BUF_SIZE)
	    strcat(buf, catgets(OBJXM_MESSAGE_CATD, 203, 18,
		"filenames too long to display\n"));
	else
	{
    	    sprintf(file_buf,"%s.pm\n%s.xpm\n%s.bm\n%s.xbm\n",
		filebase, filebase, filebase, filebase);

    	    strcat(buf, file_buf);
	}
    }

    util_set_help_data(catgets(OBJXM_MESSAGE_CATD, 203, 20,
	"The graphic file name specified could not be converted\ninto a pixmap. App Builder searches for a file of the\nform Filename.pm, Filename.xpm, Filename.bm, and\nFilename.xbm.\n\nEither none of those files exists in the directory specified\n(via the Graphic Filename field or if no path was specified,\nthen the current working directory), or a file exists but may\nbe corrupt."), NULL, NULL);

    return(buf);
}

/*
 * Given the 'base' graphic filename, attempt to load in an appropriate
 * Pixmap with the following rules:
 * 	If display is color (depth > 1), try loading the following filenames
 *	in sequence until 1 succeeds:
 *		base.pm, base.xpm (XPM), base.bm, base.xbm (Bitmap)
 *		
 *	If display is monochrome, try loading in the following sequence
 *	until 1 succeeds:
 *		base.bm, base.xbm (Bitmap), base.pm, base.xpm (XPM)
 *
 */
static int
convert_filebase_to_pixmap(
    Widget	widget,
    STRING	filebase,
    Pixmap	*pixmap_p
)
{
    int	status;

    if (ObjxmP_visual_depth > 1) /* Color - Try XPM First */
    {
	if ((status = load_pixmap(widget, filebase, ".pm", pixmap_p)) != OK)
	    if ((status = load_pixmap(widget, filebase, ".xpm", pixmap_p)) != OK)
		if ((status = load_pixmap(widget, filebase, ".bm", pixmap_p)) != OK)
		    status = load_pixmap(widget, filebase, ".xbm", pixmap_p);
    }
    else /* Monochrome - Try Bitmap First */
    {
        if ((status = load_pixmap(widget, filebase, ".bm", pixmap_p)) != OK) 
            if ((status = load_pixmap(widget, filebase, ".xbm", pixmap_p)) != OK) 
                if ((status = load_pixmap(widget, filebase, ".pm", pixmap_p)) != OK) 
                    status = load_pixmap(widget, filebase, ".xpm", pixmap_p); 
    }
    return status;
}


/*
 * Attempt to dynamically read a bitmap file and load it into a Pixmap
 */
static int
convert_file_to_pixmap(
    Widget	widget,
    STRING      filename,
    Pixmap      *pixmap_p
)
{
    int		rc = 0;			/* return code */
    Screen      *screen = NULL; 
    Pixel       fgPixel = 0;
    Pixel       bgPixel = 0; 

    *pixmap_p = 0;

    /*
     * Get default values
     */
    screen = XtScreen(widget);
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
    *pixmap_p = XmGetPixmap(screen, filename, fgPixel, bgPixel);
    if ((*pixmap_p == 0) || (*pixmap_p == XmUNSPECIFIED_PIXMAP))
    {
	return -1;
    }
    return 0;
}

    
static int
load_pixmap(
    Widget      widget,
    STRING      filebase,
    STRING	ext,
    Pixmap      *pixmap_p
)
{
    char            filename[BUF_SIZE];
    int             status;

    if ((int)(strlen(filebase) + strlen(ext) + 1) > BUF_SIZE)
	return ERR;

    strcpy(filename, filebase);
    strcat(filename, ext);
    return(status = convert_file_to_pixmap(widget, filename, pixmap_p));
}



XFontStruct *
objxm_fontlist_to_font(
    XmFontList	fontlist
)
{
    XmFontContext	context;
    XFontStruct		*font;
    XmStringCharSet     charset;

    if (!fontlist)
	return NULL;

    XmFontListInitFontContext(&context, fontlist);

    if (XmFontListGetNextFont(context, &charset, &font))
    {
        XmFontListFreeFontContext(context);
	XtFree(charset);
	return(font);
    }
    else if (util_get_verbosity() > 0)
    {
        util_puts_err("objxm_fontlist_to_font: could not convert fontlist\n");
    }

    XmFontListFreeFontContext(context);
    return NULL;

}

XmString
objxm_str_to_xmstr(
    Widget   widget,
    String   string
)
{
    XrmValue src, dst;
    XmString xmstr;

    if (util_strempty(string))
        return((XmString)NULL);

    src.size = strlen(string) + 1;
    src.addr = (XPointer)string;
    dst.size = sizeof(XmString);
    dst.addr = (XPointer)&xmstr;

    if (XtConvertAndStore(widget, XtRString, &src, XmRXmString, &dst))
        return(xmstr);
    else
        return((XmString)NULL);

}


STRING
objxm_xmstr_to_str(
    XmString    xmstr
)
{
    XmStringContext   context;
    XmStringCharSet   char_set_tag;
    XmStringDirection dir;
    Boolean           sep;
    Boolean           status = FALSE;
    char	      buf[MAXPATHLEN], *seg, *p;

    p = buf;
    if (XmStringInitContext(&context, xmstr))
    {
        while(XmStringGetNextSegment(context, &seg,
                        &char_set_tag, &dir, &sep))
	{
	    p += (strlen(strcpy(p, seg)));
	    if (sep == TRUE)
	    {
		*p++ = '\n';
		*p   = '\0';
	    }
	    XtFree(seg);
	}
	XmStringFreeContext(context);
	return((STRING)XtNewString(buf));
    }
    if (util_get_verbosity() > 0)
    {
        util_puts_err("objxm_xmstr_to_str: could not convert xmstring\n");
    }
    return NULL;

}

/*
 * Given an Accelerator String (translation Table syntax), return
 * the string that should be displayed inside the item containing the
 * accelerator
 */
String
objxm_accel_to_acceltext(
    String	accelstr
)
{
    char 	buf[128];
    static char acceltext[128];
    char	*p;	

    if (util_strempty(accelstr))
	return NULL;

    strcpy(buf, accelstr);

    p = strtok(buf, "<");
    if (p != NULL)
    {
	strcpy(acceltext, p);
        while(p != NULL)
    	{
	    /* Replace any "<..>" with "+" */
    	    p = strtok(NULL, ">");
	    if (p != NULL)
	    {
		strcat(acceltext,"+");
	    	p = strtok(NULL, "<");
	    	if (p != NULL)
		    strcat(acceltext, p);
	    }
	}
    }
    return acceltext;

}

unsigned char
objxm_linestyle_to_enum(
    AB_LINE_TYPE	linestyle
)
{
    unsigned char value;

    switch(linestyle)
    {
        case AB_LINE_SINGLE_LINE:
            value = XmSINGLE_LINE;
            break;
        case AB_LINE_DOUBLE_LINE:
            value = XmDOUBLE_LINE;
            break;
        case AB_LINE_SINGLE_DASHED_LINE:
            value = XmSINGLE_DASHED_LINE;
            break;
        case AB_LINE_DOUBLE_DASHED_LINE:
            value = XmDOUBLE_DASHED_LINE;
            break;
        case AB_LINE_NONE:
            value = XmNO_LINE;
            break;
        case AB_LINE_ETCHED_IN:
            value = XmSHADOW_ETCHED_IN;
            break;
        case AB_LINE_ETCHED_OUT:
            value = XmSHADOW_ETCHED_OUT;
            break;
        case AB_LINE_ETCHED_IN_DASH:
            value = XmSHADOW_ETCHED_IN_DASH;
            break;
        case AB_LINE_ETCHED_OUT_DASH:
            value = XmSHADOW_ETCHED_OUT_DASH;
            break;
        default:
            return (unsigned char)-1;

    }
    return value;
}

BOOL
objxm_is_menu_widget(
    Widget	widget
)
{
    /* If MenuShell OR MenuPane, return TRUE..*/
    if (XtIsSubclass(widget, xmMenuShellWidgetClass) ||
	    XtIsSubclass(XtParent(widget), xmMenuShellWidgetClass))
	    return TRUE;

    return FALSE;

}

/*
 ***********************************************************************
 * 
 *  Debugging routines
 *
 ***********************************************************************
 */

int
objxm_print(ABObj obj)
{
    return objxm_print_indented(obj, 0, util_get_verbosity());
}

int 
objxm_print_indented(ABObj obj, int spaces, int verbosity)
{
    obj_print_indented(obj, spaces, verbosity);
    if (verbosity >= 4)
    {
	ArgList	args= objxm_obj_get_ui_args(obj);
	int	num_args= objxm_get_num_args(args);
	objxm_dump_arglist_indented(obj, args, num_args, spaces);
	util_dprintf(0, "\n");
    }
    return 0;
}


int
objxm_tree_print(ABObj obj)
{
    objxm_tree_print_indented(obj, 0, util_get_verbosity());
    util_dprintf(0,"\n");
    return 0;
}


int
objxm_tree_print_indented(ABObj obj, int spaces, int verbosity)
{
    AB_TRAVERSAL	trav;
    ABObj		child = NULL;

    objxm_print_indented(obj, spaces, verbosity);

    for (trav_open(&trav, obj, AB_TRAV_CHILDREN);
	(child= trav_next(&trav)) != NULL; )
    {
	objxm_tree_print_indented(child, spaces+4, verbosity);
    }
    trav_close(&trav);

    return 0;
}


static int
indent (FILE *file, int spaces)
{
    int i= 0;
    for (i= 0; i < spaces; ++i)
    {
	fputc(' ', file);
    }
    return 0;
}

