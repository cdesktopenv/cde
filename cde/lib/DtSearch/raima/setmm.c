/* $XConsortium: setmm.c /main/2 1996/05/09 04:17:09 drk $ */
/*
 *   COMPONENT_NAME: austext
 *
 *   FUNCTIONS: d_setmm
 *
 *   ORIGINS: 157
 *
 *   OBJECT CODE ONLY SOURCE MATERIALS
 */
/*-----------------------------------------------------------------------
   setmm.c -- db_VISTA set member to member module.

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

/* Set current member to current member
*/
d_setmm(sett, sets TASK_PARM DBN_PARM)
int sett;   /* set table entry number of target member */
int sets;   /* set table entry number of source member */
TASK_DECL
DBN_DECL    /* database number */
{
   int cmtype; /* current member type */
   register int mem;
   SET_ENTRY FAR *set_ptr;
   register MEMBER_ENTRY FAR *mem_ptr;
   int memtot;

   DB_ENTER(DB_ID TASK_ID LOCK_SET(SET_IO));

   if ((d_cmtype(sets, &cmtype TASK_PARM DBN_PARM) != S_OKAY) ||
       (nset_check(sett, &sett, (SET_ENTRY FAR * FAR *)&set_ptr) != S_OKAY))
      RETURN( db_status );

   cmtype += NUM2INT(-RECMARK, rt_offset);
   sets += NUM2INT(-SETMARK, st_offset);

   for (mem = set_ptr->st_members, memtot = mem + set_ptr->st_memtot,
						mem_ptr = &member_table[mem];
	mem < memtot;
	++mem, ++mem_ptr) {
      if (mem_ptr->mt_record == cmtype)
	 RETURN( r_smem(&curr_mem[sets], sett) );
   }
   RETURN( dberr( S_INVMEM ) );
}
/* vpp -nOS2 -dUNIX -nBSD -nVANILLA_BSD -nVMS -nMEMLOCK -nWINDOWS -nFAR_ALLOC -f/usr/users/master/config/nonwin setmm.c */
