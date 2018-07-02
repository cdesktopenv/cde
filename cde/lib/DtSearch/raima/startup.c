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
/* $XConsortium: startup.c /main/2 1996/05/09 04:19:11 drk $ */
/*
 *   COMPONENT_NAME: austext
 *
 *   FUNCTIONS: startup
 *
 *   ORIGINS: 157
 *
 *   OBJECT CODE ONLY SOURCE MATERIALS
 */

/* ********************** EDIT HISTORY *******************************

 SCR    DATE    INI                   DESCRIPTION
----- --------- --- -----------------------------------------------------
  532 06-Jan-89 RSC Fixed so will link correctly for ONE_DB
      05-May-89 WLW Added ".v" tag to Currtask for MULTI_TASKing
*/

#include <stdio.h>
#include "vista.h"
#include "dbtype.h"


/* Called once at the beginning of each external function
*/
int startup(int dbn)
{
   db_status = S_OKAY;
   if ( ! db_glob_init ) {
      taskinit(&db_global);
      db_glob_init = 1;
   }

   return( db_status );
}
/* vpp -nOS2 -dUNIX -nBSD -nVANILLA_BSD -nVMS -nMEMLOCK -nWINDOWS -nFAR_ALLOC -f/usr/users/master/config/nonwin startup.c */
