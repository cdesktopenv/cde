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
