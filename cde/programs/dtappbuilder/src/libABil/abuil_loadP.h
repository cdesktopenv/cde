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
 *	$XConsortium: abuil_loadP.h /main/4 1995/11/06 18:20:38 rswiston $
 *
 * @(#)abuil_loadP.h	1.13 21 Feb 1994	cose/unity1/cde_app_builder/src/libABil
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
#ifndef _ABIL_LOAD_UIL_P_H
#define _ABIL_LOAD_UIL_P_H

/*
 * This file defines private macros and datatypes used by the
 * uil file load component.
 */
#include <stdio.h>
#include <Xm/Xm.h>

#include <ab_private/UilDef.h>
#include <uil/UilDBDef.h>
#include <uil/UilSymGl.h>	/* For databases */

#include <ab_private/abuil_load.h>

#include <ab_private/trav.h>

#define MAX_LEN         255
#define CLASS_SUFFIX	"WidgetClass"
#define APP_SHELL_CLASS	"applicationShellWidgetClass"

#define BMASK(TYPE)	(1L << ((unsigned int) (TYPE)))


typedef ABObj	(*ObjModifyProc)(char *classname, ABObj, ABObj);

typedef struct _WidgetABObjMapRec
{
    char		*widget_name;
    AB_OBJECT_TYPE	obj_type;
    unsigned int	sub_type;
    ObjModifyProc	object_proc;
    ObjModifyProc	child_proc;
} WidgetABObjMap;

typedef struct _RefResolveRec
{
    ABObj		obj;
    AB_COMPASS_POINT	dir;
} RefResolve;

extern WidgetABObjMap	*abuilP_entry_for_uil_widget(
			    sym_widget_entry_type *uil_widget
			);
extern BOOL		abuilP_store_attr_in_abobj(
			    ABObj	obj,
			    STRING	res_name,
			    AB_ARG_TYPE	res_type,
			    XtPointer	res_value
			);
extern void		abuil_add_unresolved_ref(
			    void	*entry
			);
extern void		abuilP_init_context_attrs(
			    void
			);
extern void		abuilP_store_context_attrs(
			    ABObj	ab_widget
			);

#endif	/* _ABIL_LOAD_UIL_P_H */
