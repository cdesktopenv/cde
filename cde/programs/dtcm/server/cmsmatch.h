/* $XConsortium: cmsmatch.h /main/4 1995/11/09 12:42:42 rswiston $ */
/*
 *  (c) Copyright 1993, 1994 Hewlett-Packard Company
 *  (c) Copyright 1993, 1994 International Business Machines Corp.
 *  (c) Copyright 1993, 1994 Novell, Inc.
 *  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 */

#ifndef _CMSMATCH_H
#define _CMSMATCH_H

#include <EUSCompat.h>
#include "ansi_c.h"
#include "cm.h"
#include "nametbl.h"

extern boolean_t _DtCmsMatchAttributes P((
			cms_entry *entry,
			uint num_attrs,
			cms_attribute *attrs,
			CSA_enum *ops));

#endif
