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
/* $XConsortium: gtscs.c /main/2 1996/05/09 04:07:58 drk $ */
/*
 *   COMPONENT_NAME: austext
 *
 *   FUNCTIONS: d_gtscs
 *
 *   ORIGINS: 157
 *
 *   OBJECT CODE ONLY SOURCE MATERIALS
 */
/*-----------------------------------------------------------------------
   gtscs.c -- db_VISTA set timestamp get module.

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


#ifndef	 NO_TIMESTAMP
/* Get timestamp of set
*/
d_gtscs(set, timestamp TASK_PARM DBN_PARM)
int set;
ULONG FAR *timestamp;
TASK_DECL
DBN_DECL
{
   SET_ENTRY FAR *set_ptr;

   DB_ENTER(DB_ID TASK_ID LOCK_SET(SET_NOIO));

   if (nset_check(set, &set, (SET_ENTRY FAR * FAR *)&set_ptr) != S_OKAY)
      RETURN( db_status );

   /* make sure we have a current owner */
   if ( curr_own[set] ) {
      if ( db_tssets )
	 *timestamp = cs_time[set];
      else
	 dberr(S_TIMESTAMP);
   }
   else
      dberr(S_NOCO);

   RETURN( db_status );
}
#endif
/* vpp -nOS2 -dUNIX -nBSD -nVANILLA_BSD -nVMS -nMEMLOCK -nWINDOWS -nFAR_ALLOC -f/usr/users/master/config/nonwin gtscs.c */
