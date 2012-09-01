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
 *   FUNCTIONS: EXCL_OPEN
 *              Pi
 *              cache_init
 *              clear_cache
 *              d_setfiles
 *              d_setpages
 *              dio_clear
 *              dio_close
 *              dio_clrfile
 *              dio_findpg
 *              dio_flush
 *              dio_free
 *              dio_get
 *              dio_in
 *              dio_init
 *              dio_open
 *              dio_out
 *              dio_pzalloc
 *              dio_pzclr
 *              dio_pzdel
 *              dio_pzflush
 *              dio_pzgetts
 *              dio_pzinit
 *              dio_pznext
 *              dio_pzread
 *              dio_pzsetts
 *              dio_read
 *              dio_release
 *              dio_rrlb
 *              dio_setdef
 *              dio_touch
 *              dio_write
 *              dio_wrlb
 *
 *   ORIGINS: 27, 157
 *
 *   This module contains IBM CONFIDENTIAL code. -- (IBM
 *   Confidential Restricted when combined with the aggregated
 *   modules for this product)
 *
 *   OBJECT CODE ONLY SOURCE MATERIALS
 *   (C) COPYRIGHT International Business Machines Corp. 1995, 1996
 *   All Rights Reserved
 *   US Government Users Restricted Rights - Use, duplication or
 *   disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 */
/*------------------------------------------------------------------------
 $XConsortium: dio.c /main/7 1996/11/25 18:48:24 drk $
   dio - Database Input/Output Control Functions

   Copyright (C) 1984, 1985, 1986, 1987 by Raima Corporation
------------------------------------------------------------------------*/

/* ********************** EDIT HISTORY *******************************

 SCR    DATE    INI                   DESCRIPTION
----- --------- --- -----------------------------------------------------
   76 16-JUN-88 RSC Clean up so SINGLE_USER produces good code
  240 20-Jun-88 RSC Clean up so NO_TRANS produces good code
  103 24-Jun-88 RSC Improve generation of single user version
  295 01-Jul-88 RSC make cnt_open_files global, to support initial.c mods
  333 05-Jul-88 RSC make wrlb only fiddle with rlb, not entire rid
  115 18-Jul-88 RSC Integrate VAX VMS changes into master code
  366 25-Jul-88 RSC dio_pzread should use dio_open, not DB_OPEN
      04-Aug-88 RTK MULTI_TASK changes
      11-Aug-88 RTK d_setpages and d_setfiles must return immediately if
		    another task already has opened a database.
  115 16-Aug-88 RSC Fixup of VMS integration
  423 10-Sep-88 RSC Fixup of multi-task compile problems
  420 15-Sep-88 RTK Encompassed calls to dio_unlock within an ifdef MEMLOCK
  423 15-Sep-88 RSC Also cleared last_dblu in clear_cache
  423 15-Sep-88 RSC Removed '#' from ifdef two lines up - vpp no like!!!!!
  420 16-Sep-88 RTK A couple missing FAR's
  425 05-Oct-88 RSC d_trabort wasn't completely clearing page zero, added
		    dio_pzclr to support d_trabort
  433 31-Oct-88 RSC Fix for SCR #423 wasn't quite right - query wouldn't run
  532 06-Jan-89 RSC Fix code so NO_TRANS compiles correctly
      08-Feb-89 RSC Fix from AMCY - clear cache wasn't completely swapping.
  420 13-Feb-89 WLW Cleared last_dblu in clear_cache (only safe thing to do)
      14-Feb-89 RSC Misc fixes
  588 16-Feb-89 RSC remove ifndef SINGLE_USER around CLOSE_FILES
  612 21-Feb-89 RSC always clear ovfl_addr in dbpg_table
  619 09-Mar-89 WLW call o_fileinit from dio_pzgetts, don't call dio_pzsetts
		    from dio_pzread.
      05-May-89 WLW Added ".v" tag to Currtask for MULTI_TASKing

 $Log$
 * Revision 1.2  1995/10/17  19:15:37  miker
 * Changed open mode from hardcoded O_RDWR to global var db_oflag.
 *
*/

#include <stdio.h>
#include <fcntl.h>
#include "vista.h"
#include "dbtype.h"
#include "dbswab.h"

#define DEBUG_DIO
#ifdef DEBUG_DIO
int	debugging_dio_init = FALSE;
int	debugging_dio_close = FALSE;
#endif

#ifndef NO_TRANS

/* On MS-DOS networks, files must be closed whenever a lock is freed.
   Function dio_clrfile is called whenever a lock is freed to clear
   from the cache the pages of the file whose lock is being freed.
   CLosing and opening files on Unix, VMS and other host computers, 
   however, is very slow and is not necessary for database integrity.
   The following constant definition specifies whether or not the files
   need to be closed.   A definition per supported MS-DOS compiler is 
   required.
*/
#ifdef MSC
#define CLOSE_FILES
#endif
#ifdef LAT
#define CLOSE_FILES
#endif
#ifdef WIZ
#define CLOSE_FILES
#endif
#ifdef TURBO
#define CLOSE_FILES
#endif
/*------------ transaction logging data ------------*/
#define DEFIXPAGES 4          /* default number of index cache pages */
#define MINIXPAGES 2          /* minimum number of index cache pages */
int ix_pgtab_sz = DEFIXPAGES;
LOOKUP_ENTRY_P Ix_lookup = POINTER_INIT(); /* index page lookup table */
PAGE_ENTRY_P Ixpg_table = POINTER_INIT(); /* index page table */
static int ixpg_lru_slot;     /* least recently accessed ix page */

/* transaction logging enabled flag */
int trlog_flag = 0;  /* set only by user implemented functions */

BOOLEAN use_ovfl = YES;        /* Default to using overflow */
CHAR_P Dbpgbuff = POINTER_INIT();  /* allocated by dio_init used by o_update */
/*------------ end of transaction logging data ------------*/
#endif		/* NO_TRANS */

#ifndef SINGLE_USER
#define EXCL_OPEN()	(dbopen >= 2)
#else
#define EXCL_OPEN()	(TRUE)
#endif

#define DEFDBPAGES 16         /* default number of database cache pages */
#define MINDBPAGES 8          /* minimum number of database cache pages */

extern BOOLEAN trcommit;

int db_pgtab_sz = DEFDBPAGES;

LOOKUP_ENTRY_P Db_lookup = POINTER_INIT(); /* database page lookup table */
PAGE_ENTRY_P Dbpg_table = POINTER_INIT(); /* database page table */
static struct
{
#ifdef MULTI_TASK
   TASK FAR *task;
#endif
   FILE_NO file;
   F_ADDR pageno;
   int slot;
} last_dblu;                  /* last found lookup entry in cache */

/* maximum number of open files allowed by
   operating system (user settable) */
/* On VMS systems, max_open_files need to be defined with a globaldef
   instead of just a normal external.  This will force this module to be
   included, thus giving a default value to max_open_files.  If it were
   just a normal external the VMS linker would not load this module unless
   a reference to a function in it is made and it would leave max_open_files
   with a value of 0. */
int max_open_files = 8; 
INT_P Used_files = POINTER_INIT(); /* LRU file table */
int cnt_open_files = 0;    /* count of currently open files */
static int last_file = 0;     /* least recently used file */

static int dbpg_lru_slot;      /* least recently accessed db page */
static int no_modheld;         /* number of modified or held db pages */
static FILE_NO working_file;   /* current key file being processed */

static void cache_init(P1(int) Pi(LOOKUP_ENTRY FAR *)
				      Pi(PAGE_ENTRY FAR *) Pi(int));
static int dio_pzinit(P0);
static int clear_cache(P1(FILE_NO) Pi(FILE_NO));
static int dio_pzflush(P0);
#ifdef NO_TRANS
static int dio_in(P1(PAGE_ENTRY FAR *) Pi(LOOKUP_ENTRY FAR *));
#else
static int dio_in(P1(PAGE_ENTRY FAR *) Pi(LOOKUP_ENTRY FAR *)
				 Pi(BOOLEAN));
#endif

#define used_files Used_files.ptr
#define db_lookup Db_lookup.ptr
#define dbpg_table Dbpg_table.ptr



/* Set the maximum number of open db_VISTA files
*/
int
d_setfiles(num)
int num;
{
   if ( dbpg_table ) return( dberr(S_DBCLOSE) );

   if ( num > 0 && num < 256 )
      max_open_files = num;

   return( db_status = S_OKAY );
}


/* Set number of virtual memory pages
*/
int
d_setpages(dbpgs, ixpgs)
int dbpgs; /* # of db cache pages */
int ixpgs; /* # of index cache pages - ignored in single-user version */
{
   if ( dbpg_table ) return( dberr(S_SETPAGES) );

   db_pgtab_sz = (dbpgs <= MINDBPAGES) ? MINDBPAGES : dbpgs;

#ifndef NO_TRANS
   if ( use_ovfl ) {
      ix_pgtab_sz = (ixpgs <= MINIXPAGES) ? MINIXPAGES : ixpgs;
   }
#endif
   return( db_status = S_OKAY );
}


/****************************************/
/*					*/
/*		dio_open		*/
/*					*/
/****************************************/
/* Open a database file
*/
int
dio_open( fno )
FILE_NO fno;
{
   FILE_ENTRY FAR *file_ptr, FAR *lru_file_ptr;
   register int FAR *uf_ptr;

   file_ptr = &file_table[fno];
   if ( file_ptr->ft_status == CLOSED ) {
      if ( cnt_open_files == max_open_files ) {
	 /* find least recently used file */
	 uf_ptr = &used_files[last_file];
	 lru_file_ptr = &file_table[last_file];
	 while (*uf_ptr || (lru_file_ptr->ft_status == CLOSED)) {
	    *uf_ptr = FALSE;
	    if (++last_file >= size_ft) {
	       last_file = 0;
	       lru_file_ptr = file_table;
	       uf_ptr = used_files;
	    }
	    else {
	      ++lru_file_ptr;
	      ++uf_ptr;
	   }
	 }
	 dio_close(last_file);
	 if (++last_file >= size_ft)
	    last_file = 0;
      }
      used_files[fno] = TRUE;

	 file_ptr->ft_desc = open_b(file_ptr->ft_name, db_oflag);
      if (file_ptr->ft_desc < 0)
	 return( dberr( S_NOFILE ) );
      file_ptr->ft_status = OPEN;
      ++cnt_open_files;
   }
   return( db_status = S_OKAY );
} /* dio_open() */


/****************************************/
/*					*/
/*		dio_close		*/
/*					*/
/****************************************/
/* Close a database file
*/
int
dio_close( fno )
FILE_NO fno;
{
   FILE_ENTRY FAR *file_ptr;

   file_ptr = &file_table[fno];
   if ( file_ptr->ft_status == OPEN ) {
      DB_CLOSE( file_ptr->ft_desc );
      file_ptr->ft_status = CLOSED;
      --cnt_open_files;
   }
   return( db_status = S_OKAY );
}


/****************************************/
/*					*/
/*		dio_init		*/
/*					*/
/****************************************/
/* Initialize database I/O
*/
int
dio_init()
{
   CHAR_P Tempbuff;
#define tempbuff Tempbuff.ptr

#ifdef DEBUG_DIO
    if (debugging_dio_init) {
	printf (__FILE__"300 dio_init: dbpgtab=%p pgsz=%d largest=%d\n",
	    (void *) dbpg_table, (int)page_size, (int)largest_page);
	fflush(stdout);
    }
#endif

   if ( dbpg_table ) {
      if ( dio_pzinit() != S_OKAY ) {
	 return( db_status );
      }
      if ( page_size > largest_page ) {
	 if ( (tempbuff = ALLOC(&Tempbuff, page_size, "tempbuff")) == NULL )
	    return( dberr(S_NOMEMORY) );
#ifndef NO_TRANS
	 MEM_UNLOCK(&Dbpgbuff);
	 FREE(&Dbpgbuff);
	 Dbpgbuff = Tempbuff;
#endif
	 largest_page = page_size;
      }
#ifdef DEBUG_DIO
      if (debugging_dio_init) {
	printf (__FILE__"323 dio_init: pzinited ok. pgsz=%d largest=%d\n",
	    (int)page_size, (int)largest_page);
	fflush(stdout);
      }
#endif

      return( S_OKAY );
   } /* end if ( dbpg_table ) */

   used_files =
	/* Macro references must be on one line for some compilers */ 
	(int FAR *)ALLOC(&Used_files, (size_ft+1)*sizeof(int), "used_files");
   db_lookup =
	/* Macro references must be on one line for some compilers */ 
	(LOOKUP_ENTRY FAR *)
	ALLOC(&Db_lookup, db_pgtab_sz*sizeof(LOOKUP_ENTRY), "db_lookup");
   dbpg_table =
	/* Macro references must be on one line for some compilers */ 
	(PAGE_ENTRY FAR *)
	ALLOC(&Dbpg_table, db_pgtab_sz*sizeof(PAGE_ENTRY), "dbpg_table");
#ifdef DEBUG_DIO
   if (debugging_dio_init) {
	printf (__FILE__"345 dio_init: usedfls=%p lookup=%p pgtab=%p\n",
	    (void *) used_files, (void *) db_lookup, (void *) dbpg_table);
	fflush(stdout);
   }
#endif
   if ( !used_files || !dbpg_table || !db_lookup )
      return( dberr(S_NOMEMORY) );
   byteset(used_files, 0, (size_ft + 1)*sizeof(*used_files));

#ifdef MULTI_TASK
   last_dblu.task = NULL;
#endif
   last_dblu.file = -1;
   last_dblu.pageno = -1L;
   last_dblu.slot = -1;

   /* initialize database cache */
   cache_init((int)db_pgtab_sz, db_lookup, dbpg_table, (int)page_size);
   /***cache_init(db_pgtab_sz, db_lookup, dbpg_table, page_size);****/
   if (db_status != S_OKAY) return(db_status);
#ifndef NO_TRANS
   if ( use_ovfl ) {
      ix_lookup =
	/* Macro references must be on one line for some compilers */ 
	(LOOKUP_ENTRY FAR *)
	ALLOC(&Ix_lookup, ix_pgtab_sz*sizeof(LOOKUP_ENTRY),"ix_lookup");
      ixpg_table = 
	/* Macro references must be on one line for some compilers */ 
	(PAGE_ENTRY FAR *)
	ALLOC(&Ixpg_table, ix_pgtab_sz*sizeof(PAGE_ENTRY), "ixpg_table");
      if ( !ix_lookup || !ixpg_table )
	 return( dberr(S_NOMEMORY) );

      cache_init(ix_pgtab_sz, ix_lookup, ixpg_table, IX_PAGESIZE);
      if (db_status != S_OKAY)
         return (db_status);

      if ( (dbpgbuff = ALLOC(&Dbpgbuff, page_size, "dbpgbuff")) == NULL )
	 return( dberr(S_NOMEMORY) );

      ixpg_lru_slot = 0;
   }
#endif			/* NO_TRANS */
   last_file = 0;
   dbpg_lru_slot = 0;
   no_modheld  = 0;
   working_file = NONE;

   /* initialize the page zero table and return */
#ifdef DEBUG_DIO
   if (debugging_dio_init) {
	puts (__FILE__"390 dio_init: last act is call to dio_pzinit.");
	fflush(stdout);
   }
#endif
   return( dio_pzinit() );
#undef tempbuff
} /* dio_init() */


static void	cache_init (pg_cnt, lu_ptr, pg_ptr, pgsize)
int		    pg_cnt;
LOOKUP_ENTRY FAR *  lu_ptr;
PAGE_ENTRY FAR *    pg_ptr;
int		    pgsize;
{
   register int pg_no;

#ifdef DEBUG_DIO
   if (debugging_dio_init) {
	printf (__FILE__"400 cache_init: pgcnt=%d lu=%p pgp=%p pgsz=%d\n",
	    pg_cnt, (void *) lu_ptr, (void *) pg_ptr, pgsize);
	fflush(stdout);
   }
#endif

   for (pg_no = 0; pg_no < pg_cnt; ++pg_no, ++lu_ptr, ++pg_ptr)
   {
#ifdef MULTI_TASK
      lu_ptr->task = NULL;
#endif
      lu_ptr->file = -1;
      lu_ptr->pageno = -1L;
      lu_ptr->pg_slot = pg_no;

      pg_ptr->lu_slot = pg_no;
      pg_ptr->recently_used = FALSE;
      pg_ptr->modified = FALSE;
      pg_ptr->holdcnt = 0;
#ifndef NO_TRANS
      pg_ptr->ovfl_addr = 0L;
#endif
      pg_ptr->buff = ALLOC(&pg_ptr->Buff, pgsize, db_avname);
      if (pg_ptr->buff == NULL) {
#ifdef DEBUG_DIO
	 if (debugging_dio_init) {
		printf (__FILE__"428 cache_init: alloc failed pgsz=%d\n",
		    pgsize);
		fflush(stdout);
	 }
#endif
         dberr(S_NOMEMORY);
         return;
      }
      MEM_UNLOCK(&pg_ptr->Buff);
   } /* end loop on pg_cnt */
   return;
} /* cache_init() */


/****************************************/
/*					*/
/*		dio_free		*/
/*					*/
/****************************************/
/* Free the memory allocated for pages
*/
void dio_free()
{
   register int pgt_lc;			/* loop control */
   register PAGE_ENTRY FAR *pg_ptr;

#ifdef MULTI_TASK
   if ( task_count > 1 ) {
      return;
   }
#endif
   MEM_UNLOCK(&db_global.Pgzero);
   FREE(&db_global.Pgzero);
   MEM_UNLOCK(&Used_files);
   FREE(&Used_files);
   MEM_UNLOCK(&Db_lookup);
   FREE(&Db_lookup);
   for (pgt_lc = db_pgtab_sz, pg_ptr = dbpg_table; --pgt_lc >= 0; ++pg_ptr) {
      MEM_UNLOCK(&pg_ptr->Buff);
      FREE(&pg_ptr->Buff);
   }
   MEM_UNLOCK(&Dbpg_table);
   FREE(&Dbpg_table);
#ifndef NO_TRANS
   if ( use_ovfl ) {
      MEM_UNLOCK(&Ix_lookup);
      FREE(&Ix_lookup);
      for (pgt_lc = ix_pgtab_sz, pg_ptr = ixpg_table; --pgt_lc >= 0; ++pg_ptr) {
	 MEM_UNLOCK(&pg_ptr->Buff);
	 FREE(&pg_ptr->Buff);
      }
      MEM_UNLOCK(&Ixpg_table);
      FREE(&Ixpg_table);
      MEM_UNLOCK(&Dbpgbuff);
      FREE(&Dbpgbuff);
   }
#endif
} /* dio_free() */


/****************************************/
/*					*/
/*		dio_clrfile		*/
/*					*/
/****************************************/
/* Clear pages for a single file.
*/
int
dio_clrfile(fno )
register FILE_NO fno;
{
   return( clear_cache(fno, fno+1) );
}



/****************************************/
/*					*/
/*		dio_clear		*/
/*					*/
/****************************************/
/* Clear all pages for *all* files from I/O buffer
*/
int
dio_clear()
{
   return( clear_cache(0, size_ft) );
}


/****************************************/
/*					*/
/*		clear_cache		*/
/*					*/
/****************************************/
/* Clear database page cache.
 * Clears all pages for a range of specified files.
 * Subroutine of dio_clrfile and dio_clear.
 */
static int clear_cache(fr_file, to_file )
FILE_NO fr_file;   /* clear from file "fr_file" */
FILE_NO to_file;   /* ..to (not thru) file "to_file" */
{
   FILE_NO s_file;   /* start file to be cleared */
   FILE_NO e_file;   /* end file (+1) to be cleared */
   register int i;
   LOOKUP_ENTRY FAR *lu_ptr, FAR *lu2_ptr;
   int pg_slot;
   PAGE_ENTRY FAR *pg_ptr;
   PGZERO FAR *pgzero_ptr;
   FILE_ENTRY FAR *file_ptr;
   int FAR *appl_ptr, FAR *excl_ptr;

#ifndef SINGLE_USER
   /* 
      We only clear pages which are not from static files and are
      not still locked.  The check on app_locks is made to implement 
      the ability to hold locks after the end of a transaction 
   */
   for (s_file = e_file = fr_file, file_ptr = &file_table[e_file],
		appl_ptr = &app_locks[e_file], excl_ptr = &excl_locks[e_file];
	s_file < to_file;
	++file_ptr, ++appl_ptr, ++excl_ptr) {
#else
   for (s_file = e_file = fr_file, file_ptr = &file_table[e_file];
	s_file < to_file;
	++file_ptr) {
#endif
      if ((e_file < to_file) &&
#ifndef SINGLE_USER
	  ((dbopen >= 2) || (!*appl_ptr && !*excl_ptr)) && 
#endif
	  !(file_ptr->ft_flags & STATIC)) 
	 ++e_file;
      else {
	 if (s_file < e_file) {
	    /* find range of pages to be cleared */
	    dio_findpg(s_file, 0L, NULL, NULL, &lu_ptr);
	    dio_findpg(e_file, 0L, NULL, NULL, &lu2_ptr);

#ifdef MULTI_TASK
	    last_dblu.task = NULL;
#endif
	    last_dblu.file = -1;
	    last_dblu.pageno = -1L;
	    last_dblu.slot = -1;

	    if (lu_ptr < lu2_ptr) { /* otherwise file has no pages in cache */
	       /* adjust lookup table entries */
	       while ((lu_ptr > db_lookup) && ((--lu_ptr)->file >= 0)) {
		  --lu2_ptr;

#ifdef MULTI_TASK
		  lu2_ptr->task = lu_ptr->task;
#endif
		  lu2_ptr->file = lu_ptr->file;
		  lu2_ptr->pageno = lu_ptr->pageno;
		  /* exchange page slot numbers */
                  pg_slot = lu_ptr->pg_slot;
                  lu_ptr->pg_slot = lu2_ptr->pg_slot;
                  lu2_ptr->pg_slot = pg_slot;
                  dbpg_table[pg_slot].lu_slot = lu2_ptr - db_lookup;
		  dbpg_table[lu_ptr->pg_slot].lu_slot = lu_ptr - db_lookup;
	       }

	       if ( lu_ptr->file < 0 )
		  ++lu_ptr;

	       while (lu_ptr < lu2_ptr) {

#ifdef MULTI_TASK
		  lu_ptr->task = NULL;
#endif
		  lu_ptr->file = -1;
		  lu_ptr->pageno = -1L;
                  pg_ptr = &dbpg_table[lu_ptr->pg_slot];
		  if ( pg_ptr->modified || pg_ptr->holdcnt ) {
		     --no_modheld;
		     pg_ptr->modified = FALSE;
		  }
		  pg_ptr->recently_used = FALSE;
		  pg_ptr->holdcnt = 0;
#ifndef NO_TRANS
		  pg_ptr->ovfl_addr = 0L;
#endif
		  ++lu_ptr;
	       }
	    }
	    /* clear page zeroes and close files */
	    for (i = s_file, pgzero_ptr = &pgzero[i];
		 i < e_file;
		 ++i, ++pgzero_ptr) {
	       pgzero_ptr->pz_modified = FALSE;
	       pgzero_ptr->pz_next = 0L;
#ifdef	 CLOSE_FILES
	       dio_close(i);
#endif
	    }
	 }
	 s_file = ++e_file;
      }
   }     
   return( db_status = S_OKAY );
} /* clear_cache() */


/****************************************/
/*					*/
/*		 dio_flush		*/
/*					*/
/****************************************/
/* Flushes entire database I/O cache.
 * Writes out all modified cache pages to respective files (dio_out),
 * then writes out page zero (dio_pzflush).
 */
int dio_flush()
{
   register int pgt_lc;			/* loop control */
   int fno;
   PAGE_ENTRY FAR *pg_ptr;
   LOOKUP_ENTRY FAR *lu_ptr;

#ifdef DEBUG_DIO
    if (debugging_dio_close) {
	printf (__FILE__"685 dio_flush: check cache dbpgtab=%p count=%d\n",
	    (void *) dbpg_table, (int)db_pgtab_sz);
	fflush(stdout);
    }
#endif
   if ( dbpg_table == NULL ) return( db_status = S_OKAY );

   for (pgt_lc = db_pgtab_sz, pg_ptr = dbpg_table; --pgt_lc >= 0; ++pg_ptr) {
      if (!pg_ptr->modified) {
#ifndef NO_TRANS
	 pg_ptr->ovfl_addr = 0L;		/*[612]*/
#endif
	 continue;
      }
      lu_ptr = &db_lookup[pg_ptr->lu_slot];
#ifdef MULTI_TASK
      if ( lu_ptr->task != Currtask.v.ptr ) {
	 continue;
      }
#endif
#ifndef NO_TRANS
      if ((dboptions & TRLOGGING) && trans_id && !trcommit && use_ovfl) {
	 /* flush to overflow/log file -- before tr commit */
	 if (o_write(pg_ptr, lu_ptr) != S_OKAY) return( db_status );
	 if ( lu_ptr->pageno > o_pages(lu_ptr->file) ) {
	    /* no need to rewrite this page at trcommit time */
	    pg_ptr->holdcnt = 0;
	    pg_ptr->modified = FALSE;
	    --no_modheld;
	 }
	 continue;
      }
      pg_ptr->ovfl_addr = 0L;
#endif
      /* write directly to database */
#ifdef NO_TRANS
#ifdef DEBUG_DIO
	if (debugging_dio_close) {
	    printf (__FILE__"723 dio_flush: write modified pg#%d @ %p\n",
		db_pgtab_sz - pgt_lc, (void *) pg_ptr);
	    fflush(stdout);
	}
#endif
      if (dio_out(pg_ptr, lu_ptr) != S_OKAY) return( db_status );
#else
      if (dio_out(pg_ptr, lu_ptr, 1) != S_OKAY) return( db_status );
#endif
      pg_ptr->holdcnt = 0;
      pg_ptr->modified = FALSE;
      --no_modheld;
#ifndef NO_TRANS
      if ( trlog_flag ) {
	 fno = lu_ptr->file;
	 MEM_LOCK(&pg_ptr->Buff);
	 d_trlog(fno, (int)lu_ptr->pageno, pg_ptr->buff,
		 file_table[fno].ft_pgsize);
	 MEM_UNLOCK(&pg_ptr->Buff);
      }
#endif
   }
   /* store the page zero values in the data file and return */
   return( dio_pzflush() );
} /* dio_flush() */



/* Set the default file number
*/
void dio_setdef( file_no )
FILE_NO file_no;
{
   working_file = file_no;
}



/****************************************/
/*					*/
/*		 dio_get		*/
/*					*/
/****************************************/
/* Database I/O page get
*/
int
dio_get( page_no, page_ptr, hold )
F_ADDR page_no;
char FAR * FAR *page_ptr;
int hold;
{
   PAGE_ENTRY FAR *pg_ptr;

#ifndef SINGLE_USER
   if ( dbopen == 1 ) {
      if ( !app_locks[working_file] && !excl_locks[working_file] &&
	   !(file_table[working_file].ft_flags & STATIC) )
	 return( dberr(S_NOTLOCKED) );
   }
#endif
   if ( pgzero[working_file].pz_next == 0L )
      if ( dio_pzread(working_file) != S_OKAY )
	 RETURN( db_status );

   if (dio_findpg(working_file, page_no, dbpg_table, &pg_ptr, NULL) == S_OKAY) {
      MEM_LOCK(&pg_ptr->Buff);
      *page_ptr = pg_ptr->buff;
      pg_ptr->recently_used = TRUE;
      used_files[working_file] = TRUE;
      if ( hold ) {
	 if ( ++pg_ptr->holdcnt > 1 )
	    dberr(S_SYSERR);
	 else if ( ! pg_ptr->modified )
	    ++no_modheld;
      }
   }
   return( db_status );
} /* dio_get() */



/****************************************/
/*					*/
/*		dio_touch		*/
/*					*/
/****************************************/
/* Set modified flag for a page
*/
int
dio_touch( page_no )
F_ADDR page_no;
{
   PAGE_ENTRY FAR *pg_ptr;

#ifndef NO_TRANS
   /* ensure overflow data is initialized when exclusive db access */
   if ((trans_id && (dboptions & TRLOGGING) && use_ovfl) &&
       (o_fileinit(working_file) != S_OKAY))
      return( db_status );
#endif
#ifndef SINGLE_USER
   if ( dbopen == 1 ) { 
      /* check shared access priviledges */
      if ( !trans_id && !excl_locks[working_file] )  
	 return( dberr(S_NOTRANS) );
      if ( app_locks[working_file] >= 0 && !excl_locks[working_file] )
	 return( dberr( S_NOTLOCKED ) );
   }
#endif
   if (dio_findpg(working_file, page_no, dbpg_table, &pg_ptr, NULL) == S_OKAY ) {
      pg_ptr->recently_used = TRUE;
      used_files[working_file] = TRUE;
      if ( ! pg_ptr->modified ) {
	 pg_ptr->modified = TRUE;
	 if ( ! pg_ptr->holdcnt )
	    ++no_modheld;
      }
      if ( pg_ptr->holdcnt > 0 ) {
	 --pg_ptr->holdcnt;
	 if ( pg_ptr->holdcnt ) {
	    db_status = S_OKAY;
	 }
	 MEM_UNLOCK(&pg_ptr->Buff);
      }
   }
   return( db_status );
} /* dio_touch() */



/****************************************/
/*					*/
/*		dio_read		*/
/*					*/
/****************************************/
/* Database I/O read
 * Merely returns ptr into rec in a page
 * unless a page swap is necessary.
 */
int
dio_read( dba, recptr, hold )
DB_ADDR dba;
char FAR * FAR *recptr;
int hold;
{
   FILE_NO file;
   int offset;
   F_ADDR us1, us2;
   FILE_ENTRY FAR *file_ptr;
   PAGE_ENTRY FAR *pg_ptr;

   file = NUM2INT((FILE_NO)((dba >> FILESHIFT) & FILEMASK), ft_offset);
   file_ptr = &file_table[file];

   if ( pgzero[file].pz_next == 0L )
      if ( dio_pzread(file) != S_OKAY )
	 RETURN( db_status );

#ifndef SINGLE_USER
   if ( dbopen == 1 ) {
      /* check shared access priviledges */
      if (!app_locks[file] &&
	  !excl_locks[file] &&
	  !(file_ptr->ft_flags & STATIC))
	 return( dberr(S_NOTLOCKED) );
   }
#endif
   us1 = ADDRMASK & dba;
   us2 = (us1 - 1)/file_ptr->ft_slots;
   if (dio_findpg(file, us2 + 1, dbpg_table, &pg_ptr, NULL) == S_OKAY ) {
      pg_ptr->recently_used = TRUE;
      used_files[file] = TRUE;
      offset = file_ptr->ft_slsize*(int)(us1 - 1 - us2*file_ptr->ft_slots) +
								     PGHDRSIZE;
      MEM_LOCK(&pg_ptr->Buff);
      *recptr = &pg_ptr->buff[offset];
      if ( hold ) {
	 if ( (++pg_ptr->holdcnt == 1) && !pg_ptr->modified) {
	    ++no_modheld;
	 }
      }
   }
   return( db_status );
} /* dio_read() */


/****************************************/
/*					*/
/*		dio_write		*/
/*					*/
/****************************************/
/* Database I/O write: copies data record into a page slot.
 * Finds record's page, swapping it into cache if necessary.
 * Sets page's 'touched' flags, timestamps, etc.
 * If recptr not NULL, copies rec to page cache.
 */
int
dio_write( dba, recptr, release )
DB_ADDR dba;
CONST char FAR *recptr;
int release;
{
   FILE_NO file;
   F_ADDR us1, us2;
   int offset;
   FILE_ENTRY FAR *file_ptr;
   PAGE_ENTRY FAR *pg_ptr;

   file = NUM2INT((FILE_NO)((dba >> FILESHIFT) & FILEMASK), ft_offset);

#ifndef NO_TRANS
   /* ensure overflow data is initialized when exclusive db access */
   if ((trans_id && (dboptions & TRLOGGING) && use_ovfl) &&
       (o_fileinit(file) != S_OKAY))
      return( db_status );
#endif
#ifndef SINGLE_USER
   if (dbopen == 1) {
      if (!trans_id && !excl_locks[file])  
	 return( dberr(S_NOTRANS) );

      /* check shared access priviledges */
      if ( app_locks[file] >= 0 && !excl_locks[file] )
	 return( dberr(S_NOTLOCKED) );
   }
#endif
   file_ptr = &file_table[file];
   us1 = ADDRMASK & dba;
   us2 = (us1 - 1)/file_ptr->ft_slots;
   if (dio_findpg(file, us2 + 1, dbpg_table, &pg_ptr, NULL) == S_OKAY ) {
      pg_ptr->recently_used = TRUE;
      used_files[file] = TRUE;
      if ( recptr != NULL ) {
	 offset = file_ptr->ft_slsize*(int)(us1 - 1 - us2*file_ptr->ft_slots) +
								     PGHDRSIZE;
	 MEM_LOCK(&pg_ptr->Buff);
	 bytecpy(&pg_ptr->buff[offset], recptr, file_ptr->ft_slsize);
	 MEM_UNLOCK(&pg_ptr->Buff);
      }
      if ( ! pg_ptr->modified ) {
	 pg_ptr->modified = TRUE;
	 if ( ! pg_ptr->holdcnt )
	    ++no_modheld;
      }
      if ( release ) {
	 if ( --pg_ptr->holdcnt < 0 )
	    dberr(S_SYSERR);
	 MEM_UNLOCK(&pg_ptr->Buff);
      }
   }
   return( db_status );
} /* dio_write() */


/* Release database page hold
*/
int
dio_release( dba )
DB_ADDR dba;
{
   FILE_NO file;
   F_ADDR us1, us2;
   PAGE_ENTRY FAR *pg_ptr;

   file = NUM2INT((FILE_NO)((dba >> FILESHIFT) & FILEMASK), ft_offset);
   us1 = ADDRMASK & dba;
   us2 = file_table[file].ft_slots;
   if (dio_findpg(file, ((us1 - 1)/us2) + 1, dbpg_table, &pg_ptr,
		  NULL) == S_OKAY) {
      if (pg_ptr->holdcnt) {
	 --pg_ptr->holdcnt;
	 if ( !pg_ptr->holdcnt && !pg_ptr->modified )
	    --no_modheld;
	 MEM_UNLOCK(&pg_ptr->Buff);
      }
   }
   return( db_status );
}



#ifndef SINGLE_USER
/* Read record lock bit
*/
dio_rrlb(dba, rid )
DB_ADDR dba;
INT *rid;
{
   FILE_NO file;   /* file number */
   F_ADDR page;    /* page number */
   F_ADDR sno;     /* slot number */
   F_ADDR spp;     /* slots per page */
   F_ADDR offset;  /* lseek address - offset from start of file */
   FILE_ENTRY FAR *file_ptr;

   file = NUM2INT((FILE_NO)((dba >> FILESHIFT) & FILEMASK), ft_offset);
   if ( dio_open(file) == S_OKAY ) {
      file_ptr = &file_table[file];
      sno = ADDRMASK & dba;
      spp = file_ptr->ft_slots;
      page = (sno - 1)/spp + 1;
      offset = PGHDRSIZE + page*file_ptr->ft_pgsize +
				 (sno - 1 - (page - 1)*spp)*file_ptr->ft_slsize;
      DB_LSEEK(file_ptr->ft_desc, offset, 0);
      if ( DB_READ(file_ptr->ft_desc, (char FAR *)rid, sizeof(INT))
		!= sizeof(INT) ) {
	 dberr(S_BADREAD);
      }
      NTOHS (*rid);
   }
   return( db_status );
} /* dio_rrlb() */


/* Write record lock bit
*/
dio_wrlb(dba, rid )
DB_ADDR dba;
INT rid;
{
   FILE_NO file;   /* file number */
   F_ADDR page;    /* page number */
   F_ADDR sno;     /* slot number */
   F_ADDR spp;     /* slots per page */
   F_ADDR offset;  /* offset from start of page or file */
   int clr_in_tx;  /* true if called from d_rlbclr in trx */
   INT trid;		/* [333] working rid */
   FILE_ENTRY FAR *file_ptr;
   PAGE_ENTRY FAR *pg_ptr;

   file = NUM2INT((FILE_NO)((dba >> FILESHIFT) & FILEMASK), ft_offset);
   file_ptr = &file_table[file];
   sno = ADDRMASK & dba;
   spp = file_ptr->ft_slots;
   page = ((sno - 1)/spp) + 1;
   offset = PGHDRSIZE + (sno - 1 - (page - 1)*spp)*file_ptr->ft_slsize;
   clr_in_tx = !(rid & RLBMASK) && trans_id;

   if ( dbopen > 1 || (app_locks[file] || excl_locks[file]) ) {
      /* file is locked - check if record in cache */
      if (dio_findpg(file, page, dbpg_table, &pg_ptr, NULL) == S_OKAY) {
	 MEM_LOCK(&pg_ptr->Buff);
	 /* record in cache - update only rlb in rid */
	 bytecpy(&trid, &pg_ptr->buff[offset], sizeof(INT));
	 MEM_UNLOCK(&pg_ptr->Buff);
	 rid = (trid & ~((INT)RLBMASK)) | (rid & RLBMASK);
	 bytecpy(&pg_ptr->buff[offset], &rid, sizeof(INT));
	 if ( clr_in_tx ) {
	    /* clearing within a transaction requires touching page */
	    if ( ! pg_ptr->modified ) {
	       pg_ptr->modified = TRUE;
	       if ( ! pg_ptr->holdcnt )
		  ++no_modheld;
	    }
	 }
      }
      else
	 clr_in_tx = FALSE;
   }
   if ( ! clr_in_tx ) {
      /* update only rlb directly to disk */
      if ( dio_open(file) == S_OKAY ) {
	 offset += page*file_ptr->ft_pgsize;

	 /* read rid from disk, and set/clear rlb accordingly */
	 DB_LSEEK(file_ptr->ft_desc, offset, 0);
	 if ( DB_READ(file_ptr->ft_desc, (char FAR *)&trid, sizeof(INT))
		!= sizeof(INT) ) {
	    dberr(S_BADREAD);
	 }
         NTOHS (trid);
	 rid = (trid & ~((INT)RLBMASK)) | (rid & RLBMASK);

	 /* write original rid out with modified rlb */
         trid = htons (rid); /* make a copy in trid for byte swap */
	 DB_LSEEK(file_ptr->ft_desc, offset, 0);	/* reseek */
	 if ( DB_WRITE(file_ptr->ft_desc, (char FAR *)&trid, sizeof(INT)) !=
	      sizeof(INT) )
	    dberr(S_BADWRITE);
      }
   }
   return( db_status );
} /* dio_wrlb() */
#endif			/* SINGLE_USER */


/****************************************/
/*					*/
/*		dio_findpg		*/
/*					*/
/****************************************/
/* Search a cache for page
*/
int
dio_findpg(file, page, pg_table, xpg_ptr, xlu_ptr )
FILE_NO      file;       /* file number = 0..size_ft-1 */
F_ADDR       page;       /* database page number */
PAGE_ENTRY FAR *pg_table;   /* = dbpg_table, ixpg_table, or NULL */
PAGE_ENTRY FAR * FAR *xpg_ptr;  /* pointer to page table entry for found page */
LOOKUP_ENTRY FAR * FAR *xlu_ptr;/* pointer to lookup table slot for found page*/
{
#ifdef MULTI_TASK
   CHAR_P Tempbuff;
#define tempbuff Tempbuff.ptr
#endif
   LOOKUP_ENTRY FAR *lookup;  /* = db_lookup or ix_lookup */
   int pgtab_sz;          /* = db_pgtab_sz or ix_pgtab_sz */
   long cmp;
   int cnt;
   register int lu_slot, l, u;
   LOOKUP_ENTRY FAR *lu_ptr, FAR *replu_ptr;
   PAGE_ENTRY FAR *pg_ptr;
   int *lru_ptr;
   int pg_slot;
#ifndef NO_TRANS
   BOOLEAN db_cache;      /* TRUE if currently using dbpg_table */
   F_ADDR ovfl_addr;
#endif
#ifdef MULTI_TASK
   INT pgsize;
#endif

#ifdef NO_TRANS

   /* check if desired page was last one */
#ifdef MULTI_TASK
   if ((Currtask.v.ptr == last_dblu.task) && (file == last_dblu.file) && (page == last_dblu.pageno)) {
#else
   if ((file == last_dblu.file) && (page == last_dblu.pageno)) {
#endif
      if (xlu_ptr != NULL)
         *xlu_ptr = &db_lookup[last_dblu.slot];
      if (xpg_ptr != NULL)
         *xpg_ptr = &dbpg_table[db_lookup[last_dblu.slot].pg_slot];
      return( db_status = S_OKAY );
   }
   lookup = db_lookup;
   pgtab_sz = db_pgtab_sz;
#else			/* NO_TRANS */
   if (db_cache = (!pg_table || (pg_table == dbpg_table))) {
      /* check if desired page was last one */
#ifdef MULTI_TASK
      if ((Currtask.v.ptr == last_dblu.task) && (file == last_dblu.file) &&
	     (page == last_dblu.pageno)) {
#else
      if ((file == last_dblu.file) && (page == last_dblu.pageno)) {
#endif
         if (xlu_ptr != NULL)
            *xlu_ptr = &db_lookup[last_dblu.slot];
         if (xpg_ptr != NULL)
            *xpg_ptr = &dbpg_table[db_lookup[last_dblu.slot].pg_slot];
	 return( db_status = S_OKAY );
      }
      lookup = db_lookup;
      pgtab_sz = db_pgtab_sz;
   }
   else {
      lookup = ix_lookup;
      pgtab_sz = ix_pgtab_sz;
   }
#endif			/* NO_TRANS */
   /* perform binary search of sorted lookup table */
   l = 0;
   u = pgtab_sz - 1;
   while (u >= l) {
      lu_ptr = &lookup[lu_slot = (l + u)/2];
#ifdef MULTI_TASK
      if ((cmp = Currtask.v.ptr - lu_ptr->task) == 0)
#endif
	 if ((cmp = file - lu_ptr->file) == 0)
	    cmp = page - lu_ptr->pageno;
      if (cmp < 0)
         u = lu_slot - 1;
      else if (cmp > 0)
         l = lu_slot + 1;
      else {
#ifndef NO_TRANS
         if (db_cache)
         {
#endif
#ifdef MULTI_TASK
            last_dblu.task = lu_ptr->task;
#endif
            last_dblu.file = lu_ptr->file;
            last_dblu.pageno = lu_ptr->pageno;
            last_dblu.slot = lu_slot;
#ifndef NO_TRANS
         }
#endif
         if (xlu_ptr != NULL)
            *xlu_ptr = lu_ptr;
         if (xpg_ptr != NULL)
            *xpg_ptr = &pg_table[lu_ptr->pg_slot];
	 return( db_status = S_OKAY );
      }
   }
   if ( ! pg_table ) {
      /* null page table indicates that only a lookup was desired */
      if (cmp > 0)
         ++lu_ptr;
      if (xlu_ptr != NULL)
         *xlu_ptr = lu_ptr;
      return( db_status = S_NOTFOUND );
   }
   /* page not found - read into cache */
#ifndef NO_TRANS
   if( !use_ovfl && trans_id && (no_modheld == pgtab_sz) )
      return( db_status =  S_TRCHANGES );

   /* check to see if page is in overflow file */
   ovfl_addr = 0L;
   if ( cache_ovfl && file != ov_file ) {
      if ( o_search( file, page, &ovfl_addr ) != S_OKAY )
	 return( db_status );
   }
   /* check for overflow */
   if ( db_cache && trans_id && (no_modheld == pgtab_sz) && !cache_ovfl ) {
      cache_ovfl = TRUE;
   }
   /* select a page to replace */
   if (db_cache) {
      lru_ptr = &dbpg_lru_slot;
   } else {
      lru_ptr = &ixpg_lru_slot;
   }
#else
   /* select a page to replace */
   lru_ptr = &dbpg_lru_slot;
#endif			/* NO_TRANS */
   for (cnt = 2*pgtab_sz, pg_slot = *lru_ptr, pg_ptr = &pg_table[pg_slot];
        --cnt >= 0;
        ++pg_slot, ++pg_ptr) {
      if (pg_slot >= pgtab_sz)
      {
         pg_slot = 0;
         pg_ptr = pg_table;
      }
      replu_ptr = &lookup[pg_ptr->lu_slot];
      if (!pg_ptr->recently_used && (pg_ptr->holdcnt == 0)) {
#ifdef NO_TRANS
	 if (pg_ptr->modified) {
	    dio_out(pg_ptr, replu_ptr);
	    pg_ptr->modified = FALSE;
	    --no_modheld;
	 }
#else
	 if (pg_ptr->modified) {
	    /* allow updates outside transactions for single-user mode */
#ifdef SINGLE_USER
	    if (!db_cache || (EXCL_OPEN() && !trans_id)) {
#else
#ifdef MULTI_TASK
	    MEM_LOCK(&replu_ptr->task->Excl_locks);
#endif
	    if (!db_cache || ((EXCL_OPEN() ||
#ifdef MULTI_TASK
		( replu_ptr->task == Currtask.v.ptr ) ?
		   excl_locks[lookup[pg_ptr->lu_slot].file] :
		   replu_ptr->task->Excl_locks.ptr[replu_ptr->file]) && 
		!trans_id)) {
#else
	        excl_locks[lookup[pg_ptr->lu_slot].file]) && !trans_id)) {
#endif		/* MULTI_TASK */
#endif		/* SINGLE_USER */
	       /* ix page swapping occurs here */
	       dio_out(pg_ptr, replu_ptr, db_cache);
	       pg_ptr->modified = FALSE;
	       if ( db_cache ) --no_modheld;
	    }
	    else { 
	       if (!use_ovfl || !cache_ovfl) continue; /* skip modified pages */
	       /* Write out modified page */
	       pg_ptr->modified = FALSE;
	       --no_modheld;  /* must be in db cache */
	       if (o_write(pg_ptr, replu_ptr) != S_OKAY) return( db_status );
	    }
#ifdef MULTI_TASK
	    MEM_UNLOCK(&replu_ptr->task->Excl_locks);
#endif
	 }
	 pg_ptr->ovfl_addr = ovfl_addr;
#endif			/* NO_TRANS */
	 pg_ptr->recently_used = TRUE;
         if ((*lru_ptr = (pg_slot + 1)) >= pgtab_sz)
            *lru_ptr = 0;
	 break;
      }
      else if ( pg_ptr->holdcnt == 0 )
	 pg_ptr->recently_used = FALSE;
   }
   if (cnt < 0)
      return( dberr(S_FAULT) );

   /* adjust lookup table */
   if (replu_ptr < lu_ptr) {
      if (cmp < 0)
      {
         --lu_ptr;
         --lu_slot;
      }
      while (replu_ptr < lu_ptr) {
	 bytecpy(replu_ptr, replu_ptr + 1, sizeof(*replu_ptr));
         pg_table[replu_ptr->pg_slot].lu_slot = replu_ptr - lookup;
	 ++replu_ptr;
      }
   }
   else if (replu_ptr > lu_ptr) {
      if (cmp > 0)
      {
         ++lu_ptr;
         ++lu_slot;
      }
      while (replu_ptr > lu_ptr) {
	 bytecpy(replu_ptr, replu_ptr - 1, sizeof(*replu_ptr));
         pg_table[replu_ptr->pg_slot].lu_slot = replu_ptr - lookup;
	 --replu_ptr;
      }
   }
#ifdef MULTI_TASK

#ifdef NO_TRANS
   pgsize = file_table[( lu_ptr->file > -1 ) ? lu_ptr->file : file].ft_pgsize;
#else
   pgsize = ( db_cache )
	    ? file_table[( lu_ptr->file > -1 ) ? lu_ptr->file : file].ft_pgsize
	    : file_table[ov_file].ft_pgsize;
#endif

   if ( ! pgsize ) {
      pgsize = page_size;
   }
   if ( pgsize != file_table[file].ft_pgsize ) {
      Tempbuff.ptr = NULL;
      tempbuff = ALLOC(&Tempbuff, file_table[file].ft_pgsize, "dbpgbuff");
      if ( ! tempbuff ) {
	 return( dberr(S_NOMEMORY) );
      }
      if ( pg_ptr->buff ) {
	 MEM_UNLOCK(&pg_ptr->Buff);
	 FREE(&pg_ptr->Buff);
      }
      MEM_UNLOCK(&Tempbuff);
      pg_ptr->Buff = Tempbuff;
   }
#endif /* MULTI_TASK */

#ifdef MULTI_TASK
   lu_ptr->task = Currtask.v.ptr;
#endif
   lu_ptr->file = file;
   lu_ptr->pageno = page;
   lu_ptr->pg_slot = pg_slot;
   pg_ptr->lu_slot = lu_slot;
   if (xlu_ptr != NULL)
      *xlu_ptr = lu_ptr;
   if (xpg_ptr != NULL)
      *xpg_ptr = pg_ptr;
#ifdef NO_TRANS
#ifdef MULTI_TASK
   last_dblu.task = lu_ptr->task;
#endif
   last_dblu.file = lu_ptr->file;
   last_dblu.pageno = lu_ptr->pageno;
   last_dblu.slot = lu_slot;
   dio_in(pg_ptr, lu_ptr);
#else
   if (db_cache) {
#ifdef MULTI_TASK
      last_dblu.task = lu_ptr->task;
#endif
      last_dblu.file = lu_ptr->file;
      last_dblu.pageno = lu_ptr->pageno;
      last_dblu.slot = lu_slot;
   }
   dio_in(pg_ptr, lu_ptr, db_cache);
#endif

   return( db_status );
#undef tempbuff
} /* dio_findpg() */


/****************************************/
/*					*/
/*		 dio_out		*/
/*					*/
/****************************************/
/* Writes a page in the cache to file.
 * For byte order neutrality:
 * Each page is raima header data, then a bunch of slots.
 * The first 4 bytes of the page header is the timestamp
 * when the page was written, and it's swapped here.
 * Each slot is raima record header and user record data.
 * The user record data must already have been byte
 * swapped if necessary--to vista its just a large char buf.
 * The rest of the page header and the record header for each
 * slot is byte swapped before the io by calling the
 * page swap function.
 */
int
#ifndef NO_TRANS
dio_out(pg_ptr, lu_ptr, db_cache)
#else
dio_out(pg_ptr, lu_ptr)
#endif
PAGE_ENTRY FAR *pg_ptr;    /* page table entry to be output */
LOOKUP_ENTRY FAR *lu_ptr;  /* corresponding lookup table entry */
#ifndef NO_TRANS
   BOOLEAN db_cache;      /* TRUE if pg_ptr is in db page cache */
#endif
{
   int		desc;	/* file descriptor */
   int		fno;	/* file number */
   int		pgsize;	/* size of page */
   long		addr;	/* file address */
   time_t	host_timestamp;
   ULONG	netorder_timestamp;

   MEM_LOCK(&pg_ptr->Buff);

   /* Get the current time in "network" format.
    * (Original vista code had it typed as a long.)
    */
   time (&host_timestamp);
   netorder_timestamp = (ULONG) host_timestamp;
   HTONL (netorder_timestamp);

#ifdef NO_TRANS
   fno = lu_ptr->file;
   pgsize = file_table[fno].ft_pgsize;
   addr = lu_ptr->pageno * (long)pgsize;
   memcpy (pg_ptr->buff, &netorder_timestamp, sizeof(ULONG));
#else
   if ( db_cache ) {
      fno = lu_ptr->file;
      pgsize = file_table[fno].ft_pgsize;
   }
   else
      pgsize = file_table[ov_file].ft_pgsize;

   if ( pg_ptr->ovfl_addr == 0L ) {
      /* write to database */
      addr = lu_ptr->pageno * (long)pgsize;
      memcpy (pg_ptr->buff, &netorder_timestamp, sizeof(ULONG));
   }
   else {
      /* write to overflow file */
      fno = ov_file;
      addr = pg_ptr->ovfl_addr;
   }
#endif
   if ( dio_open(fno) == S_OKAY ) {
      swab_page (pg_ptr->buff, &file_table[fno], HTON);
      desc = file_table[fno].ft_desc;
      DB_LSEEK( desc, addr, 0 );
      if (DB_WRITE( desc, pg_ptr->buff, pgsize ) != pgsize) dberr(S_BADWRITE);
   }
   MEM_UNLOCK(&pg_ptr->Buff);
   return( db_status );
} /* dio_out() */



/****************************************/
/*					*/
/*		 dio_in			*/
/*					*/
/****************************************/
#ifdef NO_TRANS
/* Read in a page to the buffer
*/
static int dio_in(pg_ptr, lu_ptr)
PAGE_ENTRY FAR *pg_ptr; /* page table entry to be input */
LOOKUP_ENTRY FAR *lu_ptr; /* corresponding to pg_ptr */
#else
/* Read in a page to the buffer
*/
static int dio_in(pg_ptr, lu_ptr, db_cache )
PAGE_ENTRY FAR *pg_ptr; /* page table entry to be input */
LOOKUP_ENTRY FAR *lu_ptr; /* corresponding to pg_ptr */
BOOLEAN db_cache;  /* TRUE if pg_ptr in db cache */
#endif
{
   int desc;   /* file descriptor */
   int fno;    /* file number */
   int pgsize; /* page size */
   long addr;  /* file address */
   PGZERO FAR *pgzero_ptr;
   FILE_ENTRY FAR *file_ptr;
   int r;

   file_ptr = &file_table[fno = lu_ptr->file];
#ifdef NO_TRANS
   pgsize = file_ptr->ft_pgsize;
   addr = lu_ptr->pageno*pgsize;
#else
   pgsize = db_cache ? file_ptr->ft_pgsize : file_table[ov_file].ft_pgsize;

   if (pg_ptr->ovfl_addr == 0L) {
      /* read from database file */
      /* if !db_cache, overflow address not set on initial read */
      addr = db_cache ? lu_ptr->pageno*pgsize :
			(pg_ptr->ovfl_addr = lu_ptr->pageno);
   }
   else {
      /* read from overflow file */
      file_ptr = &file_table[fno = ov_file];
      addr = pg_ptr->ovfl_addr;
   }
#endif
   if ( dio_open(fno) == S_OKAY ) {
      desc = file_ptr->ft_desc;
      DB_LSEEK(desc, addr, 0);
      MEM_LOCK(&pg_ptr->Buff);
      if ((r = DB_READ( desc, pg_ptr->buff, pgsize )) < pgsize) {
	 byteset(&pg_ptr->buff[r], '\0', pgsize - r);
	 DB_LSEEK(desc, addr, 0);
	 if (DB_WRITE( desc, pg_ptr->buff, pgsize ) != pgsize) {
	    /* clean up and return out of space */
	    DB_LSEEK(desc, addr, 0);
	    DB_WRITE(desc, "", 0);
	    pgzero_ptr = &pgzero[fno];
	    pgzero_ptr->pz_next--;
	    pgzero_ptr->pz_modified = TRUE;
	    dio_pzflush();
	    dberr( S_NOSPACE );
	 }
      }
      swab_page (pg_ptr->buff, file_ptr, NTOH);
      MEM_UNLOCK(&pg_ptr->Buff);
   }
   return( db_status );
} /* dio_in() */


/***********************************************************************
   Page zero handling functions for data and key files
***********************************************************************/

#ifndef	 NO_TIMESTAMP
/* Increment and return file timestamp
*/
ULONG dio_pzsetts(fno )
FILE_NO fno;
{
   ULONG ts;
   PGZERO FAR *pgzero_ptr;

   if ( db_tsrecs || db_tssets ) {
      pgzero_ptr = &pgzero[fno];
      pgzero_ptr->pz_modified = TRUE;
      ts = ++pgzero_ptr->pz_timestamp;
   }
   else
      ts = 0;

   return( ts );
}


/* Return file timestamp
*/
ULONG dio_pzgetts(fno)
FILE_NO fno;
{
   if ( pgzero[fno].pz_next == 0L )
      dio_pzread(fno);
   o_fileinit(fno);
   return ( pgzero[fno].pz_timestamp );
}

#endif

/* Initialize page zero table
*/
static int dio_pzinit()
{
   register FILE_NO i;
   PGZERO FAR *pgzero_ptr;

#ifdef DEBUG_DIO
   if (debugging_dio_init) {
	printf (__FILE__"1430 dio_pzinit: szft=%d oldsz=%d\n",
	    (int)size_ft, (int)old_size_ft);
	fflush (stdout);
   }
#endif

   /* Macro references must be on one line for some compilers */ 
   if (ALLOC_TABLE(&db_global.Pgzero, size_ft*sizeof(PGZERO), 
		      old_size_ft*sizeof(PGZERO), "pgzero") != S_OKAY ) {
#ifdef DEBUG_DIO
	if (debugging_dio_init) {
	    printf (__FILE__"1444 pzinit: alloc_table failed, db_status=%d\n",
		    db_status);
	    fflush (stdout);
	}
#endif
	return( db_status );
   }

   /* read in page zeros */
   for (i = old_size_ft, pgzero_ptr = pgzero; 
	i < size_ft;
	++i, ++pgzero_ptr) {        
      pgzero_ptr->pz_dchain = 0L;
      pgzero_ptr->pz_next = 0L;
      pgzero_ptr->pz_timestamp = 0;
      pgzero_ptr->pz_modified = FALSE;
   }
   return( db_status = S_OKAY );
} /* dio_pzinit() */


/****************************************/
/*					*/
/*	       dio_pzflush		*/
/*					*/
/****************************************/
/* Flush page zero table
 * Complement to dio_out which writes all pages except page zero.
 */
static int dio_pzflush()
{
   register FILE_NO i;
   int desc;
   register PGZERO FAR *pgzero_ptr;
   register FILE_ENTRY FAR *file_ptr;
   LONG		align_LONG;
   char		*cptr;
   int		j;

#ifndef NO_TRANS
   if ( (dboptions & TRLOGGING) && trans_id && !trcommit && use_ovfl ) {
      /* flush to overflow/log file -- before tx commit */
      for (i = 0, pgzero_ptr = pgzero; i < size_ft; ++i, ++pgzero_ptr) {
	 if (pgzero_ptr->pz_modified ) 
	    if ( o_pzwrite( i ) != S_OKAY ) return( db_status );
      }
   }
   else {
#endif
      /* flush modified page zeroes to database files */
      for (i = 0, pgzero_ptr = pgzero, file_ptr = file_table; i < size_ft; 
	   ++i, ++pgzero_ptr, ++file_ptr) {
	 if ( pgzero_ptr->pz_modified ) {
	    if ( dio_open(i) != S_OKAY )
	       return( db_status );

	    /* The only byte swap operations necessary
	     * on a page zero are the first 5 LONG integers.
	     */
	    for (j = 0, cptr = (char *) pgzero_ptr;
		 j < PGZEROSZ/sizeof(LONG);
		 ++j, cptr += sizeof(LONG)) {
		memcpy (&align_LONG, cptr, sizeof(LONG));
		HTONL (align_LONG);
		memcpy (cptr, &align_LONG, sizeof(LONG));
	    }
	    desc = file_ptr->ft_desc;
	    DB_LSEEK(desc, 0L, 0);
	    if (DB_WRITE(desc, (char FAR *)pgzero_ptr, PGZEROSZ) != PGZEROSZ) 
	       return( dberr(S_BADWRITE) );
	    pgzero_ptr->pz_modified = FALSE;
#ifndef NO_TRANS
	    if ( trlog_flag )
	       d_trlog(i, 0, (char FAR *)pgzero_ptr,  PGZEROSZ);
#endif
	 }
#ifdef	 CLOSE_FILES
	 dio_close(i);
#endif
      }
#ifndef NO_TRANS
   }
#endif
   return( db_status = S_OKAY );
} /* dio_pzflush() */


/****************************************/
/*					*/
/*		dio_pzread		*/
/*					*/
/****************************************/
/* Read a file's page zero
*/
int
dio_pzread(fno)
FILE_NO fno;  /* file number */
{
   FILE_ENTRY FAR *file_ptr;
   PGZERO FAR *pgzero_ptr;

   pgzero_ptr = &pgzero[fno];
   file_ptr = &file_table[fno];

   /* open this file, if not already open */
   dio_open(fno);
   if (file_ptr->ft_desc < 0) {
      pgzero_ptr->pz_dchain =		0;
      pgzero_ptr->pz_next =		0;
      pgzero_ptr->pz_timestamp =	0;
      pgzero_ptr->pz_modified =		FALSE;
      return( db_status );	/* db_status set by dio_open */
   }

   /* seek to and read page zero */
   DB_LSEEK(file_ptr->ft_desc, 0L, 0);
   if ( DB_READ(file_ptr->ft_desc, (char FAR *)pgzero_ptr, PGZEROSZ) 
							!= PGZEROSZ ) {
      return( dberr(S_BADREAD) );
   }
   NTOHL (pgzero_ptr->pz_dchain);
   NTOHL (pgzero_ptr->pz_next);
   NTOHL (pgzero_ptr->pz_timestamp);
   return( db_status = S_OKAY );
} /* dio_pzread() */



/****************************************/
/*					*/
/*		dio_pzalloc		*/
/*					*/
/****************************************/
/* Allocate new record slot or key node from page zero,
 * ie from delete chain if possible.
 * Returns memory address of the free slot into 'loc'.
 */
int
dio_pzalloc(fno, loc )
FILE_NO fno;    /* file number */
F_ADDR *loc;    /* pointer to allocated location */
{
   DB_ADDR dba;
   F_ADDR pg;
   char FAR *ptr;
   PGZERO FAR *pgzero_ptr;

#ifndef SINGLE_USER
   /* check shared access priviledges */
   if ( dbopen == 1 && !trans_id && !excl_locks[fno] )
      return( dberr(S_NOTRANS) );
#endif

   pgzero_ptr = &pgzero[fno];
   if ( pgzero_ptr->pz_next == 0L )
      if ( dio_pzread(fno) != S_OKAY )
	 RETURN( db_status );
   if ( file_table[fno].ft_type == KEY ) {
      if ( working_file != fno )
	 return( dberr(S_NOWORK) );
      if ( pgzero_ptr->pz_dchain == NONE || ! (dboptions & DCHAINUSE) ) {
	 if ( pgzero_ptr->pz_next == MAXRECORDS-1 )
	    return( dberr(S_RECLIMIT) );
	 pg = pgzero_ptr->pz_next++;
      }
      else {
	 pg = pgzero_ptr->pz_dchain;
	 if ( dio_get( pg, (char FAR * FAR *)&ptr, NOPGHOLD ) != S_OKAY )
	    return( db_status );
	/* Get the first key node on the delete chain.
	 * (sizeof external timestamp set to 4 bytes)
	 ****** bytecpy(&pgzero_ptr->pz_dchain,
	 ****** ptr+sizeof(long)+sizeof(INT), sizeof(F_ADDR)); *********
	 */
	 bytecpy (&pgzero_ptr->pz_dchain,
	    ptr + sizeof(LONG) + sizeof(INT), sizeof(F_ADDR));
      }
   }
   else {
      if ( ! pgzero_ptr->pz_dchain || ! (dboptions & DCHAINUSE) ) {
	 if ( pgzero_ptr->pz_next == MAXRECORDS )
	    return( dberr(S_RECLIMIT) );
	 pg = pgzero_ptr->pz_next++;
      }
      else {
	 pg = pgzero_ptr->pz_dchain;
	 dba = ((NUM2EXT(fno, ft_offset) & FILEMASK) << FILESHIFT) | pg;
	 if ( dio_read(dba, (char FAR * FAR *)&ptr, NOPGHOLD) != S_OKAY )
	    return( db_status );
	 bytecpy(&pgzero_ptr->pz_dchain, ptr+sizeof(INT), sizeof(F_ADDR));
      }
   }
   *loc = pg;
   pgzero_ptr->pz_modified = TRUE;
   return( db_status = S_OKAY );
} /* dio_pzalloc() */


/****************************************/
/*					*/
/*		dio_pzdel		*/
/*					*/
/****************************************/
/* Delete record slot or key node from page zero
*/
int
dio_pzdel(fno, loc )
FILE_NO fno;  /* file number */
F_ADDR  loc;  /* location to be freed */
{
   DB_ADDR dba;
   INT recnum;
   char FAR *ptr;
   PGZERO FAR *pgzero_ptr;

#ifndef SINGLE_USER
   /* check shared access priviledges */
   if ( dbopen == 1 && !trans_id && !excl_locks[fno] )
      return( dberr(S_NOTRANS) );
#endif

   pgzero_ptr = &pgzero[fno];
   if ( pgzero_ptr->pz_next == 0L )
      if ( dio_pzread(fno) != S_OKAY )
	 RETURN( db_status );
   if ( file_table[fno].ft_type == KEY ) {
      if ( working_file != fno )
	 return( dberr(S_NOWORK) );
      if ( dio_get( loc, (char FAR * FAR *)&ptr, PGHOLD ) != S_OKAY )
	 return( db_status );
      /*********************************************
       * Delete chain ptr in key node page is in location
       * of orphan ptr, bytes 6 - 9, not bytes 4 - 7
       * as printed in raima User Guide.
       * (sizeof external timestamp set to 4 bytes).
       ****** bytecpy(ptr+sizeof(long)+sizeof(INT),
       ****** &pgzero_ptr->pz_dchain, sizeof(F_ADDR)); *********
       *****************************************/
      bytecpy (ptr + sizeof(LONG) + sizeof(INT),
	    &pgzero_ptr->pz_dchain, sizeof(F_ADDR));
      pgzero_ptr->pz_dchain = loc;
      dio_touch( loc );
   }
   else {
      dba = ((NUM2EXT(fno, ft_offset) & FILEMASK) << FILESHIFT) | loc;
      if ( dio_read( dba, (char FAR * FAR *)&ptr , NOPGHOLD) != S_OKAY )
	 return( db_status );
      bytecpy(&recnum, ptr, sizeof(INT));
      recnum = ~recnum;  /* indicates deleted record */
      bytecpy(ptr, &recnum, sizeof(INT));
      bytecpy(ptr+sizeof(INT), &pgzero_ptr->pz_dchain, sizeof(F_ADDR));
      pgzero_ptr->pz_dchain = loc;
      if ( dio_write(dba, NULL, NOPGFREE) != S_OKAY )
	 return( db_status );
   }
   pgzero_ptr->pz_modified = TRUE;
   return( db_status = S_OKAY );
} /* dio_pzdel() */


/****************************************/
/*					*/
/*		dio_pznext		*/
/*					*/
/****************************************/
/* Return pz_next for file fno
*/
F_ADDR dio_pznext(fno)
FILE_NO fno;
{
   if ( pgzero[fno].pz_next == 0L )
      dio_pzread(fno);
   return ( pgzero[fno].pz_next );
}

/****************************************/
/*					*/
/*		dio_pzclr		*/
/*					*/
/****************************************/
/* Clear page zero cache
*/
void dio_pzclr()
{
   register FILE_NO i;
   register PGZERO FAR *pgzero_ptr;

   for (i = 0, pgzero_ptr = pgzero; i < size_ft; i++, pgzero_ptr++) {
      if (pgzero_ptr->pz_modified) {
	 pgzero_ptr->pz_next = 0L;
	 pgzero_ptr->pz_modified = FALSE;
      }
   }
   return;
}
/* vpp -nOS2 -dUNIX -nBSD -nVANILLA_BSD -nVMS -nMEMLOCK -nWINDOWS -nFAR_ALLOC -f/usr/users/master/config/nonwin dio.c */
