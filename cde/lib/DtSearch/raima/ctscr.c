/* $XConsortium: ctscr.c /main/2 1996/05/09 04:01:47 drk $ */
/*
 *   COMPONENT_NAME: austext
 *
 *   FUNCTIONS: d_ctscr
 *
 *   ORIGINS: 157
 *
 *   OBJECT CODE ONLY SOURCE MATERIALS
 */
/*-----------------------------------------------------------------------
   ctscr.c -- db_VISTA current record creation timestamp get module.

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
/* Get creation timestamp of current record
*/
d_ctscr(timestamp TASK_PARM)
ULONG FAR *timestamp;
TASK_DECL
{
   INT rec;

   DB_ENTER(NO_DB_ID TASK_ID LOCK_SET(RECORD_IO));

   /* make sure we have a current record */
   if ( ! curr_rec )
      RETURN( dberr(S_NOCR) );

   /* read current record */
   if ( dio_read( curr_rec, (char FAR * FAR *)&crloc , NOPGHOLD) != S_OKAY )
      RETURN( db_status );

   /* get record id */
   bytecpy(&rec, crloc, sizeof(INT));
   if ( rec >= 0 ) {
      rec &= ~RLBMASK; /* mask off rlb */
      if (record_table[NUM2INT(rec, rt_offset)].rt_flags & TIMESTAMPED)
	 bytecpy(timestamp, crloc + RECCRTIME, sizeof(ULONG));
      else
	 *timestamp = 0L;
   }
   else
      db_status = S_DELETED;

   RETURN( db_status );
}
#endif
/* vpp -nOS2 -dUNIX -nBSD -nVANILLA_BSD -nVMS -nMEMLOCK -nWINDOWS -nFAR_ALLOC -f/usr/users/master/config/nonwin ctscr.c */
