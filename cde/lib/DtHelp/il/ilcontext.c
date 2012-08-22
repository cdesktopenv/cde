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
 * License along with these librararies and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
/* $XConsortium: ilcontext.c /main/3 1995/10/23 15:43:53 rswiston $ */
/**---------------------------------------------------------------------
***	
***    (c)Copyright 1991 Hewlett-Packard Co.
***    
***                             RESTRICTED RIGHTS LEGEND
***    Use, duplication, or disclosure by the U.S. Government is subject to
***    restrictions as set forth in sub-paragraph (c)(1)(ii) of the Rights in
***    Technical Data and Computer Software clause in DFARS 252.227-7013.
***                             Hewlett-Packard Company
***                             3000 Hanover Street
***                             Palo Alto, CA 94304 U.S.A.
***    Rights for non-DOD U.S. Government Departments and Agencies are as set
***    forth in FAR 52.227-19(c)(1,2).
***
***-------------------------------------------------------------------*/

        /*  ilcontext.c - Contains ilCreate/DestroyContext() and related code.
        */

#include <stdlib.h>

#include "ilint.h"
#include "ilcontext.h"
#include "ilerrors.h"

        /*  Functions in /ilc/ilobject.c :
        */
        /*  Called by ilCreateContext() to create object data in the given context.
                Returns: true if add was ok, else error: caller should free all.
        */
IL_EXTERN ilBool _ilObjectInitContext (
    ilContextPtr        pContext
    );

        /*  Called by ilDestroyContext() to destroy object data in the given context.
        */
IL_EXTERN void _ilObjectDestroyContext (
    ilContextPtr        pContext
    );


        /*  ------------------------ ilInternalCreateContext ------------------------- */
        /*  Called by the macro IL_CREATE_CONTEXT() which passes in the version 
            check number.  Only check the low-order 16 bits of the versionCheck;
            that way backwards-compatible versions can be differentiated by the
            upper 16 bits (which might signal the library to do something different).
        */

ilError ilInternalCreateContext (
    int                 versionCheck,
    ilContext          *pContextReturn,         /* RETURNED */
    unsigned long       mustBeZero
    )
{
register ilContextPtr   pContext;

        /*  If internal version # > the version # the library was built with, then
            error; if <, old IL program using new library: supported.
        */
    if ((versionCheck & 0xffff) > IL_INTERNAL_VERSION)
        return IL_ERROR_VERSION_MISMATCH;
    if (mustBeZero != 0)
        return IL_ERROR_PAR_NOT_ZERO;

    pContext = (ilContextPtr)IL_MALLOC_ZERO (sizeof (ilContextRec));
    if (!pContext) 
        return IL_ERROR_MALLOC;

    if (!_ilObjectInitContext (pContext)) {
        IL_FREE (pContext);
        return IL_ERROR_MALLOC;
        }

    pContext->p.error = 0;
    pContext->p.errorInfo = 0;

        /*  Init private type code to start at standard image types.
        */
    pContext->privateType = IL_MAX_TYPE + 1;
    *pContextReturn = (ilContext)pContext;
    return 0;
}


#ifdef IL_GARBAGE_MALLOC

        /*  --------------------- ilMallocAndInitWithGarbage ------------------------ */
        /*  Referenced by IL_MALLOC() macro when IL_GARBAGE_MALLOC defined (should be
            defined only during test/debug - not in production product.)
            malloc the given nBytes, fill it with garbage and return ptr to it.
        */
IL_PRIVATE void *_ilMallocAndInitWithGarbage (
    unsigned long           nBytes
    )
{
register ilPtr              p, pMalloc;

    pMalloc = (ilPtr)malloc (nBytes);
    if (p = pMalloc) {
      while (nBytes-- > 0)
        *p++ = 0xFD;
      }
    return (void *)pMalloc;
}

#endif

        /*  ----------------------- ilGetPrivateType ----------------------- */
        /*  Public function; see spec.
        */

unsigned int ilGetPrivateType (
    ilContext           context
    )
{
register ilContextPtr   pContext;

        /*  Increment code (but not if it has wrapped to zero! and return it.
        */
    pContext = IL_CONTEXT_PTR (context);
    if (pContext->privateType != 0)
        pContext->privateType++;
    return pContext->privateType;
}


        /*  ------------------------ ilDestroyContext ---------------------------- */
        /*  Public function; see spec.
        */

ilBool ilDestroyContext (
    ilContext           context
    )
{
register ilContextPtr   pContext;
int                     i;

        /*  Destroy all objects associated with this context, then free
            any data pointed to in pAlloc array.
        */
    pContext = IL_CONTEXT_PTR (context);
    _ilObjectDestroyContext (pContext);

    for (i = 0; i < IL_CONTEXT_MAX_ALLOC; i++) {
        if (pContext->pAlloc[i])
            IL_FREE (pContext->pAlloc[i]);
        }

    IL_FREE (pContext);
    return TRUE;
}

        /*  ------------------------ ilIntersectRect ------------------------------ */
        /*  Intersect the rectangle "*pSrcRect" with the rect "*pDstRect",
            storing the result in "*pDstRect".
                Not in this file for any particular reason; no other logical place for it.
        */
IL_PRIVATE void _ilIntersectRect (
    register ilRect     *pSrcRect,
    register ilRect     *pDstRect
    )
{
long                    left, top, right, bottom, i;

        /*  Change to non-inclusive coords for easier compare.
        */
    left = pSrcRect->x;
    right = left + pSrcRect->width;
    top = pSrcRect->y;
    bottom = top + pSrcRect->height;

    if (pDstRect->x > left)
        left = pDstRect->x;
    if (pDstRect->y > top)
        top = pDstRect->y;
    i = pDstRect->x + pDstRect->width;          /* pDstRect right */
    if (i < right)
        right = i;
    i = pDstRect->y + pDstRect->height;         /* pDstRect bottom */
    if (i < bottom)
        bottom = i;

        /*  Store result into pDstRect, with width/height always >= 0.
        */
    pDstRect->x = left;
    pDstRect->y = top;
    pDstRect->width = (right <= left) ? 0 : right - left;
    pDstRect->height = (bottom <= top) ? 0 : bottom - top;
}

