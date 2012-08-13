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
/* $XConsortium: keyexist.c /main/2 1996/05/09 04:09:12 drk $ */
/*
 *   COMPONENT_NAME: austext
 *
 *   FUNCTIONS: d_keyexist
 *
 *   ORIGINS: 157
 *
 *   OBJECT CODE ONLY SOURCE MATERIALS
 */

/* ********************** EDIT HISTORY *******************************

 SCR    DATE    INI                   DESCRIPTION
----- --------- --- -----------------------------------------------------
      04-Aug-88 RTK MULTI_TASK changes
*/

#include <stdio.h>
#include "vista.h"
#include "dbtype.h"


/* Check for optional key existence
*/
int
d_keyexist(field TASK_PARM DBN_PARM)
long field; /* key field constant */
TASK_DECL
DBN_DECL    /* database number */
{
   int fld;        /* field number */
   int rec, rn;    /* record type of current record */
   char FAR *rptr;     /* pointer to current record */
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
   if ( (stat = dio_read(curr_rec, (char FAR * FAR *)&rptr, NOPGHOLD)) == S_OKAY )  {
      /* Check the bit map and return S_OKAY if already stored,
	 else S_NOTFOUND */
      if ((stat = r_tstopt(fld_ptr, rptr)) == S_OKAY)
	 stat = S_NOTFOUND;
      else if (stat == S_DUPLICATE)
	 stat = S_OKAY;
   }
   RETURN( db_status = stat );
}
/* vpp -nOS2 -dUNIX -nBSD -nVANILLA_BSD -nVMS -nMEMLOCK -nWINDOWS -nFAR_ALLOC -f/usr/users/master/config/nonwin keyexist.c */
