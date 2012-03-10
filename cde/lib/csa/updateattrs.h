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
