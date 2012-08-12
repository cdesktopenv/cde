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
/* $XConsortium: connect.c /main/2 1996/05/09 03:56:41 drk $ */
/*
 *   COMPONENT_NAME: austext
 *
 *   FUNCTIONS: Pi
 *		d_connect
 *		sortcmp
 *
 *   ORIGINS: 157
 *
 *   OBJECT CODE ONLY SOURCE MATERIALS
 */
/*-----------------------------------------------------------------------
   connect.c -- db_VISTA set connection module

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

/* Internal function Prototypes */
static int sortcmp(P1(SET_ENTRY FAR *) Pi(char FAR *) 
				  Pi(char FAR *));

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


/* Connect the current record as member of set
*/
int
d_connect(set TASK_PARM DBN_PARM)
int set; /* Set constant */
TASK_DECL
DBN_DECL /* Database number */
{
   MEM_PTR crmp;          /* current record's member pointer */
   SET_PTR cosp;          /* current owner's set pointer */
   MEM_PTR cmmp;          /* current member's member pointer */
   MEM_PTR nmmp;          /* next member's member pointer */
   char FAR *crec;            /* ptr to current record contents in cache */
   char FAR *orec;            /* ptr to current owner record contents in cache */
   char FAR *mrec;            /* ptr to current member record contents in cache */
   char FAR *nrec;            /* ptr to next member record contents in cache */
   DB_ADDR mdba;          /* db address of current member record */
   DB_ADDR ndba;          /* db address of next member record */
   INT ordering;          /* set order control variable */
   int stat, compare;     /* status code & sort comparison result */
#ifndef	 NO_TIMESTAMP
   FILE_NO file;
#endif
   SET_ENTRY FAR *set_ptr;
   DB_ADDR FAR *co_ptr;
   DB_ADDR FAR *cm_ptr;

   DB_ENTER(DB_ID TASK_ID LOCK_SET(SET_IO));

   if (nset_check(set, &set, (SET_ENTRY FAR * FAR *)&set_ptr) != S_OKAY)
      RETURN( db_status );

   /* make sure we have a current record */
   if ( curr_rec == NULL_DBA )
      RETURN( dberr(S_NOCR) );

   /* make sure we have a current owner */
   if ( *(co_ptr = &curr_own[set]) == NULL_DBA )
      RETURN( dberr(S_NOCO) );

   crec = orec = mrec = nrec = NULL;

   /* read current record */
   if ( dio_read( curr_rec, (char FAR * FAR *)&crec, PGHOLD ) != S_OKAY )
      RETURN( db_status );
   
   /* read owner record */
   if ( (stat = dio_read(*co_ptr, (char FAR * FAR *)&orec, PGHOLD)) != S_OKAY ) {
      dio_release( curr_rec );
      RETURN( stat );
   }
   /* get copy of current record's member ptr for set */
   if ( (stat = r_gmem(set, crec, (char FAR *)&crmp)) != S_OKAY )
      goto quit;

   /* ensure record not already connected to set */
   if ( ! crmp.owner == NULL_DBA ) { 
      stat = dberr(S_ISOWNED);
      goto quit;
   }
   /* get set pointer from owner */
   if ( r_gset(set, orec, (char FAR *)&cosp) != S_OKAY ) {
      stat = db_status == S_INVOWN ? dberr(S_SYSERR) : db_status;
      goto quit;
   }
   /* set current record's owner to current owner of set */
   crmp.owner = *co_ptr;

   cm_ptr = &curr_mem[set];
   /* make insertion based on set order specfication */
   if (cosp.first == NULL_DBA) {
      /* set is empty */
      /* set current owner's first and last to current record */
      cosp.first = cosp.last = curr_rec;

      /* set current record's next and previous to null */
      crmp.next = crmp.prev = NULL_DBA;
      goto inserted;
   }
   /* order is as specified in DDL */
   ordering = set_ptr->st_order;

   while (TRUE) switch (ordering) {
      case ASCENDING:
      case DESCENDING:
	 /* perform a linked insertion sort on set -
	    position the current member to the proper place and then
	    switch to order NEXT */
	 for (mdba = cosp.first; TRUE; mdba = cmmp.next) {
	    /* read member record and get member pointer from member
	       record */
	    if (((stat = dio_read(mdba, (char FAR * FAR *)&mrec, NOPGHOLD)) != S_OKAY) ||
		((stat = r_gmem(set, mrec, (char FAR *)&cmmp)) != S_OKAY))
	       goto quit;

	    /* compare sort fields of current record with member record */
	    compare = sortcmp(set_ptr, crec, mrec);
	    if ((ordering == ASCENDING) ? (compare < 0) : (compare > 0)) {
	       /* found insertion position -
		  make previous member the current member of set and
		  switch to next order processing */
	       *cm_ptr = cmmp.prev;
	       ordering = NEXT;
	       break;
	    }
	    if ( cmmp.next == NULL_DBA ) {
	       /* connect at end of list */
	       *cm_ptr = mdba;
	       ordering = NEXT;
	       break;
	    }
	 }
	 break;
      case FIRST:
	 crmp.next = cosp.first;
	 /* read current owner's first member and get first member's
	    member pointer
	 */
	 mdba = cosp.first;
	 if (((stat = dio_read(mdba, (char FAR * FAR *)&mrec, PGHOLD)) != S_OKAY) ||
	     ((stat = r_gmem(set, mrec, (char FAR *)&cmmp)) != S_OKAY))
	    goto quit;

	 /* set current member's previous, and current owner's first, to
	    current record */
	 cmmp.prev = cosp.first = curr_rec;
	 goto inserted;
      case NEXT:
	 if (!*cm_ptr) {
	    /* if no current member, next is same as first */
	    ordering = FIRST;
	    break;
	 }
	 /* insert record after current member */
	 mdba = *cm_ptr;

	 /* read current member record and get member pointer from
	    current member
	 */
	 if (((stat = dio_read(mdba, (char FAR * FAR *)&mrec, PGHOLD)) != S_OKAY) ||
	     ((stat = r_gmem(set, mrec, (char FAR *)&cmmp)) != S_OKAY))
	    goto quit;

	 /* set current record's next to current member's next */
	 crmp.next = cmmp.next;  

	 /* set current record's prev to current member */
	 crmp.prev = mdba;       

	 /* set current member's next ro current record */
	 cmmp.next = curr_rec;   

	 if (crmp.next == NULL_DBA) {
	    /* current record at end of list -
	       update set pointer's last member */
	    cosp.last = curr_rec;
	    goto inserted;
	 }
	 /* read next member record and member pointer from next member */
	 ndba = crmp.next;
	 if (((stat = dio_read(ndba, (char FAR * FAR *)&nrec, PGHOLD)) != S_OKAY) ||
	     ((stat = r_gmem(set, nrec, (char FAR *)&nmmp)) != S_OKAY))
	    goto quit;

	 /* set previous pointer in next member to current record */
	 nmmp.prev = curr_rec;
	 goto inserted;
      case LAST:
	 /* set current member to owner's last pointer */
	 *cm_ptr = cosp.last;
	 /* switch to order next */
	 ordering = NEXT;
	 break;
      default:
	 /* there are no other possible orderings! */
	 RETURN( dberr(S_SYSERR) );
   }					/* while switch */
inserted:
   /* increment total members in set */
   ++cosp.total;
#ifndef	 NO_TIMESTAMP
   /* check for timestamp */
   if ( set_ptr->st_flags & TIMESTAMPED ) {
      file = NUM2INT((FILE_NO)((*co_ptr >> FILESHIFT) & FILEMASK), ft_offset);
      cosp.timestamp = dio_pzgetts(file);
   }
#endif
   if ( mrec ) { 
      /* put member pointer back into member record and mark member
	 record as modified
      */
      if (((stat = r_pmem(set, mrec, (char FAR *)&cmmp)) != S_OKAY) ||
	  ((stat = dio_write(mdba, NULL, PGFREE)) != S_OKAY))
	 goto quit;
   }
   if ( nrec ) { 
      /* put member pointer back into next record and mark next record
	 as modified
      */
      if (((stat = r_pmem(set, nrec, (char FAR *)&nmmp)) != S_OKAY) ||
	  ((stat = dio_write(ndba, NULL, PGFREE)) != S_OKAY))
	 goto quit;
   }
   /* put set pointer back into owner record and mark owner record as
      modified; put member pointer back into current record mark current
      record as modified */
   if (((stat = r_pset(set, orec, (char FAR *)&cosp)) != S_OKAY) ||
       ((stat = dio_write(*co_ptr, NULL, PGFREE)) != S_OKAY) ||
       ((stat = r_pmem(set, crec, (char FAR *)&crmp)) != S_OKAY) ||
       ((stat = dio_write(curr_rec, NULL, PGFREE)) != S_OKAY))
      goto quit;

   /* set current member to current record */
   *cm_ptr = curr_rec;
#ifndef	 NO_TIMESTAMP
   /* check and fetch timestamps */
   if ( db_tsrecs )
      d_utscr(&cm_time[set] TASK_PARM);
   if ( db_tssets )
      cs_time[set] = cosp.timestamp;
#endif
   RETURN( db_status );

/* error return */
quit:
   if ( crec ) dio_release( curr_rec );
   if ( orec ) dio_release( *co_ptr );
   if ( mrec ) dio_release( mdba );
   if ( nrec ) dio_release( ndba );
   RETURN( db_status = stat );
}


/* Compare two sort fields
*/
static int sortcmp(set_ptr, mem1, mem2)
SET_ENTRY FAR *set_ptr; /* set table entry */
char FAR *mem1; /* member record 1 */
char FAR *mem2; /* member record 2 */
{
   INT rn1, rn2;  /* record numbers for mem1 & mem2 */
   MEMBER_ENTRY FAR *mt1, FAR *mt2;
   register MEMBER_ENTRY FAR *mt;
   int mem, memtot;
   int cmp;       /* fldcmp result */
   register int maxflds;
   register SORT_ENTRY FAR *srt1_ptr, FAR *srt2_ptr;
   FIELD_ENTRY FAR *fld_ptr;

   /* extract record numbers from record header */
   bytecpy(&rn1, mem1, sizeof(INT));
   rn1 &= ~RLBMASK; /* mask off rlb */
   bytecpy(&rn2, mem2, sizeof(INT));
   rn2 &= ~RLBMASK; /* mask off rlb */
#ifndef	 ONE_DB
   rn1 += curr_db_table->rt_offset;
   rn2 += curr_db_table->rt_offset;
#endif

   /* locate member_table entries for these record types */
   mt2 = mt1 = NULL;
   for (mem = set_ptr->st_members, memtot = mem + set_ptr->st_memtot,
						      mt = &member_table[mem];
	mem < memtot;
	++mem, ++mt) {
      if ( mt->mt_record == rn1 ) {
	 mt1 = mt;
	 if (mt2 != NULL)
	    break;
      }
      if ( mt->mt_record == rn2 ) {
	 mt2 = mt;
	 if (mt1 != NULL)
	    break;
      }
   }
   /* set maxflds to number of sort fields in set (min(mt1,mt2)) */
   maxflds = (mt1->mt_totsf <= mt2->mt_totsf) ? mt1->mt_totsf : mt2->mt_totsf;
   
   /* do comparison for each field */
   for (srt1_ptr = &sort_table[mt1->mt_sort_fld],
				    srt2_ptr = &sort_table[mt2->mt_sort_fld];
	maxflds--;
	++srt1_ptr, ++srt2_ptr) {
      /* compare the two fields */
      /* computation is pointer to next sort field in member record */
      fld_ptr = &field_table[srt1_ptr->se_fld];
      if ((cmp = fldcmp(fld_ptr, mem1 + fld_ptr->fd_ptr,
		       mem2 + field_table[srt2_ptr->se_fld].fd_ptr)))
	 return (cmp);  /* return at first unequal fields */
   }
   /* fields match */
   return (0);
}
/* vpp -nOS2 -dUNIX -nBSD -nVANILLA_BSD -nVMS -nMEMLOCK -nWINDOWS -nFAR_ALLOC -f/usr/users/master/config/nonwin connect.c */
