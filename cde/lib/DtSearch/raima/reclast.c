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
/* $XConsortium: reclast.c /main/2 1996/05/09 04:14:30 drk $ */
/*
 *   COMPONENT_NAME: austext
 *
 *   FUNCTIONS: d_reclast
 *
 *   ORIGINS: 157
 *
 *   OBJECT CODE ONLY SOURCE MATERIALS
 */
/*-----------------------------------------------------------------------

   reclast.c - find last record occurrence in database

   reclast is used to setup a scan of a database by database number
   backwards, and is complementary to recfrst.

   AUTHOR:  R.S. Carlson
   DATE:    06-Jul-88
   PROJECT: db_VISTA 3.10

   Copyright (C) 1988 by Raima Corporation

-----------------------------------------------------------------------*/

/* ********************** EDIT HISTORY *******************************

 SCR    DATE    INI                   DESCRIPTION
----- --------- --- -----------------------------------------------------
  351 06-Jul-88 RSC new function added to runtime
      04-Aug-88 RTK MULTI_TASK changes
      09-Aug-88 RSC rno-- needs to be outside ifndef SINGLE_USER
      18-Aug-88 RSC moved rn_type/dba to separate table

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
   d_reclast - find last record occurrence in database
*/
int d_reclast(
int rec,			/* record # to find last occurrence of */
int dbn				/* optional database number */
)
{
/*
   RETURNS: db_status.  Sets current record to last record, if found.
   ASSUMES: nothing.
*/
   DB_ADDR dba;			/* current database addr we're scanning */
   FILE_NO ftype;		/* file desc for file holding rec */
   F_ADDR last;			/* last slot in file */
   char *recptr;		/* record from database */
   RECORD_ENTRY *rec_ptr;	/* RECORD ENTRY for this record */
   INT rectype;			/* record type from record */
   F_ADDR rno;			/* current slot we're scanning */


   DB_ENTER(DB_ID TASK_ID LOCK_SET(RECORD_IO));

   /* validate and convert record number */
   if ( nrec_check(rec, &rec, (RECORD_ENTRY * *)&rec_ptr) != S_OKAY)
      RETURN( db_status );

   /* get the last record # for this file */
   ftype = NUM2EXT(rec_ptr->rt_file, ft_offset);
   if ( (last = dio_pznext(rec_ptr->rt_file)) <= 0 )
      RETURN( db_status );

   /* start at the end, working backwards, find a matching record */
   rno = last - 1;
   do {
      if ( rno < 1)
	 RETURN ( db_status = S_NOTFOUND );
      
      /* create the database address, and read this record */
      dba = ((FILEMASK & ftype) << FILESHIFT) | (ADDRMASK & rno);
      dio_read(dba, (char * *)&recptr, NOPGHOLD);
      if ( db_status != S_OKAY )
	 RETURN( db_status );
      
      /* See if this record is of the type we're looking for */
      bytecpy(&rectype, recptr, sizeof(INT));
      rno--;
   } while ( (int)rectype != rec );

   /* when we get here, we know a match was found */
   curr_rec = dba;			/* set current record */
   RN_REF(rn_type) = rectype;		/* setup for future recprev,recnext */
   RN_REF(rn_dba) = dba;
   RETURN( db_status = S_OKAY );
}
/* vpp -nOS2 -dUNIX -nBSD -nVANILLA_BSD -nVMS -nMEMLOCK -nWINDOWS -nFAR_ALLOC -f/usr/users/master/config/nonwin reclast.c */
