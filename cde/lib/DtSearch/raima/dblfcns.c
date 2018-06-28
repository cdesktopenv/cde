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
#ifndef SINGLE_USER
static LR_LOCK lock_reply;		/* This used to be in db_global */
#endif

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
#ifndef SINGLE_USER
static void pr_lock_descr(P1(struct lock_descr *) Pi(int) 
					    Pi(const char *));
static int process_lock(P1(struct lock_descr *) Pi(char));
static int keep_locks(P1(struct lock_descr *));
static int free_files(P1(struct lock_descr *));
#endif
static int bld_lock_tables(P0);
static int initses(P0);
static int lock_files(P1(int) Pi(LOCK_REQUEST *));
static int send_lock(P0);
static int send_free(P0);
static void reset_locks(P0);
static int recovery_check(P0);








/* Open db_VISTA database
*/
int
d_open(dbnames, opentype TASK_PARM)
const char *dbnames;
const char *opentype;
TASK_DECL
{
   DB_ENTER(NO_DB_ID TASK_ID LOCK_SET(LOCK_ALL));
#ifdef DEBUG_DBLF
   if (debugging_dopen) {
	puts (__FILE__"265 d_open");
	fflush(stdout);
   }
#endif

   if ( dbopen ) d_close(TASK_ONLY);

#ifndef SINGLE_USER
   if ( opentype ) {
      switch ( *opentype ) {
	 case 's':
	 case 'x':
#ifndef GENERAL
	    if ( netbios_chk() == 0 )
		RETURN( dberr( S_NONETBIOS ) );
#endif
	    db_lockmgr = 1;
	    strcpy(type, opentype);
	    break;
	 case 'n':
	 case 't':
	 case 'o':
	    db_lockmgr = 0;
	    strcpy(type, "x");
	    break;
	 default:
	    RETURN( dberr(S_BADTYPE) );
      }
   }
   else
      strcpy(type, "x");
#endif


#ifdef MIKER /**@@@***/
#ifndef NO_COUNTRY
   /* initialize the country table if "vista.ctb" exists */
   if ( ctb_init() != S_OKAY )
      RETURN( db_status );
#endif
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

#ifndef SINGLE_USER

   if ( db_lockmgr ) {	/* [637] Only alloc file_refs for shared open */
      /* Macro references must be on one line for some compilers */ 
      if (ALLOC_TABLE(&db_global.File_refs, size_ft*sizeof(FILE_NO),old_size_ft*sizeof(FILE_NO), "file_refs")
		!=  S_OKAY) {
	 RETURN( db_status );
      }
   }

   if ( *type == 's' ) {
      /* build application file lock tables */
      if ( bld_lock_tables() != S_OKAY )
	 RETURN( db_status );
      dbopen = 1;
   }
   else
#endif
      dbopen = 2;
#ifndef SINGLE_USER
#ifndef GENERAL
   if ( db_lockmgr ) {
#endif
      if ( initses() != S_OKAY ) {
	 dbopen = 0;
	 RETURN( db_status );
      }
#ifndef GENERAL
   }	/* [713] perform external recovery in one-user mode */
   else
      if ( recovery_check() != S_OKAY ) RETURN(db_status);
#endif
#else
#endif

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
int taskinit(tsk)
TASK *tsk;
{
   byteset(tsk, '\0', sizeof(TASK));
   tsk->No_of_dbs = 1;
#ifndef SINGLE_USER
   tsk->Lock_tries = 5;
   tsk->Dbwait_time = 1;
   tsk->Db_timeout = TIMEOUT_DEF;
   tsk->Db_lockmgr = 1;
#endif
   tsk->Dboptions = DCHAINUSE;
   return( db_status );
}


/* Initialize multiple database table entries
*/
int
initdbt(dbnames )
const char *dbnames;
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






#ifndef	SINGLE_USER
/* Initial lock manager session
*/
static int initses()
{
   LM_DBOPEN_P Send_pkt;
   LR_DBOPEN_P Recv_pkt;
   int ft_lc;			/* loop control */
   LM_TREND trend_pkt;
   int send_size, recv_size, recvd_sz;
   struct stat stbuf;
   LM_FILEID *fi_ptr;
   FILE_ENTRY *file_ptr;
   FILE_NO *fref_ptr;
   INT *rcv_fref_ptr;

   if ( (net_status=nw_addnm(dbuserid, (int *)NULL) ) != N_OKAY )
      if ( net_status == N_DUPNAME ) {
	 /* It is okay to reuse this name, but first be sure that all
	    sessions are hung up.
	 */
	 nw_cleanup(dbuserid);
      }
      else
	 return( neterr() );

   if ( nw_call("lockmgr", dbuserid, &lsn) ) {
      return( neterr() );
   }
   db_timeout = TIMEOUT_DEF;  /* reset default timeout value */

#ifdef GENERAL

   /* This section of code MUST be identical to else (DOS) below */
   send_size = 0;
   for (ft_lc = size_ft - old_size_ft, file_ptr = &file_table[old_size_ft];
	--ft_lc >= 0; ++file_ptr) 
      send_size += strlen(file_ptr->ft_name) + 1;
   send_size += sizeof(LM_DBOPEN);
   send_size += send_size % 2;
#else				/* GENERAL */
   send_size = sizeof(LM_DBOPEN) + (size_ft-1)*sizeof(LM_FILEID);
#endif				/* GENERAL */
   send_pkt = (LM_DBOPEN *)ALLOC(&Send_pkt, send_size, "send_pkt");
   recv_size = sizeof(LR_DBOPEN) + (size_ft-1)*sizeof(INT);
   recv_pkt = (LR_DBOPEN *)ALLOC(&Recv_pkt, recv_size, "recv_pkt");
   if (send_pkt == NULL || recv_pkt == NULL) {
      nw_hangup(lsn);
      return(dberr(S_NOMEMORY));
   }

   send_pkt->fcn = L_DBOPEN;
   send_pkt->nfiles = size_ft;
   send_pkt->type = type[0];
   for (ft_lc = size_ft - old_size_ft, file_ptr = &file_table[old_size_ft],
						fi_ptr = send_pkt->fnames;
#ifdef GENERAL
        --ft_lc >= 0;  fi_ptr += strlen(file_ptr->ft_name)+1,++file_ptr) {
#else
	--ft_lc >= 0;  ++fi_ptr,++file_ptr) {
#endif
      if (stat(file_ptr->ft_name, &stbuf) == -1) {
	 nw_hangup(lsn);
	 return(dberr(S_NOFILE));
      }
#ifndef GENERAL
      fi_ptr->inode = stbuf.st_ino;
      fi_ptr->device = stbuf.st_dev;
#else
      strcpy(fi_ptr,file_ptr->ft_name);
#endif
   }
send_open:
   if (nw_send(lsn, (MESSAGE *)send_pkt, send_size) ||
       nw_rcvmsg(lsn, (MESSAGE *)recv_pkt, recv_size, &recvd_sz)) {
      nw_hangup(lsn);
      return(neterr());
   }

   if ( recv_pkt->status == L_RECOVER )  {
      /* perform auto-recovery */
      d_recover( (const char *)recv_pkt->logfile CURRTASK_PARM );

      /* tell lock mgr we're done */
      trend_pkt.fcn = L_RECDONE;
      if ( nw_send(lsn, (MESSAGE *)&trend_pkt, sizeof(LM_TREND)) ) {
	 nw_hangup(lsn);
	 return(neterr());
      }
      /* re-issue open request */
      goto send_open;
   }
   if ( recv_pkt->fcn != L_DBOPEN ) {
      nw_hangup(lsn);
      return(dberr(S_NETSYNC));
   }
   if ( recv_pkt->status != L_OKAY ) {
      nw_hangup(lsn);
      nw_sestat();
#ifndef GENERAL
      taf_close();
#endif
      termfree();
      MEM_UNLOCK(&Send_pkt);
      FREE(&Send_pkt);
      MEM_UNLOCK(&Recv_pkt);
      FREE(&Recv_pkt);
      dbopen = 0;
      return(db_status = S_UNAVAIL);
   }
   if ( recv_pkt->nusers == 1 ) 
      if ( recovery_check() != S_OKAY ) {
	 nw_hangup(lsn);
	 return(db_status);
      }

   /* [656] perform initialization if not general lockmgr */
   if ( db_lockmgr ) {
      for (ft_lc = size_ft - old_size_ft, fref_ptr = &file_refs[old_size_ft],
					      rcv_fref_ptr = recv_pkt->frefs;
	   --ft_lc >= 0; ++fref_ptr, ++rcv_fref_ptr) {
	 *fref_ptr = *rcv_fref_ptr;
      }
   }
   MEM_UNLOCK(&Send_pkt);
   FREE(&Send_pkt);
   MEM_UNLOCK(&Recv_pkt);
   FREE(&Recv_pkt);

   session_active = TRUE;
   return(db_status = S_OKAY);
}
#endif



/****************************************/
/*					*/
/*		d_close			*/
/*					*/
/****************************************/
/* Close database
*/
int
d_close(TASK_ONLY)
TASK_DECL
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
#ifndef NO_COUNTRY
   /* free the country table */
   if ( db_global.ctbl_activ )
      ctbl_free();
#endif
#endif

#ifndef SINGLE_USER
      d_freeall(TASK_ONLY);
#endif
/*    termfree();
      key_close();
      sk_free();
      dio_free(); */
#ifndef SINGLE_USER
      if ( db_lockmgr ) {
	 termses();
      }
#endif
   }
   if ( dbopen ) {
#ifndef NO_TIMESTAMP
      cr_time = 0;
#endif
      setdb_on = FALSE;
      curr_db = 0;
      no_of_dbs = 1;
#ifndef SINGLE_USER
      lock_tries = 5;
      dbwait_time = 1;
      db_lockmgr = 1;
      session_active = FALSE;
#endif
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



#ifndef SINGLE_USER
/* Terminate lock manager session
*/
termses()
{
   LM_DBCLOSE_P Send_pkt;
   int ft_lc;			/* loop control */
   int send_size;
   FILE_NO *fref_ptr;
   INT *snd_fref_ptr;

   if ( session_active ) {
      send_size = sizeof(LM_DBCLOSE) + (size_ft-1)*sizeof(INT);
      send_pkt = (LM_DBCLOSE *)ALLOC(&Send_pkt, send_size, "send_pkt");
      if ( send_pkt == NULL ) return( dberr(S_NOMEMORY) );
      send_pkt->fcn = L_DBCLOSE;
      send_pkt->nfiles = size_ft;
      for (ft_lc = size_ft, fref_ptr = file_refs,
						snd_fref_ptr = send_pkt->frefs;
	   --ft_lc >= 0; ++fref_ptr, ++snd_fref_ptr)
	 *snd_fref_ptr = *fref_ptr;
      if ( nw_send(lsn, (MESSAGE *)send_pkt, send_size) )
	 return( neterr() );

      nw_hangup(lsn);
      nw_sestat();
      MEM_UNLOCK(&Send_pkt);
      FREE(&Send_pkt);
      MEM_UNLOCK(&db_global.File_refs);
      FREE(&db_global.File_refs);
      session_active = FALSE;
   }
   return( db_status = S_OKAY );
}
#endif



/* Free all allocated memory upon termination
*/
void termfree()
{
#ifndef SINGLE_USER
   int i;
   struct lock_descr *ld_ptr;
#endif

   /* free all allocated memory */
   if ( curr_mem ) {
      MEM_UNLOCK(&db_global.Curr_mem);
      FREE(&db_global.Curr_mem);
   }
   if ( curr_own ) {
      MEM_UNLOCK(&db_global.Curr_own);
      FREE(&db_global.Curr_own);
   }
#ifndef	 NO_TIMESTAMP
   if ( co_time ) {
      MEM_UNLOCK(&db_global.Co_time);
      FREE(&db_global.Co_time);
   }
   if ( cm_time ) {
      MEM_UNLOCK(&db_global.Cm_time);
      FREE(&db_global.Cm_time);
   }
   if ( cs_time ) {
      MEM_UNLOCK(&db_global.Cs_time);
      FREE(&db_global.Cs_time);
   }
#endif
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
#ifndef SINGLE_USER
   if ( app_locks ) {
      MEM_UNLOCK(&db_global.App_locks);
      FREE(&db_global.App_locks);
   }
   if ( excl_locks ) {
      MEM_UNLOCK(&db_global.Excl_locks);
      FREE(&db_global.Excl_locks);
   }
   if ( kept_locks ) {
      MEM_UNLOCK(&db_global.Kept_locks);
      FREE(&db_global.Kept_locks);
   }
   if ( rec_locks ) {
      for (i = 0, ld_ptr = rec_locks; i < size_rt; ++i, ++ld_ptr) {
	 MEM_UNLOCK(&ld_ptr->fl_list);
	 FREE(&ld_ptr->fl_list);
      }
      MEM_UNLOCK(&db_global.Rec_locks);
      FREE(&db_global.Rec_locks);
   }
   if ( set_locks ) {
      for (i = 0, ld_ptr = set_locks; i < size_st; ++i, ++ld_ptr) {
	 MEM_UNLOCK(&ld_ptr->fl_list);
	 FREE(&ld_ptr->fl_list);
      }
      MEM_UNLOCK(&db_global.Set_locks);
      FREE(&db_global.Set_locks);
   }
   if ( key_locks ) {
      for (i = 0, ld_ptr = key_locks; i < keyl_cnt; ++i, ++ld_ptr) { /*[637]*/
	 MEM_UNLOCK(&ld_ptr->fl_list);
	 FREE(&ld_ptr->fl_list);
      }
      MEM_UNLOCK(&db_global.Key_locks);
      FREE(&db_global.Key_locks);
   }
   if ( lock_pkt ) {
      MEM_UNLOCK(&db_global.Lock_pkt);
      FREE(&db_global.Lock_pkt);
   }
   if ( free_pkt ) {
      MEM_UNLOCK(&db_global.Free_pkt);
      FREE(&db_global.Free_pkt);
   }
#endif
   if ( db_table ) {
      MEM_UNLOCK(&db_global.Db_table);
      FREE(&db_global.Db_table);
   }
   if ( rn_table ) {
      MEM_UNLOCK(&db_global.Rn_table);
      FREE(&db_global.Rn_table);
   }
}










#ifndef SINGLE_USER
/* Process set/record lock
*/
static process_lock(ld_ptr, type )
struct lock_descr *ld_ptr;
char type;
{
   int fl_lc;			/* loop control */
   int fno;
   int i;
   LM_LOCKREQ *lockreq_ptr;
   FILE_NO *fl_ptr, fref;

   db_status = S_OKAY;
   ld_ptr->fl_prev = ld_ptr->fl_type;
   switch( type ) {
      case 'k':
	 if ( !trans_id )  
	    dberr( S_TRNOTACT );
	 else if ( ld_ptr->fl_prev == 'f' ) 
	    dberr( S_NOTLOCKED );
	 else if ( ld_ptr->fl_prev != 'x' ) 
	    return( keep_locks(ld_ptr) );
	 break;
      case 'r':
	 if( ld_ptr->fl_prev != 'f' ) 
	    dberr( S_NOTFREE );
	 else 
	    ld_ptr->fl_type = 'r';
	 break;
      case 'w':
	 if ( !trans_id )
	    dberr( S_TRNOTACT );
	 else if ( ld_ptr->fl_prev != 'f' && ld_ptr->fl_prev != 'r' ) 
	    dberr( S_NOTFREE );
	 else
	    ld_ptr->fl_type = 'w';
	 break;
      case 'x':
	 if ( ld_ptr->fl_prev != 'f' && ld_ptr->fl_prev != 'r' ) 
	    dberr(S_NOTFREE);
	 else
	    ld_ptr->fl_type = 'x';
	 break;
      default:  
	 dberr( S_BADTYPE );
   }
   if ( db_status == S_OKAY ) {
      /* build lock request packet */
      for (fl_lc = ld_ptr->fl_cnt, fl_ptr = FL_LIST_ACCESS(ld_ptr);
	   --fl_lc >= 0; ++fl_ptr) {
	 fref = file_refs[fno = *fl_ptr];
	 for (i = 0, lockreq_ptr = lock_pkt->locks;
	      (i < lock_pkt->nfiles) && (lockreq_ptr->fref != fref);
	      ++i, ++lockreq_ptr)
	    ;				/* null statement */

	 if (i < lock_pkt->nfiles) {
	    /* file already is in lock request packet */
	    if ( lockreq_ptr->type == 'r' || ld_ptr->fl_type == 'x' )
	       lockreq_ptr->type = ld_ptr->fl_type;
	 }
	 else if ( !excl_locks[fno] && ( !app_locks[fno] || 
	      (ld_ptr->fl_type == 'w' && app_locks[fno] > 0) ||
	      (ld_ptr->fl_type == 'x') ) ) {
	    /* add to lock request packet */
	    ++lock_pkt->nfiles;
	    lockreq_ptr->fref = fref;
	    lockreq_ptr->type = ld_ptr->fl_type;
	 }
      }
      FL_LIST_DEACCESS(ld_ptr);
   }
   return( db_status );
}
#endif








#ifndef SINGLE_USER
/* Setup table to keep locks after transaction end
*/
static keep_locks( ld_ptr )
struct lock_descr *ld_ptr;      /* Lock descriptor */
{
   int fl_lc;			/* loop control */
   FILE_NO *fl_ptr;

   /* Mark lock as kept */
   ld_ptr->fl_kept = TRUE;                

   for (fl_lc = ld_ptr->fl_cnt, fl_ptr = FL_LIST_ACCESS(ld_ptr);
	--fl_lc >= 0; ++fl_ptr)
      ++kept_locks[*fl_ptr];
   FL_LIST_DEACCESS(ld_ptr);

   return( db_status = S_OKAY );
}
#endif





#ifndef SINGLE_USER
/* Free read-locked files associated with record or set
*/
static int free_files(ld_ptr)
struct lock_descr *ld_ptr;
{
   int fl_lc;			/* loop control */
   FILE_NO fno;
   LM_LOCKREQ *lockreq_ptr;
   int *appl_ptr;
   FILE_NO fref;
   FILE_NO *fl_ptr;

   /* fill free packet */
   lock_pkt->nfiles = free_pkt->nfiles = 0; 
   for (fl_lc = ld_ptr->fl_cnt, fl_ptr = FL_LIST_ACCESS(ld_ptr);
	--fl_lc >= 0; ++fl_ptr) {
      fno = *fl_ptr;
      appl_ptr = &app_locks[fno];
      fref = file_refs[fno];
      if ( ld_ptr->fl_type == 'r' && *appl_ptr > 0 ) {
	 /* free read lock */
	 if ( --*appl_ptr == 0 && excl_locks[fno] == 0 ) {
	    free_pkt->frefs[free_pkt->nfiles++] = fref;
	    /* reset key scan position */
	    if ( file_table[fno].ft_type == 'k' )
	       key_reset(fno);
	 }
      }
      else if ( --excl_locks[fno] == 0 ) {
	 /* free exclusive access lock */
	 if ( *appl_ptr > 0 ) {
	    /* downgrade to read-lock */
	    lockreq_ptr = &lock_pkt->locks[lock_pkt->nfiles++];
	    lockreq_ptr->type = 'r';
	    lockreq_ptr->fref = fref;
	 }
	 else {
	    /* free excl-lock */
	    free_pkt->frefs[free_pkt->nfiles++] = fref;
	    dio_flush();
	    /* reset key scan position */
	    if ( file_table[fno].ft_type == 'k' )
	       key_reset(fno);
	 }
      }
      if ( ld_ptr->fl_kept ) {             
	 /* Remove hold on lock */
	 if ( --kept_locks[fno] < 0 ) return( dberr(S_BADLOCKS) );
	 ld_ptr->fl_kept = FALSE; 
      }
   }
   FL_LIST_DEACCESS(ld_ptr);
   /* send any downgrades */
   if ( send_lock() == S_OKAY ) {
      /* free any files */
      send_free();
   }
   return( db_status );
}
#endif



#ifndef SINGLE_USER
/* Reset lock descriptor tables
*/
static void reset_locks()
{
   int beg, end;
   int i;
   struct lock_descr *ld_ptr;

   /* reset record lock descriptors */
   beg = 0;
   end = size_rt;
   for (i = beg, ld_ptr = &rec_locks[i]; i < end; ++i, ++ld_ptr) {
      if ( ld_ptr->fl_kept ) {
	 ld_ptr->fl_type = 'r';
	 ld_ptr->fl_kept = FALSE;
      }
      else if ( ld_ptr->fl_type != 'x' )
	 ld_ptr->fl_type = 'f';
   }
   /* reset set lock descriptors */
   beg = 0;
   end = size_st;
   for (i = beg, ld_ptr = &set_locks[i]; i < end; ++i, ++ld_ptr) {
      if ( ld_ptr->fl_kept ) {
	 ld_ptr->fl_type = 'r';
	 ld_ptr->fl_kept = FALSE;
      }
      else if ( ld_ptr->fl_type != 'x' )
	 ld_ptr->fl_type = 'f';
   }
   /* reset key lock descriptors */
   beg = 0;
   end = keyl_cnt;
   for (i = beg, ld_ptr = &key_locks[i]; i < end; ++i, ++ld_ptr) {
      if ( ld_ptr->fl_kept ) {
	 ld_ptr->fl_type = 'r';
	 ld_ptr->fl_kept = FALSE;
      }
      else if ( ld_ptr->fl_type != 'x' )
	 ld_ptr->fl_type = 'f';
   }
}
#endif














#ifndef SINGLE_USER
/* Report a network error
*/
neterr()
{
    switch ( net_status ) {
	case N_OPENREJ:
	    db_status = dberr( S_LMBUSY );
	    break;
	case N_CALLNAME:
	    db_status = dberr( S_NOLOCKMGR );
	    break;
	case N_NAMEUSED:
	    db_status = dberr( S_DUPUSERID );
	    break;
	default:
	    db_status = dberr( S_NETERR );
	    break;
    }
    return( db_status );
}
#endif

int alloc_table(Table, new_size, old_size )
CHAR_P *Table;
#define table Table->ptr
unsigned new_size;
unsigned old_size;
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
