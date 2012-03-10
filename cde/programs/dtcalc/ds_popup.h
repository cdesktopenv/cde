/* $XConsortium: ds_popup.h /main/3 1995/11/01 12:40:17 rswiston $ */
/*                                                                      *
 *  ds_popup.h                                                          *
 *   Contains the NO_PROTO defines for functions is ds_popup.h.         *
 *                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company			*
 * (c) Copyright 1993, 1994 International Business Machines Corp.	*
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.			*
 * (c) Copyright 1993, 1994 Novell, Inc. 				*
 */

#ifndef _ds_popup_h
#define _ds_popup_h

#include <X11/Xlib.h>
#include <X11/Intrinsic.h>
#include <Xm/Xm.h>

/* For all function declarations, if ANSI then use a prototype. */

#if  defined(__STDC__)
#define P(args)  args
#else  /* ! __STDC__ */
#define P(args)  ()
#endif  /* STDC */

/* Location ops for ds_position_popup(). */

enum ds_location_op {
        DS_POPUP_RIGHT,       /* Place popup to right of baseframe */
        DS_POPUP_LEFT,        /* Place popup to left of baseframe */
        DS_POPUP_ABOVE,       /* Place popup above baseframe */
        DS_POPUP_BELOW,       /* Place popup below baseframe */
        DS_POPUP_LOR,         /* Place popup to right or left of baseframe */
        DS_POPUP_AOB,         /* Place popup above or below baseframe */
        DS_POPUP_CENTERED     /* Center popup within baseframe */
} ;

int ds_position_popup         P((Widget, Widget, enum ds_location_op)) ;
int ds_force_popup_on_screen  P((Widget, int *, int *)) ;

void ds_get_screen_size       P((Widget, int *, int *)) ;

#endif /*!_ds_popup_h*/
