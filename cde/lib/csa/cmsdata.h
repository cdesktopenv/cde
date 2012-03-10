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
