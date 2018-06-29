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
/* $XConsortium: crwrite.c /main/2 1996/05/09 03:58:39 drk $ */
/*
 *   COMPONENT_NAME: austext
 *
 *   FUNCTIONS: d_crwrite
 *
 *   ORIGINS: 157
 *
 *   OBJECT CODE ONLY SOURCE MATERIALS
 */
/*-----------------------------------------------------------------------
   crwrite.c -- db_VISTA current record field write module.

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


/* Write data to a field  in the current record
*/
int
d_crwrite(
long field, /* field constant */
char *data, /* data area to contain field contents */
int dbn     /* database number */
)
{
   int stat, fld, rec;
   RECORD_ENTRY *rec_ptr;
   FIELD_ENTRY *fld_ptr;

   DB_ENTER(DB_ID TASK_ID LOCK_SET(SET_IO));
   
   if (nfld_check(field, &rec, &fld, (RECORD_ENTRY * *)&rec_ptr, (FIELD_ENTRY * *)&fld_ptr) != S_OKAY)
      RETURN( db_status );

   /* compound keys cannot be updated directly */
   if ( fld_ptr->fd_type == COMKEY )
      RETURN( dberr(S_ISCOMKEY) );

   /* field used in compound keys cannot be updated directly */
   if ( fld_ptr->fd_flags & COMKEYED )
      RETURN( dberr(S_COMKEY) );

   /* Make sure we have a current record */
   if ( ! curr_rec )
      RETURN( dberr(S_NOCR) );

   /* Read current record */
   if ( dio_read( curr_rec, (char * *)&crloc , PGHOLD) != S_OKAY )
      RETURN( db_status );
	    
   /* check out the field */
   if ( (stat = r_chkfld(fld, fld_ptr, crloc, data)) != S_OKAY ) {
      dio_release(curr_rec);
      RETURN( db_status = stat );
   }
   /* put data into record and return */
   if ( r_pfld(fld, fld_ptr, crloc, data, &curr_rec) != S_OKAY )
      RETURN( db_status );
   dio_write(curr_rec, NULL, PGFREE);
   RETURN( db_status );
}
/* vpp -nOS2 -dUNIX -nBSD -nVANILLA_BSD -nVMS -nMEMLOCK -nWINDOWS -nFAR_ALLOC -f/usr/users/master/config/nonwin crwrite.c */
