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
/* $XConsortium: ilobject.c /main/3 1995/10/23 15:58:21 rswiston $ */
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

        /*  /ilc/ilobject.c : General object handling code, e.g. ilDestroyObject().
        */

#include <stdlib.h>

#include "ilint.h"
#include "ilcontext.h"
#include "ilerrors.h"


        /*  ------------------------ ilCreateObject ----------------------------------- */
        /*  Creates an object of size "sizeInBytes" and returns a ptr to it, or null
            if failure (the error code in "context" is set in either case).  The object
            is added to the given "context". "sizeInBytes" must be a minimum of
            "sizeof(ilObjectRec)".
                The function "Destroy" will be called by ilDestroyObject(), as:
                    Destroy (ilObjectPtr pObject);
            Before destroying this object.  The Destroy() function must free any 
            object-specific data etc.  It MUST not touch the fields in the object header
            (ilObjectRec) or free *pObject.
        */

IL_PRIVATE ilObjectPtr _ilCreateObject (
    ilContext           context,                /* context to add object to */
    int                 objectType,             /* code for object, e.g. IL_PIPE */
    void                (*Destroy)(),           /* destroy function; see above */
/* Use portable type, bug report OSF_QAR#32082 */
    size_t              sizeInBytes             /* size of object to create */
    )
{
register ilContextPtr   pContext;
register ilObjectPtr    pObject;

        /*  Allocate object rec, return if failure.
        */
    pContext = (ilContextPtr)context;
    pObject = (ilObjectPtr)IL_MALLOC (sizeInBytes);
    if (!pObject) {
        pContext->p.error = IL_ERROR_MALLOC;
        return (ilObjectPtr)NULL;
        }

        /*  Fill in object header, init refCount to 1 => no one attached to it yet.
        */
    pObject->p.context = (ilContext)pContext;
    pObject->p.objectType = objectType;
    pObject->p.pPrivate = (ilPtr)NULL;
    pObject->Destroy = Destroy;
    pObject->refCount = 1;

        /*  Add the object to the front of the list of objects.
        */
    pObject->pNext = pContext->objectHead.pNext;
    pObject->pPrev = (ilPtr)&pContext->objectHead;
    ((ilObjectPtr)pContext->objectHead.pNext)->pPrev = (ilPtr)pObject;
    pContext->objectHead.pNext = (ilPtr)pObject;

    pContext->p.error = IL_OK;
    return pObject;
}

        /*  ------------------------ ilDestroyObject -------------------------------- */
        /*  Public function: see spec.
        */

ilBool ilDestroyObject (
    ilObject            object
    )
{
register ilObjectPtr    pObject;

        /*  Downcount refCount, exit if not zero - object is still attached to others.
            Call object-specific destroy function, free client private if present, 
            remove object from the linked list, free the object.
        */
    pObject = (ilObjectPtr)object;
    if ((pObject->p.objectType == IL_NULL_OBJECT) 
     || (pObject->p.objectType > IL_MAX_OBJECT_TYPE)) {
        pObject->p.context->error = IL_ERROR_OBJECT_TYPE;
        return FALSE;
        }

    pObject->p.context->error = 0;
    if (--pObject->refCount > 0)
        return TRUE;                    /* object still attached; EXIT */

    (*pObject->Destroy) (pObject);
    if (pObject->p.pPrivate)
        free (pObject->p.pPrivate);     /* note: caller used malloc, not IL_MALLOC() */

    ((ilObjectPtr)pObject->pPrev)->pNext = (ilPtr)pObject->pNext;
    ((ilObjectPtr)pObject->pNext)->pPrev = (ilPtr)pObject->pPrev;
    IL_FREE (pObject);

    return TRUE;
}


        /*  ------------------------ ilObjectInitContext ------------------------- */
        /*  Called by ilCreateContext() to create object data in the given context.
                Returns: true if add was ok, else error: caller should free all.
        */

IL_PRIVATE ilBool _ilObjectInitContext (
    ilContextPtr        pContext
    )
{

        /*  pCOD->head points to the list of objects; init list to null (point to self).
        */
    pContext->objectHead.p.context = (ilContext)pContext;
    pContext->objectHead.pPrev = pContext->objectHead.pNext = (ilPtr)&pContext->objectHead;
    return TRUE;
}


        /*  ------------------------ ilObjectDestroyContext ------------------------- */
        /*  Called by ilDestroyContext() to destroy object data in the given context.
        */

IL_PRIVATE void _ilObjectDestroyContext (
    ilContextPtr        pContext
    )
{
register ilObjectPtr    pObject, pNextObject, pObjectHead;

        /*  Loop thru and destroy all objects until list points back to head (done).
        */
    pObjectHead = &pContext->objectHead;
    pObject = (ilObjectPtr)pObjectHead->pNext;
    while (pObject != pObjectHead) {
        pNextObject = (ilObjectPtr)pObject->pNext;
        ilDestroyObject ((ilObject)pObject);
        pObject = pNextObject;
        }
}



