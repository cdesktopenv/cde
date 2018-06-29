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
/* $XConsortium: csmread.c /main/2 1996/05/09 03:59:07 drk $ */
/*
 *   COMPONENT_NAME: austext
 *
 *   FUNCTIONS: d_csmread
 *
 *   ORIGINS: 157
 *
 *   OBJECT CODE ONLY SOURCE MATERIALS
 */
/*-----------------------------------------------------------------------
   csmread.c -- db_VISTA current set member field read module.

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


/* Read data from field of current set member
*/
int
d_csmread(
int set,    /* Set constant */
long field, /* Field constant */
char *data, /* Data area to contain field contents */
int dbn     /* database number */
)
{
   int fld, rec;
   char *recp;
   SET_ENTRY *set_ptr;
   RECORD_ENTRY *rec_ptr;
   FIELD_ENTRY *fld_ptr;

   DB_ENTER(DB_ID TASK_ID LOCK_SET(SET_IO));

   if ((nset_check(set, &set, (SET_ENTRY * *)&set_ptr) != S_OKAY) ||
       (nfld_check(field, &rec, &fld, (RECORD_ENTRY * *)&rec_ptr, (FIELD_ENTRY * *)&fld_ptr) != S_OKAY))
      RETURN( db_status );

   /* Make sure we have a current member */
   if ( ! curr_mem[set] )
      RETURN( dberr(S_NOCM) );

   /* set up to allow unlocked read */

   /* Read current member */
   dio_read( curr_mem[set], (char * *)&recp , NOPGHOLD);
   if (db_status != S_OKAY)
      RETURN( db_status );

   /* Get data from record and return */
   RETURN( r_gfld(fld_ptr, recp, data) );
}
/* vpp -nOS2 -dUNIX -nBSD -nVANILLA_BSD -nVMS -nMEMLOCK -nWINDOWS -nFAR_ALLOC -f/usr/users/master/config/nonwin csmread.c */
