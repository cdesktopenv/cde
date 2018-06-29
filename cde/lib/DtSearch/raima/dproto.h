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
/* $XConsortium: dproto.h /main/2 1996/05/09 04:05:13 drk $ */
/*
 *   COMPONENT_NAME: austext
 *
 *   FUNCTIONS: Pi
 *		d_alloc
 *		d_calloc
 *		d_cmstat
 *		d_cmtype
 *		d_connect
 *		d_costat
 *		d_cotype
 *		d_crget
 *		d_crread
 *		d_crset
 *		d_crtype
 *		d_crwrite
 *		d_csmget
 *		d_csmread
 *		d_csmset
 *		d_csmwrite
 *		d_csoget
 *		d_csoread
 *		d_csoset
 *		d_csowrite
 *		d_csstat
 *		d_ctbpath
 *		d_ctscm
 *		d_ctsco
 *		d_ctscr
 *		d_dbdpath
 *		d_dbfpath
 *		d_dblog
 *		d_dbtaf
 *		d_dbuserid
 *		d_decode_dba
 *		d_destroy
 *		d_discon
 *		d_encode_dba
 *		d_fillnew
 *		d_findco
 *		d_findfm
 *		d_findlm
 *		d_findnm
 *		d_findpm
 *		d_free
 *		d_gtscm
 *		d_gtsco
 *		d_gtscr
 *		d_gtscs
 *		d_initfile
 *		d_ismember
 *		d_isowner
 *		d_keydel
 *		d_keyexist
 *		d_keyfind
 *		d_keyfree
 *		d_keyfrst
 *		d_keylast
 *		d_keylock
 *		d_keylstat
 *		d_keynext
 *		d_keyprev
 *		d_keyread
 *		d_keystore
 *		d_lock
 *		d_makenew
 *		d_mapchar
 *		d_members
 *		d_memlock
 *		d_memlock_group
 *		d_memunlock
 *		d_memunlock_group
 *		d_off_opt
 *		d_on_opt
 *		d_open
 *		d_rdcurr
 *		d_recfree
 *		d_recfrst
 *		d_reclast
 *		d_reclock
 *		d_reclstat
 *		d_recover
 *		d_recread
 *		d_recset
 *		d_recstat
 *		d_recwrite
 *		d_renfile
 *		d_rerdcurr
 *		d_retries
 *		d_set_dberr
 *		d_setdb
 *		d_setfiles
 *		d_setfree
 *		d_setkey
 *		d_setlock
 *		d_setlstat
 *		d_setmm
 *		d_setmo
 *		d_setmr
 *		d_setom
 *		d_setoo
 *		d_setor
 *		d_setpages
 *		d_setrm
 *		d_setro
 *		d_stscm
 *		d_stsco
 *		d_stscr
 *		d_stscs
 *		d_timeout
 *		d_trbegin
 *		d_trlog
 *		d_utscm
 *		d_utsco
 *		d_utscr
 *		d_utscs
 *		d_wrcurr
 *		dberr
 *		dt_closetask
 *		dt_opentask
 *
 *   ORIGINS: 157
 *
 */
/*----------------------------------------------------------------------------
   dproto.h: db_VISTA (user) function declaration header file

   (C) Copyright 1988 by Raima Corporation.
----------------------------------------------------------------------------*/

/* ********************** EDIT HISTORY *******************************

 SCR    DATE    INI                   DESCRIPTION
----- --------- --- -----------------------------------------------------
  571 31-Jan-89 RSC Placed ctbpath inside NO_COUNTRY, not NO_TIMESTAMP
*/


int d_setfiles(int);				/* dio.c */
int d_setpages(int, int);			/* dio.c */
int d_trbound(void);				/* trlog.c */
int d_trlog(int, int, const char *, int); 
								/* trlog.c */
int d_trmark(void);				/* trlog.c */
int d_tron(void);					/* trlog.c */
int d_troff(void);					/* trlog.c */
int d_decode_dba(DB_ADDR, int *, long *); 
								/* dbacode.c */
int d_encode_dba(int, long, DB_ADDR *); 
								/* dbacode.c */

#define d_memlock(p)		/**/
#define d_memlock_group(lg)	S_OKAY
#define d_memunlock(p)		/**/
#define d_memunlock_group(lg)	/**/
char * d_alloc(unsigned);		/* alloc.c */
char * d_calloc(unsigned, unsigned);	/* alloc.c */
void d_free(CHAR_P *);			/* alloc.c */

#define dt_opentask(a) S_OKAY
#define dt_closetask(a) S_OKAY

int d_close(void);					/* dblfcns.c */
int d_cmstat(int, int);			/* cmstat.c */
int d_cmtype(int, int *, int); 	/* cmtype.c */
int d_connect(int, int);			/* connect.c */
int d_cotype(int, int *, int); 	/* cotype.c */
int d_crget(DB_ADDR *, int);		/* crget.c */
int d_crread(long, char *, int); 	/* crread.c */
int d_crset(DB_ADDR *, int);		/* crset.c */
int d_crtype(int *, int);		/* crtype.c */
int d_crwrite(long, char *, int); 	/* crwrite.c */
int d_csmget(int, DB_ADDR *, int); 	/* csmget.c */
int d_csmread(int, long, char *, int); 
								/* csmread.c */
int d_csmset(int, DB_ADDR *, int); 	/* csmset.c */
int d_csmwrite(int, long, const char *, int); 
								/* csmwrite.c */
int d_csoget(int, DB_ADDR *, int); 	/* csoget.c */
int d_csoread(int, long, char *, int); 
								/* csoread.c */
int d_csoset(int, DB_ADDR *, int); 	/* csoset.c */
int d_csowrite(int, long, const char *, int); 
								/* csowrite.c */
int d_ctbpath(const char *);		/* pathfcns.c */
int d_dbdpath(const char *);		/* dbdpath.c */
int d_dbfpath(const char *);		/* dbfpath.c */
int d_dblog(const char *);		/* dblog.c */
int d_dbtaf(const char *);		/* dbtaf.c */
int d_dbuserid(const char *);		/* dbuserid.c */
int d_delete(int);				/* delete.c */
int d_disdel(int);				/* disdel.c */
int d_recnext(int);				/* recnext.c */
int d_recprev(int);				/* recprev.c */
int d_destroy(const char *);		/* destroy.c */
int d_discon(int, int);			/* discon.c */
int d_fillnew(int, const char *, int); /* fillnew.c */
int d_findco(int, int);			/* findco.c */
int d_findfm(int, int);			/* findfm.c */
int d_findlm(int, int);			/* findlm.c */
int d_findnm(int, int);			/* findnm.c */
int d_findpm(int, int);			/* findpm.c */
int d_freeall(void);				/* dblfcns.c */
int d_initialize(int);				/* initial.c */
int d_initfile(FILE_NO, int);  		/* initial.c */
int d_ismember(int, int);			/* ismember.c */
int d_isowner(int, int);			/* isowner.c */
int d_keydel(long, int);			/* keydel.c */
int d_keyexist(long, int);			/* keyexist.c */
int d_keyfind(long, const char *, int); 
								/* keyfind.c */
int d_keyfree(long, int);			/* dblfcns.c */
int d_keyfrst(long, int);			/* keyfrst.c */
int d_keylast(long, int);			/* keylast.c */
int d_keylock(long, char *, int); 	/* dblfcns.c */
int d_keylstat(long, char *, int); 	/* dblfcns.c */
int d_keynext(long, int);			/* keynext.c */
int d_keyprev(long, int);			/* keyprev.c */
int d_keyread(char *);			/* keyfcns.c */
int d_keystore(long, int);			/* keystore.c */
int d_lock(int, LOCK_REQUEST *, int); /* dblfcns.c */
int d_makenew(int, int);			/* makenew.c */
int d_mapchar(unsigned char, unsigned char, const char *, unsigned char);  /* mapchar.c */
int d_members(int, LONG *, int); 	/* members.c */
int d_off_opt(int);				/* options.c */
int d_on_opt(int);				/* options.c */
int d_open(const char *, const char *); 
								/* dblfcns.c */
int d_rerdcurr(DB_ADDR **);		/* rwcurr.c */
int d_rdcurr(DB_ADDR **, int *);	/* rwcurr.c */
int d_wrcurr(DB_ADDR *);			/* rwcurr.c */
int d_recfree(int, int);			/* dblfcns.c */
int d_recfrst(int, int);			/* recfrst.c */
int d_reclast(int, int);			/* reclast.c */
int d_reclock(int, char *, int); 	/* dblfcns.c */
int d_reclstat(int, char *, int); 	/* dblfcns.c */
int d_recover(const, char *);		/* recover.c */
int d_recread(char *, int);		/* recread.c */
int d_recset(int, int);   			/* recset.c */
int d_recwrite(const char *, int); 	/* recwrite.c */
int d_renfile(const char *dbn, FILE_NO fno, const char *fnm); 		/* renfile.c */
int d_retries(int);				/* dblfcns.c */
int d_rlbclr(void);				/* dblfcns.c */
int d_rlbset(void);				/* dblfcns.c */
int d_rlbtst(void);				/* dblfcns.c */
int d_set_dberr(FARPROC);			/* dberr.c */
int d_setdb(int);				/* setdb.c */
int d_setfree(int, int);			/* dblfcns.c */
int d_setkey(long, const char *, int); /* makenew.c */
int d_setlock(int, char *, int); 	/* dblfcns.c */
int d_setlstat(int, char *, int); 	/* dblfcns.c */
int d_setmm(int, int, int); 		/* setmm.c */
int d_setmo(int, int, int); 		/* setmo.c */
int d_setmr(int, int);			/* setmr.c */
int d_setom(int, int, int); 		/* setom.c */
int d_setoo(int, int, int); 		/* setoo.c */
int d_setor(int, int);			/* setor.c */
int d_setrm(int, int);			/* setrm.c */
int d_setro(int, int);			/* setro.c */
int d_timeout(int);				/* dblfcns.c */
int d_trabort(void);				/* dblfcns.c */
int d_trbegin(const char *);		/* dblfcns.c */
int d_trend(void);					/* dblfcns.c */

int dberr(int);                      	/* dberr.c */

#define D_FUNCTIONS_H
/* vpp -nOS2 -dUNIX -nBSD -nVANILLA_BSD -nVMS -nMEMLOCK -nWINDOWS -nFAR_ALLOC -f/usr/users/master/config/nonwin dproto.h */
