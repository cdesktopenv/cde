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
 *   FUNCTIONS: HTONL
 *              HTONS
 *              NTOHL
 *              NTOHS
 *              SWABDIR
 *              htonl
 *              htons
 *              ntohl
 *              ntohs
 *
 *   ORIGINS: 27
 *
 *
 *   (C) COPYRIGHT International Business Machines Corp. 1996
 *   All Rights Reserved
 *   Licensed Materials - Property of IBM
 *   US Government Users Restricted Rights - Use, duplication or
 *   disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 */
/********************* DBSWAB.H **********************************
 * $XConsortium: dbswab.h /main/2 1996/10/28 14:00:06 drk $
 * July 1996.
 * Header file for all internal vista byte swap modifications.
 * The mods byte swap vista cache pages, dbd tables, etc,
 * to ensure that database files are maintained in a canonical
 * format no matter what machine they are used on.
 * The DtSearch data records are byte swapped in code at the
 * DtSearch level (dtsrswab.c, defns in SearchP.h).
 * Byte swap is performed with the standard htonl, htons, ntohl,
 * and ntohs network conversion functions for byte order neutrality.
 * Canonical integer length is enforced with the typedefs
 * LONG and INT in vista.h
 * The external database files are in big-endian ("network") order.
 * So that they can be defined as "null" macros on big endian machines,
 * new functions (1) always byte swap in place, and (2) always succeed.
 *
 * An #include "vista.h" and #include "dbtype.h" must precede this include.
 *
 * $Log$
 */
#ifndef	_H_DBSWAB
#define	_H_DBSWAB

#include <sys/types.h>
#include <netinet/in.h>

/* Record number for OR_MISCREC DtSearch record */
#define MISCREC_RECNO	3

/* Direction of io to indicate correct byte swap function.
 * HTON is host to network, internal RAM to external database file.
 * NTOH is network to host, file to RAM.
 */
typedef enum {HTON=1, NTOH} SWABDIR;

extern void	swab_page (char *pgbuf, FILE_ENTRY *file_ptr, SWABDIR direction);

#ifdef BYTE_SWAP  /* ie (BYTE_ORDER != BIG_ENDIAN) */

#define HTONL(x)	x = htonl(x)
#define HTONS(x)	x = htons(x)
#define NTOHL(x)	x = ntohl(x)
#define NTOHS(x)	x = ntohs(x)

#else	/* !BYTE_SWAP, ie (BYTE_ORDER == BIG_ENDIAN) */

#define HTONL(x)
#define HTONS(x)
#define NTOHL(x)
#define NTOHS(x)

#endif	/* BYTE_SWAP */

/******** debug stuff *******/
extern char	*debug_keyslot_ptr;
extern void	snap_dump (char *label, void *ptr, int len);

/********************* DBSWAB.H **********************************/
#endif	/* _H_DBSWAB */
