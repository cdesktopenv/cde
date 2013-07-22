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
/* $XConsortium: DisplayAreaP.h /main/18 1996/08/13 11:35:51 cde-hp $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        DisplayAreaP.h
 **
 **   Project:     Cde Help System
 **
 **   Description: Defines the Display Area structures and defines.
 **
 ****************************************************************************
 ************************************<+>*************************************/
/*
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1987, 1988, 1989, 1990, 1991, 1992,
                 1993, 1994, 1996 Hewlett-Packard Company.
 * (c) Copyright 1993, 1994, 1996 International Business Machines Corp.
 * (c) Copyright 1993, 1994, 1996 Sun Microsystems, Inc.
 * (c) Copyright 1993, 1994, 1996 Novell, Inc. 
 * (c) Copyright 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 */

#ifndef	_DtHelpDisplayAreaP_h
#define	_DtHelpDisplayAreaP_h

#include <X11/X.h>
#include <Xm/XmP.h>
#include <Dt/CanvasP.h>
#include <DtI/GraphicsP.h>

/*
 * Traversal flags
 */
#define	_DT_HELP_SHADOW_TRAVERSAL	(1 << 0)
#define	_DT_HELP_NOT_INITIALIZED	(1 << 1)
#define	_DT_HELP_TRAVERSAL_DRAWN	(1 << 2)
#define	_DT_HELP_DRAW_TOC_IND		(1 << 3)
#define	_DT_HELP_CLEAR_TOC_IND		(1 << 4)
#define	_DT_HELP_TOC_ON			(1 << 5)

/*
 * enum states for selection
 */
enum	_DtHelpSelectState
    {
	_DtHelpNothingDoing,
	_DtHelpCopyOrLink,
	_DtHelpSelectingText
    };

/*
 * Whether the display area has the focus or not
 */
#define _DT_HELP_FOCUS_FLAG           0x04

typedef struct {
	int		 used;
	int              num_pixels;
	Pixmap           pix;
	Pixmap           mask;
	Dimension        width;
	Dimension        height;
	Pixel           *pixels;
} DtHelpGraphicStruct;

typedef	struct {
	_DtCvPointer	 font_ptr;
	int	 spc_idx;
} DtHelpSpecialChars;

typedef	struct	_dtHelpDAFontMetrics {
        _DtCvUnit  ascent;         /* Maximum ascent               */
	_DtCvUnit  descent;        /* Maximum descent              */
	_DtCvUnit  average_width;  /* Average width of a character */
	_DtCvUnit  super;          /* Offset from baseline for super scripts */
	_DtCvUnit  sub;            /* Offset from baseline for sub scripts   */
} _DtHelpDAFontMetrics;

typedef	struct {
	short   inited;
	_DtHelpDAFontMetrics fm;
} DtHelpDAFSMetrics;

typedef	struct	_DtHelpDAfontInfo {
	char		**exact_fonts;	/* the list of fonts specified by the
					   toss element rather than hints.   */
	XrmDatabase	def_font_db;	/* The default font resource db      */
	XrmDatabase	font_idx_db;	/* Which font index goes with which
					   set of font resources	     */
	XFontStruct	**font_structs;	/* The font structures opened        */
	XFontSet	*font_sets;	/* The font sets opened              */
	DtHelpDAFSMetrics *fs_metrics;

	XrmQuark	lang_charset;	/* the char set for current lang     */
	int		*exact_idx;	/* The indexes for the exact fonts   */
	int		max_structs;	/* the max number of font_structs    */
	int		max_sets;	/* The max number of font_sets	     */
	int		struct_cnt;	/* the cur number of font_structs    */
	int		set_cnt;	/* The cur number of font_sets	     */
	long		def_idx;	/* The default index                 */
} DtHelpDAFontInfo;

/* 
 * SelectionScroll structure 
 */
typedef struct {
	int	horizontal_reason;
	int	vertical_reason;
} SelectionScrollStruct;

/*
 * DisplayArea structure
 */
typedef	struct _dtHelpDispAreaStruct {
	Widget	dispWid;		/* The text and graphic area. */
	Widget	vertScrollWid;		/* The vertical scroll bar    */
	Widget	horzScrollWid;		/* The horizontal scroll bar  */
	Boolean vertIsMapped;
	Boolean horzIsMapped;
	short	neededFlags;		/* _DtHelpAS_NEEDED flags        */
	short   nl_to_space;		/* are newlines in multibyte     */
					/* strings turned into spaces?   */

	Dimension	formWidth;	/* Pixel width of the parent  area  */
	Dimension	formHeight;	/* Pixel height of the parent area  */
	Dimension	dispWidth;	/* Pixel width of the display area  */
	Dimension	dispHeight;	/* Pixel height of the display area */
	Dimension	dispUseHeight;	/* Pixel height of the display area
					   minus the decor margin.          */
	Dimension	dispUseWidth;	/* Pixel width of the display area
					   minus the decor margin.          */
	Dimension	marginWidth;	/* Pixel padding at the left and
					   right of the display area.       */
	Dimension	marginHeight;	/* Pixel padding at the top and
					   bottom of the display area.      */

	short	decorThickness;		/* the shadow thickness plus highlight
					   thickness of the display area    */

	void	(*hyperCall)();		/* The hypertext callback */
	void	(*resizeCall)();	/* The resize callback    */
	int	(*exec_filter)();	/* The execution filter callback    */
	XtPointer	clientData;	/* The client's data for the callback */

	Pixel	traversalColor;		/* The client's traversal color */
	Pixel	foregroundColor;	/* The client's foreground color     */
	Pixel	backgroundColor;	/* The client's foreground color     */
	Pixel	searchColor;		/* The client's search hilite color  */
	GC	pixmapGC;
	GC	normalGC;
	GC	invertGC;
	Pixmap  def_pix;		/* the default 'missing pixmap'      */
	Dimension def_pix_width;	/* the width of the default pixmap   */
	Dimension def_pix_height;	/* the height of the default pixmap  */
        _DtGrContext *context;          /* image converter context */

	Colormap  colormap;		/* The colormap to use		*/
	Visual	 *visual;		/* The visual to use		*/

	DtHelpDAFontInfo font_info;	/* The font information		*/

	int	depth;			/* The depth of the window	*/

	int	fontAscent;
	int	lineHeight;
	int	leading;
	long	charWidth;		/* The average size of a character   */
	int	moveThreshold;		/* The number of pixels that must
					   be moved before a copy-paste
					   action occurs.                    */
	int	underLine;
	int	lineThickness;		/* For traversal box and underline   */
	int	firstVisible;		/* The absolute number of the first
					   line visible in the window.      */
	int	nextNonVisible;		/* The absolute number of the first
					   line non visible, next to the last
					   visible window.  */
	int	visibleCount;		/* The number of lines viewable     */
	int	maxYpos;		/* Maximum Y positioning	    */

	int	virtualX;		/* The virtual x of the window      */
	int	maxX;			/* The max virtual x of a line      */

	int	max_spc;		/* The maximum special characters   */
	int	cur_spc;		/* The current unused structure     */
	int	timerX;			/* Used for button clicks/selections */
	int	timerY;			/* Used for button clicks/selections */
	int	scr_timer_x;		/* Used for button clicks/selections */
	int	scr_timer_y;		/* Used for button clicks/selections */
	int     vert_init_scr;          /* The initial vert scrolling timeout*/
	int     vert_rep_scr;           /* The repeat vert scrolling timeout */
	int     horz_init_scr;          /* The initial horz scrolling timeout*/
	int     horz_rep_scr;           /* The repeat horz scrolling timeout */
	_DtCvUnit	toc_width;	/* The width  of the toc indicator   */
	_DtCvUnit	toc_height;	/* The height of the toc indicator   */
	_DtCvUnit	toc_y;		/* The y coordinate of the toc       */
	_DtCvUnit	toc_base;	/* The baseline coordinate of the toc*/

	Time	 anchor_time;		/* Indicates the primary selection
					   time.                              */
	Boolean	 primary;		/* Indicates if this widget has the
					   primary selection                  */
	Boolean	 text_selected;		/* Indicates if the selection has
					   occurred     		      */
	enum _DtHelpSelectState select_state;
					/* Indicates the state of the current
					   selection.                         */
	short	 toc_flag;		/* Indicates if the traversal indicator
					   is always on.		      */
	_DtCvPointer toc_indicator;	/* The indicator used in the toc */

	XtIntervalId scr_timer_id;
	SelectionScrollStruct scr_timer_data;
	DtHelpSpecialChars *spc_chars;	/* Structure containing the spc chars */
	_DtCvHandle	  canvas;
	_DtCvTopicPtr	  lst_topic;
	wchar_t		 *cant_begin_chars;	/* characters that cannot    */
						/* begin a line of text      */
	wchar_t		 *cant_end_chars;	/* characters that cannot    */
						/* end a line of text        */

	short	dtinfo;			/* Indicates if being used by dtinfo */
	Pixmap  stipple;		/* stippled pixmap */

	/* callback to be called whenever you manipulate the display */
	/* area's vertical scrollbar directly using XtSetValues, */
	/* because the application (dtinfo) has callbacks on the */
	/* scrollbar, these do not get called when we adjust it with */
	/* XtSetValues() */ 
	void	(*vScrollNotify)(void *,unsigned int); 

	/* dtinfo requires this for doing link previews */
	void    (*armCallback)(void*); 

	_DtCvValue honor_size;		/* Layout parameter for _DtCvSetTopic */
	_DtCvRenderType render_type; 	/* Render type performed on expose */
	unsigned short media_resolution; 	/* used for scaling images */

} DtHelpDispAreaStruct;

#endif /* _DtHelpDisplayAreaP_h */
