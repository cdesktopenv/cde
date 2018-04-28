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
/* $XConsortium: symbolic.h /main/4 1995/11/01 15:57:03 rswiston $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        symbolic.h
 **
 **   Project:     CDE
 **
 **   Description: Public include file for symbolic.c
 **
 **
 **   (c) Copyright 1987, 1988, 1989, 1990, 1991, 1992
 **       by Hewlett-Packard Company
 **
 **
 **
 ****************************************************************************
 ************************************<+>*************************************/

#ifndef _Dtksh_symbolic_h
#define _Dtksh_symbolic_h



extern struct symlist * fsymbolic( 
                        struct memtbl *tbl) ;
extern int do_symbolic( 
                        int argc,
                        char **argv) ;
extern int add_symbolic( 
                        int isflag,
                        struct memtbl *tbl,
                        struct symarray *syms,
                        int nsyms) ;



#endif /* _Dtksh_symbolic_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
