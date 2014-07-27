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
/* $XConsortium: ilinvert.c /main/5 1996/06/19 12:21:28 ageorge $ */
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

/* =============================================================================
     /ilc/ilinvert.c : Image Library conversion routines for bitonal polarity.
   =============================================================================== */


#include "ilint.h"
#include "ilpipelem.h"
#include "ilconvert.h"
#include "ilerrors.h"



typedef struct {
    ilBool          bitonal;                /* bitonal flag */
    long            nPixels;                /* width - 1 of src/dst images */
    long            srcRowBytes;            /* bytes/row of src image */
    ilPtr           pSrcPixels;             /* ptr to start of src pixels */
    long            dstRowBytes;            /* bytes/row of dst image */
    ilPtr           pDstPixels;             /* ptr to start of dst pixels */
    } ilinvertRec, *ilinvertPtr;


/* =============================================================================================================================
             ilInitInvert     -   Init() function.
   ============================================================================================================================= */


static ilError ilInitInvert (
    ilinvertPtr  pPriv,
    ilImageInfo        *pSrcImage,
    ilImageInfo        *pDstImage
    )
{

    pPriv->srcRowBytes = pSrcImage->plane[0].nBytesPerRow;
    pPriv->pSrcPixels = pSrcImage->plane[0].pPixels;
    pPriv->dstRowBytes = pDstImage->plane[0].nBytesPerRow;
    pPriv->pDstPixels = pDstImage->plane[0].pPixels;

    pPriv->nPixels =  (pPriv->srcRowBytes < pPriv->dstRowBytes)  ?  pPriv->srcRowBytes :  pPriv->dstRowBytes;

    return IL_OK;
}


/* =============================================================================================================================
            ilExecuteinvert    -    Execute() function for bitonal & grayscale polarity conversion.
   ============================================================================================================================= */

static ilError ilExecuteinvert  (
    ilExecuteData          *pData,
    long                    dstLine,
    long                   *pNLines             /* ignored on input */
    )
{
ilinvertPtr                 pPriv;
long                        srcnbytes, dstnbytes;
register ilPtr              psrcline, pdstline;
register ilPtr              psrc, pdst;
register long               nLines, nPixels;



    pPriv = (ilinvertPtr )pData->pPrivate;
    srcnbytes = pPriv->srcRowBytes;
    psrcline = pPriv->pSrcPixels + pData->srcLine * srcnbytes;
    dstnbytes = pPriv->dstRowBytes;
    pdstline = pPriv->pDstPixels + dstLine * dstnbytes;

    if (pPriv->nPixels < 0)  return 0;

    nLines = *pNLines;
    if (nLines <= 0) return 0;

    while ( nLines--  > 0 ) {
          psrc = psrcline;
          pdst = pdstline;
          nPixels = pPriv->nPixels;

          while ( nPixels--  >  0 )  *pdst++ = ~*psrc++;

          psrcline += srcnbytes;
          pdstline += dstnbytes;
    }


    return IL_OK;
}

/* =============================================================================================================================
            ilAddInvertFilter - function to add invert filter to the pipe - called by ilconvert.
   ============================================================================================================================= */
IL_PRIVATE ilBool _ilAddInvertFilter ( 
      ilPipe        pipe,
      ilImageDes    *pPipedes,
      ilPipeInfo    *pInfo
      )
{
ilDstElementData            dstData;
ilinvertPtr                 pPriv;
unsigned int                state;
ilPipeInfo                  info;
ilImageDes                  imdes;
ilImageFormat               imformat;


    state = ilGetPipeInfo(pipe, TRUE, &info, &imdes, &imformat);
    if(state != IL_PIPE_FORMING) {
       if (!pipe->context->error)
             ilDeclarePipeInvalid(pipe, IL_ERROR_PIPE_STATE);
       return FALSE;
    }

    dstData.producerObject = (ilObject)NULL;
    dstData.pDes           = pPipedes;
    dstData.pFormat        = (ilImageFormat *)NULL;
    dstData.width          = pInfo->width;
    dstData.height         = pInfo->height;
    dstData.stripHeight    = 0;
    dstData.constantStrip  = FALSE;
    dstData.pPalette       = (unsigned short *)NULL;
    pPriv = (ilinvertPtr ) ilAddPipeElement (pipe, IL_FILTER, sizeof (ilinvertRec), 0, (ilSrcElementData *)NULL,
                                             &dstData, ilInitInvert, IL_NPF, IL_NPF, ilExecuteinvert, NULL, 0);
    if (!pPriv) return FALSE;                    

    if ( imdes.type == IL_BITONAL )  pPriv->bitonal = TRUE;
    else pPriv->bitonal = FALSE;

    return TRUE;
}


