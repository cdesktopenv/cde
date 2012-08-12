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
 *   FUNCTIONS: P1
 *              POINTER_ASSIGN
 *              POINTER_INIT
 *              Pi
 *              Piv
 *              Pv
 *              int
 *
 *   ORIGINS: 27,157
 *
 *   (C) COPYRIGHT International Business Machines Corp. 1995, 1996
 *   All Rights Reserved
 *   US Government Users Restricted Rights - Use, duplication or
 *   disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 */
/*----------------------------------------------------------------------
  $TOG: vista.h /main/7 1998/04/03 17:10:52 mgreess $
  vista.h: db_VISTA standard header file

   This file should be included in all programs which use db_VISTA.  It
   contains database status/error code definitions and standard type
   definitions.

   This file should only include user-oriented constructs.

   (C) Copyright 1984, 1985, 1986, 1987 by Raima Corp.

----------------------------------------------------------------------*/

/* ********************** EDIT HISTORY *******************************

 SCR    DATE    INI                   DESCRIPTION
----- --------- --- -----------------------------------------------------
      03-AUG-88 RTK Multi-tasking support - see "#ifdef MULTI_TASK"
      08-AUG-88 RTK Declared globals rn_dba, rn_type, and ft_offset to
		    support recfrst, recnext, etc in ONE_DB mode.
      16-Sep-88 RSC Fixed DBN_Dx to use variable # of arg macros.
      28-Sep-88 WLW Corrected the db_VERSION to 3.10 from 3.01.
      11-Oct-88 RSC Fixed DBN_Dx to use variable # of arg macros (again)
      11-Oct-88 RSC Fixed ONE_DB to compile correctly
      17-Oct-88 RSC Replaced MSC with ANSI, moved compiler check to dbtype
  439 17-Nov-88 RSC Placed some multi-tasking stuff within MULTI_TASK
  441 06-Dec-88 RSC Modifications to make gen lockmgr work with 3.10
      26-Jan-89 RSC Add defn for UINT (for lockmgr)
  420 13-Feb-89 WLW Removed unused prototypes of {Make,Free}ProcInstance
 *
 * $Log$
 * Revision 1.2  1995/10/17  18:33:01  miker
 * Added global db_oflag, changed values of db_VERSION and dbd_VERSION:
 * - Allow access to read-only databases.
 * - Expand maximum file names from 48 to 240 chars (DtSrFILENMLEN).
 * - Disable transaction processing (NO_TRANS) to obviate need
 *   for vista.taf and vista.log files.
 * - Disregard all environment variables.
 * - Replace default dberr.c with one that doesn't read stdin.
 * - Rename library from libvista to prevent confusion with old code.
 * - Prefix all vista utility names with "dt...", also to deconfuse.
 *
 */
#if defined(linux)
# define __SVR4_I386_ABI_L1__
#endif
#include <limits.h>	/* pickup WORD_BIT, LONG_BIT */
#if defined(linux)
# undef __SVR4_I386_ABI_L1__
# ifndef WORD_BIT
# define WORD_BIT 32
# endif
# ifndef LONG_BIT
# define LONG_BIT 32
# endif
#endif

#define db_VERSION "3.dt"	/* was "3.10" */
#define dbd_VERSION "V3.dt\032"	/* was "V3.00\032" */

/* -------------------status codes-------------------- */
/* user errors */
#define S_DBOPEN    -1    /* database not opened */
#define S_INVSET    -2    /* invalid set */
#define S_INVREC    -3    /* invalid record */
#define S_INVDB     -4    /* invalid database */
#define S_INVFLD    -5    /* invalid field name */
#define S_INVADDR   -6    /* invalid db_address */
#define S_NOCR      -7    /* no current record */
#define S_NOCO      -8    /* set has no current owner */
#define S_NOCM      -9    /* set has no current member */
#define S_KEYREQD   -10   /* key value required */
#define S_BADTYPE   -11   /* invalid lock type */
#define S_HASMEM    -12   /* record is owner of non-empty set(s) */
#define S_ISMEM     -13   /* record is member of set(s) */
#define S_ISOWNED   -14   /* member already owned */
#define S_ISCOMKEY  -15   /* field is a compound key */
#define S_NOTCON    -16   /* record not connected to set */
#define S_NOTKEY    -17   /* field is not a valid key */
#define S_INVOWN    -18   /* record not legal owner of set */
#define S_INVMEM    -19   /* record not legal member of set */
#define S_SETPAGES  -20   /* error in d_setpages (database open or bad param) */
#define S_INCOMPAT  -21   /* incompatible dictionary file */
#define S_DELSYS    -22   /* illegal attempt to delete system record */
#define S_NOTFREE   -23   /* attempt to locked previously locked rec or set */
#define S_NOTLOCKED -24   /* attempt to access unlocked record or set */
#define S_TRANSID   -25   /* transaction id not be supplied */
#define S_TRACTIVE  -26   /* transaction already active */
#define S_TRNOTACT  -27   /* transaction not currently active */
#define S_TRLOCKS   -28   /* transaction cannot begin due to locked files */
#define S_TRFREE    -29   /* attempt to free a lock inside a transaction */
#define S_TRCHANGES -30   /* too many pages changed within transaction */
#define S_NOTRANS   -31   /* attempted update outside of transaction */
#define S_EXCLUSIVE -32   /* functions requires exclusive db access */
#define S_STATIC    -33   /* Attempted to write lock a static file */
#define S_USERID    -34   /* No user id exists */
#define S_NAMELEN   -35   /* database file/path name too long */
#define S_RENAME    -36   /* invalid file number was passed to d_renfile */
#define S_NOTOPTKEY -37   /* field is not an optional key */
#define S_BADFIELD  -38   /* field not defined in current record type */
#define S_COMKEY    -39   /* record/field has/in a compound key */
#define S_INVNUM    -40   /* invalid record or set number */
#define S_TIMESTAMP -41   /* record or set not timestamped */
#define S_BADUSERID -42   /* invalid user id - not alphanumeric */
#define S_NONETBIOS -43   /* NetBIOS is not installed on machine */
#define S_BADBDPATH -44	  /* bad # of elements in DBDPATH */
#define S_BADBFPATH -45   /* bad # of elements in DBFPATH */
#define S_NOTYPE    -46	  /* No current record type */
#define S_INVSORT   -47	  /* Invalid country table sort string */
#define S_DBCLOSE   -48   /* database not closed */

/* system errors */
#define S_NOSPACE   -900  /* no more space on file */
#define S_SYSERR    -901  /* system error */
#define S_FAULT     -902  /* page fault -- changed during usage */
#define S_NOWORK    -903  /* no working file set in dio */
#define S_NOMEMORY  -904  /* unable to allocate sufficient memory */
#define S_NOFILE    -905  /* unable to locate a file */
#define S_DBLACCESS -906  /* unable to access db lock file */
#define S_DBLERR    -907  /* db lock file open/access error */
#define S_BADLOCKS  -908  /* inconsistent database locks */
#define S_RECLIMIT  -909  /* file record limit reached */
#define S_KEYERR    -910  /* key file inconsistency detected */
#define S_USERLIMIT -911  /* Max concurrent user limit reached */
#define S_FSEEK     -912  /* Bad seek on database file */
#define S_INVFILE   -913  /* Invalid file specified */
#define S_BADREAD   -914  /* Bad read on database/overflow file */
#define S_NETSYNC   -915  /* Network synchronization error */
#define S_DEBUG     -916  /* Debugging check interrupt */
#define S_NETERR    -917  /* Network communications error */
#define S_RECOVERY  -918  /* Auto-recovery is in process */
#define S_BADWRITE  -919  /* Bad write on database/overflow file */
#define S_NOLOCKMGR -920  /* Unable to open lockmgr session */
#define S_DUPUSERID -921  /* DBUSERID is already used by another user */
#define S_LMBUSY    -922  /* The lock manager is busy */

/* function statuses */
#define S_OKAY      0     /* normal return, okay */
#define S_EOS       1     /* end of set */
#define S_NOTFOUND  2     /* record not found */
#define S_DUPLICATE 3     /* duplicate key */
#define S_KEYSEQ    4     /* field type used out of sequence in d_keynext */
#define S_UNAVAIL   5     /* database file currently unavailable */
#define S_DELETED   6     /* record/set deleted since last accessed */
#define S_UPDATED   7     /* record/set updated since last accessed */
#define S_LOCKED    8     /* current record's lock bit is set */
#define S_UNLOCKED  9     /* current record's lock bit is clear */

/* In order to share databases among heterogenous machines,
 * certain integer types must be defined that are invariably
 * 2 and 4 bytes on all architectures.
 * Note WORD_BIT == 32 does not guarantee short == 16.
 */

#ifdef LONG
#undef LONG
#endif

#if   (WORD_BIT == 16)
  typedef int             INT;
  typedef unsigned int    UINT;
#elif (WORD_BIT == 32)
  typedef short           INT;
  typedef unsigned short  UINT;
#else
#error Unable to typedef INT and UINT
#endif

#if (LONG_BIT == 32)
  typedef long            LONG;
  typedef unsigned long   ULONG;
#elif   (WORD_BIT == 32)
  typedef int             LONG;
  typedef unsigned int    ULONG;
#else
#error Unable to typedef LONG and ULONG
#endif

typedef LONG	BOOLEAN;

#define MAXRECORDS 16777215
#define NULL_DBA 0

/* db_VISTA database address */
#define DB_ADDR LONG  

/* db_VISTA file number */
#define FILE_NO INT   

/* file address: page or record number */
#define F_ADDR LONG   

/* record number indicator */
#define RECMARK 10000
/* field number indicator = rec * FLDMARK + fld_in_rec */
#define FLDMARK 1000
/* set number indicator - must be greater than RECMARK */
#define SETMARK 20000

/* runtime option flags */
#define DCHAINUSE  0x01
#define TRLOGGING  0x02
#define ARCLOGGING 0x04
#define IGNORECASE 0x08

/* grouped lock request packet */
extern int db_status;
#ifndef NO_TRANS
extern int rlb_status;
#endif

/* Mike Russell's custom open() flag.  Default is O_RDWR.
 * Change to O_RDONLY prior to d_open() if not updating
 * the databases, if their permissions do not allow
 * writing, if they're on cd-roms, etc.
 * Not used internally for open functions that are known
 * to require write permissions (eg initializations),
 * or known to be read-only.
 */
extern int db_oflag;

/* Use the 'const' keyword unless told otherwise */
#ifndef NO_CONST
#define CONST const
#else
#define CONST /**/
#endif

#ifndef GENERAL			/* GENERAL lockmgr */
#ifdef MSC			/* MSC */
#ifndef NO_EXT_KEYWORDS		   /* NO_EXT_KEYWORDS */
#ifndef PASCAL			      /* PASCAL */
#define PASCAL pascal
#endif			      	      /* PASCAL */
#ifndef CDECL			      /* CDECL */
#define CDECL cdecl
#endif			              /* CDECL */
#endif			           /* NO_EXT_KEYWORDS */
#endif		   		/* MSC */
#endif				/* GENERAL lockmgr */

#ifndef FAR
#define FAR /**/
#endif

#ifndef PASCAL
#define PASCAL /**/
#endif

#ifndef CDECL
#define CDECL /**/
#endif

#define EXTERNAL_FIXED    FAR PASCAL
#define EXTERNAL_VARIABLE FAR CDECL
#define INTERNAL_FIXED    PASCAL
#define INTERNAL_VARIABLE CDECL

typedef int (FAR PASCAL *FARPROC)();
typedef char *HANDLE;

#ifdef NO_DBN_PARM
#ifdef DBN_IN_LIB
#define EXTERNAL_DBN EXTERNAL_VARIABLE
#else
#define EXTERNAL_DBN EXTERNAL_FIXED
#endif
#else
#define EXTERNAL_DBN EXTERNAL_VARIABLE
#endif

#define LOCK_DESC /**/
#define POINTER_INIT()		{  NULL  }
#define POINTER_ASSIGN(a)	{  a  }

typedef struct {char FAR *ptr; LOCK_DESC} CHAR_P;
typedef struct {DB_ADDR FAR *ptr; LOCK_DESC} DB_ADDR_P;
#ifdef MULTI_TASK
typedef struct {struct TASK_S FAR *ptr; LOCK_DESC} TASK_P;
typedef struct {char FAR *ptr; LOCK_DESC} QFAKE_P;
typedef struct {
   TASK_P v;
   QFAKE_P q;
} DB_TASK;
#endif


/* Allow for function prototyping */
#ifdef LINT_ARGS
#define P0	void	/* parameterless function */
#define P1(t)	t	/* first (or only) parameter in function */
#define Pi(t)	,t	/* subsequent (2,3,...) parameter in function */
#ifdef ANSI
#define Piv(t)	,...	/* begining of variable number of parameters */
#define Pv(t)	/**/	/* function has 0 or more parameters */
#else
#define Piv(t)	,	/* [XENIX] begining of variable number of parameters */
#define Pv(t)	,	/* [XENIX] function has 0 or more parameters */
#endif
#else
#define P0	/**/
#define P1(t)	/**/
#define Pi(t)	/**/
#define Piv(t)	/**/
#define Pv(t)	/**/
#endif

#ifdef  ONE_DB
#define NO_DBN
#endif

#ifdef NO_DBN_PARM 
#define NO_DBN
#endif

#ifndef NO_DBN
#define DBN_D1 Pv(int)
#define DBN_FIX_D1 P1(int)
#define DBN_Dn Piv(int)
#else
#define DBN_D1 P0
#define DBN_FIX_D1 /**/
#define DBN_Dn /**/
#endif

#ifdef MULTI_TASK
#define TASK_DBN_D1 P1(DB_TASK) DBN_Dn
#define TASK_D1 P1(DB_TASK)
#define TASK_Di Pi(DB_TASK)
#define TASK_PTR_D1 P1(DB_TASK FAR *)
#define TASK_PTR_Di Pi(DB_TASK FAR *)
#define CURRTASK_PARM            , Currtask
#define CURRTASK_ONLY            Currtask
#else
#define TASK_DBN_D1 DBN_D1
#define TASK_D1 P0
#define TASK_Di /**/
#define TASK_PTR_D1 P0
#define TASK_PTR_Di /**/
#define CURRTASK_PARM            /**/
#define CURRTASK_ONLY            /**/
#endif

#define LOCK_D1 /**/
#define LOCK_Di /**/

typedef struct {
   int  item;  /* record or set number */
   char type;  /* lock type: 'r', 'w', 'x', 'k' */
} LOCK_REQUEST;


#include "dproto.h"
#ifdef MULTI_TASK
#include "task.h"
#endif

/* vpp -nOS2 -dUNIX -nBSD -nVANILLA_BSD -nVMS -nMEMLOCK -nWINDOWS -nFAR_ALLOC vista.h */
