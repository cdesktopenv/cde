/* $XConsortium: crget.c /main/2 1996/05/09 03:57:29 drk $ */
/*
 *   COMPONENT_NAME: austext
 *
 *   FUNCTIONS: d_crget
 *
 *   ORIGINS: 157
 *
 *   OBJECT CODE ONLY SOURCE MATERIALS
 */
/*-----------------------------------------------------------------------
   crget.c -- db_VISTA current record database address get module.

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

/* Get current record
*/
d_crget(dba TASK_PARM DBN_PARM)
DB_ADDR FAR *dba; /* db address of record to become current */
TASK_DECL
DBN_DECL
{
   DB_ENTER(DB_ID TASK_ID LOCK_SET(RECORD_NOIO));

   if ( ! dbopen ) RETURN( dberr(S_DBOPEN) );

   if ( *dba = curr_rec )
      db_status = S_OKAY;
   else
      db_status = S_NOCR;

   RETURN( db_status );
}
/* vpp -nOS2 -dUNIX -nBSD -nVANILLA_BSD -nVMS -nMEMLOCK -nWINDOWS -nFAR_ALLOC -f/usr/users/master/config/nonwin crget.c */
