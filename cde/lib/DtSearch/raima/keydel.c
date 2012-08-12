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
/* $XConsortium: keydel.c /main/2 1996/05/09 04:08:58 drk $ */
/*
 *   COMPONENT_NAME: austext
 *
 *   FUNCTIONS: d_keydel
 *
 *   ORIGINS: 157
 *
 *   OBJECT CODE ONLY SOURCE MATERIALS
 */
/*-----------------------------------------------------------------------
   keydel.c -- db_VISTA key deletion module.

   (C) Copyright 1987 by Raima Corporation.
-----------------------------------------------------------------------*/

/* ********************** EDIT HISTORY *******************************

 SCR    DATE    INI                   DESCRIPTION
----- --------- --- -----------------------------------------------------
  158 15-JUN-88 RSC added flag to key_bldcom - always compliment compound keys
      04-Aug-88 RTK MULTI_TASK changes

*/
#include <stdio.h>
#include "vista.h"
#include "dbtype.h"

#define FALSE 0
#define TRUE 1

/* Delete optional key value
*/
int
d_keydel(field TASK_PARM DBN_PARM)
long field; /* key field constant */
TASK_DECL
DBN_DECL    /* database number */
{
   int fld;        /* field number */
   int rec, rn;    /* record type of current record */
   char FAR *rptr;     /* pointer to current record */
   CONST char FAR *fptr;     /* pointer to field contents */
   char ckey[256]; /* compound key data */
   int stat;
   RECORD_ENTRY FAR *rec_ptr;
   FIELD_ENTRY FAR *fld_ptr;

   DB_ENTER(DB_ID TASK_ID LOCK_SET(RECORD_IO));

   if (nfld_check(field, &rec, &fld, (RECORD_ENTRY FAR * FAR *)&rec_ptr, (FIELD_ENTRY FAR * FAR *)&fld_ptr) != S_OKAY)
      RETURN( db_status );

   /* ensure current record is valid for this field */
   d_crtype(&rn TASK_PARM DBN_PARM);

   if (rec != NUM2INT(rn - RECMARK, rt_offset))
      RETURN( dberr(S_BADFIELD) );

   /* ensure field is an optional key field */
   if ( ! (fld_ptr->fd_flags & OPTKEYMASK) )
      RETURN( dberr(S_NOTOPTKEY) );

   /* read current record */
   if ( (stat = dio_read(curr_rec, (char FAR * FAR *)&rptr, PGHOLD)) == S_OKAY )  {
      /* Make sure that the key has been stored */
      if ( r_tstopt( fld_ptr, rptr ) == S_OKAY ) {
	 if ( dio_release( curr_rec ) != S_OKAY )
	    RETURN( db_status );
	 RETURN( db_status = S_NOTFOUND );
      }
      if ( fld_ptr->fd_type == COMKEY ) {
	 key_bldcom(fld, rptr + rec_ptr->rt_data, ckey, TRUE);
	 fptr = ckey;
      }
      else
	 fptr = rptr + fld_ptr->fd_ptr;

      /* delete key from value in current record */
      stat = key_delete(fld, fptr, curr_rec);

      /* Clear the optional key flag in the record */
      if ( stat == S_OKAY )
	 stat = r_clropt( fld_ptr, rptr );

      /* The data record has been updated */
      dio_write(curr_rec, rptr, PGFREE);  
   }
   RETURN( db_status = stat );
}
/* vpp -nOS2 -dUNIX -nBSD -nVANILLA_BSD -nVMS -nMEMLOCK -nWINDOWS -nFAR_ALLOC -f/usr/users/master/config/nonwin keydel.c */
