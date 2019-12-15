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
/* $XConsortium: extra.h /main/4 1995/11/01 15:55:04 rswiston $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        extra.h
 **
 **   Project:     CDE
 **
 **   Description: Public include file for extra.c
 **
 **
 **   (c) Copyright 1987, 1988, 1989, 1990, 1991, 1992
 **       by Hewlett-Packard Company
 **
 **
 **
 ****************************************************************************
 ************************************<+>*************************************/

#ifndef _Dtksh_extra_h
#define _Dtksh_extra_h


extern void env_set( 
                        char *var) ;
extern void env_set_gbl( 
                        char *vareqval) ;
extern char * env_get( 
                        char *var) ;
extern int ksh_eval( 
                        char *cmd) ;
extern void env_set_var( 
                        char *var,
                        char *val) ;
extern void env_blank( 
                        char *var) ;
extern void printerr( 
                        char *cmd,
                        char *msg1,
                        char *msg2) ;
extern void printerrf( 
                        char *cmd,
                        char *fmt,
                        char *arg0,
                        char *arg1,
                        char *arg2,
                        char *arg3,
                        char *arg4,
                        char *arg5,
                        char *arg6,
                        char *arg7) ;


#endif /* _Dtksh_extra_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
