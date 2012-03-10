/* $XConsortium: csoread.c /main/2 1996/05/09 04:00:07 drk $ */
/*
 *   COMPONENT_NAME: austext
 *
 *   FUNCTIONS: d_csoread
 *
 *   ORIGINS: 157
 *
 *   OBJECT CODE ONLY SOURCE MATERIALS
 */
/*-----------------------------------------------------------------------
   csoread.c -- db_VISTA current set owner field read module.

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


/* Read data from field  of current set owner
*/
d_csoread(set, field, data TASK_PARM DBN_PARM)
int set;    /* Set constant */
long field; /* Field constant */
char FAR *data; /* Data area to contain field contents */
TASK_DECL
DBN_DECL    /* database number */
{
   int fld, rec;
#ifndef SINGLE_USER
   int dbopen_sv;
#endif
   char FAR *recp;
   SET_ENTRY FAR *set_ptr;
   RECORD_ENTRY FAR *rec_ptr;
   FIELD_ENTRY FAR *fld_ptr;

   DB_ENTER(DB_ID TASK_ID LOCK_SET(SET_IO));

   if ((nset_check(set, &set, (SET_ENTRY FAR * FAR *)&set_ptr) != S_OKAY) ||
       (nfld_check(field, &rec, &fld, (RECORD_ENTRY FAR * FAR *)&rec_ptr, (FIELD_ENTRY FAR * FAR *)&fld_ptr) != S_OKAY))
      RETURN( db_status );

   /* Make sure we have a current owner */
   if ( ! curr_own[set] )
      RETURN( dberr(S_NOCO) );

   /* set up to allow unlocked read */
#ifndef SINGLE_USER
   dbopen_sv = dbopen;
   dbopen = 2;
#endif

   /* Read current owner */
   dio_read( curr_own[set], (char FAR * FAR *)&recp , NOPGHOLD);
#ifndef SINGLE_USER
   dbopen = dbopen_sv;
#endif
   if (db_status != S_OKAY)
      RETURN( db_status );
  
   /* Get data from record and return */
   RETURN( r_gfld(fld_ptr, recp, data) );
}
/* vpp -nOS2 -dUNIX -nBSD -nVANILLA_BSD -nVMS -nMEMLOCK -nWINDOWS -nFAR_ALLOC -f/usr/users/master/config/nonwin csoread.c */
