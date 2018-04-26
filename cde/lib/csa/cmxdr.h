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
/* $XConsortium: cmxdr.h /main/1 1996/04/21 19:22:17 drk $ */
/*
 *  (c) Copyright 1993, 1994 Hewlett-Packard Company
 *  (c) Copyright 1993, 1994 International Business Machines Corp.
 *  (c) Copyright 1993, 1994 Novell, Inc.
 *  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 */

#ifndef _CMXDR_H
#define _CMXDR_H

#include <EUSCompat.h>
#include "ansi_c.h"
#include "csa.h"

/*
 * xdr routines for xapia csa data structures
 */

bool_t xdr_CSA_extension P((XDR *xdrs, CSA_extension *objp));

bool_t xdr_CSA_date_time_entry P((XDR *xdrs, CSA_date_time_entry *objp));

bool_t xdr_CSA_date_time_list P((XDR *xdrs, CSA_date_time_list *objp));

bool_t xdr_CSA_calendar_user P((XDR *xdrs, CSA_calendar_user *objp));

bool_t xdr_CSA_access_rights P((XDR *xdrs, CSA_access_rights *objp));

bool_t xdr_CSA_access_list P((XDR *xdrs, CSA_access_list *objp));

bool_t xdr_CSA_attendee P((XDR *xdrs, CSA_attendee *objp));

bool_t xdr_CSA_attendee_list P((XDR *xdrs, CSA_attendee_list *objp));

bool_t xdr_CSA_opaque_data P((XDR *xdrs, CSA_opaque_data *objp));

bool_t xdr_CSA_reminder P((XDR *xdrs, CSA_reminder *objp));

bool_t xdr_CSA_attribute_value P((XDR *xdrs, CSA_attribute_value *objp));

#endif

