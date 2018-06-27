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
/* $XConsortium: ilrotate.c /main/4 1996/01/08 12:16:56 lehors $ */
/* =============================================================================================================================

     /ilc/ilrotate.c : Images Library rotation routines.   



        Date        Mail-ID     Description
        --------    ----------- -------------------------------------------------------------------
        07/2/91    larsson     Initial Coding.
        11/4/91    voegelin    moved ilBitReverseTable to ildata.c

   ============================================================================================================================= */

#include "ilint.h"
#include "ilpipelem.h"
#include "ilerrors.h"


#define LONGSZ        4
#define WORDPOS       32
#define BYTESIZE      8
#define LEFT_BIT_ON   0x80000000
#define ROT90         1 
#define ROT180        2
#define ROT270        3



 typedef struct {
        int            illinecount;           /* running line count as pipe strips are executed */
        int            ilXCenter;             /* center of image width  */
        int            ilYCenter;             /* center of image height */
        int            ilRotateDstheight;     /* destination height value saved to avoid strip sizes  */
        int            ilRotateSrcheight;     /* src height value saved to avoid strip sizes  */
        unsigned long  dstMask;               /* destination bit mask for 90 degree rotations (bitonal) */
        int            rtype;                 /* rotation type used for setting dstMask */
        ilBool         Lastbit;               /* Flag indicating leftmost bit of current word is written */
 }  ilRotatePriv,  *ilRotatePrivptr;



/* =============================================================================================================================
   ============================================================================================================================= */


static ilError ilRotateInit(
    ilRotatePrivptr pPrivate,
    ilImageInfo     *pSrcImage,
    ilImageInfo     *pDstImage
    )

{
 unsigned char  *pdstline;
 ilImagePlaneInfo        *pplane;
 int                     bitoff;


   /* Initialize counters */

   pPrivate->illinecount =  1;
   pPrivate->ilXCenter   =  pSrcImage->width/2;
   pPrivate->ilYCenter   =  pPrivate->ilRotateSrcheight/2;
            

   /* Zero out destination memory for bitonal format handling */

   pplane   = &pDstImage->plane[0];
   pdstline =  (unsigned char *) (pplane->pPixels);

   bitoff  = (pplane->nBytesPerRow * 8) - pDstImage->width;

   if ( pPrivate->rtype == ROT90)  pPrivate->dstMask = 1 << bitoff;
   else pPrivate->dstMask = LEFT_BIT_ON;

   if ( (long) pPrivate->dstMask < 0 ) pPrivate->Lastbit = TRUE; 
   else  pPrivate->Lastbit = FALSE;

   bzero((char *)pdstline,
	 (pplane->nBytesPerRow * pPrivate->ilRotateDstheight));

   return IL_OK;
}






/* =============================================================================================================================

      ilRotate90BitonalExecute  -  90 degree cw Rotation for images with bit per pixel format .

   ============================================================================================================================= */
static ilError ilRotate90BitonalExecute (
    ilExecuteData  *pData,
    unsigned long           dstLine,
    unsigned long          *pNLines
    )
{
   

unsigned long     *psrc, *pdst, *psrcline, *pdstline;
unsigned long     srcnwords, dstnwords;
ilImagePlaneInfo           *pplane;
int               x, y, xoffset, lastcount;
ilRotatePrivptr   pPriv;

unsigned long     srcMask;
unsigned long     srcLong;
ilBool            shortwidth;


   if (*pNLines <= 0)  return IL_OK;

   pplane                  =  &pData->pSrcImage->plane[0];
   srcnwords               =  (pplane->nBytesPerRow + LONGSZ - 1)/LONGSZ;
   psrcline                =  (unsigned long *) (pplane->pPixels) +  pData->srcLine * srcnwords;

   pplane                  =  &pData->pDstImage->plane[0];
   dstnwords               =  (pplane->nBytesPerRow + LONGSZ - 1)/LONGSZ;
   pdstline                =  (unsigned long *) (pplane->pPixels)  +  dstLine * dstnwords;
   pPriv                   =  (ilRotatePrivptr) pData->pPrivate;


   /* Rotate cw 90 degrees  map (x,y) to (-y,x) about its center  [ width-y,  x ]  */
   psrc = psrcline;
   
   srcLong = *psrc++;
   srcMask = LEFT_BIT_ON;

   shortwidth = (((srcnwords * WORDPOS) - pData->pSrcImage->width) > 0) ? TRUE : FALSE;

   lastcount = pPriv->illinecount - 1;
   for (y = pPriv->illinecount;   y <= (lastcount + *pNLines);  y++, pPriv->illinecount++ )  {

        xoffset = (pData->pDstImage->width - y)/WORDPOS;

        for ( x = 0;  x < pData->pSrcImage->width;  x++) {

            if ( srcLong & srcMask ) {  /* Copy bit */

                 pdst = pdstline + (dstnwords * x) + xoffset;
                 *pdst = *pdst | pPriv->dstMask;
            }

            srcMask >>= 1;     /* Shift to next pixel */
            if ( !srcMask ) {
                srcLong = *psrc++;
                srcMask = LEFT_BIT_ON;
            }

        } 

        if ( !pPriv->Lastbit )  pPriv->dstMask <<= 1;
        else  {
           pPriv->dstMask = 1;
           pPriv->Lastbit = FALSE;
        }

        if ( (long) pPriv->dstMask < 0 ) pPriv->Lastbit = TRUE; 

        if(shortwidth) {
                srcLong = *psrc++;
                srcMask = LEFT_BIT_ON;
        }

   }            

   /* No. of lines written is the destination height */
   if  ( pPriv->illinecount <= pPriv->ilRotateSrcheight ) *pNLines = 0;
   else   *pNLines = pPriv->ilRotateDstheight;
   
   return IL_OK;
}


/* =============================================================================================================================

      ilRotate180BitonalExecute  -  180 degree Rotation for images with bit per pixel format .

   ============================================================================================================================= */
static ilError ilRotate180BitonalExecute (
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
ilRotatePrivptr   pPriv;


   if (*pNLines <= 0)  return IL_OK;

   pplane                  =  &pData->pSrcImage->plane[0];
   srcnbytes               =  pplane->nBytesPerRow;
   psrcline                =  (unsigned char *) (pplane->pPixels) +  pData->srcLine * srcnbytes;

   pplane                  =  &pData->pDstImage->plane[0];
   dstnbytes               =  pplane->nBytesPerRow;
   pdstline                =  (unsigned char *) (pplane->pPixels)  +  dstLine * dstnbytes;
   pPriv                   =  (ilRotatePrivptr) pData->pPrivate;

   loffset    = pData->pSrcImage->width % BYTESIZE;
   roffset    = BYTESIZE - loffset;
   widthbytes = pData->pSrcImage->width / BYTESIZE;
   if ( loffset > 0 ) widthbytes++;


   /* Rotate 180 degrees  map (x,y) to (-x,-y)  about its center [ width-x,  height-y ]  */

   lastcount = pPriv->illinecount - 1;
   for (y = pPriv->illinecount;   y <= (lastcount + *pNLines);  y++, pPriv->illinecount++ )  {

      psrc       = psrcline + widthbytes - 1;
      psrcbefore = psrc - 1;

      pdst = pdstline + (dstnbytes * (pPriv->ilRotateSrcheight - y));

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
   if  ( pPriv->illinecount <= pPriv->ilRotateSrcheight ) *pNLines = 0;
   else   *pNLines = pPriv->ilRotateDstheight;
   
   return IL_OK;
}


/* =============================================================================================================================

      ilRotate270BitonalExecute  -  270 degree cw Rotation for images with bit per pixel format .

   ============================================================================================================================= */
static ilError ilRotate270BitonalExecute (
    ilExecuteData  *pData,
    unsigned long           dstLine,
    unsigned long          *pNLines
    )
{


unsigned long     *psrc, *pdst, *psrcline, *pdstline;
unsigned long     srcnwords, dstnwords;
ilImagePlaneInfo           *pplane;
int               x, y, xoffset, lastcount;
ilRotatePrivptr   pPriv;

unsigned long     srcMask, dstMask;
unsigned long     srcLong, dstLong;
ilBool            shortwidth;



   if (*pNLines <= 0)  return IL_OK;

   pplane                  =  &pData->pSrcImage->plane[0];
   srcnwords               =  (pplane->nBytesPerRow + LONGSZ - 1)/LONGSZ;
   psrcline                =  (unsigned long *) (pplane->pPixels) +  pData->srcLine * srcnwords;

   pplane                  =  &pData->pDstImage->plane[0];
   dstnwords               =  (pplane->nBytesPerRow + LONGSZ - 1)/LONGSZ;
   pdstline                =  (unsigned long *) (pplane->pPixels)  +  dstLine * dstnwords;
   pPriv                   =  (ilRotatePrivptr) pData->pPrivate;



   /* Rotate cw 270 degrees  map (x,y) to (y,-x) about its center  [ y,  height-x ]  */
   psrc = psrcline;
   
   srcLong = *psrc++;
   srcMask = LEFT_BIT_ON;

   shortwidth = (((srcnwords * WORDPOS) - pData->pSrcImage->width) > 0) ? TRUE : FALSE;

   lastcount = pPriv->illinecount - 1;
   for (y = pPriv->illinecount;   y <= (lastcount + *pNLines);  y++, pPriv->illinecount++ )  {

        xoffset = (y - 1)/WORDPOS;

        for ( x = 1;  x <= pData->pSrcImage->width; x++) {

            if ( srcLong & srcMask ) {  /* perform copy */

                 pdst = pdstline + (dstnwords * (pPriv->ilRotateDstheight - x)) + xoffset;
                 *pdst = *pdst | pPriv->dstMask;
            }

            /* shift to next pixel */
            srcMask >>= 1;
            if ( !srcMask ) {
                srcLong = *psrc++;
                srcMask = LEFT_BIT_ON;
            }

        } 

        pPriv->dstMask >>= 1;
        if( !pPriv->dstMask ) pPriv->dstMask = LEFT_BIT_ON;

        if(shortwidth) {
                srcLong = *psrc++;
                srcMask = LEFT_BIT_ON;
        }


   }            

   /* No. of lines written is the destination height */
   if  ( pPriv->illinecount <= pPriv->ilRotateSrcheight ) *pNLines = 0;
   else   *pNLines = pPriv->ilRotateDstheight;
   
   return IL_OK;
}




/* =============================================================================================================================

      ilRotate903ByteExecute  -  90 degree cw Rotation for images with 24 bits per pixel format .

   ============================================================================================================================= */
static ilError ilRotate903ByteExecute (
    ilExecuteData   *pData,
    unsigned long           dstLine,
    unsigned long          *pNLines
    )
{
unsigned char     *psrc, *pdst, *psrcline, *pdstline;
unsigned long     srcnbytes, dstnbytes;
ilImagePlaneInfo           *pplane;
int               x, y, xoffset, lastcount;
ilRotatePrivptr   pPriv;


   if (*pNLines <= 0)  return IL_OK;

   pplane                  =  &pData->pSrcImage->plane[0];
   srcnbytes               =  pplane->nBytesPerRow;
   psrcline                =  (unsigned char *) (pplane->pPixels) +  pData->srcLine * srcnbytes;

   pplane                  =  &pData->pDstImage->plane[0];
   dstnbytes               =  pplane->nBytesPerRow;
   pdstline                =  (unsigned char *) (pplane->pPixels)  +  dstLine * dstnbytes;
   pPriv                   =  (ilRotatePrivptr) pData->pPrivate;



   /* Rotate cw 90 degrees  map (x,y) to (-y,x) about its center  [ width-y,  x ]  */

   lastcount = pPriv->illinecount - 1;
   for (y = pPriv->illinecount;   y <= (lastcount + *pNLines);  y++, pPriv->illinecount++ )  {

      psrc = psrcline;
      xoffset =  (pData->pDstImage->width - y) * 3;

      for ( x = 0;   x < pData->pSrcImage->width;  x++) {
         pdst = pdstline + (dstnbytes * x) + xoffset;
         *pdst++ = *psrc++;
         *pdst++ = *psrc++;
         *pdst   = *psrc++;
      } 

      psrcline += srcnbytes;
   }


   /* No. of lines written is the destination height */
   if  ( pPriv->illinecount <= pPriv->ilRotateSrcheight ) *pNLines = 0;
   else   *pNLines = pPriv->ilRotateDstheight;
   
   return IL_OK;

}



/* =============================================================================================================================

      ilRotate1803ByteExecute  -  180 degree Rotation for images with 24 bits per pixel format .

   ============================================================================================================================= */
static ilError ilRotate1803ByteExecute (
    ilExecuteData   *pData,
    unsigned long           dstLine,
    unsigned long          *pNLines
    )
{


unsigned char     *psrc, *pdst, *psrcline, *pdstline;
unsigned long     srcnbytes, dstnbytes;
ilImagePlaneInfo           *pplane;
int               x, y, xoffset, lastcount;
ilRotatePrivptr   pPriv;


   if (*pNLines <= 0)  return IL_OK;

   pplane                  =  &pData->pSrcImage->plane[0];
   srcnbytes               =  pplane->nBytesPerRow;
   psrcline                =  (unsigned char *) (pplane->pPixels) +  pData->srcLine * srcnbytes;

   pplane                  =  &pData->pDstImage->plane[0];
   dstnbytes               =  pplane->nBytesPerRow;
   pdstline                =  (unsigned char *) (pplane->pPixels)  +  dstLine * dstnbytes;
   pPriv                   =  (ilRotatePrivptr) pData->pPrivate;



   /* Rotate 180 degrees  map (x,y) to (-x,-y)  about its center [ width-x,  height-y ]  */
 
   xoffset = pData->pSrcImage->width * 3;

   lastcount = pPriv->illinecount - 1;
   for (y = pPriv->illinecount;   y <= (lastcount + *pNLines);  y++, pPriv->illinecount++ )  {

      psrc = psrcline + 2;
      pdst = pdstline + (dstnbytes * (pPriv->ilRotateSrcheight - y)) + xoffset - 1;
      for ( x = 0;   x < pData->pSrcImage->width;  x++,  psrc += 5) {
                    *pdst--  =  *psrc--;
                    *pdst--  =  *psrc--;
                    *pdst--  =  *psrc;
      }
      psrcline += srcnbytes;

   }


   /* No. of lines written is the destination height */
   if  ( pPriv->illinecount <= pPriv->ilRotateSrcheight ) *pNLines = 0;
   else   *pNLines = pPriv->ilRotateDstheight;
   
   return IL_OK;
}



/* =============================================================================================================================

      ilRotate2703ByteExecute  -  270 degree cw Rotation for images with 24 bits per pixel format .

   ============================================================================================================================= */
static ilError ilRotate2703ByteExecute (
    ilExecuteData   *pData,
    unsigned long           dstLine,
    unsigned long          *pNLines
    )
{



unsigned char     *psrc, *pdst, *psrcline, *pdstline;
unsigned long     srcnbytes, dstnbytes;
ilImagePlaneInfo           *pplane;
int               x, y, lastcount, xoffset;
ilRotatePrivptr   pPriv;


   if (*pNLines <= 0)  return IL_OK;

   pplane                  =  &pData->pSrcImage->plane[0];
   srcnbytes               =  pplane->nBytesPerRow;
   psrcline                =  (unsigned char *) (pplane->pPixels) +  pData->srcLine * srcnbytes;

   pplane                  =  &pData->pDstImage->plane[0];
   dstnbytes               =  pplane->nBytesPerRow;
   pdstline                =  (unsigned char *) (pplane->pPixels)  +  dstLine * dstnbytes;
   pPriv                   =  (ilRotatePrivptr) pData->pPrivate;


   /* Rotate cw 270 degrees  map (x,y) to (y,-x) about its center  [ y,  height-x ]  */

   lastcount = pPriv->illinecount - 1;
   for (y = pPriv->illinecount;   y <= (lastcount + *pNLines);  y++, pPriv->illinecount++ )  {

      psrc = psrcline;
      xoffset = 3 * (y-1);

      for ( x = 1;   x <= pData->pSrcImage->width;  x++) {
         pdst = pdstline + (dstnbytes * (pPriv->ilRotateDstheight - x)) + xoffset;
         *pdst++ = *psrc++;
         *pdst++ = *psrc++;
         *pdst   = *psrc++;
      } 

      psrcline += srcnbytes;
   }

   /* No. of lines written is the destination height */
   if  ( pPriv->illinecount <= pPriv->ilRotateSrcheight ) *pNLines = 0;
   else   *pNLines = pPriv->ilRotateDstheight;
   
   return IL_OK;
}

/* =============================================================================================================================

      ilRotate90ByteExecute  -  90 degree cw Rotation for images with byte per pixel format .

   ============================================================================================================================= */
static ilError ilRotate90ByteExecute (
    ilExecuteData   *pData,
    unsigned long           dstLine,
    unsigned long          *pNLines
    )
{



unsigned char     *psrc, *pdst, *psrcline, *pdstline;
unsigned long     srcnbytes, dstnbytes;
ilImagePlaneInfo           *pplane;
int               x, y, xoffset, lastcount;
unsigned int               dstrowpixels;
ilRotatePrivptr   pPriv;


   if (*pNLines <= 0)  return IL_OK;

   pplane                  =  &pData->pSrcImage->plane[0];
   srcnbytes               =  pplane->nBytesPerRow;
   psrcline                =  (unsigned char *) (pplane->pPixels) +  pData->srcLine * srcnbytes;

   pplane                  =  &pData->pDstImage->plane[0];
   dstnbytes               =  pplane->nBytesPerRow;
   pdstline                =  (unsigned char *) (pplane->pPixels)  +  dstLine * dstnbytes;
   pPriv                   =  (ilRotatePrivptr) pData->pPrivate;


   /* Rotate cw 90 degrees  map (x,y) to (-y,x) about its center  [ width-y, x ]  */

   lastcount = pPriv->illinecount - 1;
   for (y = pPriv->illinecount;   y <= (lastcount + *pNLines);  y++, pPriv->illinecount++ )  {

      psrc = psrcline;
      xoffset =  pData->pDstImage->width - y;

      for ( x = 0;   x < pData->pSrcImage->width;  x++) {
         pdst = pdstline + (dstnbytes * x) + xoffset;
         *pdst = *psrc++;
      } 

      psrcline += srcnbytes;
   }

   /* No. of lines written is the destination height */
   if  ( pPriv->illinecount <= pPriv->ilRotateSrcheight ) *pNLines = 0;
   else   *pNLines = pPriv->ilRotateDstheight;
   
   return IL_OK;
}

/* =============================================================================================================================

      ilRotate180ByteExecute  -  180 degree Rotation for images with byte per pixel format .

   ============================================================================================================================= */
static ilError ilRotate180ByteExecute (
    ilExecuteData   *pData,
    unsigned long           dstLine,
    unsigned long          *pNLines
    )
{


unsigned char     *psrc, *pdst, *psrcline, *pdstline;
unsigned long     srcnbytes, dstnbytes;
ilImagePlaneInfo           *pplane;
int               x, y, lastcount;
ilRotatePrivptr   pPriv;


   if (*pNLines <= 0)  return IL_OK;

   pplane                  =  &pData->pSrcImage->plane[0];
   srcnbytes               =  pplane->nBytesPerRow;
   psrcline                =  (unsigned char *) (pplane->pPixels) +  pData->srcLine * srcnbytes;

   pplane                  =  &pData->pDstImage->plane[0];
   dstnbytes               =  pplane->nBytesPerRow;
   pdstline                =  (unsigned char *) (pplane->pPixels)  +  dstLine * dstnbytes;
   pPriv                   =  (ilRotatePrivptr) pData->pPrivate;




   /* Rotate 180 degrees  map (x,y) to (-x,-y) about its center  [ width-x,  height-y ]  */

   lastcount = pPriv->illinecount - 1;
   for (y = pPriv->illinecount;   y <= (lastcount + *pNLines);  y++, pPriv->illinecount++ )  {

      psrc = psrcline;
      pdst = pdstline + (dstnbytes * (pPriv->ilRotateSrcheight - y)) + srcnbytes - 1;
      for ( x = 0;   x < pData->pSrcImage->width;  x++) *pdst--  =  *psrc++;
      psrcline += srcnbytes;
   }


   /* No. of lines written is the destination height */
   if  ( pPriv->illinecount <= pPriv->ilRotateSrcheight ) *pNLines = 0;
   else   *pNLines = pPriv->ilRotateDstheight;
   
   return IL_OK;
}

/* =============================================================================================================================

      ilRotate270ByteExecute  -  270 degree cw Rotation for images with byte per pixel format .

   ============================================================================================================================= */
static ilError ilRotate270ByteExecute (
    ilExecuteData   *pData,
    unsigned long           dstLine,
    unsigned long          *pNLines
    )
{


unsigned char     *psrc, *pdst, *psrcline, *pdstline;
unsigned long     srcnbytes, dstnbytes;
ilImagePlaneInfo           *pplane;
int               x, y, lastcount, xoffset;
ilRotatePrivptr   pPriv;


   if (*pNLines <= 0)  return IL_OK;

   pplane                  =  &pData->pSrcImage->plane[0];
   srcnbytes               =  pplane->nBytesPerRow;
   psrcline                =  (unsigned char *) (pplane->pPixels) +  pData->srcLine * srcnbytes;

   pplane                  =  &pData->pDstImage->plane[0];
   dstnbytes               =  pplane->nBytesPerRow;
   pdstline                =  (unsigned char *) (pplane->pPixels)  +  dstLine * dstnbytes;
   pPriv                   =  (ilRotatePrivptr) pData->pPrivate;


   /* Rotate cw 270 degrees  map (x,y) to (y,-x) about its center  [ y,  height-x ]  */
   
   lastcount = pPriv->illinecount - 1;
   for (y = pPriv->illinecount;   y <= (lastcount + *pNLines);  y++, pPriv->illinecount++ )  {

      psrc = psrcline;

      for ( x = 1;   x <= pData->pSrcImage->width;  x++) {
         pdst = pdstline + (dstnbytes * (pPriv->ilRotateDstheight - x)) + (y-1);
         *pdst = *psrc++;
      } 

      psrcline += srcnbytes;
   }


   /* No. of lines written is the destination height */
   if  ( pPriv->illinecount <= pPriv->ilRotateSrcheight ) *pNLines = 0;
   else   *pNLines = pPriv->ilRotateDstheight;
   
   return IL_OK;
}



/* =============================================================================================================================

      ilRotate90   - Add a rotate filter to an existing pipe for 90 degree rotations - checking 
                  for format types and doing an explicit conversion if necessary. Positive factors
                  cause clockwise rotations - negative counter clockwise.

   ============================================================================================================================= */

ilBool ilRotate90 (
    ilPipe              pipe,
    int                 factor
    )

{
unsigned int              state;
ilPipeInfo                info;
ilRotatePrivptr  pPriv;
ilDstElementData          dstdata;
ilImageDes                imdes;
ilImageFormat             imformat;
ilBool                    convert;
ilBool                    bitonal;
ilBool                    cw;
unsigned int              rtype;
#define PIPE_FLAGS        

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
         

       /* Determine rotation type */
       cw = (factor > 0) ? TRUE : FALSE;
       switch ( abs(factor) % 4) {
            case 0:
                     pipe->context->error = IL_OK;     /* 0 Rotation - nothing required. */
                     return TRUE;
            case 1:
                     rtype = (cw) ? ROT90 : ROT270;
                     break;
            case 2:  
                     rtype = ROT180;
                     break;
            case 3:
                     rtype = (cw) ? ROT270 : ROT90;
       }


       dstdata.producerObject = (ilObject) NULL;
       dstdata.pDes           = (ilImageDes *) NULL;
       dstdata.pFormat        = (ilImageFormat *) NULL;
       dstdata.pPalette       = info.pPalette;

       /* flip-flop image if necessary */
       if(rtype != ROT180) {
          dstdata.width          = info.height;
          dstdata.height         = info.width;
       }
       else {
          dstdata.width          = info.width;
          dstdata.height         = info.height;
       }

       /* set output strip height */
       dstdata.stripHeight    = dstdata.height;
       dstdata.constantStrip  = TRUE;


       switch (imdes.nSamplesPerPixel) {
            case 3:
                     switch (rtype) {
                           case ROT90:
                               pPriv = (ilRotatePrivptr) ilAddPipeElement(pipe, IL_FILTER, sizeof(ilRotatePriv), IL_ADD_PIPE_HOLD_DST, (ilSrcElementData *) NULL,
                                                                          &dstdata, ilRotateInit, IL_NPF, IL_NPF, ilRotate903ByteExecute, 0);
                               break;
                           case ROT180:
                               pPriv = (ilRotatePrivptr) ilAddPipeElement(pipe, IL_FILTER, sizeof(ilRotatePriv), IL_ADD_PIPE_HOLD_DST, (ilSrcElementData *) NULL,
                                                                          &dstdata, ilRotateInit, IL_NPF, IL_NPF, ilRotate1803ByteExecute, 0);
                               break;
                           case ROT270:
                               pPriv = (ilRotatePrivptr) ilAddPipeElement(pipe, IL_FILTER, sizeof(ilRotatePriv), IL_ADD_PIPE_HOLD_DST, (ilSrcElementData *) NULL,
                                                                          &dstdata, ilRotateInit, IL_NPF, IL_NPF, ilRotate2703ByteExecute, 0);
                               break;
                     }
                     break;

            case 1:  
                     if(bitonal) {
                        switch (rtype) {
                           case ROT90:
                               pPriv = (ilRotatePrivptr) ilAddPipeElement(pipe, IL_FILTER, sizeof(ilRotatePriv), IL_ADD_PIPE_HOLD_DST, (ilSrcElementData *) NULL,
                                                                          &dstdata, ilRotateInit, IL_NPF, IL_NPF, ilRotate90BitonalExecute, 0);
                               break;
                           case ROT180:
                               pPriv = (ilRotatePrivptr) ilAddPipeElement(pipe, IL_FILTER, sizeof(ilRotatePriv), IL_ADD_PIPE_HOLD_DST, (ilSrcElementData *) NULL,
                                                                          &dstdata, ilRotateInit, IL_NPF, IL_NPF, ilRotate180BitonalExecute, 0);
                               break;
                           case ROT270:
                               pPriv = (ilRotatePrivptr) ilAddPipeElement(pipe, IL_FILTER, sizeof(ilRotatePriv), IL_ADD_PIPE_HOLD_DST, (ilSrcElementData *) NULL,
                                                                          &dstdata, ilRotateInit, IL_NPF, IL_NPF, ilRotate270BitonalExecute, 0);  
                               break;
                        }
                     }
                     else {
                        switch (rtype) {
                           case ROT90:
                               pPriv = (ilRotatePrivptr) ilAddPipeElement(pipe, IL_FILTER, sizeof(ilRotatePriv), IL_ADD_PIPE_HOLD_DST, (ilSrcElementData *) NULL,
                                                                          &dstdata, ilRotateInit, IL_NPF, IL_NPF, ilRotate90ByteExecute, 0);
                               break;
                           case ROT180:
                               pPriv = (ilRotatePrivptr) ilAddPipeElement(pipe, IL_FILTER, sizeof(ilRotatePriv), IL_ADD_PIPE_HOLD_DST, (ilSrcElementData *) NULL,
                                                                          &dstdata, ilRotateInit, IL_NPF, IL_NPF, ilRotate180ByteExecute, 0);
                               break;
                           case ROT270:
                               pPriv = (ilRotatePrivptr) ilAddPipeElement(pipe, IL_FILTER, sizeof(ilRotatePriv), IL_ADD_PIPE_HOLD_DST, (ilSrcElementData *) NULL,
                                                                          &dstdata, ilRotateInit, IL_NPF, IL_NPF, ilRotate270ByteExecute, 0);
                               break;
                        }
                     }

       }

       if(!pPriv)
            return FALSE;

       /* Save away true heights */
       pPriv->ilRotateDstheight  = dstdata.height;
       pPriv->ilRotateSrcheight  = info.height;   
       pPriv->rtype = rtype;

       pipe->context->error = IL_OK;
       return TRUE;
}


