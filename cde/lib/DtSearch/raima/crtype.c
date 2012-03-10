/* $XConsortium: crtype.c /main/2 1996/05/09 03:58:25 drk $ */
/*
 *   COMPONENT_NAME: austext
 *
 *   FUNCTIONS: d_crtype
 *
 *   ORIGINS: 157
 *
 *   OBJECT CODE ONLY SOURCE MATERIALS
 */
/*-----------------------------------------------------------------------
   crtype.c -- db_VISTA current record type module.

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

/* Get current record type
*/
d_crtype(crtype TASK_PARM DBN_PARM)
int FAR *crtype;
TASK_DECL
DBN_DECL
{
   INT crt;
#ifndef SINGLE_USER
   int dbopen_sv;
#endif

   DB_ENTER(DB_ID TASK_ID LOCK_SET(RECORD_IO));

   if ( ! dbopen ) RETURN( dberr(S_DBOPEN) );
   
   if ( ! curr_rec )
      RETURN( dberr( S_NOCR ) );

   /* set up to allow unlocked read */
#ifndef SINGLE_USER
   dbopen_sv = dbopen;
   dbopen = 2;
#endif

   /* Read current record */
   dio_read(curr_rec, (char FAR * FAR *)&crloc, NOPGHOLD);
#ifndef SINGLE_USER
   dbopen = dbopen_sv;
#endif
   if (db_status != S_OKAY)
      RETURN( db_status );
   
   /* Fetch record type from record header */
   bytecpy(&crt, crloc, sizeof(INT));
   crt &= ~RLBMASK; /* mask off rlb */
   *crtype = (int)crt + RECMARK;

   RETURN( db_status = S_OKAY );
}
/* vpp -nOS2 -dUNIX -nBSD -nVANILLA_BSD -nVMS -nMEMLOCK -nWINDOWS -nFAR_ALLOC -f/usr/users/master/config/nonwin crtype.c */
