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
/******************************************************************************
*******************************************************************************
*
*  (c) Copyright 1992 HEWLETT-PACKARD COMPANY
*  ALL RIGHTS RESERVED
*  
*******************************************************************************
******************************************************************************/
#ifndef _DtButtonG_h
#define _DtButtonG_h

#include <Xm/Xm.h>

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

#ifndef DtIsButtonGadget
#define DtIsButtonGadget(w) XtIsSubclass(w, dtButtonGadgetClass)
#endif /* DtIsButtonGadget */

/* ArrowButton externs for application accessible functions */

extern Widget DtCreateButtonGadget( 
                        Widget parent,
                        char *name,
                        ArgList arglist,
                        Cardinal argcount) ;

/* Button Gadget */

externalref WidgetClass dtButtonGadgetClass;

typedef struct _DtButtonGadgetClassRec * DtButtonGadgetClass;
typedef struct _DtButtonGadgetRec      * DtButtonGadget;

typedef struct
{
    int     reason;
    XEvent  *event;
    int	    click_count;
} DtButtonCallbackStruct;


#if defined(__cplusplus) || defined(c_plusplus)
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _DtButtonG_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
