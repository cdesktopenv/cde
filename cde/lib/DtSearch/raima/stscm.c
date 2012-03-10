/* $XConsortium: stscm.c /main/2 1996/05/09 04:19:24 drk $ */
/*
 *   COMPONENT_NAME: austext
 *
 *   FUNCTIONS: d_stscm
 *
 *   ORIGINS: 157
 *
 *   OBJECT CODE ONLY SOURCE MATERIALS
 */
/*-----------------------------------------------------------------------
   stscm.c -- db_VISTA current member timestamp set module.

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
/* Set timestamp of current member
*/
d_stscm(set, timestamp TASK_PARM DBN_PARM)
int set;
ULONG timestamp;
TASK_DECL
DBN_DECL
{
   SET_ENTRY FAR *set_ptr;

   DB_ENTER(DB_ID TASK_ID LOCK_SET(SET_NOIO));

   if (nset_check(set, &set, (SET_ENTRY FAR * FAR *)&set_ptr) != S_OKAY)
      RETURN( db_status );

   /* make sure we have a current member */
   if ( curr_mem[set] ) {
      if ( db_tsrecs )
	 cm_time[set] = timestamp;
      else
	 dberr(S_TIMESTAMP);
   }
   else
      dberr(S_NOCM);

   RETURN( db_status );
}
#endif
/* vpp -nOS2 -dUNIX -nBSD -nVANILLA_BSD -nVMS -nMEMLOCK -nWINDOWS -nFAR_ALLOC -f/usr/users/master/config/nonwin stscm.c */
