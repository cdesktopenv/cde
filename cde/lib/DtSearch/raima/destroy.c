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
/* $XConsortium: destroy.c /main/2 1996/05/09 04:04:29 drk $ */
/*
 *   COMPONENT_NAME: austext
 *
 *   FUNCTIONS: d_destroy
 *
 *   ORIGINS: 157
 *
 *   OBJECT CODE ONLY SOURCE MATERIALS
 */
/*-----------------------------------------------------------------------
   destroy.c -- db_VISTA database remove module.

   (C) Copyright 1987 by Raima Corporation.
-----------------------------------------------------------------------*/

/* ********************** EDIT HISTORY *******************************

 SCR    DATE    INI                   DESCRIPTION
----- --------- --- -----------------------------------------------------
      04-Aug-88 RTK MULTI_TASK changes
      17-Oct-88 RSC Changed return to RETURN
*/

#include <stdio.h>
#include "vista.h"
#include "dbtype.h"


/* Database remove function
*/
/* Warning: this function will destroy the entire contents
   of the database
*/
int
d_destroy(dbname TASK_PARM)
CONST char FAR *dbname;
TASK_DECL
{
   register int ft_lc;			/* loop control */
   register FILE_ENTRY FAR *file_ptr;
   int ovfl_save;

   DB_ENTER(NO_DB_ID TASK_ID LOCK_SET(LOCK_ALL));

   /* database must be closed (might be multiple databases open) */
   if ( dbopen ) d_close(TASK_ONLY);
#ifndef NO_TRANS

   ovfl_save = use_ovfl;
   use_ovfl = NO;
#endif

   /* prepare for the inittab and read in the database tables */
   if ((initdbt(dbname) != S_OKAY) || (inittab() != S_OKAY))
      RETURN( db_status );

   /* remove db files in file_table */
   for (ft_lc = size_ft, file_ptr = file_table; --ft_lc >= 0; ++file_ptr) {
      unlink(file_ptr->ft_name);
   }

   /* free all residual memory */
   termfree();
#ifndef NO_TRANS

   use_ovfl = ovfl_save;
#endif

   RETURN( db_status );
}
/* vpp -nOS2 -dUNIX -nBSD -nVANILLA_BSD -nVMS -nMEMLOCK -nWINDOWS -nFAR_ALLOC -f/usr/users/master/config/nonwin destroy.c */
