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
/* $XConsortium: ilhplrotate.c /main/3 1995/10/23 15:48:20 rswiston $ */
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

/* =============================================================================================================================

     /ilc/ilhplrotate.c : Images Library arbitrary rotation routines.   using HP-LABs
                            image rotation code
   ============================================================================================================================= */

#include "ilint.h"
#include "ilpipelem.h"
#include "ilerrors.h"
#include "ilhplrotation.h"


 typedef struct {
        DATA_IN_PTR    data_in;               /* pointer to where to put input data */
        DATA_OUT_PTR   buf;
        ilBool         restart;               /* TRUE on pipe re-execution */
        short          in_w;                  /* Arguments to IR_Rotate_Image_Begin */
        short          in_h;
        short          in_nSPP;
        short          mode;
        float          degrees;
        unsigned char  bg_r, bg_g, bg_b;
 }  ilRotatePriv,  *ilRotatePrivptr;


/* =============================================================================================================================

      ilRotateInit  -  Init for Rotation for images 

   ============================================================================================================================= */


static ilError ilRotateInit2(
    ilRotatePrivptr p,
    ilImageInfo     *pSrcImage,
    ilImageInfo     *pDstImage
    )
 
{
DATA_IN_PTR data_in;
DATA_OUT_PTR buf;
short          out_w, out_h;
   /* Re-Initialize rotation code if necessary */
    if (p->restart) {
       data_in = _il_Rotate_Image_Begin (p->in_w , p->in_h, p->in_nSPP, p->mode,
                   p->degrees, p->bg_r, p->bg_g, p->bg_b, &out_w, &out_h);

       buf = (DATA_OUT_PTR) malloc(out_w * p->in_nSPP);
       if (!buf) {
           return FALSE;
       }
           
       p->buf = buf;
       p->data_in = data_in;
    }
    else
      p->restart = TRUE;

   return IL_OK;
}



 
/* =============================================================================================================================

      ilRotateCleanup  -  Cleanup for Rotation for images 

   ============================================================================================================================= */
static ilError ilRotateCleanup (
    ilRotatePrivptr   pPriv,
    ilBool                  aborting
    )
{
   free (pPriv->buf);
   _il_Rotate_Image_End();
   return IL_OK;
}



/* =============================================================================================================================

      ilRotateExecute  -  Rotation for images 

   ============================================================================================================================= */
static ilError ilRotateExecute (
    ilExecuteData   *pData,
    unsigned long           dstLine,
    unsigned long          *pNLines
    )
{



unsigned char     *psrc, *pdst, *psrcline, *pdstline;
unsigned long     srcnbytes, dstnbytes;
ilImagePlaneInfo           *pplane;
ilRotatePrivptr   pPriv;
 int                       nrows, nlines, nlinesout;

   if (*pNLines <= 0)  return IL_OK;

   pplane                  =  &pData->pSrcImage->plane[0];
   srcnbytes               =  pplane->nBytesPerRow;
   psrcline                =  (unsigned char *) (pplane->pPixels) +  pData->srcLine * srcnbytes;

   pplane                  =  &pData->pDstImage->plane[0];
   dstnbytes               =  pplane->nBytesPerRow;
   pdstline                =  (unsigned char *) (pplane->pPixels)  +  dstLine * dstnbytes;
   pPriv                   =  (ilRotatePrivptr) pData->pPrivate;

 

   nlines = *pNLines;
   nlinesout = 0;
   for (nlines = *pNLines; nlines > 0; nlines--){
      bcopy  ((char *) psrcline, (char *) pPriv->data_in, (int) srcnbytes);
      for (nrows = _il_Rotate_Send_Row(&pPriv->data_in); 
           nrows > 0; --nrows) { 
         _il_Rotate_Get_Row(pPriv->buf);
         bcopy  ((char *) pPriv->buf, (char *) pdstline, (int) dstnbytes);
         pdstline += dstnbytes;
         nlinesout++;
      }
      psrcline += srcnbytes;
   }
   *pNLines = nlinesout;
   return IL_OK;
}


/* =============================================================================================================================

      ilRotate - Add a rotate filter to an existing pipe for 0 - 90 degree rotations - checking 
                  for format types and doing an explicit conversion if necessary. Positive factors
                  cause clockwise rotations - negative counter clockwise.

   ============================================================================================================================= */

ilBool ilRotate(
    ilPipe              pipe,
    int                 degrees,
    int                 interpMode,
    unsigned long       bg_color[]
    )

{
unsigned int              state;
ilPipeInfo                info;
ilRotatePrivptr  pPriv;
ilDstElementData          dstdata;
ilImageDes                imdes;
ilImageFormat             imformat;
ilBool                    convert;
unsigned int              rtype;
short                     out_w, out_h, nrows;
DATA_IN_PTR data_in;
DATA_OUT_PTR buf;

#define PIPE_FLAGS        

       /* Get ptr to pipe info and check state */

       state = ilGetPipeInfo(pipe, TRUE, &info, &imdes, &imformat);
       if(state != IL_PIPE_FORMING) {
         if (!pipe->context->error)
             ilDeclarePipeInvalid(pipe, IL_ERROR_PIPE_STATE);
         return FALSE;
       }

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
                                      if (imdes.type  == IL_PALETTE &&
                                          interpMode != IL_ROTATE_SIMPLE) {
                                          imdes.type = IL_RGB;
                                          convert = TRUE;
                                        }
                                      break;

                             case 1:  /* Bitonal */
                                      return ilDeclarePipeInvalid
                                            (pipe, IL_ERROR_NOT_IMPLEMENTED);
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

       degrees %= 360;
       if (degrees < 0) degrees += 360;
/*
 * temporarily, special code can only handle 0 - 90 rotation
 * also, code rotates ccw, but we want to rotate clockwise.
 */
       if (!ilRotate90(pipe, (degrees+89)/90)) return FALSE;
       ilGetPipeInfo (pipe, FALSE, &info, (ilImageDes *)NULL, (ilImageFormat *)NULL);
       degrees = 90 - (degrees % 90);

/* check if 90 deg rotations is sufficient */

       if (degrees == 90) {pipe->context->error = IL_OK; return TRUE;} 

       data_in = _il_Rotate_Image_Begin ( (short) info.width, (short) info.height,
          (short) imdes.nSamplesPerPixel, (short) interpMode,
          (float) degrees,
          (unsigned char) bg_color[0], (unsigned char) bg_color[1], (unsigned char) bg_color[2],
          &out_w, &out_h);

       dstdata.producerObject = (ilObject) NULL;
       dstdata.pDes           = (ilImageDes *) NULL;
       dstdata.pFormat        = (ilImageFormat *) NULL;
       dstdata.pPalette       = info.pPalette;

       dstdata.width          = out_w;
       dstdata.height         = out_h;

       /* set output strip height */
       dstdata.stripHeight    = dstdata.height;
       dstdata.constantStrip  = FALSE;


       pPriv = (ilRotatePrivptr) ilAddPipeElement (pipe, IL_FILTER, 
                   sizeof(ilRotatePriv), IL_ADD_PIPE_HOLD_DST, 
                   (ilSrcElementData *) NULL, &dstdata, ilRotateInit2,
                   ilRotateCleanup, IL_NPF, ilRotateExecute, 0);

       if(!pPriv)
            return FALSE;

       buf = (DATA_OUT_PTR) malloc(out_w *  (short) imdes.nSamplesPerPixel);
       if (!buf) {
           pipe->context->error = IL_ERROR_MALLOC;
           free(pPriv);
           return FALSE;
       }
           
       pPriv->buf = buf;
       pPriv->data_in            = data_in;
       pPriv->restart = FALSE;

/* Arguments to IR_Rotate_Image_Begin */
       pPriv->in_w    = (short) info.width ;
       pPriv->in_h    = (short) info.height;
       pPriv->in_nSPP = (short) imdes.nSamplesPerPixel;
       pPriv->mode    = (short) interpMode;
       pPriv->degrees = (float) degrees;
       pPriv->bg_r    = (unsigned char) bg_color[0];
       pPriv->bg_g    = (unsigned char) bg_color[1];
       pPriv->bg_b    = (unsigned char) bg_color[2];
 
       pipe->context->error = IL_OK;
       return TRUE;
}


