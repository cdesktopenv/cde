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
/* $XConsortium: setmo.c /main/2 1996/05/09 04:17:25 drk $ */
/*
 *   COMPONENT_NAME: austext
 *
 *   FUNCTIONS: d_setmo
 *
 *   ORIGINS: 157
 *
 *   OBJECT CODE ONLY SOURCE MATERIALS
 */
/*-----------------------------------------------------------------------
   setmo.c -- db_VISTA set member to owner module.

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

/* Set current member to current owner
*/
int
d_setmo(setm, seto TASK_PARM DBN_PARM)
int setm;   /* set table entry number of member */
int seto;   /* set table entry number of owner */
TASK_DECL
DBN_DECL    /* database number */
{
   register int mem;
   SET_ENTRY FAR *setm_ptr, FAR *seto_ptr;
   register MEMBER_ENTRY FAR *mem_ptr;
   int memtot;

   DB_ENTER(DB_ID TASK_ID LOCK_SET(SET_IO));

   if ((nset_check(seto, &seto, (SET_ENTRY FAR * FAR *)&seto_ptr) != S_OKAY) ||
       (nset_check(setm, &setm, (SET_ENTRY FAR * FAR *)&setm_ptr) != S_OKAY))
      RETURN( db_status );

   if (null_dba((char FAR *)&curr_own[seto])) RETURN( dberr(S_NOCO) );

   for (mem = setm_ptr->st_members, memtot = mem + setm_ptr->st_memtot,
						mem_ptr = &member_table[mem];
	mem < memtot;
	++mem, ++mem_ptr) {
      if (mem_ptr->mt_record == seto_ptr->st_own_rt)
	 RETURN( r_smem(&curr_own[seto], setm) );
   }
   RETURN( dberr( S_INVMEM ) );
}
/* vpp -nOS2 -dUNIX -nBSD -nVANILLA_BSD -nVMS -nMEMLOCK -nWINDOWS -nFAR_ALLOC -f/usr/users/master/config/nonwin setmo.c */
