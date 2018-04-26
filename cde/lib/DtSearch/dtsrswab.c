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
 *   FUNCTIONS: swab_dbrec
 *              swab_objrec
 *
 *   ORIGINS: 27
 *
 *   (C) COPYRIGHT International Business Machines Corp. 1996
 *   All Rights Reserved
 *   Licensed Materials - Property of IBM
 *   US Government Users Restricted Rights - Use, duplication or
 *   disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 */
/********************* DTSRSWAB.C **********************************
 * $XConsortium: dtsrswab.c /main/2 1996/10/28 13:58:11 drk $
 * July 1996.
 * These functions byte swap and other manipulations on
 * DtSearch or_...  structures to ensure that database records
 * are maintained in a canonical format no matter what machine
 * they are used on.
 * The functions use the standard htonl, htons, ntohl, and ntohs
 * network conversion functions for byte order neutrality.
 * They use the DtSrINT16 and DtSrINT32 typedefs in Search.h
 * to control integer size.
 * The external database records are in the canonical,
 * big-endian "network" order.
 * So that they can be defined as "null" macros on big endian machines,
 * the functions (1) always byte swap in place and  (2) always succeed.
 *
 * $Log$
 */
#include "SearchP.h"

/********************************/
/*				*/
/*	    swab_objrec		*/
/*				*/
/********************************/
void	swab_objrec (struct or_objrec *rec,  SWABDIR direction)
{
#ifndef BYTE_SWAP
    return;
#else
    if (direction == NTOH) {
	NTOHL (rec->or_objflags);
	NTOHL (rec->or_objuflags);
	NTOHL (rec->or_objsize);
	NTOHL (rec->or_objdate);
	NTOHL (rec->or_objsecmask);

	NTOHS (rec->or_objaccess);
	NTOHS (rec->or_objtype);
	NTOHS (rec->or_objcost);
	NTOHS (rec->or_objhdroffset);
	NTOHS (rec->or_objeureka);
    }
    else {	/* going the other direction */
	HTONL (rec->or_objflags);
	HTONL (rec->or_objuflags);
	HTONL (rec->or_objsize);
	HTONL (rec->or_objdate);
	HTONL (rec->or_objsecmask);

	HTONS (rec->or_objaccess);
	HTONS (rec->or_objtype);
	HTONS (rec->or_objcost);
	HTONS (rec->or_objhdroffset);
	HTONS (rec->or_objeureka);
    }
    return;
#endif /* BYTE_SWAP */
}  /* swab_objrec() */


/********************************/
/*				*/
/*	    swab_dbrec		*/
/*				*/
/********************************/
void	swab_dbrec (struct or_dbrec *rec,  SWABDIR direction)
{
#ifndef BYTE_SWAP
    return;
#else
    if (direction == NTOH) {
	NTOHL (rec->or_dbflags);
	NTOHL (rec->or_dbuflags);
	NTOHL (rec->or_reccount);
	NTOHL (rec->or_maxdba);
	NTOHL (rec->or_availd99);
	NTOHL (rec->or_unavaild99);
	NTOHL (rec->or_hufid);
	NTOHL (rec->or_dbsecmask);

	NTOHS (rec->or_dbotype);
	NTOHS (rec->or_compflags);
	NTOHS (rec->or_dbaccess);
	NTOHS (rec->or_minwordsz);
	NTOHS (rec->or_maxwordsz);
	NTOHS (rec->or_recslots);
	NTOHS (rec->or_fzkeysz);
	NTOHS (rec->or_abstrsz);
	NTOHS (rec->or_language);
    }

    else {	/* going the other direction */
	HTONL (rec->or_dbflags);
	HTONL (rec->or_dbuflags);
	HTONL (rec->or_reccount);
	HTONL (rec->or_maxdba);
	HTONL (rec->or_availd99);
	HTONL (rec->or_unavaild99);
	HTONL (rec->or_hufid);
	HTONL (rec->or_dbsecmask);

	HTONS (rec->or_dbotype);
	HTONS (rec->or_compflags);
	HTONS (rec->or_dbaccess);
	HTONS (rec->or_minwordsz);
	HTONS (rec->or_maxwordsz);
	HTONS (rec->or_recslots);
	HTONS (rec->or_fzkeysz);
	HTONS (rec->or_abstrsz);
	HTONS (rec->or_language);
    }
    return;
#endif /* BYTE_SWAP */
}  /* swab_dbrec() */

/********************* DTSRSWAB.C **********************************/
