/* $XConsortium: dbfpath.c /main/2 1996/05/09 04:02:40 drk $ */
/*
 *   COMPONENT_NAME: austext
 *
 *   FUNCTIONS: d_dbfpath
 *
 *   ORIGINS: 157
 *
 *   OBJECT CODE ONLY SOURCE MATERIALS
 */
/*-----------------------------------------------------------------------
   dbfpath.c -- db_VISTA files path set module.

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

/* Set Database Files Path
*/
d_dbfpath(path TASK_PARM)
CONST char FAR *path;
TASK_DECL
{
   DB_ENTER(NO_DB_ID TASK_ID LOCK_SET(LOCK_NONE));

   if (dbopen)
      dberr(S_DBOPEN);
   else if (strlen(path) >= FILENMLEN - 1)
      dberr(S_NAMELEN);
   else {
      strcpy(dbfpath, path);
      db_status = S_OKAY;
   }
   RETURN( db_status );
}
/* vpp -nOS2 -dUNIX -nBSD -nVANILLA_BSD -nVMS -nMEMLOCK -nWINDOWS -nFAR_ALLOC -f/usr/users/master/config/nonwin dbfpath.c */
