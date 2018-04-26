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
/* $XConsortium: updateattrs.h /main/1 1996/04/21 19:24:55 drk $ */
/*
 *  (c) Copyright 1993, 1994 Hewlett-Packard Company
 *  (c) Copyright 1993, 1994 International Business Machines Corp.
 *  (c) Copyright 1993, 1994 Novell, Inc.
 *  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 */

#ifndef _UPDATEATTRS_H
#define _UPDATEATTRS_H

#include <EUSCompat.h>
#include "ansi_c.h"
#include "csa.h"
#include "cm.h"
#include "nametbl.h"

extern CSA_return_code _DtCmUpdateAttributes P((
			uint		numsrc,
			cms_attribute	*srcattrs,
			uint		*numdst,
			cms_attribute	**dstattrs,
			_DtCmNameTable	**tbl,
			boolean_t	caltbl,
			int		**types,
			boolean_t	makecopy));

extern CSA_return_code _DtCmUpdateAttribute P((
			cms_attribute	*from,
			cms_attribute	*to));

extern CSA_return_code _DtCmUpdateAccessListAttrVal P((
			cms_attribute_value *newval,
			cms_attribute_value **attrval));

extern CSA_return_code _DtCmUpdateSint32AttrVal P((
				cms_attribute_value *newval,
				cms_attribute_value **attrval));

extern CSA_return_code _DtCmUpdateStringAttrVal P((
				cms_attribute_value *newval,
				cms_attribute_value **attrval));

extern CSA_return_code _DtCmUpdateReminderAttrVal P((
				cms_attribute_value *newval,
				cms_attribute_value **attrval));

extern CSA_return_code _DtCmUpdateDateTimeListAttrVal P((
				cms_attribute_value *newval,
				cms_attribute_value **attrval));

extern CSA_return_code _DtCmUpdateOpaqueDataAttrVal P((
				cms_attribute_value *newval,
				cms_attribute_value **attrval));

#endif
