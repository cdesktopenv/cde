/* $XConsortium: setmo.c /main/2 1996/05/09 04:17:25 drk $ */
/*
 *   COMPONENT_NAME: austext
 *
 *   FUNCTIONS: d_setmo
 *
 *   ORIGINS: 157
 *
 *   OBJECT CODE ONLY SOURCE MATERIALS
 */
/*-----------------------------------------------------------------------
   setmo.c -- db_VISTA set member to owner module.

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

/* Set current member to current owner
*/
d_setmo(setm, seto TASK_PARM DBN_PARM)
int setm;   /* set table entry number of member */
int seto;   /* set table entry number of owner */
TASK_DECL
DBN_DECL    /* database number */
{
   register int mem;
   SET_ENTRY FAR *setm_ptr, FAR *seto_ptr;
   register MEMBER_ENTRY FAR *mem_ptr;
   int memtot;

   DB_ENTER(DB_ID TASK_ID LOCK_SET(SET_IO));

   if ((nset_check(seto, &seto, (SET_ENTRY FAR * FAR *)&seto_ptr) != S_OKAY) ||
       (nset_check(setm, &setm, (SET_ENTRY FAR * FAR *)&setm_ptr) != S_OKAY))
      RETURN( db_status );

   if (null_dba((char FAR *)&curr_own[seto])) RETURN( dberr(S_NOCO) );

   for (mem = setm_ptr->st_members, memtot = mem + setm_ptr->st_memtot,
						mem_ptr = &member_table[mem];
	mem < memtot;
	++mem, ++mem_ptr) {
      if (mem_ptr->mt_record == seto_ptr->st_own_rt)
	 RETURN( r_smem(&curr_own[seto], setm) );
   }
   RETURN( dberr( S_INVMEM ) );
}
/* vpp -nOS2 -dUNIX -nBSD -nVANILLA_BSD -nVMS -nMEMLOCK -nWINDOWS -nFAR_ALLOC -f/usr/users/master/config/nonwin setmo.c */
