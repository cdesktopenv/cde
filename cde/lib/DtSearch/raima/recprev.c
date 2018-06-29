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
/* $XConsortium: recprev.c /main/2 1996/05/09 04:15:00 drk $ */
/*
 *   COMPONENT_NAME: austext
 *
 *   FUNCTIONS: d_recprev
 *
 *   ORIGINS: 157
 *
 *   OBJECT CODE ONLY SOURCE MATERIALS
 */
/*-----------------------------------------------------------------------

   recprev.c - scan database backwards via database address

   d_recprev is used to scan the database backwards via the database address.
   It is the compliment to recnext.  It is generally called after d_reclast.

   AUTHOR:  R.S. Carlson
   DATE:    06-Jul-88
   PROJECT: db_VISTA 3.10

   Copyright (C) 1988 by Raima Corporation

-----------------------------------------------------------------------*/

/* ********************** EDIT HISTORY *******************************

 SCR    DATE    INI                   DESCRIPTION
----- --------- --- -----------------------------------------------------
  351 06-Jul-88 RSC added new function to runtime
      04-Aug-88 RTK MULTI_TASK changes
      18-Aug-88 RSC moved rn_type/dba to separate table.
      17-Oct-88 RSC changed return to RETURN

*/


/* ********************** INCLUDE FILES ****************************** */

#include <stdio.h>
#include "vista.h"
#include "dbtype.h"

/* ********************** GLOBAL VARIABLE DECLARATIONS *************** */
/* ********************** GLOBAL FUNCTION DECLARATIONS *************** */
/* ********************** EXTERNAL VARIABLE DECLARATIONS ************* */
/* ********************** EXTERNAL FUNCTION DECLARATIONS ************* */
/* ********************** LOCAL VARIABLE DECLARATIONS **************** */
/* ********************** LOCAL FUNCTION DECLARATIONS **************** */

/* ======================================================================
   d_recprev - find previous record via database address
*/
int d_recprev(int dbn)
{
/*
   RETURNS: db_status.  Also sets current record to previous matched record.
   ASSUMES: nothing.
*/
   DB_ADDR dba;			/* current database addr we're scanning */
   FILE_NO fno;			/* current file we're scanning */
   F_ADDR last;			/* last slot in file */
   int rec_ndx;			/* index of RECORD ENTRY (not used) */
   char *recptr;		/* record from database */
   RECORD_ENTRY *rec_ptr;	/* RECORD ENTRY for this record */
   INT rectype;			/* record type from record */
   F_ADDR rno;			/* current slot we're scanning */
   FILE_NO ft;			/* normalized file */


   DB_ENTER(DB_ID TASK_ID LOCK_SET(RECORD_IO));

   /* setup current record and file number */
   if ( RN_REF(rn_type) < 0) RETURN( dberr(S_NOTYPE) );
   if ( RN_REF(rn_dba) ) {
      fno = (FILE_NO)((RN_REF(rn_dba) >> FILESHIFT) & FILEMASK);
      rno = RN_REF(rn_dba) & ADDRMASK;
   }
   else {		/* no current rec, get fno from rn_type */
      nrec_check(RN_REF(rn_type) + RECMARK, &rec_ndx, (RECORD_ENTRY * *)&rec_ptr);
      fno = NUM2EXT(rec_ptr->rt_file, ft_offset);
      fno = (int)((fno >> FILESHIFT) & FILEMASK);

      /* compute rno as last slot in file */
      if ( (last = dio_pznext(rec_ptr->rt_file)) <= 0 )
         RETURN( db_status );

      rno = last;
   }
   ft = NUM2INT( fno, ft_offset );
   
   /* scan backwards looking for a record of the same type */
   rno--;
   do {
      if (rno < 1) RETURN( db_status = S_NOTFOUND );
      dba = ((fno & FILEMASK) << FILESHIFT) | (rno & ADDRMASK);
      dio_read(dba, (char * *)&recptr, NOPGHOLD);
      if (db_status != S_OKAY) RETURN( db_status );

      /* see if we've found a match */
      bytecpy(&rectype, recptr, sizeof(INT));
      rno--;
   } while ( rectype != RN_REF(rn_type) );

   /* when we get here, we know a match was found */
   curr_rec = dba;		/* set current record to match */
   RN_REF(rn_type) = rectype;
   RN_REF(rn_dba) = dba;
   RETURN( db_status = S_OKAY );
}
/* vpp -nOS2 -dUNIX -nBSD -nVANILLA_BSD -nVMS -nMEMLOCK -nWINDOWS -nFAR_ALLOC -f/usr/users/master/config/nonwin recprev.c */
