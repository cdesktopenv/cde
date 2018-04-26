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
/* $XConsortium: struct.h /main/4 1995/11/01 15:56:45 rswiston $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        struct.h
 **
 **   Project:     CDE
 **
 **   Description: Public include file for struct.c
 **
 **
 **   (c) Copyright 1987, 1988, 1989, 1990, 1991, 1992
 **       by Hewlett-Packard Company
 **
 **
 **
 ****************************************************************************
 ************************************<+>*************************************/

#ifndef _Dtksh_struct_h
#define _Dtksh_struct_h

#include "exksh.h"



extern memtbl_t * ffind( 
                        memtbl_t *tbl,
                        char *fld,
                        char **pptr) ;
extern int do_struct( 
                        int argc,
                        char **argv) ;
extern int do_typedef( 
                        int argc,
                        char **argv) ;
extern int parse_decl( 
                        char * argv0,
                        struct memtbl *mem,
                        char *decl,
                        int tst) ;
extern int do_structlist( 
                        int argc,
                        char **argv) ;
extern int add_structlist( 
                        struct memtbl **memptr,
                        char *prefix,
                        int id) ;
extern int strparse( 
                        memtbl_t *tbl,
                        char **pbuf,
                        char *val) ;
extern int strfree( 
                        char *buf,
                        char *type) ;
extern int do_sizeof( 
                        int argc,
                        char **argv) ;
extern memtbl_t * all_tbl_find( 
                        char *name,
                        int tbl,
                        long id) ;
extern memtbl_t * all_tbl_search( 
                        char *name,
                        int flag) ;
extern memtbl_t * asl_find( 
                        memtbl_t *ptbl,
                        memtbl_t *tbls,
                        char *fld,
                        char **pptr) ;



#endif /* _Dtksh_struct_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
