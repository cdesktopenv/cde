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
 *	$XConsortium: ab_utils.h /main/3 1995/11/06 17:14:16 rswiston $
 *
 * @(#)ab_utils.h	1.10 95/03/09 Copyright 1993 Sun Microsystems
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
 * ab_utils.h 
 */
#ifndef _AB_UTILS_H_
#define _AB_UTILS_H_

#include <ab_private/abio.h>

#define InTestMode      AB_builder_mode == MODE_TEST_SHOWN || \
                        AB_builder_mode == MODE_TEST_PROJ
#define InBuildMode     AB_builder_mode == MODE_BUILD

/* To implement dtbuilder window protocol */
#define WindowHidden            0x0000
#define WindowUp                0x0001
#define WindowIconified         0x0002

typedef enum
{
	AB_WIN_LEADER,
        AB_WIN_WINDOW,
        AB_WIN_DIALOG,
        AB_WIN_MODAL,
        AB_WIN_TYPE_NUM_VALUES
} AB_WIN_TYPE;

typedef enum
{
	AB_WPOS_UNSPECIFIED,
	AB_WPOS_TILE_ABOVE,
	AB_WPOS_TILE_BELOW,
	AB_WPOS_TILE_HORIZONTAL,
	AB_WPOS_TILE_LEFT,
	AB_WPOS_TILE_RIGHT,
	AB_WPOS_TILE_VERTICAL,
	AB_WPOS_STACK_DIAGONAL,
	AB_WPOS_STACK_CENTER,
	AB_WPOS_TYPE_NUM_VALUES
} AB_WPOS_TYPE;

typedef enum
{
        AB_STATUS_OBJ_TYPE,
        AB_STATUS_OBJ_NAME,
        AB_STATUS_OBJ_POS,
        AB_STATUS_OBJ_SIZE,
        AB_STATUS_CURS_POS,
        AB_STATUS_CUR_MODULE,
        AB_STATUS_NUM_VALUES /* number of valid values - MUST BE
LAST */
        /* ANSI: no comma after last enum item! */
} AB_STATUS_INFO;

typedef struct AB_WINDOW {
    Widget		widget;
    AB_WIN_TYPE		type;
    unsigned long 	state;
    struct AB_WINDOW	*next;
} ABWindow;

extern void	ab_register_window(
		    Widget		widget,
		    AB_WIN_TYPE		type,
		    unsigned long 	init_state,
		    Widget		init_pos_ref_win,
		    AB_WPOS_TYPE	init_pos_type,
		    XtCallbackProc	close_callback,
		    XtPointer		close_clientdata
		);
extern void	ab_show_window(
		    Widget		widget
		);
extern void     ab_takedown_windows(void);

extern void     ab_putback_windows(void);

extern BOOL	ab_window_leader_iconified(void);

extern BOOL	ab_window_is_open(
		    Widget		widget
		);

extern void	ab_position_window(
		    Widget		widget,
		    Widget		ref_widget,
		    AB_WPOS_TYPE	pos_type
		);

extern BOOL	ab_is_cur_dir(
		    STRING dir
		);

extern int	ab_change_dir(
		    STRING new_dir
		);

extern STRING	ab_get_cur_dir(void);

extern int	ab_cvt_image_file_to_pixmap(
			Widget	widget,
			STRING	file_name,
			Pixmap	*pixmap_out
		);

extern int	ab_post_instantiate(
			ABObj	obj
		);

extern int	ab_update_stat_region(
    		    AB_STATUS_INFO type,
    		    String         valuestring
		);

extern void	ab_set_busy_cursor(
		    BOOL	on
		);

extern void	ab_palette_set_active(
                    BOOL        active
		);

extern void	ab_exit_dtbuilder(void);

#endif /* _AB_UTILS_H_ */
