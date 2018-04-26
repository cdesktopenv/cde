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
/* $XConsortium: attr.h /main/1 1996/04/21 19:21:44 drk $ */
/*
 *  (c) Copyright 1993, 1994 Hewlett-Packard Company
 *  (c) Copyright 1993, 1994 International Business Machines Corp.
 *  (c) Copyright 1993, 1994 Novell, Inc.
 *  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 */

#ifndef _ATTR_H
#define _ATTR_H

#include <EUSCompat.h>
#include "ansi_c.h"
#include "csa.h"
#include "cm.h"

/*
 * types of reminder used by old versions
 */

#define _DtCM_OLD_ATTR_BEEP_REMINDER		"bp"
#define _DtCM_OLD_ATTR_FLASH_REMINDER		"fl"
#define _DtCM_OLD_ATTR_MAIL_REMINDER		"ml"
#define _DtCM_OLD_ATTR_POPUP_REMINDER		"op"

/*
 * number of attributes for old and new format calendars
 */
#define _DtCM_OLD_CAL_ATTR_SIZE			9
#define _DtCM_OLD_ENTRY_ATTR_SIZE		19
#define _DtCM_DEFINED_CAL_ATTR_SIZE		15
#define _DtCM_DEFINED_ENTRY_ATTR_SIZE		34

/*
 * "repeat forever" value used in old format calendars
 */
#define _DtCM_OLD_REPEAT_FOREVER		999999999

/*
 * Attribute number supported by old backends
 */

typedef enum {
	_DtCm_old_attr_unknown,
	_DtCm_old_attr_id,
	_DtCm_old_attr_time,
	_DtCm_old_attr_type,
	_DtCm_old_attr_type2,
	_DtCm_old_attr_duration,
	_DtCm_old_attr_what,
	_DtCm_old_attr_author,
	_DtCm_old_attr_beep_reminder,
	_DtCm_old_attr_flash_reminder,
	_DtCm_old_attr_mail_reminder,
	_DtCm_old_attr_popup_reminder,
	_DtCm_old_attr_repeat_type,
	_DtCm_old_attr_repeat_times,
	_DtCm_old_attr_showtime,
	_DtCm_old_attr_status,
	_DtCm_old_attr_privacy,
	_DtCm_old_attr_repeat_nth_interval,
	_DtCm_old_attr_repeat_nth_weeknum,
	_DtCm_old_attr_end_date
} _DtCm_old_attrs;

/* attribute information structure */
typedef struct {
	int		index;
	CSA_enum	type;
	int             fst_vers;       /* 1st data version supporting this
					 * attr
					 */
	_DtCm_old_attrs	oldattr;
	boolean_t	nex_ro;		/* readonly for non-extensible file 
					 * version
					 */
	boolean_t	ex_ro;		/* readonly for extensible file version
					 */
} _DtCmAttrInfo;

/* calendar attribute information */
extern _DtCmAttrInfo _CSA_cal_attr_info[];

/* entry attribute information */
extern _DtCmAttrInfo _CSA_entry_attr_info[];

/*
 * external function declarations
 */

extern CSA_return_code _DtCm_check_cal_csa_attributes P((
				int		fvers,
				uint		num_attrs,
				CSA_attribute	*attrs,
				char		*cname,
				boolean_t	checkreadonly,
				boolean_t	firsttime,
				boolean_t	checkattrnum));

extern CSA_return_code _DtCm_check_cal_cms_attributes P((
				int		fvers,
				uint		num_attrs,
				cms_attribute	*attrs,
				char		*owner,
				char		*cname,
				boolean_t	checkreadonly,
				boolean_t	firsttime,
				boolean_t	checkattrnum));

extern CSA_return_code	_DtCm_check_entry_attributes P((
				int		fversion,
				uint		size,
				CSA_attribute	*attrs,
				CSA_flags	utype,
				boolean_t	checkattrnum));

extern CSA_return_code _DtCm_check_entry_cms_attributes P((
				int		fvers,
				uint		num_attrs,
				cms_attribute	*attrs,
				CSA_flags	utype,
				boolean_t	checkattrnum));

extern CSA_return_code	_DtCm_copy_cms_attributes P((
				uint srcsize,
				cms_attribute *srcattrs,
				uint *dstsize,
				cms_attribute **dstattrs));

extern CSA_return_code	_DtCm_copy_cms_attribute P((
				cms_attribute	*to,
				cms_attribute	*from,
				boolean_t copyname));

extern CSA_return_code	_DtCm_copy_cms_attr_val P((
				cms_attribute_value *from,
				cms_attribute_value **to));

extern cms_access_entry *_DtCm_copy_cms_access_list P((
				cms_access_entry *alist));

extern CSA_date_time_list _DtCm_copy_date_time_list P((
				CSA_date_time_list dlist));

extern CSA_return_code _DtCm_cms2csa_attribute P((
				cms_attribute from,
				CSA_attribute *to));

extern CSA_return_code _DtCm_cms2csa_attrval P((
				cms_attribute_value *from,
				CSA_attribute_value **to));

extern CSA_return_code _DtCm_cms2csa_access_list P((
				cms_access_entry *cmslist,
				CSA_access_rights **csalist));

extern CSA_return_code _DtCm_csa2cms_access_list P((
				CSA_access_rights *csalist,
				cms_access_entry **cmslist));

extern CSA_return_code _DtCm_copy_reminder P((
				CSA_reminder *from,
				CSA_reminder **to));

extern CSA_return_code _DtCm_copy_opaque_data P((
				CSA_opaque_data *from,
				CSA_opaque_data **to));

extern void		_DtCm_free_csa_access_list P((CSA_access_list alist));

extern void 		_DtCm_free_cms_attributes P((
				uint size,
				cms_attribute *attrs));

extern void 		_DtCm_free_attributes	P((
				uint size,
				CSA_attribute * attrs));

extern void		_DtCm_free_cms_attribute_values P((
				uint size,
				cms_attribute *attrs));

extern void		_DtCm_free_attribute_values P((
				uint size,
				CSA_attribute *attrs));

extern void		_DtCm_free_cms_attribute_value P((
				cms_attribute_value *val));

extern void		_DtCm_free_attribute_value P((
				CSA_attribute_value *val));

extern void		_DtCm_free_cms_access_entry P((cms_access_entry *list));

extern void		_DtCm_free_date_time_list P((CSA_date_time_list list));

extern void		_DtCm_free_reminder P((CSA_reminder *val));

extern void		_DtCm_free_opaque_data P((CSA_opaque_data *val));

extern char 		*_DtCm_old_reminder_name_to_name P((
				char *oldname));

extern int		_DtCm_old_reminder_name_to_index P((
				char *name));

extern CSA_return_code	_DtCm_get_old_attr_by_name P((
				char *name,
				_DtCm_old_attrs *attr));

extern CSA_return_code	_DtCm_get_old_attr_by_index P((
				int index,
				_DtCm_old_attrs *attr));

extern CSA_return_code	_DtCm_set_uint32_attrval P((
				uint numval,
				cms_attribute_value **val));

extern CSA_return_code	_DtCm_set_sint32_attrval P((
				int numval,
				cms_attribute_value **val));

extern CSA_return_code	_DtCm_set_string_attrval P((
				char *strval,
				cms_attribute_value **val,
				CSA_enum type));

extern CSA_return_code	_DtCm_set_user_attrval P((
				char *user,
				cms_attribute_value **val));

extern CSA_return_code	_DtCm_set_reminder_attrval P((
				CSA_reminder *remval,
				cms_attribute_value **val));

extern CSA_return_code	_DtCm_set_access_attrval P((
				cms_access_entry *aval,
				cms_attribute_value **val));

extern CSA_return_code	_DtCm_set_opaque_attrval P((
				CSA_opaque_data *opqval,
				cms_attribute_value **val));

extern CSA_return_code	_DtCm_set_csa_access_attrval P((
				cms_access_entry *aval,
				CSA_attribute_value **val));

extern CSA_return_code _DtCm_set_csa_uint32_attrval P((
				uint numval,
				CSA_attribute_value **attrval));

extern CSA_return_code _DtCm_set_csa_string_attrval P((
				char *strval,
				CSA_attribute_value **attrval,
				CSA_enum type));

extern void _DtCm_get_attribute_types(uint size, int *types);

#endif
