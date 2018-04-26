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
/* $XConsortium: misc.h /main/4 1995/11/01 15:56:04 rswiston $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        misc.h
 **
 **   Project:     CDE
 **
 **   Description: Public include file for misc.c
 **
 **
 **   (c) Copyright 1987, 1988, 1989, 1990, 1991, 1992
 **       by Hewlett-Packard Company
 **
 **
 **
 ****************************************************************************
 ************************************<+>*************************************/

#ifndef _Dtksh_misc_h
#define _Dtksh_misc_h



extern int symcomp( 
                        const void *sym1,
                        const void *sym2) ;
extern void * getaddr( 
                        char *str) ;
extern int do_deref( 
                        int argc,
                        char **argv) ;
extern void * nop( 
                        void *var) ;
extern void * save_alloc( 
                        void *var) ;


#endif /* _Dtksh_misc_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
