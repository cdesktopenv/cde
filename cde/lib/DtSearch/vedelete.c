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
/*
 *   COMPONENT_NAME: austext
 *
 *   FUNCTIONS: compare_dba
 *		d99_to_dba
 *		delete_addrs
 *		print_counters
 *		print_dots
 *		ve_delete
 *
 *   ORIGINS: 27
 *
 *
 *   (C) COPYRIGHT International Business Machines Corp. 1993,1995
 *   All Rights Reserved
 *   Licensed Materials - Property of IBM
 *   US Government Users Restricted Rights - Use, duplication or
 *   disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 */
/*************************** VEDELETE.C ****************************
 * $XConsortium: vedelete.c /main/6 1996/08/12 13:21:16 cde-ibm $
 * Implements the opera OE_DELETE_RECID and OE_DELETE_BATCH functions.
 * For each dba in usrblk.dbatab (count is in usrblk.dbcount):
 * deletes header rec, text blobs, and user notes; 
 * database addresses on inverted index file;
 * and associated words and stems btree references.
 *
 * $Log$
 * Revision 2.2  1995/10/26  14:22:06  miker
 * Added prolog.
 *
 * Revision 2.1  1995/09/22  22:29:07  miker
 * Freeze DtSearch 0.1, AusText 2.1.8
 *
 * Revision 1.10  1995/09/05  19:22:46  miker
 * Made usrblk, ausapi_msglist global.  Deleted obsolete socblk refs.
 * Minor name changes for DtSearch.
 *
 * Revision 1.9  1995/07/18  22:30:58  miker
 * Delete msglist arg from vista_abort() function calls.
 */
#include "SearchE.h"
#include <ctype.h>
#include <stdlib.h>
#include "vista.h"

#define PROGNAME	"VEDELETE"
#define SKIP_UPDATE	0
#define UPDATE_WORD	1
#define	DELETE_WORD	2
#define WORDS_PER_DOT	1000
#define	MAX_REC_READ	128	/* Max number of addresses to be read from
				 * database addresses file. 128 *
				 * sizeof(DB_ADDR) = 512 bytes. This is the
				 * size of one block read from hard disk. */

/************  GLOBAL VARIABLES  ***************/
static struct or_hwordrec got_hword;
static struct or_lwordrec got_lword;
static struct or_swordrec got_sword;
static int      vistano;

static FILE    *dtbs_addr_fp;
static int      debugging = FALSE;
static int      verbose_debugging = FALSE;
static char    *keyptr = NULL;
static long     updated_words, deleted_words, skipped_words;
static long     freed_bytes;
static DtSrINT16
		recslots;
static long	word_count, dot_count;


/********************************/
/*				*/
/*	   print_dots		*/
/*				*/
/********************************/
/* Called for every single database word.
 * Prints a dot for every WORDS_PER_DOT word,
 * a space every 10 dots, and a new line every 50 dots.
 */
static void	print_dots (void)
{
    word_count++;
    if (word_count % WORDS_PER_DOT == 0) {
	word_count = 0;
	fputc ('.', aa_stderr);
	dot_count++;
	if (dot_count % 50 == 0) {
	    fputc ('\n', aa_stderr);
	    dot_count = 0;
	}
	else if (dot_count % 10 == 0)
	    fputc (' ', aa_stderr);
	fflush (aa_stderr);
    }
    return;
} /* print_dots() */


/********************************/
/*				*/
/*	    d99_to_dba		*/
/*				*/
/********************************/
/* Convert d99 dba format (the record NUMBER in the high
 * 3 bytes, and statistical info in lower byte)
 * to vista dba format (d00 file number in high byte,
 * slot number in lower 3 bytes).  The conversion
 * accounts for the number of slots each record requires,
 * as well as an offset for the dbrec in slot #1.
 * (There is no slot #0 in vista; that's a NULL_DBA).
 * Formula: subtract 1, multiply by num reclots per rec, add 2.
 */
static DB_ADDR	d99_to_dba (DB_ADDR d99)
{
#define D00_HIBYTE	((DB_ADDR) OR_D00 << 24)
    /* Move hi 3 bytes to lo 3 bytes and zero out hi byte */
    d99 >>= 8;
return D00_HIBYTE | ((--d99) * (DB_ADDR) recslots + 2);
}	/* d99_to_dba() */


/********************************/
/*				*/
/*	   compare_dba		*/
/*				*/
/********************************/
static int      compare_dba (const void *dba1, const void *dba2)
{
    DB_ADDR        *i = (DB_ADDR *) dba1;
    DB_ADDR        *j = (DB_ADDR *) dba2;

    if (*i < *j)
	return -1;
    if (*i > *j)
	return 1;
    return 0;
}	/* compare_dba() */


/********************************/
/*				*/
/*	delete_addrs		*/
/*				*/
/********************************/
/* This function is called for each word in the database.
 * It reads and rewrites all the dbas in the d99 file for the word,
 * deleting those that are listed in the usrblk dba table.
 * Return - UPDATE_WORD if there are still some dba's in the
 * d99 file for this word.
 * Return - DELETE_WORD if all the dba's in the d99 were in
 * the usrblk table.
 * Return - SKIP_UPDATE if none of the dba's in the d99 file
 * were in the usrblk table.
 */
static int      delete_addrs (void)
{
    DB_ADDR         copy_addrs[MAX_REC_READ];
    DB_ADDR         word_addrs[MAX_REC_READ];
    DB_ADDR         vista_dba;
    char           *dba_eq;
    int             not_done = TRUE;
    int		i, addrs_found;
    size_t	num_reads, num_writes, ncopy;
    DtSrINT32	num_addrs, num_found;
    long	read_offset, write_offset;

    if (fseek (dtbs_addr_fp, got_hword.or_hwoffset, SEEK_SET)) {
	printf (PROGNAME "80 "
	    "fseek failed on '%s%s.d99'.  Offset=%ld.  Word='%s'.\n",
	    usrblk.dblk->path, usrblk.dblk->name,
	    (long) got_hword.or_hwoffset, keyptr);
	DtSearchExit (99);
    }
    write_offset = ftell (dtbs_addr_fp);
    if (got_hword.or_hwoffset != write_offset) {
	printf (PROGNAME "88 ftell failed on '%s%s.d99'."
	    "  hwoffset=%ld  write_offset=%ld.\n",
	    usrblk.dblk->path, usrblk.dblk->name,
	    (long)got_hword.or_hwoffset, write_offset);
	DtSearchExit (99);
    }
    num_addrs = got_hword.or_hwaddrs;
    addrs_found = FALSE;
    num_found = 0;

    /* OUTER LOOP: for each block of dbas */
    while (not_done) {
	ncopy = 0;
	/*
	 * read only a single block of max recs, or fewer dba's if this is
	 * the last block 
	 */
	if (num_addrs > MAX_REC_READ) {
	    num_reads = MAX_REC_READ;
	    num_addrs -= MAX_REC_READ;
	}
	else {
	    not_done = FALSE;
	    num_reads = num_addrs;
	}
	if (fread (word_addrs, sizeof (DB_ADDR), num_reads, dtbs_addr_fp)
	    != num_reads) {
	    puts ("\n\r" PROGNAME "98 "
		"AusText is shutting down due to corrupted k2x/d99 links.");
	    DtSearchExit (99);
	}

	/* INNER LOOP: each dba in this block */
	for (i = 0; i < num_reads; i++) {
	    vista_dba = d99_to_dba (ntohl (word_addrs[i]));
	    dba_eq = (char *) bsearch (
		&vista_dba,	/* key to search for */
		usrblk.dbatab,	/* start of the table */
		usrblk.dbacount,	/* # elems in the table */
		sizeof (DB_ADDR),	/* size of each elem */
		compare_dba);	/* my compare function */

	    if (dba_eq != NULL) {
		/*
		 * HIT! the passed word links to this dba. Don't copy the dba
		 * to copy array. 
		 */
		num_found++;
		addrs_found = TRUE;
	    }
	    else {
		/* MISS! Keep the dba by copying it to the copy array */
		copy_addrs[ncopy++] = htonl (word_addrs[i]);
	    }
	}			/* end INNER LOOP for each dba in curr block */

	if (addrs_found) {
	    /*
	     * Once we start deleting dba's: save the curr 'read' loc, write
	     * the shorter 'copy' array to the output file at the curr
	     * 'write' loc, update the 'write' loc ptr, restore the 'read'
	     * loc. 
	     */
	    read_offset = ftell (dtbs_addr_fp);
	    if (read_offset <= 0L) {
		printf (PROGNAME "169 ftell failed on '%s%s.d99'.\n",
		    usrblk.dblk->path, usrblk.dblk->name);
		DtSearchExit (99);
	    }
	    if (fseek (dtbs_addr_fp, write_offset, SEEK_SET)) {
		printf (PROGNAME "175 fseek to %ld failed on '%s%s.d99'.\n",
		    write_offset, usrblk.dblk->path, usrblk.dblk->name);
		DtSearchExit (99);
	    }
	    num_writes = fwrite (copy_addrs, sizeof (DB_ADDR),
		ncopy, dtbs_addr_fp);
	    if (num_writes != ncopy) {
		printf (PROGNAME "283 fwrite at pos %ld failed on '%s%s.d99'.\n"
		    "  Wrote %lu dba's instead of %lu dba's.\n",
		    write_offset, usrblk.dblk->path, usrblk.dblk->name,
		    (unsigned long) num_writes, (unsigned long) ncopy);
		DtSearchExit (99);
	    }
	    write_offset = ftell (dtbs_addr_fp);
	    if (write_offset <= 0L) {
		printf (PROGNAME "191 ftell failed on '%s%s.d99'.\n",
		    usrblk.dblk->path, usrblk.dblk->name);
		DtSearchExit (99);
	    }
	    if (fseek (dtbs_addr_fp, read_offset, SEEK_SET)) {
		printf (PROGNAME "198 fseek to %ld failed on '%s%s.d99'.\n",
		    read_offset, usrblk.dblk->path, usrblk.dblk->name);
		DtSearchExit (99);
	    }
	}			/* end if (addrs_found) */
	else {
	    /* [do nothing, leave this block as is, just incr write ptr] */
	    write_offset = ftell (dtbs_addr_fp);
	    if (write_offset <= 0L) {
		printf (PROGNAME "208 ftell failed on '%s%s.d99'.\n",
		    usrblk.dblk->path, usrblk.dblk->name);
		DtSearchExit (99);
	    }
	}
    }				/* end OUTER LOOP for this word */

    if (addrs_found) {
	got_hword.or_hwaddrs -= num_found;
	got_hword.or_hwfree += num_found;
	if (got_hword.or_hwaddrs < 0) {
	    printf (PROGNAME "220 duplicate dbas for '%s' in '%s%s.d99'.\n",
		keyptr, usrblk.dblk->path, usrblk.dblk->name);
	    DtSearchExit (99);
	}
	if (got_hword.or_hwaddrs == 0) {
	    deleted_words++;
	    freed_bytes += sizeof (DB_ADDR) * got_hword.or_hwfree;
	    return DELETE_WORD;
	}
	else {
	    /*
	     * @@@ fill out newly freed dba positions with FF's as debugging
	     * aid? 
	     */
	    updated_words++;
	    return UPDATE_WORD;
	}
    }		/* end if (addrs_found) */
    skipped_words++;
    return SKIP_UPDATE;
}	/* delete_addrs() */


/****************************************/
/*					*/
/*	      print_counters		*/
/*					*/
/****************************************/
/* Initializes accumulators, or generates msg
 * of accumulator status and empties the accumulators.
 */
static void     print_counters (char *wordtype)
{
    char            msgbuf[256];
    static long     totupdwords, totdelwords, totskipwords;
    static long     totfreedbytes;

    if (wordtype == NULL) {
	/* initialize buckets */
	totupdwords = totdelwords = totskipwords = 0L;
	updated_words = deleted_words = skipped_words = 0L;
	freed_bytes = totfreedbytes = 0L;
    }

    else if ((long) wordtype == -1L) {
	/* final totals msg */
	sprintf (msgbuf, PROGNAME " Batch Totals: "
	    "updated %ld, deleted %ld, unaffected %ld.\n"
	    "  Old word count = %ld, New word count = %ld.\n"
	    "  Freed %ld total bytes in d99 file.\n"
	    ,totupdwords, totdelwords, totskipwords
	    ,totupdwords + totdelwords + totskipwords
	    ,totupdwords + totskipwords
	    ,totfreedbytes);
	DtSearchAddMessage (msgbuf);
    }

    else {
	/* regular progress msg */
	sprintf (msgbuf, PROGNAME " %ld %s Words: "
	    "%ld updated, %ld deleted, %ld unaffected.\n"
	    "  Freed %ld bytes in d99 file."
	    ,updated_words + deleted_words + skipped_words
	    ,wordtype
	    ,updated_words, deleted_words, skipped_words
	    ,freed_bytes
	    );
	DtSearchAddMessage (msgbuf);
	totupdwords += updated_words;
	totdelwords += deleted_words;
	totskipwords += skipped_words;
	totfreedbytes += freed_bytes;
	updated_words = deleted_words = skipped_words = freed_bytes = 0L;
    }
    return;
}	/* print_counters() */


/****************************************/
/*					*/
/*	        ve_delete		*/
/*					*/
/****************************************/
/* First do Gendler's code: words, stems, and inverted indexes.
 * If it goes down, at least the database itself won't be corrupted.
 * Then do Russell's code: delete text blobs, misc recs,
 * and the object records.  If it goes down here, only one
 * or two records will be corrupted and none of Gendler's
 * word searches will retrieve records yet to be deleted.
 * The total database record count is adjusted between the
 * loops because it's only used for Gendler's code.
 * Addresses in usrblk.dbatab are PRESUMED VALID object records.
@@@@@@ still one bug left.  Semantic inverted index files (d97, d98)
@@@@@@ not yet updated.  But cborodin reindexes everything every time
@@@@@@ it runs so only failures will occur for semantic searches (rare),
@@@@@@ after tomita deletions (rare), before cborodin runs (common).
@@@@@@ Since this window is very small and no database corruption
@@@@@@ can occur, by order of management I'm to fix it later.
@@@@@@ The fix is: include the same module used in cborodin to reindex
@@@@@@ completely.  On a 3 gig database that takes about 2 minutes.
 */
void            ve_delete (void)
{
    DB_ADDR         dba;
    char            charbuf[200 + DtSrMAX_DB_KEYSIZE];
    int             i, j;
    int             ret_code;
    DtSrINT32	*reccount = &usrblk.dblk->dbrec.or_reccount;
    DtSrINT16	maxwordsz = usrblk.dblk->dbrec.or_maxwordsz;
    int		dbacount = usrblk.dbacount;

    if (dbacount < 1) {
NOTHING_TO_DO:
	DtSearchAddMessage (PROGNAME"336 "
	    "Database address table is empty; nothing to delete!");
	usrblk.retncode = OE_NOOP;
	return;
    }

    recslots = usrblk.dblk->dbrec.or_recslots;
    vistano = usrblk.dblk->vistano;
    dtbs_addr_fp = usrblk.dblk->iifile;
    debugging = ((usrblk.debug & USRDBG_DELETE) != 0L);
    verbose_debugging = ((usrblk.debug & USRDBG_VERBOSE) != 0L);

    print_counters (NULL);	/* initialize all counters */
    if (debugging) {
	fprintf (aa_stderr, PROGNAME "185 "
	    "db='%s' vistano=%d maxwordsz=%d dbacount=%d.\n",
	    usrblk.dblk->name, vistano, (int)maxwordsz, dbacount);
	if (verbose_debugging)
	  for (i = 0; i < dbacount; i++) {
	    dba = usrblk.dbatab[i];
	    CRSET (PROGNAME "178", &dba, vistano);
	    CRREAD (PROGNAME "179", OR_OBJKEY, charbuf, vistano);
	    fprintf (aa_stderr, " #%d\tdba=%ld:%ld  key='%s'\n",
		i, dba>>24, dba & 0xffffff, charbuf);
	  }
    }
    /* Sort the array of database addresses.
     * After sorting, eliminate any duplicate dba's
     * and adjust dbacount if necessary.
     */
    qsort (usrblk.dbatab, (size_t)dbacount, sizeof(DB_ADDR), compare_dba);
    for (i = 1;  i < dbacount;  i++) {
	if (usrblk.dbatab[i-1] != usrblk.dbatab[i])
	    continue;
	dba = usrblk.dbatab[i];
	sprintf (charbuf, PROGNAME"370 Duplicate dba = %ld:%ld ignored.",
	    (long)dba>>24, (long)(dba & 0x00ffffff));
	DtSearchAddMessage (charbuf);
	if (debugging)
	    fprintf (aa_stderr, "%s\n", charbuf);
	/* subloop moves rest of table up by one */
	for (j = i + 1;  j < dbacount;  j++)
	    usrblk.dbatab[j-1] = usrblk.dbatab[j];
	dbacount--;
	usrblk.dbacount = dbacount;	/* in case caller uses this */
    }
    if (dbacount < 1)
	goto NOTHING_TO_DO;

    if (debugging) {
	fprintf (aa_stderr, PROGNAME"355 "
	    "After sorting dbatab, dbacount=%d.\n", dbacount);
	if (verbose_debugging)
	  for (i = 0; i < dbacount; i++) {
	    dba = usrblk.dbatab[i];
	    CRSET (PROGNAME "358", &dba, vistano);
	    CRREAD (PROGNAME "359", OR_OBJKEY, charbuf, vistano);
	    fprintf (aa_stderr, " #%d\tdba=%ld:%ld  key='%s'\n",
		i, (long)dba>>24, (long)(dba & 0x00ffffff), charbuf);
	  }
    }

    /*
     * There are 3 identical Gendler Loops, one for each possible group of
     * word sizes: swords, lwords, and hwords. To speed up this lengthy
     * process, only those loops will be executed as determined by database's
     * maxwordsz. However delete_addrs() function always uses the
     * buffer for huge words, so the necessary fields will be copied into
     * that buffer for the smaller word sizes. 
     */

    /*--------- Gendler's SWORD Loop ---------*/
    /* every database has short words */
    fprintf (aa_stderr, PROGNAME "368 "
	"Entering SHORT word loop.  Each dot = %d words.\n",
	WORDS_PER_DOT);
    fflush (aa_stderr);
    word_count = 0;
    dot_count = 0;

    updated_words = skipped_words = deleted_words = 0L;
    freed_bytes = 0L;
    KEYFRST (PROGNAME "203", OR_SWORDKEY, vistano);
    while (db_status == S_OKAY) {
	print_dots();
	RECREAD (PROGNAME "182", &got_sword, vistano);
	got_hword.or_hwoffset =		ntohl (got_sword.or_swoffset);
	got_hword.or_hwaddrs =		ntohl (got_sword.or_swaddrs);
	got_hword.or_hwfree =		ntohl (got_sword.or_swfree);
	keyptr = got_sword.or_swordkey;
	ret_code = delete_addrs();
	if (ret_code == UPDATE_WORD) {
	    got_sword.or_swoffset =	htonl (got_hword.or_hwoffset);
	    got_sword.or_swaddrs =	htonl (got_hword.or_hwaddrs);
	    got_sword.or_swfree =	htonl (got_hword.or_hwfree);
	    RECWRITE (PROGNAME "183", &got_sword, vistano);
	}
	else if (ret_code == DELETE_WORD) {
	    DELETE (PROGNAME "184", vistano);
	}
	KEYNEXT (PROGNAME "196", OR_SWORDKEY, vistano);
    }				/* end loop on each sword */

    print_counters ("Short");
    /* end SWORDs */

    /*--------- Gendler's LWORD Loop ---------*/
    fprintf (aa_stderr, PROGNAME "398 "
	"Entering LONG word loop.  Each dot = %d words.\n",
	WORDS_PER_DOT);
    fflush (aa_stderr);
    word_count = 0;
    dot_count = 0;

    updated_words = skipped_words = deleted_words = 0L;
    freed_bytes = 0L;
    KEYFRST (PROGNAME "243", OR_LWORDKEY, vistano);
    while (db_status == S_OKAY) {
	print_dots();
	RECREAD (PROGNAME "246", &got_lword, vistano);
	got_hword.or_hwoffset =		ntohs (got_lword.or_lwoffset);
	got_hword.or_hwaddrs =		ntohs (got_lword.or_lwaddrs);
	got_hword.or_hwfree =		ntohs (got_lword.or_lwfree);
	keyptr = got_lword.or_lwordkey;
	ret_code = delete_addrs ();
	if (ret_code == UPDATE_WORD) {
	    got_lword.or_lwoffset =	htons (got_hword.or_hwoffset);
	    got_lword.or_lwaddrs =	htons (got_hword.or_hwaddrs);
	    got_lword.or_lwfree =	htons (got_hword.or_hwfree);
	    RECWRITE (PROGNAME "252", &got_lword, vistano);
	}
	else if (ret_code == DELETE_WORD) {
	    DELETE (PROGNAME "256", vistano);
	}
	KEYNEXT (PROGNAME "258", OR_LWORDKEY, vistano);
    } /* end LONGs */
    print_counters ("Long");

    /*--------- Gendler's HWORD Loop --------- */
    fprintf (aa_stderr, PROGNAME "429 "
	"Entering HUGE word loop.  Each dot = %d words.\n",
	WORDS_PER_DOT);
    fflush (aa_stderr);
    word_count = 0;
    dot_count = 0;

    updated_words = skipped_words = deleted_words = 0L;
    freed_bytes = 0L;
    KEYFRST (PROGNAME "280", OR_HWORDKEY, vistano);
    while (db_status == S_OKAY) {
	print_dots();
	RECREAD (PROGNAME "284", &got_hword, vistano);
	NTOHS (got_hword.or_hwoffset);
	NTOHS (got_hword.or_hwaddrs);
	NTOHS (got_hword.or_hwfree);
	keyptr = got_hword.or_hwordkey;
	ret_code = delete_addrs ();
	if (ret_code == UPDATE_WORD) {
	    HTONS (got_hword.or_hwoffset);
	    HTONS (got_hword.or_hwaddrs);
	    HTONS (got_hword.or_hwfree);
	    RECWRITE (PROGNAME "289", &got_hword, vistano);
	}
	else if (ret_code == DELETE_WORD) {
	    DELETE (PROGNAME "293", vistano);
	}
	KEYNEXT (PROGNAME "295", OR_HWORDKEY, vistano);
    } /* end HUGEs */
    print_counters ("Huge");

    /* print final batch totals */
    print_counters ((char *) -1);

    /*--------- Russell's Loop ---------*/
    fprintf (aa_stderr,
	PROGNAME"470 All words processed.  Now deleting %d objects...\n",
	dbacount);
    for (i = 0; i < dbacount; i++) {
	dba = usrblk.dbatab[i];

	if (verbose_debugging) {
	    fprintf (aa_stderr, PROGNAME "471 "
		"Deleting object #%2d,\tdba %ld:%ld.\n",
		i, (long)dba>>24, (long)(dba & 0x00ffffff));
	    fflush (aa_stderr);
	}
	/* Delete blobs.  Use real d_csoset at first call
	 * so we can print out fancy error msg.
	 */
	d_csoset (OR_OBJ_BLOBS, &dba, vistano);
	if (db_status != 0) {
	    fputs (vista_msg(PROGNAME"152"), aa_stderr);
	    fprintf (aa_stderr, PROGNAME"153 Abort: "
		"vistano=%d, dbatab #%d (out of %d), dba=%ld:%ld.\n",
		vistano, i, dbacount, (long)dba>>24, (long)(dba & 0x00ffffff));
	    fflush (aa_stderr);
	    DtSearchExit (53);
	}
	FINDFM (PROGNAME "155", OR_OBJ_BLOBS, vistano);
	while (db_status != S_EOS) {
	    DISDEL (PROGNAME "158", vistano);
	    FINDFM (PROGNAME "159", OR_OBJ_BLOBS, vistano);
	}

	/* Delete misc records (old user notes, abstracts, fzkeys) */
	CSOSET (PROGNAME "142", OR_OBJ_MISCS, &dba, vistano);
	FINDFM (PROGNAME "145", OR_OBJ_MISCS, vistano);
	while (db_status != S_EOS) {
	    DISDEL (PROGNAME "148", vistano);
	    FINDFM (PROGNAME "149", OR_OBJ_MISCS, vistano);
	}

	/* Pull the final plug by deleting the object record itself */
	CRSET (PROGNAME "200", &dba, vistano);
	DISDEL (PROGNAME "201", vistano);
    }				/* end of Russell's loop */

    /* Adjust total record count */
    *reccount -= dbacount;
    if (*reccount < 0)
	*reccount = 0;
    RECFRST (PROGNAME "355", OR_DBREC, vistano);	/* seqtl retrieval */
    if (db_status != S_OKAY)
	vista_abort (PROGNAME"356");
    HTONL (*reccount);
    CRWRITE (PROGNAME "341", OR_RECCOUNT, reccount, vistano);
    if (db_status != S_OKAY)
	vista_abort (PROGNAME"342");

    usrblk.retncode = OE_OK;
    return;
}	/* ve_delete() */

/*************************** VEDELETE.C ****************************/
