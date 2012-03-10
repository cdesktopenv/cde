/* $XConsortium: dblog.c /main/2 1996/05/09 04:03:14 drk $ */
/*
 *   COMPONENT_NAME: austext
 *
 *   FUNCTIONS: d_dblog
 *
 *   ORIGINS: 157
 *
 *   OBJECT CODE ONLY SOURCE MATERIALS
 */
/*-----------------------------------------------------------------------
   dblog.c -- db_VISTA database log filename/path set module.

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

/* Set database log file name/path
*/
d_dblog(log TASK_PARM)
CONST char FAR *log;
TASK_DECL
{
   DB_ENTER(NO_DB_ID TASK_ID LOCK_SET(LOCK_NONE));

   if (dbopen)
      dberr(S_DBOPEN);
   else {
      strncpy(dblog, log, FILENMLEN - 1);
      dblog[FILENMLEN - 1] = '\0';
      db_status = S_OKAY;
   }
   RETURN( db_status );
}
/* vpp -nOS2 -dUNIX -nBSD -nVANILLA_BSD -nVMS -nMEMLOCK -nWINDOWS -nFAR_ALLOC -f/usr/users/master/config/nonwin dblog.c */
