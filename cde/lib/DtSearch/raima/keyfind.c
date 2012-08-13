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
/* $XConsortium: keyfind.c /main/2 1996/05/09 04:09:43 drk $ */
/*
 *   COMPONENT_NAME: austext
 *
 *   FUNCTIONS: Pi
 *		chk_desc_key
 *		d_keyfind
 *
 *   ORIGINS: 157
 *
 *   OBJECT CODE ONLY SOURCE MATERIALS
 */
/*-----------------------------------------------------------------------
   keyfind.c -- db_VISTA key find module.

   (C) Copyright 1987 by Raima Corporation.
-----------------------------------------------------------------------*/

/* ********************** EDIT HISTORY *******************************

 SCR    DATE    INI                   DESCRIPTION
----- --------- --- -----------------------------------------------------
      04-Aug-88 RTK MULTI_TASK changes
  310 10-Aug-88 RSC Cleanup function prototype.
*/

#include <stdio.h>
#include "vista.h"
#include "dbtype.h"

/* Internal function prototypes */
static void chk_desc_key(P1(int) Pi(FIELD_ENTRY FAR *) 
					Pi(CONST char FAR *) Pi(char FAR *));

/* Find record thru key field
*/
int
d_keyfind(field, fldval TASK_PARM DBN_PARM)
long  field;  /* field constant */
CONST char FAR *fldval; /* value of the data field */
TASK_DECL
DBN_DECL      /* database number */
{
   int fld, rec;
   DB_ADDR dba;
   RECORD_ENTRY FAR *rec_ptr;
   FIELD_ENTRY FAR *fld_ptr;
   char ckey[256];

   DB_ENTER(DB_ID TASK_ID LOCK_SET(RECORD_IO));

   if ((nfld_check(field, &rec, &fld, (RECORD_ENTRY FAR * FAR *)&rec_ptr, (FIELD_ENTRY FAR * FAR *)&fld_ptr) != S_OKAY) ||
       /* initialize key function operation */
       (key_init(fld) != S_OKAY))
      RETURN( db_status );

   if ( fldval == NULL )
      key_boundary(KEYFIND, &dba);
   else {
      /* locate record with specified key */
      if (fld_ptr->fd_type == 'k') {
	 chk_desc_key(fld, fld_ptr, fldval, ckey);
	 fldval = ckey;
      }
      dba = NULL_DBA;
      if ( key_locpos( fldval, &dba ) != S_OKAY )
	 RETURN( db_status );
      
      /* set current record to found db addr */
      curr_rec = dba;
   }
#ifndef	 NO_TIMESTAMP
   /* set timestamp */
   if ( curr_rec && db_tsrecs )
      d_utscr( &cr_time TASK_PARM );
#endif
   RETURN( db_status = S_OKAY );
}


/* Check compound key value for descending fields
*/
static void chk_desc_key(fld, fld_ptr, fldval, ckey)
int fld;
FIELD_ENTRY FAR *fld_ptr;
CONST char FAR *fldval;
char FAR *ckey;
{
   register int kt_lc;			/* loop control */
#ifndef	 NO_FLOAT
   float fv;
   double dv;
#endif
   char FAR *fptr;
   char FAR *tptr;
   FIELD_ENTRY FAR *kfld_ptr;
   register KEY_ENTRY FAR *key_ptr;

   /* complement descending compound key values */
   for (kt_lc = size_kt - fld_ptr->fd_ptr,
					key_ptr = &key_table[fld_ptr->fd_ptr];
	(--kt_lc >= 0) && (key_ptr->kt_key == fld); ++key_ptr) {
      kfld_ptr = &field_table[key_ptr->kt_field];
      fptr = (char FAR *)fldval + key_ptr->kt_ptr;
      tptr = ckey + key_ptr->kt_ptr;
      if ( key_ptr->kt_sort == 'd' ) {
	 switch ( kfld_ptr->fd_type ) {
#ifndef	 NO_FLOAT
	    case FLOAT:
	       bytecpy(&fv, fptr, sizeof(float));
	       fv = (float)0.0 - fv;
	       bytecpy(fptr, &fv, sizeof(float));
	       break;
	    case DOUBLE:
	       bytecpy(&dv, fptr, sizeof(double));
	       dv = 0.0 - dv;
	       bytecpy(fptr, &dv, sizeof(double));
	       break;
#endif
	    case CHARACTER:
	       key_cmpcpy(tptr, fptr, kfld_ptr->fd_len);
	       if ( kfld_ptr->fd_dim[0] > 1 && kfld_ptr->fd_dim[1] == 0 ) {
		  /* make sure a null byte is at the end */
		  tptr[kfld_ptr->fd_len-1] = '\0';
	       }
	       break;
	    default:
	       key_cmpcpy(tptr, fptr, kfld_ptr->fd_len);
	 }
      }
      else
	 bytecpy(tptr, fptr, kfld_ptr->fd_len);
   }
}
/* vpp -nOS2 -dUNIX -nBSD -nVANILLA_BSD -nVMS -nMEMLOCK -nWINDOWS -nFAR_ALLOC -f/usr/users/master/config/nonwin keyfind.c */
