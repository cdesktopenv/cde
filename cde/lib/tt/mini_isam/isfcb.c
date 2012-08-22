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
/*%%  $XConsortium: isfcb.c /main/3 1995/10/23 11:38:34 rswiston $ 			 				 */
#ifndef lint
static char sccsid[] = "@(#)isfcb.c 1.9 89/07/17 Copyr 1988 Sun Micro";
#endif
/*
 * Copyright (c) 1988 by Sun Microsystems, Inc.
 */

/*
 * isfcb.c
 *
 * Description: _ambuild()
 *	File Control Block functions
 *	
 *
 */
#include <stdlib.h>
#include "isam_impl.h"
#include <sys/stat.h>

static int _create_datfile(), _create_indfile(), _create_varfile();
static void _remove_datfile(), _remove_indfile(), _remove_varfile();
Static int _open_datfile(), _open_indfile(), _open_varfile();

/*
 * _isfcb_create(isfname, crdat, crind, crvar, owner, group, u_mask, errcode)
 *
 * Create ISAM file: create UNIX files (dat/ind/var), 
 * and initialize File Control Block.
 *
 * Return 0 if create is successful, or -1 if any error. In the case of 
  *an error, the errcode block is set.
 */

Fcb *
_isfcb_create(isfname, crdat, crind, crvar, owner, group, u_mask, errcode)
    char		*isfname;
    int			crdat, crind, crvar; /* 0/1 flags */
    int			owner, group;
    struct errcode	*errcode;
    int			u_mask;
{
    register Fcb	*fcb;
    int			dat_fd = -1;
    int			ind_fd = -1;
    int			var_fd = -1;
    int			oldumask = umask (u_mask); /* Change umask to client's */

    /*
     * Create the UNIX file for .rec file.
     */
    if (crdat && (dat_fd = _create_datfile (isfname)) == -1) {
	_amseterrcode(errcode, errno);
	goto ERROR;
    }

    /*
     * If a primary is specified, create .ind file.
     */
    if (crind && (ind_fd = _create_indfile (isfname)) == -1) {
	_amseterrcode(errcode, errno);
	goto ERROR;
    }
 
    /*
     * If the ISAM file is for variable length records, create .var file.
     */
    if (crvar && (var_fd = _create_varfile (isfname)) == -1) {
	_amseterrcode(errcode, errno);
	goto ERROR;
    }

    /*
     * Change user and group onwer ship of the file.
     * This has affect only when executed by the netisamd daemon.
     */
    if (dat_fd != -1)
	(void) fchown(dat_fd, owner, group);
    if (ind_fd != -1)
	(void) fchown(ind_fd, owner, group);
    if (var_fd != -1)
	(void) fchown(var_fd, owner, group);

    /*
     * Allocate File Control Block.
     */
    fcb = (Fcb *) _ismalloc(sizeof(*fcb));
    memset ((char *) fcb, 0, sizeof(*fcb));

    fcb->isfname = _isallocstring(isfname);
    fcb->rdonly = FALSE;
    fcb->datfd = dat_fd;	
    fcb->indfd = ind_fd;
    fcb->varfd = var_fd;

    fcb->datsize = N_CNTL_PAGES;	     /* Control Pages */
    fcb->indfreelist = FREELIST_NOPAGE;

    /* Key descriptor */
    fcb->nkeys = 1;
    fcb->keys = (Keydesc2 *) _ismalloc(sizeof(fcb->keys[0]));
    memset((char *)fcb->keys, 0, sizeof(fcb->keys[0]));
    
    return (fcb);

 ERROR:
    /* Undo whatever was done. */
    if (dat_fd != -1) {	
	(void) close(dat_fd);
	_remove_datfile(isfname);
    }
    if (ind_fd != -1) {
	(void) close(ind_fd);
	_remove_indfile(isfname);
    }	
    if (var_fd != -1) {
	(void) close(var_fd);
	_remove_varfile(isfname);
    }

    (void) umask(oldumask);

    return (NULL);
}

/*
 * _isfcb_setlength(fcb, varflag, minreclen, maxreclen)
 *
 * Set FCB attributes pertaining to record length.
 */

void
_isfcb_setreclength(fcb, varflag, minreclen, maxreclen)
    register Fcb	*fcb;
    Bool       		varflag;
    int			minreclen, maxreclen;
{
    fcb->varflag = varflag;
    fcb->minreclen = minreclen;
    fcb->maxreclen = maxreclen;
}

/*
 * _isfcb_open(isfname, errcode)
 *
 * Open ISAM file: open UNIX files and create File Control Block.
 *
 * Return 0 if open is successful, or -1 if any error. In the case of an error,
 * the errcode block is set.
 *
 * Note that rdonly is not fuly implemented. Now, all ISAM files are assumed
 * to have permissions set to 0666 and may be opened in RW mode.
 * If read-only media are used then the _open_datfile() function would have to
 * try first to open the file in RW mode, and of that failed, try to 
 * open it in RO mode. The rdonly flag is used to reject any isamopen() with
 * INOUT or OUTPUT mode on such files.
 */

Fcb *
_isfcb_open(isfname, errcode)
    char		*isfname;
    struct errcode	*errcode;
{
    register Fcb	*fcb;
    int			dat_fd = -1;
    int			ind_fd = -1;
    int			var_fd = -1;
    Bool		rdonly;		     /* set to 1 if file is Read-Only */

    /*
     *  Open the UNIX file for .rec file.
     */
    if ((dat_fd = _open_datfile (isfname, &rdonly)) == -1 || errno == EMFILE) {
	_amseterrcode(errcode, errno);
	return (NULL);
    }

    /*
     * Open .ind file.
     */
    ind_fd = _open_indfile (isfname, rdonly);
 
    /*
     * Open .var file.
     */
    var_fd = _open_varfile (isfname, rdonly);


    /*
     * Allocate File Control Block.
     */
    fcb = (Fcb *) _ismalloc(sizeof(*fcb));
    memset ((char *) fcb, 0, sizeof(*fcb));

    fcb->isfname = _isallocstring(isfname);
    fcb->rdonly = rdonly;
    fcb->datfd = dat_fd;	
    fcb->indfd = ind_fd;
    fcb->varfd = var_fd;

    /* Key descriptor */
    fcb->nkeys = 1;
    fcb->keys = (Keydesc2 *) _ismalloc(sizeof(fcb->keys[0]));
    memset((char *)fcb->keys, 0, sizeof(fcb->keys[0]));

    return (fcb);
}

/*
 * _isfcb_nfds(fcb)
 *
 * Return the number of UNIX fd consumed by the fcb block.
 */

int
_isfcb_nfds(fcb)
    register Fcb	*fcb;
{
    register int	count = 0;

    if (fcb->datfd != -1)
	count++;

    if (fcb->indfd != -1)
	count++;

    if (fcb->varfd != -1)
	count++;
    
    return (count);
}


/*
 * _isfcb_remove(fcb)
 *
 * Remove UNIX files associated with an FCB.
 */

void
_isfcb_remove(fcb)
    register Fcb	*fcb;
{
    if (fcb->datfd)
	_remove_datfile(fcb->isfname);
    
    if (fcb->indfd)
	_remove_indfile(fcb->isfname);

    if (fcb->varfd)
	_remove_varfile(fcb->isfname);
}

/*
 * _isfcb_close(fcb)
 *
 * Close UNIX files associated with an FCB, deallocate the FCB block.
 */

void
_isfcb_close(fcb)
    register Fcb	*fcb;
{
    assert (fcb != NULL);
    assert (fcb->isfname != NULL);

    (void) close(fcb->datfd);
    (void) close(fcb->indfd);
    (void) close(fcb->varfd);

    _isfreestring(fcb->isfname);
    free((char *)fcb->keys);
    free((char *)fcb);
}

/*
 * _isfcb_cntlpg_w(fcb)
 *
 * Write information from the control block to the disk.
 * Note that the Control Page transfer bypasses the disk buffer manager.
 *
 * Return 0 if write was successful, return -1 if any error.
 */

int
_isfcb_cntlpg_w(fcb)
    register Fcb	*fcb;
{
    char	       	cntl_page[ISCNTLSIZE];
    int			dat_fd = fcb->datfd;
    int			i;

    /* Clear the page. */
    memset (cntl_page, 0, sizeof(cntl_page));

    /* Set Magic number. */
    (void)strcpy(cntl_page + CP_MAGIC_OFF, ISMAGIC);

    /* Set Block size */
    stshort(ISPAGESIZE, cntl_page + CP_BLOCKSIZE_OFF);

    /* Set NetISAM version stamp. */
    (void)strcpy(cntl_page + CP_VERSION_OFF, ISVERSION);

    /* .rec file size in blocks. */
    stlong((long)fcb->datsize, cntl_page + CP_DATSIZE_OFF);

    /* .ind file size in blocks. */
    stlong((long)fcb->indsize, cntl_page + CP_INDSIZE_OFF);

    /* .var file size in blocks. */
    stlong((long)fcb->varsize, cntl_page + CP_VARSIZE_OFF);

    /* Variable length 0/1 flag. */
    stshort((short)fcb->varflag, cntl_page + CP_VARFLAG_OFF);

    /* Number of records. */
    stlong((long)fcb->nrecords, cntl_page + CP_NRECORDS_OFF);

    /* Minimum and maximum record length. */
    stshort((short)fcb->minreclen, cntl_page + CP_MINRECLEN_OFF);
    stshort((short)fcb->maxreclen, cntl_page + CP_MAXRECLEN_OFF);

    /* Last record number. */
    strecno(fcb->lastrecno, cntl_page + CP_LASTRECNO_OFF);

    /* Free record number. */
    strecno(fcb->freerecno, cntl_page + CP_FREERECNO_OFF);
 
    /* Number of keys */
    stshort((short)fcb->nkeys, cntl_page + CP_NKEYS_OFF);

    /* Last key id */
    stlong((long)fcb->lastkeyid, cntl_page + CP_LASTKEYID_OFF);

    /* ind. free list head */
    stlong((long)fcb->indfreelist, cntl_page + CP_INDFREELIST_OFF);

    /* offset of the end of .var file */
    stlong((long)fcb->varend, cntl_page + CP_VAREND_OFF);

    /* Key descriptors. */
    for (i = 0; i < fcb->nkeys; i++) {
	stkey(fcb->keys + i, cntl_page + CP_KEYS_OFF + i * K2_LEN);
    }

    /* Increment stamp1 and stamp2 to indicate change in the Control Page. */
    fcb->changestamp1++;
    fcb->changestamp2++;

    stlong((long)fcb->changestamp1, cntl_page + CP_CHANGESTAMP1_OFF);
    stlong((long)fcb->changestamp2, cntl_page + CP_CHANGESTAMP2_OFF);
    
    /* 
     * Write the buffer to the disk.
     */
    _isseekpg(dat_fd, ISCNTLPGOFF);
    _iswritepg(dat_fd, cntl_page);
    _iswritepg(dat_fd, cntl_page + ISPAGESIZE);

    return (ISOK);
}

/*
 * _isfcb_cntlpg_w2(fcb)
 *
 * Write information from the control block to the disk.
 * Write only selected fields of the control block to avoid the overhead
 * of coding and decoding.
 * Note that the Control Page transfer bypasses the disk buffer manager.
 *
 * Return 0 if write was successful, return -1 if any error.
 */

int
_isfcb_cntlpg_w2(fcb)
    register Fcb	*fcb;
{
    char	       	cntl_page[CP_VAREND_OFF+CP_VAREND_LEN];
    int			dat_fd = fcb->datfd;

    /* 
     * Read the page from disk.
     */
    _isseekpg(dat_fd, ISCNTLPGOFF);
    (void)read(dat_fd, cntl_page, sizeof(cntl_page));

    /* .rec file size in blocks. */
    stlong((long)fcb->datsize, cntl_page + CP_DATSIZE_OFF);

    /* .ind file size in blocks. */
    stlong((long)fcb->indsize, cntl_page + CP_INDSIZE_OFF);

    /* .var file size in blocks. */
    stlong((long)fcb->varsize, cntl_page + CP_VARSIZE_OFF);

    /* Number of records. */
    stlong((long)fcb->nrecords, cntl_page + CP_NRECORDS_OFF);

    /* Last record number. */
    strecno(fcb->lastrecno, cntl_page + CP_LASTRECNO_OFF);

    /* Free record number. */
    strecno(fcb->freerecno, cntl_page + CP_FREERECNO_OFF);

    /* ind. free list head */
    stlong((long)fcb->indfreelist, cntl_page + CP_INDFREELIST_OFF);

    /* end of .var file */
    stlong((long)fcb->varend, cntl_page + CP_VAREND_OFF);

    /* Increment stamp2 to indicate change in the Control Page. */
    fcb->changestamp2++;
    stlong((long)fcb->changestamp2, cntl_page + CP_CHANGESTAMP2_OFF);
    

    /* 
     * Write the buffer to the disk.
     */
    _isseekpg(dat_fd, ISCNTLPGOFF);
    (void)write(dat_fd, cntl_page, sizeof(cntl_page));

    return (ISOK);
}

/*
 * _isfcb_cntlpg_r(fcb)
 *
 * Read information from control page and store it in the FCB.
 * Note that the Control Page transfer bypasses the disk buffer manager.
 *
 * Return 0 if read was successful, return -1 if any error.
 */

int
_isfcb_cntlpg_r(fcb)
    register Fcb	*fcb;
{
    char	       	cntl_page[ISCNTLSIZE];
    int			dat_fd = fcb->datfd;
    int			i;
    
    /* 
     * Read the page from the disk.
     */
    _isseekpg(dat_fd, ISCNTLPGOFF);
    _isreadpg(dat_fd, cntl_page);
    _isreadpg(dat_fd, cntl_page + ISPAGESIZE);

    /* block size */
    fcb->blocksize = ldshort(cntl_page + CP_BLOCKSIZE_OFF);

    /* .rec file size in blocks. */
    fcb->datsize = ldlong(cntl_page + CP_DATSIZE_OFF);

    /* .ind file size in blocks. */
    fcb->indsize = ldlong(cntl_page + CP_INDSIZE_OFF);

    /* .var file size in blocks. */
    fcb->varsize = ldlong(cntl_page + CP_VARSIZE_OFF);

    /* Variable length 0/1 flag. */
    fcb->varflag = (Bool)ldshort(cntl_page + CP_VARFLAG_OFF);

    /* Number of records. */
    fcb->nrecords = ldlong(cntl_page + CP_NRECORDS_OFF);

    /* Minimum and maximum record length. */
    fcb->minreclen = ldunshort(cntl_page + CP_MINRECLEN_OFF);
    fcb->maxreclen = ldunshort(cntl_page + CP_MAXRECLEN_OFF);

    /* Last record number. */
    fcb->lastrecno = ldrecno(cntl_page + CP_LASTRECNO_OFF);

    /* Free record number. */
    fcb->freerecno = ldrecno(cntl_page + CP_FREERECNO_OFF);

    /* Last key id */
    fcb->lastkeyid = ldlong(cntl_page + CP_LASTKEYID_OFF);

    /* .ind free list head. */
    fcb->indfreelist = ldlong(cntl_page + CP_INDFREELIST_OFF);

    /* end of .var file */
    fcb->varend = ldlong(cntl_page + CP_VAREND_OFF);

    /* Number of keys */
    fcb->nkeys = ldshort(cntl_page + CP_NKEYS_OFF);

    /*
     * Read key descriptors.
     */
    fcb->keys = (Keydesc2 *) 
	_isrealloc((char *)fcb->keys,
		   (unsigned) (sizeof(Keydesc2) * fcb->nkeys));
    memset((char *)fcb->keys, 0, sizeof(Keydesc2) * fcb->nkeys);

    for (i = 0; i < fcb->nkeys; i++) {
	ldkey(fcb->keys + i, cntl_page + CP_KEYS_OFF + i * K2_LEN);
    }

    /* Changestamp1 */
    fcb->changestamp1 = ldlong(cntl_page + CP_CHANGESTAMP1_OFF);

    /* Changestamp2 */
    fcb->changestamp2 = ldlong(cntl_page + CP_CHANGESTAMP2_OFF);

    /*
     * Open .ind file in situations when some other process has created
     * keys and this process has just learned it now.
     */
    if (fcb->nkeys > 1 || !FCB_NOPRIMARY_KEY(fcb)) {
/*
	if (_open2_indfile(fcb) != ISOK)
	    _isfatal_error("_open2_indfile()");
*/
	(void)_open2_indfile(fcb);
    }

    
    return (ISOK);
}


/*
 * _isfcb_cntlpg_r2(fcb)
 *
 * Read information from the control block on the disk.
 * Read only selected fields of the control block to avoid the overhead
 * of coding and decoding.
 * Note that the Control Page transfer bypasses the disk buffer manager.
 *
 * Return 0 if write was successful, return -1 if any error.
 */

int
_isfcb_cntlpg_r2(fcb)
    register Fcb	*fcb;
{
    char	       	cntl_page[CP_VAREND_OFF+CP_VAREND_LEN];
    int			dat_fd = fcb->datfd;

    /* 
     * Read the page from disk.
     */
    _isseekpg(dat_fd, ISCNTLPGOFF);
    (void)read(dat_fd, cntl_page, sizeof(cntl_page));

    /*
     * Check changestamp1. If the stamp has changed, we must read the entire
     * page and update the FCB.
     */
    if (ldlong(cntl_page + CP_CHANGESTAMP1_OFF) != fcb->changestamp1) {
	(void)_isfcb_cntlpg_r(fcb);
    }

    /* 
     *_isfcb_cntlpg_r2() is called if transaction is rolled back.
     * We cannot test changestamp2; we must read the info into the FCB
     * always.
     */
       
#if 0
    /*
     * Check changestamp2. If the stamp has not changed, the FCB contains
     * up-to-date information.
     */
    if (ldlong(cntl_page + CP_CHANGESTAMP2_OFF) == fcb->changestamp2) {
	return (ISOK);
    }
#endif

    /* .rec file size in blocks. */
    fcb->datsize = ldlong(cntl_page + CP_DATSIZE_OFF);

    /* .ind file size in blocks. */
    fcb->indsize = ldlong(cntl_page + CP_INDSIZE_OFF);

    /* .var file size in blocks. */
    fcb->varsize = ldlong(cntl_page + CP_VARSIZE_OFF);

    /* Number of records. */
    fcb->nrecords = ldlong(cntl_page + CP_NRECORDS_OFF);

    /* Last record number. */
    fcb->lastrecno = ldrecno(cntl_page + CP_LASTRECNO_OFF);

    /* Free record number. */
    fcb->freerecno = ldrecno(cntl_page + CP_FREERECNO_OFF);

    /* .ind free list head. */
    fcb->indfreelist = ldlong(cntl_page + CP_INDFREELIST_OFF);

    /* end of .var file */
    fcb->varend = ldlong(cntl_page + CP_VAREND_OFF);

    /* Changestamp2 */
    fcb->changestamp2 = ldlong(cntl_page + CP_CHANGESTAMP2_OFF);
    
    return (ISOK);
}


/*
 * _create_datfile(isfname)
 *
 * Create .rec file for ISAM file isfname.
 */

Static int
_create_datfile(isfname)
    char	*isfname;
{
	int	fd;
	char	namebuf[MAXPATHLEN];

	(void) strcpy(namebuf, isfname);
	_makedat_isfname(namebuf);
	
	fd = open (namebuf, O_CREAT | O_EXCL | O_RDWR, 0666);
	if (fd > -1) {
		fcntl(fd, F_SETFD, 1);	/* Close on exec */
	}
	return (fd);
}

/*
 * _create_indfile(isfname)
 *
 * Create .ind file for ISAM file isfname.
 */

Static int
_create_indfile(isfname)
    char	*isfname;
{
	int	fd;
	char	namebuf[MAXPATHLEN];

	(void) strcpy(namebuf, isfname);
	_makeind_isfname(namebuf);

	fd = open (namebuf, O_CREAT | O_EXCL | O_RDWR, 0666);
	if (fd > -1) {
		fcntl(fd, F_SETFD, 1);	/* Close on exec */
	}
	return (fd);
}

/*
 * _create_varfile(isfname)
 *
 * Create .var file for ISAM file isfname.
 */

Static int
_create_varfile(isfname)
    char	*isfname;
{
	int	fd;
	char	namebuf[MAXPATHLEN];

	(void) strcpy(namebuf, isfname);
	_makevar_isfname(namebuf);

	fd = open (namebuf, O_CREAT | O_EXCL | O_RDWR, 0666);
	if (fd > -1) {
		fcntl(fd, F_SETFD, 1);	/* Close on exec */
	}
	return (fd);
}


/*
 * _remove_datfile(isfname)
 *
 * Remove .rec file for ISAM file isfname.
 */

Static void
_remove_datfile(isfname)
    char	*isfname;
{
    char	namebuf[MAXPATHLEN];

    (void) strcpy(namebuf, isfname);
    _makedat_isfname(namebuf);

    (void) unlink(namebuf);
}

/*
 * _remove_indfile(isfname)
 *
 * Remove .ind file for ISAM file isfname.
 */

Static void
_remove_indfile(isfname)
    char	*isfname;
{
    char	namebuf[MAXPATHLEN];

    (void) strcpy(namebuf, isfname);
    _makeind_isfname(namebuf);

    (void) unlink(namebuf);
}

/*
 * _remove_varfile(isfname)
 *
 * Remove .var file for ISAM file isfname.
 */

Static void
_remove_varfile(isfname)
    char	*isfname;
{
    char	namebuf[MAXPATHLEN];

    (void) strcpy(namebuf, isfname);
    _makevar_isfname(namebuf);

    (void) unlink(namebuf);
}


/*
 * _open_datfile(isfname)
 *
 * Open .rec file for ISAM file isfname.
 */

Static int
_open_datfile(isfname, rdonly)
    char	*isfname;
    Bool	*rdonly;
{
    char	namebuf[MAXPATHLEN];
    int		ret;

    (void) strcpy(namebuf, isfname);
    _makedat_isfname(namebuf);

    if ((ret = open (namebuf, O_RDWR)) != -1) {
	*rdonly = FALSE;
	fcntl(ret, F_SETFD, 1);		/* Close on exec */
	return (ret);
    }

    *rdonly = TRUE;
    ret = open (namebuf, O_RDONLY);
    if (ret > -1) {
	    fcntl(ret, F_SETFD, 1);	/* Close on exec */
    }
    return (ret);
}

/*
 * _open_indfile(isfname)
 *
 * Open .ind file for ISAM file isfname.
 */

Static int
_open_indfile(isfname, rdonly)
    char	*isfname;
    Bool	rdonly;
{
	int	fd;
	char	namebuf[MAXPATHLEN];

	(void) strcpy(namebuf, isfname);
	_makeind_isfname(namebuf);

	fd = open (namebuf, (rdonly==TRUE)?O_RDONLY:O_RDWR);
	if (fd > -1) {
	    fcntl(fd, F_SETFD, 1);	/* Close on exec */
	}
	return (fd);
}

/*
 * _open_varfile(isfname)
 *
 * Open .var file for ISAM file isfname.
 */

Static int
_open_varfile(isfname, rdonly)
    char	*isfname;
    Bool	rdonly;
{
	int	fd;
	char	namebuf[MAXPATHLEN];

	(void) strcpy(namebuf, isfname);
	_makevar_isfname(namebuf);

	fd = open (namebuf, (rdonly==TRUE)?O_RDONLY:O_RDWR);
	if (fd > -1) {
		fcntl(fd, F_SETFD, 1);	/* Close on exec */
	}
	return (fd);
}

int
_check_isam_magic(fcb)
    Fcb		*fcb;
{
    char		magicbuffer[CP_MAGIC_LEN];

    (void)lseek(fcb->datfd, 0L, 0);
    if (read(fcb->datfd, magicbuffer, CP_MAGIC_LEN) < CP_MAGIC_LEN ||
	/* The following test for compatibilty with `SunISAM 1.0 Beta files. */
	strncmp(magicbuffer, "SunISAM", strlen(ISMAGIC)) != 0 &&
	strncmp(magicbuffer, ISMAGIC, strlen(ISMAGIC)) != 0) {
	return ISERROR;
    }
    else
	return ISOK;
}


/*
 * _open2_indfile(fcb)
 *
 * Open (or create) .ind file for ISAM file if the .ind file
 * is not open already (or does not exist).
 */

int
_open2_indfile(fcb)
    Fcb		*fcb;
{
    char	namebuf[MAXPATHLEN];
    struct stat buf;
    int		openmode;

    if (fcb->indfd != -1)
	return (ISOK);

    (void) strcpy(namebuf, fcb->isfname);
    _makeind_isfname(namebuf);

    (void)fstat(fcb->datfd, &buf);

    openmode = (fcb->rdonly) ? O_RDONLY : O_RDWR;

    if (fcb->indsize == 0)
	openmode |= O_CREAT;

    fcb->indfd =  open(namebuf, openmode, buf.st_mode);
    if (fcb->indfd > -1) {
	    fcntl(fcb->indfd, F_SETFD, 1);	/* Close on exec */
    }

    if(fcb->indfd == -1 && (openmode & O_CREAT)) {
	_isfatal_error("Cannot create .ind file");
    }

    if (fcb->indfd != -1) {
	(void) _watchfd_incr(1);
	(void)fchown(fcb->indfd, buf.st_uid, buf.st_gid);
    }

    return ((fcb->indfd == -1) ? ISERROR : ISOK);
}
