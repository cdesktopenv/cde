/* $XConsortium: dbtaf.c /main/3 1996/05/09 04:03:32 drk $ */
/*
 *   COMPONENT_NAME: austext
 *
 *   FUNCTIONS: d_dbtaf
 *
 *   ORIGINS: 157
 *
 *   OBJECT CODE ONLY SOURCE MATERIALS
 */
/*-----------------------------------------------------------------------
   dbtaf.c -- db_VISTA transaction activity filename/path set module.

   (C) Copyright 1987 by Raima Corporation.
-----------------------------------------------------------------------*/

/* ********************** EDIT HISTORY *******************************

 SCR    DATE    INI                   DESCRIPTION
----- --------- --- -----------------------------------------------------
      04-Aug-88 RTK MULTI_TASK changes
      05-May-89 WLW Added TASK_PARM for multi-tasking
*/

#include <stdio.h>
#include "vista.h"
#include "dbtype.h"

static char VISTATAF[] = "vista.taf";

/* Set database transaction activity file name/path
*/
d_dbtaf(taf TASK_PARM)
CONST char FAR *taf;
TASK_DECL
{
   char dbtaf[FILENMLEN];
   int len;

   DB_ENTER(NO_DB_ID TASK_ID LOCK_SET(LOCK_NONE));
   if (dbopen)
      dberr(S_DBOPEN);
   else {
      len = strlen(taf);
      if (len > (FILENMLEN - 1)) {
	 strncpy(dbtaf, taf, FILENMLEN - 1);
	 dbtaf[FILENMLEN - 1] = '\0';
      }
      else if (taf[len - 1] == DIRCHAR) {
	 if (len > (FILENMLEN - sizeof(VISTATAF))) {
	    strncpy(dbtaf, taf, FILENMLEN - sizeof(VISTATAF) - 1);
	    dbtaf[(len = (FILENMLEN - sizeof(VISTATAF))) - 1] = DIRCHAR;
	 }
	 else
	    strcpy(dbtaf, taf);
	 strcpy(&dbtaf[len], VISTATAF);
      }
      else
	 strcpy(dbtaf, taf);
      db_status = S_OKAY;
   }
   RETURN (db_status = S_OKAY);
}
/* vpp -nOS2 -dUNIX -nBSD -nVANILLA_BSD -nVMS -nMEMLOCK -nWINDOWS -nFAR_ALLOC -f/usr/users/master/config/nonwin dbtaf.c */
