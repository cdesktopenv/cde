/* $XConsortium: ismember.c /main/2 1996/05/09 04:08:28 drk $ */
/*
 *   COMPONENT_NAME: austext
 *
 *   FUNCTIONS: d_ismember
 *
 *   ORIGINS: 157
 *
 *   OBJECT CODE ONLY SOURCE MATERIALS
 */
/*-----------------------------------------------------------------------
   ismember.c -- db_VISTA membership test module.

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


/* Check to see if current record is a connected member of SET
*/
d_ismember(set TASK_PARM DBN_PARM)
int set;  /* set table entry */
TASK_DECL
DBN_DECL  /* database number */
{
   char crmp[MEMPSIZE];   /* current record's member pointer */
   SET_ENTRY FAR *set_ptr;

   DB_ENTER(DB_ID TASK_ID LOCK_SET(SET_IO));
   
   if (nset_check(set, &set, (SET_ENTRY FAR * FAR *)&set_ptr) != S_OKAY)
      RETURN( db_status );

   /* Make sure we have a current record */
   if ( ! curr_rec )
      RETURN( dberr(S_NOCR) );

   /* Read current record and check connection */
   if ((dio_read( curr_rec, (char FAR * FAR *)&crloc , NOPGHOLD) == S_OKAY) &&
       (r_gmem(set, crloc, crmp) == S_OKAY) &&
       null_dba(crmp + MP_OWNER))
      db_status = S_EOS; /* end-of-set if curr rec not owned */

   RETURN( db_status );
}
/* vpp -nOS2 -dUNIX -nBSD -nVANILLA_BSD -nVMS -nMEMLOCK -nWINDOWS -nFAR_ALLOC -f/usr/users/master/config/nonwin ismember.c */
