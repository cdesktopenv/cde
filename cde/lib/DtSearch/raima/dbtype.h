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
 *   FUNCTIONS: ALLOC
 *              BITS
 *              CALLOC
 *              DB_ENTER
 *              DB_REF
 *              FREE
 *              LOCK_SET
 *              MEM_LOCK
 *              MEM_UNLOCK
 *              NUM2EXT
 *              NUM2INT
 *              ORIGIN
 *              RETURN
 *              RN_REF
 *              TABLE_SIZE
 *              arraysize
 *              bytecmp
 *              bytecpy
 *              byteset
 *
 *   ORIGINS: 27,157
 *
 *   (C) COPYRIGHT International Business Machines Corp. 1995, 1996
 *   All Rights Reserved
 *   US Government Users Restricted Rights - Use, duplication or
 *   disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 */
/*----------------------------------------------------------------------------
 $TOG: dbtype.h /main/6 1998/03/19 19:00:38 mgreess $
   dbtype.h: Standard db_VISTA header file containing:
   
      - DDL dictionary type definitions 
      - Machine dependent constant definitions
      - Miscellaneous constants used by runtime functions
      - Virtual memory page table type definitions

   (An #include "vista.h" must precede this include)

   (C) Copyright 1984, 1985, 1986 by Raima Corporation.
----------------------------------------------------------------------------*/

/* ********************** EDIT HISTORY *******************************

 SCR    DATE    INI                   DESCRIPTION
----- --------- --- -----------------------------------------------------
  101 21-Jun-88 RSC Added rn_type and rn_dba to database table
  344 06-Jul-88 RSC LOGFILELEN also defined in dblock.h - add ifndef
  115 18-Jul-88 RSC Integrate VAX VMS changes
      21-Jul-88 RSC Fix bugaboo in VAX integration
  368 28-Jul-88 RSC Integrate BSD changes into code
  310 10-Aug-88 RSC Cleanup function prototype
      19-Aug-88 RSC Moved rn_type,rn_dba to separate table for compatibility
		    with db_QUERY and db_REVISE
  423 10-Sep-88 RSC Add dbpush names for netunix.c
  423 22-Sep-88 RSC Modify/simplify DB_ENTER macro for Lattice compiler
  368 23-Sep-88 RSC Change type of pid from INT to int and
		    corrected defn of MESSAGE
  423 26-Sep-88 RSC Moved INIT_PAGE_P defn to initial.c
      11-Oct-88 RSC Fix for ONE_DB compile
  441 06-Dec-88 RSC Move defn of MESSAGE from netgen.h to here
  532 06-Jan-89 RSC Fixed defn of DB_ID for ONE_DB

 $Log$
 * Revision 1.2  1995/10/13  18:29:38  miker
 * Added #define DtSrFILENMLEN to expand max filename len.
 * Changed FILENMLEN from 48 to DtSrFILENMLEN (240: 256 - 16).
 */

/* Compiler dependencies are handled with ifdef macros:
NOTE: One of these macros should be defined on the compile command,
for example
	cc -c -DUNIX fillnew.c
Failure to do so will produce compilation errors.

   UNIX    = Unix C compiler
   LAT     = Lattice
   MSC     = Microsoft
   AZ      = Aztec
   C86     = CI C86
   DS      = DeSmet
   VMS     = Vax/VMS
   HHS     = Harris H-series
*/

#ifdef NO_TRANS
#define SINGLE_USER
#endif
#ifdef SINGLE_USER
#define NO_TIMESTAMP
#endif

#define PGHOLD 1
#define NOPGHOLD 0
#define PGFREE 1
#define NOPGFREE 0

#define KEYFIND 0
#define KEYNEXT 1
#define KEYPREV 2
#define KEYFRST 3
#define KEYLAST 4

/* dictionary attribute flags */
#define SORTFLD     0x0001 /* field is a set sort field */
#define STRUCTFLD   0x0002 /* field is sub-field of struct */
#define UNSIGNEDFLD 0x0004 /* field is unsigned */
#define STATIC      0x0002 /* file/record is static */
#define LOCAL       0x0004 /* file/record is local */
#define COMKEYED    0x0010 /* record/field contains/included in compound key */
#define TIMESTAMPED 0x0001 /* record/set is timestamped */

#define TRUE 1
#define FALSE 0
#define YES 1
#define NO 0

#define BITS_PER_BYTE 8

/* Number of bits in a "type" */
#define BITS( type )  (BITS_PER_BYTE * sizeof( type ))

/* Number of elements in a vector */
#define arraysize(v) (sizeof(v)/sizeof(*(v)))

/* Number of bytes in a disk block */
#define D_BLKSZ 512     

#define DBD_COMPAT_LEN 6
#define INT_SIZE sizeof(int) 
#define SHORT_SIZE sizeof(short)
#define LONG_SIZE sizeof(long) 
#ifndef	 NO_FLOAT
#define FLOAT_SIZE sizeof(float)
#define DOUBLE_SIZE sizeof(double)
#endif
#define CHAR_SIZE sizeof(char)
#define DB_ADDR_SIZE sizeof(LONG)
#define PGHDRSIZE 4
#define CTBNAME "vista.ctb"

#define RECHDRSIZE 6
#define NONE -1

/* FILEMASK, ADDRMASK, FILESHIFT always operate on 4-byte variables */
#define FILEMASK 0x000000FF
#define ADDRMASK 0x00FFFFFF
#define FILESHIFT 24

#define OPTKEYSHIFT 10
#define OPTKEYMASK 0xFC00
#define OPTKEYNDX 0x003F
#define OPTKEY_LIMIT 63
#define RLBMASK 0x4000

/* The following constants all depend on the size of INT and DB_ADDR */

/* creation timestamp in rec hdr = sizeof(INT) + DB_ADDR_SIZE */
#define RECCRTIME 6
/* update timestamp in rec hdr  = RECCRTIME + sizeof(LONG) */
#define RECUPTIME 10

/*                           member count   prior + next     timestamp (opt) */
/* max size of set pointer = sizeof(LONG) + 2*DB_ADDR_SIZE + sizeof(ULONG)   */
#define SETPSIZE   16
/* Offset to total members in set ptr */
#define SP_MEMBERS 0  
/* Offset to first member ptr in set ptr */
#define SP_FIRST   4  
/* Offset to last member ptr in set ptr = 2+DB_ADDR_SIZE */
#define SP_LAST    8  
#ifndef	 NO_TIMESTAMP
/* Offset to timestamp in set ptr */
#define SP_UTIME   12
#endif
/* Size of member pointer = 3*DB_ADDR_SIZE */
#define MEMPSIZE   12 
/* Offset to owner ptr in member ptr */
#define MP_OWNER   0  
/* Offset to previous member ptr in member ptr = DB_ADDR_SIZE */
#define MP_PREV    4  
/* Offset to next member ptr in member ptr = 2*DB_ADDR_SIZE */
#define MP_NEXT    8  

/* maximum length of a database file name */
#define DtSrFILENMLEN	240	/* expand max filename len from 48 */
#ifndef FILENMLEN
#define FILENMLEN  DtSrFILENMLEN
#endif
#ifndef LOGFILELEN
#define LOGFILELEN 48
#endif
/* maximum length of a database name */
#define DBNMLEN 12
/* maximum length of a database path */
#define PATHLEN FILENMLEN-15

#define MAXDIMS 3

#define OPEN 'o'
#define CLOSED 'c'
#define DATA 'd'
#define KEY 'k'
#define OVERFLOW 'o'

typedef struct FILE_ENTRY_S {
   char ft_name[FILENMLEN];  /* name of file */
   INT  ft_desc;             /* file descriptor */
   char ft_status;           /* 'o'=opened, 'c'=closed */
   char ft_type;             /* 'd'=data,   'k'=key,   'o'=overflow */
   INT  ft_slots;            /* record slots per page */
   INT  ft_slsize;           /* size of record slots in bytes */
   INT  ft_pgsize;           /* size of page */
   INT  ft_flags;            /* 0x0002 is set if file is static */
} FILE_ENTRY;

typedef struct RECORD_ENTRY_S {
   INT rt_file;      /* file table entry of file containing record */
   INT rt_len;       /* total length of record */
   INT rt_data;      /* offset to start of data in record */
   INT rt_fields;    /* first field def in field_table */
   INT rt_fdtot;     /* total number of fields in record */
   INT rt_flags;     /* 0x0001 is set if record is timestamped
                        0x0002 is set if record is static */
} RECORD_ENTRY;


#define FIRST      'f'
#define LAST       'l'
#define ASCENDING  'a'
#define DESCENDING 'd'
#define NOORDER    'n'
#define NEXT       'n'
typedef struct SET_ENTRY_S {
   INT st_order;     /* 'f'=first, 'l'=last, 'a'=ascending,
                        'd'=descending, 'n'=no order */
   INT st_own_rt;    /* record table entry of owner */
   INT st_own_ptr;   /* offset to set pointers in record */
   INT st_members;   /* index of first member record in member table */
   INT st_memtot;    /* total number of members of set */
   INT st_flags;     /* 0x0001 is set if record is timestamped */
} SET_ENTRY;


typedef struct MEMBER_ENTRY_S {
   INT mt_record;    /* record table entry for this member */
   INT mt_mem_ptr;   /* offset to member ptrs in record */
   INT mt_sort_fld;  /* sort table entry of first sort field */
   INT mt_totsf;     /* total number of sort fields */
} MEMBER_ENTRY;


typedef struct SORT_ENTRY_S {
   INT se_fld;       /* field table entry of sort field */
   INT se_set;       /* set table entry of sorted set */
} SORT_ENTRY;


#define NOKEY      'n'
#define DUPLICATES 'd'
#define UNIQUE     'u'
#define CHARACTER  'c'
#define SHORTINT   's'
#define REGINT     'i'
#define LONGINT    'l'
#ifndef	 NO_FLOAT
#define FLOAT      'f'
#define DOUBLE     'F'
#endif
#define DBADDR     'd'
#define GROUPED    'g'
#define COMKEY     'k'
typedef struct FIELD_ENTRY_S {
   char fd_key;          /* 'n'=none, 'd'=duplicates, 'u'=unique */
   char fd_type;         /* 'c'=char, 's'=short, 'i'=int, 'l'=long, 'd'=DB_ADDR,
                            'f' = float, 'D' = double, 'g'=grouped field, 
		            'k' = compound key */
   INT fd_len;           /* length of field in bytes */
   INT fd_dim[MAXDIMS];  /* size of each array dimension */
   INT fd_keyfile;       /* file_table entry for key file */
   INT fd_keyno;         /* key prefix number */
   INT fd_ptr;           /* offset to field in record or 
		            1st compound key field in key_table */
   INT fd_rec;           /* record table entry of record containing field */
   INT fd_flags;         /* 0x0001 is set if field in "by" clause of set spec 
                            0x0002 is set if field member of struct field 
                            0x0004 is set if field is unsigned data type
                            0x0008 is set if field is an optional key */
} FIELD_ENTRY;

/* compound key table entry declaration */
typedef struct KEY_ENTRY_S {
   INT  kt_key;    /* compound key field number */
   INT  kt_field;  /* field number of included field */
   INT  kt_ptr;    /* offset to start of field data in key */
   INT  kt_sort;   /* 'a' = ascending, 'd' = descending */
} KEY_ENTRY;

#ifndef	 ONE_DB
/* database table entry declaration */
typedef struct DB_ENTRY_S {
   char db_name[DBNMLEN];   /* name of this database */
   char db_path[PATHLEN];   /* name of path to this database */
   DB_ADDR sysdba;  /* database address of system record */
   DB_ADDR curr_dbt_rec;/* this db's current record */
   INT  Page_size;  /* size of this db's page */
   INT  Size_ft;    /* size of this db's file_table */
   INT  ft_offset;  /* offset to start of this db's file_table entries */
   INT  Size_rt;    /* size of this db's record_table */
   INT  rt_offset;  /* offset to start of this db's record_table entries */
   INT  Size_fd;    /* size of this db's field_table */
   INT  fd_offset;  /* offset to start of this db's field_table entries */
   INT  Size_st;    /* size of this db's set_table */
   INT  st_offset;  /* offset to start of this db's set_table entries */
   INT  Size_mt;    /* size of this db's member_table */
   INT  mt_offset;  /* offset to start of this db's member_table entries */
   INT  Size_srt;   /* size of this db's sort_table */
   INT  srt_offset; /* offset to start of this db's sort_table entries */
   INT  Size_kt;    /* size of this db's key_table */
   INT  kt_offset;  /* offset to strary of this db's key table entries */
   INT  key_offset; /* key prefix offset for this db */
} DB_ENTRY;

/* Structure containing current record type & address for recfrst/set/next..*/
typedef struct RN_ENTRY_S {
   INT rn_type;	    /* Last record type supplied to recfrst/recset */
   DB_ADDR rn_dba;  /* Last db addr computed by recfrst/recset/recnext */
} RN_ENTRY;
#define	 DB_REF(item)	      	 (db_global.Curr_db_table->item)
#define  RN_REF(item)		 (curr_rn_table->item)
#define	 NUM2INT(num, offset) 	 ((num) + db_global.Curr_db_table->offset)
#define	 NUM2EXT(num, offset) 	 ((num) - db_global.Curr_db_table->offset)
#define	 ORIGIN(offset)	      	 (curr_db_table->offset)
#ifndef NO_DBN
#define	 CURR_DB_PARM	      	 , curr_db
#define	 DBN_PARM    	      	 , dbn
#define  DBN_ONLY		 dbn
#define	 DBN_DECL    	      	 int dbn;
#else
#define	 CURR_DB_PARM	      	 /**/
#define	 DBN_PARM    	      	 /**/
#define  DBN_ONLY		 /**/
#define	 DBN_DECL    	      	 /**/
#endif
#else
#define	 DB_REF(item)     	 (db_global.item)
#define  RN_REF(item)		 (db_global.item)
#define	 NUM2INT(num, offset) 	 (num)
#define	 NUM2EXT(num, offset) 	 (num)
#define	 ORIGIN(offset)	      	 (0)
#define	 CURR_DB_PARM	      	 /**/
#define	 DBN_PARM    	      	 /**/
#define  DBN_ONLY		 /**/
#define	 DBN_DECL    	      	 /**/
#endif
#define	 TABLE_SIZE(size)     	 DB_REF(size)

/* DBN_PARM... and TASK_PARM... are used for function interfaces */
#ifdef MULTI_TASK
#define  TASK_PARM		 , task
#define  TASK_PARM_DBN		 task ,
#define  TASK_ONLY		 task
#define  TASK_DECL		 DB_TASK task;
#define  TASK_PTR_DECL		 DB_TASK FAR *task;
#define  TASK_DBN_ONLY		 task DBN_PARM
#else
#define  TASK_PARM		 /**/
#define  TASK_PARM_DBN		 /**/
#define  TASK_ONLY		 /**/
#define  TASK_DECL		 /**/
#define  TASK_PTR_DECL		 /**/
#define  TASK_DBN_ONLY		 DBN_ONLY
#endif

#ifndef NO_COUNTRY
/* Country code table definition */
typedef struct CNTRY_TBL_S {
   unsigned char out_chr;
   unsigned char sort_as1;
   unsigned char sort_as2;
   unsigned char sub_sort;
} CNTRY_TBL;
#endif

/* names lists structure declaration */
typedef struct OBJNAMES_S {
    char **recs;
    char **flds;
    char **sets;
} OBJNAMES;


/* directory separator character:
   MS-DOS: '\'
   UNIX:   '/'
*/
#ifdef HHS
#define DIRCHAR '*'
#else
#define DIRCHAR '/'
#endif

/* The following define aid in the use of a set of routines which use RMS
   directly to do the i/o to database files. */

#define DB_OPEN open_b
#define DB_CLOSE close
#define DB_LSEEK lseek
#define DB_WRITE write
#define DB_READ  read

typedef struct {FILE_NO FAR *ptr; LOCK_DESC} FILE_NO_P;
typedef struct {int FAR *ptr; LOCK_DESC} INT_P;
typedef struct {struct NODE_PATH_S FAR *ptr; LOCK_DESC} NODE_PATH_P;
#ifndef SINGLE_USER
typedef struct {struct lock_descr FAR *ptr; LOCK_DESC} LOCK_DESCR_P;
typedef struct {struct LM_LOCK_S FAR *ptr; LOCK_DESC} LM_LOCK_P;
typedef struct {struct LM_FREE_S FAR *ptr; LOCK_DESC} LM_FREE_P;
typedef struct {struct LM_DBOPEN_S FAR *ptr; LOCK_DESC} LM_DBOPEN_P;
typedef struct {struct LM_DBCLOSE_S FAR *ptr; LOCK_DESC} LM_DBCLOSE_P;
typedef struct {struct LR_DBOPEN_S FAR *ptr; LOCK_DESC} LR_DBOPEN_P;
#endif
typedef struct {struct KEY_INFO_S FAR *ptr; LOCK_DESC} KEY_INFO_P;
typedef struct {struct ren_entry FAR *ptr; LOCK_DESC} REN_ENTRY_P;
typedef struct {struct RN_ENTRY_S FAR *ptr; LOCK_DESC} RN_ENTRY_P;
typedef struct {struct LOOKUP_ENTRY_S FAR *ptr; LOCK_DESC} LOOKUP_ENTRY_P;
typedef struct {struct page_entry FAR *ptr; LOCK_DESC} PAGE_ENTRY_P;
typedef struct {struct DB_ENTRY_S FAR *ptr; LOCK_DESC} DB_ENTRY_P;
typedef struct {struct FILE_ENTRY_S FAR *ptr; LOCK_DESC} FILE_ENTRY_P;
typedef struct {struct RECORD_ENTRY_S FAR *ptr; LOCK_DESC} RECORD_ENTRY_P;
typedef struct {struct SET_ENTRY_S FAR *ptr; LOCK_DESC} SET_ENTRY_P;
typedef struct {struct MEMBER_ENTRY_S FAR *ptr; LOCK_DESC} MEMBER_ENTRY_P;
typedef struct {struct SORT_ENTRY_S FAR *ptr; LOCK_DESC} SORT_ENTRY_P;
typedef struct {struct FIELD_ENTRY_S FAR *ptr; LOCK_DESC} FIELD_ENTRY_P;
typedef struct {struct KEY_ENTRY_S FAR *ptr; LOCK_DESC} KEY_ENTRY_P;
typedef struct {struct PGZERO_S FAR *ptr; LOCK_DESC} PGZERO_P;
typedef struct {struct RI_ENTRY_S FAR *ptr; LOCK_DESC} RI_ENTRY_P;
typedef struct {ULONG FAR *ptr; LOCK_DESC} ULONG_P;
typedef struct {LONG FAR *ptr; LOCK_DESC} LONG_P;
#ifndef NO_COUNTRY
typedef struct {struct CNTRY_TBL_S FAR *ptr; LOCK_DESC} CNTRY_TBL_P;
#endif
struct sk {
   INT sk_fld;
   CHAR_P sk_val;
} __SK__;
typedef struct { struct sk FAR *ptr; LOCK_DESC } SK_P;

/* node key search path stack entry: one per level per key field */
typedef struct NODE_PATH_S {
   F_ADDR node;       /* node (page) number  */
   INT    slot;       /* slot number of key */
} NODE_PATH;

/* index key information: one entry per key field */
typedef struct KEY_INFO_S {
   NODE_PATH_P Node_path; /* stack of node #s in search path */
   INT         level;     /* current level # in node path */
   INT         max_lvls;  /* maximum possible levels for key */
   INT         lstat;     /* last key function status */
   INT         fldno;     /* field number of key */
   FILE_NO     keyfile;   /* key file containing this key */
   CHAR_P      Keyval;    /* ptr to last key value */
   DB_ADDR     dba;       /* db address of last key */
} KEY_INFO;
#define node_path Node_path.ptr
#define keyval Keyval.ptr

/* key slot structure */
typedef struct key_slot {
   F_ADDR  child;         /* child node pointer */
   INT     keyno;         /* key number */
   char    data[1];       /* start of key data */
} KEY_SLOT;

typedef union key_type {
   char kd[256];
   struct key_slot ks;
} KEY_TYPE;

/* file rename table entry declarations */
typedef struct ren_entry {
   CHAR_P Ren_db_name;
   CHAR_P File_name;
   FILE_NO file_no;
} REN_ENTRY;

#define bytecpy(s1, s2, n) memcpy((char *)(s1), (char *)(s2), (unsigned)(n))
#define bytecmp(s1, s2, n) memcmp((char *)(s1), (char *)(s2), (unsigned)(n))
#define byteset(s, c, n) memset((char *)(s), (char)(c), (unsigned)(n))

typedef struct MESSAGE_S {
#ifdef GENERAL
    INT mtype;
    char mtext[1];
#else				/* GENERAL */
    LONG mtype;
    int  pid;
    char mtext[1];
#endif				/* GENERAL */
} MESSAGE;

#ifdef ONE_DB
#define NO_DBN
#endif

#ifndef NO_DBN			   /* NO_DBN */
#ifdef COMMA				 /* COMMA */
#define DB_ID , dbn
#define NO_DB_ID , -1
#define DBN_PARAM , b
#else				 	 /* COMMA */
#define DB_ID dbn
#define NO_DB_ID -1
#define DBN_PARAM b
#define COMMA
#endif				 	 /* COMMA */
#else                              /* NO_DBN */
#define DBN_PARAM /**/
#define DB_ID /**/
#define DB_PARM /**/
#define NO_DB_ID /**/
#endif				   /* NO_DBN */

#ifdef MULTI_TASK
#ifdef COMMA
#define TASK_ID , &task
#define TASK_PTR_ID , task
#define NO_TASK_ID , NULL
#define TASK_PARAM , c
#else
#define TASK_ID &task
#define TASK_PTR_ID task
#define NO_TASK_ID NULL
#define TASK_PARAM c
#define COMMA
#endif
#else
#define TASK_PARAM /**/
#define TASK_ID /**/
#define TASK_PTR_ID /**/
#define NO_TASK_ID /**/
#endif

#define LOCK_PARAM /**/
#define LOCK_PARM /**/
#define LOCK_DECL /**/
#define LOCK_SET(a) /**/

#define TDTL_PARAM DBN_PARAM TASK_PARAM LOCK_PARAM

#define DB_ENTER(TDTL_PARAM) if ( startup(TDTL_PARAM) != S_OKAY ) { return( db_status ); } else

#define RETURN(x) return( x )
#define ALLOC(l, b, v) d_alloc(b)
#define CALLOC(l, i, s, v) d_calloc(i, s)
#define MEM_LOCK(cp)	d_memlock((CHAR_P FAR *)(cp))
#define MEM_UNLOCK(cp) 	d_memunlock((CHAR_P FAR *)(cp))
#define FREE(cp)	d_free((CHAR_P FAR *)(cp))

/* Lock sets are defined in dblfcns.c */
#define LOCK_NONE	-1
#define LOCK_ALL	0
#define LOCK_IO		1
#define LOCK_NOIO	2
#define RECORD_IO	3
#define RECORD_NOIO	4
#define SET_IO		5
#define SET_NOIO	6

#ifdef GENERAL
#define TIMEOUT_DEF	20
#else
#define TIMEOUT_DEF	10
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include "trxlog.h"
#include "dblock.h"
#include "ll.h"
#include "dbxtrn.h"

typedef struct LOOKUP_ENTRY_S {
#ifdef MULTI_TASK
   TASK FAR *task;	      /* task pointer */
#endif
   FILE_NO file;              /* file table entry number (0..size_ft-1) */
   F_ADDR  pageno;            /* database page number */
   short   pg_slot;	      /* page table slot pointing to this entry */
} LOOKUP_ENTRY;

/* virtual page table entry */
typedef struct page_entry {
   BOOLEAN recently_used;     /* LRU flag */
   BOOLEAN modified;          /* TRUE if page has been modified */
   INT     holdcnt;           /* "hold-in-cache" counter */
   short   lu_slot;           /* lookup table slot pointing to this entry */
#ifndef NO_TRANS
   F_ADDR  ovfl_addr;         /* overflow file address of page */
#endif
   CHAR_P Buff;             /* page buffer pointer */
} PAGE_ENTRY;
#define buff Buff.ptr

#include <string.h>
#ifndef _STRING_H
#define _STRING_H
#endif
#include "proto.h"
#ifdef MULTI_TASK
#ifdef NO_DT_COVER
#include "dtcover.h"
#endif
#endif

#ifndef SINGLE_USER
/* record/set lock descriptor */
struct lock_descr {  
   FILE_NO_P fl_list;      /* array of files used by record/set */
   int	     fl_cnt;    /* Number of elements in fl_list */
   BOOLEAN   fl_kept;   /* Is lock kept after transaction? */
   char      fl_type;   /* lock type: 'r' = read, 'w' = write, 
				     'x' = exclusive, 'f' = free */ 
   char      fl_prev;   /* previous lock type */
};
#endif
/* vpp -nOS2 -dUNIX -nBSD -nVANILLA_BSD -nVMS -nMEMLOCK -nWINDOWS -nFAR_ALLOC dbtype.h */
