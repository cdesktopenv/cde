/* $XConsortium: widget.h /main/4 1995/11/01 15:57:51 rswiston $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        widget.h
 **
 **   Project:     CDE
 **
 **   Description: Public include file for widget.c
 **
 **
 **   (c) Copyright 1987, 1988, 1989, 1990, 1991, 1992
 **       by Hewlett-Packard Company
 **
 **
 **
 ****************************************************************************
 ************************************<+>*************************************/

#ifndef _Dtksh_widget_h
#define _Dtksh_widget_h

#include <X11/X.h>
#include <X11/Intrinsic.h>
#include <X11/IntrinsicP.h>
#define NO_AST
#include "dtksh.h"
#undef NO_AST

extern int Wtab_free;
extern wtab_t **W;
extern int NumW;
extern int MaxW;



extern void init_widgets( void ) ;
extern classtab_t * str_to_class( 
                        char *arg0,
                        char *s) ;
extern Widget DtkshNameToWidget( 
                        String s) ;
extern wtab_t * str_to_wtab( 
                        char *arg0,
                        char *v) ;
extern Widget handle_to_widget( 
                        char *arg0,
                        char *handle) ;
extern wtab_t * widget_to_wtab( 
                        Widget w) ;
extern void get_new_wtab( 
                        wtab_t **w,
                        char *name) ;
extern int do_DtLoadWidget( 
                        int argc,
                        char *argv[]) ;
extern int do_DtWidgetInfo( 
                        int argc,
                        char *argv[]) ;
extern void alt_env_set_var( 
                        char *variable,
                        char *value) ;



#endif /* _Dtksh_widget_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
