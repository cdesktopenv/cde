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
/* $XConsortium: options.c /main/2 1996/05/09 04:13:11 drk $ */
/*
 *   COMPONENT_NAME: austext
 *
 *   FUNCTIONS: d_off_opt
 *		d_on_opt
 *
 *   ORIGINS: 157
 *
 *   OBJECT CODE ONLY SOURCE MATERIALS
 */
/*-----------------------------------------------------------------------
   options.c -- db_VISTA options module.

   (C) Copyright 1987 by Raima Corporation.
-----------------------------------------------------------------------*/
#include <stdio.h>
#include "vista.h"
#include "dbtype.h"


/* Turn on db_VISTA runtime options
*/
int
d_on_opt(optflag TASK_PARM)
int optflag;
TASK_DECL
{
   int i;

   DB_ENTER(NO_DB_ID TASK_ID LOCK_SET(LOCK_ALL));
   dboptions |= optflag;

#ifndef NO_TRANS
   if ( optflag & ARCLOGGING ) d_tron();
#endif

#ifndef NO_COUNTRY
   if ( optflag & IGNORECASE ) {
      if ( !db_global.ctbl_activ) {
	 if (ctbl_alloc() != S_OKAY )
	    RETURN (db_status);
	 db_global.ctbl_activ = TRUE;
      }
      for ( i = 97; i < 123; i++)
	 /* map lower to upper */
	 db_global.country_tbl.ptr[i].sort_as1 = (char)(i-32);
   }
#endif

   RETURN( db_status = S_OKAY );
}


/* Turn off db_VISTA runtime options
*/
int
d_off_opt(optflag TASK_PARM)
int optflag;
TASK_DECL
{
   int i;

   DB_ENTER(NO_DB_ID TASK_ID LOCK_SET(LOCK_ALL));
   dboptions &= ~optflag;

#ifndef NO_TRANS
   if ( optflag & ARCLOGGING ) d_troff();
#endif

#ifndef NO_COUNTRY
   if ( optflag & IGNORECASE ) {
      if ( db_global.ctbl_activ) {
	 for ( i = 97; i < 123; i++)
	    /* restore lower to lower */
	    db_global.country_tbl.ptr[i].sort_as1 = (char)i;
      }
   }
#endif

   RETURN( db_status = S_OKAY );
}
/* vpp -nOS2 -dUNIX -nBSD -nVANILLA_BSD -nVMS -nMEMLOCK -nWINDOWS -nFAR_ALLOC -f/usr/users/master/config/nonwin options.c */
