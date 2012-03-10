/* $XConsortium: cmstat.c /main/2 1996/05/09 03:56:07 drk $ */
/*
 *   COMPONENT_NAME: austext
 *
 *   FUNCTIONS: d_cmstat
 *
 *   ORIGINS: 157
 *
 *   OBJECT CODE ONLY SOURCE MATERIALS
 */
/*-----------------------------------------------------------------------
   cmstat.c -- db_VISTA member timestamp status check module.

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
/* Test timestamp status of current member
*/
d_cmstat(set TASK_PARM DBN_PARM)
int set;
TASK_DECL
DBN_DECL
{
   ULONG cts, uts;
   ULONG cmts;

   DB_ENTER(DB_ID TASK_ID LOCK_SET(SET_IO));
   if ( d_ctscm(set, &cts TASK_PARM DBN_PARM) == S_OKAY ) {
      if ( cts ) {
	 cmts = cm_time[NUM2INT(set - SETMARK, st_offset)];
	 if ( cts > cmts )
	    db_status = S_DELETED;
	 else {
	    d_utscm(set, &uts TASK_PARM DBN_PARM);
	    if ( uts > cmts )
	       db_status = S_UPDATED;
	 }
      }
      else
	 dberr(S_TIMESTAMP);
   }
   RETURN( db_status );
}
#endif
/* vpp -nOS2 -dUNIX -nBSD -nVANILLA_BSD -nVMS -nMEMLOCK -nWINDOWS -nFAR_ALLOC -f/usr/users/master/config/nonwin cmstat.c */
