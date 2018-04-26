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
/* $XConsortium: icon.c /main/3 1995/10/27 10:39:09 rswiston $ */
/*****************************************************************************
 *****************************************************************************
 **
 **   File:         icon.c
 **
 **   Description:  Icon support functions for the CDE Drag & Drop Demo.
 **
 **  (c) Copyright 1993, 1994 Hewlett-Packard Company
 **  (c) Copyright 1993, 1994 International Business Machines Corp.
 **  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 **  (c) Copyright 1993, 1994 Unix System Labs, Inc., a subsidiary of
 **      Novell, Inc.
 **
 ****************************************************************************
 ************************************<+>*************************************/

#include <stdio.h>

#include <Xm/Screen.h>

#include <Dt/Dts.h>
#include <Dt/Dnd.h>

#include "icon.h"

 /*************************************************************************
 *
 *       Data Structures & Private Declarations For Icon Handling Functions
 *
 **************************************************************************/

#define ICON_TEXT_YGAP  2

#define ICON_PATH               "/usr/dt/appconfig/icons/C"
#define ICON_BITMAP_SUFFIX      "m.bm"
#define ICON_MASK_SUFFIX        "m_m.bm"

/*
 * Default Generic Icon Bitmap & Mask
 */

#define iconWidth 32
#define iconHeight 32
static unsigned char iconBits[] = {
   0x00, 0x00, 0x00, 0x00, 0xf8, 0xff, 0xff, 0x0f, 0x08, 0x00, 0x00, 0x08,
   0x08, 0x00, 0x00, 0x08, 0x08, 0x00, 0x00, 0x08, 0x88, 0x9d, 0x6d, 0x08,
   0x08, 0x00, 0x00, 0x08, 0x08, 0x00, 0x00, 0x08, 0x88, 0xf9, 0xed, 0x08,
   0x08, 0x00, 0x00, 0x08, 0xc8, 0x6e, 0x7d, 0x08, 0x08, 0x00, 0x00, 0x08,
   0xc8, 0x9d, 0xef, 0x09, 0x08, 0x00, 0x00, 0x08, 0xc8, 0xdd, 0xb3, 0x08,
   0x08, 0x00, 0x00, 0x08, 0xc8, 0xdf, 0xfd, 0x09, 0x08, 0x00, 0x00, 0x08,
   0xc8, 0xf6, 0xde, 0x08, 0x08, 0x00, 0x00, 0x08, 0x08, 0x00, 0x00, 0x08,
   0x88, 0xed, 0xd2, 0x08, 0x08, 0x00, 0x00, 0x08, 0xc8, 0xbb, 0xf3, 0x08,
   0x08, 0x00, 0x00, 0x08, 0x48, 0xdf, 0x5e, 0x08, 0x08, 0x00, 0x00, 0x08,
   0xc8, 0x36, 0x00, 0x08, 0x08, 0x00, 0x00, 0x08, 0x08, 0x00, 0x00, 0x08,
   0x08, 0x00, 0x00, 0x08, 0xf8, 0xff, 0xff, 0x0f};
 
#define iconMaskWidth 32
#define iconMaskHeight 32
static unsigned char iconMaskBits[] = {
   0x00, 0x00, 0x00, 0x00, 0xf8, 0xff, 0xff, 0x0f, 0xf8, 0xff, 0xff, 0x0f,
   0xf8, 0xff, 0xff, 0x0f, 0xf8, 0xff, 0xff, 0x0f, 0xf8, 0xff, 0xff, 0x0f,
   0xf8, 0xff, 0xff, 0x0f, 0xf8, 0xff, 0xff, 0x0f, 0xf8, 0xff, 0xff, 0x0f,
   0xf8, 0xff, 0xff, 0x0f, 0xf8, 0xff, 0xff, 0x0f, 0xf8, 0xff, 0xff, 0x0f,
   0xf8, 0xff, 0xff, 0x0f, 0xf8, 0xff, 0xff, 0x0f, 0xf8, 0xff, 0xff, 0x0f,
   0xf8, 0xff, 0xff, 0x0f, 0xf8, 0xff, 0xff, 0x0f, 0xf8, 0xff, 0xff, 0x0f,
   0xf8, 0xff, 0xff, 0x0f, 0xf8, 0xff, 0xff, 0x0f, 0xf8, 0xff, 0xff, 0x0f,
   0xf8, 0xff, 0xff, 0x0f, 0xf8, 0xff, 0xff, 0x0f, 0xf8, 0xff, 0xff, 0x0f,
   0xf8, 0xff, 0xff, 0x0f, 0xf8, 0xff, 0xff, 0x0f, 0xf8, 0xff, 0xff, 0x0f,
   0xf8, 0xff, 0xff, 0x0f, 0xf8, 0xff, 0xff, 0x0f, 0xf8, 0xff, 0xff, 0x0f,
   0xf8, 0xff, 0xff, 0x0f, 0xf8, 0xff, 0xff, 0x0f};
 
/*
 * Blank Bitmap
 */

#define blankWidth 16
#define blankHeight 16
static unsigned char blankBits[] = {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

/*
 * Private Icon Handling Function Declarations
 */

static void	iconClearBitmap(Display*, Window, GC, Pixmap, int, int);
static Pixmap	iconDefaultBitmap(Display*, Window, IconBitmapPart,
			unsigned short*, unsigned short*);
static void	iconFree(IconInfo*);
static Pixmap	iconGetBitmap(Widget, char*, IconBitmapPart, unsigned short*,
			unsigned short*);
static char	*iconGetIconPath(void*, int, IconTyping, IconBitmapPart);
static Pixmap	iconMergeBitmaps(Widget, Pixmap, int, int, Pixmap, int, int,
			int, int, Boolean);

 /*************************************************************************
 *
 *	Public Icon Handling Functions
 *
 **************************************************************************/

/*
 * IconCreateDouble
 *
 * Create a double icon from a single icon. The double icon has a bitmap
 * consisting of 2 images of the original bitmap offset from one another.
 */
void
IconCreateDouble(
	Widget		widget,
	IconInfo	*oldIcon,
	int		xOffset,
	int		yOffset,
	IconInfo	*newIcon,
	short		xx,
	short		yy)
{
	newIcon->bitmap = iconMergeBitmaps(widget,
		oldIcon->bitmap, oldIcon->icon.width, oldIcon->icon.height,
		oldIcon->bitmap, oldIcon->icon.width, oldIcon->icon.height,
		xOffset, yOffset, False);
	newIcon->mask = iconMergeBitmaps(widget,
		oldIcon->mask, oldIcon->icon.width, oldIcon->icon.height,
		oldIcon->mask, oldIcon->icon.width, oldIcon->icon.height,
		xOffset, yOffset, True);
	newIcon->icon.width = oldIcon->icon.width + 10;
	newIcon->icon.height = oldIcon->icon.height + 10;
	newIcon->name = XtNewString("Multiple");
	newIcon->icon.x = xx;
	newIcon->icon.y = yy;
	newIcon->dragIcon = DtDndCreateSourceIcon(widget,
		newIcon->bitmap, newIcon->mask);
}

/*
 * IconDelete
 *
 * Remove the icon from its list and deallocate memory used by the icon.
 */
void
IconDelete(
	Widget		drawArea,
	IconInfo	*iconPtr)
{
	IconInfo	*iconList;

	if (iconPtr == NULL || drawArea == NULL) {
		return;
	}

	if (iconPtr->next != NULL) { /* iconPtr is not the tail of the list */
		iconPtr->next->prev = iconPtr->prev;
	}

	if (iconPtr->prev != NULL) { /* iconPtr is not the head of the list */
		iconPtr->prev->next = iconPtr->next;
	}
	if (iconPtr->next == NULL) { /* iconPtr is the tail of the list */
		iconList = iconPtr->prev;
	} else {
		iconList = iconPtr->next;
	}

	while (iconList != NULL && iconList->prev != NULL) {
		iconList = iconList->prev;
	}
	iconFree(iconPtr);

	XtVaSetValues(drawArea, XmNuserData, iconList, NULL);
}

/*
 * IconDraw
 *
 * Render given icon in the window of the given widget.
 */
void
IconDraw(
	Widget		widget,
	IconInfo	*iconPtr)
{
	static GC	graphicsContext = NULL;
	static XFontStruct *fontStruct;
	Display		*display = XtDisplayOfObject(widget);
	Window		window = XtWindow(widget);
	int		screen = DefaultScreen(display);
	XGCValues	gcValues;

	if (graphicsContext == NULL) {
		gcValues.foreground = BlackPixel(display, screen);
                gcValues.background = WhitePixel(display, screen);
                graphicsContext = XCreateGC(display, window,
                                GCForeground | GCBackground, &gcValues);
	}

	/*
	 * Draw icon
	 */

	gcValues.clip_mask = iconPtr->mask;
	gcValues.clip_x_origin = iconPtr->icon.x;
	gcValues.clip_y_origin = iconPtr->icon.y;
	XChangeGC(display, graphicsContext,
		GCClipMask | GCClipXOrigin | GCClipYOrigin, &gcValues);
	XCopyPlane(display, iconPtr->bitmap, window, graphicsContext, 0, 0,
		iconPtr->icon.width, iconPtr->icon.height,
		iconPtr->icon.x, iconPtr->icon.y, 1L);

	/*
	 * Draw icon name centered below icon
	 */

        if (iconPtr->name != NULL) {
                int             nameX, nameY;
                int             direction, ascent, decent;
                XCharStruct     overall;
                Widget          xmScreen;
 
                if (fontStruct == NULL) {
                        xmScreen = XmGetXmScreen(XtScreen(widget));
                        XtVaGetValues(xmScreen, XmNfont, &fontStruct, NULL);
                }
                XTextExtents(fontStruct, iconPtr->name, strlen(iconPtr->name),
                        &direction, &ascent, &decent, &overall);
 
                nameX = (iconPtr->icon.x + (iconPtr->icon.width/2)) -
                        (overall.width/2);
                nameY = iconPtr->icon.y + iconPtr->icon.height +
                        ICON_TEXT_YGAP + ascent;
 
                gcValues.font = fontStruct->fid;
                gcValues.clip_mask = None;
                XChangeGC(display, graphicsContext,
                        GCFont | GCClipMask, &gcValues);
                XDrawString(display, window, graphicsContext, nameX, nameY,
                        iconPtr->name, strlen(iconPtr->name));
        }
}

/*
 * IconInitialize
 *
 * Initialize given icon with given data. Get icon bitmap based on data
 * associated with the icon. This data may be a file name, a buffer or
 * a type name. Create source icon for dragging based on the bitmap and mask.
 */
void
IconInitialize(
	Widget		widget,
	IconInfo	*iconPtr,
	short		x,
	short		y,
	void		*data,
	int		dataLen,
	char		*name,
	IconTyping	typing)
{
	char		*iconPath;

	iconPath = iconGetIconPath(data, dataLen, typing, IconMask);
	iconPtr->mask = iconGetBitmap(widget, iconPath, IconMask,
		&(iconPtr->icon.width), &(iconPtr->icon.height));
	XtFree(iconPath);

	iconPath = iconGetIconPath(data, dataLen, typing, IconBitmap);
	iconPtr->bitmap = iconGetBitmap(widget, iconPath, IconBitmap,
		&(iconPtr->icon.width), &(iconPtr->icon.height));
	XtFree(iconPath);

        iconPtr->icon.x = x;
	iconPtr->icon.y = y;
        iconPtr->name = XtNewString(name);
        iconPtr->dragIcon = DtDndCreateSourceIcon(widget, 
                iconPtr->bitmap, iconPtr->mask); 
}

/*
 * IconNew
 *
 * Allocate memory for a new icon structure and clear.
 */
IconInfo*
IconNew()
{
	IconInfo	*iconPtr;

	iconPtr = (IconInfo*) XtMalloc(sizeof(IconInfo));
	memset(iconPtr, 0x00, sizeof(IconInfo));

	return iconPtr;
}

 /*************************************************************************
 *
 *	Private Icon Handling Functions
 *
 **************************************************************************/

/*
 * iconClearBitmap
 *
 * Clear a bitmap by filling it with zeros.
 */
static void
iconClearBitmap(
	Display		*display,
	Window		window,
	GC		graphicsContext,
	Pixmap		bitmap,
	int		width,
	int		height)
{
	int		xx, yy;
	static Pixmap	blankBitmap = NULL;

	if (blankBitmap == NULL) {
		blankBitmap = XCreateBitmapFromData(display, window,
			(char *)blankBits, blankWidth, blankHeight);
	}
	for (xx = 0; xx < width + blankWidth; xx += blankWidth) {
	        for (yy = 0; yy < height + blankHeight; yy += blankHeight) {
			XCopyArea(display, blankBitmap, bitmap,
				graphicsContext, 0, 0,
				blankWidth, blankHeight, xx, yy);
		}
	}
}

/*
 * iconDefaultBitmap
 *
 * Create default icon bitmap or mask and set width and height accordingly.
 */
static Pixmap
iconDefaultBitmap(
	Display		*display,
	Window		window,
	IconBitmapPart	iconPart,
	unsigned short	*width,
	unsigned short	*height)
{
	static Pixmap	bitmap;
	static Pixmap	mask;

	if (iconPart == IconMask) { /* create default mask */
		if (mask == NULL) {
			mask = XCreateBitmapFromData(display, window,
			   (char *)iconMaskBits, iconMaskWidth, iconMaskHeight);
		}
		*width = iconMaskWidth;
		*height = iconMaskHeight;

		return mask;

	} else { /* create default bitmap */
		if (bitmap == NULL) {
			bitmap = XCreateBitmapFromData(display, window,
				(char *)iconBits, iconWidth, iconHeight);
		}
		*width = iconWidth;
		*height = iconHeight;

		return bitmap;
	}
}

/*
 * iconFree
 *
 * Deallocate the icon and associated resources.
 */
static void
iconFree(
	IconInfo	*iconPtr)
{
	Display		*display = XtDisplayOfObject(iconPtr->dragIcon);

	if (iconPtr == NULL) {
		return;
	}

	XtFree(iconPtr->name);
	/*
	 * REMIND: This needs to free other things too...
	 * XFreePixmap(display, iconPtr->bitmap);
	 * XFreePixmap(display, iconPtr->mask);
	 * Free(iconPtr->dragIcon);
	 */
	XtFree((char *)iconPtr);
}

/*
 * iconGetBitmap
 *
 * Get a bitmap for the icon based on the path and name of the icon. 
 * If no bitmap file is found use a built-in default.
 */
static Pixmap
iconGetBitmap(
	Widget		widget,
	char		*iconPath,
	IconBitmapPart	iconPart,
	unsigned short	*returnWidth,
	unsigned short	*returnHeight)
{
	Display		*display = XtDisplayOfObject(widget);
	Window		window = DefaultRootWindow(display);
	int		status;
	int		xHot, yHot;
	Pixmap		bitmap;
	unsigned int	width, height;
	
	*returnWidth = *returnHeight = 0;

	if (iconPath != NULL) {

		status = XReadBitmapFile(display, window, iconPath,
			&width, &height, &bitmap, &xHot, &yHot);

		if (status == BitmapSuccess) {
			*returnWidth = (unsigned short) width;
			*returnHeight = (unsigned short) height;
		} else {
			printf(
			    "Unable to read icon from bitmap file \"%s\".\n"
			    "Using default icon bitmap.\n",
			    iconPath, NULL);
			bitmap = iconDefaultBitmap(display, window, iconPart,
				returnWidth, returnHeight);
		}
	} else {
		bitmap = iconDefaultBitmap(display, window, iconPart,
			returnWidth, returnHeight);
	}
	return bitmap;
}

/*
 * iconGetIconPath
 *
 * Get the file path and name for an icon based on the given data and
 * method of typing that data. This function assumes the data typing
 * database has been loaded with DtDtsLoadDataTypes().
 */
static char*
iconGetIconPath(
	void		*data,
	int		dataLen,
	IconTyping	typing,
	IconBitmapPart	iconPart)
{
	char		iconPath[MAXPATHLEN + 1],
			*iconName,
			*iconSuffix;

	switch (typing) {
	case IconByFile:
		iconName = DtDtsFileToAttributeValue(data, "ICON");
		break;
	case IconByData:
		iconName = DtDtsBufferToAttributeValue(data, dataLen,
			"ICON", NULL);
		break;
	case IconByType:
		iconName = DtDtsDataTypeToAttributeValue(data, "ICON", NULL);
		break;
	default:
		iconName = NULL;
	}
	if (iconName != NULL) {
		if (iconPart == IconMask) {
			iconSuffix = ICON_MASK_SUFFIX;
		} else {
			iconSuffix = ICON_BITMAP_SUFFIX;
		}
		sprintf(iconPath, "%s/%s.%s", ICON_PATH, iconName, iconSuffix);
		DtDtsFreeAttributeValue(iconName);
	}
	return XtNewString(iconPath);
}

/*
 * iconMergeBitmaps
 *
 * Create new bitmap consisting of the offset composition of the given bitmaps.
 */
static Pixmap
iconMergeBitmaps(
	Widget		widget,
	Pixmap		bitmap1,
	int		width1,
	int		height1,
	Pixmap		bitmap2,
	int		width2,
	int		height2,
	int		xOffset,
	int		yOffset,
	Boolean		clear)
{
	Display		*display = XtDisplayOfObject(widget);
	Window		window = DefaultRootWindow(display);
	int		screen = DefaultScreen(display);
	Pixmap		mergedBitmap, blankBitmap;
	int		extraX, extraY, width, height;
	static GC	graphicsContext = NULL;
	XGCValues	gcValues;

	extraX = width2 - width1 + xOffset + 1;
	if (extraX < 0) extraX = 0;
	width = width1 + extraX;

	extraY = height2 - height1 + yOffset + 1;
	if (extraY < 0) extraY = 0;
	height = height1 + extraY;

	mergedBitmap = XCreatePixmap(display, window, width, height, 1);

	if (graphicsContext == NULL) {
                graphicsContext = XCreateGC(display, mergedBitmap, 0L, NULL);
	}

	if (clear) {
		iconClearBitmap(display, window, graphicsContext, mergedBitmap,
			width, height);
	}

	XCopyArea(display, bitmap2, mergedBitmap, graphicsContext, 0, 0,
		width2, height2, xOffset, yOffset);

	XCopyArea(display, bitmap1, mergedBitmap, graphicsContext, 0, 0,
		width1, height1, 0, 0);

	return mergedBitmap;
}

