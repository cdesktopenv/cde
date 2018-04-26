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
/* $XConsortium: genlib.h /main/4 1995/11/01 15:55:32 rswiston $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        genlib.h
 **
 **   Project:     CDE
 **
 **   Description: Public include file for genlib.c
 **
 **
 **   (c) Copyright 1987, 1988, 1989, 1990, 1991, 1992
 **       by Hewlett-Packard Company
 **
 **
 **
 ****************************************************************************
 ************************************<+>*************************************/

#ifndef _Dtksh_genlib_h
#define _Dtksh_genlib_h

#include "exksh.h"

extern struct libstruct *All_libs;



extern int xk_usage( 
                        char *funcname) ;
extern unsigned long fsym( 
                        char *str,
                        int lib) ;
extern int do_findsym( 
                        int argc,
                        char **argv) ;
#ifndef SPRINTF_RET_LEN
extern int lsprintf( 
                        char *buf,
                        char *fmt,
                        unsigned long arg1,
                        unsigned long arg2,
                        unsigned long arg3,
                        unsigned long arg4,
                        unsigned long arg5,
                        unsigned long arg6,
                        unsigned long arg7) ;
#endif



#endif /* _Dtksh_genlib_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
