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
/* $XConsortium: makenew.c /main/2 1996/05/09 04:11:43 drk $ */
/*
 *   COMPONENT_NAME: austext
 *
 *   FUNCTIONS: d_makenew
 *		d_setkey
 *		sk_free
 *
 *   ORIGINS: 157
 *
 *   OBJECT CODE ONLY SOURCE MATERIALS
 */
/*-----------------------------------------------------------------------
   makenew.c -- db_VISTA (empty) record creation module

   (C) Copyright 1987 by Raima Corp.
-----------------------------------------------------------------------*/

/* ********************** EDIT HISTORY *******************************

 SCR    DATE    INI                   DESCRIPTION
----- --------- --- -----------------------------------------------------
      04-Aug-88 RTK MULTI_TASK changes
      11-Oct-88 RTK Moved sk_list into db_global
*/

#include <stdio.h>
#include "vista.h"
#include "dbtype.h"


/* Set the value of a key field
*/
int
d_setkey( field, fldvalue TASK_PARM DBN_PARM )
long field;
CONST char FAR *fldvalue;
TASK_DECL
DBN_DECL
{
   SK_P FAR *sk_ptr;
   SK_P sk_p;
   int fld, rec;
   RECORD_ENTRY FAR *rec_ptr;
   FIELD_ENTRY FAR *fld_ptr;

   DB_ENTER(DB_ID TASK_ID LOCK_SET(RECORD_NOIO));

   if (nfld_check(field, &rec, &fld, (RECORD_ENTRY FAR * FAR *)&rec_ptr, (FIELD_ENTRY FAR * FAR *)&fld_ptr) != S_OKAY)
      RETURN( db_status );

   if ( fld_ptr->fd_key == 'n' )
      RETURN( dberr(S_NOTKEY) );

   ll_access(&sk_list);
   while ((sk_ptr = (SK_P FAR *)ll_next(&sk_list)) != NULL) {
      if ( sk_ptr->ptr->sk_fld == fld ) {
         MEM_LOCK(&sk_ptr->ptr->sk_val);
	 if ( fld_ptr->fd_type != CHARACTER || fld_ptr->fd_dim[1] )
	    bytecpy( sk_ptr->ptr->sk_val.ptr, fldvalue, fld_ptr->fd_len );
	 else if ( fld_ptr->fd_dim[0] )
	    strncpy( sk_ptr->ptr->sk_val.ptr, fldvalue, fld_ptr->fd_len );
	 else
	    *(sk_ptr->ptr->sk_val.ptr) = *fldvalue;
         MEM_UNLOCK(&sk_ptr->ptr->sk_val);
	 ll_deaccess(&sk_list);
	 RETURN( db_status = S_OKAY );
      }
   }
   /* need to allocate a slot for a new fld */
   sk_p.ptr = (struct sk FAR *)ALLOC(&sk_p, sizeof(struct sk), "sk_ptr->ptr");
   if ( sk_p.ptr == NULL )
      RETURN( dberr( S_NOMEMORY ) );
   if ( ll_prepend(&sk_list, (CHAR_P FAR *)&sk_p) != S_OKAY ) {
      RETURN( db_status );
   }
   sk_p.ptr->sk_fld = fld;
   sk_p.ptr->sk_val.ptr = ALLOC(&sk_p.ptr->sk_val, fld_ptr->fd_len + 1, 
				   "sk_p.ptr->sk_val.ptr");
   if ( sk_p.ptr->sk_val.ptr == NULL )
      RETURN( dberr( S_NOMEMORY ) );

   if ( fld_ptr->fd_type != CHARACTER || fld_ptr->fd_dim[1] )
      bytecpy( sk_p.ptr->sk_val.ptr, fldvalue, fld_ptr->fd_len );
   else if ( fld_ptr->fd_dim[0] )
      strncpy( sk_p.ptr->sk_val.ptr, fldvalue, fld_ptr->fd_len );
   else
      *(sk_p.ptr->sk_val.ptr) = *fldvalue;
   MEM_UNLOCK(&sk_p.ptr->sk_val);
   ll_deaccess(&sk_list);

   RETURN( db_status = S_OKAY );
}



/* Free the memory allocated for the sk_list
*/
int sk_free()
{
   SK_P FAR *sk_ptr;

   ll_access(&sk_list);
   while ((sk_ptr = (SK_P FAR *)ll_next(&sk_list)) != NULL) {
      MEM_UNLOCK(&sk_ptr->ptr->sk_val);
      FREE(&sk_ptr->ptr->sk_val);
   }
   ll_deaccess(&sk_list);
   ll_free(&sk_list);
   return( db_status );
}


/* Create a new empty record
*/
int
d_makenew( nrec TASK_PARM DBN_PARM )
int nrec;
TASK_DECL
DBN_DECL
{
#ifndef	 NO_TIMESTAMP
   ULONG timestamp;
#endif
   DB_ADDR db_addr;
   INT recnum, fld, stat;
   FILE_NO file;
   F_ADDR rec_addr;
   char FAR *ptr;
   SK_P FAR *sk_ptr;
   RECORD_ENTRY FAR *rec_ptr;
   FIELD_ENTRY FAR *fld_ptr;
   int fldtot;

   DB_ENTER(DB_ID TASK_ID LOCK_SET(RECORD_IO));

   if (nrec_check(nrec, &nrec, (RECORD_ENTRY FAR * FAR *)&rec_ptr) != S_OKAY)
      RETURN( db_status );

   recnum = NUM2EXT(nrec, rt_offset);
   if ( rec_ptr->rt_flags & COMKEYED )
      RETURN( dberr(S_COMKEY) );

   /* check for duplicate keys */
   db_addr = curr_rec;
   for (fld = rec_ptr->rt_fields, fldtot = fld + rec_ptr->rt_fdtot,
						   fld_ptr = &field_table[fld];
	fld < fldtot;
	++fld, ++fld_ptr) {
      if ((fld_ptr->fd_key == UNIQUE) && !(fld_ptr->fd_flags & OPTKEYMASK)) {
	 /* locate the key value in the set_key table */
	 ll_access(&sk_list);
	 while (((sk_ptr = (SK_P FAR *)ll_next(&sk_list)) != NULL) &&
		(sk_ptr->ptr->sk_fld != fld))
	    ;				/* NOP */
	 if (sk_ptr == NULL) {
	    ll_deaccess(&sk_list);
	    RETURN( dberr( S_KEYREQD ) );
	 }
	 MEM_LOCK(&sk_ptr->ptr->sk_val);
	 d_keyfind(FLDMARK*(long)recnum + (long)(fld - rec_ptr->rt_fields),
		   sk_ptr->ptr->sk_val.ptr TASK_PARM DBN_PARM);
	 MEM_UNLOCK(&sk_ptr->ptr->sk_val);
	 curr_rec = db_addr;
         ll_deaccess(&sk_list);
	 if ( db_status == S_OKAY ) RETURN( db_status = S_DUPLICATE );
      }
   }
   /* pull out the file number */
   file = rec_ptr->rt_file;
   db_addr = (NUM2EXT(file, ft_offset) & FILEMASK) << FILESHIFT;

   /* select a record pointer to use */
   if ( dio_pzalloc(file, &rec_addr) != S_OKAY )
      RETURN( db_status );
   db_addr |= rec_addr;

   /* read record */
   if ( dio_read( db_addr, (char FAR * FAR *)&ptr, PGHOLD) != S_OKAY )
      RETURN( db_status );

   /* zero fill the record */
   byteset(ptr, 0, rec_ptr->rt_len);

   /* place the record number and db_addr at the start of the record */
   bytecpy( ptr, &recnum, sizeof(INT) );
   bytecpy( ptr+sizeof(INT), &db_addr, DB_ADDR_SIZE);
#ifndef	 NO_TIMESTAMP
   /* check for timestamp */
   if ( rec_ptr->rt_flags & TIMESTAMPED ) {
      timestamp = dio_pzgetts(file);
      bytecpy( ptr + RECCRTIME, &timestamp, sizeof(LONG));
      bytecpy( ptr + RECUPTIME, &timestamp, sizeof(LONG));
   }
   else timestamp = 0L;
#endif
   /* for each keyed field, enter the key value into the key file */
   for (fld = rec_ptr->rt_fields, fldtot = fld + rec_ptr->rt_fdtot,
						   fld_ptr = &field_table[fld];
	fld < fldtot;
	++fld, ++fld_ptr) {
      if ((fld_ptr->fd_key != 'n') && !(fld_ptr->fd_flags & OPTKEYMASK)) {
	 /* locate the key value in the set_key table */
         ll_access(&sk_list);
	 sk_ptr = (SK_P FAR *)ll_first(&sk_list);
	 while (sk_ptr != NULL) {
	    if ( sk_ptr->ptr->sk_fld == fld ) {
	       MEM_LOCK(&sk_ptr->ptr->sk_val);
	       if ((stat = key_insert(fld, sk_ptr->ptr->sk_val.ptr, db_addr)) 
								 != S_OKAY) {
		  dio_write(db_addr, NULL, PGFREE);
		  r_delrec( nrec, db_addr );
	          MEM_UNLOCK(&sk_ptr->ptr->sk_val);
		  ll_deaccess(&sk_list);
		  RETURN( db_status = stat );
	       }
	       if ((fld_ptr->fd_type != CHARACTER) || fld_ptr->fd_dim[0] )
		  bytecpy(ptr+fld_ptr->fd_ptr, sk_ptr->ptr->sk_val.ptr,
			  fld_ptr->fd_len);
	       else
		  strncpy(ptr + fld_ptr->fd_ptr, sk_ptr->ptr->sk_val.ptr,
			  fld_ptr->fd_len);
	       MEM_UNLOCK(&sk_ptr->ptr->sk_val);
	       break;
	    }
	    sk_ptr = (SK_P FAR *)ll_next(&sk_list);
	 }
	 ll_deaccess(&sk_list);
	 if ( sk_ptr == NULL ) RETURN( dberr( S_KEYREQD ) );
      }
   }
   if ( dio_write(db_addr, NULL, PGFREE) == S_OKAY ) {
      curr_rec = db_addr;
#ifndef	 NO_TIMESTAMP
      if ( db_tsrecs )
	 cr_time = timestamp;
#endif
   }
   RETURN( db_status );
}
/* vpp -nOS2 -dUNIX -nBSD -nVANILLA_BSD -nVMS -nMEMLOCK -nWINDOWS -nFAR_ALLOC -f/usr/users/master/config/nonwin makenew.c */
