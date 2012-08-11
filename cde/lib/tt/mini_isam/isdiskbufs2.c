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
/*%%  $XConsortium: isdiskbufs2.c /main/3 1995/10/23 11:37:47 rswiston $ 			 				 */
#ifndef lint
static char sccsid[] = "@(#)isdiskbufs2.c 1.3 89/07/17 Copyr 1988 Sun Micro";
#endif

/*
 * Copyright (c) 1988 by Sun Microsystems, Inc.
 */

/*
 * _isdiskbufs.c
 *
 * Description:
 *	ISAM disk buffer managament
 *
 */

/************************ NON MAPPED I/O version ***************************/

#include "isam_impl.h"

extern struct dlink *_isdln_next(), *_isdln_first();

#define ISMAXBUFFERS	200		     /* Use 20 buffers */
#define ISHASHHDRS	256		     /* Must be a power of two */
#define ISHASHMASK      (ISHASHHDRS-1)

#define __hashblkno(fcb,blkno) (((size_t)(fcb)+(blkno)) & ISHASHMASK)


#define base ((char *)0)
#define _isdln_insert(l,e) _isdln_base_insert(base,(l),(e))
#define _isdln_append(l,e)  _isdln_base_append(base,(l),(e))
#define _isdln_remove(e)  _isdln_base_remove(base,(e))
#define _isdln_first(l)  _isdln_base_first(base,(l))
#define _isdln_next(l)  _isdln_base_next(base,(l))
#define _isdln_prev(l)  _isdln_base_prev(base,(l))
#define _isdln_makeempty(l)  _isdln_base_makeempty(base,(l))
#define _isdln_isempty(l)  _isdln_base_isempty(base,(l))

/*---------------------- Local data ---------------------------------------*/
static Bufhdr *_getavail(), *_findblock();
static void _disk_init(), _commit1buffer(), _rollback1buffer(), _flush1buffer();
static void _makenodata();

Bufhdr bufhdrs [ISMAXBUFFERS];
struct dlink  hashhdrs [ISHASHHDRS];	     /* Heads of hashed lists */

struct dlink  availlist;		     /* Available buffer list */
struct dlink  *pavail = &availlist;

struct dlink  changelist;		     /* Change buffer list */
struct dlink  *pchangl = &changelist;

struct dlink  fixlist; 			     /* Fixed buffer list */
struct dlink  *pfixl = &fixlist;

static int    availn;			     /* Number of available buffers */
static int    minavailn;		     /* Minimum available buffers */
static int    maxavailn;		     /* Stop flushing when 
					      * when maxavailn buffers
					      * are available */


#define ISB_FIXED (ISB_RFIXED | ISB_WFIXED)
#define MINAVAILN	40		     /* in procent of total # buffers */
#define MAXAVAILN	60		     /* in procent of total # buffers */



Bufhdr *
_isdisk_fix(fcb, unixfd, blkno, mode)
    Fcb		*fcb;
    int		unixfd;			     /* .rec, .ind., .var */
    Blkno	blkno;
    int		mode;
{
    register Bufhdr *p, *p2;
    struct dlink	   *hashl;

    /*
     * Initialize some local data.
     */
    _disk_init();	

    if (fcb->datfd == unixfd)
	assert(blkno != 0);		     /* Never access control page */

    hashl = (hashhdrs +__hashblkno(fcb,blkno));
    
    /* Try to find the page in buffer pool. */
    if ((p = _findblock(fcb, unixfd, blkno)) == NULL) {
	
	/* Page is not in the pool - install it. */
	if (mode != ISFIXNOREAD) {
	    p = _getavail();		     /* Get free page from pool */
	    _isdln_insert(hashl,&p->isb_hash); /* Insert into hash list */
	    
	    _isseekpg(unixfd, blkno);
	    _isreadpg(unixfd, p->isb_buffer);

	    p->isb_flags = ISB_READ;
	    p->isb_oldcopy = NULL;
	    p->isb_fcb = fcb;
	    p->isb_unixfd  = unixfd;
	    p->isb_blkno = blkno;
	}
	else
	    p = NULL;
    }	
    
    if (p && (p->isb_flags & ISB_FIXED)==0) {
	
	/* Remove buffer from pavail (or pchangl) list. */
	_isdln_remove(&p->isb_aclist);


	if (!(p->isb_flags & ISB_CHANGE))
	    availn--;

    }
    
    if (mode == ISFIXREAD) {
	assert(p);
	
	if(!(p->isb_flags & ISB_FIXED)) {  
	    
	    /* Add buffer to pfixl list. */
	    _isdln_append(pfixl,&p->isb_flist);
	    p->isb_flags |= ISB_RFIXED;
	    
	}
	return (p);
    }	/* if (p) */
    else {
	
	/* If buffer is already fixed for write, no other actions are necces. */
	if (p && p->isb_flags & ISB_FIXED) {
	    assert((p->isb_flags & ISB_RFIXED) == 0); /* Buffer cannot be */
	    /* fixed for read when is fixed */
	    /* being fixed for write */
	    return (p);
	}
	
	/* Create shadow page */
	p2 = _getavail();		     /* Get free page from pool */
	availn--;
	_isdln_remove(&p2->isb_aclist);	     /* Remove from pavail */
	_isdln_insert(hashl,&p2->isb_hash);  /* Insert into hash list */
	_isdln_insert(pfixl,&p2->isb_flist); /* Insert into pfixl list */
	
	p2->isb_fcb = fcb;
	p2->isb_unixfd = unixfd;
	p2->isb_blkno = blkno;
	p2->isb_flags = ISB_READ|ISB_WFIXED; /* Mark buffer as dirty */
	
	if (mode == ISFIXWRITE)		     /* Copy buffer content */
	    memcpy(p2->isb_buffer,p->isb_buffer,ISPAGESIZE);
	
	p2->isb_oldcopy = p;
	
	/* Make old copy */
	if (p) {
	    assert ((p->isb_flags & ISB_FIXED) == 0);
	    assert((p->isb_flags & ISB_OLDCOPY) == 0);
	    
	    p->isb_flags |= ISB_OLDCOPY;
	    _isdln_remove(&p->isb_hash);
	}
	
	return (p2);
    }
}

void
_isdisk_unfix (p)
    register Bufhdr	*p;
{
    if (!(p->isb_flags & ISB_FIXED))
    assert(p->isb_flags & ISB_FIXED);
    
    if (p->isb_flags & ISB_WFIXED)	     /* Unfix at commit/abort time */
	return;
    
    p->isb_flags &= ~ISB_FIXED;		     /* Clear bit */
    _isdln_remove(&p->isb_flist);	     /* Remove from pfixl */
    
    /* Append to pavail or pchangl list. */
    if (p->isb_flags & ISB_CHANGE)
	_isdln_append(pchangl,&p->isb_aclist); /* Append to pchangl list */
    else {
	_isdln_append(pavail,&p->isb_aclist); /* Append to pavail list */
	availn++;
    }
}

void
_isdisk_commit1 (p)
    Bufhdr	*p;
{
    _commit1buffer(p);
}

void
_isdisk_commit()
{
    register Bufhdr *p;
    struct dlink	   *e;
    
    while ((e = _isdln_first(pfixl)) != pfixl) {
	p = GETBASE(e,bufhdr,isb_flist);  /* Get pointer to bufhdr */
	assert(p->isb_flags & ISB_WFIXED);
	_commit1buffer(p);
    }
}

void
_isdisk_rollback()
{
    register Bufhdr 	*p;
    struct dlink	   *e;
    
    while ((e = _isdln_first(pfixl)) != pfixl) {
	p = GETBASE(e,bufhdr,isb_flist);  /* Get pointer to bufhdr */
	assert(p->isb_flags & ISB_FIXED);
	if (p->isb_flags & ISB_WFIXED)
	    _rollback1buffer(p);
	else 
	    _isdisk_unfix(p);
    }
}

Bufhdr *
_isdisk_refix(p, newmode)
    Bufhdr	*p;
    int			newmode;
{
    Blkno	blkno = p->isb_blkno;
    Fcb		*fcb = p->isb_fcb;
    int		unixfd = p->isb_unixfd;
    
    assert(newmode == ISFIXWRITE);
    
    if (p->isb_flags & ISB_RFIXED) {	
	_isdisk_unfix(p);
	return (_isdisk_fix(fcb, unixfd, blkno, ISFIXWRITE));
    }
    else
	return (p);
}

void
_isdisk_sync()
{
    extern time_t _istimeget();
    register Bufhdr *p;
    struct dlink	   *e;
    
    while ((e = _isdln_first(pchangl)) != pchangl) {
	p = GETBASE(e,bufhdr,isb_aclist);  /* Get pointer to bufhdr */
	assert(p->isb_flags & ISB_CHANGE);
	assert((p->isb_flags & ISB_FIXED)==0);
	_flush1buffer(p);
    }
}

void
_isdisk_inval()
{
    extern time_t _istimeget();
    register Bufhdr *p;
    struct dlink	   *e;
    
    /* ensure pavail is initialized before using it */

    if (pavail->dln_forward == 0) {
	    _isdln_makeempty(pavail);
    }

    e = pavail;

    while ((e = _isdln_prev(e)) != pavail) {
	p = GETBASE(e,bufhdr,isb_aclist);  /* Get pointer to bufhdr */
    
	if ((p->isb_flags & ISB_READ) == 0)  
	    break;

	_isdln_remove(&p->isb_hash);
	p->isb_flags = ISB_NODATA;	     /* Mark as no data in the buffer */
    }
}


#if ISDEBUG
_isdisk_dumphd()
{
    register Bufhdr *p;
    int			    i;
    
    (void)printf("\nInd isfd   blkno mode temp oldcopy\n");
    for (p = bufhdrs, i = 0; i < ISMAXBUFFERS; p++,i++)
	if (p->isb_flags != ISB_NODATA)
	    (void) printf("%3d: %3d  %6d   %2x     %3d\n",i,
			  _isfd_getisfd(p->isb_pisfd),
			  p->isb_blkno,p->isb_flags,
			  p->isb_oldcopy?(p->isb_oldcopy - bufhdrs):-1);
}

aclistdump(lh)
    struct dlink	   *lh;
{
    register Bufhdr *p;
    struct dlink	   *e;
    
    for (e = _isdln_first(lh); e != lh; e = _isdln_next(e)) {
	p = GETBASE(e,bufhdr,isb_aclist);  /* Get pointer to bufhdr */
	(void) printf("%3d: %3d  %6d   %2x     %3d\n",p-bufhdrs,
		      _isfd_getisfd(p->isb_pisfd),
		      p->isb_blkno,p->isb_flags,
		      p->isb_oldcopy?(p->isb_oldcopy - bufhdrs):-1);
    }
}

flistdump(lh)
    struct dlink	   *lh;
{
    register Bufhdr *p;
    struct dlink	   *e;
    
    for (e = _isdln_first(lh); e != lh; e = _isdln_next(e)) {
	p = GETBASE(e,bufhdr,isb_flist);  /* Get pointer to bufhdr */
	(void) printf("%3d: %3d  %6d   %2x     %3d\n",p-bufhdrs,
		      _isfd_getisfd(p->isb_pisfd),
		      p->isb_blkno,p->isb_flags,
		      p->isb_oldcopy?(p->isb_oldcopy - bufhdrs):-1);
    }
}

#endif


/*------------------------ Local functions ---------------------------------*/

Static void
_disk_init()
{
    static Bool  initialized = FALSE;
    register int	i;
    
    if (initialized == TRUE)
	return;

    initialized = TRUE;
    
    /* Initialize hash queue list heads. */
    for (i = 0; i < ISHASHHDRS; i++) {
	_isdln_makeempty(hashhdrs+i);
    }

    /* initialize pavail, pchangel, and pfixl lists to empty. */

    _isdln_makeempty(pavail);
    _isdln_makeempty(pchangl);
    _isdln_makeempty(pfixl);
    
    /* Link all buffers into pavail list. */
    for (i = 0; i < ISMAXBUFFERS; i++) {
	bufhdrs[i].isb_buffer = _ismalloc(ISPAGESIZE);
	_isdln_append(pavail,&bufhdrs[i].isb_aclist);
	availn++;
    }
    
    /* Set maxavailn and minavailn. */
    minavailn = (ISMAXBUFFERS * MINAVAILN) / 100;
    maxavailn = (ISMAXBUFFERS * MAXAVAILN) / 100;
}

/* _getavail() - get available buffer in disk */
Static Bufhdr *
_getavail()
{
    register Bufhdr *p;
    register struct dlink  *q;
    
    if ((q = _isdln_first(pavail)) == pavail) {
	_isfatal_error("No buffer in pool available");
    }
    
    p = GETBASE(q,bufhdr,isb_aclist);
    
    if (p->isb_flags & ISB_READ) {	     /* Remove from hash queue */
	_isdln_remove(&p->isb_hash);
	p->isb_flags = ISB_NODATA;	     /* Mark as no data in the buffer */
    }
    
    return ((Bufhdr *) p);
}

/* _findblock() - Find block in buffer pool */
Static Bufhdr *
_findblock(fcb, unixfd, blkno)
    Fcb		*fcb;
    int		unixfd;
    Blkno	blkno;
{
    register Bufhdr *p;
    struct dlink	   *lh, *e;
    int			   hashval;
    
    hashval = __hashblkno(fcb,blkno);
    
    lh = hashhdrs + hashval;		     /* lh is list head */
    for (e = _isdln_first(lh); e != lh; e = _isdln_next(e)) {
	p = GETBASE(e,bufhdr,isb_hash);  /* Get pointer to bufhdr */
	if (p->isb_blkno == blkno && p->isb_fcb == fcb && p->isb_unixfd == unixfd) {
	    assert(p->isb_flags != ISB_NODATA); 
	    return(p);
	}
    }
    
    return (NULL);
}

/* _commit1buffer() - Commit changes to buffer */
Static void
_commit1buffer(p)
    register Bufhdr	*p;
{
    assert(p->isb_flags & ISB_WFIXED);	     /* Fixed for read buffers should */
    /* go through _isdisk_unfix() */
    
    /* Free old permanent buffer if any exists. */
    if (p->isb_oldcopy) {
	_makenodata(p->isb_oldcopy);	     /* Make this buffer available */
    }
    
    /* Remove buffer from list of fixed buffers. */
    /* Append buffer to list of changed buffers. */
    _isdln_remove(&p->isb_flist);
    _isdln_append(pchangl,&p->isb_aclist);
    p->isb_flags &= ~ISB_FIXED;
    p->isb_flags |= ISB_CHANGE;
}

/* _rollback1buffer() - Rollback changes to buffer */
Static void
_rollback1buffer(p)
    register Bufhdr	*p;
{
    register Bufhdr	*p2;
    
    assert(p->isb_flags & ISB_WFIXED);	     /* Fixed for read buffers should */
    /* go through _isdisk_unfix() */
    
    /* Re-install old copy if that exists. */
    if ((p2 = p->isb_oldcopy) != NULL) {
	if (p2->isb_flags & ISB_CHANGE) {
	    _isdln_append(pchangl,&p2->isb_aclist);
	}
	else {
	    _isdln_append(pavail,&p2->isb_aclist);
	    availn++;
	}
	p2->isb_flags &= ~ISB_OLDCOPY;	     /* Clear bit */
	
	/* See implementation of _isdln_append() that this will work. */
	_isdln_append(&p->isb_hash,&p2->isb_hash); /* Insert into hash list */
    }	
    
    
    _isdln_remove(&p->isb_hash);	     /* Remove bufer from hash list */
    _isdln_remove(&p->isb_flist);	     /* Remove bufer from pfixl */
    _makenodata(p);			     /* Make this buffer available */
}

/* _makenodata() - make buffer available with no data in it*/
Static void
_makenodata(p)
    register Bufhdr	*p;
{
    assert(p->isb_flags & ISB_READ);
    
    p->isb_flags = ISB_NODATA;
    _isdln_insert(pavail,&p->isb_aclist);
    availn++;
}

/* _flush1buffer() - flush buffer to disk */
Static void
_flush1buffer(p)
    register Bufhdr	*p;
{
    assert(p->isb_flags & ISB_CHANGE);
    
    _isseekpg(p->isb_unixfd, p->isb_blkno);
    _iswritepg(p->isb_unixfd, p->isb_buffer);
    
    p->isb_flags &= ~ISB_CHANGE;	     /* clear change flag */
    
    _isdln_remove(&p->isb_aclist);	     /* Remove from pchangl */
    _isdln_append(pavail,&p->isb_aclist);    /* Append to pavail */
    availn++;
}
