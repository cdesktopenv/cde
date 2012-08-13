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
/* $XConsortium: recwrite.c /main/2 1996/05/09 04:16:09 drk $ */
/*
 *   COMPONENT_NAME: austext
 *
 *   FUNCTIONS: d_recwrite
 *
 *   ORIGINS: 157
 *
 *   OBJECT CODE ONLY SOURCE MATERIALS
 */
/*-----------------------------------------------------------------------
   recwrite.c -- db_VISTA current record write module.

   (C) Copyright 1987 by Raima Corporation.
-----------------------------------------------------------------------*/

/* ********************** EDIT HISTORY *******************************

 SCR    DATE    INI                   DESCRIPTION
----- --------- --- -----------------------------------------------------
  158 15-JUN-88 RSC passed new flag to key_bldcom
      04-Aug-88 RTK MULTI_TASK changes

*/
#include <stdio.h>
#include "vista.h"
#include "dbtype.h"


/* Write contents to current record
*/
int
d_recwrite(rec TASK_PARM DBN_PARM)
CONST char FAR *rec; /* ptr to record area */
TASK_DECL
DBN_DECL
{
#ifndef	 NO_TIMESTAMP
   ULONG timestamp;
#endif
   INT  rt;        /* record type */
   char FAR *fptr;     /* field data pointer */
   char ckey[256]; /* current compound key data */
   char nkey[256]; /* new compound key data */
   int stat;
   register int fld;
   RECORD_ENTRY FAR *rec_ptr;
   register FIELD_ENTRY FAR *fld_ptr;

   DB_ENTER(DB_ID TASK_ID LOCK_SET(SET_IO));

   if ( ! dbopen ) RETURN( dberr(S_DBOPEN) );

   /* Make sure we have a current record */
   if ( ! curr_rec )
      RETURN( dberr(S_NOCR) );

   /* Read current record */
   if ( dio_read( curr_rec, (char FAR * FAR *)&crloc, PGHOLD) != S_OKAY )
      RETURN( db_status );

   /* Copy record type from record */
   bytecpy(&rt, crloc, sizeof(INT));
   rt &= ~RLBMASK; /* mask off rlb */
#ifndef	 ONE_DB
   rt += curr_db_table->rt_offset;
#endif
   rec_ptr = &record_table[rt];

   /* Check out each field before they are changed */
   for (fld = rec_ptr->rt_fields, fld_ptr = &field_table[fld];
	(fld < size_fd) && (fld_ptr->fd_rec == rt);
	++fld, ++fld_ptr) {

      /* Build compound key for new data supplied by user.  Note: cflag
	 must be the same here as in the 1st key_bldcom for r_chkfld */
      if ( fld_ptr->fd_type == COMKEY ) {
	 key_bldcom(fld, (char FAR *)rec, nkey, FALSE);
	 fptr = nkey;
      }
      else
	 fptr = (char FAR *)rec + fld_ptr->fd_ptr - rec_ptr->rt_data;

      if ( ! (fld_ptr->fd_flags & STRUCTFLD) ) {
	 if ( (stat = r_chkfld(fld, fld_ptr, crloc, fptr)) != S_OKAY ) {
	    dio_release(curr_rec);
	    RETURN( db_status = stat );
	 }
      }
   }
   /* Copy data from rec into crloc */
   for (fld = (rt == size_rt-1) ? (size_fd - 1) :
				  ((rec_ptr + 1)->rt_fields - 1),
						   fld_ptr = &field_table[fld];
	fld >= rec_ptr->rt_fields;
	--fld, --fld_ptr) {
      /* go backwards so comkeys are processed first */
      if ( fld_ptr->fd_type == COMKEY ) {
	 /* build old and new keys */
	 key_bldcom(fld, crloc + rec_ptr->rt_data, ckey, TRUE);
	 key_bldcom(fld, (char FAR *)rec, nkey, TRUE);

	 /* make sure value has changed */
	 if ((fldcmp(fld_ptr, ckey, nkey) != 0) &&
	     /* if the key has been stored */
	     (!(fld_ptr->fd_flags & OPTKEYMASK) || r_tstopt(fld_ptr, crloc))) {
	    /* delete the old key */ 
	    if ( key_delete(fld, ckey, curr_rec) == S_OKAY ) {
	       /* insert the new one */
	       if ( key_insert( fld, nkey, curr_rec ) != S_OKAY )
		  RETURN( db_status );
	    }
	    else 
	       RETURN( db_status == S_NOTFOUND? dberr(S_KEYERR): db_status );
	 }
      }
      else if ( ! (STRUCTFLD & fld_ptr->fd_flags) ) {
	 /* ignore sub-fields of structures */
	 if (r_pfld(fld, fld_ptr, crloc,
		    rec + fld_ptr->fd_ptr - rec_ptr->rt_data,
		    &curr_rec) != S_OKAY)  {
	    stat = db_status;
	    dio_release(curr_rec);
	    RETURN( db_status = stat );
	 }
      }
   }
#ifndef	 NO_TIMESTAMP
   /* check for timestamp */
   if ( rec_ptr->rt_flags & TIMESTAMPED ) {
      timestamp = dio_pzgetts(rec_ptr->rt_file);
      bytecpy( crloc + RECUPTIME, &timestamp, sizeof(LONG));
   }
   else
      timestamp = 0L;
#endif
   /* write current record to page */
   dio_write(curr_rec, NULL, PGFREE);
#ifndef	 NO_TIMESTAMP
   if (( db_status == S_OKAY ) && db_tsrecs )
      cr_time = timestamp;
#endif
   RETURN( db_status );
}
/* vpp -nOS2 -dUNIX -nBSD -nVANILLA_BSD -nVMS -nMEMLOCK -nWINDOWS -nFAR_ALLOC -f/usr/users/master/config/nonwin recwrite.c */
