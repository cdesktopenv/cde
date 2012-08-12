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
/* $XConsortium: findpm.c /main/2 1996/05/09 04:06:57 drk $ */
/*
 *   COMPONENT_NAME: austext
 *
 *   FUNCTIONS: d_findpm
 *
 *   ORIGINS: 157
 *
 *   OBJECT CODE ONLY SOURCE MATERIALS
 */
/*-----------------------------------------------------------------------
   findpm.c -- db_VISTA find previous member module

   (C) Copyright 1987 by Raima Corp.
-----------------------------------------------------------------------*/

/* ********************** EDIT HISTORY *******************************

 SCR    DATE    INI                   DESCRIPTION
----- --------- --- -----------------------------------------------------
      04-Aug-88 RTK MULTI_TASK changes
*/

#include <stdio.h>
#include "vista.h"
#include "dbtype.h"

/* Find previous member of set
*/
int
d_findpm(nset TASK_PARM DBN_PARM)
int nset;
TASK_DECL
DBN_DECL
{
   int set;
   char memp[MEMPSIZE];
   DB_ADDR mem;
   char FAR *recp;
   DB_ADDR FAR *cm_ptr;
   SET_ENTRY FAR *set_ptr;

   DB_ENTER(DB_ID TASK_ID LOCK_SET(SET_IO));

   if (nset_check(nset, &set, (SET_ENTRY FAR * FAR *)&set_ptr) != S_OKAY)
      RETURN( db_status );

   /* make sure we have a current owner */
   if ( ! curr_own[set] )
      RETURN( dberr(S_NOCO) );

   /* find last member if no current member */
   if ( !*(cm_ptr = &curr_mem[set]) )
      RETURN( d_findlm(nset TASK_PARM DBN_PARM) );

   /* read current member of set and get member pointer from record */
   if ((dio_read(*cm_ptr, (char FAR * FAR *)&recp, NOPGHOLD) != S_OKAY) ||
       (r_gmem(set, recp, memp) != S_OKAY))
      RETURN( db_status );

   /* extract address of previous member */
   bytecpy( &mem, memp+MP_PREV, DB_ADDR_SIZE);

   /* set current record and member */
   *cm_ptr = mem;

   if ( ! mem ) /* end of set */
      RETURN( db_status = S_EOS );

   curr_rec = mem;
#ifndef	 NO_TIMESTAMP
   /* set timestamps */
   if ( db_tsrecs ) {
      d_utscr( &cr_time TASK_PARM );
      cm_time[set] = cr_time;
   }
#endif
   RETURN( db_status = S_OKAY );
}
/* vpp -nOS2 -dUNIX -nBSD -nVANILLA_BSD -nVMS -nMEMLOCK -nWINDOWS -nFAR_ALLOC -f/usr/users/master/config/nonwin findpm.c */
