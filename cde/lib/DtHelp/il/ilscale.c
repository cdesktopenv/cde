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
/* $XConsortium: ilscale.c /main/6 1996/09/24 17:13:24 drk $ */
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

#include "ilint.h"
#include "ilpipelem.h"
#include "ilscaleint.h"
#include "ilerrors.h"
#include "ilconvert.h"

#define LONGSZ        4
#ifdef LSB_BIT_ORDER
# define SPECIAL_MASK_BIT    0x00000001          /* for LSB bit order */
# define SHIFT_MASK(m)       ((m) <<= 1)
#else
# define SPECIAL_MASK_BIT    0x80000000          /* for MSB bit order */
# define SHIFT_MASK(m)       ((m) >>= 1)
#endif


 typedef struct {
        long   ilScaleAccDown;     /* accumulator value used in scale down algorithm */
        long   ilScaleAccUp;       /* accumulator value used in scale  up  algorithm */
        long   ilScaleDstheight;   /* destination height value saved to avoid strip sizes  */
        long   ilScaleSrcheight;   /* src height value saved to avoid strip sizes  */
 } ilScalePriv,  *ilScalePrivptr;

#ifdef LSB_BIT_ORDER
extern void flip_bits(register ilPtr start, register unsigned n);
#else
# define flip_bits(start,n)      /*EMPTY*/
#endif


/* =============================================================================================================================
        ilScaleInit  -  Initialization function for a scale filter performed with the 
                        method of replication/decimation.  This gets called when the pipe 
                        is executed for the first time.    
   ============================================================================================================================= */


static ilError ilScaleInit(
    ilScalePrivptr  pPrivate,
    ilImageInfo     *pSrcImage,
    ilImageInfo     *pDstImage
    )

{
   pPrivate->ilScaleAccDown = pPrivate->ilScaleSrcheight - pPrivate->ilScaleDstheight;
   pPrivate->ilScaleAccUp   = pPrivate->ilScaleDstheight - pPrivate->ilScaleSrcheight;

   return IL_OK;
}




/* =============================================================================================================================
        ilswbitdown  -  Routine to scale down the width of an image by decimation.
                        Operates on bitonal images.
   ============================================================================================================================= */

static void ilswbitdown(
    register CARD32 *psrcline,
    register CARD32 *pdstline,
    unsigned long   srcwidth,   
    unsigned long   dstwidth
    )

{
register int            acc, nbits;
register CARD32		srcMask;
register CARD32		srcLong;
CARD32			dstLong;
ilBool copy;
ilBool Lastbit;

  srcMask  =  SPECIAL_MASK_BIT;

  srcLong  =  *psrcline++;
  dstLong  =  1;
  Lastbit  =  FALSE;

  acc      =  srcwidth - dstwidth;
  copy     =  (acc == 0)  ?  TRUE  :  FALSE;

  nbits = dstwidth;

  if(copy == TRUE) {         /*  degenerate case optimized  */

       while (nbits-- ) {

            /* copy bit logic */
            dstLong <<= 1;
            if (srcMask & srcLong)  dstLong++;

            if(Lastbit) {  

                flip_bits((ilPtr)&dstLong, sizeof(dstLong));

                *pdstline++ = dstLong;
                dstLong = 1;
                Lastbit = FALSE;
            } 

   
            /* See if the last bit in the word is coming */
            if( (INT32) dstLong < 0) Lastbit = TRUE;

            SHIFT_MASK(srcMask);
            if (!srcMask) {  /* after 32 bits reset mask and get new srcline */ 
                srcMask = SPECIAL_MASK_BIT;
                srcLong = *psrcline++;   
            }

       }   /*  End While  */

  }
  else {       /*  scale down algorithm */


       while (nbits-- ) {     

            for(; acc > 0;  acc -= dstwidth) {   /*  skip bits according to scale ratio */

                 SHIFT_MASK(srcMask);
 
                 if (!srcMask) {  /* after 32 bits reset mask and get new srcline */ 
                     srcMask = SPECIAL_MASK_BIT;
                      srcLong = *psrcline++;   
                 }
            }
            acc += (srcwidth - dstwidth);


            /* copy bit logic */
            dstLong <<= 1;
            if (srcMask & srcLong)  dstLong++;

            if(Lastbit) {  

                flip_bits((ilPtr)&dstLong, sizeof(dstLong));

                *pdstline++ = dstLong;
                dstLong = 1;
                Lastbit = FALSE;
            } 


            /* See if the last bit in the word is coming */
            if( (INT32) dstLong < 0) Lastbit = TRUE;

            SHIFT_MASK(srcMask);
            if (!srcMask) {  /* after 32 bits reset mask and get new srcline */ 
                srcMask = SPECIAL_MASK_BIT;  
                srcLong = *psrcline++;   
            }


       }   /*  End While  */


  } 


  /* Shift up and write out any last bits */
  
  if (dstLong != 1) {      /* has data not copied to pdstline */
       while ( (INT32) dstLong > 0)  dstLong <<= 1;  /* shift up */
       dstLong <<= 1;
       flip_bits((ilPtr)&dstLong, sizeof(dstLong));
       *pdstline++ = dstLong;
  }
       
}

/* =============================================================================================================================
        ilswbitup   -  Routine to scale up the width of an image by replication.
                       Operates on bitonal images.
   ============================================================================================================================= */

static void ilswbitup(
    register CARD32 *psrcline,
    register CARD32 *pdstline,
    unsigned long   srcwidth,   
    unsigned long   dstwidth
    )

{
register int     acc, nbits;
register CARD32	 srcMask;
register CARD32	 srcLong;
CARD32		 dstLong;
ilBool           Lastbit;

  srcMask  =  SPECIAL_MASK_BIT;

  srcLong  =  *psrcline++;
  dstLong  =  1;
  Lastbit  =  FALSE;

  acc = dstwidth - srcwidth;
  nbits = srcwidth;

  while (nbits-- ) {

       for(; acc >= 0; acc  -= srcwidth) {      /* keep copying */

              /*    copy bit logic               */
              dstLong <<= 1;
              if (srcMask & srcLong)  dstLong++;

              if(Lastbit) {  

                  flip_bits((ilPtr)&dstLong, sizeof(dstLong));

                  *pdstline++ = dstLong;
                  dstLong = 1;
                  Lastbit = FALSE;
              } 

              /* See if the last bit in the word is coming */
              if( (INT32) dstLong < 0) Lastbit = TRUE;
       }


       acc  += dstwidth;

       /*  increment the mask to the next src bit   */
       SHIFT_MASK(srcMask);
 
       if (!srcMask) {  /* after 32 bits reset mask and get new srcline */ 
           srcMask = SPECIAL_MASK_BIT;
           srcLong = *psrcline++;   
       }

  }   /*  End While  */


  /* Shift up and write out any last bits */
  
  if (dstLong != 1) {      /* has data not copied to pdstline */
       while ( (INT32) dstLong > 0)  dstLong <<= 1;  /* shift up */
       dstLong <<= 1;
       flip_bits((ilPtr)&dstLong, sizeof(dstLong));
       *pdstline++ = dstLong;
  }

}





/* =============================================================================================================================
        ilsw3bytedown  -  Routine to scale down the width of an image by decimation.
                          Operates on 24 bit images.
   ============================================================================================================================= */

static void ilsw3bytedown(
    register unsigned char   *psrcline,
    register unsigned char   *pdstline,
    unsigned long   srcwidth,   
    unsigned long   dstwidth
    )

{
register int acc, n3bytes;
ilBool copy;


  acc  =  srcwidth - dstwidth;
  copy =  (acc == 0)  ?  TRUE  :  FALSE;

  n3bytes = dstwidth;

  if(copy == TRUE) {         /*  degenerate case optimized  */
       while (n3bytes--) {
           *pdstline++  = *psrcline++;   /* 3 Bytes to copy */
           *pdstline++  = *psrcline++;
           *pdstline++  = *psrcline++;
       }
  }
  else {   /*  scale down algorithm  */

       while (n3bytes--) {

           for(; acc > 0;  acc -= dstwidth)  psrcline += 3;
           acc += (srcwidth - dstwidth);

           *pdstline++  = *psrcline++;   /* 3 Bytes to copy */
           *pdstline++  = *psrcline++;
           *pdstline++  = *psrcline++;
       }

  }

}

/* =============================================================================================================================
        ilsw3byteup  -  Routine to scale up the width of an image by replication.
                        Operates on 24 bit images.
   ============================================================================================================================= */

static void ilsw3byteup(
    register unsigned char   *psrcline,
    register unsigned char   *pdstline,
    unsigned long   srcwidth,   
    unsigned long   dstwidth
    )

{
register int acc, n3bytes;

  acc = dstwidth - srcwidth;
  n3bytes = srcwidth;

  while (n3bytes--) {

       for(; acc >= 0; acc  -= srcwidth) {
                *pdstline++ = *psrcline;
                *pdstline++ = *(psrcline + 1);
                *pdstline++ = *(psrcline + 2);
       }

       acc += dstwidth;
       psrcline += 3;
  }

}



 
/* =============================================================================================================================
        ilswbytedown  -  Routine to scale down the width of an image by decimation.
                         Operates on 8 bit images.
   ============================================================================================================================= */

static void ilswbytedown(
    register unsigned char   *psrcline,
    register unsigned char   *pdstline,
    unsigned long   srcwidth,   
    unsigned long   dstwidth
    )

{
register int acc, nbytes;
ilBool copy;


  acc = srcwidth - dstwidth;
  copy =  (acc == 0)  ?  TRUE  :  FALSE;

  nbytes = dstwidth;

  if(copy == TRUE) {         /*  degenerate case optimized  */
        while (nbytes-- )   *pdstline++  = *psrcline++;
  }
  else {   /*  scale down algorithm  */

        while (nbytes-- ) {

            for(; acc > 0;  acc -= dstwidth)  psrcline++;
            acc += (srcwidth - dstwidth);

            *pdstline++  = *psrcline++;
        }

  }

}

/* =============================================================================================================================
        ilswbyteup  -  Routine to scale up the width of an image by replication.
                       Operates on 8 bit images.
   ============================================================================================================================= */

static void ilswbyteup(
    register unsigned char   *psrcline,
    register unsigned char   *pdstline,
    unsigned long   srcwidth,   
    unsigned long   dstwidth
    )

{
register int acc, nbytes;

  acc = dstwidth - srcwidth;
  nbytes = srcwidth;

  while (nbytes--) {

       for(; acc >= 0; acc  -= srcwidth)  *pdstline++ = *psrcline;

       acc += dstwidth;
       psrcline++;

  }
}





/* =============================================================================================================================

      ilScaleBitonalExecute  -  Scale processing for images with bit per pixel format .
                                This function that will get called at least once when the
                                pipe is executed.  This routine loops through the number of 
                                lines, scaling the height and calls the appropriate function 
                                to scale the width.
   ============================================================================================================================= */
static ilError ilScaleBitonalExecute (
    register ilExecuteData  *pData,
    unsigned long           dstLine,
    unsigned long          *pNLines
    )
{
register CARD32		   *psrcline, *pdstline;
register unsigned long     srcnwords, dstnwords;
register long              nlines;
register unsigned long     srcwidth, dstwidth;
ilImagePlaneInfo           *pplane;
register unsigned long     local_noDstLine;
register ilScalePrivptr    pPriv;



   nlines                  =  *pNLines;
   if (nlines <= 0)        return IL_OK;

   pplane                  =  &pData->pSrcImage->plane[0];
   srcnwords               =  (pplane->nBytesPerRow + LONGSZ - 1)/LONGSZ;
   psrcline                =  (CARD32 *) (pplane->pPixels)  +  pData->srcLine * srcnwords; 

   pplane                  =  &pData->pDstImage->plane[0];
   dstnwords               =  (pplane->nBytesPerRow + LONGSZ - 1)/LONGSZ;
   pdstline                =  (CARD32 *) (pplane->pPixels)  +  dstLine * dstnwords; 
   pPriv                   =  (ilScalePrivptr) pData->pPrivate;

   srcwidth  = pData->pSrcImage->width;
   dstwidth  = pData->pDstImage->width;

   local_noDstLine = 0;



   if (pPriv->ilScaleSrcheight == pPriv->ilScaleDstheight)  {    /*   copy case optimized  */

           while (nlines--  > 0) {
                if(srcwidth >= dstwidth)
                     ilswbitdown (psrcline, pdstline, srcwidth,  dstwidth);
                else if (srcwidth < dstwidth) 
                     ilswbitup (psrcline, pdstline, srcwidth, dstwidth);

                local_noDstLine++;
                psrcline += srcnwords;
                pdstline += dstnwords;
           }    
   }
   else if (pPriv->ilScaleSrcheight > pPriv->ilScaleDstheight) {        /* scale height down */

            while (TRUE) {
                while (pPriv->ilScaleAccDown > 0) {
                    if (nlines-- <= 0) 
                        break;
                    pPriv->ilScaleAccDown -= pPriv->ilScaleDstheight;
                    psrcline += srcnwords;
                }
                if (nlines-- <= 0)
                    break;

                pPriv->ilScaleAccDown += (pPriv->ilScaleSrcheight - pPriv->ilScaleDstheight);

                if(srcwidth >= dstwidth)
                     ilswbitdown (psrcline, pdstline, srcwidth,  dstwidth);
                else if (srcwidth < dstwidth) 
                     ilswbitup (psrcline, pdstline, srcwidth, dstwidth);

                local_noDstLine++;
                psrcline += srcnwords;
                pdstline += dstnwords;
           }    

   }
   else if (pPriv->ilScaleSrcheight < pPriv->ilScaleDstheight)  {   /* scale height up */

              while (nlines--  > 0) {

                  for(; pPriv->ilScaleAccUp >= 0; pPriv->ilScaleAccUp  -= pPriv->ilScaleSrcheight) {

                     if(srcwidth >= dstwidth)
                         ilswbitdown (psrcline, pdstline, srcwidth, dstwidth);
                     else if (srcwidth < dstwidth) 
                         ilswbitup (psrcline, pdstline, srcwidth, dstwidth);

                     local_noDstLine++;
                     pdstline += dstnwords;
                  }

                  pPriv->ilScaleAccUp += pPriv->ilScaleDstheight;
                  psrcline += srcnwords;
              }

   }


   *pNLines = local_noDstLine;
   return IL_OK;

}


/* =============================================================================================================================

      ilScale3ByteExecute  -  Scale processing for images with 24 bits per pixel format .
                                This function that will get called at least once when the
                                pipe is executed.  This routine loops through the number of 
                                lines, scaling the height and calls the appropriate function 
                                to scale the width.
   ============================================================================================================================= */
static ilError ilScale3ByteExecute (
    register ilExecuteData   *pData,
    unsigned long           dstLine,
    unsigned long          *pNLines
    )
{
register unsigned char     *psrcline, *pdstline;
register unsigned long     srcnbytes, dstnbytes;
register long              nlines;
register unsigned long     srcwidth, dstwidth;
ilImagePlaneInfo           *pplane;
register unsigned long     local_noDstLine;
register ilScalePrivptr    pPriv;



   nlines                  =  *pNLines;
   if (nlines <= 0)        return IL_OK;

   pplane                  =  &pData->pSrcImage->plane[0];
   srcnbytes               =  pplane->nBytesPerRow;
   psrcline                =  (unsigned char *) (pplane->pPixels)  +  pData->srcLine * srcnbytes; 

   pplane                  =  &pData->pDstImage->plane[0];
   dstnbytes               =  pplane->nBytesPerRow;
   pdstline                =  (unsigned char *) (pplane->pPixels)  +  dstLine * dstnbytes; 
   pPriv                   =  (ilScalePrivptr) pData->pPrivate;


   srcwidth  = pData->pSrcImage->width;
   dstwidth  = pData->pDstImage->width;

   local_noDstLine = 0;



   if (pPriv->ilScaleSrcheight == pPriv->ilScaleDstheight)  {    /*   copy case optimized  */

           while (nlines--  > 0) {
                if(srcwidth >= dstwidth)
                     ilsw3bytedown (psrcline, pdstline, srcwidth,  dstwidth);
                else if (srcwidth < dstwidth) 
                     ilsw3byteup (psrcline, pdstline, srcwidth, dstwidth);

                local_noDstLine++;
                psrcline += srcnbytes;
                pdstline += dstnbytes;
           }    
   }
   else if (pPriv->ilScaleSrcheight >  pPriv->ilScaleDstheight) {        /* scale height down */

            while (TRUE) {
                while (pPriv->ilScaleAccDown > 0) {
                    if (nlines-- <= 0) 
                        break;
                    pPriv->ilScaleAccDown -= pPriv->ilScaleDstheight;
                    psrcline += srcnbytes;
                }
                if (nlines-- <= 0)
                    break;

                pPriv->ilScaleAccDown += (pPriv->ilScaleSrcheight - pPriv->ilScaleDstheight);

                if(srcwidth >= dstwidth)
                     ilsw3bytedown (psrcline, pdstline, srcwidth,  dstwidth);
                else if (srcwidth < dstwidth) 
                     ilsw3byteup (psrcline, pdstline, srcwidth, dstwidth);

                local_noDstLine++;
                psrcline += srcnbytes;
                pdstline += dstnbytes;
           }    

   }
   else if (pPriv->ilScaleSrcheight < pPriv->ilScaleDstheight)  {   /* scale height up */

              while (nlines-- > 0) {

                  for(; pPriv->ilScaleAccUp >= 0; pPriv->ilScaleAccUp  -= pPriv->ilScaleSrcheight) {

                     if(srcwidth >= dstwidth)
                         ilsw3bytedown (psrcline, pdstline, srcwidth, dstwidth);
                     else if (srcwidth < dstwidth) 
                         ilsw3byteup (psrcline, pdstline, srcwidth, dstwidth);

                     local_noDstLine++;
                     pdstline += dstnbytes;
                  }

                  pPriv->ilScaleAccUp += pPriv->ilScaleDstheight;
                  psrcline += srcnbytes;
              }

   }

   *pNLines = local_noDstLine;
   return IL_OK;

}




/* =============================================================================================================================

      ilScaleByteExecute  -  Scale processing for images with byte per pixel format .
                                This function that will get called at least once when the
                                pipe is executed.  This routine loops through the number of 
                                lines, scaling the height and calls the appropriate function 
                                to scale the width.
   ============================================================================================================================= */
static ilError ilScaleByteExecute (
    register ilExecuteData   *pData,
    unsigned long           dstLine,
    unsigned long          *pNLines
    )
{
register unsigned char     *psrcline, *pdstline;
register unsigned long     srcnbytes, dstnbytes;
register long              nlines;
register unsigned long     srcwidth, dstwidth;
ilImagePlaneInfo           *pplane;
register unsigned long     local_noDstLine;
register ilScalePrivptr    pPriv;



   nlines                  =  *pNLines;
   if (nlines <= 0)        return IL_OK;

   pplane                  =  &pData->pSrcImage->plane[0];
   srcnbytes               =  pplane->nBytesPerRow;
   psrcline                =  (unsigned char *) (pplane->pPixels)  +  pData->srcLine * srcnbytes; 

   pplane                  =  &pData->pDstImage->plane[0];
   dstnbytes               =  pplane->nBytesPerRow;
   pdstline                =  (unsigned char *) (pplane->pPixels)  +  dstLine * dstnbytes; 
   pPriv                   =  (ilScalePrivptr) pData->pPrivate;


   srcwidth  = pData->pSrcImage->width;
   dstwidth  = pData->pDstImage->width;

   local_noDstLine = 0;


   if (pPriv->ilScaleSrcheight == pPriv->ilScaleDstheight)  {    /*   copy case optimized  */

           while (nlines--  > 0) {
                if(srcwidth >= dstwidth)
                     ilswbytedown (psrcline, pdstline, srcwidth,  dstwidth);
                else if (srcwidth < dstwidth) 
                     ilswbyteup (psrcline, pdstline, srcwidth, dstwidth);

                local_noDstLine++;
                psrcline += srcnbytes;
                pdstline += dstnbytes;
           }    
   }
   else if (pPriv->ilScaleSrcheight > pPriv->ilScaleDstheight ) {        /* scale height down */

            while (TRUE) {
                while (pPriv->ilScaleAccDown > 0) {
                    if (nlines-- <= 0) 
                        break;
                    pPriv->ilScaleAccDown -= pPriv->ilScaleDstheight;
                    psrcline += srcnbytes;
                }
                if (nlines-- <= 0)
                    break;

                pPriv->ilScaleAccDown += (pPriv->ilScaleSrcheight - pPriv->ilScaleDstheight);

                if(srcwidth >= dstwidth)
                     ilswbytedown (psrcline, pdstline, srcwidth,  dstwidth);
                else if (srcwidth < dstwidth) 
                     ilswbyteup (psrcline, pdstline, srcwidth, dstwidth);

                local_noDstLine++;
                psrcline += srcnbytes;
                pdstline += dstnbytes;
           }    

   }
   else if (pPriv->ilScaleSrcheight < pPriv->ilScaleDstheight )  {   /* scale height up */

              while (nlines--  > 0) {

                  for(; pPriv->ilScaleAccUp >= 0; pPriv->ilScaleAccUp  -= pPriv->ilScaleSrcheight) {

                     if(srcwidth >= dstwidth)
                         ilswbytedown (psrcline, pdstline, srcwidth, dstwidth);
                     else if (srcwidth < dstwidth) 
                         ilswbyteup (psrcline, pdstline, srcwidth, dstwidth);

                     local_noDstLine++;
                     pdstline += dstnbytes;
                  }

                  pPriv->ilScaleAccUp += pPriv->ilScaleDstheight ;
                  psrcline += srcnbytes;
              }

   }

   *pNLines = local_noDstLine;
   return IL_OK;

}


/* =============================================================================================================================

      ilScale   - This is the pipe constructor function that is called by the 
                  application to Add a scale filter to an existing pipe.
                  This routine will check for format types and do an explicit
                  conversion if necessary.
   ============================================================================================================================= */
ilBool ilScale (
    ilPipe              pipe,
    long                dstWidth,
    long                dstHeight,
    int                 option,
    void               *pOptionData
    )

{
unsigned int             state;
ilPipeInfo               info;
register ilScalePrivptr  pPriv;
ilDstElementData         dstdata;
ilImageDes               imdes;
ilImageFormat            imformat;
ilBool                   convert;
ilBool                   bitonal;
static unsigned long    defaultNGrays = 256;
static unsigned long    defaultBitonalThreshold = 192;

       /* Get ptr to pipe info and check state */

       state = ilGetPipeInfo(pipe, TRUE, &info, &imdes, &imformat);
       if(state != IL_PIPE_FORMING) {
         if (!pipe->context->error)
             ilDeclarePipeInvalid(pipe, IL_ERROR_PIPE_STATE);
         return FALSE;
       }

       /* if scale is by exactly 2x or 4x, try to use fast scale code */
       if (option == IL_SCALE_SAMPLE) {
           int scaleFactor;
           if (((info.width << 1) == dstWidth) && ((info.height << 1) == dstHeight))
               scaleFactor = 1;
           else if (((info.width << 2) == dstWidth) && ((info.height << 2) == dstHeight))
               scaleFactor = 2;
           else scaleFactor = 0;
           if (scaleFactor) {
               if (_ilFastUpsample (pipe, &info, &imdes, &imformat, scaleFactor))
                   return TRUE;                  /* scale filter added; exit */
               else if (pipe->context->error)
                   return FALSE;                 /* error occurred; exit */
               }
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
       if((dstWidth <= 0) || (dstHeight <= 0))
           return ilDeclarePipeInvalid(pipe, IL_ERROR_ZERO_SIZE_IMAGE);
         
       switch (option) {
            case IL_SCALE_SIMPLE:

                  if ((dstWidth == info.width) && (dstHeight == info.height)) return TRUE;
                  break;

            case IL_SCALE_SAMPLE:

                  /* demotion to simple if not scaling down */
                  if ((info.width < dstWidth) || (info.height < dstHeight)) break;

                  if ( ((dstWidth == info.width) && (dstHeight == info.height)) &&
                       ((!bitonal) || (!pOptionData)) )  return TRUE;


                  /* if a palette image first convert to RGB */
                  if (imdes.type == IL_PALETTE) {
                       if (!ilConvert (pipe, IL_DES_RGB, IL_FORMAT_3BYTE_PIXEL, 0, (ilPtr)NULL))
                           return FALSE;
                       ilGetPipeInfo (pipe, FALSE, &info, &imdes, &imformat);
                  }

                  if (bitonal) {
                        /* Use default threshold if not supplied */
                      if (!pOptionData) pOptionData = &defaultBitonalThreshold;
                      _ilAreaScaleBitonal (pipe, dstWidth, dstHeight, (int *)pOptionData, &info);
                      }
                  else         
                      _ilScaleSample (pipe, imdes.nSamplesPerPixel, dstWidth, dstHeight, 
                                     256, imdes.blackIsZero, &info);
                  return (pipe->context->error) ? FALSE : TRUE;

            case IL_SCALE_BITONAL_TO_GRAY:

                    /*  Error condition if not scaling down (no demotion) or if the image
                        is not bitonal.  If pOptionData is null, default to 256 levels of gray.
                    */
                if (!pOptionData) pOptionData = &defaultNGrays;
                if ((info.width < dstWidth) || (info.height < dstHeight) || !bitonal)
                    return ilDeclarePipeInvalid(pipe, IL_ERROR_SCALE_BITONAL_TO_GRAY);

                _ilScaleBitonalToGray (pipe, dstWidth, dstHeight, 
                             *((unsigned long *)pOptionData), imdes.blackIsZero, &info);
                return (pipe->context->error) ? FALSE : TRUE;

            default:
                  return ilDeclarePipeInvalid(pipe, IL_ERROR_INVALID_OPTION);
       }




       dstdata.producerObject = (ilObject) NULL;
       dstdata.pDes           = (ilImageDes *) NULL;
       dstdata.pFormat        = (ilImageFormat *) NULL;
       dstdata.width          = dstWidth;
       dstdata.height         = dstHeight;
       dstdata.pPalette       = info.pPalette;;

       /* set output strip height */
       dstdata.stripHeight    = (dstHeight * info.recommendedStripHeight)/info.height + 1;
       dstdata.constantStrip  = FALSE;

       switch (imdes.nSamplesPerPixel) {
            case 3:
                     pPriv = (ilScalePrivptr) ilAddPipeElement(pipe, IL_FILTER, sizeof(ilScalePriv), 0, (ilSrcElementData *) NULL,
                                                               &dstdata, ilScaleInit, IL_NPF, IL_NPF, ilScale3ByteExecute, NULL, 0);
                     break;

            case 1:  
                     if(bitonal) 
                       pPriv = (ilScalePrivptr) ilAddPipeElement(pipe, IL_FILTER, sizeof(ilScalePriv), 0, (ilSrcElementData *) NULL,
                                                                 &dstdata, ilScaleInit, IL_NPF, IL_NPF, ilScaleBitonalExecute, NULL, 0);
                     else 
                       pPriv = (ilScalePrivptr) ilAddPipeElement(pipe, IL_FILTER, sizeof(ilScalePriv), 0, (ilSrcElementData *) NULL,
                                                                 &dstdata, ilScaleInit, IL_NPF, IL_NPF, ilScaleByteExecute, NULL, 0);
       }

       if(!pPriv)
            return FALSE;

       /* Save away true heights */
       pPriv->ilScaleDstheight  = dstHeight;
       pPriv->ilScaleSrcheight  = info.height;

       pipe->context->error = IL_OK;
       return TRUE;
}

