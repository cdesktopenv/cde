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
/* $XConsortium: constants.h /main/3 1995/11/02 14:03:35 rswiston $ */
/*********************************************************************
*  (c) Copyright 1993, 1994 Hewlett-Packard Company	
*  (c) Copyright 1993, 1994 International Business Machines Corp.
*  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
*  (c) Copyright 1993, 1994 Unix System Labs, Inc., a subsidiary of
*      Novell, Inc.
**********************************************************************/
#include <X11/Xlib.h>

/***
#define DEBUG         True
***/

#define CLASS_NAME    "Dticon"

/*
 * This is the linked list of Help dialogs, which
 * will be created dynamically, as needed.
 */

typedef struct _helpStruct {
    struct _helpStruct *pNext;
    struct _helpStruct *pPrev;
    Widget dialog;
    Boolean inUse;
} HelpStruct;

typedef struct {
    Boolean useBMS;
    String  session;
    String  bmSuffix;
    String  pmSuffix;
    int     maxIconWidth;
    int     maxIconHeight;
    Boolean useFileFilter;
    Boolean useFileLists;
} ApplicationData, *ApplicationDataPtr;

typedef struct {
    Boolean   useSession;
    int       iconicState;
    Position  x, y;
    Dimension width, height;
} SessionData, *SessionDataPtr;

typedef struct {
    XColor       xcolor;                /* pixel #, red, blue, green, etc. */
    Pixel        mono;                  /* black or white pixel for mono   */
    Pixel        newCell;               /* new color cell for grab-screen  */
} PixelTableItem;

typedef struct {
    int              pixelTableSize;    /* number of allocated table items */
    int              numItems;          /* number of items in the table    */
    int              lastFound;         /* last lookup result, check first */
    PixelTableItem  *item;              /* array of actual table entries   */
} PixelTable;

#define PIXEL_TABLE_MONO(i)      pixelTable.item[i].mono
#define PIXEL_TABLE_NEW_CELL(i)  pixelTable.item[i].newCell
#define PIXEL_TABLE_INC 20          /* size to increase table by each time */

#ifndef NULL
#define NULL   0
#endif

/* Maximum icon demensions */
#define MAX_ICON_WIDTH   256
#define MAX_ICON_HEIGHT  256

/* Maximum icon demensions */
#define MAX_FNAME        256

#define min(a, b)	((a < b) ? a : b)
#define max(a, b)	((a > b) ? a : b)
#ifndef abs
#if !defined(__linux__)
#define abs(a)		(((a) < 0) ? -(a) : (a))
#endif
#endif
#define mag(a,b)	((a-b) < 0 ? (b-a) : (a-b))

#define DARK		0
#define LIGHT		1

/* rotation direction */
#define ROTATE_L        1
#define ROTATE_R        2

/* File I/O flags */
#define READ_FLAGS (XpmColorSymbols)
#define WRITE_FLAGS (XpmSize|XpmHotspot|XpmCharsPerPixel|XpmInfos)

/* State flags for Init_Icons() */
#define DO_NOT_SAVE     0
#define DO_SAVE         1

/* Graphics Ops. tools */
#define POINT           1
#define FLOOD           2
#define LINE            3
#define POLYLINE        4
#define RECTANGLE       5
#define POLYGON         6
#define CIRCLE          7
#define ELLIPSE         8
#define ERASER          9
#define SELECT         10

/* Secondary Graphics Ops. */
#define S_WAIT_RELEASE 99
#define S_PASTE       100
#define S_ROTATE      101
#define S_SCALE_1     102
#define S_SCALE_2     103
#define S_HOTSPOT     104
#define S_GRAB        105

/* Interrupt flags for SELECT Ops. */
#define INITIAL         0
#define CONTINUE        1

#define NUM_GFX_OPS    11	/* number of Graphics Ops. tools + 1*/
#define DEFAULT_MAG     8	/* default magnification factor */
#define GAMMA_CUTOFF  150	/* x-over pt. for black/white fg color */
#define NUM_STATICS    16	/* number of static colors */
#define NUM_DYNAMICS    6	/* number of dynamic colors */
#define NUM_PENS      (NUM_STATICS+NUM_DYNAMICS)

/* max. points for polylines/polygons */
#define MAX_PTS       200

/* dialog state flag values */
#define NONE            0
#define NEW             1
#define OPEN            2
#define SAVE            3
#define SAVE_AS         4
#define GRAB            5
#define DROP            6
#define QUIT           99

/* Transfer_Back_Image() state flag values */
#define HOLLOW          0
#define FILL            1

/* File I/O state flag values */
#define FILE_READ       0
#define FILE_WRITE      1

/* output file format flags */
#define FORMAT_XPM      0
#define FORMAT_XBM      1
#define FORMAT_NONE     2

/* which color block? */
#define STATIC_COLOR    0
#define DYNAMIC_COLOR   1

/* static color flags */
#define COLOR1          0
#define COLOR2          1
#define COLOR3          2
#define COLOR4          3
#define COLOR5          4
#define COLOR6          5
#define COLOR7          6
#define COLOR8          7
#define GREY1           8
#define GREY2           9
#define GREY3          10
#define GREY4          11
#define GREY5          12
#define GREY6          13
#define GREY7          14
#define GREY8          15

/* dynamic color flags */
#define BG_COLOR      100
#define FG_COLOR      101
#define TS_COLOR      102
#define BS_COLOR      103
#define SELECT_COLOR  104
#define TRANS_COLOR   105

/* flip orientation flags */
#define HORIZONTAL      0
#define VERTICAL        1

/* magnification factor flags */
#define MAG_2X    2
#define MAG_3X    3
#define MAG_4X    4
#define MAG_5X    5
#define MAG_6X    6
#define MAG_8X    8
#define MAG_10X  10
#define MAG_12X  12

/* command-line parameter flags */
#define AUTO_FILE        0
#define AUTO_SIZE        1
#define NUM_PARAMS       2
