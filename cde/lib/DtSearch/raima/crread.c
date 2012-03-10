/* $XConsortium: crread.c /main/2 1996/05/09 03:57:43 drk $ */
/*
 *   COMPONENT_NAME: austext
 *
 *   FUNCTIONS: d_crread
 *
 *   ORIGINS: 157
 *
 *   OBJECT CODE ONLY SOURCE MATERIALS
 */
/*-----------------------------------------------------------------------
   crread.c -- db_VISTA current record field read module.

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


/* Read data from field  of current record
*/
d_crread(field, data TASK_PARM DBN_PARM)
long field; /* Field constant */
char FAR *data; /* Data area to contain field contents */
TASK_DECL
DBN_DECL    /* database number */
{
   int fld, rec;
#ifndef SINGLE_USER
   int dbopen_sv;
#endif
   RECORD_ENTRY FAR *rec_ptr;
   FIELD_ENTRY FAR *fld_ptr;

   DB_ENTER(DB_ID TASK_ID LOCK_SET(RECORD_IO));

   if (nfld_check(field, &rec, &fld, (RECORD_ENTRY FAR * FAR *)&rec_ptr, (FIELD_ENTRY FAR * FAR *)&fld_ptr) != S_OKAY)
      RETURN( db_status );

   /* Make sure we have a current record */
   if ( ! curr_rec )
      RETURN( dberr(S_NOCR) );

   /* set up to allow unlocked read */
#ifndef SINGLE_USER
   dbopen_sv = dbopen;
   dbopen = 2;
#endif

   /* Read current record */
   dio_read( curr_rec, (char FAR * FAR *)&crloc , NOPGHOLD);
#ifndef SINGLE_USER
   dbopen = dbopen_sv;
#endif
   if (db_status != S_OKAY)
      RETURN( db_status );
  
   /* Get data from record and return */
   RETURN( r_gfld(fld_ptr, crloc, data) );
}
/* vpp -nOS2 -dUNIX -nBSD -nVANILLA_BSD -nVMS -nMEMLOCK -nWINDOWS -nFAR_ALLOC -f/usr/users/master/config/nonwin crread.c */
