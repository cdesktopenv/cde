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


int d_setfiles(P1(int));				/* dio.c */
int d_setpages(P1(int) Pi(int));			/* dio.c */
int d_trbound(P0);				/* trlog.c */
int d_trlog(P1(int) Pi(int) Pi(const char *) Pi(int)); 
								/* trlog.c */
int d_trmark(P0);				/* trlog.c */
int d_tron(P0);					/* trlog.c */
int d_troff(P0);					/* trlog.c */
int d_decode_dba(P1(DB_ADDR) Pi(int *) Pi(long *)); 
								/* dbacode.c */
int d_encode_dba(P1(int) Pi(long) Pi(DB_ADDR *)); 
								/* dbacode.c */

#define d_memlock(p)		/**/
#define d_memlock_group(lg)	S_OKAY
#define d_memunlock(p)		/**/
#define d_memunlock_group(lg)	/**/
char * d_alloc(P1(unsigned));		/* alloc.c */
char * d_calloc(P1(unsigned) Pi(unsigned));	/* alloc.c */
void d_free(P1(CHAR_P *));			/* alloc.c */

#define dt_opentask(a) S_OKAY
#define dt_closetask(a) S_OKAY

int d_close(P0);					/* dblfcns.c */
int d_cmstat(P1(int) DBN_Dn);			/* cmstat.c */
int d_cmtype(P1(int) Pi(int *) DBN_Dn); 	/* cmtype.c */
int d_connect(P1(int) DBN_Dn);			/* connect.c */
int d_cotype(P1(int) Pi(int *) DBN_Dn); 	/* cotype.c */
int d_crget(P1(DB_ADDR *) DBN_Dn);		/* crget.c */
int d_crread(P1(long) Pi(char *) DBN_Dn); 	/* crread.c */
int d_crset(P1(DB_ADDR *) DBN_Dn);		/* crset.c */
int d_crtype(P1(int *) DBN_Dn);		/* crtype.c */
int d_crwrite(P1(long) Pi(char *) DBN_Dn); 	/* crwrite.c */
int d_csmget(P1(int) Pi(DB_ADDR *) DBN_Dn); 	/* csmget.c */
int d_csmread(P1(int) Pi(long) Pi(char *) DBN_Dn); 
								/* csmread.c */
int d_csmset(P1(int) Pi(DB_ADDR *) DBN_Dn); 	/* csmset.c */
int d_csmwrite(P1(int) Pi(long) Pi(const char *) DBN_Dn); 
								/* csmwrite.c */
int d_csoget(P1(int) Pi(DB_ADDR *) DBN_Dn); 	/* csoget.c */
int d_csoread(P1(int) Pi(long) Pi(char *) DBN_Dn); 
								/* csoread.c */
int d_csoset(P1(int) Pi(DB_ADDR *) DBN_Dn); 	/* csoset.c */
int d_csowrite(P1(int) Pi(long) Pi(const char *) DBN_Dn); 
								/* csowrite.c */
int d_ctbpath(P1(const char *));		/* pathfcns.c */
int d_dbdpath(P1(const char *));		/* dbdpath.c */
int d_dbfpath(P1(const char *));		/* dbfpath.c */
int d_dblog(P1(const char *));		/* dblog.c */
int d_dbtaf(P1(const char *));		/* dbtaf.c */
int d_dbuserid(P1(const char *));		/* dbuserid.c */
int d_delete(DBN_D1);				/* delete.c */
int d_disdel(DBN_D1);				/* disdel.c */
int d_recnext(DBN_D1);				/* recnext.c */
int d_recprev(DBN_D1);				/* recprev.c */
int d_destroy(P1(const char *));		/* destroy.c */
int d_discon(P1(int) DBN_Dn);			/* discon.c */
int d_fillnew(P1(int) Pi(const char *) DBN_Dn); /* fillnew.c */
int d_findco(P1(int) DBN_Dn);			/* findco.c */
int d_findfm(P1(int) DBN_Dn);			/* findfm.c */
int d_findlm(P1(int) DBN_Dn);			/* findlm.c */
int d_findnm(P1(int) DBN_Dn);			/* findnm.c */
int d_findpm(P1(int) DBN_Dn);			/* findpm.c */
int d_freeall(P0);				/* dblfcns.c */
int d_initialize(DBN_D1);				/* initial.c */
int d_initfile(P1(FILE_NO) DBN_Dn);  		/* initial.c */
int d_ismember(P1(int) DBN_Dn);			/* ismember.c */
int d_isowner(P1(int) DBN_Dn);			/* isowner.c */
int d_keydel(P1(long) DBN_Dn);			/* keydel.c */
int d_keyexist(P1(long) DBN_Dn);			/* keyexist.c */
int d_keyfind(P1(long) Pi(const char *) DBN_Dn); 
								/* keyfind.c */
int d_keyfree(P1(long) DBN_Dn);			/* dblfcns.c */
int d_keyfrst(P1(long) DBN_Dn);			/* keyfrst.c */
int d_keylast(P1(long) DBN_Dn);			/* keylast.c */
int d_keylock(P1(long) Pi(char *) DBN_Dn); 	/* dblfcns.c */
int d_keylstat(P1(long) Pi(char *) DBN_Dn); 	/* dblfcns.c */
int d_keynext(P1(long) DBN_Dn);			/* keynext.c */
int d_keyprev(P1(long) DBN_Dn);			/* keyprev.c */
int d_keyread(P1(char *));			/* keyfcns.c */
int d_keystore(P1(long) DBN_Dn);			/* keystore.c */
int d_lock(P1(int) Pi(LOCK_REQUEST *) DBN_Dn); /* dblfcns.c */
int d_makenew(P1(int) DBN_Dn);			/* makenew.c */
int d_mapchar(P1(unsigned char) Pi(unsigned char) 
		      Pi(const char *) Pi(unsigned char));  /* mapchar.c */
int d_members(P1(int) Pi(LONG *) DBN_Dn); 	/* members.c */
int d_off_opt(P1(int));				/* options.c */
int d_on_opt(P1(int));				/* options.c */
int d_open(P1(const char *) Pi(const char *)); 
								/* dblfcns.c */
int d_rerdcurr(P1(DB_ADDR **));		/* rwcurr.c */
int d_rdcurr(P1(DB_ADDR **) Pi(int *));	/* rwcurr.c */
int d_wrcurr(P1(DB_ADDR *));			/* rwcurr.c */
int d_recfree(P1(int) DBN_Dn);			/* dblfcns.c */
int d_recfrst(P1(int) DBN_Dn);			/* recfrst.c */
int d_reclast(P1(int) DBN_Dn);			/* reclast.c */
int d_reclock(P1(int) Pi(char *) DBN_Dn); 	/* dblfcns.c */
int d_reclstat(P1(int) Pi(char *) DBN_Dn); 	/* dblfcns.c */
int d_recover(P1(const char *));		/* recover.c */
int d_recread(P1(char *) DBN_Dn);		/* recread.c */
int d_recset(P1(int) DBN_Dn);   			/* recset.c */
int d_recwrite(P1(const char *) DBN_Dn); 	/* recwrite.c */
int d_renfile(P1(const char *) Pi(FILE_NO) 
				Pi(const char *)); 		/* renfile.c */
int d_retries(P1(int));				/* dblfcns.c */
int d_rlbclr(P0);				/* dblfcns.c */
int d_rlbset(P0);				/* dblfcns.c */
int d_rlbtst(P0);				/* dblfcns.c */
int d_set_dberr(P1(FARPROC));			/* dberr.c */
int d_setdb(P1(int));				/* setdb.c */
int d_setfree(P1(int) DBN_Dn);			/* dblfcns.c */
int d_setkey(P1(long) Pi(const char *) DBN_Dn); /* makenew.c */
int d_setlock(P1(int) Pi(char *) DBN_Dn); 	/* dblfcns.c */
int d_setlstat(P1(int) Pi(char *) DBN_Dn); 	/* dblfcns.c */
int d_setmm(P1(int) Pi(int) DBN_Dn); 		/* setmm.c */
int d_setmo(P1(int) Pi(int) DBN_Dn); 		/* setmo.c */
int d_setmr(P1(int) DBN_Dn);			/* setmr.c */
int d_setom(P1(int) Pi(int) DBN_Dn); 		/* setom.c */
int d_setoo(P1(int) Pi(int) DBN_Dn); 		/* setoo.c */
int d_setor(P1(int) DBN_Dn);			/* setor.c */
int d_setrm(P1(int) DBN_Dn);			/* setrm.c */
int d_setro(P1(int) DBN_Dn);			/* setro.c */
int d_timeout(P1(int));				/* dblfcns.c */
int d_trabort(P0);				/* dblfcns.c */
int d_trbegin(P1(const char *));		/* dblfcns.c */
int d_trend(P0);					/* dblfcns.c */

int dberr(P1(int));                      	/* dberr.c */

#define D_FUNCTIONS_H
/* vpp -nOS2 -dUNIX -nBSD -nVANILLA_BSD -nVMS -nMEMLOCK -nWINDOWS -nFAR_ALLOC -f/usr/users/master/config/nonwin dproto.h */
