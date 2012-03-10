/* $XConsortium: csstat.c /main/2 1996/05/09 04:00:57 drk $ */
/*
 *   COMPONENT_NAME: austext
 *
 *   FUNCTIONS: d_csstat
 *
 *   ORIGINS: 157
 *
 *   OBJECT CODE ONLY SOURCE MATERIALS
 */
/*-----------------------------------------------------------------------
   csstat.c -- db_VISTA current set timestamp status check module.

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
/* Test timestamp status of current set
*/
d_csstat(set TASK_PARM DBN_PARM)
int set;
TASK_DECL
DBN_DECL
{
   ULONG uts;

   DB_ENTER(DB_ID TASK_ID LOCK_SET(SET_IO));

   if ( d_utscs(set, &uts TASK_PARM DBN_PARM) == S_OKAY ) {
      if ( uts ) {
	 if ( uts > cs_time[NUM2INT(set - SETMARK, st_offset)] )
	    db_status = S_UPDATED;
      }
      else
	 dberr(S_TIMESTAMP);
   }
   RETURN( db_status );
}
#endif
/* vpp -nOS2 -dUNIX -nBSD -nVANILLA_BSD -nVMS -nMEMLOCK -nWINDOWS -nFAR_ALLOC -f/usr/users/master/config/nonwin csstat.c */
