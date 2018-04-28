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
/* $XConsortium: define.h /main/4 1995/11/01 15:51:12 rswiston $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        define.h
 **
 **   Project:     CDE
 **
 **   Description: Public include file for define.c
 **
 **
 **   (c) Copyright 1987, 1988, 1989, 1990, 1991, 1992
 **       by Hewlett-Packard Company
 **
 **
 **
 ****************************************************************************
 ************************************<+>*************************************/

#ifndef _Dtksh_define_h
#define _Dtksh_define_h



extern int do_define( 
                        int argc,
                        char **argv) ;
extern int fdef( 
                        char *str,
                        unsigned long *val) ;
extern int do_deflist( 
                        int argc,
                        char **argv) ;
extern int do_finddef( 
                        int argc,
                        char **argv) ;



#endif /* _Dtksh_define_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
