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
/* $XConsortium: ilcontext.h /main/3 1995/10/23 15:44:04 rswiston $ */
#ifndef ILCONTEXT_H
#define ILCONTEXT_H

    /*  Defines what a context looks like internally.
    */

#ifndef ILINT_H
#include "ilint.h"
#endif

        /*  Internal view of the context, i.e. the public type "ilContext". */

        /*  Indices into ilContextRec.pAlloc */
#define IL_CONTEXT_ALLOC_EFS    0           /* owned by /ilc/ilefs.c */
#define IL_CONTEXT_MAX_ALLOC    4           /* max # of above pAlloc indices */

typedef struct {
    ilContextPublicRec  p;                  /* public view of context, /ilinc/il.h */

        /*  Owned fields - if you are accessing them outside of the named
            owning file, you're doing something wrong!
        */
    ilPtr               pImageData;         /* owned by /ilc/ilimage.c */
    unsigned int        privateType;        /* owned by /ilc/ilcontext.c */
    ilObjectRec         objectHead;         /* owned by /ilc/ilobject.c */
    ilPtr               pXData;             /* owned by /ilc/ilX.c */

        /*  Array of ptrs which if non-null are freed _after_ all objects destroyed. */
    ilPtr               pAlloc [IL_CONTEXT_MAX_ALLOC];
    } ilContextRec, *ilContextPtr;

        /*  Typecast the given ptr to an ilContextPtr. The given ptr should be an 
            ilContext; this macro does not check.
        */
#define IL_CONTEXT_PTR(_context) ((ilContextPtr)_context)

#endif
