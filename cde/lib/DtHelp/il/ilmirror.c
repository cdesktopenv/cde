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
/* $XConsortium: ilmirror.c /main/4 1996/01/08 12:16:48 lehors $ */
/* =============================================================================================================================

     /ilc/ilmirror.c : Images Library mirror routines.   



        Date        Mail-ID     Description
        --------    ----------- -------------------------------------------------------------------
        07/11/91    larsson     Initial Coding.

        

   ============================================================================================================================= */

#include "ilint.h"
#include "ilpipelem.h"
#include "ilerrors.h"

#define LONGSZ        4
#define WORDPOS       32
#define BYTESIZE      8
#define LEFT_BIT_ON   0x80000000


    /*  Table of bytes with equivalent bits flipped, in /ilc/ilrotate.c */
extern const unsigned char ilBitReverseTable [];


 typedef struct {
        int    illinecount;          /* running line count as pipe strips are executed */
        int    ilMirrorheight;       /* src height value saved to avoid strip sizes  */
 }  ilMirrorPriv,  *ilMirrorPrivptr;



/* =============================================================================================================================
   ============================================================================================================================= */


static ilError ilMirrorInit(
    ilMirrorPrivptr  pPrivate,
    ilImageInfo     *pSrcImage,
    ilImageInfo     *pDstImage
    )

{
 unsigned char  *pdstline;
 ilImagePlaneInfo           *pplane;


   /* Initialize counters */

   pPrivate->illinecount =  1;
            
   /* Zero out destination memory for bitonal format handling */
   pplane   = &pDstImage->plane[0];
   pdstline =  (unsigned char *) (pplane->pPixels);

   bzero((char *)pdstline, (pplane->nBytesPerRow * pPrivate->ilMirrorheight) );

   return IL_OK;
}


/* =============================================================================================================================

      ilMirrorXBitonalExecute  -  Vertical mirroring for images with bit per pixel format .

   ============================================================================================================================= */
static ilError ilMirrorXBitonalExecute (
    ilExecuteData  *pData,
    unsigned long           dstLine,
    unsigned long          *pNLines
    )
{
                        

unsigned char     *psrc, *pdst, *psrcline, *pdstline;
ilImagePlaneInfo           *pplane;
int               x, y, lastcount, srcnbytes, dstnbytes;
ilMirrorPrivptr   pPriv;



   if (*pNLines <= 0)  return IL_OK;

   pplane                  =  &pData->pSrcImage->plane[0];
   srcnbytes               =  pplane->nBytesPerRow;
   psrcline                =  (unsigned char *) (pplane->pPixels) +  pData->srcLine * srcnbytes;

   pplane                  =  &pData->pDstImage->plane[0];
   dstnbytes               =  pplane->nBytesPerRow;
   pdstline                =  (unsigned char *) (pplane->pPixels)  +  dstLine * dstnbytes;
   pPriv                   =  (ilMirrorPrivptr) pData->pPrivate;



   /* Mirror vertically  map (x,y) to (x, -y) about its center  [ x,  height-y ]  */

   lastcount = pPriv->illinecount - 1;
   for (y = pPriv->illinecount;   y <= (lastcount + *pNLines);  y++, pPriv->illinecount++ )  {

      psrc = psrcline;
      pdst = pdstline + (dstnbytes * (pPriv->ilMirrorheight - y));
      for ( x = 0;   x < srcnbytes;  x++)  *pdst++  = *psrc++;
      psrcline += srcnbytes;

   }


   /* No. of lines written is the destination height */
   if  ( pPriv->illinecount <= pPriv->ilMirrorheight ) *pNLines = 0;
   else   *pNLines = pPriv->ilMirrorheight;
   
   return IL_OK;
}



/* =============================================================================================================================

      ilMirrorYBitonalExecute  -  Horizontal mirroring for images with bit per pixel format .

   ============================================================================================================================= */
static ilError ilMirrorYBitonalExecute (
    ilExecuteData  *pData,
    unsigned long           dstLine,
    unsigned long          *pNLines
    )
{
                        

unsigned char     *psrc, *pdst, *psrcline, *psrcbefore, *pdstline;
unsigned char     srcbyte;
ilImagePlaneInfo           *pplane;
int               x, y, lastcount, loffset, roffset;
int               srcnbytes, dstnbytes, widthbytes;
ilMirrorPrivptr   pPriv;


   if (*pNLines <= 0)  return IL_OK;

   pplane                  =  &pData->pSrcImage->plane[0];
   srcnbytes               =  pplane->nBytesPerRow;
   psrcline                =  (unsigned char *) (pplane->pPixels) +  pData->srcLine * srcnbytes;

   pplane                  =  &pData->pDstImage->plane[0];
   dstnbytes               =  pplane->nBytesPerRow;
   pdstline                =  (unsigned char *) (pplane->pPixels)  +  dstLine * dstnbytes;
   pPriv                   =  (ilMirrorPrivptr) pData->pPrivate;


   loffset    = pData->pSrcImage->width % BYTESIZE;
   roffset    = BYTESIZE - loffset;
   widthbytes = pData->pSrcImage->width / BYTESIZE;
   if ( loffset > 0 ) widthbytes++;


   /* Mirror horizontally  map (x,y) to (-x, y) about its center  [ width-x,  y ]  */

   lastcount = pPriv->illinecount - 1;
   for (y = lastcount;   y < (lastcount + *pNLines);  y++, pPriv->illinecount++ )  {

      psrc       = psrcline + widthbytes - 1;
      psrcbefore = psrc - 1;

      pdst = pdstline + (dstnbytes * y);

      if ( loffset == 0 ) 
           for ( x = 0;  x < widthbytes;  x++)   *pdst++ = ilBitReverseTable[ *psrc-- ];
      else  { 
           for ( x = 0;   x < widthbytes;  x++) {

                   if ( psrcbefore < psrcline )    srcbyte = (*psrc >> roffset);
                   else       srcbyte = (*psrcbefore << loffset) | (*psrc >> roffset);

                   *pdst++  =  ilBitReverseTable[srcbyte];
                   psrc--;
                   psrcbefore--;
           }
      }

      psrcline += srcnbytes;
   }


   /* No. of lines written is the destination height */
   if  ( pPriv->illinecount <= pPriv->ilMirrorheight ) *pNLines = 0;
   else   *pNLines = pPriv->ilMirrorheight;
   
   return IL_OK;
}

/* =============================================================================================================================

      ilMirrorX3ByteExecute  -  Vertical mirroring for images with 24 bits per pixel format .

   ============================================================================================================================= */
static ilError ilMirrorX3ByteExecute (
    ilExecuteData   *pData,
    unsigned long           dstLine,
    unsigned long          *pNLines
    )
{


unsigned char     *psrc, *pdst, *psrcline, *pdstline;
unsigned long     srcnbytes, dstnbytes;
ilImagePlaneInfo           *pplane;
int               x, y, lastcount;
ilMirrorPrivptr   pPriv;


   if (*pNLines <= 0)  return IL_OK;

   pplane                  =  &pData->pSrcImage->plane[0];
   srcnbytes               =  pplane->nBytesPerRow;
   psrcline                =  (unsigned char *) (pplane->pPixels) +  pData->srcLine * srcnbytes;

   pplane                  =  &pData->pDstImage->plane[0];
   dstnbytes               =  pplane->nBytesPerRow;
   pdstline                =  (unsigned char *) (pplane->pPixels)  +  dstLine * dstnbytes;
   pPriv                   =  (ilMirrorPrivptr) pData->pPrivate;



   /* Mirror vertically  map (x,y) to (x, -y) about its center  [ x,  height-y ]  */

   lastcount = pPriv->illinecount - 1;
   for (y = pPriv->illinecount;   y <= (lastcount + *pNLines);  y++, pPriv->illinecount++ )  {

      psrc = psrcline;

      pdst = pdstline + (dstnbytes * (pPriv->ilMirrorheight - y));
      for ( x = 0;   x < pData->pSrcImage->width;  x++) {
           *pdst++  =  *psrc++;
           *pdst++  =  *psrc++;
           *pdst++  =  *psrc++;
      }
      psrcline += srcnbytes;

   }


   /* No. of lines written is the destination height */
   if  ( pPriv->illinecount <= pPriv->ilMirrorheight ) *pNLines = 0;
   else   *pNLines = pPriv->ilMirrorheight;
   
   return IL_OK;
}


/* =============================================================================================================================

      ilMirrorY3ByteExecute  -  Horizontal mirroring for images with 24 bits per pixel format .

   ============================================================================================================================= */
static ilError ilMirrorY3ByteExecute (
    ilExecuteData   *pData,
    unsigned long           dstLine,
    unsigned long          *pNLines
    )
{


unsigned char     *psrc, *pdst, *psrcline, *pdstline;
unsigned long     srcnbytes, dstnbytes;
ilImagePlaneInfo           *pplane;
int               x, y, xoffset, lastcount;
ilMirrorPrivptr   pPriv;


   if (*pNLines <= 0)  return IL_OK;

   pplane                  =  &pData->pSrcImage->plane[0];
   srcnbytes               =  pplane->nBytesPerRow;
   psrcline                =  (unsigned char *) (pplane->pPixels) +  pData->srcLine * srcnbytes;

   pplane                  =  &pData->pDstImage->plane[0];
   dstnbytes               =  pplane->nBytesPerRow;
   pdstline                =  (unsigned char *) (pplane->pPixels)  +  dstLine * dstnbytes;
   pPriv                   =  (ilMirrorPrivptr) pData->pPrivate;



   /* Mirror horizontally  map (x,y) to (-x, y) about its center  [ width-x,  y ]  */

   xoffset = pData->pSrcImage->width * 3;

   lastcount = pPriv->illinecount - 1;
   for (y = lastcount;   y < (lastcount + *pNLines);  y++, pPriv->illinecount++ )  {

      psrc = psrcline + 2;

      pdst = pdstline + (dstnbytes * y) + xoffset - 1;
      for ( x = 0;   x < pData->pSrcImage->width;  x++,  psrc += 5) {
                *pdst--  =  *psrc--;
                *pdst--  =  *psrc--;
                *pdst--  =  *psrc;
      }
      psrcline += srcnbytes;
      
   }


   /* No. of lines written is the destination height */
   if  ( pPriv->illinecount <= pPriv->ilMirrorheight ) *pNLines = 0;
   else   *pNLines = pPriv->ilMirrorheight;
   
   return IL_OK;
}


/* =============================================================================================================================

      ilMirrorXByteExecute  -  Vertical mirroring for images with byte per pixel format .

   ============================================================================================================================= */
static ilError ilMirrorXByteExecute (
    ilExecuteData   *pData,
    unsigned long           dstLine,
    unsigned long          *pNLines
    )
{


unsigned char     *psrc, *pdst, *psrcline, *pdstline;
unsigned long     srcnbytes, dstnbytes;
ilImagePlaneInfo           *pplane;
int               x, y, lastcount;
ilMirrorPrivptr   pPriv;


   if (*pNLines <= 0)  return IL_OK;

   pplane                  =  &pData->pSrcImage->plane[0];
   srcnbytes               =  pplane->nBytesPerRow;
   psrcline                =  (unsigned char *) (pplane->pPixels) +  pData->srcLine * srcnbytes;

   pplane                  =  &pData->pDstImage->plane[0];
   dstnbytes               =  pplane->nBytesPerRow;
   pdstline                =  (unsigned char *) (pplane->pPixels)  +  dstLine * dstnbytes;
   pPriv                   =  (ilMirrorPrivptr) pData->pPrivate;




   /* Mirror vertically  map (x,y) to (x, -y) about its center  [ x,  height-y ]  */

   lastcount = pPriv->illinecount - 1;
   for (y = pPriv->illinecount;   y <= (lastcount + *pNLines);  y++, pPriv->illinecount++ )  {

      psrc = psrcline;
      pdst = pdstline + (dstnbytes * (pPriv->ilMirrorheight - y));
      for ( x = 0;   x < pData->pSrcImage->width;  x++) *pdst++  =  *psrc++;
      psrcline += srcnbytes;

   }


   /* No. of lines written is the destination height */
   if  ( pPriv->illinecount <= pPriv->ilMirrorheight ) *pNLines = 0;
   else   *pNLines = pPriv->ilMirrorheight;
   
   return IL_OK;
}                       


/* =============================================================================================================================

      ilMirrorYByteExecute  -  Horizontal mirroring for images with byte per pixel format .

   ============================================================================================================================= */
static ilError ilMirrorYByteExecute (
    ilExecuteData   *pData,
    unsigned long           dstLine,
    unsigned long          *pNLines
    )
{


unsigned char     *psrc, *pdst, *psrcline, *pdstline;
unsigned long     srcnbytes, dstnbytes;
ilImagePlaneInfo           *pplane;
int               x, y, lastcount;
ilMirrorPrivptr   pPriv;


   if (*pNLines <= 0)  return IL_OK;

   pplane                  =  &pData->pSrcImage->plane[0];
   srcnbytes               =  pplane->nBytesPerRow;
   psrcline                =  (unsigned char *) (pplane->pPixels) +  pData->srcLine * srcnbytes;

   pplane                  =  &pData->pDstImage->plane[0];
   dstnbytes               =  pplane->nBytesPerRow;
   pdstline                =  (unsigned char *) (pplane->pPixels)  +  dstLine * dstnbytes;
   pPriv                   =  (ilMirrorPrivptr) pData->pPrivate;




   /* Mirror horizontally  map (x,y) to (-x, y) about its center  [ width-x,  y ]  */

   lastcount = pPriv->illinecount - 1;
   for (y = lastcount;   y < (lastcount + *pNLines);  y++, pPriv->illinecount++ )  {

      psrc = psrcline;
      pdst = pdstline + (dstnbytes * y) + srcnbytes - 1;
      for ( x = 0;   x < pData->pSrcImage->width;  x++) *pdst--  =  *psrc++;
      psrcline += srcnbytes;

   }


   /* No. of lines written is the destination height */
   if  ( pPriv->illinecount <= pPriv->ilMirrorheight ) *pNLines = 0;
   else   *pNLines = pPriv->ilMirrorheight;
   
   return IL_OK;
}

/* =============================================================================================================================

      ilMirror -  Add a mirror filter to an existing pipe  - checking for format types 
                  and doing an explicit conversion if necessary.  Direction parameter
                  specifies verticle (MIRRORX) or horizontal (MIRRORY).
                  

   ============================================================================================================================= */

ilBool ilMirror (
    ilPipe              pipe,
    int                 direction
    )

{
unsigned int              state;
ilPipeInfo                info;
ilMirrorPrivptr  pPriv;
ilDstElementData          dstdata;
ilImageDes                imdes;
ilImageFormat             imformat;
ilBool                    convert;
ilBool                    bitonal;
ilBool                    cw;
unsigned int              rtype;


       /* Get ptr to pipe info and check state */

       state = ilGetPipeInfo(pipe, TRUE, &info, &imdes, &imformat);
       if(state != IL_PIPE_FORMING) {
         if (!pipe->context->error)
             ilDeclarePipeInvalid(pipe, IL_ERROR_PIPE_STATE);
         return FALSE;
       }


       bitonal = FALSE;

       /* Check for valid Formats */
       convert = FALSE;
       switch (imdes.nSamplesPerPixel)  {
            case  3: /* RGB or YUV */  
                     if(imformat.sampleOrder != IL_SAMPLE_PIXELS)  { 
                           imformat.sampleOrder = IL_SAMPLE_PIXELS;
                           convert = TRUE;
                     }

                     if((imformat.nBitsPerSample[0] != 8) ||
                        (imformat.nBitsPerSample[1] != 8) ||
                        (imformat.nBitsPerSample[2] != 8))  {

                          imformat.nBitsPerSample[0] = 8;
                          imformat.nBitsPerSample[1] = 8;
                          imformat.nBitsPerSample[2] = 8;
                          convert = TRUE;
                     }
                     break;

            case  1:
                     switch (imformat.nBitsPerSample[0]) {
                             case 8:  /* Byte per pixel */
                                      break;

                             case 1:  /* Bitonal */
                                      bitonal = TRUE; 
                                      if (imformat.rowBitAlign != 32) {
                                         imformat.rowBitAlign = 32;
                                         convert = TRUE;
                                      }
                                      break;

                             default: /* something other than 1 - try 8 */
                                      imformat.nBitsPerSample[0] = 8;
                                      convert = TRUE;
                     }  
                     break;

            default:
                      return ilDeclarePipeInvalid(pipe, IL_ERROR_NOT_IMPLEMENTED);
       }


       if(convert) {
            if (!ilConvert(pipe, &imdes, &imformat, 0, NULL))
                return FALSE;
            ilGetPipeInfo (pipe, FALSE, &info, (ilImageDes *)NULL, (ilImageFormat *)NULL);
       }
         

       /* if invalid direction flag as an error */
       if ((direction != IL_MIRRORX) && (direction != IL_MIRRORY))
            return ilDeclarePipeInvalid(pipe, IL_ERROR_INVALID_OPTION);

       dstdata.producerObject = (ilObject) NULL;
       dstdata.pDes           = (ilImageDes *) NULL;
       dstdata.pFormat        = (ilImageFormat *) NULL;
       dstdata.pPalette       = info.pPalette;

       dstdata.width          = info.width;
       dstdata.height         = info.height;

       /* set output strip height */
       dstdata.stripHeight    = dstdata.height;
       dstdata.constantStrip  = TRUE;


       switch (imdes.nSamplesPerPixel) {
            case 3:            
                      switch (direction) {
                        case IL_MIRRORX:
                               pPriv = (ilMirrorPrivptr) ilAddPipeElement(pipe, IL_FILTER, sizeof(ilMirrorPriv), 0, (ilSrcElementData *) NULL,
                                                                          &dstdata, ilMirrorInit, IL_NPF, IL_NPF, ilMirrorX3ByteExecute, 0);
                               break;
                        case IL_MIRRORY:
                               pPriv = (ilMirrorPrivptr) ilAddPipeElement(pipe, IL_FILTER, sizeof(ilMirrorPriv), 0, (ilSrcElementData *) NULL,
                                                                          &dstdata, ilMirrorInit, IL_NPF, IL_NPF, ilMirrorY3ByteExecute, 0);
                      }
                      break;

            case 1:  
                 if(bitonal) {
                      switch (direction) {
                        case IL_MIRRORX:
                               pPriv = (ilMirrorPrivptr) ilAddPipeElement(pipe, IL_FILTER, sizeof(ilMirrorPriv), 0, (ilSrcElementData *) NULL,
                                                                          &dstdata, ilMirrorInit, IL_NPF, IL_NPF, ilMirrorXBitonalExecute, 0);
                               break;
                        case IL_MIRRORY:
                               pPriv = (ilMirrorPrivptr) ilAddPipeElement(pipe, IL_FILTER, sizeof(ilMirrorPriv), 0, (ilSrcElementData *) NULL,
                                                                          &dstdata, ilMirrorInit, IL_NPF, IL_NPF, ilMirrorYBitonalExecute, 0);
                      }
                 }
                 else {
                      switch (direction) {
                        case IL_MIRRORX:
                               pPriv = (ilMirrorPrivptr) ilAddPipeElement(pipe, IL_FILTER, sizeof(ilMirrorPriv), 0, (ilSrcElementData *) NULL,
                                                                          &dstdata, ilMirrorInit, IL_NPF, IL_NPF, ilMirrorXByteExecute, 0);
                               break;
                        case IL_MIRRORY:
                               pPriv = (ilMirrorPrivptr) ilAddPipeElement(pipe, IL_FILTER, sizeof(ilMirrorPriv), 0, (ilSrcElementData *) NULL,
                                                                          &dstdata, ilMirrorInit, IL_NPF, IL_NPF, ilMirrorYByteExecute, 0);
                      }
                 }

       }

       if(!pPriv)
            return FALSE;

       /* Save away true height */
       pPriv->ilMirrorheight = info.height;   

       pipe->context->error = IL_OK;
       return TRUE;
}


