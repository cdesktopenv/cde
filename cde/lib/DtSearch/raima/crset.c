/* $XConsortium: crset.c /main/2 1996/05/09 03:57:56 drk $ */
/*
 *   COMPONENT_NAME: austext
 *
 *   FUNCTIONS: d_crset
 *
 *   ORIGINS: 157
 *
 *   OBJECT CODE ONLY SOURCE MATERIALS
 */
/*-----------------------------------------------------------------------
   crset.c -- db_VISTA current record database address set module.

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

/* Set current record
*/
d_crset(dba TASK_PARM DBN_PARM)
DB_ADDR FAR *dba; /* db address of record to become current */
TASK_DECL
DBN_DECL
{
   DB_ENTER(DB_ID TASK_ID LOCK_SET(RECORD_IO));

   if ( ! dbopen ) RETURN( dberr(S_DBOPEN) );

   if ( dba == NULL ) {
      curr_rec = NULL_DBA;
      db_status = S_OKAY;
   }
   else if ( check_dba(*dba) == S_OKAY ) 
      curr_rec = *dba;
   
   RETURN( db_status );
}
/* vpp -nOS2 -dUNIX -nBSD -nVANILLA_BSD -nVMS -nMEMLOCK -nWINDOWS -nFAR_ALLOC -f/usr/users/master/config/nonwin crset.c */
