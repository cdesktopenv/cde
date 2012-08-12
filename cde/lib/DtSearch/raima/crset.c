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
/* $XConsortium: crset.c /main/2 1996/05/09 03:57:56 drk $ */
/*
 *   COMPONENT_NAME: austext
 *
 *   FUNCTIONS: d_crset
 *
 *   ORIGINS: 157
 *
 *   OBJECT CODE ONLY SOURCE MATERIALS
 */
/*-----------------------------------------------------------------------
   crset.c -- db_VISTA current record database address set module.

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

/* Set current record
*/
int
d_crset(dba TASK_PARM DBN_PARM)
DB_ADDR FAR *dba; /* db address of record to become current */
TASK_DECL
DBN_DECL
{
   DB_ENTER(DB_ID TASK_ID LOCK_SET(RECORD_IO));

   if ( ! dbopen ) RETURN( dberr(S_DBOPEN) );

   if ( dba == NULL ) {
      curr_rec = NULL_DBA;
      db_status = S_OKAY;
   }
   else if ( check_dba(*dba) == S_OKAY ) 
      curr_rec = *dba;
   
   RETURN( db_status );
}
/* vpp -nOS2 -dUNIX -nBSD -nVANILLA_BSD -nVMS -nMEMLOCK -nWINDOWS -nFAR_ALLOC -f/usr/users/master/config/nonwin crset.c */
