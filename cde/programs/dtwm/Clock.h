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
/**---------------------------------------------------------------------
***	
***	file:		Clock.h
***
***	project:	MotifPlus Widgets
***
***	description:	Public include file for DtClock class.
***			Portions adapted from the Xaw Clock widget.
***	
***	
***			(c) Copyright 1990 by Hewlett-Packard Company.
***
***
Copyright 1987, 1988 by Digital Equipment Corporation, Maynard, Massachusetts,
and the Massachusetts Institute of Technology, Cambridge, Massachusetts.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the names of Digital or MIT not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.  

DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.
***
***
***-------------------------------------------------------------------*/

#ifndef _DtClock_h
#define _DtClock_h

#include <Xm/Xm.h>

#ifndef DtIsClock
#define DtIsClock(w) XtIsSubclass(w, dtClockGadgetClass)
#endif /* DtIsClock */

/********    Public Function Declarations    ********/

extern Widget DtCreateClock( 
                        Widget parent,
                        String name,
                        ArgList arglist,
                        Cardinal argcount) ;

extern WidgetClass dtClockGadgetClass;

typedef struct _DtClockClassRec * DtClockClass;
typedef struct _DtClockRec      * DtClockGadget;

#endif /* _DtClock_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
