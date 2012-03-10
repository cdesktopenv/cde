/* $XConsortium: startup.c /main/2 1996/05/09 04:19:11 drk $ */
/*
 *   COMPONENT_NAME: austext
 *
 *   FUNCTIONS: startup
 *
 *   ORIGINS: 157
 *
 *   OBJECT CODE ONLY SOURCE MATERIALS
 */

/* ********************** EDIT HISTORY *******************************

 SCR    DATE    INI                   DESCRIPTION
----- --------- --- -----------------------------------------------------
  532 06-Jan-89 RSC Fixed so will link correctly for ONE_DB
      05-May-89 WLW Added ".v" tag to Currtask for MULTI_TASKing
*/

#include <stdio.h>
#include "vista.h"
#include "dbtype.h"


/* Called once at the beginning of each external function
*/
int startup(DB_ID TASK_PTR_ID LOCK_PARM)
DBN_DECL
TASK_PTR_DECL
LOCK_DECL
{
   db_status = S_OKAY;
   if ( ! db_glob_init ) {
#ifdef MULTI_TASK
      Currtask.v.ptr = (TASK FAR *)&db_global;
#endif
      taskinit(&db_global);
      db_glob_init = 1;
   }
#ifdef MULTI_TASK
   if ( task != 0 ) {
      task_switch(task);
   }
#endif
#ifndef NO_DBN_PARM
#ifndef ONE_DB
   if ( dbn >= 0 && dbn_check(dbn) != S_OKAY ) {
      return( db_status );
   }
#endif
#endif
   return( db_status );
}
/* vpp -nOS2 -dUNIX -nBSD -nVANILLA_BSD -nVMS -nMEMLOCK -nWINDOWS -nFAR_ALLOC -f/usr/users/master/config/nonwin startup.c */
