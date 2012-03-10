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
