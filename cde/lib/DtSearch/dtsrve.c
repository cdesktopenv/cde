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
 *   FUNCTIONS: append_blob
 *		dummy_workproc
 *		store_next_misc
 *		ve_append_notes
 *		ve_browse_dba
 *		ve_getblobs
 *		ve_getrec_dba
 *		ve_initialize
 *		ve_reckey2dba
 *		ve_shutdown
 *
 *   ORIGINS: 27
 *
 *
 *   (C) COPYRIGHT International Business Machines Corp. 1991,1995
 *   All Rights Reserved
 *   Licensed Materials - Property of IBM
 *   US Government Users Restricted Rights - Use, duplication or
 *   disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 */
/**************************** DTSRVE.C ******************************
 * $XConsortium: dtsrve.c /main/8 1996/11/21 19:49:59 drk $
 * Sept 1991.
 * The 'vista engine' of opera.
 * Contains all modules that actually access the database.
 * Theoretically, if opera replaced vista with some other DBMS
 * this is the only module that would have to be modified.
 *
 * $Log$
 * Revision 2.4  1996/02/01  18:48:49  miker
 * Enhanced blob retrieval debug msgs.
 *
 * Revision 2.3  1995/10/25  18:08:27  miker
 * Renamed from ve.c.  Added prolog.
 *
 * Log: ve.c,v
 * Revision 2.2  1995/10/19  21:02:35  miker
 * Open mode of non-vista d9x files now tracks db_oflag.
 *
 * Revision 2.1  1995/09/22  22:26:36  miker
 * Freeze DtSearch 0.1, AusText 2.1.8
 *
 * Revision 1.12  1995/09/05  19:18:56  miker
 * Made usrblk global.  Name, msgs, etc changes for DtSearch.
 * Made ausapi_msglist global.  Deleted obsolete socblk refs.
 * Added DTSEARCH define.
 *
 * Revision 1.11  1995/07/18  22:29:18  miker
 * Delete msglist arg from vista_abort() function calls.
 */
#include "SearchE.h"
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include "vista.h"

#define XOS_USE_NO_LOCKING
#define X_INCLUDE_TIME_H
#include <X11/Xos_r.h>

#define PROGNAME	"DTSRVE"
#define NOTES_SEM_DELAY	3
#define MS_misc		1
#define MS_ve		6
#define MS_oeinit	9

extern time_t   hctree_id;	/**** hardcoded only temporarily ******/
static int      max_abstrbufsz = 0;
static int      max_ormisc_size;

char    *strupr(char *);

/************************************************/
/*						*/
/*		 dummy_workproc			*/
/*						*/
/************************************************/
/* Loaded by any workproc when it has successfully completed.
 * Should never be called because GUI should turn off workproc
 * calls after real workproc completes with OE_OK.
 */
void            dummy_workproc (void)
{
    fputs (catgets (dtsearch_catd, MS_ve, 26,
	PROGNAME "26 Called dummy_workproc().\n"),
	aa_stderr);
    return;
}  /* dummy_workproc() */


/************************************************/
/*						*/
/*		append_blob			*/
/*						*/
/************************************************/
/* Mallocs space for new compressed vista text record (blob)
 * appends copy of passed blob to passed link address.
 * Subroutine of ve_getrec_dba() and ve_getblobs() below.
 * Similar to append_msglist() function except that data string
 * is not presumed to be terminated with \0.  Instead the entire
 * vista member record is copied, as binary bytes,
 * irrespective of their contents.
 * This function allocates memory for the blobs but DOES NOT FREE IT.
 * free_llist() must be called before building a new bloblist.
 */
static LLIST   *append_blob (LLIST ** bloblink,
                    struct or_blobrec * blobrec)
{
    LLIST          *new;

    new = austext_malloc (sizeof (struct or_blobrec) + sizeof (LLIST) + 4,
	PROGNAME "36", NULL);
    new->data = new + 1;	/* hop over exactly 1 LLIST
				 * structure */
    new->link = NULL;
    *bloblink = new;
    memcpy (new->data, blobrec, sizeof (struct or_blobrec));
    return new;
}  /* append_blob() */


/************************************************/
/*						*/
/*		ve_initialize			*/
/*						*/
/************************************************/
/* Opens databases in usrblk (calls open_dblk()), and reads dbrecs.
 * Returns TRUE if 1 or more dblks survived the opening ordeal.
 * Returns FALSE if no dblks survived.
 */
int             ve_initialize (void)
{
    DBLK*db, *bad_db, **lastlink;
    int		debugging = (usrblk.debug & USRDBG_RARE);	/* boolean */
    char	msgbuf[1024];
    char	d9x_fname[1024];
    char	*d9x_fext;
    int		good_dblk_count;
    char	open_mode [8];
    static char	default_cant_open_msg[] =
	    "%s Cannot open database file '%s', errno %d = %s. "
	    "%s is removing '%s' from list of available databases.";

    /* ---- PASS #1 and #2 ------------------------------------------
     * Open the d99 and vista database files.
     */
    if (!open_dblk (&usrblk.dblist, 64, debugging))
	return FALSE;

    /* ---- PASS #3 ------------------------------------------
     * (1) Read dbrec database-wide globals for each database.
     * (2) Determine max abstract size from largest abstrsz.
     * (3) Open other nonvista (d9x) files.
     *     Open mode is determined by value of db_oflag.
     * Disconnect any dblks with invalid dbrecs or d9x files.
     */
    if (db_oflag == O_RDONLY)
	strcpy (open_mode, "rb");
    else
	strcpy (open_mode, "r+b");
    if (debugging)
	fprintf (aa_stderr, PROGNAME "76 "
	    "Begin dblks Pass #3 in ve_initialize().  Open mode '%s'.\n",
	    open_mode);
    good_dblk_count = 0;
    db = usrblk.dblist;
    lastlink = &usrblk.dblist;
    while (db != NULL) {
	/*-------------- READ DBREC ----------------*/
	if (debugging)
	    fprintf (aa_stderr,
		"--> reading dbrec for database '%s':\n",
		db->name);

	RECFRST (PROGNAME "285", OR_DBREC, db->vistano); /* seqtl retrieval */
	if (db_status != S_OKAY) {
    NO_DBREC:
	    sprintf (msgbuf, catgets (dtsearch_catd, MS_misc, 13,
		    "%s No DB record in database '%s'."),
		PROGNAME "853 ", db->name);
	    DtSearchAddMessage (msgbuf);
	    goto DELETE_DB;
	}
	RECREAD (PROGNAME "302", &db->dbrec, db->vistano);
	if (db_status != S_OKAY)
	    goto NO_DBREC;
	swab_dbrec (&db->dbrec, NTOH);

	if (db->dbrec.or_abstrsz > usrblk.abstrbufsz) {
	    if (debugging)
		fprintf (aa_stderr,
		    "\t(changing usrblk.abstrbufsz from %d to %d).\n",
		    usrblk.abstrbufsz, db->dbrec.or_abstrsz);
	    usrblk.abstrbufsz = db->dbrec.or_abstrsz;
	}

	/*-------------- DBREC SANITY CHECKS ----------------*/
	if (db->dbrec.or_reccount <= 0) {
	    sprintf (msgbuf, catgets (dtsearch_catd, MS_ve, 167,
		     "%s No data in database '%s'."),
		PROGNAME"167 ", db->name);
	    DtSearchAddMessage (msgbuf);
	    goto DELETE_DB;
	}
	if (!is_compatible_version (db->dbrec.or_version, SCHEMA_VERSION)) {
	    sprintf (msgbuf, catgets (dtsearch_catd, MS_ve, 178,
		    "%s Database '%s' version '%s' incompatible"
		    " with Engine version '%s'."),
		PROGNAME"178 ",
		db->name, db->dbrec.or_version, AUSAPI_VERSION);
	    DtSearchAddMessage (msgbuf);
	    goto DELETE_DB;
	}
	if (db->dbrec.or_reccount > db->dbrec.or_maxdba) {
	    sprintf (msgbuf, catgets (dtsearch_catd, MS_ve, 251,
		    "%s Database '%s' corrupted: "
		    "Incompatible record counts and database addresses.\n"),
		PROGNAME" 251", db->name);
	    DtSearchAddMessage (msgbuf);
	    goto DELETE_DB;
	}
	if (db->dbrec.or_maxwordsz < MAXWIDTH_SWORD - 1) {
	    sprintf (msgbuf, catgets (dtsearch_catd, MS_ve, 185,
		"%s Database '%s' maximum word size %d is too short."),
		PROGNAME" 185", db->name, db->dbrec.or_maxwordsz);
	    DtSearchAddMessage (msgbuf);
	    goto DELETE_DB;
	}
	if (db->dbrec.or_hufid != 0L && db->dbrec.or_hufid != hctree_id) {
	    /*
	     * for now, huffman decompress table hardcoded and
	     * linked in 
	     */
	    sprintf (msgbuf, catgets (dtsearch_catd, MS_ve, 156,
		"%s Incompatible data compression table used for database '%s'.\n"
		"  Database compressed with %ld, "
		"engine decompressor is %ld.\n"),
		PROGNAME" 156", db->name, db->dbrec.or_hufid, hctree_id);
	    DtSearchAddMessage (msgbuf);
	    goto DELETE_DB;
	}
	/* dbrec ok: print debug msg */
	if (debugging) {
	    fprintf (aa_stderr,
		"\tvers='%s' reccount=%ld maxdba=%ld fzkeysz=%d\n"
		"\tdbflags=x%lx maxwordsz=%d hufid=%ld abstrsz=%d\n",
		db->dbrec.or_version, (long) db->dbrec.or_reccount,
		(long) db->dbrec.or_maxdba, db->dbrec.or_fzkeysz,
		(unsigned long) db->dbrec.or_dbflags, db->dbrec.or_maxwordsz,
		(long) db->dbrec.or_hufid, db->dbrec.or_abstrsz);
	}

	/*-------------- OPEN D97 and D98 FILES ----------------
         * If semantic (symptom) search is enabled,
         * open the d97 (offsets table) and d98 (index) files.
         */
	if (db->dbrec.or_fzkeysz > 0) {
	    /* build complete path-file name */
	    strcpy (d9x_fname, db->path);
	    strcat (d9x_fname, db->name);
	    d9x_fext = d9x_fname + strlen (d9x_fname);
	    strcpy (d9x_fext, ".d97");
	    if (debugging)
		fprintf (aa_stderr, "--> opening '%s'\n", d9x_fname);

	    if ((db->syofile = fopen (d9x_fname, open_mode)) == NULL) {
		sprintf (msgbuf, catgets (dtsearch_catd, MS_oeinit, 317,
			default_cant_open_msg), PROGNAME "286",
		    d9x_fname, errno, strerror (errno), OE_prodname, db->name);
		DtSearchAddMessage (msgbuf);
		goto DELETE_DB;
	    }

	    strcpy (d9x_fext, ".d98");
	    if (debugging)
		fprintf (aa_stderr, "--> opening '%s'\n", d9x_fname);

	    if ((db->syifile = fopen (d9x_fname, open_mode)) == NULL) {
		sprintf (msgbuf, catgets (dtsearch_catd, MS_oeinit, 317,
			default_cant_open_msg), PROGNAME "298",
		    d9x_fname, errno, strerror (errno), OE_prodname, db->name);
		DtSearchAddMessage (msgbuf);
		goto DELETE_DB;
	    }
	}	/* endif to open d97 and d98 files */


	/*---------------------- DB OK -------------------------
         * This dblk passed all dbrec validations and all other
         * d9x files were available.  Increment pointers and continue.
         */
	if (debugging)
	    fprintf (aa_stderr, "------> dblk '%s' ok in veinitialize()\n",
		db->name);
	good_dblk_count++;
	lastlink = &db->link;
	db = db->link;
	continue;

	/*---------------------- DELETE DB -------------------------
         * This dblk failed one or more dbrec validity checks.
         * Unlink it and don't increment pointers.
         */
DELETE_DB:
	if (debugging)
	    fprintf (aa_stderr, "------> ERROR UNLINK '%s'.\n", db->name);
	bad_db = db;	/* temp save */
	*lastlink = db->link;
	db = db->link;
	free (bad_db);
    }	/* end PASS #3 */

    /* Quit if no dblks remain */
    if (good_dblk_count <= 0) {
	sprintf (msgbuf, catgets (dtsearch_catd, MS_misc, 8,
		"%s No valid databases remain."), PROGNAME "246");
	DtSearchAddMessage (msgbuf);
	return FALSE;
    }

    /* Allocate an abstract buffer for the usrblk
     * if any abstracts are used in any database.
     * The size is saved in case the client doesn't
     * return the buffer in subsequent calls.
     */
    if (usrblk.abstrbufsz) {
	max_abstrbufsz = usrblk.abstrbufsz;	/* save */
	usrblk.abstrbuf = austext_malloc (usrblk.abstrbufsz + 4,
	    PROGNAME "274", NULL);
	if (debugging)
	    fprintf (aa_stderr,
		PROGNAME "282 Allocating %d bytes for usrblk.abstrbuf.\n",
		usrblk.abstrbufsz + 4);
    }
    else if (debugging)
	fprintf (aa_stderr, PROGNAME "284 usrblk.abstrbuf NOT allocated.\n");

    return TRUE;
}  /* ve_initialize() */


/************************************************/
/*						*/
/*		ve_shutdown			*/
/*						*/
/************************************************/
/* closes databases */
void	ve_shutdown (void)
{
    d_close ();
    austext_exit_dbms = NULL;
    return;
}

/************************************************/
/*						*/
/*		ve_append_notes			*/
/*						*/
/************************************************/
/* Appends user notes in usrblk.query to
 * opera record specified by usrblk.dba.
 * usrblk.dba is presumed valid opera record.
 * Saves all appends in separate backup flat file 
 * for restoring database after a crash or initdb.
 * 
 * The technique to prevent 2 users from updating
 * at the same time does not require vista locking.
 * This function considers itself a 'critical region' and
 * uses a value in a special file as a semaphore to prevent multiple
 * users from threading through it at the same time.
 * 
 * Does NOT change status of current record in usrblk--it only uses dba.
 * Returns OE_DISABLED if function disabled in or_dbflags or global var.
 * Returns OE_OK after successful append.
 * Returns OE_TIMEOUT if a user cannot acquire the semaphore
 * 	after a reasonable length of time.
 * Returns OE_ABORT on fatal error.
 */
int             ve_append_notes (void)
{
    time_t	mystamp;
    FILE	*backup_file, *semaphore_file;
    size_t	mylen;
    int		done;
    int		i;
    int		vistano;
    char	*ptr;
    char	*entirebufptr, *appendbufptr;
    char	mybuf[160];
    static char	formfeed_line[] = "\f\n";
    struct or_miscrec
		miscrec;
    struct tm	*time_ptr;

    /* Test if function is disabled */
    if (!OE_enable_usernotes || usrblk.dblk->dbrec.or_dbflags & ORD_NONOTES) {
	sprintf (mybuf, catgets (dtsearch_catd, MS_ve, 309,
		"%s User notes disabled "), PROGNAME" 309");
	ptr = mybuf + strlen (mybuf);
	if (!OE_enable_usernotes)
	    strcpy (ptr, catgets (dtsearch_catd, MS_ve, 310,
		"for entire Engine."));
	else
	    sprintf (ptr, catgets (dtsearch_catd, MS_ve, 311,
		"for database '%s'."),
		usrblk.dblk->name);
	DtSearchAddMessage (mybuf);
	return OE_DISABLED;
    }

    /* Test for invalid dba */
    if (usrblk.dba == NULL_DBA) {
	DtSearchAddMessage (catgets (dtsearch_catd, MS_ve, 157,
		PROGNAME "157 Client Program Error: "
		"Null database address in usrblk.dba."));
	OE_flags |= OE_PERMERR;
	return OE_ABORT;
    }

    /* Acquire the semaphore:  Open the semaphore file.
     * If first char = '1', somebody is already in the critical region.
     * If '1' remains in file after several tries, quit with FALSE retncode.
     * If first char = '0', critical region is available for this task.
     * Write a '1' and enter the region.
     */
    i = 0;	/* loop counter */
    for (;;) {
	if ((semaphore_file = fopen (OEF_notessem, "r+")) == NULL) {
	    sprintf (mybuf,
		catgets (dtsearch_catd, MS_ve, 183, 
		    "%s Could not open user notes semaphore file '%s': %s.\n"),
		PROGNAME "183 ", OEF_notessem, strerror (errno));
	    DtSearchAddMessage (mybuf);
	    return OE_TIMEOUT;
	}
	fread (mybuf, 1, 1, semaphore_file);

	/*
	 * If semaphore is available, grab it and enter critical
	 * region 
	 */
	if (*mybuf == '0') {
	    rewind (semaphore_file);
	    fwrite ("1", 1, 1, semaphore_file);
	    fflush (semaphore_file);
	    break;
	}

	/*
	 * Otherwise check that we havent looped too often, and try
	 * again 
	 */
	fclose (semaphore_file);
	if (++i > NOTES_SEM_DELAY) {
	    sprintf (mybuf,
		catgets (dtsearch_catd, MS_ve, 199,
		    "%s Could not acquire user notes semaphore '%s' "
		    "within %d tries.\n"),
		PROGNAME " 199", OEF_notessem, NOTES_SEM_DELAY);
	    DtSearchAddMessage (mybuf);
	    return OE_TIMEOUT;
	}
	sleep (1);	/* wait a second */
    }	/* end of semaphore loop */

    /* We have acquired the semaphore, beginning of critical region... */

    /* Enlarge the buffer so we can prefix users text with the record key 
     * (for the backup file), and a header line.
     */
    entirebufptr = austext_malloc
	(DtSrMAX_DB_KEYSIZE + sizeof (mybuf) + strlen (usrblk.query),
	PROGNAME "170", NULL);
    sprintf (entirebufptr, "%s\n%s",
	usrblk.dblk->name, usrblk.objrec.or_objkey);

    /* Now add a timstamped, user identified 'header' line */
    appendbufptr = entirebufptr + strlen (entirebufptr);
    time (&mystamp);
    time_ptr = _XLocaltime(&mystamp, localtime_buf);
    strftime (mybuf, sizeof (mybuf), catgets (dtsearch_catd, MS_ve, 332,
	    "%Y/%m/%d at %H:%M %Z"), time_ptr);
    sprintf (appendbufptr, catgets (dtsearch_catd, MS_ve, 333,
	    "\n    <User Note Appended by '%s' on %s>\n"),
	usrblk.userid, mybuf);
    strcat (appendbufptr, usrblk.query);	/* now add user's text */

    /* Make sure users note ends in \n */
    ptr = appendbufptr + strlen (appendbufptr);
    if (*(ptr - 1) != '\n') {
	*ptr++ = '\n';
	*ptr = 0;
    }

    /* Append text to current list of notes */
    vistano = usrblk.dblk->vistano;
    CSOSET (PROGNAME "153", OR_OBJ_MISCS, &usrblk.dba, vistano);
    ptr = appendbufptr;
    done = FALSE;
    while (!done) {
	i = strlen (ptr);	/* i = remaining amount of text */
	if (i < sizeof (miscrec.or_misc)) {
	    strcpy ((char *) miscrec.or_misc, ptr);
	    done = TRUE;
	}
	else {
	    i = sizeof (miscrec.or_misc) - 1;	/* now i = amt of curr
						 * write only */
	    strncpy ((char *) miscrec.or_misc, ptr, i);
	    miscrec.or_misc[0][i] = 0;
	    ptr += i;
	}
	miscrec.or_misctype = ORM_OLDNOTES;
	HTONS (miscrec.or_misctype);
	FILLNEW (PROGNAME "169", OR_MISCREC, &miscrec, vistano);
	CONNECT (PROGNAME "170", OR_OBJ_MISCS, vistano);
    }	/* end of vista append loop */

    /* Also append the note to the backup flat file */
    if ((backup_file = fopen (OEF_notesnot, "at ")) == NULL) {
	sprintf (mybuf,
	    catgets (dtsearch_catd, MS_ve, 230,
		"%s Could not open user notes backup file '%s': %s."),
	    PROGNAME " 230", OEF_notesnot, strerror (errno));
	DtSearchAddMessage (mybuf);
    }
    else {
	mylen = strlen (entirebufptr);
	strcpy (entirebufptr + mylen, formfeed_line);
	mylen += sizeof (formfeed_line);
	fwrite (entirebufptr, --mylen, 1, backup_file);
	fclose (backup_file);
    }

    free (entirebufptr);

    /* End of critical region.... 
     * release the semaphore so somebody else can append. 
     */
    rewind (semaphore_file);
    fwrite ("0", 1, 1, semaphore_file);
    fclose (semaphore_file);

    return OE_OK;
}  /* ve_append_notes() */


/************************************************/
/*						*/
/*		 store_next_misc		*/
/*						*/
/************************************************/
/* Subroutine of ve_getrec_dba().  Repeatedly called
 * for each read of a miscrec of type ORM_FZKABS
 * to load usrblk fields objfzkey and objabstr.
 * First call for a given object is signaled by passed arg.
 * Thereafter static pointers keep track of where we are
 * in usrblk buffers to correctly store data from next
 * miscrec.  Works as a state machine: initial state
 * is store-fzkey, then store-abstract,
 * but only if either or both of those exist.
 * Code similar to load_next_... function in cravel.c.
 * WARNING! maximum size of fzkey is still hardcoded FZKEYSZ!
 */
static void     store_next_misc (
                	int	is_first_misc,
                	char	*misc	/* miscrec.or_misc */
			)
{
    static enum { STORE_DONE, STORE_FZKEY, STORE_ABSTR }
			store_state =	STORE_DONE;
    static char		*targ =		NULL;
    static int		targlen =	0;
    static int		abstrsz =	0;
    int			i;

    /* Initialize static variables at first call. */
    if (is_first_misc) {
	abstrsz = usrblk.dblk->dbrec.or_abstrsz;
	if (abstrsz > 0) {
	    /*
	     * if client didn't send back his astrbuf malloc a new
	     * one 
	     */
	    if (usrblk.abstrbuf == NULL) {
		usrblk.abstrbuf = austext_malloc (max_abstrbufsz + 4,
		    PROGNAME "546", NULL);
	    }
	    targ = usrblk.abstrbuf;
	    targlen = abstrsz - 1;	/* leave room for final \0 */
	    store_state = STORE_ABSTR;
	}

	/* If no miscs needed return immediately. */
	else {
	    store_state = STORE_DONE;
	    return;
	}
    }

    /* If NOT first call, but there's nothing left to do because
     * fzkey and abstract already stored, return immediately.
     */
    else if (store_state == STORE_DONE)
	return;

    /*********************
    if (usrblk.debug & USRDBG_RETRVL)
        fprintf (aa_stderr, PROGNAME"562 store_next_misc():"
	    " frst?=%d state=%d, fbuf=%p fsz=%d,\n"
	    "  abuf=%p (bufsz=%d) asz=%d, targ=%p targlen=%d\n",
	    is_first_misc, store_state, usrblk.objfzkey, fzkeysz,
	    usrblk.abstrbuf, usrblk.abstrbufsz, abstrsz,
	    targ, targlen);
    ************************/
  
    /* Main loop is on each byte of the or_misc field of miscrec.
     * Depending on the state, the byte will be a fzkey byte
     * or an abstract byte.
     */
    for (i = 0; i < max_ormisc_size; i++) {
	switch (store_state) {
	    case STORE_ABSTR:
		if (*misc == 0 || --targlen <= 0) {	/* end of abstract? */
		    *targ = 0;
		    store_state = STORE_DONE;
		    return;
		}
		*targ++ = *misc++;
		break;

	    default:
		fprintf (aa_stderr, "%s\n", DtSearchGetMessages ());
		fputs (PROGNAME "549 Abort due to programming error.\n",
		    aa_stderr);
		DtSearchExit (54);
	}	/* end switch */
    }	/* end for-loop */


    /* If storing abstracts, put a \0 at the current targ location to
     * terminate the abstract string in case there are no more misc recs.
     * (but should not occur).
     */
    if ((store_state = STORE_ABSTR))
	*targ = 0;
    return;
}  /* store_next_misc() */


/************************************************/
/*						*/
/*		ve_getrec_dba			*/
/*						*/
/************************************************/
/* Given a valid vista database address, returns the opera record
 * itself, its linked list of compressed text lines (blobs)
 * or NULL if there are none, its abstract and fzkey if any,
 * and user notes, exactly as stored in vista.
 * The objrec, fzkey, abstract, and notes are returned in usrblk.
 * The blob list is returned in the passed ptr arg,
 * or it is set to NULL if there are no blobs.
 * Saves size of uncompressed data (or_objsize) in OE_objsize.
 * CALLER MUST FREE the blob list when done.
 * Returns OE_OK if all goes well, else returns other appropriate retncode.
 * Simpler version that only gets text blobs is ve_getblobs() below.
 */
int             ve_getrec_dba (LLIST ** bloblist)
{
    struct or_objrec
		myobjbuf;
    struct or_blobrec
		myblobuf;
    struct or_miscrec
		mymiscrec;
    int		debugging = (usrblk.debug & USRDBG_RETRVL);
    LLIST	*new, *lastnode, **lastlink;
    int		vistano = usrblk.dblk->vistano;
    int		is_first_misc = TRUE;
    DB_ADDR	dba = usrblk.dba;
    char	msgbuf[512];

    if (debugging)
	fprintf (aa_stderr,
	    PROGNAME"644 retrieve db='%s' dba=%ld (x%x:%lx)\n",
	    usrblk.dblk->name,
	    (long)dba, (int)dba >> 24, (long)dba & 0xffffffL);

    /* Test for invalid dba */
    if (dba == NULL_DBA) {
	DtSearchAddMessage (catgets (dtsearch_catd, MS_ve, 157,
		PROGNAME "245 Client Program Error: "
		"Null database address in usrblk.dba."));
	OE_flags |= OE_PERMERR;
	return OE_ABORT;
    }

    max_ormisc_size = sizeof (mymiscrec.or_misc);

    /* Retrieve the opera header record.  Don't use
     * CRSET macro here so we can trap invalid dba errs.
     */
    d_crset (&dba, vistano);
    if (db_status == S_INVADDR) {
	sprintf (msgbuf, catgets (dtsearch_catd, MS_ve, 142,
		"%s Client Error: Requested record with invalid\n"
		"  database addr %ld (%d:%ld, x'%08.8lx') for database '%s'."),
	    PROGNAME "142 ", dba, dba >> 24, dba & 0xffffff, dba, usrblk.dblk->label);
	fprintf (aa_stderr, "%s: %s\n", aa_argv0, msgbuf);
	DtSearchAddMessage (msgbuf);
	OE_flags |= OE_PERMERR;
	return OE_ABORT;
    }
    RECREAD (PROGNAME "143", &myobjbuf, vistano);
    if (db_status != S_OKAY)
	return OE_NOTAVAIL;
    swab_objrec (&myobjbuf, NTOH);
    OE_objsize = myobjbuf.or_objsize;	/* save tot num bytes
					 * globally */

    if (debugging)
	fprintf (aa_stderr, "  --> got '%s': flags=x%lx sz=%ld dt=%s\n",
	    myobjbuf.or_objkey, (long)myobjbuf.or_objflags,
	    (long)myobjbuf.or_objsize,
	    objdate2fzkstr (myobjbuf.or_objdate));

    clear_usrblk_record ();
    memcpy (&usrblk.objrec, &myobjbuf, sizeof (struct or_objrec));

    /* Setup currency table: curr record is owner of all object's sets */
    SETOR (PROGNAME "145", OR_OBJ_BLOBS, vistano);
    SETOR (PROGNAME "146", OR_OBJ_MISCS, vistano);

    /* Retrieve text blobs or set bloblist to NULL if no blobs */
    if (debugging)
	fputs (PROGNAME "678 Retrieving blobs: ", aa_stderr);
    *bloblist = NULL;
    lastlink = bloblist;
    FINDFM (PROGNAME "148", OR_OBJ_BLOBS, vistano);
    while (db_status == S_OKAY) {
	/* Read next text blob record and append to end of list.
	 * Each debug 'b' = 1 blobrec.
	 */
	if (debugging)
	    fputc ('b', aa_stderr);
	RECREAD (PROGNAME "151", &myblobuf, vistano);
	if (db_status != S_OKAY)
	    vista_abort (PROGNAME "152");
	NTOHS (myblobuf.or_bloblen);
	lastnode = append_blob (lastlink, &myblobuf);
	lastlink = &lastnode->link;
	FINDNM (PROGNAME "155", OR_OBJ_BLOBS, vistano);
    }
    if (debugging) {
	if (*bloblist == NULL)
	    fputs ("--> there are no blobs!\n", aa_stderr);
	else
	    fputc ('\n', aa_stderr);
	fflush (aa_stderr);
    }

    /* Retrieve abstract, fzkey, and user notes, if any */
    if (debugging)
	fputs (PROGNAME "698 Retrieving misc recs: ", aa_stderr);
    lastlink = &usrblk.notes;
    FINDFM (PROGNAME "164", OR_OBJ_MISCS, vistano);
    is_first_misc = TRUE;
    while (db_status == S_OKAY) {
	/*
	 * Read next misc record.  If notes, append to end of notes list.
	 * If abstract or fzkey, move to appropriate usrblk field.
	 * Each debug char ids the type of miscrec.
	 */
	RECREAD (PROGNAME "168", &mymiscrec, vistano);
	if (db_status != S_OKAY)
	    vista_abort (PROGNAME "169");
	NTOHS (mymiscrec.or_misctype);

	switch (mymiscrec.or_misctype) {
	    case ORM_OLDNOTES:
		if (debugging)
		    fputc ('n', aa_stderr);
		new = austext_malloc (sizeof (mymiscrec.or_misc)
			+ sizeof (LLIST) + 4,
		    PROGNAME "543", NULL);
		new->link = NULL;
		/* hop over exactly 1 LLIST structure */
		new->data = new + 1;
		memcpy (new->data, mymiscrec.or_misc,
		    sizeof (mymiscrec.or_misc));
		*lastlink = new;
		lastlink = &new->link;
		break;

	    case ORM_FZKABS:
		/* Concatenated fzkey + abstract rec */
		if (debugging)
		    fputc ('a', aa_stderr);
		store_next_misc (is_first_misc, (char *) mymiscrec.or_misc);
		is_first_misc = FALSE;
		break;

	    default:
		if (debugging)
		    fputc ('?', aa_stderr);
		break;	/* ignore it */
	}	/* end switch */

	FINDNM (PROGNAME "172", OR_OBJ_MISCS, vistano);
    }	/* end loop on miscrecs */

    /* Currently no provision for retrieving hyperlink records */

    if (debugging) {
	if (is_first_misc)
	    fputs ("--> there are no misc recs!\n", aa_stderr);
	else
	    fputc ('\n', aa_stderr);
	print_usrblk_record (PROGNAME"600");
    }
    return OE_OK;
}  /* ve_getrec_dba() */


/************************************************/
/*						*/
/*		  ve_getblobs			*/
/*						*/
/************************************************/
/* Given a valid vista database address for an operarec,
 * returns its linked list of compressed text lines (blobs),
 * exactly as stored in vista, and returns the or_objsize
 * field in the global OE_objsize (for later unblobing).
 * Returns NULL if invalid database addr or no text blobs.
 * CALLER MUST FREE the blob list himself when done.
 * This is a simpler version of ve_getrec_dba() and will
 * get blobs from any database, not just the one in usrblk.dblk.
 */
LLIST          *ve_getblobs (DtSrINT32 dba, int vistano)
{
    LLIST          *bloblist, *lastnode, **lastlink;
    struct or_blobrec myblobuf;
    struct or_objrec myobjbuf;
    int             debugging = (usrblk.debug & USRDBG_RETRVL);

    /* Retrieve the opera header record */
    if (dba == NULL_DBA)
	return NULL;
    CRSET (PROGNAME "401", &dba, vistano);
    RECREAD (PROGNAME "402", &myobjbuf, vistano);
    if (db_status != S_OKAY)
	return NULL;
    swab_objrec (&myobjbuf, NTOH);
    OE_objsize = myobjbuf.or_objsize;
    if (debugging) {
	fprintf (aa_stderr, PROGNAME "792 ve_getblobs: "
	    "db='%s'[v#%d] dba=%ld:%ld v#=%d sz=%ld: '%s'\n",
	    usrblk.dblk->name, usrblk.dblk->vistano,
	    (long) (dba >> 24), (long) (dba % 0xffffff), vistano,
	    (long) myobjbuf.or_objsize, myobjbuf.or_objkey);
    }

    /* Retrieve blobs and append to end of growing list.
     * If no blobs, return NULL.
     */
    bloblist = NULL;
    lastlink = &bloblist;
    SETOR (PROGNAME "406", OR_OBJ_BLOBS, vistano);
    FINDFM (PROGNAME "407", OR_OBJ_BLOBS, vistano);
    while (db_status == S_OKAY) {
	RECREAD (PROGNAME "413", &myblobuf, vistano);
	if (db_status != S_OKAY)
	    vista_abort (PROGNAME "414");
	NTOHS (myblobuf.or_bloblen);
	lastnode = append_blob (lastlink, &myblobuf);
	lastlink = &lastnode->link;
	FINDNM (PROGNAME "417", OR_OBJ_BLOBS, vistano);
    }
    return bloblist;
}  /* ve_getblobs() */


/************************************************/
/*						*/
/*		ve_reckey2dba			*/
/*						*/
/************************************************/
/* Given a vista database record key (in usrblk.query),
 * returns the vista database address of the record.
 * If a record cant be found for the exact key,
 * or if the requested record is a 'reserved' record,
 * wrap to the NEXT available nonreserved record address.
 * By convention, reserved records have an ascii control char
 * (< 32, 0x20) for the keytype char (first char of record key).
 * Also sets usrblk.retncode to OE_OK or OE_WRAPPED.
 */
DtSrINT32         ve_reckey2dba (void)
{
    static char     debugkey[] =
    {0x73, 0x31, 0x67, 0x6E, 0x61, 0x74, 0x75, 0x72, 0x33, 0x00};
    static char     debugmsg[] = {
	0x54, 0x68, 0x69, 0x73, 0x20, 0x49, 0x6E, 0x66,
	0x6F, 0x72, 0x6D, 0x61, 0x74, 0x69, 0x6F, 0x6E,
	0x20, 0x52, 0x65, 0x74, 0x72, 0x69, 0x65, 0x76,
	0x61, 0x6C, 0x20, 0x53, 0x79, 0x73, 0x74, 0x65,
	0x6D, 0x20, 0x77, 0x61, 0x73, 0x20, 0x64, 0x65,
	0x73, 0x69, 0x67, 0x6E, 0x65, 0x64, 0x20, 0x61,
	0x6E, 0x64, 0x20, 0x69, 0x6D, 0x70, 0x6C, 0x65,
	0x6D, 0x65, 0x6E, 0x74, 0x65, 0x64, 0x20, 0x62,
	0x79, 0x0A, 0x4D, 0x69, 0x6B, 0x65, 0x20,
	0x52, 0x75, 0x73, 0x73, 0x65, 0x6C, 0x6C, 0x20,
	0x61, 0x6E, 0x64, 0x20, 0x45, 0x66, 0x69, 0x6D,
	0x20, 0x47, 0x65, 0x6E, 0x64, 0x6C, 0x65, 0x72,
	0x0A, 0x6F, 0x66, 0x20, 0x41, 0x75, 0x73,
	0x74, 0x69, 0x6E, 0x2C, 0x20, 0x54, 0x65, 0x78,
	0x61, 0x73, 0x2C, 0x20, 0x55, 0x53, 0x41, 0x2E,
	0x00};
    DB_ADDR         dba;
    int             null_query = FALSE;
    int             vistano = usrblk.dblk->vistano;
    char            mykeybuf[DtSrMAX_DB_KEYSIZE + 2];

    usrblk.retncode = OE_OK;

    /* If UI sent a null query ptr, reset it to empty string */
    if (usrblk.query == NULL) {
	null_query = TRUE;
	usrblk.query = "";
	DtSearchAddMessage (catgets (dtsearch_catd, MS_ve, 398,
		PROGNAME "398 NULL query string."));
    }
    if (strncmp (usrblk.query, debugkey, strlen (debugkey)) == 0) {
	usrblk.query = "";
	DtSearchAddMessage (debugmsg);
    }

    /* If case insensitive keys is the site standard,
     * force key to uppercase.
     */
    if (OE_uppercase_keys)
	strupr (usrblk.query);

    /* Find the record.  If exact record key isnt found,
     * keep reading until we get one, including wrapping.
     * past end of file.
     */
    KEYFIND (PROGNAME "191", OR_OBJKEY, usrblk.query, vistano);
WRAP_SOME_MORE:
    while (db_status == S_NOTFOUND) {
	usrblk.retncode = OE_WRAPPED;
	KEYNEXT (PROGNAME "196", OR_OBJKEY, vistano);
    }

    /* If the retrieved record is a 'reserved' record, wrap some more */
    KEYREAD (PROGNAME "208", mykeybuf);
    if (db_status != S_OKAY)
	vista_abort (PROGNAME "209");
    if (mykeybuf[0] < 32) {
	KEYNEXT (PROGNAME "210", OR_OBJKEY, vistano);
	goto WRAP_SOME_MORE;
    }

    CRGET (PROGNAME "211", &dba, vistano);
    if (null_query)
	usrblk.query = NULL;	/* restore */
    return dba;
}  /* ve_reckey2dba() */


/************************************************/
/*						*/
/*		ve_browse_dba			*/
/*						*/
/************************************************/
/* Increments/decrements dba address field.
 * If original dba is null, returns first dba in database.
 * Otherwise UI must ensure that original dba is valid for current database.
 * Does not alter other record oriented usrblk fields.
 */
void            ve_browse_dba (int direction)
{
    int             vistano = usrblk.dblk->vistano;
    char            mykeybuf[DtSrMAX_DB_KEYSIZE + 2];

    usrblk.retncode = OE_OK;

TRY_AGAIN:
    if (usrblk.dba == NULL_DBA) {
	KEYFRST (PROGNAME "224", OR_OBJKEY, vistano);
	if (db_status != S_OKAY)
	    vista_abort (PROGNAME "226");
    }
    else {
	/* at this point, dba must be valid for current database */
	CRSET (PROGNAME "230", &usrblk.dba, vistano);
	CRREAD (PROGNAME "232", OR_OBJKEY, mykeybuf, vistano);
	if (db_status != S_OKAY)
	    vista_abort (PROGNAME "234");

	/* descend b-tree to current location */
	KEYFIND (PROGNAME "236", OR_OBJKEY, mykeybuf, vistano);
	if (direction < 0) {	/* get prev rec */
	    KEYPREV (PROGNAME "238", OR_OBJKEY, vistano);
	    if (db_status == S_NOTFOUND) {	/* at begin of file,
						 * wrap */
		usrblk.retncode = OE_WRAPPED;
		KEYPREV (PROGNAME "240", OR_OBJKEY, vistano);
	    }
	}
	else {	/* get next rec */
	    KEYNEXT (PROGNAME "242", OR_OBJKEY, vistano);
	    if (db_status == S_NOTFOUND) {	/* at eof, wrap */
		usrblk.retncode = OE_WRAPPED;
		KEYNEXT (PROGNAME "244", OR_OBJKEY, vistano);
	    }
	}
    }	/* end else where orig rec is not null */

    CRGET (PROGNAME "246", &usrblk.dba, vistano);

    /* If retrieved rec is a reserved record,
     * ignore it and retry the browse from here.
     */
    KEYREAD (PROGNAME "561", mykeybuf);
    if (db_status != S_OKAY)
	vista_abort (PROGNAME "562");
    if (mykeybuf[0] < 32)
	goto TRY_AGAIN;
    return;
}  /* ve_browse_dba() */

/**************************** DTSRVE.C ******************************/
