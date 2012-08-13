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
/* $XConsortium: keystore.c /main/2 1996/05/09 04:10:59 drk $ */
/*
 *   COMPONENT_NAME: austext
 *
 *   FUNCTIONS: d_keystore
 *
 *   ORIGINS: 157
 *
 *   OBJECT CODE ONLY SOURCE MATERIALS
 */
/*-----------------------------------------------------------------------
   keystore.c -- db_VISTA optional key store module.

   (C) Copyright 1987 by Raima Corporation.
-----------------------------------------------------------------------*/

/* ********************** EDIT HISTORY *******************************

 SCR    DATE    INI                   DESCRIPTION
----- --------- --- -----------------------------------------------------
  158 15-JUN-88 RSC passed new flag to key_bldcom - always compliment compound
      04-Aug-88 RTK MULTI_TASK changes
*/
#include <stdio.h>
#include "vista.h"
#include "dbtype.h"

#define FALSE 0
#define TRUE 1

/* Store optional key value
*/
int
d_keystore(field TASK_PARM DBN_PARM)
long field; /* key field constant */
TASK_DECL
DBN_DECL    /* database number */
{
   int fld;        /* field number */
   int rec, rn;    /* record type of current record */
   char FAR *rptr;     /* pointer to current record */
   char FAR *fptr;     /* pointer to field contents */
   char ckey[256]; /* compound key */
   int stat;
   RECORD_ENTRY FAR *rec_ptr;
   FIELD_ENTRY FAR *fld_ptr;

   DB_ENTER(DB_ID TASK_ID LOCK_SET(RECORD_IO));

   if (nfld_check(field, &rec, &fld, (RECORD_ENTRY FAR * FAR *)&rec_ptr, (FIELD_ENTRY FAR * FAR *)&fld_ptr) != S_OKAY)
      RETURN( db_status );

   /* Make sure we have a current record */
   if ( ! curr_rec )
      RETURN( dberr(S_NOCR) );

   /* ensure current record is valid for this field */
   d_crtype(&rn TASK_PARM DBN_PARM);

   if (rec != NUM2INT(rn - RECMARK, rt_offset))
      RETURN( dberr(S_BADFIELD) );

   /* ensure field is an optional key field */
   if ( ! (fld_ptr->fd_flags & OPTKEYMASK) )
      RETURN( dberr(S_NOTOPTKEY) );

   /* read current record */
   if ( (stat = dio_read(curr_rec, (char FAR * FAR *)&rptr, PGHOLD)) == S_OKAY )  {
      /* Make sure that the key has not already been stored */
      if ( (stat = r_tstopt( fld_ptr, rptr )) != S_OKAY ) {
	 if ( dio_release( curr_rec ) != S_OKAY )
	    RETURN( db_status );
	 RETURN( db_status = S_OKAY );
      }

      if ( fld_ptr->fd_type == COMKEY ) {
	 key_bldcom(fld, rptr + rec_ptr->rt_data, ckey, TRUE);
	 fptr = ckey;
      }
      else
	 fptr = rptr + fld_ptr->fd_ptr;

      /* store key from value in current record */
      stat = key_insert(fld, fptr, curr_rec);

      /* Set the optional key bit */
      if ( stat == S_OKAY )
	 stat = r_setopt( fld_ptr, rptr );

      /* The data record has been modified */
      dio_write(curr_rec, rptr, PGFREE);  
   }
   RETURN( db_status = stat );
}
/* vpp -nOS2 -dUNIX -nBSD -nVANILLA_BSD -nVMS -nMEMLOCK -nWINDOWS -nFAR_ALLOC -f/usr/users/master/config/nonwin keystore.c */
