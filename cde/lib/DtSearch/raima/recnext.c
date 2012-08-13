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
/* $XConsortium: recnext.c /main/2 1996/05/09 04:14:47 drk $ */
/*
 *   COMPONENT_NAME: austext
 *
 *   FUNCTIONS: d_recnext
 *
 *   ORIGINS: 157
 *
 *   OBJECT CODE ONLY SOURCE MATERIALS
 */

/* ********************** EDIT HISTORY *******************************

 SCR    DATE    INI                   DESCRIPTION
----- --------- --- -----------------------------------------------------
  101 21-Jun-88 RSC Use rn_type and rn_dba instead of curr_rec
  103 24-Jun-88 RSC Improve generation of single user version
      04-Aug-88 RTK MULTI_TASK changes
      18-Aug-88 RSC moved rn_type/dba to separate table

*/

#include <stdio.h>
#include "vista.h"
#include "dbtype.h"


/* Find next record of type
*/
int
d_recnext(TASK_DBN_ONLY)
TASK_DECL
DBN_DECL
{
   INT rectype;
   FILE_NO fno;
   FILE_NO ft;
   DB_ADDR dba;
#ifndef SINGLE_USER
   int dbopen_sv;
#endif
   int rec_ndx;			/* Index into record table */
   RECORD_ENTRY FAR *rec_ptr;	/* Pointer to record table */
   char FAR *recptr;
   F_ADDR rno, last;

   DB_ENTER(DB_ID TASK_ID LOCK_SET(RECORD_IO));

   /* look for the current record type */
   if ( RN_REF(rn_type) < 0 ) RETURN( dberr(S_NOTYPE) );

   /* get the record number and file number from the current record */
   if (RN_REF(rn_dba)) {
      fno = (FILE_NO)((RN_REF(rn_dba) >> FILESHIFT) & FILEMASK);
      rno = RN_REF(rn_dba) & ADDRMASK;
   }
   else {			/* No current rec - get fno from rn_type */
      nrec_check(RN_REF(rn_type) + RECMARK, &rec_ndx, (RECORD_ENTRY FAR * FAR *)&rec_ptr);
      fno = (FILE_NO)NUM2EXT(rec_ptr->rt_file, ft_offset);
      fno = (int)((fno >> FILESHIFT) & FILEMASK);
      rno = 1;
   }
   ft = NUM2INT( fno, ft_offset );

   /* start looking at the next record number */
   if ( (last = dio_pznext(ft)) <= 0 )
      RETURN( db_status );

   ++rno;
   do {
	/* make sure we haven't gone past the end of the file */
	if ( rno >= last ) RETURN( db_status = S_NOTFOUND );

	/* create the database address to read */
	dba = ( (FILEMASK & fno) << FILESHIFT ) | (ADDRMASK & rno);

	/* set up to allow unlocked read */
#ifndef SINGLE_USER
	dbopen_sv = dbopen;
	dbopen = 2;
#endif

	/* read the record */
	dio_read( dba, (char FAR * FAR *)&recptr, NOPGHOLD );
#ifndef SINGLE_USER
	dbopen = dbopen_sv;
#endif
	if ( db_status != S_OKAY )
	    RETURN( db_status );

	/* get the record type out of the record */
	bytecpy( &rectype, recptr, sizeof(INT) );
#ifndef SINGLE_USER
	rectype &= ~RLBMASK;
#endif

	++rno;
   } while ( rectype != RN_REF(rn_type) );

   /* set the current record */
   curr_rec = dba;
   RN_REF(rn_type) = rectype;
   RN_REF(rn_dba)  = dba;

   RETURN( db_status = S_OKAY );
}
/* vpp -nOS2 -dUNIX -nBSD -nVANILLA_BSD -nVMS -nMEMLOCK -nWINDOWS -nFAR_ALLOC -f/usr/users/master/config/nonwin recnext.c */
