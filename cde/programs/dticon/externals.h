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
/* $XConsortium: externals.h /main/3 1995/11/02 14:04:25 rswiston $ */
/*********************************************************************
*  (c) Copyright 1993, 1994 Hewlett-Packard Company	
*  (c) Copyright 1993, 1994 International Business Machines Corp.
*  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
*  (c) Copyright 1993, 1994 Unix System Labs, Inc., a subsidiary of
*      Novell, Inc.
**********************************************************************/
#include <Xm/Xm.h>
#include <Dt/xpm.h>
#include "constants.h"

extern char    *black_string;
extern char    *white_string;
extern char    *none_string;

extern HelpStruct   *help_head;
extern int           query_state, error_state;

extern Display   *dpy;
extern Window     root;
extern Window     tablet_win;
extern Widget     tablet_wid;
extern int        screen;
extern Screen    *screen_ptr;
extern Colormap   Cmap;
extern Cursor     cursor;
extern GC         Color_gc;
extern GC         Mono_gc;
extern GC         Flicker_gc;
extern GC         Grid_gc;
extern GC         Erase_gc;
extern int        cmap_size;
extern int        MagFactor;
extern int        GraphicsOp;
extern int        Backup_G_Op;
extern int        CurrentColor;
extern int        ColorBlock;
extern int        tx, ty, last_tx, last_ty;
extern int        ix, iy, last_ix, last_iy;
extern int        ClickCount;
extern int        multiClickTime;
extern int        format;
extern int        fileIOMode;
extern int        fileFormat;
extern char       last_fname[MAX_FNAME];
extern int        pointCount;
extern XPoint     pointList[MAX_PTS];
extern XRectangle current_box;
extern XRectangle select_box;
extern XImage    *CutCopy;
extern XImage    *CutCopy_mono;
extern XImage    *Rotate;
extern XImage    *Rotate_mono;
extern XImage    *Scale;
extern XImage    *Scale_mono;
extern int        icon_width, icon_height;
extern int        backup_width, backup_height;
extern Pixmap     color_icon, prev_color_icon;
extern Pixmap     mono_icon, prev_mono_icon;
extern Boolean    status, Dirty;
extern int        X_Hot, Y_Hot;
extern Boolean    hotSpot;
extern Boolean    FirstRigid, GridEnabled, FloodToBorder, Selected, Anchored;
extern Pixel      black_pixel, white_pixel;
extern Pixel      Foreground, Background, TopShadow, BottomShadow;
extern Pixel      Select, Transparent;
extern Pixel      StaticPen[NUM_STATICS], DynamicPen[NUM_DYNAMICS];
extern Pixel      StaticMono[NUM_STATICS], DynamicMono[NUM_DYNAMICS];
extern Widget     StaticWid[NUM_STATICS], DynamicWid[NUM_DYNAMICS];
extern Widget     GraphicOpsWid[NUM_GFX_OPS];
extern Boolean    DialogFlag;
extern Boolean    FillSolids;
extern Boolean    UndoFlag;
extern Boolean    param_flag[NUM_PARAMS];
extern Boolean    argsNeedProcessed;
extern XpmAttributes xpm_ReadAttribs, xpm_WriteAttribs;
/* extern int     numSymbols; */
extern XpmColorSymbol *colorSymbols;
extern char      *hints_cmt;
extern char      *colors_cmt;
extern char      *pixels_cmt;
extern char       dynamic_c_str[NUM_DYNAMICS][30];
extern char      *color_table[NUM_STATICS+NUM_DYNAMICS][6];
extern PixelTable pixelTable;

/* globals related to the user interface (motif, etc.) */
extern Widget             TopLevel;
extern char              *execName;
extern char              *progName;
extern XtAppContext       AppContext;
extern Widget             mainWindow,
			  dtIconShell,
			  queryDialog, 
                          stdErrDialog, 
                          newIconDialog,
                          fileIODialog;
extern Widget             iconImage, 
                          monoImage;
extern ApplicationData    xrdb;
extern SessionData        session;



#ifdef DEBUG
extern int           debug;
extern int           debug_status;
extern XImage       *debug_image, *debug_shape;
/* extern unsigned char XErrorFlag; */
#endif
