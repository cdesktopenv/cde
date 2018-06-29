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
/* $XConsortium: findnm.c /main/2 1996/05/09 04:06:45 drk $ */
/*
 *   COMPONENT_NAME: austext
 *
 *   FUNCTIONS: d_findnm
 *
 *   ORIGINS: 157
 *
 *   OBJECT CODE ONLY SOURCE MATERIALS
 */
/*-----------------------------------------------------------------------
   findnm.c -- db_VISTA find next member module

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

/* Find next member of set
*/
int
d_findnm(int nset, int dbn)
{
   int set;
   char memp[MEMPSIZE];
   DB_ADDR mem;
   char *recp;
   DB_ADDR *cm_ptr;
   SET_ENTRY *set_ptr;

   DB_ENTER(DB_ID TASK_ID LOCK_SET(SET_IO));

   if (nset_check(nset, &set, (SET_ENTRY * *)&set_ptr) != S_OKAY)
      RETURN( db_status );

   /* make sure we have a current owner */
   if ( ! curr_own[set] )
      RETURN( dberr(S_NOCO) );

   /* find first member if no current member */
   if ( !*(cm_ptr = &curr_mem[set]) )
      RETURN( d_findfm(nset , dbn) );

   /* read current member of set and get member pointer from record */
   if ((dio_read(*cm_ptr, (char * *)&recp, NOPGHOLD) != S_OKAY) ||
       (r_gmem(set, recp, memp) != S_OKAY))
      RETURN( db_status );

   /* extract address of next member */
   bytecpy(&mem, memp+MP_NEXT, DB_ADDR_SIZE);

   /* set current record and member */
   *cm_ptr = mem;

   if ( ! mem ) /* end of set */
      RETURN( db_status = S_EOS );

   curr_rec  = mem;
   RETURN( db_status = S_OKAY );
}
/* vpp -nOS2 -dUNIX -nBSD -nVANILLA_BSD -nVMS -nMEMLOCK -nWINDOWS -nFAR_ALLOC -f/usr/users/master/config/nonwin findnm.c */
