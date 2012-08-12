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
/* $XConsortium: dbacode.c /main/2 1996/05/09 04:02:05 drk $ */
/*
 *   COMPONENT_NAME: austext
 *
 *   FUNCTIONS: d_decode_dba
 *		d_encode_dba
 *
 *   ORIGINS: 157
 *
 *   OBJECT CODE ONLY SOURCE MATERIALS
 */
/*-----------------------------------------------------------------------
   dbacode.c -- Module for encoding and decoding database addresses

   (C) Copyright 1987 by Raima Corporation.
-----------------------------------------------------------------------*/
#include <stdio.h>
#include "vista.h"
#include "dbtype.h"


/* Decode file and slot number from a database address
*/
int
d_decode_dba(dba, file, slot)
DB_ADDR dba;  /* database address */
int FAR *file;    /* returned file number */
long FAR *slot;   /* returned slot number */
{
   *file = (int)(FILEMASK & (dba >> FILESHIFT));
   *slot = ADDRMASK & dba;
   return ( db_status = S_OKAY );
}



/* Encode database address from file and slot number
*/
int
d_encode_dba(file, slot, dba)
int file;      /* returned file number */
long slot;     /* returned slot number */
DB_ADDR FAR *dba;  /* database address */
{
   *dba = ((FILEMASK & file) << FILESHIFT) | slot;
   return ( db_status = S_OKAY );
}
/* vpp -nOS2 -dUNIX -nBSD -nVANILLA_BSD -nVMS -nMEMLOCK -nWINDOWS -nFAR_ALLOC -f/usr/users/master/config/nonwin dbacode.c */
