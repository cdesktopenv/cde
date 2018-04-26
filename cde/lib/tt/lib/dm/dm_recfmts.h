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
/*%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			 */
/*%%  (c) Copyright 1993, 1994 International Business Machines Corp.	 */
/*%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			 */
/*%%  (c) Copyright 1993, 1994 Novell, Inc. 				 */
/*%%  $XConsortium: dm_recfmts.h /main/3 1995/10/23 10:15:36 rswiston $ 			 				 */
/* -*-C++-*-
 *
 * dm_recfmts.h
 *
 * Structs which map each record type in the data base. 
 * Very few programs need to know this information, since record
 * access should be done through the record class member functions.
 * However, for documentation, debugging, and the inspect and
 * repair tool, it's useful to have these structs defined.
 * 
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 */

#ifndef _DM_RECFMTS_H
#define _DM_RECFMTS_H

#include "tt_const.h"
#include "isam.h"     
#include "tt_options.h"
#if defined(OPT_BUG_SUNOS_4) && defined(__GNUG__)
#	define offsetof(ty,mem) ((size_t)&(((ty*)0)->mem))
#else
#	include <stddef.h>
#endif

// Ints are stored in records in "Network" (XDR) byte order, as 4-byte
// quantities.
#define XDR_LONG_SIZE 4
#define XDR_SHORT_SIZE 2
#define MAX_KEY_LEN 120

struct Table_oid_prop {
	unsigned char	objkey[OID_KEY_LENGTH];
	char	propname[MAX_PROP_LENGTH];
	char	propval[ISMAXRECLEN-OID_KEY_LENGTH-MAX_PROP_LENGTH]; //variable
};

struct Table_oid_container {
	unsigned char 	objkey[OID_KEY_LENGTH];
	unsigned char	dockey[OID_KEY_LENGTH];
};

struct Table_oid_access {
	unsigned char 	objkey[OID_KEY_LENGTH];
	char	owner[XDR_LONG_SIZE]; // uid_t
	char	group[XDR_LONG_SIZE]; // gid_t
	char	mode[XDR_LONG_SIZE]; // mode_t
};

struct Table_docoid_path {
	unsigned char	dockey[OID_KEY_LENGTH];
	char	filepath[PATH_LENGTH]; // variable: minimum MAX_KEY_LEN
};

struct Table_msg_queue {
	unsigned char	dockey[OID_KEY_LENGTH];
	char	id[XDR_LONG_SIZE];
	char	part[XDR_LONG_SIZE];
	// Really ought to reserve some space for new attributes...
	char	body[ISMAXRECLEN-OID_KEY_LENGTH-2*XDR_LONG_SIZE];
};


struct Table_link_access {
	char	linkkey[OID_KEY_LENGTH];
	char	owner[XDR_SHORT_SIZE]; // uid_t
	char	group[XDR_SHORT_SIZE]; // gid_t
	char	mode[XDR_SHORT_SIZE]; // mode_t
};

struct Table_link_ends {
	// it's important to keep linkkey and direction together so the
	// DM_KEY_END index works for both linkkey and linkkey+direction
	// queries.
	unsigned char	linkkey[OID_KEY_LENGTH];
	char	direction;
	unsigned char	endkey[OID_KEY_LENGTH];
	// the two locations follow one after another here, each terminated
	// by a null character.
	// <dbref corresponding to linkkey>'\0'<dbref corr. to. endkey>'\0'
	// This makes updating a pain, but saves space in the db. (If
	// we were really serious about saving space in the db, we ought
	// to encode the second location relative to the first: e.g.
	// the second location starts with a number, the number of
	// characters in common with the first location, followed by
	// the different characters.  In the common case of the locations
	// being the same, this would collapse the second location to
	// three bytes (a short for the length and the null).  In the
	// rare case of no commonality it costs two bytes.  But we
	// don't yet implement this scheme... RFM 4/15/91)
	char	dbrefs[ISMAXRECLEN-2*OID_KEY_LENGTH-1];	// 2*variable
};

struct Table_link_prop {
	unsigned char	linkkey[OID_KEY_LENGTH];
	char	propname[MAX_PROP_LENGTH];
	char	propval[ISMAXRECLEN-OID_KEY_LENGTH-MAX_PROP_LENGTH]; //variable
};

#endif
