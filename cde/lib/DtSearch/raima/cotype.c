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
/* $XConsortium: cotype.c /main/2 1996/05/09 03:57:15 drk $ */
/*
 *   COMPONENT_NAME: austext
 *
 *   FUNCTIONS: d_cotype
 *
 *   ORIGINS: 157
 *
 *   OBJECT CODE ONLY SOURCE MATERIALS
 */
/*-----------------------------------------------------------------------
   cotype.c -- db_VISTA owner type module.

   (C) Copyright 1987 by Raima Corporation.
-----------------------------------------------------------------------*/

/* ********************** EDIT HISTORY *******************************

 SCR    DATE    INI                   DESCRIPTION
----- --------- --- -----------------------------------------------------
      04-Aug-88 RTK MULTI_TASK changes
*/

#include <stdio.h>
#include "vista.h"
#include "dbtype.h"


/* Get current owner type
*/
int
d_cotype(set, cotype TASK_PARM DBN_PARM)
int set;
int FAR *cotype;
TASK_DECL
DBN_DECL
{
   char FAR *orec;
   INT crt;
#ifndef SINGLE_USER
   int dbopen_sv;
#endif
   SET_ENTRY FAR *set_ptr;

   DB_ENTER(DB_ID TASK_ID LOCK_SET(SET_IO));

   if (nset_check(set, &set, (SET_ENTRY FAR * FAR *)&set_ptr) != S_OKAY)
      RETURN( db_status );

   if ( ! curr_own[set] )
      RETURN( dberr( S_NOCO ) );

   /* set up to allow unlocked read */
#ifndef SINGLE_USER
   dbopen_sv = dbopen;
   dbopen = 2;
#endif

   /* Read current owner */
   dio_read(curr_own[set], (char FAR * FAR *)&orec, NOPGHOLD);
#ifndef SINGLE_USER
   dbopen = dbopen_sv;
#endif
   if (db_status != S_OKAY)
      RETURN( db_status );

   /* Fetch record type from record header */
   bytecpy(&crt, orec, sizeof(INT));
   crt &= ~RLBMASK; /* mask off rlb */
   *cotype = (int)crt + RECMARK;

   RETURN( db_status );
}
/* vpp -nOS2 -dUNIX -nBSD -nVANILLA_BSD -nVMS -nMEMLOCK -nWINDOWS -nFAR_ALLOC -f/usr/users/master/config/nonwin cotype.c */
