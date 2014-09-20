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
 *      $XConsortium: lib_func_strings.c /main/3 1995/11/06 18:06:25 rswiston $
 *
 *      @(#)lib_func_strings.c	1.46 08 May 1995
 *
 *      RESTRICTED CONFIDENTIAL INFORMATION:
 *
 *      The information in this document is subject to special
 *      restrictions in a confidential disclosure agreement between
 *      HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
 *      document outside HP, IBM, Sun, USL, SCO, or Univel without
 *      Sun's specific written approval.  This document and all copies
 *      and derivative works thereof must be returned or destroyed at
 *      Sun's request.
 *
 *      Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 *
 */


/*
 *  lib_func_strings.c
 */

#include "lib_func_stringsP.h"

/*********************************************************
**							**
**       Constants (#define and const)			**
**							**
**********************************************************/

/*********************************************************
**							**
**       Private Functions (C declarations and macros)	**
**							**
**********************************************************/

/*********************************************************
**							**
**       Data						**
**							**
**********************************************************/

/*
 * abmfP_lib_cvt_file_to_pixmap
 */
static LibFuncRec abmfP_lrc_cvt_file_to_pixmap = 
{
    /* name */
    "dtb_cvt_file_to_pixmap",
    
    /* proto */
    "int dtb_cvt_file_to_pixmap(\n"
    "    String	fileName,\n"
    "    Widget	widget,\n"
    "    Pixmap	*pixmapReturnPtr\n"
    ");",
    
    /* def */
    "/*\n"
    " * Create/load a Pixmap given an XPM or Bitmap files\n"
    " * NOTE: this allocates a server Pixmap;  it is the responsibility\n"
    " * of the caller to free the Pixmap\n"
    " */\n"
    "int\n"
    "dtb_cvt_file_to_pixmap(\n"
    "    String	fileName,\n"
    "    Widget	widget,\n"
    "    Pixmap	*pixmapReturnPtr\n"
    ")\n"
    "{\n"
    "#define pixmapReturn (*pixmapReturnPtr)\n"
    "    Pixmap	pixmap = 0;\n"
    "    Screen	*screen = NULL;\n"
    "    Pixel	fgPixel = 0;\n"
    "    Pixel	bgPixel = 0;\n"
    "    char	image_path[MAXPATHLEN+1];\n"
    "    Boolean	pixmap_found = False;\n"
    "\n"
    "    /*\n"
    "     * Get default values\n"
    "     */\n"
    "    screen = XtScreenOfObject(widget);\n"
    "    fgPixel = WhitePixelOfScreen(screen);\n"
    "    bgPixel = BlackPixelOfScreen(screen);\n"
    "\n"
    "    /*\n"
    "     * Get proper colors for widget\n"
    "     */\n"
    "    XtVaGetValues(widget,\n"
    "	XmNforeground, &fgPixel,\n"
    "	XmNbackground, &bgPixel,\n"
    "	NULL);\n"
    "\n"
    "    /*\n"
    "     * In CDE, XmGetPixmap handles .xpm files, as well.\n"
    "     */\n"
    "    if (!pixmap_found)\n"
    "    {\n"
    "        pixmap = XmGetPixmap(screen, fileName, fgPixel, bgPixel);\n"
    "    }\n"
    "    pixmap_found = ((pixmap != 0) && (pixmap != XmUNSPECIFIED_PIXMAP));\n"
    "\n"
    "    if (!pixmap_found)\n"
    "    {\n"
    "        sprintf(image_path, \"%s/%s\", dtb_get_exe_dir(), fileName);\n"
    "        pixmap = XmGetPixmap(screen, image_path, fgPixel, bgPixel);\n"
    "    }\n"
    "    pixmap_found = ((pixmap != 0) && (pixmap != XmUNSPECIFIED_PIXMAP));\n"
    "\n"
    "    if (!pixmap_found)\n"
    "    {\n"
    "        sprintf(image_path, \"%s/bitmaps/%s\", dtb_get_exe_dir(), fileName);\n"
    "        pixmap = XmGetPixmap(screen, image_path, fgPixel, bgPixel);\n"
    "    }\n"
    "    pixmap_found = ((pixmap != 0) && (pixmap != XmUNSPECIFIED_PIXMAP));\n"
    "\n"
    "    if (!pixmap_found)\n"
    "    {\n"
    "	return -1;\n"
    "    }\n"
    "\n"
    "    pixmapReturn = pixmap;\n"
    "    pixmapReturn = pixmap;\n"
    "    return 0;\n"
    "#undef pixmapReturn\n"
    "}"
}; /* abmfP_lrc_cvt_file_to_pixmap */
LibFunc abmfP_lib_cvt_file_to_pixmap = &(abmfP_lrc_cvt_file_to_pixmap);

/*
 * abmfP_lib_create_greyed_pixmap
 */
static LibFuncRec abmfP_lrc_create_greyed_pixmap = 
{
    /* name */
    "dtb_create_greyed_pixmap",
    
    /* proto */
    "Pixmap dtb_create_greyed_pixmap(\n"
    "    Widget	widget,\n"
    "    Pixmap	pixmap\n"
    ");",
    
    /* def */
    "/*\n"
    " * For a given pixmap, create a 50% greyed version.  Most likely this will\n"
    " * be used where the source pixmap is the labelPixmap for a widget and an \n"
    " * insensitivePixmap is needed so the widget will look right when it is \n"
    " * \"not sensitive\" (\"greyed out\" or \"inactive\").\n"
    " * \n"
    " * NOTE: This routine creates a Pixmap, which is an X server resource.  The\n"
    " *       created pixmap must be freed by the caller when it is no longer\n"
    " *	 needed.\n"
    " */\n"
    "Pixmap\n"
    "dtb_create_greyed_pixmap(\n"
    "    Widget	widget,\n"
    "    Pixmap	pixmap\n"
    ")\n"
    "{\n"
    "    Display	 *dpy;\n"
    "    Window	 root;\n"
    "    Pixmap       insensitive_pixmap;\n"
    "    Pixel	 background;\n"
    "    unsigned int width, height, depth, bw;\n"
    "    int		 x,y;\n"
    "    XGCValues    gcv;\n"
    "    XtGCMask     gcm;\n"
    "    GC           gc;\n"
    "\n"
    "\n"
    "    dpy = XtDisplayOfObject(widget);\n"
    "\n"
    "    if(pixmap == XmUNSPECIFIED_PIXMAP || pixmap == 0) {\n"
    "	return(0);\n"
    "    }\n"
    "\n"
    "    XtVaGetValues(widget,\n"
    "	XmNbackground, &background,\n"
    "	NULL);\n"
    "\n"
    "    /* Get width/height of source pixmap */\n"
    "    if (!XGetGeometry(dpy,pixmap,&root,&x,&y,&width,&height,&bw,&depth)) {\n"
    "	    return((Pixmap)0);\n"
    "    }\n"
    "    gcv.foreground = background;\n"
    "    gcv.fill_style = FillStippled;\n"
    "    gcv.stipple = XmGetPixmapByDepth(XtScreenOfObject(widget),\n"
    "			\"50_foreground\", 1, 0, 1);\n"
    "    gcm = GCForeground | GCFillStyle | GCStipple;\n"
    "    gc = XtGetGC(widget, gcm, &gcv);\n"
    "\n"
    "    /* Create insensitive pixmap */\n"
    "    insensitive_pixmap = XCreatePixmap(dpy, pixmap, width, height, depth);\n"
    "    XCopyArea(dpy, pixmap, insensitive_pixmap, gc, 0, 0, width, height, 0, 0);\n"
    "    XFillRectangle(dpy, insensitive_pixmap, gc, 0, 0, width, height);\n"
    "\n"
    "    XtReleaseGC(widget, gc);\n"
    "    return(insensitive_pixmap);\n"
    "}"
}; /* abmfP_lrc_create_greyed_pixmap */
LibFunc abmfP_lib_create_greyed_pixmap = &(abmfP_lrc_create_greyed_pixmap);

/*
 * abmfP_lib_set_label_pixmaps
 */
static LibFuncRec abmfP_lrc_set_label_pixmaps = 
{
    /* name */
    "dtb_set_label_pixmaps",
    
    /* proto */
    "int dtb_set_label_pixmaps(\n"
    "    Widget	widget,\n"
    "    Pixmap	labelPixmap,\n"
    "    Pixmap	labelInsensitivePixmap\n"
    ");",
    
    /* def */
    "/*\n"
    " * Sets the label and insensitive label pixmaps of the widget.  \n"
    " *\n"
    " * If either (or both) pixmap is 0, it is ignored.\n"
    " */\n"
    "int\n"
    "dtb_set_label_pixmaps(\n"
    "    Widget	widget,\n"
    "    Pixmap	labelPixmap,\n"
    "    Pixmap	labelInsensitivePixmap\n"
    ")\n"
    "{\n"
    "    if (   (widget == 0)\n"
    "	|| ((labelPixmap == 0) && (labelInsensitivePixmap == 0)) )\n"
    "    {\n"
    "	return -1;\n"
    "    }\n"
    "\n"
    "    /*\n"
    "     * Set the approriate resources.\n"
    "     */\n"
    "    XtVaSetValues(widget, XmNlabelType,	XmPIXMAP, NULL);\n"
    "    if (labelPixmap != NULL)\n"
    "    {\n"
    "	XtVaSetValues(widget, XmNlabelPixmap, labelPixmap, NULL);\n"
    "    }\n"
    "    if (labelInsensitivePixmap != NULL)\n"
    "    {\n"
    "	XtVaSetValues(widget, XmNlabelInsensitivePixmap, \n"
    "				labelInsensitivePixmap, NULL);\n"
    "    }\n"
    "\n"
    "    return 0;\n"
    "}"
}; /* abmfP_lrc_set_label_pixmaps */
LibFunc abmfP_lib_set_label_pixmaps = &(abmfP_lrc_set_label_pixmaps);

/*
 * abmfP_lib_file_has_extension
 */
static LibFuncRec abmfP_lrc_file_has_extension = 
{
    /* name */
    "dtb_file_has_extension",
    
    /* proto */
    "Boolean dtb_file_has_extension(\n"
    "    String	fileName, \n"
    "    String	extension\n"
    ");",
    
    /* def */
    "/*\n"
    " * Returns True if the fileName has the extension\n"
    " */\n"
    "Boolean\n"
    "dtb_file_has_extension(\n"
    "    String	fileName, \n"
    "    String	extension\n"
    ")\n"
    "{\n"
    "    Boolean        hasExt = False;\n"
    "\n"
    "    if (extension == NULL)\n"
    "    {\n"
    "        hasExt = ( (fileName == NULL) || (strlen(fileName) == 0) );\n"
    "    }   \n"
    "    else\n"
    "    {   \n"
    "        if (fileName == NULL)\n"
    "            hasExt = False;\n"
    "        else\n"
    "        {\n"
    "            char *dotPtr= strrchr(fileName, '.');\n"
    "            if (dotPtr == NULL)\n"
    "                hasExt= False;\n"
    "            else if (strcmp(dotPtr+1, extension) == 0)\n"
    "	 	hasExt = True;\n"
    "        }\n"
    "    }\n"
    "    return hasExt;\n"
    "}"
}; /* abmfP_lrc_file_has_extension */
LibFunc abmfP_lib_file_has_extension = &(abmfP_lrc_file_has_extension);

/*
 * abmfP_lib_cvt_filebase_to_pixmap
 */
static LibFuncRec abmfP_lrc_cvt_filebase_to_pixmap = 
{
    /* name */
    "dtb_cvt_filebase_to_pixmap",
    
    /* proto */
    "int dtb_cvt_filebase_to_pixmap(\n"
    "    Widget	widget,\n"
    "    String	fileBase,\n"
    "    String	extension,\n"
    "    Pixmap	*pixmap_ptr\n"
    ");",
    
    /* def */
    "/*\n"
    " * Appends the extension to fileBase and attempts to load in\n"
    " * the Pixmap \n"
    " */\n"
    "int\n"
    "dtb_cvt_filebase_to_pixmap(\n"
    "    Widget	widget,\n"
    "    String	fileBase,\n"
    "    String	extension,\n"
    "    Pixmap	*pixmap_ptr\n"
    ")\n"
    "{\n"
    "    char        fileName[512];  \n"
    "    int		rc = 0;\n"
    "\n"
    "    strcpy(fileName, fileBase);\n"
    "    strcat(fileName, extension);\n"
    "    rc = dtb_cvt_file_to_pixmap(fileName, widget, pixmap_ptr);\n"
    "    return rc;\n"
    "}"
}; /* abmfP_lrc_cvt_filebase_to_pixmap */
LibFunc abmfP_lib_cvt_filebase_to_pixmap = &(abmfP_lrc_cvt_filebase_to_pixmap);

/*
 * abmfP_lib_cvt_image_file_to_pixmap
 */
static LibFuncRec abmfP_lrc_cvt_image_file_to_pixmap = 
{
    /* name */
    "dtb_cvt_image_file_to_pixmap",
    
    /* proto */
    "int dtb_cvt_image_file_to_pixmap(\n"
    "		Widget	widget,\n"
    "		String	fileName,\n"
    "		Pixmap	*pixmap\n"
    ");",
    
    /* def */
    "int\n"
    "dtb_cvt_image_file_to_pixmap(\n"
    "		Widget	widget,\n"
    "		String	fileName,\n"
    "		Pixmap	*pixmap\n"
    ")\n"
    "{\n"
    "    int		rc = 0;		/* return code */\n"
    "    Pixmap	tmpPixmap = 0;\n"
    "    int		depth;\n"
    "\n"
    "    if (dtb_file_has_extension(fileName, \"pm\") ||\n"
    " 	dtb_file_has_extension(fileName, \"xpm\") ||	\n"
    " 	dtb_file_has_extension(fileName, \"bm\") ||	\n"
    " 	dtb_file_has_extension(fileName, \"xbm\"))\n"
    "    {\n"
    "    	/* If explicit filename requested, use it directly */\n"
    "	rc = dtb_cvt_file_to_pixmap(fileName, widget, &tmpPixmap);\n"
    "    }\n"
    "    else /* Append extensions to locate best graphic match */\n"
    "    {\n"
    "    	XtVaGetValues(XtIsSubclass(widget, xmGadgetClass)? XtParent(widget) : widget, \n"
    "		XmNdepth, &depth, NULL);\n"
    "\n"
    "    	if (depth > 1) /* Look for Color Graphics First */\n"
    "    	{\n"
    "    	    rc = dtb_cvt_filebase_to_pixmap(widget, fileName, \".pm\", &tmpPixmap);\n"
    "    	    if (rc < 0)\n"
    "	    	rc = dtb_cvt_filebase_to_pixmap(widget, fileName, \".xpm\", &tmpPixmap);\n"
    "	    if (rc < 0)\n"
    "            	rc = dtb_cvt_filebase_to_pixmap(widget, fileName, \".bm\", &tmpPixmap);\n"
    "            if (rc < 0) \n"
    "            	rc = dtb_cvt_filebase_to_pixmap(widget, fileName, \".xbm\", &tmpPixmap); \n"
    "    	}\n"
    "    	else /* Look for Monochrome First */\n"
    "    	{\n"
    "            rc = dtb_cvt_filebase_to_pixmap(widget, fileName, \".bm\", &tmpPixmap);\n"
    "            if (rc < 0)\n"
    "            	rc = dtb_cvt_filebase_to_pixmap(widget, fileName, \".xbm\", &tmpPixmap);\n"
    "            if (rc < 0) \n"
    "            	rc = dtb_cvt_filebase_to_pixmap(widget, fileName, \".pm\", &tmpPixmap); \n"
    "            if (rc < 0) \n"
    "            	rc = dtb_cvt_filebase_to_pixmap(widget, fileName, \".xpm\", &tmpPixmap);  \n"
    "    	}\n"
    "    }\n"
    "\n"
    "    if (rc < 0)\n"
    "    {\n"
    "	*pixmap = 0;\n"
    "	return rc;\n"
    "    }\n"
    "\n"
    "    *pixmap = tmpPixmap;\n"
    "    return 0;\n"
    "}"
}; /* abmfP_lrc_cvt_image_file_to_pixmap */
LibFunc abmfP_lib_cvt_image_file_to_pixmap = &(abmfP_lrc_cvt_image_file_to_pixmap);

/*
 * abmfP_lib_set_label_from_image_file
 */
static LibFuncRec abmfP_lrc_set_label_from_image_file = 
{
    /* name */
    "dtb_set_label_from_image_file",
    
    /* proto */
    "int dtb_set_label_from_image_file(\n"
    "		Widget	widget,\n"
    "		String	fileName\n"
    ");",
    
    /* def */
    "/* \n"
    " * Sets the XmNlabel from the image file (either xbitmap or xpixmap format).\n"
    " *\n"
    " * returns negative on error.\n"
    " */\n"
    "int\n"
    "dtb_set_label_from_image_file(\n"
    "		Widget	widget,\n"
    "		String	fileName\n"
    ")\n"
    "{\n"
    "    int		rc = 0;		/* return code */\n"
    "    Pixmap	labelPixmap = 0;\n"
    "    Pixmap	insensitivePixmap = 0;\n"
    "    int		depth;\n"
    "\n"
    "    rc = dtb_cvt_image_file_to_pixmap(widget, fileName, &labelPixmap);\n"
    "    if (rc < 0)\n"
    "    {\n"
    "	return rc;\n"
    "    }\n"
    "\n"
    "    insensitivePixmap = dtb_create_greyed_pixmap(widget,labelPixmap);\n"
    "    rc = dtb_set_label_pixmaps(widget, labelPixmap, insensitivePixmap);\n"
    "    if (rc < 0)\n"
    "    {\n"
    "	return rc;\n"
    "    }\n"
    "\n"
    "    return 0;\n"
    "}"
}; /* abmfP_lrc_set_label_from_image_file */
LibFunc abmfP_lib_set_label_from_image_file = &(abmfP_lrc_set_label_from_image_file);

/*
 * abmfP_lib_set_label_from_bitmap_data
 */
static LibFuncRec abmfP_lrc_set_label_from_bitmap_data = 
{
    /* name */
    "dtb_set_label_from_bitmap_data",
    
    /* proto */
    "int dtb_set_label_from_bitmap_data(\n"
    "    Widget		widget,\n"
    "    int			width,\n"
    "    int			height,\n"
    "    unsigned char	*bitmapData\n"
    ");",
    
    /* def */
    "/*\n"
    " * Sets both the sensitive and insensitve pixmaps\n"
    " */\n"
    "int\n"
    "dtb_set_label_from_bitmap_data(\n"
    "    Widget		widget,\n"
    "    int			width,\n"
    "    int			height,\n"
    "    unsigned char	*bitmapData\n"
    ")\n"
    "{\n"
    "    Display		*display = NULL;\n"
    "    Screen		*screen = NULL;\n"
    "    Drawable		window = 0;\n"
    "    long		bgPixel = 0;\n"
    "    long		fgPixel = 0;\n"
    "    unsigned int	depth = 0;\n"
    "    Pixmap		labelPixmap = 0;\n"
    "\n"
    "    if (   (widget == 0)\n"
    "	|| (width < 1)\n"
    "	|| (height < 1)\n"
    "	|| (bitmapData == NULL) )\n"
    "    {\n"
    "        return -1;\n"
    "    }\n"
    "\n"
    "    /*\n"
    "     * Get a whole slew of information X needs\n"
    "     */\n"
    "    {\n"
    "	Pixel		widgetBg = 0;\n"
    "	Pixel		widgetFg = 0;\n"
    "	int		widgetDepth = 0;\n"
    "\n"
    "        display = XtDisplay(widget);\n"
    "        screen = XtScreen(widget);\n"
    "        window = XtWindow(widget);\n"
    "	if (window == 0)\n"
    "	{\n"
    "	    /* Widget has not been realized, yet */\n"
    "	    window = RootWindowOfScreen(screen);\n"
    "	}\n"
    "\n"
    "	XtVaGetValues(XtIsSubclass(widget, xmGadgetClass)? XtParent(widget) : widget,\n"
    "		XmNbackground,	&widgetBg,\n"
    "		XmNforeground,	&widgetFg,\n"
    "		XmNdepth,	&widgetDepth,\n"
    "		NULL);\n"
    "	bgPixel = widgetBg;\n"
    "	fgPixel = widgetFg;\n"
    "	depth = widgetDepth;\n"
    "    }\n"
    "\n"
    "    /*\n"
    "     * Create the pixmap\n"
    "     */\n"
    "    labelPixmap = XCreatePixmapFromBitmapData(\n"
    "		display,\n"
    "		window,\n"
    "		(char *) bitmapData,\n"
    "		width, height,\n"
    "		fgPixel, bgPixel,\n"
    "		depth);\n"
    "    if (labelPixmap == 0)\n"
    "    {\n"
    "	return -1;\n"
    "    }\n"
    "\n"
    "    dtb_set_label_pixmaps(widget, labelPixmap, 0);\n"
    "\n"
    "    return 0;\n"
    "}"
}; /* abmfP_lrc_set_label_from_bitmap_data */
LibFunc abmfP_lib_set_label_from_bitmap_data = &(abmfP_lrc_set_label_from_bitmap_data);

/*
 * abmfP_lib_set_label_from_xpm_data
 */
static LibFuncRec abmfP_lrc_set_label_from_xpm_data = 
{
    /* name */
    "dtb_set_label_from_xpm_data",
    
    /* proto */
    "int dtb_set_label_from_xpm_data(\n"
    "    Widget	widget,\n"
    "    char	*xpmData[]\n"
    ");",
    
    /* def */
    "/*\n"
    " * Sets both the sensitive and insensitve pixmaps\n"
    " */\n"
    "int\n"
    "dtb_set_label_from_xpm_data(\n"
    "    Widget	widget,\n"
    "    char	*xpmData[]\n"
    ")\n"
    "{\n"
    "    int		status = 0;\n"
    "    Display	*display = NULL;\n"
    "    Screen	*screen = NULL;\n"
    "    Drawable	window = 0;\n"
    "    Pixmap	labelPixmap = 0;\n"
    "\n"
    "    if (   (widget == 0)\n"
    "	|| (xpmData == NULL) )\n"
    "    {\n"
    "	return -1;\n"
    "    }\n"
    "\n"
    "    display = XtDisplay(widget);\n"
    "    screen = XtScreen(widget);\n"
    "    window = XtWindow(widget);\n"
    "    if (window == 0)\n"
    "    {\n"
    "        /* Widget has not been realized, yet */\n"
    "        window = RootWindowOfScreen(screen);\n"
    "    }\n"
    "\n"
    "    status = _DtXpmCreatePixmapFromData(\n"
    "            display,\n"
    "            window,\n"
    "            xpmData,\n"
    "            &labelPixmap,\n"
    "            (Pixmap *)NULL,\n"
    "            (XpmAttributes *)NULL);\n"
    "    if (status != XpmSuccess)\n"
    "        return -1;\n"
    "\n"
    "    dtb_set_label_pixmaps(widget, labelPixmap, 0);\n"
    "\n"
    "    return 0;\n"
    "}"
}; /* abmfP_lrc_set_label_from_xpm_data */
LibFunc abmfP_lib_set_label_from_xpm_data = &(abmfP_lrc_set_label_from_xpm_data);

/*
 * abmfP_lib_cvt_resource_from_string
 */
static LibFuncRec abmfP_lrc_cvt_resource_from_string = 
{
    /* name */
    "dtb_cvt_resource_from_string",
    
    /* proto */
    "unsigned long dtb_cvt_resource_from_string(\n"
    "    Widget		parent,\n"
    "    String		res_type,\n"
    "    unsigned int	size_of_type,\n"
    "    String		res_str_value,\n"
    "    unsigned long	error_value\n"
    ");",
    
    /* def */
    "unsigned long\n"
    "dtb_cvt_resource_from_string(\n"
    "    Widget		parent,\n"
    "    String		res_type,\n"
    "    unsigned int	size_of_type,\n"
    "    String		res_str_value,\n"
    "    unsigned long	error_value\n"
    ")\n"
    "{\n"
    "    unsigned long	cvt_value_return = error_value;\n"
    "    unsigned char	cvt_value1 = 0;\n"
    "    unsigned short	cvt_value2 = 0;\n"
    "    unsigned int	cvt_value3 = 0;\n"
    "    unsigned long	cvt_value4 = 0;\n"
    "    XtPointer		cvt_value_ptr = NULL;\n"
    "    int			which_cvt_value = -1;\n"
    "    XrmValue		source;\n"
    "    XrmValue		dest;\n"
    "\n"
    "    if (size_of_type > sizeof(cvt_value_return))\n"
    "    {\n"
    "	/* Type we are converting to is too large */\n"
    "	return cvt_value_return;\n"
    "    }\n"
    "\n"
    "    /*\n"
    "     * Get a data object of the appropriate size\n"
    "     */\n"
    "    if (size_of_type == sizeof(cvt_value1))\n"
    "    {\n"
    "	which_cvt_value = 1;\n"
    "	cvt_value_ptr = (XtPointer)&cvt_value1;\n"
    "    }\n"
    "    else if (size_of_type == sizeof(cvt_value2))\n"
    "    {\n"
    "	which_cvt_value = 2;\n"
    "	cvt_value_ptr = (XtPointer)&cvt_value2;\n"
    "    }\n"
    "    else if (size_of_type == sizeof(cvt_value3))\n"
    "    {\n"
    "	which_cvt_value = 3;\n"
    "	cvt_value_ptr = (XtPointer)&cvt_value3;\n"
    "    }\n"
    "    else if (size_of_type == sizeof(cvt_value4))\n"
    "    {\n"
    "	which_cvt_value = 4;\n"
    "	cvt_value_ptr = (XtPointer)&cvt_value4;\n"
    "    }\n"
    "    else\n"
    "    {\n"
    "	return cvt_value_return;\n"
    "    }\n"
    "\n"
    "    /*\n"
    "     * Actually do the conversion\n"
    "     */\n"
    "    source.size = strlen(res_str_value) + 1;\n"
    "    source.addr = res_str_value;\n"
    "\n"
    "    dest.size   = size_of_type;\n"
    "    dest.addr   = (char *)cvt_value_ptr;\n"
    "\n"
    "    if (XtConvertAndStore(parent, XtRString, &source,\n"
    "		res_type, &dest) != 0)\n"
    "    {\n"
    "	switch (which_cvt_value)\n"
    "	{\n"
    "	    case 1:\n"
    "		cvt_value_return = (unsigned long)cvt_value1;\n"
    "	    break;\n"
    "\n"
    "	    case 2:\n"
    "		cvt_value_return = (unsigned long)cvt_value2;\n"
    "	    break;\n"
    "\n"
    "	    case 3:\n"
    "		cvt_value_return = (unsigned long)cvt_value3;\n"
    "	    break;\n"
    "\n"
    "	    case 4:\n"
    "		cvt_value_return = (unsigned long)cvt_value4;\n"
    "	    break;\n"
    "	}\n"
    "    }\n"
    "\n"
    "    return cvt_value_return;\n"
    "}"
}; /* abmfP_lrc_cvt_resource_from_string */
LibFunc abmfP_lib_cvt_resource_from_string = &(abmfP_lrc_cvt_resource_from_string);

/*
 * abmfP_lib_save_toplevel_widget
 */
static LibFuncRec abmfP_lrc_save_toplevel_widget = 
{
    /* name */
    "dtb_save_toplevel_widget",
    
    /* proto */
    "void dtb_save_toplevel_widget(\n"
    "    Widget	toplevel\n"
    ");",
    
    /* def */
    "/*\n"
    "** Routines to save and access the toplevel widget for an application.\n"
    "** This is useful in dtb_ convenience functions, and also probably by\n"
    "** developers in routines they provide in their _stubs.c files.\n"
    "** static Widget dtb_project_toplevel_widget = (Widget) 0;\n"
    "*/\n"
    "void\n"
    "dtb_save_toplevel_widget(\n"
    "    Widget	toplevel\n"
    ")\n"
    "{\n"
    "	dtb_project_toplevel_widget = toplevel;\n"
    "}"
}; /* abmfP_lrc_save_toplevel_widget */
LibFunc abmfP_lib_save_toplevel_widget = &(abmfP_lrc_save_toplevel_widget);

/*
 * abmfP_lib_get_toplevel_widget
 */
static LibFuncRec abmfP_lrc_get_toplevel_widget = 
{
    /* name */
    "dtb_get_toplevel_widget",
    
    /* proto */
    "Widget dtb_get_toplevel_widget();",
    
    /* def */
    "Widget\n"
    "dtb_get_toplevel_widget()\n"
    "{\n"
    "	return(dtb_project_toplevel_widget);\n"
    "}"
}; /* abmfP_lrc_get_toplevel_widget */
LibFunc abmfP_lib_get_toplevel_widget = &(abmfP_lrc_get_toplevel_widget);

/*
 * abmfP_lib_remove_sash_focus
 */
static LibFuncRec abmfP_lrc_remove_sash_focus = 
{
    /* name */
    "dtb_remove_sash_focus",
    
    /* proto */
    "void dtb_remove_sash_focus(\n"
    "    Widget	widget\n"
    ");",
    
    /* def */
    "/*\n"
    "** Function to turn off traversal on the invisible sash within a\n"
    "** PanedWindow.  This is primarily used for the PanedWindow within\n"
    "** a Custom Dialog object.\n"
    "*/\n"
    "void\n"
    "dtb_remove_sash_focus(\n"
    "    Widget	widget\n"
    ")\n"
    "{\n"
    "    WidgetList	children;\n"
    "    int		numChildren, i;\n"
    "\n"
    "    if (widget == 0 || !XtIsSubclass(widget, xmPanedWindowWidgetClass))\n"
    "	return;\n"
    "\n"
    "    XtVaGetValues(widget,\n"
    "	XmNchildren,	&children,\n"
    "	XmNnumChildren, &numChildren,\n"
    "	NULL);\n"
    "\n"
    "    for(i = 0; i < numChildren; i++)\n"
    "	if (XtIsSubclass(children[i], xmSashWidgetClass))\n"
    "	    XtVaSetValues(children[i], XmNtraversalOn, False, NULL);\n"
    "}"
}; /* abmfP_lrc_remove_sash_focus */
LibFunc abmfP_lib_remove_sash_focus = &(abmfP_lrc_remove_sash_focus);

/*
 * abmfP_lib_save_command
 */
static LibFuncRec abmfP_lrc_save_command = 
{
    /* name */
    "dtb_save_command",
    
    /* proto */
    "void dtb_save_command(\n"
    "    char	*argv0\n"
    ");",
    
    /* def */
    "/*\n"
    " ** Routines to save and access the command used to invoke the application. \n"
    " */\n"
    "void\n"
    "dtb_save_command(\n"
    "    char	*argv0\n"
    ")\n"
    "{\n"
    "    char	exe_dir[MAXPATHLEN+1];\n"
    "    dtb_save_command_str = argv0; \n"
    "\n"
    "    /*\n"
    "     * Save the path to the executable\n"
    "     */\n"
    "    if (determine_exe_dir(argv0, exe_dir, MAXPATHLEN+1) >= 0)\n"
    "    {\n"
    "	dtb_exe_dir = (char *)malloc(strlen(exe_dir)+1);\n"
    "	if (dtb_exe_dir != NULL)\n"
    "	{\n"
    "	    strcpy(dtb_exe_dir, exe_dir);\n"
    "	}\n"
    "    }\n"
    "}"
}; /* abmfP_lrc_save_command */
LibFunc abmfP_lib_save_command = &(abmfP_lrc_save_command);

/*
 * abmfP_lib_get_command
 */
static LibFuncRec abmfP_lrc_get_command = 
{
    /* name */
    "dtb_get_command",
    
    /* proto */
    "char *  dtb_get_command();",
    
    /* def */
    "char * \n"
    "dtb_get_command() \n"
    "{\n"
    "    return(dtb_save_command_str); \n"
    "}"
}; /* abmfP_lrc_get_command */
LibFunc abmfP_lib_get_command = &(abmfP_lrc_get_command);

/*
 * abmfP_lib_help_dispatch
 */
static LibFuncRec abmfP_lrc_help_dispatch = 
{
    /* name */
    "dtb_help_dispatch",
    
    /* proto */
    "void dtb_help_dispatch(\n"
    "    Widget 	widget,\n"
    "    XtPointer 	clientData,\n"
    "    XtPointer 	callData\n"
    ");",
    
    /* def */
    "/* \n"
    "** Generic callback function to be attached as XmNhelpCallback and\n"
    "** provide support for on-object and Help-key help.  The help text to\n"
    "** be displayed is provided via a specialized data structure passed in\n"
    "** as client data.\n"
    "*/\n"
    "\n"
    "void\n"
    "dtb_help_dispatch(\n"
    "    Widget 	widget,\n"
    "    XtPointer 	clientData,\n"
    "    XtPointer 	callData\n"
    ")\n"
    "{\n"
    "    DtbObjectHelpData 	help_data = (DtbObjectHelpData)clientData;\n"
    "    int             	i;\n"
    "    Arg             	wargs[10];\n"
    "    char		buffer[100];\n"
    "    Widget		back_button;\n"
    "    static Widget	Quick_help_dialog = (Widget)0;\n"
    "    static Widget	MoreButton;\n"
    "\n"
    "    /* \n"
    "    ** In order to save the more-help info (help volume & location ID) as part\n"
    "    ** of the quick help dialog's backtrack mechanism, we have to splice the \n"
    "    ** volume & ID strings together and save them as the help volume field.\n"
    "    ** If there isn't supplemental help information, we save a null string.\n"
    "    **\n"
    "    ** Checking the status of the more-help info also lets us decide whether\n"
    "    ** the \"More...\" button should be enabled on the dialog.\n"
    "    */\n"
    "    if( help_data->help_volume     ==0 || *(help_data->help_volume) == NULL ||\n"
    "	help_data->help_locationID ==0 || *(help_data->help_locationID)== NULL){\n"
    "		buffer[0] = '\\0';\n"
    "    }\n"
    "    else {\n"
    "	sprintf(buffer,\"%s/%s\",help_data->help_volume,help_data->help_locationID);\n"
    "    }\n"
    "\n"
    "    /* \n"
    "    ** If this is our first time to post help, create the proper dialog and\n"
    "    ** set its attributes to suit the current object.  If not, then just\n"
    "    ** update the attributes.\n"
    "    **\n"
    "    ** (You have to be careful about gratuitous SetValues on the dialog because\n"
    "    ** its internal stack mechanism takes repeated settings as separate items\n"
    "    ** and updates the stack for each.)\n"
    "    */\n"
    "    if(Quick_help_dialog == (Widget)0) {\n"
    "        /* Create shared help dialog */\n"
    "        i = 0;\n"
    "	XtSetArg(wargs[i],XmNtitle, \"Application Help\");            i++;\n"
    "	XtSetArg(wargs[i],DtNhelpType, DtHELP_TYPE_DYNAMIC_STRING); i++;\n"
    "	XtSetArg(wargs[i],DtNstringData,help_data->help_text);      i++;\n"
    "        XtSetArg(wargs[i],DtNhelpVolume,buffer);		    i++;\n"
    "	Quick_help_dialog = DtCreateHelpQuickDialog(dtb_get_toplevel_widget(),\n"
    "		\"Help\",wargs,i);\n"
    "\n"
    "	/* \n"
    "	** Fetch out the Dialog's More button child and hook the 'more help'\n"
    "	** handler to its activateCallback.  Set it's current status to\n"
    "	** indicate whether this object has supplemental help data. \n"
    "	*/\n"
    "	MoreButton = DtHelpQuickDialogGetChild(Quick_help_dialog,\n"
    "		DtHELP_QUICK_MORE_BUTTON);\n"
    "	XtManageChild(MoreButton);\n"
    "	XtAddCallback(MoreButton,XmNactivateCallback,dtb_more_help_dispatch,\n"
    "		(XtPointer)Quick_help_dialog);\n"
    "	if(buffer[0] == '\\0') XtSetSensitive(MoreButton,False);\n"
    "\n"
    "	/* \n"
    "	** Fetch out the Dialog's Backtrack button child & hook a callback\n"
    "	** that will control button sensitivity based on the presence of more\n"
    "	** help data.\n"
    "	*/\n"
    "	back_button = DtHelpQuickDialogGetChild(Quick_help_dialog,\n"
    "		DtHELP_QUICK_BACK_BUTTON);\n"
    "	XtAddCallback(back_button,XmNactivateCallback,dtb_help_back_hdlr,\n"
    "		(XtPointer)Quick_help_dialog);\n"
    "    }\n"
    "    /* Otherwise the dialog already exists so we just set the attributes. */\n"
    "    else {\n"
    "	/* \n"
    "	** If we have supplemental help info, enable the more button.\n"
    "	** Also save this info for later use in the backtrack handler.\n"
    "	*/\n"
    "	if(buffer[0] == '\\0') {\n"
    "	    XtSetSensitive(MoreButton,False);\n"
    "	}\n"
    "	else {\n"
    "	    XtSetSensitive(MoreButton,True);\n"
    "	}\n"
    "\n"
    "        XtVaSetValues(Quick_help_dialog,\n"
    "    	    DtNhelpType, DtHELP_TYPE_DYNAMIC_STRING,\n"
    "            DtNhelpVolume,buffer,\n"
    "            DtNstringData,help_data->help_text,\n"
    "	    NULL);\n"
    "    }\n"
    "\n"
    "    /* Now display the help dialog */\n"
    "    XtManageChild(Quick_help_dialog);\n"
    "}"
}; /* abmfP_lrc_help_dispatch */
LibFunc abmfP_lib_help_dispatch = &(abmfP_lrc_help_dispatch);

/*
 * abmfP_lib_help_back_hdlr
 */
static LibFuncRec abmfP_lrc_help_back_hdlr = 
{
    /* name */
    "dtb_help_back_hdlr",
    
    /* proto */
    "void dtb_help_back_hdlr(\n"
    "    Widget 	widget,\n"
    "    XtPointer 	clientData,\n"
    "    XtPointer 	callData\n"
    ");",
    
    /* def */
    "/*\n"
    "** Callback that is added to the QuickHelpDialog widget's \"Backtrack\" button\n"
    "** and is used to control the \"More..\" button.  At each step in the backtrack,\n"
    "** this routine checks to see if there is help volume & location info stored\n"
    "** in the dialog's helpVolume resource.  If so, then the \"More...\" button is\n"
    "** enabled.  If not, then it is disabled.\n"
    "*/\n"
    "void\n"
    "dtb_help_back_hdlr(\n"
    "    Widget 	widget,\n"
    "    XtPointer 	clientData,\n"
    "    XtPointer 	callData\n"
    ")\n"
    "{\n"
    "    String		buffer, text, vol, loc;\n"
    "    char		*cp;\n"
    "    Widget		more_button;\n"
    "    Widget		help_dialog = (Widget)clientData;\n"
    "\n"
    "    /* Fetch the saved volume/locationID information from the dialog widget */\n"
    "    XtVaGetValues(help_dialog,\n"
    "	DtNhelpVolume,&buffer,\n"
    "	DtNstringData,&text,\n"
    "	NULL);\n"
    "\n"
    "    /* Get a handle to the \"More...\" button */\n"
    "    more_button = DtHelpQuickDialogGetChild(help_dialog,\n"
    "		DtHELP_QUICK_MORE_BUTTON);\n"
    "    /* \n"
    "    ** Parse the combined volume/locationID string.  Disable the \"More...\"\n"
    "    ** button if there isn't any help info, and enable it if there is.\n"
    "    */\n"
    "    if( buffer == 0 || (*buffer == NULL) ||\n"
    "	(cp=strrchr(buffer,'/')) == (char *)NULL) {\n"
    "		XtSetSensitive(more_button,False);\n"
    "    }\n"
    "    else {\n"
    "		XtSetSensitive(more_button,True);\n"
    "    }\n"
    "}"
}; /* abmfP_lrc_help_back_hdlr */
LibFunc abmfP_lib_help_back_hdlr = &(abmfP_lrc_help_back_hdlr);

/*
 * abmfP_lib_more_help_dispatch
 */
static LibFuncRec abmfP_lrc_more_help_dispatch = 
{
    /* name */
    "dtb_more_help_dispatch",
    
    /* proto */
    "void dtb_more_help_dispatch(\n"
    "    Widget 	widget,\n"
    "    XtPointer 	clientData,\n"
    "    XtPointer 	callData\n"
    ");",
    
    /* def */
    "/*\n"
    "** This callback is invoked when the user presses \"More...\" on the\n"
    "** QuickHelpDialog.  It figures out whether a help volume entry is associated\n"
    "** with the displayed help text, and if so it brings up a GeneralHelpDialog\n"
    "** to display the appropriate help volume information.\n"
    "*/\n"
    "void\n"
    "dtb_more_help_dispatch(\n"
    "    Widget 	widget,\n"
    "    XtPointer 	clientData,\n"
    "    XtPointer 	callData\n"
    ")\n"
    "{\n"
    "    int             	i;\n"
    "    Arg             	wargs[10];\n"
    "    String		buffer, vol, loc;\n"
    "    char		*cp;\n"
    "    static Widget	GeneralHelpDialog = (Widget) 0;\n"
    "    Widget		help_dialog = (Widget)clientData;\n"
    "    Widget		more_button;\n"
    "\n"
    "    /* Fetch the saved volume/locationID information from the dialog widget */\n"
    "    XtVaGetValues(help_dialog,\n"
    "	DtNhelpVolume,&buffer,\n"
    "	NULL);\n"
    "\n"
    "    /* \n"
    "    ** Parse the combined volume/locationID string.  If that fails there\n"
    "    ** must be no data, so don't bother displaying the GeneralHelpDialog.\n"
    "    ** (We shouldn't be in this callback routine if that happens, though...)\n"
    "    */\n"
    "    if( (cp=strrchr(buffer,'/')) != (char *)NULL) {\n"
    "	*cp++ = 0;\n"
    "	vol = buffer;\n"
    "	loc = cp; \n"
    "    }\n"
    "\n"
    "    if(GeneralHelpDialog == (Widget)0) {\n"
    "	/* Create General Help Dialog */\n"
    "        i = 0;\n"
    "	XtSetArg(wargs[i],XmNtitle, \"Application Help\");        i++;\n"
    "	XtSetArg(wargs[i],DtNhelpType, DtHELP_TYPE_TOPIC);      i++;\n"
    "	XtSetArg(wargs[i],DtNhelpVolume, vol);			i++;\n"
    "	XtSetArg(wargs[i],DtNlocationId,loc);			i++;\n"
    "\n"
    "	GeneralHelpDialog = DtCreateHelpDialog(dtb_get_toplevel_widget(),\n"
    "		\"GeneralHelp\",wargs,i);\n"
    "    }\n"
    "    else {\n"
    "        i = 0;\n"
    "        XtSetArg(wargs[i],DtNhelpType, DtHELP_TYPE_TOPIC);  	i++;\n"
    "        XtSetArg(wargs[i],DtNhelpVolume,vol);			i++;\n"
    "        XtSetArg(wargs[i],DtNlocationId,loc);			i++;\n"
    "        XtSetValues(GeneralHelpDialog,wargs,i);\n"
    "    }\n"
    "\n"
    "    /* Now take down the quick help dialog and display the full help one */\n"
    "    XtManageChild(GeneralHelpDialog);\n"
    "    XtUnmanageChild(help_dialog);\n"
    "}"
}; /* abmfP_lrc_more_help_dispatch */
LibFunc abmfP_lib_more_help_dispatch = &(abmfP_lrc_more_help_dispatch);

/*
 * abmfP_lib_do_onitem_help
 */
static LibFuncRec abmfP_lrc_do_onitem_help = 
{
    /* name */
    "dtb_do_onitem_help",
    
    /* proto */
    "void dtb_do_onitem_help();",
    
    /* def */
    "/*\n"
    "** Utility function used to provide support for on-item help.\n"
    "** It is typically invoked via a callback on the \"On Item\" item in the\n"
    "** main menubar's \"Help\" menu.\n"
    "*/\n"
    "void\n"
    "dtb_do_onitem_help()\n"
    "{\n"
    "    Widget	target;\n"
    "\n"
    "    /* Call the DtHelp routine that supports interactive on-item help. */\n"
    "    if(DtHelpReturnSelectedWidgetId(dtb_get_toplevel_widget(),\n"
    "	(Cursor)NULL,&target) != DtHELP_SELECT_VALID) return;\n"
    "	\n"
    "    /*\n"
    "    ** Starting at the target widget, wander up the widget tree looking\n"
    "    ** for one that has an XmNhelpCallback, and call the first one we\n"
    "    ** find.\n"
    "    */\n"
    "    while(target != (Widget)0) {\n"
    "	if( XtHasCallbacks(target,XmNhelpCallback) == XtCallbackHasSome) {\n"
    "	    XtCallCallbacks(target,XmNhelpCallback,(XtPointer)NULL);\n"
    "	    return;\n"
    "	}\n"
    "	else {\n"
    "	    target = XtParent(target);\n"
    "	}\n"
    "    }\n"
    "    return;\n"
    "}"
}; /* abmfP_lrc_do_onitem_help */
LibFunc abmfP_lib_do_onitem_help = &(abmfP_lrc_do_onitem_help);

/*
 * abmfP_lib_show_help_volume_info
 */
static LibFuncRec abmfP_lrc_show_help_volume_info = 
{
    /* name */
    "dtb_show_help_volume_info",
    
    /* proto */
    "int dtb_show_help_volume_info(\n"
    "    char	*volume_name,\n"
    "    char	*location_id\n"
    ");",
    
    /* def */
    "/*\n"
    "** Utility function called to display help volume information.\n"
    "** It needs the name of the help volume and the location ID (both as\n"
    "** strings) so it can configure the full help dialog widget properly.\n"
    "*/\n"
    "int\n"
    "dtb_show_help_volume_info(\n"
    "    char	*volume_name,\n"
    "    char	*location_id\n"
    ")\n"
    "{\n"
    "    int             	i;\n"
    "    Arg             	wargs[10];\n"
    "    static Widget	GeneralHelpDialog = (Widget)0;\n"
    "    \n"
    "    if(GeneralHelpDialog == (Widget)0) {\n"
    "	/* Create General Help Dialog */\n"
    "        i = 0;\n"
    "	XtSetArg(wargs[i],XmNtitle, \"Application Help\");        i++;\n"
    "	XtSetArg(wargs[i],DtNhelpType, DtHELP_TYPE_TOPIC);      i++;\n"
    "	XtSetArg(wargs[i],DtNhelpVolume, volume_name);		i++;\n"
    "	XtSetArg(wargs[i],DtNlocationId,location_id);		i++;\n"
    "\n"
    "	GeneralHelpDialog = DtCreateHelpDialog(dtb_get_toplevel_widget(),\n"
    "		\"GeneralHelp\",wargs,i);\n"
    "    }\n"
    "    else {\n"
    "        i = 0;\n"
    "        XtSetArg(wargs[i],DtNhelpType, DtHELP_TYPE_TOPIC);  	i++;\n"
    "        XtSetArg(wargs[i],DtNhelpVolume,volume_name);		i++;\n"
    "        XtSetArg(wargs[i],DtNlocationId,location_id);		i++;\n"
    "        XtSetValues(GeneralHelpDialog,wargs,i);\n"
    "    }\n"
    "\n"
    "    /* Now display the full help dialog */\n"
    "    XtManageChild(GeneralHelpDialog);\n"
    "\n"
    "    return(0);\n"
    "}"
}; /* abmfP_lrc_show_help_volume_info */
LibFunc abmfP_lib_show_help_volume_info = &(abmfP_lrc_show_help_volume_info);

/*
 * abmfP_lib_session_save
 */
static LibFuncRec abmfP_lrc_session_save = 
{
    /* name */
    "dtb_session_save",
    
    /* proto */
    "void dtb_session_save(\n"
    "    Widget 	widget,\n"
    "    XtPointer 	clientData,\n"
    "    XtPointer 	callData\n"
    ");",
    
    /* def */
    "/*\n"
    " * dtb_session_save()\n"
    " * Callback that is called when the application (top level\n"
    " * widget of application) gets a WM_SAVE_YOURSELF ClientMessage\n"
    " * This callback will call the client/application's session\n"
    " * save callback.\n"
    " */\n"
    "void\n"
    "dtb_session_save(\n"
    "    Widget 	widget,\n"
    "    XtPointer 	clientData,\n"
    "    XtPointer 	callData\n"
    ")\n"
    "{\n"
    "    int				new_argc,\n"
    "    				client_argc = 0,\n"
    "    				new_argc_counter,\n"
    "    				i;\n"
    "    char			**new_argv,\n"
    "    				**client_argv = NULL,\n"
    "    				*session_file_path,\n"
    "    				*session_file_name,\n"
    "    				*app_name = NULL;\n"
    "    Boolean			status = False;\n"
    "    DtbClientSessionSaveCB	session_saveCB;\n"
    "\n"
    "    /*\n"
    "     * Return if no widget passed in.\n"
    "     */\n"
    "    if (!widget)\n"
    "        return;\n"
    "\n"
    "    /*\n"
    "     * Get session file path/name to store application's state\n"
    "     */\n"
    "    if (DtSessionSavePath(widget, &session_file_path, &session_file_name) == False)\n"
    "        return;\n"
    "\n"
    "    /*\n"
    "     * Get client session save callback\n"
    "     */\n"
    "    session_saveCB = dtb_get_client_session_saveCB();\n"
    "\n"
    "    /*\n"
    "     * Call client session save callback\n"
    "     */\n"
    "    if (session_saveCB)\n"
    "        /*\n"
    "         * client_argv and client_argc are the variables that\n"
    "         * will contain any extra command line options\n"
    "         * that need to be used when invoking the application\n"
    "         * to bring it to the current state.\n"
    "         */\n"
    "        status = session_saveCB(widget, session_file_path,\n"
    "        			&client_argv, &client_argc);\n"
    "\n"
    "    /*\n"
    "     * Generate the reinvoking command and add it as the property value\n"
    "     */\n"
    "\n"
    "    /*  \n"
    "     * Fetch command used to invoke application\n"
    "     */\n"
    "    app_name = dtb_get_command();\n"
    "\n"
    "    /*\n"
    "     * new_argc and new_argc are the variables used to reconstruct\n"
    "     * the command to re-invoke the application\n"
    "     */\n"
    "\n"
    "    /*\n"
    "     * Start new_argc with:\n"
    "     *	1	for argv[0], normally the application\n"
    "     *	client_argc	any extra command line options as\n"
    "     *		returned from client session save\n"
    "     *		callback\n"
    "     */\n"
    "    new_argc = 1 + client_argc;\n"
    "\n"
    "    /*\n"
    "     * If the status returned from session save callback is 'True',\n"
    "     * the session file was actually used. This means we need to\n"
    "     * add:\n"
    "     * 	-session <session file name>\n"
    "     * to the command saved, which is 2 more strings.\n"
    "     */\n"
    "    if (status)\n"
    "        new_argc += 2;\n"
    "\n"
    "    /*\n"
    "     * Allocate vector\n"
    "     */\n"
    "    new_argv = (char **)XtMalloc((sizeof(char **) * new_argc));\n"
    "\n"
    "    /*\n"
    "     * Set new_argv[0] to be the application name\n"
    "     */\n"
    "    new_argc_counter = 0;\n"
    "    new_argv[new_argc_counter] = app_name;\n"
    "    new_argc_counter++;\n"
    "\n"
    "    /*\n"
    "     * Proceed to copy every command line option from\n"
    "     * client_argv. Skip -session, if found.\n"
    "     */\n"
    "    for (i=0; i < client_argc;)\n"
    "    {\n"
    "        if (strcmp(client_argv[i], \"-session\"))\n"
    "        {\n"
    "            new_argv[new_argc_counter] = client_argv[i];\n"
    "            new_argc_counter++;\n"
    "        }\n"
    "        else\n"
    "        {\n"
    "            /*\n"
    "             * Skip \"-session\"\n"
    "             * The next increment below will skip the session file.\n"
    "             */\n"
    "            ++i;\n"
    "        }\n"
    "\n"
    "        ++i;\n"
    "        \n"
    "    }\n"
    "\n"
    "    /*\n"
    "     * If session file used, add\n"
    "     *	-session <session file name>\n"
    "     */\n"
    "    if (status)\n"
    "    {\n"
    "        new_argv[new_argc_counter] = \"-session\";\n"
    "        new_argc_counter++;\n"
    "        new_argv[new_argc_counter] = session_file_name;\n"
    "    }\n"
    "    else\n"
    "    {\n"
    "        /*\n"
    "         * otherwise, destroy session file\n"
    "         */\n"
    "        (void)unlink(session_file_path);\n"
    "    }\n"
    "\n"
    "    /*\n"
    "     * Set WM_COMMAND property with vector constructed\n"
    "     */\n"
    "    XSetCommand(XtDisplay(widget), XtWindow(widget),\n"
    "    		new_argv, new_argc);\n"
    "\n"
    "    /*\n"
    "     * Free argument vector\n"
    "     */\n"
    "    XtFree ((char *)new_argv);\n"
    "\n"
    "    /*\n"
    "     * CDE Sessioning API states that the path/name\n"
    "     * strings have to be free'd by the application.\n"
    "     */\n"
    "    XtFree ((char *)session_file_path);\n"
    "    XtFree ((char *)session_file_name);\n"
    "}"
}; /* abmfP_lrc_session_save */
LibFunc abmfP_lib_session_save = &(abmfP_lrc_session_save);

/*
 * abmfP_lib_get_client_session_saveCB
 */
static LibFuncRec abmfP_lrc_get_client_session_saveCB = 
{
    /* name */
    "dtb_get_client_session_saveCB",
    
    /* proto */
    "DtbClientSessionSaveCB dtb_get_client_session_saveCB();",
    
    /* def */
    "/*\n"
    " * dtb_get_client_session_saveCB()\n"
    " */\n"
    "DtbClientSessionSaveCB\n"
    "dtb_get_client_session_saveCB()\n"
    "{\n"
    "    return(dtb_client_session_saveCB);\n"
    "\n"
    "}"
}; /* abmfP_lrc_get_client_session_saveCB */
LibFunc abmfP_lib_get_client_session_saveCB = &(abmfP_lrc_get_client_session_saveCB);

/*
 * abmfP_lib_set_client_session_saveCB
 */
static LibFuncRec abmfP_lrc_set_client_session_saveCB = 
{
    /* name */
    "dtb_set_client_session_saveCB",
    
    /* proto */
    "void dtb_set_client_session_saveCB(\n"
    "    DtbClientSessionSaveCB 	session_saveCB\n"
    ");",
    
    /* def */
    "/*\n"
    " * dtb_set_client_session_saveCB()\n"
    " */\n"
    "void\n"
    "dtb_set_client_session_saveCB(\n"
    "    DtbClientSessionSaveCB 	session_saveCB\n"
    ")\n"
    "{\n"
    "    dtb_client_session_saveCB = session_saveCB;\n"
    "\n"
    "}"
}; /* abmfP_lrc_set_client_session_saveCB */
LibFunc abmfP_lib_set_client_session_saveCB = &(abmfP_lrc_set_client_session_saveCB);

/*
 * abmfP_lib_session_restore
 */
static LibFuncRec abmfP_lrc_session_restore = 
{
    /* name */
    "dtb_session_restore",
    
    /* proto */
    "void dtb_session_restore(\n"
    "    Widget 	widget,\n"
    "    char 	*session_file\n"
    ");",
    
    /* def */
    "/*\n"
    " * dtb_session_restore()\n"
    " * Callback that is called during session restore (application\n"
    " * startup). It is called only if the application was invoked\n"
    " * with:\n"
    " *	-session <session file name>\n"
    " * It calls the client session restore callback.\n"
    " */\n"
    "void\n"
    "dtb_session_restore(\n"
    "    Widget 	widget,\n"
    "    char 	*session_file\n"
    ")\n"
    "{\n"
    "    DtbClientSessionRestoreCB	session_restoreCB;\n"
    "    char			*session_file_path;\n"
    "    Boolean			status;\n"
    "\n"
    "    /*\n"
    "     * If parameters are NULL, return.\n"
    "     */\n"
    "    if (!widget || !session_file)\n"
    "        return;\n"
    "\n"
    "    /*\n"
    "     * Get path of session file to read\n"
    "     */\n"
    "    if (DtSessionRestorePath(widget, &session_file_path, session_file) == False)\n"
    "        return;\n"
    "\n"
    "    /*\n"
    "     * Get client session restore callback\n"
    "     */\n"
    "    session_restoreCB = dtb_get_client_session_restoreCB();\n"
    "\n"
    "    /*\n"
    "     * Call client session restore callback\n"
    "     */\n"
    "    if (session_restoreCB)\n"
    "        status = session_restoreCB(widget, session_file_path);\n"
    "\n"
    "    /*\n"
    "     * CDE Sessioning API states that the path\n"
    "     * string has to be free'd by the application.\n"
    "     */\n"
    "    if (!session_file_path)\n"
    "        XtFree((char *)session_file_path);\n"
    "\n"
    "}"
}; /* abmfP_lrc_session_restore */
LibFunc abmfP_lib_session_restore = &(abmfP_lrc_session_restore);

/*
 * abmfP_lib_set_client_session_restoreCB
 */
static LibFuncRec abmfP_lrc_set_client_session_restoreCB = 
{
    /* name */
    "dtb_set_client_session_restoreCB",
    
    /* proto */
    "void dtb_set_client_session_restoreCB(\n"
    "    DtbClientSessionRestoreCB 	session_restoreCB\n"
    ");",
    
    /* def */
    "/*\n"
    " * dtb_set_client_session_restoreCB()\n"
    " */\n"
    "void\n"
    "dtb_set_client_session_restoreCB(\n"
    "    DtbClientSessionRestoreCB 	session_restoreCB\n"
    ")\n"
    "{\n"
    "    dtb_client_session_restoreCB = session_restoreCB;\n"
    "\n"
    "}"
}; /* abmfP_lrc_set_client_session_restoreCB */
LibFunc abmfP_lib_set_client_session_restoreCB = &(abmfP_lrc_set_client_session_restoreCB);

/*
 * abmfP_lib_get_client_session_restoreCB
 */
static LibFuncRec abmfP_lrc_get_client_session_restoreCB = 
{
    /* name */
    "dtb_get_client_session_restoreCB",
    
    /* proto */
    "DtbClientSessionRestoreCB dtb_get_client_session_restoreCB();",
    
    /* def */
    "/*\n"
    " * dtb_get_client_session_restoreCB()\n"
    " */\n"
    "DtbClientSessionRestoreCB\n"
    "dtb_get_client_session_restoreCB()\n"
    "{\n"
    "    return(dtb_client_session_restoreCB);\n"
    "\n"
    "}"
}; /* abmfP_lrc_get_client_session_restoreCB */
LibFunc abmfP_lib_get_client_session_restoreCB = &(abmfP_lrc_get_client_session_restoreCB);

/*
 * abmfP_lib_create_message_dlg
 */
static LibFuncRec abmfP_lrc_create_message_dlg = 
{
    /* name */
    "dtb_create_message_dlg",
    
    /* proto */
    "Widget dtb_create_message_dlg(\n"
    "    Widget		parent,\n"
    "    DtbMessageData	mbr,\n"
    "    XmString		override_msg,\n"
    "    DtbObjectHelpData	override_help\n"
    ");",
    
    /* def */
    "/*\n"
    " * Create a Message Dialog.\n"
    " */\n"
    "Widget\n"
    "dtb_create_message_dlg(\n"
    "    Widget		parent,\n"
    "    DtbMessageData	mbr,\n"
    "    XmString		override_msg,\n"
    "    DtbObjectHelpData	override_help\n"
    ")\n"
    "{\n"
    "    Widget      	msg_dlg = (Widget) 0;\n"
    "    Widget      	shell = (Widget) 0;\n"
    "    Widget      	button = (Widget) 0;\n"
    "    Widget		action_button = (Widget) 0;\n"
    "    unsigned char	default_btn = XmDIALOG_NONE;\n"
    "    Arg         	arg[12];\n"
    "    int         	n = 0;\n"
    "\n"
    "    /* The dialog should be parented off of a Shell,\n"
    "     * so walk up the tree to find the parent's shell\n"
    "     * ancestor...\n"
    "     */\n"
    "    shell = parent;\n"
    "    while(!XtIsSubclass(shell, shellWidgetClass))\n"
    "        shell = XtParent(shell);\n"
    " \n"
    "    msg_dlg = XmCreateMessageDialog(shell,\"dtb_msg_dlg\", NULL, 0);\n"
    " \n"
    "    if (!mbr->cancel_button)\n"
    "    {\n"
    "        button = XmMessageBoxGetChild(msg_dlg, XmDIALOG_CANCEL_BUTTON);\n"
    "        XtUnmanageChild(button);\n"
    "    }\n"
    "    if (!mbr->help_button)\n"
    "    {\n"
    "        button = XmMessageBoxGetChild(msg_dlg, XmDIALOG_HELP_BUTTON);\n"
    "        XtUnmanageChild(button);\n"
    "    }\n"
    "    if (mbr->action1_label == (XmString) NULL)\n"
    "    {\n"
    "        button = XmMessageBoxGetChild(msg_dlg, XmDIALOG_OK_BUTTON);\n"
    "        XtUnmanageChild(button);\n"
    "    }\n"
    "    else\n"
    "    {\n"
    "        XtSetArg(arg[n], XmNokLabelString, mbr->action1_label); n++;\n"
    "    }\n"
    "    /* Create an extra button for the MessageBox */\n"
    "    if (mbr->action2_label != (XmString) NULL)\n"
    "    {\n"
    "	button = XtVaCreateManagedWidget(\"action2_button\",\n"
    "			xmPushButtonWidgetClass,\n"
    "			msg_dlg,\n"
    "			XmNlabelString,	mbr->action2_label,\n"
    "			XmNuserData,	DTB_ACTION2_BUTTON,\n"
    "			NULL);\n"
    "    }\n"
    "    /* Create an extra button for the MessageBox */\n"
    "    if (mbr->action3_label != (XmString) NULL)\n"
    "    {\n"
    "        button = XtVaCreateManagedWidget(\"action3_button\",\n"
    "                        xmPushButtonWidgetClass,\n"
    "                        msg_dlg,\n"
    "                        XmNlabelString, mbr->action3_label,\n"
    "                        XmNuserData,    DTB_ACTION3_BUTTON,\n"
    "                        NULL);\n"
    "    }\n"
    "\n"
    "    XtSetArg(arg[n], XmNdialogType,       mbr->type);            n++;\n"
    "    XtSetValues(msg_dlg, arg, n);\n"
    "\n"
    "    if (override_msg != (XmString) NULL)\n"
    "    {\n"
    "	XtVaSetValues(msg_dlg, XmNmessageString, override_msg, NULL);\n"
    "    }\n"
    "    else if (mbr->message != (XmString) NULL)\n"
    "    {\n"
    "	XtVaSetValues(msg_dlg, XmNmessageString, mbr->message, NULL);\n"
    "    }\n"
    "\n"
    "    if (mbr->title != (XmString) NULL)\n"
    "    {\n"
    "	XtVaSetValues(msg_dlg, XmNdialogTitle, mbr->title, NULL);\n"
    "    }\n"
    "    else\n"
    "    {\n"
    "        XmString        null_str;\n"
    "\n"
    "        null_str = XmStringCreateLocalized(\" \");\n"
    "        XtVaSetValues(msg_dlg, XmNdialogTitle, null_str, NULL);\n"
    "        XmStringFree(null_str);\n"
    "    }\n"
    "\n"
    "\n"
    "    switch (mbr->default_button)\n"
    "    {\n"
    "	case DTB_ACTION1_BUTTON:\n"
    "	    default_btn = XmDIALOG_OK_BUTTON;\n"
    "	    break;\n"
    "	case DTB_ACTION2_BUTTON:\n"
    "	case DTB_ACTION3_BUTTON:\n"
    "	case DTB_NONE:\n"
    "	    default_btn = XmDIALOG_NONE;\n"
    "	    break;\n"
    "	case DTB_CANCEL_BUTTON:\n"
    "	    default_btn = XmDIALOG_CANCEL_BUTTON;\n"
    "	    break;\n"
    "	default:\n"
    "	    break;\n"
    "    }\n"
    "    XtVaSetValues(msg_dlg, XmNdefaultButtonType, default_btn, NULL);\n"
    "\n"
    "    if (mbr->default_button == DTB_ACTION2_BUTTON)\n"
    "    {\n"
    "	action_button = dtb_MessageBoxGetActionButton(msg_dlg,\n"
    "				DTB_ACTION2_BUTTON);\n"
    "    }\n"
    "    else if (mbr->default_button == DTB_ACTION3_BUTTON)\n"
    "    {\n"
    "        action_button = dtb_MessageBoxGetActionButton(msg_dlg,\n"
    "                                DTB_ACTION3_BUTTON);\n"
    "    }\n"
    "\n"
    "    if (action_button != (Widget) 0)\n"
    "    {\n"
    "	XtVaSetValues(action_button, \n"
    "                        XmNdefaultButtonShadowThickness, 2,\n"
    "			XmNshowAsDefault, True, \n"
    "			NULL);\n"
    "	XtVaSetValues(msg_dlg, XmNdefaultButton, action_button, NULL);\n"
    "    }\n"
    "\n"
    "    XtAddCallback(XtParent(msg_dlg), XtNpopdownCallback, destroyCB,\n"
    "		 (XtPointer) override_help);\n"
    "\n"
    "    return(msg_dlg);\n"
    "}"
}; /* abmfP_lrc_create_message_dlg */
LibFunc abmfP_lib_create_message_dlg = &(abmfP_lrc_create_message_dlg);

/*
 * abmfP_lib_destroyCB
 */
static LibFuncRec abmfP_lrc_destroyCB = 
{
    /* name */
    "destroyCB",
    
    /* proto */
    "static void destroyCB(\n"
    "    Widget      widget,\n"
    "    XtPointer   client_data,\n"
    "    XtPointer   call_data\n"
    ");",
    
    /* def */
    "/*\n"
    " * popdownCallback for MessageBox.\n"
    " */\n"
    "static void\n"
    "destroyCB(\n"
    "    Widget      widget,\n"
    "    XtPointer   client_data,\n"
    "    XtPointer   call_data\n"
    ")\n"
    "{\n"
    "    DtbObjectHelpData 	help_data = (DtbObjectHelpData)client_data;\n"
    "\n"
    "    if (help_data != (DtbObjectHelpData) NULL)\n"
    "    {\n"
    "	if (help_data->help_text)\n"
    "	    XtFree((char *)help_data->help_text);\n"
    "	if (help_data->help_volume)\n"
    "	    XtFree((char *)help_data->help_volume);\n"
    "	if (help_data->help_locationID)\n"
    "	    XtFree((char *)help_data->help_locationID);\n"
    "	XtFree((char *)help_data);\n"
    "    }\n"
    "\n"
    "    XtDestroyWidget(widget);\n"
    "}"
}; /* abmfP_lrc_destroyCB */
LibFunc abmfP_lib_destroyCB = &(abmfP_lrc_destroyCB);

/*
 * abmfP_lib_MessageBoxGetActionButton
 */
static LibFuncRec abmfP_lrc_MessageBoxGetActionButton = 
{
    /* name */
    "dtb_MessageBoxGetActionButton",
    
    /* proto */
    "Widget dtb_MessageBoxGetActionButton(\n"
    "    Widget	msg_dlg,\n"
    "    DTB_BUTTON	which_btn\n"
    ");",
    
    /* def */
    "/* \n"
    " * Get handle to Action2 button.\n"
    " */\n"
    "Widget\n"
    "dtb_MessageBoxGetActionButton(\n"
    "    Widget	msg_dlg,\n"
    "    DTB_BUTTON	which_btn\n"
    ")\n"
    "{\n"
    "    int			i, numChildren = 0;\n"
    "    WidgetList		children = NULL;\n"
    "    Widget		action_button = 0;\n"
    "    int			button = -1;\n"
    "    Boolean		Found = False;\n"
    "\n"
    "    XtVaGetValues(msg_dlg,\n"
    "			XmNnumChildren, &numChildren,\n"
    "			XmNchildren, &children,\n"
    "			NULL);\n"
    "    for (i = 0; i < numChildren && !Found; i++)\n"
    "    {\n"
    "	XtVaGetValues(children[i], XmNuserData, &button, NULL);\n"
    "	if (which_btn == (DTB_BUTTON) button)\n"
    "	{\n"
    "	    Found = True;\n"
    "	    action_button = children[i];\n"
    "	}\n"
    "    }\n"
    "    return (action_button);\n"
    "}"
}; /* abmfP_lrc_MessageBoxGetActionButton */
LibFunc abmfP_lib_MessageBoxGetActionButton = &(abmfP_lrc_MessageBoxGetActionButton);

/*
 * abmfP_lib_show_message
 */
static LibFuncRec abmfP_lrc_show_message = 
{
    /* name */
    "dtb_show_message",
    
    /* proto */
    "void dtb_show_message(\n"
    "    Widget		parent,\n"
    "    DtbMessageData	mbr,\n"
    "    XmString		override_msg,\n"
    "    DtbObjectHelpData	override_help\n"
    ");",
    
    /* def */
    "/*\n"
    " * Use this routine to post a non-modal message. It should\n"
    " * be used to post Information and Working messages.\n"
    " */\n"
    "void\n"
    "dtb_show_message(\n"
    "    Widget		parent,\n"
    "    DtbMessageData	mbr,\n"
    "    XmString		override_msg,\n"
    "    DtbObjectHelpData	override_help\n"
    ")\n"
    "{\n"
    "    DtbObjectHelpData	help_data_copy = (DtbObjectHelpData)NULL;\n"
    "    Widget      	msg_dlg = (Widget) 0, \n"
    "			action_btn = (Widget) 0;\n"
    "\n"
    "    if (override_help != (DtbObjectHelpData) NULL)\n"
    "    {\n"
    "	help_data_copy = (DtbObjectHelpData)XtMalloc(sizeof(DtbObjectHelpDataRec));\n"
    "\n"
    "	help_data_copy->help_text = override_help->help_text ? \n"
    "		XtNewString(override_help->help_text) : \n"
    "		NULL;\n"
    "	help_data_copy->help_volume = override_help->help_volume ? \n"
    "		XtNewString(override_help->help_volume) :\n"
    "		NULL;\n"
    "	help_data_copy->help_locationID = override_help->help_locationID ?\n"
    "		XtNewString(override_help->help_locationID) :\n"
    "		NULL;\n"
    "    }\n"
    "\n"
    "    msg_dlg = dtb_create_message_dlg(parent, \n"
    "		mbr, override_msg, help_data_copy);\n"
    "\n"
    "    if (msg_dlg == (Widget) 0)\n"
    "	return;\n"
    "\n"
    "    /* Add Callbacks if necessary */\n"
    "    if (mbr->action1_callback != (XtCallbackProc) NULL)\n"
    "        XtAddCallback(msg_dlg, XmNokCallback, mbr->action1_callback, NULL);\n"
    "    if (mbr->cancel_callback != (XtCallbackProc) NULL)\n"
    "        XtAddCallback(msg_dlg, XmNcancelCallback, mbr->cancel_callback, NULL);\n"
    "    if (mbr->action2_callback != (XtCallbackProc) NULL)\n"
    "    {\n"
    "	action_btn = dtb_MessageBoxGetActionButton(msg_dlg, DTB_ACTION2_BUTTON);\n"
    "	if (action_btn != 0)\n"
    "            XtAddCallback(action_btn, XmNactivateCallback,\n"
    "			  mbr->action2_callback, NULL);\n"
    "    }\n"
    "    if (mbr->action3_callback != (XtCallbackProc) NULL)\n"
    "    {\n"
    "        action_btn = dtb_MessageBoxGetActionButton(msg_dlg, DTB_ACTION3_BUTTON);        if (action_btn != 0)\n"
    "            XtAddCallback(action_btn, XmNactivateCallback,\n"
    "                          mbr->action3_callback, NULL);\n"
    "    }\n"
    "    if (mbr->help_button)\n"
    "    {\n"
    "	if (help_data_copy != (DtbObjectHelpData) NULL)\n"
    "	{\n"
    "            XtAddCallback(msg_dlg,\n"
    "                        XmNhelpCallback, dtb_help_dispatch,\n"
    "                        (XtPointer) help_data_copy);\n"
    "	}\n"
    "	else if (mbr->help_data.help_text != (char *) NULL)\n"
    "	{\n"
    "	    XtAddCallback(msg_dlg, \n"
    "			XmNhelpCallback, dtb_help_dispatch,\n"
    "			(XtPointer) &(mbr->help_data));\n"
    "	}\n"
    "    }\n"
    "\n"
    "    XtManageChild(msg_dlg);\n"
    "    XRaiseWindow(XtDisplay(msg_dlg), XtWindow(XtParent(msg_dlg)));\n"
    "}"
}; /* abmfP_lrc_show_message */
LibFunc abmfP_lib_show_message = &(abmfP_lrc_show_message);

/*
 * abmfP_lib_show_modal_message
 */
static LibFuncRec abmfP_lrc_show_modal_message = 
{
    /* name */
    "dtb_show_modal_message",
    
    /* proto */
    "DTB_MODAL_ANSWER dtb_show_modal_message(\n"
    "    Widget		parent,\n"
    "    DtbMessageData	mbr,\n"
    "    XmString		override_msg,\n"
    "    DtbObjectHelpData	override_help,\n"
    "    Widget         	*modal_dlg_pane_out_ptr\n"
    ");",
    
    /* def */
    "/*\n"
    " * Use this routine to post a modal message.\n"
    " * It should be used to post Error, Question, and Warning messages.\n"
    " * It returns the information on which button was pressed. A switch\n"
    " * statement can then be done to process the answer.\n"
    " */\n"
    "DTB_MODAL_ANSWER\n"
    "dtb_show_modal_message(\n"
    "    Widget		parent,\n"
    "    DtbMessageData	mbr,\n"
    "    XmString		override_msg,\n"
    "    DtbObjectHelpData	override_help,\n"
    "    Widget         	*modal_dlg_pane_out_ptr\n"
    ")                                 \n"
    "{\n"
    "    XtAppContext		app;\n"
    "    Widget			modal_dlg_pane = (Widget) 0;\n"
    "    Widget			action2_button = (Widget) 0;\n"
    "    Widget			action3_button = (Widget) 0;\n"
    "    DtbObjectHelpData		help_data_copy = (DtbObjectHelpData)NULL;\n"
    "    DTB_MODAL_ANSWER		answer = DTB_ANSWER_NONE;\n"
    "    XtCallbackRec ok_callback[] = {\n"
    "            {(XtCallbackProc)modal_dlgCB, (XtPointer) DTB_ANSWER_ACTION1},\n"
    "            {(XtCallbackProc) NULL, (XtPointer) NULL}\n"
    "    };\n"
    "    XtCallbackRec cancel_callback[] = {\n"
    "            {(XtCallbackProc)modal_dlgCB, (XtPointer) DTB_ANSWER_CANCEL},\n"
    "            {(XtCallbackProc) NULL, (XtPointer) NULL}\n"
    "    };\n"
    "\n"
    "    if (override_help != (DtbObjectHelpData) NULL)\n"
    "    {\n"
    "	help_data_copy = (DtbObjectHelpData)XtMalloc(sizeof(DtbObjectHelpDataRec));\n"
    "\n"
    "	help_data_copy->help_text = override_help->help_text ? \n"
    "		XtNewString(override_help->help_text) : \n"
    "		NULL;\n"
    "	help_data_copy->help_volume = override_help->help_volume ? \n"
    "		XtNewString(override_help->help_volume) :\n"
    "		NULL;\n"
    "	help_data_copy->help_locationID = override_help->help_locationID ?\n"
    "		XtNewString(override_help->help_locationID) :\n"
    "		NULL;\n"
    "    }\n"
    "\n"
    "    modal_dlg_pane = dtb_create_message_dlg(parent, mbr, \n"
    "				override_msg, help_data_copy);\n"
    "\n"
    "    if (modal_dlg_pane == (Widget) 0)\n"
    "	return (answer);\n"
    " \n"
    "    XtVaSetValues(modal_dlg_pane,\n"
    "                XmNdialogStyle,    XmDIALOG_FULL_APPLICATION_MODAL,\n"
    "                XmNokCallback,     &ok_callback,\n"
    "                XmNcancelCallback, &cancel_callback,\n"
    "                XmNuserData,       &answer,\n"
    "                NULL);\n"
    "\n"
    "    action2_button = dtb_MessageBoxGetActionButton(modal_dlg_pane,\n"
    "				DTB_ACTION2_BUTTON);\n"
    "    if (action2_button != (Widget) 0)\n"
    "    {\n"
    "	XtVaSetValues(action2_button,\n"
    "                        XmNuserData, (XtPointer) &answer,\n"
    "                        NULL);\n"
    "        XtAddCallback(action2_button,\n"
    "                        XmNactivateCallback, modal_dlgCB,\n"
    "                        (XtPointer) DTB_ANSWER_ACTION2);\n"
    "    }\n"
    "\n"
    "    action3_button = dtb_MessageBoxGetActionButton(modal_dlg_pane,\n"
    "                                DTB_ACTION3_BUTTON);\n"
    "    if (action3_button != (Widget) NULL)\n"
    "    {\n"
    "        XtVaSetValues(action3_button,\n"
    "                        XmNuserData, (XtPointer) &answer,\n"
    "                        NULL);\n"
    "        XtAddCallback(action3_button,\n"
    "                        XmNactivateCallback, modal_dlgCB,\n"
    "                        (XtPointer) DTB_ANSWER_ACTION3);\n"
    "    }\n"
    "\n"
    "    if (mbr->help_button) \n"
    "    { \n"
    "        if (help_data_copy != (DtbObjectHelpData) NULL)\n"
    "        {\n"
    "            XtAddCallback(modal_dlg_pane, \n"
    "                        XmNhelpCallback, dtb_help_dispatch, \n"
    "                        (XtPointer) help_data_copy); \n"
    "	}\n"
    "        else if (mbr->help_data.help_text != (char *) NULL) \n"
    "        { \n"
    "            XtAddCallback(modal_dlg_pane, \n"
    "                        XmNhelpCallback, dtb_help_dispatch, \n"
    "                        (XtPointer) &(mbr->help_data)); \n"
    "        }\n"
    "    }   \n"
    "\n"
    "    /* Popup Modal MessageDialog and wait for answer */\n"
    "    XtManageChild(modal_dlg_pane);\n"
    "    XRaiseWindow(XtDisplay(modal_dlg_pane), XtWindow(XtParent(modal_dlg_pane)));\n"
    "\n"
    "    app = XtDisplayToApplicationContext(XtDisplay(modal_dlg_pane));\n"
    "    while (answer == DTB_ANSWER_NONE)\n"
    "        XtAppProcessEvent(app, XtIMAll);\n"
    " \n"
    "    if (modal_dlg_pane_out_ptr != NULL)\n"
    "    {\n"
    "        (*modal_dlg_pane_out_ptr) = modal_dlg_pane;\n"
    "    }\n"
    "    return(answer);\n"
    "}"
}; /* abmfP_lrc_show_modal_message */
LibFunc abmfP_lib_show_modal_message = &(abmfP_lrc_show_modal_message);

/*
 * abmfP_lib_modal_dlgCB
 */
static LibFuncRec abmfP_lrc_modal_dlgCB = 
{
    /* name */
    "modal_dlgCB",
    
    /* proto */
    "static void modal_dlgCB(\n"
    "    Widget      widget,\n"
    "    XtPointer   client_data,\n"
    "    XtPointer   call_data\n"
    ");",
    
    /* def */
    "/*\n"
    " * This is the activateCallback for the MessageBox buttons.\n"
    " * It returns the button which was pressed.\n"
    " */\n"
    "static void\n"
    "modal_dlgCB(\n"
    "    Widget      widget,\n"
    "    XtPointer   client_data,\n"
    "    XtPointer   call_data\n"
    ")\n"
    "{\n"
    "    DTB_MODAL_ANSWER     op = (DTB_MODAL_ANSWER) client_data;\n"
    "    DTB_MODAL_ANSWER     *answerp = NULL;\n"
    " \n"
    "    XtVaGetValues(widget, XmNuserData, &answerp, NULL);\n"
    " \n"
    "    /* Will cause Modal dialog to return */\n"
    "    *answerp = op;\n"
    "}"
}; /* abmfP_lrc_modal_dlgCB */
LibFunc abmfP_lib_modal_dlgCB = &(abmfP_lrc_modal_dlgCB);

/*
 * abmfP_lib_children_center
 */
static LibFuncRec abmfP_lrc_children_center = 
{
    /* name */
    "dtb_children_center",
    
    /* proto */
    "void dtb_children_center(\n"
    "    Widget		form,\n"
    "    DTB_CENTERING_TYPES	type\n"
    ");",
    
    /* def */
    "/*\n"
    " * This function will center all the passed form's children.\n"
    " * The type of centering depends on what 'type' is.\n"
    " */\n"
    "void\n"
    "dtb_children_center(\n"
    "    Widget		form,\n"
    "    DTB_CENTERING_TYPES	type\n"
    ")\n"
    "{\n"
    "    WidgetList		children_list;\n"
    "    int			i, \n"
    "			num_children;\n"
    "\n"
    "    if (!form || (type == DTB_CENTER_NONE))\n"
    "	return;\n"
    "\n"
    "    /*\n"
    "     * Get children list\n"
    "     */\n"
    "    XtVaGetValues(form,\n"
    "            XmNnumChildren, &num_children,\n"
    "            XmNchildren, &children_list,\n"
    "            NULL);\n"
    "\n"
    "    /*\n"
    "     * Center all children\n"
    "     */\n"
    "    for (i=0; i < num_children; ++i)\n"
    "    {\n"
    "	dtb_center(children_list[i], type);\n"
    "    }\n"
    "}"
}; /* abmfP_lrc_children_center */
LibFunc abmfP_lib_children_center = &(abmfP_lrc_children_center);

/*
 * abmfP_lib_children_uncenter
 */
static LibFuncRec abmfP_lrc_children_uncenter = 
{
    /* name */
    "dtb_children_uncenter",
    
    /* proto */
    "void dtb_children_uncenter(\n"
    "    Widget		form,\n"
    "    DTB_CENTERING_TYPES	type\n"
    ");",
    
    /* def */
    "/*\n"
    " * This function 'uncenters' the children of the passed\n"
    " * form widget.\n"
    " */\n"
    "void\n"
    "dtb_children_uncenter(\n"
    "    Widget		form,\n"
    "    DTB_CENTERING_TYPES	type\n"
    ")\n"
    "{\n"
    "    WidgetList		children_list;\n"
    "    int			i;\n"
    "    int			num_children;\n"
    "\n"
    "    if (!form || (type == DTB_CENTER_NONE))\n"
    "	return;\n"
    "\n"
    "    /*\n"
    "     * Get children list\n"
    "     */\n"
    "    XtVaGetValues(form,\n"
    "            XmNnumChildren, &num_children,\n"
    "            XmNchildren, &children_list,\n"
    "            NULL);\n"
    "\n"
    "    /*\n"
    "     * Center all children\n"
    "     */\n"
    "    for (i=0; i < num_children; ++i)\n"
    "    {\n"
    "	dtb_uncenter(children_list[i], type);\n"
    "    }\n"
    "}"
}; /* abmfP_lrc_children_uncenter */
LibFunc abmfP_lib_children_uncenter = &(abmfP_lrc_children_uncenter);

/*
 * abmfP_lib_center
 */
static LibFuncRec abmfP_lrc_center = 
{
    /* name */
    "dtb_center",
    
    /* proto */
    "void dtb_center(\n"
    "    Widget		form_child,\n"
    "    DTB_CENTERING_TYPES	type\n"
    ");",
    
    /* def */
    "/*\n"
    " * This function centers the passed widget.\n"
    " * This is done by setting the proper offsets.\n"
    " * Dynamic centering is accomplished by attaching an event handler\n"
    " * which detect resizes and recomputes and sets the appropriate offset.\n"
    " */\n"
    "void\n"
    "dtb_center(\n"
    "    Widget		form_child,\n"
    "    DTB_CENTERING_TYPES	type\n"
    ")\n"
    "{\n"
    "    if (!form_child || (type == DTB_CENTER_NONE))\n"
    "	return;\n"
    "\n"
    "   center_widget(form_child, type);\n"
    "\n"
    "   XtAddEventHandler(form_child,\n"
    "            StructureNotifyMask, False,\n"
    "            centering_handler, (XtPointer)type);\n"
    "}"
}; /* abmfP_lrc_center */
LibFunc abmfP_lib_center = &(abmfP_lrc_center);

/*
 * abmfP_lib_uncenter
 */
static LibFuncRec abmfP_lrc_uncenter = 
{
    /* name */
    "dtb_uncenter",
    
    /* proto */
    "void dtb_uncenter(\n"
    "    Widget		form_child,\n"
    "    DTB_CENTERING_TYPES	type\n"
    ");",
    
    /* def */
    "/*\n"
    " * This function 'uncenters' the passed widget.\n"
    " * This involves resetting the attachment offsets\n"
    " * and removing the resize event handler.\n"
    " */\n"
    "void\n"
    "dtb_uncenter(\n"
    "    Widget		form_child,\n"
    "    DTB_CENTERING_TYPES	type\n"
    ")\n"
    "{\n"
    "    if (!form_child || (type == DTB_CENTER_NONE))\n"
    "	return;\n"
    "\n"
    "   uncenter_widget(form_child, type);\n"
    "\n"
    "   XtRemoveEventHandler(form_child,\n"
    "            StructureNotifyMask, False,\n"
    "            centering_handler, (XtPointer)type);\n"
    "}"
}; /* abmfP_lrc_uncenter */
LibFunc abmfP_lib_uncenter = &(abmfP_lrc_uncenter);

/*
 * abmfP_lib_center_widget
 */
static LibFuncRec abmfP_lrc_center_widget = 
{
    /* name */
    "center_widget",
    
    /* proto */
    "static void  center_widget(\n"
    "    Widget		form_child,\n"
    "    DTB_CENTERING_TYPES	type\n"
    ");",
    
    /* def */
    "/*\n"
    " * This function centers the passed widget.\n"
    " * This is done by making the appropriate offset equal \n"
    " * to the negative half of it's width/height (depending\n"
    " * on whether horizontal or vertical centering was chosen.\n"
    " */\n"
    "static void \n"
    "center_widget(\n"
    "    Widget		form_child,\n"
    "    DTB_CENTERING_TYPES	type\n"
    ")\n"
    "{\n"
    "    Widget		parent;\n"
    "    Dimension		width = 0,\n"
    "			height = 0;\n"
    "    int			center_offset;\n"
    "    unsigned char	left_attach = XmATTACH_NONE,\n"
    "			top_attach = XmATTACH_NONE;\n"
    "\n"
    "    if (!form_child || !XtIsManaged(form_child) || !XtIsRealized(form_child))\n"
    "	return;\n"
    "\n"
    "    parent = XtParent(form_child);\n"
    "\n"
    "    if (!parent || !XtIsSubclass(parent, xmFormWidgetClass))\n"
    "	return;\n"
    "\n"
    "    XtVaGetValues(form_child,\n"
    "		XmNwidth, &width,\n"
    "		XmNheight, &height,\n"
    "		XmNleftAttachment, &left_attach,\n"
    "		XmNtopAttachment, &top_attach,\n"
    "                NULL);\n"
    "\n"
    "    switch (type) {\n"
    "	case DTB_CENTER_POSITION_VERT:\n"
    "\n"
    "            if (left_attach != XmATTACH_POSITION)\n"
    "	        return;\n"
    "\n"
    "            center_offset = -(width/2);\n"
    "\n"
    "            XtVaSetValues(form_child,\n"
    "		XmNleftOffset, center_offset,\n"
    "		NULL);\n"
    "\n"
    "	break;\n"
    "\n"
    "	case DTB_CENTER_POSITION_HORIZ:\n"
    "\n"
    "            if (top_attach != XmATTACH_POSITION)\n"
    "	        return;\n"
    "\n"
    "            center_offset = -(height/2);\n"
    "\n"
    "            XtVaSetValues(form_child,\n"
    "		XmNtopOffset, center_offset,\n"
    "		NULL);\n"
    "	break;\n"
    "\n"
    "	case DTB_CENTER_POSITION_BOTH:\n"
    "	{\n"
    "            int		left_offset,\n"
    "			top_offset;\n"
    "\n"
    "            if ((left_attach != XmATTACH_POSITION) &&\n"
    "	        (top_attach != XmATTACH_POSITION))\n"
    "	        return;\n"
    "	    \n"
    "	    left_offset = -(width/2);\n"
    "	    top_offset = -(height/2);\n"
    "\n"
    "            XtVaSetValues(form_child,\n"
    "		XmNleftOffset, left_offset,\n"
    "		XmNtopOffset, top_offset,\n"
    "		NULL);\n"
    "	}\n"
    "	break;\n"
    "    }\n"
    "}"
}; /* abmfP_lrc_center_widget */
LibFunc abmfP_lib_center_widget = &(abmfP_lrc_center_widget);

/*
 * abmfP_lib_uncenter_widget
 */
static LibFuncRec abmfP_lrc_uncenter_widget = 
{
    /* name */
    "uncenter_widget",
    
    /* proto */
    "static void  uncenter_widget(\n"
    "    Widget		form_child,\n"
    "    DTB_CENTERING_TYPES	type\n"
    ");",
    
    /* def */
    "/*\n"
    " * This function 'uncenters' the passed widget.\n"
    " * It merely resets the offsets of the top/left attachments to 0.\n"
    " */\n"
    "static void \n"
    "uncenter_widget(\n"
    "    Widget		form_child,\n"
    "    DTB_CENTERING_TYPES	type\n"
    ")\n"
    "{\n"
    "    Widget		parent;\n"
    "    unsigned char	left_attach = XmATTACH_NONE,\n"
    "			top_attach = XmATTACH_NONE;\n"
    "\n"
    "    if (!form_child || !XtIsManaged(form_child) || !XtIsRealized(form_child))\n"
    "	return;\n"
    "\n"
    "    parent = XtParent(form_child);\n"
    "\n"
    "    if (!parent || !XtIsSubclass(parent, xmFormWidgetClass))\n"
    "	return;\n"
    "\n"
    "    XtVaGetValues(form_child,\n"
    "		XmNleftAttachment, &left_attach,\n"
    "		XmNtopAttachment, &top_attach,\n"
    "                NULL);\n"
    "\n"
    "    switch (type) {\n"
    "	case DTB_CENTER_POSITION_VERT:\n"
    "\n"
    "            if (left_attach != XmATTACH_POSITION)\n"
    "	        return;\n"
    "\n"
    "            XtVaSetValues(form_child,\n"
    "		XmNleftOffset, 0,\n"
    "		NULL);\n"
    "\n"
    "	break;\n"
    "\n"
    "	case DTB_CENTER_POSITION_HORIZ:\n"
    "\n"
    "            if (top_attach != XmATTACH_POSITION)\n"
    "	        return;\n"
    "\n"
    "            XtVaSetValues(form_child,\n"
    "		XmNtopOffset, 0,\n"
    "		NULL);\n"
    "	break;\n"
    "\n"
    "	case DTB_CENTER_POSITION_BOTH:\n"
    "\n"
    "            if ((left_attach != XmATTACH_POSITION) &&\n"
    "	        (top_attach != XmATTACH_POSITION))\n"
    "	        return;\n"
    "\n"
    "            XtVaSetValues(form_child,\n"
    "		XmNleftOffset, 0,\n"
    "		XmNtopOffset, 0,\n"
    "		NULL);\n"
    "	break;\n"
    "    }\n"
    "}"
}; /* abmfP_lrc_uncenter_widget */
LibFunc abmfP_lib_uncenter_widget = &(abmfP_lrc_uncenter_widget);

/*
 * abmfP_lib_centering_handler
 */
static LibFuncRec abmfP_lrc_centering_handler = 
{
    /* name */
    "centering_handler",
    
    /* proto */
    "static void centering_handler(\n"
    "    Widget	widget,\n"
    "    XtPointer	client_data,\n"
    "    XEvent	*event,\n"
    "    Boolean	*cont_dispatch\n"
    ");",
    
    /* def */
    "/*\n"
    " * Event handler to center a widget\n"
    " * The type of centering needed is passed in as client_data\n"
    " */\n"
    "static void\n"
    "centering_handler(\n"
    "    Widget	widget,\n"
    "    XtPointer	client_data,\n"
    "    XEvent	*event,\n"
    "    Boolean	*cont_dispatch\n"
    ")\n"
    "{\n"
    "    XConfigureEvent	*xcon = &event->xconfigure;\n"
    "    Widget		resized_child;\n"
    "    DTB_CENTERING_TYPES	type = (DTB_CENTERING_TYPES)client_data;\n"
    "\n"
    "\n"
    "    if ((event->type != ConfigureNotify) && (event->type != MapNotify))\n"
    "        return;\n"
    "\n"
    "    resized_child = XtWindowToWidget(XtDisplay(widget), xcon->window);\n"
    "\n"
    "    if (!resized_child)\n"
    "	return;\n"
    "\n"
    "    center_widget(resized_child, type);\n"
    "}"
}; /* abmfP_lrc_centering_handler */
LibFunc abmfP_lib_centering_handler = &(abmfP_lrc_centering_handler);

/*
 * abmfP_lib_get_label_widget
 */
static LibFuncRec abmfP_lrc_get_label_widget = 
{
    /* name */
    "get_label_widget",
    
    /* proto */
    "static Widget get_label_widget(\n"
    "    Widget	widget\n"
    ");",
    
    /* def */
    "/*\n"
    " * Given a widget, return it's label widget.\n"
    " */\n"
    "static Widget\n"
    "get_label_widget(\n"
    "    Widget	widget\n"
    ")\n"
    "{\n"
    "    WidgetList	children_list;\n"
    "    Widget	label_widget = 0;\n"
    "    int		i,\n"
    "		num_children = 0;\n"
    "    char	*subobj_name = NULL,\n"
    "		*label_name = NULL;\n"
    "    char	*underscore_ptr = NULL;\n"
    "\n"
    "    if (XtIsSubclass(widget, xmLabelWidgetClass))  {\n"
    "	return(widget);\n"
    "    }\n"
    "\n"
    "    subobj_name = XtName(widget);\n"
    "    label_name = (char *)XtMalloc(1 + strlen(subobj_name) + strlen(\"_label\") + 5);\n"
    "    label_name[0] = '*';\n"
    "    strcpy(label_name+1, subobj_name);\n"
    "    if ((underscore_ptr = strrchr(label_name, '_')) != NULL)\n"
    "    {\n"
    "	strcpy(underscore_ptr, \"_label\");\n"
    "        label_widget = XtNameToWidget(widget, label_name);\n"
    "    }\n"
    "    if (label_widget == 0)\n"
    "    {\n"
    "	strcpy(label_name+1, subobj_name);\n"
    "	strcat(label_name, \"_label\");\n"
    "        label_widget = XtNameToWidget(widget, label_name);\n"
    "    }\n"
    "\n"
    "    XtFree((char *)label_name);\n"
    "\n"
    "    if (label_widget)\n"
    "	return(label_widget);\n"
    "\n"
    "    /*\n"
    "     * How to look for 1st child of group object ??\n"
    "     * How do we know if 'widget' is a group object ??\n"
    "     * For now, just check if it is a form\n"
    "     */\n"
    "    if (XtIsSubclass(widget, xmFormWidgetClass) || \n"
    "        XtIsSubclass(widget, xmFrameWidgetClass))\n"
    "        XtVaGetValues(widget,\n"
    "            XmNnumChildren, &num_children,\n"
    "            XmNchildren, &children_list,\n"
    "            NULL);\n"
    "\n"
    "    if (num_children > 0)\n"
    "        return(get_label_widget(children_list[0]));\n"
    "\n"
    "    return (0);\n"
    "}"
}; /* abmfP_lrc_get_label_widget */
LibFunc abmfP_lib_get_label_widget = &(abmfP_lrc_get_label_widget);

/*
 * abmfP_lib_get_offset_from_ancestor
 */
static LibFuncRec abmfP_lrc_get_offset_from_ancestor = 
{
    /* name */
    "get_offset_from_ancestor",
    
    /* proto */
    "static Position get_offset_from_ancestor(\n"
    "    Widget	ancestor,\n"
    "    Widget	w\n"
    ");",
    
    /* def */
    "static Position\n"
    "get_offset_from_ancestor(\n"
    "    Widget	ancestor,\n"
    "    Widget	w\n"
    ")\n"
    "{\n"
    "    Widget	cur = w;\n"
    "    Widget	cur_parent = 0;\n"
    "    Position	offset = 0;\n"
    "\n"
    "    if (!ancestor || !w || (w == ancestor))\n"
    "	return (0);\n"
    "\n"
    "    XtVaGetValues(cur, XmNx, &offset, NULL);\n"
    "\n"
    "    cur_parent = XtParent(cur);\n"
    "\n"
    "    while (cur_parent != ancestor)\n"
    "    {\n"
    "        Position	tmp_offset = 0;\n"
    "\n"
    "	cur = cur_parent;\n"
    "        XtVaGetValues(cur, XmNx, &tmp_offset, NULL);\n"
    "	\n"
    "	offset += tmp_offset;\n"
    "        cur_parent = XtParent(cur);\n"
    "    }\n"
    "\n"
    "    return (offset);\n"
    "}"
}; /* abmfP_lrc_get_offset_from_ancestor */
LibFunc abmfP_lib_get_offset_from_ancestor = &(abmfP_lrc_get_offset_from_ancestor);

/*
 * abmfP_lib_get_label_width
 */
static LibFuncRec abmfP_lrc_get_label_width = 
{
    /* name */
    "get_label_width",
    
    /* proto */
    "static Dimension get_label_width(\n"
    "    Widget	widget\n"
    ");",
    
    /* def */
    "static Dimension\n"
    "get_label_width(\n"
    "    Widget	widget\n"
    ")\n"
    "{\n"
    "    WidgetList	children_list;\n"
    "    Widget	lbl_widget = 0;\n"
    "    Dimension	lbl_width = 0;\n"
    "\n"
    "    lbl_widget = get_label_widget(widget);\n"
    "\n"
    "    if (lbl_widget)\n"
    "    {\n"
    "	Position	offset = 0;\n"
    "\n"
    "        XtVaGetValues(lbl_widget,\n"
    "            XmNwidth, &lbl_width,\n"
    "            NULL);\n"
    "\n"
    "	offset = get_offset_from_ancestor(widget, lbl_widget);\n"
    "\n"
    "	lbl_width += (Dimension)offset;\n"
    "    }\n"
    "    \n"
    "    return (lbl_width);\n"
    "}"
}; /* abmfP_lrc_get_label_width */
LibFunc abmfP_lib_get_label_width = &(abmfP_lrc_get_label_width);

/*
 * abmfP_lib_get_widest_label
 */
static LibFuncRec abmfP_lrc_get_widest_label = 
{
    /* name */
    "get_widest_label",
    
    /* proto */
    "static void get_widest_label(\n"
    "    WidgetList	list,\n"
    "    int		count,\n"
    "    Widget	*child_widget,\n"
    "    Dimension	*label_width\n"
    ");",
    
    /* def */
    "static void\n"
    "get_widest_label(\n"
    "    WidgetList	list,\n"
    "    int		count,\n"
    "    Widget	*child_widget,\n"
    "    Dimension	*label_width\n"
    ")\n"
    "{\n"
    "    Widget	cur_widest = 0;\n"
    "    Dimension	cur_width = 0;\n"
    "    int		i;\n"
    "\n"
    "    for (i = 0; i < count; ++i)\n"
    "    {\n"
    "	Dimension	tmp;\n"
    "\n"
    "	tmp = get_label_width(list[i]);\n"
    "\n"
    "	if (tmp > cur_width)\n"
    "	{\n"
    "	    cur_width = tmp;\n"
    "	    cur_widest = list[i];\n"
    "	}\n"
    "    }\n"
    "    \n"
    "    *child_widget = cur_widest;\n"
    "    *label_width = cur_width;\n"
    "}"
}; /* abmfP_lrc_get_widest_label */
LibFunc abmfP_lib_get_widest_label = &(abmfP_lrc_get_widest_label);

/*
 * abmfP_lib_get_widest_value
 */
static LibFuncRec abmfP_lrc_get_widest_value = 
{
    /* name */
    "get_widest_value",
    
    /* proto */
    "static void get_widest_value(\n"
    "    WidgetList	list,\n"
    "    int		count,\n"
    "    Widget	*child_widget,\n"
    "    Dimension	*value_width\n"
    ");",
    
    /* def */
    "static void\n"
    "get_widest_value(\n"
    "    WidgetList	list,\n"
    "    int		count,\n"
    "    Widget	*child_widget,\n"
    "    Dimension	*value_width\n"
    ")\n"
    "{\n"
    "    Widget	cur_widest = 0;\n"
    "    Dimension	cur_width = 0;\n"
    "    int		i;\n"
    "\n"
    "    for (i = 0; i < count; ++i)\n"
    "    {\n"
    "	Dimension	tmp, label_width, obj_width = 0;\n"
    "\n"
    "	label_width = get_label_width(list[i]);\n"
    "	XtVaGetValues(list[i], XmNwidth, &obj_width, NULL);\n"
    "\n"
    "	tmp = obj_width - label_width;\n"
    "\n"
    "	if (tmp > cur_width)\n"
    "	{\n"
    "	    cur_width = tmp;\n"
    "	    cur_widest = list[i];\n"
    "	}\n"
    "    }\n"
    "    \n"
    "    *child_widget = cur_widest;\n"
    "    *value_width = cur_width;\n"
    "}"
}; /* abmfP_lrc_get_widest_value */
LibFunc abmfP_lib_get_widest_value = &(abmfP_lrc_get_widest_value);

/*
 * abmfP_lib_get_widget_rect
 */
static LibFuncRec abmfP_lrc_get_widget_rect = 
{
    /* name */
    "get_widget_rect",
    
    /* proto */
    "static void get_widget_rect(\n"
    "    Widget widget,\n"
    "    XRectangle *rect\n"
    ");",
    
    /* def */
    "static void\n"
    "get_widget_rect(\n"
    "    Widget widget,\n"
    "    XRectangle *rect\n"
    ")\n"
    "{\n"
    "    if (!rect)\n"
    "	return;\n"
    "\n"
    "    XtVaGetValues(widget,\n"
    "        XtNwidth,       (XtArgVal)&(rect->width),\n"
    "        XtNheight,      (XtArgVal)&(rect->height),\n"
    "        XtNx,           (XtArgVal)&(rect->x),\n"
    "        XtNy,           (XtArgVal)&(rect->y),\n"
    "        NULL);\n"
    "}"
}; /* abmfP_lrc_get_widget_rect */
LibFunc abmfP_lib_get_widget_rect = &(abmfP_lrc_get_widget_rect);

/*
 * abmfP_lib_get_greatest_size
 */
static LibFuncRec abmfP_lrc_get_greatest_size = 
{
    /* name */
    "get_greatest_size",
    
    /* proto */
    "static void get_greatest_size(\n"
    "    Widget	*list,\n"
    "    int		count,\n"
    "    int		*width,\n"
    "    int		*height,\n"
    "    Widget	*tallest,\n"
    "    Widget	*widest\n"
    ");",
    
    /* def */
    "static void\n"
    "get_greatest_size(\n"
    "    Widget	*list,\n"
    "    int		count,\n"
    "    int		*width,\n"
    "    int		*height,\n"
    "    Widget	*tallest,\n"
    "    Widget	*widest\n"
    ")\n"
    "{\n"
    "    XRectangle  w_rect;\n"
    "    int         i;\n"
    "    int		previous_width, previous_height;\n"
    "\n"
    "    if (!list || (count < 0))\n"
    "	return;\n"
    "\n"
    "    get_widget_rect(list[0], &w_rect);\n"
    "\n"
    "    *width = w_rect.width;\n"
    "    previous_width = *width;\n"
    "\n"
    "    *height = w_rect.height;\n"
    "    previous_height = *height;\n"
    "\n"
    "    if (tallest != NULL)\n"
    "        *tallest = list[0];\n"
    "\n"
    "    if (widest != NULL)\n"
    "    	*widest = list[0];\n"
    "\n"
    "    for (i=0; i < count; i++)\n"
    "    {\n"
    "        get_widget_rect(list[i], &w_rect);\n"
    "\n"
    "        *width = max((int) w_rect.width, (int) *width); \n"
    "	if (widest != NULL && *width > previous_width)\n"
    "		*widest = list[i];\n"
    "\n"
    "        *height = max((int) w_rect.height, (int)*height);\n"
    "	if (tallest != NULL && *height > previous_height)\n"
    "		*tallest = list[i];\n"
    "    }\n"
    "}"
}; /* abmfP_lrc_get_greatest_size */
LibFunc abmfP_lib_get_greatest_size = &(abmfP_lrc_get_greatest_size);

/*
 * abmfP_lib_get_group_cell_size
 */
static LibFuncRec abmfP_lrc_get_group_cell_size = 
{
    /* name */
    "get_group_cell_size",
    
    /* proto */
    "static void get_group_cell_size(\n"
    "    Widget		parent,\n"
    "    DtbGroupInfo	*group_info,\n"
    "    int			*cell_width,\n"
    "    int			*cell_height\n"
    ");",
    
    /* def */
    "static void\n"
    "get_group_cell_size(\n"
    "    Widget		parent,\n"
    "    DtbGroupInfo	*group_info,\n"
    "    int			*cell_width,\n"
    "    int			*cell_height\n"
    ")\n"
    "{\n"
    "    WidgetList	children_list = NULL;\n"
    "    int		i,\n"
    "		num_children = 0;\n"
    "\n"
    "    /*\n"
    "     * Get children list\n"
    "     */\n"
    "    XtVaGetValues(parent,\n"
    "            XmNnumChildren, &num_children,\n"
    "            XmNchildren, &children_list,\n"
    "            NULL);\n"
    "\n"
    "    get_greatest_size(children_list, num_children,\n"
    "		cell_width, cell_height,\n"
    "		(Widget *)NULL, (Widget *)NULL);\n"
    "}"
}; /* abmfP_lrc_get_group_cell_size */
LibFunc abmfP_lib_get_group_cell_size = &(abmfP_lrc_get_group_cell_size);

/*
 * abmfP_lib_get_group_row_col
 */
static LibFuncRec abmfP_lrc_get_group_row_col = 
{
    /* name */
    "get_group_row_col",
    
    /* proto */
    "static void get_group_row_col(\n"
    "    Widget		parent,\n"
    "    DtbGroupInfo	*group_info,\n"
    "    int			*rows,\n"
    "    int			*cols\n"
    ");",
    
    /* def */
    "static void\n"
    "get_group_row_col(\n"
    "    Widget		parent,\n"
    "    DtbGroupInfo	*group_info,\n"
    "    int			*rows,\n"
    "    int			*cols\n"
    ")\n"
    "{\n"
    "    WidgetList	children_list = NULL;\n"
    "    int		num_rows,\n"
    "		num_cols,\n"
    "    		num_children;\n"
    "\n"
    "    if (!parent || !group_info)\n"
    "    {\n"
    "	*rows = *cols = -1;\n"
    "\n"
    "	return;\n"
    "    }\n"
    "\n"
    "    /*\n"
    "     * Get children list\n"
    "     */\n"
    "    XtVaGetValues(parent,\n"
    "            XmNnumChildren, &num_children,\n"
    "            XmNchildren, &children_list,\n"
    "            NULL);\n"
    "\n"
    "    num_rows = group_info->num_rows;\n"
    "    num_cols = group_info->num_cols;\n"
    "\n"
    "    if ((num_rows <= 0) && (num_cols <= 0))\n"
    "    {\n"
    "	*rows = *cols = -1;\n"
    "\n"
    "	return;\n"
    "    }\n"
    "\n"
    "    if (num_cols <= 0)\n"
    "        num_cols = (num_children/num_rows) + ((num_children % num_rows) ? 1 : 0);\n"
    "\n"
    "    if (num_rows <= 0)\n"
    "        num_rows = (num_children/num_cols) + ((num_children % num_cols) ? 1 : 0);\n"
    "\n"
    "    *rows = num_rows;\n"
    "    *cols = num_cols;\n"
    "}"
}; /* abmfP_lrc_get_group_row_col */
LibFunc abmfP_lib_get_group_row_col = &(abmfP_lrc_get_group_row_col);

/*
 * abmfP_lib_get_group_child
 */
static LibFuncRec abmfP_lrc_get_group_child = 
{
    /* name */
    "get_group_child",
    
    /* proto */
    "static Widget get_group_child(\n"
    "    Widget		parent,\n"
    "    DtbGroupInfo	*group_info,\n"
    "    int			x_pos,\n"
    "    int			y_pos\n"
    ");",
    
    /* def */
    "static Widget\n"
    "get_group_child(\n"
    "    Widget		parent,\n"
    "    DtbGroupInfo	*group_info,\n"
    "    int			x_pos,\n"
    "    int			y_pos\n"
    ")\n"
    "{\n"
    "    DTB_GROUP_TYPES	group_type;\n"
    "    WidgetList		children_list = NULL;\n"
    "    Widget		ret_child = 0;\n"
    "    int			num_children = 0,\n"
    "			num_rows,\n"
    "			num_columns,\n"
    "    			i = -1;\n"
    "\n"
    "    if (!parent || !group_info || \n"
    "       (x_pos < 0) || (y_pos < 0))\n"
    "	return (0);\n"
    "\n"
    "    group_type = group_info->group_type;\n"
    "    num_rows = group_info->num_rows;\n"
    "    num_columns = group_info->num_cols;\n"
    "\n"
    "    /*\n"
    "     * Get number of children\n"
    "     */\n"
    "    XtVaGetValues(parent,\n"
    "            XmNnumChildren, &num_children,\n"
    "            XmNchildren, &children_list,\n"
    "            NULL);\n"
    "\n"
    "    if (num_children <= 0)\n"
    "	return (0);\n"
    "\n"
    "    switch (group_type)\n"
    "    {\n"
    "	case DTB_GROUP_NONE:\n"
    "	break;\n"
    "\n"
    "	case DTB_GROUP_ROWS:\n"
    "	    /*\n"
    "	     * num_rows = 1\n"
    "	     * y_pos is ignored\n"
    "	     */\n"
    "	    i = x_pos;\n"
    "	break;\n"
    "\n"
    "	case DTB_GROUP_COLUMNS:\n"
    "	    /*\n"
    "	     * num_columns = 1\n"
    "	     * x_pos is ignored\n"
    "	     */\n"
    "	    i = y_pos;\n"
    "	break;\n"
    "\n"
    "	case DTB_GROUP_ROWSCOLUMNS:\n"
    "	    if (!num_rows && !num_columns)\n"
    "		break;\n"
    "\n"
    "	    if (num_rows > 0)\n"
    "	    {\n"
    "		/*\n"
    "		 * ROWFIRST\n"
    "		 */\n"
    "		if (y_pos < num_rows)\n"
    "		    i = (x_pos * num_rows) + y_pos;\n"
    "	    }\n"
    "	    else\n"
    "	    {\n"
    "		/*\n"
    "		 * COLFIRST\n"
    "		 */\n"
    "		if (x_pos < num_columns)\n"
    "		    i = x_pos + (y_pos * num_columns);\n"
    "	    }\n"
    "	break;\n"
    "\n"
    "	default:\n"
    "	break;\n"
    "    }\n"
    "\n"
    "    if ((i >= 0) && (i < num_children))\n"
    "    {\n"
    "        ret_child = children_list[i];\n"
    "    }\n"
    "\n"
    "    return (ret_child);\n"
    "}"
}; /* abmfP_lrc_get_group_child */
LibFunc abmfP_lib_get_group_child = &(abmfP_lrc_get_group_child);

/*
 * abmfP_lib_children_align
 */
static LibFuncRec abmfP_lrc_children_align = 
{
    /* name */
    "dtb_children_align",
    
    /* proto */
    "void dtb_children_align(\n"
    "    Widget		parent,\n"
    "    DTB_GROUP_TYPES	group_type,\n"
    "    DTB_ALIGN_TYPES	row_align,\n"
    "    DTB_ALIGN_TYPES	col_align,\n"
    "    int			margin,\n"
    "    int			num_rows,\n"
    "    int			num_cols,\n"
    "    int			hoffset,\n"
    "    int			voffset\n"
    ");",
    
    /* def */
    "void\n"
    "dtb_children_align(\n"
    "    Widget		parent,\n"
    "    DTB_GROUP_TYPES	group_type,\n"
    "    DTB_ALIGN_TYPES	row_align,\n"
    "    DTB_ALIGN_TYPES	col_align,\n"
    "    int			margin,\n"
    "    int			num_rows,\n"
    "    int			num_cols,\n"
    "    int			hoffset,\n"
    "    int			voffset\n"
    ")\n"
    "{\n"
    "    DtbGroupInfo		*group_info;\n"
    "\n"
    "    switch (group_type)\n"
    "    {\n"
    "	case DTB_GROUP_COLUMNS:\n"
    "	    num_rows = 0;\n"
    "	    num_cols = 1;\n"
    "	break;\n"
    "\n"
    "	case DTB_GROUP_ROWS:\n"
    "	    num_rows = 1;\n"
    "	    num_cols = 0;\n"
    "	break;\n"
    "\n"
    "    }\n"
    "\n"
    "    group_info = (DtbGroupInfo *)XtMalloc(sizeof(DtbGroupInfo));\n"
    "\n"
    "    group_info->group_type = group_type;\n"
    "    group_info->row_align = row_align;\n"
    "    group_info->col_align = col_align;\n"
    "    group_info->margin = margin;\n"
    "    group_info->num_rows = num_rows;\n"
    "    group_info->num_cols = num_cols;\n"
    "    group_info->hoffset = hoffset;\n"
    "    group_info->voffset = voffset;\n"
    "    group_info->ref_widget = 0;\n"
    "\n"
    "    align_children(parent, group_info, True);\n"
    "\n"
    "    /*\n"
    "     * Register expose handler\n"
    "     * Some group objects depend on it's members' sizes for their layout.\n"
    "     * Unfortunately, some group members have invalid sizes prior to\n"
    "     * XtRealize(), so the group layout has to be recalculated after the\n"
    "     * group is realized or exposed in this case, since there is no realize\n"
    "     * callback.\n"
    "     */\n"
    "    switch(group_info->group_type)\n"
    "    {\n"
    "	case DTB_GROUP_NONE:\n"
    "	break;\n"
    "\n"
    "	case DTB_GROUP_ROWS:\n"
    "	    if (group_info->row_align == DTB_ALIGN_HCENTER)\n"
    "		XtAddEventHandler(parent,\n"
    "	                ExposureMask, False,\n"
    "	                expose_handler, (XtPointer)group_info);\n"
    "	break;\n"
    "\n"
    "	case DTB_GROUP_COLUMNS:\n"
    "	    if ((group_info->col_align == DTB_ALIGN_LABELS) || \n"
    "		(group_info->col_align == DTB_ALIGN_VCENTER))\n"
    "		XtAddEventHandler(parent,\n"
    "	                ExposureMask, False,\n"
    "	                expose_handler, (XtPointer)group_info);\n"
    "	break;\n"
    "\n"
    "	case DTB_GROUP_ROWSCOLUMNS:\n"
    "	    if ((group_info->row_align == DTB_ALIGN_HCENTER) ||\n"
    "	        (group_info->col_align == DTB_ALIGN_LABELS) || \n"
    "		(group_info->col_align == DTB_ALIGN_VCENTER))\n"
    "		XtAddEventHandler(parent,\n"
    "	                ExposureMask, False,\n"
    "	                expose_handler, (XtPointer)group_info);\n"
    "	break;\n"
    "\n"
    "    }\n"
    "\n"
    "    XtAddCallback(parent, XtNdestroyCallback, \n"
    "		free_group_info, (XtPointer)group_info);\n"
    "}"
}; /* abmfP_lrc_children_align */
LibFunc abmfP_lib_children_align = &(abmfP_lrc_children_align);

/*
 * abmfP_lib_align_children
 */
static LibFuncRec abmfP_lrc_align_children = 
{
    /* name */
    "align_children",
    
    /* proto */
    "static void align_children(\n"
    "    Widget		parent,\n"
    "    DtbGroupInfo	*group_info,\n"
    "    Boolean		init\n"
    ");",
    
    /* def */
    "static void\n"
    "align_children(\n"
    "    Widget		parent,\n"
    "    DtbGroupInfo	*group_info,\n"
    "    Boolean		init\n"
    ")\n"
    "{\n"
    "    if (!parent || !group_info)\n"
    "	return;\n"
    "\n"
    "    switch(group_info->group_type)\n"
    "    {\n"
    "        case DTB_GROUP_NONE:\n"
    "        break;\n"
    "\n"
    "        case DTB_GROUP_ROWS:\n"
    "	    align_rows(parent, group_info, init);\n"
    "	    align_left(parent, group_info);\n"
    "        break;\n"
    "\n"
    "        case DTB_GROUP_COLUMNS:\n"
    "	    align_cols(parent, group_info, init);\n"
    "	    align_top(parent, group_info);\n"
    "        break;\n"
    "\n"
    "        case DTB_GROUP_ROWSCOLUMNS:\n"
    "	    align_rows(parent, group_info, init);\n"
    "	    align_cols(parent, group_info, init);\n"
    "        break;\n"
    "\n"
    "    }\n"
    "\n"
    "}"
}; /* abmfP_lrc_align_children */
LibFunc abmfP_lib_align_children = &(abmfP_lrc_align_children);

/*
 * abmfP_lib_align_handler
 */
static LibFuncRec abmfP_lrc_align_handler = 
{
    /* name */
    "align_handler",
    
    /* proto */
    "static void align_handler(\n"
    "    Widget	widget,\n"
    "    XtPointer	client_data,\n"
    "    XEvent	*event,\n"
    "    Boolean	*cont_dispatch\n"
    ");",
    
    /* def */
    "static void\n"
    "align_handler(\n"
    "    Widget	widget,\n"
    "    XtPointer	client_data,\n"
    "    XEvent	*event,\n"
    "    Boolean	*cont_dispatch\n"
    ")\n"
    "{\n"
    "    DtbGroupInfo	*group_info = (DtbGroupInfo *)client_data;\n"
    "    WidgetList	children_list;\n"
    "    int		i,\n"
    "		num_children = 0;\n"
    "    Boolean	relayout_all = False;\n"
    "\n"
    "\n"
    "    /*\n"
    "     * Get children list\n"
    "     */\n"
    "    XtVaGetValues(widget,\n"
    "            XmNnumChildren, &num_children,\n"
    "            XmNchildren, &children_list,\n"
    "            NULL);\n"
    "\n"
    "    if (num_children <= 0)\n"
    "	return;\n"
    "\n"
    "    XtRemoveEventHandler(widget,\n"
    "                SubstructureNotifyMask, False,\n"
    "                align_handler, (XtPointer)client_data);\n"
    "\n"
    "    if (event->type == ConfigureNotify) {\n"
    "        XConfigureEvent	*xcon = &event->xconfigure;\n"
    "	Widget		resized_child;\n"
    "\n"
    "	if (xcon->window != xcon->event)\n"
    "	{\n"
    "            resized_child = XtWindowToWidget(XtDisplay(widget), xcon->window);\n"
    "\n"
    "            switch(group_info->group_type)\n"
    "            {\n"
    "                case DTB_GROUP_NONE:\n"
    "                break;\n"
    "\n"
    "                case DTB_GROUP_ROWS:\n"
    "                    if (group_info->row_align == DTB_ALIGN_HCENTER)\n"
    "                        relayout_all = True;\n"
    "                break;\n"
    "\n"
    "                case DTB_GROUP_COLUMNS:\n"
    "                    if ((group_info->col_align == DTB_ALIGN_LABELS) ||\n"
    "                        (group_info->col_align == DTB_ALIGN_VCENTER))\n"
    "                            relayout_all = True;\n"
    "                break;\n"
    "\n"
    "                case DTB_GROUP_ROWSCOLUMNS:\n"
    "                    if ((group_info->row_align == DTB_ALIGN_HCENTER) ||\n"
    "                        (group_info->col_align == DTB_ALIGN_LABELS) ||\n"
    "                        (group_info->col_align == DTB_ALIGN_VCENTER))\n"
    "                        relayout_all = True;\n"
    "                break;\n"
    "\n"
    "            }\n"
    "        }\n"
    "    }\n"
    "\n"
    "    /*\n"
    "     * Relayout when new widgets are created\n"
    "     */\n"
    "    if (event->type == CreateNotify) {\n"
    "        XCreateWindowEvent	*xcreate = &event->xcreatewindow;\n"
    "\n"
    "	relayout_all = True;\n"
    "    }\n"
    "\n"
    "    /*\n"
    "     * Relayout when widgets are destroyed\n"
    "     */\n"
    "    if (event->type == DestroyNotify) {\n"
    "        XDestroyWindowEvent	*xdestroy = &event->xdestroywindow;\n"
    "	Widget			destroyed_child;\n"
    "\n"
    "        destroyed_child = XtWindowToWidget(XtDisplay(widget), \n"
    "			xdestroy->window);\n"
    "\n"
    "	relayout_all = True;\n"
    "    }\n"
    "\n"
    "    if (relayout_all)\n"
    "    {\n"
    "        align_children(widget, group_info, False);\n"
    "    }\n"
    "\n"
    "    XtAddEventHandler(widget,\n"
    "                SubstructureNotifyMask, False,\n"
    "                align_handler, (XtPointer)client_data);\n"
    "}"
}; /* abmfP_lrc_align_handler */
LibFunc abmfP_lib_align_handler = &(abmfP_lrc_align_handler);

/*
 * abmfP_lib_expose_handler
 */
static LibFuncRec abmfP_lrc_expose_handler = 
{
    /* name */
    "expose_handler",
    
    /* proto */
    "static void expose_handler(\n"
    "    Widget	widget,\n"
    "    XtPointer	client_data,\n"
    "    XEvent	*event,\n"
    "    Boolean	*cont_dispatch\n"
    ");",
    
    /* def */
    "static void\n"
    "expose_handler(\n"
    "    Widget	widget,\n"
    "    XtPointer	client_data,\n"
    "    XEvent	*event,\n"
    "    Boolean	*cont_dispatch\n"
    ")\n"
    "{\n"
    "    DtbGroupInfo	*group_info = (DtbGroupInfo *)client_data;\n"
    "    WidgetList	children_list;\n"
    "    int		i,\n"
    "		num_children = 0;\n"
    "    Boolean	relayout_all = False,\n"
    "		register_align_handler = False;\n"
    "\n"
    "\n"
    "    if (event->type != Expose) \n"
    "	return;\n"
    "    \n"
    "    if (!group_info)\n"
    "	return;\n"
    "\n"
    "    /*\n"
    "     * Get children list\n"
    "     */\n"
    "    XtVaGetValues(widget,\n"
    "            XmNnumChildren, &num_children,\n"
    "            XmNchildren, &children_list,\n"
    "            NULL);\n"
    "\n"
    "    if (num_children <= 0)\n"
    "	return;\n"
    "\n"
    "    XtRemoveEventHandler(widget,\n"
    "                ExposureMask, False,\n"
    "                expose_handler, (XtPointer)client_data);\n"
    "\n"
    "    switch(group_info->group_type)\n"
    "    {\n"
    "        case DTB_GROUP_NONE:\n"
    "        break;\n"
    "\n"
    "        case DTB_GROUP_ROWS:\n"
    "        if (group_info->row_align == DTB_ALIGN_HCENTER)\n"
    "        {\n"
    "            relayout_all = True;\n"
    "\n"
    "            register_align_handler = True;\n"
    "        }\n"
    "        break;\n"
    "\n"
    "        case DTB_GROUP_COLUMNS:\n"
    "        if ((group_info->col_align == DTB_ALIGN_LABELS) ||\n"
    "            (group_info->col_align == DTB_ALIGN_VCENTER))\n"
    "        {\n"
    "            relayout_all = True;\n"
    "\n"
    "            register_align_handler = True;\n"
    "        }\n"
    "        break;\n"
    "\n"
    "        case DTB_GROUP_ROWSCOLUMNS:\n"
    "        if ((group_info->row_align == DTB_ALIGN_HCENTER) ||\n"
    "            (group_info->col_align == DTB_ALIGN_LABELS) ||\n"
    "            (group_info->col_align == DTB_ALIGN_VCENTER))\n"
    "        {\n"
    "            relayout_all = True;\n"
    "\n"
    "            register_align_handler = True;\n"
    "        }\n"
    "        break;\n"
    "    }\n"
    "\n"
    "    if (relayout_all)\n"
    "    {\n"
    "        align_children(widget, group_info, False);\n"
    "    }\n"
    "\n"
    "    if (register_align_handler)\n"
    "    {\n"
    "	/*\n"
    "	 * Register align handler to relayout group if/when\n"
    "	 * any of it's members resize\n"
    "	 */\n"
    "	XtAddEventHandler(widget,\n"
    "            SubstructureNotifyMask, False,\n"
    "            align_handler, (XtPointer)group_info);\n"
    "    }\n"
    "}"
}; /* abmfP_lrc_expose_handler */
LibFunc abmfP_lib_expose_handler = &(abmfP_lrc_expose_handler);

/*
 * abmfP_lib_free_group_info
 */
static LibFuncRec abmfP_lrc_free_group_info = 
{
    /* name */
    "free_group_info",
    
    /* proto */
    "static void		 free_group_info(\n"
    "    Widget	widget,\n"
    "    XtPointer	client_data,\n"
    "    XtPointer	call_data\n"
    ");",
    
    /* def */
    "static void		\n"
    "free_group_info(\n"
    "    Widget	widget,\n"
    "    XtPointer	client_data,\n"
    "    XtPointer	call_data\n"
    ")\n"
    "{\n"
    "    DtbGroupInfo	*group_info = (DtbGroupInfo *)client_data;;\n"
    "\n"
    "    XtFree((char *)group_info);\n"
    "}"
}; /* abmfP_lrc_free_group_info */
LibFunc abmfP_lib_free_group_info = &(abmfP_lrc_free_group_info);

/*
 * abmfP_lib_align_rows
 */
static LibFuncRec abmfP_lrc_align_rows = 
{
    /* name */
    "align_rows",
    
    /* proto */
    "static void align_rows(\n"
    "    Widget		parent,\n"
    "    DtbGroupInfo	*group_info,\n"
    "    Boolean		init\n"
    ");",
    
    /* def */
    "static void\n"
    "align_rows(\n"
    "    Widget		parent,\n"
    "    DtbGroupInfo	*group_info,\n"
    "    Boolean		init\n"
    ")\n"
    "{\n"
    "    if (!parent || !group_info || (group_info->group_type == DTB_GROUP_COLUMNS))\n"
    "	return;\n"
    "\n"
    "    switch (group_info->row_align)\n"
    "    {\n"
    "        case DTB_ALIGN_TOP:\n"
    "            align_top(parent, group_info);\n"
    "        break;\n"
    "\n"
    "        case DTB_ALIGN_HCENTER:\n"
    "            align_hcenter(parent, group_info, init);\n"
    "        break;\n"
    "\n"
    "        case DTB_ALIGN_BOTTOM:\n"
    "            align_bottom(parent, group_info);\n"
    "        break;\n"
    "\n"
    "        default:\n"
    "        break;\n"
    "    }\n"
    "}"
}; /* abmfP_lrc_align_rows */
LibFunc abmfP_lib_align_rows = &(abmfP_lrc_align_rows);

/*
 * abmfP_lib_align_cols
 */
static LibFuncRec abmfP_lrc_align_cols = 
{
    /* name */
    "align_cols",
    
    /* proto */
    "static void align_cols(\n"
    "    Widget		parent,\n"
    "    DtbGroupInfo	*group_info,\n"
    "    Boolean		init\n"
    ");",
    
    /* def */
    "static void\n"
    "align_cols(\n"
    "    Widget		parent,\n"
    "    DtbGroupInfo	*group_info,\n"
    "    Boolean		init\n"
    ")\n"
    "{\n"
    "    if (!parent || !group_info || (group_info->group_type == DTB_GROUP_ROWS))\n"
    "	return;\n"
    "\n"
    "    switch (group_info->col_align)\n"
    "    {\n"
    "        case DTB_ALIGN_LEFT:\n"
    "            align_left(parent, group_info);\n"
    "        break;\n"
    "\n"
    "        case DTB_ALIGN_LABELS:\n"
    "            align_labels(parent, group_info);\n"
    "        break;\n"
    "\n"
    "        case DTB_ALIGN_VCENTER:\n"
    "            align_vcenter(parent, group_info, init);\n"
    "        break;\n"
    "\n"
    "        case DTB_ALIGN_RIGHT:\n"
    "            align_right(parent, group_info);\n"
    "        break;\n"
    "\n"
    "        default:\n"
    "        break;\n"
    "    }\n"
    "}"
}; /* abmfP_lrc_align_cols */
LibFunc abmfP_lib_align_cols = &(abmfP_lrc_align_cols);

/*
 * abmfP_lib_align_left
 */
static LibFuncRec abmfP_lrc_align_left = 
{
    /* name */
    "align_left",
    
    /* proto */
    "static void align_left(\n"
    "    Widget		parent,\n"
    "    DtbGroupInfo	*group_info\n"
    ");",
    
    /* def */
    "static void\n"
    "align_left(\n"
    "    Widget		parent,\n"
    "    DtbGroupInfo	*group_info\n"
    ")\n"
    "{\n"
    "    WidgetList	children_list;\n"
    "    Widget	child, \n"
    "		previous_child;\n"
    "    int		num_children = 0,\n"
    "		num_columns,\n"
    "		num_rows,\n"
    "		cell_width,\n"
    "		cell_height,\n"
    "		i,\n"
    "		j;\n"
    "  \n"
    "    if (!parent || !group_info)\n"
    "	return;\n"
    "\n"
    "    /*\n"
    "     * Get children list\n"
    "     */\n"
    "    XtVaGetValues(parent,\n"
    "            XmNnumChildren, &num_children,\n"
    "            XmNchildren, &children_list,\n"
    "            NULL);\n"
    "\n"
    "    if (num_children <= 0)\n"
    "	return;\n"
    "    \n"
    "    get_group_cell_size(parent, group_info, &cell_width, &cell_height);\n"
    "    get_group_row_col(parent, group_info, &num_rows, &num_columns);\n"
    "\n"
    "    for (j = 0; j < num_rows; j++)\n"
    "    {\n"
    "        for (i = 0; i < num_columns; i++)\n"
    "        {\n"
    "            Arg		args[12];\n"
    "	    int		n = 0;\n"
    "\n"
    "	    child = get_group_child(parent, group_info, i, j);\n"
    "\n"
    "	    if (!child)\n"
    "		continue;\n"
    "\n"
    "	    if ((i == 0) && (j == 0))\n"
    "	    {\n"
    "	        XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM);	n++;\n"
    "	        XtSetArg(args[n], XmNleftOffset, 0);			n++;\n"
    "		XtSetArg(args[n], XmNrightAttachment, XmATTACH_NONE);	n++;\n"
    "\n"
    "	        XtSetValues(child, args, n);\n"
    "\n"
    "		continue;\n"
    "	    }\n"
    "\n"
    "	    if (j == 0)\n"
    "	    {\n"
    "	        int			offset = group_info->hoffset;\n"
    "		DTB_GROUP_TYPES		group_type = group_info->group_type;\n"
    "\n"
    "	        previous_child = get_group_child(parent, group_info, i-1, j);\n"
    "\n"
    "		if (!previous_child)\n"
    "		    continue;\n"
    "\n"
    "                if (group_type == DTB_GROUP_ROWSCOLUMNS)\n"
    "		{\n"
    "		    Dimension	width = 0;\n"
    "\n"
    "		    XtVaGetValues(previous_child, XmNwidth, &width, NULL);\n"
    "		    offset += (cell_width - (int)(width));\n"
    "		}\n"
    "\n"
    "	        XtSetArg(args[n], XmNleftAttachment, \n"
    "				XmATTACH_WIDGET);			n++;\n"
    "	        XtSetArg(args[n], XmNleftWidget, previous_child);	n++;\n"
    "	        XtSetArg(args[n], XmNleftOffset, offset);		n++;\n"
    "		XtSetArg(args[n], XmNrightAttachment, XmATTACH_NONE);	n++;\n"
    "\n"
    "	        XtSetValues(child, args, n);\n"
    "\n"
    "		continue;\n"
    "	    }\n"
    "\n"
    "	    previous_child = get_group_child(parent, group_info, i, j-1);\n"
    "\n"
    "	    if (previous_child)\n"
    "	    {\n"
    "	        XtSetArg(args[n], XmNleftAttachment, \n"
    "				    XmATTACH_OPPOSITE_WIDGET);		n++;\n"
    "	        XtSetArg(args[n], XmNleftWidget, previous_child);	n++;\n"
    "	        XtSetArg(args[n], XmNleftOffset, 0);			n++;\n"
    "		XtSetArg(args[n], XmNrightAttachment, XmATTACH_NONE);	n++;\n"
    "\n"
    "	        XtSetValues(child, args, n);\n"
    "	    }\n"
    "        }\n"
    "    }\n"
    "}"
}; /* abmfP_lrc_align_left */
LibFunc abmfP_lib_align_left = &(abmfP_lrc_align_left);

/*
 * abmfP_lib_align_right
 */
static LibFuncRec abmfP_lrc_align_right = 
{
    /* name */
    "align_right",
    
    /* proto */
    "static void align_right(\n"
    "    Widget		parent,\n"
    "    DtbGroupInfo	*group_info\n"
    ");",
    
    /* def */
    "static void\n"
    "align_right(\n"
    "    Widget		parent,\n"
    "    DtbGroupInfo	*group_info\n"
    ")\n"
    "{\n"
    "    WidgetList	children_list;\n"
    "    Widget	child, \n"
    "		previous_child;\n"
    "    int		num_children = 0,\n"
    "		num_columns,\n"
    "		num_rows,\n"
    "		cell_width,\n"
    "		cell_height,\n"
    "	        offset,\n"
    "		i,\n"
    "		j;\n"
    "\n"
    "    if (!parent || !group_info)\n"
    "	return;\n"
    "\n"
    "    /*\n"
    "     * Get children list\n"
    "     */\n"
    "    XtVaGetValues(parent,\n"
    "            XmNnumChildren, &num_children,\n"
    "            XmNchildren, &children_list,\n"
    "            NULL);\n"
    "\n"
    "    if (num_children <= 0)\n"
    "	return;\n"
    "    \n"
    "    get_group_cell_size(parent, group_info, &cell_width, &cell_height);\n"
    "    get_group_row_col(parent, group_info, &num_rows, &num_columns);\n"
    "\n"
    "    for (j = 0; j < num_rows; j++)\n"
    "    {\n"
    "        for (i = 0; i < num_columns; i++)\n"
    "        {\n"
    "            Arg		args[12];\n"
    "	    int		n = 0;\n"
    "\n"
    "	    child = get_group_child(parent, group_info, i, j);\n"
    "\n"
    "	    if (!child)\n"
    "		continue;\n"
    "\n"
    "	    if ((i == 0) && (j == 0))\n"
    "	    {\n"
    "		Dimension	width = 0;\n"
    "\n"
    "		XtVaGetValues(child, XmNwidth, &width, NULL);\n"
    "\n"
    "		offset = (cell_width - width);\n"
    "\n"
    "	        XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM);	n++;\n"
    "	        XtSetArg(args[n], XmNleftOffset, offset);		n++;\n"
    "		XtSetArg(args[n], XmNrightAttachment, XmATTACH_NONE);	n++;\n"
    "\n"
    "	        XtSetValues(child, args, n);\n"
    "\n"
    "		continue;\n"
    "	    }\n"
    "\n"
    "	    if (j == 0)\n"
    "	    {\n"
    "	        previous_child = get_group_child(parent, group_info, i-1, j);\n"
    "\n"
    "		if (!previous_child)\n"
    "		    continue;\n"
    "\n"
    "	        offset = group_info->hoffset;\n"
    "\n"
    "                if (group_info->group_type == DTB_GROUP_ROWSCOLUMNS)\n"
    "		{\n"
    "		    Dimension	width = 0;\n"
    "\n"
    "		    XtVaGetValues(child, XmNwidth, &width, NULL);\n"
    "		    offset += (cell_width - width);\n"
    "		}\n"
    "\n"
    "	        XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET);	n++;\n"
    "	        XtSetArg(args[n], XmNleftWidget, previous_child);	n++;\n"
    "	        XtSetArg(args[n], XmNleftOffset, offset);		n++;\n"
    "		XtSetArg(args[n], XmNrightAttachment, XmATTACH_NONE);	n++;\n"
    "\n"
    "	        XtSetValues(child, args, n);\n"
    "\n"
    "		continue;\n"
    "	    }\n"
    "\n"
    "	    previous_child = get_group_child(parent, group_info, i, j-1);\n"
    "\n"
    "	    if (previous_child)\n"
    "	    {\n"
    "	        XtSetArg(args[n], XmNrightAttachment, \n"
    "				XmATTACH_OPPOSITE_WIDGET);		n++;\n"
    "	        XtSetArg(args[n], XmNrightWidget, previous_child);	n++;\n"
    "	        XtSetArg(args[n], XmNrightOffset, 0);			n++;\n"
    "		XtSetArg(args[n], XmNleftAttachment, XmATTACH_NONE);	n++;\n"
    "\n"
    "	        XtSetValues(child, args, n);\n"
    "	    }\n"
    "        }\n"
    "    }\n"
    "}"
}; /* abmfP_lrc_align_right */
LibFunc abmfP_lib_align_right = &(abmfP_lrc_align_right);

/*
 * abmfP_lib_align_labels
 */
static LibFuncRec abmfP_lrc_align_labels = 
{
    /* name */
    "align_labels",
    
    /* proto */
    "static void align_labels(\n"
    "    Widget		parent,\n"
    "    DtbGroupInfo	*group_info\n"
    ");",
    
    /* def */
    "static void\n"
    "align_labels(\n"
    "    Widget		parent,\n"
    "    DtbGroupInfo	*group_info\n"
    ")\n"
    "{\n"
    "    WidgetList	children_list = NULL,\n"
    "		one_col;\n"
    "    Widget	previous_child = 0,\n"
    "		child,\n"
    "		ref_widget,\n"
    "		previous_ref_widget = 0;\n"
    "    Dimension	ref_lbl_width = 0,\n"
    "		max_label_width = 0,\n"
    "		max_value_width = 0;\n"
    "    int		num_children = 0,\n"
    "		num_rows,\n"
    "		num_columns,\n"
    "		cell_width,\n"
    "		cell_height,\n"
    "		offset,\n"
    "		i,\n"
    "		j,\n"
    "		ref_x;\n"
    "\n"
    "    if (!parent || !group_info)\n"
    "	return;\n"
    "\n"
    "    /*\n"
    "     * Get children list\n"
    "     */\n"
    "    XtVaGetValues(parent,\n"
    "            XmNnumChildren, &num_children,\n"
    "            XmNchildren, &children_list,\n"
    "            NULL);\n"
    "\n"
    "    if (num_children <= 0)\n"
    "	return;\n"
    "    \n"
    "    get_group_cell_size(parent, group_info, &cell_width, &cell_height);\n"
    "\n"
    "    get_widest_label(children_list, num_children, &child, &max_label_width);\n"
    "    get_widest_value(children_list, num_children, &child, &max_value_width);\n"
    "\n"
    "    if (cell_width < (int)(max_label_width + max_value_width))\n"
    "        cell_width = (int)(max_label_width + max_value_width);\n"
    "\n"
    "    get_group_row_col(parent, group_info, &num_rows, &num_columns);\n"
    "\n"
    "    if (num_rows > 0)\n"
    "	one_col = (WidgetList)XtMalloc(num_rows * sizeof(WidgetList));\n"
    "\n"
    "    for (i = 0; i < num_columns; i++)\n"
    "    {\n"
    "	Widget		ref_widget;\n"
    "	Dimension	ref_width;\n"
    "        Arg		args[12];\n"
    "	int		n = 0;\n"
    "\n"
    "        for (j = 0; j < num_rows; j++)\n"
    "	    one_col[j] = get_group_child(parent, group_info, i, j);\n"
    "\n"
    "	get_widest_label(one_col, num_rows, &ref_widget, &ref_width);\n"
    "\n"
    "	if (!ref_widget)\n"
    "	    continue;\n"
    "\n"
    "	if (previous_ref_widget)\n"
    "	    offset = (i * (group_info->hoffset + cell_width));\n"
    "	else\n"
    "	    offset = 0;\n"
    "\n"
    "	XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM);	n++;\n"
    "	XtSetArg(args[n], XmNleftOffset, offset);		n++;\n"
    "        XtSetArg(args[n], XmNrightAttachment, XmATTACH_NONE);	n++;\n"
    "\n"
    "	XtSetValues(ref_widget, args, n);\n"
    "\n"
    "        for (j = 0; j < num_rows; j++)\n"
    "        {\n"
    "	    child = get_group_child(parent, group_info, i, j);\n"
    "\n"
    "	    if (!child || (child == ref_widget))\n"
    "		continue;\n"
    "\n"
    "	    offset = (i * (group_info->hoffset + cell_width));\n"
    "	    offset += (int)(ref_width - get_label_width(child));\n"
    "\n"
    "	    n = 0;\n"
    "	    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM);	n++;\n"
    "	    XtSetArg(args[n], XmNleftOffset, offset);			n++;\n"
    "	    XtSetArg(args[n], XmNrightAttachment, XmATTACH_NONE);	n++;\n"
    "\n"
    "	    XtSetValues(child, args, n);\n"
    "        }\n"
    "\n"
    "	previous_ref_widget = ref_widget;\n"
    "    }\n"
    "\n"
    "    if (num_rows > 0)\n"
    "        XtFree((char*)one_col);\n"
    "}"
}; /* abmfP_lrc_align_labels */
LibFunc abmfP_lib_align_labels = &(abmfP_lrc_align_labels);

/*
 * abmfP_lib_align_vcenter
 */
static LibFuncRec abmfP_lrc_align_vcenter = 
{
    /* name */
    "align_vcenter",
    
    /* proto */
    "static void align_vcenter(\n"
    "    Widget		parent,\n"
    "    DtbGroupInfo	*group_info,\n"
    "    Boolean		init\n"
    ");",
    
    /* def */
    "static void\n"
    "align_vcenter(\n"
    "    Widget		parent,\n"
    "    DtbGroupInfo	*group_info,\n"
    "    Boolean		init\n"
    ")\n"
    "{\n"
    "    WidgetList	children_list;\n"
    "    Widget	child, \n"
    "		previous_child;\n"
    "    DTB_GROUP_TYPES group_type;\n"
    "    int		num_children = 0,\n"
    "		num_columns,\n"
    "		num_rows,\n"
    "		cell_width,\n"
    "		cell_height,\n"
    "		group_width,\n"
    "		group_height,\n"
    "		offset,\n"
    "		gridline,\n"
    "		i,\n"
    "		j;\n"
    "\n"
    "    if (!parent || !group_info)\n"
    "	return;\n"
    "\n"
    "    /*\n"
    "     * Get children list\n"
    "     */\n"
    "    XtVaGetValues(parent,\n"
    "            XmNnumChildren, &num_children,\n"
    "            XmNchildren, &children_list,\n"
    "            NULL);\n"
    "\n"
    "    if (num_children <= 0)\n"
    "	return;\n"
    "    \n"
    "    get_group_cell_size(parent, group_info, &cell_width, &cell_height);\n"
    "	 \n"
    "    get_group_row_col(parent, group_info, &num_rows, &num_columns);\n"
    "	      \n"
    "    offset = group_info->hoffset;\n"
    "		   \n"
    "    group_type = group_info->group_type;\n"
    "\n"
    "    if (group_type == DTB_GROUP_ROWSCOLUMNS)\n"
    "    {\n"
    "        group_width = (num_columns * cell_width) + ((num_columns-1) * offset);\n"
    "    }\n"
    "\n"
    "    for (i = 0; i < num_columns; i++)\n"
    "    {\n"
    "        if (group_type == DTB_GROUP_ROWSCOLUMNS)\n"
    "	    gridline = (((i * (cell_width + offset)) + (cell_width/2)) * 100)/group_width;\n"
    "	else\n"
    "	    gridline = 50;\n"
    "\n"
    "        for (j = 0; j < num_rows; j++)\n"
    "        {\n"
    "            Arg		args[12];\n"
    "	    int		n = 0;\n"
    "	    Dimension	width = 0;\n"
    "\n"
    "	    child = get_group_child(parent, group_info, i, j);\n"
    "\n"
    "	    if (!child)\n"
    "		continue;\n"
    "\n"
    "	    XtVaGetValues(child, XmNwidth, &width, NULL);\n"
    "\n"
    "	    if (init)\n"
    "	    {\n"
    "		int	offset = 0;\n"
    "\n"
    "		if (!XtIsSubclass(child, compositeWidgetClass))\n"
    "		{\n"
    "	            offset = (cell_width - (int)width)/2;\n"
    "\n"
    "                    if (group_type == DTB_GROUP_ROWSCOLUMNS)\n"
    "	                offset += (i * (cell_width + group_info->hoffset));\n"
    "		}\n"
    "\n"
    "	        XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM);	n++;\n"
    "	        XtSetArg(args[n], XmNleftOffset, offset);		n++;\n"
    "		XtSetArg(args[n], XmNrightAttachment, XmATTACH_NONE);	n++;\n"
    "	    }\n"
    "	    else\n"
    "	    {\n"
    "	        XtSetArg(args[n], XmNleftAttachment, \n"
    "				XmATTACH_POSITION);			n++;\n"
    "	        XtSetArg(args[n], XmNleftPosition, gridline);		n++;\n"
    "	        XtSetArg(args[n], XmNleftOffset, (int)(-(width/2)));	n++;\n"
    "		XtSetArg(args[n], XmNrightAttachment, XmATTACH_NONE);	n++;\n"
    "	    }\n"
    "\n"
    "	    XtSetValues(child, args, n);\n"
    "        }\n"
    "    }\n"
    "}"
}; /* abmfP_lrc_align_vcenter */
LibFunc abmfP_lib_align_vcenter = &(abmfP_lrc_align_vcenter);

/*
 * abmfP_lib_align_top
 */
static LibFuncRec abmfP_lrc_align_top = 
{
    /* name */
    "align_top",
    
    /* proto */
    "static void align_top(\n"
    "    Widget		parent,\n"
    "    DtbGroupInfo	*group_info\n"
    ");",
    
    /* def */
    "static void\n"
    "align_top(\n"
    "    Widget		parent,\n"
    "    DtbGroupInfo	*group_info\n"
    ")\n"
    "{\n"
    "    WidgetList	children_list;\n"
    "    Widget	previous_child = 0,\n"
    "		child,\n"
    "		cur_child;\n"
    "    int		num_children = 0,\n"
    "		num_columns,\n"
    "		num_rows,\n"
    "		cell_width,\n"
    "		cell_height,\n"
    "		i,\n"
    "		j;\n"
    "\n"
    "    if (!parent || !group_info)\n"
    "	return;\n"
    "\n"
    "    /*\n"
    "     * Get children list\n"
    "     */\n"
    "    XtVaGetValues(parent,\n"
    "            XmNnumChildren, &num_children,\n"
    "            XmNchildren, &children_list,\n"
    "            NULL);\n"
    "\n"
    "    if (num_children <= 0)\n"
    "	return;\n"
    "    \n"
    "    get_group_cell_size(parent, group_info, &cell_width, &cell_height);\n"
    "    get_group_row_col(parent, group_info, &num_rows, &num_columns);\n"
    "\n"
    "    for (j = 0; j < num_rows; j++)\n"
    "    {\n"
    "        for (i = 0; i < num_columns; i++)\n"
    "        {\n"
    "	    Arg args[12];\n"
    "	    int n = 0;\n"
    "\n"
    "	    child = get_group_child(parent, group_info, i, j);\n"
    "\n"
    "	    if (!child)\n"
    "		continue;\n"
    "\n"
    "	    if ((i == 0) && (j == 0))\n"
    "	    {\n"
    "	        XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM);	n++;\n"
    "	        XtSetArg(args[n], XmNtopOffset, 0);			n++;\n"
    "	        XtSetArg(args[n], XmNbottomAttachment, XmATTACH_NONE);	n++;\n"
    "\n"
    "	        XtSetValues(child, args, n);\n"
    "\n"
    "		continue;\n"
    "	    }\n"
    "\n"
    "	    if (i == 0)\n"
    "	    {\n"
    "	        previous_child = get_group_child(parent, group_info, 0, j-1);\n"
    "\n"
    "	        if (previous_child)\n"
    "	        {\n"
    "                    DTB_GROUP_TYPES	group_type = group_info->group_type;\n"
    "	            int			offset = group_info->voffset;\n"
    "    \n"
    "                    if (group_type == DTB_GROUP_ROWSCOLUMNS)\n"
    "		    {\n"
    "			Dimension	height = 0;\n"
    "\n"
    "			XtVaGetValues(previous_child, XmNheight, &height, NULL);\n"
    "\n"
    "		        offset += (cell_height - (int)(height));\n"
    "		    }\n"
    "\n"
    "	            XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET);	n++;\n"
    "	            XtSetArg(args[n], XmNtopWidget, previous_child);		n++;\n"
    "	            XtSetArg(args[n], XmNtopOffset, offset);			n++;\n"
    "	            XtSetArg(args[n], XmNbottomAttachment, XmATTACH_NONE);	n++;\n"
    "\n"
    "	            XtSetValues(child, args, n);\n"
    "	        }\n"
    "		continue;\n"
    "	    }\n"
    "\n"
    "	    previous_child = get_group_child(parent, group_info, i-1, j);\n"
    "\n"
    "	    if (previous_child)\n"
    "	    {\n"
    "	        XtSetArg(args[n], XmNtopAttachment, XmATTACH_OPPOSITE_WIDGET);	n++;\n"
    "	        XtSetArg(args[n], XmNtopWidget, previous_child);		n++;\n"
    "	        XtSetArg(args[n], XmNtopOffset, 0);				n++;\n"
    "	        XtSetArg(args[n], XmNbottomAttachment, XmATTACH_NONE);		n++;\n"
    "\n"
    "	        XtSetValues(child, args, n);\n"
    "	    }\n"
    "\n"
    "        }\n"
    "    }\n"
    "}"
}; /* abmfP_lrc_align_top */
LibFunc abmfP_lib_align_top = &(abmfP_lrc_align_top);

/*
 * abmfP_lib_align_bottom
 */
static LibFuncRec abmfP_lrc_align_bottom = 
{
    /* name */
    "align_bottom",
    
    /* proto */
    "static void align_bottom(\n"
    "    Widget		parent,\n"
    "    DtbGroupInfo	*group_info\n"
    ");",
    
    /* def */
    "static void\n"
    "align_bottom(\n"
    "    Widget		parent,\n"
    "    DtbGroupInfo	*group_info\n"
    ")\n"
    "{\n"
    "    WidgetList	children_list;\n"
    "    Widget	child, \n"
    "		previous_child;\n"
    "    DTB_GROUP_TYPES group_type;\n"
    "    int		num_children = 0,\n"
    "		num_columns,\n"
    "		num_rows,\n"
    "		cell_height,\n"
    "		cell_width,\n"
    "		offset,\n"
    "		i,\n"
    "		j;\n"
    "  \n"
    "    if (!parent || !group_info)\n"
    "	return;\n"
    "\n"
    "    /*\n"
    "     * Get children list\n"
    "     */\n"
    "    XtVaGetValues(parent,\n"
    "            XmNnumChildren, &num_children,\n"
    "            XmNchildren, &children_list,\n"
    "            NULL);\n"
    "\n"
    "    if (num_children <= 0)\n"
    "	return;\n"
    "    \n"
    "    get_group_cell_size(parent, group_info, &cell_width, &cell_height);\n"
    "    get_group_row_col(parent, group_info, &num_rows, &num_columns);\n"
    "\n"
    "    for (j = 0; j < num_rows; j++)\n"
    "    {\n"
    "        for (i = 0; i < num_columns; i++)\n"
    "        {\n"
    "            Arg		args[12];\n"
    "	    int		n = 0;\n"
    "\n"
    "	    child = get_group_child(parent, group_info, i, j);\n"
    "\n"
    "	    if (!child)\n"
    "		continue;\n"
    "\n"
    "	    if ((i == 0) && (j == 0))\n"
    "	    {\n"
    "		Dimension	height = 0;\n"
    "\n"
    "		XtVaGetValues(child, XmNheight, &height, NULL);\n"
    "\n"
    "		offset = cell_height - height;\n"
    "\n"
    "	        XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM);	n++;\n"
    "	        XtSetArg(args[n], XmNtopOffset, offset);		n++;\n"
    "	        XtSetArg(args[n], XmNbottomAttachment, XmATTACH_NONE);	n++;\n"
    "\n"
    "	        XtSetValues(child, args, n);\n"
    "\n"
    "		continue;\n"
    "	    }\n"
    "\n"
    "	    if (i == 0)\n"
    "	    {\n"
    "	        previous_child = get_group_child(parent, group_info, 0, j-1);\n"
    "\n"
    "	        if (previous_child)\n"
    "	        {\n"
    "		    Dimension	height = 0;\n"
    "\n"
    "		    XtVaGetValues(child, XmNheight, &height, NULL);\n"
    "\n"
    "	            offset = group_info->voffset;\n"
    "\n"
    "                    if (group_info->group_type == DTB_GROUP_ROWSCOLUMNS)\n"
    "		        offset += (cell_height - height);\n"
    "\n"
    "	            XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET);	n++;\n"
    "	            XtSetArg(args[n], XmNtopWidget, previous_child);		n++;\n"
    "	            XtSetArg(args[n], XmNtopOffset, offset);			n++;\n"
    "	            XtSetArg(args[n], XmNbottomAttachment, XmATTACH_NONE);	n++;\n"
    "\n"
    "	            XtSetValues(child, args, n);\n"
    "	        }\n"
    "		continue;\n"
    "	    }\n"
    "\n"
    "	    previous_child = get_group_child(parent, group_info, i-1, j);\n"
    "\n"
    "	    if (child && previous_child)\n"
    "	    {\n"
    "	        XtSetArg(args[n], XmNbottomAttachment, \n"
    "				XmATTACH_OPPOSITE_WIDGET);		n++;\n"
    "	        XtSetArg(args[n], XmNbottomWidget, previous_child);	n++;\n"
    "	        XtSetArg(args[n], XmNbottomOffset, 0);			n++;\n"
    "	        XtSetArg(args[n], XmNtopAttachment, XmATTACH_NONE);	n++;\n"
    "\n"
    "	        XtSetValues(child, args, n);\n"
    "	    }\n"
    "        }\n"
    "    }\n"
    "}"
}; /* abmfP_lrc_align_bottom */
LibFunc abmfP_lib_align_bottom = &(abmfP_lrc_align_bottom);

/*
 * abmfP_lib_align_hcenter
 */
static LibFuncRec abmfP_lrc_align_hcenter = 
{
    /* name */
    "align_hcenter",
    
    /* proto */
    "static void align_hcenter(\n"
    "    Widget		parent,\n"
    "    DtbGroupInfo	*group_info,\n"
    "    Boolean		init\n"
    ");",
    
    /* def */
    "static void\n"
    "align_hcenter(\n"
    "    Widget		parent,\n"
    "    DtbGroupInfo	*group_info,\n"
    "    Boolean		init\n"
    ")\n"
    "{\n"
    "    WidgetList	children_list = NULL;\n"
    "    Widget	child, \n"
    "		previous_child;\n"
    "    DTB_GROUP_TYPES group_type;\n"
    "    int		num_children = 0,\n"
    "		num_columns,\n"
    "		num_rows,\n"
    "		cell_width,\n"
    "		cell_height,\n"
    "		group_width,\n"
    "		group_height,\n"
    "		offset,\n"
    "		gridline,\n"
    "		i,\n"
    "		j;\n"
    "  \n"
    "    if (!parent || !group_info)\n"
    "	return;\n"
    "\n"
    "    /*\n"
    "     * Get children list\n"
    "     */\n"
    "    XtVaGetValues(parent,\n"
    "            XmNnumChildren, &num_children,\n"
    "            XmNchildren, &children_list,\n"
    "            NULL);\n"
    "\n"
    "    if (num_children <= 0)\n"
    "	return;\n"
    "    \n"
    "    group_type = group_info->group_type;\n"
    "\n"
    "    get_group_cell_size(parent, group_info, &cell_width, &cell_height);\n"
    "    get_group_row_col(parent, group_info, &num_rows, &num_columns);\n"
    "\n"
    "    offset = group_info->voffset;\n"
    "\n"
    "    if (group_type == DTB_GROUP_ROWSCOLUMNS)\n"
    "    {\n"
    "        group_height = (num_rows * cell_height) + ((num_rows-1) * offset);\n"
    "    }\n"
    "\n"
    "    for (j = 0; j < num_rows; j++)\n"
    "    {\n"
    "        if (group_type == DTB_GROUP_ROWSCOLUMNS)\n"
    "	    gridline = \n"
    "		(((j * (cell_height + offset)) + (cell_height/2)) * 100)/group_height;\n"
    "	else\n"
    "	    gridline = 50;\n"
    "\n"
    "        for (i = 0; i < num_columns; i++)\n"
    "        {\n"
    "            Arg		args[12];\n"
    "	    int		n = 0;\n"
    "	    Dimension	height = 0;\n"
    "\n"
    "	    child = get_group_child(parent, group_info, i, j);\n"
    "\n"
    "	    if (!child)\n"
    "		continue;\n"
    "\n"
    "	    XtVaGetValues(child, XmNheight, &height, NULL);\n"
    "\n"
    "	    if (init)\n"
    "	    {\n"
    "		int	init_offset = 0;\n"
    "\n"
    "                if (!XtIsSubclass(child, compositeWidgetClass))\n"
    "		{\n"
    "		    init_offset = (cell_height - (int)height)/2;\n"
    "\n"
    "		    if (group_type == DTB_GROUP_ROWSCOLUMNS)\n"
    "		    init_offset += (j * (cell_height + group_info->voffset));\n"
    "		}\n"
    "\n"
    "	        XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM);	n++;\n"
    "	        XtSetArg(args[n], XmNtopOffset, init_offset);		n++;\n"
    "	        XtSetArg(args[n], XmNbottomAttachment, XmATTACH_NONE);	n++;\n"
    "	    }\n"
    "	    else\n"
    "	    {\n"
    "	        XtSetArg(args[n], XmNtopAttachment, \n"
    "				XmATTACH_POSITION);			n++;\n"
    "	        XtSetArg(args[n], XmNtopPosition, gridline);		n++;\n"
    "	        XtSetArg(args[n], XmNtopOffset, (int)(-(height/2)));	n++;\n"
    "	        XtSetArg(args[n], XmNbottomAttachment, XmATTACH_NONE);	n++;\n"
    "	    }\n"
    "\n"
    "	    XtSetValues(child, args, n);\n"
    "        }\n"
    "    }\n"
    "}"
}; /* abmfP_lrc_align_hcenter */
LibFunc abmfP_lib_align_hcenter = &(abmfP_lrc_align_hcenter);

/*
 * abmfP_lib_get_exe_dir
 */
static LibFuncRec abmfP_lrc_get_exe_dir = 
{
    /* name */
    "dtb_get_exe_dir",
    
    /* proto */
    "String dtb_get_exe_dir(void);",
    
    /* def */
    "/*\n"
    " * Returns the directory that the executable for this process was loaded \n"
    " * from.\n"
    " */\n"
    "String\n"
    "dtb_get_exe_dir(void)\n"
    "{\n"
    "    return dtb_exe_dir;\n"
    "}"
}; /* abmfP_lrc_get_exe_dir */
LibFunc abmfP_lib_get_exe_dir = &(abmfP_lrc_get_exe_dir);

/*
 * abmfP_lib_determine_exe_dir
 */
static LibFuncRec abmfP_lrc_determine_exe_dir = 
{
    /* name */
    "determine_exe_dir",
    
    /* proto */
    "static int  determine_exe_dir(\n"
    "    char 	*argv0,\n"
    "    char 	*buf,\n"
    "    int 	bufSize\n"
    ");",
    
    /* def */
    "/*\n"
    " * Determines the directory the executable for this process was loaded from.\n"
    " */\n"
    "static int \n"
    "determine_exe_dir(\n"
    "    char 	*argv0,\n"
    "    char 	*buf,\n"
    "    int 	bufSize\n"
    ")\n"
    "{\n"
    "    Boolean	foundDir= False;\n"
    "\n"
    "    if ((buf == NULL) || (bufSize < 1))\n"
    "    {\n"
    "	return -1;\n"
    "    }\n"
    "    \n"
    "    if (determine_exe_dir_from_argv(argv0, buf, bufSize) >= 0)\n"
    "    {\n"
    "	foundDir = True;\n"
    "    }\n"
    "\n"
    "    if (!foundDir)\n"
    "    {\n"
    "	if (determine_exe_dir_from_path(argv0, buf, bufSize) >= 0)\n"
    "	{\n"
    "	    foundDir = True;\n"
    "	}\n"
    "    }\n"
    "\n"
    "    /*\n"
    "     * Convert relative path to absolute, so that directory changes will\n"
    "     * not affect us.\n"
    "     */\n"
    "    if (foundDir && (buf[0] != '/'))\n"
    "    {\n"
    "	char	cwd[MAXPATHLEN+1];\n"
    "	char	*env_pwd = NULL;\n"
    "	char	*path_prefix = NULL;\n"
    "	char	abs_exe_dir[MAXPATHLEN+1];\n"
    "\n"
    "	if (getcwd(cwd, MAXPATHLEN+1) != NULL)\n"
    "	{\n"
    "	    path_prefix = cwd;\n"
    "	}\n"
    "	else\n"
    "	{\n"
    "	    env_pwd = getenv(\"PWD\");\n"
    "	    if (env_pwd != NULL)\n"
    "	    {\n"
    "		path_prefix = env_pwd;\n"
    "	    }\n"
    "	}\n"
    "\n"
    "	if (path_prefix != NULL)\n"
    "	{\n"
    "	    strcpy(abs_exe_dir, path_prefix);\n"
    "	    if (strcmp(buf, \".\") != 0)\n"
    "	    {\n"
    "		strcat(abs_exe_dir, \"/\");\n"
    "		strcat(abs_exe_dir, buf);\n"
    "	    }\n"
    "	    strcpy(buf, abs_exe_dir);\n"
    "	}\n"
    "    }\n"
    "\n"
    "    return foundDir? 0:-1;\n"
    "}"
}; /* abmfP_lrc_determine_exe_dir */
LibFunc abmfP_lib_determine_exe_dir = &(abmfP_lrc_determine_exe_dir);

/*
 * abmfP_lib_determine_exe_dir_from_argv
 */
static LibFuncRec abmfP_lrc_determine_exe_dir_from_argv = 
{
    /* name */
    "determine_exe_dir_from_argv",
    
    /* proto */
    "static int determine_exe_dir_from_argv(\n"
    "    char 	*argv0,\n"
    "    char 	*buf,\n"
    "    int 	bufSize\n"
    ");",
    
    /* def */
    "/*\n"
    " *  Looks for absolute path in arv[0].\n"
    " */\n"
    "static int\n"
    "determine_exe_dir_from_argv(\n"
    "    char 	*argv0,\n"
    "    char 	*buf,\n"
    "    int 	bufSize\n"
    ")\n"
    "{\n"
    "    int		i= 0;\n"
    "    Boolean	foundit= False;\n"
    "\n"
    "    for (i= strlen(argv0)-1; (i >= 0) && (argv0[i] != '/'); --i)\n"
    "    {\n"
    "    }\n"
    "\n"
    "    if (i >= 0)\n"
    "    {\n"
    "	int	maxStringSize= min(i, bufSize);\n"
    "	strncpy(buf, argv0, maxStringSize);\n"
    "	buf[min(maxStringSize, bufSize-1)]= 0;\n"
    "	foundit = True;\n"
    "    }\n"
    "\n"
    "    return foundit? 0:-1;\n"
    "}"
}; /* abmfP_lrc_determine_exe_dir_from_argv */
LibFunc abmfP_lib_determine_exe_dir_from_argv = &(abmfP_lrc_determine_exe_dir_from_argv);

/*
 * abmfP_lib_determine_exe_dir_from_path
 */
static LibFuncRec abmfP_lrc_determine_exe_dir_from_path = 
{
    /* name */
    "determine_exe_dir_from_path",
    
    /* proto */
    "static int determine_exe_dir_from_path (\n"
    "    char 	*argv0,\n"
    "    char 	*buf,\n"
    "    int 	bufSize\n"
    ");",
    
    /* def */
    "/*\n"
    " * Assumes: bufSize > 0\n"
    " */\n"
    "static int\n"
    "determine_exe_dir_from_path (\n"
    "    char 	*argv0,\n"
    "    char 	*buf,\n"
    "    int 	bufSize\n"
    ")\n"
    "{\n"
    "    Boolean	foundDir= False;\n"
    "    Boolean	moreDirs= True;\n"
    "    char	*szExeName= argv0;\n"
    "    int		iExeNameLen= strlen(szExeName);\n"
    "    char	*szTemp= NULL;\n"
    "    char	szPathVar[MAXPATHLEN+1];\n"
    "    int		iPathVarLen= 0;\n"
    "    char	szCurrentPath[MAXPATHLEN+1];\n"
    "    int		iCurrentPathLen= 0;\n"
    "    int		iCurrentPathStart= 0;\n"
    "    int		i = 0;\n"
    "    uid_t	euid= geteuid();\n"
    "    uid_t	egid= getegid();\n"
    "\n"
    "    szTemp= getenv(\"PATH\");\n"
    "    if (szTemp == NULL)\n"
    "    {\n"
    "	moreDirs= False;\n"
    "    }\n"
    "    else\n"
    "    {\n"
    "	strncpy(szPathVar, szTemp, MAXPATHLEN);\n"
    "	szPathVar[MAXPATHLEN]= 0;\n"
    "	iPathVarLen= strlen(szPathVar);\n"
    "    }\n"
    "\n"
    "    while ((!foundDir) && (moreDirs))\n"
    "    {\n"
    "	/* find the current directory name */\n"
    "	for (i= iCurrentPathStart; (i < iPathVarLen) && (szPathVar[i] != ':'); \n"
    "	    )\n"
    "    	{\n"
    "	    ++i;\n"
    "	}\n"
    "	iCurrentPathLen= i - iCurrentPathStart;\n"
    "	if ((iCurrentPathLen + iExeNameLen + 2) > MAXPATHLEN)\n"
    "	{\n"
    "	    iCurrentPathLen= MAXPATHLEN - (iExeNameLen + 2);\n"
    "	}\n"
    "\n"
    "	/* create a possible path to the executable */\n"
    "	strncpy(szCurrentPath, &szPathVar[iCurrentPathStart], iCurrentPathLen);\n"
    "	szCurrentPath[iCurrentPathLen]= 0;\n"
    "	strcat(szCurrentPath, \"/\");\n"
    "	strcat(szCurrentPath, szExeName);\n"
    "\n"
    "	/* see if the executable exists (and we can execute it) */\n"
    "	if (path_is_executable(szCurrentPath, euid, egid))\n"
    "	{\n"
    "	    foundDir= True;\n"
    "	}\n"
    "\n"
    "	/* skip past the current directory name */\n"
    "	if (!foundDir)\n"
    "	{\n"
    "	    iCurrentPathStart+= iCurrentPathLen;\n"
    "	    while (   (iCurrentPathStart < iPathVarLen) \n"
    "		   && (szPathVar[iCurrentPathStart] != ':') )\n"
    "	    {\n"
    "		++iCurrentPathStart;	/* find : */\n"
    "	    }\n"
    "	    if (iCurrentPathStart < iPathVarLen) \n"
    "	    {\n"
    "		++iCurrentPathStart;	/* skip : */\n"
    "	    }\n"
    "	    if (iCurrentPathStart >= iPathVarLen)\n"
    "	    {\n"
    "		moreDirs= False;\n"
    "	    }\n"
    "	}\n"
    "    } /* while !foundDir */\n"
    "\n"
    "    if (foundDir)\n"
    "    {\n"
    "	szCurrentPath[iCurrentPathLen]= 0;\n"
    "	strncpy(buf, szCurrentPath, bufSize);\n"
    "	buf[bufSize-1]= 0;\n"
    "    }\n"
    "    return foundDir? 0:-1;\n"
    "}"
}; /* abmfP_lrc_determine_exe_dir_from_path */
LibFunc abmfP_lib_determine_exe_dir_from_path = &(abmfP_lrc_determine_exe_dir_from_path);

/*
 * abmfP_lib_path_is_executable
 */
static LibFuncRec abmfP_lrc_path_is_executable = 
{
    /* name */
    "path_is_executable",
    
    /* proto */
    "static Boolean path_is_executable(\n"
    "    char 	*path,\n"
    "    uid_t	euid,\n"
    "    gid_t 	egid\n"
    ");",
    
    /* def */
    "/*\n"
    " * returns False is path does not exist or is not executable\n"
    " */\n"
    "static Boolean\n"
    "path_is_executable(\n"
    "    char 	*path,\n"
    "    uid_t	euid,\n"
    "    gid_t 	egid\n"
    ")\n"
    "{\n"
    "    Boolean	bExecutable= False;\n"
    "    struct stat	sStat;\n"
    "\n"
    "    if (stat(path, &sStat) == 0)\n"
    "    {\n"
    "	Boolean	bDetermined= False;\n"
    "\n"
    "	if (!bDetermined)\n"
    "	{\n"
    "	    if (!S_ISREG(sStat.st_mode))\n"
    "	    {\n"
    "		/* not a regular file */\n"
    "		bDetermined= True;\n"
    "		bExecutable= False;\n"
    "	    }\n"
    "	}\n"
    "\n"
    "	if (!bDetermined)\n"
    "	{\n"
    "	    if (   (euid == 0) \n"
    "	        && (   ((sStat.st_mode & S_IXOTH) != 0)\n"
    "		    || ((sStat.st_mode & S_IXGRP) != 0)\n"
    "		    || ((sStat.st_mode & S_IXUSR) != 0) )\n"
    "	       )\n"
    "	    {\n"
    "		bDetermined= True;\n"
    "		bExecutable= True;\n"
    "	    }\n"
    "	}\n"
    "\n"
    "	if (!bDetermined)\n"
    "	{\n"
    "	    if (   (((sStat.st_mode & S_IXOTH) != 0)    )\n"
    "		|| (((sStat.st_mode & S_IXGRP) != 0) && (sStat.st_gid == egid))\n"
    "		|| (((sStat.st_mode & S_IXUSR) != 0) && (sStat.st_gid == euid))\n"
    "	       )\n"
    "	    {\n"
    "		bDetermined= True;\n"
    "	        bExecutable= True;\n"
    "	    }\n"
    "	}\n"
    "    } /* if stat */\n"
    "\n"
    "    return bExecutable;\n"
    "}"
}; /* abmfP_lrc_path_is_executable */
LibFunc abmfP_lib_path_is_executable = &(abmfP_lrc_path_is_executable);

/*
 * abmfP_lib_set_tt_msg_quitCB
 */
static LibFuncRec abmfP_lrc_set_tt_msg_quitCB = 
{
    /* name */
    "dtb_set_tt_msg_quitCB",
    
    /* proto */
    "void dtb_set_tt_msg_quitCB(\n"
    "    DtbTTMsgHandlerCB 	msg_quitCB\n"
    ");",
    
    /* def */
    "/*\n"
    " * dtb_set_tt_msg_quitCB()\n"
    " */\n"
    "void\n"
    "dtb_set_tt_msg_quitCB(\n"
    "    DtbTTMsgHandlerCB 	msg_quitCB\n"
    ")\n"
    "{\n"
    "    dtb_tt_msg_quitCB = msg_quitCB;\n"
    "}"
}; /* abmfP_lrc_set_tt_msg_quitCB */
LibFunc abmfP_lib_set_tt_msg_quitCB = &(abmfP_lrc_set_tt_msg_quitCB);

/*
 * abmfP_lib_get_tt_msg_quitCB
 */
static LibFuncRec abmfP_lrc_get_tt_msg_quitCB = 
{
    /* name */
    "dtb_get_tt_msg_quitCB",
    
    /* proto */
    "static DtbTTMsgHandlerCB dtb_get_tt_msg_quitCB();",
    
    /* def */
    "/*\n"
    " * dtb_get_tt_msg_quitCB()\n"
    " */\n"
    "static DtbTTMsgHandlerCB\n"
    "dtb_get_tt_msg_quitCB()\n"
    "{\n"
    "    return(dtb_tt_msg_quitCB);\n"
    "}"
}; /* abmfP_lrc_get_tt_msg_quitCB */
LibFunc abmfP_lib_get_tt_msg_quitCB = &(abmfP_lrc_get_tt_msg_quitCB);

/*
 * abmfP_lib_tt_msg_quit
 */
static LibFuncRec abmfP_lrc_tt_msg_quit = 
{
    /* name */
    "dtb_tt_msg_quit",
    
    /* proto */
    "static Boolean dtb_tt_msg_quit(\n"
    "    Tt_message	msg\n"
    ");",
    
    /* def */
    "/*\n"
    " * dtb_tt_msg_quit()\n"
    " * Handle the ToolTalk Desktop \"Quit\" message.\n"
    " * Call Application-defined callback if it exists.\n"
    " */\n"
    "static Boolean\n"
    "dtb_tt_msg_quit(\n"
    "    Tt_message	msg\n"
    ")\n"
    "{\n"
    "    DtbTTMsgHandlerCB	msg_quitCB = NULL;\n"
    "    Boolean		handled = True;\n"
    "\n"
    "    msg_quitCB = dtb_get_tt_msg_quitCB();\n"
    "    if (msg_quitCB)\n"
    "	handled = msg_quitCB(msg, (void *)NULL);\n"
    "\n"
    "    return handled;\n"
    "}"
}; /* abmfP_lrc_tt_msg_quit */
LibFunc abmfP_lib_tt_msg_quit = &(abmfP_lrc_tt_msg_quit);

/*
 * abmfP_lib_set_tt_msg_do_commandCB
 */
static LibFuncRec abmfP_lrc_set_tt_msg_do_commandCB = 
{
    /* name */
    "dtb_set_tt_msg_do_commandCB",
    
    /* proto */
    "void dtb_set_tt_msg_do_commandCB(\n"
    "    DtbTTMsgHandlerCB 	msg_do_commandCB\n"
    ");",
    
    /* def */
    "/*\n"
    " * dtb_set_tt_msg_do_commandCB()\n"
    " */\n"
    "void\n"
    "dtb_set_tt_msg_do_commandCB(\n"
    "    DtbTTMsgHandlerCB 	msg_do_commandCB\n"
    ")\n"
    "{\n"
    "    dtb_tt_msg_do_commandCB = msg_do_commandCB;\n"
    "}"
}; /* abmfP_lrc_set_tt_msg_do_commandCB */
LibFunc abmfP_lib_set_tt_msg_do_commandCB = &(abmfP_lrc_set_tt_msg_do_commandCB);

/*
 * abmfP_lib_get_tt_msg_do_commandCB
 */
static LibFuncRec abmfP_lrc_get_tt_msg_do_commandCB = 
{
    /* name */
    "dtb_get_tt_msg_do_commandCB",
    
    /* proto */
    "static DtbTTMsgHandlerCB dtb_get_tt_msg_do_commandCB();",
    
    /* def */
    "/* \n"
    " * dtb_get_tt_msg_do_commandCB()\n"
    " */ \n"
    "static DtbTTMsgHandlerCB\n"
    "dtb_get_tt_msg_do_commandCB()\n"
    "{ \n"
    "    return(dtb_tt_msg_do_commandCB);\n"
    "}"
}; /* abmfP_lrc_get_tt_msg_do_commandCB */
LibFunc abmfP_lib_get_tt_msg_do_commandCB = &(abmfP_lrc_get_tt_msg_do_commandCB);

/*
 * abmfP_lib_tt_msg_do_command
 */
static LibFuncRec abmfP_lrc_tt_msg_do_command = 
{
    /* name */
    "dtb_tt_msg_do_command",
    
    /* proto */
    "static Boolean dtb_tt_msg_do_command(\n"
    "    Tt_message	msg\n"
    ");",
    
    /* def */
    "/* \n"
    " * dtb_tt_msg_do_command()\n"
    " * Handle the ToolTalk Desktop \"Do Command\" message. \n"
    " * Call Application-defined callback if it exists. \n"
    " */ \n"
    "static Boolean\n"
    "dtb_tt_msg_do_command(\n"
    "    Tt_message	msg\n"
    ")\n"
    "{\n"
    "    DtbTTMsgHandlerCB 	msg_do_commandCB = NULL;\n"
    "    Boolean             handled = True;\n"
    " \n"
    "    msg_do_commandCB = dtb_get_tt_msg_do_commandCB();\n"
    "    if (msg_do_commandCB)\n"
    "        handled = msg_do_commandCB(msg, (void *)NULL);\n"
    " \n"
    "    return handled;\n"
    "}"
}; /* abmfP_lrc_tt_msg_do_command */
LibFunc abmfP_lib_tt_msg_do_command = &(abmfP_lrc_tt_msg_do_command);

/*
 * abmfP_lib_set_tt_msg_get_statusCB
 */
static LibFuncRec abmfP_lrc_set_tt_msg_get_statusCB = 
{
    /* name */
    "dtb_set_tt_msg_get_statusCB",
    
    /* proto */
    "void dtb_set_tt_msg_get_statusCB(\n"
    "    DtbTTMsgHandlerCB 	msg_get_statusCB\n"
    ");",
    
    /* def */
    "/*\n"
    " * dtb_set_tt_msg_get_statusCB()\n"
    " */\n"
    "void\n"
    "dtb_set_tt_msg_get_statusCB(\n"
    "    DtbTTMsgHandlerCB 	msg_get_statusCB\n"
    ")\n"
    "{\n"
    "    dtb_tt_msg_get_statusCB = msg_get_statusCB;\n"
    "}"
}; /* abmfP_lrc_set_tt_msg_get_statusCB */
LibFunc abmfP_lib_set_tt_msg_get_statusCB = &(abmfP_lrc_set_tt_msg_get_statusCB);

/*
 * abmfP_lib_get_tt_msg_get_statusCB
 */
static LibFuncRec abmfP_lrc_get_tt_msg_get_statusCB = 
{
    /* name */
    "dtb_get_tt_msg_get_statusCB",
    
    /* proto */
    "static DtbTTMsgHandlerCB dtb_get_tt_msg_get_statusCB();",
    
    /* def */
    "/* \n"
    " * dtb_get_tt_msg_get_statusCB()\n"
    " */ \n"
    "static DtbTTMsgHandlerCB\n"
    "dtb_get_tt_msg_get_statusCB()\n"
    "{ \n"
    "    return(dtb_tt_msg_get_statusCB);\n"
    "}"
}; /* abmfP_lrc_get_tt_msg_get_statusCB */
LibFunc abmfP_lib_get_tt_msg_get_statusCB = &(abmfP_lrc_get_tt_msg_get_statusCB);

/*
 * abmfP_lib_tt_msg_get_status
 */
static LibFuncRec abmfP_lrc_tt_msg_get_status = 
{
    /* name */
    "dtb_tt_msg_get_status",
    
    /* proto */
    "static Boolean dtb_tt_msg_get_status(\n"
    "    Tt_message  msg\n"
    ");",
    
    /* def */
    "/* \n"
    " * dtb_tt_msg_get_status()\n"
    " * Handle the ToolTalk Desktop \"Get Status\" message. \n"
    " * Call Application-defined callback if it exists. \n"
    " */ \n"
    "static Boolean\n"
    "dtb_tt_msg_get_status(\n"
    "    Tt_message  msg\n"
    ")\n"
    "{\n"
    "    DtbTTMsgHandlerCB 	msg_get_statusCB = NULL;\n"
    "    Boolean             handled = True;\n"
    "\n"
    "    msg_get_statusCB = dtb_get_tt_msg_get_statusCB();\n"
    "    if (msg_get_statusCB)\n"
    "        handled = msg_get_statusCB(msg, (void *)NULL);\n"
    "\n"
    "    return handled;\n"
    "}"
}; /* abmfP_lrc_tt_msg_get_status */
LibFunc abmfP_lib_tt_msg_get_status = &(abmfP_lrc_tt_msg_get_status);

/*
 * abmfP_lib_set_tt_msg_pause_resumeCB
 */
static LibFuncRec abmfP_lrc_set_tt_msg_pause_resumeCB = 
{
    /* name */
    "dtb_set_tt_msg_pause_resumeCB",
    
    /* proto */
    "void dtb_set_tt_msg_pause_resumeCB(\n"
    "    DtbTTMsgHandlerCB 	msg_pause_resumeCB\n"
    ");",
    
    /* def */
    "/*\n"
    " * dtb_set_tt_msg_pause_resumeCB()\n"
    " */\n"
    "void\n"
    "dtb_set_tt_msg_pause_resumeCB(\n"
    "    DtbTTMsgHandlerCB 	msg_pause_resumeCB\n"
    ")\n"
    "{\n"
    "    dtb_tt_msg_pause_resumeCB = msg_pause_resumeCB;\n"
    "}"
}; /* abmfP_lrc_set_tt_msg_pause_resumeCB */
LibFunc abmfP_lib_set_tt_msg_pause_resumeCB = &(abmfP_lrc_set_tt_msg_pause_resumeCB);

/*
 * abmfP_lib_get_tt_msg_pause_resumeCB
 */
static LibFuncRec abmfP_lrc_get_tt_msg_pause_resumeCB = 
{
    /* name */
    "dtb_get_tt_msg_pause_resumeCB",
    
    /* proto */
    "static DtbTTMsgHandlerCB dtb_get_tt_msg_pause_resumeCB();",
    
    /* def */
    "/* \n"
    " * dtb_get_tt_msg_pause_resumeCB()\n"
    " */ \n"
    "static DtbTTMsgHandlerCB\n"
    "dtb_get_tt_msg_pause_resumeCB()\n"
    "{ \n"
    "    return(dtb_tt_msg_pause_resumeCB);\n"
    "}"
}; /* abmfP_lrc_get_tt_msg_pause_resumeCB */
LibFunc abmfP_lib_get_tt_msg_pause_resumeCB = &(abmfP_lrc_get_tt_msg_pause_resumeCB);

/*
 * abmfP_lib_tt_msg_pause_resume
 */
static LibFuncRec abmfP_lrc_tt_msg_pause_resume = 
{
    /* name */
    "dtb_tt_msg_pause_resume",
    
    /* proto */
    "static Boolean dtb_tt_msg_pause_resume(\n"
    "    Tt_message	msg,\n"
    "    Boolean	sensitive\n"
    ");",
    
    /* def */
    "/* \n"
    " * dtb_tt_msg_pause_resume()\n"
    " * Handle the ToolTalk Desktop \"Pause/Resume\" messages. \n"
    " * Call Application-defined callback if it exists. \n"
    " */ \n"
    "static Boolean\n"
    "dtb_tt_msg_pause_resume(\n"
    "    Tt_message	msg,\n"
    "    Boolean	sensitive\n"
    ")\n"
    "{\n"
    "    DtbTTMsgHandlerCB 	msg_pause_resumeCB = NULL;\n"
    "    Boolean             handled = True;\n"
    "\n"
    "    msg_pause_resumeCB = dtb_get_tt_msg_pause_resumeCB();\n"
    "    if (msg_pause_resumeCB)\n"
    "        handled = msg_pause_resumeCB(msg, (void *)sensitive);\n"
    "\n"
    "    return handled;\n"
    "}"
}; /* abmfP_lrc_tt_msg_pause_resume */
LibFunc abmfP_lib_tt_msg_pause_resume = &(abmfP_lrc_tt_msg_pause_resume);

/*
 * abmfP_lib_tt_contractCB
 */
static LibFuncRec abmfP_lrc_tt_contractCB = 
{
    /* name */
    "dtb_tt_contractCB",
    
    /* proto */
    "Tt_message dtb_tt_contractCB(\n"
    "    Tt_message	msg,\n"
    "    void	*client_data,\n"
    "    Tt_message	contract\n"
    ");",
    
    /* def */
    "/* \n"
    " * dtb_tt_contractCB()\n"
    " * Receives any ToolTalk Desktop messages sent to application\n"
    " * and dispatches them to the appropriate Application-defined callbacks\n"
    " * if they exist.\n"
    " */ \n"
    "Tt_message\n"
    "dtb_tt_contractCB(\n"
    "    Tt_message	msg,\n"
    "    void	*client_data,\n"
    "    Tt_message	contract\n"
    ")\n"
    "{        \n"
    "    char	*op_string;\n"
    "    Tttk_op	op;\n"
    "    Boolean	sensitive = True;\n"
    "\n"
    "    if (contract != 0) \n"
    "    {\n"
    "        tttk_message_fail( msg, TT_DESKTOP_ENOMSG, 0, 1 );\n"
    "        return 0;\n"
    "    }\n"
    "    op_string = tt_message_op(msg);\n"
    "    op = tttk_string_op(op_string);\n"
    "    tt_free(op_string);\n"
    "\n"
    "    switch(op)\n"
    "    {\n"
    "	case TTDT_QUIT:\n"
    "	    if (dtb_tt_msg_quit(msg) == True)\n"
    "	        return 0;\n"
    "	    break;\n"
    "\n"
    "	case TTDT_GET_STATUS:\n"
    "	    if (dtb_tt_msg_get_status(msg) == True)\n"
    "                return 0; \n"
    "	    break;\n"
    "\n"
    "        case TTDT_PAUSE:\n"
    "            sensitive = False;\n"
    "        case TTDT_RESUME:\n"
    "	    if (dtb_tt_msg_pause_resume(msg, sensitive) == True)\n"
    "	        return 0;\n"
    "	    break;\n"
    "\n"
    "	case TTDT_DO_COMMAND:\n"
    "	    if (dtb_tt_msg_do_command(msg) == True)\n"
    "	        return 0;\n"
    "	    break;\n"
    "	}\n"
    "	return msg;\n"
    "\n"
    "}"
}; /* abmfP_lrc_tt_contractCB */
LibFunc abmfP_lib_tt_contractCB = &(abmfP_lrc_tt_contractCB);

/*
 * abmfP_lib_tt_close
 */
static LibFuncRec abmfP_lrc_tt_close = 
{
    /* name */
    "dtb_tt_close",
    
    /* proto */
    "void dtb_tt_close();",
    
    /* def */
    "/*\n"
    " * dtb_tt_close()\n"
    " * Close ToolTalk Connection\n"
    " */\n"
    "void\n"
    "dtb_tt_close()\n"
    "{\n"
    "    ttdt_close(NULL, NULL, True);\n"
    "}"
}; /* abmfP_lrc_tt_close */
LibFunc abmfP_lib_tt_close = &(abmfP_lrc_tt_close);

/*
 * abmfP_lib_call_help_callback
 */
static LibFuncRec abmfP_lrc_call_help_callback = 
{
    /* name */
    "dtb_call_help_callback",
    
    /* proto */
    "void  dtb_call_help_callback(\n"
    "    Widget widget,\n"
    "    XtPointer clientData,\n"
    "    XtPointer callData\n"
    ");",
    
    /* def */
    "/* \n"
    "** dtb_call_help_callback()\n"
    "** Utility routine to call the help callbacks on a target widget.  This\n"
    "** is predominantly used to display help data on a dialog by having this\n"
    "** function as the activate callback on the dialog's help button.\n"
    "*/\n"
    "void \n"
    "dtb_call_help_callback(\n"
    "    Widget widget,\n"
    "    XtPointer clientData,\n"
    "    XtPointer callData\n"
    ")\n"
    "{\n"
    "	Widget target = (Widget)clientData;\n"
    "\n"
    "	XtCallCallbacks(target,XmNhelpCallback,(XtPointer)NULL);\n"
    "}"
}; /* abmfP_lrc_call_help_callback */
LibFunc abmfP_lib_call_help_callback = &(abmfP_lrc_call_help_callback);

/*
 * abmfP_lib_popup_menu_register
 */
static LibFuncRec abmfP_lrc_popup_menu_register = 
{
    /* name */
    "dtb_popup_menu_register",
    
    /* proto */
    "int  dtb_popup_menu_register(Widget popupMenu, Widget parent);",
    
    /* def */
    "/*\n"
    " * Registers a popup menu to be brought by button three on the parent\n"
    " */\n"
    "int \n"
    "dtb_popup_menu_register(Widget popupMenu, Widget parent)\n"
    "{\n"
    "    static XtTranslations	popupMenuTrans = NULL;\n"
    "    static XtActionsRec		menuActions[] =\n"
    "    {\n"
    "	{\"DtbPopupMenu\", (XtActionProc)dtb_popup_menu }\n"
    "    };\n"
    "    int		i = 0;\n"
    "    Boolean	foundEntry = False;\n"
    "\n"
    "    if (popupMenuTrans == NULL)\n"
    "    {\n"
    "	XtAppContext	appContext = \n"
    "		XtWidgetToApplicationContext(dtb_get_toplevel_widget());\n"
    "	XtAppAddActions(appContext, menuActions, XtNumber(menuActions));\n"
    "	popupMenuTrans = XtParseTranslationTable(\n"
    "				\"<Btn3Down>: DtbPopupMenu()\");\n"
    "\n"
    "    }\n"
    "    XtOverrideTranslations(parent, popupMenuTrans);\n"
    "\n"
    "    /*\n"
    "     * Save the reference from this widget to the menu\n"
    "     */\n"
    "    /* see if an entry already exists */\n"
    "    for (i = 0; i < numPopupMenus; ++i)\n"
    "    {\n"
    "	if (   (popupMenus[i].widget == parent)\n"
    "	    || (popupMenus[i].menu == popupMenu) )\n"
    "	{\n"
    "	    foundEntry = True;\n"
    "	    break;\n"
    "	}\n"
    "    }\n"
    "    /* look for an empty slot */\n"
    "    if (!foundEntry)\n"
    "    {\n"
    "	/* look for an empty slot */\n"
    "	for (i = 0; i < numPopupMenus; ++i)\n"
    "	{\n"
    "	    if (popupMenus[i].widget == 0)\n"
    "	    {\n"
    "		foundEntry = True;\n"
    "		break;\n"
    "	    }\n"
    "	}\n"
    "    }\n"
    "    /* make a new slot, if necessary */\n"
    "    if (!foundEntry)\n"
    "    {\n"
    "	/* It's not in the list - add it */\n"
    "	DtbMenuRef newPopupMenus = (DtbMenuRef)\n"
    "		realloc(popupMenus, sizeof(DtbMenuRefRec)*(numPopupMenus+1));\n"
    "	if (newPopupMenus != NULL)\n"
    "	{\n"
    "	    popupMenus = newPopupMenus;\n"
    "	    ++numPopupMenus;\n"
    "	    i = numPopupMenus-1;\n"
    "	    foundEntry = True;\n"
    "	}\n"
    "    }\n"
    "\n"
    "    /* we have a slot; fill it in */\n"
    "    if (foundEntry)\n"
    "    {\n"
    "        popupMenus[i].widget = parent;\n"
    "        popupMenus[i].menu = popupMenu;\n"
    "	XtAddCallback(popupMenus[i].widget,\n"
    "	    XmNdestroyCallback, dtb_popup_menu_destroyCB, (XtPointer)NULL);\n"
    "	XtAddCallback(popupMenus[i].menu,\n"
    "	    XmNdestroyCallback, dtb_popup_menu_destroyCB, (XtPointer)NULL);\n"
    "    }\n"
    "\n"
    "    return 0;\n"
    "}"
}; /* abmfP_lrc_popup_menu_register */
LibFunc abmfP_lib_popup_menu_register = &(abmfP_lrc_popup_menu_register);

/*
 * abmfP_lib_popup_menu
 */
static LibFuncRec abmfP_lrc_popup_menu = 
{
    /* name */
    "dtb_popup_menu",
    
    /* proto */
    "static void dtb_popup_menu(\n"
    "    Widget	widget,\n"
    "    XEvent	*event,\n"
    "    String	*params,\n"
    "    Cardinal	*num_params\n"
    ");",
    
    /* def */
    "static void\n"
    "dtb_popup_menu(\n"
    "    Widget	widget,\n"
    "    XEvent	*event,\n"
    "    String	*params,\n"
    "    Cardinal	*num_params\n"
    ")\n"
    "{\n"
    "    int		i = 0;\n"
    "    Widget	menu = 0;\n"
    "\n"
    "    if (event->type == ButtonPress)\n"
    "    {\n"
    "        for (i = 0 ; i < numPopupMenus; ++i)\n"
    "        {\n"
    "	    if (popupMenus[i].widget == widget)\n"
    "	    {\n"
    "		menu = popupMenus[i].menu;\n"
    "	    }\n"
    "	}\n"
    "    }\n"
    "\n"
    "    if (menu != 0)\n"
    "    {\n"
    "	XmMenuPosition(menu, (XButtonPressedEvent*)event);\n"
    "	XtManageChild(menu);\n"
    "    }\n"
    "}"
}; /* abmfP_lrc_popup_menu */
LibFunc abmfP_lib_popup_menu = &(abmfP_lrc_popup_menu);

/*
 * abmfP_lib_popup_menu_destroyCB
 */
static LibFuncRec abmfP_lrc_popup_menu_destroyCB = 
{
    /* name */
    "dtb_popup_menu_destroyCB",
    
    /* proto */
    "static void  dtb_popup_menu_destroyCB(\n"
    "			Widget		widget,\n"
    "			XtPointer	clientData,\n"
    "			XtPointer	callData\n"
    ");",
    
    /* def */
    "/*\n"
    " * This keeps the list of popup menus up-to-date, if widgets are destroyed\n"
    " */\n"
    "static void \n"
    "dtb_popup_menu_destroyCB(\n"
    "			Widget		widget,\n"
    "			XtPointer	clientData,\n"
    "			XtPointer	callData\n"
    ")\n"
    "{\n"
    "    int		i = 0;\n"
    "    for (i = 0; i < numPopupMenus; ++i)\n"
    "    {\n"
    "	if (   (popupMenus[i].widget == widget) \n"
    "	    || (popupMenus[i].menu == widget) )\n"
    "	{\n"
    "	    popupMenus[i].widget = 0;\n"
    "	    popupMenus[i].menu = NULL;\n"
    "	    break;\n"
    "	}\n"
    "    }\n"
    "}"
}; /* abmfP_lrc_popup_menu_destroyCB */
LibFunc abmfP_lib_popup_menu_destroyCB = &(abmfP_lrc_popup_menu_destroyCB);

/*
 * abmfP_lib_drag_site_register
 */
static LibFuncRec abmfP_lrc_drag_site_register = 
{
    /* name */
    "dtb_drag_site_register",
    
    /* proto */
    "int dtb_drag_site_register(\n"
    "                        Widget 			widget,\n"
    "			DtbDndDragCallback	callback,\n"
    "			DtDndProtocol		protocol,\n"
    "			unsigned char		operations,\n"
    "			Boolean			bufferIsText,\n"
    "			Boolean			allowDropOnRootWindow,\n"
    "			Pixmap			cursor,\n"
    "			Pixmap			cursorMask,\n"
    "			DtbDragSiteHandle	*dragSiteHandleOut\n"
    ");",
    
    /* def */
    "/*\n"
    " * Returns non-negative if successful.\n"
    " */\n"
    "int\n"
    "dtb_drag_site_register(\n"
    "                        Widget 			widget,\n"
    "			DtbDndDragCallback	callback,\n"
    "			DtDndProtocol		protocol,\n"
    "			unsigned char		operations,\n"
    "			Boolean			bufferIsText,\n"
    "			Boolean			allowDropOnRootWindow,\n"
    "			Pixmap			cursor,\n"
    "			Pixmap			cursorMask,\n"
    "			DtbDragSiteHandle	*dragSiteHandleOut\n"
    ")\n"
    "{\n"
    "    DtbDragSite dragSite = (DtbDragSite)XtCalloc(1,sizeof(DtbDragSiteRec));\n"
    "    Widget	sourceIcon = 0;\n"
    "\n"
    "    if (dragSite != NULL)\n"
    "    {\n"
    "        dragSite->widget = widget;\n"
    "    	dragSite->protocol = protocol;\n"
    "    	dragSite->operations = operations;\n"
    "	dragSite->bufferIsText = bufferIsText;\n"
    "	dragSite->allowDropOnRootWindow = allowDropOnRootWindow;\n"
    "	if ((cursor != 0) && (cursorMask != 0))\n"
    "	{\n"
    "	    dragSite->sourceIcon = \n"
    "			DtDndCreateSourceIcon(widget, cursor, cursorMask);\n"
    "	}\n"
    "    	dragSite->callback = callback;\n"
    "        dragSite->convertCBRec[0].callback = dtb_drag_convertCB;\n"
    "        dragSite->dragToRootCBRec[0].callback = dtb_drag_to_rootCB;\n"
    "        dragSite->dragFinishCBRec[0].callback = dtb_drag_finishCB;\n"
    "    \n"
    "        XtAddEventHandler(widget, Button1MotionMask, False,\n"
    "            (XtEventHandler)dtb_drag_button1_motion_handler, \n"
    "	    (XtPointer)dragSite);\n"
    "    }\n"
    "    \n"
    "    /*\n"
    "     * Pass back a handle, so that this can be freed, later. Unregistering\n"
    "     * drag sites is not currently implemented, but this gives the ability\n"
    "     * to provide this functionality in the future.\n"
    "     */\n"
    "    if (dragSiteHandleOut != NULL)\n"
    "    {\n"
    "        *dragSiteHandleOut = (DtbDragSiteHandle)dragSite;\n"
    "    }\n"
    "    return 0;\n"
    "}"
}; /* abmfP_lrc_drag_site_register */
LibFunc abmfP_lib_drag_site_register = &(abmfP_lrc_drag_site_register);

/*
 * abmfP_lib_drop_site_register
 */
static LibFuncRec abmfP_lrc_drop_site_register = 
{
    /* name */
    "dtb_drop_site_register",
    
    /* proto */
    "int dtb_drop_site_register(\n"
    "                        Widget                  widget,\n"
    "                        DtbDndDropCallback      callback,\n"
    "                        DtDndProtocol		protocols,\n"
    "                        unsigned char           operations,\n"
    "                        Boolean                 dropsOnChildren,\n"
    "                        Boolean                 preservePreviousRegistration,\n"
    "                        DtbDropSiteHandle       *dropSiteHandleOut\n"
    ");",
    
    /* def */
    "int\n"
    "dtb_drop_site_register(\n"
    "                        Widget                  widget,\n"
    "                        DtbDndDropCallback      callback,\n"
    "                        DtDndProtocol		protocols,\n"
    "                        unsigned char           operations,\n"
    "                        Boolean                 dropsOnChildren,\n"
    "                        Boolean                 preservePreviousRegistration,\n"
    "                        DtbDropSiteHandle       *dropSiteHandleOut\n"
    ")\n"
    "{\n"
    "    DtbDropSite dropSite = (DtbDropSite)NULL;\n"
    "\n"
    "    if (   (callback != NULL)\n"
    "	&& ((dropSite = (DtbDropSite)XtCalloc(1,sizeof(DtbDropSiteRec))) \n"
    "								!= NULL) \n"
    "       )\n"
    "    {\n"
    "	DtbDndDropRegisterInfoRec	regInfo;\n"
    "\n"
    "	/* initialize the data */\n"
    "        dropSite->widget = widget;\n"
    "        dropSite->callback = callback;\n"
    "        dropSite->protocols = protocols;\n"
    "        dropSite->operations = operations;\n"
    "        dropSite->dropsOnChildren = dropsOnChildren;\n"
    "        dropSite->preservePreviousRegistration = preservePreviousRegistration;\n"
    "        dropSite->animateCBRec[0].callback = dtb_drop_animateCB;\n"
    "        dropSite->animateCBRec[0].closure = (XtPointer)dropSite;\n"
    "        dropSite->transferCBRec[0].callback = dtb_drop_transferCB;\n"
    "        dropSite->transferCBRec[0].closure = (XtPointer)dropSite;\n"
    "\n"
    "        /* Let the client modify the drop site initialization */\n"
    "        regInfo.protocols = dropSite->protocols;\n"
    "        regInfo.operations = dropSite->operations;\n"
    "        regInfo.textIsBuffer = dropSite->textIsBuffer;\n"
    "        regInfo.preservePreviousRegistration = \n"
    "				dropSite->preservePreviousRegistration;\n"
    "        regInfo.respondToDropsOnChildren = dropSite->dropsOnChildren;\n"
    "        dropSite->callback(widget, DTB_DND_REGISTER, &regInfo, NULL, NULL);\n"
    "\n"
    "        /* actually register it! */\n"
    "        DtDndVaDropRegister(\n"
    "	    widget, regInfo.protocols, regInfo.operations,\n"
    "	    dropSite->transferCBRec,\n"
    "	    DtNregisterChildren, regInfo.respondToDropsOnChildren,\n"
    "	    DtNtextIsBuffer, regInfo.textIsBuffer,\n"
    "#ifdef DtNpreserveRegistration\n"
    "	    DtNpreserveRegistration, regInfo.preservePreviousRegistration,\n"
    "#endif\n"
    "	    DtNdropAnimateCallback, dropSite->animateCBRec,\n"
    "	    NULL);\n"
    "    }\n"
    "\n"
    "    if (dropSiteHandleOut != NULL)\n"
    "    {\n"
    "	*dropSiteHandleOut = (DtbDropSiteHandle)dropSite;\n"
    "    }\n"
    "\n"
    "    return ((dropSite == NULL)? -1:0);\n"
    "}"
}; /* abmfP_lrc_drop_site_register */
LibFunc abmfP_lib_drop_site_register = &(abmfP_lrc_drop_site_register);

/*
 * abmfP_lib_drag_terminate
 */
static LibFuncRec abmfP_lrc_drag_terminate = 
{
    /* name */
    "dtb_drag_terminate",
    
    /* proto */
    "static int dtb_drag_terminate(DtbDragSite dragSite);",
    
    /* def */
    "static int\n"
    "dtb_drag_terminate(DtbDragSite dragSite)\n"
    "{\n"
    "    dragInProgress = False;\n"
    "    dragInitialX = -1;\n"
    "    dragInitialY = -1;\n"
    "    dragSite->convertCBRec[0].closure = NULL;\n"
    "    dragSite->dragToRootCBRec[0].closure = NULL;\n"
    "    dragSite->dragFinishCBRec[0].closure = NULL;\n"
    "    return 0;\n"
    "}"
}; /* abmfP_lrc_drag_terminate */
LibFunc abmfP_lib_drag_terminate = &(abmfP_lrc_drag_terminate);

/*
 * abmfP_lib_drag_button1_motion_handler
 */
static LibFuncRec abmfP_lrc_drag_button1_motion_handler = 
{
    /* name */
    "dtb_drag_button1_motion_handler",
    
    /* proto */
    "static void dtb_drag_button1_motion_handler(\n"
    "        Widget          dragInitiator,\n"
    "        XtPointer       clientData,\n"
    "        XEvent         *event\n"
    ");",
    
    /* def */
    "/*\n"
    " * dragMotionHandler\n"
    " *\n"
    " * Determine if the pointer has moved beyond the drag threshold while button 1\n"
    " * was being held down.\n"
    " */\n"
    "static void\n"
    "dtb_drag_button1_motion_handler(\n"
    "        Widget          dragInitiator,\n"
    "        XtPointer       clientData,\n"
    "        XEvent         *event\n"
    ")\n"
    "{\n"
    "    int             	diffX, diffY;\n"
    "    DtbDragSite		dragSite = (DtbDragSite)clientData;\n"
    "\n"
    "    if (!dragInProgress) {\n"
    "        /*\n"
    "         * If the drag is just starting, set initial button down coords\n"
    "         */\n"
    "        if (dragInitialX == -1 && dragInitialY == -1) {\n"
    "                dragInitialX = event->xmotion.x;\n"
    "                dragInitialY = event->xmotion.y;\n"
    "        }\n"
    "\n"
    "        /*\n"
    "         * Find out how far pointer has moved since button press\n"
    "         */\n"
    "        diffX = dragInitialX - event->xmotion.x;\n"
    "        diffY = dragInitialY - event->xmotion.y;\n"
    "        \n"
    "        if ((ABS(diffX) >= DRAG_THRESHOLD) ||\n"
    "            (ABS(diffY) >= DRAG_THRESHOLD)) \n"
    "	{\n"
    "		dtb_drag_start(dragSite, event);\n"
    "                dragInitialX = -1;\n"
    "                dragInitialY = -1;\n"
    "        }\n"
    "    }\n"
    "}"
}; /* abmfP_lrc_drag_button1_motion_handler */
LibFunc abmfP_lib_drag_button1_motion_handler = &(abmfP_lrc_drag_button1_motion_handler);

/*
 * abmfP_lib_drag_start
 */
static LibFuncRec abmfP_lrc_drag_start = 
{
    /* name */
    "dtb_drag_start",
    
    /* proto */
    "static int dtb_drag_start(DtbDragSite dragSite, XEvent *event);",
    
    /* def */
    "static int\n"
    "dtb_drag_start(DtbDragSite dragSite, XEvent *event)\n"
    "{\n"
    "    DtbDndDragStartInfoRec	dragStart;\n"
    "    Arg				args[3];\n"
    "    int				n = 0;\n"
    "\n"
    "    dragInProgress = True;\n"
    "\n"
    "    dragSite->convertCBRec[0].closure = (XtPointer)dragSite;\n"
    "    dragSite->dragToRootCBRec[0].closure = (XtPointer)dragSite;\n"
    "    dragSite->dragFinishCBRec[0].closure = (XtPointer)dragSite;\n"
    "\n"
    "    /*\n"
    "     * Call the client callback to update values for drag start\n"
    "     */\n"
    "    memset((void*)&dragStart, 0, sizeof(DtbDndDragStartInfoRec));\n"
    "    dragStart.protocol= dragSite->protocol;\n"
    "    dragStart.operations = dragSite->operations;\n"
    "    dragStart.cursor = dragSite->sourceIcon;\n"
    "    dragStart.bufferIsText = dragSite->bufferIsText;\n"
    "    dragStart.allowDropOnRootWindow = dragSite->allowDropOnRootWindow;\n"
    "    dragStart.numItems = 1;\n"
    "    if (dragSite->callback != NULL)\n"
    "    {\n"
    "        dragSite->callback(DTB_DND_DRAG_START, &dragStart, NULL,NULL,NULL,NULL);\n"
    "    }\n"
    "\n"
    "    n = 0;\n"
    "    if (dragStart.cursor != NULL)\n"
    "    {\n"
    "	XtSetArg(args[n], DtNsourceIcon, dragStart.cursor); ++n;\n"
    "    }\n"
    "    if (   (dragStart.bufferIsText) \n"
    "	&& (dragStart.protocol == DtDND_BUFFER_TRANSFER) )\n"
    "    {\n"
    "	XtSetArg(args[n], DtNbufferIsText, True); ++n;\n"
    "    }\n"
    "    if (dragStart.allowDropOnRootWindow)\n"
    "    {\n"
    "	XtSetArg(args[n], DtNdropOnRootCallback, (XtPointer)dragSite->dragToRootCBRec); ++n;\n"
    "    }\n"
    "    if (DtDndDragStart(dragSite->widget, event, \n"
    "	dragStart.protocol,\n"
    "	dragStart.numItems,\n"
    "	dragStart.operations,\n"
    "	dragSite->convertCBRec, dragSite->dragFinishCBRec,\n"
    "	args, n) == NULL)\n"
    "    {\n"
    "	/* drag start failed */\n"
    "	dtb_drag_terminate(dragSite);\n"
    "	return -1;\n"
    "    }\n"
    "    return 0;\n"
    "}"
}; /* abmfP_lrc_drag_start */
LibFunc abmfP_lib_drag_start = &(abmfP_lrc_drag_start);

/*
 * abmfP_lib_drag_convertCB
 */
static LibFuncRec abmfP_lrc_drag_convertCB = 
{
    /* name */
    "dtb_drag_convertCB",
    
    /* proto */
    "static void dtb_drag_convertCB(\n"
    "			Widget		dragContext,\n"
    "			XtPointer	clientData,\n"
    "			XtPointer	callData\n"
    ");",
    
    /* def */
    "static void\n"
    "dtb_drag_convertCB(\n"
    "			Widget		dragContext,\n"
    "			XtPointer	clientData,\n"
    "			XtPointer	callData\n"
    ")\n"
    "{\n"
    "    DtbDragSite			dragSite = (DtbDragSite)clientData;\n"
    "    DtDndConvertCallback	convert = (DtDndConvertCallback)callData;\n"
    "    DtDndContext		*dragData = convert->dragData;\n"
    "    int				i = 0;\n"
    "\n"
    "    switch (convert->reason)\n"
    "    {\n"
    "	case DtCR_DND_CONVERT_DATA:\n"
    "	    for (i = 0; i < dragData->numItems; ++i)\n"
    "	    {\n"
    "		switch (dragData->protocol)\n"
    "		{\n"
    "		    case DtDND_BUFFER_TRANSFER:\n"
    "			dragData->data.buffers[i].bp = NULL;\n"
    "			dragData->data.buffers[i].size = 0;\n"
    "		    break;\n"
    "		    case DtDND_FILENAME_TRANSFER:\n"
    "			dragData->data.files[i] = \"/etc/passwd\";\n"
    "		    break;\n"
    "		    case DtDND_TEXT_TRANSFER:\n"
    "			dragData->data.strings[i] = XmStringCreateLocalized(\"Hello, world!\\n\");\n"
    "		    break;\n"
    "		}\n"
    "	    }\n"
    "	    dragSite->callback(DTB_DND_CONVERT,NULL,convert,NULL,NULL,NULL);\n"
    "	break;\n"
    "\n"
    "	case DtCR_DND_CONVERT_DELETE:\n"
    "	    dragSite->callback(DTB_DND_DELETE,NULL,NULL,NULL,convert,NULL);\n"
    "	break;\n"
    "    }\n"
    "}"
}; /* abmfP_lrc_drag_convertCB */
LibFunc abmfP_lib_drag_convertCB = &(abmfP_lrc_drag_convertCB);

/*
 * abmfP_lib_drag_to_rootCB
 */
static LibFuncRec abmfP_lrc_drag_to_rootCB = 
{
    /* name */
    "dtb_drag_to_rootCB",
    
    /* proto */
    "static void dtb_drag_to_rootCB(\n"
    "			Widget		dragContext,\n"
    "			XtPointer	clientData,\n"
    "			XtPointer	callData\n"
    ");",
    
    /* def */
    "static void\n"
    "dtb_drag_to_rootCB(\n"
    "			Widget		dragContext,\n"
    "			XtPointer	clientData,\n"
    "			XtPointer	callData\n"
    ")\n"
    "{\n"
    "    DtbDragSite			dragSite = (DtbDragSite)clientData;\n"
    "    DtbDndDroppedOnRootWindowInfoRec	dropInfo;\n"
    "    memset(&dropInfo, 0, sizeof(DtbDndDroppedOnRootWindowInfoRec));\n"
    "\n"
    "    dropInfo.droppedOnRootWindow = True;\n"
    "    dragSite->callback(DTB_DND_DROPPED_ON_ROOT_WINDOW,\n"
    "			NULL,NULL,&dropInfo,NULL,NULL);\n"
    "}"
}; /* abmfP_lrc_drag_to_rootCB */
LibFunc abmfP_lib_drag_to_rootCB = &(abmfP_lrc_drag_to_rootCB);

/*
 * abmfP_lib_drag_finishCB
 */
static LibFuncRec abmfP_lrc_drag_finishCB = 
{
    /* name */
    "dtb_drag_finishCB",
    
    /* proto */
    "static void dtb_drag_finishCB(\n"
    "			Widget		dragContext,\n"
    "			XtPointer	clientData,\n"
    "			XtPointer	callData\n"
    ");",
    
    /* def */
    "static void\n"
    "dtb_drag_finishCB(\n"
    "			Widget		dragContext,\n"
    "			XtPointer	clientData,\n"
    "			XtPointer	callData\n"
    ")\n"
    "{\n"
    "    DtbDragSite			dragSite = (DtbDragSite)clientData;\n"
    "    DtDndDragFinishCallback	finish = (DtDndDragFinishCallback)callData;\n"
    "\n"
    "    dragSite->callback(DTB_DND_FINISH,NULL,NULL,NULL,NULL,finish);\n"
    "    dtb_drag_terminate((DtbDragSite)clientData);\n"
    "}"
}; /* abmfP_lrc_drag_finishCB */
LibFunc abmfP_lib_drag_finishCB = &(abmfP_lrc_drag_finishCB);

/*
 * abmfP_lib_default_dragCB
 */
static LibFuncRec abmfP_lrc_default_dragCB = 
{
    /* name */
    "dtb_default_dragCB",
    
    /* proto */
    "void  dtb_default_dragCB(\n"
    "			DTB_DND_REQUEST			request,\n"
    "			DtbDndDragStartInfo		dragStart,\n"
    "			DtDndConvertCallback		convert,\n"
    "			DtbDndDroppedOnRootWindowInfo	dropOnRootWindow,\n"
    "			DtDndConvertCallback		deleteSource,\n"
    "			DtDndDragFinishCallback		finish\n"
    ");",
    
    /* def */
    "void \n"
    "dtb_default_dragCB(\n"
    "			DTB_DND_REQUEST			request,\n"
    "			DtbDndDragStartInfo		dragStart,\n"
    "			DtDndConvertCallback		convert,\n"
    "			DtbDndDroppedOnRootWindowInfo	dropOnRootWindow,\n"
    "			DtDndConvertCallback		deleteSource,\n"
    "			DtDndDragFinishCallback		finish\n"
    ")\n"
    "{\n"
    "    /*** DTB_USER_CODE_START vvv Add C variables and code below vvv ***/\n"
    "    /*** DTB_USER_CODE_END   ^^^ Add C variables and code above ^^^ ***/\n"
    "\n"
    "    switch (request)\n"
    "    {\n"
    "	case DTB_DND_DRAG_START:\n"
    "        /** DTB_USER_CODE_START \n"
    "	 **\n"
    "	 ** The drag will start normally, using the defaults for this\n"
    "	 ** drag site, and one item to transfer assumed.\n"
    "	 **\n"
    "	 ** To override the default protocol, operations, cursor, or \n"
    "	 ** number of of items to transfer, add code after this comment.\n"
    "	 **/\n"
    "        printf(\"action: dtb_default_dragCB(DTB_DND_DRAG_START)\\n\");\n"
    "        /*** DTB_USER_CODE_END   ^^^ Add C code above ^^^ ***/\n"
    "	break;\n"
    "\n"
    "	case DTB_DND_CONVERT:\n"
    "        /** DTB_USER_CODE_START\n"
    "	 **\n"
    "	 ** Here, the data must be supplied to the dropped-on object.\n"
    "	 **/\n"
    "        printf(\"action: dtb_default_dragCB(DTB_DND_CONVERT)\\n\");\n"
    "        /*** DTB_USER_CODE_END   ^^^ Add C code above ^^^ ***/\n"
    "	break;\n"
    "\n"
    "	case DTB_DND_DELETE:\n"
    "        /** DTB_USER_CODE_START\n"
    "	 **\n"
    "	 ** A move has been completed. Add code to delete the source\n"
    "	 ** object, after this comment.\n"
    "	 **/\n"
    "        printf(\"action: dtb_default_dragCB(DTB_DND_DELETE)\\n\");\n"
    "        /*** DTB_USER_CODE_END   ^^^ Add C code above ^^^ ***/\n"
    "	break;\n"
    "\n"
    "	case DTB_DND_DROPPED_ON_ROOT_WINDOW:\n"
    "        /** DTB_USER_CODE_START\n"
    "	 **\n"
    "	 ** The dragged data was dropped on the root window.\n"
    "	 **/\n"
    "        printf(\"action: dtb_default_dragCB(DTB_DND_DROPPED_ON_ROOT_WINDOW)\\n\");\n"
    "        /*** DTB_USER_CODE_END   ^^^ Add C code above ^^^ ***/\n"
    "	break;\n"
    "\n"
    "	case DTB_DND_FINISH:\n"
    "        /** DTB_USER_CODE_START\n"
    "	 **\n"
    "	 ** The drag and drop operation has completed (successfully or\n"
    "	 ** unsuccessfully). Clean up and allocated memory or other\n"
    "	 ** resources allocated during this drag.\n"
    "	 **/\n"
    "        printf(\"action: dtb_default_dragCB(DTB_DND_FINISH)\\n\");\n"
    "        /*** DTB_USER_CODE_END   ^^^ Add C code above ^^^ ***/\n"
    "	break;\n"
    "    }\n"
    "    \n"
    "    /*** DTB_USER_CODE_START vvv Add C code below vvv ***/\n"
    "    /*** DTB_USER_CODE_END   ^^^ Add C code above ^^^ ***/\n"
    "}"
}; /* abmfP_lrc_default_dragCB */
LibFunc abmfP_lib_default_dragCB = &(abmfP_lrc_default_dragCB);

/*
 * abmfP_lib_default_dropCB
 */
static LibFuncRec abmfP_lrc_default_dropCB = 
{
    /* name */
    "dtb_default_dropCB",
    
    /* proto */
    "void  dtb_default_dropCB(\n"
    "    Widget			widget,\n"
    "    DTB_DND_REQUEST		request,\n"
    "    DtbDndDropRegisterInfo	registerInfo,\n"
    "    DtDndTransferCallback	receiveInfo,\n"
    "    DtDndDropAnimateCallback	animateInfo\n"
    ");",
    
    /* def */
    "/*\n"
    " * NOTE: THE PARAMETERS OF TYPE *Callback ARE NOT CALLBACKS! THEY ARE\n"
    " *       STANDARD DATA STRUCTURES.\n"
    " */\n"
    "void \n"
    "dtb_default_dropCB(\n"
    "    Widget			widget,\n"
    "    DTB_DND_REQUEST		request,\n"
    "    DtbDndDropRegisterInfo	registerInfo,\n"
    "    DtDndTransferCallback	receiveInfo,\n"
    "    DtDndDropAnimateCallback	animateInfo\n"
    ")\n"
    "{\n"
    "    /*** DTB_USER_CODE_START vvv Add C variables and code below vvv ***/\n"
    "    /*** DTB_USER_CODE_END   ^^^ Add C variables and code above ^^^ ***/\n"
    "\n"
    "    /*\n"
    "     * The cases in this switch statement have been written in the order\n"
    "     * that these requests will normally be received.\n"
    "     */\n"
    "    switch (request)\n"
    "    {\n"
    "	case DTB_DND_REGISTER:\n"
    "        /** DTB_USER_CODE_START \n"
    "	 **\n"
    "	 ** The drag site will be automatically be registered with the \n"
    "	 ** options chosen in the Application Builder, with default values\n"
    "	 ** for any implementation-specific options.\n"
    "	 **\n"
    "	 ** To override the options chosen for this drop site, or to \n"
    "	 ** affect implementation-specific options not available from\n"
    "	 ** the Application Builder, add code after this comment.\n"
    "	 **/\n"
    "        printf(\"action: dtb_default_dropCB(DTB_DND_REGISTER)\\n\");\n"
    "        /*** DTB_USER_CODE_END   ^^^ Add C code above ^^^ ***/\n"
    "	break;\n"
    "\n"
    "	case DTB_DND_RECEIVE_DATA:\n"
    "        /** DTB_USER_CODE_START\n"
    "	 **\n"
    "	 ** The data has been supplied by the dragged-from object. The\n"
    "	 ** dragged data will be freed after this function returns, so\n"
    "	 ** any data needed in the future must be copied.\n"
    "	 **/\n"
    "        printf(\"action: dtb_default_dropCB(DTB_DND_RECEIVE_DATA)\\n\");\n"
    "        /*** DTB_USER_CODE_END   ^^^ Add C code above ^^^ ***/\n"
    "	break;\n"
    "\n"
    "	case DTB_DND_ANIMATE:\n"
    "        /** DTB_USER_CODE_START\n"
    "	 **\n"
    "	 ** Give visual feedback on the acceptability of the provided\n"
    "	 ** data.\n"
    "	 **/\n"
    "        printf(\"action: dtb_default_dropCB(DTB_DND_ANIMATE)\\n\");\n"
    "        /*** DTB_USER_CODE_END   ^^^ Add C code above ^^^ ***/\n"
    "	break;\n"
    "    }\n"
    "    \n"
    "    /*** DTB_USER_CODE_START vvv Add C code below vvv ***/\n"
    "    /*** DTB_USER_CODE_END   ^^^ Add C code above ^^^ ***/\n"
    "}"
}; /* abmfP_lrc_default_dropCB */
LibFunc abmfP_lib_default_dropCB = &(abmfP_lrc_default_dropCB);

/*
 * abmfP_lib_drop_animateCB
 */
static LibFuncRec abmfP_lrc_drop_animateCB = 
{
    /* name */
    "dtb_drop_animateCB",
    
    /* proto */
    "static void dtb_drop_animateCB(\n"
    "			Widget		dragContext,\n"
    "			XtPointer	clientData,\n"
    "			XtPointer	callData\n"
    ");",
    
    /* def */
    "static void\n"
    "dtb_drop_animateCB(\n"
    "			Widget		dragContext,\n"
    "			XtPointer	clientData,\n"
    "			XtPointer	callData\n"
    ")\n"
    "{\n"
    "    DtbDropSite		dropSite = (DtbDropSite)clientData;\n"
    "    DtDndDropAnimateCallback 	animateInfo = \n"
    "					(DtDndDropAnimateCallback)callData;\n"
    "    dropSite->callback(\n"
    "	dropSite->widget, DTB_DND_ANIMATE, NULL, NULL, animateInfo);\n"
    "}"
}; /* abmfP_lrc_drop_animateCB */
LibFunc abmfP_lib_drop_animateCB = &(abmfP_lrc_drop_animateCB);

/*
 * abmfP_lib_drop_transferCB
 */
static LibFuncRec abmfP_lrc_drop_transferCB = 
{
    /* name */
    "dtb_drop_transferCB",
    
    /* proto */
    "static void dtb_drop_transferCB(\n"
    "			Widget		dragContext,\n"
    "			XtPointer	clientData,\n"
    "			XtPointer	callData\n"
    ");",
    
    /* def */
    "static void\n"
    "dtb_drop_transferCB(\n"
    "			Widget		dragContext,\n"
    "			XtPointer	clientData,\n"
    "			XtPointer	callData\n"
    ")\n"
    "{\n"
    "    DtbDropSite			dropSite = (DtbDropSite)clientData;\n"
    "    DtDndTransferCallback	transferInfo = (DtDndTransferCallback)callData;\n"
    "\n"
    "    dropSite->callback(\n"
    "	dropSite->widget, DTB_DND_RECEIVE_DATA, NULL, transferInfo, NULL);\n"
    "}"
}; /* abmfP_lrc_drop_transferCB */
LibFunc abmfP_lib_drop_transferCB = &(abmfP_lrc_drop_transferCB);
