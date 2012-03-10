/* $XConsortium: csmget.c /main/2 1996/05/09 03:58:53 drk $ */
/*
 *   COMPONENT_NAME: austext
 *
 *   FUNCTIONS: d_csmget
 *
 *   ORIGINS: 157
 *
 *   OBJECT CODE ONLY SOURCE MATERIALS
 */
/*-----------------------------------------------------------------------
   csmget.c -- db_VISTA current set member database address get module.

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

/* Get current set member
*/
d_csmget(set, dba TASK_PARM DBN_PARM)
int set;      /* Set table entry */
DB_ADDR FAR *dba; /* db address of record to become current */
TASK_DECL
DBN_DECL      /* database number */
{
   SET_ENTRY FAR *set_ptr;

   DB_ENTER(DB_ID TASK_ID LOCK_SET(SET_NOIO));

   if (nset_check(set, &set, (SET_ENTRY FAR * FAR *)&set_ptr) != S_OKAY)
      RETURN( db_status );

   if ( ! (*dba = curr_mem[set]) )
      db_status = S_NOCM;

   RETURN( db_status );
}
/* vpp -nOS2 -dUNIX -nBSD -nVANILLA_BSD -nVMS -nMEMLOCK -nWINDOWS -nFAR_ALLOC -f/usr/users/master/config/nonwin csmget.c */
