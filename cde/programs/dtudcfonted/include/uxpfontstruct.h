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
/* $XConsortium: uxpfontstruct.h /main/1 1995/09/14 20:52:27 cde-fuj2 $ */
/*
 *  All Rights Reserved, Copyright (c) FUJITSU LIMITED 1995
 *
 *  This is unpublished proprietary source code of FUJITSU LIMITED
 *
 *  Authors: Seiya Miyazaki     FUJITSU LIMITED
 *           Hiroyuki Chiba     FUJITSU LIMITED
 *
 */

#ifndef FEFONTSTRUCT_H
#define FEFONTSTRUCT_H 1

/* 
 * for use in version
 */

#define FE_PREFIXS	"FEF/"
#define FE_PREFIXL	4


/*
 * for use in permission.
 */

#define FE_DISPLAY_FONT	0x01
#define FE_PRINTER_FONT	0x02

/*
 * uxp Extended Font Structure
 */

typedef struct _FeFontInfo {
    char		version[32];	/* version */
    char		copyright[64];	/* Copyright */
    unsigned int	permission;	/* purpose for use */ 
    char		reserve[24];
    int			privatesize;	/* size of private area */
} FeFontInfoRec;

typedef struct _FeFontInfo *FeFontInfoPtr;

#endif /* FEFONTSTRUCT_H */
