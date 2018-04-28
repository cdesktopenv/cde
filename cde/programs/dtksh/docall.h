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
/* $XConsortium: docall.h /main/4 1995/11/01 15:51:36 rswiston $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        docall.h
 **
 **   Project:     CDE
 **
 **   Description: Public include file for docall.c
 **
 **
 **   (c) Copyright 1987, 1988, 1989, 1990, 1991, 1992
 **       by Hewlett-Packard Company
 **
 **
 **
 ****************************************************************************
 ************************************<+>*************************************/

#ifndef _Dtksh_docall_h
#define _Dtksh_docall_h

#include "exksh.h"

extern int _Prdebug;
extern struct memtbl Null_tbl;
extern char xk_ret_buffer[];
extern char * xk_ret_buf;
extern struct Bfunction xk_prdebug;



extern int do_field_get( 
                        int argc,
                        char **argv) ;
extern int do_call( 
                        int argc,
                        char **argv) ;
extern int asl_set( 
                        char *argv0,
                        memtbl_t *tblarray,
                        char *desc,
                        unsigned char **pargs) ;
extern int do_field_comp( 
                        int argc,
                        char **argv) ;



#endif /* _Dtksh_docall_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
