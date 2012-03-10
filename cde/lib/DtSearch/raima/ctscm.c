/* $XConsortium: ctscm.c /main/2 1996/05/09 04:01:14 drk $ */
/*
 *   COMPONENT_NAME: austext
 *
 *   FUNCTIONS: d_ctscm
 *
 *   ORIGINS: 157
 *
 *   OBJECT CODE ONLY SOURCE MATERIALS
 */
/*-----------------------------------------------------------------------
   ctscm.c -- db_VISTA current member creation timestamp get module.

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
/* Get creation timestamp of current member
*/
d_ctscm(set, timestamp TASK_PARM DBN_PARM)
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

   /* make sure we have a current member */
   if ( ! curr_mem[set] )
      RETURN( dberr(S_NOCM) );

   /* read current member */
   if ( dio_read( curr_mem[set], (char FAR * FAR *)&rptr , NOPGHOLD) != S_OKAY )
      RETURN( db_status );

   /* get record id */
   bytecpy(&rec, rptr, sizeof(INT));
   if ( rec >= 0 ) {
      rec &= ~RLBMASK; /* mask off rlb */
      if (record_table[NUM2INT(rec, rt_offset)].rt_flags & TIMESTAMPED)
	 bytecpy(timestamp, rptr + RECCRTIME, sizeof(ULONG));
      else
	 *timestamp = 0L;
   }
   else
      db_status = S_DELETED;

   RETURN( db_status );
}
#endif
/* vpp -nOS2 -dUNIX -nBSD -nVANILLA_BSD -nVMS -nMEMLOCK -nWINDOWS -nFAR_ALLOC -f/usr/users/master/config/nonwin ctscm.c */
