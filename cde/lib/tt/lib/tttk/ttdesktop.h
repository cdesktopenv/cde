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
/*%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			 */
/*%%  (c) Copyright 1993, 1994 International Business Machines Corp.	 */
/*%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			 */
/*%%  (c) Copyright 1993, 1994 Novell, Inc. 				 */
/*%%  $XConsortium: ttdesktop.h /main/3 1995/10/23 10:31:59 rswiston $ 			 				 */
/*
 * @(#)ttdesktop.h	1.3 93/09/07
 *
 * Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 */
#ifndef ttdesktop_h
#define ttdesktop_h

#include "tttk/tttk.h"
#include "tttk/tttkmessage.h"

typedef struct {
	int	    repliesOutStanding;
	int         width;
	int         height;
	int         xOffset;
	int         yOffset;
	char       *display;
} DisplayInfo;

Tt_pattern		_ttDesktopPatternFinish(
				Tt_pattern	pat,
				Tt_message	commission,
				int		register_it
			);
Tt_message		_ttDesktopMessageFinish(
				Tt_message		msg,
				Tt_message		commission,
				int			send
			);
Widget			_ttdt_realized_widget(
				void	       *widget,
				int		mappedWhenManaged_Shell = 1
			);
Tt_message		_ttdt_posix_cb(
				Tt_message	msg,
				Tt_pattern	pat,
				void	       *clientCB,
				void	       *clientData
			);
Tt_message		_ttdt_contract_cb(
				Tt_message	msg,
				Tt_pattern	pat,
				void	       *clientCB,
				void	       *clientData
			);
Tt_pattern		_ttdt_pat(
				Tttk_op			op,
				_TtDtMessageCB		internalCB,
				Tt_category		category,
				Tt_message		commission,
				void		       *clientCB,
				void		       *clientdata,
				int			register_it
			);

Tt_message		_ttdt_do_GSet_Geometry(
				Tt_message	msg,
				void	       *widget,
				Tt_message	,
				int	       *width,
				int	       *height,
				int	       *xOffset,
				int	       *yOffset
			);
Tt_message		_ttdt_do_wm_state(
				Tt_message	msg,
				void	       *widget,
				Tt_message	,
				int	       *iconified_or_mapped
			);
Tt_message		_ttdt_do_Set_XInfo(
				Tt_message	msg,
				void	       *widget,
				Tt_message	,
				char           *display,
				int		visual,
				int		depth
			);
Tt_message		_ttdt_do_Get_XInfo(
				Tt_message	msg,
				void	       *widget,
				Tt_message	,
				char          **display,
				int	       *visual,
				int	       *depth
			);
Tt_message		_ttdt_do_RaiseLower(
				Tt_message	msg,
				void	       *widget,
				Tt_message
			);
/*
 **********************************************************************
 *
 * Desktop: POSIX
 *
 **********************************************************************
 */
Tt_message		ttdt_Started(
				Tt_message	context,
				const char     *toolName,
				const char     *vendor,
				const char     *version,
				int		send_and_destroy
			);
Tt_message		ttdt_Stopped(
				Tt_message	context,
				const char     *toolName,
				const char     *vendor,
				const char     *version,
				int		send_and_destroy
			);
Tt_message		ttdt_Status(
				Tt_message	context,
				Tt_message	commission,
				const char     *status,
				const char     *toolName,
				const char     *vendor,
				const char     *version,
				int		send_and_destroy
			);

/*
 * Get_Locale
 */
typedef Tt_message	(*Ttdt_Get_Locale_msg_cb)(
				Tt_message	msg,
				void	       *clientdata,
				const char    **categories,
				const char    **locales
			);
Tt_message		ttdt_Get_Locale(
				const char     *handler,
				Tt_message	commission,
				Ttdt_Get_Locale_msg_cb	callback,
				void	       *clientdata,
				const char    **categories,
				int             send
			);
/*
 * Get_Situation
 */
typedef Tt_message	(*Ttdt_Get_Situation_msg_cb)(
				Tt_message	msg,
				void	       *clientdata,
				char           *cwd
			);
Tt_message		ttdt_Get_Situation(
				const char     *handler,
				Tt_message	commission,
				Ttdt_Get_Situation_msg_cb	callback,
				void	       *clientdata,
				int             send
			);
/*
 * Quit
 */
typedef Tt_message	(*Ttdt_Quit_cb)(
				Tt_message		msg,
				void		       *clientdata,
				Tt_message		commission,
				int			silent,
				int			force
			);
Tt_pattern		ttdt_Quit_pat(
				Tt_category		category,
				Tt_message		commission,
				Ttdt_Quit_cb		callback,
				void		       *clientdata,
				int			register_it
			);
/*
 * Pause
 */
Tt_pattern		ttdt_Pause_pat(
				Tt_category		category,
				Tt_message		commission,
				Ttdt_contract_cb	callback,
				void		       *clientdata,
				int			register_it
			);
/*
 **********************************************************************
 *
 * Desktop: X11
 *
 **********************************************************************
 */
/*
 * Get_Geometry
 */
typedef Tt_message	(*Ttdt_Geometry_in_cb)(
				Tt_message		msg,
				void		       *clientdata,
				Tt_message		commission,
				int			width,
				int			height,
				int			xoffset,
				int			yoffset
			);
typedef Tt_message	(*Ttdt_Geometry_out_cb)(
				Tt_message		msg,
				void		       *clientdata,
				Tt_message		commission,
				int		       *width,
				int		       *height,
				int		       *xoffset,
				int		       *yoffset
			);
Tt_message		ttdt_Get_Geometry(
				const char	       *handler,
				Tt_message		commission,
				Ttdt_Geometry_in_cb	callback,
				void		       *clientdata,
				int			send
			);
Tt_pattern		ttdt_Get_Geometry_pat(
				Tt_category		category,
				Tt_message		commission,
				Ttdt_Geometry_out_cb	callback,
				void		       *clientdata,
				int			register_it
			);
/*
 * Set_Geometry
 */
Tt_message		ttdt_Set_Geometry(
				const char	       *handler,
				Tt_message		commission,
				Tt_class		req_or_edict,
				Ttdt_Geometry_in_cb	callback,
				void		       *clientdata,
				int			width,
				int			height,
				int			xoffset,
				int			yoffset,
				int			send
			);
Tt_pattern		ttdt_Set_Geometry_pat(
				Tt_category		category,
				Tt_message		commission,
				Ttdt_Geometry_out_cb	callback,
				void		       *clientdata,
				int			register_it
			);
/*
 * Get_Iconified
 */
typedef Tt_message	(*Ttdt_Iconified_in_cb)(
				Tt_message		msg,
				void		       *clientdata,
				Tt_message		commission,
				int			iconified
			);
typedef Tt_message	(*Ttdt_Iconified_out_cb)(
				Tt_message		msg,
				void		       *clientdata,
				Tt_message		commission,
				int		       *iconified
			);
Tt_message		ttdt_Get_Iconified(
				const char	       *handler,
				Tt_message		commission,
				Ttdt_Iconified_in_cb	callback,
				void		       *clientdata,
				int			send
			);
Tt_pattern		ttdt_Get_Iconified_pat(
				Tt_category		category,
				Tt_message		commission,
				Ttdt_Iconified_out_cb	callback,
				void		       *clientdata,
				int			register_it
			);
/*
 * Set_Iconified
 */
Tt_message		ttdt_Set_Iconified(
				const char	       *handler,
				Tt_message		commission,
				Tt_class		req_or_edict,
				Ttdt_Iconified_in_cb	callback,
				void		       *clientdata,
				int			iconified,
				int			send
			);
Tt_pattern		ttdt_Set_Iconified_pat(
				Tt_category		category,
				Tt_message		commission,
				Ttdt_Iconified_out_cb	callback,
				void		       *clientdata,
				int			register_it
			);
/*
 * Get_XInfo
 */
typedef Tt_message	(*Ttdt_XInfo_in_cb)(
				Tt_message		msg,
				void		       *clientdata,
				Tt_message		commission,
				char	               *display,
				int			visual,
				int			depth
			);
typedef Tt_message	(*Ttdt_XInfo_out_cb)(
				Tt_message		msg,
				void		       *clientdata,
				Tt_message		commission,
				char	              **display,
				int		       *visual,
				int		       *depth
			);
Tt_message		ttdt_Get_XInfo(
				const char	       *handler,
				Tt_message		commission,
				Ttdt_XInfo_in_cb	callback,
				void		       *clientdata,
				int			send
			);
Tt_pattern		ttdt_Get_XInfo_pat(
				Tt_category		category,
				Tt_message		commission,
				Ttdt_XInfo_out_cb	callback,
				void		       *clientdata,
				int			register_it
			);
/*
 * Set_XInfo
 */
Tt_message		ttdt_Set_XInfo(
				const char	       *handler,
				Tt_message		commission,
				Tt_class		req_or_edict,
				Ttdt_XInfo_in_cb	callback,
				void	               *clientdata,
				const char             *display,
				int			visual,
				int			depth,
				int			send
			);
Tt_pattern		ttdt_Set_XInfo_pat(
				Tt_category		category,
				Tt_message		commission,
				Ttdt_XInfo_in_cb	callback,
				void		       *clientdata,
				int			register_it
			);
/*
 * Raise
 */
Tt_message		ttdt_Raise(
				const char	       *handler,
				Tt_message		commission,
				Tt_class		req_or_edict,
				Ttdt_contract_cb	callback,
				void		       *clientdata,
				int			send
			);
Tt_pattern		ttdt_Raise_pat(
				Tt_category		category,
				Tt_message		commission,
				Ttdt_contract_cb	callback,
				void		       *clientdata,
				int			register_it
			);
#endif
