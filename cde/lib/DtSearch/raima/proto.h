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
void dbautorec(P0);

/* From dblfcns.c: */
int taskinit(P1(TASK FAR *));
int initdbt(P1(CONST char FAR *));
int alloc_table(P1(CHAR_P FAR *) Pi(unsigned) Pi(unsigned));
#define ALLOC_TABLE(t, n, o, v)	alloc_table((CHAR_P FAR *)(t), n, o)
#ifndef SINGLE_USER
int  termses(P0);
#endif
void termfree(P0);
#ifndef SINGLE_USER
int neterr(P0);
#endif

/* From dio.c: */
int dio_open(P1(FILE_NO));
int dio_close(P1(FILE_NO));
int dio_init(P0);
void dio_free(P0);
int dio_clrfile(P1(FILE_NO));
int dio_clear(P0);
int dio_flush(P0);
void dio_setdef(P1(FILE_NO));
int dio_get(P1(F_ADDR) Pi(char FAR * FAR *) Pi(int));
int dio_touch(P1(F_ADDR));
int dio_read(P1(DB_ADDR) Pi(char FAR * FAR *) Pi(int));
int dio_write(P1(DB_ADDR) Pi(CONST char FAR *) Pi(int));
int dio_release(P1(DB_ADDR));
int dio_rrlb(P1(DB_ADDR) Pi(INT *));
int dio_wrlb(P1(DB_ADDR) Pi(INT));
int dio_findpg(P1(FILE_NO) Pi(F_ADDR) Pi(PAGE_ENTRY FAR *)
			      Pi(PAGE_ENTRY FAR * FAR *) 
			      Pi(LOOKUP_ENTRY FAR * FAR *));
#ifdef NO_TRANS
int dio_out(P1(PAGE_ENTRY FAR *) Pi(LOOKUP_ENTRY FAR *));
#else
int dio_out(P1(PAGE_ENTRY FAR *) Pi(LOOKUP_ENTRY FAR *) 
			   Pi(BOOLEAN));
#endif
#ifndef  NO_TIMESTAMP
ULONG dio_pzsetts(P1(FILE_NO));
ULONG dio_pzgetts(P1(FILE_NO));
#endif
int dio_pzread(P1(FILE_NO));
int dio_pzalloc(P1(FILE_NO) Pi(F_ADDR *));
int dio_pzdel(P1(FILE_NO) Pi(F_ADDR));
F_ADDR dio_pznext(P1(FILE_NO));
void dio_pzclr(P0);

/* From initenv.c: */
int initenv(P0);

/* From inittab.c: */
int inittab(P0);

/* From keyfcns.c: */
int key_open(P0);
void key_close(P0);
int key_init(P1(int));
int key_reset(P1(FILE_NO));
int key_locpos(P1(CONST char FAR *) Pi(DB_ADDR FAR *));
int key_scan(P1(int) Pi(DB_ADDR *));
int key_boundary(P1(int) Pi(DB_ADDR *));
int key_insert(P1(int) Pi(CONST char FAR *) Pi(DB_ADDR));
int key_delete(P1(int) Pi(CONST char FAR *) Pi(DB_ADDR));
/* Due to a problem with MSC 5.1, we couldn't declare the 2nd parm
   of key_bldcom as const (see keyfcns.c) */
int key_bldcom(P1(int) Pi(char FAR *) Pi(char FAR *) Pi(int));
/* Because of a "problem" with the MSC 5.1 compiler, we couldn't declare
   the second argument to key_cmpcpy with a const attribute.  Hence we
   were not able to define the 2nd argument of key_bldcom as const.  */
void key_cmpcpy(P1(char FAR *) Pi(char FAR *) Pi(INT));

/* From libfcns.c: */
#ifndef ONE_DB
int dbn_check(P1(int));
#endif
int nset_check(P1(int) Pi(int *) Pi(SET_ENTRY FAR * FAR *));
int nfld_check(P1(long) Pi(int *) Pi(int *) 
			      Pi(RECORD_ENTRY FAR * FAR *) 
			      Pi(FIELD_ENTRY FAR * FAR *));
int nrec_check(P1(int) Pi(int *) Pi(RECORD_ENTRY FAR * FAR *));
int fldcmp(P1(FIELD_ENTRY FAR *) Pi(CONST char FAR *) 
			     Pi(CONST char FAR *));
int INTcmp(P1(CONST char FAR *) Pi(CONST char FAR *));
int ADDRcmp(P1(CONST DB_ADDR FAR *) Pi(CONST DB_ADDR FAR *));
int null_dba(P1(CONST char FAR *));
int check_dba(P1(DB_ADDR));

/* From makenew.c: */
int sk_free(P0);

/* From netwdos.c: */
int netbios_chk(P0);
int nw_reset(P1(int) Pi(int));
int nw_addnm(P1(char FAR *) Pi(int *));
int nw_cleanup(P1(char FAR *));
int nw_sestat(P0);
int nw_delnm(P1(char FAR *));
int nw_call(P1(char FAR *) Pi(char FAR *) Pi(int *));
int nw_hangup(P1(int));
int nw_send(P1(int) Pi(MESSAGE FAR *) Pi(int));
int nw_rcvmsg(P1(int) Pi(MESSAGE FAR *) Pi(int) Pi(int *));

/* From opens.c: */
int open_b(P1(CONST char FAR *) Pi(int));

/* From ovfcns.c: */
int o_setup(P0);
int o_init(P0);
int o_fileinit(P1(FILE_NO));
int o_search(P1(FILE_NO) Pi(F_ADDR) Pi(F_ADDR *));
int o_write(P1(PAGE_ENTRY FAR *) Pi(LOOKUP_ENTRY FAR *));
int o_pzwrite(P1(FILE_NO));
int o_flush(P0);
int o_update(P0);
long o_pages(P1(FILE_NO));
void o_free(P0);

/* From recfcns.c: */
int r_chkfld(P1(INT) Pi(FIELD_ENTRY FAR *) Pi(char FAR *)
	     Pi(CONST char FAR *));
int r_delrec(P1(INT) Pi(DB_ADDR));
int r_gfld(P1(FIELD_ENTRY FAR *) Pi(char FAR *) Pi(char FAR *));
int r_gmem(P1(int) Pi(char FAR *) Pi(char FAR *));
int r_gset(P1(int) Pi(char FAR *) Pi(char FAR *));
int r_pfld(P1(INT) Pi(FIELD_ENTRY FAR *) Pi(char FAR *) 
			     Pi(CONST char FAR *) Pi(DB_ADDR FAR *));
int r_pmem(P1(int) Pi(char FAR *) Pi(char FAR *));
int r_pset(P1(int) Pi(char FAR *) Pi(char FAR *));
int r_smem(P1(DB_ADDR FAR *) Pi(INT));
int r_setopt(P1(FIELD_ENTRY FAR *) Pi(char FAR *));
int r_clropt(P1(FIELD_ENTRY FAR *) Pi(char FAR *));
int r_tstopt(P1(FIELD_ENTRY FAR *) Pi(char FAR *));

/* From pathfcns.c */
int con_dbd(P1(char FAR *) Pi(char FAR *) Pi(char FAR *));
int con_dbf(P1(char FAR *) Pi(char FAR *) Pi(char FAR *) 
			   Pi(char FAR *));
char FAR * get_element(P1(char FAR *) Pi(int));

/* From renfile.c: */
int renfiles(P0);

/* From startup.c: */
#ifndef NO_DBN_PARM
int startup(DBN_FIX_D1 TASK_PTR_Di LOCK_Di);
#else
#ifdef MULTI_TASK
int startup(TASK_PTR_D1 LOCK_Di);
#else
#define startup() S_OKAY
#endif
#endif

/* From taffcns.c: */
int taf_open(P0);
int taf_close(P0);
int taf_access(P0);
int taf_release(P0);
int taf_add(P1(CONST char FAR *));
int taf_del(P1(CONST char FAR *) );

/* From task.c */
#ifdef MULTI_TASK
void task_switch(P1(DB_TASK FAR *));
#endif

/* From mapchar.c */
int  ctb_init(P0);
int  ctbl_alloc(P0);
void ctbl_free(P0);

/* System functions */
#ifndef _STRING_H
#ifndef linux
  void CDECL memcpy(P1(char *) Pi(char *) Pi(int));
#endif
  int  CDECL memcmp(P1(char *) Pi(char *) Pi(int));
  void CDECL memset(P1(char *) Pi(char) Pi(int));

  int CDECL strcmp(P1(CONST char *) Pi(CONST char *));
  char * CDECL strcpy(P1(char *) Pi(CONST char *));
  char * CDECL strcat(P1(char *) Pi(CONST char *));
  int CDECL strlen(P1(CONST char *));
  int CDECL strncmp(P1(CONST char *) Pi(CONST char *) Pi(int));
  char * CDECL strncpy(P1(char *) Pi(CONST char *) Pi(int));
  char * CDECL strchr(P1(CONST char *) Pi(int));
  char * CDECL strrchr(P1(CONST char *) Pi(int));
#endif

void CDECL free(P1(void FAR *));  /* AIXV3 - DPG 07/05/90 */
time_t CDECL time(P1(long *));
int CDECL write(P1(int) Pi(char *) Pi(unsigned int));
int CDECL read(P1(int) Pi(char *) Pi(unsigned int));
long CDECL lseek(P1(int) Pi(long) Pi(int));
int CDECL close(P1(int));
void CDECL exit(P1(int));
char * CDECL getenv(P1(CONST char *));
int CDECL locking(P1(int) Pi(int) Pi(long));
int CDECL unlink(P1(CONST char *));
/* vpp -nOS2 -dUNIX -nBSD -nVANILLA_BSD -nVMS -nMEMLOCK -nWINDOWS -nFAR_ALLOC -f/usr/users/master/config/nonwin proto.h */
