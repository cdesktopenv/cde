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
 * License along with these librararies and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
/* $XConsortium: free.h /main/1 1996/04/21 19:23:19 drk $ */
/*
 *  (c) Copyright 1993, 1994 Hewlett-Packard Company
 *  (c) Copyright 1993, 1994 International Business Machines Corp.
 *  (c) Copyright 1993, 1994 Novell, Inc.
 *  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 */

#ifndef _FREE_H
#define _FREE_H

#include <EUSCompat.h>
#include "ansi_c.h"
#include <sys/types.h>

extern CSA_calendar_user *_DtCm_alloc_calendar_users(uint num_elem);

extern char **_DtCm_alloc_character_pointers(uint num_elem);

extern CSA_attribute *_DtCm_alloc_attributes(uint num_elem);

extern CSA_entry_handle *_DtCm_alloc_entry_handles(uint num_elem);

extern CSA_reminder_reference *_DtCm_alloc_reminder_references(uint num_elem);

extern void *_DtCm_alloc_entry();

extern CSA_return_code _DtCm_free(void *ptr);

extern void _DtCm_free_character_pointers P((uint num_elem, void *vptr));

#endif

