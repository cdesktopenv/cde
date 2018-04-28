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
/*******************************************************************************
**
**  cmsattr.h
**
**  static char sccsid[] = "@(#)cmsattr.h 1.2 94/10/05 Copyr 1991 Sun Microsystems, Inc.";
**
**  $XConsortium: cmsattr.h /main/3 1995/11/03 11:09:10 rswiston $
**
**  RESTRICTED CONFIDENTIAL INFORMATION:
**
**  The information in this document is subject to special
**  restrictions in a confidential disclosure agreement between
**  HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
**  document outside HP, IBM, Sun, USL, SCO, or Univel without
**  Sun's specific written approval.  This document and all copies
**  and derivative works thereof must be returned or destroyed at
**  Sun's request.
**
**  Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
**
*******************************************************************************/
#ifndef _CMSATTR_H
#define _CMSATTR_H

#include "ansi_c.h"
#include "cm.h"
#include "cmscalendar.h"

extern CSA_return_code _DtCmsUpdateSint32AttrVal P((
				cms_attribute_value *newval,
				cms_attribute_value **attrval));

extern CSA_return_code _DtCmsUpdateUint32AttrVal P((
				cms_attribute_value *newval,
				cms_attribute_value **attrval));

extern CSA_return_code _DtCmsUpdateStringAttrVal P((
				cms_attribute_value *newval,
				cms_attribute_value **attrval));

extern CSA_return_code _DtCmsUpdateAccessListAttrVal P((
				cms_attribute_value *newval,
				cms_attribute_value **attrval));

extern CSA_return_code _DtCmsUpdateReminderAttrVal P((
				cms_attribute_value *newval,
				cms_attribute_value **attrval));

extern CSA_return_code _DtCmsUpdateDateTimeListAttrVal P((
				cms_attribute_value *newval,
				cms_attribute_value **attrval));

extern CSA_return_code _DtCmsUpdateOpaqueDataAttrVal P((
				cms_attribute_value *newval,
				cms_attribute_value **attrval));

#endif
