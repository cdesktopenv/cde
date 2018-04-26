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
/* $XConsortium: appt4.h /main/1 1996/04/21 19:21:37 drk $ */
/*
 *  (c) Copyright 1993, 1994 Hewlett-Packard Company
 *  (c) Copyright 1993, 1994 International Business Machines Corp.
 *  (c) Copyright 1993, 1994 Novell, Inc.
 *  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 */

#ifndef _APPT4_H
#define _APPT4_H

/*
 * this file contains allocate and free routines for v4 data structures
 */

#include "rtable4.h"
#include "ansi_c.h"

extern Appt_4 *_DtCm_make_appt4 P(());

extern Appt_4 *_DtCm_copy_one_appt4 P((Appt_4 *a4));

extern Appt_4 *_DtCm_copy_appt4 P((Appt_4 *a4));

extern Appt_4 *_DtCm_copy_semiprivate_appt4 P((Appt_4 *original));

extern Except_4 *_DtCm_copy_excpt4 P((Except_4 *e4));

extern Abb_Appt_4 *_DtCm_appt_to_abbrev4 P((Appt_4 *original));

extern Abb_Appt_4 *_DtCm_appt_to_semiprivate_abbrev4 P((Appt_4 *original));

extern void _DtCm_free_appt4 P((Appt_4 *a));

extern void _DtCm_free_abbrev_appt4 P((Abb_Appt_4 *a));

extern Attribute_4 * _DtCm_make_attr4 P(());

extern void _DtCm_free_attr4 P((Attribute_4 *a));

extern Reminder_4 *_DtCm_copy_reminder4 P((Reminder_4 *r4));

extern void _DtCm_free_reminder4 P((Reminder_4 *r));

extern void _DtCm_free_keyentry4 P((Uid_4 *k));

extern Access_Entry_4 *_DtCm_make_access_entry4 P((char *who, int perms));

extern Access_Entry_4 *_DtCm_copy_access_list4 P((Access_Entry_4 *l4));

extern void _DtCm_free_access_list4 P((Access_Entry_4 *l));

extern void _DtCm_free_excpt4 P((Except_4 *e));

extern char * _DtCm_get_default_what_string P(());

#endif
