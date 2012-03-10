/* $XConsortium: findnm.c /main/2 1996/05/09 04:06:45 drk $ */
/*
 *   COMPONENT_NAME: austext
 *
 *   FUNCTIONS: d_findnm
 *
 *   ORIGINS: 157
 *
 *   OBJECT CODE ONLY SOURCE MATERIALS
 */
/*-----------------------------------------------------------------------
   findnm.c -- db_VISTA find next member module

   (C) Copyright 1987 by Raima Corp.
-----------------------------------------------------------------------*/

/* ********************** EDIT HISTORY *******************************

 SCR    DATE    INI                   DESCRIPTION
----- --------- --- -----------------------------------------------------
      04-Aug-88 RTK MULTI_TASK changes
*/

#include <stdio.h>
#include "vista.h"
#include "dbtype.h"

/* Find next member of set
*/
d_findnm(nset TASK_PARM DBN_PARM)
int nset;
TASK_DECL
DBN_DECL
{
   int set;
   char memp[MEMPSIZE];
   DB_ADDR mem;
   char FAR *recp;
   DB_ADDR FAR *cm_ptr;
   SET_ENTRY FAR *set_ptr;

   DB_ENTER(DB_ID TASK_ID LOCK_SET(SET_IO));

   if (nset_check(nset, &set, (SET_ENTRY FAR * FAR *)&set_ptr) != S_OKAY)
      RETURN( db_status );

   /* make sure we have a current owner */
   if ( ! curr_own[set] )
      RETURN( dberr(S_NOCO) );

   /* find first member if no current member */
   if ( !*(cm_ptr = &curr_mem[set]) )
      RETURN( d_findfm(nset TASK_PARM DBN_PARM) );

   /* read current member of set and get member pointer from record */
   if ((dio_read(*cm_ptr, (char FAR * FAR *)&recp, NOPGHOLD) != S_OKAY) ||
       (r_gmem(set, recp, memp) != S_OKAY))
      RETURN( db_status );

   /* extract address of next member */
   bytecpy(&mem, memp+MP_NEXT, DB_ADDR_SIZE);

   /* set current record and member */
   *cm_ptr = mem;

   if ( ! mem ) /* end of set */
      RETURN( db_status = S_EOS );

   curr_rec  = mem;
#ifndef	 NO_TIMESTAMP
   /* set timestamps */
   if ( db_tsrecs ) {
      d_utscr( &cr_time TASK_PARM );
      cm_time[set] = cr_time;
   }
#endif
   RETURN( db_status = S_OKAY );
}
/* vpp -nOS2 -dUNIX -nBSD -nVANILLA_BSD -nVMS -nMEMLOCK -nWINDOWS -nFAR_ALLOC -f/usr/users/master/config/nonwin findnm.c */
