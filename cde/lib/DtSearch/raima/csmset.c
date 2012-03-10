/* $XConsortium: csmset.c /main/2 1996/05/09 03:59:23 drk $ */
/*
 *   COMPONENT_NAME: austext
 *
 *   FUNCTIONS: d_csmset
 *
 *   ORIGINS: 157
 *
 *   OBJECT CODE ONLY SOURCE MATERIALS
 */
/*-----------------------------------------------------------------------
   csmset.c -- db_VISTA current set member database address set module.

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

/* Set current set member
*/
d_csmset(set, dba TASK_PARM DBN_PARM)
int set;      /* Set table entry */
DB_ADDR FAR *dba; /* db address of record to become current */
TASK_DECL
DBN_DECL      /* database number */
{
   INT type;
   char FAR *ptr;
   SET_ENTRY FAR *set_ptr;
   MEMBER_ENTRY FAR *mem_ptr;
   int mem, memtot;

   DB_ENTER(DB_ID TASK_ID LOCK_SET(SET_IO));

   if (nset_check(set, &set, (SET_ENTRY FAR * FAR *)&set_ptr) != S_OKAY)
      RETURN( db_status );

   if ( dba == NULL ) {
      curr_mem[set] = NULL_DBA;
   }
   else if ( check_dba(*dba) == S_OKAY ) {
      /* get the record type */
      if ( dio_read( *dba, (char FAR * FAR *)&ptr, NOPGHOLD ) ) RETURN( db_status );
      bytecpy( &type, ptr, sizeof(INT) );
      type &= ~RLBMASK;
#ifndef	 ONE_DB
      type += curr_db_table->rt_offset;
#endif

      for (mem = set_ptr->st_members, memtot = mem + set_ptr->st_memtot,
						mem_ptr = &member_table[mem];
	   mem < memtot;
	   ++mem, ++mem_ptr) {
	 if (mem_ptr->mt_record == type)
	    RETURN( r_smem(dba, set) );
      }
      dberr( S_INVMEM );
   }
   RETURN( db_status );
}
/* vpp -nOS2 -dUNIX -nBSD -nVANILLA_BSD -nVMS -nMEMLOCK -nWINDOWS -nFAR_ALLOC -f/usr/users/master/config/nonwin csmset.c */
