/* $XConsortium: crstat.c /main/2 1996/05/09 03:58:12 drk $ */
/*
 *   COMPONENT_NAME: austext
 *
 *   FUNCTIONS: d_crstat
 *
 *   ORIGINS: 157
 *
 *   OBJECT CODE ONLY SOURCE MATERIALS
 */
/*-----------------------------------------------------------------------
   crstat.c -- db_VISTA current record timestamp status check module.

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


#ifndef	 NO_TIMESTAMP
/* Test timestamp status of current record
*/
d_crstat(TASK_ONLY)
TASK_DECL
{
   ULONG cts, uts;

   DB_ENTER(NO_DB_ID TASK_ID LOCK_SET(RECORD_IO));

   if ( d_ctscr(&cts TASK_PARM) == S_OKAY ) {
      if ( cts ) {
	 d_utscr(&uts TASK_PARM);
	 if ( cts > cr_time )
	    db_status = S_DELETED;
	 else if ( uts > cr_time )
	    db_status = S_UPDATED;
      }
      else
	 dberr(S_TIMESTAMP);
   }
   RETURN( db_status );
}
#endif
/* vpp -nOS2 -dUNIX -nBSD -nVANILLA_BSD -nVMS -nMEMLOCK -nWINDOWS -nFAR_ALLOC -f/usr/users/master/config/nonwin crstat.c */
