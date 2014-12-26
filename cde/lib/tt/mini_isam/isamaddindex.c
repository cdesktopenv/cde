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
/*%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			 */
/*%%  (c) Copyright 1993, 1994 International Business Machines Corp.	 */
/*%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			 */
/*%%  (c) Copyright 1993, 1994 Novell, Inc. 				 */
/*%%  $XConsortium: isamaddindex.c /main/3 1995/10/23 11:34:12 rswiston $ 			 				 */
#ifndef lint
static char sccsid[] = "@(#)isamaddindex.c 1.14 89/09/14 Copyr 1988 Sun Micro";
#endif
/*
 * Copyright (c) 1988 by Sun Microsystems, Inc.
 */

/*
 * isaddindex.c
 *
 * Description: 
 *	Add secondary index
 *	
 *
 */

#include "isam_impl.h"
#include <sys/types.h>
#include <sys/stat.h>

extern int _iskeycmp();

static void _readallrecords(), _attach_dups_serial();
static Blkno _buildbtree();
static int _duplicate_exist();
static void checkavailfd(void);

/*
 * _amaddindex(isfhandle, keydesc,  errcode)
 *
 * _amaddindex() build a secondary index
 *
 * Input params:
 *	isfhandle	Handle of ISAM file
 *	keydesc		Key descriptor
 *
 * Output params:
 *	errcode		error status of the operation
 *
 */

/*ARGSUSED*/
int
_amaddindex(isfhandle, keydesc, errcode)
    Bytearray		*isfhandle;
    struct keydesc	*keydesc;
    struct errcode	*errcode;
{
    Fcb			*fcb = NULL;
    Keydesc2		keydesc2;
    int			err;


    _isam_entryhook();
    
    /*
     * Check if 1 UNIX file decriptor is available.
     */
    checkavailfd();

    /*
     * Get FCB corresponding to the isfhandle handle.
     */
    if ((fcb = _openfcb(isfhandle, errcode)) == NULL) {
	_isam_exithook();
	return (ISERROR);
    }

    /*
     * Check that the limit on the number of keys is not exceeded.
     */
    if (fcb->nkeys >= MAXNKEYS) {
	_amseterrcode(errcode, ETOOMANY);
	goto ERROR;
    }

    /*
     * Update information in FCB from CNTL page on the disk
     */
    (void)_isfcb_cntlpg_r(fcb);

    /*
     * Check key descriptor for validity.
     */
    if (_validate_keydesc(keydesc, fcb->minreclen) != ISOK) {
	_amseterrcode(errcode, EBADKEY);
	goto ERROR;
    }

    /*
     * Convert key descriptor to internal form.
     */
    _iskey_xtoi (&keydesc2, keydesc);

    /* Check if key already exists. */
    if (_isfcb_findkey(fcb ,&keydesc2) != (Keydesc2 *) 0) {
	_amseterrcode(errcode, EKEXISTS);
	goto ERROR;
    }

    /*
     * Open (or even create) .ind file if that is not open (created) yet.
     */
    if (_open2_indfile(fcb) != ISOK) {
	_amseterrcode(errcode, EACCES);
	goto ERROR;
    }
	
    /*
     * Create index structure.
     */
    if ((err = _create_index(fcb ,&keydesc2)) != ISOK) { 
	_amseterrcode(errcode, err);
	goto ERROR;
    }

    /*
     * Add key descriptor to FCB.
     */
    if (_isfcb_altkeyadd(fcb, &keydesc2) == ISERROR) {	   
	_amseterrcode(errcode, ETOOMANY);
	goto ERROR;
    }

    _amseterrcode(errcode, ISOK);

    _issignals_mask();
    _isdisk_commit();
    _isdisk_sync();
    _isdisk_inval();

    /*
     * Update CNTL Page from the FCB.
     */
    (void)_isfcb_cntlpg_w(fcb);
    _issignals_unmask();

    _isam_exithook();
    return (ISOK);

 ERROR:

    _isdisk_rollback();
    _isdisk_inval();

    /*
     * Restore FCB from CNTL page.
     */
    if (fcb) (void)_isfcb_cntlpg_r(fcb);

    _isam_exithook();
    return (ISERROR);
}

/*
 * _amaddprimary(isfhandle, keydesc, errcode)
 *
 * _amaddprimary() build primary key
 *
 * Input params:
 *	isfhandle	Handle of ISAM file
 *	keydesc		Key descriptor
 *
 * Output params:
 *	errcode		error status of the operation
 *
 */

/*ARGSUSED*/
int
_amaddprimary(isfhandle, keydesc, errcode)
    Bytearray		*isfhandle;
    struct keydesc	*keydesc;
    struct errcode	*errcode;
{
    Fcb			*fcb = NULL;
    Keydesc2		keydesc2;
    int			err;

    _isam_entryhook(); 
   
    /*
     * Check if 1 UNIX file decriptor is available.
     */
    checkavailfd();

    /*
     * Get FCB corresponding to the isfhandle handle.
     */
    if ((fcb = _openfcb(isfhandle, errcode)) == NULL) {
	_isam_exithook();
	return (ISERROR);
    }

    /*
     * Check that the limit on the numbrer of keys is not exceeded.
     */
    if (fcb->nkeys >= MAXNKEYS) {
	_amseterrcode(errcode, ETOOMANY);
	goto ERROR;
    }

    /*
     * Update information in FCB from CNTL page on the disk
     */
    (void)_isfcb_cntlpg_r(fcb);

    /*
     * Check key descriptor for validity.
     */
    if (_validate_keydesc(keydesc, fcb->minreclen) != ISOK) {
	_amseterrcode(errcode, EBADKEY);
	goto ERROR;
    }

    /*
     * Convert key descriptor to internal form.
     */
    _iskey_xtoi (&keydesc2, keydesc);

    /* Check if key already exists. */
    if (_isfcb_findkey(fcb ,&keydesc2) != (Keydesc2 *) 0) {
	_amseterrcode(errcode, EKEXISTS);
	goto ERROR;
    }

    /*
     * Check that primary key does not already exist.
     */
    if (!FCB_NOPRIMARY_KEY(fcb)) {
	_amseterrcode(errcode, EKEXISTS);
	goto ERROR;
    }

    /*
     * Open (or even create) .ind file if that is not open (created) yet.
     */
    if (_open2_indfile(fcb) != ISOK) {
	_amseterrcode(errcode, EACCES);
	goto ERROR;
    }
	
    /*
     * Create index structure.
     */
    if ((err = _create_index(fcb ,&keydesc2)) != ISOK) { 
	_amseterrcode(errcode, err);
	goto ERROR;
    }

    /*
     * Add key descriptor to FCB.
     */
    if (_isfcb_primkeyadd(fcb, &keydesc2) == ISERROR) {	   
	_amseterrcode(errcode, ETOOMANY);
	goto ERROR;
    }

    _amseterrcode(errcode, ISOK);

    _issignals_mask();
    _isdisk_commit();
    _isdisk_sync();
    _isdisk_inval();

    /*
     * Update CNTL Page from the FCB.
     */
    (void)_isfcb_cntlpg_w(fcb);
    _issignals_unmask();

    _isam_exithook();
    return (ISOK);

 ERROR:
    _isdisk_rollback();
    _isdisk_inval();

    /*
     * Restore FCB from CNTL page.
     */
    if (fcb) (void)_isfcb_cntlpg_r(fcb);

    _isam_exithook();
    return (ISERROR);
}

/*
 * _create_index()
 *
 * Read data file, extract key from record, sort keys, create index.
 * Check unique constraint, create duplicate serial numbers.
 */

int _create_index(fcb, pkeydesc2)
    Fcb			*fcb;
    Keydesc2		*pkeydesc2;
{
    Issort		*srt;
    int			keylength = pkeydesc2->k2_len;

    /* 
     * Set comparison function for sorting. 
     * 
     * nparts + 1 is used to compare keys that allow duplicates.
     * The (nparts + 1) comparison will never be reached on ISNODUPS key.
     */
    _iskeycmp_set(pkeydesc2, pkeydesc2->k2_nparts+1); 

    /* 
     * Handle empty file as a special case, to avoid nasty behavior 
     * in buildbtree() arithmetics. 
     */
    if (fcb->nrecords == 0L) {
	pkeydesc2->k2_rootnode = _buildbtree(fcb, pkeydesc2, (Issort *) NULL);
	return (ISOK);
    }
	

    /* 
     * Create a sorter for this key descriptor. 
     */
    srt = _issort_create(keylength, (int)fcb->nrecords, _iskeycmp);

    /* 
     * Read sequentially all records, extract keys, and
     * insert the keys into the sorter. 
     */
    _readallrecords(fcb, srt, pkeydesc2);

    _issort_sort(srt);			     /* Sort the keys */

    /*
     * Check for potential duplicates if the index is ISNODUPS.
     */
    if (!ALLOWS_DUPS2(pkeydesc2)) {
	if (_duplicate_exist(srt, keylength)) {
	    _issort_destroy(srt);	    
	    return (EDUPL);
	}
    }

    /*
     * Attach duplicate serial numbers to the keys that are ISDUPS.
     */
    if (ALLOWS_DUPS2(pkeydesc2)) {
	_attach_dups_serial(srt, pkeydesc2);
    }

    /* 
     * Allocate and build the B-tree 
     */
    pkeydesc2->k2_rootnode = _buildbtree(fcb, pkeydesc2, srt);

    _issort_destroy(srt);		     /* Destroy sorter */

    return (ISOK);
}

/*
 * _readallrecords()
 *
 * REad all records, extract keys, and insert them into sorter.
 */

Static void
_readallrecords(fcb, srt, pkeydesc2)
    Fcb			*fcb;	
    Issort		*srt;
    Keydesc2		*pkeydesc2;
{
	char		record [ISMAXRECLEN];
	char		keybuf [MAXKEYSIZE];
	Recno		recnum;
	int		reclen = 0;
	int		(*rec_read)() = (fcb->varflag?_vlrec_read:_flrec_read);
	
	for (recnum = 1; recnum <= fcb->lastrecno; recnum++) {
	
		if (rec_read(fcb, record, recnum, &reclen) != ISOK)
			continue;	     /* Skip deleted record */
		
		/*
		 * Zero out the entire key buffer to allow for using 
		 * memcmp() as comparison function to compare whole keys.
		 */
		memset(keybuf, 0, pkeydesc2->k2_len);
		
		/*
		 * Extract key parts from record buffer.
		 */
		_iskey_extract(pkeydesc2, record, keybuf); 
		
		/* 
		 *  Add recno to key 
		 */
		strecno(recnum, keybuf + KEY_RECNO_OFF); 
		
		_issort_insert(srt, keybuf); /* Insert key into sorter */
		
		
	}
}

/*
 * _attach_dups_serial()
 *
 * Attach serial numbers to duplicate keys
 */

Static void
_attach_dups_serial(srt, pkeydesc2)
    Issort		*srt;
    Keydesc2		*pkeydesc2;
{
    int			netkeylength = pkeydesc2->k2_len - RECNOSIZE - DUPIDSIZE;
    char		*curkey;
    char		*lastkey = NULL;
    int			dup_serial = 1;

    _issort_rewind(srt);

    while (curkey = _issort_read(srt)) {
	if (lastkey && memcmp(lastkey + RECNOSIZE + DUPIDSIZE,
			      curkey + RECNOSIZE + DUPIDSIZE,
			       netkeylength) == 0)
	    dup_serial++;
	else
	    dup_serial = 1;

	/*
	 * Store serial number in the key.
	 */
	stdupser(dup_serial, curkey + KEY_DUPS_OFF);
	
	lastkey = curkey;
    }
}

/* 
 * _buildbtree() 
 *
 * Create B-tree. 
 */

Static Blkno 
_buildbtree(fcb, pkeydesc2, srt)
    Fcb			*fcb;
    Keydesc2		*pkeydesc2;
    Issort		*srt;
{
    Bufhdr		*_allockpage();
    int			depth;
    int 		nrecords = fcb->nrecords;
    int			keyspernode[ISMAXBTRLEVEL];
    int			one_more[ISMAXBTRLEVEL];
    char		*nodebuf[ISMAXBTRLEVEL];
    Bufhdr	 	*nodebufhd[ISMAXBTRLEVEL];
    Blkno		pageblkno[ISMAXBTRLEVEL];
    int			curindex[ISMAXBTRLEVEL];
    char		*keyp;
    int			keylength = pkeydesc2->k2_len;
    int 		leafcapac = getkeysperleaf(keylength);
    int			nleafcapac = getkeyspernode(keylength);
    int		       	perleaf;
    int			blocks_in_this_level, blocks_in_prev_level;
    int			slack = ISLEAFSLACK; 
    int			i;
					     
    /* 
     * Handle an empty B-tree as a special case.
     */
    if (fcb->nrecords == 0) {
	(void)_allockpage(fcb, leafcapac, 0, pageblkno + 0);
	return (pageblkno[0]);
    }

    /*
     * COMPRESS changes the fill factor to 100%
     */
    if ((pkeydesc2->k2_flags & COMPRESS) == COMPRESS)
	slack = 0;

    /* 
     * Figure out fill factors for each level. 
     */
    perleaf = ((double)leafcapac * (100.0 - (double) slack)) / 100.0;

    /* 
     * Make it more robust.
     */
    if (perleaf >= leafcapac)
	perleaf = leafcapac;

    if (perleaf < leafcapac/2 + 1)
	perleaf = leafcapac/2 + 1;

    /* 
     * Iterativelly determince values in keyspernode[] and one_mode[] 
     */
    blocks_in_this_level = nrecords / perleaf;
    if (blocks_in_this_level * leafcapac < nrecords)
	blocks_in_this_level++;

    keyspernode[0] = nrecords / blocks_in_this_level;
    one_more[0] = nrecords % blocks_in_this_level;

    
    for (depth = 1; blocks_in_this_level > 1; depth++) {
	blocks_in_prev_level = blocks_in_this_level;

	blocks_in_this_level = (blocks_in_prev_level-1) / nleafcapac + 1;
	keyspernode[depth] = blocks_in_prev_level / blocks_in_this_level;
	one_more[depth] = blocks_in_prev_level % blocks_in_this_level;
    }

    if (depth >= ISMAXBTRLEVEL)
	_isfatal_error("Too many levels in B-tree");

    /*
     * Make sure that we start reading keys from the beginning.
     */
    _issort_rewind(srt);

    /* 
     * Boot the Main loop. 
     */
    for (i = 0; i < depth ; i++) {
	curindex[i] = ISPAGESIZE;	     /* Any big number will do */
	one_more[i]++;
	nodebuf[i] = NULL;
	nodebufhd[i] = NULL;
    }

    /*
     * Main loop.
     */
    while ((keyp = _issort_read(srt)) != (char *) NULL) {

	if (curindex[0] >= keyspernode[0] + (one_more[0] > 0)) {

	    /* 
	     * Commit all changed buffers here to avoid buffer pool overflow.
	     */
	    if (nodebufhd[0]) {
		_isdisk_commit1(nodebufhd[0]);
		_isdisk_sync();
	    }

	    /* Allocate new leaf. */
	    nodebufhd[0] = _allockpage(fcb, leafcapac, 0, pageblkno+0);
	    nodebuf[0] = nodebufhd[0]->isb_buffer;
	    one_more[0]--;
	    curindex[0] = 0;
	}

	/* Copy key into the page. */
	memcpy( nodebuf[0] + BT_KEYS_OFF + keylength * curindex[0],keyp, 
	      keylength);
	stshort((short) curindex[0] + 1, nodebuf[0] + BT_NKEYS_OFF);

	if (curindex[0]++ == 0) {	     /* Store first key in  */
					     /* higher levels */
					     
	    for (i = 1;i < depth; i++) {
		if (curindex[i] >= keyspernode[i] + (one_more[i] > 0)) {

		    /* Unfix buffer. */
		    if (nodebufhd[i]) 
			_isdisk_commit1(nodebufhd[i]);

		    nodebufhd[i] = _allockpage(fcb, nleafcapac, i,  pageblkno+i);
		    nodebuf[i] = nodebufhd[i]->isb_buffer;
		    one_more[i]--;
		    curindex[i] = 0;
		}
		
		/* Copy key into page. */
		memcpy( nodebuf[i] + BT_KEYS_OFF + keylength * curindex[i],keyp,
		       keylength);

		/* Store pointer to level below. */
		stblkno(pageblkno[i-1], nodebuf[i] + ISPAGESIZE - 
			(curindex[i]+1) * BLKNOSIZE);

		stshort((short) curindex[i] + 1, nodebuf[i] + BT_NKEYS_OFF);
		if (curindex[i]++ > 0) {
		    break;		     
		}
	    }
	}
	
    }

    return (pageblkno [depth - 1]);	     /* Return blkno of B-tree root */
}

/* 
 * _duplicate_exist()
 * 
 * Return 1 if there are duplicates in the sorted key object. 
 */

Static int _duplicate_exist(srt, keylength)
    Issort	*srt;
    int		keylength;
{
    char	*curkey;
    char	*lastkey = (char *) 0;
    int		netkeylength = keylength - RECNOSIZE;

    _issort_rewind(srt);

    while (curkey = _issort_read(srt)) {
	    if (lastkey && memcmp(lastkey + RECNOSIZE, curkey + RECNOSIZE,
				  netkeylength) == 0) 
		    return 1;		     /* Duplicate key found */

	    lastkey = curkey;
    }
    return 0;				     /* No duplicates found */
}



static void
checkavailfd(void)
{
    Fcb			*fcb;
    Bytearray		*isfhandle2;

    /*
     * Check that there are UNIX file descriptors available.	
     */
    while (_watchfd_check() < 1) {
	/*
	 * Find victim (LRU FCB) and close it.
	 */
	if((isfhandle2 = _mngfcb_victim()) == NULL)
	    _isfatal_error ("_openfcb() cannot find LRU victim");

	fcb = _mngfcb_find(isfhandle2);
	(void) _watchfd_decr(_isfcb_nfds(fcb));
	_isfcb_close(fcb);
	_mngfcb_delete(isfhandle2);
    }
}
