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
