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
/*%%  $TOG: isam_impl.h /main/4 1998/03/16 14:41:21 mgreess $ 			 				 */
/* @(#)isam_impl.h	1.11 93/09/07 */

/*
 * Copyright (c) 1988 by Sun Microsystems, Inc.
 */

/*
 * isam_impl.h
 *
 * Description:
 *	NetISAM implementation specific definitions
 *
 */

#ifndef _ISAM_IMPL_H
#define _ISAM_IMPL_H

#include <assert.h> 
#include <stdio.h>
#include <fcntl.h>
#include <setjmp.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/time.h>
/* AIX does not define FD_SETSIZE in sys/types.h. */
#if !defined(FD_SETSIZE)
#include <sys/select.h>
#endif

/*
 * #include sys/types.h is to get u_long, u_short, dev_t
 *
 * #include sys/param.h is to get MAXHOSTNAMELEN
 */

#include "isam.h"

/*
 * Static is used in definitions of local functions. It is defined to 
 * "static" normally, but to an empty string when compiled for profiling
 * purposes (-p or -pg options to the cc compiler). If a function is
 * defined as static, then the profiling tools account the time spent in
 * this function to the physically previous global function which is
 * incorrect and makes the output of the profiler tools useless.
 */
#if defined(PROF) || defined(GPROF)
#define Static	
#else
#define Static static
#endif

extern int isdupl;

typedef long Recno;			     /* record numbers */
typedef long Blkno;			     /* block numbers */
typedef long Time;			     /* standard UNIX time */
typedef int  Isfd;			     /* ISAM file descriptor */

typedef int  Bool;
#ifndef FALSE
#define FALSE	0
#define TRUE	1
#endif

#define ISOPENMODE	0xff		     /* Mask to get ISINPUT/ISOUTPUT */
					     /* or ISINOUT */
#define ISLOCKMODE	0xff00		     /* Mask to get lock mode */
#define ISREADMODE	0xff		     /* Mask to get ISFIRST, etc. */

#define ISLENMODE	0x10000		     /* Mask to get FIX/VAR length */

#define ISPAGESIZE	1024		     /* page size */

#define N_CNTL_PAGES	2		     /* 
					      * Number of control pages 
					      * at the beg. of .rec file 
					      */
#define ISCNTLSIZE	(ISPAGESIZE * N_CNTL_PAGES)


#define ISMAGIC		"NetISAM"	     /* 'magic number' in ISAM files */

#ifndef ISVERSION
#define ISVERSION	"Unknown"
#endif

#define MAXFCB_UNIXFD	30		     /* Maximum number of UNIX fd
					      * used by the FCB module
					      */

#define ISLEAFSLACK	10		     /* Default slack in leaves */

/* Default RPC timeout values */
#define ISRPCS_TO_DEF	60		     /* secs for short oper. */
#define ISRPCL_TO_DEF  (120 * 60)	     /* secs for long oper. */
#define ISTCP_TO_DEF   (3 * 60)		     /* secs to reconnect TCP/IP */
#define ISRPC_POLL_TO_DEF 30		     /* interval to hear from polled client */
#define ISTCP_RETRY_SLEEP 5		     /* time between tries to
					      * establish a connection 
					      */

/* 
 * The X/OPEN ISAM internal data representations.
 * The #define statements are used to declare type lengths and offsets
 * in structures in order for the definitions to be compiler independent.
 */

#define TIMETYPE	LONGTYPE	     /* standard UNIX time */
#define TIMESIZE	LONGSIZE	

#define BLKNOTYPE	LONGTYPE	     /* block (page) numbers */
#define BLKNOSIZE	LONGSIZE

#define RECNOTYPE	LONGTYPE	     /* record numbers */
#define RECNOSIZE	LONGSIZE

/* 
 * SHORTTYPE is a safeguard - if the X/OPEN ISAM changes INTTYPE to 4 bytes 
 * all the internals of NetISAM will work. The only change will be
 * re-defining SHORTTYPE.	
 */

#define SHORTTYPE	INTTYPE		     
#define SHORTSIZE	INTSIZE

/* The following defines are used in isstart() */
#define WHOLEKEY	0
#define USE_PHYS_ORDER(keydesc)  (keydesc->k_nparts == 0)

/* Structure for storing and passing variable length byte array data */
typedef struct bytearray {
    u_short	length;
    char	*data;
} Bytearray;

extern Bytearray _bytearr_getempty(), _bytearr_dup(), _bytearr_new();
extern void _bytearr_free();


/* Client identification - used to identify owners of locks */
/*
   #define IPADDRLEN	4
*/

/* Definitions related to ISAM file descriptor (isfd.c file) */
#define MAXISFD		FD_SETSIZE
#define NOISFD		(-1)		     /* Not a valid file descriptor */

extern struct fab *_isfd_find();
extern void _isfd_delete();
extern Isfd _isfd_insert();

/* Definitions related to File access block (isfab.c file */

/* File open mode */
enum openmode  { OM_INPUT = 0, OM_OUTPUT = 1, OM_INOUT = 2, OM_BADMODE = 3};
enum lockmode  { LM_FAST = 0, LM_EXCL = 1, LM_AUTOMATIC = 2, LM_MANUAL = 3, LM_BADMODE = 4}; 

/* isread() read modes */
enum readmode { RM_FIRST = 0, RM_LAST = 1, RM_NEXT = 2, RM_PREV = 3, 
		    RM_CURR = 4, RM_EQUAL = 5, RM_GREAT = 6, RM_GTEQ = 7,
		    RM_LESS = 8, RM_LTEQ = 9, RM_BADMODE = 10 };

/* lock flag is passed to Access Method module for every record oriented op. */
/* Bit position in the lock flag: */

enum openmode _getopenmode();
enum readmode _getreadmode();

/* Error code structure */
struct errcode {
    int		iserrno;
    char	isstat [4];
};

typedef struct fab {
    Isfd		isfd;		     /* ISAM file descriptor */
    enum openmode 	openmode;	     /* File access mode */
    enum lockmode	lockmode;	     /* File lock mode */
    Bool		locked;		     /* TRUE if file is locked */
    Bool		varlength;	     /* TRUE if variable length records
					      * supported by this ISAM file */
    int			minreclen;	     /* Minimum record length */
    int			maxreclen;	     /* Maximum record length */
    char		*isamhost;	     /* IP host name */
    char		*isfname;	     /* ISAM file local pathname */
    Bytearray		isfhandle;	     /* ISAM file handle */
    Bytearray		curpos;		     /* Current record position */
    struct errcode	errcode;	     /* Error codes structure */
} Fab;

/* values for filemode */
#define LOCAL_FILE	0
#define REMOTE_FILE	1
#define NFS_FILE	2		     /* File is remote, but is
					      * accessed via NFS
					      */

#define FAB_ISFDSET(fab, isfd) (fab->isfd = isfd)

extern Fab *_fab_new();
void _fab_destroy();

/* ISAM file identification for locking purposes. */
typedef struct lckfid {
    dev_t		diskid;		     /* Disk device id */
    ino_t		inode;		     /* .rec file inode number */
} Lckfid; 

/* File Control Block */
typedef struct fcb {
    char		*isfname;	     /* ISAM file name */
    int			blocksize;	     /* Block size */
    Bool		rdonly;		     /* file is read-only */
    Lckfid		lckfid;		     /* File id for locking */
    int			datfd;		     /* UNIX file descr. of .rec file */
    int			indfd;		     /* UNIX file descr. of .ind file */
    int			varfd;		     /* UNIX file descr. of .var file */
    Blkno		datsize;	     /* dat file size in blocks */
    Blkno		indsize;	     /* ind file size in blocks */
    Blkno		varsize;	     /* var file size in blocks */
    Bool		varflag;	     /* TRUE if variable length*/
    long		nrecords;	     /* Number of records */
    int			minreclen;	     /* Minimum record length */
    int			maxreclen;	     /* Maximum record length */
    Recno		lastrecno;	     /* Last recno in use */
    Recno		freerecno;	     /* Pointer to first free record */
    int			lastkeyid;	     /* Last key identifier used */
    int			changestamp1;	     /* Stamp 1 of last change */
    int			changestamp2;	     /* Stamp 2 of last change */
    Blkno		indfreelist;	     /* Head of freepage list of .ind */

    int			nkeys;		     /* Number of keys */
    struct keydesc2	*keys;		     /* Key descriptors */
    long		varend;		     /* Offset of the last byte */
					     /* in .var file */
    int                 lockfd;        /* lock file fd for locking .rec for 5.0 */
} Fcb;

#define FCB_NOPRIMARY_KEY(fcb) ((fcb)->keys[0].k2_nparts == 0)

/* Current Record Position (interpreted only by Access Method) */
enum crpflag { CRP_UNDEF = 0, CRP_ON = 1, CRP_AFTER = 2, CRP_BEFORE = 3,
	       CRP_BEFOREANY = 4, CRP_AFTERANY = 5};
typedef struct crp {
    int			keyid;		     /* Key index */
    enum crpflag	flag;		     /* ON/BEFORE/AFTER/UNDEF */
    Recno		recno;		     /* Record number */
    int			matchkeylen;	     /* match so many bytes of key */
    char		key[1];		     /* Index position */
} Crp;

#define PHYS_ORDER   (-1)		     /* value for keyid when physical
					      order is used*/

/* 
 * keydesc2 is the 'internal copy'of keydesc. 
 * keydesc2 contains all information needed by internally by NetISAM,
 * whereas keydesc conforms to the X/OPEN ISAM. 
 */

#define NPARTS2 (NPARTS+2)		     /* duplid and recno */
#define DUPSMASK   001			     /* Mask to get ISDUPS/ISNODUPS */
#define ISPRIMKEY	040		     /* this is primary key */
#define ALLOWS_DUPS2(pkdesc2) (((pkdesc2)->k2_flags & DUPSMASK) == ISDUPS)
#define ALLOWS_DUPS(pkdesc) (((pkdesc)->k_flags & DUPSMASK) == ISDUPS)

/* serial number of duplicate keys */
#define DUPIDTYPE	LONGTYPE
#define DUPIDSIZE	LONGSIZE

#define KEY_RECNO_OFF	0
#define KEY_DUPS_OFF	RECNOSIZE

#define stdupser(n, b) stlong((long)(n), (b))
#define lddupser(b)  ((int)ldlong(b))

struct keypart2 {
    u_short 	kp2_start;		     /* starting byte of key part */
    short	kp2_leng;		     /* length in bytes */
    short	kp2_type;		     /* type of key part */
    u_short	kp2_offset;		     /* offset in key buffer */
};

typedef struct keydesc2 {
    short 	k2_flags;		     /* flags */
    short	k2_nparts;		     /* number of parts in key */
    short	k2_len;			     /* length of the whole key */
    Blkno	k2_rootnode;		     /* pointer to root node */
    int		k2_keyid;		     /* Key identifier */
    struct keypart2 k2_part[NPARTS2];	     /* each part */
} Keydesc2;

typedef unsigned long rel_addr;
/* Double linked list element */
struct dlink {
    rel_addr	 dln_forward;		     /* Forward link */
    rel_addr	 dln_backward;		     /* Backward link */
};

/* Disk buffer management definitions */

/* cache buffer header */
typedef struct bufhdr {
    Fcb		*isb_fcb;		     /* Pointer to FCB */
    int		isb_unixfd;		     /* UNIX file descriptor */
    Blkno	isb_blkno;		     /* block number */
    struct dlink isb_hash;		     /* hashed list*/
    struct dlink isb_aclist;		     /* available or changed list */
    struct dlink isb_flist;		     /* list of fixed blocks */
    char	*isb_buf_w;		     /* malloc() buffer with dirty data */
    char	*isb_buf_r;		     /* points to mapped segment */
    char	*isb_buffer;		     /* set to isb_buf_r or isb_buf_w */
    char	isb_flags;		     /* flags - defs. see below */
    struct bufhdr *isb_oldcopy;		     /* pointer to old copy */
} Bufhdr;

/* mapped segment header */
typedef struct maphdr {
    Fcb		*m_fcb;			     /* Pointer to FCB */
    int		m_unixfd;		     /* UNIX file descriptor */
    char       	*m_addr;		     /* Pointer to beginning of seg. */
    int		m_segm_num;		     /* Segment number in the file */
    unsigned long m_stamp;		     /* Assigned when touched */
} Maphdr;


/* values of isb_flags */
#define ISB_NODATA	00		     /* block has no data */
#define ISB_READ	01		     /* page has valid data */
#define ISB_CHANGE	02		     /* page must be flushed */
#define ISB_RFIXED	04		     /* block is fixed for read*/
#define ISB_WFIXED     010		     /* block is fixed for write*/
#define ISB_OLDCOPY    020		     /* block is old copy */

/* mode values to is__cache_fix() */
#define ISFIXREAD	1		     /* fix for read */
#define ISFIXWRITE	2		     /* fix for update */
#define ISFIXNOREAD	3		     /* fix for update, don't read */


/* 
 * Macro to get pointer to structure if pointer to some element is known 
 */
#define GETBASE(p,s,e) ((struct s *) ((char *)(p) - (int)&((struct s *)0)->e))

/* In memory sorting object */
typedef struct issort {
    int		ist_reclength;		     /* record length in bytes */
    int		ist_allocrecs;		     /* memory allocated for so */
					     /* many records */
    int		ist_nrecs;		     /* number of records inserted */
    int		ist_currec;		     /* current position */
    int		(*ist_compf) ();	     /* comparison function */
    char	*ist_array;		     /* array of records */
} Issort;


/* btree is object used for processing B-tree operations */

#define ISMAXBTRLEVEL	12		     /* Maximum level of B-tree */

typedef struct btree {
    Fcb		*fcb;
    Keydesc2	*keydesc2;
    int		depth;			     /* depth of the B-tree */
    Bufhdr 	*bufhdr[ISMAXBTRLEVEL];	     /* fixed blocks buf. headers */
    int		curpos[ISMAXBTRLEVEL];	     /* current position on block */
} Btree;

/* Lock manager related definitions. */
/* Entry in lock table */

/* Number of buckets for hashing by clientid. Must be a power of 2*/
#ifndef HASHPROCSIZE
#define HASHPROCSIZE	64
#endif

/* 
 * Number of buckets for hashing by record number and fileid.
 * Must be a power of 2 
 */
#ifndef HASHENTSIZE
#define HASHENTSIZE	1024
#endif

/* Pseudo record numbers used by file locking. */
#define AVAIL_RECNO	(0L)		     /* Entry is available */
#define ALLFILE_RECNO	(-1L)		     /* Entire file lock */
#define	OPENFILE_RECNO	(-2L)		     /* Used by isopen() */

/* Lock table file header */
struct lockfilehdr {
    int			size;		     /* size of LOCKTABLEFILE 
					      * set to 0 forces reinitialization
					      * at next lock request.
					      */
    int			prochdsn;	     /* Number of lists hashed by 
					      *	clientid */
    int			enthdsn;	     /* Number of list hashed by
					      * record number */
    int			lockentriesn;	     /* total number of lock entries */
    struct dlink	avail;		     /* Double linked list of 
					      * available lock entries */
};

/* Exported filesystem options structure and constants*/
 
#define RW_CLIENTS_SIZE 1     /* Not currently needed */
#define UID_SIZE 10           /* Big enough to hold a Unix UID */
#define ROOT_OPT_SIZE 240     /* List of hosts for which root access
                                 is allowed -- can be quite long */
#define SHARE_BUFFER_SIZE 400
 
struct export_opts {
  int read_write; /* 0 => read-only, 1 => read/write */
  char rw_clients[1]; /* Not interesting, len=1 to save space */
  char export_dir[1]; /* ditto here */
  char anon[UID_SIZE];
  char root[ROOT_OPT_SIZE];
  int secure; /* RPC authentication flag: 0 => AUTH_UNIX, 1 => AUTH_DES */
};


extern Fcb *_isfcb_create();
extern Fcb *_isfcb_open();
extern void _isfcb_remove();
extern void _isfcb_close();
extern void _isfcb_setreclength();
extern void _isfcb_setprimkey();
extern int _isfcb_cntlpg_w();


#define DAT_SUFFIX	".rec"
#define IND_SUFFIX	".ind"
#define VAR_SUFFIX	".var"
#define LOCK_SUFFIX     ".lock"         /* yh: lock file for .rec to solve mmap in 5.0 */

/* Values of deleted flag for fixed length records */
#define FL_RECDELETED	0
#define FL_RECEXISTS    1

/* Special values of tail pointer for variable length records */
#define VL_RECDELETED	((long ) -1L)
#define VL_RECNOTAIL	((long ) -2L)


/* extern long lseek(); */

extern char *_ismalloc(), *_isrealloc(), *_isallocstring();
extern void _isfree(), _isfreestring();
extern void _isfatal_error(), _setiserrno2(), _seterr_errcode();
extern void _makedat_isfname();
extern void _makeind_isfname();
extern void _makevar_isfname();
extern void _amseterrcode();

extern void _isseekpg(), _isreadpg(), _iswritepg();

extern Bufhdr *_isdisk_fix(), *_isdisk_refix();
extern void _isdisk_commit(), _isdisk_sync(), _isdisk_inval(), 
    _isdisk_commit1(), _isdisk_unfix();
extern void _isdisk_rollback();
extern void _cp_tofile(), _cp_fromfile();
Blkno  _extend_file();

extern void _isdln_base_insert(), _isdln_base_remove(), _isdln_base_append(), 
    _isdln_base_makeempty();
extern struct dlink *_isdln_base_prev(), *_isdln_base_next(), *_isdln_base_first();

extern void _mngfcb_insert(), _mngfcb_delete();
extern Fcb *_mngfcb_find();
extern Bytearray *_mngfcb_victim();
extern char *_getisfname();
extern Bytearray _makeisfhandle();

extern Bytearray _makeisfhandle();
extern Fcb *_openfcb();

extern Time _amtime_set(), _amtime_get();

extern void _islock_closefile(), _islock_unlockfile();

extern Keydesc2 *_isfcb_findkey(), *_isfcb_indfindkey();
extern void stkey(), ldkey();

extern Issort *_issort_create();
extern char *_issort_read();
extern void _issort_destroy(), _issort_insert(), _issort_sort(), 
    _issort_rewind();

extern void _iskeycmp_set();
extern int _iskeycmp();

extern char *_isbsearch();

extern void _iskey_extract();
extern Blkno _isindfreel_alloc();
extern void _isindfreel_free();
extern long _fl_getpos();

extern Btree *_isbtree_create();
extern void _isbtree_destroy(), _isbtree_search(), _isbtree_insert(),
    _isbtree_remove();
extern char *_isbtree_current(), *_isbtree_next();

extern void _iskey_fillmin(), _iskey_fillmax();

extern Bufhdr *_allockpage();

extern void _del1key();
extern int _add1key(), _change1key();
extern void _delkeys();
extern char *_ismap_getaddr();
extern void _ismap_endop(), _ismap_unmapfcb();

extern void _isam_entryhook(), _isam_exithook();

#define stshort(n,p) stint((n), (p))
#define ldshort(p) ldint(p)

#define strecno(n,p) stlong((long)(n), (p))
#define ldrecno(p) ((Recno)ldlong(p))

#define stblkno(n,p) stlong((long)(n), (p))
#define ldblkno(p) ((Blkno)ldlong(p))

extern int _flrec_read(), _flrec_write(), _flrec_rewrite(), _flrec_delete(),
    _flrec_wrrec();
extern int _vlrec_read(), _vlrec_write(), _vlrec_rewrite(), _vlrec_delete(),
    _vlrec_wrrec();

/*------------ UNIX file formats ---------------------------------------------*/

#define ISCNTLPGOFF	((Blkno) 0)	     /* offset of Control Page */

/* internal key descriptor */

#define KP2_START_OFF	0		     /* see struct keypart2 */
#define KP2_LENGTH_OFF	SHORTSIZE
#define KP2_TYPE_OFF	(2*SHORTSIZE)
#define KP2_OFFSET_OFF	(3*SHORTSIZE)
#define KP2_LEN		(4*SHORTSIZE)

#define K2_FLAGS_OFF	0		     /* see struct keydesc2 */
#define K2_NPARTS_OFF	SHORTSIZE	
#define K2_LEN_OFF	(2*SHORTSIZE)
#define K2_ROOT_OFF	(3*SHORTSIZE)
#define K2_KEYID_OFF    (3*SHORTSIZE+BLKNOSIZE)
#define K2_KEYPART_OFF	(3*SHORTSIZE+2*BLKNOSIZE)
#define K2_LEN		(3*SHORTSIZE+2*BLKNOSIZE+ NPARTS2*KP2_LEN)

#define NULL_BLKNO	0L
#define NULL_RECNO	0L
#define PAGE1_BLKNO	0		     /* Block number of control page */
/*------------------------- Control Page  layout -----------------------------*/

/* Magic number (must be equal to "NetISAM") */
#define CP_MAGIC_OFF	0
#define CP_MAGIC_LEN	8

/* Version stamp */
#define CP_VERSION_OFF (CP_MAGIC_OFF+CP_MAGIC_LEN)
#define CP_VERSION_LEN 8

/* Application stamp */
#define CP_APPLMAGIC_OFF (CP_VERSION_OFF+ CP_VERSION_LEN)
#define CP_APPLMAGIC_LEN ISAPPLMAGICLEN

/* Block size */
#define CP_BLOCKSIZE_OFF (CP_APPLMAGIC_OFF+CP_APPLMAGIC_LEN)
#define CP_BLOCKSIZE_LEN SHORTSIZE

/* file size in blocks */
#define CP_DATSIZE_OFF	(CP_BLOCKSIZE_OFF+CP_BLOCKSIZE_LEN)	
#define CP_DATSIZE_LEN	BLKNOSIZE

/* file size in blocks */
#define CP_INDSIZE_OFF	(CP_DATSIZE_OFF+CP_DATSIZE_LEN)	
#define CP_INDSIZE_LEN	BLKNOSIZE

/* file size in blocks */
#define CP_VARSIZE_OFF	(CP_INDSIZE_OFF+CP_INDSIZE_LEN)
#define CP_VARSIZE_LEN	BLKNOSIZE

/* 0/1 flag telling whether file supports variable length records. */
#define CP_VARFLAG_OFF	(CP_VARSIZE_OFF+CP_VARSIZE_LEN)
#define CP_VARFLAG_LEN	SHORTSIZE

/* number of record in the file */
#define CP_NRECORDS_OFF (CP_VARFLAG_OFF+CP_VARFLAG_LEN)
#define CP_NRECORDS_LEN LONGSIZE

/*  minimum record length */
#define CP_MINRECLEN_OFF (CP_NRECORDS_OFF+CP_NRECORDS_LEN)
#define CP_MINRECLEN_LEN  SHORTSIZE

/*  maximum record length */
#define CP_MAXRECLEN_OFF (CP_MINRECLEN_OFF+CP_MINRECLEN_LEN)
#define CP_MAXRECLEN_LEN  SHORTSIZE

/* 
 * Left 0 and not used in NetISAM 1.0.
 * Will be set at restructuring to some optimal value x 
 * (minreclen <= x <= maxreclen).
 * Record with length will be stored in .rec file only. 
 * In NetISAM 1.0  x == minreclen.
 */
/*  split record record length */
#define CP_SPLITRECLEN_OFF (CP_MAXRECLEN_OFF+CP_MAXRECLEN_LEN)
#define CP_SPLITRECLEN_LEN  SHORTSIZE

/* Recno of last record */
#define CP_LASTRECNO_OFF  (CP_SPLITRECLEN_OFF+CP_SPLITRECLEN_LEN)
#define CP_LASTRECNO_LEN  RECNOSIZE

/* Head of deleted records list */
#define CP_FREERECNO_OFF  (CP_LASTRECNO_OFF+CP_LASTRECNO_LEN)
#define CP_FREERECNO_LEN  RECNOSIZE

/* 0/1 flag telling whether file has a primary key */
#define CP_HASPRIMKEY_OFF (CP_FREERECNO_OFF+CP_FREERECNO_LEN)
#define CP_HASPRIMKEY_LEN SHORTSIZE

/* Last key indentifier. */
#define CP_LASTKEYID_OFF (CP_HASPRIMKEY_OFF+CP_HASPRIMKEY_LEN)
#define CP_LASTKEYID_LEN  LONGSIZE	

/* Stamp 1 of last change */
#define CP_CHANGESTAMP1_OFF (CP_LASTKEYID_OFF+CP_LASTKEYID_LEN)
#define CP_CHANGESTAMP1_LEN  LONGSIZE	

/* Stamp 2 of last change */
#define CP_CHANGESTAMP2_OFF (CP_CHANGESTAMP1_OFF+CP_CHANGESTAMP1_LEN)
#define CP_CHANGESTAMP2_LEN  LONGSIZE	

/* .ind file free list */
#define CP_INDFREELIST_OFF (CP_CHANGESTAMP2_OFF+CP_CHANGESTAMP2_LEN)
#define CP_INDFREELIST_LEN  BLKNOSIZE

/* number of keys */
#define	CP_NKEYS_OFF	(CP_INDFREELIST_OFF+CP_INDFREELIST_LEN)
#define CP_NKEYS_LEN	SHORTSIZE

/* use 4 bytes of reserved space */
#define CP_VAREND_OFF   (CP_NKEYS_OFF+CP_NKEYS_LEN)
#define CP_VAREND_LEN	LONGSIZE

/* use 4 bytes of reserved space for time stamp*/
#define CP_UNIQUE_OFF   (CP_VAREND_OFF+CP_VAREND_LEN)
#define CP_UNIQUE_LEN	LONGSIZE

/* reserve some space for future */
#define CP_RESERVE_OFF	(CP_UNIQUE_OFF+CP_UNIQUE_LEN)
#define CP_RESERVE_LEN   120		     /* 128 was reserved originally */

/* table of key descriptors */
#define CP_KEYS_OFF	(CP_RESERVE_OFF+CP_RESERVE_LEN)

#define MAXNKEYS2 ((ISCNTLSIZE - CP_KEYS_OFF) / K2_LEN)
#define MAXNKEYS	17

#if (MAXNKEYS2 < MAXNKEYS)
Cause a compiler error here. There is not enought space in the control
page to hold MAXNKEYS.
#endif

/*------ page type indicator values -------------*/
#define PT_INDEX	1		     /* B-tree page */
#define PT_FREELIST	2		     /* free list */

/*-------------- B-Tree page layout -----------------------------------*/
/* page type */
#define BT_TYPE_OFF	0
#define BT_TYPE_LEN	SHORTSIZE

/* level (leaves have level 0) */
#define BT_LEVEL_OFF	(BT_TYPE_OFF+BT_TYPE_LEN)
#define BT_LEVEL_LEN	SHORTSIZE

/* page capacity */
#define BT_CAPAC_OFF	(BT_LEVEL_OFF+BT_LEVEL_LEN)
#define BT_CAPAC_LEN	SHORTSIZE

/* current number of keys */
#define BT_NKEYS_OFF	(BT_CAPAC_OFF+BT_CAPAC_LEN)
#define BT_NKEYS_LEN	SHORTSIZE

/* array of key entries */
#define BT_KEYS_OFF	(BT_NKEYS_OFF+BT_NKEYS_LEN)

/* down pointers are stored at the end of the page */

/*----------- Free List Page Layout --------------------------------------*/
/* page type */
#define FL_TYPE_OFF	0
#define FL_TYPE_LEN	SHORTSIZE

/* pointer to next block in the list */
#define FL_NEXT_OFF	(FL_TYPE_OFF + FL_TYPE_LEN)
#define FL_NEXT_LEN	BLKNOSIZE

/* number of free page pointers */
#define FL_NPOINTERS_OFF  (FL_NEXT_OFF + FL_NEXT_LEN)
#define FL_NPOINTERS_LEN  SHORTSIZE

/* free page pointers table */
#define FL_POINTERS_OFF   (FL_NPOINTERS_OFF + FL_NPOINTERS_LEN)

/* maximum number of pointers that can be stored in a page */
#define FL_MAXNPOINTERS ((ISPAGESIZE - FL_POINTERS_OFF) / BLKNOSIZE)

#define FREELIST_NOPAGE ((Blkno) -1)

/* .var file related defines */
#define VR_FRAMELEN_OFF  0
#define VR_TAILLEN_OFF   2


/* maximum and minimum values are in B-tree seaches */
#if LONG_BIT == 64
#define ISMAXLONG   { 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff }
#define ISMINLONG   { 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }
#else
#define ISMAXLONG	{ 0x7f, 0xff, 0xff, 0xff }
#define ISMINLONG	{ 0x80, 0x00, 0x00, 0x00 }
#endif

#define ISMAXSHORT	{ 0x7f, 0xff }
#define ISMINSHORT	{ 0x80, 0x00 }

#define ISMINCHAR	('\0')
#define ISMAXCHAR	('\377')

#define ISMINBIN	('\0')
#define ISMAXBIN	('\377')

/* double and float are declared using IEEE bit paterns */

#define ISMAXDOUBLE      { 0x7f, 0xf0, 0, /* rest is 0 */ }
#define ISMINDOUBLE      { 0xff, 0xf0, 0, /* rest is 0 */ }

#define ISMAXFLOAT       { 0x7f, 0x80, 0, /* rest is 0 */ }
#define ISMINFLOAT       { 0xff, 0x80, 0, /* rest is 0 */ }


/* Hide these from user till we return error code */
extern char	isstat3;
extern char	isstat4;

#define OP_BUILD		0
#define OP_OPEN			1

#endif

