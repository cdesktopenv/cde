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
/* $XConsortium: lexcode.h /main/3 1996/06/19 17:15:33 drk $ */
/* Definitions of lexical codes needed by both lextaba.c and lexrf.c. */

#define FCE    27   /* FRE  Free character in use as an entity reference */
#define FRE     0   /* FREECHAR that is not in a CON delimiter-in-context. */
#define LITC   21   /* LIT LITA PIC or EE in use as a literal terminator */
#define MSC3   15   /* ]    Also MSC[2]. */
#define NET    17   /* /    When enabled. */
#define ETI    16   /* /    Actually ETAGO[2] */
#define SPCR   19   /* Space in use as SR8. */
#define TGO2   25   /* <    TAGO; also MDO[1], PIO[1] */
#define CDE    11   /* NONSGML   delcdata CDATA/SDATA delimiter */
