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
/* $XConsortium: findco.c /main/2 1996/05/09 04:06:00 drk $ */
/*
 *   COMPONENT_NAME: austext
 *
 *   FUNCTIONS: d_findco
 *
 *   ORIGINS: 157
 *
 *   OBJECT CODE ONLY SOURCE MATERIALS
 */
/*-----------------------------------------------------------------------
   findco.c -- db_VISTA find owner of current record module

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

/* Find owner of current record
*/
int
d_findco(nset TASK_PARM DBN_PARM)
int nset;
TASK_DECL
DBN_DECL
{
   int set;
   char mem[MEMPSIZE];
   DB_ADDR own;
   SET_ENTRY FAR *set_ptr;

   DB_ENTER(DB_ID TASK_ID LOCK_SET(SET_IO));

   if (nset_check(nset, &set, (SET_ENTRY FAR * FAR *)&set_ptr) != S_OKAY)
      RETURN( db_status );

   /* Make sure we have a current record */
   if ( ! curr_rec )
      RETURN( dberr(S_NOCR) );

   /* Read current record */
   if ( dio_read(curr_rec, (char FAR * FAR *)&crloc, NOPGHOLD) != S_OKAY )
      RETURN( db_status );
   
   /* Get the member ptr for this set */
   if ( r_gmem(set, crloc, mem) != S_OKAY )
      RETURN( db_status );

   /* Extract the owner ptr from the member ptr */
   bytecpy(&own, mem+MP_OWNER, DB_ADDR_SIZE);
   if ( ! own )  /* Record not connected to set */
      RETURN( dberr( S_NOTCON ) );

   /* set the new current owner and member */
   curr_own[set] = own;
   curr_mem[set] = curr_rec;
   curr_rec      = own;
#ifndef	 NO_TIMESTAMP
   /* set any timestamps */
   if ( db_tsrecs ) {
      d_utscr( &cr_time TASK_PARM );
      d_utscm(nset, &cm_time[set] TASK_PARM DBN_PARM);
      co_time[set] = cr_time;
   }
   if ( db_tssets )
      d_utscs(nset, &cs_time[set] TASK_PARM DBN_PARM);
#endif
   RETURN( db_status = S_OKAY );
}
/* vpp -nOS2 -dUNIX -nBSD -nVANILLA_BSD -nVMS -nMEMLOCK -nWINDOWS -nFAR_ALLOC -f/usr/users/master/config/nonwin findco.c */
