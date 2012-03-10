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
