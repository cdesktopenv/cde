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
/* $XConsortium: convert4-3.h /main/1 1996/04/21 19:22:46 drk $ */
/*
 *  (c) Copyright 1993, 1994 Hewlett-Packard Company
 *  (c) Copyright 1993, 1994 International Business Machines Corp.
 *  (c) Copyright 1993, 1994 Novell, Inc.
 *  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 */

#ifndef _CONVERT4_3_H
#define _CONVERT4_3_H

#include "ansi_c.h"

/* convert4-3.h:  conversion routines for rpc.cmsd version 4 to
   rpc.cmsd version 3 data types
*/

extern Table_Res_3 *_DtCm_tableres4_to_tableres3 P((Table_Res_4*));
extern Table_Status_3 _DtCm_tablestat4_to_tablestat3 P((Table_Status_4));
extern Registration_Status_3 _DtCm_regstat4_to_regstat3 P((Registration_Status_4));
extern Access_Status_3 _DtCm_accstat4_to_accstat3 P((Access_Status_4));
extern Access_Args_3 *_DtCm_accargs4_to_accargs3 P((Access_Args_4*));
extern Uid_3 *_DtCm_uid4_to_uid3 P((Uid_4*));
extern Range_3 *_DtCm_range4_to_range3 P((Range_4*));
extern Appt_3 *_DtCm_appt4_to_appt3 P((Appt_4*));
extern void _DtCm_id4_to_id3 P((Id_4*, Id_3*));
extern Access_Entry_3 *_DtCm_acclist4_to_acclist3 P((Access_Entry_4*));
extern Keyrange_3 *_DtCm_keyrange4_to_keyrange3 P((Keyrange_4*));
extern Uid_3 *_DtCm_uidopt4_to_uid3 P((Uidopt_4*));
extern void _DtCm_free_attr3 P((Attribute_3 *a));
extern void _DtCm_free_appt3 P((Appt_3 *appts));

#endif
