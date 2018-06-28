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
/* $XConsortium: trxlog.h /main/2 1996/05/09 04:20:43 drk $ */
/*
 *   COMPONENT_NAME: austext
 *
 *   FUNCTIONS: BITMAP_SIZE
 *		BM_BASE
 *		IX_BASE
 *		IX_SIZE
 *		PZ_BASE
 *
 *   ORIGINS: 157
 *
 */

/* ********************** EDIT HISTORY *******************************

 SCR    DATE    INI                   DESCRIPTION
----- --------- --- -----------------------------------------------------
  611 21-Feb-89 RSC The defn of BUI for unix and vms needs parenthesis
*/


/* trxlog.h  -  header file to define structures, constants, etc. for the
                memory cache overflow and transaction log file control
==========================================================================
*/
/*
   The following constants control the functioning of the cache overflow
   and transaction logging processes

   BUI             The number of bits in an unsigned int
   IX_PAGESIZE     The size (in bytes) of an index page
   IX_EPP          The number of entries that will fit on an index page
   BITMAP_SIZE     The size of the index bitmap (in unsigned int units)
   IX_SIZE         The number of index pages needed to control the db pages
   OADDR_OF_IXP    Calculates the overflow file address of an index page #
   
==========================================================================
*/
/* (BITS(unsigned int)) */
#define BUI (8*sizeof(unsigned int))


/*
==========================================================================
*/

/* page zero table entry */
#define PGZEROSZ (2*sizeof(F_ADDR)+sizeof(ULONG))
typedef struct PGZERO_S {
   F_ADDR  pz_dchain;         /* delete chain pointer */
   F_ADDR  pz_next;           /* next available record number */
   ULONG   pz_timestamp;      /* file's timestamp value */
   BOOLEAN pz_modified;       /* TRUE if page zero has been modified */
} PGZERO;

/* binary search lookup table entry */


/* Maximum number of transactions which can commit a time */
#define TAFLIMIT 1

#define TRXLOG_H
/* End - trxlog.h */
/* vpp -nOS2 -dUNIX -nBSD -nVANILLA_BSD -nVMS -nMEMLOCK -nWINDOWS -nFAR_ALLOC -f/usr/users/master/config/nonwin trxlog.h */
