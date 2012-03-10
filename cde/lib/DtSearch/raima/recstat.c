/* $XConsortium: recstat.c /main/2 1996/05/09 04:15:50 drk $ */
/*
 *   COMPONENT_NAME: austext
 *
 *   FUNCTIONS: d_recstat
 *
 *   ORIGINS: 157
 *
 *   OBJECT CODE ONLY SOURCE MATERIALS
 */
/*-----------------------------------------------------------------------
   recstat.c -- db_VISTA record timestamp status check module.

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
/* Test timestamp status of record
*/
d_recstat(dba, rts TASK_PARM)
DB_ADDR dba;
ULONG   rts;
TASK_DECL
{
   INT rec;
   char FAR *ptr;
   ULONG cts, uts;

   DB_ENTER(NO_DB_ID TASK_ID LOCK_SET(RECORD_IO));

   if ((check_dba(dba) != S_OKAY) || (dio_read(dba, (char FAR * FAR *)&ptr, NOPGHOLD) != S_OKAY))
      RETURN( db_status );

   /* get record id */
   bytecpy(&rec, ptr, sizeof(INT));
   if ( rec >= 0 ) {
      rec &= ~RLBMASK; /* mask off rlb */
#ifndef	 ONE_DB
      rec += curr_db_table->rt_offset;
#endif
      if ( record_table[rec].rt_flags & TIMESTAMPED ) {
	 bytecpy(&cts, ptr + RECCRTIME, sizeof(ULONG));
	 if ( cts > rts )
	    db_status = S_DELETED;
	 else {
	    bytecpy(&uts, ptr + RECUPTIME, sizeof(ULONG));
	    if ( uts > rts )
	       db_status = S_UPDATED;
	 }
      }
      else
	 dberr(S_TIMESTAMP);
   }
   else
      db_status = S_DELETED;

   RETURN( db_status );
}
#endif
/* vpp -nOS2 -dUNIX -nBSD -nVANILLA_BSD -nVMS -nMEMLOCK -nWINDOWS -nFAR_ALLOC -f/usr/users/master/config/nonwin recstat.c */
