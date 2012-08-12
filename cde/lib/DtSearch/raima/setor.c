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
/* $XConsortium: setor.c /main/2 1996/05/09 04:18:27 drk $ */
/*
 *   COMPONENT_NAME: austext
 *
 *   FUNCTIONS: d_setor
 *
 *   ORIGINS: 157
 *
 *   OBJECT CODE ONLY SOURCE MATERIALS
 */
/*-----------------------------------------------------------------------
   setor.c -- db_VISTA set owner to current record module.

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

/* Set current owner to current record
*/
int
d_setor(nset TASK_PARM DBN_PARM)
int nset;  /* set number */
TASK_DECL
DBN_DECL   /* database number */
{
   int set;
   int crtype; /* current record type */
   SET_ENTRY FAR *set_ptr;

   DB_ENTER(DB_ID TASK_ID LOCK_SET(SET_IO));

   if ((nset_check(nset, &set, (SET_ENTRY FAR * FAR *)&set_ptr) != S_OKAY) ||
       (d_crtype(&crtype TASK_PARM DBN_PARM) != S_OKAY))
      RETURN( db_status );

   crtype += NUM2INT(-RECMARK, rt_offset);

   if ( set_ptr->st_own_rt != crtype )
      RETURN( dberr( S_INVOWN ) );
      
   curr_own[set] = curr_rec;
   curr_mem[set] = NULL_DBA;
#ifndef	 NO_TIMESTAMP
   /* set timestamps */
   if ( db_tsrecs ) {
      co_time[set] = cr_time;
      cm_time[set] = 0L;
   }
   if ( db_tssets ) {
      d_utscs( nset, &cs_time[set] TASK_PARM DBN_PARM );
   }
#endif
   RETURN( db_status );
}
/* vpp -nOS2 -dUNIX -nBSD -nVANILLA_BSD -nVMS -nMEMLOCK -nWINDOWS -nFAR_ALLOC -f/usr/users/master/config/nonwin setor.c */
