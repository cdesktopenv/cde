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
