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
/* $XConsortium: boolsrch.c /main/4 1996/09/23 21:00:18 cde-ibm $
 *
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1996 Hewlett-Packard Company.
 * (c) Copyright 1996 International Business Machines Corp.
 * (c) Copyright 1996 Sun Microsystems, Inc.
 * (c) Copyright 1996 Novell, Inc. 
 * (c) Copyright 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 */
/*
 *   COMPONENT_NAME: austext
 *
 *   FUNCTIONS: boolean_search
 *		calc_result_bitvec_WK
 *		calculate_idfs
 *		dbread_filter_WK
 *		get_proximity
 *		got_USR_STOPSRCH
 *		load_DtSrResults_WK
 *		load_or_wordrecs
 *		read_d99
 *		read_recno
 *		read_stem_bitvec_WK
 *		stuff_DtSrResult
 *		weights_filter_WK
 *
 *   ORIGINS: 27
 *
 *
 *   (C) COPYRIGHT International Business Machines Corp. 1996
 *   All Rights Reserved
 *   Licensed Materials - Property of IBM
 *   US Government Users Restricted Rights - Use, duplication or
 *   disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 */
/********************* BOOLSRCH.C **********************
 * $Id: boolsrch.c /main/4 1996/09/23 21:00:18 cde-ibm $
 * February 1996.
 * The vista code from the original vewords.c.
 * Given a final truth table and stems array from the user's boolean
 * query (output of boolean_search()), find all database records
 * containing the truth table's set operations and return
 * their database addresses in a resultlist.
 * See boolpars.h for format and limitations of TRUTHTAB.
 *
 *-------------- D99DBA TO DBA CONVERSION ----------------
 * 'd99dbas' are not real vista dbas!  They were modified
 * as follows to permit shorter bit vectors,
 * and to minimize bit shifts at search time.
 *     vista_dba   <-  (OR_D00 << 24) | vista_slot
 *     vista_slot  <-  ((d99recno - 1) * or_recslots) + 2
 *     d99dba      <-  (d99recno << 8) | weight_byte
 *     d99recno    <-  ((vista_slot - 2) / or_recslots) + 1
 * The d99 and bitvec recno of the first rec is 1.
 * The slotno (vista dba) of the first rec is 2
 * (dbrec occupies first slot and vista slots begin at 1).
 *
 * $Log$
 * Revision 1.5  1996/03/20  19:21:49  miker
 * Completed collocations code.  Restored get_colloc_bitvec() from colloc.c.
 *
 * Revision 1.4  1996/03/18  22:06:24  miker
 * Bug fix.  Zero permute NOT queries always returned no hits.
 *
 * Revision 1.3  1996/03/13  23:05:24  miker
 * Change long double constant to regular float for better portability.
 *
 * Revision 1.2  1996/03/13  22:36:37  miker
 * Changed char to UCHAR several places; similar typecasts.
 * Moved collocations processing to colloc.c.
 *
 * Revision 1.1  1996/03/05  15:52:06  miker
 * Initial revision
 */
/***#define _ALL_SOURCE****/     /* to pickup typedefs for shm vnodes */
#include "SearchE.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "vista.h"
#include "boolpars.h"

#define	PROGNAME	"BOOLSRCH"
#define INIT_ITERATIONS	50
#define MS_boolsrch	16
/*
 * DBAS_PER_BLOCK is the max number of dbas to be read
 * from d99 file.  Note DBAS_PER_BLOCK * sizeof(DB_ADDR) = 512 bytes,
 * the standard blksize of one hard disk block.
 */
#define DBAS_PER_BLOCK	128

#define RESET_BIT(bv, by, bm)	bv[by] &= (UCHAR) ~bm

#if (DtSrMAX_STEMCOUNT != 8)
#error DtSrMAX_STEMCOUNT does not equal 8.
#endif

/****************************************/
/*					*/
/*		  PROXWT		*/
/*					*/
/****************************************/
typedef struct {
    float	wt;
    long	byteno;
    int		bitmask;
    int		proximity;
    }	PROXWT;

/****************************************/
/*					*/
/*		  GLOBALS		*/
/*					*/
/****************************************/
int			debugging_boolsrch =	FALSE;

static int		all_key_types =		TRUE;
static UCHAR		*bitvec_allocp =	NULL;
static size_t		bitvec_allocsz =	0;
static long		bitveclen;	/* 1/8 of tot_addr_count */
static UCHAR		*bitvecs [DtSrMAX_STEMCOUNT];
static int		check_dates =		FALSE;
static int		do_stat_sort =		FALSE;
static double		idf [DtSrMAX_STEMCOUNT];
static char		*msgbuf =		NULL;
static int		need_zero_permute =	FALSE;
static struct or_objrec	objrec;
static DB_ADDR		objrecdba;
static int		or_abstrsz =		0;
static int		or_fzkeysz =		0;
static short		or_language =		DtSrLaENG;
static long		or_maxdba;	/* largest dba in database */
static long		or_reccount;	/* tot num db obj (real_num_rec) */
static long		or_recslots;	/* D00 slots per obj (slot_d00) */
static struct or_hwordrec
			*or_wordrecs =		NULL;
static PROXWT		*proxwts =		NULL;
static int		proxwtct;
static UCHAR		*result_bitvec;
static long		result_count =		0;
static DtSrResult	*resultlist =		NULL;
static int		save_stemno =		0;
static long		tot_addr_count; /* may be > reccount bcs deletes */
static int		vistano;
static float		*wtvec =		NULL;

extern void     find_keyword (char *cur_word, int vista_num);
extern void     read_wordstr (struct or_hwordrec * glob_word, int vista_num);

/************************************************/
/*						*/
/*		got_USR_STOPSRCH		*/
/*						*/
/************************************************/
/* Called at beginning of every workproc.
 * Returns TRUE if user pushed STOP SEARCH button,
 * else FALSE.
 */
static int	got_USR_STOPSRCH (void)
{
    if ((usrblk.flags & USR_STOPSRCH) == 0)
	return FALSE;
    if (OE_flags & OE_AUDIT)
	oe_write_audit_rec (-1L);
    usrblk.retncode = OE_USER_STOP;
    return TRUE;
}

/****************************************/
/*					*/
/*	       read_recno		*/
/*					*/
/****************************************/
/* Utility function.
 * Reads a database record given a d99 record number.
 * Returns TRUE and loads globals objrec and objrecdba
 * on success, else returns FALSE.
 */
static int	read_recno (long recno)
{
    /* Convert recno to a real dba */
    objrecdba = (recno - 1) * or_recslots + 2;
    if (objrecdba >= or_maxdba)
	    return FALSE;
    objrecdba |= (OR_D00 << 24);

    /* Read the object record.
     * Skip records with database read errors.
     * Use d_crset instead of CRSET and d_recread
     * instead of RECREAD to trap vista errors
     * without aborting.
     */
    d_crset (&objrecdba, vistano);
    if (db_status != S_OKAY) {
BAD_DBA:
	if (debugging_boolsrch) {
	    fprintf (aa_stderr,
		PROGNAME"434 Invalid dba %ld.  "
		"recno=%ld bitvec[%ld]=%02x  db_status=%d.\n",
		objrecdba, recno, recno>>3, 1<<(recno%8), db_status);
	    fflush (aa_stderr);
	}
	return FALSE;
    }
    d_recread (&objrec, vistano);
    if (db_status != S_OKAY)
	goto BAD_DBA;
    swab_objrec (&objrec, NTOH);
    return TRUE;
} /* read_recno() */


/************************************************/
/*						*/
/*		 calculate_idfs			*/
/*						*/
/************************************************/
/* Subroutine of boolean_search() initialization.
 * Loads idf[] (inverse doc frequency) for each stem.
 * IDF = 1.0 for a word that occurs in every record.
 * For a word that occurs only once in entire database:
 *  NUM OF DB RECS    IDF OF SINGULAR WORD
 *	        10	 4.32
 *	       100	 7.64
 *	     1,000	10.97
 *	    10,000	14.29
 *	   100,000	17.61
 *	 1,000,000	20.93
 *	10,000,000	24.25
 */
static void	calculate_idfs (void)
{
    int		i;
    double	dbl;

    for (i = 0;  i < saveusr.stemcount;  i++) {
	if (	or_wordrecs[i].or_hwaddrs == 0  ||
		or_wordrecs[i].or_hwordkey[0] == '@')
	    idf[i] = 0.0;
	else {
	    /* ln(2) = 0.693147181 */
	    dbl =  (double) or_reccount / (double) or_wordrecs[i].or_hwaddrs;
	    idf[i] = log(dbl)  /  0.693147181  +  1.0;
	    if (debugging_boolsrch)
		fprintf (aa_stderr,
		    PROGNAME"733 IDF[%d]  numdocs=%5ld  idf=%lf\n",
		    i, (long) or_wordrecs[i].or_hwaddrs, idf[i]);
	}
    }
    return;
} /* calculate_idfs() */


/************************************************/
/*						*/
/*		load_or_wordrecs		*/
/*						*/
/************************************************/
/* Subroutine of boolean_search() initialization.
 * Loads or_wordrecs[] array with vista key file
 * records for each term in saveusr.stems.
 * Returns TRUE on success.  Else returns FALSE with
 * appropriate usrblk.retncode and user msgs on msglist.
 */
static int	load_or_wordrecs (void)
{
    int		i, j, k;
    int		stemno;
    struct or_hwordrec
		*wordrec;
    int		colloc_count =	0;
    int		not_found_count =	0;

    if (or_wordrecs)
	free (or_wordrecs);
    or_wordrecs = austext_malloc (
	saveusr.stemcount * sizeof (struct or_hwordrec) + 16,
	PROGNAME "782", NULL);

    for (stemno = 0; stemno < saveusr.stemcount; stemno++) {

	wordrec = &or_wordrecs [stemno];

	/* If this is a collocation term,
	 * save the two indexes and the collocation
	 * value in the wordrec buffer instead of usual
	 * offsets and dba counts.
	 */
	if (saveusr.stems[stemno][0] == '@') {
	    strcpy (wordrec->or_hwordkey, saveusr.stems[stemno]);
	    sscanf (saveusr.stems[stemno], COLLOC_STEM_FORMAT, &i, &j, &k);
	    wordrec->or_hwoffset = i;
	    wordrec->or_hwfree = j;
	    wordrec->or_hwaddrs = k;
	    colloc_count++;
	    continue;
	}

	if (debugging_boolsrch)
	    fprintf (aa_stderr, PROGNAME"823 KEYFIND[%d] ", stemno);
	find_keyword (saveusr.stems[stemno], vistano);
	/*
	 * If term is found, add it to the or_wordrecs[] array.
	 * But it is an error to include a word in more records
	 * than the max specified in site config file.  This is
	 * meaningful for databases where certain common high
	 * frequency words slip by which should be on the stoplist.
	 * It's possible in huge databases to run out of memory
	 * assembling very long resultlists.
	 */
	if (db_status == S_OKAY) {
	    strncpy (wordrec->or_hwordkey, saveusr.stems[stemno],
		DtSrMAXWIDTH_HWORD);
	    wordrec->or_hwordkey [DtSrMAXWIDTH_HWORD - 1] = 0;
	    read_wordstr (wordrec, vistano);
	    if (db_status != S_OKAY) {
		/* Probable corrupted database.  The btree
		 * read succeeded but the record read failed.
		 */
		sprintf (msgbuf, catgets(dtsearch_catd, MS_boolsrch, 6,
		    "%s Database Error.  Word '%s' is\n"
		    "listed in database '%s' but has no index record.") ,
		    PROGNAME"295", usrblk.stems[stemno], usrblk.dblk->label);
		DtSearchAddMessage (msgbuf);
		usrblk.retncode = OE_SYSTEM_STOP;
		if (debugging_boolsrch)
		    fprintf (aa_stderr,
			"db error, db_status = %d.\n", db_status);
		return FALSE;
	    }
	    if (debugging_boolsrch)
		fprintf (aa_stderr, "ofs=%ld addrs=%ld free=%ld\n",
		    (long) wordrec->or_hwoffset,
		    (long) wordrec->or_hwaddrs,
		    (long) wordrec->or_hwfree);
	    if (wordrec->or_hwaddrs > OE_words_hitlimit) {
		sprintf (msgbuf, catgets (dtsearch_catd, MS_boolsrch, 14,
		    "%s '%s' has more than %ld hits.\n"
		    "Please remove it from the query or raise the WHITLIM\n"
		    "value in the search engine configuration file."),
		    PROGNAME"1444", wordrec->or_hwordkey, OE_words_hitlimit);
		DtSearchAddMessage (msgbuf);
		/* Also log WHITLIM msg for administrator... */
		fprintf (aa_stderr, "%s\n", msgbuf);
		usrblk.retncode = OE_BAD_QUERY;
		return FALSE;
	    }
	}

	/* Only other possible nonfatal vista return is S_NOTFOUND.
	 * If qry_is_all_ANDs we can quit right now.
	 * Otherwise switch off all bits in the word's bit vector. 
	 */
	else if (qry_is_all_ANDs) {
	    if (debugging_boolsrch)
		fputs ("not found, qry_all_ANDs, quit.\n", aa_stderr);
	    usrblk.retncode = OE_NOTAVAIL;
	    return FALSE;
	}

	else {
	    memset (wordrec, 0, sizeof(struct or_hwordrec));
	    if (debugging_boolsrch)
		fputs ("not found, addrs-->0.\n", aa_stderr);
	    not_found_count++;
	}

    } /* end loop for each term in saveusr.stems[] */

    /* It's a failure if all the user's words
     * don't exist in database.
     */
    if (not_found_count + colloc_count >= saveusr.stemcount) {
	usrblk.retncode = OE_NOTAVAIL;
	return FALSE;
    }

    return TRUE;
} /* load_or_wordrecs() */


/****************************************/
/*					*/
/*	       get_proximity		*/
/*					*/
/****************************************/
/* Subroutine of stuff_DtSrResult().
 * Given d99recno, finds proxwt[] for record,
 * calculates and returns integer proximity.
 */
static int	get_proximity (long recno)
{
    long	byteno = recno >> 3;
    int		bitmask = 1 << (recno % 8);
    int		i;
    for (i = 0;  i < proxwtct;  i++)
	if (proxwts[i].byteno == byteno && proxwts[i].bitmask == bitmask)
	    break;
    if (i >= proxwtct)
	return -1;
    return proxwts[i].proximity;
} /* get_proximity() */


/****************************************/
/*					*/
/*	     stuff_DtSrResult		*/
/*					*/
/****************************************/
/* Subroutine of load_DtSrResults_WK().
 * Loads passed DtSrResult structure with data from global objrec.
 * Performs additional vista reads as necessary to get misc recs.
 */
static void	stuff_DtSrResult (
		    DtSrResult		*new,
		    long		recno)
{
    int		m;
    int		fzkey_remaining;
    char	*src, *targ, *targend;
    static struct or_miscrec
		miscrecbuf;

    new->objflags =	objrec.or_objflags;
    new->objuflags =	objrec.or_objuflags;
    new->objsize =	objrec.or_objsize;
    new->objdate =	objrec.or_objdate;
    new->objtype =	objrec.or_objtype;
    new->objcost =	objrec.or_objcost;
    new->dbn =		OE_dbn;
    new->dba =		objrecdba;
    new->language =	or_language;
    strncpy (new->reckey, objrec.or_objkey, DtSrMAX_DB_KEYSIZE);
    if (do_stat_sort)
	new->proximity = get_proximity (recno);

    /* The abstract immediately follows the fuzzy key
     * in the FZKABS misc recs.  It may span several recs.
     */
    new->abstractp =	(char *) (new + 1);
    if (or_abstrsz > 0) {
	targ = new->abstractp;
	targend = targ + or_abstrsz - 1;
	fzkey_remaining = or_fzkeysz;
	CRSET (PROGNAME"226", &objrecdba, vistano);
	SETOR (PROGNAME"227", OR_OBJ_MISCS, saveusr.vistano);
	FINDFM (PROGNAME"228", OR_OBJ_MISCS, saveusr.vistano);

	while (db_status == S_OKAY) {
	    RECREAD (PROGNAME"2209", &miscrecbuf, saveusr.vistano);
	    NTOHS (miscrecbuf.or_misctype);
	    if (miscrecbuf.or_misctype == ORM_FZKABS) {
		src = (char *) miscrecbuf.or_misc;

		for (m = 0;   m < sizeof(miscrecbuf.or_misc);   m++) {

		    /* skip over the fzkey */
		    if (fzkey_remaining > 0) {
			src++;
			fzkey_remaining--;
			continue;
		    }

		    /* copy the abstract */
		    *targ = *src;
		    if (*src++ == 0 || targ++ >= targend) {
			*targ = 0;
			targ = targend;  /* force outer loop end */
			break;
		    }
		} /* end for-loop m */
	    } /* end (misctype == FZKABS) */

	    if (targ >= targend)
		break;
	    FINDNM (PROGNAME"545", OR_OBJ_MISCS, saveusr.vistano);
	} /* end while-loop */

    } /* endif: (or_abstrsz > 0) */

    return;
} /* stuff_DtSrResult() */


/****************************************/
/*					*/
/*	    load_DtSrResults_WK		*/
/*					*/
/****************************************/
/* Builds DtSrResult list for every record
 * in result_bitvec, but not more than aa_maxhits.
 */
static void	load_DtSrResults_WK (void)
{
    long		recno;
    int			bitno;
    long		byteno;
    int			i;
    long		dittocount;
    DtSrResult		*resultp;
    size_t		resultsz = sizeof(DtSrResult) + or_abstrsz + 4;

    if (got_USR_STOPSRCH())
	return;
    if (resultlist) {
	DtSearchFreeResults (&resultlist);
	resultlist = NULL;
    }

    /* Make a single pass through the final result_bitvec.
     * For each nonzero bit, ie each database record
     * that satisfies the query requirements,
     * retrieve the record and push it onto the
     * DtSrResult list.  If not sorting records,
     * stop when we reach the user's specified aa_maxhits count.
     */
    dittocount = 0;
    for (recno = 1;  recno < tot_addr_count;  recno++) {
	byteno = recno >> 3;	/* divide by 8 */
	bitno = recno % 8;

	/* Skip zero bits */
	if ((result_bitvec[byteno] & (1 << bitno)) == 0)
	    continue;

	if (!read_recno (recno))
	    continue;

	/* Create new DtSrResult node, push it onto resultlist. */
	resultp = austext_malloc (resultsz + 4, PROGNAME"466", NULL);
	memset (resultp, 0, resultsz);
	resultp->link = resultlist;
	resultlist = resultp;

	/* Load the new DtSrResult node from the object record */
	stuff_DtSrResult (resultp, recno);

	/* Check if any more reads are necessary.
	 * If not sorting, stop after aa_maxhits.
	 * If sorting, there won't be more than
	 * aa_maxhits recs in the bitvec anyway.
	 */
	dittocount++;
	if (dittocount >= aa_maxhits)
	    break;

    }  /* end bitvec loop */


    /*--------- All Done.  Clean up and return to caller. ---------*/
/*@@@@@@  make separate workproc call if aa_maxhits > 100.
  @@@@@ sort may take a long time */
    if (wtvec) {
	free (wtvec);
	wtvec = NULL;
    }
    if (proxwts) {
	free (proxwts);
	proxwts = NULL;
    }

    if (dittocount <= 0) {
	usrblk.workproc = dummy_workproc;
	usrblk.retncode = OE_NOTAVAIL;
	return;
    }

    usrblk.retncode =	OE_OK;
    usrblk.workproc =	dummy_workproc;

    usrblk.stemcount =	saveusr.stemcount;
    if (usrblk.search_type == 'W')
	memcpy (usrblk.stems, saveusr.stems,
	    saveusr.stemcount * DtSrMAXWIDTH_HWORD);
    else
	/* Don't copy first char (ctrl-o) stem */
	for (i = 0;  i < saveusr.stemcount;  i++)
	    strcpy (usrblk.stems[i], &saveusr.stems[i][1]);

    if (do_stat_sort)
	DtSearchSortResults (&resultlist, DtSrSORT_PROX);
    usrblk.dittocount =	dittocount;
    if (usrblk.dittolist)
	DtSearchFreeResults (&usrblk.dittolist);
    usrblk.dittolist =	resultlist;
    resultlist = NULL;
    return;
} /* load_DtSrResults_WK() */


/****************************************/
/*					*/
/*	    weights_filter_WK		*/
/*					*/
/****************************************/
/* This workproc is called only if we're doing statistical sorting.
 * (1) It reduces the result_bitvec to it's final size,
 * containing only the highest aa_maxhits statistical weights
 * in wtvec.
 * (2) It replaces (possibly large) wtvec with (probably much smaller)
 * array of PROXWT structures containing the selected records'
 * weights and calculated proximities, for final ranking sort.
 *
 */
static void	weights_filter_WK (void)
{
    int		i;
    double	scalefac;
    long	recno;
    int		smallest, biggest;
    float	biggestwt;
    long	byteno, smallest_byteno;
    int		bitmask, smallest_bitmask;

    if (got_USR_STOPSRCH())
	return;

    /* Init weight filtering */
    if (proxwts)
	free (proxwts);
    proxwtct = (result_count < aa_maxhits)? result_count : aa_maxhits;
    proxwts = austext_malloc (proxwtct * sizeof(PROXWT) + 4,
	PROGNAME"429", NULL);
    memset (proxwts, 0, proxwtct * sizeof(PROXWT));
    smallest = 0;
    scalefac = 0.0;
    biggestwt = 0.0;	/* biggest single wt of all docs */

    /* One pass thru entire result_bitvec */
    for (recno = 1;  recno < tot_addr_count;  recno++) {
	byteno = recno >> 3;
	bitmask = 1 << (recno % 8);

	/* Skip zero bits */
	if ((result_bitvec[byteno] & bitmask) == 0)
	    continue;

	/* Make scalefac = sum of squares of all wts in bitvec.
	 * It's possible that all or some of the weights are
	 * zero (eg queries like "~aaa" or "~aaa | bbb").
	 * In this case give them a very small positive number
	 * so we don't divide by zero later on.
	 */
	if (wtvec[recno] == 0.0)
	    wtvec[recno] = 0.1;
	scalefac += (double) wtvec[recno] * (double) wtvec[recno];

	/*
	 * The following logic first fills up the proxwts table.
	 * After that if a bitvec's weight is larger than the smallest
	 * proxwt, replace the smallest proxwt with the new weight
	 * and switch off the previous smallest in the original bitvec.
	 */

	/*
	 * Just discard rec on bitvec if it's weight
	 * is smaller than the current smallest.
	 */
	if (wtvec [recno] <= proxwts[smallest].wt) {
	    RESET_BIT (result_bitvec, byteno, bitmask);
	    result_count--;
	    continue;
	}
	/*
	 * Else discard current smallest if
	 * table full, ie it really points to something.
	 */
	if (proxwts[smallest].wt > 0.0) {
	    smallest_byteno = proxwts[smallest].byteno;
	    smallest_bitmask = proxwts[smallest].bitmask;
	    RESET_BIT (result_bitvec, smallest_byteno, smallest_bitmask);
	    result_count--;
	}

	/* Add this weight to the proxwts table. */
	proxwts [smallest] .wt =	wtvec [recno];
	proxwts [smallest] .byteno =	byteno;
	proxwts [smallest] .bitmask =	bitmask;

	/* Keep track of the table entry that has
	 * the highest weight.  This will eventually
	 * be the first sorted hit on the hitlist.
	 * It's weight/proximity will be used
	 * to scale the proximities of the
	 * other hits.
         */
	if (biggestwt < wtvec[recno]) {
	    biggestwt = wtvec[recno];
	    biggest = smallest;
	}

	/* Find the next smallest */
	smallest = 0;
	for (i = 1;  i < proxwtct;  i++) {
	    if (proxwts[i].wt < proxwts[smallest].wt)
		smallest = i;
	}

    } /* end loop on every recno */

    free (wtvec);
    wtvec = NULL;

    /* PROXIMITY CALCULATIONS.
     * In order to translate statistical weight into an AusText
     * proximity, basically you have to invert it, then scale it.
     * The statistical weight is a similarity measure: the
     * larger it is the more similar the document to the query.
     * But AusText 'proximity' is like a 'distance' measure,
     * the smaller the number the closer the document is to the query.
     *
     * First 'normalize' each document's statistical
     * weight to be a fraction between 0 and 1.  Done
     * by calculating a normalization factor,
     * the sqrt of the sum of squares of weights of all
     * docs that would have qualified for the hitlist
     * if we weren't truncating.   Note cosine-based normalization
     * factor (Pythagorean) always >= largest wt so we can
     * guarantee all normalized weights are > 0.0 and <= 1.0.
     * 
     * The proximity itself is calculated as the 'percent value'
     * that the doc is 'distant' from perfection (1.0 or 100%).
     * For example, if the normalized weight of the first record
     * is .931 then it's proximity will be 7 (100% - 93% = 7).
     *
     * The proximity of every other hit is scaled away
     * from the first because the normalization algorithm
     * tends to clump proximities when there are a lot of hits.
     * Specifically the proximity of every hit is a constant
     * scale factor (derived from the first proximity),
     * divided by it's weight.
     *
     * A "bulls eye" (normalized weight = 1.0, proximity == 0)
     * for the first hit is not allowed so scale factor will
     * not also be zero.  Otherwise *all* hits in that particular
     * results list would be bulls eyes.
     */
    scalefac = (double) biggestwt / sqrt (scalefac);
			/* normalized weight of first hit */
    scalefac = (1.0 - scalefac) * 100.0;
			/* proximity of first hit */
    if (scalefac < 1.0)
	scalefac = 1.0;
			/* No bulls eyes */
    scalefac *= (double) biggestwt * 1.2;
			/* scale factor for other hits */
    for (i = 0;  i < proxwtct;  i++) {
	proxwts[i].proximity = (int) (scalefac / (double) proxwts[i].wt);
	if (proxwts[i].proximity > 9999)
	    proxwts[i].proximity = 9999;
    }

    if (debugging_boolsrch) {
	fprintf (aa_stderr,
	    PROGNAME"489 FINAL PROXWTS proxwtct=%d bigwt=%.2f scalefac=%.2lf\n",
	    proxwtct, biggestwt, scalefac);
        for (i=0;  i<10;  i++) {
	    if (i >= proxwtct)
		break;
	    fprintf (aa_stderr,
		"  byteno=%3ld bitmask=%02x wt=%.2f prox=%d\n",
		proxwts[i].byteno, proxwts[i].bitmask,
		proxwts[i].wt, proxwts[i].proximity);
        }
	fprintf (aa_stderr, PROGNAME"499 WEIGHT RESULTS resultct=%ld  bv=\n",
	    result_count);
        for (i=0;  i<22;  i++) {
	    if (i >= bitveclen)
		break;
	    fprintf (aa_stderr, " %02x", (int) result_bitvec[i]);
        }
        fputc ('\n', aa_stderr);
        fflush (aa_stderr);
    }

    usrblk.retncode = OE_SEARCHING;
    usrblk.workproc = load_DtSrResults_WK;
    return;
} /* weights_filter_WK() */


/****************************************/
/*					*/
/*	    dbread_filter_WK		*/
/*					*/
/****************************************/
/* Called if we must remove documents from result_bitvec
 * because of keytype or date,
 */
static void	dbread_filter_WK (void)
{
    long	recno;
    long	byteno;
    int		bitmask;
    long	discards;

    if (got_USR_STOPSRCH())
	return;
    if (debugging_boolsrch) {
	discards = 0;
	fputs (PROGNAME"865 DBREAD discards (k=keytype d=date):\n", aa_stderr);
	fflush (aa_stderr);
    }

    /* One pass thru entire result_bitvec */
    for (recno = 1;  recno < tot_addr_count;  recno++) {
	byteno = recno >> 3;
	bitmask = 1 << (recno % 8);

	if ((result_bitvec[byteno] & bitmask) == 0)
	    continue;

	if (!read_recno (recno))
	    continue;

	/* Skip undesired record types */
	if (!all_key_types) {
	    if (strchr (saveusr.ktchars, objrec.or_objkey[0]) == NULL) {
		RESET_BIT (result_bitvec, byteno, bitmask);
		result_count--;
		if (debugging_boolsrch) {
		    discards++;
		    fputc ('k', aa_stderr);
		    fflush (aa_stderr);
		}
		continue;
	    }
	}

	/* Skip record if out of date range */
	if (check_dates) {
	    if (!objdate_in_range (objrec.or_objdate,
			usrblk.objdate1, usrblk.objdate2)) {
		RESET_BIT (result_bitvec, byteno, bitmask);
		result_count--;
		if (debugging_boolsrch) {
		    discards++;
		    fputc ('d', aa_stderr);
		    fflush (aa_stderr);
		}
		continue;
	    }
	}

    } /* end loop on every recno */

    if (debugging_boolsrch) {
	int	i;
	if (discards)
	    fputc ('\n', aa_stderr);
	fprintf (aa_stderr,
	    PROGNAME"857 DBREAD RESULTS discards=%ld resultct=%ld  bv=\n",
	    discards, result_count);
        for (i=0;  i<22;  i++) {
	    if (i >= bitveclen)
		break;
	    fprintf (aa_stderr, " %02x", (int) result_bitvec[i]);
        }
        fputc ('\n', aa_stderr);
        fflush (aa_stderr);
    }

    /* Determine next workproc.
     * (1) If no records survived the read db filter,
     *     we're done, return 'no hits'.
     * (2) If we're sorting, the next workproc reduces the
     *     bitvec to the aa_maxhits recs with the highest
     *     statistical weights.
     * (3) Otherwise the next workproc just loads the hitlist.
     */
    if (result_count <= 0) {
	usrblk.retncode = OE_NOTAVAIL;
	usrblk.workproc = dummy_workproc;
    }
    else if (do_stat_sort) {
	usrblk.retncode = OE_SEARCHING;
	usrblk.workproc = weights_filter_WK;
    }
    else {
	if (debugging_boolsrch)
	    fprintf (aa_stderr,
		PROGNAME"931 No sorting by statistical weights.\n");
	usrblk.retncode = OE_SEARCHING;
	usrblk.workproc = load_DtSrResults_WK;
    }
    return;
} /* dbread_filter_WK() */


/****************************************/
/*					*/
/*	  calc_result_bitvec_WK		*/
/*					*/
/****************************************/
/* Second workproc after read_stem_bitvec_WK().
 * If possible, minimizes size of truth table permutes,
 * then applies them to stem bitvecs to create result_bitvec.
 */
static void	calc_result_bitvec_WK (void)
{
    int		mask;
    int		cpm;
    long	byteno;
    int		bitno, stemno;
    UCHAR	permute;
    UCHAR	my_permutes [256];
    int		my_pmsz;
    int		i;

    if (got_USR_STOPSRCH())
	return;

    /* If there are fewer than a full complement of stems,
     * minimize size of truth table by discarding
     * permutes that refer to unused stems.
     */
    if (saveusr.stemcount < DtSrMAX_STEMCOUNT) {
	/* Set high order bits of mask to mark unused stem positions */
	mask = 0;
	for (i = 0;  i < saveusr.stemcount;  i++)
	    mask |= 1 << i;
	mask = ~mask;

	/* 'cpm' is a candidate permute */
	my_pmsz = 0;
	for (cpm = 0;  cpm < 256;  cpm++) {
	    /*
	     * Discard candidate if it refers to an unused stem.
	     */
	    if (cpm & mask)
		continue;
	    /*
	     * Otherwise if candidate is in final_truthtab, keep it.
	     */
	    for (i = 0;  i < final_truthtab.pmsz;  i++) {
		if (final_truthtab.permutes[i] == (UCHAR) cpm) {
		    my_permutes [my_pmsz] = (UCHAR) cpm;
		    my_pmsz++;
		}
	    }
	}
	if (debugging_boolsrch) {
	    fprintf (aa_stderr,
		PROGNAME"565 Minimize truth table, pmsz=%d-->%d\n  permutes=",
		final_truthtab.pmsz, my_pmsz);
            for (i=0;  i<16;  i++) {
                if (i >= my_pmsz)
                    break;
                fprintf (aa_stderr, " %02x", (int) my_permutes [i]);
            }
            fputc ('\n', aa_stderr);
            fflush (aa_stderr);
	}
	final_truthtab.permutes = my_permutes;
	final_truthtab.pmsz = my_pmsz;
    } /* end minimize of permutes */

    /* Calculate result bit vector.
     * Loop 1 is a single pass through the bit vectors
     * (a bit loop inside a byte loop).
     * For each nonzero bit, ie each database record
     * that has at least one of the query terms in it,
     * build a 'permute' equivalent to the boolean
     * representation of the terms in that record (Loop 2).
     * Then search the truth table permutes for a match (Loop 3).
     * If found, set the record's bit in the result_bitvec.
     */

    /* LOOP 1.  For each database addr... */
    result_count = 0;
    for (byteno = 0;  byteno < bitveclen;  byteno++) {
	for (bitno = 0;  bitno < 8;  bitno++) {
	    mask = 1 << bitno;

	    /* LOOP 2.  Build permute for each query term. */
	    permute = 0;
	    for (stemno = 0;  stemno < saveusr.stemcount;  stemno++) {
		if (bitvecs [stemno] [byteno]  &  (UCHAR) mask)
		    permute |= 1 << stemno;
	    }

	    /* LOOP 3.  Search truth table for matching permute. */
	    for (i = 0;  i < final_truthtab.pmsz;  i++) {
		if (final_truthtab.permutes[i] == permute) {
		    result_bitvec [byteno] |= (UCHAR) mask;
		    result_count++;
		}
	    }
	}
    }

    if (debugging_boolsrch) {
	fprintf (aa_stderr, PROGNAME"621 PRELIM RESULTS resultct=%ld  bv=\n",
	    result_count);
        for (i=0;  i<22;  i++) {
	    if (i >= bitveclen)
		break;
	    fprintf (aa_stderr, " %02x", (int) result_bitvec[i]);
        }
        fputc ('\n', aa_stderr);
        fflush (aa_stderr);
    }

    /* The next workprocs are 'filters', reducing the size
     * of result_bitvec by removing various unwanted records.
     * They're done in the following order:
     * (1) If no records survived the truth table manipulations,
     *     we're done, return 'no hits'.
     * (2) If we must remove documents because of keytype or date,
     *     the next workproc is the filter that reads the database.
     * (3) If we're sorting, the next workproc reduces the
     *     bitvec to the aa_maxhits recs with the highest
     *     statistical weights.
     * (4) Otherwise the next workproc just loads the hitlist.
     */
    if (result_count <= 0) {
	usrblk.retncode = OE_NOTAVAIL;
	usrblk.workproc = dummy_workproc;
    }
    else if (!all_key_types || check_dates) {
	usrblk.retncode = OE_SEARCHING;
	usrblk.workproc = dbread_filter_WK;
    }
    else if (do_stat_sort) {
	if (debugging_boolsrch)
	    fprintf (aa_stderr,
		PROGNAME"948 No db reads necessary for date or keytype.\n");
	usrblk.retncode = OE_SEARCHING;
	usrblk.workproc = weights_filter_WK;
    }
    else {
	if (debugging_boolsrch)
	    fprintf (aa_stderr,
		PROGNAME"625 No filtering: no sort and no db reads.\n");
	usrblk.retncode = OE_SEARCHING;
	usrblk.workproc = load_DtSrResults_WK;
    }
    return;
} /* calc_result_bitvec_WK() */


/****************************************/
/*					*/
/*		read_d99		*/
/*					*/
/****************************************/
/* Subroutine of read_stem_bitvec_WK().
 * Repeatedly called to get each d99dba in the inverted index
 * file (d99) for a specific index term.  The first call passes
 * the term's wordrec with d99 offset and size information.
 * Subsequent calls pass NULL.
 * Returns valid d99dba, or 0 at end of term's index, or -1 on error.
 * Actual reads are performed a disk block at a time,
 * with dbas stored in a static buffer for the next call.
 */
static DB_ADDR	read_d99 (struct or_hwordrec *wordrec)
{
    static DB_ADDR	readbuf [DBAS_PER_BLOCK];
    static DB_ADDR	*bufptr, *endbuf;
    static FILE		*fptr;
    static long		bal_read, request_read;

    /* First call for new term */
    if (wordrec) {
	fptr = usrblk.dblk->iifile;
	fseek (fptr, wordrec->or_hwoffset, SEEK_SET);
	bal_read = wordrec->or_hwaddrs;
	bufptr = endbuf = 0;	/* triggers block read */
    }

    /* Time to read another block */
    if (bufptr >= endbuf) {
	if (bal_read <= 0)
	    return 0;
	if (bal_read > DBAS_PER_BLOCK) {
	    request_read = DBAS_PER_BLOCK;
	    bal_read -= DBAS_PER_BLOCK;
	    endbuf = readbuf + DBAS_PER_BLOCK;
	}
	else {
	    /* last block is usually short */
	    request_read = bal_read;
	    bal_read = 0;
	    endbuf = readbuf + request_read;
	}
	if (fread (readbuf, sizeof(DB_ADDR), request_read, fptr)
		!= request_read) {
	    sprintf (msgbuf, catgets(dtsearch_catd, MS_boolsrch, 28,
		"%s Database Read Error in %s.d99.") ,
		PROGNAME"428", usrblk.dblk->name);
	    DtSearchAddMessage (msgbuf);
	    return -1;
	}
	bufptr = readbuf;
    }

    /******return *bufptr++;*******/
    return ntohl (*bufptr++);
} /* read_d99() */


/****************************************/
/*					*/
/*	     get_colloc_bitvec		*/
/*					*/
/****************************************/
/* Subroutine of read_stem_bitvec_WK().
 * Constructs a 'collocation bitvector' for current save_stemno.
 * A collocation expression requests the return of all records
 * containing both of two terms (a kind of boolean AND) such that 
 * the occurrences are within n characters of each other.
 * For example "ICE @5 CREAM" requests the return of all records
 * containing both "ICE" and "CREAM" but only if they are separated
 * by no more than 5 characters.
 *
 * Since offset information is not stored in the inverted index
 * this module initially returns the intersection of the two words'
 * bit vectors (boolean AND).  Then it retrieves each record,
 * builds an offset (hilites) table for each of the two words,
 * then compares the offset differences in the tables.
 * If no occurrence pairs are within the specified separation
 * range, the record is deleted from the bitvector.
 * Returns 0 if successful, otherwise returns -1 and msgs.
@@@@ rewrite as its own workproc--reading/hiliting can take a long time...
 */
static int	get_colloc_bitvec (void)
{
    int		stemno_A =	or_wordrecs[save_stemno].or_hwoffset;
    int		stemno_B =	or_wordrecs[save_stemno].or_hwfree;
    long	range =		or_wordrecs[save_stemno].or_hwaddrs;
    UCHAR	*bitvec_A =	bitvecs [stemno_A];
    UCHAR	*bitvec_B =	bitvecs [stemno_B];
    UCHAR	*bitvec_C =	bitvecs [save_stemno];
    long	byteno, recno;
    UCHAR	bitmask;
    int		parse_type;
    int		got_a_colloc;
    char	*stemp;
    DtSrHitword *hitwords_A, *hitwords_B;
    long	hitwcount_A, hitwcount_B;
    long	threshold_range;
    DB_ADDR	dba;
    LLIST	*bloblist;
    long	a, b, offset_A, offset_B;

    /* First construct the set intersection (AND) of
     * each of the collocated terms in the colloc bitvec.
     */
    for (byteno = 0;  byteno < bitveclen;  byteno++)
	bitvec_C [byteno] = bitvec_A [byteno] & bitvec_B [byteno];
    if (debugging_boolsrch) {
	int	i;
	fprintf (aa_stderr,
	    PROGNAME"312 INTERSECT[%d] (colloc %d & %d):\n",
		save_stemno, stemno_A, stemno_B);
	for  (i=0; i<bitveclen; i++) {
	    fprintf (aa_stderr, " %02x", bitvec_C[i]);
	    if (i > 22)
		break;
	}
	fputc ('\n', aa_stderr);
	fflush (aa_stderr);
    }

    /* Read cleartext for each rec in intersection/colloc bitvec.
     * Get hitwords (hilite table) for each collocation term.
     * Switch off recs in bitvec where no term pairs are in
     * collocation range. 
     */
    for (recno = 1;  recno < tot_addr_count;  recno++) {
	byteno = recno >> 3;	/* divide by 8 */
	bitmask = 1 << (recno % 8);

	/* Skip zero bits */
	if ((bitvec_C[byteno] & bitmask) == 0)
	    continue;

	/* Convert recno to vista database address.
	 * Silently skip rec if dba doesn't exist.
	 */
	dba = (recno - 1) * or_recslots + 2;
	if (dba >= or_maxdba) {
	    RESET_BIT (bitvec_C, byteno, bitmask);
            continue;
	}
	dba |= (OR_D00 << 24);

	/* Silently skip records that have no document text */
	if ((bloblist = ve_getblobs (dba, vistano)) == NULL) {
	    if (debugging_boolsrch) {
		fprintf (aa_stderr,
		    PROGNAME"126 No blobs for recno=%ld byteno=%ld mask%02x\n",
		    recno, byteno, bitmask);
		fflush (aa_stderr);
	    }
	    RESET_BIT (bitvec_C, byteno, bitmask);
	    continue;
	}

	/* Uncompress record text into usrblk.cleartext */
	if (oe_unblob (bloblist) != OE_OK)
	    return -1;

	/* Build 'hilite' table for stem A.  If stem
	 * can't be found in the record, silently skip it.
	 * Otherwise save the table.
	 */
	stemp = saveusr.stems [stemno_A];
	if (stemp[0] == STEM_CH) {
	    parse_type = 'S';
	    stemp++;
	}
	else
	    parse_type = 'W';
	if (!hilite_cleartext (parse_type, stemp, 1)) {
	    RESET_BIT (bitvec_C, byteno, bitmask);
	    continue;
	}
	hitwords_A = usrblk.hitwords;
	hitwcount_A = usrblk.hitwcount;
	usrblk.hitwords = NULL;
	usrblk.hitwcount = 0;

	/* In the same way build 'hilite' table for stem B */
	stemp = saveusr.stems [stemno_B];
	if (stemp[0] == STEM_CH) {
	    parse_type = 'S';
	    stemp++;
	}
	else
	    parse_type = 'W';
	if (!hilite_cleartext (parse_type, stemp, 1)) {
	    RESET_BIT (bitvec_C, byteno, bitmask);
	    free (hitwords_A);
	    continue;
	}
	hitwords_B = usrblk.hitwords;
	hitwcount_B = usrblk.hitwcount;
	usrblk.hitwords = NULL;
	usrblk.hitwcount = 0;

	/* Compare the two hilite tables for range matches */
	got_a_colloc = FALSE;
	b = 0;
	for (a = 0;  a < hitwcount_A;  a++) {
	    offset_A = hitwords_A[a].offset;
	    threshold_range = offset_A + hitwords_A[a].length + range;
	    for (;  b < hitwcount_B;  b++) {
		offset_B = hitwords_B[b].offset;

		/* Advance B to first entry past A's offset */
		if (offset_B <= offset_A )
		    continue;	/* ...the B loop */
		if (offset_B <= threshold_range)
		    got_a_colloc = TRUE;
		break;		/* ...the B loop */
	    }  /* end B loop */
	    if (got_a_colloc  ||  b >= hitwcount_B)
		break;		/* ...the A loop */
	} /* end A loop */
	free (hitwords_A);
	free (hitwords_B);

	/* If no collocations found within range,
	 * switch off rec in colloc bitvec.
	 */
	if (!got_a_colloc)
	    RESET_BIT (bitvec_C, byteno, bitmask);

    } /* end loop on each recno in intersection/colloc bitvec */

    return 0;
} /* get_colloc_bitvec() */


/****************************************/
/*					*/
/*	    read_stem_bitvec_WK		*/
/*					*/
/****************************************/
/* First workproc after boolean_search().
 * Each iterative call loads one (save_stemno) real stem's bitvec. 
 * After last stem bitvec loaded, sets up
 * call to next workproc in sequence.
 */
static void	read_stem_bitvec_WK (void)
{
    long	byteno;
    DB_ADDR	d99recno;
    float	weight;

    if (got_USR_STOPSRCH())
	return;

    /* Process collocation 'stems' */
    if (saveusr.stems [save_stemno] [0] == '@') {
	d99recno = get_colloc_bitvec();
	goto DONE_READING;
    }

    for (	d99recno = read_d99 (&or_wordrecs [save_stemno]);
		d99recno;
		d99recno = read_d99 (NULL)) {
	if (d99recno == -1)	/* read error */
	    break;

	/* Save low byte 'statistical weight' value.
	 * It can only be 0 - 255.
	 */
	if (do_stat_sort)
	    weight = (float) (d99recno & 0x000000ff) + 1.0;

        d99recno = (d99recno >> 8) & 0x00ffffff;

	/* Set correct bit in bitvec.
	 * The byte number is the recno divided by 8.
	 * The bit number is the remainder after division by 8.
	 */
	if ((byteno = d99recno >> 3) >= bitveclen) {
	    sprintf (msgbuf, catgets(dtsearch_catd, MS_boolsrch, 32,
		"%s Database Error: %s '%s'\n"
		"in database '%s' has invalid d99 record number %ld.") ,
		PROGNAME"394",
		(usrblk.search_type == 'W') ?
			catgets(dtsearch_catd, MS_boolsrch, 33, "Word") :
			catgets(dtsearch_catd, MS_boolsrch, 34, "Stem of"),
		usrblk.stems [save_stemno],
		usrblk.dblk->label,
		d99recno);
	    DtSearchAddMessage (msgbuf);
	    d99recno = -1;	/* force error return */
	    goto DONE_READING;
	}
	bitvecs [save_stemno] [byteno] |= 1 << (d99recno % 8);

	/* Add to correct weight in weight vector.
	 * IDF ranges between 1.0 and 20.0, and weight
	 * is 1 - 256, so we're adding 1 - ~5000 to wtvec.
	 */
	if (do_stat_sort)
	    wtvec [d99recno] += weight * (float) idf [save_stemno];

    } /* end loop that retrieves every d99recno for curr stem */

DONE_READING:

    if (debugging_boolsrch) {
	int	i;
	if (debugging_boolsrch)
	    fprintf (aa_stderr, PROGNAME"313 BITVEC[%d]:\n", save_stemno);
	for  (i=0; i<bitveclen; i++) {
	    fprintf (aa_stderr, " %02x", bitvecs[save_stemno][i]);
	    if (i > 22)
		break;
	}
	fputc ('\n', aa_stderr);
	fflush (aa_stderr);
    }

    if (d99recno == 0) {
	/* Normal conclusion.  Increment to next stem.
	 * If not all stems have been read,
	 * this is still the next workproc.
	 * Otherwise the next workproc is the one
	 * merging all bitvectors into the final
	 * result bitvec using the truth table.
	 */
	usrblk.retncode = OE_SEARCHING;
	if (++save_stemno < saveusr.stemcount)
	    usrblk.workproc = read_stem_bitvec_WK;
	else
	    usrblk.workproc = calc_result_bitvec_WK;
    }
    else
	/* d99recno must be -1 */
	usrblk.retncode = OE_SYSTEM_STOP;
    return;
} /* read_stem_bitvec_WK() */


/****************************************/
/*					*/
/*	      boolean_search		*/
/*					*/
/****************************************/
/* Called from Opera_Engine after successful boolean_parse().
 * Expects valid globals: saveusr.stems, saveusr.stemcount,
 * usrblk.stems (contains original unstemmed query terms for msgs),
 * usrblk.search_type, final_truthtab, qry_has_no_NOTs,
 * and qry_is_all_ANDs.
 * Based on parts of the function ve_word_search().
 * Upon return, usrblk.retncode, msglist, etc is appropriately loaded.
 * Upon successful return usrblk.stems, usrblk.stemcount,
 * and dittolist are also loaded.
 */
void	boolean_search (void)
{
    int		i, j;
    size_t	allocsz_needed;

    /* Sanity checks */
    if (	saveusr.stemcount <= 0		||
		final_truthtab.pmsz <= 0	||
		final_truthtab.pmsz >= 256	) {
	fprintf (aa_stderr, catgets(dtsearch_catd, MS_boolsrch, 35,
	    "%s Program Error: stemct=%d pmsz=%d\n") ,
	    PROGNAME"1404", saveusr.stemcount, final_truthtab.pmsz);
	DtSearchExit (14);
    }

    /*---------- Init globals ----------*/
    if (!msgbuf)
	msgbuf = austext_malloc (500, PROGNAME"393", NULL);
    debugging_boolsrch =	(usrblk.debug & USRDBG_SRCHCMPL);
    need_zero_permute =	(final_truthtab.permutes[0] == 0);
    do_stat_sort =	((usrblk.flags & USR_SORT_WHITL) != 0);
    check_dates =	(usrblk.objdate1 || usrblk.objdate2);
    or_abstrsz =	usrblk.dblk->dbrec.or_abstrsz;
    or_fzkeysz =	usrblk.dblk->dbrec.or_fzkeysz;
    or_language =	usrblk.dblk->dbrec.or_language;
    or_maxdba =		usrblk.dblk->dbrec.or_maxdba;
    usrblk.flags &=	~USR_STOPSRCH;	/* turn off stop button */

    saveusr.vistano = vistano =	usrblk.dblk->vistano;
    saveusr.dittolist =		NULL;
    saveusr.dittocount =	0L;
    saveusr.iterations =	INIT_ITERATIONS;
    /*
     * saveusr.ktchars is a string holding
     * first char of desired record ids.
     */
    all_key_types =		TRUE;
    for (i = 0, j = 0; i < usrblk.dblk->ktcount; i++) {
	if (usrblk.dblk->keytypes[i].is_selected)
	    saveusr.ktchars[j++] = usrblk.dblk->keytypes[i].ktchar;
	else
	    all_key_types =	FALSE;
    }
    saveusr.ktchars[j] = '\0';

    or_recslots =	(long) (usrblk.dblk->dbrec.or_recslots);
    or_reccount =	usrblk.dblk->dbrec.or_reccount; 

    /* RECFRST is just to get the slot# (dba) of the
     * first real object record after the dbrec.
     * Currently the dbrec occupies only one slot,
     * the first (#1), so dba will usually be #2.
     */
/********
    RECFRST(PROGNAME"2545", OR_OBJREC, saveusr.vistano);
    CRGET(PROGNAME"2546", &dba, saveusr.vistano);
    dba &= 0x00FFFFFF;
********/
    tot_addr_count =	((usrblk.dblk->dbrec.or_maxdba + 1) / or_recslots) + 1;
    bitveclen =		(tot_addr_count >> 3) + 1;

    if (debugging_boolsrch) {
	fprintf (aa_stderr, PROGNAME"360 "
	    "boolean_search: typ='%c' needzpm?=%d sort?=%d maxhits=%d\n"
	    "  maxdba=%ld recct=%ld recslts=%ld\n"
	    "  totnmadr=%ld bvln=%ld allkts?=%d  ktchars='%s'\n"
	    ,usrblk.search_type
	    ,need_zero_permute
	    ,do_stat_sort
	    ,aa_maxhits
	    ,(long) usrblk.dblk->dbrec.or_maxdba
	    ,or_reccount
	    ,or_recslots
	    ,tot_addr_count
	    ,bitveclen
	    ,all_key_types
	    ,saveusr.ktchars
	    );
	fflush (aa_stderr);
    }


    /*---------- Read vista btree ----------
     * Load or_wordrecs[] array for each term in saveusr.stems.
     */
    if (!load_or_wordrecs())
	return;

    /* If statistically sorting final resultlist, calculate
     * idf (inverse document frequency) for each term using
     * the frequency data in or_wordrecs[].
     */
    if (do_stat_sort)
	calculate_idfs();

    /* Bitvector allocation.  Number needed is one for each stem,
     * plus one extra to accumulate the result bitvector.
     */
    allocsz_needed = bitveclen * (saveusr.stemcount + 1);
    if (debugging_boolsrch)
	fprintf (aa_stderr, PROGNAME"430 "
	    "bitvecs[] alloc needed=%lu (bvln=%ld stems=%d+1), have=%lu.\n",
	    (unsigned long) allocsz_needed, bitveclen, saveusr.stemcount, (unsigned long) bitvec_allocsz);
    if (bitvec_allocsz < allocsz_needed) {
	if (bitvec_allocp)
	    free (bitvec_allocp);
	bitvec_allocp = austext_malloc (allocsz_needed + 16,
	    PROGNAME"508", NULL);
	if (debugging_boolsrch)
	    fprintf (aa_stderr, PROGNAME"432 bitvecs[] realloc %lu-->%lu.\n",
		(unsigned long) bitvec_allocsz, (unsigned long) allocsz_needed);
	bitvec_allocsz = allocsz_needed;
    }

    /* Clear all bitvecs to zero and assign them */
    memset (bitvec_allocp, 0, allocsz_needed);
    for (i = 0;  i < saveusr.stemcount;  i++)
	bitvecs[i] = bitvec_allocp + (i * bitveclen);
    result_bitvec = bitvec_allocp + (i * bitveclen);

    /* If sorting statistically, allocate weight vector.
     * One float for each db record.
     */
    if (wtvec) {
	free (wtvec);
	wtvec = NULL;
    }
    if (do_stat_sort) {
	wtvec = austext_malloc ((tot_addr_count + 4) * sizeof(float) + 4,
	    PROGNAME"040", NULL);
	memset (wtvec, 0, (tot_addr_count + 4) * sizeof(float));
    }

    /* The 'zero permute' is every record that has
     * NONE of the query terms in it.  It can only be
     * generated if a NOT operator was included in the query.
     */
    if (need_zero_permute) {
	sprintf (msgbuf, catgets (dtsearch_catd, MS_boolsrch, 15,
	    "%s Your query requires retrieving every\n"
	    "document in the database that does not have any of\n"
	    "your query words.  This type of search may take an\n"
	    "unusually long time."),
	    PROGNAME"1536");
	DtSearchAddMessage (msgbuf);
    }

    if (debugging_boolsrch)
	fflush (aa_stderr);

    /* Searches may take a long time.  To allow gui to put a
     * a 'working' dialog box and a 'cancel' button,
     * we pass execution to workprocs.
     * If user cannot cancel search no matter how
     * long it may take, we call each of the subsequent
     * workproc functions directly from here.
     * Otherwise they will themselves setup each
     * subsequent call to usrblk.workproc(), as long as
     * the previous call returns OE_SEARCHING and the user
     * hasn't pushed USR_STOPSRCH.
     */
    usrblk.workproc = read_stem_bitvec_WK;
    save_stemno = 0;	/* global arg for first workproc */
    usrblk.workproc();	/* direct call to first workproc */

    if ((usrblk.flags & USR_NO_ITERATE) != 0  &&
		(usrblk.debug & USRDBG_ITERATE) == 0) {
	while (usrblk.retncode == OE_SEARCHING)
	    usrblk.workproc();
    }
    return;

} /* boolean_search() */

/************************** BOOLSRCH.C **********************/
