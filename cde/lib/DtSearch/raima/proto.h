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
/* $TOG: proto.h /main/5 1998/03/19 19:00:54 mgreess $ */
/*
 *   COMPONENT_NAME: austext
 *
 *   FUNCTIONS: ADDRcmp
 *		ALLOC_TABLE
 *		INTcmp
 *		Pi
 *		alloc_table
 *		check_dba
 *		close
 *		con_dbd
 *		con_dbf
 *		dbn_check
 *		dio_close
 *		dio_clrfile
 *		dio_findpg
 *		dio_get
 *		dio_open
 *		dio_out
 *		dio_pzalloc
 *		dio_pzdel
 *		dio_pzgetts
 *		dio_pznext
 *		dio_pzread
 *		dio_pzsetts
 *		dio_read
 *		dio_release
 *		dio_rrlb
 *		dio_setdef
 *		dio_touch
 *		dio_write
 *		dio_wrlb
 *		exit
 *		fldcmp
 *		free
 *		get_element
 *		getenv
 *		initdbt
 *		key_bldcom
 *		key_boundary
 *		key_cmpcpy
 *		key_delete
 *		key_init
 *		key_insert
 *		key_locpos
 *		key_reset
 *		key_scan
 *		locking
 *		lseek
 *		memcmp
 *		memcpy
 *		memset
 *		nfld_check
 *		nrec_check
 *		nset_check
 *		null_dba
 *		nw_addnm
 *		nw_call
 *		nw_cleanup
 *		nw_delnm
 *		nw_hangup
 *		nw_rcvmsg
 *		nw_reset
 *		nw_send
 *		o_fileinit
 *		o_pages
 *		o_pzwrite
 *		o_search
 *		o_write
 *		open_b
 *		r_chkfld
 *		r_clropt
 *		r_delrec
 *		r_gfld
 *		r_gmem
 *		r_gset
 *		r_pfld
 *		r_pmem
 *		r_pset
 *		r_setopt
 *		r_smem
 *		r_tstopt
 *		read
 *		startup
 *		strcat
 *		strchr
 *		strcmp
 *		strcpy
 *		strlen
 *		strncmp
 *		strncpy
 *		strrchr
 *		taf_add
 *		taf_del
 *		task_switch
 *		taskinit
 *		time
 *		unlink
 *		write
 *
 *   ORIGINS: 157
 *
 */
/*----------------------------------------------------------------------------
   proto.h: db_VISTA function declaration header file

   (This file must be included after dbtype.h)

   (C) Copyright 1988 by Raima Corporation.
----------------------------------------------------------------------------*/

/* ********************** EDIT HISTORY *******************************

 SCR    DATE    INI                   DESCRIPTION
----- --------- --- -----------------------------------------------------
      03-AUG-88 RTK Added internal MULTI_TASKing function prototypes
  310 10-Aug-88 RSC Cleanup of function prototypes

*/

/* From dberr.c: */
void dbautorec(void);

/* From dblfcns.c: */
int taskinit(TASK *);
int initdbt(const char *);
int alloc_table(CHAR_P *, unsigned, unsigned);
#define ALLOC_TABLE(t, n, o, v)	alloc_table((CHAR_P *)(t), n, o)
void termfree(void);

/* From dio.c: */
int dio_open(FILE_NO);
int dio_close(FILE_NO);
int dio_init(void);
void dio_free(void);
int dio_clrfile(FILE_NO);
int dio_clear(void);
int dio_flush(void);
void dio_setdef(FILE_NO);
int dio_get(F_ADDR, char **, int);
int dio_touch(F_ADDR);
int dio_read(DB_ADDR, char **, int);
int dio_write(DB_ADDR, const char *, int);
int dio_release(DB_ADDR);
int dio_rrlb(DB_ADDR, INT *);
int dio_wrlb(DB_ADDR, INT);
int dio_findpg(FILE_NO, F_ADDR, PAGE_ENTRY *, PAGE_ENTRY **, LOOKUP_ENTRY **);
int dio_out(PAGE_ENTRY *, LOOKUP_ENTRY *);
int dio_pzread(FILE_NO);
int dio_pzalloc(FILE_NO, F_ADDR *);
int dio_pzdel(FILE_NO, F_ADDR);
F_ADDR dio_pznext(FILE_NO);
void dio_pzclr(void);

/* From initenv.c: */
int initenv(void);

/* From inittab.c: */
int inittab(void);

/* From keyfcns.c: */
int key_open(void);
void key_close(void);
int key_init(int);
int key_reset(FILE_NO);
int key_locpos(const char *, DB_ADDR *);
int key_scan(int, DB_ADDR *);
int key_boundary(int, DB_ADDR *);
int key_insert(int, const char *, DB_ADDR);
int key_delete(int, const char *, DB_ADDR);
/* Due to a problem with MSC 5.1, we couldn't declare the 2nd parm
   of key_bldcom as const (see keyfcns.c) */
int key_bldcom(int, char *, char *, int);
/* Because of a "problem" with the MSC 5.1 compiler, we couldn't declare
   the second argument to key_cmpcpy with a const attribute.  Hence we
   were not able to define the 2nd argument of key_bldcom as const.  */
void key_cmpcpy(char *, char *, INT);

/* From libfcns.c: */
int dbn_check(int);
int nset_check(int, int *, SET_ENTRY **);
int nfld_check(long, int *, int *, RECORD_ENTRY **, FIELD_ENTRY **);
int nrec_check(int, int *, RECORD_ENTRY **);
int fldcmp(FIELD_ENTRY *, const char *, const char *);
int INTcmp(const char *, const char *);
int ADDRcmp(const DB_ADDR *, const DB_ADDR *);
int null_dba(const char *);
int check_dba(DB_ADDR);

/* From makenew.c: */
int sk_free(void);

/* From netwdos.c: */
int netbios_chk(void);
int nw_reset(int, int);
int nw_addnm(char *, int *);
int nw_cleanup(char *);
int nw_sestat(void);
int nw_delnm(char *);
int nw_call(char *, char *, int *);
int nw_hangup(int);
int nw_send(int, MESSAGE *, int);
int nw_rcvmsg(int, MESSAGE *, int, int *);

/* From opens.c: */
int open_b(const char *, int);

/* From ovfcns.c: */
int o_setup(void);
int o_init(void);
int o_fileinit(FILE_NO);
int o_search(FILE_NO, F_ADDR, F_ADDR *);
int o_write(PAGE_ENTRY *, LOOKUP_ENTRY *);
int o_pzwrite(FILE_NO);
int o_flush(void);
int o_update(void);
long o_pages(FILE_NO);
void o_free(void);

/* From recfcns.c: */
int r_chkfld(INT, FIELD_ENTRY *, char *, const char *);
int r_delrec(INT, DB_ADDR);
int r_gfld(FIELD_ENTRY *, char *, char *);
int r_gmem(int, char *, char *);
int r_gset(int, char *, char *);
int r_pfld(INT, FIELD_ENTRY *, char *, const char *, DB_ADDR *);
int r_pmem(int, char *, char *);
int r_pset(int, char *, char *);
int r_smem(DB_ADDR *, INT);
int r_setopt(FIELD_ENTRY *, char *);
int r_clropt(FIELD_ENTRY *, char *);
int r_tstopt(FIELD_ENTRY *, char *);

/* From pathfcns.c */
int con_dbd(char *, char *, char *);
int con_dbf(char *, char *, char *, char *);
char * get_element(char *, int);

/* From renfile.c: */
int renfiles(void);

/* From startup.c: */
int startup(DBN_FIX_D1 TASK_PTR_Di LOCK_Di);

/* From taffcns.c: */
int taf_open(void);
int taf_close(void);
int taf_access(void);
int taf_release(void);
int taf_add(const char *);
int taf_del(const char *);

/* From mapchar.c */
int  ctb_init(void);
int  ctbl_alloc(void);
void ctbl_free(void);

/* vpp -nOS2 -dUNIX -nBSD -nVANILLA_BSD -nVMS -nMEMLOCK -nWINDOWS -nFAR_ALLOC -f/usr/users/master/config/nonwin proto.h */
