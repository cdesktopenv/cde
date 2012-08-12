/*
 * CDE - Common Desktop Environment
 *
 * Copyright (c) 1993-2012, The Open Group. All rights reserved.
 *
 * These libraries and programs are free software; you can
 * redistribute them and/or modify them under the terms of the GNU
 * Lesser General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * These libraries and programs are distributed in the hope that
 * they will be useful, but WITHOUT ANY WARRANTY; without even the
 * implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with these librararies and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
/* $XConsortium: discon.c /main/2 1996/05/09 04:04:43 drk $ */
/*
 *   COMPONENT_NAME: austext
 *
 *   FUNCTIONS: d_discon
 *
 *   ORIGINS: 157
 *
 *   OBJECT CODE ONLY SOURCE MATERIALS
 */
/*-----------------------------------------------------------------------
   discon.c -- db_VISTA set dicconnection module

   (C) Copyright 1985, 1986, 1987 by Raima Corp.
-----------------------------------------------------------------------*/

/* ********************** EDIT HISTORY *******************************

 SCR    DATE    INI                   DESCRIPTION
----- --------- --- -----------------------------------------------------
      04-Aug-88 RTK MULTI_TASK changes
*/

#include <stdio.h>
#include "vista.h"
#include "dbtype.h"

/* set pointer structure definition */
typedef struct {
   LONG    total;     /* total number of members in set */
   DB_ADDR first;     /* database address of first member in set */
   DB_ADDR last;      /* database address of last member in set */
#ifndef	 NO_TIMESTAMP
   ULONG   timestamp; /* set update timestamp - if used */
#endif
} SET_PTR;

/* member pointer structure definition */
typedef struct {
   DB_ADDR owner;     /* database address of owner record */
   DB_ADDR prev;      /* database address of previous member in set */
   DB_ADDR next;      /* database address of next member in set */
} MEM_PTR;


/* Disconnect the current member of set
*/
int
d_discon(nset TASK_PARM DBN_PARM)
int nset; /* set number */
TASK_DECL
DBN_DECL  /* database number */
{
   SET_PTR cosp;          /* current owner's set pointer */
   MEM_PTR cmmp;          /* member's member pointer */
   MEM_PTR npmp;          /* next or previous member's member pointer */
   char FAR *orec;            /* ptr to current owner record contents in cache */
   char FAR *mrec = NULL;     /* ptr to member record contents in cache */
   char FAR *nprec = NULL;    /* ptr to next or prev record contents in cache */
   DB_ADDR mdba;          /* db address of member record */
   DB_ADDR npdba;         /* db address of next or previous member */
   int set;               /* set_table entry */
   int stat;              /* status code variable */
#ifndef	 NO_TIMESTAMP
   FILE_NO file;          /* file containing owner record */
#endif
   SET_ENTRY FAR *set_ptr;
   DB_ADDR FAR *co_ptr, FAR *cm_ptr;

   DB_ENTER(DB_ID TASK_ID LOCK_SET(SET_IO));

   if (nset_check(nset, &set, (SET_ENTRY FAR * FAR *)&set_ptr) != S_OKAY)
      RETURN( db_status );

   /* make sure we have a current owner */
   if ( !*(co_ptr = &curr_own[set]) )
      RETURN( dberr(S_NOCO) );

   /* make sure we have a current member */
   if ( !*(cm_ptr = &curr_mem[set]) )
      RETURN( dberr(S_NOCM) );

   /* read member record */
   mdba = *cm_ptr;
   if ( dio_read(mdba, (char FAR * FAR *)&mrec, PGHOLD) != S_OKAY )
      RETURN( db_status );

   /* ensure record is connected */
   if ( (stat = r_gmem(set, mrec, (char FAR *)&cmmp)) != S_OKAY )
      goto quit_b;
   if ( cmmp.owner == NULL_DBA ) { /* checks owner pointer */
      stat = S_NOTCON;
      goto quit_b;
   }
   /* read owner record */
   if ( (stat = dio_read(*co_ptr, (char FAR * FAR *)&orec, PGHOLD)) != S_OKAY )
      goto quit_b;

   /* get set pointer from owner */
   if ( r_gset(set, orec, (char FAR *)&cosp) != S_OKAY )
      goto quit_a;

   if ( cmmp.next == NULL_DBA )
      /* last record in set */
      cosp.last = cmmp.prev;
   else {
      /* set next record's prev to current member's prev */
      npdba = cmmp.next;
      if ((dio_read(npdba, (char FAR * FAR *)&nprec, NOPGHOLD) != S_OKAY) ||
	  (r_gmem(set, nprec, (char FAR *)&npmp) != S_OKAY))
	 goto quit_a;
      npmp.prev = cmmp.prev;
      if ((r_pmem(set, nprec, (char FAR *)&npmp) != S_OKAY) ||
	  (dio_write(npdba, NULL, NOPGFREE) != S_OKAY))
	 goto quit_a;
   }
   if ( cmmp.prev == NULL_DBA )
      /* first record in set */
      cosp.first = cmmp.next;
   else {
      /* set previous record's next to current member's next */
      npdba = cmmp.prev;
      if ((dio_read(npdba, (char FAR * FAR *)&nprec, NOPGHOLD) != S_OKAY) ||
	  (r_gmem(set, nprec, (char FAR *)&npmp) != S_OKAY))
	 goto quit_a;
      npmp.next = cmmp.next;
      if ((r_pmem(set, nprec, (char FAR *)&npmp) != S_OKAY) ||
	  (dio_write(npdba, NULL, NOPGFREE) != S_OKAY))
	 goto quit_a;
   }
#ifndef	 NO_TIMESTAMP
   /* check for timestamp */
   if ( set_ptr->st_flags & TIMESTAMPED ) {
      file = NUM2INT((FILE_NO)((*co_ptr >> FILESHIFT) & FILEMASK), ft_offset);
      cosp.timestamp = dio_pzgetts(file);
   }
#endif
   /* update membership count */
   --cosp.total;

   /* update owner record's set pointer */
   if ((r_pset(set, orec, (char FAR *)&cosp) != S_OKAY) ||
       (dio_write(*co_ptr, NULL, PGFREE) != S_OKAY))
      RETURN( db_status );

   /* update current record and current member */
   curr_rec  = mdba;
   *cm_ptr = cmmp.next;

   /* make member record's member pointer null */
   cmmp.owner = cmmp.prev = cmmp.next = NULL_DBA;
   
   /* update member record */
   if ((r_pmem(set, mrec, (char FAR *)&cmmp) != S_OKAY) ||
       (dio_write(mdba, NULL, PGFREE) != S_OKAY))
      RETURN( db_status );
#ifndef	 NO_TIMESTAMP
   /* note timestamps */
   if ( db_tsrecs )
      d_utscr( &cr_time TASK_PARM );
   if ( db_tsrecs && *cm_ptr )
      d_utscm(nset, &cm_time[set] TASK_PARM DBN_PARM);

   /* check for timestamp */
   if ( set_ptr->st_flags & TIMESTAMPED )
      cs_time[set] = cosp.timestamp;
#endif
   RETURN( db_status = *cm_ptr ? S_OKAY : S_EOS );

quit_a:
   stat = db_status;
   dio_write(mdba, NULL, PGFREE);
quit_b:
   dio_write(*co_ptr, NULL, PGFREE);
   RETURN( db_status = stat );
}
/* vpp -nOS2 -dUNIX -nBSD -nVANILLA_BSD -nVMS -nMEMLOCK -nWINDOWS -nFAR_ALLOC -f/usr/users/master/config/nonwin discon.c */
