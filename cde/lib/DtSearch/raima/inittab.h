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
 *   FUNCTIONS: sizeof
 *
 *   ORIGINS: 27,157
 *
 *   (C) COPYRIGHT International Business Machines Corp. 1995, 1996
 *   All Rights Reserved
 *   US Government Users Restricted Rights - Use, duplication or
 *   disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 */
/*----------------------------------------------------------------------------
 $XConsortium: inittab.h /main/5 1996/11/25 18:48:42 drk $
   inittab.h: db_VISTA header file containing:
   
      - Compatible dictionary file versions

   This file is intended to be included in inittab.c only.  There are
   multiple versions of inittab.c which share this header.

   (An #include "vista.h" must precede this include)

   (C) Copyright 1987 by Raima Corporation.
----------------------------------------------------------------------------*/

/* ********************** EDIT HISTORY *******************************

 SCR    DATE    INI                   DESCRIPTION
----- --------- --- -----------------------------------------------------
  101 21-Jun-88 RSC Added decl for rn_type and rn_dba for ONE_DB
      08-Aug-88 RTK Moved rn_dba and rn_type to vista.h - needed in rec*.c
      18-Aug-88 RSC Moved rn_type/dba to separate table, add curr_rn_table..

 $Log$
 * Revision 1.2  1995/10/13  18:36:46  miker
 * Changed compat_dbd[] from hardcoded str to dbd_VERSION.
 * (dbd_VERSION in vista.h changed bcs expanded max file name length.)
 *
*/


int db_status = 0;

/* database dictionary tables */
INT largest_page = 0;

char *crloc = NULL;

INT old_size_ft = 0;
INT old_size_fd = 0;
INT old_size_st = 0;
INT old_size_mt = 0;
INT old_size_srt = 0;
INT old_size_kt = 0;
INT old_size_rt = 0;
INT old_no_of_dbs = 0;

/* Compatible dictionary versions */
char *compat_dbd[] = {
   dbd_VERSION
};
int size_compat = sizeof(compat_dbd)/sizeof(*compat_dbd);
/* vpp -nOS2 -dUNIX -nBSD -nVANILLA_BSD -nVMS -nMEMLOCK -nWINDOWS -nFAR_ALLOC -f/usr/users/master/config/nonwin inittab.h */
