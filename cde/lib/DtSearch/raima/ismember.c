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
/* $XConsortium: ismember.c /main/2 1996/05/09 04:08:28 drk $ */
/*
 *   COMPONENT_NAME: austext
 *
 *   FUNCTIONS: d_ismember
 *
 *   ORIGINS: 157
 *
 *   OBJECT CODE ONLY SOURCE MATERIALS
 */
/*-----------------------------------------------------------------------
   ismember.c -- db_VISTA membership test module.

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


/* Check to see if current record is a connected member of SET
*/
int
d_ismember(set TASK_PARM DBN_PARM)
int set;  /* set table entry */
TASK_DECL
DBN_DECL  /* database number */
{
   char crmp[MEMPSIZE];   /* current record's member pointer */
   SET_ENTRY FAR *set_ptr;

   DB_ENTER(DB_ID TASK_ID LOCK_SET(SET_IO));
   
   if (nset_check(set, &set, (SET_ENTRY FAR * FAR *)&set_ptr) != S_OKAY)
      RETURN( db_status );

   /* Make sure we have a current record */
   if ( ! curr_rec )
      RETURN( dberr(S_NOCR) );

   /* Read current record and check connection */
   if ((dio_read( curr_rec, (char FAR * FAR *)&crloc , NOPGHOLD) == S_OKAY) &&
       (r_gmem(set, crloc, crmp) == S_OKAY) &&
       null_dba(crmp + MP_OWNER))
      db_status = S_EOS; /* end-of-set if curr rec not owned */

   RETURN( db_status );
}
/* vpp -nOS2 -dUNIX -nBSD -nVANILLA_BSD -nVMS -nMEMLOCK -nWINDOWS -nFAR_ALLOC -f/usr/users/master/config/nonwin ismember.c */
