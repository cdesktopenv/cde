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
/*%%  $XConsortium: tt_gettext_c.h /main/3 1995/10/23 10:39:13 rswiston $ 			 				 */
/*
 *
 * tt_gettext_c.h
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 *
 * Copied from SunOS 4.1.1 gettext.h
 */
#if !defined(_TT_GETTEXT_C_H)
#define _TT_GETTEXT_C_H

/* common between installtxt and gettext() */
/* Since we don't have our own copy of installtxt, we just blindly hope
 * that the format doesn't change.
 */

#define	ARMAG		"!<LC_MESSAGES>\n"
#define ARFMAG  	"`\n"
#define	SARMAG		15
#define INT             0 
#define STR             1

struct ar_hdr {
        unsigned char ar_name[16];
	unsigned char ar_date[12];
        unsigned char ar_uid[6];
        unsigned char ar_gid[6];
        unsigned char ar_mode[8];
        unsigned char ar_size[10];
	unsigned char ar_fill;
	unsigned char ar_sep;
	unsigned char ar_quote;
        unsigned char ar_fmag[2];
};

struct msg_header {
	int maxno;	/* number of message tags in following 
		 	 * domain structure */

	int ptr;	/* relative offset from msg_header to start 
			 * of target strings block */

	short format_type;
	char format[MAXFMTS];	/* Format string */

};

struct index {
	int msgnumber;	/* The value of mesasge tag expressed as an integer
*/
	int rel_addr;	/* The relative offset to the real target string
*/
};
#endif
