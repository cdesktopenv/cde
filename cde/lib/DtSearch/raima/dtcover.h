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
/* $XConsortium: dtcover.h /main/2 1996/05/09 04:05:27 drk $ */
/*
 *   COMPONENT_NAME: austext
 *
 *   FUNCTIONS: none
 *
 *   ORIGINS: 157
 *
 */
/*----------------------------------------------------------------------------
   dtcover.h: converts d_* calls to dt_* for MULTI_TASKing

   (C) Copyright 1988 by Raima Corporation.
----------------------------------------------------------------------------*/
/* ********************** EDIT HISTORY *******************************

 SCR    DATE    INI                   DESCRIPTION
----- --------- --- -----------------------------------------------------
  571 31-Jan-89 RSC Placed ctbpath inside NO_COUNTRY, not NO_TIMESTAMP
*/


#define d_close			dt_close
#define d_cmstat		dt_cmstat
#define d_cmtype		dt_cmtype
#define d_connect		dt_connect
#define d_cotype		dt_cotype
#define d_crget			dt_crget
#define d_crread		dt_crread
#define d_crset			dt_crset
#define d_crtype		dt_crtype
#define d_crwrite		dt_crwrite
#define d_csmget		dt_csmget
#define d_csmread		dt_csmread
#define d_csmset		dt_csmset
#define d_csmwrite		dt_csmwrite
#define d_csoget		dt_csoget
#define d_csoread		dt_csoread
#define d_csoset		dt_csoset
#define d_csowrite		dt_csowrite
#define d_ctbpath		dt_ctbpath
#define d_dbdpath		dt_dbdpath
#define d_dbfpath		dt_dbfpath
#define d_dblog			dt_dblog
#define d_dbtaf			dt_dbtaf
#define d_dbuserid		dt_dbuserid
#define d_delete		dt_delete
#define d_disdel		dt_disdel
#define d_recnext		dt_recnext
#define d_recprev		dt_recprev
#define d_destroy		dt_destroy
#define d_discon		dt_discon
#define d_fillnew		dt_fillnew
#define d_findco		dt_findco
#define d_findfm		dt_findfm
#define d_findlm		dt_findlm
#define d_findnm		dt_findnm
#define d_findpm		dt_findpm
#define d_freeall		dt_freeall
#define d_initialize		dt_initialize
#define d_initfile		dt_initfile
#define d_ismember		dt_ismember
#define d_isowner		dt_isowner
#define d_keydel		dt_keydel
#define d_keyexist		dt_keyexist
#define d_keyfind		dt_keyfind
#define d_keyfree		dt_keyfree
#define d_keyfrst		dt_keyfrst
#define d_keylast		dt_keylast
#define d_keylock		dt_keylock
#define d_keylstat		dt_keylstat
#define d_keynext		dt_keynext
#define d_keyprev		dt_keyprev
#define d_keyread		dt_keyread
#define d_keystore		dt_keystore
#define d_lock			dt_lock
#define d_makenew		dt_makenew
#define d_mapchar		dt_mapchar
#define d_members		dt_members
#define d_off_opt		dt_off_opt
#define d_on_opt		dt_on_opt
#define d_open			dt_open
#define d_rdcurr		dt_rdcurr
#define d_rerdcurr		dt_rerdcurr
#define d_recfree		dt_recfree
#define d_recfrst		dt_recfrst
#define d_reclast		dt_reclast
#define d_reclock		dt_reclock
#define d_reclstat		dt_reclstat
#define d_recover		dt_recover
#define d_recread		dt_recread
#define d_recset		dt_recset
#define d_recwrite		dt_recwrite
#define d_renfile		dt_renfile
#define d_retries		dt_retries
#define d_rlbclr		dt_rlbclr
#define d_rlbset		dt_rlbset
#define d_rlbtst		dt_rlbtst
#define d_set_dberr		dt_set_dberr
#define d_setdb			dt_setdb
#define d_setfree		dt_setfree
#define d_setkey		dt_setkey
#define d_setlock		dt_setlock
#define d_setlstat		dt_setlstat
#define d_setmm			dt_setmm
#define d_setmo			dt_setmo
#define d_setmr			dt_setmr
#define d_setom			dt_setom
#define d_setoo			dt_setoo
#define d_setor			dt_setor
#define d_setrm			dt_setrm
#define d_setro			dt_setro
#define d_timeout		dt_timeout
#define d_trabort		dt_trabort
#define d_trbegin		dt_trbegin
#define d_trend			dt_trend
#define d_wrcurr		dt_wrcurr
/* vpp -nOS2 -dUNIX -nBSD -nVANILLA_BSD -nVMS -nMEMLOCK -nWINDOWS -nFAR_ALLOC dtcover.h */
