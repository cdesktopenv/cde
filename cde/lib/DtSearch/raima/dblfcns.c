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

static LR_LOCK lock_reply;		/* This used to be in db_global */

#ifdef MULTI_TASK
DB_TASK Currtask = {POINTER_ASSIGN((TASK FAR *)&db_global), POINTER_ASSIGN((char FAR *)NULL)};
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

#ifndef NO_TRANS
#ifndef SINGLE_USER
int rlb_status;
static char type[5];        /* open type (s or x) */
#endif
#endif

#ifndef NO_TRANS
#ifndef GENERAL
/* transaction activity file info */
extern INT  taf_count;
extern char taf_files[TAFLIMIT][FILENMLEN];
#endif
#endif

/* Internal function prototypes */
#ifndef SINGLE_USER
static void pr_lock_descr(P1(struct lock_descr FAR *) Pi(int) 
					    Pi(CONST char FAR *));
static int process_lock(P1(struct lock_descr FAR *) Pi(char));
static int keep_locks(P1(struct lock_descr FAR *));
static int free_files(P1(struct lock_descr FAR *));
#endif
static int bld_lock_tables(P0);
static int initses(P0);
static int lock_files(P1(int) Pi(LOCK_REQUEST FAR *));
static int send_lock(P0);
static int send_free(P0);
static void reset_locks(P0);
static int recovery_check(P0);




#ifndef NO_TRANS
/* Set the number of lock request retries
*/
d_retries(num TASK_PARM)
int num;
TASK_DECL
{
   DB_ENTER(NO_DB_ID TASK_ID LOCK_SET(LOCK_NONE));

#ifndef SINGLE_USER
   lock_tries = num;
#endif
   RETURN( db_status = S_OKAY );
}
#endif


#ifndef NO_TRANS
/* Set the lock request timeout value
*/
d_timeout(secs TASK_PARM)
int secs;
TASK_DECL
{
#ifdef SINGLE_USER
   return(db_status = S_OKAY);
#else
   LM_SETTIME sto;  /* send timeout packet */

   DB_ENTER(NO_DB_ID TASK_ID LOCK_SET(LOCK_ALL));
   if ( !dbopen ) RETURN( dberr(S_DBOPEN) );

   if ( dbopen == 1 ) {
      sto.fcn  = L_SETTIME;
      sto.secs = secs;
      if ( nw_send(lsn, (MESSAGE FAR *)&sto, sizeof(LM_SETTIME)) )
	 RETURN( neterr() );
      db_timeout = secs;
   }
   RETURN( db_status = S_OKAY );
#endif
}
#endif


/* Open db_VISTA database
*/
int
d_open(dbnames, opentype TASK_PARM)
CONST char FAR *dbnames;
CONST char FAR *opentype;
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

#ifndef NO_TRANS
#ifndef GENERAL
   /* open transaction activity file */
   if ( taf_open() != S_OKAY ) 
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
#ifndef NO_TRANS
   /* [713] perform external recovery in single-user mode */
   if ( recovery_check() != S_OKAY ) RETURN(db_status);
#endif
#endif
#ifndef NO_TRANS
   if ( use_ovfl ) {
      if ( o_setup() != S_OKAY ) RETURN( db_status );
   }
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
TASK FAR *tsk;
{
   byteset(tsk, '\0', sizeof(TASK));
#ifndef ONE_DB
   tsk->No_of_dbs = 1;
#endif
#ifndef SINGLE_USER
   tsk->Lock_tries = 5;
   tsk->Dbwait_time = 1;
   tsk->Db_timeout = TIMEOUT_DEF;
   tsk->Db_lockmgr = 1;
#endif
#ifndef NO_TRANS
   tsk->Dboptions = DCHAINUSE | TRLOGGING;
#else
   tsk->Dboptions = DCHAINUSE;
#endif
   return( db_status );
}


/* Initialize multiple database table entries
*/
int
initdbt(dbnames )
CONST char FAR *dbnames;
{
   register int dbt_lc;			/* loop control */
   char dbfile [DtSrFILENMLEN];
   char FAR *ptr;
#ifndef	 ONE_DB
   CONST char FAR *cp;
   register int i;
#endif

#ifndef	 ONE_DB
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
#else
      strcpy(dbfile, dbnames);
#endif
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
#ifndef	 ONE_DB
   }
#endif
   return( db_status = S_OKAY );
} /* initdbt() */



#ifndef NO_TRANS
/* Check for possible recovery
*/
static recovery_check()
{
#ifndef SINGLE_USER
   LM_TREND trend_pkt;
#ifndef GENERAL
   register int tn;     /* transaction number */
   register int tc;     /* transaction count */
#endif
#endif

#ifndef GENERAL
   /* open tr activity file */
   if ( taf_access() == S_OKAY ) {
      taf_release();
#endif
#ifdef SINGLE_USER
      if (taf_count != 0) {
	 if (d_recover(taf_files[0] CURRTASK_PARM) != S_OKAY)
	    return( db_status );
	 taf_count = 0;
      }
#else
#ifndef GENERAL
      if ( tc = taf_count ) {
	 /* perform recovery on each file */
	 for ( tn = 0; tn < tc; ++tn ) {
	    if ( d_recover(taf_files[0] CURRTASK_PARM) != S_OKAY ) return( db_status );
	 }
	 taf_count = 0;
      }
#endif
#endif
#ifndef GENERAL
   }
#endif
#ifndef SINGLE_USER
   if ( db_lockmgr ) {
      /* tell lock manager that we're done */
      trend_pkt.fcn = L_RECDONE;
      if ( nw_send(lsn, (MESSAGE FAR *)&trend_pkt, sizeof(LM_TREND)) )
	 neterr();
   }
#endif

   return( db_status );
}
#endif



#ifndef	SINGLE_USER
/* Initial lock manager session
*/
static int initses()
{
   LM_DBOPEN_P Send_pkt;
   LR_DBOPEN_P Recv_pkt;
   register int ft_lc;			/* loop control */
   LM_TREND trend_pkt;
   int send_size, recv_size, recvd_sz;
   struct stat stbuf;
   LM_FILEID *fi_ptr;
   register FILE_ENTRY FAR *file_ptr;
   FILE_NO FAR *fref_ptr;
   INT FAR *rcv_fref_ptr;

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
   send_pkt = (LM_DBOPEN FAR *)ALLOC(&Send_pkt, send_size, "send_pkt");
   recv_size = sizeof(LR_DBOPEN) + (size_ft-1)*sizeof(INT);
   recv_pkt = (LR_DBOPEN FAR *)ALLOC(&Recv_pkt, recv_size, "recv_pkt");
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
   if (nw_send(lsn, (MESSAGE FAR *)send_pkt, send_size) ||
       nw_rcvmsg(lsn, (MESSAGE FAR *)recv_pkt, recv_size, &recvd_sz)) {
      nw_hangup(lsn);
      return(neterr());
   }

   if ( recv_pkt->status == L_RECOVER )  {
      /* perform auto-recovery */
      d_recover( (CONST char FAR *)recv_pkt->logfile CURRTASK_PARM );

      /* tell lock mgr we're done */
      trend_pkt.fcn = L_RECDONE;
      if ( nw_send(lsn, (MESSAGE FAR *)&trend_pkt, sizeof(LM_TREND)) ) {
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

#ifndef NO_TRANS
/* Build application file lock tables
*/
static int bld_lock_tables()
{
#ifndef SINGLE_USER
   register int fd_lc;		/* loop control */
   register int st_lc;		/* loop control */
   INT_P File_used;
#define file_used File_used.ptr
   int rec;
   int mem, memtot;
   register FILE_NO i;
   FILE_NO fl_cnt;
   struct lock_descr FAR *ld_ptr;
   RECORD_ENTRY FAR *rec_ptr;
   FIELD_ENTRY FAR *fld_ptr;
   SET_ENTRY FAR *set_ptr;
   MEMBER_ENTRY FAR *mem_ptr;
   register int FAR *fu_ptr;
   FILE_NO FAR *fl_ptr;
   unsigned new_size;
   unsigned old_size;
   int old_keyl_cnt;

   old_size = old_size_ft*sizeof(int);
   new_size = size_ft*sizeof(int);
   File_used.ptr = NULL;
   /* Macro references must be on one line for some compilers */ 
   if ((ALLOC_TABLE(&db_global.App_locks, new_size, old_size, "app_locks")
								!= S_OKAY) ||
       (ALLOC_TABLE(&db_global.Excl_locks, new_size, old_size, "excl_locks")
								!= S_OKAY) ||
       (ALLOC_TABLE(&db_global.Kept_locks, new_size, old_size, "kept_locks")
								!= S_OKAY) ||
       (ALLOC_TABLE(&File_used, new_size, old_size, "file_used")
								!= S_OKAY)) {
      return( db_status );
   }

   old_size = old_size_rt * sizeof(struct lock_descr);
   new_size = size_rt * sizeof(struct lock_descr);
   if ((ALLOC_TABLE(&db_global.Rec_locks, new_size, old_size, "rec_locks")
								!= S_OKAY)) {
      return( db_status );
   }

   if ( size_st ) {
      new_size = size_st * sizeof(struct lock_descr);
      old_size = old_size_st * sizeof(struct lock_descr);
      /* Macro references must be on one line for some compilers */ 
      if (ALLOC_TABLE(&db_global.Set_locks, new_size, old_size, "set_locks")
								!= S_OKAY ) {
	 return( db_status );
      }
   }

   /* build rec_locks table */
   for (rec = old_size_rt, rec_ptr = &record_table[old_size_rt], 
	   ld_ptr = rec_locks;
	rec < size_rt;
	++rec, ++rec_ptr, ++ld_ptr) {
      ld_ptr->fl_type = 'f'; 
      ld_ptr->fl_prev = 'f';			/*[367] init to free */
      ld_ptr->fl_kept = FALSE;

      /* put record's data file in list */
      file_used[rec_ptr->rt_file] = TRUE;

      /* add any key files to list */
      fl_cnt = 1;  /* count of used files */
      for (fd_lc = size_fd - rec_ptr->rt_fields,
	      fld_ptr = &field_table[rec_ptr->rt_fields];
	   (--fd_lc >= 0) && (fld_ptr->fd_rec == rec);
	   ++fld_ptr) {
	 if ( fld_ptr->fd_key != NOKEY )  {
	    fu_ptr = &file_used[fld_ptr->fd_keyfile];
	    if (!*fu_ptr) {
	       *fu_ptr = TRUE;
	       ++fl_cnt;
	    }
	 }
      }
      ld_ptr->fl_cnt = fl_cnt;
      ld_ptr->fl_list.ptr =
		/* Macro references must be on one line for some compilers */ 
    (FILE_NO FAR *)ALLOC(&ld_ptr->fl_list, fl_cnt*sizeof(FILE_NO), db_avname);
      if ( ld_ptr->fl_list.ptr == NULL ) return( dberr(S_NOMEMORY) );
      fl_ptr = ld_ptr->fl_list.ptr;
      for (i = 0, fu_ptr = file_used; i < size_ft; ++i, ++fu_ptr) {
	 if (*fu_ptr) {
	    *fu_ptr = FALSE;
	    *fl_ptr++ = i;
	 }
      }
      FL_LIST_DEACCESS(ld_ptr);
   }
   /* build set_locks table */
   if ( size_st ) {
      for (st_lc = size_st - old_size_st, set_ptr = &set_table[old_size_st],
							   ld_ptr = set_locks;
	   --st_lc >= 0; ++set_ptr, ++ld_ptr) {
	 /* add owner's data file */
	 file_used[record_table[set_ptr->st_own_rt].rt_file] = TRUE;
	 ld_ptr->fl_type = 'f'; 
	 ld_ptr->fl_prev = 'f';			/*[367] init to free */
	 ld_ptr->fl_kept = FALSE;

	 /* add member record data files to list */
	 fl_cnt = 1; /* count of used files */
	 for (mem = set_ptr->st_members, memtot = mem + set_ptr->st_memtot,
						   mem_ptr = &member_table[mem];
	      mem < memtot;
	      ++mem, ++mem_ptr) {
	    fu_ptr = &file_used[record_table[mem_ptr->mt_record].rt_file];
	    if (!*fu_ptr) {
	       *fu_ptr = TRUE;
	       ++fl_cnt;
	    }
	 }
	 ld_ptr->fl_cnt = fl_cnt;
	 ld_ptr->fl_list.ptr =
		   /* Macro references must be on one line for some compilers */ 
       (FILE_NO FAR *)ALLOC(&ld_ptr->fl_list, fl_cnt*sizeof(FILE_NO), db_avname);
	 if ( ld_ptr->fl_list.ptr == NULL ) return( dberr(S_NOMEMORY) );
	 fl_ptr = ld_ptr->fl_list.ptr;
	 for (i = 0, fu_ptr = file_used; i < size_ft; ++i, ++fu_ptr) {
	    if (*fu_ptr) {
	       *fu_ptr = FALSE;
	       *fl_ptr++ = i;
	    }
	 }
	 FL_LIST_DEACCESS(ld_ptr);
      }
   }
   /* build key_locks table */
   keyl_cnt = 0;
   old_keyl_cnt = keyl_cnt;
   for (fd_lc = size_fd - old_size_fd, fld_ptr = &field_table[old_size_fd];
	--fd_lc >= 0; ++fld_ptr) {
      /* count number of keys */
      if (fld_ptr->fd_key != NOKEY)
	 ++keyl_cnt;
   }
   if ( keyl_cnt ) {
      old_size = old_keyl_cnt*sizeof(struct lock_descr);
      new_size = keyl_cnt*sizeof(struct lock_descr);
      /* Macro references must be on one line for some compilers */ 
      if (ALLOC_TABLE(&db_global.Key_locks, new_size, old_size, "key_locks")
								   != S_OKAY) {
	 return( db_status );
      }
      for (fd_lc = size_fd - old_size_fd, fld_ptr = &field_table[old_size_fd],
							   ld_ptr = key_locks;
	   --fd_lc >= 0; ++fld_ptr) {
	 if (fld_ptr->fd_key != NOKEY) {
	    ld_ptr->fl_type = 'f';
	    ld_ptr->fl_prev = 'f';			/*[367] init to free */
	    ld_ptr->fl_kept = FALSE;
	    ld_ptr->fl_cnt = 1;
	    ld_ptr->fl_list.ptr = (FILE_NO FAR *)ALLOC(&ld_ptr->fl_list, ld_ptr->fl_cnt*sizeof(FILE_NO), "fl_list");
	    if ( ld_ptr->fl_list.ptr == NULL ) return( dberr(S_NOMEMORY) );
	    *(ld_ptr->fl_list.ptr) = fld_ptr->fd_keyfile;
	    FL_LIST_DEACCESS(ld_ptr);
	    ++ld_ptr;
	 }
      }
   }
   lp_size = sizeof(LM_LOCK) + (size_ft-1)*sizeof(LM_LOCKREQ);
   fp_size = sizeof(LM_FREE) + (size_ft-1)*sizeof(INT);
   lock_pkt = (LM_LOCK FAR *)ALLOC(&db_global.Lock_pkt, lp_size, "lock_pkt");
   free_pkt = (LM_FREE FAR *)ALLOC(&db_global.Free_pkt, fp_size, "free_pkt");
   if ( !lock_pkt || !free_pkt ) return( dberr(S_NOMEMORY) );
   lock_pkt->fcn = L_LOCK;
   free_pkt->fcn = L_FREE;
   MEM_UNLOCK(&File_used);
   FREE(&File_used);
#endif

   return( db_status = S_OKAY );
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
   register int i;

   DB_ENTER(NO_DB_ID TASK_ID LOCK_SET(LOCK_ALL));

   if ( dbopen ) {
      db_status = S_OKAY;
#ifndef NO_TRANS
      /* in case they forgot to end the transaction */
      if ( trans_id ) 
	 d_trabort(TASK_ONLY);
      else
#ifndef SINGLE_USER
	 if ( dbopen >= 2 )
#endif
#endif
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

#ifndef NO_TRANS
#ifndef GENERAL
      taf_close();
#endif
#endif
#ifndef SINGLE_USER
      d_freeall(TASK_ONLY);
#endif
#ifndef NO_TRANS
      if ( use_ovfl ) o_free();
#endif
      termfree();
      key_close();
      sk_free();
      dio_free();
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
#ifndef ONE_DB
      setdb_on = FALSE;
      curr_db = 0;
      no_of_dbs = 1;
#endif
#ifndef SINGLE_USER
      lock_tries = 5;
      dbwait_time = 1;
      db_lockmgr = 1;
      session_active = FALSE;
#endif
#ifndef NO_TRANS
      cache_ovfl = FALSE;
      ov_initaddr = 0L;
      ov_rootaddr = 0L;
      ov_nextaddr = 0L;
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
#ifdef MULTI_TASK
      bytecpy(task.v.ptr, &db_global, sizeof(TASK));
#endif
   }
   RETURN( db_status );
} /* d_close() */



#ifndef SINGLE_USER
/* Terminate lock manager session
*/
termses()
{
   LM_DBCLOSE_P Send_pkt;
   register int ft_lc;			/* loop control */
   int send_size;
   register FILE_NO FAR *fref_ptr;
   register INT FAR *snd_fref_ptr;

   if ( session_active ) {
      send_size = sizeof(LM_DBCLOSE) + (size_ft-1)*sizeof(INT);
      send_pkt = (LM_DBCLOSE FAR *)ALLOC(&Send_pkt, send_size, "send_pkt");
      if ( send_pkt == NULL ) return( dberr(S_NOMEMORY) );
      send_pkt->fcn = L_DBCLOSE;
      send_pkt->nfiles = size_ft;
      for (ft_lc = size_ft, fref_ptr = file_refs,
						snd_fref_ptr = send_pkt->frefs;
	   --ft_lc >= 0; ++fref_ptr, ++snd_fref_ptr)
	 *snd_fref_ptr = *fref_ptr;
      if ( nw_send(lsn, (MESSAGE FAR *)send_pkt, send_size) )
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
   register int i;
   register struct lock_descr FAR *ld_ptr;
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
#ifndef	 ONE_DB
   if ( db_table ) {
      MEM_UNLOCK(&db_global.Db_table);
      FREE(&db_global.Db_table);
   }
   if ( rn_table ) {
      MEM_UNLOCK(&db_global.Rn_table);
      FREE(&db_global.Rn_table);
   }
#endif
}

#ifndef NO_TRANS
/* Establish record file locks
*/
d_reclock(rec, lock_type TASK_PARM DBN_PARM)
int rec;
char FAR *lock_type;
TASK_DECL
DBN_DECL
{
#ifdef SINGLE_USER
   return(db_status = S_OKAY);
#else
   LOCK_REQUEST lr;

   DB_ENTER(DB_ID TASK_ID LOCK_SET(LOCK_IO));

   lr.item = rec;
   lr.type = *lock_type;

   RETURN( d_lock(1, &lr TASK_PARM DBN_PARM) );
#endif
}
#endif


#ifndef NO_TRANS
/* Establish set file locks
*/
d_setlock(set, lock_type TASK_PARM DBN_PARM)
int  set;
char FAR *lock_type;
TASK_DECL
DBN_DECL
{
#ifdef SINGLE_USER
   return (db_status = S_OKAY);
#else
   LOCK_REQUEST lr;

   DB_ENTER(DB_ID TASK_ID LOCK_SET(LOCK_IO));

   lr.item = set;
   lr.type = *lock_type;

   RETURN( d_lock(1, &lr TASK_PARM DBN_PARM) );
#endif
}
#endif

#ifndef NO_TRANS
/* Lock key file 
*/
d_keylock(key, lock_type TASK_PARM DBN_PARM)
long key;  /* field number of key */
char FAR *lock_type;
TASK_DECL
DBN_DECL
{
#ifdef SINGLE_USER
   return (db_status = S_OKAY);
#else
   int fld, rec;
   LOCK_REQUEST lr;
   RECORD_ENTRY FAR *rec_ptr;
   FIELD_ENTRY FAR *fld_ptr;

   DB_ENTER(DB_ID TASK_ID LOCK_SET(LOCK_IO));

   if (nfld_check(key, &rec, &fld, (RECORD_ENTRY FAR * FAR *)&rec_ptr, (FIELD_ENTRY FAR * FAR *)&fld_ptr) != S_OKAY)
      RETURN( db_status );

   if (fld_ptr->fd_key == NOKEY)
      RETURN( dberr(S_NOTKEY) );

   /* KEYMARK allows 'fld' to be recognized as a key file.  It is already
      adjusted (in nfld_check) to INTernal format.  Don't play with it in
      d_lock and lock_files.
   */
   lr.item = fld + KEYMARK;
   lr.type = *lock_type;

   RETURN( d_lock(1, &lr TASK_PARM DBN_PARM) );
#endif
}
#endif

#ifndef NO_TRANS
/* Return lock status for record type
*/
d_reclstat(rec, lstat TASK_PARM DBN_PARM)
int rec;
char FAR *lstat;
TASK_DECL
DBN_DECL
{
#ifdef SINGLE_USER
   *lstat = 'f';
   return( db_status = S_OKAY );
#else
   RECORD_ENTRY FAR *rec_ptr;

   DB_ENTER(DB_ID TASK_ID LOCK_SET(LOCK_NOIO));

   if (nrec_check(rec, &rec, (RECORD_ENTRY FAR * FAR *)&rec_ptr) != S_OKAY)
      RETURN( db_status );

   if ( dbopen >= 2 )
      *lstat = 'f';
   else {
      if (rec_ptr->rt_flags & STATIC)
	 *lstat = 's';
      else
	 *lstat = rec_locks[rec].fl_type;
   }
   RETURN( db_status = S_OKAY );
#endif
}
#endif

#ifndef NO_TRANS
/* Return lock status for set type
*/
d_setlstat(set, lstat TASK_PARM DBN_PARM)
int set;
char FAR *lstat;
TASK_DECL
DBN_DECL
{
#ifdef SINGLE_USER
   *lstat = 'f';
   return (db_status = S_OKAY);
#else
   SET_ENTRY FAR *set_ptr;

   DB_ENTER(DB_ID TASK_ID LOCK_SET(LOCK_NOIO));

   if (nset_check(set, &set, (SET_ENTRY FAR * FAR *)&set_ptr) != S_OKAY)
      RETURN( db_status );

   if ( dbopen >= 2 )
      *lstat = 'f';
   else
      *lstat = set_locks[set].fl_type;

   RETURN( db_status = S_OKAY );
#endif
}
#endif

#ifndef NO_TRANS
/* Return lock status for key type
*/
d_keylstat(key, lstat TASK_PARM DBN_PARM)
long key;
char FAR *lstat;
TASK_DECL
DBN_DECL
{
#ifdef SINGLE_USER
   *lstat = 'f';
   return (db_status = S_OKAY);
#else
   int fld, rec;
   RECORD_ENTRY FAR *rec_ptr;
   FIELD_ENTRY FAR *fld_ptr;

   DB_ENTER(DB_ID TASK_ID LOCK_SET(LOCK_NOIO));

   if (nfld_check(key, &rec, &fld, (RECORD_ENTRY FAR * FAR *)&rec_ptr, (FIELD_ENTRY FAR * FAR *)&fld_ptr) != S_OKAY)
      RETURN( db_status );

   if (fld_ptr->fd_key == NOKEY)
      RETURN( dberr(S_NOTKEY) );

   if ( dbopen >= 2 )
      *lstat = 'f';
   else {
      if ( file_table[fld_ptr->fd_keyfile].ft_flags & STATIC )
	 *lstat = 's';
      else
	 *lstat = key_locks[fld_ptr->fd_keyno].fl_type;
   }
   RETURN( db_status = S_OKAY );
#endif
}
#endif

#ifndef NO_TRANS
/* Lock a group of records and/or sets
*/
d_lock(count, lrpkt TASK_PARM DBN_PARM)
int count;
LOCK_REQUEST FAR *lrpkt;
TASK_DECL
DBN_DECL
{
#ifdef SINGLE_USER
   return (db_status = S_OKAY);
#else
   register int item;
   register int i;
   register LOCK_REQUEST FAR *lrpkt_ptr;
   struct lock_descr FAR *ld_ptr;

   DB_ENTER(DB_ID TASK_ID LOCK_SET(LOCK_IO));

   if ( dbopen >= 2 )
      RETURN( db_status = S_OKAY );

   lock_pkt->nfiles = 0;
   for (i = 0, lrpkt_ptr = lrpkt;
	(db_status == S_OKAY) && (i < count);
	++i, ++lrpkt_ptr) {
      if ( lrpkt_ptr->item >= KEYMARK ) {
	 /* do not adjust lrpkt->item (see comment in d_keylock) */
	 item = field_table[lrpkt_ptr->item - KEYMARK].fd_keyno;
	 process_lock(&key_locks[item], lrpkt_ptr->type);
      }
      else if ( lrpkt_ptr->item >= SETMARK ) {
	 item = NUM2INT(lrpkt_ptr->item - SETMARK, st_offset);
	 process_lock(&set_locks[item], lrpkt_ptr->type);
      }
      else if ( lrpkt_ptr->item >= RECMARK ) {
	 item = NUM2INT(lrpkt_ptr->item - RECMARK, rt_offset);
	 if ( record_table[item].rt_flags & STATIC )
	    dberr(S_STATIC);
	 else
	    process_lock(&rec_locks[item], lrpkt_ptr->type);
      }
      else
	 dberr( S_INVNUM );
   }
   if ( db_status == S_OKAY )
      lock_files(count, lrpkt);

   if ( db_status != S_OKAY ) {
      /* reset lock descriptor tables to previous state */
      for (i = 0, lrpkt_ptr = lrpkt; i < count; ++i, ++lrpkt_ptr) {
	 /* do not adjust lrpkt->item (see comment in d_keylock) */
	 if ( lrpkt_ptr->item >= KEYMARK ) {
	    item = field_table[lrpkt_ptr->item - KEYMARK].fd_keyno;
	    ld_ptr = &key_locks[item];
	 }
	 else if ( lrpkt_ptr->item >= SETMARK ) {
	    item = NUM2INT(lrpkt_ptr->item - SETMARK, st_offset);
	    ld_ptr = &set_locks[item];
	 }
	 else if ( lrpkt_ptr->item >= RECMARK ) {
	    item = NUM2INT(lrpkt_ptr->item - RECMARK, rt_offset);
	    ld_ptr = &rec_locks[item];
	 }
	 else
	    continue;
	 ld_ptr->fl_type = ld_ptr->fl_prev;
      }
   }
   RETURN( db_status );
#endif
}
#endif


#ifndef SINGLE_USER
/* Process set/record lock
*/
static process_lock(ld_ptr, type )
struct lock_descr FAR *ld_ptr;
char type;
{
   register int fl_lc;			/* loop control */
   int fno;
   register int i;
   register LM_LOCKREQ FAR *lockreq_ptr;
   FILE_NO FAR *fl_ptr, fref;

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


#ifndef NO_TRANS
/* Lock database files
*/
static lock_files(count, lrpkt )
int count;
LOCK_REQUEST FAR *lrpkt;
{
#ifndef SINGLE_USER
   register int fl_lc;			/* loop control */
   struct lock_descr FAR *ld_ptr;
   FILE_NO fno;
   register int item;
   int l;
   LOCK_REQUEST FAR *lrpkt_ptr;
   int FAR *appl_ptr, FAR *excl_ptr;
   FILE_NO FAR *fl_ptr;
   
   lock_reply.status = L_OKAY;
   if ( lock_pkt->nfiles == 0 ) goto skip_send;

   if ( send_lock() != S_OKAY )
      return( db_status );

skip_send:
   switch ( lock_reply.status ) {
      case L_OKAY:
	 /* update app_locks and excl_lock tables */
	 for (l = 0, lrpkt_ptr = lrpkt; l < count; ++l, ++lrpkt_ptr) {
	    if (lrpkt_ptr->type == 'k')
	       continue; /* skip keep lock requests */
	    /* process each record/set lock */
	    /* do not adjust lrpkt->item (see comment in d_keylock) */
	    if ( lrpkt_ptr->item >= KEYMARK ) {
	       item = field_table[lrpkt_ptr->item - KEYMARK].fd_keyno;
	       ld_ptr = &key_locks[item];
	    }
	    else if ( lrpkt_ptr->item >= SETMARK ) {
	       item = NUM2INT(lrpkt_ptr->item - SETMARK, st_offset);
	       ld_ptr = &set_locks[item];
	    }
	    else {
	       item = NUM2INT(lrpkt_ptr->item - RECMARK, rt_offset);
	       ld_ptr = &rec_locks[item];
	    }
	    for (fl_lc = ld_ptr->fl_cnt, fl_ptr = FL_LIST_ACCESS(ld_ptr);
		 --fl_lc >= 0; ++fl_ptr) {
	       /* process each file for each record/set lock */
	       fno = *fl_ptr;
	       appl_ptr = &app_locks[fno];
	       excl_ptr = &excl_locks[fno];
	       if ( !*appl_ptr && !*excl_ptr ) {
		  /* clear file's pages from cache */
		  dio_clrfile(fno);
	       }
	       if ( ld_ptr->fl_type == 'r' ) {
		  if ( *appl_ptr >= 0 ) 
		     /* increment if file free or read-locked */
		     ++*appl_ptr;
	       }
	       else {
		  if ( ld_ptr->fl_type == 'w' ) 
		     *appl_ptr = -1;
		  else if ( ld_ptr->fl_type == 'x' ) {
		     ++*excl_ptr;
		     if ( ld_ptr->fl_prev == 'r' ) {
			/* read to excl lock upgrade */
			--*appl_ptr;
		     }
		  }
	       }
	    }
	    FL_LIST_DEACCESS(ld_ptr);
	 }
	 break;
      case L_UNAVAIL:
      case L_TIMEOUT:
	 return( db_status = S_UNAVAIL );
      default:
	 return( dberr(S_SYSERR) );
   }
#endif

   return( db_status = S_OKAY );
}
#endif


#ifndef NO_TRANS
/* Send lock request 
*/
static int send_lock()
{
#ifndef SINGLE_USER
   LM_TREND trend_pkt;
   int send_size, recv_size;

   if ( lock_pkt->nfiles ) {
      /* send lock request */
      send_size = sizeof(LM_LOCK) + (lock_pkt->nfiles-1)*sizeof(LM_LOCKREQ);
      if ( send_size > lp_size )
	 return( dberr(S_SYSERR) );
      
req_locks:
#ifdef MONITOR
      printf("nw_send(lsn,lock_pkt->fcn=%ld,size=%d\n",lock_pkt->fcn,send_size);
#endif
      if ( nw_send(lsn, (MESSAGE FAR *)lock_pkt, send_size) )
	 return( neterr() );

      if ( nw_rcvmsg(lsn, (MESSAGE FAR *)&lock_reply, sizeof(LR_LOCK), &recv_size) )
	 return( neterr() );
#ifdef MONITOR
      printf("nw_rcvmsg(lock_reply.fcn=%ld,lock_reply.status=%d\n",
	 lock_reply.fcn,lock_reply.status);
#endif

      /* request must always be granted */
      if ( lock_reply.fcn != L_LOCK )
	 return( dberr(S_NETSYNC) );

      if (lock_reply.status == L_RECOVER) {
	 /* perform auto-recovery */
	 d_recover(lock_reply.logfile CURRTASK_PARM);

	 /* tell lock mgr we're done */
	 trend_pkt.fcn = L_RECDONE;
	 if (nw_send(lsn, (MESSAGE FAR *)&trend_pkt, sizeof(LM_TREND)))
	    return( neterr() );

	 /* re-issue lock request */
	 goto req_locks;
      }
      if (lock_reply.status == L_QUEUEFULL) {
	 sleep(dbwait_time);
	 goto req_locks;
      }
   }
#endif

   return( db_status = S_OKAY );
}
#endif


#ifndef NO_TRANS
/* Free key lock
*/
d_keyfree(key TASK_PARM DBN_PARM)
long key;
TASK_DECL
DBN_DECL
{
#ifdef SINGLE_USER
   return (db_status = S_OKAY);
#else
   int fld, rec;
   RECORD_ENTRY FAR *rec_ptr;
   FIELD_ENTRY FAR *fld_ptr;
   struct lock_descr FAR *ld_ptr;

   DB_ENTER(DB_ID TASK_ID LOCK_SET(LOCK_IO));

   if (nfld_check(key, &rec, &fld, (RECORD_ENTRY FAR * FAR *)&rec_ptr, (FIELD_ENTRY FAR * FAR *)&fld_ptr) != S_OKAY)
      RETURN( db_status );

   if ( fld_ptr->fd_key == NOKEY )
      RETURN( dberr(S_NOTKEY) );

   if ( dbopen >= 2 )  /* exclusive access needs no locks */
      RETURN( db_status = S_OKAY );

   ld_ptr = &key_locks[fld_ptr->fd_keyno];
   if ( trans_id )
      RETURN( dberr(S_TRFREE) );

   if ( ld_ptr->fl_type == 'f' )
      RETURN( dberr(S_NOTLOCKED) );

   free_files(ld_ptr);
   ld_ptr->fl_type = 'f';

   RETURN( db_status );
#endif
}
#endif


#ifndef SINGLE_USER
/* Setup table to keep locks after transaction end
*/
static keep_locks( ld_ptr )
struct lock_descr FAR *ld_ptr;      /* Lock descriptor */
{
   register int fl_lc;			/* loop control */
   register FILE_NO FAR *fl_ptr;

   /* Mark lock as kept */
   ld_ptr->fl_kept = TRUE;                

   for (fl_lc = ld_ptr->fl_cnt, fl_ptr = FL_LIST_ACCESS(ld_ptr);
	--fl_lc >= 0; ++fl_ptr)
      ++kept_locks[*fl_ptr];
   FL_LIST_DEACCESS(ld_ptr);

   return( db_status = S_OKAY );
}
#endif

#ifndef NO_TRANS
/* Free record lock
*/
d_recfree(rec TASK_PARM DBN_PARM)
int rec;
TASK_DECL
DBN_DECL
{
#ifdef SINGLE_USER
   return (db_status = S_OKAY);
#else
   RECORD_ENTRY FAR *rec_ptr;
   struct lock_descr FAR *ld_ptr;

   DB_ENTER(DB_ID TASK_ID LOCK_SET(LOCK_IO));

   if (nrec_check(rec, &rec, (RECORD_ENTRY FAR * FAR *)&rec_ptr) != S_OKAY)
      RETURN( db_status );

   if ( dbopen >= 2 )  /* exclusive access needs no locks */
      RETURN( db_status = S_OKAY );

   ld_ptr = &rec_locks[rec];

   if ( trans_id )
      RETURN( dberr(S_TRFREE) );

   if ( ld_ptr->fl_type == 'f' )
      RETURN( dberr(S_NOTLOCKED) );

   free_files(ld_ptr);
   ld_ptr->fl_type = 'f';

   RETURN( db_status );
#endif
}
#endif

#ifndef NO_TRANS
/* Free set lock
*/
d_setfree(set TASK_PARM DBN_PARM)
int set;
TASK_DECL
DBN_DECL
{
#ifdef SINGLE_USER
   return (db_status = S_OKAY);
#else
   SET_ENTRY FAR *set_ptr;
   struct lock_descr FAR *ld_ptr;

   DB_ENTER(DB_ID TASK_ID LOCK_SET(LOCK_IO));

   if (nset_check(set, &set, (SET_ENTRY FAR * FAR *)&set_ptr) != S_OKAY)
      RETURN( db_status );

   if ( dbopen >= 2 )  /* exclusive access needs no locks */
      RETURN( db_status = S_OKAY );

   ld_ptr = &set_locks[set];

   if ( trans_id )
      RETURN( dberr(S_TRFREE) );

   if ( ld_ptr->fl_type == 'f' )
      RETURN( dberr(S_NOTLOCKED) );

   free_files(ld_ptr);
   ld_ptr->fl_type = 'f';

   RETURN( db_status );
#endif
}
#endif



#ifndef SINGLE_USER
/* Free read-locked files associated with record or set
*/
static int free_files(ld_ptr)
struct lock_descr FAR *ld_ptr;
{
   register int fl_lc;			/* loop control */
   FILE_NO fno;
   LM_LOCKREQ FAR *lockreq_ptr;
   int FAR *appl_ptr;
   FILE_NO fref;
   register FILE_NO FAR *fl_ptr;

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

#ifndef NO_TRANS
/* free all locked files 
*/
d_freeall(TASK_ONLY)
TASK_DECL
{
#ifdef SINGLE_USER
   return (db_status = S_OKAY);
#else
   register int i;
   register FILE_NO FAR *fref_ptr;
   register int FAR *appl_ptr;

   DB_ENTER(NO_DB_ID TASK_ID LOCK_SET(LOCK_IO));

   if ( ! dbopen ) RETURN( dberr(S_DBOPEN) );

   if ( dbopen >= 2 )  /* exclusive access needs no locks */
      RETURN( db_status = S_OKAY );

   if ( trans_id ) RETURN( dberr(S_TRFREE) );

   free_pkt->nfiles = 0;
   for (i = 0, fref_ptr = file_refs, appl_ptr = app_locks;
	i < size_ft;
	++i, ++fref_ptr, ++appl_ptr) {
      if (*appl_ptr) {
	 *appl_ptr = FALSE;
	 if (!excl_locks[i])
	    free_pkt->frefs[free_pkt->nfiles++] = *fref_ptr;
      }
   }
   /* send free files packet */
   if ( send_free() != S_OKAY )
      RETURN( db_status );
   
   /* reset all lock descriptors */
   reset_locks();

   /* reset all key file positions */
   key_reset(size_ft);

   /* Clear cache pages and return */
   RETURN( dio_clear() );
#endif
}
#endif


#ifndef SINGLE_USER
/* Reset lock descriptor tables
*/
static void reset_locks()
{
   int beg, end;
   register int i;
   register struct lock_descr FAR *ld_ptr;

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

#ifndef NO_TRANS
/* Send free files packet
*/
static int send_free()
{
#ifndef SINGLE_USER
   int send_size;

   /* send any free packets */
   if ( free_pkt->nfiles ) {
      send_size = sizeof(LM_FREE) + (free_pkt->nfiles-1)*sizeof(INT);
      if ( send_size > fp_size )
	 return ( dberr(S_SYSERR) );

      if ( nw_send(lsn, (MESSAGE FAR *)free_pkt, send_size) ) 
	 return( neterr() );
   }
#endif
   return( db_status = S_OKAY );
}
#endif


#ifndef NO_TRANS
/*------------------------------------------------------------------------
   Record Lock Bit Functions
------------------------------------------------------------------------*/

/* Set record lock bit of current record
*/
d_rlbset(TASK_ONLY)
TASK_DECL
{
#ifndef SINGLE_USER
   FILE_NO file;
   INT rid;
   int record_lock;
#endif

   DB_ENTER(NO_DB_ID TASK_ID LOCK_SET(LOCK_IO));

   if ( ! curr_rec ) RETURN( dberr(S_NOCR) );
   
#ifndef SINGLE_USER
   file = NUM2INT((FILE_NO)((curr_rec >> FILESHIFT) & FILEMASK), ft_offset);

   if ( dbopen == 1 && 
       (record_lock = (app_locks[file] >= 0 && !excl_locks[file])) ) {
      /* request record-lock on file */
      lock_pkt->nfiles = 1;
      lock_pkt->locks[0].type = 'R';
      lock_pkt->locks[0].fref = file_refs[file];
      if ( send_lock() != S_OKAY ) RETURN( db_status );
      if ( lock_reply.status != L_OKAY ) 
	 RETURN( db_status = S_UNAVAIL );
   }
   if ( dio_rrlb(curr_rec, &rid) != S_OKAY )
      RETURN( db_status );
   if ( rid & RLBMASK )
      rlb_status = S_LOCKED;
   else {
      rid |= RLBMASK;
      rlb_status = dio_wrlb(curr_rec, rid);
   }
   if ( dbopen == 1 && record_lock ) {
      /* free or downgrade record-lock on file */
      if ( app_locks[file] ) {
	 lock_pkt->nfiles = 1;
	 lock_pkt->locks[0].type = 'r';
	 lock_pkt->locks[0].fref = file_refs[file];
	 if ( send_lock() != S_OKAY ) RETURN( db_status );
      }
      else {
	 free_pkt->nfiles = 1;
	 free_pkt->frefs[0] = file_refs[file];
	 if ( send_free() != S_OKAY ) RETURN( db_status );
      }
   }
   RETURN( db_status = rlb_status );
#else
   RETURN( db_status = S_OKAY );
#endif
}
#endif

#ifndef NO_TRANS
/* Clear record lock bit of current record
*/
d_rlbclr(TASK_ONLY)
TASK_DECL
{
#ifndef SINGLE_USER
   FILE_NO file;
   INT rid;
#endif

   DB_ENTER(NO_DB_ID TASK_ID LOCK_SET(LOCK_IO));

   if ( ! curr_rec ) RETURN( dberr(S_NOCR) );

#ifndef SINGLE_USER
   file = NUM2INT((FILE_NO)((curr_rec >> FILESHIFT) & FILEMASK), ft_offset);   

   /* ensure that changes are allowed */
   if (dbopen == 1 && trans_id && app_locks[file] >= 0 && !excl_locks[file]) 
      RETURN( dberr(S_NOTLOCKED) );
   
   if ( dbopen == 1 && ! trans_id ) {
      /* request record-lock on file */
      lock_pkt->nfiles = 1;
      lock_pkt->locks[0].type = 'R';
      lock_pkt->locks[0].fref = file_refs[file];
      if ( send_lock() != S_OKAY ) RETURN( db_status );
      if ( lock_reply.status != L_OKAY ) 
	 RETURN( db_status = S_UNAVAIL );
   }

   /* read rlb */
   if ( dio_rrlb(curr_rec, &rid) != S_OKAY )
      RETURN( db_status );

   /* clear rlb */
   rid &= ~RLBMASK;
   rlb_status = S_UNLOCKED;
   dio_wrlb(curr_rec, rid);

   if ( dbopen == 1 && ! trans_id ) {
      /* free or downgrade record-lock on file */
      if ( app_locks[file] ) {
	 lock_pkt->nfiles = 1;
	 lock_pkt->locks[0].type = 'r';
	 lock_pkt->locks[0].fref = file_refs[file];
	 if ( send_lock() != S_OKAY ) RETURN( db_status );
      }
      else {
	 free_pkt->nfiles = 1;
	 free_pkt->frefs[0] = file_refs[file];
	 if ( send_free() != S_OKAY ) RETURN( db_status );
      }
   }
#else
   db_status = S_OKAY;
#endif
   RETURN( db_status );
}
#endif


#ifndef NO_TRANS
/* Test record lock bit of current record
*/
d_rlbtst(TASK_ONLY)
TASK_DECL
{
#ifndef SINGLE_USER
   INT rid;
#endif

   DB_ENTER(NO_DB_ID TASK_ID LOCK_SET(LOCK_IO));

   if ( ! curr_rec ) RETURN( dberr(S_NOCR) );
   
#ifndef SINGLE_USER
   if ( dio_rrlb(curr_rec, &rid) != S_OKAY )
      RETURN( db_status );

   if ( rid & RLBMASK )
      db_status = S_LOCKED;
   else
      db_status = S_UNLOCKED;

   RETURN( rlb_status = db_status );
#else
   RETURN( db_status = S_UNLOCKED );
#endif

}
#endif



#ifndef NO_TRANS
/*------------------------------------------------------------------------
   Database Transaction Processing Functions
------------------------------------------------------------------------*/

/* Begin transaction
*/
d_trbegin(tid TASK_PARM)
CONST char FAR *tid;
TASK_DECL
{
   DB_ENTER(NO_DB_ID TASK_ID LOCK_SET(LOCK_IO));

   db_status = S_OKAY;

   if ( ! dbopen ) RETURN( dberr(S_DBOPEN) );

   if ( tid == NULL ) RETURN( dberr(S_TRANSID) );
   

   if ( trans_id ) RETURN( dberr(S_TRACTIVE) );

   /* changes were possible outside a transaction */
   dio_flush(); 

   if ( use_ovfl ) {
      o_init();
   }
   trans_id = tid;
   RETURN( db_status );
}
#endif


#ifndef NO_TRANS
/* End transaction
*/
d_trend(TASK_ONLY)
TASK_DECL
{
#ifndef SINGLE_USER
   register int ft_lc;			/* loop control */
   LM_TRCOMMIT trcom_pkt;
   LM_TREND trend_pkt;
   LM_LOCKREQ FAR *lockreq_ptr;
   register FILE_NO FAR *fref_ptr;
   register int FAR *appl_ptr, FAR *keptl_ptr, FAR *excl_ptr;
#endif

   DB_ENTER(NO_DB_ID TASK_ID LOCK_SET(LOCK_IO));

   db_status = S_OKAY;
   if( ! trans_id ) RETURN( dberr(S_TRNOTACT) );

   if( trlog_flag ) 
      /* mark start of trx in archive log file */
      d_trmark(); 

   /* flush data to database or overflow */
   if ( dio_flush() != S_OKAY ) RETURN( db_status );

   if ( (dboptions & TRLOGGING) && use_ovfl ) {
      /* End trx using overflow file */

      /* flush recovery data to overflow file */
      if ( o_flush() != S_OKAY ) RETURN( db_status );

#ifndef SINGLE_USER
      trcom_pkt.fcn = L_TRCOMMIT;
      strcpy(trcom_pkt.logfile, dblog);
      if ( nw_send(lsn, (MESSAGE FAR *)&trcom_pkt, sizeof(LM_TRCOMMIT)) )
	 RETURN( neterr() );
#endif
      trcommit = TRUE;

#ifndef GENERAL
      if ( taf_add(dblog) != S_OKAY ) RETURN( db_status ); /* after nw_send */
#endif

      /* allow for user interrupt to test recovery */
      if ( db_txtest ) dberr(S_DEBUG);

      if( cache_ovfl ) {
	 /* update db from overflow file */
	 if ( o_update() != S_OKAY ) RETURN( db_status );
      }

      /* flush modified cache data to database */
      if ( dio_flush() != S_OKAY ) RETURN( db_status );

#ifndef GENERAL
      if ( taf_del(dblog) != S_OKAY ) RETURN( db_status ); /* before nw_send */
#endif
   }
#ifndef SINGLE_USER
      trend_pkt.fcn = L_TREND;
      if ( nw_send(lsn, (MESSAGE FAR *)&trend_pkt, sizeof(LM_TREND)) )
	 RETURN( neterr() );
#endif
      trcommit = FALSE;

   if( trlog_flag ) 
      /* mark end of trx in archive log file */
      d_trbound(); 

   trans_id = NULL;
   o_init();			/*[305] clear cache_ovfl flag */

#ifndef SINGLE_USER
   if ( dbopen == 1 ) {
      /* free unkept, non-exclusive file locks */
      lock_pkt->nfiles = free_pkt->nfiles = 0;
      for (ft_lc = size_ft, fref_ptr = file_refs, appl_ptr = app_locks,
				keptl_ptr = kept_locks, excl_ptr = excl_locks;
	   --ft_lc >= 0; ++fref_ptr, ++appl_ptr, ++keptl_ptr, ++excl_ptr) {
	 if (*excl_ptr)
	    *appl_ptr = *keptl_ptr;
	 else if ( *appl_ptr == -1 ) {
	    if ( (*appl_ptr = *keptl_ptr) > 0 ) {
	       lockreq_ptr = &lock_pkt->locks[lock_pkt->nfiles++];
	       lockreq_ptr->type = 'r';
	       lockreq_ptr->fref = *fref_ptr;
	    }
	    else
	       free_pkt->frefs[free_pkt->nfiles++] = *fref_ptr;
	 }
	 else if ( *appl_ptr && (*appl_ptr = *keptl_ptr) == 0 )
	    free_pkt->frefs[free_pkt->nfiles++] = *fref_ptr;
	 *keptl_ptr = 0;
      }
      /* send lock downgrade request */
      if ( send_lock() != S_OKAY || send_free() != S_OKAY ) 
	 RETURN( db_status );

      /* clear lock descriptors */
      reset_locks();

      /* reset all key file positions */
      key_reset(size_ft);

      /* clear page buffers */
      dio_clear();
   }
#endif
   RETURN( db_status );
}
#endif


#ifndef NO_TRANS
/* Abort transaction
*/
d_trabort(TASK_ONLY)
TASK_DECL
{
#ifdef SINGLE_USER
   DB_ENTER(NO_DB_ID TASK_ID LOCK_SET(LOCK_IO));
   if (!trans_id)
      RETURN (dberr(S_TRNOTACT));
   trans_id = NULL;
   dio_pzclr();			/*[425] clear page zero BEFORE dio_clear */
   dio_clear();				/*[353] clear cache */
   RETURN (db_status = S_OKAY);
#else
   register int i;
   register int FAR *keptl_ptr;
   register struct lock_descr FAR *ld_ptr;

   DB_ENTER(NO_DB_ID TASK_ID LOCK_SET(LOCK_IO));

   db_status = S_OKAY;
   if ( ! trans_id ) RETURN( dberr(S_TRNOTACT) );

   if ( dbopen == 1 ) {
      /* Revert any kept locks to unkept status */
      for (i = 0, keptl_ptr = kept_locks; i < size_ft; ++i, ++keptl_ptr)
	 *keptl_ptr = 0;
      for (i = 0, ld_ptr = rec_locks; i < size_rt; ++i, ++ld_ptr)
	 ld_ptr->fl_kept = FALSE;
      for (i = 0, ld_ptr = set_locks; i < size_st; ++i, ++ld_ptr)
	 ld_ptr->fl_kept = FALSE;
      for (i = 0, ld_ptr = key_locks; i < keyl_cnt; ++i, ++ld_ptr)
	 ld_ptr->fl_kept = FALSE;
   }
   trans_id = NULL;
   o_init();		/*[305] clear cache_ovfl flag */

   dio_pzclr();		/*[425] clear page zero BEFORE d_freeall */
   if ( dbopen == 1 ) d_freeall(TASK_ONLY);
   dio_clear();

   RETURN( db_status = S_OKAY );
#endif
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
CHAR_P FAR *Table;
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
#ifndef ONE_DB
   if ( old_size ) {
      bytecpy(Temp_table.ptr, table, old_size);
      MEM_UNLOCK(Table);
      FREE(Table);
   }
#endif
   *Table = Temp_table;
   return( db_status );
}
/* vpp -nOS2 -dUNIX -nBSD -nVANILLA_BSD -nVMS -nMEMLOCK -nWINDOWS -nFAR_ALLOC -f/usr/users/master/config/nonwin dblfcns.c */
