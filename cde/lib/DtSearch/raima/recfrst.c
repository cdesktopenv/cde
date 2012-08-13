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
/* $XConsortium: recfrst.c /main/2 1996/05/09 04:14:13 drk $ */
/*
 *   COMPONENT_NAME: austext
 *
 *   FUNCTIONS: d_recfrst
 *
 *   ORIGINS: 157
 *
 *   OBJECT CODE ONLY SOURCE MATERIALS
 */
/* ********************** EDIT HISTORY *******************************

 SCR    DATE    INI                   DESCRIPTION
----- --------- --- -----------------------------------------------------
  101 21-Jun-88 RSC Saved type and computed dba into rn_type and rn_dba
  103 24-Jun-88 RSC Improve generation of single user version
      04-Aug-88 RTK MULTI_TASK changes
      18-Aug-88 RSC moved rn_type/dba to separate table

*/

#include <stdio.h>
#include "vista.h"
#include "dbtype.h"


/* Find first record of type
*/
int
d_recfrst( rec TASK_PARM DBN_PARM )
int rec;	/* record type/table entry */
TASK_DECL
DBN_DECL
{
   INT rectype;
   FILE_NO ftype;
   DB_ADDR dba;
   char FAR *recptr;
#ifndef SINGLE_USER
   int dbopen_sv;
#endif
   F_ADDR rno, last;
   RECORD_ENTRY FAR *rec_ptr;

   DB_ENTER(DB_ID TASK_ID LOCK_SET(RECORD_IO));

   if (nrec_check(rec, &rec, (RECORD_ENTRY FAR * FAR *)&rec_ptr) != S_OKAY)
      RETURN( db_status );
#ifndef	 ONE_DB
   rec -= curr_db_table->rt_offset;
#endif

   /* get the normalized number of file containing this record type */
   ftype = NUM2EXT(rec_ptr->rt_file, ft_offset);

   if ( (last = dio_pznext(rec_ptr->rt_file)) <= 0 )
      RETURN( db_status );

   rno = 1;
   do {
      /* make sure we haven't gone past the end of the file */
      if ( rno >= last ) RETURN( db_status = S_NOTFOUND );

      /* create the database address to read */
      dba = ( (FILEMASK & ftype) << FILESHIFT ) | (ADDRMASK & rno);

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
   } while ( (int)rectype != rec );

   /* set the current record and type */
   curr_rec = dba;
   RN_REF(rn_type) = rectype;
   RN_REF(rn_dba) = dba;

   RETURN( db_status = S_OKAY );
}
/* vpp -nOS2 -dUNIX -nBSD -nVANILLA_BSD -nVMS -nMEMLOCK -nWINDOWS -nFAR_ALLOC -f/usr/users/master/config/nonwin recfrst.c */
