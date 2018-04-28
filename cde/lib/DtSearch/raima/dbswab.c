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
 * License along with these libraries and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
/*
 *   COMPONENT_NAME: austext
 *
 *   FUNCTIONS: swab_page
 *
 *   ORIGINS: 27
 *
 *   (C) COPYRIGHT International Business Machines Corp. 1996
 *   All Rights Reserved
 *   Licensed Materials - Property of IBM
 *   US Government Users Restricted Rights - Use, duplication or
 *   disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 */
/********************* DBSWAB.C **********************************
 * $XConsortium: dbswab.c /main/3 1996/10/28 13:59:11 drk $
 * July 1996.
 * These functions do byte swap and other manipulations on
 * vista cache pages to ensure that database files are
 * maintained in a canonical format no matter what machine
 * they are used on.
 * The DtSearch data records are presumed already in external
 * file format by code at the DtSearch level (dtsrswab.c).
 * These functions use the standard htonl, htons, ntohl, and ntohs
 * network conversion functions for byte order neutrality.
 * They use the LONG and INT typedefs in vista.h
 * to control integer size.
 * The external database files are in the canonical,
 * big-endian "network" order.
 * So that they can be defined as "null" macros on big endian machines,
 * the functions (1) always byte swap in place, and (2) always succeed.
 *
 * $Log$
 */
#include "vista.h"
#include "dbtype.h"
#include "dbswab.h"

#define DEBUG_DBSWAB
#ifdef DEBUG_DBSWAB
#include <stdio.h>
int	debugging_dbswab = FALSE;
int	debugging_key_swabs = FALSE;
#endif


/********************************/
/*				*/
/*	    swab_page		*/
/*				*/
/********************************/
/* Performs byte swap on all slots of any cache page except page 0.
 * 4-byte timestamp page header already swapped by caller.
 *
 * WARNING!  The algorithms will only work for the
 * DtSearch schema.  For data pages, they ignore the
 * 1-byte optional key flags except for the OR_MISC record.
 * The first 6 bytes of each slot (rectype, dba)
 * are always byte swapped.  Thereafter every 4-bytes
 * is swapped as set or member ptrs until we reach
 * the data offset.  For OR_MISC records we presume
 * exactly 1 byte of optional key flags before the
 * set/member ptrs.
 */
void	swab_page (char *pgbuf, FILE_ENTRY *file_ptr, SWABDIR direction)
{
#ifndef BYTE_SWAP
    return;
#else
    INT		slotno;
    char	*slotptr;
    char	*cptr;
    INT		slsize = file_ptr->ft_slsize;
    INT		slots = file_ptr->ft_slots;	/* # slots per page */
    INT		recno;
    INT		used_count;
    INT		key_prefix;
    INT		curr_offset, data_offset;
    INT  	align_INT;
    LONG	align_LONG;

    /*----------------- DATA PAGES --------------------
     * If file is 'data' file, slots begin immediately after
     * the page's timestamp.  For each slot we have to swap
     * the optional set and member tables as well as fixed
     * slot header data.
     */
    if (file_ptr->ft_type == DATA) {
#ifdef DEBUG_DBSWAB
	if (debugging_dbswab) {
	    printf (__FILE__"073 %s DATA page '%s': slct=%d slsz=%d pgsz=%d\n",
		(direction == HTON)? "WRITE" : "READ",
		file_ptr->ft_name, (int)file_ptr->ft_slots,
		 (int)file_ptr->ft_slsize,  (int)file_ptr->ft_pgsize);
	    fflush(stdout);
	}
#endif

	for (	slotno = 0,  slotptr = pgbuf + 4; /* over timestamp */
		slotno < slots;
		slotno++,  slotptr += slsize) {

	    /* record number (= record type)
	     * Save the correct host order value in 'recno'.
	     */
	    memcpy (&align_INT, slotptr, sizeof(INT));
	    if (direction == HTON) {
		recno = align_INT;
		HTONS (align_INT);
	    }
	    else {
		NTOHS (align_INT);
		recno = align_INT;
	    }
	    memcpy (slotptr, &align_INT, sizeof(INT));

	    /* dba, or delete chain ptr */
	    memcpy (&align_LONG, slotptr + sizeof(INT), sizeof(LONG));
	    align_LONG = HTONL (align_LONG);
	    memcpy (slotptr + sizeof(INT), &align_LONG, sizeof(LONG));

	    /* If this is a deleted record, we're done */
	    if (recno & 0x8000)
		 continue;

	    /* Swap 4-byte set/member ptrs till we hit data */
	    recno &= 0x0fff;	/* switch off delete and lock bits */
	    data_offset = record_table[recno].rt_data;
	    for (	curr_offset = (recno == MISCREC_RECNO)? 7 : 6;
			curr_offset < data_offset;
			curr_offset += sizeof(LONG)) {
		memcpy (&align_LONG, slotptr + curr_offset, sizeof(LONG));
		align_LONG = HTONL (align_LONG);
		memcpy (slotptr + curr_offset, &align_LONG, sizeof(LONG));
	    }
	} /* end loop on each slot */
    } /* end if (data page) */

    /*----------------- KEY PAGES --------------------
     * DtSearch doesn't use anything but key and data files
     * so this must be a key file.  A b-tree node = one cache page.
     * Before the slots begin in a key file page there is
     * additional header data to be swapped after the timestamp:
     * the used_slots count (2 bytes) and the orphan ptr (4 bytes).
     * If used_slots == 0, then the page is not used and the
     * orphan ptr is really a delete chain ptr for the entire node.
     */
    else {
	/* 'used_slots'
	 * Save the correct host order value in 'used_count'.
	 */
	memcpy (&align_INT, pgbuf + 4, sizeof(INT));
	if (direction == HTON) {
	    used_count = align_INT;
	    HTONS (align_INT);
	}
	else {
	    NTOHS (align_INT);
	    used_count = align_INT;
	}
	memcpy (pgbuf + 4, &align_INT, sizeof(INT));

	/* 'orphan' ptr or 'delete chain' ptr */
	memcpy (&align_LONG, pgbuf + 6, sizeof(LONG));
	align_LONG = HTONL (align_LONG);
	memcpy (pgbuf + 6, &align_LONG, sizeof(LONG));

#ifdef DEBUG_DBSWAB
	if (debugging_dbswab || debugging_key_swabs) {
	    printf (__FILE__
		"124 %s KEY page '%s': slct=%d used=%d slsz=%d pgsz=%d\n",
		(direction == HTON)? "WRITE" : "READ",
		file_ptr->ft_name, (int)slots,
		(int)used_count,
		(int)slsize,  (int)file_ptr->ft_pgsize);
	    fflush (stdout);
	}
#endif
	/* For each used slot we have to swap the key prefix
	 * at the beginning of the slot, then at the *end*
	 * of the key, the dba, followed by the child node ptr.
	 * (This is not the format raima published but it's true!)
	 * Slots start at pg offset 10: timestamp + used_slots + orphan ptr.
	 * Note that if used_count == 0, we won't process
	 * this deleted node any further.
	 */
	for (	slotno = 0,  slotptr = pgbuf + 10;
		slotno < used_count;
		slotno++,  slotptr += slsize) {

	    /* 'key prefix.  Save host order value. */
	    memcpy (&align_INT, slotptr, sizeof(INT));
	    if (direction == HTON) {
		key_prefix = align_INT;
		HTONS (align_INT);
	    }
	    else {
		NTOHS (align_INT);
		key_prefix = align_INT;
	    }
	    memcpy (slotptr, &align_INT, sizeof(INT));

#ifdef DEBUG_DBSWAB
	    if (debugging_key_swabs)
		printf ("  slot #%02d: prefix=%d",
		    (int)slotno, (int)key_prefix);
#endif
	    /* database address.
	     * All DtSearch keys are unswapped char strings.
	     * The dba is just past the key, 8 bytes before
	     * of the end of the slot.
	     */
	    cptr = slotptr + slsize - 8;

	    memcpy (&align_LONG, cptr, sizeof(LONG));
	    align_LONG = HTONL (align_LONG);
	    memcpy (cptr, &align_LONG, sizeof(LONG));

#ifdef DEBUG_DBSWAB
	    if (debugging_key_swabs) {
		printf (" dba=x%08lx key='%s'\n", (direction == NTOH)?
		    (long)align_LONG : (long)ntohl(align_LONG),
		    slotptr + 2);
		fflush (stdout);
	    }
#endif

	    /* 'child ptr'.  after dba, 4 bytes before end of slot. */
	    cptr += 4;
	    memcpy (&align_LONG, cptr, sizeof(LONG));
	    align_LONG = HTONL (align_LONG);
	    memcpy (cptr, &align_LONG, sizeof(LONG));

	} /* end loop on each slot */
    } /* end if (key page) */

    return;
#endif /* BYTE_SWAP */
}  /* swab_page() */

/********************* DBSWAB.C **********************************/
