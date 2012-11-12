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
/* $XConsortium: rwcurr.c /main/2 1996/05/09 04:16:39 drk $ */
/*
 *   COMPONENT_NAME: austext
 *
 *   FUNCTIONS: d_rdcurr
 *		d_rerdcurr
 *		d_wrcurr
 *
 *   ORIGINS: 157
 *
 *   OBJECT CODE ONLY SOURCE MATERIALS
 */
/*-----------------------------------------------------------------------
   rwcurr.c -- db_VISTA currency table save/restore module.

   (C) Copyright 1987 by Raima Corporation.
-----------------------------------------------------------------------*/

/* ********************** EDIT HISTORY *******************************

 SCR    DATE    INI                   DESCRIPTION
----- --------- --- -----------------------------------------------------
      04-Aug-88 RTK MULTI_TASK changes
 420  16-Aug-88 RTK Made function parameters consistent with existing code.
      09-Mar-89 WLW Non-windows version needed to do free() instead of FREE().
*/

#include <stdio.h>
#include "vista.h"
#include "dbtype.h"


/* Read currency table
*/
int
d_rdcurr(currbuff, currsize TASK_PARM)
DB_ADDR **currbuff;
int FAR *currsize;
TASK_DECL
{
   int cs;

   DB_ENTER(NO_DB_ID TASK_ID LOCK_SET(SET_NOIO));

   cs = 2*size_st + 1;
#ifndef	 NO_TIMESTAMP
   if ( db_tsrecs ) cs *= 2;
   if ( db_tssets ) cs += size_st;
#endif
   *currbuff = (DB_ADDR *)ALLOC(NULL, cs*sizeof(DB_ADDR), "currbuff");
   if ( *currbuff == NULL ) RETURN( dberr(S_NOMEMORY) );
   *currsize = cs * sizeof(DB_ADDR);
   RETURN( d_rerdcurr(currbuff TASK_PARM) );
}

int d_rerdcurr(currbuff TASK_PARM)
DB_ADDR **currbuff;
TASK_DECL
{
   DB_ADDR FAR *cb_ptr;

   DB_ENTER(NO_DB_ID TASK_ID LOCK_SET(SET_NOIO));

   *(cb_ptr = *currbuff) = curr_rec;
   bytecpy(++cb_ptr, curr_own, size_st*sizeof(*cb_ptr));
   bytecpy(cb_ptr += size_st, curr_mem, size_st*sizeof(*cb_ptr));
#ifndef	 NO_TIMESTAMP
   if ( db_tsrecs ) {
      *(cb_ptr += size_st) = cr_time;
      bytecpy(++cb_ptr, co_time, size_st*sizeof(*cb_ptr));
      bytecpy(cb_ptr += size_st, cm_time, size_st*sizeof(*cb_ptr));
   }
   if ( db_tssets ) {
      bytecpy(cb_ptr + size_st, cs_time, size_st*sizeof(*cb_ptr));
   }
#endif
   MEM_UNLOCK(currbuff);
   RETURN( db_status = S_OKAY );
}



/* Write currency table
*/
int
d_wrcurr(currbuff TASK_PARM)
DB_ADDR *currbuff;
TASK_DECL
{
   DB_ADDR FAR *cb_ptr;

   DB_ENTER(NO_DB_ID TASK_ID LOCK_SET(SET_NOIO));

   if ( (cb_ptr = currbuff) != NULL) {
      curr_rec = *cb_ptr;
      bytecpy(curr_own, ++cb_ptr, size_st*sizeof(*cb_ptr));
      bytecpy(curr_mem, cb_ptr += size_st, size_st*sizeof(*cb_ptr));
#ifndef	 NO_TIMESTAMP
      if ( db_tsrecs ) {
	 cr_time = *(cb_ptr += size_st);
	 bytecpy(co_time, ++cb_ptr, size_st*sizeof(*cb_ptr));
	 bytecpy(cm_time, cb_ptr += size_st, size_st*sizeof(*cb_ptr));
      }
      if ( db_tssets ) {
	 bytecpy(cs_time, cb_ptr + size_st, size_st*sizeof(*cb_ptr));
      }
#endif
   }
   free(currbuff);
   RETURN( db_status = S_OKAY );
}
/* vpp -nOS2 -dUNIX -nBSD -nVANILLA_BSD -nVMS -nMEMLOCK -nWINDOWS -nFAR_ALLOC -f/usr/users/master/config/nonwin rwcurr.c */
