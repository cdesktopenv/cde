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
/* $XConsortium: initial.c /main/3 1996/08/12 12:34:00 cde-ibm $ */
/*
 *   COMPONENT_NAME: austext
 *
 *   FUNCTIONS: d_initfile
 *		d_initialize
 *
 *   ORIGINS: 157
 *
 *   OBJECT CODE ONLY SOURCE MATERIALS
 */
/*-----------------------------------------------------------------------
   initial.c -- db_VISTA database initialization module.

   (C) Copyright 1987 by Raima Corporation.
-----------------------------------------------------------------------*/

/* ********************** EDIT HISTORY *******************************

 SCR    DATE    INI                   DESCRIPTION
----- --------- --- -----------------------------------------------------
  295 01-Jul-88 RSC d_initialize/d_initfile can corrupt disk
  115 19-Jul-88 RSC Integrate VAX/VMS changes into code
  365 25-Jul-88 RSC Add dbn argument to d_initialize
  368 28-Jul-88 RSC Integrate BSD changes into code
      04-Aug-88 RTK MULTI_TASK changes
  115 16-Aug-88 RSC Fixup of VMS integration
*/

#include <stdio.h>
#include <fcntl.h>
#include "vista.h"
#include "dbtype.h"
#include "dbswab.h"

#define LENVDBID 48

typedef union INIT_PAGE_U {
   struct {
      F_ADDR  dchain;              /* delete chain pointer */
      F_ADDR  next;                /* next page or record slot */
      LONG    timestamp;           /* file's timestamp value */
      LONG    cdate;               /* creation date,time */
      LONG    bdate;               /* date/time of last backup */
      char vdb_id[LENVDBID];       /* db_vista id mark */
   } pg0;
   struct {
      LONG  chg_date;              /* date of last page change */
      char  init_int[sizeof(INT)];  /* # filled slots on key file;
				      System record # on data file */
      char  init_addr[sizeof(LONG)];  /* NONE node pointer on key file;
				      System record db_addr on data file */
      char  init_crts[sizeof(LONG)];  /* if system record is timestamped */
      char  init_upts[sizeof(LONG)];  /* if system record is timestamped */
   } pg1;
} INIT_PAGE;

typedef struct {union INIT_PAGE_U FAR *ptr; LOCK_DESC} INIT_PAGE_P;

static char nulls[5] = "\0\0\0\0";
static int dbfile;
extern int cnt_open_files;		/* see dio.c */
extern int max_open_files;		/* see dio.c */


/* Database initialization function
*/
int
d_initialize(TASK_DBN_ONLY)
TASK_DECL
DBN_DECL
{
   FILE_NO fno;

   DB_ENTER(DB_ID TASK_ID LOCK_SET(RECORD_IO));

   if (dbopen != 2)
      dberr(S_EXCLUSIVE);
   else {
      /* initialize db files in file_table */
      for (fno = 0; fno < DB_REF(Size_ft); ++fno) {
	 if ( d_initfile(fno TASK_PARM DBN_PARM) != S_OKAY )
	    break;
      }
   }
   RETURN( db_status );
}



/* Initialize database file
*/
int
d_initfile(fno TASK_PARM DBN_PARM)
FILE_NO fno; /* file table entry of file to be initialized */
TASK_DECL
DBN_DECL
{
   INIT_PAGE_P Page;
#define page (Page.ptr)
   F_ADDR addr;
   ULONG ts;
   INT rno, rec;
   FILE_ENTRY FAR *file_ptr;
   RECORD_ENTRY FAR *rec_ptr;
   time_t	local_timestamp;
   LONG		extern_timestamp;

   DB_ENTER(DB_ID TASK_ID LOCK_SET(RECORD_IO));

   if ( dbopen != 2 )
      RETURN( dberr(S_EXCLUSIVE) );

   time (&local_timestamp);
   extern_timestamp = htonl ((LONG) local_timestamp);

#ifndef	 ONE_DB
   fno += curr_db_table->ft_offset;
#endif
   file_ptr = &file_table[fno];

   /* If file is open - close it */
   if (file_ptr->ft_status == OPEN) dio_close(fno);
   else if (cnt_open_files == max_open_files) {
      /* We don't have enuf files, open then close this file to free one */
      dio_open(fno);
      dio_close(fno);
   }
   dio_clrfile(fno); 			/* clear file's pages */

   if ((dbfile = DB_OPEN(file_ptr->ft_name, O_RDWR | O_CREAT | O_TRUNC)) < 0)
      RETURN( dberr(S_NOFILE) );
   
   page = (INIT_PAGE FAR *)ALLOC(&Page, file_ptr->ft_pgsize, "page");
   if ( page == NULL ) RETURN( dberr(S_NOMEMORY) );

   /*--------- Init PAGE 0 ---------*/
   byteset(page, '\0', file_ptr->ft_pgsize);
   /**time(&page->pg0.cdate);***/
   page->pg0.cdate =		extern_timestamp;
   strcpy(page->pg0.vdb_id,	"db_VISTA Version ");
   strcat(page->pg0.vdb_id,	db_VERSION);
   page->pg0.bdate =		0;
   if (file_ptr->ft_type == KEY) {
      /*--------- Write KEY FILE PAGE 0 ---------*/
      page->pg0.dchain =	htonl ((LONG) NONE);
      page->pg0.next =		htonl (2);
      page->pg0.timestamp =	0; /* not really used by key file */
      DB_WRITE(dbfile, (char FAR *)page, (int)file_ptr->ft_pgsize);

      /*--------- Write KEY FILE PAGE 1 ---------*/
      byteset(page, '\0', file_ptr->ft_pgsize);
      page->pg1.chg_date =	extern_timestamp;
      /* node 1, current # of filled slots */
      bytecpy(page->pg1.init_int, nulls, sizeof(INT));
      /* node 1, NONE page pointer */
      addr =			-1;
      bytecpy(page->pg1.init_addr, &addr, sizeof(F_ADDR));
      DB_WRITE(dbfile, (char FAR *)page, (int)file_ptr->ft_pgsize);
   }
   else {
      /*--------- Init DATA PAGE 0 ---------
       * We'll write a page 0 and page 1 for the file (if any)
       * with the system record, otherwise just a generic page 0.
       */
      page->pg0.dchain =	0;
      page->pg0.timestamp =	htonl (1); 
      /* check to see if this file contains a system record */
      for (rec = 0, rec_ptr = record_table; rec < size_rt; ++rec, ++rec_ptr) {
	 if ((rec_ptr->rt_fdtot == -1) && (rec_ptr->rt_file == fno)) {

	    /*---Write special DATA FILE PAGE 0 for system record file ---*/
	    page->pg0.next =	htonl (2);
	    DB_WRITE(dbfile, (char FAR *)page, (int)file_ptr->ft_pgsize);

	    /*--------- Write DATA FILE PAGE 1 for system record ---------*/
	    byteset(page, '\0', file_ptr->ft_pgsize);
	    /* Data page header is current timestamp */
	    /*****time(&page->pg1.chg_date);****/
	    page->pg1.chg_date =	extern_timestamp;
	    addr =		(((LONG) NUM2EXT(fno, ft_offset) & FILEMASK)
					<< FILESHIFT) | 1;
	    HTONL (addr);
	    bytecpy(page->pg1.init_addr, &addr, sizeof(F_ADDR));

	    /* Slot 1 (system record) header is recid, dba,
	     * maybe two phony timestamps, no set ptrs or member ptrs.
	     */
	    rno =		htons (NUM2EXT(rec, rt_offset));
	    bytecpy(page->pg1.init_int, &rno, sizeof(INT));
	    if ( rec_ptr->rt_flags & TIMESTAMPED ) {
	       /* timestamp system record */
	       ts =		htonl (1);
	       bytecpy(page->pg1.init_crts, &ts, sizeof(ULONG));
	       bytecpy(page->pg1.init_upts, &ts, sizeof(ULONG));
	    }
	    DB_WRITE(dbfile, (char FAR *)page, (int)file_ptr->ft_pgsize);
	    break;
	 }
      }
      /*--------- Write generic DATA FILE PAGE 0 ---------
       * If we went through whole record table without finding
       * a system record, just write a generic page 0.
       */
      if (rec == size_rt) {
	 page->pg0.next =	htonl (1);
	 DB_WRITE(dbfile, (char FAR *)page, (int)file_ptr->ft_pgsize);
      }
   }
   /* close database file */
   DB_CLOSE(dbfile);
   dio_pzread(fno);  /* re-read file header */
   MEM_UNLOCK(&Page);
   FREE(&Page);
   RETURN( db_status );
}
/* vpp -nOS2 -dUNIX -nBSD -nVANILLA_BSD -nVMS -nMEMLOCK -nWINDOWS -nFAR_ALLOC -f/usr/users/master/config/nonwin initial.c */
