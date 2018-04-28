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
/*
 * $XConsortium: TermP.h /main/1 1996/04/21 19:16:07 drk $";
 */

/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */

#ifndef	_Dt_TermP_h
#define	_Dt_TermP_h

#include "TermPrimP.h"
#include "Term.h"
#include "TermData.h"
#include "TermFunctionKey.h"

#ifdef	__cplusplus
extern "C" {
#endif	/* __cplusplus */

/* Vt class structure... */

typedef struct _DtTermClassPart
{
    int foo;
} DtTermClassPart;

/* full clas record declaration for Vt class... */
typedef struct _DtTermClassRec {
    CoreClassPart		core_class;
    XmPrimitiveClassPart	primitive_class;
    DtTermPrimitiveClassPart		term_primitive_class;
    DtTermClassPart		term_class;
} DtTermClassRec;

externalref DtTermClassRec dtTermClassRec;

/* vt instance record... */
typedef struct _DtTermPart
{
    DtTermData			td;	/* non-widget terminal data	*/
    Boolean                     autoWrap;
    Boolean                     reverseWrap;
    Boolean                     sunFunctionKeys;
    Boolean                     c132;
    Boolean			appKeypadMode;
    Boolean			appCursorMode;
} DtTermPart;

/* full instance record declaration... */

typedef struct _DtTermRec {
    CorePart		core;
    XmPrimitivePart	primitive;
    DtTermPrimitivePart	term;
    DtTermPart		vt;
} DtTermRec;

/* private function declarations... */
/* end private function declarations... */

#ifdef	__cplusplus
} /* close scope of 'extern "C"'... */
#endif	/* __cplusplus */

#endif	/* _Dt_TermP_h */
/* DON'T ADD ANYTHING AFTER THIS #endif... */
