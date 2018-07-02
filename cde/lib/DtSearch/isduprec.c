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
 *   FUNCTIONS: dump_hashtab
 *		is_duprec
 *		main
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
/******************* ISDUPREC.C *******************
 * $XConsortium: isduprec.c /main/5 1996/05/07 13:37:35 drk $
 * June 1993.
 * Is_duprec() returns 0 (FALSE) for every record id it is passed
 * unless one is passed that duplicates a previous one,
 * in which case it returns 1 (TRUE).
 * It ensures that duplicate record ids in an .fzk file
 * are not processed by either ravel or borodin.
 * It does it by storing each recid into a hash table and
 * searching the table before storing a new recid.
 * Returns 2 on errors (malloc out of space, etc);
 *
 * Global 'duprec_hashsize' can be changed to any rational value
 * for a hash table size (say 1000 to 30,000) prior to the first call
 * of is_duprec().  It should be roughly => to the total number of
 * different record ids expected to be passed to is_duprec().
 * If initialized to 0 before the first call, that will disable
 * duplicate checking, i.e. is_duprec() will allocate no memory
 * and always return 0.
 *
 * $Log$
 * Revision 2.2  1995/10/25  17:22:48  miker
 * Added prolog.
 *
 * Revision 2.1  1995/09/22  20:56:44  miker
 * Freeze DtSearch 0.1, AusText 2.1.8
 *
 * Revision 1.3  1995/09/05  18:11:45  miker
 * Minor changes so ansi c compilers won't whine.
 */
#include <stdlib.h>
#include <string.h>

#ifdef TEST
#include <stdio.h>
#include <errno.h>
#endif

#define PROGNAME	"ISDUPREC"
#define HASHSIZE	3000L
#define NOT_A_DUP	0
#define IS_A_DUP	1
#define OUT_OF_MEM	2

unsigned long   duprec_hashsize = HASHSIZE;

/************************************************/
/*						*/
/*		    HASHNODE			*/
/*						*/
/************************************************/
/* The hash table is a HASHSIZE array of pointers to these structures.
 * Each pointer is initialized to NULL.
 * Additions are handled by filling in a HASHNODE pointed to
 * by the table pointer.  The 'recid' is NOT a char array of length
 * 1, but a string whose length varies depending on the actual
 * length of the passed record id.  Each hashnode is malloced
 * for exactly the right length.  Collisions are handled by linking
 * additional nodes off of the original one.
 */
typedef struct hash_tag {
    struct hash_tag *link;
    char            recid[2];	/* actual array size varies */
}               HASHNODE;


#ifdef TEST
/************************************************/
/*						*/
/*		  dump_hashtab()		*/
/*						*/
/************************************************/
/* For debugging, prints out all recids in hashtab, skipping empty bkts */
static void     dump_hashtab (HASHNODE ** hashtab)
{
    HASHNODE       *hp, **hpp;
    int             i;
    printf (PROGNAME "67 dump_hashtab(%p):\n", hashtab);
    for (i = 0, hpp = hashtab; i < duprec_hashsize; i++, hpp++) {
	if (*hpp) {
	    printf ("  %4d:", i);
	    fflush (stdout);
	    for (hp = *hpp; hp != NULL; hp = hp->link)
		printf (" '%s'", hp->recid);
	    putchar ('\n');
	    fflush (stdout);
	}
    }
    return;
}  /* dump_hashtab() */

#endif	/* TEST */


/************************************************/
/*						*/
/*		   is_duprec()			*/
/*						*/
/************************************************/
/* Normal return is 0 indicating that passed record id is unique.
 * Also immediately returns 0 if duplicate checking has been
 * turned off by setting global 'duprec_hashsize' to zero.
 * Returns 1 if record id is a duplicate.
 * Returns 2 if out of memory.
 * First call uses 'duprec_hashsize' to create hash table.
 */
int             is_duprec (char *recid)
{
    static HASHNODE **hashtab = NULL;
    static unsigned long primes[10] =
    {1013, 1511, 2203, 3511, 5003, 10007, 15013, 20011, 25013, 30001};

    unsigned long   i;
    char           *cp;
    unsigned long   sum;
    HASHNODE       *hp, **hpp;

    if (duprec_hashsize == 0UL)
	return NOT_A_DUP;

/* Generate hash table at first call only */
    if (hashtab == NULL) {
	/*
	 * adjust table size upward to nearest preordained prime
	 * number 
	 */
	for (i = 0; i < 9 && primes[i] < duprec_hashsize; i++);
	duprec_hashsize = primes[i];
#ifdef TEST
	printf (PROGNAME "117 Create hash table, duprec_hashsize set = %ld.\n",
	    duprec_hashsize);
#endif

	hashtab = malloc ((duprec_hashsize + 2L) * sizeof (HASHNODE *));
	if (hashtab == NULL)
	    return OUT_OF_MEM;

	/* init table to all NULL pointers. */
	hpp = hashtab;
	for (i = duprec_hashsize + 2L; i > 0L; i--)
	    *hpp++ = NULL;
    }

    /*****dump_hashtab(hashtab);******/

    /* HASH FUNCTION:  H(recid) = (SUM(i*recid[i])) mod M,
     * where M is table size (prime), and SUM is calculated
     * for i=1 to end of recid.  Multiplying the position by the character
     * value at that position minimizes the influence of identical
     * characters at the beginnings and ends of recids,
     * and also usually yields a number larger than M.
     * Not skipping over the first position (the keytype char) helps
     * efficiently catch recids that are blank after the keytype.
     */
    sum = 0UL;
    i = 1;
    cp = recid;
    while (*cp != 0)
	sum += i++ * (*cp++);
    hpp = &(hashtab[sum % duprec_hashsize]);	/* hpp = head of linked
						 * list */

#ifdef TEST
    printf (PROGNAME "150 is_duprec('%s')=hashtab[%lu]=%p: ",
	recid, sum % duprec_hashsize, *hpp);
    fflush (stdout);
    i = 0;
#endif

    /* Search linked list (if any) for hashnode containing recid */
    for (hp = *hpp; hp != NULL; hp = hp->link) {
#ifdef TEST
	i++;
#endif

	if (strcmp (hp->recid, recid) == 0) {
#ifdef TEST
	    printf ("DUP!@listpos=%d\n", i);
#endif
	    return IS_A_DUP;
	}
	hpp = &hp->link;	/* now hpp = tail of linked list */
    }
#ifdef TEST
    printf ("miss@listlen=%d\n", i);
#endif

    /* Not a duplicate.  Add current recid to hash table. */
    if ((hp = malloc (sizeof (HASHNODE) + strlen (recid) + 2)) == NULL)
	return OUT_OF_MEM;
    strcpy (hp->recid, recid);
    hp->link = NULL;
    /*****hp->link = *hpp;******/
    *hpp = hp;
    return NOT_A_DUP;
}  /* is_duprec() */


#ifdef MAIN
/************************************************/
/*						*/
/*		     main()			*/
/*						*/
/************************************************/
main (int argc, char *argv[])
{
    int             i;
    FILE           *f;
    char            buf[2048];

    if (argc < 2) {
	printf ("USAGE: %s <file> [n]\n"
	    "where file contains list of char strings\n"
	    "and optional n changes hash table size.\n",
	    argv[0]);
	return;
    }
    if ((f = fopen (argv[1], "r")) == NULL) {
	printf ("Can't open %s: %s\n", argv[1], strerror (errno));
	return;
    }
    if (argc >= 3)
	duprec_hashsize = atol (argv[2]);

    while (fgets (buf, sizeof (buf), f) != NULL) {
	buf[sizeof (buf) - 1] = 0;
	i = is_duprec (buf);
	printf ("%s", buf);	/* each buf should end in \n */
	if (i > 1)
	    break;
    }
    return;
}

#endif
/******************* ISDUPREC.C *******************/
