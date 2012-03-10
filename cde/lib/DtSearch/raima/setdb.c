/* $XConsortium: setdb.c /main/2 1996/05/09 04:16:53 drk $ */
/*
 *   COMPONENT_NAME: austext
 *
 *   FUNCTIONS: d_setdb
 *
 *   ORIGINS: 157
 *
 *   OBJECT CODE ONLY SOURCE MATERIALS
 */
/*-----------------------------------------------------------------------
   setdb.c -- db_VISTA current database set module.

   (C) Copyright 1987 by Raima Corporation.
-----------------------------------------------------------------------*/

/* ********************** EDIT HISTORY *******************************

 SCR    DATE    INI                   DESCRIPTION
----- --------- --- -----------------------------------------------------
      04-Aug-88 RTK MULTI_TASK changes
  419 05-Oct-88 RSC also need to set curr_rn_table
  420 06-Dec-88 WLW Updated Curr_db_table when using setdb.
*/

#include <stdio.h>
#include "vista.h"
#include "dbtype.h"

#ifndef	 ONE_DB
/* Set current database
*/
d_setdb(dbn TASK_PARM)
int dbn;   /* database number */
TASK_DECL
{
   DB_ENTER(DB_ID TASK_ID LOCK_SET(RECORD_NOIO));

/* For WINDOWS, all that needs to happen here is the dbn_check() which
   DB_ENTER results in */
   if ( dbn < 0 || dbn >= no_of_dbs )
      RETURN( dberr( S_INVDB ) );
   
   setdb_on = TRUE;
   curr_db_table->curr_dbt_rec = curr_rec;
   curr_db_table = &db_table[curr_db = dbn];
   curr_rn_table = &rn_table[dbn];            /*[419]*/
   curr_rec = curr_db_table->curr_dbt_rec;

   RETURN( db_status = S_OKAY );
}
#endif
/* vpp -nOS2 -dUNIX -nBSD -nVANILLA_BSD -nVMS -nMEMLOCK -nWINDOWS -nFAR_ALLOC -f/usr/users/master/config/nonwin setdb.c */
