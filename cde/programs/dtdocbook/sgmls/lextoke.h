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
/* $XConsortium: lextoke.h /main/3 1996/06/19 17:15:50 drk $ */
/* LEXTOKE.H: Symbols for tokenization lexical classes.
*/
#define INV     0   /* Invalid Chars      Not allowed in an SGML name. */
#define REC     1   /* Record Boundary    RS and RE. */
#define SEP     2   /* Separator          TAB. */
#define SP      3   /* SPACE */
#define NMC     4   /* NAMECHAR  . _      Period, underscore (plus NMS, NUM). */
#define NMS     5   /* NAMESTRT           Lower and uppercase letters */
#define NU      6   /* NUMERAL            Numerals */
#define EOB     7   /* NONCHAR   28       End disk buffer. */
