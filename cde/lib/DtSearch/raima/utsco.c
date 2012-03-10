/* $XConsortium: utsco.c /main/2 1996/05/09 04:21:13 drk $ */
/*
 *   COMPONENT_NAME: austext
 *
 *   FUNCTIONS: d_utsco
 *
 *   ORIGINS: 157
 *
 *   OBJECT CODE ONLY SOURCE MATERIALS
 */
/*-----------------------------------------------------------------------
   utsco.c -- db_VISTA current owner update timestamp get module.

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
/* Get update timestamp of current owner
*/
d_utsco(set, timestamp TASK_PARM DBN_PARM)
int set;
ULONG FAR *timestamp;
TASK_DECL
DBN_DECL
{
   INT rec;
   char FAR *rptr;
   SET_ENTRY FAR *set_ptr;

   DB_ENTER(DB_ID TASK_ID LOCK_SET(SET_IO));

   if (nset_check(set, &set, (SET_ENTRY FAR * FAR *)&set_ptr) != S_OKAY)
      RETURN( db_status );

   /* make sure we have a current owner */
   if ( ! curr_own[set] )
      RETURN( dberr(S_NOCO) );

   /* read current owner */
   if ( dio_read( curr_own[set], (char FAR * FAR *)&rptr , NOPGHOLD) != S_OKAY )
      RETURN( db_status );

   /* get record id */
   bytecpy(&rec, rptr, sizeof(INT));
   if ( rec >= 0 ) {
      rec &= ~RLBMASK; /* mask off rlb */
#ifndef	 ONE_DB
      rec += curr_db_table->rt_offset;
#endif
      if ( record_table[rec].rt_flags & TIMESTAMPED )
	 bytecpy(timestamp, rptr + RECUPTIME, sizeof(ULONG));
      else
	 *timestamp = 0L;
      db_status = S_OKAY;
   }
   else
      db_status = S_DELETED;

   RETURN( db_status );
}
#endif
/* vpp -nOS2 -dUNIX -nBSD -nVANILLA_BSD -nVMS -nMEMLOCK -nWINDOWS -nFAR_ALLOC -f/usr/users/master/config/nonwin utsco.c */
