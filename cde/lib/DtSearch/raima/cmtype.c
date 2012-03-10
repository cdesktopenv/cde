/* $XConsortium: cmtype.c /main/2 1996/05/09 03:56:24 drk $ */
/*
 *   COMPONENT_NAME: austext
 *
 *   FUNCTIONS: d_cmtype
 *
 *   ORIGINS: 157
 *
 *   OBJECT CODE ONLY SOURCE MATERIALS
 */
/*-----------------------------------------------------------------------
   cmtype.c -- db_VISTA member type module.

   (C) Copyright 1987 by Raima Corporation.
-----------------------------------------------------------------------*/

/* ********************** EDIT HISTORY *******************************

 SCR    DATE    INI                   DESCRIPTION
----- --------- --- -----------------------------------------------------
  103 24-Jun-88 RSC Improve generation of single-user version
      04-Aug-88 RTK MULTI_TASK changes

*/

#include <stdio.h>
#include "vista.h"
#include "dbtype.h"

/* Get current member type
*/
d_cmtype(set, cmtype TASK_PARM DBN_PARM)
int set;
int FAR *cmtype;
TASK_DECL
DBN_DECL
{
   char FAR *mrec;
   INT crt;
   SET_ENTRY FAR *set_ptr;
#ifndef SINGLE_USER
   int dbopen_sv;
#endif

   DB_ENTER(DB_ID TASK_ID LOCK_SET(SET_IO));

   if (nset_check(set, &set, (SET_ENTRY FAR * FAR *)&set_ptr) != S_OKAY)
      RETURN( db_status );

   if ( ! curr_mem[set] )
      RETURN( dberr( S_NOCM ) );

#ifndef SINGLE_USER
   /* set up to allow unlocked read */
   dbopen_sv = dbopen;
   dbopen = 2;
#endif

   /* Read current member */
   dio_read(curr_mem[set], (char FAR * FAR *)&mrec, NOPGHOLD);
#ifndef SINGLE_USER
   dbopen = dbopen_sv;
#endif
   if (db_status != S_OKAY)
      RETURN( db_status );

   /* Fetch record type from record header */
   bytecpy(&crt, mrec, sizeof(INT));
#ifndef SINGLE_USER
   crt &= ~RLBMASK; /* mask off rlb */
#endif
   *cmtype = (int)crt + RECMARK;

   RETURN( db_status );
}
/* vpp -nOS2 -dUNIX -nBSD -nVANILLA_BSD -nVMS -nMEMLOCK -nWINDOWS -nFAR_ALLOC -f/usr/users/master/config/nonwin cmtype.c */
