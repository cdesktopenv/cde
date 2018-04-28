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
