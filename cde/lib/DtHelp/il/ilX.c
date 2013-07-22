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
/* $XConsortium: ilX.c /main/11 1996/11/12 05:02:17 pascale $ */
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
#include "ilcontext.h"
#include "ilX.h"
#include <math.h>
#include <stdlib.h>
#include <X11/Xutil.h>
#include "ilpipelem.h"
#include "ilerrors.h"

#include <Xm/XmPrivate.h>

#define X_COLOR_MAX             65535       /* max value for X colormap RGB 
					       value */

    /*  Buffer allocation checks.  Buffers are allocated for example when 
	writing to a 24 bit drawable: the pipe image pixels are converted 
	from 3 bytes to 4 bytes (CARD32).
    */
#define MAX_IMAGE_BUFFER_SIZE  100000       /* force strips if larger than 
					       this ... */
#define BEST_IMAGE_BUFFER_SIZE 50000        /* to strips of this size */

    /*  Masks for ilFreeColorData(); see below.
    */
#define IL_FREE_XCOLORS         (1<<0)
#define IL_FREE_XGRAYS          (1<<1)

    /*  Codes for view of X , in "ilXWCRec.i.visualType".  When the visual is
        unsupported or an insufficient # of colors can be allocated from the
	colormap, the drawable defaults to IL_XWC_BITONAL.  Images rendered 
	to such an XWC are converted to bitonal, then XPutImage() of an 
	XYBitmap is done - the XWC GC's foreground/background colors are used 
	for black/white.
    */
#define IL_X_MAX_VISUAL_TYPE  3             /* max value of IL_XWC_BITONAL 
					       et al*/

    /*  Default # of grays or dither colors to allocate for pseudo-color. */
#define IL_DEFAULT_X_GRAYS  32
#define NRED                4
#define NGREEN              8
#define NBLUE               4

    /*  Internals of an XWC (X Write Context), an IL object created by 
	ilCreateXWC().
        An XWC is associated with the visual "visual" and "colormap".
        "i" contains basic information returned by ilQueryXWC().
            If pColors is non-null, it points to a list of length "nColors",
        of colors allocated from "colormap".  "pad" in each colorcell is 
	used as a flag:
        == 0 if color not allocated, else color was sucessfully allocated.
            If non-null, colorMapImage is an image for mapping to X images 
	    using ilMap().  
        For example, for 8 bit pseudo-color when 484 color handling is used,
        "pColors" points to 128 colors, and "colorMapImage" points to a 256 
	pixel image where each entry is the pixel to use for color 
	(red*4*8 + green*8 + blue).
        If "colorMapImage" is null then the alloc failed or has not been tried.
        Note: above applies also to Direct/TrueColor visuals, with the same 
	names.
        The above also applies to the "gray" fields, with the "gray" names.
    */
typedef struct {
    ilObjectRec         o;                  /* std header: MUST BE FIRST */
    ilXWCInfo           i;                  /* see above */
    XVisualInfo         visualInfo;         /* X info on "visual" */
    XColor             *pColors;            /* ptr to list of allocated 
					       colors or null */
    int                 nColors;            /* size of *pColors list */
    ilClientImage       colorMapImage;      /* see above */
    ilBool              mapDirect;          /* ilMap(colorMapImage), 
					       Direct/TrueColor */
    XColor             *pGrays;             /* ptr to list of allocated grays 
					       or null */
    int                 nGrays;             /* size of *pGrays list */
    ilClientImage       grayMapImage;       /* same as colorMapImage, but for 
					       grays */
    } ilXWCRec, *ilXWCPtr;

typedef ilError   (*executeFunctionType)();

        /*  ------------------------ ilFreeColorData ---------------------- */
        /*  Free the color list and translation table in the given XWC if 
	    present. "freeMasks" indicates which should be freed: 
	    IL_FREE_XCOLORS/XGRAYS, or ~0 for everything.
        */
static void ilFreeColorData (
    register ilXWCPtr   pXWC,
    unsigned long       freeMask
    )
{
int                     i;
XColor                 *pColor;

        /*  If color list present: free each color if allocated (pad != 0); 
	    free list.
        */
if (freeMask & IL_FREE_XCOLORS) {
    if (pXWC->pColors) {
        for (i = 0, pColor = pXWC->pColors; i < pXWC->nColors; i++) {
            if (pColor->pad)
                XFreeColors (pXWC->i.display, pXWC->i.colormap, 
			     &pColor->pixel, 1, 0L);
            pColor++;
            }
        IL_FREE (pXWC->pColors);
        pXWC->pColors = (XColor *)NULL;
        pXWC->nColors = 0;
        }
    if (pXWC->colorMapImage) {
        ilDestroyObject (pXWC->colorMapImage);
        pXWC->colorMapImage = (ilClientImage)NULL;
        }
    }

        /*  Same for gray list
        */
if (freeMask & IL_FREE_XGRAYS) {
    if (pXWC->pGrays) {
        for (i = 0, pColor = pXWC->pGrays; i < pXWC->nGrays; i++) {
            if (pColor->pad)
                XFreeColors (pXWC->i.display, pXWC->i.colormap, 
			     &pColor->pixel, 1, 0L);
            pColor++;
            }
        IL_FREE (pXWC->pGrays);
        pXWC->pGrays = (XColor *)NULL;
        pXWC->nGrays = 0;
        }
    if (pXWC->grayMapImage) {
        ilDestroyObject (pXWC->grayMapImage);
        pXWC->grayMapImage = (ilClientImage)NULL;
        }
    }
}

        /*  ----------------------- ilDestroyXWC -------------------------- */
        /*  Destroy() function for XWC objects.  Free the color list if 
	    present.
            The GC for the default XWC will be destroyed when the context is
            freed, by ilXDestroyContext() below.
        */
static void ilDestroyXWC (
    ilPtr pPrivate
    )
{
ilObject object = (ilObject)pPrivate;
register ilXWCPtr       pXWC;

    pXWC = (ilXWCPtr)object;

/* ~ on signed integer is not recommended, play it safe here */
    ilFreeColorData (pXWC, ~(0UL));
}


        /*  --------------------- ilGammaCorrect -------------------------- */
        /*  Returns a value from 0..scaleValue, given "value", from 
	    0.."maxValue" - 1, scaled up to "scaleValue" (e.g. 65535 for 
	    X color values).
            Currently a misnomer - not doing any gamma correction 
	    (gamma = 1.0).
        */
static int ilGammaCorrect (
    long                scaleValue,
    int                 value,
    int                 maxValue
    )
{
register int            allocValue;

    if (value == 0) 
        allocValue = 0;
    else if (value == (maxValue - 1))
        allocValue = scaleValue;
    else {
        allocValue = value * scaleValue / (maxValue-1);
        if (allocValue < 0)
            allocValue = 0;
        else if (allocValue > scaleValue)
            allocValue = scaleValue;
        }
    return allocValue;
}


        /*  --------------------- ilAllocateXDitherColors ------------------ */
        /*  Internal function to ilCreateXWC().  Attempts to allocate enough 
	    colors in pXWC->i.colormap to support the visual as a 484 
	    pseudo-color visual.
            An error code is returned to the given context if an error 
	    (e.g. malloc) error occurs; IL_OK if the grays could not be 
	    alloc'd (but false is returned).
        */
static ilBool ilAllocateXDitherColors (
    ilContext               context,
    register ilXWCPtr       pXWC
    )
{

int             i, red, green, blue;                
XColor         *pColor;                             
ilPtr           pTranslate;                         
ilImageInfo     imageInfo, *pImageInfo;

        context->error = IL_OK;                         /* assume no errors */
        if (pXWC->colorMapImage)
            return TRUE;                           /* already alloc'd; EXIT */

            /*  Allocate a client image to use as LUT for mapping to X's 
		colors.
                Point pTranslate to the allocated pixels.
            */
        imageInfo.pDes = IL_DES_GRAY;
        imageInfo.pFormat = IL_FORMAT_BYTE;
        imageInfo.width = 256;
        imageInfo.height = 1;
        imageInfo.clientPixels = FALSE;
        pXWC->colorMapImage = ilCreateClientImage (context, &imageInfo, 0);
        if (!pXWC->colorMapImage) 
            return FALSE;                               /* EXIT */
        ilQueryClientImage (pXWC->colorMapImage, &pImageInfo, 0);
        pTranslate = pImageInfo->plane[0].pPixels;

        pXWC->nColors = NRED * NGREEN * NBLUE;
        pXWC->pColors = (XColor *)IL_MALLOC (pXWC->nColors * sizeof(XColor));
        if (!pXWC->pColors) {
            ilDestroyObject (pXWC->colorMapImage);
            pXWC->colorMapImage = (ilClientImage)NULL;
            context->error = IL_ERROR_MALLOC;
            return FALSE;                               /* EXIT */
            }
        for (i = 0, pColor = pXWC->pColors; i < pXWC->nColors; i++, pColor++)
            pColor->pad = FALSE;            /* init pad, in case below fails */
                                                                              
            /*  Allocate 484 colors (128 in all) */
        pColor = pXWC->pColors;
        for (red = 0; red < NRED; red++)
          for (green = 0; green < NGREEN; green++)
              for (blue = 0; blue < NBLUE; blue++) {
                pColor->red = ilGammaCorrect (X_COLOR_MAX, red, NRED);
                pColor->green = ilGammaCorrect (X_COLOR_MAX, green, NGREEN);
                pColor->blue = ilGammaCorrect (X_COLOR_MAX, blue, NBLUE);
                if (!XAllocColor (pXWC->i.display, pXWC->i.colormap, pColor)) {
                    ilFreeColorData (pXWC, IL_FREE_XCOLORS);
                    return FALSE;
                    }
                pTranslate [red*(NGREEN*NBLUE) + green*NBLUE + blue] = 
		    pColor->pixel;
                pColor->pad = TRUE;         /* mark as alloc'd */
                pColor++;
                }

    return TRUE;                            /* colors successfully alloc'd */
}
                                                                             

    /*  ---------------------------- ilAllocateXGrays ---------------------- */
    /*  Allocate "nGrays" from the colormap associated with the given XWC or 
	return error.
        The grays are ramped evenly from 0..nGrays-1.  If there are "nGrays" 
	existing grays, they are used, else if not nGrays, they are
	deallocated.
        An error code is returned to the given context if an error 
	(e.g. malloc) error occurs; IL_OK if the grays could not be alloc'd 
	(but false is returned).
    */
static ilBool ilAllocateXGrays (
    ilContext               context,
    register ilXWCPtr       pXWC,
    int                     nGrays
    )
{
int             i, index;
XColor         *pColor;                             
ilPtr           pTranslate;                         
ilImageInfo     imageInfo, *pImageInfo;
double          spreadFactor;

        /*  Use the grays if the same number, else deallocate them.
        */
    context->error = IL_OK;                         /* assume no errors */
    if (pXWC->pGrays) {
        if (pXWC->nGrays == nGrays)
            return TRUE;
        else ilFreeColorData (pXWC, IL_FREE_XGRAYS);
    }

    if (nGrays > 256)
        return FALSE;                               /* EXIT */

        /*  Allocate a client image to use as LUT for mapping to X's colors.
            Point pTranslate to the allocated pixels.
        */
    imageInfo.pDes = IL_DES_GRAY;
    imageInfo.pFormat = IL_FORMAT_BYTE;
    imageInfo.width = 256;
    imageInfo.height = 1;
    imageInfo.clientPixels = FALSE;
    pXWC->grayMapImage = ilCreateClientImage (context, &imageInfo, 0);
    if (!pXWC->grayMapImage) 
        return FALSE;                               /* EXIT */
    ilQueryClientImage (pXWC->grayMapImage, &pImageInfo, 0);
    pTranslate = pImageInfo->plane[0].pPixels;

    pXWC->nGrays = nGrays;
    pXWC->pGrays = (XColor *)IL_MALLOC (pXWC->nGrays * sizeof(XColor));
    if (!pXWC->pGrays) {
        ilDestroyObject (pXWC->grayMapImage);
        pXWC->colorMapImage = (ilClientImage)NULL;
        context->error = IL_ERROR_MALLOC;
        return FALSE;                               /* EXIT */
        }
    for (i = 0, pColor = pXWC->pGrays; i < pXWC->nGrays; i++, pColor++)
        pColor->pad = FALSE;                /* init pad, in case below fails */
                                                                              
        /*  Allocate the grays; return error on any failure. */
    for (i = 0; i < nGrays; i++) {
        pColor = &pXWC->pGrays [i];
        pColor->red = pColor->green = pColor->blue = 
	    ilGammaCorrect (X_COLOR_MAX, i, nGrays);
        if (!XAllocColor (pXWC->i.display, pXWC->i.colormap, pColor)) {
            pColor->pad = FALSE;
            ilFreeColorData (pXWC, IL_FREE_XGRAYS);
            return FALSE;                           /* EXIT */
            }                                                                  
        pColor->pad = TRUE;                                                   
        }

        /*  Grays allocated ok; spread the allocated colors over the whole 256
            area.  In effect, color-slam all 256-level gray images into nGrays.
            NOTE: gray images with other than 256 levels could be supported in
	    future!
        */
    spreadFactor = ((double)(nGrays - 1)) / (double)255;
    pColor = pXWC->pGrays;
    for (i = 0; i < 256; i++) {
        index = spreadFactor * (double)i + 0.5;
        if (index > (nGrays - 1))
            index = nGrays - 1;
        pTranslate [i] = pColor [index].pixel;
        }

    return TRUE;
}


    /*  ---------------------- ilAllocateXDirectColors --------------------- */
    /*  Allocate "nLevels" levels of each of RGB for a DirectColor or a 
	TrueColor visual.
        "nLevels" should be passed as 256; it becomes 1/2 of that if failure 
	occurs this function recurses on failure until an unacceptable # of 
	levels not alloc'd.
    */
static ilBool ilAllocateXDirectColors (
    register ilXWCPtr       pXWC,
    int                     nLevels
    )
{
int             i, j, pixel, red, green, blue, nReplicate;
XColor         *pColor;                             
ilPtr           pTranslate;                         
ilImageInfo     imageInfo, *pImageInfo;
ilContext       context;
#define         MIN_DIRECT_LEVELS       32  /* min # of acceptable levels */

    if (pXWC->colorMapImage)
        return TRUE;                                /* already alloc'd; EXIT */

        /*  Create a 256x1 3 byte/pixel map image. */
    context = pXWC->o.p.context;
    imageInfo.pDes = IL_DES_RGB;
    imageInfo.pFormat = IL_FORMAT_3BYTE_PIXEL;
    imageInfo.width = 256;
    imageInfo.height = 1;
    imageInfo.clientPixels = FALSE;
    pXWC->colorMapImage = ilCreateClientImage (context, &imageInfo, 0);
    if (!pXWC->colorMapImage) 
        return FALSE;
    ilQueryClientImage (pXWC->colorMapImage, &pImageInfo, 0);
    pTranslate = pImageInfo->plane[0].pPixels;

        /*  Try to allocate nLevels read-only levels each of RGB, in a logical
	    ramp order.  Set pXWC->mapDirect to false if pixel value != RGB 
	    value, in which case pipe image pixels must be ilMap()'d to X 
	    pixel values.
            If can't allocate, try to allocate nLevels/2 unless that is too 
	    few colors, in which we fail.  Spread the X pixels into the 256 
	    entry map image.
        */
    pXWC->nColors = nLevels;
    pXWC->pColors = (XColor *)IL_MALLOC_ZERO (pXWC->nColors * sizeof(XColor));
    if (!pXWC->pColors) {
        ilDestroyObject (pXWC->colorMapImage);
        pXWC->colorMapImage = (ilClientImage)NULL;
        context->error = IL_ERROR_MALLOC;
        return FALSE;
        }

    pXWC->mapDirect = FALSE;
    nReplicate = 256 / nLevels;     /* # of times to replicate pixel into map 
				       image */
    for (i = 0, pColor = pXWC->pColors; i < nLevels; i++, pColor++) {
        pColor->red = pColor->green = pColor->blue = 
            ilGammaCorrect (X_COLOR_MAX, i, nLevels);
        pColor->flags = DoRed | DoGreen | DoBlue;
        if (!XAllocColor (pXWC->i.display, pXWC->i.colormap, pColor)) {
            ilFreeColorData (pXWC, IL_FREE_XCOLORS);
            if (nLevels <= MIN_DIRECT_LEVELS)
                return FALSE;
            else return ilAllocateXDirectColors (pXWC, nLevels / 2);
            }
        pColor->pad = TRUE;                         /* mark as alloc'd */
        pixel = pColor->pixel;
        red = (pixel >> 16) & 0xff;
        green = (pixel >> 8) & 0xff;
        blue = pixel & 0xff;
        if ((red != i) || (green != i) || (blue != i))
            pXWC->mapDirect = TRUE;
        for (j = 0; j < nReplicate; j++) {       /* replicate into map image */
            *pTranslate++ = red;
            *pTranslate++ = green;
            *pTranslate++ = blue;
            }
        }
    return TRUE;
}



        /*  ------------------------ ilChangeXWC --------------------------- */
        /*  Public function; see spec.
        */
ilBool ilChangeXWC (
    ilXWC                   XWC,
    unsigned int            code,
    void                   *pData
    )
{
register ilXWCPtr           pXWC;
register ilContext          context;

    pXWC = (ilXWCPtr)XWC;
    context = pXWC->o.p.context;
    context->error = IL_OK;               /* assume no error */
    if (pXWC->o.p.objectType != IL_XWC) {
        context->error = IL_ERROR_OBJECT_TYPE;
        return FALSE;                                           /* EXIT */
        }

    switch (code) {

        /*  Allocate grays: allocate grays if gray or pseudo-color, else 
	    return true if gray display supported (e.g. to True/DirectColor).
        */
      case IL_XWC_ALLOC_GRAYS:
        if (pData) {
            context->error = IL_ERROR_OPTION_DATA;
            return FALSE;
            }
        switch (pXWC->i.visualType) {
          case IL_XWC_BITONAL:
            return FALSE;
          case IL_XWC_GRAY_8:
          case IL_XWC_COLOR_8:
            if (ilAllocateXGrays (context, pXWC, IL_DEFAULT_X_GRAYS))
                return TRUE;
            else return FALSE;
          case IL_XWC_COLOR_24:
            return TRUE;
            }

        /*  Allocate colors: 8 and 24 bit color only */
      case IL_XWC_ALLOC_COLORS:
        if (pData) {
            context->error = IL_ERROR_OPTION_DATA;
            return FALSE;
            }
        if ((pXWC->i.visualType == IL_XWC_COLOR_8)
         && ilAllocateXDitherColors (context, pXWC))
             return TRUE;
        else if ((pXWC->i.visualType == IL_XWC_COLOR_24)
         && ilAllocateXDirectColors (pXWC, 256)) 
             return TRUE;
        else return FALSE;

      case IL_XWC_FREE_GRAYS:
        if (pData) {
            context->error = IL_ERROR_OPTION_DATA;
            return FALSE;
            }
        ilFreeColorData (pXWC, IL_FREE_XGRAYS);
        break;

        /*  Free colors if 8 or 24 bit color */
      case IL_XWC_FREE_COLORS:
        if (pData) {
            context->error = IL_ERROR_OPTION_DATA;
            return FALSE;
            }
        if ((pXWC->i.visualType == IL_XWC_COLOR_8) 
         || (pXWC->i.visualType == IL_XWC_COLOR_24))
            ilFreeColorData (pXWC, IL_FREE_XCOLORS);
        break;

      case IL_XWC_SET_DITHER_METHOD: 
      { unsigned int ditherMethod;
        ditherMethod = *((unsigned int *)pData);
        if ((ditherMethod != IL_AREA_DITHER)
         && (ditherMethod != IL_DIFFUSION)
         && (ditherMethod != IL_QUICK_DIFFUSION)) {
            context->error = IL_ERROR_OPTION_DATA;
            return FALSE;
            }
        pXWC->i.ditherMethod = ditherMethod;
        break;
      }

      case IL_XWC_SET_RAW_MODE:
        pXWC->i.rawMode = *((ilBool *)pData);
        break;

      default:
        context->error = IL_ERROR_INVALID_OPTION;
        return FALSE;
        }   /* END switch code */

    return TRUE;
}


        /*  ------------------------ ilQueryXWC --------------------------- */
        /*  Public function; see spec.
        */
ilBool ilQueryXWC (
    ilXWC                   XWC,
    ilXWCInfo              *pInfo
    )
{
register ilXWCPtr           pXWC;

    pXWC = (ilXWCPtr)XWC;
    if (pXWC->o.p.objectType != IL_XWC) {
        pXWC->o.p.context->error = IL_ERROR_OBJECT_TYPE;
        return FALSE;                                           /* EXIT */
        }

    *pInfo = pXWC->i;
    pXWC->o.p.context->error = IL_OK;
    return TRUE;
}


        /*  ------------------------ ilCreateXWC ------------------------- */
        /*  Public function; see spec.
            Return an error if no (null) display in this context.
        */

ilXWC ilCreateXWC (
    ilContext               context,
    Display                *display,
    Visual                 *visual,
    Colormap                colormap,
    GC                      gc,
    unsigned long           mustBeZero1,
    unsigned long           mustBeZero2
    )
{
register ilXWCPtr       pXWC;
XVisualInfo             template, *pVisualInfo;
int                     notUsed;

    if ((mustBeZero1 != 0) || (mustBeZero2 != 0)) {
        context->error = IL_ERROR_PAR_NOT_ZERO;
        return (ilXWC)NULL;                                 /* EXIT */
        }

        /*  A null visual or colormap means output will be to bitmap, else
            get the info for the given visual; exit if not found. 
        */
    if (!visual || !colormap) {
        visual = (Visual *)NULL;
        colormap = (Colormap)0;
        }
    else {
        template.visualid = XVisualIDFromVisual (visual);
        pVisualInfo = XGetVisualInfo (display, VisualIDMask, &template, 
				      &notUsed);
        if (!pVisualInfo) {
            context->error = IL_ERROR_X_RESOURCE;
            return (ilXWC)NULL;                             /* EXIT */
            }
        }

        /*  Create an XWC object and fill it in.
        */
    pXWC = (ilXWCPtr)_ilCreateObject (context, IL_XWC, ilDestroyXWC, 
				      sizeof (ilXWCRec));
    if (!pXWC)
        return (ilXWC)NULL;                                 /* EXIT */

    pXWC->i.display = display;
    pXWC->i.visual = visual;
    if (visual) {
        pXWC->visualInfo = *pVisualInfo;
        free (pVisualInfo);              /* free list from XGetVisualInfo() */
        }
    pXWC->i.gc = gc;
    pXWC->i.colormap = colormap;
    pXWC->pColors = (XColor *)NULL;
    pXWC->nColors = 0;
    pXWC->colorMapImage = (ilClientImage)NULL;
    pXWC->pGrays = (XColor *)NULL;
    pXWC->nGrays = 0;
    pXWC->grayMapImage = (ilClientImage)NULL;
    pXWC->i.ditherMethod = IL_AREA_DITHER;
    pXWC->i.rawMode = FALSE;

        /*  Check the class of the given visual and fill in XWC des and format.
            If visual class not supported default to bitonal.
            May also default to bitonal (or color demote to gray) later if 
	    insufficient colors or grays can be allocated.
            If null visual, do bitonal output.
        */
    pXWC->i.staticVisual = TRUE;            /* assume non-writeable colormap */
    if (!visual)
        pXWC->i.visualType = IL_XWC_BITONAL;
    else switch (pXWC->visualInfo.class) {
        case PseudoColor:
            pXWC->i.staticVisual = FALSE;
        case StaticColor:
            if (pXWC->visualInfo.depth == 8)
                 pXWC->i.visualType = IL_XWC_COLOR_8;
            else pXWC->i.visualType = IL_XWC_BITONAL;
            break;

            /*  Support True/DirectColor if 32 bits format 
		= "<unused 8><8R><8G><8B>". */
        case DirectColor:
            pXWC->i.staticVisual = FALSE;
        case TrueColor:
            if ((pXWC->visualInfo.depth == 24)
             && (pXWC->visualInfo.red_mask == 0xff0000)
             && (pXWC->visualInfo.green_mask == 0xff00)
             && (pXWC->visualInfo.blue_mask == 0xff))
                 pXWC->i.visualType = IL_XWC_COLOR_24;
            else pXWC->i.visualType = IL_XWC_BITONAL;
            break;

        case GrayScale:
            pXWC->i.staticVisual = FALSE;
        case StaticGray:
            if (pXWC->visualInfo.depth == 8)
                 pXWC->i.visualType = IL_XWC_GRAY_8;
            else pXWC->i.visualType = IL_XWC_BITONAL;
            break;
        }   /* END switch visual class */

        /*  If a bitonal visual, mark as static (non-allocatable colors) */
    if (pXWC->i.visualType == IL_XWC_BITONAL)
        pXWC->i.staticVisual = TRUE;

    context->error = IL_OK;
    return (ilXWC)pXWC;
}



/*  ======================== WRITE X DRAWABLE CODE ======================== */ 

    /*  Private block for ilRead/WriteXDrawable pipe element. Notes:
        Inited by ilWriteXDrawable():
           imageDepth      depth of image to create - set into pPriv when added
           imageFormat     X format of image: XYBitmap (bitonal) or ZPixmap
           isLongImage     if true: image is 32bits/pixel: set image rowBytes 
	                   to width*4.
                           If false: set image rowBytes to row bytes of 
			   pSrcImage.
           bufferSize      size of buffer to create and attach to Ximage.
           pXWC            ptr to XWC for this operation
           drawable        id of X drawable (Window or Pixmap) to read or write
           x, y            read: src (x,y).
                           write: dst (x,y) from caller

        Inited by ilWriteXInit():
           Ximage          a temp XImage created by Init.  Its data ptr is 
                           pointed directly at the read/write buffer, or 
			   equals "pBuffer"
           pBuffer         ptr to buffer allocated, if bufferSize > 0
           linesDone       zeroed by Init(): # of lines written / read so far,
                           i.e. y + linesDone is next line to read / write.
    */
typedef struct {
    int                 imageDepth;
    int                 imageFormat;
    ilBool              isLongImage;
    unsigned long       bufferSize;
    ilXWCPtr            pXWC;
    Drawable            drawable;
    ilPtr               pBuffer;
    XImage             *pXImage;
    int                 x, y;
    long                linesDone;
    } ilXPrivRec, *ilXPrivPtr;


        /*  ----------------------- ilXDestroy ----------------------------- */
        /*  Destroy() function for ilWriteXDrawable() pipe element.
            "Destroys" the associated XWC, whose refCount must have been 
	    incremented when the element was successfully added to the pipe - 
	    Destroy downcounts it.
        */
static ilError ilXDestroy (
    ilPtr pPrivate
    )
{
    ilXPrivPtr pPriv = (ilXPrivPtr)pPrivate;
    ilDestroyObject ((ilObject)pPriv->pXWC);
    return IL_OK;
}



        /*  ------------------------- ilWriteXInit ------------------------- */
        /*  Init() function for ilWriteXDrawable() for all src image types.
            Creates an XImage, which is just a data structure - creates no 
	    pixel buffer.
            The XImage is freed by ilWriteXCleanup().
        */
static ilError ilWriteXInit (
    ilPtr                  pPrivate,
    ilImageInfo            *pSrcImage,
    ilImageInfo            *pDstImage                   /* ignored */
    )
{
register ilXPrivPtr pPriv = (ilXPrivPtr)pPrivate;
register ilXWCPtr           pXWC;
register XImage            *pXImage;

        /*  Create a temp buffer for image pixels if requested. */
    if (pPriv->bufferSize > 0) {
        pPriv->pBuffer = (ilPtr)IL_MALLOC (pPriv->bufferSize);
        if (!pPriv->pBuffer)
            return IL_ERROR_MALLOC;                     /* EXIT */
        }
    else pPriv->pBuffer = NULL;

        /*  Create an XImage that will be pointed to each buffer (strip) to 
	    write. */
    pXWC = pPriv->pXWC;
    pXImage = XCreateImage (pXWC->i.display, pXWC->i.visual, 
			    pPriv->imageDepth, pPriv->imageFormat, 0, 
			    (char *)pPriv->pBuffer, pSrcImage->width, 
			    pSrcImage->height, pSrcImage->pFormat->rowBitAlign,
			    (pPriv->isLongImage) ? (pSrcImage->width * 4) :
                            pSrcImage->plane[0].nBytesPerRow);
    if (!pXImage)
        return IL_ERROR_MALLOC;                         /* EXIT */

        /*  Set bit/byte order to MSBFirst.  
            NOTE: byte order should be set to byte order of client machine!!!
        */

#ifdef LSB_BIT_ORDER
    pXImage->byte_order = LSBFirst;
#else
    pXImage->byte_order = MSBFirst; 
#endif
    pXImage->bitmap_bit_order = MSBFirst;
    pPriv->pXImage = pXImage;
    pPriv->linesDone = 0;

    return IL_OK;
}

        /*  --------------------- ilWriteXCleanup -------------------------- */
        /*  Cleanup() function for ilWriteXDrawable() for all src image types.
            Flushes the output if not aborted; frees the XImage created by 
	    Init() below.
        */
static ilError ilWriteXCleanup (
    ilPtr                   pPrivate,
    ilBool                  aborting
    )
{
        /*  Flush if not aborting.  Null the "data" field in image or else 
	    XDestroyImage() will try to free the pixels; destroy the image, 
	    created by Init().
        */
    register ilXPrivPtr pPriv = (ilXPrivPtr)pPrivate;
    if (!aborting)
        XFlush (pPriv->pXWC->i.display);
    if (pPriv->pBuffer)
        IL_FREE (pPriv->pBuffer);
    pPriv->pXImage->data = (char *)NULL;
    XDestroyImage (pPriv->pXImage);
    return IL_OK;
}

        /*  --------------------- ilWriteXExecute ------------------------- */
        /*  Execute() function for ilWriteXDrawable(), for rendering bit or 
	    byte data.
            Points an XImage created by ilWriteXInit() to the src image bits
            and does an XPutImage(), then bumps a line count so next strip is 
	    written *pNLines below this one in the drawable.
        */
static ilError ilWriteXExecute (
    register ilExecuteData  *pData,
    long                     dstLine,
    long                    *pNLines,
    float                    ratio
)
{
register ilXPrivPtr         pPriv;
register XImage            *pXImage;
ilXWCPtr                    pXWC;
unsigned int                nSrcLines;
unsigned int                scaledWidth,scaledHeight;
int                         scaledX, scaledY;

    pPriv = (ilXPrivPtr)pData->pPrivate;
    nSrcLines = *pNLines;
    pXImage = pPriv->pXImage;
    pXImage->height = nSrcLines;
    pXImage->data = (char *)(pData->pSrcImage->plane[0].pPixels + 
                     pData->srcLine * pData->pSrcImage->plane[0].nBytesPerRow);

    pXWC = pPriv->pXWC;

    scaledWidth = pXImage->width * ratio + 0.5;
    scaledX = pPriv->x * ratio + 0.5;
    scaledY = (pPriv->y + pPriv->linesDone) * ratio + 0.5;
    /* we have to think about rounding problem, so recalculate every time. */
    scaledHeight = ((nSrcLines + pPriv->linesDone) * ratio + 0.5) 
	- scaledY;

    _XmPutScaledImage (pXWC->i.display, pPriv->drawable, 
                       pXWC->i.gc, pXImage, 
		       0, 0, scaledX, scaledY,
		       pXImage->width, nSrcLines,
		       scaledWidth,scaledHeight);

    pPriv->linesDone += nSrcLines;

    return IL_OK;
}

        /*  ------------------- ilWriteXOptLongExecute --------------------- */
        /*  Execute() function for ilWriteXDrawable(), for display to optimal 
	    4 bytes pixel frame buffers.  "Optimal" means the 4 bytes (CARD32) 
	    = "<unused 8> <8 R> <8 G> <8 B>",
            which is the format most commonly used right now.
            Reformat the pipe image pixels, which are 3byte pixel RGB (as 
	    required by pXWC->format), into 4 bytes words and do the 
	    XPutImage(), then bump linesDone.
        */
static ilError ilWriteXOptLongExecute (
    register ilExecuteData  *pData,
    long                     dstLine,
    long                    *pNLines,
    float                    ratio
    )
{
ilXPrivPtr                  pPriv;
ilImageInfo                *pSrcImage;
XImage                     *pXImage;
ilXWCPtr                    pXWC;
ilPtr                       pSrcLine;
long                        srcRowBytes, width, nSrcLines, nLinesM1;
register long               nLongsM1;
register ilPtr              pSrc;
register CARD32            *pDst, temp;
unsigned int                scaledWidth,scaledHeight;
int scaledX, scaledY ;

    nSrcLines = *pNLines;
    if (nSrcLines <= 0)
        return;                                     /* no lines, EXIT */
    pPriv = (ilXPrivPtr)pData->pPrivate;
    pSrcImage = pData->pSrcImage;
    width = pSrcImage->width;
    if (width <= 0)
        return;                                     /* no width, EXIT */
    srcRowBytes = pSrcImage->plane[0].nBytesPerRow;
    pSrcLine = (ilPtr)(pSrcImage->plane[0].pPixels + 
		       pData->srcLine * srcRowBytes);
    pXImage = pPriv->pXImage;
    pXImage->height = nSrcLines;
    pXImage->data = (char *)pPriv->pBuffer;
    pDst = (CARD32 *)pPriv->pBuffer;

        /*  pSrcLine points to first line of pipe image; pDst to buffer of 
	    X image.
            Convert 3 byte pipe image to CARD32 X buffer image.  Dont need 
	    dstRowBytes because buffer is exactly "width" longs wide (we 
	    created it.)
        */
    nLinesM1 = nSrcLines - 1;
    do {
        pSrc = pSrcLine;
        pSrcLine += srcRowBytes;
        nLongsM1 = width - 1;
        do {
            temp = *pSrc++;
            temp <<= 8;
            temp |= *pSrc++;
            temp <<= 8;
            temp |= *pSrc++;
            *pDst++ = temp;
            } while (--nLongsM1 >= 0);
        } while (--nLinesM1 >= 0);

    pXWC = pPriv->pXWC;

    scaledWidth = pXImage->width * ratio;
    scaledHeight = nSrcLines * ratio;
    scaledX = pPriv->x * ratio  ;
    scaledY = (pPriv->y + pPriv->linesDone)* ratio  ;

    _XmPutScaledImage (pXWC->i.display, pPriv->drawable, pXWC->i.gc, pXImage, 
		       0, 0, scaledX, scaledY,
		       pXImage->width, nSrcLines,
	               scaledWidth,scaledHeight);

    pPriv->linesDone += nSrcLines;

    return IL_OK;
}


        /*  ------------------------ ilConvertForXWrite -------------------- */
        /*  Public function; see spec.
            Side effect: set pXWC->i.writeType, code for how it will render.
        */
ilBool ilConvertForXWrite (
    ilPipe              pipe,
    ilXWC               XWC,
    ilBool              mapPixels,
    unsigned long       mustBeZero,
    void               *mustBeNull
    )
{
ilPipeInfo                  info;
ilImageDes                  des;
ilImageFormat               format;
unsigned int                writeType;
register ilXWCPtr           pXWC;

    /*  Table indexed by [dst visual type], yielding writeType when in raw 
	mode */
static unsigned int rawWriteMethodTable [(IL_X_MAX_VISUAL_TYPE + 1)] = {
    IL_XWC_WRITE_BITONAL,             /* IL_XWC_BITONAL  */
    IL_XWC_WRITE_GRAY,                /* IL_XWC_GRAY_8   */
    IL_XWC_WRITE_COLOR_DITHER,        /* IL_XWC_COLOR_8  */
    IL_XWC_WRITE_COLOR                /* IL_XWC_COLOR_24 */ };

    /*  Table indexed by [image type, dst visual type], yielding writeType,
	assuming best case: colors/grays alloc'd succesfully; else writeType 
	degrades.
    */
static unsigned int writeTypeTable [(IL_MAX_TYPE + 1) * 
				   (IL_X_MAX_VISUAL_TYPE + 1)] =
{   /* (src) image type    (dst) visual type         writeType       */

    /* IL_BITONAL          IL_XWC_BITONAL    */      IL_XWC_WRITE_BITONAL,
    /*                     IL_XWC_GRAY_8     */      IL_XWC_WRITE_BITONAL,
    /*                     IL_XWC_COLOR_8    */      IL_XWC_WRITE_BITONAL,
    /*                     IL_XWC_COLOR_24   */      IL_XWC_WRITE_BITONAL,

    /* IL_GRAY             IL_XWC_BITONAL    */      IL_XWC_WRITE_BITONAL,
    /*                     IL_XWC_GRAY_8     */      IL_XWC_WRITE_GRAY,
    /*                     IL_XWC_COLOR_8    */      IL_XWC_WRITE_GRAY,
    /*                     IL_XWC_COLOR_24   */      IL_XWC_WRITE_COLOR,

    /* IL_PALETTE          IL_XWC_BITONAL    */      IL_XWC_WRITE_BITONAL,
    /*                     IL_XWC_GRAY_8     */      IL_XWC_WRITE_GRAY,
    /*                     IL_XWC_COLOR_8    */      IL_XWC_WRITE_COLOR_DITHER,
    /*                     IL_XWC_COLOR_24   */      IL_XWC_WRITE_COLOR,

    /* IL_RGB              IL_XWC_BITONAL    */      IL_XWC_WRITE_BITONAL,
    /*                     IL_XWC_GRAY_8     */      IL_XWC_WRITE_GRAY,
    /*                     IL_XWC_COLOR_8    */      IL_XWC_WRITE_COLOR_DITHER,
    /*                     IL_XWC_COLOR_24   */      IL_XWC_WRITE_COLOR,

    /* IL_YCBCR            IL_XWC_BITONAL    */      IL_XWC_WRITE_BITONAL,
    /*                     IL_XWC_GRAY_8     */      IL_XWC_WRITE_GRAY,
    /*                     IL_XWC_COLOR_8    */      IL_XWC_WRITE_COLOR_DITHER,
    /*                     IL_XWC_COLOR_24   */      IL_XWC_WRITE_COLOR
};

    

    if (pipe->objectType != IL_PIPE) {
        pipe->context->error = IL_ERROR_OBJECT_TYPE;
        return FALSE;
        }
    if (mustBeZero || mustBeNull)
        return ilDeclarePipeInvalid (pipe, IL_ERROR_PAR_NOT_ZERO);

        /*  Validate that given XWC is one, and is same context as pipe. */
    pXWC = (ilXWCPtr)XWC;
    if (pXWC->o.p.objectType != IL_XWC)
        return ilDeclarePipeInvalid (pipe, IL_ERROR_OBJECT_TYPE);
    if (pXWC->o.p.context != pipe->context)
        return ilDeclarePipeInvalid (pipe, IL_ERROR_CONTEXT_MISMATCH);

        /*  Get pipe info and decompress; if pipe not in IL_PIPE_FORMING state,
	    error. */
    if (ilGetPipeInfo (pipe, TRUE, &info, &des, &format) != IL_PIPE_FORMING) {
        if (!pipe->context->error)
            ilDeclarePipeInvalid (pipe, IL_ERROR_PIPE_STATE);
        return FALSE;
        }

        /*  If raw mode, allocate no colors, output based on dst visual type, 
	    except for bit/pixel images: output using bitonal approach; else:
            get "writeType" based on des.type (private types not supported) 
	    and pXWC->i.visualType.  Check for allocated dither colors 
	    (default to gray if not enough) and grays (default to bitonal if 
	    not enough).
            Pretend YCbCr is RGB - an ilConvert() to RGB will be done below 
	    anyway.
        */
    if (pXWC->i.rawMode) {
        if ((des.nSamplesPerPixel == 1) && (format.nBitsPerSample[0] == 1))
             writeType = IL_XWC_WRITE_BITONAL;
        else writeType = rawWriteMethodTable [pXWC->i.visualType];
        }
    else { 
        if (des.type > IL_MAX_TYPE)
            return ilDeclarePipeInvalid (pipe, IL_ERROR_IMAGE_TYPE);
        if (des.type == IL_YCBCR)
            des.type = IL_RGB;

        writeType = writeTypeTable 
                 [des.type * (IL_X_MAX_VISUAL_TYPE + 1) + pXWC->i.visualType];
        if (writeType == IL_XWC_WRITE_COLOR_DITHER) {
            if (!pXWC->colorMapImage)
                ilChangeXWC ((ilXWC)pXWC, IL_XWC_ALLOC_DITHER_COLORS, 
			     (ilPtr)NULL);
            if (!pXWC->colorMapImage) 
                writeType = IL_XWC_WRITE_GRAY;
            }
        if (writeType == IL_XWC_WRITE_GRAY) {
            if (!pXWC->grayMapImage)
                ilChangeXWC ((ilXWC)pXWC, IL_XWC_ALLOC_GRAYS, (ilPtr)NULL);
            if (!pXWC->grayMapImage) 
                writeType = IL_XWC_WRITE_BITONAL;
            }

            /*  If RGB display alloc colors or dflt to bitonal. */
        if (writeType == IL_XWC_WRITE_COLOR) {
            if (!pXWC->colorMapImage)
                if (!ilAllocateXDirectColors (pXWC, 256))
                    writeType = IL_XWC_WRITE_BITONAL;
            }
        }   /* END not raw mode */

        /*  Do ilConvert() and add filter(s) based on "writeType". */
    switch (writeType) {

        /*  Bitonal: do an XPutImage() with XYBitmap, which renders image 0's 
	    with GC background color, 1's with foreground, as defined by IL 
	    spec.
            If rawMode, must be bit/pixel.
        */
      case IL_XWC_WRITE_BITONAL:
        if (pXWC->i.rawMode) {
            if ((des.nSamplesPerPixel != 1) || (format.nBitsPerSample[0] != 1))
                return ilDeclarePipeInvalid (pipe, IL_ERROR_RAW_MODE);
            }
        else if (!ilConvert (pipe, IL_DES_BITONAL, IL_FORMAT_BIT, 0, NULL))
            return FALSE;
        break;

        /*  Gray: do ilMap() which maps 256 gray shades to the equivalent 
	    X pixel values, possibly using fewer shades of gray (slam; not 
	    dithered to lesser # grays).
        */
      case IL_XWC_WRITE_GRAY:
        if (pXWC->i.rawMode) {
            if ((des.nSamplesPerPixel != 1) || (format.nBitsPerSample[0] != 8))
                return ilDeclarePipeInvalid (pipe, IL_ERROR_RAW_MODE);
            }
        else {
            if (!ilConvert (pipe, IL_DES_GRAY, IL_FORMAT_BYTE, 0, NULL))
                return FALSE;
            if (mapPixels)
                if (!ilMap (pipe, pXWC->grayMapImage))
                    return FALSE;
            }
        break;

        /*  Dithered RGB: if raw mode, accept if byte/pixel; 
            if a pre-dithered palette image in pipe, ilMap() to map colors to 
	    X pixels;
            otherwise dither cvt to palette w/ map to the allocated colors. 
        */
      case IL_XWC_WRITE_COLOR_DITHER:
        if (pXWC->i.rawMode) {
            if ((des.nSamplesPerPixel != 1) || (format.nBitsPerSample[0] != 8))
                return ilDeclarePipeInvalid (pipe, IL_ERROR_RAW_MODE);
            }
        else if ((des.flags & IL_DITHERED_PALETTE)
              && (des.typeInfo.palette.levels[0] == NRED)
              && (des.typeInfo.palette.levels[1] == NGREEN)
              && (des.typeInfo.palette.levels[2] == NBLUE)) {
                if ((format.nBitsPerSample[0] != 8)
                 && !ilConvert (pipe, (ilImageDes *)NULL, IL_FORMAT_BYTE, 0, 
				NULL))
                    return FALSE;
                if (mapPixels)
                    if (!ilMap (pipe, pXWC->colorMapImage))
                        return FALSE;
                }
        else {
            ilConvertToPaletteInfo  cvt;
            cvt.method = pXWC->i.ditherMethod;
            cvt.levels[0] = NRED; 
	    cvt.levels[1] = NGREEN; 
	    cvt.levels[2] = NBLUE;
            cvt.kernelSize = 8;
            if (mapPixels) {
                cvt.dstType = IL_GRAY;     /* any type other than IL_PALETTE */
                cvt.mapImage = pXWC->colorMapImage;
                }
            else {
                cvt.dstType = IL_PALETTE;
                cvt.mapImage = (ilClientImage)NULL;
                }
            if (!ilConvert (pipe, IL_DES_PALETTE, IL_FORMAT_BYTE, 
			    IL_CONVERT_TO_PALETTE, &cvt))
                return FALSE;
            }
        break;

            /*  RGB: if raw mode, check format, else convert to RGB, and if 
		mapPixels and map needed (ramp not 0..255 each component) do 
		an ilMap().
            */
      case IL_XWC_WRITE_COLOR:
        {
        if (pXWC->i.rawMode) {
            if ((des.nSamplesPerPixel != 3) 
             || (format.nBitsPerSample[0] != 8)
             || (format.nBitsPerSample[1] != 8)
             || (format.nBitsPerSample[2] != 8)
             || (format.sampleOrder != IL_SAMPLE_PIXELS))
                return ilDeclarePipeInvalid (pipe, IL_ERROR_RAW_MODE);
            }
        else {
            if (!ilConvert (pipe, IL_DES_RGB, IL_FORMAT_3BYTE_PIXEL, 0, NULL))
                return FALSE;
            if (mapPixels && pXWC->mapDirect)
                if (!ilMap (pipe, pXWC->colorMapImage))
                    return FALSE;
            }
        }
        break;
    }   /* END switch writeType */

    pXWC->i.writeType = writeType;       /* set method of writing to be used */
    pipe->context->error = IL_OK;
    return TRUE;
}


        /*  ------------------------ ilWriteXDrawable ---------------------- */
        /*  Public function; see spec.
        */
ilBool ilWriteXDrawable (
    ilPipe                  pipe,
    Drawable                drawable,
    ilXWC                   XWC,
    ilRect                 *pSrcRect,
    int                     dstX,
    int                     dstY,
    unsigned long           mustBeZero
    )
{
ilPipeInfo                  info;
register ilXWCPtr           pXWC;
register ilXPrivPtr         pPriv;
ilSrcElementData           *pSrcData, srcData;
unsigned long               bufferSize;
/*
** We need to use the execute fucntion with the ratio parameter.
*/
ilError                   (*executeFunction)( register ilExecuteData *,
					      long,
					      long *,
					      float);

        /*  If a src clip rect, insert an ilCrop() element before us.
            In the future, should look at pipe and handle some cases directly.
        */
    if (pSrcRect && !ilCrop (pipe, pSrcRect))
        return FALSE;

        /*  Convert as necessary, mapping pixels to X pixels and get 
	    pXWC->i.writeType */
    pXWC = (ilXWCPtr)XWC;
    if (!ilConvertForXWrite (pipe, (ilXWC)pXWC, TRUE, 0, NULL))
        return FALSE;

    ilGetPipeInfo (pipe, FALSE, &info, (ilImageDes *)NULL, 
		   (ilImageFormat *)NULL);

        /*  If writing 24 bits: use separate executeFunction, set "bufferSize"
	    to buffer to be allocated, and if too large force pipe to give us 
	    smaller strips.
        */
    if (pXWC->i.writeType == IL_XWC_WRITE_COLOR) {
        bufferSize = (info.width * 4) * info.stripHeight;
        if (bufferSize > MAX_IMAGE_BUFFER_SIZE) {
            srcData.consumerImage = (ilObject)NULL;
            srcData.stripHeight = BEST_IMAGE_BUFFER_SIZE / (4 * info.width);
            if (srcData.stripHeight < 1)
                srcData.stripHeight = 1;
            srcData.constantStrip = FALSE;
            srcData.minBufferHeight = 0;
            pSrcData = &srcData;
            }
        else pSrcData = (ilSrcElementData *)NULL;
        executeFunction = ilWriteXOptLongExecute;
        }
    else {
        pSrcData = (ilSrcElementData *)NULL;
        executeFunction = ilWriteXExecute;
        }

        /*  Add a consumer which does an XPutImage() using the pipe data 
	    cvt'd above.
	    Increment the XWC refCount; element's Destroy() function 
	    ilXDestroy() will decrement it when this pipe element is removed.
        */
    pPriv = (ilXPrivPtr)ilAddPipeElement (pipe, IL_CONSUMER, 
        sizeof(ilXPrivRec), 0, pSrcData, (ilDstElementData *)NULL,
        ilWriteXInit, ilWriteXCleanup, ilXDestroy, NULL, executeFunction, 0);
    if (!pPriv) return FALSE;                          /* EXIT */

    pPriv->pXWC = pXWC;
    pPriv->drawable = drawable;
    pPriv->x = dstX;
    pPriv->y = dstY;
    pXWC->o.refCount++;

        /*  Set writeType-specific values in *pPriv */
    switch (pXWC->i.writeType) {
      case IL_XWC_WRITE_BITONAL:
        pPriv->imageDepth = 1;
        pPriv->imageFormat = XYBitmap;
        pPriv->isLongImage = FALSE;
        pPriv->bufferSize = 0;
        break;
      case IL_XWC_WRITE_GRAY:
        pPriv->imageDepth = 8;
        pPriv->imageFormat = ZPixmap;
        pPriv->isLongImage = FALSE;
        pPriv->bufferSize = 0;
        break;
      case IL_XWC_WRITE_COLOR_DITHER:
        pPriv->imageDepth = 8;
        pPriv->imageFormat = ZPixmap;
        pPriv->isLongImage = FALSE;
        pPriv->bufferSize = 0;
        break;
      case IL_XWC_WRITE_COLOR:
        pPriv->bufferSize = bufferSize;             /* from above */
        pPriv->imageDepth = 24;
        pPriv->imageFormat = ZPixmap;
        pPriv->isLongImage = TRUE;
        break;
        }

    pipe->context->error = IL_OK;
    return TRUE;
}

