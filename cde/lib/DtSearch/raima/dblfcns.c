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
 * License along with these libraries and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
/*
 *   COMPONENT_NAME: austext
 *
 *   FUNCTIONS: FL_LIST_ACCESS
 *              FL_LIST_DEACCESS
 *              Pi
 *              alloc_table
 *              bld_lock_tables
 *              d_close
 *              d_freeall
 *              d_keyfree
 *              d_keylock
 *              d_keylstat
 *              d_lock
 *              d_open
 *              d_recfree
 *              d_reclock
 *              d_reclstat
 *              d_retries
 *              d_rlbclr
 *              d_rlbset
 *              d_rlbtst
 *              d_setfree
 *              d_setlock
 *              d_setlstat
 *              d_timeout
 *              d_trabort
 *              d_trbegin
 *              d_trend
 *              free_files
 *              initdbt
 *              initses
 *              keep_locks
 *              lock_files
 *              neterr
 *              pr_lock_descr
 *              process_lock
 *              recovery_check
 *              reset_locks
 *              send_free
 *              send_lock
 *              taskinit
 *              termfree
 *              termses
 *
 *   ORIGINS: 27,157
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
/*-----------------------------------------------------------------------=
 $XConsortium: dblfcns.c /main/6 1996/11/25 18:48:05 drk $
   dblfcns -- Database Access & Locking Functions

   This file contains functions which open/close a
   db_VISTA database and manage multiuser access
   to the db_VISTA database files

   (C) Copyright 1985, 1986, 1987 by Raima Corp.
-----------------------------------------------------------------------*/

/* ********************** EDIT HISTORY *******************************

 SCR    DATE    INI                   DESCRIPTION
----- --------- --- -----------------------------------------------------
   76 16-Jun-88 RSC Make dblfcns consistent when SINGLE_USER defined
  240 17-Jun-88 RSC Make dblfcns consistent when NO_TRANS defined
  103 24-Jun-88 RSC Improve generation of single user version
  237 29-Jun-88 RSC Do not permit recfree/setfree/keyfree inside transaction
  272 29-Jun-88 RSC make sure log entry is added to taf after lockmgr, and
		    deleted before lockmgr
  305 01-Jul-88 RSC clear cache_ovfl flag after d_trend and d_trabort
  ??? 06-Jul_88 WLW include GENERAL lockmgr changes
  353 14-Jul-88 RSC place dio_clear outside ifndef SINGLE_USER in d_trabort
  367 14-Jul-88 RSC initialize prev lock to 'f' in bld_lock_tables
  115 18-Jul-88 RSC Integrate VAX/VMS changes into master source
   76 27-Jul-88 RSC More work making dblfcns work with SINGLE_USER
      03-AUG-88 RTK MULTI_TASKing changes
  310 10-Aug-88 RSC Cleanup of function prototypes
      11-Aug-88 RTK Incremental database open/multi-tasking changes
      18-Aug-88 RSC Moved rn_type/dba to separate table
  423 10-Sep-88 RSC Allocated wrong size for rec_locks
  423 12-Sep-88 RSC Moved o_free above termfree
  423 15-Sep-88 RSC Initialized no_of_dbs to 1 in d_close
  424 21-Sep-88 RSC Integrated International Character Set (ESM)
      04-Oct-88 WLW Removed taskinit call from d_close, replaced with init's
  425 05-Oct-88 RSC d_trabort was not completely clearing page zero
      05-Oct-88 RSC must also init no_of_dbs = 1 in d_close (cf 04-Oct above)
  420 07-Oct-88 RSC Unoptimized usage of fl_list - was full of bugs.
      11-Oct-88 RSC Fix for clean compile under Lattice 3.3
  441 08-Dec-88 RSC Place call to netbios_chk within ifndef GENERAL
		    Placed setting of inode/device within ifndef GENERAL
		    Removed undef UNIX / define MSC inside GENERAL
  440 13-Dec-88 RSC Removed LR_LOCK lock_reply from db_global to scalar
  440 22-Dec-88 RSC More modifications for General Lockmgr
  539 18-Jan-89 RSC General Lockmgr was broke when open mode = one user
  420 24-Jan-89 WLW Added ifdef's for SINGLE_USER in lock sets
  571 27-Jan-89 RSC Remove defn of taf_close - General lockmgr lattice port issue
  420 14-Feb-89 WLW Corrected KEYMARK handling in d_lock and lock_files
  637 08-Mar-89 RSC Should not alloc file_refs in exclusive access, wasn't
		    Freeing fl_list of key_locks
  713 08-May-89 WLW Make external recovery work for single-user and one-user
  656 08-May-89 WLW Eliminate assignment to unallocated memory with gen lm.

 $Log$
 * Revision 1.2  1995/10/13  18:44:53  miker
 * Change hardcoded dbfile[] size from 48 to DtSrFILENMLEN.
 * Remove call to initenv()--disregard environment variables.
 */

/* To work with the General Lock Manager, the Unix case of using inode
   number and device to identify a file is not used.
*/
#ifdef GENERAL
#define IS_UNIX_REALLY
#endif

#define DEBUG_DBLF
int     debugging_dopen = 0;	/* 1 = TRUE */

#include <stdio.h>
#include "vista.h"
#include "dbtype.h"

#ifdef IS_UNIX_REALLY
#undef DIRCHAR
#define DIRCHAR '/'
#endif

#define KEYMARK 30000

#define send_pkt (Send_pkt.ptr)
#define recv_pkt (Recv_pkt.ptr)

TASK db_global = { 0 };
int db_glob_init = 0;

/* As a quick fix to the General Lockmgr, the structure lock_reply was
   removed from db_global.  However, this assumes that db_VISTA would
   never be preempted in the multi-tasking version, and that all function
   calls would complete before a new task is run.  If this assumption is
   ever "broken" then lock_reply will need to be placed back within db_global
   again */

extern CHAR_P Dbpgbuff;  /* allocated by dio_init used by o_update */
extern LOOKUP_ENTRY_P Db_lookup; /* database page lookup table */
extern PAGE_ENTRY_P Dbpg_table; /* database page table */
extern LOOKUP_ENTRY_P Ix_lookup; /* index page lookup table */
extern PAGE_ENTRY_P Ixpg_table; /* index page table */
extern INT_P Used_files;



#define lsn (db_global.Lsn)

BOOLEAN trcommit = FALSE;
int db_txtest = 0;          /* transaction commit failure testing flag */

#define FL_LIST_ACCESS(ld_ptr)   (FILE_NO *)(ld_ptr)->fl_list.ptr
#define FL_LIST_DEACCESS(ld_ptr) /**/



/* Internal function prototypes */
static int bld_lock_tables(void);
static int initses(void);
static int lock_files(int, LOCK_REQUEST *);
static int send_lock(void);
static int send_free(void);
static void reset_locks(void);
static int recovery_check(void);


/* Open db_VISTA database
*/
int
d_open(const char *dbnames, const char *opentype)
{
   DB_ENTER(NO_DB_ID TASK_ID LOCK_SET(LOCK_ALL));
#ifdef DEBUG_DBLF
   if (debugging_dopen) {
	puts (__FILE__"265 d_open");
	fflush(stdout);
   }
#endif

   if ( dbopen ) d_close();



#ifdef MIKER /**@@@***/
   /* initialize the country table if "vista.ctb" exists */
   if ( ctb_init() != S_OKAY )
      RETURN( db_status );
#endif


   /* initialize multi-db tables */
   if ( initdbt(dbnames) != S_OKAY ) RETURN( db_status );

   /* read in schema tables */
   if ( inittab() != S_OKAY ) RETURN( db_status );

#ifdef DEBUG_DBLF
   if (debugging_dopen) {
	puts(__FILE__"324 d_open calling renfiles");
	fflush(stdout);
   }
#endif
   if ( renfiles() != S_OKAY ) RETURN( db_status );

      dbopen = 2;

#ifdef DEBUG_DBLF
   if (debugging_dopen) {
	    printf(__FILE__"392 d_open before key_open. pgsz=%hd lrgst=%hd\n",
		page_size,largest_page);
	    fflush(stdout);
   }
#endif
   if ( key_open() == S_OKAY ) {
      if ( dio_init() == S_OKAY ) {
	 RETURN( db_status );
      }
   }
   dbopen = 0;
#ifdef DEBUG_DBLF
   if (debugging_dopen) {
	    printf(__FILE__"404 d_open after dio_init. pgsz=%hd lrgst=%hd\n",
		page_size,largest_page);
	    fflush(stdout);
   }
#endif
   RETURN( db_status );
} /* d_open() */


/* Initialize a task structure
*/
int taskinit(TASK *tsk)
{
   byteset(tsk, '\0', sizeof(TASK));
   tsk->No_of_dbs = 1;
   tsk->Dboptions = DCHAINUSE;
   return( db_status );
}


/* Initialize multiple database table entries
*/
int
initdbt(const char *dbnames)
{
   int dbt_lc;			/* loop control */
   char dbfile [DtSrFILENMLEN];
   char *ptr;
   const char *cp;
   int i;

   /* compute number of databases to be opened */
   old_no_of_dbs = (( no_of_dbs == 1 ) ? 0 : no_of_dbs);
   for ( cp = dbnames; *cp; ++cp )
      if ( *cp == ';' ) ++no_of_dbs;
#ifdef DEBUG_DBLF
   if (debugging_dopen) {
	printf(__FILE__"457 initdbt: new#dbs=%d\n", (int)no_of_dbs);
	fflush(stdout);
   }
#endif

   /* Now make sure there are the right # of elements in dbd/dbfpath */
   if (dbdpath[0]) {
      if (get_element(dbdpath,no_of_dbs-1) == NULL) 	/* Not enuf? */
	 return( dberr(S_BADBDPATH) );
      if (strrchr(dbdpath,';') != NULL)		/* Is dbdpath single element */
	 if (get_element(dbdpath,no_of_dbs) != NULL) 	/* Too many? */
	    return( dberr(S_BADBDPATH) );
   }
   if (dbfpath[0]) {
      if (get_element(dbfpath,no_of_dbs-1) == NULL)	/* Not enuf? */
	 return( dberr(S_BADBFPATH) );
      if (strrchr(dbfpath,';') != NULL) {	/* Is dbfpath single element */
	 if (get_element(dbfpath,no_of_dbs) != NULL)	/* Too many? */
	    return( dberr(S_BADBFPATH) );
      }
   }

   /* allocate db_table space */
   /* Macro references must be on one line for some compilers */ 
   if ((ALLOC_TABLE(&db_global.Db_table, no_of_dbs*sizeof(DB_ENTRY), 
		    old_no_of_dbs*sizeof(DB_ENTRY), "db_table") != S_OKAY) ||
       (ALLOC_TABLE(&db_global.Rn_table, no_of_dbs*sizeof(RN_ENTRY), 
		    old_no_of_dbs*sizeof(RN_ENTRY), "rn_table") != S_OKAY)) {
      return( db_status );
   }

   /* initialize db_table entries */
   for (dbt_lc = no_of_dbs, cp = dbnames, 
	   curr_db_table = &db_table[old_no_of_dbs]; 
	--dbt_lc >= 0; 
	++cp, ++curr_db_table) {
      /* extract database name */
      for ( i = 0; *cp && *cp != ';'; ++cp, ++i )
	 dbfile[i] = *cp;
      dbfile[i] = '\0';
      if ( (ptr = strrchr(dbfile, DIRCHAR)) == NULL ) 
	 ptr = strrchr(dbfile, ':');
      if ( ptr ) {
	 if ( strlen(ptr+1) >= DBNMLEN ) RETURN( dberr(S_NAMELEN) );
	 strcpy(DB_REF(db_name), ptr+1);
	 *(ptr+1) = '\0';
	 if ( strlen(dbfile) >= PATHLEN ) RETURN( dberr(S_NAMELEN) );
	 strcpy(DB_REF(db_path), dbfile);
      }
      else {
	 strcpy(DB_REF(db_path), "");
	 strcpy(DB_REF(db_name), dbfile);
      }
   }
   return( db_status = S_OKAY );
} /* initdbt() */









/****************************************/
/*					*/
/*		d_close			*/
/*					*/
/****************************************/
/* Close database
*/
int
d_close(void)
{
   int i;

   DB_ENTER(NO_DB_ID TASK_ID LOCK_SET(LOCK_ALL));

   if ( dbopen ) {
      db_status = S_OKAY;
	    dio_flush(); 

      for (i = 0; i < size_ft; ++i) {
	 /* close all files */
	 dio_close(i); 
      }

#ifdef MIKER /**@@@***/
   /* free the country table */
   if ( db_global.ctbl_activ )
      ctbl_free();
#endif

/*    termfree();
      key_close();
      sk_free();
      dio_free(); */
   }
   if ( dbopen ) {
      setdb_on = FALSE;
      curr_db = 0;
      no_of_dbs = 1;
      db_status = S_OKAY;
      curr_rec = NULL_DBA;
      size_ft = 0;
      size_rt = 0;
      size_fd = 0;
      size_st = 0;
      size_mt = 0;
      size_srt = 0;
      size_kt = 0;
      no_of_keys = 0;
      dbopen = 0;
   }
   RETURN( db_status );
} /* d_close() */






/* Free all allocated memory upon termination
*/
void termfree(void)
{

   /* free all allocated memory */
   if ( curr_mem ) {
      MEM_UNLOCK(&db_global.Curr_mem);
      FREE(&db_global.Curr_mem);
   }
   if ( curr_own ) {
      MEM_UNLOCK(&db_global.Curr_own);
      FREE(&db_global.Curr_own);
   }
   if ( sort_table ) {
      MEM_UNLOCK(&db_global.Sort_table);
      FREE(&db_global.Sort_table);
   }
   if ( member_table ) {
      MEM_UNLOCK(&db_global.Member_table);
      FREE(&db_global.Member_table);
   }
   if ( set_table ) {
      MEM_UNLOCK(&db_global.Set_table);
      FREE(&db_global.Set_table);
   }
   if ( field_table ) {
      MEM_UNLOCK(&db_global.Field_table);
      FREE(&db_global.Field_table);
   }
   if ( key_table ) {
      MEM_UNLOCK(&db_global.Key_table);
      FREE(&db_global.Key_table);
   }
   if ( record_table ) {
      MEM_UNLOCK(&db_global.Record_table);
      FREE(&db_global.Record_table);
   }
   if ( file_table ) {
      MEM_UNLOCK(&db_global.File_table);
      FREE(&db_global.File_table);
   }
   if ( db_table ) {
      MEM_UNLOCK(&db_global.Db_table);
      FREE(&db_global.Db_table);
   }
   if ( rn_table ) {
      MEM_UNLOCK(&db_global.Rn_table);
      FREE(&db_global.Rn_table);
   }
}

int alloc_table(
CHAR_P *Table,
#define table Table->ptr
unsigned new_size,
unsigned old_size
)
{
   CHAR_P Temp_table;

   Temp_table.ptr = ALLOC(&Temp_table, new_size, varname);
   if ( Temp_table.ptr == NULL ) {
      return( dberr(S_NOMEMORY) );
   }
   byteset(&Temp_table.ptr[old_size], 0, new_size - old_size);
   if ( old_size ) {
      bytecpy(Temp_table.ptr, table, old_size);
      MEM_UNLOCK(Table);
      FREE(Table);
   }
   *Table = Temp_table;
   return( db_status );
}
/* vpp -nOS2 -dUNIX -nBSD -nVANILLA_BSD -nVMS -nMEMLOCK -nWINDOWS -nFAR_ALLOC -f/usr/users/master/config/nonwin dblfcns.c */
