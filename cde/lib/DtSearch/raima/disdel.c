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
/* $XConsortium: disdel.c /main/2 1996/05/09 04:04:57 drk $ */
/*
 *   COMPONENT_NAME: austext
 *
 *   FUNCTIONS: d_disdel
 *
 *   ORIGINS: 157
 *
 *   OBJECT CODE ONLY SOURCE MATERIALS
 */
/*-----------------------------------------------------------------------
   disdel.c -- db_VISTA set disconnection/deletion module

   (C) Copyright 1985, 1986, 1987 by Raima Corp.
-----------------------------------------------------------------------*/

/* ********************** EDIT HISTORY *******************************

 SCR    DATE    INI                   DESCRIPTION
----- --------- --- -----------------------------------------------------
      04-Aug-88 RTK MULTI_TASK changes
  518 06-Jan-89 RSC d_discon infinite loops (not checking d_discon status)
*/

#include <stdio.h>
#include "vista.h"
#include "dbtype.h"

/* Disconnect from all sets (owner and member) and delete record
*/
int
d_disdel(TASK_DBN_ONLY)
TASK_DECL
DBN_DECL
{
   int rectype, nset, cset;
   register int set, mem;
   DB_ADDR dba, odba, mdba;
   DB_ADDR FAR *co_ptr, FAR *cm_ptr;
   register SET_ENTRY FAR *set_ptr;
   register MEMBER_ENTRY FAR *mem_ptr;
   int memtot;

   DB_ENTER(DB_ID TASK_ID LOCK_SET(SET_IO));

   if ( d_crtype(&rectype TASK_PARM DBN_PARM) != S_OKAY )
      RETURN( db_status );
   rectype += NUM2INT(-RECMARK, rt_offset);

   d_crget(&dba TASK_PARM DBN_PARM);
   for (set = 0, set_ptr = &set_table[ORIGIN(st_offset)];
	set < TABLE_SIZE(Size_st);
	++set, ++set_ptr) {
      nset = set + SETMARK;
      cset = set + ORIGIN(st_offset);
      co_ptr = &curr_own[cset];
      cm_ptr = &curr_mem[cset];
      odba = *co_ptr;
      mdba = *cm_ptr;
      if (set_ptr->st_own_rt == rectype) {
	 /* disconnect all member records from set s */
	 d_setor(nset TASK_PARM DBN_PARM);
	 while (d_findfm(nset TASK_PARM DBN_PARM) == S_OKAY)
	    if (d_discon(nset TASK_PARM DBN_PARM) < S_OKAY)
		  RETURN (db_status);
	 d_setro(nset TASK_PARM DBN_PARM);
      }
      for (mem = set_ptr->st_members, memtot = mem + set_ptr->st_memtot,
						mem_ptr = &member_table[mem];
	   mem < memtot;
	   ++mem, ++mem_ptr) {
	 if (mem_ptr->mt_record == rectype) {
	    /* disconnect current record from set */
	    if (d_ismember(nset TASK_PARM DBN_PARM) == S_OKAY) {
	       d_csmset(nset, &dba TASK_PARM DBN_PARM);
	       if (d_discon(nset TASK_PARM DBN_PARM) < S_OKAY)
		  RETURN (db_status);
	    }
	 }
      }
      curr_rec = dba;
      if ( dba == odba ) {
	 *co_ptr = NULL_DBA;
	 *cm_ptr = NULL_DBA;
      }
      else 
	 *co_ptr = odba;

      if ( dba == mdba ) 
	 *cm_ptr = NULL_DBA;
      else
	 *cm_ptr = mdba;
   }
   RETURN( d_delete(TASK_DBN_ONLY) );
}
/* vpp -nOS2 -dUNIX -nBSD -nVANILLA_BSD -nVMS -nMEMLOCK -nWINDOWS -nFAR_ALLOC -f/usr/users/master/config/nonwin disdel.c */
