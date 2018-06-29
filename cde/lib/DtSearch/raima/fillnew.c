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
/* $XConsortium: fillnew.c /main/3 1996/08/12 12:33:46 cde-ibm $ */
/*
 *   COMPONENT_NAME: austext
 *
 *   FUNCTIONS: d_fillnew
 *
 *   ORIGINS: 157
 *
 *   OBJECT CODE ONLY SOURCE MATERIALS
 */
/*-----------------------------------------------------------------------
   fillnew.c -- db_VISTA (filled) record creation module

   (C) Copyright 1987 by Raima Corp.
-----------------------------------------------------------------------*/

/* ********************** EDIT HISTORY *******************************

 SCR    DATE    INI                   DESCRIPTION
----- --------- --- -----------------------------------------------------
  158 15-JUN-88 RSC inform key_bldcom NOT to complement compound keys
      04-Aug-88 RTK MULTI_TASK changes

*/
#include <stdio.h>
#include "vista.h"
#include "dbtype.h"

#define FALSE 0
#define TRUE 1


/* Create and fill a new record
*/
int
d_fillnew(
int nrec,       /* record number */
const char *recval, /* record value */
int dbn       /* database number */
)
{
   DB_ADDR db_addr;
   INT recnum, stat;
   FILE_NO file;
   F_ADDR rec_addr;
   char *ptr;
   char key[256];
   RECORD_ENTRY *rec_ptr;
   FIELD_ENTRY *fld_ptr;
   int fld, fldtot;

   DB_ENTER(DB_ID TASK_ID LOCK_SET(RECORD_IO));

   if (nrec_check(nrec, &nrec, (RECORD_ENTRY * *)&rec_ptr) != S_OKAY)
      RETURN( db_status );
   recnum = NUM2EXT(nrec, rt_offset);

   /* check for duplicate keys */
   db_addr = curr_rec;
   for (fld = rec_ptr->rt_fields, fldtot = fld + rec_ptr->rt_fdtot,
						   fld_ptr = &field_table[fld];
	(fld < fldtot) || (fld_ptr->fd_type == COMKEY);
	++fld, ++fld_ptr) {
      if ((fld_ptr->fd_key == UNIQUE) && !(fld_ptr->fd_flags & OPTKEYMASK)) {
	 if (fld_ptr->fd_type != COMKEY)
	    ptr = (char *)recval + fld_ptr->fd_ptr - rec_ptr->rt_data;
	 else
	    key_bldcom(fld, (char *)recval, ptr = key, FALSE); /* Don't complement */
	 d_keyfind(FLDMARK*(long)recnum + (fld - rec_ptr->rt_fields), ptr 
		      , dbn);
	 curr_rec = db_addr;
	 if ( db_status == S_OKAY ) 
	    RETURN( db_status = S_DUPLICATE );
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
   if (dio_read(db_addr, (char * *)&ptr, PGHOLD) != S_OKAY)
      RETURN( db_status );

   /* zero fill the record */
   byteset(ptr, 0, rec_ptr->rt_len);

   /* place the record number and db_addr at the start of the record */
   bytecpy( ptr, &recnum, sizeof(INT) );
   bytecpy( ptr+sizeof(INT), &db_addr, DB_ADDR_SIZE );
   /* copy the record value into place */
   bytecpy( ptr+rec_ptr->rt_data, recval, rec_ptr->rt_len-rec_ptr->rt_data );

   /* for each keyed field, enter the key value into the key file */
   for (fld = rec_ptr->rt_fields, fldtot = fld + rec_ptr->rt_fdtot,
						   fld_ptr = &field_table[fld];
	(fld < fldtot) || (fld_ptr->fd_type == COMKEY);
	++fld, ++fld_ptr) {
      if ((fld_ptr->fd_key != 'n') && !(fld_ptr->fd_flags & OPTKEYMASK)) {
	 if ( fld_ptr->fd_type != COMKEY )
	    ptr = (char *)recval + fld_ptr->fd_ptr - rec_ptr->rt_data;
	 else
	    key_bldcom(fld, (char *)recval, ptr = key, TRUE);
	 if ((stat = key_insert(fld, ptr, db_addr)) != S_OKAY) {
	    r_delrec( nrec, db_addr );
	    dio_write(db_addr, NULL, PGFREE);
	    RETURN( db_status = stat );
	 }
      }
   }
   /* 'touch' page, do page's update housekeeping */
   if ( dio_write(db_addr, NULL, PGFREE) == S_OKAY ) {
      curr_rec = db_addr;

   }
   RETURN( db_status );
}
/* vpp -nOS2 -dUNIX -nBSD -nVANILLA_BSD -nVMS -nMEMLOCK -nWINDOWS -nFAR_ALLOC -f/usr/users/master/config/nonwin fillnew.c */
