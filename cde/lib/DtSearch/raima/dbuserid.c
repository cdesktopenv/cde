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
/* $XConsortium: dbuserid.c /main/2 1996/05/09 04:03:46 drk $ */
/*
 *   COMPONENT_NAME: austext
 *
 *   FUNCTIONS: d_dbuserid
 *
 *   ORIGINS: 157
 *
 *   OBJECT CODE ONLY SOURCE MATERIALS
 */
/*-----------------------------------------------------------------------
   dbuserid.c -- db_VISTA user identifier set module.

   (C) Copyright 1987 by Raima Corporation.
-----------------------------------------------------------------------*/

/* ********************** EDIT HISTORY *******************************

 SCR    DATE    INI                   DESCRIPTION
----- --------- --- -----------------------------------------------------
      04-Aug-88 RTK MULTI_TASK changes
  418 05-Oct-88 RSC also allow underscores in dbuserid
*/

#include <stdio.h>
#include <ctype.h>
#include "vista.h"
#include "dbtype.h"

/* Set Database User Identifier
*/
int
d_dbuserid(id TASK_PARM)
CONST char FAR *id;
TASK_DECL
{
   CONST char FAR *chk_id;

   DB_ENTER(NO_DB_ID TASK_ID LOCK_SET(LOCK_NONE));

   for (chk_id = id; *chk_id; chk_id++) {
      if (isascii(*chk_id)) {
	 if (!isalnum(*chk_id)) {
	    if (*chk_id != '_') RETURN ( dberr(S_BADUSERID));
	 }
      }
      else RETURN ( dberr(S_BADUSERID));
   }

   if (dbopen)
      dberr(S_DBOPEN);
   else {
      strncpy(dbuserid, id, FILENMLEN - 1);
      dbuserid[FILENMLEN - 1] = '\0';
      db_status = S_OKAY;
   }
   RETURN( db_status );
}
/* vpp -nOS2 -dUNIX -nBSD -nVANILLA_BSD -nVMS -nMEMLOCK -nWINDOWS -nFAR_ALLOC -f/usr/users/master/config/nonwin dbuserid.c */
