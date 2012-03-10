/* $XConsortium: setrm.c /main/2 1996/05/09 04:18:40 drk $ */
/*
 *   COMPONENT_NAME: austext
 *
 *   FUNCTIONS: d_setrm
 *
 *   ORIGINS: 157
 *
 *   OBJECT CODE ONLY SOURCE MATERIALS
 */
/*-----------------------------------------------------------------------
   setrm.c -- db_VISTA set current record to member module.

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

/* Set current record to current member
*/
d_setrm(set TASK_PARM DBN_PARM)
int set;   /* set table entry number */
TASK_DECL
DBN_DECL   /* database number */
{
   SET_ENTRY FAR *set_ptr;

   DB_ENTER(DB_ID TASK_ID LOCK_SET(SET_NOIO));
   
   if (nset_check(set, &set, (SET_ENTRY FAR * FAR *)&set_ptr) != S_OKAY)
      RETURN( db_status );

   if ( ! curr_mem[set] ) 
      RETURN( dberr( S_NOCM ) );

   curr_rec = curr_mem[set];
#ifndef	 NO_TIMESTAMP
   /* set timestamp */
   if ( db_tsrecs )
      cr_time = cm_time[set];
#endif
   RETURN( db_status = S_OKAY );
}
/* vpp -nOS2 -dUNIX -nBSD -nVANILLA_BSD -nVMS -nMEMLOCK -nWINDOWS -nFAR_ALLOC -f/usr/users/master/config/nonwin setrm.c */
