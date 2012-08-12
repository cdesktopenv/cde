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
/* $XConsortium: setmr.c /main/2 1996/05/09 04:17:41 drk $ */
/*
 *   COMPONENT_NAME: austext
 *
 *   FUNCTIONS: d_setmr
 *
 *   ORIGINS: 157
 *
 *   OBJECT CODE ONLY SOURCE MATERIALS
 */
/*-----------------------------------------------------------------------
   setmr.c -- db_VISTA set member to current record module.

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

/* Set current member to current record
*/
int
d_setmr(set TASK_PARM DBN_PARM)
int set;   /* set table entry number */
TASK_DECL
DBN_DECL   /* database number */
{
   int crtype; /* current record type */
   register int mem;
   SET_ENTRY FAR *set_ptr;
   register MEMBER_ENTRY FAR *mem_ptr;
   int memtot;

   DB_ENTER(DB_ID TASK_ID LOCK_SET(SET_IO));

   if (nset_check(set, &set, (SET_ENTRY FAR * FAR *)&set_ptr) != S_OKAY)
      RETURN( db_status );

   if ( ! curr_rec )
      RETURN( dberr( S_NOCR ) );

   if ( d_crtype(&crtype TASK_PARM DBN_PARM) != S_OKAY )
      RETURN( db_status );
   crtype += NUM2INT(-RECMARK, rt_offset);

   for (mem = set_ptr->st_members, memtot = mem + set_ptr->st_memtot,
						mem_ptr = &member_table[mem];
	mem < memtot;
	++mem, ++mem_ptr) {
      if ( mem_ptr->mt_record == crtype )
	 RETURN( r_smem( &curr_rec, set ) );
   }
   RETURN( dberr( S_INVMEM ) );
}
/* vpp -nOS2 -dUNIX -nBSD -nVANILLA_BSD -nVMS -nMEMLOCK -nWINDOWS -nFAR_ALLOC -f/usr/users/master/config/nonwin setmr.c */
