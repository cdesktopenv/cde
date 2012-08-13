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
 *   FUNCTIONS: CNCRD_MEMORY_AREA_LIST
 *              QUERY_STEM_STR
 *              STAT_STR
 *              TREENODE
 *              build_bin_tree
 *              comp_stat
 *              descend_tree
 *              efim_qsort
 *              fill_stem
 *              get_next_memory_block
 *              init_global_memory
 *              init_memory
 *              inv_index_bin_tree
 *              load_ditto_str
 *              release_shm_mem
 *              stat_search
 *              traverse_tree
 *              ve_statistical
 *
 *   ORIGINS: 27
 *
 *   (C) COPYRIGHT International Business Machines Corp. 1993,1995
 *   All Rights Reserved
 *   US Government Users Restricted Rights - Use, duplication or
 *   disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 */
/*************************** VESTATIS.C ****************************
 * $XConsortium: vestatis.c /main/9 1996/11/25 18:49:04 drk $
 * 1993.
 * Statistically sorted stems search.
 *
 * $Log$
 * Revision 2.3  1996/02/01  19:35:55  miker
 * AusText 2.1.11, DtSearch 0.3:  Uses new single word parser/stemmers.
 *
 * Revision 2.2  1995/10/25  15:00:05  miker
 * Added prolog.
 *
 * Revision 2.1  1995/09/22  22:30:42  miker
 * Freeze DtSearch 0.1, AusText 2.1.8
 * Revision 1.11  1995/09/07  23:30:15  miker
 * ...One last try (sigh).
 * Revision 1.10  1995/09/07  19:08:01  miker
 * Last fix incorrectly coded.
 * Revision 1.9  1995/09/07  16:25:11  miker
 * Fixed solaris bus fault caused by TREENODE structure
 * not being aligned on machines word boundary.  Fault occurred
 * only when query contained more than one word.
 * Revision 1.8  1995/09/05  19:31:37  miker
 * Made usrblk and ausapi_msglist global.  Replaced Socrates()
 * with calls to parser() and stemmer().  Deleted socblk.
 * Numerous name changes.  All for DtSearch...
 */
#ifndef _ALL_SOURCE
#  define _ALL_SOURCE	/* to pickup typedefs for shm vnodes */
#endif
#include "SearchE.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "vista.h"

/*-------------------------- GLOBALS ----------------------------*/
/**** declaration for the global memory pointers ****/
#define	PROGNAME	"VESTATIS"
#define	MEMORY_SIZE	64000	/* 65536 is 64 KBytes of memory */
#define REC_TYPES       256
#define NORM_VALUE      30
#undef INFINITY /* XXX does GCC's __builtin_inff() work here? */
#define INFINITY        9999.0
#define	SORT_MESG	10000
#define	CHAR_BITS	8
#define	STACKSZ		256
#define	MED_3_VALUE	7
#define	TIME_ITERATION	1
#define LOG2		0.693147181
#define MS_vestatis	17
#define STRUCT_ALIGN	sizeof(char*)

static int      SHM_FLAG = IPC_CREAT | S_IRUSR | S_IWUSR | S_IWGRP |
			S_IRGRP | S_IROTH | S_IWOTH;

static int      mem_break;

static char    *mem_start;
static char    *cur_pos;
static long     mem_offset;
static long     total_memory_size;

typedef struct q_s {
    char            stem[DtSrMAXWIDTH_HWORD];
    int             count;
}               QUERY_STEM_STR;

typedef struct mem_area {
    char           *start_of_mem_block;
    long            block_size;
    struct mem_area *next_block;
}               CNCRD_MEMORY_AREA_LIST;

typedef struct bintree {
    struct bintree *rlink;	/* ptr to next node in linked list or
				 * right link in binary tree */
    struct bintree *llink;	/* left link in binary tree */
    char           *word;	/* ptr to word in the query */
    int             count;
}               TREENODE;

typedef struct s_a {
    DB_ADDR	dba;
    float	wght;
    DtSrINT32	num_word_hits;
}               STAT_STR;

static STAT_STR *stat_array = NULL;
static TREENODE *root_node;
static TREENODE *top_of_stack;
static TREENODE *stack;
static TREENODE *pres;
static TREENODE *prev;
static TREENODE *next;
static TREENODE *avail_node;
static CNCRD_MEMORY_AREA_LIST *memory_blocks = NULL;
static CNCRD_MEMORY_AREA_LIST *cur_mem_ptr;
static QUERY_STEM_STR *query_stems = NULL;
static DB_ADDR *word_addrs = NULL;
static int      num_diff_words = 0;
static char     begin_search;
static char     begin_sort;
static char     begin_load_ditto;
static char     begin_qsort;
static char     qsort_done;
static DtSrINT32	real_num_rec;
static DtSrINT32	num_hits;
static DtSrINT32	total_num_addrs;
static DtSrINT32	dba_offset;
static unsigned char rec_type_tab[REC_TYPES];
static char     vestat_msgbuf[256];
static int      mes_search_box;
static int      slot_d00;

extern char    *chmat ();
extern void     find_keyword (char *cur_word, int vista_num);
extern void     read_wordstr (struct or_hwordrec * glob_word, int vista_num);
extern void     write_wordstr (struct or_hwordrec * glob_word, int vista_num);

static void     stat_search (void);	/* redefined below */


/********************************/
/*				*/
/*	Release Shared Memory	*/
/*				*/
/********************************/
void            release_shm_mem (void)
{
    if (global_memory_ptr != NULL) {
	if (shmdt (global_memory_ptr) == -1) {
	    DtSearchAddMessage (catgets (dtsearch_catd, MS_vestatis, 104,
		PROGNAME "104 Cannot detach shared memory "));
	    OE_flags |= OE_PERMERR;
	    usrblk.retncode = OE_ABORT;
	    return;
	}
	if (shmctl (shm_id, IPC_RMID, NULL) == -1) {
	    DtSearchAddMessage (catgets (dtsearch_catd, MS_vestatis, 110,
		    PROGNAME "110 Cannot remove shared memory "));
	    OE_flags |= OE_PERMERR;
	    usrblk.retncode = OE_ABORT;
	    return;
	}
	global_memory_ptr = NULL;
    }

    return;
} /* release_shm_mem() */


/********************************/
/*				*/
/*	Init Global Memory	*/
/*				*/
/********************************/
/* addrs - largest DBA slot in D00 file in the current database
 * r_addrs - total records count in the current database.
 */
static int	init_global_memory (DtSrINT32 addrs, DtSrINT32 r_addrs)
{
    long	i, j;
    size_t	k;

    i = DtSrMAX_STEMCOUNT * ((addrs >> 3) + 1) * 2 +
	addrs * sizeof (int) + sizeof (DB_ADDR) * r_addrs;
    j = sizeof (STAT_STR) * addrs + sizeof (DB_ADDR) * r_addrs;
    k = (i > j) ? i : j;
    shm_id = shmget (IPC_PRIVATE, k, SHM_FLAG);
    if ((global_memory_ptr = (char *) shmat (shm_id, (char *) 0, 0)) ==
	((char *) -1)) {
	DtSearchAddMessage (catgets (dtsearch_catd, MS_vestatis, 115,
		PROGNAME "115 No shared memory available"));
	OE_flags |= OE_PERMERR;
	usrblk.retncode = OE_ABORT;
	return FALSE;
    }
    return TRUE;
} /* init_global_memory() */


/****************************************/
/*					*/
/*		efim_qsort		*/
/*					*/
/****************************************/
/* Custom quick sort algorithm (medium-of-3 partitioning).
 * Coded for efficiency given our expected data characteristics,
 * and for interruptability.
 */
int             efim_qsort (void)
{
    time_t          start_time;
    double          time_dif;
    static long     left, right;
    static long     scan_l, scan_r, mid3, pvidx, l_size, r_size;
    static long     sptr;
    static float    pivot, temp, stack_l[STACKSZ], stack_r[STACKSZ];
    static DB_ADDR  dba;

    /* Test whether user has pushed STOP button since last call. */
    if (usrblk.flags & USR_STOPSRCH) {
	if (OE_flags & OE_AUDIT)
	    oe_write_audit_rec (-1L);
	usrblk.retncode = OE_USER_STOP;
	release_shm_mem ();
	return TRUE;
    }

    if (begin_qsort) {
	sptr = 0;
	left = 0;
	right = num_hits - 1;
	begin_qsort = FALSE;
    }

    time (&start_time);

    for (;;) {
	/* check iteration loop */
	time_dif = difftime (time (NULL), start_time);
	if ((time_dif > TIME_ITERATION
		|| usrblk.debug & USRDBG_ITERATE) &&
	    !(usrblk.flags & USR_NO_ITERATE)) {
	    usrblk.retncode = OE_SEARCHING;
	    usrblk.workproc = stat_search;
	    mes_search_box = TRUE;
	    return TRUE;
	}
	while (right > left) {
	    if ((right - left) > MED_3_VALUE) {
		/*
		 * compute value for the median-of-three partitioning 
		 */
		mid3 = (left + right) >> 1;
		/*
		 * three-sort left, middle, and right elements 
		 */
		if ((stat_array + left)->wght < (stat_array + mid3)->wght) {
		    temp = (stat_array + left)->wght;
		    (stat_array + left)->wght =
			(stat_array + mid3)->wght;
		    (stat_array + mid3)->wght = temp;
		    dba = (stat_array + left)->dba;
		    (stat_array + left)->dba =
			(stat_array + mid3)->dba;
		    (stat_array + mid3)->dba = dba;
		}

		if ((stat_array + left)->wght < (stat_array + right)->wght) {
		    temp = (stat_array + left)->wght;
		    (stat_array + left)->wght =
			(stat_array + right)->wght;
		    (stat_array + right)->wght = temp;
		    dba = (stat_array + left)->dba;
		    (stat_array + left)->dba =
			(stat_array + right)->dba;
		    (stat_array + right)->dba = dba;
		}

		if ((stat_array + mid3)->wght < (stat_array + right)->wght) {
		    temp = (stat_array + mid3)->wght;
		    (stat_array + mid3)->wght =
			(stat_array + right)->wght;
		    (stat_array + right)->wght = temp;
		    dba = (stat_array + mid3)->dba;
		    (stat_array + mid3)->dba =
			(stat_array + right)->dba;
		    (stat_array + right)->dba = dba;
		}

		/* select pivot element index */
		pvidx = right - 1;

		/* exchange pivot with the middle element */
		temp = (stat_array + mid3)->wght;
		(stat_array + mid3)->wght = (stat_array + pvidx)->wght;
		(stat_array + pvidx)->wght = temp;
		dba = (stat_array + mid3)->dba;
		(stat_array + mid3)->dba = (stat_array + pvidx)->dba;
		(stat_array + pvidx)->dba = dba;

		/* setup for partitioning */
		scan_l = left + 1;
		scan_r = right - 2;
	    }
	    else {
		/* select pivot element index */
		pvidx = right;

		/* set scanning indexes */
		scan_l = left;
		scan_r = right - 1;
	    }

	    /* select pivot element */
	    pivot = (stat_array + pvidx)->wght;

	    for (;;) {
		/* scan from left */
		while ((stat_array + scan_l)->wght > pivot) {
		    scan_l++;
		}

		/* scan from right */
		while ((stat_array + scan_r)->wght < pivot) {
		    if (scan_r == 0) {
			break;
		    }
		    scan_r--;
		}

		/* if scan have met, exit inner loop */
		if (scan_l >= scan_r) {
		    break;
		}

		/* exchange elements */
		temp = (stat_array + scan_r)->wght;
		(stat_array + scan_r)->wght = (stat_array + scan_l)->wght;
		(stat_array + scan_l)->wght = temp;
		dba = (stat_array + scan_r)->dba;
		(stat_array + scan_r)->dba = (stat_array + scan_l)->dba;
		(stat_array + scan_l)->dba = dba;

		/* move scans to next elements */
		scan_l++;
		scan_r--;
	    }

	    if (scan_l != pvidx) {
		/* exchange finale element */
		temp = (stat_array + pvidx)->wght;
		(stat_array + pvidx)->wght = (stat_array + scan_l)->wght;
		(stat_array + scan_l)->wght = temp;
		dba = (stat_array + pvidx)->dba;
		(stat_array + pvidx)->dba = (stat_array + scan_l)->dba;
		(stat_array + scan_l)->dba = dba;
	    }

	    /* calculate section sizes */
	    l_size = scan_l - left;
	    r_size = right - scan_l;

	    /* place largest section on stack */
	    if (l_size > r_size) {
		/* ignore 1-element sections */
		if (l_size > 1) {
		    sptr++;

		    if (sptr == STACKSZ) {
			fputs (catgets (dtsearch_catd, MS_vestatis, 107,
			    PROGNAME "107 Qsort stack overflow.\n"),
			    aa_stderr);
			OE_flags |= OE_PERMERR;
			usrblk.retncode = OE_ABORT;
			release_shm_mem ();
			return FALSE;
		    }

		    *(stack_l + sptr) = left;
		    *(stack_r + sptr) = scan_l - 1;
		}

		/* ignore 1-element sections */
		if (r_size != 0) {
		    left = scan_l + 1;
		}
		else {
		    break;
		}
	    }

	    else {
		/* ignore 1-element sections */

		if (r_size > 1) {
		    sptr++;

		    if (sptr == STACKSZ) {
			fputs (catgets (dtsearch_catd, MS_vestatis, 107,
			    PROGNAME "107 Qsort stack overflow.\n"),
			    aa_stderr);
			OE_flags |= OE_PERMERR;
			usrblk.retncode = OE_ABORT;
			release_shm_mem ();
			return FALSE;
		    }

		    *(stack_l + sptr) = scan_l + 1;
		    *(stack_r + sptr) = right;
		}

		/* ignore 1-element sections */
		if (l_size != 0) {
		    right = scan_l - 1;
		}
		else {
		    break;
		}
	    }
	}

	/* iterate with values from stack (if any) */
	if (sptr) {
	    left = *(stack_l + sptr);
	    right = *(stack_r + sptr);
	    sptr--;
	}
	else {
	    break;
	}
    }

    qsort_done = TRUE;
    return TRUE;
} /* efim_qsort() */


/****************************************/
/*					*/
/*		fill_stem		*/
/*					*/
/****************************************/
/* "Visit" subroutine of descend_tree(), which is itself subroutine
 * of traverse_tree().  Builds query_stems array
 * and establishes its size in num_diff_words.
 */
static void     fill_stem (TREENODE * cur_stem)
{
    query_stems[num_diff_words].count = cur_stem->count;
    strcpy (query_stems[num_diff_words].stem, cur_stem->word);
    num_diff_words++;
    return;
} /* fill_stem() */


/****************************************/
/*					*/
/*		descend_tree		*/
/*					*/
/****************************************/
/* Subroutine of traverse_tree(), Robson tree traversal algorithm. */
static void     descend_tree (void)
{
    int             not_done = TRUE;

    while (not_done) {
	/* end of 'descent' subalgorithm? */
	if ((pres->llink == NULL) && (pres->rlink == NULL)) {
	    /* Preorder, Symmetric Order and Postorder */
	    fill_stem (pres);
	    avail_node = pres;
	    return;
	}
	if (pres->llink != NULL) {
	    /* Preorder */
	    fill_stem (pres);
	    next = pres->llink;
	    pres->llink = prev;
	    prev = pres;
	    pres = next;
	}
	else {
	    /* Preorder and Symmetric Order */
	    fill_stem (pres);
	    next = pres->rlink;
	    pres->rlink = prev;
	    prev = pres;
	    pres = next;
	}
    }
    return;
} /* descend_tree() */


/********************************/
/*				*/
/*	  traverse_tree		*/
/*				*/
/********************************/
/* The algorithm is based on the J. M. ROBSON link inversion traversal
 * algorithm for binary trees. Ref. Thomas A. STANDISH  pp. 77-78.
 */
static void     traverse_tree (void)
{
    int             not_done = TRUE;
    int             descend = TRUE;

    /* initialize the variables */
    pres = root_node;
    prev = pres;
    top_of_stack = NULL;
    stack = NULL;

    while (not_done) {
	if (descend) {
	    descend_tree ();
	}
	if (pres == root_node) {
	    return;
	}
	if (prev->rlink == NULL) {
	    /* Symmetric Order and Postorder */
	    /***	fill_stem(prev); ***/
	    next = prev->llink;
	    prev->llink = pres;
	    pres = prev;
	    prev = next;
	    descend = FALSE;
	}
	else {
	    if (prev->llink == NULL) {
		/* Postorder */
		/** fill_stem(prev); **/
		next = prev->rlink;
		prev->rlink = pres;
		pres = prev;
		prev = next;
		descend = FALSE;
	    }
	    else {
		if (prev == top_of_stack) {
		    /* Postorder */
		    /** fill_stem(prev); **/
		    next = stack;
		    top_of_stack = stack->rlink;
		    stack = stack->llink;
		    next->llink = NULL;
		    next->rlink = NULL;
		    next = prev->llink;
		    prev->llink = prev->rlink;
		    prev->rlink = pres;
		    pres = prev;
		    prev = next;
		    descend = FALSE;
		}
		else {
		    /* Symmetric Order */
		    /***	fill_stem(prev); ***/
		    avail_node->llink = stack;
		    avail_node->rlink = top_of_stack;
		    stack = avail_node;
		    top_of_stack = prev;
		    next = prev->rlink;
		    prev->rlink = pres;
		    pres = next;
		    descend = TRUE;
		}
	    }
	}
    }
} /* traverse_tree() */


/********************************/
/*                              */
/*      Get Next Memory Block	*/
/*                              */
/********************************/
void            get_next_memory_block (size_t node_size)
{
    CNCRD_MEMORY_AREA_LIST *temp_ptr;

    temp_ptr = memory_blocks;

    /*
     * We run out of pre-allocated memory. Allocate additional block of
     * memory 
     */
    if (cur_mem_ptr == NULL) {
	total_memory_size += node_size;
	mem_start = (char *) malloc (total_memory_size);
	mem_offset = 0L;
	mem_offset += node_size;
	cur_pos = mem_start;
	if (mem_start == NULL) {
	    fprintf (aa_stderr, catgets (dtsearch_catd, MS_vestatis, 310,
		"%s Out of Memory.  Need %ld bytes.\n"),
		PROGNAME "310", total_memory_size);
	    OE_flags |= OE_PERMERR;
	    usrblk.retncode = OE_ABORT;
	    release_shm_mem ();
	    return;
	}
	/*
	 * allocate space for the next member of the memory blocks link
	 * list 
	 */
	memory_blocks = (CNCRD_MEMORY_AREA_LIST *)
	    malloc (sizeof (CNCRD_MEMORY_AREA_LIST) + 2);
	if (memory_blocks == NULL) {
	    fputs (catgets (dtsearch_catd, MS_vestatis, 314,
		    PROGNAME"314 Out of Memory.\n"), aa_stderr);
	    OE_flags |= OE_PERMERR;
	    usrblk.retncode = OE_ABORT;
	    release_shm_mem ();
	    return;
	}
	memory_blocks->start_of_mem_block = mem_start;
	memory_blocks->next_block = temp_ptr;
	memory_blocks->block_size = total_memory_size;
	/**** allocation of initial memory blocks is done ****/
    }
    /* Use next available block of memory */
    else {
	mem_start = cur_mem_ptr->start_of_mem_block;
	total_memory_size = cur_mem_ptr->block_size;
	cur_mem_ptr = cur_mem_ptr->next_block;
	mem_offset = 0L;
	mem_offset += node_size;
	cur_pos = mem_start;
    }

    return;
} /* get_next_memory_block() */



/********************************/
/*                              */
/*       build_bin_tree		*/
/*                              */
/********************************/
/* Subroutine of inv_index_bin_tree().
 * Called for each stem in query.
 * Inserts new stem (already uppercase) into tree
 * or increments existing stem's count.
 * Returns TRUE and incr num_diff_words if new stem inserted.
 * Returns FALSE if existing stem's count merely incremented.
 * Returns FALSE and OE_ABORT set on error. 
 */
static int      build_bin_tree (char *cur_word)
{
    int             i;
    int             wordlen;
    size_t          treenode_size;
    TREENODE       *new;
    TREENODE      **this_link;

    wordlen = strlen (cur_word);

    /* Determine the amount of memory needed for the
     * new node.  Add in a pad amount to align it
     * on the machine's word (integer) boundary.
     * Some machines aren't happy about misaligned
     * structures and we're emulating our own malloc.
     * (Thanks, and a tip o' the hat to Takuki Kamiya).
     */
    treenode_size = sizeof (TREENODE) + wordlen + 2;
    treenode_size +=
	(STRUCT_ALIGN - treenode_size % STRUCT_ALIGN)  %  STRUCT_ALIGN;

    /* allocate a new node and load its data fields */
    mem_offset += treenode_size;
    if (mem_offset > total_memory_size) {
	/* allocate new chunk of memory */
	get_next_memory_block (treenode_size);
	if (usrblk.retncode == OE_ABORT)
	    return FALSE;
    }
    new = (TREENODE *) cur_pos;
    cur_pos = mem_start + mem_offset;
    new->llink = NULL;
    new->rlink = NULL;
    new->word = (char *) new + sizeof (TREENODE);
    new->count = 1;
    strcpy (new->word, cur_word);

    /* Insert current word into binary tree */
    for (this_link = &root_node; *this_link != NULL;) {
	i = strcmp (new->word, (*this_link)->word);

	/* Test for current word already in the binary tree */
	if (i == 0) {
	    mem_offset -= treenode_size;
	    cur_pos = mem_start + mem_offset;
	    (*this_link)->count++;
	    return FALSE;	/* no point in continuing descent */
	}

	/* Descend tree to find correct insertion point */
	this_link = (i < 0) ?
	    &(*this_link)->llink : &(*this_link)->rlink;
    }				/* end for loop to find tree insertion
				 * point */

    /* Insert new node at current location in tree */
    *this_link = new;

    num_diff_words++;

    return TRUE;
} /* build_bin_tree() */


/************************/
/*			*/
/*	init_memory	*/
/*			*/
/************************/
/* Initialize the first block of memory for the binary tree.
 * This function is called only once at each run of the offline program.
 */
void            init_memory (void)
{
    mem_start = (char *) malloc (MEMORY_SIZE);
    if (mem_start == NULL) {
	fprintf (aa_stderr, catgets (dtsearch_catd, MS_vestatis, 310,
	    "%s Out of Memory.  Need %ld bytes.\n"), PROGNAME "310", MEMORY_SIZE);
	OE_flags |= OE_PERMERR;
	usrblk.retncode = OE_ABORT;
	release_shm_mem ();
	return;
    }
    total_memory_size = MEMORY_SIZE;
    cur_pos = mem_start;
    mem_offset = 0L;

    /*
     * Allocate space for the first member of the memory blocks link list 
     */
    memory_blocks = (CNCRD_MEMORY_AREA_LIST *)
	malloc (sizeof (CNCRD_MEMORY_AREA_LIST) + 2);
    if (memory_blocks == NULL) {
	fputs (catgets (dtsearch_catd, MS_vestatis, 314,
		PROGNAME "314 Out of Memory.\n"), aa_stderr);
	OE_flags |= OE_PERMERR;
	usrblk.retncode = OE_ABORT;
	release_shm_mem ();
	return;
    }
    memory_blocks->start_of_mem_block = mem_start;
    memory_blocks->block_size = total_memory_size;
    memory_blocks->next_block = NULL;
    cur_mem_ptr = NULL;

    return;
} /* init_memory() */


/********************************/
/*				*/
/*	inv_index_bin_tree	*/
/*				*/
/********************************/
/* Builds binary tree of all stems in query.
 * Returns TRUE and loads num_diff_words with number
 * of nodes in tree if tree successfully built,
 * or if query is empty.
 * Returns FALSE on any error (causing eventual engine abort).
 */
static int      inv_index_bin_tree (void)
{
    char	*cptr;
    DBLK	*dblk = usrblk.dblk;
    PARG	parg;

    /* First time initialize the first block of memory */
    if (memory_blocks == NULL) {
	/** INITIALIZE MEMORY **/
	init_memory ();
	if (usrblk.retncode == OE_ABORT)
	    return FALSE;
	root_node = NULL;
    }

    /* WORD LOOP.  Parse and stem each word in query.
     * Add each stem to bin tree or incr its count.
     */
    memset (&parg, 0, sizeof(PARG));
    parg.dblk = dblk;
    parg.string = usrblk.query;
    for ( cptr = dblk->parser (&parg);
	  cptr;
	  cptr = dblk->parser (NULL)) {
	build_bin_tree (dblk->stemmer (cptr, dblk));
	if (usrblk.retncode == OE_ABORT)
	    return FALSE;
    }

    return TRUE;
} /* inv_index_bin_tree() */


/************************/
/*			*/
/*	comp_stat	*/
/*			*/
/************************/
int             comp_stat (void *val1, void *val2)
{
    STAT_STR       *bkt1;
    STAT_STR       *bkt2;

    bkt1 = (STAT_STR *) val1;
    bkt2 = (STAT_STR *) val2;
    if ((bkt2->wght) > (bkt1->wght)) {
	return 1;
    }
    else {
	return -1;
    }
} /* comp_stat() */


/************************************************/
/*						*/
/*		load_ditto_str			*/
/*						*/
/************************************************/
/* Last function called from statistical search.
 * Builds a real AusText hitlist from the sorted stat_array,
 * translating the statistical weights to AusText 'proximity'
 * values, and truncating the hitlist at user's maxhits.
 * Working variables made static for speeeeeeeed.
 */
void            load_ditto_str (void)
{
    struct or_objrec cur_rec;	/* structure taken from austext.h */
    struct or_miscrec rec_data;
    static time_t   start_time;
    static double   time_dif;
    static DB_ADDR  dba1;
    static DtSrResult   *cur_ditto_mem;
    static DtSrResult   *ditto_llist;
    static DtSrResult   *temp_ditto;
    static int		debugging;
    static int		m;
    static DtSrINT32	d0024;
    static DtSrINT32	maxhits;
    static DtSrINT32	i32, i32_start, j32;
    static int      fzkeysz, fzkey_remaining, abstrsz, dittosz;
    static char    *src, *targ, *targend;
    static int	    check_dates = FALSE;
    static double   sum = 0.0;
    static double   sum1, sum2, sum3, sum4;

    debugging = (usrblk.debug & USRDBG_SRCHCMPL);
    maxhits = usrblk.dblk->maxhits;
    fzkeysz = usrblk.dblk->dbrec.or_fzkeysz;
    abstrsz = usrblk.dblk->dbrec.or_abstrsz;
    dittosz = sizeof (DtSrResult) + abstrsz + 16;
    if (debugging)
	fprintf (aa_stderr, PROGNAME "773 "
	    "numhits=%ld maxhits=%d numwords=%d abstrsz=%d\n",
	    (long)num_hits, (int)maxhits, num_diff_words, abstrsz);

    if (begin_load_ditto) {
	/* test for zero hits */
	if (num_hits == 0) {
	    usrblk.workproc = dummy_workproc;
	    usrblk.retncode = OE_NOTAVAIL;
	    if (OE_flags & OE_AUDIT)
		oe_write_audit_rec (0L);
	    release_shm_mem ();
	    return;
	}

	check_dates = (usrblk.objdate1 || usrblk.objdate2);

	/* In order to translate statistical weight into an AusText
	 * proximity, basically you have to invert it, then scale it.
	 * The statistical weight is a similarity measure: the
	 * larger it is the more similar the document to the query.
	 * But AusText 'proximity' is like a 'distance' measure,
	 * the smaller the number the closer the document is to the query.
	 *
	 * First 'normalize' each document's statistical
	 * weight to be a fraction between 0 and 1.  Do this
	 * by calculating a normalization factor (sum1), the
	 * sqrt of the sum of squares of first NORM_VALUE weights.
	 * (Trying to make the inversion scheme produce
	 * reasonable proximity numbers for these first records).
	 *
	 * To complete proximity initialization, he uses
	 * the sum1 factor to determine and keep the first record's
	 * normalized weight (sum), presumably a fraction close
	 * to 1.0, and the first record's proximity (sum2),
	 * basically the percent
	 * value that the first doc is 'distant' from perfection (1.0 or 100%).
	 * For example, if the normalized weight of the first record is .931
	 * then the proximity will be 7 (100% - 93% = 7%).  He does this now
	 * because he's going to use this first proximity (sum2) as a scaling
	 * factor to stretch out all the subsequent proximities so they
	 * look reasonable.
	 */
	sum = 0.0;
	for (i32 = 0; i32 < num_hits; i32++) {
	    sum1 = (double) (stat_array + i32)->wght /
		(double) num_diff_words;
	    sum += sum1 * sum1;
	    if (i32 >= NORM_VALUE)
		break;
	}
	/*
	 * sum1 = normalization factor.
	 * sum = normalized weight (betw 0 and 1) of first record.
	 * sum2 = proximity of first record, proximity scale factor.
	 */
	sum1 = sqrt (sum);
	sum = ((stat_array + 0)->wght / num_diff_words) / sum1;
	sum2 = (1.0 - sum) * 100.0;
	if (debugging)
	    fprintf (aa_stderr, PROGNAME "844 "
		"normfac=%.2lf normwt(#1)=%.2lf prox(#1)=%.2lf\n",
		sum1, sum, sum2);

	/* Preallocate first hit on ditto_list */
	ditto_llist = (DtSrResult *) austext_malloc (dittosz,
	    PROGNAME "449", NULL);
	j32 = 0;
	i32_start = 0;
	d0024 = OR_D00 << 24;
	begin_load_ditto = FALSE;
    }				/* endif (begin_load_ditto) */

    /* Test whether user has pushed STOP button since last call */
    if (usrblk.flags & USR_STOPSRCH) {
	if (OE_flags & OE_AUDIT)
	    oe_write_audit_rec (-1L);
	usrblk.retncode = OE_USER_STOP;
	release_shm_mem ();
	if (j32 == 0)
	    free (ditto_llist);
	else
	    free_llist ((LLIST **) &ditto_llist);
	return;
    }

    time (&start_time);

    /**** MAIN DtSrResult LIST BUILD LOOP ****/
    for (i32 = i32_start; i32 < num_hits; i32++) {
	/* check iteration loop */
	time_dif = difftime (time (NULL), start_time);
	if ((time_dif > TIME_ITERATION
		|| usrblk.debug & USRDBG_ITERATE) &&
	    !(usrblk.flags & USR_NO_ITERATE)) {
	    i32_start = i32;
	    usrblk.retncode = OE_SEARCHING;
	    usrblk.workproc = load_ditto_str;
	    mes_search_box = TRUE;
	    return;
	}

	dba1 = ((stat_array + i32)->dba * slot_d00 - dba_offset)
	    | d0024;

	/*
	 * Don't use CRSET or RECREAD macros here so we can trap invalid
	 * dba errs. 
	 */
	d_crset (&dba1, saveusr.vistano);
	if (db_status < 0) {
	    fprintf (aa_stderr, catgets (dtsearch_catd, MS_vestatis, 437,
		"%s: db_status = %d, dba = %d:%ld (x'%08.8lx'), vistano = %d\n"),
		PROGNAME "437", db_status, (dba1 & 0xff000000) >> 24,
		dba1 & 0xffffff, dba1, saveusr.vistano);
	    OE_flags |= OE_PERMERR;
	    usrblk.retncode = OE_ABORT;
	    release_shm_mem ();
	    return;
	}
	d_recread (&cur_rec, saveusr.vistano);
	if (db_status < 0) {
	    fprintf (aa_stderr, catgets (dtsearch_catd, MS_vestatis, 437,
		    "%s: db_status = %d, dba = %d:%ld (x'%08.8lx'), vistano = %d\n"),
		PROGNAME "437", db_status, (dba1 & 0xff000000) >> 24,
		dba1 & 0xffffff, dba1, saveusr.vistano);
	    OE_flags |= OE_PERMERR;
	    usrblk.retncode = OE_ABORT;
	    release_shm_mem ();
	    return;
	}
	swab_objrec (&cur_rec, NTOH);

	/* Skip any record with undesired keytype
	 * char, ie first char of key.
	 */
	if (*(rec_type_tab + cur_rec.or_objkey[0]) == 0)
	    continue;

	/* Skip record if out of date range. */
	if (check_dates)
	    if (!objdate_in_range (cur_rec.or_objdate,
			usrblk.objdate1, usrblk.objdate2))
		continue;

	if (j32 == 0)	/* first ditto node already allocated */
	    cur_ditto_mem = ditto_llist;
	else {
	    cur_ditto_mem = malloc (dittosz);
	    if (cur_ditto_mem == NULL) {
		fputs ( catgets (dtsearch_catd, MS_vestatis, 504,
		    PROGNAME "504 Cannot allocate cur_ditto\n"),
		    aa_stderr);
		OE_flags |= OE_PERMERR;
		usrblk.retncode = OE_ABORT;
		release_shm_mem ();
		return;
	    }
	    temp_ditto->link = cur_ditto_mem;
	}

	/* Load the ditto_list for this dba */
	memset (cur_ditto_mem, 0, sizeof(DtSrResult));
	cur_ditto_mem->dbn = OE_dbn;
	cur_ditto_mem->dba = dba1;
	strcpy (cur_ditto_mem->reckey,	cur_rec.or_objkey);
	cur_ditto_mem->objsize =	cur_rec.or_objsize;
	cur_ditto_mem->objdate =	cur_rec.or_objdate;
	cur_ditto_mem->objflags =	cur_rec.or_objflags;
	cur_ditto_mem->objuflags =	cur_rec.or_objuflags;
	cur_ditto_mem->objtype =	cur_rec.or_objtype;
	cur_ditto_mem->objcost =	cur_rec.or_objcost;

	/*****cur_ditto_mem->flags = 0;****/
	cur_ditto_mem->abstractp = (char *) cur_ditto_mem +
	    sizeof (DtSrResult);
	cur_ditto_mem->abstractp[0] = 0;

	/* Translate statistical weight into AusText proximity.
	 * sum3 = normalized weight (betw 0 and 1).
	 * sum4 = prox = ratio of this normalized weight to
	 * first rec's weight, scaled by the first rec's proximity.
	 * No proximity is allowed to exceed some very large number.
	 */
	sum3 = ((stat_array + i32)->wght / num_diff_words) / sum1;
	sum4 = sum2 * (sum / sum3);
	if (sum4 > INFINITY)
	    sum4 = INFINITY;
	cur_ditto_mem->proximity = sum4;

	if (debugging)
	    fprintf (aa_stderr,
		"  --> dba=%ld normwt=%.4lf prox=%d key='%s'\n",
		dba1, sum3, cur_ditto_mem->proximity,
		cur_ditto_mem->reckey);

	/*
	 * The abstract immediately follows the fuzzy key in the FZKABS
	 * misc recs.  It may span several recs. 
	 */
	if (abstrsz > 0) {
	    targ = cur_ditto_mem->abstractp;
	    targend = targ + abstrsz - 1;
	    fzkey_remaining = fzkeysz;
	    SETOR (PROGNAME "2270", OR_OBJ_MISCS, saveusr.vistano);
	    FINDFM (PROGNAME "2271", OR_OBJ_MISCS, saveusr.vistano);
	    while (db_status == S_OKAY) {
		RECREAD (PROGNAME "549", &rec_data, saveusr.vistano);
		NTOHS (rec_data.or_misctype);
		if (rec_data.or_misctype == ORM_FZKABS) {
		    src = (char *) rec_data.or_misc;
		    for (m = 0; m < sizeof(rec_data.or_misc); m++) {
			if (fzkey_remaining > 0) {
			    src++;
			    fzkey_remaining--;
			    continue;	/* inner for-loop on m */
			}
			*targ = *src;
			if (*src++ == 0 || targ++ >= targend) {
			    *targ = 0;
			    targ = targend;	/* make outer loop end */
			    break;
			}
		    }		/* end for-loop for curr misc rec */
		}		/* endif: misctype == FZKABS */
		if (targ >= targend)
		    break;
		FINDNM (PROGNAME "545", OR_OBJ_MISCS, saveusr.vistano);
	    }			/* end while-loop */
	}			/* endif: (abstrsz > 0) */

	cur_ditto_mem->link = NULL;
	temp_ditto = cur_ditto_mem;

	/* Increment to next hit.
	 * Break loop when we reach user's specified maxhits.
	 */
	j32++;	/* [j32 same as i] !? */
	if (j32 >= maxhits)
	    break;
    } /* i32-loop on each hit in ditto list */

    if (j32 == 0) {
	usrblk.workproc = dummy_workproc;
	usrblk.retncode = OE_NOTAVAIL;
	if (OE_flags & OE_AUDIT)
	    oe_write_audit_rec (0L);
	release_shm_mem ();
	return;
    }

    if (num_hits >= maxhits) {
	if (!(usrblk.flags & USR_NO_INFOMSGS)) {
	    sprintf (vestat_msgbuf, catgets (dtsearch_catd, MS_vestatis, 421,
		"$s Total Number Hits = %ld. Discarded hits beyond maximum number specified."),
		PROGNAME "421", (long)num_hits);
	    DtSearchAddMessage (vestat_msgbuf);
	}
    }

    free_llist ((LLIST **) &usrblk.dittolist);
    usrblk.dittolist = ditto_llist;
    usrblk.dittocount = j32;
    usrblk.workproc = dummy_workproc;
    usrblk.retncode = OE_OK;
    if (OE_flags & OE_AUDIT)
	oe_write_audit_rec ((long) num_hits);
    /***** Free shared memory *****/
    release_shm_mem ();
    return;
} /* load_ditto_str() */


/****************************************/
/*					*/
/*		stat_search		*/
/*					*/
/****************************************/
/* Subroutine of ve_statistical() and interruptable workproc.
 */
static void     stat_search (void)
{
    time_t          start_time;
    double          time_dif;
    DB_ADDR         temp, temp1;
    struct or_hwordrec word1;	/* structure taken from austext.h */
    double          idf, cur_weight;
    int		qs;
    DtSrINT32	int32, j32;
/*****@@@    size_t          size;****/
    static int      qs_start;

    /* Test whether user has pushed STOP button since last call */
    if (usrblk.flags & USR_STOPSRCH) {
	if (OE_flags & OE_AUDIT)
	    oe_write_audit_rec (-1L);
	usrblk.retncode = OE_USER_STOP;
	release_shm_mem ();
	return;
    }

    if (begin_sort) {
	begin_qsort = TRUE;
	qsort_done = FALSE;
	if (begin_search) {
	    qs_start = 0;
	    begin_search = FALSE;
	}

	time (&start_time);

	/*
	 * For every query stem, read d99.  For every dba in d99 for each
	 * stem, update object's stat array node with rec count and a
	 * weight based on the IDF for this stem. (IDF is described
	 * below). Saveusr.stemcount = lesser of DtSrMAX_STEMCOUNT or
	 * num_diff_words. All stems are stored in d99 beginning with ^O
	 * (decimal 15). Index qs = curr query stem
	 */
	for (qs = qs_start; qs < saveusr.stemcount; qs++) {
	    word1.or_hwordkey[0] = 15;
	    word1.or_hwordkey[1] = '\0';
	    strcat (word1.or_hwordkey, query_stems[qs].stem);
	    find_keyword (word1.or_hwordkey, saveusr.vistano);
	    /*
	     * If word is not in the database, ignore it. [ If word
	     * not in database, why not take the next stem in query_stems
	     * array, if any? ]
	     */
	    if (db_status != S_OKAY)
		word1.or_hwaddrs = 0;
	    else
		read_wordstr (&word1, saveusr.vistano);
	    if (word1.or_hwaddrs > 0) {
		fseek (usrblk.dblk->iifile, word1.or_hwoffset,
		    SEEK_SET);
		/****@@@size = sizeof (DB_ADDR) * word1.or_hwaddrs;***/
		fread (word_addrs, sizeof(DB_ADDR),
		    (size_t)word1.or_hwaddrs, usrblk.dblk->iifile);

		/*
		 * Calculate IDF (inverse document frequency) for this
		 * word. The IDF is a statistical ratio of the number
		 * of documents containing the word and the total
		 * number of documents in the entire corpus.
		 * It is calculated here on the fly to save space in the
		 * database. IDF = {log (totnumdocs / numdocswithword) /
		 * log(2)} + 1.  Note that an IDF of 1 means the word
		 * occurs in every doc (it's meaningless).  An IDF of 19
		 * means the word occurs once in every 300,000 recs.
		 * Note that by dividing by log(2) the IDF also tells
		 * us how many binary digits are necessary to discriminate
		 * the word.  Finally I think 1.0 was added to prevent
		 * it ever becoming zero when converted to integer.
		 */
		idf = (log ((double) real_num_rec / (double) word1.or_hwaddrs)
			/ LOG2) + 1.0;

		/*
		 * WEIGHT PASS #1:
		 * Update the stat array node for each doc (ie dba) which
		 * includes this stem.  Specifically,
		 * sum the product of the IDF and word-doc weight into
		 * the 'wght' bucket, and update the number of query
		 * words this doc contains. Note that the d99 dba format
		 * is slot# in hi 3 bytes, word-doc weights in lo byte.
		 */
		for (j32 = 0; j32 < word1.or_hwaddrs; j32++) {
		    NTOHL (word_addrs [j32]);
		    temp1 = *(word_addrs + j32);	/* d99 dba */
		    cur_weight = (double) (temp1 & 0xFF);	/* lo byte */
		    temp = temp1 >> 8;	/* slot# */
		    ((stat_array + temp)->num_word_hits)++;
		    ((stat_array + temp)->dba) = temp;
		    ((stat_array + temp)->wght) += (float) (cur_weight * idf);
		}
	    }			/* end if (word1.or_hwaddrs > 0), ie
				 * query word exists */

	    /*
	     * If the query words were common, the last double loop may
	     * have taken a long time.  If so, return now to the user
	     * interface to allow the gui to respond to button clicks
	     * (like CANCEL buttons). 
	     */
	    time_dif = difftime (time (NULL), start_time);
	    if ((time_dif > TIME_ITERATION
		    || usrblk.debug & USRDBG_ITERATE) &&
		!(usrblk.flags & USR_NO_ITERATE)) {
		if (qs == saveusr.stemcount - 1) {
		    usrblk.retncode = OE_SEARCHING;
		    usrblk.workproc = stat_search;
		    mes_search_box = TRUE;
		    return;
		}
		else {
		    qs_start = qs + 1;
		    usrblk.retncode = OE_SEARCHING;
		    usrblk.workproc = stat_search;
		    mes_search_box = TRUE;
		    return;
		}
	    }			/* end if (time_dif > TIME_ITERATION */
	}			/* end qs-loop on each query stem */

	/*
	 * Entire stat array contains one node for every possible dba
	 * (doc).  Collapse the records that were actually referenced by
	 * the query words into the top portion of the array.
	 * Set 'num_hits' to the collapsed stat array size, ie
	 * num_hits = the total number of docs that will be on
	 * the prelim hitlist, prior to sort and truncation to user's maxhits.
	 *
	 * WEIGHT PASS #2:
	 * While we're at it, finalize the accumulated 'wght' field, which
	 * will be our sort field, by multiplying it by the ratio of the
	 * number of query words in the document divided by the number of
	 * words in the query. 
	 * Thus the final sort field for each doc is the sum
	 * over all the query words in the doc of 3 factors:
	 * 1) IDF (relative weight of each query word in corpus), times
	 * 2) d99wght (relative weight of each query word in doc), times
	 * 3) weight based on number of different query words in this doc.
	 */
	num_hits = 0;
	for (int32 = 0; int32 < total_num_addrs; int32++) {
	    if (stat_array[int32].wght > 0) {
		(stat_array + num_hits)->num_word_hits =
		    (stat_array + int32)->num_word_hits;
		(stat_array + num_hits)->wght = (stat_array + int32)->wght *
		    ((double) (stat_array + int32)->num_word_hits /
		    (double) num_diff_words);
		(stat_array + num_hits)->dba = (stat_array + int32)->dba;
		num_hits++;
	    }
	}

	/*
	 * We're about to sort the actual hits. If the number of them
	 * exceeds a certain threshold, return to the user interface one
	 * more time to again allow the gui to respond to user CANCEL
	 * events. 
	 */
	if (num_hits > SORT_MESG && !(usrblk.flags & USR_NO_ITERATE)) {
	    if (!mes_search_box) {
		DtSearchAddMessage (catgets (dtsearch_catd, MS_vestatis, 990,
		    PROGNAME"990 The system is now sorting. Please wait."));
	    }
	    usrblk.retncode = OE_SEARCHING;
	    usrblk.workproc = stat_search;
	    mes_search_box = TRUE;
	    begin_sort = FALSE;
	    return;
	}

    }				/* end if (begin_sort) */

    /* Sort the preliminary hitlist (stat_array)
     * by the calculated statistical weights.
     */
    if (!efim_qsort ())
	return;

    /* Build a real AusText hitlist from the sorted stat_array,
     * translating the statistical weights to AusText 'proximity'
     * values, and truncating the hitlist at user's maxhits.
     */
    if (qsort_done) {
	begin_load_ditto = TRUE;
	load_ditto_str ();
    }

    return;
} /* stat_search() */



/****************************************/
/*					*/
/*	      ve_statistical		*/
/*					*/
/****************************************/
void            ve_statistical (void)
{
    void            stat_search (void);
    DB_ADDR         dba;
    int             i, j;
    DtSrINT32	int32;

    mes_search_box = FALSE;
    usrblk.flags &= ~USR_STOPSRCH;	/* turn off stop button */
    usrblk.retncode = OE_OK;
    usrblk = usrblk;
    saveusr.vistano = usrblk.dblk->vistano;
    saveusr.dittolist = NULL;
    saveusr.dittocount = 0L;
    saveusr.iterations = 1;

    /****** find total number of records in the database *********/
    RECFRST (PROGNAME "1067", OR_OBJREC, saveusr.vistano);
    CRGET (PROGNAME "1068", &dba, saveusr.vistano);
    real_num_rec = usrblk.dblk->dbrec.or_reccount;
    slot_d00 = usrblk.dblk->dbrec.or_recslots;
    dba_offset = slot_d00 - (dba & 0x00FFFFFF);
    total_num_addrs = (usrblk.dblk->dbrec.or_maxdba -
	(dba & 0x00FFFFFF) + 1) / slot_d00 + 1;
    /* stat_array size = 1 node for every possible object */

    if (usrblk.query[0] == 0) {
	DtSearchAddMessage (catgets (dtsearch_catd, MS_vestatis,
		677, PROGNAME "677 Query field is empty."));
	usrblk.retncode = OE_BAD_QUERY;
	return;
    }

    /*
     * Build binary tree of each stem in query containing count of number
     * of occurrences of stem in query. Loads num_diff_words with number
     * of nodes in tree. 
     */
    num_diff_words = 0;
    inv_index_bin_tree();
    if (usrblk.retncode == OE_ABORT)
	return;
    if (num_diff_words < 1) {
	usrblk.retncode = OE_NOTAVAIL;
	return;
    }

    /***** allocate memory for query_stems array *********/
    if (query_stems != NULL) {
	free (query_stems);
	query_stems = NULL;
    }
    query_stems = (QUERY_STEM_STR *) austext_malloc
	(sizeof (QUERY_STEM_STR) * (num_diff_words + 1),
	PROGNAME " 371", NULL);

    /*
     * Traverse tree to build query_stems array, each array node = tree
     * node, ie each unique stem in query and its count in query.
     * Num_diff_words now used as index for growing array. 
     */
    num_diff_words = 0;
    traverse_tree ();

    /*
     * For each new query initialize memory offset, current memory start
     * position, and total size for the available memory. Starts from the
     * first member in the link list of memory blocks. 
     */
    root_node = NULL;
    mem_start = memory_blocks->start_of_mem_block;
    total_memory_size = memory_blocks->block_size;
    cur_mem_ptr = memory_blocks->next_block;
    cur_pos = mem_start;
    mem_offset = 0L;

    /*
     * Copy first DtSrMAX_STEMCOUNT stems into the saveusr.stems. [So no more
     * than DtSrMAX_STEMCOUNT will be used in search or hiliting!] 
     */
    for (i = 0; i < num_diff_words; i++) {
	if (i == DtSrMAX_STEMCOUNT)
	    break;
	strcpy (usrblk.stems[i], query_stems[i].stem);
    }
    usrblk.stemcount = i;
    saveusr.stemcount = i;

    /* Prepare a string holding first char of desired record ids */
    for (i = 0; i < REC_TYPES; i++)
	*(rec_type_tab + i) = 0;
    for (i = 0, j = 0; i < usrblk.dblk->ktcount; i++)
	if (usrblk.dblk->keytypes[i].is_selected)
	    *(rec_type_tab + usrblk.dblk->keytypes[i].ktchar) = 1;
    saveusr.ktchars[j] = '\0';

    /*
     * New code using shared memory:
     * Allocate global block of shared memory,
     * and assign parts of this memory to each array.
     * Stat array has an element for every possible db object.
     * Set whole stat array to binary zeroes.
     */
    if (!init_global_memory (total_num_addrs, real_num_rec))
	return;
    stat_array = (STAT_STR *) global_memory_ptr;
    word_addrs = (DB_ADDR *) (global_memory_ptr +
	total_num_addrs * sizeof (STAT_STR));
    for (int32 = 0; int32 < total_num_addrs; int32++) {
	(stat_array + int32)->wght = 0.0;
	(stat_array + int32)->num_word_hits = 0;
    }
    /***** end of memory allocation for statistical array *********/

    /* stat_search(): Search d99 and sum the statistical weights.
     * Calls efim_qsort() to sort the hitlist by the weights.
     */
    begin_search = TRUE;	/* global initialization and state flags */
    begin_sort = TRUE;
    stat_search ();

    return;
} /* ve_statistical() */

/*************************** VESTATIS.C ****************************/
