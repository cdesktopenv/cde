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
/* $XConsortium: ilefs.c /main/3 1995/10/23 15:47:01 rswiston $ */
/**---------------------------------------------------------------------
***	
***    (c)Copyright 1992 Hewlett-Packard Co.
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

#include <string.h>

#include "ilint.h"
#include "ilcontext.h"
#include "ilefs.h"
#include "ilpipeint.h"
#include "ilerrors.h"

    /*  Beginning of file and file type recs - separate so list heads are smaller. */
typedef struct {
    ilObjectRec             o;                  /* std header: MUST BE FIRST */
    struct _ilEFSFileTypeRec *pNext, *pPrev;    /* forward / back ptrs */
    } ilEFSHeaderRec, *ilEFSHeaderPtr;

    /*  Private definition of an ilEFSFileType: an IL object. */
typedef struct _ilEFSFileTypeRec {
    ilEFSHeaderRec          h;                  /* header: MUST BE FIRST */
    ilEFSFileTypeInfo       info;               /* public file type info */
    } ilEFSFileTypeRec, *ilEFSFileTypePtr;


    /*  EFS private data, hung off of context.pAlloc[IL_CONTEXT_ALLOC_EFS]. */
typedef struct {
    ilEFSHeaderRec          fileTypeHead;       /* list of active file types */
    ilEFSHeaderRec          replacedFileTypeHead; /* list of replaced file types */
    } ilEFSPrivateRec, *ilEFSPrivatePtr;


    /*  Private definition of an ilEFSFile: an IL object. */
typedef struct {
    ilObjectRec             o;                  /* std header: MUST BE FIRST */
    ilEFSFileInfo           info;               /* public file type info */
    ilEFSFileTypePtr        pFileType;          /* convenience: ptr to file type */
    } ilEFSFileRec, *ilEFSFilePtr;


    /*  Add _pFileType in front of element _pNextFileType */
#define LINK_FILE_TYPE(_pFileType, _pNextFileType) { \
    (_pFileType)->h.pNext = (_pNextFileType); \
    (_pFileType)->h.pPrev = (_pNextFileType)->h.pPrev; \
    (_pFileType)->h.pPrev->h.pNext = (_pFileType); \
    (_pNextFileType)->h.pPrev = (_pFileType); \
    }

    /*  Remove the given file type from its linked list */
#define UNLINK_FILE_TYPE(_pFileType) { \
    (_pFileType)->h.pPrev->h.pNext = (_pFileType)->h.pNext; \
    (_pFileType)->h.pNext->h.pPrev = (_pFileType)->h.pPrev; \
    }

    /*  Return true if given two strings are equal */
#define STRING_EQUAL(str, cmpstr) (strcmp ((str), (cmpstr)) == 0)

        /*  In efsinit.c :
            Called by the IL when EFS function is called.  Calls the individual
            Init() function for each file type to be supported by EFS.
        */
extern ilBool _ilefsInitStandardFiles (
    ilContext               context
    );


        /*  ------------------------ ilInitEFS ---------------------------------- */
        /*  Init EFS if not already inited (non-null ptr off of context.)
            Return ptr to EFS context private or null if error.
        */
static ilEFSPrivatePtr ilInitEFS (
    ilContext               context
    )
{
ilEFSPrivatePtr    pPriv;

        /*  If EFS file type data not present malloc and zero it, and then point
            context pAlloc to it - if not, could recurse forever.
        */
    context->error = IL_OK;
    pPriv = (ilEFSPrivatePtr)((ilContextPtr)context)->pAlloc[IL_CONTEXT_ALLOC_EFS];
    if (!pPriv) {
        pPriv = (ilEFSPrivatePtr)IL_MALLOC (sizeof (ilEFSPrivateRec));
        if (!pPriv) {
            context->error = IL_ERROR_MALLOC;
            return (ilEFSPrivatePtr)NULL;
            }
        ((ilContextPtr)context)->pAlloc[IL_CONTEXT_ALLOC_EFS] = (ilPtr)pPriv;

            /*  Init file type list to null. */
        pPriv->fileTypeHead.pNext = pPriv->fileTypeHead.pPrev = 
            (ilEFSFileTypePtr)&pPriv->fileTypeHead;
        pPriv->replacedFileTypeHead.pNext = pPriv->replacedFileTypeHead.pPrev = 
            (ilEFSFileTypePtr)&pPriv->replacedFileTypeHead;

            /*  Call to external lib(s) to callback and add each file type */
        if (!_ilefsInitStandardFiles (context)) {
            IL_FREE (pPriv);
            return (ilEFSPrivatePtr)NULL;
            }
        }
    return pPriv;
}


/*  ================================== FILE TYPE CODE =============================== */

        /*  ------------------------ ilFindFileType ---------------------------------- */
        /*  Find the file type with the given "name" in the list of file types whose
            head is pointed to by "pListHead".  Return ptr to found file type or null.
        */
static ilEFSFileTypePtr ilFindFileType (
    char                   *name,
    ilEFSFileTypePtr        pListHead
    )
{
ilEFSFileTypePtr   pFileType;

    pFileType = pListHead->h.pNext;
    while (pFileType != pListHead) {
        if (STRING_EQUAL (name, pFileType->info.name))
            return pFileType;                   /* found, EXIT */
        pFileType = pFileType->h.pNext;
        }
    return (ilEFSFileTypePtr)NULL;              /* not found, return null */
}


        /*  ------------------------ ilAddFileTypeToList --------------------------- */
        /*  Add the file type pointed to by "pFileType" to the list of file types whose
            head is pointed to by "pListHead".  Element is placed in list based its
            checkOrder.
        */
static void ilAddFileTypeToList (
    ilEFSFileTypePtr pFileType,
    ilEFSFileTypePtr        pListHead
    )
{
ilEFSFileTypePtr   pListFileType;
int                checkOrder;

    checkOrder = pFileType->info.checkOrder;
    pListFileType = pListHead->h.pNext;
    while (pListFileType != pListHead) {
        if (checkOrder > pListFileType->info.checkOrder)
            break;                              /* spot found; break */
        pListFileType = pListFileType->h.pNext;
        }
    LINK_FILE_TYPE (pFileType, pListFileType)   /* insert in front of pListFileType */
}


        /*  ------------------------ ilEFSAddFileType ---------------------------- */
        /*  Public function: see spec.
        */

        /*  Object Destroy() function for file type objects. */
static void ilDestroyFileType (
    ilEFSFileTypePtr pFileType
    )
{
ilEFSPrivatePtr             pPriv;
ilEFSFileTypePtr   pReplaced;

    pPriv = (ilEFSPrivatePtr)
        ((ilContextPtr)(pFileType->h.o.p.context))->pAlloc[IL_CONTEXT_ALLOC_EFS];

        /*  Remove file type from its current list.  Search the "replaced" list for a
            file type of same name; if found, move it from replaced to active list.
        */
    UNLINK_FILE_TYPE (pFileType)
    pReplaced = ilFindFileType (pFileType->info.name, 
                                (ilEFSFileTypePtr)&pPriv->replacedFileTypeHead);
    if (pReplaced) {
        UNLINK_FILE_TYPE (pReplaced)
        ilAddFileTypeToList (pReplaced, (ilEFSFileTypePtr)&pPriv->fileTypeHead);
        }
}


ilEFSFileType ilEFSAddFileType (
    ilContext               context,
    ilEFSFileTypeInfo *pInfo,
    void                   *pOptions
    )
{
ilEFSPrivatePtr             pPriv;
int                         i;
ilEFSFileTypePtr   pFileType, pReplace;
unsigned long      openModes;

        /* masks for defined bits in ilEFSFileTypeInfo - others invalid */
#define OPEN_MODE_MASKS (1<<IL_EFS_READ | 1<<IL_EFS_READ_SEQUENTIAL | 1<<IL_EFS_WRITE)
#define ATTRIBUTES_MASKS (IL_EFS_MULTI_PAGE_READS | IL_EFS_MULTI_PAGE_WRITES | \
                          IL_EFS_MASK_READS | IL_EFS_MASK_WRITES | IL_EFS_SCALEABLE_READS)

    if (pOptions) {
        context->error = IL_ERROR_PAR_NOT_ZERO;
        return (ilEFSFileType)NULL;
        }

        /*  Validate *pInfo: strings present, unused mask bits zero, functions right. */
    if ((strlen (pInfo->name) > (IL_EFS_MAX_NAME_CHARS - 1))
     || (strlen (pInfo->displayName) > (IL_EFS_MAX_DISPLAY_NAME_CHARS - 1))
     || (pInfo->nExtensions > IL_EFS_MAX_EXTENSIONS))
        goto badFileTypeInfo;

    for (i = 0; i < pInfo->nExtensions; i++)
        if (strlen (pInfo->extensions[i]) > (IL_EFS_MAX_EXTENSION_CHARS - 1))
            goto badFileTypeInfo;

    openModes = pInfo->openModes;
    if ((openModes & ~OPEN_MODE_MASKS)
     || (pInfo->attributes & ~ATTRIBUTES_MASKS))
        goto badFileTypeInfo;

    if (((!pInfo->Open || !pInfo->Open) && openModes)
     || (!pInfo->Seek && (openModes & 1<<IL_EFS_READ) 
           && (pInfo->attributes & IL_EFS_MULTI_PAGE_READS))
     || ((!pInfo->GetPageInfo || !pInfo->ReadImage) 
          && (openModes & (1<<IL_EFS_READ | 1<<IL_EFS_READ_SEQUENTIAL)))
     || (!pInfo->WriteImage && (openModes & 1<<IL_EFS_WRITE)))
        goto badFileTypeInfo;

    for (i = 0; i < IL_EFS_TYPE_RESERVED_SIZE; i++)
        if (pInfo->reserved[i] != 0)
            goto badFileTypeInfo;

        /*  Init EFS file types if not already inited. */
    if (!(pPriv = ilInitEFS (context))) 
        return (ilEFSFileType)NULL;

        /*  Create a file type object, exit if failure. Set *pInfo into it. */
    pFileType = (ilEFSFileTypePtr)_ilCreateObject (context, IL_EFS_FILE_TYPE, 
                ilDestroyFileType, sizeof (ilEFSFileTypeRec));
    if (!pFileType) return (ilEFSFileType)NULL;
    pFileType->info = *pInfo;

        /*  Search the active list for file type with same name.  If found, remove it
            from active list and add to replaced list, in front of any with same name.
        */
    pReplace = ilFindFileType (pFileType->info.name, 
                               (ilEFSFileTypePtr)&pPriv->fileTypeHead);
    if (pReplace) {
        ilEFSFileTypePtr pReplaceFront;
        UNLINK_FILE_TYPE (pReplace)
        pReplaceFront = ilFindFileType (pFileType->info.name, 
                                        (ilEFSFileTypePtr)&pPriv->fileTypeHead);
        if (!pReplaceFront)
            pReplaceFront = (ilEFSFileTypePtr)&pPriv->fileTypeHead;
        LINK_FILE_TYPE (pReplace, pReplaceFront)
        }

        /*  Add the file type to the active list */
    ilAddFileTypeToList (pFileType, (ilEFSFileTypePtr)&pPriv->fileTypeHead);

    context->error = IL_OK;
    return (ilEFSFileType)pFileType;

        /*  goto point if invalid file type info: return error. */
badFileTypeInfo:
    context->error = IL_ERROR_EFS_FILE_TYPE_INFO;
    return (ilEFSFileType)NULL;
}


        /*  ------------------------ ilEFSGetFileTypeInfo ---------------------------- */
        /*  Public function: see spec.
        */
ilBool ilEFSGetFileTypeInfo (
    ilEFSFileType           fileType,
    ilEFSFileTypeInfo      *pInfo
    )
{
ilEFSFileTypePtr   pFileType;

    pFileType = (ilEFSFileTypePtr)fileType;
    if (pFileType->h.o.p.objectType != IL_EFS_FILE_TYPE) {
        pFileType->h.o.p.context->error = IL_ERROR_OBJECT_TYPE;
        return FALSE;
        }

    *pInfo = pFileType->info;
    pFileType->h.o.p.context->error = IL_OK;
    return TRUE;
}


        /*  ------------------------ ilEFSListFileTypes ------------------------------ */
        /*  Public function: see spec.
        */
ilBool ilEFSListFileTypes (
    ilContext               context,
     int           *pNFileTypes,
    ilEFSFileType         **pfileTypes
    )
{
ilEFSPrivatePtr             pPriv;
int                nFileTypes;
ilEFSFileTypePtr   pFileType;
ilEFSFileType     *pfileType;

    *pNFileTypes = 0;
    *pfileTypes = (ilEFSFileType *)NULL;

        /*  Init EFS file types if not already inited. */
    if (!(pPriv = ilInitEFS (context)))
        return FALSE;

        /*  Count the number of file types in the list */
    nFileTypes = 0;
    pFileType = pPriv->fileTypeHead.pNext;
    while (pFileType != (ilEFSFileTypePtr)&pPriv->fileTypeHead) {
        pFileType = pFileType->h.pNext;
        nFileTypes++;
        }

        /*  Malloc space for returned file types - use malloc(); caller uses free().
            If no file types, still malloc something to return non-null ptr.
        */
    pfileType = (ilEFSFileType *)malloc ((nFileTypes > 0) ? 
                                          nFileTypes * sizeof (ilEFSFileType) : 4);
    if (!pfileType) {
        context->error = IL_ERROR_MALLOC;
        return FALSE;
        }

        /*  Return # of file types; traverse list to return ptrs to them. */
    *pfileTypes = pfileType;
    *pNFileTypes = nFileTypes;
    pFileType = pPriv->fileTypeHead.pNext;
    while (nFileTypes-- > 0) {
        *pfileType++ = (ilEFSFileType)pFileType;
        pFileType = pFileType->h.pNext;
        }

    context->error = IL_OK;
    return TRUE;
}


/*  =================================== FILE CODE ================================== */


        /*  --------------------------- ilEFSOpenFile ------------------------------- */
        /*  Public function: see spec.
        */

        /*  Object Destroy() function for file objects. */
static void ilDestroyFile (
    ilEFSFilePtr            pFile
    )
{
ilEFSFileTypePtr   pFileType;

        /*  Get ptr to file type; if null, file not actually open yet; skip Close() */
    pFileType = (ilEFSFileTypePtr)pFile->info.fileType;
    if (pFileType)
        (*pFileType->info.Close) (pFile);
}


ilEFSFile ilEFSOpen (
    ilContext               context,
    char                   *fileName,
    unsigned int            openMode,
    unsigned long           searchOptions,
    char                   *typeName,
    void                   *pOptions
    )
{
ilEFSPrivatePtr             pPriv;
ilEFSFilePtr       pFile;
ilEFSFileTypePtr   pFileType, pListHead;
ilBool                      readOpen;
long                        nPages;
ilPtr                       pOpenPriv;
char                        extension [IL_EFS_MAX_EXTENSION_CHARS];

        /*  Validate pOptions, openMode (set readOpen true if a read). */
    context->error = IL_OK;
    if (pOptions) {
        context->error = IL_ERROR_PAR_NOT_ZERO;
        return (ilEFSFileType)NULL;
        }
    switch (openMode) {
      case IL_EFS_READ: 
      case IL_EFS_READ_SEQUENTIAL: 
          readOpen = TRUE; break;
      case IL_EFS_WRITE: 
          readOpen = FALSE; break;
      default:
        context->error = IL_ERROR_EFS_OPEN_MODE;
        return (ilEFSFile)NULL;
        }

        /*  Init EFS file types if not already inited. */
    if (!(pPriv = ilInitEFS (context)))
        return (ilEFSFile)NULL;

        /*  Add a file type object - goto openError to destroy it if an error later. */
    pFile = (ilEFSFilePtr)_ilCreateObject (context, IL_EFS_FILE, ilDestroyFile, 
                                          sizeof (ilEFSFileRec));
    if (!pFile) return (ilEFSFile)NULL;

        /*  Find pFileType for this file, searches enabled by mask in searchOptions.
            First try typeName, if non-null.  When found at any point: if openMode
            not supported, error - except for checking mode - else call Open() for
            the file type.  If it returns any error other than "not mine" abort.
        */
    pFileType = (ilEFSFileTypePtr)NULL;
    pListHead = (ilEFSFileTypePtr)&pPriv->fileTypeHead;
    if (typeName && (searchOptions & IL_EFS_BY_TYPE_NAME)) {
        pFileType = ilFindFileType (typeName, pListHead);
        if (pFileType) {                                    /* file type found */
            if (!(pFileType->info.openModes & (1 << openMode))) {
                context->error = IL_ERROR_EFS_OPEN_MODE;
                goto openError;
                }
            pOpenPriv = (*pFileType->info.Open) (pFileType, fileName, openMode, &nPages);
            if (!pOpenPriv) {
                if (context->error == IL_ERROR_EFS_NOT_MINE)
                    pFileType = (ilEFSFileTypePtr)NULL;     /* try next search method */
                else goto openError;
                }
            pFile->info.howFound = IL_EFS_BY_CHECKING;
            }
        }   /* END open by type name */


        /*  If not found, search for extension if enabled. */
    if (!pFileType && (searchOptions & IL_EFS_BY_EXTENSION)) {
        char                       *pExtension;
        ilEFSFileTypePtr   pSearch;
        int                         nChars;

        pExtension = strrchr (fileName, '.');
        if (pExtension) {                               /* is a "." in fileName */
            pExtension++;                               /* point past "." */
            nChars = strlen (pExtension);
            if (nChars > (IL_EFS_MAX_EXTENSION_CHARS - 1))
                nChars = IL_EFS_MAX_EXTENSION_CHARS - 1;
            bcopy (pExtension, extension, nChars);      /* extract "extension" */
            extension [nChars] = 0;

                /* Search list for extension match until pFileType found or list done */
            pSearch = pListHead->h.pNext;
            while (!pFileType && (pSearch != pListHead)) {
                int nExtensions = pSearch->info.nExtensions;
                while (nExtensions-- > 0)
                    if (STRING_EQUAL (extension, pSearch->info.extensions[nExtensions])) {
                        pFileType = pSearch;            /* extension found; quit */
                        pFile->info.howFound = IL_EFS_BY_CHECKING;
                        break;
                        }
                pSearch = pSearch->h.pNext;
                }
            if (pFileType) {
                if (!(pFileType->info.openModes & (1 << openMode))) {
                    context->error = IL_ERROR_EFS_OPEN_MODE;
                    goto openError;
                    }
                pOpenPriv = (*pFileType->info.Open) (pFileType, fileName, openMode, &nPages);
                if (!pOpenPriv) {
                    if (context->error == IL_ERROR_EFS_NOT_MINE)
                        pFileType = (ilEFSFileTypePtr)NULL;
                    else goto openError;
                    }
                }
            }   /* END have extension */
        }       /* END open by extension */


        /*  If not found, search by checking if a read openMode.   For each file type,
            try open if enabled (checkOrder != 0) and openMode supported for file type.
        */
    if (!pFileType && readOpen && (searchOptions & IL_EFS_BY_CHECKING)) {
        ilEFSFileTypePtr   pSearch;

        pSearch = pListHead->h.pNext;
        while (pSearch != pListHead) {
            if (pSearch->info.checkOrder && (pSearch->info.openModes & (1 << openMode))) {
                pOpenPriv = (*pSearch->info.Open) (pSearch, fileName, openMode, &nPages);
                if (pOpenPriv) {
                    pFileType = pSearch;                /* found right file type; break */
                    pFile->info.howFound = IL_EFS_BY_CHECKING;
                    break;
                    }
                else if (context->error != IL_ERROR_EFS_NOT_MINE)
                    goto openError;                     /* some error; give up */
                }
            pSearch = pSearch->h.pNext;
            }
        }   /* END open by checking */

        /*  If not found above, error: can't find file type handler for this file. */
    if (!pFileType) {
        context->error = IL_ERROR_EFS_NO_FILE_TYPE;
        goto openError;
        }

        /*  File type found.  Fill in info for the file, return pFile.  The object's
            pPrivate is set to the ptr returned from file type's Open().
        */
    pFile->o.p.pPrivate = pOpenPriv;
    pFile->info.fileType = (ilEFSFileType)pFileType;
    pFile->info.openMode = openMode;
    pFile->info.attributes = pFileType->info.attributes;
    pFile->info.nPages = nPages;
    pFile->pFileType = pFileType;

    context->error = IL_OK;
    return (ilEFSFile)pFile;                            /* success; return file */

        /*  openError: goto here on error after file object created. The Open() was not
            successful, so set fileType to null so ilDestroyFile() does not call Close().
        */
openError:
{   ilError     error;
    error = context->error;                 /* save error code */
    pFile->info.fileType = (ilEFSFileType)NULL;
    ilDestroyObject ((ilObject)pFile);
    context->error = error;                 /* restore error code */
    return (ilEFSFile)NULL;
}
}


        /*  ------------------------ ilEFSGetFileInfo ---------------------------- */
        /*  Public function: see spec.
        */
ilBool ilEFSGetFileInfo (
    ilEFSFile               file,
    ilEFSFileInfo          *pInfo                   /* RETURNED */
    )
{
ilEFSFilePtr       pFile;

    pFile = (ilEFSFilePtr)file;
    if (pFile->o.p.objectType != IL_EFS_FILE) {
        pFile->o.p.context->error = IL_ERROR_OBJECT_TYPE;
        return FALSE;
        }

        /*  Return file info; fill in "inUse": file in use if refCount > 1 */
    *pInfo = pFile->info;
    pInfo->inUse = (pFile->o.refCount > 1);
    pFile->o.p.context->error = IL_OK;
    return TRUE;
}


        /*  ------------------------ ilEFSSeek ---------------------------- */
        /*  Public function: see spec.
        */
ilBool ilEFSSeek (
    ilEFSFile               file,
    long                    page,
    void                   *pOptions
    )
{
ilEFSFilePtr       pFile;

    pFile = (ilEFSFilePtr)file;
    if (pOptions) {
        pFile->o.p.context->error = IL_ERROR_PAR_NOT_ZERO;
        return FALSE;
        }
    if (pFile->o.p.objectType != IL_EFS_FILE) {
        pFile->o.p.context->error = IL_ERROR_OBJECT_TYPE;
        return FALSE;
        }

        /*  Validate that file was opened for random read; error if not. */
    if (pFile->info.openMode != IL_EFS_READ) {
        pFile->o.p.context->error = IL_ERROR_EFS_OPEN_MODE;
        return FALSE;
        }

        /*  Call Seek() only if multi-page reads supported; not error if not. */
    pFile->o.p.context->error = IL_OK;
    if (!(pFile->info.attributes & IL_EFS_MULTI_PAGE_READS))
        return TRUE;
    else return (*pFile->pFileType->info.Seek) (file, page);
}


        /*  ------------------------ ilEFSGetPageInfo ---------------------------- */
        /*  Public function: see spec.
        */
ilBool ilEFSGetPageInfo (
    ilEFSFile               file,
    ilEFSPageInfo          *pInfo
    )
{
ilEFSFilePtr       pFile;

    pFile = (ilEFSFilePtr)file;
    if (pFile->o.p.objectType != IL_EFS_FILE) {
        pFile->o.p.context->error = IL_ERROR_OBJECT_TYPE;
        return FALSE;
        }

        /*  Validate that file was opened for read; call GetPageInfo() if so. */
    if ((pFile->info.openMode != IL_EFS_READ) 
     && (pFile->info.openMode != IL_EFS_READ_SEQUENTIAL)) {
        pFile->o.p.context->error = IL_ERROR_EFS_OPEN_MODE;
        return FALSE;
        }

    pFile->o.p.context->error = IL_OK;
    return (*pFile->pFileType->info.GetPageInfo) (file, pInfo);
}


        /*  ------------------------ ilEFSReadImage ---------------------------- */
        /*  Public function: see spec.
        */
ilBool ilEFSReadImage (
    ilPipe                  pipe,
    ilEFSFile               file,
    unsigned int            readMode,
    long                    width,
    long                    height,
    void                   *pOptions
    )
{
ilEFSFilePtr       pFile;

    pFile = (ilEFSFilePtr)file;
    if (pOptions) {
        pFile->o.p.context->error = IL_ERROR_PAR_NOT_ZERO;
        return FALSE;
        }
    if ((pipe->objectType != IL_PIPE) || (pFile->o.p.objectType != IL_EFS_FILE)) {
        pFile->o.p.context->error = IL_ERROR_OBJECT_TYPE;
        return FALSE;
        }

        /*  Validate readMode: mask allowed only if supported by file type. */
    switch (readMode) {
      case IL_EFS_READ_MAIN:
        break;
      case IL_EFS_READ_MASK:
        if (pFile->pFileType->info.attributes & IL_EFS_MASK_READS)
            break;                                      /* else fall thru for error */
      default:
        pFile->o.p.context->error = IL_ERROR_EFS_READ_MODE;
        return FALSE;
        }

        /*  Validate that file was opened for read. */
    if ((pFile->info.openMode != IL_EFS_READ) 
     && (pFile->info.openMode != IL_EFS_READ_SEQUENTIAL)) {
        pFile->o.p.context->error = IL_ERROR_EFS_OPEN_MODE;
        return FALSE;
        }

        /*  If pipe element added successfully inc file's refCount to mark in use,
            and add file to list of objects to be destroyed when pipe emptied.
        */
    pFile->o.p.context->error = IL_OK;
    if ((*pFile->pFileType->info.ReadImage) (pipe, file, readMode, width, height)) {
        pFile->o.refCount++;
        return _ilAddPipeDestroyObject (pipe, (ilObject)pFile);
        }
    else return FALSE;
}


        /*  ------------------------ ilEFSWriteImage ---------------------------- */
        /*  Public function: see spec.
        */
ilBool ilEFSWriteImage (
    ilPipe                  pipe,
    ilEFSFile               file,
    long                    xRes,
    long                    yRes,
    ilClientImage           maskImage,
    void                   *pOptions
    )
{
ilEFSFilePtr       pFile;

    pFile = (ilEFSFilePtr)file;
    if (pOptions) {
        pFile->o.p.context->error = IL_ERROR_PAR_NOT_ZERO;
        return FALSE;
        }
    if ((pipe->objectType != IL_PIPE) || (pFile->o.p.objectType != IL_EFS_FILE)) {
        pFile->o.p.context->error = IL_ERROR_OBJECT_TYPE;
        return FALSE;
        }

        /*  Validate that file was opened for write. */
    if (pFile->info.openMode != IL_EFS_WRITE) {
        pFile->o.p.context->error = IL_ERROR_EFS_OPEN_MODE;
        return FALSE;
        }

        /*  If file type doesnt handle masks, ignore maskImage, else validate it */
    if (maskImage) {
        if (!(pFile->pFileType->info.attributes & IL_EFS_MASK_WRITES))
            maskImage = (ilClientImage)NULL;
        else {
            ilImageInfo            *pInfo;
            const ilImageDes    *pDes;
            const ilImageFormat *pFormat;

            if (!ilQueryClientImage (maskImage, &pInfo, 0))
                return FALSE;
            pDes = pInfo->pDes;
            pFormat = pInfo->pFormat;
            if ((pDes->compression != IL_UNCOMPRESSED)
             || (pDes->nSamplesPerPixel != 1)
             || (pFormat->rowBitAlign != 32)
             || (pFormat->nBitsPerSample[0] != 1)) {
                pFile->o.p.context->error = IL_ERROR_EFS_OPEN_MODE;
                return FALSE;
                }
            }
        }

        /*  If pipe element added successfully inc file's refCount to mark in use,
            and add file/maskImage to list of objects to be destroyed when pipe emptied.
        */
    pFile->o.p.context->error = IL_OK;
    if ((*pFile->pFileType->info.WriteImage) (pipe, file, xRes, yRes, maskImage)) {
        if (maskImage) {
            ((ilObjectPtr)maskImage)->refCount++;
            _ilAddPipeDestroyObject (pipe, maskImage);
            }
        pFile->o.refCount++;
        return _ilAddPipeDestroyObject (pipe, (ilObject)pFile);
        }
    else return FALSE;
}

