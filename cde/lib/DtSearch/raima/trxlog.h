/* $XConsortium: trxlog.h /main/2 1996/05/09 04:20:43 drk $ */
/*
 *   COMPONENT_NAME: austext
 *
 *   FUNCTIONS: BITMAP_SIZE
 *		BM_BASE
 *		IX_BASE
 *		IX_SIZE
 *		PZ_BASE
 *
 *   ORIGINS: 157
 *
 */

/* ********************** EDIT HISTORY *******************************

 SCR    DATE    INI                   DESCRIPTION
----- --------- --- -----------------------------------------------------
  611 21-Feb-89 RSC The defn of BUI for unix and vms needs parenthesis
*/


/* trxlog.h  -  header file to define structures, constants, etc. for the
                memory cache overflow and transaction log file control
==========================================================================
*/
/*
   The following constants control the functioning of the cache overflow
   and transaction logging processes

   BUI             The number of bits in an unsigned int
   IX_PAGESIZE     The size (in bytes) of an index page
   IX_EPP          The number of entries that will fit on an index page
   BITMAP_SIZE     The size of the index bitmap (in unsigned int units)
   IX_SIZE         The number of index pages needed to control the db pages
   OADDR_OF_IXP    Calculates the overflow file address of an index page #
   
==========================================================================
*/
/* (BITS(unsigned int)) */
#define BUI (8*sizeof(unsigned int))

#ifndef NO_TRANS
/* ((((256*sizeof(F_ADDR))+D_BLKSZ-1) / D_BLKSZ)*D_BLKSZ) */
#define IX_PAGESIZE 1024

/* (IX_PAGESIZE / sizeof(F_ADDR)) */
#define IX_EPP 256

#define BITMAP_SIZE(pcnt) ((int)((IX_SIZE(pcnt)+(BUI-1)) / BUI))

#define IX_SIZE(pcnt) ((long)( ((pcnt) + (IX_EPP-1)) / IX_EPP))

/* Next define the base file offsets for entries in the overflow file */

#define BM_BASE( file ) ( root_ix[file].base )
#define IX_BASE(file, pcnt) ((long)(BM_BASE(file) + (BITMAP_SIZE(pcnt)*sizeof(unsigned int))))
#define PZ_BASE(file, pcnt) ((long)(IX_BASE(file, pcnt) + (IX_SIZE(pcnt)*IX_PAGESIZE)))

/*
==========================================================================
*/

/* The following typedef'ed structure defines a single entry in the
   root index data.  */

typedef struct RI_ENTRY_S {
   LONG     pg_cnt;           /* Number of pages currently in file */
   F_ADDR   base;             /* Base of data stored in overflow */
   BOOLEAN  pz_modified;      /* Was page zero written to overflow? */
   INT_P    Bitmap;           /* Used index page bitmap */
} RI_ENTRY;
#define bitmap Bitmap.ptr
#define RI_ENTRY_IOSIZE (sizeof(RI_ENTRY)-sizeof(INT_P)+sizeof(INT *))
#endif

/*
==========================================================================
*/

/* page zero table entry */
#define PGZEROSZ (2*sizeof(F_ADDR)+sizeof(ULONG))
typedef struct PGZERO_S {
   F_ADDR  pz_dchain;         /* delete chain pointer */
   F_ADDR  pz_next;           /* next available record number */
   ULONG   pz_timestamp;      /* file's timestamp value */
   BOOLEAN pz_modified;       /* TRUE if page zero has been modified */
} PGZERO;

/* binary search lookup table entry */

#ifndef NO_TRANS
/* External declarations */
extern int trlog_flag;        /* Transaction logging enabled flag */
extern int trcommit;          /* Transaction commit in progress flag */
extern BOOLEAN use_ovfl;      /* Do we use the overflow file ? */
#endif

/* Maximum number of transactions which can commit a time */
#ifndef SINGLE_USER
#define TAFLIMIT 5
#else
#define TAFLIMIT 1
#endif

#define TRXLOG_H
/* End - trxlog.h */
/* vpp -nOS2 -dUNIX -nBSD -nVANILLA_BSD -nVMS -nMEMLOCK -nWINDOWS -nFAR_ALLOC -f/usr/users/master/config/nonwin trxlog.h */
