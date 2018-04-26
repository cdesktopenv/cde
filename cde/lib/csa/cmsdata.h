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
/* $XConsortium: cmsdata.h /main/1 1996/04/21 19:22:11 drk $ */
/*
 *  (c) Copyright 1993, 1994 Hewlett-Packard Company
 *  (c) Copyright 1993, 1994 International Business Machines Corp.
 *  (c) Copyright 1993, 1994 Novell, Inc.
 *  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 */

#ifndef _CMSDATA_H
#define _CMSDATA_H

#include <EUSCompat.h>
#include "ansi_c.h"
#include "nametbl.h"
#include "cm.h"

/* standard calendar attr name and entry attr name table */
extern _DtCmNameTable *_DtCm_cal_name_tbl;
extern _DtCmNameTable *_DtCm_entry_name_tbl;

extern void _DtCm_init_hash P(());

extern cms_entry *_DtCm_make_cms_entry P((_DtCmNameTable *tbl));

extern CSA_return_code _DtCm_copy_cms_entry P((
				cms_entry *e,
				cms_entry **e_r));

extern void _DtCm_free_cms_entry P((cms_entry *entry));

extern void _DtCm_free_cms_entries P((cms_entry *entry));

extern CSA_return_code _DtCmGrowAttrArray P((
			uint		*num_attr,
			cms_attribute	**attrs,
			cms_attribute	*attr));

#endif
