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
/* $XConsortium: recset.c /main/2 1996/05/09 04:15:34 drk $ */
/*
 *   COMPONENT_NAME: austext
 *
 *   FUNCTIONS: d_recset
 *
 *   ORIGINS: 157
 *
 *   OBJECT CODE ONLY SOURCE MATERIALS
 */
/*-----------------------------------------------------------------------

   recset.c - set record type and database address to current.

   This function sets the current record type and database address to the
   current record.  It is used in conjunction with d_recnext to then scan
   forward from this point on.

   AUTHOR:  R.S. Carlson
   DATE:    21-Jun-88
   PROJECT: db_VISTA 3.10 enhancements

   Copyright (C) 1988 by Raima Corporation

-----------------------------------------------------------------------*/

/* ********************** EDIT HISTORY *******************************

 SCR    DATE    INI                   DESCRIPTION
----- --------- --- -----------------------------------------------------
      04-Aug-88 RTK MULTI_TASK changes
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
   set record type and database address to current
*/
int d_recset(
int rec,			/* record type */
int dbn
)
{
/*
   RETURNS: db_status return code.
   ASSUMES: nothing.
*/
   FILE_NO rfile;		/* file containing user specified rec */
   FILE_NO fno;			/* file containing current record */
   int rec_ndx;			/* Index into record table */
   RECORD_ENTRY *rec_ptr;	/* Pointer to record table */

   DB_ENTER(DB_ID TASK_ID LOCK_SET(RECORD_NOIO));

   /* Check rec parameter user passed */
   if (nrec_check(rec, &rec_ndx, (RECORD_ENTRY * *)&rec_ptr) != S_OKAY)
      RETURN( db_status );
   
   /* Check to make sure current record is in this file */
   rfile = (FILE_NO)(NUM2EXT(rec_ptr->rt_file, ft_offset));
   fno = (FILE_NO)((curr_rec >> FILESHIFT) & FILEMASK);
   if (fno != rfile)
      RETURN( dberr (S_INVREC) );
   
   /* Everything is okay - save the type and database address */
   RN_REF(rn_type) = rec - RECMARK;
   RN_REF(rn_dba)  = curr_rec;
   RETURN( db_status = S_OKAY );
}
/* vpp -nOS2 -dUNIX -nBSD -nVANILLA_BSD -nVMS -nMEMLOCK -nWINDOWS -nFAR_ALLOC -f/usr/users/master/config/nonwin recset.c */
