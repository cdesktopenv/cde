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
/*%%  $XConsortium: isam.h /main/3 1995/10/23 11:33:51 rswiston $ 			 				 */
/*
@(#)isam.h
1.17 90/01/20
SMI */

/*
 * Copyright (c) 1988 by Sun Microsystems, Inc.
 */

/*
 * isam.h
 *
 * Description:
 *	isam.h is to be included in NetISAM C application programs.
 *
 * Make sure all changes in this file are reflected in lib/db/tt_client_isam.h 
 */


#ifndef _ISAM_H
#define _ISAM_H

#include <limits.h>
#ifndef LONG_BIT
#define LONG_BIT 32
#endif

extern short ldint();
extern long ldlong();
extern float ldfloat();
extern double lddbl();
extern short ldshort();
extern int ldchar(), stchar(), stlong(), stint(), stdbl(), stfloat();

extern int	iserrno;		     /* isam error number code */
extern long	isrecnum;		     /* record number of last call */
extern int	isreclen;		     /* used for variable length recs */
extern char	isstat1;		     /* cobol status characters */
extern char	isstat2;

#define CHARTYPE	0
#define CHARSIZE        1

#define INTTYPE		1
#define INTSIZE		2

#define LONGTYPE	2
#define LONGSIZE        LONG_BIT/8

#define DOUBLETYPE	3
#define DOUBLESIZE	8

#define FLOATTYPE	4
#define FLOATSIZE	4

#define BINTYPE		5
#define BINSIZE   	1

#define MINTYPE		0
#define MAXTYPE		6

#define ISDESC		0x80		     /* add to make descending type */
#define TYPEMASK	0x7f		     /* type mask */

#define BYTEMASK	0xff		     /* mask for one byte */
#define BYTESHFT	8		     /* shift for one byte */

/* mode parameter in isread() and isstart() */

#define ISFIRST		0		     /* first record */
#define ISLAST		1		     /* last record */
#define ISNEXT		2		     /* next record */
#define ISPREV		3		     /* previous record */
#define ISCURR		4		     /* current record */

#define ISEQUAL		5		     /* equal value */
#define ISGREAT		6		     /* greater value */
#define ISGTEQ		7		     /* >= value */
#define ISLESS		8		     /* < */
#define ISLTEQ		9		     /* <= */

/* isopen, isbuild lock modes */
#define ISAUTOLOCK	0x200		     /* automatic record lock */
#define ISMANULOCK	0x400		     /* manual record lock */
#define ISEXCLLOCK	0x800		     /* exclusive isam file lock */
#define ISFASTLOCK      0x4000               /* no locking when EXCLLOCK! */

/* isread lock modes */
#define ISLOCK		0x100		     /* lock record before reading */
#define ISSKIPLOCK	0x1000		     /* advance record pointer to */
					     /* locked record */
/* isstart lock mode for automatic record locking */
#define ISKEEPLOCK	0x2000		     /* keep record locked */

/* Fix/Variable length records mode */
#define ISFIXLEN	0
#define ISVARLEN	0x10000

/* Access remote file via NFS */
#define ISNFS		0x20000


#define ISINPUT		0		     /* open for input only */
#define ISOUTPUT	1		     /* open for output only */
#define ISINOUT		2		     /* open for input and output */

#define MAXKEYSIZE	150		     /* max number of bytes in key */
#define NPARTS		8		     /* max number of key parts */

/*#define ISMAXRECLEN	8192	*/	     /* Maximum number of bytes in record */
#define ISMAXRECLEN     32767                /* 32K - 1 */
#define ISMINRECLEN	   4		     /* Minimum number of bytes in record */

struct keypart {
    unsigned short 	kp_start;	     /* starting byte of key part */
    short	kp_leng;		     /* length in bytes */
    short	kp_type;		     /* type of key part */
};

struct keydesc {
    short 	k_flags;		     /* flags */
    short	k_nparts;		     /* number of parts in key */
    struct keypart k_part[NPARTS];	     /* each part */
};

/* The next three defines are for compatibility with X/OPEN */
#define k_start		k_part[0].kp_start
#define k_leng	        k_part[0].kp_leng
#define k_type		k_part[0].kp_type

#define	ISNODUPS	000		     /* no duplicates and  */
					     /* no compression */
#define ISDUPS		001		     /* duplicates allowed */
#define COMPRESS	020		     /* full compression */

struct dictinfo {
    short	di_nkeys;		     /* number of keys defined */
 unsigned short	di_recsize;		     /* data record size */
    short	di_idxsize;		     /* index record size */
    long	di_nrecords;		     /* number of records in file */
};

/* msb in di_nkeys indicates that ISAM file supports variable length records */
#define DICTVARLENBIT	0x8000

/* mask to access the true number of keys */
#define DICTNKEYSMASK      (DICTVARLENBIT - 1) 
					       

#define ENOERROR	  0		     /* No error */
#define EDUPL		100		     /* duplicate record */
#define ENOTOPEN	101		     /* file not open */
#define EBADARG		102		     /* illegal argument */
#define	EBADKEY		103		     /* illegal key desc */
#define ETOOMANY	104		     /* too many files open */
#define EBADFILE	105		     /* bad ISAM file format */
#define ENOTEXCL	106		     /* non-exclusive access */
#define ELOCKED		107		     /* record locked */
#define EKEXISTS	108		     /* key already exists */
#define EPRIMKEY	109		     /* is primary key */
#define EENDFILE	110		     /* end/begin of file */
#define ENOREC		111		     /* no record found */
#define ENOCURR		112		     /* no current record */
#define EFLOCKED	113		     /* file locked */
#define EFNAME		114		     /* file name too long */
#define EBADMEM		116		     /* cannot allocate memory */

/* NetISAM specific error codes  non XOPEN compliant*/
#define ETIMEOUT	1117		     /* RPC timeout */
#define ERPC 		1118		     /* Broken TCP/IP */
#define ETCP		1119		     /* Cannot connect to server */
#define EIMPORT		1120		     /* Cannot import */
#define ENODAEMON       1121                  /* no local daemon */
#define EFATAL          1122                  /* internal fatal error */
#define ELANG           1123                  /* Locale/LANG mismatch */


#define ISOK		0		     /* no error return code */
#define ISERROR 	-1		     /* error return code */


extern struct keydesc *nokey;		     /* May be used as parameter
					      * to isbuild() if no
					      * primary key is to be built
					      */

/* ischeckindex() diagmode values */
#define	CHK_NODIAG	0		     /* do not print any messages */
#define	CHK_DIAG	1		     /* Access Level module will print to stderr */

/* values of corrupt parameter of ischeckindex() */
#define	CHK_OK		0		     /* ISAM file is not corrupted */
#define	CHK_DAT		1		     /* .dat file is corrupted */
#define	CHK_VAR		2		     /* .var file is corrupted */
#define	CHK_IND		3		     /* .ind file is corrupted */


/* iscntl() definitions */
/* For one isfd - use iscntl(isfd, func, ...) */
#define ISCNTL_RPCS_TO_SET   1		     /* short oper. timeout (in sec) */
#define ISCNTL_RPCS_TO_GET   2		     /* short oper. timeout (in sec) */
#define ISCNTL_RPCL_TO_SET   3		     /* long oper. timeout (in sec) */
#define ISCNTL_RPCL_TO_GET   4		     /* long oper. timeout (in sec) */
#define ISCNTL_TCP_TO_SET    5		     /* timeout to reconnect TCP?IP */
#define ISCNTL_TCP_TO_GET    6		     /* timeout to reconnect TCP?IP */
#define ISCNTL_APPLMAGIC_WRITE 7	     /* Write Appl. Magic */
#define ISCNTL_APPLMAGIC_READ  8	     /* Read Appl. Magic */
#define ISCNTL_FSYNC		9	     /* Flush pages to disk */
#define ISCNTL_FDLIMIT_SET      10	     /* Set UNIX fd usage limit */
#define ISCNTL_FDLIMIT_GET      11	     /* Get UNIX fd usage limit */
#define ISCNTL_FATAL	12		     /* Specify NetISAM fatal error hadler */
#define ISCNTL_MASKSIGNALS	13	     /* Enable or Disable masking signals during NetISAM operations */

#define ISAPPLMAGICLEN		32

/*
 * ALLISFD in iscntl() calls means that the operation should be done 
 * on all open file descriptors, or the operation is not related to 
 * any file descriptor (e.g: maskintg signals)
 */
#define ALLISFD	 	(-2)	

/* isshowlocks() and isshowhostlocks() return an array of struct showlock */
#define IPADDRLEN	4		     /* IP address length */
struct showlock {
    short	dev;			     /* Major/Minor device number,
					      * actually type dev_t */
    long	inode;			     /* inode number */
    long	recnum;			     /* record number */
    char	hostid[IPADDRLEN];	     /* IP address of host */
    short	pid;			     /* UNIX process ID */
    long	uid;			     /* User ID */
    short	type;			     /* See below */
};


/* NetISAM Programmer Toolkit related definitions */
	
struct isfldmap {
        char	*flm_fldname;
	short	flm_recoff;
	short	flm_bufoff;
	short	flm_type;
	short	flm_length;
};

struct istableinfo {
	char	*tbi_tablename;
	int	tbi_reclength;
	int	tbi_nfields;
	struct isfldmap *tbi_fields;
	int	tbi_nkeys;
	struct keydesc *tbi_keys;
	char	**tbi_keynames;
};

/* record descriptor isreadm/iswritem record descriptor */
struct recorddesc {
	char	*rec_buffer;
	long	rec_number;
	short	rec_length;
	short	rec_locked;
};

/*
 * The following defines and variable definitions are not used by NetISAM 1.0.
 * They are defined for compatibility with VSX 2.5.
 */
#define EBADCOLL	(-1)

#define IO_OPEN		0x10		     /* open() */
#define IO_CREA		0x20		     /* creat() */
#define IO_SEEK		0x30		     /* lseek() */
#define IO_READ		0x40		     /* read() */
#define IO_WRIT		0x50		     /* write() */
#define IO_LOCK		0x60		     /* lockf() */
#define IO_IOCTL	0x70		     /* ioctl() */
#define IO_RENAME	0x80		     /* rename() */
#define IO_UNLINK	0x90		     /* unlink() */

extern int iserrio;


#endif /* !_ISAM_H */
