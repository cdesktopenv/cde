/* $XConsortium: gtscr.c /main/2 1996/05/09 04:07:42 drk $ */
/*
 *   COMPONENT_NAME: austext
 *
 *   FUNCTIONS: d_gtscr
 *
 *   ORIGINS: 157
 *
 *   OBJECT CODE ONLY SOURCE MATERIALS
 */
/*-----------------------------------------------------------------------
   gtscr.c -- db_VISTA current record timestamp get module.

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
/* Get timestamp of current record
*/
d_gtscr(timestamp TASK_PARM)
ULONG FAR *timestamp;
TASK_DECL
{
   DB_ENTER(NO_DB_ID TASK_ID LOCK_SET(RECORD_NOIO));

   db_status = S_OKAY;

   /* make sure we have a current record */
   if ( curr_rec ) {
      if ( db_tsrecs )
	 *timestamp = cr_time;
      else
	 dberr(S_TIMESTAMP);
   }
   else
      dberr(S_NOCR);

   RETURN( db_status );
}
#endif
/* vpp -nOS2 -dUNIX -nBSD -nVANILLA_BSD -nVMS -nMEMLOCK -nWINDOWS -nFAR_ALLOC -f/usr/users/master/config/nonwin gtscr.c */
