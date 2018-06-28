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
/* $XConsortium: task.h /main/2 1996/05/09 04:20:28 drk $ */
/*
 *   COMPONENT_NAME: austext
 *
 *   FUNCTIONS: Pi
 *		d_close
 *		d_cmstat
 *		d_cmtype
 *		d_connect
 *		d_costat
 *		d_cotype
 *		d_crget
 *		d_crread
 *		d_crset
 *		d_crstat
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
 *		d_ctscm
 *		d_ctsco
 *		d_ctscr
 *		d_dbdpath
 *		d_dbfpath
 *		d_dblog
 *		d_dbuserid
 *		d_delete
 *		d_destroy
 *		d_discon
 *		d_disdel
 *		d_fillnew
 *		d_findco
 *		d_findfm
 *		d_findlm
 *		d_findnm
 *		d_findpm
 *		d_freeall
 *		d_gtscm
 *		d_gtsco
 *		d_gtscr
 *		d_gtscs
 *		d_initfile
 *		d_initialize
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
 *		d_members
 *		d_off_opt
 *		d_on_opt
 *		d_open
 *		d_rdcurr
 *		d_recfree
 *		d_recfrst
 *		d_reclast
 *		d_reclock
 *		d_reclstat
 *		d_recnext
 *		d_recover
 *		d_recprev
 *		d_recread
 *		d_recset
 *		d_recstat
 *		d_recwrite
 *		d_renfile
 *		d_rerdcurr
 *		d_retries
 *		d_rlbclr
 *		d_rlbset
 *		d_rlbtst
 *		d_set_dberr
 *		d_setdb
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
 *		d_setrm
 *		d_setro
 *		d_stscm
 *		d_stsco
 *		d_stscr
 *		d_stscs
 *		d_timeout
 *		d_trabort
 *		d_trbegin
 *		d_trend
 *		d_utscm
 *		d_utsco
 *		d_utscr
 *		d_utscs
 *		d_wrcurr
 *		dt_closetask
 *		dt_cmstat
 *		dt_cmtype
 *		dt_connect
 *		dt_costat
 *		dt_cotype
 *		dt_crget
 *		dt_crread
 *		dt_crset
 *		dt_crtype
 *		dt_crwrite
 *		dt_csmget
 *		dt_csmread
 *		dt_csmset
 *		dt_csmwrite
 *		dt_csoget
 *		dt_csoread
 *		dt_csoset
 *		dt_csowrite
 *		dt_csstat
 *		dt_ctbpath
 *		dt_ctscm
 *		dt_ctsco
 *		dt_ctscr
 *		dt_dbdpath
 *		dt_dbfpath
 *		dt_dblog
 *		dt_dbuserid
 *		dt_destroy
 *		dt_discon
 *		dt_fillnew
 *		dt_findco
 *		dt_findfm
 *		dt_findlm
 *		dt_findnm
 *		dt_findpm
 *		dt_gtscm
 *		dt_gtsco
 *		dt_gtscr
 *		dt_gtscs
 *		dt_initfile
 *		dt_ismember
 *		dt_isowner
 *		dt_keydel
 *		dt_keyexist
 *		dt_keyfind
 *		dt_keyfree
 *		dt_keyfrst
 *		dt_keylast
 *		dt_keylock
 *		dt_keylstat
 *		dt_keynext
 *		dt_keyprev
 *		dt_keyread
 *		dt_keystore
 *		dt_lock
 *		dt_makenew
 *		dt_mapchar
 *		dt_members
 *		dt_off_opt
 *		dt_on_opt
 *		dt_open
 *		dt_opentask
 *		dt_rdcurr
 *		dt_recfree
 *		dt_recfrst
 *		dt_reclast
 *		dt_reclock
 *		dt_reclstat
 *		dt_recover
 *		dt_recread
 *		dt_recset
 *		dt_recstat
 *		dt_recwrite
 *		dt_renfile
 *		dt_rerdcurr
 *		dt_retries
 *		dt_set_dberr
 *		dt_setdb
 *		dt_setfree
 *		dt_setkey
 *		dt_setlock
 *		dt_setlstat
 *		dt_setmm
 *		dt_setmo
 *		dt_setmr
 *		dt_setom
 *		dt_setoo
 *		dt_setor
 *		dt_setrm
 *		dt_setro
 *		dt_stscm
 *		dt_stsco
 *		dt_stscr
 *		dt_stscs
 *		dt_timeout
 *		dt_trbegin
 *		dt_utscm
 *		dt_utsco
 *		dt_utscr
 *		dt_utscs
 *		dt_wrcurr
 *
 *   ORIGINS: 157
 *
 */
/*----------------------------------------------------------------------------
   task.h: db_VISTA task cover function declaration header file

   (C) Copyright 1988 by Raima Corporation.
----------------------------------------------------------------------------*/
/* ********************** EDIT HISTORY *******************************

 SCR    DATE    INI                   DESCRIPTION
----- --------- --- -----------------------------------------------------
  571 31-Jan-89 RSC Placed ctbpath inside NO_COUNTRY, not NO_TIMESTAMP
*/


int EXTERNAL_FIXED dt_opentask(P1(DB_TASK *));
int EXTERNAL_FIXED dt_closetask(P1(DB_TASK));

int EXTERNAL_FIXED dt_close(TASK_D1);				/* dblfcns.c */
int EXTERNAL_DBN   dt_cmstat(P1(int) TASK_Di DBN_Dn);		/* cmstat.c */
int EXTERNAL_DBN   dt_cmtype(P1(int) Pi(int *) TASK_Di DBN_Dn); 
								/* cmtype.c */
int EXTERNAL_DBN   dt_connect(P1(int) TASK_Di DBN_Dn);		/* connect.c */
#ifndef NO_TIMESTAMP
int EXTERNAL_DBN   dt_costat(P1(int) TASK_Di DBN_Dn);		/* costat.c */
#endif
int EXTERNAL_DBN   dt_cotype(P1(int) Pi(int *) TASK_Di DBN_Dn); 
								/* cotype.c */
int EXTERNAL_DBN   dt_crget(P1(DB_ADDR *) TASK_Di DBN_Dn);	/* crget.c */
int EXTERNAL_DBN   dt_crread(P1(long) Pi(char *) TASK_Di DBN_Dn);
								/* crread.c */
int EXTERNAL_DBN   dt_crset(P1(DB_ADDR *) TASK_Di DBN_Dn);	/* crset.c */
#ifndef NO_TIMESTAMP
int EXTERNAL_FIXED dt_crstat(TASK_D1);				/* crstat.c */
#endif
int EXTERNAL_DBN   dt_crtype(P1(int *) TASK_Di DBN_Dn);	/* crtype.c */
int EXTERNAL_DBN   dt_crwrite(P1(long) Pi(char *) TASK_Di DBN_Dn); 
								/* crwrite.c */
int EXTERNAL_DBN   dt_csmget(P1(int) Pi(DB_ADDR *) TASK_Di DBN_Dn); 
								/* csmget.c */
int EXTERNAL_DBN   dt_csmread(P1(int) Pi(long) Pi(char *) TASK_Di DBN_Dn); 
								/* csmread.c */
int EXTERNAL_DBN   dt_csmset(P1(int) Pi(DB_ADDR *) TASK_Di DBN_Dn); 
								/* csmset.c */
int EXTERNAL_DBN   dt_csmwrite(P1(int) Pi(long) Pi(const char *) TASK_Di 
				  DBN_Dn); 			/* csmwrite.c */
int EXTERNAL_DBN   dt_csoget(P1(int) Pi(DB_ADDR *) TASK_Di DBN_Dn); 
								/* csoget.c */
int EXTERNAL_DBN   dt_csoread(P1(int) Pi(long) Pi(char *) TASK_Di DBN_Dn); 
								/* csoread.c */
int EXTERNAL_DBN   dt_csoset(P1(int) Pi(DB_ADDR *) TASK_Di DBN_Dn); 
								/* csoset.c */
int EXTERNAL_DBN   dt_csowrite(P1(int) Pi(long) Pi(const char *) TASK_Di 
				  DBN_Dn); 			/* csowrite.c */
#ifndef NO_TIMESTAMP
int EXTERNAL_DBN   dt_csstat(P1(int) TASK_Di DBN_Dn);		/* csstat.c */
int EXTERNAL_DBN   dt_ctscm(P1(int) Pi(ULONG *) TASK_Di DBN_Dn); 
								/* ctscm.c */
int EXTERNAL_DBN   dt_ctsco(P1(int) Pi(ULONG *) TASK_Di DBN_Dn); 
								/* ctsco.c */
int EXTERNAL_FIXED dt_ctscr(P1(ULONG *) TASK_Di );		/* ctscr.c */
#endif
#ifndef NO_COUNTRY
int EXTERNAL_FIXED dt_ctbpath(P1(const char *) TASK_Di);	/* pathfcns.c */
#endif
int EXTERNAL_FIXED dt_dbdpath(P1(const char *) TASK_Di );	/* dbdpath.c */
int EXTERNAL_FIXED dt_dbfpath(P1(const char *) TASK_Di );	/* dbfpath.c */
int EXTERNAL_FIXED dt_dblog(P1(const char *) TASK_Di );	/* dblog.c */
int EXTERNAL_FIXED dt_dbuserid(P1(const char *) TASK_Di );	/* dbuserid.c */
int EXTERNAL_DBN   dt_delete(TASK_D1 DBN_Dn);			/* delete.c */
int EXTERNAL_DBN   dt_disdel(TASK_D1 DBN_Dn);			/* disdel.c */
int EXTERNAL_DBN   dt_recnext(TASK_D1 DBN_Dn);			/* recnext.c */
int EXTERNAL_DBN   dt_recprev(TASK_D1 DBN_Dn);			/* recprev.c */
int EXTERNAL_FIXED dt_destroy(P1(const char *) TASK_Di );	/* destroy.c */
int EXTERNAL_DBN   dt_discon(P1(int) TASK_Di DBN_Dn);		/* discon.c */
int EXTERNAL_DBN   dt_fillnew(P1(int) Pi(const char *) TASK_Di DBN_Dn); 
								/* fillnew.c */
int EXTERNAL_DBN   dt_findco(P1(int) TASK_Di DBN_Dn);		/* findco.c */
int EXTERNAL_DBN   dt_findfm(P1(int) TASK_Di DBN_Dn);		/* findfm.c */
int EXTERNAL_DBN   dt_findlm(P1(int) TASK_Di DBN_Dn);		/* findlm.c */
int EXTERNAL_DBN   dt_findnm(P1(int) TASK_Di DBN_Dn);		/* findnm.c */
int EXTERNAL_DBN   dt_findpm(P1(int) TASK_Di DBN_Dn);		/* findpm.c */
int EXTERNAL_FIXED dt_freeall(TASK_D1);				/* dblfcns.c */
#ifndef NO_TIMESTAMP
int EXTERNAL_DBN   dt_gtscm(P1(int) Pi(ULONG *) TASK_Di DBN_Dn); 
								/* gtscm.c */
int EXTERNAL_DBN   dt_gtsco(P1(int) Pi(ULONG *) TASK_Di DBN_Dn); 
								/* gtsco.c */
int EXTERNAL_FIXED dt_gtscr(P1(ULONG *) TASK_Di );		/* gtscr.c */
int EXTERNAL_DBN   dt_gtscs(P1(int) Pi(ULONG *) TASK_Di DBN_Dn); 
								/* gtscs.c */
#endif
int EXTERNAL_DBN   dt_initialize(TASK_D1 DBN_Dn);		/* initial.c */
int EXTERNAL_DBN   dt_initfile(P1(FILE_NO) TASK_Di DBN_Dn);  	/* initial.c */
int EXTERNAL_DBN   dt_ismember(P1(int) TASK_Di DBN_Dn);		/* ismember.c */
int EXTERNAL_DBN   dt_isowner(P1(int) TASK_Di DBN_Dn);		/* isowner.c */
int EXTERNAL_DBN   dt_keydel(P1(long) TASK_Di DBN_Dn);		/* keydel.c */
int EXTERNAL_DBN   dt_keyexist(P1(long) TASK_Di DBN_Dn);	/* keyexist.c */
int EXTERNAL_DBN   dt_keyfind(P1(long) Pi(const char *) TASK_Di DBN_Dn); 
								/* keyfind.c */
int EXTERNAL_DBN   dt_keyfree(P1(long) TASK_Di DBN_Dn);		/* dblfcns.c */
int EXTERNAL_DBN   dt_keyfrst(P1(long) TASK_Di DBN_Dn);		/* keyfrst.c */
int EXTERNAL_DBN   dt_keylast(P1(long) TASK_Di DBN_Dn);		/* keylast.c */
int EXTERNAL_DBN   dt_keylock(P1(long) Pi(char *) TASK_Di DBN_Dn); 
								/* dblfcns.c */
int EXTERNAL_DBN   dt_keylstat(P1(long) Pi(char *) TASK_Di DBN_Dn); 
								/* dblfcns.c */
int EXTERNAL_DBN   dt_keynext(P1(long) TASK_Di DBN_Dn);		/* keynext.c */
int EXTERNAL_DBN   dt_keyprev(P1(long) TASK_Di DBN_Dn);		/* keyprev.c */
int EXTERNAL_FIXED dt_keyread(P1(char *) TASK_Di );		/* keyfcns.c */
int EXTERNAL_DBN   dt_keystore(P1(long) TASK_Di DBN_Dn);	/* keystore.c */
int EXTERNAL_DBN   dt_lock(P1(int) Pi(LOCK_REQUEST *) TASK_Di DBN_Dn); 
								/* dblfcns.c */
int EXTERNAL_DBN   dt_makenew(P1(int) TASK_Di DBN_Dn);		/* makenew.c */
int EXTERNAL_FIXED dt_mapchar(P1(unsigned char) Pi(unsigned char) 
		      Pi(const char *) Pi(unsigned char) TASK_Di);
								/* mapchar.c */
int EXTERNAL_DBN   dt_members(P1(int) Pi(LONG *) TASK_Di DBN_Dn); 
								/* members.c */
int EXTERNAL_FIXED dt_off_opt(P1(int) TASK_Di );		/* options.c */
int EXTERNAL_FIXED dt_on_opt(P1(int) TASK_Di );			/* options.c */
int EXTERNAL_FIXED dt_open(P1(const char *) Pi(const char *) TASK_Di);
								/* dblfcns.c */
int EXTERNAL_FIXED dt_rdcurr(P1(DB_ADDR **) Pi(int *) TASK_Di); 
								/* rwcurr.c */
int EXTERNAL_FIXED dt_rerdcurr(P1(DB_ADDR **) TASK_Di); 	/* rwcurr.c */
int EXTERNAL_FIXED dt_wrcurr(P1(DB_ADDR *) TASK_Di);	/* rwcurr.c */
int EXTERNAL_DBN   dt_recfree(P1(int) TASK_Di DBN_Dn);		/* dblfcns.c */
int EXTERNAL_DBN   dt_recfrst(P1(int) TASK_Di DBN_Dn);		/* recfrst.c */
int EXTERNAL_DBN   dt_reclast(P1(int) TASK_Di DBN_Dn);		/* reclast.c */
int EXTERNAL_DBN   dt_reclock(P1(int) Pi(char *) TASK_Di DBN_Dn); 
								/* dblfcns.c */
int EXTERNAL_DBN   dt_reclstat(P1(int) Pi(char *) TASK_Di DBN_Dn); 
								/* dblfcns.c */
int EXTERNAL_FIXED dt_recover(P1(const char *) TASK_Di );	/* recover.c */
int EXTERNAL_DBN   dt_recread(P1(char *) TASK_Di DBN_Dn);	/* recread.c */
int EXTERNAL_DBN   dt_recset(P1(int) TASK_Di DBN_Dn);   	/* recset.c */
#ifndef NO_TIMESTAMP
int EXTERNAL_FIXED dt_recstat(P1(DB_ADDR) Pi(ULONG) TASK_Di );	/* recstat.c */
#endif
int EXTERNAL_DBN   dt_recwrite(P1(const char *) TASK_Di DBN_Dn); 
								/* recwrite.c */
int EXTERNAL_FIXED dt_renfile(P1(const char *) Pi(FILE_NO) 
				 Pi(const char *) TASK_Di); /* renfile.c */
int EXTERNAL_FIXED dt_retries(P1(int) TASK_Di );		/* dblfcns.c */
int EXTERNAL_FIXED dt_rlbclr(TASK_D1);				/* dblfcns.c */
int EXTERNAL_FIXED dt_rlbset(TASK_D1);				/* dblfcns.c */
int EXTERNAL_FIXED dt_rlbtst(TASK_D1);				/* dblfcns.c */
int EXTERNAL_FIXED dt_set_dberr(P1(FARPROC) TASK_Di);		/* dberr.c */
#ifndef ONE_DB
int EXTERNAL_FIXED dt_setdb(P1(int) TASK_Di);			/* setdb.c */
#endif
int EXTERNAL_DBN   dt_setfree(P1(int) TASK_Di DBN_Dn);		/* dblfcns.c */
int EXTERNAL_DBN   dt_setkey(P1(long) Pi(const char *) TASK_Di DBN_Dn); 
								/* makenew.c */
int EXTERNAL_DBN   dt_setlock(P1(int) Pi(char *) TASK_Di DBN_Dn); 
								/* dblfcns.c */
int EXTERNAL_DBN   dt_setlstat(P1(int) Pi(char *) TASK_Di DBN_Dn); 
								/* dblfcns.c */
int EXTERNAL_DBN   dt_setmm(P1(int) Pi(int) TASK_Di DBN_Dn); 	/* setmm.c */
int EXTERNAL_DBN   dt_setmo(P1(int) Pi(int) TASK_Di DBN_Dn); 	/* setmo.c */
int EXTERNAL_DBN   dt_setmr(P1(int) TASK_Di DBN_Dn);		/* setmr.c */
int EXTERNAL_DBN   dt_setom(P1(int) Pi(int) TASK_Di DBN_Dn); 	/* setom.c */
int EXTERNAL_DBN   dt_setoo(P1(int) Pi(int) TASK_Di DBN_Dn); 	/* setoo.c */
int EXTERNAL_DBN   dt_setor(P1(int) TASK_Di DBN_Dn);		/* setor.c */
int EXTERNAL_DBN   dt_setrm(P1(int) TASK_Di DBN_Dn);		/* setrm.c */
int EXTERNAL_DBN   dt_setro(P1(int) TASK_Di DBN_Dn);		/* setro.c */
#ifndef NO_TIMESTAMP
int EXTERNAL_DBN   dt_stscm(P1(int) Pi(ULONG) TASK_Di DBN_Dn); 	/* stscm.c */
int EXTERNAL_DBN   dt_stsco(P1(int) Pi(ULONG) TASK_Di DBN_Dn); 	/* stsco.c */
int EXTERNAL_FIXED dt_stscr(P1(ULONG) TASK_Di );		/* stscr.c */
int EXTERNAL_DBN   dt_stscs(P1(int) Pi(ULONG) TASK_Di DBN_Dn); 	/* stscs.c */
#endif
int EXTERNAL_FIXED dt_timeout(P1(int) TASK_Di );		/* dblfcns.c */
int EXTERNAL_FIXED dt_trabort(TASK_D1);				/* dblfcns.c */
int EXTERNAL_FIXED dt_trbegin(P1(const char *) TASK_Di);	/* dblfcns.c */
int EXTERNAL_FIXED dt_trend(TASK_D1);				/* dblfcns.c */
#ifndef NO_TIMESTAMP
int EXTERNAL_DBN   dt_utscm(P1(int) Pi(ULONG *) TASK_Di DBN_Dn); 
								/* utscm.c */
int EXTERNAL_DBN   dt_utsco(P1(int) Pi(ULONG *) TASK_Di DBN_Dn); 
								/* utsco.c */
int EXTERNAL_FIXED dt_utscr(P1(ULONG *) TASK_Di );		/* utscr.c */
int EXTERNAL_DBN   dt_utscs(P1(int) Pi(ULONG *) TASK_Di DBN_Dn); 
								/* utscs.c */
#endif

#ifndef NO_DT_COVER

#define d_close()                    dt_close(CURRTASK_ONLY)
#define d_dbdpath(a)                 dt_dbdpath(a CURRTASK_PARM)
#define d_dbfpath(a)                 dt_dbfpath(a CURRTASK_PARM)
#define d_dblog(a)                   dt_dblog(a CURRTASK_PARM)
#define d_dbuserid(a)                dt_dbuserid(a CURRTASK_PARM)
#define d_destroy(a)                 dt_destroy(a CURRTASK_PARM)
#define d_freeall()                  dt_freeall(CURRTASK_ONLY)
#define d_off_opt(a)                 dt_off_opt(a CURRTASK_PARM)
#define d_on_opt(a)                  dt_on_opt(a CURRTASK_PARM)
#define d_open(a, b)                 dt_open(a, b CURRTASK_PARM)
#define d_rdcurr(a, b)               dt_rdcurr(a, b CURRTASK_PARM)
#define d_rerdcurr(a, b)             dt_rerdcurr(a, b CURRTASK_PARM)
#define d_recover(a)                 dt_recover(a CURRTASK_PARM)
#define d_renfile(a, b, c)           dt_renfile(a, b, c CURRTASK_PARM)
#define d_retries(a)                 dt_retries(a CURRTASK_PARM)
#define d_set_dberr(a, b)            dt_set_dberr(a, b, CURRTASK_PARM)
#ifndef ONE_DB
#define d_setdb(a)                   dt_setdb(a CURRTASK_PARM)
#endif
#define d_timeout(a)                 dt_timeout(a CURRTASK_PARM)
#define d_trabort()                  dt_trabort(CURRTASK_ONLY)
#define d_trbegin(a)                 dt_trbegin(a CURRTASK_PARM)
#define d_trend()                    dt_trend(CURRTASK_ONLY)
#define d_wrcurr(a)                  dt_wrcurr(a CURRTASK_PARM)
#ifndef NO_TIMESTAMP
#define d_crstat()                   dt_crstat(CURRTASK_ONLY)
#define d_gtscr(a)                   dt_gtscr(a CURRTASK_PARM)
#define d_recstat(a, b)              dt_recstat(a, b CURRTASK_PARM)
#define d_rlbclr()                   dt_rlbclr(CURRTASK_ONLY)
#define d_rlbset()                   dt_rlbset(CURRTASK_ONLY)
#define d_rlbtst()                   dt_rlbtst(CURRTASK_ONLY)
#define d_utscr(a)                   dt_utscr(a CURRTASK_PARM)
#endif

#define d_cmstat(a DBN_PARM)         dt_cmstat(a CURRTASK_PARM DBN_PARM)
#define d_cmtype(a, b DBN_PARM)      dt_cmtype(a, b CURRTASK_PARM DBN_PARM)
#define d_connect(a DBN_PARM)        dt_connect(a CURRTASK_PARM DBN_PARM)
#ifndef NO_TIMESTAMP
#define d_costat(a DBN_PARM)         dt_costat(a CURRTASK_PARM DBN_PARM)
#endif
#define d_cotype(a, b DBN_PARM)      dt_cotype(a, b CURRTASK_PARM DBN_PARM)
#define d_crget(a DBN_PARM)          dt_crget(a CURRTASK_PARM DBN_PARM)
#define d_crread(a, b DBN_PARM)      dt_crread(a, b CURRTASK_PARM DBN_PARM)
#define d_crset(a DBN_PARM)          dt_crset(a CURRTASK_PARM DBN_PARM)
#define d_crtype(a DBN_PARM)         dt_crtype(a CURRTASK_PARM DBN_PARM)
#define d_crwrite(a, b DBN_PARM)     dt_crwrite(a, b CURRTASK_PARM DBN_PARM)
#define d_csmget(a, b DBN_PARM)      dt_csmget(a, b CURRTASK_PARM DBN_PARM)
#define d_csmread(a, b, c DBN_PARM)  dt_csmread(a, b, c CURRTASK_PARM DBN_PARM)
#define d_csmset(a, b DBN_PARM)      dt_csmset(a, b CURRTASK_PARM DBN_PARM)
#define d_csmwrite(a, b, c DBN_PARM) dt_csmwrite(a, b, c CURRTASK_PARM DBN_PARM)
#define d_csoget(a, b DBN_PARM)      dt_csoget(a, b CURRTASK_PARM DBN_PARM)
#define d_csoread(a, b, c DBN_PARM)  dt_csoread(a, b, c CURRTASK_PARM DBN_PARM)
#define d_csoset(a, b DBN_PARM)      dt_csoset(a, b CURRTASK_PARM DBN_PARM)
#define d_csowrite(a, b, c DBN_PARM) dt_csowrite(a, b, c CURRTASK_PARM DBN_PARM)
#ifndef NO_TIMESTAMP
#define d_csstat(a DBN_PARM)         dt_csstat(a CURRTASK_PARM DBN_PARM)
#define d_ctscm(a, b DBN_PARM)       dt_ctscm(a, b CURRTASK_PARM DBN_PARM)
#define d_ctsco(a, b DBN_PARM)       dt_ctsco(a, b CURRTASK_PARM DBN_PARM)
#define d_ctscr(a DBN_PARM)          dt_ctscr(a CURRTASK_PARM)
#endif
#define d_delete(DBN_PARM)           dt_delete(CURRTASK_ONLY DBN_PARM)
#define d_disdel(DBN_PARM)           dt_disdel(CURRTASK_ONLY DBN_PARM)
#define d_recnext(DBN_PARM)          dt_recnext(CURRTASK_ONLY DBN_PARM)
#define d_recprev(DBN_PARM)          dt_recprev(CURRTASK_ONLY DBN_PARM)
#define d_discon(a DBN_PARM)         dt_discon(a CURRTASK_PARM DBN_PARM)
#define d_fillnew(a, b DBN_PARM)     dt_fillnew(a, b CURRTASK_PARM DBN_PARM)
#define d_findco(a DBN_PARM)         dt_findco(a CURRTASK_PARM DBN_PARM)
#define d_findfm(a DBN_PARM)         dt_findfm(a CURRTASK_PARM DBN_PARM)
#define d_findlm(a DBN_PARM)         dt_findlm(a CURRTASK_PARM DBN_PARM)
#define d_findnm(a DBN_PARM)         dt_findnm(a CURRTASK_PARM DBN_PARM)
#define d_findpm(a DBN_PARM)         dt_findpm(a CURRTASK_PARM DBN_PARM)
#ifndef NO_TIMESTAMP
#define d_gtscm(a, b DBN_PARM)       dt_gtscm(a, b CURRTASK_PARM DBN_PARM)
#define d_gtsco(a, b DBN_PARM)       dt_gtsco(a, b CURRTASK_PARM DBN_PARM)
#define d_gtscs(a, b DBN_PARM)       dt_gtscs(a, b CURRTASK_PARM DBN_PARM)
#endif
#define d_initialize(DBN_PARM)       dt_initialize(CURRTASK_ONLY DBN_PARM)
#define d_initfile(a DBN_PARM)       dt_initfile(a CURRTASK_PARM DBN_PARM)
#define d_ismember(a DBN_PARM)       dt_ismember(a CURRTASK_PARM DBN_PARM)
#define d_isowner(a DBN_PARM)        dt_isowner(a CURRTASK_PARM DBN_PARM)
#define d_keydel(a DBN_PARM)         dt_keydel(a CURRTASK_PARM DBN_PARM)
#define d_keyexist(a DBN_PARM)       dt_keyexist(a CURRTASK_PARM DBN_PARM)
#define d_keyfind(a, b DBN_PARM)     dt_keyfind(a, b CURRTASK_PARM DBN_PARM)
#define d_keyfree(a DBN_PARM)        dt_keyfree(a CURRTASK_PARM DBN_PARM)
#define d_keyfrst(a DBN_PARM)        dt_keyfrst(a CURRTASK_PARM DBN_PARM)
#define d_keylast(a DBN_PARM)        dt_keylast(a CURRTASK_PARM DBN_PARM)
#define d_keylock(a, b DBN_PARM)     dt_keylock(a, b CURRTASK_PARM DBN_PARM)
#define d_keylstat(a, b DBN_PARM)    dt_keylstat(a, b CURRTASK_PARM DBN_PARM)
#define d_keynext(a DBN_PARM)        dt_keynext(a CURRTASK_PARM DBN_PARM)
#define d_keyprev(a DBN_PARM)        dt_keyprev(a CURRTASK_PARM DBN_PARM)
#define d_keyread(a DBN_PARM)        dt_keyread(a CURRTASK_PARM)
#define d_keystore(a DBN_PARM)       dt_keystore(a CURRTASK_PARM DBN_PARM)
#define d_lock(a, b DBN_PARM)        dt_lock(a, b CURRTASK_PARM DBN_PARM)
#define d_makenew(a DBN_PARM)        dt_makenew(a CURRTASK_PARM DBN_PARM)
#define d_members(a, b DBN_PARM)     dt_members(a, b CURRTASK_PARM DBN_PARM)
#define d_recfree(a DBN_PARM)        dt_recfree(a CURRTASK_PARM DBN_PARM)
#define d_recfrst(a DBN_PARM)        dt_recfrst(a CURRTASK_PARM DBN_PARM)
#define d_reclast(a DBN_PARM)        dt_reclast(a CURRTASK_PARM DBN_PARM)
#define d_reclock(a, b DBN_PARM)     dt_reclock(a, b CURRTASK_PARM DBN_PARM)
#define d_reclstat(a, b DBN_PARM)    dt_reclstat(a, b CURRTASK_PARM DBN_PARM)
#define d_recread(a DBN_PARM)        dt_recread(a CURRTASK_PARM DBN_PARM)
#define d_recset(a DBN_PARM)         dt_recset(a CURRTASK_PARM DBN_PARM)
#define d_recwrite(a DBN_PARM)       dt_recwrite(a CURRTASK_PARM DBN_PARM)
#define d_setfree(a DBN_PARM)        dt_setfree(a CURRTASK_PARM DBN_PARM)
#define d_setkey(a, b DBN_PARM)      dt_setkey(a, b CURRTASK_PARM DBN_PARM)
#define d_setlock(a, b DBN_PARM)     dt_setlock(a, b CURRTASK_PARM DBN_PARM)
#define d_setlstat(a, b DBN_PARM)    dt_setlstat(a, b CURRTASK_PARM DBN_PARM)
#define d_setmm(a, b DBN_PARM)       dt_setmm(a, b CURRTASK_PARM DBN_PARM)
#define d_setmo(a, b DBN_PARM)       dt_setmo(a, b CURRTASK_PARM DBN_PARM)
#define d_setmr(a DBN_PARM)          dt_setmr(a CURRTASK_PARM DBN_PARM)
#define d_setom(a, b DBN_PARM)       dt_setom(a, b CURRTASK_PARM DBN_PARM)
#define d_setoo(a, b DBN_PARM)       dt_setoo(a, b CURRTASK_PARM DBN_PARM)
#define d_setor(a DBN_PARM)          dt_setor(a CURRTASK_PARM DBN_PARM)
#define d_setrm(a DBN_PARM)          dt_setrm(a CURRTASK_PARM DBN_PARM)
#define d_setro(a DBN_PARM)          dt_setro(a CURRTASK_PARM DBN_PARM)
#ifndef NO_TIMESTAMP
#define d_stscm(a, b DBN_PARM)       dt_stscm(a, b CURRTASK_PARM DBN_PARM)
#define d_stsco(a, b DBN_PARM)       dt_stsco(a, b CURRTASK_PARM DBN_PARM)
#define d_stscr(a DBN_PARM)          dt_stscr(a CURRTASK_PARM DBN_PARM)
#define d_stscs(a, b DBN_PARM)       dt_stscs(a, b CURRTASK_PARM DBN_PARM)
#endif
#ifndef NO_TIMESTAMP
#define d_utscm(a, b DBN_PARM)       dt_utscm(a, b CURRTASK_PARM DBN_PARM)
#define d_utsco(a, b DBN_PARM)       dt_utsco(a, b CURRTASK_PARM DBN_PARM)
#define d_utscs(a, b DBN_PARM)       dt_utscs(a, b CURRTASK_PARM DBN_PARM)
#endif

#endif /* NO_DT_COVER */
/* vpp -nOS2 -dUNIX -nBSD -nVANILLA_BSD -nVMS -nMEMLOCK -nWINDOWS -nFAR_ALLOC task.h */
