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
/* $XConsortium: recread.c /main/2 1996/05/09 04:15:18 drk $ */
/*
 *   COMPONENT_NAME: austext
 *
 *   FUNCTIONS: d_recread
 *
 *   ORIGINS: 157
 *
 *   OBJECT CODE ONLY SOURCE MATERIALS
 */
/*-----------------------------------------------------------------------
   recread.c -- db_VISTA current record read module.

   (C) Copyright 1987 by Raima Corporation.
-----------------------------------------------------------------------*/

/* ********************** EDIT HISTORY *******************************

 SCR    DATE    INI                   DESCRIPTION
----- --------- --- -----------------------------------------------------
  103 27-Jun-88 RSC Improve generation of single user version
      04-Aug-88 RTK MULTI_TASK changes
*/

#include <stdio.h>
#include "vista.h"
#include "dbtype.h"


/* Read contents of current record
*/
int
d_recread(rec TASK_PARM DBN_PARM)
char FAR *rec; /* ptr to record area */
TASK_DECL
DBN_DECL
{
   INT  rt;     /* record type */
   DB_ADDR dba;
#ifndef SINGLE_USER
   int dbopen_sv;
#endif
   RECORD_ENTRY FAR *rec_ptr;

   DB_ENTER(DB_ID TASK_ID LOCK_SET(RECORD_IO));

   if ( ! dbopen ) RETURN( dberr(S_DBOPEN) );

   /* Make sure we have a current record */
   if ( ! curr_rec )
      RETURN( dberr(S_NOCR) );

   /* set up to allow unlocked read access */
#ifndef SINGLE_USER
   dbopen_sv = dbopen;
   dbopen = 2;
#endif

   /* read current record */
   dio_read( curr_rec, (char FAR * FAR *)&crloc, NOPGHOLD);
#ifndef SINGLE_USER
   dbopen = dbopen_sv;
#endif
   if ( db_status != S_OKAY )
      RETURN( db_status );

   /* copy record type from record */
   bytecpy(&rt, crloc, sizeof(INT));
   if ( rt < 0 )
      RETURN( db_status = S_DELETED );

#ifndef SINGLE_USER
   if ( rt & RLBMASK ) {
      rt &= ~RLBMASK; /* mask off rlb */
      rlb_status = S_LOCKED;
   }
   else {
      rlb_status = S_UNLOCKED;
   }
#endif
   rec_ptr = &record_table[NUM2INT(rt, rt_offset)];

   /* Copy db_addr from record and check with curr_rec */
   bytecpy(&dba, crloc+sizeof(INT), DB_ADDR_SIZE);
   if ( ADDRcmp(&dba, &curr_rec) != 0 )
      RETURN( dberr(S_INVADDR) );

   /* Copy data from crloc into rec */
   bytecpy(rec, &crloc[rec_ptr->rt_data], rec_ptr->rt_len - rec_ptr->rt_data);

   RETURN( db_status = S_OKAY );
}
/* vpp -nOS2 -dUNIX -nBSD -nVANILLA_BSD -nVMS -nMEMLOCK -nWINDOWS -nFAR_ALLOC -f/usr/users/master/config/nonwin recread.c */
