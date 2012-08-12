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
/* $XConsortium: delete.c /main/2 1996/05/09 04:04:16 drk $ */
/*
 *   COMPONENT_NAME: austext
 *
 *   FUNCTIONS: d_delete
 *
 *   ORIGINS: 157
 *
 *   OBJECT CODE ONLY SOURCE MATERIALS
 */
/*-----------------------------------------------------------------------
   delete.c -- db_VISTA record deletion module.

   (C) Copyright 1987 by Raima Corporation.
-----------------------------------------------------------------------*/

/* ********************** EDIT HISTORY *******************************

 SCR    DATE    INI                   DESCRIPTION
----- --------- --- -----------------------------------------------------
      04-Aug-88 RTK MULTI_TASK changes
*/

#include <stdio.h>
#include "vista.h"
#include "dbtype.h"

static DB_ADDR zap_dba = NULL_DBA;


/* Delete the current record, with error checking
*/
int
d_delete(TASK_DBN_ONLY)
TASK_DECL
DBN_DECL
{
   register int st_lc, mt_lc;		/* loop control */
   INT i, rt;
   char FAR *ptr;
   SET_ENTRY FAR *set_ptr;
   MEMBER_ENTRY FAR *mem_ptr;
   DB_ADDR FAR *co_ptr, FAR *cm_ptr;
#ifndef	 NO_TIMESTAMP
   ULONG FAR *cots_ptr, FAR *cmts_ptr, FAR *sts_ptr;
#endif

   DB_ENTER(DB_ID TASK_ID LOCK_SET(SET_IO));

   if ( ! dbopen ) RETURN( dberr(S_DBOPEN) );

   if ( ! curr_rec )  RETURN( dberr( S_NOCR ) );

   /* get the record type of the current record */
   if ( dio_read( curr_rec, (char FAR * FAR *)&ptr , NOPGHOLD) != S_OKAY )
      RETURN( db_status );
   bytecpy( &rt, ptr, sizeof(INT) );
   if (rt < 0)
      RETURN( dberr(S_INVADDR) );
   rt &= ~RLBMASK; /* mask off rlb */
#ifndef	 ONE_DB
   rt += curr_db_table->rt_offset;
#endif

   /* make sure this is not the system record */
   if ( record_table[rt].rt_fdtot == -1 )
      RETURN( dberr( S_DELSYS ) );

   /* scan the set list for sets that this record owns to confirm it is
      empty
   */
   for (st_lc = size_st, set_ptr = set_table; --st_lc >= 0; ++set_ptr) {
      if ((set_ptr->st_own_rt == rt) &&
	  (bytecmp(ptr+set_ptr->st_own_ptr+SP_FIRST,&zap_dba,DB_ADDR_SIZE) != 0))
	    RETURN( dberr( S_HASMEM ) );
   }
   /* scan the member list for sets which own this record */
   for (mt_lc = size_mt, mem_ptr = member_table; --mt_lc >= 0; ++mem_ptr) {
      if ((mem_ptr->mt_record == rt) &&
	  (bytecmp(ptr+mem_ptr->mt_mem_ptr+MP_OWNER,&zap_dba,DB_ADDR_SIZE) != 0))
	    RETURN( dberr( S_ISMEM ) );
   }
   /* delete record */
   if ( r_delrec( rt, curr_rec ) == S_OKAY ) {
      /* nullify any currency containing deleted record */
#ifndef	 NO_TIMESTAMP
      for (i = 0, co_ptr = curr_own, cm_ptr = curr_mem, cots_ptr = co_time,
				       cmts_ptr = cm_time, sts_ptr = cs_time;
	   i < size_st;
	   ++i, ++co_ptr, ++cm_ptr, ++cots_ptr, ++cmts_ptr, ++sts_ptr) {
#else
      for (i = 0, co_ptr = curr_own, cm_ptr = curr_mem;
	   i < size_st;
	   ++i, ++co_ptr, ++cm_ptr) {
#endif
	 if ( ADDRcmp(&curr_rec, co_ptr) == 0 ) {
	    *co_ptr = NULL_DBA;
#ifndef	 NO_TIMESTAMP
	    if ( db_tsrecs ) *cots_ptr = 0L;
	    if ( db_tssets ) *sts_ptr = 0L;
#endif
	 }
	 if ( ADDRcmp(&curr_rec, cm_ptr) == 0 ) {
	    *cm_ptr = NULL_DBA;
#ifndef	 NO_TIMESTAMP
	    if ( db_tsrecs ) *cmts_ptr = 0L;
#endif
	 }
      }
      curr_rec = NULL_DBA;
#ifndef	 NO_TIMESTAMP
      cr_time  = 0L;
#endif
   }
   RETURN( db_status );
}
/* vpp -nOS2 -dUNIX -nBSD -nVANILLA_BSD -nVMS -nMEMLOCK -nWINDOWS -nFAR_ALLOC -f/usr/users/master/config/nonwin delete.c */
