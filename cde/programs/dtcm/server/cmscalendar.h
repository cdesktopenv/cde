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
/* $XConsortium: cmscalendar.h /main/4 1995/11/09 12:40:57 rswiston $ */
/*
 *  (c) Copyright 1993, 1994 Hewlett-Packard Company
 *  (c) Copyright 1993, 1994 International Business Machines Corp.
 *  (c) Copyright 1993, 1994 Novell, Inc.
 *  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 */

#ifndef _CMSCALENDAR_H
#define _CMSCALENDAR_H

#include "ansi_c.h"
#include "csa.h"
#include "cm.h"
#include "nametbl.h"
#include "tree.h"
#include "list.h"
#include "data.h"
#include "reminder.h"
#include "rtable4.h"
#include "callback.h"
#include "log.h"

#define	GET_R_ACCESS(cal)		((Access_Entry_4 *) (cal->r_access))
#define	SET_R_ACCESS(cal,v)		(cal)->r_access = (caddr_t) v
#define	GET_W_ACCESS(cal)		((Access_Entry_4 *) (cal->w_access))
#define	SET_W_ACCESS(cal,v)		(cal)->w_access = (caddr_t) v
#define	GET_D_ACCESS(cal)		((Access_Entry_4 *) (cal->d_access))
#define	SET_D_ACCESS(cal,v)		(cal)->d_access = (caddr_t) v
#define	GET_X_ACCESS(cal)		((Access_Entry_4 *) (cal->x_access))
#define	SET_X_ACCESS(cal,v)		(cal)->x_access = (caddr_t) v
#define	APPT_TREE(info)			((Rb_tree *) ((info)->tree))
#define	REPT_LIST(info)			((Hc_list *) ((info)->list))
#define	APPT_KEY(p_appt)		((Appt_4 *) (p_appt))->appt_id.key
#define	APPT_TICK(p_appt)		((Appt_4 *) (p_appt))->appt_id.tick

typedef CSA_return_code (*_DtCmGetAttrFunc)();

typedef	struct __DtCmsCalendar {
	char		*owner;
	char		*calendar;
	_DtCmNameTable	*cal_tbl;
	_DtCmNameTable	*entry_tbl;
	int		*types;		/* type associated with entry attrs */
	int		num_entry_attrs; /* number of entry attrs associated
					  * with this calendar */
	boolean_t	hashed;		/* true if file converted to hashed
					 * format */
	int		fversion;
	long		lastkey;
	boolean_t	modified;	/* if true, do garbage collection */
	Rb_tree		*tree;		/* for single appointments */
	Hc_list		*list;		/* for repeating appointments */
	Rm_que		*rm_queue;	/* active reminder queue, version 1 */
	_DtCmsRemQueue	*remq;		/* reminder queue, version 4 */
	caddr_t		r_access;	/* read access, version 1 */
	caddr_t		w_access;	/* write access, version 1 */
	caddr_t		d_access;	/* delete access, version 1 */
	caddr_t 	x_access;	/* exec access, version 1 */
	Access_Entry_4	*alist;		/* combined v1 access list */
	uint		num_attrs;	/* number of calendar attrs,version 4 */
	cms_attribute 	*attrs;		/* calendar attributes, version 4 */
	_DtCmGetAttrFunc *getattrfuncs;	/* array of function ptr to get attrs */
	_DtCmsRegistrationInfo *rlist;	/* client registration list */
	boolean_t	*checkowner;
	struct __DtCmsCalendar *next;
} _DtCmsCalendar;


extern _DtCmsCalendar * _DtCmsMakeCalendar P((
				char *owner,
				char *name));

extern void _DtCmsPutInCalList P((_DtCmsCalendar *cal));

extern void _DtCmsFreeCalendar P((_DtCmsCalendar *cal));

extern CSA_return_code _DtCmsSetFileVersion P((
				_DtCmsCalendar *cal,
				int version));

extern CSA_return_code _DtCmsLoadCalendar P((
				char *target,
				_DtCmsCalendar **cal));

extern char * _DtCmsGetCalendarOwner P((char *target));

extern CSA_return_code _DtCmsGetCalendarByName P((
				char *target,
				boolean_t load,
				_DtCmsCalendar **cal));

extern CSA_return_code _DtCmsInsertEntry4Parser P((
				_DtCmsCalendar *cal,
				cms_entry *entry));

extern void _DtCmsSetAccess4Parser P((
				_DtCmsCalendar *cal,
				Access_Entry_4 *list,
				int type));

extern void _DtCmsSetCalendarAttrs4Parser P((
				_DtCmsCalendar *cal,
				int len,
				cms_attribute *attrs));

extern void _DtCmsGenerateKey P((_DtCmsCalendar *cal, long *key));

extern CSA_return_code _DtCmsEnumerateUp P((
				_DtCmsCalendar *cal,
				_DtCmsEnumerateProc doit));

extern void _DtCmsEnumerateDown P((
				_DtCmsCalendar *cal,
				_DtCmsEnumerateProc doit));

extern CSA_return_code _DtCmsRbToCsaStat P((Rb_Status rb_stat));

extern CSA_return_code _DtCmsGetCalAttrsByName P((
				_DtCmsCalendar	*cal,
				uint		num_names,
				cms_attr_name	*names,
				uint		*num_attrs_r,
				cms_attribute	**attrs_r));

extern CSA_return_code _DtCmsGetAllCalAttrs P((
				_DtCmsCalendar	*cal,
				u_int		*num_attrs_r,
				cms_attribute	**attrs_r,
				boolean_t	returnall));

extern CSA_return_code _DtCmsGetCalAttrNames P((
				_DtCmsCalendar	*cal,
				uint		*num_names_r,
				cms_attr_name	**names_r));

extern void _DtCmsFreeCmsAttrNames P((uint num, cms_attr_name *names));

extern CSA_return_code _DtCmsUpdateCalAttributesAndLog P((
				_DtCmsCalendar	*cal,
				uint		numsrc,
				cms_attribute	*srcattrs,
				uint		access));

extern CSA_return_code _DtCmsV5TransactLog P((
				_DtCmsCalendar	*cal,
				cms_entry	*e,
				_DtCmsLogOps	op));

#endif
