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
/* $XConsortium: exksh_tbls.h /main/4 1995/11/01 15:54:45 rswiston $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        exksh_tbls.h
 **
 **   Project:     CDE
 **
 **   Description: Public include file for exksh_tbls.c
 **
 **
 **   (c) Copyright 1987, 1988, 1989, 1990, 1991, 1992
 **       by Hewlett-Packard Company
 **
 **
 **
 ****************************************************************************
 ************************************<+>*************************************/

#ifndef _Dtksh_exksh_tbls_h
#define _Dtksh_exksh_tbls_h

extern int Pr_tmpnonames;
extern int _Delim;



extern int (*find_special( 
                        int type,
                        char *name))() ;
extern int set_special( 
                        char *name,
                        int (*free)(),
                        int (*parse)(),
                        int (*print)()) ;
extern int xk_parse( 
                        memtbl_t *tbl,
                        char **buf,
                        char *p,
                        int nptr,
                        int sub,
                        void *pass,
                        memtbl_t *(*tbl_find)()) ;
extern int xk_get_delim( 
                        memtbl_t *tbl,
                        char *p) ;
extern int xk_get_pardelim( 
                        memtbl_t *tbl,
                        char *p) ;
extern int xk_print( 
                        memtbl_t *tbl,
                        char **buf,
                        char *p,
                        int nptr,
                        int sub,
                        void *pass,
                        memtbl_t *(*tbl_find)()) ;
extern int xk_free( 
                        memtbl_t *tbl,
                        char *p,
                        int nptr,
                        int sub,
                        memtbl_t *(*tbl_find)()) ;



#endif /* _Dtksh_exksh_tbls_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
