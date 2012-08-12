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
 *   FUNCTIONS: alloc_dict
 *              initcurr
 *              inittab
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
/*-----------------------------------------------------------------------
 $XConsortium: inittab.c /main/5 1996/08/12 12:34:15 cde-ibm $
   inittab.c -- db_VISTA table initialization module.

   (C) Copyright 1987 by Raima Corporation.
-----------------------------------------------------------------------*/

/* ********************** EDIT HISTORY *******************************

 SCR    DATE    INI                   DESCRIPTION
----- --------- --- -----------------------------------------------------
  101 21-Jun-88 RSC Initialized rn_type and rn_dba
      23-Jun-88 RSC Make const_dbd, const_dbf unique to 7 chars
  274 30-Jun-88 RSC Initialization of curr_rec was goofy (erroneous)
  115 19-Jul-88 RSC Integrate VAX/VMS changes into code
      04-Aug-88 RTK MULTI_TASK changes
      18-Aug-88 RSC moved rn_type/dba to separate table.
  417 31-Aug-88 RSC Initialized curr_rn_table.
  420 19-Oct-88 RSC key_count was initialized wrong causing memory overwrites
  438 06-Jan-89 RSC need to explicitly initialize size_xx
  532 06-Jan-89 RSC Fixes to compile with ONE_DB
 *
 * $Log$
 * Revision 1.3  1995/10/17  19:19:59  miker
 * Changed .dbd file open mode from O_RDWR to O_RDONLY.
 * When initializing tables only need to read .dbd file.
 *
 * Revision 1.2  1995/10/13  18:48:16  miker
 * Change hardcoded dbfile[] size from 48 to DtSrFILENMLEN.
 *
 * Revision 1.1  1995/10/13  18:48:00  miker
 * Original vista source.
 */

#include <stdio.h>
#include <fcntl.h>
#include "vista.h"
#include "dbtype.h"
#include "inittab.h"
#include "dbswab.h"

#define DEBUG_INITTAB
int	debugging_inittab =	FALSE;
int	dump_init_tables =	FALSE;


/* Internal function prototypes */
static int alloc_dict(P0);
static int initcurr(P0);




/* Initialize database tables
*/
int
inittab()
{
   register int dbt_lc;			/* loop control */
   register INT i, j;
   int key_offset = 0, key_count;
   char dbfile[DtSrFILENMLEN], dbd_ver[DBD_COMPAT_LEN + 1];
   char dbname[FILENMLEN];	/* Temporary working space */
   int dbf;
   FILE_ENTRY FAR *file_ptr;
   FIELD_ENTRY FAR *fld_ptr;
#ifndef	 ONE_DB
#define	 DB_ENABLE   1
#else
#define	 DB_ENABLE   0
#endif
#ifndef	 NO_TIMESTAMP
#define	 TS_ENABLE   1
#else
#define	 TS_ENABLE   0
#endif
#if   DB_ENABLE | TS_ENABLE
   RECORD_ENTRY FAR *rec_ptr;
   SET_ENTRY FAR *set_ptr;
#endif
#ifndef	 ONE_DB
   MEMBER_ENTRY FAR *mem_ptr;
   SORT_ENTRY FAR *srt_ptr;
   KEY_ENTRY FAR *key_ptr;
#endif

#ifndef	 NO_TIMESTAMP
   db_tsrecs = db_tssets = FALSE;
#endif
   size_ft = size_rt = size_st = size_mt = size_srt = size_fd = size_kt = 0;

   /* compute individual dictionary sizes and offsets */
#ifndef	 ONE_DB
   for (dbt_lc = 0, curr_db_table = &db_table[old_no_of_dbs]; 
	dbt_lc < no_of_dbs; ++dbt_lc, ++curr_db_table) {
#endif

      /* form database dictionary name */
      if ( DB_REF(db_path[0]) )
	 strcpy(dbname, DB_REF(db_path));
      else
	 dbname[0] = '\0';

#ifdef DEBUG_INITTAB
      if (debugging_inittab) {
	    printf (__FILE__"100 inittab: path='%s' dbname='%s'\n",
		dbname, DB_REF(db_name));
	    fflush (stdout);
      }
#endif
      if (strlen(dbname) + strlen(DB_REF(db_name)) >= FILENMLEN+4)
	 return( dberr(S_NAMELEN) );
      strcat(dbname, DB_REF(db_name));
      if (con_dbd(dbfile, dbname, get_element(dbdpath, dbt_lc)) != S_OKAY)
	 return( dberr(db_status) );

      /*----------------- PASS 1 -------------------
       * In this first pass, only opening to determine
       * required table sizes, so opening read-only is ok.
       */
      if ( (dbf = open_b(dbfile, O_RDONLY)) < 0 )
	 return( dberr( S_INVDB ) );

      /* Read in and verify the dictionary version */
      DB_READ(dbf, dbd_ver, DBD_COMPAT_LEN);
      dbd_ver[DBD_COMPAT_LEN] = '\0';
      for ( i=0; i<size_compat; i++ ) {
	 if ( strcmp( dbd_ver, compat_dbd[i] ) == 0 ) goto goodver;
      }

      /* Incompatible dictionary file */
      close( dbf );
      return( dberr( S_INCOMPAT ) );
goodver:

      /* Read in database page size */
      DB_READ(dbf, (char FAR *)&DB_REF(Page_size), sizeof(INT));
      NTOHS (DB_REF(Page_size));

      /* Read in table sizes */
      DB_READ(dbf, (char FAR *)&DB_REF(Size_ft), sizeof(INT));
      NTOHS (DB_REF(Size_ft));
      DB_READ(dbf, (char FAR *)&DB_REF(Size_rt), sizeof(INT));
      NTOHS (DB_REF(Size_rt));
      DB_READ(dbf, (char FAR *)&DB_REF(Size_fd), sizeof(INT));
      NTOHS (DB_REF(Size_fd));
      DB_READ(dbf, (char FAR *)&DB_REF(Size_st), sizeof(INT));
      NTOHS (DB_REF(Size_st));
      DB_READ(dbf, (char FAR *)&DB_REF(Size_mt), sizeof(INT));
      NTOHS (DB_REF(Size_mt));
      DB_READ(dbf, (char FAR *)&DB_REF(Size_srt), sizeof(INT));
      NTOHS (DB_REF(Size_srt));
      DB_READ(dbf, (char FAR *)&DB_REF(Size_kt), sizeof(INT));
      NTOHS (DB_REF(Size_kt));
      close(dbf);	/* end of PASS 1 */

#ifdef DEBUG_INITTAB
      if (debugging_inittab) {
	 printf (__FILE__"152 sizes: pg=%d ft=%d rt=%d fd=%d\n"
	    "  st=%d mt=%d srt=%d kt=%d\n",
	    (int) DB_REF(Page_size),
	    (int) DB_REF(Size_ft),
	    (int) DB_REF(Size_rt),
	    (int) DB_REF(Size_fd),
	    (int) DB_REF(Size_st),
	    (int) DB_REF(Size_mt),
	    (int) DB_REF(Size_srt),
	    (int) DB_REF(Size_kt)
	    );
	  fflush (stdout);
      }
#endif

      DB_REF(sysdba) = NULL_DBA;

#ifndef	 ONE_DB
      /* update merged dictionary offsets and sizes */
      if ( curr_db_table->Page_size > page_size ) {
	    page_size = curr_db_table->Page_size;
#ifdef DEBUG_INITTAB
	    if (debugging_inittab) {
		printf (__FILE__"191 db's page_size-->%d (largest = %d)\n",
		    (int)page_size, (int)largest_page);
		fflush(stdout);
	    }
#endif
      }

      curr_db_table->ft_offset = size_ft;
      size_ft += curr_db_table->Size_ft;
      curr_db_table->rt_offset = size_rt;
      size_rt += curr_db_table->Size_rt;
      curr_db_table->fd_offset = size_fd;
      size_fd += curr_db_table->Size_fd;
      curr_db_table->st_offset = size_st;
      size_st += curr_db_table->Size_st;
      curr_db_table->mt_offset = size_mt;
      size_mt += curr_db_table->Size_mt;
      curr_db_table->srt_offset = size_srt;
      size_srt += curr_db_table->Size_srt;
      curr_db_table->kt_offset = size_kt;
      size_kt += curr_db_table->Size_kt;
   }
#endif
   /* allocate dictionary space */
   if ( alloc_dict() != S_OKAY ) return( db_status );

   /* read in and adjust dictionary entries for each database */
#ifndef	 ONE_DB
   for (dbt_lc = 0, curr_db_table = &db_table[old_no_of_dbs]; 
	dbt_lc < no_of_dbs; 
	++dbt_lc, ++curr_db_table) {
#endif

      /* form database dictionary name */
      if ( DB_REF(db_path[0]) )
	 strcpy(dbname, DB_REF(db_path));
      else
	 dbname[0] = '\0';
      if (strlen(dbname) + strlen(DB_REF(db_name)) >= FILENMLEN+4)
	 return( dberr(S_NAMELEN) );
      strcat(dbname,DB_REF(db_name));
      if (con_dbd(dbfile,dbname,get_element(dbdpath, dbt_lc)) != S_OKAY)
	 return( dberr(db_status) );
#ifdef DEBUG_INITTAB
      if (dump_init_tables) {
	printf (__FILE__"247 Tables for database '%s':\n", dbfile);
	fflush (stdout);
      }
#endif

      /*----------------- PASS 2 -------------------
       * Second pass just loads allocated tables,
       * so opening .dbd file read-only is still ok.
       */
      dbf = open_b (dbfile, O_RDONLY);
      DB_LSEEK(dbf, DBD_COMPAT_LEN + 8L*sizeof(INT), 0);

      /*----------------- FILE TABLE -------------------*/
      DB_READ(dbf, (char FAR *)&file_table[ORIGIN(ft_offset)],
	    (DB_REF(Size_ft)*sizeof(FILE_ENTRY)));
      /* Invalid if sizeof(xxxx_ENTRY) diff on each machine */
      for (	i = 0, file_ptr = &file_table[ORIGIN(ft_offset)];
		i < DB_REF(Size_ft);
		i++, file_ptr++) {
	    /* Byte swap each INT on LITTLE_ENDIAN machines */
	    NTOHS (file_ptr->ft_slots);
	    NTOHS (file_ptr->ft_slsize);
	    NTOHS (file_ptr->ft_pgsize);
	    NTOHS (file_ptr->ft_flags);
#ifdef DEBUG_INITTAB
	    if (dump_init_tables) {
		printf (" FILE#%d: ty=%c slts=%2d slsz=%3d pgsz=%d '%s'\n",
		    (int)i,
		    file_ptr->ft_type,
		    (int)file_ptr->ft_slots,
		    (int)file_ptr->ft_slsize,
		    (int)file_ptr->ft_pgsize,
		    file_ptr->ft_name);
		fflush (stdout);
	    }
#endif
      }

      /*----------------- RECORD TABLE -------------------*/
      DB_READ(dbf, (char FAR *)&record_table[ORIGIN(rt_offset)],
	   (DB_REF(Size_rt)*sizeof(RECORD_ENTRY)));
      for (	i = 0, rec_ptr = &record_table[ORIGIN(rt_offset)];
		i < DB_REF(Size_rt);
		i++, rec_ptr++) {
	    /* Byte swap each INT on LITTLE_ENDIAN machines */
	    NTOHS (rec_ptr->rt_file);
	    NTOHS (rec_ptr->rt_len);
	    NTOHS (rec_ptr->rt_data);
	    NTOHS (rec_ptr->rt_fields);
	    NTOHS (rec_ptr->rt_fdtot);
	    NTOHS (rec_ptr->rt_flags);
#ifdef DEBUG_INITTAB
	    if (dump_init_tables) {
		printf (
		    " REC #%d: fil=%d len=%3d data=%2d fld1=%2d flds=%2d\n",
		    (int)i,
		    (int)rec_ptr->rt_file,
		    (int)rec_ptr->rt_len,
		    (int)rec_ptr->rt_data,
		    (int)rec_ptr->rt_fields,
		    (int)rec_ptr->rt_fdtot);
		fflush (stdout);
	    }
#endif
      }

      /*----------------- FIELD TABLE -------------------*/
      DB_READ(dbf, (char FAR *)&field_table[ORIGIN(fd_offset)],
	   (DB_REF(Size_fd)*sizeof(FIELD_ENTRY)));
      for (	i = 0, fld_ptr = &field_table[ORIGIN(fd_offset)];
		i < DB_REF(Size_fd);
		i++, fld_ptr++) {
	    /* Byte swap each INT on LITTLE_ENDIAN machines */
	    NTOHS (fld_ptr->fd_len);
	    NTOHS (fld_ptr->fd_keyfile);
	    NTOHS (fld_ptr->fd_keyno);
	    NTOHS (fld_ptr->fd_ptr);
	    NTOHS (fld_ptr->fd_rec);
	    NTOHS (fld_ptr->fd_flags);
	    for (j=0;  j<MAXDIMS;  j++)
	        NTOHS (fld_ptr->fd_dim[j]);
#ifdef DEBUG_INITTAB
	    if (dump_init_tables) {
		if (i == 0)
		    puts ("         key typ len kfil key# ofs rec# flg dims");
		printf (
		    " FLD#%2d  %c   %c  %3d  %d    %d   %3d  %d    %x",
		    (int)i,
		    fld_ptr->fd_key,
		    fld_ptr->fd_type,
		    (int)fld_ptr->fd_len,
		    (int)fld_ptr->fd_keyfile,
		    (int)fld_ptr->fd_keyno,
		    (int)fld_ptr->fd_ptr,
		    (int)fld_ptr->fd_rec,
		    (int)fld_ptr->fd_flags);
		for (j=0; j<MAXDIMS; j++)
		    if (fld_ptr->fd_dim[j])
			printf (" %d:%d", j, (int)fld_ptr->fd_dim[j]);
		putchar ('\n');
		fflush (stdout);
	    }
#endif
      }

      /*----------------- SET TABLE -------------------*/
      DB_READ(dbf, (char FAR *)&set_table[ORIGIN(st_offset)],
	   (DB_REF(Size_st)*sizeof(SET_ENTRY)));
      for (	i = 0, set_ptr = &set_table[ORIGIN(st_offset)];
		i < DB_REF(Size_st);
		i++, set_ptr++) {
	    /* Byte swap each INT on LITTLE_ENDIAN machines */
	    NTOHS (set_ptr->st_order);
	    NTOHS (set_ptr->st_own_rt);
	    NTOHS (set_ptr->st_own_ptr);
	    NTOHS (set_ptr->st_members);
	    NTOHS (set_ptr->st_memtot);
	    NTOHS (set_ptr->st_flags);
#ifdef DEBUG_INITTAB
	    if (dump_init_tables) {
		printf (
		    " SET #%d: ord=%c owner=%d ownofs=%2d mem1=%d mems=%d\n",
		    (int)i,
		    (char)set_ptr->st_order,
		    (int)set_ptr->st_own_rt,
		    (int)set_ptr->st_own_ptr,
		    (int)set_ptr->st_members,
		    (int)set_ptr->st_memtot);
		fflush (stdout);
	    }
#endif
      }

      /*----------------- MEMBER TABLE -------------------*/
      DB_READ(dbf, (char FAR *)&member_table[ORIGIN(mt_offset)],
	   (DB_REF(Size_mt)*sizeof(MEMBER_ENTRY)));
      for (	i = 0, mem_ptr = &member_table[ORIGIN(mt_offset)];
		i < DB_REF(Size_mt);
		i++, mem_ptr++) {
	    /* Byte swap each INT on LITTLE_ENDIAN machines */
	    NTOHS (mem_ptr->mt_record);
	    NTOHS (mem_ptr->mt_mem_ptr);
	    NTOHS (mem_ptr->mt_sort_fld);
	    NTOHS (mem_ptr->mt_totsf);
#ifdef DEBUG_INITTAB
	    if (dump_init_tables) {
		printf (
		    " MEM #%d: rec=%d ofs=%d sort1=%d sorts=%d\n",
		    (int)i,
		    (int)mem_ptr->mt_record,
		    (int)mem_ptr->mt_mem_ptr,
		    (int)mem_ptr->mt_sort_fld,
		    (int)mem_ptr->mt_totsf);
		fflush (stdout);
	    }
#endif
      }


      DB_READ(dbf, (char FAR *)&sort_table[ORIGIN(srt_offset)],
	   (DB_REF(Size_srt)*sizeof(SORT_ENTRY)));
      /* Member sort tables not used by DtSearch @@@ */
      if (DB_REF(Size_srt)) {
    	    /* Byte swap each INT on LITTLE_ENDIAN machines */
	    srt_ptr = &sort_table[ORIGIN(srt_offset)];
	    NTOHS (srt_ptr->se_fld);
	    NTOHS (srt_ptr->se_set);
      }

      DB_READ(dbf, (char FAR *)&key_table[ORIGIN(kt_offset)],
	   (DB_REF(Size_kt)*sizeof(KEY_ENTRY)));
      /* Compound key tables not used by DtSearch @@@ */
      if (DB_REF(Size_kt)) {
	    /* Byte swap each INT on LITTLE_ENDIAN machines */
	    key_ptr = &key_table[ORIGIN(kt_offset)];
	    NTOHS (key_ptr->kt_key);
	    NTOHS (key_ptr->kt_field);
	    NTOHS (key_ptr->kt_ptr);
	    NTOHS (key_ptr->kt_sort);
      }

      close(dbf);	/* end of PASS 2 */
#ifdef DEBUG_INITTAB
      dump_init_tables = FALSE;
#endif
#ifndef	 ONE_DB
      curr_db_table->key_offset = key_offset;
#endif

      /* update file table path entries */
      if ( DB_REF(db_path[0]) || dbfpath[0] ) {
	 for (i = 0, file_ptr = &file_table[ORIGIN(ft_offset)];
	      i < DB_REF(Size_ft);
	      ++i, ++file_ptr) {

	    /* Construct the data/key file name */
	    if ( DB_REF(db_path[0]) )
	       strcpy(dbname, DB_REF(db_path));
	    else
	       dbname[0] = '\0';
	    if (strlen(dbname) + strlen(DB_REF(db_name)) >= FILENMLEN+4)
	       return( dberr(S_NAMELEN) );
	    strcat(dbname, DB_REF(db_name));
	    if (con_dbf(dbfile, file_ptr->ft_name, dbname,
	       get_element(dbfpath, dbt_lc)) != S_OKAY)
	       return( dberr(db_status) );

	    /* Save new name in dictionary */
	    strcpy(file_ptr->ft_name, dbfile);
	 } 
      }
#if   DB_ENABLE | TS_ENABLE
      /* adjust record table entries */
      for (i = ORIGIN(rt_offset), rec_ptr = &record_table[ORIGIN(rt_offset)];
	   i < ORIGIN(rt_offset) + DB_REF(Size_rt);
	   ++i, ++rec_ptr) {
#ifndef	 ONE_DB
	 rec_ptr->rt_file += curr_db_table->ft_offset;
	 rec_ptr->rt_fields += curr_db_table->fd_offset;
#endif
#ifndef	 NO_TIMESTAMP
	 if ( rec_ptr->rt_flags & TIMESTAMPED ) {
	    db_tsrecs = TRUE;
#ifdef ONE_DB
	    break;
#endif
	 }
#endif
      }
#endif
      /* adjust field table entries */
      for (key_count = 0, i = ORIGIN(fd_offset), 
	      fld_ptr = &field_table[ORIGIN(fd_offset)];
	   i < ORIGIN(fd_offset) + DB_REF(Size_fd);
	   ++i, ++fld_ptr) {
#ifndef	 ONE_DB
	 fld_ptr->fd_rec += curr_db_table->rt_offset;
#endif
	 if ( fld_ptr->fd_key != NOKEY ) {
	    fld_ptr->fd_keyno += key_offset;
	    ++key_count;
#ifndef	 ONE_DB
	    fld_ptr->fd_keyfile += curr_db_table->ft_offset;
	    if ( fld_ptr->fd_type == 'k' )
	       fld_ptr->fd_ptr += curr_db_table->kt_offset;
#endif
	 }
      }
      key_offset += key_count;

#if   DB_ENABLE | TS_ENABLE
      /* adjust set table entries */
      for (i = ORIGIN(st_offset), set_ptr = &set_table[ORIGIN(st_offset)];
	   i < ORIGIN(st_offset) + DB_REF(Size_st);
	   ++i, ++set_ptr) {
#ifndef	 ONE_DB
	 set_ptr->st_own_rt += curr_db_table->rt_offset;
	 set_ptr->st_members += curr_db_table->mt_offset;
#endif
#ifndef	 NO_TIMESTAMP
	 if ( set_ptr->st_flags & TIMESTAMPED ) {
	    db_tssets = TRUE;
#ifdef ONE_DB
	    break;
#endif
	 }
#endif
      }
#endif

#ifndef	 ONE_DB
      /* adjust member table entries */
      for (i = curr_db_table->mt_offset, 
	      mem_ptr = &member_table[curr_db_table->mt_offset];
	   i < curr_db_table->mt_offset + curr_db_table->Size_mt;
	   ++i, ++mem_ptr) {
	 mem_ptr->mt_record += curr_db_table->rt_offset;
	 mem_ptr->mt_sort_fld += curr_db_table->srt_offset;
      }

      /* adjust sort table entries */
      for (i = curr_db_table->srt_offset, 
	      srt_ptr = &sort_table[curr_db_table->srt_offset];
	   i < curr_db_table->srt_offset + curr_db_table->Size_srt;
	   ++i, ++srt_ptr) {
	 srt_ptr->se_fld += curr_db_table->fd_offset;
	 srt_ptr->se_set += curr_db_table->st_offset;
      }

      /* adjust key table entries */
      for (i = curr_db_table->kt_offset, 
	      key_ptr = &key_table[curr_db_table->kt_offset];
	   i < curr_db_table->kt_offset + curr_db_table->Size_kt;
	   ++i, ++key_ptr) {
	 key_ptr->kt_key += curr_db_table->fd_offset;
	 key_ptr->kt_field += curr_db_table->fd_offset;
      }
   }  /* end loop for each database */
#endif
   initcurr();
   return( db_status );
}



/* Allocate space for dictionary
*/
static int alloc_dict()
{
   int old_size;
   int new_size;
   int extra_file = 0;
#ifndef ONE_DB
   DB_ENTRY FAR *db_ptr;
#endif

   /* allocate and initialize file_table */
#ifndef NO_TRANS
   if ( use_ovfl ) {
      extra_file = 1;
   }
#endif

#ifndef ONE_DB
   if ( old_no_of_dbs == 0 ) {
      old_size_ft = 0;
      old_size_fd = 0;
      old_size_st = 0;
      old_size_mt = 0;
      old_size_srt = 0;
      old_size_kt = 0;
      old_size_rt = 0;
   }
   else {
      db_ptr = &db_table[old_no_of_dbs];
      old_size_ft = db_ptr->Size_ft + db_ptr->ft_offset + extra_file;
      old_size_fd = db_ptr->Size_fd + db_ptr->fd_offset;
      old_size_st = db_ptr->Size_st + db_ptr->st_offset;
      old_size_mt = db_ptr->Size_mt + db_ptr->mt_offset;
      old_size_srt = db_ptr->Size_srt + db_ptr->srt_offset;
      old_size_kt = db_ptr->Size_kt + db_ptr->kt_offset;
      old_size_rt = db_ptr->Size_rt + db_ptr->rt_offset;
   }
#endif

   new_size = (size_ft + extra_file) * sizeof(FILE_ENTRY);
   old_size = old_size_ft * sizeof(FILE_ENTRY);
   if ( ALLOC_TABLE(&db_global.File_table, new_size, old_size, "file_table")
								!= S_OKAY ) {
      return( db_status );
   }

   /* allocate record_table */
   new_size = size_rt * sizeof(RECORD_ENTRY);
   old_size = old_size_rt * sizeof(RECORD_ENTRY);
   if ( ALLOC_TABLE(&db_global.Record_table, new_size, old_size, "record_table")
								!= S_OKAY ) {
      return( db_status );
   }

   /* allocate field_table */
   new_size = size_fd * sizeof(FIELD_ENTRY);
   old_size = old_size_fd * sizeof(FIELD_ENTRY);
   if ( ALLOC_TABLE(&db_global.Field_table, new_size, old_size, "field_table")
   								!= S_OKAY ) {
      return( db_status );
   }

   /* allocate set table */
   if ( size_st ) {
      new_size = size_st * sizeof(SET_ENTRY);
      old_size = old_size_st * sizeof(SET_ENTRY);
      if ( ALLOC_TABLE(&db_global.Set_table, new_size, old_size, "set_table")
								!= S_OKAY ) {
	 return( db_status );
      }
   } else set_table = NULL;

   /* allocate member_table */
   if ( size_mt ) {
      new_size = size_mt * sizeof(MEMBER_ENTRY);
      old_size = old_size_mt * sizeof(MEMBER_ENTRY);
      if ( ALLOC_TABLE(&db_global.Member_table, new_size, old_size, "member_table")
								!= S_OKAY ) {
	 return( db_status );
      }
   } else member_table = NULL;

   /* allocate sort_table */
   if ( size_srt ) {
      new_size = size_srt * sizeof(SORT_ENTRY);
      old_size = old_size_srt * sizeof(SORT_ENTRY);
      if ( ALLOC_TABLE(&db_global.Sort_table, new_size, old_size, "sort_table")
								!= S_OKAY ) {
	 return( db_status );
      }
   } else sort_table = NULL;

   /* allocate key_table */
   if ( size_kt ) {
      new_size = size_kt * sizeof(KEY_ENTRY);
      old_size = old_size_kt * sizeof(KEY_ENTRY);
      if ( ALLOC_TABLE(&db_global.Key_table, new_size, old_size, "key_table")
								!= S_OKAY ) {
	 return( db_status );
      }
   } else key_table = NULL;

   return( db_status = S_OKAY );
}

/* Initialize currency tables 
*/
static int initcurr()
{
   register int dbt_lc;			/* loop control */
   register int rec, i;
   RECORD_ENTRY FAR *rec_ptr;
   SET_ENTRY FAR *set_ptr;
   DB_ADDR FAR *co_ptr;
   int old_size;
   int new_size;

   /* Initialize current record and type */
#ifndef	 ONE_DB
   for (dbt_lc = no_of_dbs, curr_db_table = &db_table[old_no_of_dbs],
				curr_rn_table = &rn_table[old_no_of_dbs];
	--dbt_lc >= 0; ++curr_db_table, ++curr_rn_table) {
      DB_REF(curr_dbt_rec) = NULL_DBA;
#endif
      RN_REF(rn_dba)   = NULL_DBA;
      RN_REF(rn_type)  = -1;
#ifndef ONE_DB
   }
#endif

   if ( size_st ) {
      new_size = size_st * sizeof(DB_ADDR);
      old_size = old_size_st * sizeof(DB_ADDR);
      if ( ALLOC_TABLE(&db_global.Curr_own, new_size, old_size, "curr_own")
								!= S_OKAY ) {
	 return( db_status );
      }
      if ( ALLOC_TABLE(&db_global.Curr_mem, new_size, old_size, "curr_mem")
								!= S_OKAY ) {
	 return( db_status );
      }
#ifndef	 NO_TIMESTAMP
      new_size = size_st * sizeof(ULONG);
      old_size = old_size_st * sizeof(ULONG);
      if ( db_tsrecs ) {
	 if ( ALLOC_TABLE(&db_global.Co_time, new_size, old_size, "co_time")
								!= S_OKAY ) {
	    return( db_status );
	 }
	 if ( ALLOC_TABLE(&db_global.Cm_time, new_size, old_size, "cm_time")
								!= S_OKAY ) {
	    return( db_status );
	 }
      }
      if ( db_tssets ) {
	 if ( ALLOC_TABLE(&db_global.Cs_time, new_size, old_size, "cs_time")
								!= S_OKAY ) {
	    return( db_status );
	 }
      }
#endif
      /* for each db make system record as curr_own of its sets */
#ifndef	 ONE_DB
      for (dbt_lc = no_of_dbs, curr_db_table = &db_table[old_no_of_dbs]; 
	   --dbt_lc >= 0; ++curr_db_table) {
#endif
	 for (rec = ORIGIN(rt_offset), 
		 rec_ptr = &record_table[ORIGIN(rt_offset)];
	      rec < ORIGIN(rt_offset) + DB_REF(Size_rt);
	      ++rec, ++rec_ptr) {
	    if (rec_ptr->rt_fdtot == -1) { 
	       /* found system record */
	       curr_rec = ((FILEMASK & NUM2EXT(rec_ptr->rt_file, ft_offset))
							    << FILESHIFT) | 1L;
	       /* make system record current of sets it owns */
	       for (i = ORIGIN(st_offset), 
		       set_ptr = &set_table[ORIGIN(st_offset)],
		       co_ptr = &curr_own[ORIGIN(st_offset)];
		    i < ORIGIN(st_offset) + DB_REF(Size_st);
		    ++i, ++set_ptr, ++co_ptr) {
 		  if (set_ptr->st_own_rt == rec) {
		     *co_ptr = curr_rec;
		  }
	       }
	       DB_REF(sysdba) = curr_rec;
#ifndef ONE_DB
 	       DB_REF(curr_dbt_rec) = curr_rec;
#endif
	       break;
	    }
	 }
#ifndef	 ONE_DB
      }
#endif
   }
   else {
      curr_own = NULL;
      curr_mem = NULL;
   }
#ifndef	 ONE_DB
   curr_db = 0;
   MEM_LOCK(&db_global.Db_table);
   curr_db_table = db_table;
   MEM_LOCK(&db_global.Rn_table);
   curr_rn_table = rn_table;
   setdb_on = FALSE;
   curr_rec = DB_REF(curr_dbt_rec);
#endif
   return( db_status = S_OKAY );
}
/* vpp -nOS2 -dUNIX -nBSD -nVANILLA_BSD -nVMS -nMEMLOCK -nWINDOWS -nFAR_ALLOC -f/usr/users/master/config/nonwin inittab.c */
