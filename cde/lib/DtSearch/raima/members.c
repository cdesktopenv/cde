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
/* $XConsortium: members.c /main/2 1996/05/09 04:12:13 drk $ */
/*
 *   COMPONENT_NAME: austext
 *
 *   FUNCTIONS: d_members
 *
 *   ORIGINS: 157
 *
 *   OBJECT CODE ONLY SOURCE MATERIALS
 */
/*-----------------------------------------------------------------------
   members.c -- db_VISTA set member count get module

   (C) Copyright 1987 by Raima Corp.
-----------------------------------------------------------------------*/

/* ********************** EDIT HISTORY *******************************

 SCR    DATE    INI                   DESCRIPTION
----- --------- --- -----------------------------------------------------
      04-Aug-88 RTK MULTI_TASK changes
*/

#include <stdio.h>
#include "vista.h"
#include "dbtype.h"


/* Get number of members of the current set owner
*/
int
d_members(set, tot TASK_PARM DBN_PARM)
int set;     /* Set table entry */
LONG FAR *tot;   /* total members of set */
TASK_DECL
DBN_DECL     /* database number */
{
   char setp[SETPSIZE];
   char FAR *recp;
   LONG total;
   SET_ENTRY FAR *set_ptr;

   DB_ENTER(DB_ID TASK_ID LOCK_SET(SET_IO));

   if (nset_check(set, &set, (SET_ENTRY FAR * FAR *)&set_ptr) != S_OKAY)
      RETURN( db_status );

   /* Make sure we have a current owner */
   if ( ! curr_own[set] )
      RETURN( dberr(S_NOCO) );

   /* Read owner record */
   if ( dio_read( curr_own[set], (char FAR * FAR *)&recp, NOPGHOLD) != S_OKAY )
      RETURN( db_status );

   /* Get set pointer from owner record */
   if ( r_gset(set, recp, setp) != S_OKAY )
      RETURN( (db_status == S_INVOWN) ? dberr(S_SYSERR) : db_status );

   /* Copy total members from owner pointer */
   bytecpy(&total, setp+SP_MEMBERS, sizeof(LONG));
   *tot = total;

   RETURN( db_status = S_OKAY );
}
/* vpp -nOS2 -dUNIX -nBSD -nVANILLA_BSD -nVMS -nMEMLOCK -nWINDOWS -nFAR_ALLOC -f/usr/users/master/config/nonwin members.c */
