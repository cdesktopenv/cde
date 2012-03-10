/* $XConsortium: stscs.c /main/2 1996/05/09 04:20:12 drk $ */
/*
 *   COMPONENT_NAME: austext
 *
 *   FUNCTIONS: d_stscs
 *
 *   ORIGINS: 157
 *
 *   OBJECT CODE ONLY SOURCE MATERIALS
 */
/*-----------------------------------------------------------------------
   stscs.c -- db_VISTA set timestamp set module.

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
/* Set timestamp of set
*/
d_stscs(set, timestamp TASK_PARM DBN_PARM)
int set;
ULONG timestamp;
TASK_DECL
DBN_DECL
{
   SET_ENTRY FAR *set_ptr;

   DB_ENTER(DB_ID TASK_ID LOCK_SET(SET_NOIO));

   if (nset_check(set, &set, (SET_ENTRY FAR * FAR *)&set_ptr) != S_OKAY)
      RETURN( db_status );

   /* make sure we have a current owner */
   if ( curr_own[set] ) {
      if ( db_tssets )
	 cs_time[set] = timestamp;
      else
	 dberr(S_TIMESTAMP);
   }
   else
      dberr(S_NOCO);

   RETURN( db_status );
}
#endif
/* vpp -nOS2 -dUNIX -nBSD -nVANILLA_BSD -nVMS -nMEMLOCK -nWINDOWS -nFAR_ALLOC -f/usr/users/master/config/nonwin stscs.c */
