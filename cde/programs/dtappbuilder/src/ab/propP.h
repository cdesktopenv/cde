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
 *	$XConsortium: propP.h /main/3 1995/11/06 17:48:54 rswiston $
 *
 * @(#)propP.h	1.7 95/04/27 Copyright 1993 Sun Microsystems
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
 * propP.h - 	Module-private defines structures/routines required for 
 *		property sheets
 */
#ifndef _PROPP_H_
#define _PROPP_H_

#include <X11/Intrinsic.h>
#if !defined(CSRG_BASED)
#include <values.h>
#endif
#include <ab_private/obj.h>
#include <ab_private/pal.h>
#include <ab_private/abobj_edit.h>

/*
 * Enum values for Prop Sheet Mode
 */
typedef enum
{
	PROP_LOAD,
	PROP_EDIT,
        PROP_MODE_NUM_VALUES
} PROP_MODE;



/*
 * Property Dialog Manipulation Routines
 */
extern void     propP_changebar_init(
                    Widget      changebar,
                    Widget      setting
                );

extern void	propP_combobox_chgCB(
                    Widget      field,
                    XtPointer   clientdata,
                    XtPointer   calldata
                );

extern void     propP_field_chgCB(
                    Widget      field,
                    XtPointer   clientdata,
                    XtPointer   calldata
                );
extern int	propP_field_set_value(
		    Widget		field,
		    STRING		value,
		    BOOL		trip_changebar
		);
extern int	propP_field_set_numeric_value(
		    Widget		field,
		    int			value,
		    BOOL		trip_changebar
		);

extern void	propP_labeltypeCB(
                    Widget      item,
                    XtPointer   clientdata,
                    XtPointer   calldata
                );
extern void     propP_options_itemCB(
                    Widget      item,
                    XtPointer   clientdata,
                    XtPointer   calldata
                );
extern  void    propP_popup_message(
                    Widget      w,
                    STRING      msg,
		    BOOL	modal
                );
extern void     propP_radiobox_itemCB(
                    Widget      item,
                    XtPointer   clientdata,
                    XtPointer   calldata
                );
extern void	propP_setting_chgCB(
                    Widget    	item,
                    XtPointer 	clientdata,
                    XtPointer 	calldata
                );

#endif /* _PROPP_H_ */
