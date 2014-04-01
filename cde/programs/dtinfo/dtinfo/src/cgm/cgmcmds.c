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
/* $XConsortium: cgmcmds.c /main/12 1996/11/11 11:32:39 cde-hal $ */
/* module to process the CGM binary format commands */
/* optimised for calling XLib commands */
#include "cgm.h"		/* our defs */
#include <math.h>	/* math functions */
#include <stdio.h>	/* math functions */
/* everything is in C rather than C++ for portability */
/* means we must use some file-scope globals */
#define MAXCOLRS 1024 /* allow only this many colours for mapped devices */
static Display *display;
static Window win;
static int depth;
static Colormap cmap;
static Visual *vis;
static int visualClass, visualID, colormap_size, bits_per_rgb;
static unsigned long redMask, greenMask, blueMask;
static unsigned int visualDepth;
#ifdef DEBUG_VISUAL
/* in case we need this for error messages */
static char *visualNames[6] = {"StaticGrey", "GrayScale", "StaticColor",
			       "PseudoColor", "TrueColor", "DirectColor"};
#endif
static Dimension *ret_width;
static Dimension *ret_height;
static XColor my_colrs[MAXCOLRS];
static Pixel **ret_colrs;
static int *ret_number;
static float xScale;
static int max_size = 512, max_y, max_x;
static Pixmap P;
static Pixmap *retPixmap;
/* Graphics Contexts, and ancillaries */
static GC fillGC, edgeGC, textGC, markGC, lineGC, cellGC;
static XGCValues myStruct; /* for convenience */
static unsigned long mask;
static Pixel blackPixel, whitePixel;
static float xPxlMM, yPxlMM; /* pixels per mm */
static double pi;
static int setBackColr = 0; /* has the background colour been set ? */
/* index into this for dashed line specs */
static char lineArray[6] = {9, 9, 3, 3, 3, 3};
/* deal with Colour map */
#define COLCONV 65535.0 /* for conversion to XColors */
static Pixel coltabPixels[MAXCOLRS];
/* for clipping */
static Region clipRegion, extRegion;
/* for text */
static partialText *textPtr;
static int textW, textH, textX, textY;

/* EAM */
static float ScaleFactor;
/* now the Binary format specific routines that read the various data types */
/* all of these routines take the address of a pointer and step the pointer */
/* forward before they return */
/* the general Binary format string */
static int b_str(unsigned char **in_ptr, char **out_ptr)
{
  int p_len, data_left, i;
  unsigned char *my_ptr;
  
  p_len = (int) *(*in_ptr)++;
  if (p_len == 255) {	/* more to come */
    data_left = (**in_ptr >> 7) && 1;
    p_len = ((**in_ptr & 127) << 8) + (int) *(*in_ptr + 1);
    *in_ptr += 2;	/* skip over this bit */
    if (data_left) {		/* fix later */
      burp(stderr, "truncating string at %d bytes\n", p_len);
      return(CGM_WARN);
    }
  }
  if (!(my_ptr = (unsigned char *) malloc(p_len + 1))) {
    burp(stderr, "couldn't grab %d bytes in b_str\n", p_len + 1);
    return(0);
  }
  *out_ptr = (char *) my_ptr;
  for (i=0; i<p_len; ++i) *my_ptr++ = *(*in_ptr)++;
  *my_ptr = (unsigned char) 0;
  
  return(1);
}
/* now the integers */
/* unsigned CGM integer at arbitrary legal precision */
static unsigned int b_guint(in_ptr, bits_p)
     unsigned char **in_ptr;		/* pointer to the input data pointer */
     int bits_p;			/* no. of bits precision */
{
  int i;
  unsigned int val;
  
  val = *(*in_ptr)++;
  for (i=1; i<(bits_p / 8); ++i)
    val = (val << 8) | *(*in_ptr)++;
  
  return(val);
}
/* signed CGM integer at arbitrary legal precision */
static int b_gsint(in_ptr, bits_p)
     unsigned char **in_ptr;		/* pointer to the input data pointer */
     int bits_p;			/* no. of bits precision */
{
  int i, val;
  /* have to worry about sign extension, may not have signed char */
  if (**in_ptr & (1 << 7)) val = (-1 ^ 255) | *(*in_ptr)++;
  else val = *(*in_ptr)++;
  for (i=1; i<(bits_p / 8); ++i)
    val = (val << 8) | *(*in_ptr)++;
  
  return(val);
}
/* now the reals */
/* the fixed point real */
static double b_fixed(dat_ptr, prec)
     unsigned char **dat_ptr;
     rp_type *prec;
{
  double ret;
  /* do it in two parts; the big (first) part and fractional (second) part */
  
  ret = (double) b_gsint(dat_ptr, prec->exp);	/* first part */
  ret += (double) b_guint(dat_ptr, prec->fract) / (1 << prec->fract);
  
  return(ret);
}
/* the IEEE floating point */
static double b_ieee(dat_ptr, prec)
     unsigned char **dat_ptr;
     rp_type *prec;
{
#define TABLESIZE 128
  static double shift_table[TABLESIZE];
  double ret, dsp;
  int is_neg, i;	/* is it a negative number ? */
  unsigned long big_part, small_part;
  /* use table for both speed and safety */
  if (shift_table[0] == 0) {
    for (i=0; i<TABLESIZE; ++i) shift_table[i] = 1.0 / (1 << i);
  }

  /* this is basically copied from the IEEE spec */
  
  is_neg = (**dat_ptr >> 7) & 1;
  
  if ((prec->exp + prec->fract) == 32) {	/* 32-bit precision */
    big_part = (((*dat_ptr)[0] & 127) << 1) + (((*dat_ptr)[1] >> 7) & 1);
    small_part = (((*dat_ptr)[1] & 127) << 16) +
      ((*dat_ptr)[2] << 8) + (*dat_ptr)[3];
    *dat_ptr += 4;	/* skip forward */
    /* now check for special cases */
    if (big_part == 255) {
      if (small_part == 0) {
	/* ret = (double) HUGE_VAL;	 biggest possible */
	ret = (double) 1000000;		/* biggest possible */
      } else {
	burp(stderr, "IEEE undefined real, small part = %ld, != 0\n",
	     small_part);
	return(0.0);
      }
    } else if (big_part == 0) {
      if (small_part == 0) {
	ret = 0.0;
      } else {
	ret = small_part * pow(2.0, -149.0);
      }
    }
    else {	/* normal case */
      /* ret = 1.0 + small_part * pow(2.0, -23.0); */
      ret = 1.0 + small_part * shift_table[23];
      /* ret *= pow(2.0, (double) big_part - 127.0); */
      if (big_part < TABLESIZE) 
	ret *= shift_table[TABLESIZE - 1 - big_part];
      else ret *= pow(2.0, (double) big_part - 127.0);
      /* avoid use of pow, gives trouble on some compilers */
    }
  } else 	if ((prec->exp + prec->fract) == 64) {	/* 64-bit precision */
    big_part = ((**dat_ptr & 127) << 4) + 
      ((*(*dat_ptr + 1) >> 4) & 15);
    dsp = (double) (*(*dat_ptr + 1) & 15);
    for (i=2; i<8; ++i) {
      dsp *= 256;
      dsp += (double) *(*dat_ptr + i);
    }
    *dat_ptr += 8;		/* skip forward */
    if (big_part == 2047) {
      if (dsp == 0.0) {
	/* ret = (double) HUGE_VAL;	 biggest possible */
	ret = (double) 1000000;		/* biggest possible */
      } else {
	burp(stderr, "IEEE undefined real, dsp = %f, != 0.0\n", dsp);
	return(0.0);
      }
    } else if (big_part == 0) {		
      if (dsp == 0.0) ret = 0.0;
      else ret = dsp * pow(2.0, -1074.0);
    } else {
      ret = 1.0 + dsp * pow(2.0, -52.0);
      ret *= pow(2.0, (double) big_part - 1023.0);
    }
  } else {
    burp(stderr, "illegal precision in IEEE\n");
    return(0.0);
  }
  if (is_neg) ret = -ret;
  return(ret);
#undef TABLESIZE
}
/* the general real */
static double b_real(dat_ptr, rprec)
     unsigned char **dat_ptr;
     rp_type *rprec;
{
  if (rprec->fixed == 0) {	/* floating point */
    return(b_ieee(dat_ptr, rprec));
  } else if (rprec->fixed == 1) {	/* fixed */
    return(b_fixed(dat_ptr, rprec));
  } else {
    burp(stderr, "illegal real precision %d\n", rprec->fixed);
    return(0.0);
  }
}
/* XLib specific functions */
/* the setup routine */
int xl_setup(Screen *screen, Display *inDisplay, Window inWin, int inDepth,
	     Colormap inCmap, Visual *inVis, GC inGC, Dimension *in_width,
	     Dimension *in_height, Pixel **in_colrs, int *in_number,
	     Pixmap *inPixmap, unsigned short sf)
{
  XVisualInfo myTemp, *myInfo;
  int tmp;
  ScaleFactor = (float)sf;
  ScaleFactor /= 100.0;
  /* store the info */
  display = inDisplay;
  win = inWin;
  depth = inDepth;
  cmap = inCmap;
  vis = inVis;
  ret_width = in_width;
  ret_height = in_height;
  ret_colrs = in_colrs;
  /* figure out our visual class */
  visualID = XVisualIDFromVisual(inVis);
  myTemp.visualid = visualID;
  /* get the visual info */
  myInfo = XGetVisualInfo(display, VisualIDMask, &myTemp, &tmp);
  visualClass = myInfo->class; /* crazy for XLib to use this word ! */
  visualDepth = myInfo->depth;
  redMask = myInfo->red_mask;
  greenMask = myInfo->green_mask;
  blueMask = myInfo->blue_mask;
  colormap_size = myInfo->colormap_size;
  bits_per_rgb = myInfo->bits_per_rgb;
#ifdef DEBUG_VISUAL
  fprintf(stderr, "visual class is %s\n", visualNames[visualClass]);
  fprintf(stderr, "depth = %d, mask = (%ld, %ld, %ld), cmap_size = %d, bits = %d\n",
	  visualDepth, redMask, greenMask, blueMask, colormap_size,
	  bits_per_rgb);
#endif
  XFree(myInfo);
  /* get the memory now for in_colrs in case we get a malformed cgm file */
  *ret_colrs = (Pixel *) malloc(MAXCOLRS * sizeof(Pixel));
  ret_number = in_number;
  *ret_number = 0;
  retPixmap = inPixmap; /* so we can change *inPixmap later */
  /* make the GCs */
  myStruct.line_width = 0; /* default to 0 for speed */
  myStruct.line_style = LineSolid;
  myStruct.fill_style = FillSolid;
  myStruct.cap_style = CapButt;
  myStruct.join_style = JoinBevel; /* screws up with Miter */
  mask =
    GCLineWidth |
      GCLineStyle |
	GCFillStyle |
	  GCCapStyle |
	    GCJoinStyle;
  fillGC = XCreateGC(display, win, mask, &myStruct);
  edgeGC = XCreateGC(display, win, mask, &myStruct);
  textGC = XCreateGC(display, win, mask, &myStruct);  
  markGC = XCreateGC(display, win, mask, &myStruct);
  lineGC = XCreateGC(display, win, mask, &myStruct);
  cellGC = XCreateGC(display, win, mask, &myStruct);
  /* take the foreground and background colours from the input GC */
  mask =
    GCBackground |
      GCForeground;
  XCopyGC(display, inGC, mask, fillGC);
  XCopyGC(display, inGC, mask, edgeGC);
  XCopyGC(display, inGC, mask, textGC);
  XCopyGC(display, inGC, mask, textGC);
  XCopyGC(display, inGC, mask, lineGC);
  pi = 4 * atan(1.0);
  /* get our black and white pixels */
  blackPixel = BlackPixelOfScreen(screen);
  whitePixel = WhitePixelOfScreen(screen);
  xPxlMM = ((float) WidthOfScreen(screen)) / WidthMMOfScreen(screen);
  yPxlMM = ((float) HeightOfScreen(screen)) / HeightMMOfScreen(screen);
  return 1;
}
/* how we get a X coordinate, use float, probably truncate later */
static float getX(unsigned char **dat_ptr, cgm_s_type *cgm_s)
{
  switch (cgm_s->vdctype) {
  case VDC_INT: return xScale *
    (b_gsint(dat_ptr, cgm_s->vdcintprec)  - cgm_s->vdcextent[0].i);
  case VDC_REAL: return xScale *
      (b_real(dat_ptr, &(cgm_s->vdcrprec)) - cgm_s->vdcextent[0].r);
  default: return 0; /* where did this come from ? */
  }
}
/* how we get a Y coordinate, use float, probably truncate later */
static float getY(unsigned char **dat_ptr, cgm_s_type *cgm_s)
{
  switch (cgm_s->vdctype) {
  case VDC_INT: return xScale *
    (b_gsint(dat_ptr, cgm_s->vdcintprec)  - cgm_s->vdcextent[1].i);
  case VDC_REAL: return xScale *
      (b_real(dat_ptr, &(cgm_s->vdcrprec)) - cgm_s->vdcextent[1].r);
  default: return 0; /* where did this come from ? */
  }
}
/* how we get a VDC, use float, probably truncate later, no offset needed */
static float getVDC(unsigned char **dat_ptr, cgm_s_type *cgm_s)
{
  switch (cgm_s->vdctype) {
  case VDC_INT: return xScale * b_gsint(dat_ptr, cgm_s->vdcintprec);
  case VDC_REAL: return xScale * b_real(dat_ptr, &(cgm_s->vdcrprec));
  default: return 0; /* where did this come from ? */
  }
}
/* quick macro for colour difference */
#define XCOLRDIF(colr1, colr2) \
(abs(colr1.red - colr2.red) + \
 abs(colr1.green - colr2.green) + \
 abs(colr1.blue - colr2.blue))
/* this is where all colours are sorted out, even indexed colours */
static Pixel getPixel(cgm_s_type *cgm_s, int iColrs[3])
{
  Status ret;
  XColor inColr;
  Pixel bestPixel = 0;
  float dColrs[3];
  float fDiv;
  int i, bestI;
  unsigned int bestDif, tempDif;
  /* normalize colours for X routines */
  for (i=0; i<3; ++i) {
    fDiv = cgm_s->cvextent[i+3] - cgm_s->cvextent[i]; /* for safety */
    dColrs[i] = (iColrs[i] - cgm_s->cvextent[i]) / fDiv;
  }
  /* we split up by visual class */
  switch (visualClass) {
  case 5: /* DirectColor */
  case 2: /* StaticColor */
  case 4: /* TrueColor */
    /* can get the pixel directly for these classes */
    /* assume it's better to do that than work with the colour map */
    bestPixel = (redMask & (int) (redMask * dColrs[0])) |
      (greenMask & (int) (greenMask * dColrs[1])) |
      (blueMask & (int) (blueMask * dColrs[2]));
    return bestPixel;
    /* now the visuals for which XAllocColor makes sense */
  default: /* just in case */
  case 1: /* GreyScale */
  case 0 : /* Static Grey */
    if (visualDepth == 2) { /* monochrome, a common display */
      return ((dColrs[0] + dColrs[1] + dColrs[2]) > 0.9) ? blackPixel :
	whitePixel;
    }
    /* else fall thru to the pseudo colour visual for XAllocColor */
  case 3: /* PseudoColor */
    /* just ask for closest colour */
    inColr.red = (unsigned short) (dColrs[0] * COLCONV + 0.4999);
    inColr.green = (unsigned short) (dColrs[1] * COLCONV + 0.4999);
    inColr.blue = (unsigned short) (dColrs[2] * COLCONV + 0.4999);
    ret = XAllocColor(display, cmap, &inColr);
    if (ret) {
      /* lets see if it's a new one or not */
      for (i=0; i<*ret_number; ++i) {
	if (my_colrs[i].pixel == inColr.pixel)
        {
          XFreeColors(display, cmap, &inColr.pixel,1, 0);
          return inColr.pixel;
        }
      }
      /* must be a new one */
      my_colrs[*ret_number].red = inColr.red;
      my_colrs[*ret_number].green = inColr.green;
      my_colrs[*ret_number].blue = inColr.blue;
      my_colrs[*ret_number].pixel = inColr.pixel;
      if (*ret_number < (MAXCOLRS - 1))
      {
         ++*ret_number;
      }
      return inColr.pixel;
    } else {
      /* look for best so far */
      /* if nothing, fall back on black/white */
      if (!*ret_number) return ((dColrs[0] + dColrs[1] + dColrs[2]) > 0.9)
	? blackPixel : whitePixel;
      /* at least have some to work with */
      bestI = 0;
      bestPixel = my_colrs[bestI].pixel;
      bestDif = XCOLRDIF(my_colrs[bestI], inColr);
      for (i=1; i<*ret_number; ++i) { /* real ugly, but do for now */
	tempDif = XCOLRDIF(my_colrs[i], inColr);
	if (tempDif < bestDif) {
	  bestPixel = my_colrs[i].pixel;
	  bestDif = tempDif;
	  bestI = i;
	}
      }
    }
    return bestPixel;
  } /* end of switch */
}
/* a direct colour */
static void setRGB(cgm_s_type *cgm_s, int iColrs[3], GC inGC)
{
  Pixel usePixel;
  usePixel = getPixel(cgm_s, iColrs);
  XSetForeground(display, inGC, usePixel);
}
/* an indexed colour */
static Pixel getIPixel(int index)
{
  if ((index >= 0) && (index < MAXCOLRS) && coltabPixels[index]) {
    return coltabPixels[index];
  } else return blackPixel;; 
}
static void setIndex(cgm_s_type *cgm_s, int index, GC inGC)
{
  XSetForeground(display, inGC, getIPixel(index));    
}
/* now the functions that do the work */
/* Delimiters */
/* Begin Metafile */
static int bmf(unsigned char *dat_ptr, struct cmd_info_s *cmd_ptr,
	       cgm_s_type *cgm_s)
{
  /* nothing for now */
  return 1;
}
/* End Metafile */
static int emf(unsigned char *dat_ptr, struct cmd_info_s *cmd_ptr,
	       cgm_s_type *cgm_s)
{
  /* nothing for now */
  return 1;
}
/* Begin Picture */
static int bpic(unsigned char *dat_ptr, struct cmd_info_s *cmd_ptr,
		cgm_s_type *cgm_s)
{
  /* nothing for now */
  return 1;
}
/* Begin Picture Body */
static int bpicbody(unsigned char *dat_ptr, struct cmd_info_s *cmd_ptr,
		    cgm_s_type *cgm_s)
{
  XPoint myClipRect[4];
  GC clearGC;
  /* look at VDC extent and figure out Pixmap size and scaling */
  float x0, x1, y0, y1, w, h, maxWH;

  if (cgm_s->vdctype) { /* real VDC's */
    x0 = cgm_s->vdcextent[0].r;
    y0 = cgm_s->vdcextent[1].r;
    x1 = cgm_s->vdcextent[2].r;
    y1 = cgm_s->vdcextent[3].r;
  } else { /* integer VDC'c */
    x0 = cgm_s->vdcextent[0].i;
    y0 = cgm_s->vdcextent[1].i;
    x1 = cgm_s->vdcextent[2].i;
    y1 = cgm_s->vdcextent[3].i;
  }
  w = (x1 > x0) ? x1 - x0 : x0 - x1;
  h = (y1 > y0) ? y1 - y0 : y0 - y1;
  /* presently have max_size for size of the window */
  /* need to override max_size if we have metric scaling set */
  if (cgm_s->scalmode) { /* metric scaling */
    /* there are supposed to be cgm_s->scalmode_f millimetres per vdc unit */
    /* find the biggest direction */
    maxWH = (w > h) ? w : h;
    /* assuming square pixels, how many do we need ? */
    max_size = (int) (xPxlMM * maxWH * cgm_s->scalmode_f);
  }

  if (h > w) { /* taller than wide */
    xScale = max_size / h;
    max_y = *ret_height = max_size;
    max_x = *ret_width = max_size * w / h;
  } else { /* wider than tall */
    if (w == 0) w = 1; /* just in case */
    xScale = max_size / w;
    max_x = *ret_width = max_size;
    max_y = *ret_height = max_size * h / w;
  }
  if (ScaleFactor > 0)
  {
    max_y =  *ret_height = ScaleFactor * max_y;
    max_x =  *ret_width = ScaleFactor * max_x;
    /*max_size *= ScaleFactor; */
    xScale *= ScaleFactor;
  }


  /* set up for clipping */
  myClipRect[0].x = myClipRect[0].y = myClipRect[3].x = myClipRect[1].y = 0;
  myClipRect[1].x = myClipRect[2].x = max_x;
  myClipRect[2].y = myClipRect[3].y = max_y;
  clipRegion = XPolygonRegion(myClipRect, 4, EvenOddRule);
  extRegion = XPolygonRegion(myClipRect, 4, EvenOddRule);
  /* make the Pixmap */
  *retPixmap = P = XCreatePixmap(display, win, *ret_width, *ret_height, depth);
  /* fill it with the right background colour */
  if (setBackColr) { /* set explicitly */
    myStruct.foreground = getPixel(cgm_s, cgm_s->backcolr);
  } else { /* use incoming GC */
    mask = GCBackground | GCForeground;
    XGetGCValues(display, lineGC, mask, &myStruct);
    /* now put the background colr in the foreground and do the fill */
    myStruct.foreground = myStruct.background;
  }
  myStruct.fill_style = FillSolid;
  mask = GCFillStyle | GCForeground;
  clearGC = XCreateGC(display, win, mask, &myStruct);
  XFillRectangle(display, P, clearGC, 0, 0, *ret_width, *ret_height);
  /* no text yet to set */
  textPtr = NULL;
  return(1);
}
/* End Picture */
static int epic(unsigned char *dat_ptr, struct cmd_info_s *cmd_ptr,
		cgm_s_type *cgm_s)
{
  /* need to fill out the pixels array */
  int i;
  for (i=0; i < *ret_number; ++i) {
    (*ret_colrs)[i] = my_colrs[i].pixel;
  }
  return 1;
}
/* Metafile Descriptors */
/* Metafile Version */
static int mfversion(dat_ptr, cmd_ptr, cgm_s)
     unsigned char *dat_ptr;
     struct cmd_info_s *cmd_ptr;
     cgm_s_type *cgm_s;
{
  /* nothing for now */
  return 1;
}
/* Metafile Descriptor */
static int mfdescrip(dat_ptr, cmd_ptr, cgm_s)
     unsigned char *dat_ptr;
     struct cmd_info_s *cmd_ptr;
     cgm_s_type *cgm_s;
{
  /* nothing for now */
  return 1;
}
/* VDC type */
static int vdctype(dat_ptr, cmd_ptr, cgm_s)
     unsigned char *dat_ptr;
     struct cmd_info_s *cmd_ptr;
     cgm_s_type *cgm_s;
{
  /* 0=>integer, 1=>real */ 
  
  cgm_s->vdctype = b_gsint(&dat_ptr, 16);
  
  return 1;
}
/* Integer Precision */
static int intprec(dat_ptr, cmd_ptr, cgm_s)
     unsigned char *dat_ptr;
     struct cmd_info_s *cmd_ptr;
     cgm_s_type *cgm_s;
{
  cgm_s->intprec = b_gsint(&dat_ptr, cgm_s->intprec);
  return 1;
}
/* Real Precision */
static int realprec(dat_ptr, cmd_ptr, cgm_s)
     unsigned char *dat_ptr;
     struct cmd_info_s *cmd_ptr;
     cgm_s_type *cgm_s;
{
  cgm_s->realprec.fixed = b_gsint(&dat_ptr, cgm_s->intprec);
  cgm_s->realprec.exp = b_gsint(&dat_ptr, cgm_s->intprec);
  cgm_s->realprec.fract = b_gsint(&dat_ptr, cgm_s->intprec);
  
  return 1;
}
/* Index Precision */
static int indexprec(dat_ptr, cmd_ptr, cgm_s)
     unsigned char *dat_ptr;
     struct cmd_info_s *cmd_ptr;
     cgm_s_type *cgm_s;
{
  cgm_s->indexprec = b_gsint(&dat_ptr, cgm_s->intprec);
  return 1;
}
/* Colour Precision */
static int colprec(dat_ptr, cmd_ptr, cgm_s)
     unsigned char *dat_ptr;
     struct cmd_info_s *cmd_ptr;
     cgm_s_type *cgm_s;
{
  cgm_s->colprec = b_gsint(&dat_ptr, cgm_s->intprec);
  return 1;
}
/* Colour Index Precision */
static int cindprec(dat_ptr, cmd_ptr, cgm_s)
     unsigned char *dat_ptr;
     struct cmd_info_s *cmd_ptr;
     cgm_s_type *cgm_s;
{
  cgm_s->cindprec = b_gsint(&dat_ptr, cgm_s->intprec);
  return 1;
}
/* Maximum Colour Index */
static int maxcind(dat_ptr, cmd_ptr, cgm_s)
     unsigned char *dat_ptr;
     struct cmd_info_s *cmd_ptr;
     cgm_s_type *cgm_s;
{
  cgm_s->maxcind = b_guint(&dat_ptr, cgm_s->cindprec);
  return 1;
}
/* Colour Value Extent */
static int cvextent(dat_ptr, cmd_ptr, cgm_s)
     unsigned char *dat_ptr;
     struct cmd_info_s *cmd_ptr;
     cgm_s_type *cgm_s;
{
  int i;
  for (i=0; i<6; ++i)
    cgm_s->cvextent[i] = b_guint(&dat_ptr, cgm_s->colprec);
  /* don't allow problems */
  for (i=0; i<3; ++i) {
    if (cgm_s->cvextent[i+3] == cgm_s->cvextent[i]) {
      cgm_s->cvextent[i+3] = 0;
      cgm_s->cvextent[i+3] = 255;
    }
  }
  return 1;
}
/* Metafile Element List */
static int mfellist(dat_ptr, cmd_ptr, cgm_s)
     unsigned char *dat_ptr;
     struct cmd_info_s *cmd_ptr;
     cgm_s_type *cgm_s;
{
  int i;
  cgm_s->mfellist_s = b_gsint(&dat_ptr, cgm_s->intprec);
  if (!(cgm_s->mfellist = (int *) malloc(sizeof(int) * 2 * 
					 cgm_s->mfellist_s))) {
    burp(stderr, "couldn't get memory for MF ellement list\n");
    return(0);
  }
  for (i=0; i<2 * cgm_s->mfellist_s; ++i) 
    *(cgm_s->mfellist + i) = b_gsint(&dat_ptr, cgm_s->indexprec);
  return 1;
}
/* Metafile Defaults Replacement, a complex element */
static int mfdefrep(dat_ptr, cmd_ptr, cgm_s)
     unsigned char *dat_ptr;
     struct cmd_info_s *cmd_ptr;
     cgm_s_type *cgm_s;
{
  struct cmd_info_s new_cmd;	/* for the new commands */
  int i, new_len, b_to_move, data_left;
  unsigned char *out_ptr, *end_ptr, *new_ptr, *start_ptr;
  
  /* in this element, many other elements can be encoded in its 	    */
  /* parameter list. So we take them one at a time and hand them over */
  /* to do_b_cmd. Thus do_b_cmd is being called recursively. 	    */
  if (!(out_ptr = (unsigned char *) malloc(cmd_ptr->p_len))) {
    burp(stderr, 
	 "couldn't make memory for metafile defaults replacement\n");
    return(0);
  }	/* made some memory for decoded commands */
  /* now put the decoded commands into the new memory */
  /* setup pointers */
  new_ptr = out_ptr;
  end_ptr = dat_ptr + cmd_ptr->p_len;
  /* initialise the new command pointer */
  new_cmd.no = cmd_ptr->no;
  new_cmd.sub_no = 1;
  new_cmd.byte_no = 0;
  while (dat_ptr < end_ptr) {	/* some input left */
    start_ptr = dat_ptr;		/* mark place */
    /* now decipher the command header (lots of magic numbers !)*/
    new_cmd.Class = (int) ((dat_ptr[0] >> 4) & 15);
    new_cmd.element = (int) (((dat_ptr[0] << 3) & 127) |
			     ((dat_ptr[1] >> 5) & 7));
    new_cmd.p_len = (int) (dat_ptr[1] & 31);
    if ((new_cmd.Class != B_PDESCLASS) &&
	(new_cmd.Class != B_CTRLCLASS) &&
	(new_cmd.Class != B_ATTRCLASS)) {
      burp(stderr, "illegal command in Metafile Defaults Replacement: ");
      burp(stderr, "class = %d, element = %d\n", new_cmd.Class,
	   new_cmd.element);
    }
    dat_ptr += 2;		/* skip over header */
    if (new_cmd.p_len < 31) {		/* short form */
      b_to_move = (new_cmd.p_len % 2) ? new_cmd.p_len + 1 
	: new_cmd.p_len;
      for (i=0; i<b_to_move; ++i) *out_ptr++ = *dat_ptr++;
    } else {			/* long form */
      data_left = 1;
      new_cmd.p_len = 0;			/* start fresh */
      while (data_left) {		/* some data still to get */
	/* how much to come ? */
	new_len = (int) (((dat_ptr[0] & 127) << 8) | 
			 dat_ptr[1]);
	/* any more to come ? */
	data_left = (dat_ptr[0] >> 7) & 1;
	new_cmd.p_len += new_len;
	dat_ptr += 2;
	b_to_move = (new_len % 2) ? new_len + 1 : new_len;
	for (i=0; i<b_to_move; ++i) *out_ptr++ = *dat_ptr++;
      }
    }
    /* now make the call */
    if (!do_b_cmd(&new_cmd, new_ptr, cgm_s)) {
      burp(stderr, "error in do_b_cmd while replacing defaults\n");
      return(0);
    }	    
    ++new_cmd.sub_no;
    new_cmd.byte_no += (dat_ptr - start_ptr);
    out_ptr = new_ptr;	/* back to the beginning */
  }
  
  /* now tell the devices that we are finished replacing defaults */
  return 1;
}
/* Font List, store the data, but ignored for now */
static int fontlist(dat_ptr, cmd_ptr, cgm_s)
     unsigned char *dat_ptr;
     struct cmd_info_s *cmd_ptr;
     cgm_s_type *cgm_s;
{
  int i;
  unsigned char *my_ptr = NULL;
  
  my_ptr = dat_ptr;
  for (i=0; (i<MAX_FONTS) && (my_ptr < (dat_ptr + cmd_ptr->p_len - 3));
       ++i) {
    if (!b_str(&my_ptr, cgm_s->fontlist + i)) {
      burp(stderr, "couldn't get font name\n");
      return(0);
    }
  }
  /* zero out the rest of the fonts */
  for (; i<MAX_FONTS; ++i) cgm_s->fontlist[i] = NULL;
  
  return 1;
}
/* Character Set List, stored but ignored */
static int charlist(dat_ptr, cmd_ptr, cgm_s)
     unsigned char *dat_ptr;
     struct cmd_info_s *cmd_ptr;
     cgm_s_type *cgm_s;
{
  int i;
  unsigned char *my_ptr = NULL;
  
  my_ptr = dat_ptr;
  for (i=0; (i<MAX_FONTS) && (my_ptr < (dat_ptr + cmd_ptr->p_len - 3));
       ++i) {
    cgm_s->chartype[i] = b_gsint(&my_ptr, 16);
    if (!b_str(&my_ptr, cgm_s->charlist + i)) {
      burp(stderr, "couldn't get character set list\n");
      return(0);
    }
  }
  /* zero out the rest */
  for (; i<MAX_FONTS; ++i) {
    cgm_s->charlist[i] = NULL;
    cgm_s->chartype[i] = 0;
  }
  return 1;
}
/* Character Announcer */
static int charannounce(dat_ptr, cmd_ptr, cgm_s)
     unsigned char *dat_ptr;
     struct cmd_info_s *cmd_ptr;
     cgm_s_type *cgm_s;
{
  cgm_s->charannounce = b_gsint(&dat_ptr, 16);
  return 1;
}
/* Picture Descriptors */
/* Scaling Mode */
static int scalmode(dat_ptr, cmd_ptr, cgm_s)
     unsigned char *dat_ptr;
     struct cmd_info_s *cmd_ptr;
     cgm_s_type *cgm_s;
{
  
  cgm_s->scalmode = b_gsint(&dat_ptr, 16);
  if (cgm_s->scalmode) cgm_s->scalmode_f = 
    b_ieee(&dat_ptr, &(cgm_s->realprec));	/* strange default */
  
  return 1;
}
/* Colour Selection Mode */
static int colselmode(dat_ptr, cmd_ptr, cgm_s)
     unsigned char *dat_ptr;
     struct cmd_info_s *cmd_ptr;
     cgm_s_type *cgm_s;
{
  
  cgm_s->colselmode = b_gsint(&dat_ptr, 16);
  return 1;
}
/* Line Width Specification Mode */
static int lwidspecmode(dat_ptr, cmd_ptr, cgm_s)
     unsigned char *dat_ptr;
     struct cmd_info_s *cmd_ptr;
     cgm_s_type *cgm_s;
{
  cgm_s->lwidspecmode = b_gsint(&dat_ptr, 16);
  return 1;
}
/* Marker Size Specification Mode */
static int marksizspecmode(dat_ptr, cmd_ptr, cgm_s)
     unsigned char *dat_ptr;
     struct cmd_info_s *cmd_ptr;
     cgm_s_type *cgm_s;
{
  cgm_s->marksizspecmode = b_gsint(&dat_ptr, 16);
  return 1;
}
/* Edge Width Specification Mode */
static int edwidspecmode(dat_ptr, cmd_ptr, cgm_s)
     unsigned char *dat_ptr;
     struct cmd_info_s *cmd_ptr;
     cgm_s_type *cgm_s;
{
  cgm_s->edwidspecmode = b_gsint(&dat_ptr, 16);
  return 1;
}
/* VDC Extent */
static int vdcextent(dat_ptr, cmd_ptr, cgm_s)
     unsigned char *dat_ptr;
     struct cmd_info_s *cmd_ptr;
     cgm_s_type *cgm_s;
{
  int i;
  
  switch (cgm_s->vdctype) {
  case VDC_INT:	for (i=0; i<4; ++i) cgm_s->vdcextent[i].i = 
    b_gsint(&dat_ptr, cgm_s->vdcintprec);
    break;
  case VDC_REAL:	for (i=0; i<4; ++i) cgm_s->vdcextent[i].r = 
    b_real(&dat_ptr, &(cgm_s->vdcrprec));
    break;
  }
  return 1;
}
/* Background Colour */
static int backcolr(dat_ptr, cmd_ptr, cgm_s)
     unsigned char *dat_ptr;
     struct cmd_info_s *cmd_ptr;
     cgm_s_type *cgm_s;
{
  int i;
  for (i=0; i<3; ++i)
    cgm_s->backcolr[i] = b_guint(&dat_ptr, cgm_s->colprec);
  setBackColr = 0;
  return 1;
}
/* Control Elements */
/* VDC Integer Precision */
static int vdcintprec(dat_ptr, cmd_ptr, cgm_s)
     unsigned char *dat_ptr;
     struct cmd_info_s *cmd_ptr;
     cgm_s_type *cgm_s;
{
  cgm_s->vdcintprec = b_gsint(&dat_ptr, cgm_s->intprec);
  return 1;
}
/* VDC Real Precision */
static int vdcrprec(dat_ptr, cmd_ptr, cgm_s)
     unsigned char *dat_ptr;
     struct cmd_info_s *cmd_ptr;
     cgm_s_type *cgm_s;
{
  cgm_s->vdcrprec.fixed = b_gsint(&dat_ptr, cgm_s->intprec);
  cgm_s->vdcrprec.exp = b_gsint(&dat_ptr, cgm_s->intprec);
  cgm_s->vdcrprec.fract = b_gsint(&dat_ptr, cgm_s->intprec);
  
  return 1;
}
/* Auxiliary Colour */
static int auxcolr(dat_ptr, cmd_ptr, cgm_s)
     unsigned char *dat_ptr;
     struct cmd_info_s *cmd_ptr;
     cgm_s_type *cgm_s;
{
  int i;
  switch(cgm_s->colselmode) {
  case I_C_M:	cgm_s->auxcolr[3] = b_guint(&dat_ptr, cgm_s->cindprec);
    break;
  case D_C_M:	for (i=0; i<3; ++i) 
    cgm_s->auxcolr[i] = b_gsint(&dat_ptr, cgm_s->colprec);
    break;
  }
  return 1;
}
/* Transparency */
static int transp(dat_ptr, cmd_ptr, cgm_s)
     unsigned char *dat_ptr;
     struct cmd_info_s *cmd_ptr;
     cgm_s_type *cgm_s;
{
  cgm_s->transp = b_gsint(&dat_ptr, 16);
  return 1;
}
/* return a set of XPoints, and their number */
unsigned int getXPoints(unsigned char *dat_ptr,
			struct cmd_info_s *cmd_ptr, cgm_s_type *cgm_s,
			XPoint **inPtr)
{
  int i, noPts;
  static XPoint *myPtr = NULL;
  static int mySize = 0;
  if (cgm_s->vdctype == VDC_INT) {
    noPts = (8 * cmd_ptr->p_len) / (2 * cgm_s->vdcintprec);
  } else if (cgm_s->vdctype == VDC_REAL) {
    noPts = (8 * cmd_ptr->p_len) /
      (2 * (cgm_s->vdcrprec.fract + cgm_s->vdcrprec.exp));
  } else return 0;
  if (noPts < 1) return 0;
  if (noPts > mySize) { /* need more points memory */
    if (myPtr) free(myPtr); /* eliminate old memory */
    myPtr = (XPoint *) malloc(sizeof(XPoint) * noPts);
    mySize = noPts;
  }
  /* fill it out */
  for (i=0; i<noPts; ++i) {
    myPtr[i].x = getX(&dat_ptr, cgm_s);
    myPtr[i].y = max_y - getY(&dat_ptr, cgm_s);
  }
  /* fill out the return values */
  *inPtr = myPtr;
  return noPts;
}
/* Clipping Rectangle */
static int cliprect(unsigned char *dat_ptr, struct cmd_info_s *cmd_ptr,
		    cgm_s_type *cgm_s)
{
  int noPts;
  XPoint *myPtr = NULL, myClipRect[4];
  noPts = getXPoints(dat_ptr, cmd_ptr, cgm_s, &myPtr);
  if ((noPts < 2 ) || !myPtr) return 0;
  /* have to get clipping rectangle set up correctly */
  if (myPtr[0].x < myPtr[1].x) {
    myClipRect[0].x = myClipRect[3].x = myPtr[0].x - 1;
    myClipRect[1].x = myClipRect[2].x = myPtr[1].x - 1;
  } else {
    myClipRect[0].x = myClipRect[3].x = myPtr[1].x - 1;
    myClipRect[1].x = myClipRect[2].x = myPtr[0].x - 1;
  }
  if (myPtr[0].y < myPtr[1].y) {
    myClipRect[0].y = myClipRect[1].y = myPtr[0].y - 1;
    myClipRect[2].y = myClipRect[3].y = myPtr[1].y - 1;
  } else {
    myClipRect[0].y = myClipRect[1].y = myPtr[1].y - 1;
    myClipRect[2].y = myClipRect[3].y = myPtr[0].y - 1;
  }
  clipRegion = XPolygonRegion(myClipRect, 4, EvenOddRule);

  /* if clipping is on, set the clipping area */
  if (cgm_s->clipindic) {
    XSetRegion(display, lineGC, clipRegion);
    XSetRegion(display, textGC, clipRegion);
    XSetRegion(display, fillGC, clipRegion);
    XSetRegion(display, edgeGC, clipRegion);
    XSetRegion(display, cellGC, clipRegion);
    XSetRegion(display, markGC, clipRegion);
  }
  
  return 1;
}
/* Clipping Indicator */
static int clipindic(unsigned char *dat_ptr, struct cmd_info_s *cmd_ptr,
		     cgm_s_type *cgm_s)
{
  cgm_s->clipindic = b_gsint(&dat_ptr, 16);
  if (cgm_s->clipindic) {
    XSetRegion(display, lineGC, clipRegion);
    XSetRegion(display, textGC, clipRegion);
    XSetRegion(display, fillGC, clipRegion);
    XSetRegion(display, edgeGC, clipRegion);
    XSetRegion(display, cellGC, clipRegion);
    XSetRegion(display, markGC, clipRegion);
  } else {
    XSetRegion(display, lineGC, extRegion);
    XSetRegion(display, textGC, extRegion);
    XSetRegion(display, fillGC, extRegion);
    XSetRegion(display, edgeGC, extRegion);
    XSetRegion(display, cellGC, extRegion);
    XSetRegion(display, markGC, extRegion);
  }
  return 1;
}
/* Graphical primitives */
unsigned int getXSegments(unsigned char *dat_ptr,
			  struct cmd_info_s *cmd_ptr, cgm_s_type *cgm_s,
			  XSegment **inPtr)
{
  int i, noSegments;
  XSegment *myPtr = NULL; 
  /* figure out how many segments we have */
  if (cgm_s->vdctype == VDC_INT) {
    noSegments = (8 * cmd_ptr->p_len)  / (4 * cgm_s->vdcintprec);
  } else if (cgm_s->vdctype == VDC_REAL) {
    noSegments = (8 * cmd_ptr->p_len) /
      (4 *(cgm_s->vdcrprec.fract + cgm_s->vdcrprec.exp));
  } else return 0;
  if (noSegments < 1) return 0;
  /* make memory for our segments */
  myPtr = (XSegment *) malloc(sizeof(XSegment) * noSegments);
  /* and fill it out */
  for (i=0; i<noSegments; ++i) {
    myPtr[i].x1 = getX(&dat_ptr, cgm_s);
    myPtr[i].y1 = max_y - getY(&dat_ptr, cgm_s);
    myPtr[i].x2 = getX(&dat_ptr, cgm_s);
    myPtr[i].y2 = max_y - getY(&dat_ptr, cgm_s);
  }
  /* return our values */
  *inPtr = myPtr;
  return noSegments;
}
/* Polyline */
static int polyline(dat_ptr, cmd_ptr, cgm_s)
     unsigned char *dat_ptr;
     struct cmd_info_s *cmd_ptr;
     cgm_s_type *cgm_s;
{
  int noPts;
  XPoint *myPtr = NULL;
  noPts = getXPoints(dat_ptr, cmd_ptr, cgm_s, &myPtr);
  if ((noPts < 1) || !myPtr) return 0;
  XDrawLines(display, P, lineGC, myPtr, noPts, CoordModeOrigin);  
  return(1);
}
/* Disjoint Polyline, on/off segments */
static int dispoly(dat_ptr, cmd_ptr, cgm_s)
unsigned char *dat_ptr;
struct cmd_info_s *cmd_ptr;
cgm_s_type *cgm_s;
{
  int noSegments;
  XSegment *myPtr = NULL;

  noSegments = getXSegments(dat_ptr, cmd_ptr, cgm_s, &myPtr);
  if ((noSegments < 1) || !myPtr) return 0;
  XDrawSegments(display, P, lineGC, myPtr, noSegments);  
  if (myPtr) free(myPtr);
  return(1);
}
/* Polymarker */
static int polymarker(dat_ptr, cmd_ptr, cgm_s)
     unsigned char *dat_ptr;
     struct cmd_info_s *cmd_ptr;
     cgm_s_type *cgm_s;
{
  int noPts, i, x, y, size, asize;
  /* figure out the number of points */
  if (cgm_s->vdctype == VDC_INT) {
    noPts = (8 * cmd_ptr->p_len) / (2 * cgm_s->vdcintprec);
  } else if (cgm_s->vdctype == VDC_REAL) {
    noPts = (8 * cmd_ptr->p_len) /
      (2 * (cgm_s->vdcrprec.fract + cgm_s->vdcrprec.exp));
  } else return 0;
  if (noPts < 1) return 0;

  /* go thru the points, drawing the markers */
  size = cgm_s->msize.i / 2;
  if (size < 1) size = 1; /* minimum size */
  asize = 0.7 * size; /* asterisk/cross size */
  if (asize < 1) asize = 1; /* minimum size */

  for (i=0; i<noPts; ++i) {
    x = getX(&dat_ptr, cgm_s);
    y = max_y - getY(&dat_ptr, cgm_s);
    switch(cgm_s->mtype) { /* what type of marker */
    case 1: /* dot */
      XDrawPoint(display, P, markGC, x, y);
      break;
    case 2: /* plus */
      XDrawLine(display, P, markGC, x - size, y, x + size, y);
      XDrawLine(display, P, markGC, x, y - size, x, y + size);
      break;
    case 4: /* circle */
      XDrawArc(display, P, markGC, x - size, y - size,
	       2 * size, 2 * size, 0, 360 * 64);
      break;
    case 3: /* asterisk, use fallthru */
      XDrawLine(display, P, markGC, x - size, y, x + size, y);
      XDrawLine(display, P, markGC, x, y - size, x, y + size);
    case 5: /* cross */
      XDrawLine(display, P, markGC, x - asize, y - asize, x + asize, y + asize);
      XDrawLine(display, P, markGC, x - asize, y + asize, x + asize, y - asize);
      break;
    default: /* do nothing */
      break;
    }
  }
  return(1);
}
/* in cgmtext.c */
/* actually draw some text */
void draw_cgm_text(Display*,  Drawable, GC, cgm_s_type*, int, int, int, int,
		   partialText*);
/* got a final flag, so actually draw text */
void doText(cgm_s_type *cgm_s)
{
  partialText *myPtr, *tmpPtr;
  /* draw the text */
  draw_cgm_text(display, P, textGC, cgm_s, textX, textY, textW, textH,  textPtr);
  /* clean up memory */
  for (myPtr = textPtr; myPtr; myPtr = tmpPtr) {
    tmpPtr = myPtr->next;
    free(myPtr->text);
    free(myPtr);
  }
  textPtr = NULL;
  textX = textY = textW = textH = 0;
}
/* Regular Text */
static int text(dat_ptr, cmd_ptr, cgm_s)
     unsigned char *dat_ptr;
     struct cmd_info_s *cmd_ptr;
     cgm_s_type *cgm_s;
{
  int final_flag;
  partialText *newPtr, *myPtr;
   /* clean up in case we didn't get a final flag (e.g., old NCAR) */
  if (textPtr) doText(cgm_s);
  
  /* first get the point for the string */
  textX = getX(&dat_ptr, cgm_s);
  textY = max_y - getY(&dat_ptr, cgm_s);
  /* make some memory */
  newPtr = (partialText *) malloc(sizeof(partialText));
  newPtr->text = NULL;
  newPtr->next = NULL;
  newPtr->tcolr = cgm_s->tPixel;
  newPtr->cexpfac = cgm_s->cexpfac;
  newPtr->cspace = cgm_s->cspace;
  newPtr->cheight = cgm_s->cheight.i;
  /* now the final/not final flag */
  final_flag = b_gsint(&dat_ptr, 16);
  

  /* now the string */
  if (!b_str(&dat_ptr, &(newPtr->text))) {
    burp(stderr, "couldn't get text string\n");
    return(0);
  }
  /* append it to our pointer */
  if (textPtr) {
    for (myPtr = textPtr; myPtr->next; myPtr = myPtr->next);
    myPtr->next = newPtr;
  } else textPtr = newPtr;
  /* any more to come ? */
  if (final_flag) doText(cgm_s);
  return 1;
}
/* Restricted Text */
static int restext(dat_ptr, cmd_ptr, cgm_s)
     unsigned char *dat_ptr;
     struct cmd_info_s *cmd_ptr;
     cgm_s_type *cgm_s;
{
  int final_flag;
  partialText *newPtr, *myPtr;
  
  /* first the width and height */
  textW = getVDC(&dat_ptr, cgm_s);
  textH = getVDC(&dat_ptr, cgm_s);
  /* now get the point for the string */
  textX = getX(&dat_ptr, cgm_s);
  textY = max_y - getY(&dat_ptr, cgm_s);
  /* now the final/not final flag */
  final_flag = b_gsint(&dat_ptr, 16);
  /* make some memory */
  newPtr = (partialText *) malloc(sizeof(partialText));
  newPtr->text = NULL;
  newPtr->next = NULL;
  newPtr->tcolr = cgm_s->tPixel;
  newPtr->cexpfac = cgm_s->cexpfac;
  newPtr->cspace = cgm_s->cspace;
  newPtr->cheight = cgm_s->cheight.i;

  /* now the string */
  if (!b_str(&dat_ptr, &(newPtr->text))) {
    burp(stderr, "couldn't get text string\n");
    return(0);
  }
  /* append it to our pointer */
  if (textPtr) {
    for (myPtr = textPtr; myPtr->next; myPtr = myPtr->next);
    myPtr->next = newPtr;
  } else textPtr = newPtr;
  /* any more to come ? */
  if (final_flag) doText(cgm_s);
  return 1;
}
/* Appended Text */
static int apptext(dat_ptr, cmd_ptr, cgm_s)
     unsigned char *dat_ptr;
     struct cmd_info_s *cmd_ptr;
     cgm_s_type *cgm_s;
{
  partialText *newPtr, *myPtr;
  if (!textPtr) return 0; /* can't append if there's nothing started ! */
  
  /* make some memory */
  newPtr = (partialText *) malloc(sizeof(partialText));
  newPtr->text = NULL;
  newPtr->next = NULL;
  newPtr->tcolr = cgm_s->tPixel;
  newPtr->cexpfac = cgm_s->cexpfac;
  newPtr->cspace = cgm_s->cspace;
  newPtr->cheight = cgm_s->cheight.i;
  
  /* now the string */
  if (!b_str(&dat_ptr, &(newPtr->text))) {
    burp(stderr, "couldn't get text string\n");
    return(0);
  }
  /* append it to our pointer */
  /* find last partial text */
  for (myPtr = textPtr; myPtr->next; myPtr = myPtr->next);
  myPtr->next = newPtr;
  return 1;
}
/* Polygon */
static int polygon(dat_ptr, cmd_ptr, cgm_s)
     unsigned char *dat_ptr;
     struct cmd_info_s *cmd_ptr;
     cgm_s_type *cgm_s;
{
  int noPts;
  XPoint *myPtr = NULL;
  noPts = getXPoints(dat_ptr, cmd_ptr, cgm_s, &myPtr);
  if ((noPts < 2) || !myPtr) return 0;
  /* fill the polygon */
  XFillPolygon(display, P, fillGC, myPtr, noPts, Complex, CoordModeOrigin);
  /* draw the edge ? */
  if (cgm_s->evis) { /* not necessary, but here for optimization */
    XDrawLines(display, P, edgeGC, myPtr, noPts, CoordModeOrigin);  
    /* close the edge */
    XDrawLine(display, P, edgeGC, myPtr[0].x, myPtr[0].y,
	      myPtr[noPts - 1].x, myPtr[noPts - 1].y);
  }
  return 1;
}
/* Polyset */
static int polyset(dat_ptr, cmd_ptr, cgm_s)
     unsigned char *dat_ptr;
     struct cmd_info_s *cmd_ptr;
     cgm_s_type *cgm_s;
{
  int i, noPts, *vPtr, lastFill;
  XPoint *myPtr;
  
  /* get the number of pts */
  switch (cgm_s->vdctype) {
  case VDC_INT:	noPts = (8 * cmd_ptr->p_len) / (2 * cgm_s->vdcintprec + 16);
    break;
  case VDC_REAL:	noPts = (8 * cmd_ptr->p_len) / 
    (2 * (cgm_s->vdcrprec.fract + cgm_s->vdcrprec.exp) + 16);
    break;
  default: return 0; /* shouldn't happen */
  }
  /* now make the memory */
  myPtr = (XPoint *) malloc(sizeof(XPoint) * noPts);
  vPtr = (int *) malloc(sizeof(int) * noPts);
  /* check we got it */
  if ((noPts < 2) || !myPtr || !vPtr) return 0;
  /* and fill it out */
  for (i=0; i<noPts; ++i) {
    myPtr[i].x = getX(&dat_ptr, cgm_s);
    myPtr[i].y = max_y - getY(&dat_ptr, cgm_s);
    vPtr[i] = b_gsint(&dat_ptr, 16);	/* the flag */
  }
  /* draw the polygon set */
  lastFill = 0; /* no closure pts yet */
  for (i=0; i<noPts; ++i) {
    /* check the edge out flag */
    switch (vPtr[i]) {
    case 0: /* invisible edge */
      break;
    case 1: /* visible edge */
      if (i < (noPts - 1))
	XDrawLine(display, P, edgeGC, myPtr[i].x, myPtr[i].y,
		  myPtr[i + 1].x, myPtr[i + 1].y);
      break;
    case 2: /* close, invisible edge */
      if (i > (lastFill + 2)) 
	XFillPolygon(display, P, fillGC, myPtr + lastFill, i - lastFill + 1,
		     Complex, CoordModeOrigin);
      lastFill = i + 1;
      break;
    case 3: /* close, visible edge */
      if (i > (lastFill + 2)) 
	XFillPolygon(display, P, fillGC, myPtr + lastFill, i - lastFill + 1,
		     Complex, CoordModeOrigin);
      if (i > lastFill)
	XDrawLine(display, P, edgeGC, myPtr[i].x, myPtr[i].y,
		  myPtr[lastFill].x, myPtr[lastFill].y);
      lastFill = i + 1;
      break;
    default: /* do nothing */
      break;
    }
  }
  /* free the memory */
  free(myPtr);
  free(vPtr); 
  return 1;
}
/* Cell Array */
/* need specific functions to deal with cell array colours */
/* macro to get an unsigned integer value from a cell array */
static int temp;
#define UINT unsigned int
#define AINT(ptr, prec, out, done) switch(prec) {\
case 32: out = (((UINT)ptr[0]) << 24) + (((UINT)ptr[1]) << 16)\
 + (((UINT)ptr[2]) << 8) + ptr[3]; ptr += 4; break;\
case 24: out = (((UINT)ptr[0]) << 16)\
 + (((UINT)ptr[1]) << 8) + ptr[2]; ptr += 3; break;\
case 16: out = (((UINT)ptr[0]) << 8) + ptr[1]; ptr += 2; break;\
case 8: out = *ptr++; break;\
case 4: if(!(temp=(((done + 1) * prec) % 8))) out = *ptr++ & 15; \
else out = (*ptr >> temp) & 15; break;\
case 2: if(!(temp=(((done + 1) * prec) % 8))) out = *ptr++ & 3; \
else out = (*ptr >> (8 - temp)) & 3; break;\
case 1: if(!(temp=(((done + 1) * prec) % 8))) out = *ptr++ & 1; \
else out = (*ptr >> (8 - temp)) & 1; break;}
/* get a packed list */
static void getListPixels(unsigned char *datPtr, cgm_s_type *cgm_s,
			  int nx, int ny, int prec, Pixel *retPtr)
{
  unsigned char *startPtr;
  int rowSize, i, j, k, done, iColrs[3];
  int iCol = 0;
  /* may be in direct colour mode or indexed colour mode */
  switch (cgm_s->colselmode) {
  case D_C_M: /* direct colour */
    rowSize = (nx * 3 * prec + 7) / 8; /* no of bytes per row */
    if (rowSize % 2) ++rowSize; /* round up */
    for (i = 0; i < ny; ++i) {
      startPtr = datPtr;
      done = 0;
      for (j = 0; j < nx; ++j) {
	for (k=0; k<3; ++k) {
	  AINT(startPtr, prec, iColrs[k], done);
	  ++done;
	}
	*retPtr++ = getPixel(cgm_s, iColrs);
      }
      datPtr += rowSize;
    }
    break;
  case I_C_M:
    rowSize = (nx * prec + 7) / 8; /* no of bytes per row */
    if (rowSize % 2) ++rowSize; /* round up */
    for (i = 0; i < ny; ++i) {
      startPtr = datPtr;
      done = 0;
      for (j = 0; j < nx; ++j) {
	AINT(startPtr, prec, iCol, done);
	++done;
	*retPtr++ = getIPixel(iCol);
      }
      datPtr += rowSize;
    }
    break;
  default: /* nothing */ break;
  }
}
/* get a run-length encoded list */
static void getRLPixels(unsigned char *dat_ptr, cgm_s_type *cgm_s,
			int nx, int ny, int prec, Pixel *retPtr)
{
  static unsigned char bitMask[8] = {255, 127, 63, 31, 15, 7, 3, 1};
  unsigned char *rowPtr;
  Pixel myPixel;
  int i, j, k, done, iColrs[3], bitOffset,count;
  int iCol = 0;
  Pixel tP;

  for (i=0; i<ny; ++i) { /* loop through the rows */
    done = j = bitOffset = 0;
    rowPtr = dat_ptr;
    while (j < nx) { /* still some left in this row */
      /* pick up the count of repeated colours, may span bytes */
      count = (bitOffset) ?
	(dat_ptr[0] & bitMask[bitOffset] << (bitOffset + 8)) +
	(dat_ptr[1] << bitOffset) +
	(dat_ptr[2] >> (8 - bitOffset)) :
	(dat_ptr[0] << 8) + dat_ptr[1];
      if ((j + count) > nx) return; /* too many pixels */
      dat_ptr += 2;
      done += 16 / prec;
      /* now get the next colour and replicate it count times */
      switch (cgm_s->colselmode) {
      case D_C_M: /* direct colour */
	/* get the r, g, b values */
	for (k=0; k<3; ++k) {
	  AINT(dat_ptr, prec, iColrs[k], done);
	  ++done;
	}
	myPixel = getPixel(cgm_s, iColrs);
	for (k=0; k<count; ++k) retPtr[i * nx + j + k] = myPixel;
	j += count;
	break;
      case I_C_M:
	/* get one pixel */
	AINT(dat_ptr, prec, iCol, done);
	tP = getIPixel(iCol);
	/* and replicate it */
	for (k=0; k<count; ++k) {
	  retPtr[i * nx + j + k] = tP
;
	}
	++done;
	j += count;
	break;
      default: /* nothing */ break;
      } /* end of switch */
      bitOffset = (done * prec) % 8;
    } /* end of row */
    /* may have to skip to end of row */
    if (bitOffset) ++dat_ptr; /* incomplete byte */
    if (((size_t) dat_ptr - (size_t) rowPtr) % 2) ++dat_ptr; /* need round up */
  } /* end of cell array */
}


/* build a pixel array, (x, y) = (x + y * nx) storage */
static Pixel *getPixels(unsigned char *dat_ptr, cgm_s_type *cgm_s,
			int nx, int ny, int localPrec, int repMode)
{
  Pixel *retPtr;
  retPtr = (Pixel *) malloc(nx * ny * sizeof(Pixel));
  if (!retPtr) return NULL;
  /* get the local colour precision */
  if (!localPrec) localPrec =
		    (cgm_s->colselmode) ? cgm_s->colprec :cgm_s->cindprec;

  /* get the pixels for each case */
  if (repMode) { /* packed list */
    getListPixels(dat_ptr, cgm_s, nx, ny, localPrec, retPtr);
  } else { /* run-length encoded */
    getRLPixels(dat_ptr, cgm_s, nx, ny, localPrec, retPtr);
  }
  return retPtr;
}
/* actually get the cellarray command */
static int cellarray(dat_ptr, cmd_ptr, cgm_s)
     unsigned char *dat_ptr;
     struct cmd_info_s *cmd_ptr;
     cgm_s_type *cgm_s;
{
  int i, x[3], y[3], nx, ny, local_prec, rep_mode, xSize, ySize;
  int Qx, Qy, Rx, Ry, det, xMin, xMax, yMin, yMax, newX, newY, ix, iy,
  oldX, oldY;
  double ddet, axx, axy, ayx, ayy;
  XImage *myImage;
  Pixel *myPixels;
  
  /* first get the corner co-ordinates */
  for (i=0; i<3; ++i) {
    x[i] = getX(&dat_ptr, cgm_s);
    y[i] = getY(&dat_ptr, cgm_s);
  }
  /* now the bit array specifics */
  nx = b_gsint(&dat_ptr, cgm_s->intprec);
  ny = b_gsint(&dat_ptr, cgm_s->intprec);
  local_prec = b_gsint(&dat_ptr, cgm_s->intprec);
  rep_mode = b_gsint(&dat_ptr, 16);
  /* get the expanded cell array */
  if (!(myPixels = getPixels(dat_ptr, cgm_s, nx, ny, local_prec, rep_mode)))
    return 0;
  
  /* make sure we're not co-linear */
  Qx = x[1] - x[0];
  Qy = y[1] - y[0];
  Rx = x[2] - x[0];
  Ry = y[2] - y[0];
  /* figure out the determinant */
  if (!(det = Rx * Qy - Ry * Qx)) return 1; /* co-linear */
  ddet = (double) det; /* explicit cast for Ultrix bug */
  /* figure out the transformation matrix */
  /* need nx+1, ny+1, because o integer arithmetic expanding range */
  axx = (nx + 1) * (Qy - Ry) / ddet;
  axy = (nx + 1) * (Rx - Qx) / ddet;
  ayx = - (ny + 1) * Ry / ddet;
  ayy = (ny + 1)  * Rx / ddet;
  /* figure out the 4th pt and use it as beginning of bounding rectangle */
  xMin = xMax = x[0] + x[1] - x[2];
  yMin = yMax = y[0] + y[1] - y[2];
  for (i=0; i<3; ++i) {
    if (x[i] < xMin) xMin = x[i];
    if (x[i] > xMax) xMax = x[i];
    if (y[i] < yMin) yMin = y[i];
    if (y[i] > yMax) yMax = y[i];
  }
  /* make sure we're inside the pixmap */
  if (xMin < 0) xMin = 0;
  if (yMin < 0) yMin = 0;
  if (xMax > max_x) xMax = max_x;
  if (yMax > max_y) yMax = max_y;
  /* get the image */
  xSize = xMax - xMin;
  ySize = yMax - yMin;
  myImage = XGetImage(display, P, xMin, max_y - yMax, xSize, ySize,
		      ~0, ZPixmap);
  /* now go thru the screen pixels */
  for (iy=1; iy <= ySize; ++iy) {
    /* get the real Image y coord, relative to (x0, y0) */
    oldY = iy - (y[0] - yMin);
    for (ix=0; ix<xSize; ++ix) {
      /* get the real Image x coord, relative to (x0, y0) */
      oldX = ix - (x[0] - xMin);
      /* see if we have data for the transformed point */
      newX = (int) (axx * oldX + axy * oldY - 0.99);
      newY = (int) (ayx * oldX + ayy * oldY - 0.99);
      if ((newX >= 0) && (newX < nx) && (newY >= 0) && (newY < ny)) {
	XPutPixel(myImage, ix, ySize - iy, myPixels[newX + newY * nx]);
      }
    }
  }
  /* copy the XImage back */

  XPutImage(display, P, cellGC, myImage, 0, 0, xMin, max_y - yMax,
	    xSize, ySize);
  /* return the memory */
  XDestroyImage(myImage);
  free(myPixels);
  return 1;
}
/* Rectangle */
static int rectangle(dat_ptr, cmd_ptr, cgm_s)
     unsigned char *dat_ptr;
     struct cmd_info_s *cmd_ptr;
     cgm_s_type *cgm_s;
{
  int i, xPts[2], yPts[2], x, y, old_style;
  unsigned int w, h;
  
  /* just 2 pts to grab */
  for (i=0; i<2; ++i) {
    xPts[i] = getX(&dat_ptr, cgm_s);
    yPts[i] = max_y - getY(&dat_ptr, cgm_s);
  }
  /* now arrange them to the satisfaction of XLib */
  if (xPts[0] < xPts[1]) {
    w = xPts[1] - xPts[0];
    x = xPts[0];
  } else {
    w = xPts[0] - xPts[1];
    x = xPts[1];
  }
  if (yPts[0] < yPts[1]) {
    h = yPts[1] - yPts[0];
    y = yPts[0];
  } else {
    h = yPts[0] - yPts[1];
    y = yPts[1];
  }
  /* and fill the rectangle */
  XFillRectangle(display, P, fillGC, x, y, w, h);
  /* and the edge, have to make sure corners look OK */
  /* store the old value of cap style */
  XGetGCValues(display, edgeGC, GCCapStyle, &myStruct);
  old_style = myStruct.cap_style;
  myStruct.cap_style = CapProjecting;
  XChangeGC(display, edgeGC, GCCapStyle, &myStruct);  
  /* draw the lines */
  XDrawLine(display, P, edgeGC, x, y, x + w, y);
  XDrawLine(display, P, edgeGC, x + w, y, x + w, y + h);
  XDrawLine(display, P, edgeGC, x + w, y + h, x, y + h);
  XDrawLine(display, P, edgeGC, x, y + h, x, y);
  /* restore the old value */
  myStruct.cap_style = old_style;
  XChangeGC(display, edgeGC, GCCapStyle, &myStruct);  
  return 1;
}
/* Circle */
static int circle(dat_ptr, cmd_ptr, cgm_s)
     unsigned char *dat_ptr;
     struct cmd_info_s *cmd_ptr;
     cgm_s_type *cgm_s;
{
  int x, y, r;
  /* get the center position and radius */
  x = getX(&dat_ptr, cgm_s);
  y = max_y - getY(&dat_ptr, cgm_s);
  r = getVDC(&dat_ptr, cgm_s);
  if (r < 0) return 0;
  /* fill the circle */
  XSetArcMode(display, fillGC, ArcChord); /* for safety */
  XFillArc(display, P, fillGC, x - r, y - r, 2 * r, 2 * r, 0, 360 * 64);
  /* draw the edge */
  XDrawArc(display, P, edgeGC, x - r, y - r, 2 * r, 2 * r, 0, 360 * 64);
  return 1;
}
/* get radius, center and angles from 3 pts */
static int getArc(float inX[3], float inY[3], int *outXC, int *outYC,
		  double *outTheta0, double *outDtheta)
{
  int i, r, xc, yc;
  double theta[3], dtheta;
  float m1 = 0;
  float m2, x[5], y[5];
  /* for convenience */
  for (i=0; i<3; ++i) {
    x[i] = inX[i];
    y[i] = inY[i];
  }
  /* get the midpoints between the pts */
  for (i=0; i<2; ++i) {
    x[i+3] = (x[i] + x[i+1]) / 2;
    y[i+3] = (y[i] + y[i+1]) / 2;
  }
  /*
    the perpendiculars to the 2 lines thru their midpts will meet at the center
    thus we have 4 eqns for 4 unknowns:
    xc = x3 + a * (y1 - y0)
    yc = y3 - a * (x1 - x0)
    xc = x4 + b * (y2 - y1)
    yc = y4 - b * (x2 - x1)
    we can eliminate a and b to find xc and yc
    (xc - x3) / (y1 - y0) = - (yc - y3) / (x1 - x0)
    (xc - x4) / (y2 - y1) = - (yc - y4) / (x2 - x1)
  */
  /* get xc first */
  if (y[1] == y[0]) xc = x[3];
  else if (y[2] == y[1]) xc = x[4];
  else { /* divisors are non-zero, get radii slopes */
    m1 = -(x[1] - x[0]) / (y[1] - y[0]);
    m2 = -(x[2] - x[1]) / (y[2] - y[1]);
    /* (xc - x3) * m1 + y3 = (xc - x4) * m2 + y4, i.e.,
       xc * (m1 - m2) = m1 * x3 - m2 * x4 + y4 - y3 */
    xc = (m1 * x[3] - m2 * x[4] + y[4] - y[3]) / (m1 - m2);
  }
  if (xc == x[3]) yc = y[3];
  else yc = y[3] + m1 * (xc - x[3]);
   /* have the center, now get the radius */
#define SQ(x) ((double) ((x) * (x)))
   r = sqrt(SQ(x[0] - xc) + SQ(y[0] - yc));

  /* now get the angles */

  for (i=0; i<3; ++i) {
    theta[i] = atan2(y[i] - yc, x[i] - xc);
    if (theta[i] < 0) theta[i] += 2 * pi; /* for XLib coords */
  }
  /* figure out the delta theta angle */
  dtheta = theta[2] - theta[0];
  if (theta[1] > theta[0]) { /* need to go clockwise */
    if (dtheta < 0) dtheta += 2 * pi;
  } else { /* need to go anticlockwise */
    if (dtheta > 0) dtheta -= 2 * pi;
  }
  *outXC = xc;
  *outYC = yc;
  *outTheta0 = theta[0];
  *outDtheta = dtheta;
  return r;
}
/* Circular Arc, set by 3 points */
static int circ3(dat_ptr, cmd_ptr, cgm_s)
     unsigned char *dat_ptr;
     struct cmd_info_s *cmd_ptr;
     cgm_s_type *cgm_s;
{
  int xc, yc, r, i;
  double theta0, dtheta;
  float det1, x[3], y[3];

  /* just 3 pts to grab */
  for (i=0; i<3; ++i) {
    x[i] = getX(&dat_ptr, cgm_s);
    y[i] = getY(&dat_ptr, cgm_s);
  }
  /* get the determinant, find out if they're co-linear */
  det1 = (x[2] - x[0]) * (y[1] - y[0])
    - (x[1] - x[0]) * (y[2] - y[0]);
  if (det1 == 0) { /* co-linear, just connect points */
    XDrawLine(display, P, lineGC, x[0], max_y - y[0], x[1], max_y - y[1]);
    XDrawLine(display, P, lineGC, x[1], max_y - y[1], x[2], max_y - y[2]);
    return 1;
  }
  /* now need to figure out arc in appropriate coordinates */
  r = getArc(x, y, &xc, &yc, &theta0, &dtheta);
  /* convert to left handed XLib coords */
  yc = max_y - yc;
  if (r == 0) return 0;
  XDrawArc(display, P, lineGC, xc - r, yc - r, 2 * r, 2 * r,
	   (int) ((theta0 / pi) * 180 * 64),
	   (int) ((dtheta / pi) * 180 * 64));
  return 1;
}
/* Circular Arc, set by 3 points, close */
static int circ3close(dat_ptr, cmd_ptr, cgm_s)
     unsigned char *dat_ptr;
     struct cmd_info_s *cmd_ptr;
     cgm_s_type *cgm_s;
{
  int xc, yc, r, i, close_type, x0, y0, x2, y2;
  float det1, x[3], y[3]; /* use floats since we must manipulate them */
  double theta0, dtheta;
  /* just 3 pts plus an eint to grab */
  for (i=0; i<3; ++i) {
    x[i] = getX(&dat_ptr, cgm_s);
    y[i] = getY(&dat_ptr, cgm_s);
  }
  close_type = b_gsint(&dat_ptr, 16); /* how do we close the arc ? */
  /* get the determinant, find out if they're co-linear */
  det1 = (x[2] - x[0]) * (y[1] - y[0])
    - (x[1] - x[0]) * (y[2] - y[0]);
  if (det1 == 0) { /* co-linear, just connect points */
    XDrawLine(display, P, lineGC, x[0], max_y - y[0], x[1], max_y - y[1]);
    XDrawLine(display, P, lineGC, x[1], max_y - y[1], x[2], max_y - y[2]);
    return 1;
  }
  /* now need to figure out arc in appropriate coordinates */
  r = getArc(x, y, &xc, &yc, &theta0, &dtheta);
  if (r == 0) return 0;
  /* convert to left handed XLib coords */
  yc = max_y - yc;
  /* need the ends, use angles so things match up */
  x0 = (int) (xc + r * cos(theta0));
  y0 = (int) (yc - r * sin(theta0));
  x2 = (int) (xc + r * cos(theta0 + dtheta));
  y2 = (int) (yc - r * sin(theta0 + dtheta));
  /* fill the arc, use the correct type of close */
  if (close_type) XSetArcMode(display, fillGC, ArcChord);
  else XSetArcMode(display, fillGC, ArcPieSlice);
  /* now the actual fill */
  XFillArc(display, P, fillGC, xc - r, yc - r, 2 * r, 2 * r,
	   (int) ((theta0 / pi) * 180 * 64),
	   (int) ((dtheta / pi) * 180 * 64));
  /* and draw the edge */
  XDrawArc(display, P, edgeGC, xc - r, yc - r, 2 * r, 2 * r,
	   (int) ((theta0 / pi) * 180 * 64),
	   (int) ((dtheta / pi) * 180 * 64));
  /* and close it */
  if (close_type) { /* chord closure */
    XDrawLine(display, P, edgeGC, x0, y0, x2, y2);
  } else { /* pie closure */
    XDrawLine(display, P, edgeGC, x0, y0, xc, yc);
    XDrawLine(display, P, edgeGC, xc, yc, x2, y2);
  }
  return 1;
}
/* return angles from directions in radians */
static double getAngles(int xc, int yc, float dxy[4], double  *outTheta0)
{
  int i;
  double theta[2], dtheta;
  for (i=0; i<2; ++i) {
    theta[i] = atan2(dxy[2 * i + 1], dxy[2 * i]);
    if (theta[i] < 0) theta[i] += 2 * pi; /* for XLib coords */
  }
  dtheta = theta[1] - theta[0];
  /* need to go in the positive direction */
  if (dtheta < 0) dtheta += 2 * pi;
  *outTheta0 = theta[0];
  return dtheta;
}
/* Circular Arc, set by center */
static int circcentre(dat_ptr, cmd_ptr, cgm_s)
     unsigned char *dat_ptr;
     struct cmd_info_s *cmd_ptr;
     cgm_s_type *cgm_s;
{
  int i, xc, yc, r;
  double theta0, dtheta;
  float dxy[4];
  /* get the center */
  xc = getX(&dat_ptr, cgm_s);
  yc = getY(&dat_ptr, cgm_s);
  /* get the tangent vectors */
  for (i=0; i<4; ++i) dxy[i] = getVDC(&dat_ptr, cgm_s);
  /* get the radius */
  r = getVDC(&dat_ptr, cgm_s);
  if (r == 0) return 1;
  /* get the angles */
  dtheta = getAngles(xc, yc, dxy, &theta0);
  if (dtheta == 0) return 1;
  /* convert to left handed XLib coords */
  yc = max_y - yc;
  XDrawArc(display, P, lineGC, xc - r, yc - r, 2 * r, 2 * r,
	   (int) ((theta0 / pi) * 180 * 64),
	   (int) ((dtheta / pi) * 180 * 64));
  
  return 1;
}
/* Circular Arc, set by center, close */
static int circcclose(dat_ptr, cmd_ptr, cgm_s)
     unsigned char *dat_ptr;
     struct cmd_info_s *cmd_ptr;
     cgm_s_type *cgm_s;
{
  int i, xc, yc, r, close_type, x0, y0, x1, y1;
  double theta0, dtheta;
  float dxy[4];
  /* get the center */
  xc = getX(&dat_ptr, cgm_s);
  yc = getY(&dat_ptr, cgm_s);
  /* get the tangent vectors */
  for (i=0; i<4; ++i) dxy[i] = getVDC(&dat_ptr, cgm_s);
  /* get the radius */
  r = getVDC(&dat_ptr, cgm_s);
  /* get the close type */
  close_type = b_gsint(&dat_ptr, 16);
  /* get the angles */
  dtheta = getAngles(xc, yc, dxy, &theta0);
  if (dtheta == 0) return 1;
  /* convert to left handed XLib coords */
  yc = max_y - yc;
  /* need to get the ends */
  x0 = xc + r * cos(theta0);
  y0 = yc - r * sin(theta0);
  x1 = xc + r * cos(theta0 + dtheta);
  y1 = yc - r * sin(theta0 + dtheta);
  /* fill the arc, use the correct type of close */
  if (close_type) XSetArcMode(display, fillGC, ArcChord);
  else XSetArcMode(display, fillGC, ArcPieSlice);
  XFillArc(display, P, fillGC, xc - r, yc - r, 2 * r, 2 * r,
	   (int) ((theta0 / pi) * 180 * 64),
	   (int) ((dtheta / pi) * 180 * 64));
  /* now draw the edge */
  XDrawArc(display, P, edgeGC, xc - r, yc - r, 2 * r, 2 * r,
	   (int) ((theta0 / pi) * 180 * 64),
	   (int) ((dtheta / pi) * 180 * 64));
  /* and close it */
  if (close_type) { /* chord closure */
    XDrawLine(display, P, edgeGC, x0, y0, x1, y1);
  } else { /* pie closure */
    XDrawLine(display, P, edgeGC, x0, y0, xc, yc);
    XDrawLine(display, P, edgeGC, xc, yc, x1, y1);
  }

  return 1;
}
/* generate a set of points for an elliptical outline between 2 angles */
/* assume centred at the origin, we get two conjugate diameters */
/* dxy gives tangent of angles */
/* 
  when the ellipse is parallel to the axes we have:
  x = a cos(theta), y = b sin(theta), where theta is parametric
  when we rotate thru an angle alpha we have
  x = a cos(theta) * cos(alpha) - b sin(theta) * sin(alpha),
  y = b sin(theta) * cos(alpha) + a * cos(theta) * sin(alpha)
  use formulae and variable names from theory of conjugate diameters
  (not misleading formulae in ANSI document)
*/
static int getEllipseXPoints(int x1, int y1, int x2, int y2,
			     float dxy[4], XPoint **outPtr)
{
  int nSteps, i, rot;
  double D, S, alpha, a, b, ca, sa, c0, c1, s0, s1, cd, sd;
  double meanR, theta1, theta2, dtheta, alpha1, alpha2, dalpha, delta;
  double argx, argy;
  XPoint *myPtr;
  D = x2 * y1 - y2 * x1;
  S = (SQ(x1) + SQ(x2) + SQ(y1) + SQ(y2)) / 2.0;
  /* the two radii */
  a = sqrt(S + sqrt(SQ(S) - SQ(D)));
  b = sqrt(S - sqrt(SQ(S) - SQ(D)));
  /* the angle of axes rotation */
  argy = 2.0 * (x1 * y1 + x2 * y2);
  argx = (SQ(x1) + SQ(x2) - SQ(y1) - SQ(y2)) / 2.0;

  /* atan2(y,x) takes the arctangent of y/x. One would assumen
  ** that if x is zero, then things blow up. This is not so for
  ** solaris. It seems that the solris math lib knows how to handle
  ** atan(0,0) which is the case here. However, things blow up on
  ** hp. So, if argx is 0, then make argy zero and argx 1.
  */
  if (argx == 0)
  {
    argy = 0;
    argx = 1;
  }
  alpha = atan2(argy, argx);
  ca = cos(alpha);
  sa = sin(alpha);
  /* we need to figure out the direction of parametric rotation */
  /* we always go in the direction from CD 1 to 2 thru smallest angle */
  alpha1 = atan2((double) y1, (double) x1);
  alpha2 = atan2((double) y2, (double) x2);
  dalpha = (alpha2 > alpha1) ? alpha2 - alpha1 : alpha1 - alpha2;
  rot = ((dalpha <= pi) && (alpha2 > alpha1)) ||
    ((dalpha <= pi) && (alpha2 < alpha1)) ? 1 : -1;
  /* we need to relate the incoming angles to the parametric angles */
  argy = a * (ca * dxy[1] - sa * dxy[0]);
  argx = b * (ca * dxy[0] + sa * dxy[1]);
  if (argx == 0)
  {
    argy = 0;
    argx = 1;
  }
  theta1 = atan2(argy, argx);

  argy = a * (ca * dxy[3] - sa * dxy[2]);
  argx = b * (ca * dxy[2] + sa * dxy[3]);
  if (argx == 0)
  {
    argy = 0;
    argx = 1;
  }
  theta2 = atan2(argy, argx);
/*
  theta1 = atan2(a * (ca * dxy[1] - sa * dxy[0]),
		 b * (ca * dxy[0] + sa * dxy[1]));
  theta2 = atan2(a * (ca * dxy[3] - sa * dxy[2]),
		 b * (ca * dxy[2] + sa * dxy[3]));
*/
  /* need to figure out angle to move thru in parameter space */
  /* always start at theta1 and move to theta2 */
  /* note (theta1 == theta2) => delta = 2 * pi */
  if (rot > 0) { /* move in clockwise direction */
    delta = (theta2 > theta1) ? theta2 - theta1 : theta2 + 2 * pi - theta1;
  } else { /* move in anticlockwise direction */
    delta = (theta2 < theta1) ? theta2 - theta1 : theta2 - 2 * pi - theta1;
  }
  /* choose a reasonable number of steps */
  meanR = sqrt(SQ(a) + SQ(b)) / 2;
  /* relate number of steps to circumference */
  nSteps = meanR * delta;
  if (nSteps < 0) nSteps = - nSteps;
  ++nSteps; /* so that we always have at least 2 points */

  dtheta = delta / nSteps;
  /* make the points memory, always add two so it can be closed */
  myPtr = (XPoint *) malloc(sizeof(XPoint) * (nSteps + 2));
  if (myPtr == NULL)
    return 0;
  /* and fill it out */
  c0 = cos(theta1);
  s0 = sin(theta1);
  cd = cos(dtheta);
  sd = sin(dtheta);
  for (i=0; i<nSteps; ++i) {
    myPtr[i].x = a * c0 * ca - b * s0 * sa;
    myPtr[i].y = a * c0 * sa + b * s0 * ca;
    /* use recursion relation to step cos and sin */
    c1 = c0 * cd - s0 * sd;
    s1 = c0 * sd + s0 * cd;
    c0 = c1;
    s0 = s1;
  }
  /* fill out the return */
  *outPtr = myPtr;
  return nSteps;
}
/* Ellipse */
static int ellipse(dat_ptr, cmd_ptr, cgm_s)
     unsigned char *dat_ptr;
     struct cmd_info_s *cmd_ptr;
     cgm_s_type *cgm_s;
{
  int xc, yc, cdx0, cdy0, cdx1, cdy1, noPts, i;
  XPoint *myPtr = NULL;
  static float dxy[4] = {1, 0, 1, 0}; /* make a full ellipse */
  /* get the Ellipse centre */
  xc = getX(&dat_ptr, cgm_s);
  yc = getY(&dat_ptr, cgm_s);
  /* get the first conjugate diameter endpoint */
  cdx0 = getX(&dat_ptr, cgm_s);
  cdy0 = getY(&dat_ptr, cgm_s);
  /* get the second conjugate diameter endpoint */
  cdx1 = getX(&dat_ptr, cgm_s);
  cdy1 = getY(&dat_ptr, cgm_s);
  /* have to go ahead and get the points on the curve for a polyline call */
  /* subtract out the centre for simplicity */
  noPts = getEllipseXPoints(cdx0 - xc, cdy0 - yc, cdx1 - xc, cdy1 - yc,
			    dxy, &myPtr);
  if (noPts == 0) return 0;
  /* convert to LH co-ord system, add centre back in */
  for (i=0; i<noPts; ++i) {
    myPtr[i].x = xc + myPtr[i].x;
    myPtr[i].y = max_y - (yc + myPtr[i].y);
  }
  /* make sure it closes */
  myPtr[noPts].x = myPtr[0].x;
  myPtr[noPts].y = myPtr[0].y;
  /* fill the ellipse */
  XFillPolygon(display, P, fillGC, myPtr, noPts, Convex, CoordModeOrigin);
  /* draw the outline */
  XDrawLines(display, P, edgeGC, myPtr, noPts + 1, CoordModeOrigin);  
  free(myPtr);
  return 1;
}
/* Elliptical arc */
static int elarc(dat_ptr, cmd_ptr, cgm_s)
     unsigned char *dat_ptr;
     struct cmd_info_s *cmd_ptr;
     cgm_s_type *cgm_s;
{
  int xc, yc, cdx0, cdy0, cdx1, cdy1, i, noPts;
  float dxy[4];
  XPoint *myPtr = NULL;
  /* get the Ellipse centre */
  xc = getX(&dat_ptr, cgm_s);
  yc = getY(&dat_ptr, cgm_s);
  /* get the first conjugate diameter endpoint */
  cdx0 = getX(&dat_ptr, cgm_s);
  cdy0 = getY(&dat_ptr, cgm_s);
  /* get the second conjugate diameter endpoint */
  cdx1 = getX(&dat_ptr, cgm_s);
  cdy1 = getY(&dat_ptr, cgm_s);
  /* get the tangent vectors */
  for (i=0; i<4; ++i) dxy[i] = getVDC(&dat_ptr, cgm_s);
  /* have to go ahead and get the points on the curve for a polyline call */
  /* subtract out the centre for simplicity */
  noPts = getEllipseXPoints(cdx0 - xc, cdy0 - yc, cdx1 - xc, cdy1 - yc,
			    dxy, &myPtr);
  if (noPts == 0) return 0;
  /* convert to LH co-ord system, add centre back in */
  for (i=0; i<noPts; ++i) {
    myPtr[i].x = xc + myPtr[i].x;
    myPtr[i].y = max_y - (yc + myPtr[i].y);
  }
  /* draw the outline */
  XDrawLines(display, P, lineGC, myPtr, noPts, CoordModeOrigin);  
  free(myPtr);
  
  return 1;
}
/* Elliptical arc, close */
static int elarcclose(dat_ptr, cmd_ptr, cgm_s)
     unsigned char *dat_ptr;
     struct cmd_info_s *cmd_ptr;
     cgm_s_type *cgm_s;
{
  int xc, yc, cdx0, cdy0, cdx1, cdy1, i, close_type, noPts;
  float dxy[4];
  XPoint *myPtr = NULL;
  /* get the Ellipse centre */
  xc = getX(&dat_ptr, cgm_s);
  yc = getY(&dat_ptr, cgm_s);
  /* get the first conjugate diameter endpoint */
  cdx0 = getX(&dat_ptr, cgm_s);
  cdy0 = getY(&dat_ptr, cgm_s);
  /* get the second conjugate diameter endpoint */
  cdx1 = getX(&dat_ptr, cgm_s);
  cdy1 = getY(&dat_ptr, cgm_s);
  /* get the tangent vectors */
  for (i=0; i<4; ++i) dxy[i] = getVDC(&dat_ptr, cgm_s);
  /* get the close type */
  close_type = b_gsint(&dat_ptr, 16);
  /* have to go ahead and get the points on the curve for a polyline call */
  /* subtract out the centre for simplicity */
  noPts = getEllipseXPoints(cdx0 - xc, cdy0 - yc, cdx1 - xc, cdy1 - yc,
			    dxy, &myPtr);
  if (noPts == 0) return 0;
  /* convert to LH co-ord system, add centre back in */
  for (i=0; i<noPts; ++i) {
    myPtr[i].x = xc + myPtr[i].x;
    myPtr[i].y = max_y - (yc + myPtr[i].y);
  }
  /* now add in the closure */
  if (close_type) { /* chord closure */
    myPtr[noPts].x = myPtr[0].x;
    myPtr[noPts].y = myPtr[0].y;
    ++noPts;
  } else { /* pie closure */
    myPtr[noPts].x = xc;
    myPtr[noPts].y = max_y - yc;
    myPtr[noPts + 1].x = myPtr[0].x;
    myPtr[noPts + 1].y = myPtr[0].y;
    noPts += 2;
  }
  /* fill the arc */
  XFillPolygon(display, P, fillGC, myPtr, noPts, Convex, CoordModeOrigin);  
  /* draw the outline */
  XDrawLines(display, P, edgeGC, myPtr, noPts, CoordModeOrigin);  
  free(myPtr);
  
  return 1;
}
/* Attribute elements */
static int setLineType(int inType, GC inGC)
{
  /* set the GC */
  switch(inType) {
  case 1: /* solid */
    myStruct.line_style = LineSolid;
    XChangeGC(display, inGC, GCLineStyle, &myStruct);
    break;
  case 2: /* dash */
    myStruct.line_style = LineOnOffDash;
    XChangeGC(display, inGC, GCLineStyle, &myStruct);
    XSetDashes(display, inGC, 0, lineArray, 2);
    break;
  case 3: /* dot */
    myStruct.line_style = LineOnOffDash;
    XChangeGC(display, inGC, GCLineStyle, &myStruct);
    XSetDashes(display, inGC, 0, lineArray + 2, 2);
    break;
  case 4: /* dash-dot */
    myStruct.line_style = LineOnOffDash;
    XChangeGC(display, inGC, GCLineStyle, &myStruct);
    XSetDashes(display, inGC, 0, lineArray, 4);
    break;
  case 5: /* dash-dot-dot */
    myStruct.line_style = LineOnOffDash;
    XChangeGC(display, inGC, GCLineStyle, &myStruct);
    XSetDashes(display, inGC, 0, lineArray, 6);
    break;
  default: return 1; /* no idea */
  }
  return 1;
}
/* Line Type */
static int ltype(dat_ptr, cmd_ptr, cgm_s)
     unsigned char *dat_ptr;
     struct cmd_info_s *cmd_ptr;
     cgm_s_type *cgm_s;
{
  /* get the line type */
  cgm_s->ltype = b_gsint(&dat_ptr, cgm_s->indexprec);
  return setLineType(cgm_s->ltype, lineGC);
}
/* Line Width */
static int lwidth(dat_ptr, cmd_ptr, cgm_s)
     unsigned char *dat_ptr;
     struct cmd_info_s *cmd_ptr;
     cgm_s_type *cgm_s;
{
  switch (cgm_s->lwidspecmode) {
  case ABSOLUTE:
    switch (cgm_s->vdctype) {
    case VDC_INT: cgm_s->lwidth.i = xScale * b_gsint(&dat_ptr, cgm_s->vdcintprec);
      break;
    case VDC_REAL: cgm_s->lwidth.r = xScale * b_real(&dat_ptr, &(cgm_s->vdcrprec));
      cgm_s->lwidth.i = cgm_s->lwidth.r;
      break;
    }
    break;
  case SCALED:	cgm_s->lwidth.s = b_real(&dat_ptr, &(cgm_s->realprec));
    cgm_s->lwidth.i = cgm_s->lwidth.s; /* scale from 1 */
    break;
  }
  /* some servers can't handle widths of 1 */
  myStruct.line_width = (cgm_s->lwidth.i < 2) ? 0 : cgm_s->lwidth.i;
  XChangeGC(display, lineGC, GCLineWidth, &myStruct);
  return 1;
}
/* Line Colour */
static int lcolr(dat_ptr, cmd_ptr, cgm_s)
     unsigned char *dat_ptr;
     struct cmd_info_s *cmd_ptr;
     cgm_s_type *cgm_s;
{
  int i;
  
  /* may be in direct colour mode or indexed colour mode */
  switch (cgm_s->colselmode) {
  case D_C_M:	for (i=0; i<3; ++i) cgm_s->lcolr[i] = 
    b_guint(&dat_ptr, cgm_s->colprec);
    setRGB(cgm_s, cgm_s->lcolr, lineGC);
    break;
  case I_C_M:	cgm_s->lcolr[3] = b_guint(&dat_ptr, cgm_s->cindprec);
    setIndex(cgm_s, cgm_s->lcolr[3], lineGC);
    break;
  default: /* nothing */ break;
  }
  return 1;
}
/* Marker Type */
static int mtype(dat_ptr, cmd_ptr, cgm_s)
     unsigned char *dat_ptr;
     struct cmd_info_s *cmd_ptr;
     cgm_s_type *cgm_s;
{
  /* get the line type */
  cgm_s->mtype = b_gsint(&dat_ptr, cgm_s->indexprec);
  
  return 1;
}
/* Marker Size */
static int msize(dat_ptr, cmd_ptr, cgm_s)
     unsigned char *dat_ptr;
     struct cmd_info_s *cmd_ptr;
     cgm_s_type *cgm_s;
{
  switch (cgm_s->marksizspecmode) {
  case ABSOLUTE:
    switch (cgm_s->vdctype) {
    case VDC_INT:	
    case VDC_REAL:
      cgm_s->msize.i = getVDC(&dat_ptr, cgm_s);
      break;
    }
    break;
  case SCALED:	cgm_s->msize.s = b_real(&dat_ptr, &(cgm_s->realprec));
    cgm_s->msize.i = (int) (cgm_s->msize.s * 5);
    break;
  }
  return 1;
}
/* Marker Colour */
static int mcolr(dat_ptr, cmd_ptr, cgm_s)
     unsigned char *dat_ptr;
     struct cmd_info_s *cmd_ptr;
     cgm_s_type *cgm_s;
{
  int i;
  /* may be in direct colour mode or indexed colour mode */
  switch (cgm_s->colselmode) {
  case D_C_M:	for (i=0; i<3; ++i) cgm_s->mcolr[i] = 
    b_guint(&dat_ptr, cgm_s->colprec);
    setRGB(cgm_s, cgm_s->mcolr, markGC);
    break;
  case I_C_M:	cgm_s->mcolr[3] = b_guint(&dat_ptr, cgm_s->cindprec);
    setIndex(cgm_s, cgm_s->mcolr[3], markGC);
    break;
  }
  return 1;
}
/* Text Font Index, ignored at the moment */
static int tfindex(unsigned char *dat_ptr, struct cmd_info_s *cmd_ptr,
		   cgm_s_type *cgm_s)
{
  /* get the text font index */
  cgm_s->tfindex = b_gsint(&dat_ptr, cgm_s->indexprec);
  
  return 1;
}
/* Text Precision, ignored at the moment; stroke everything */
static int tprec(unsigned char *dat_ptr, struct cmd_info_s *cmd_ptr,
		 cgm_s_type *cgm_s)
{
  /* get the text font index */
  cgm_s->tprec = b_gsint(&dat_ptr, 16);
  
  return 1;
}
/* Character Expansion Factor */
static int cexpfac(unsigned char *dat_ptr, struct cmd_info_s *cmd_ptr,
		   cgm_s_type *cgm_s)
{
  cgm_s->cexpfac = b_real(&dat_ptr, &(cgm_s->realprec));
  
  return 1;
}
/* Character Space */
static int cspace(unsigned char *dat_ptr, struct cmd_info_s *cmd_ptr,
		  cgm_s_type *cgm_s)
{
  cgm_s->cspace = b_real(&dat_ptr, &(cgm_s->realprec));
  
  return 1;
}
/* Text Colour */
static int tcolr(unsigned char *dat_ptr, struct cmd_info_s *cmd_ptr,
		 cgm_s_type *cgm_s)
{
  int i, index;
  
  /* may be in direct colour mode or indexed colour mode */
  switch (cgm_s->colselmode) {
  case D_C_M:
    for (i=0; i<3; ++i) {
      cgm_s->tcolr[i] = b_guint(&dat_ptr, cgm_s->colprec);
    }
    cgm_s->tPixel = getPixel(cgm_s, cgm_s->tcolr);
    break;
  case I_C_M:
    cgm_s->tcolr[3] = index = b_guint(&dat_ptr, cgm_s->cindprec);
    if ((index >= 0) && (index < MAXCOLRS) && coltabPixels[index]) {
      cgm_s->tPixel = coltabPixels[index];
    } else cgm_s->tPixel = blackPixel; 
    break;
  }
  
  return 1;
}
/* Character Height */
static int cheight(unsigned char *dat_ptr, struct cmd_info_s *cmd_ptr,
		   cgm_s_type *cgm_s)
{
  /* just 1 VDC to grab */
  cgm_s->cheight.i = getVDC(&dat_ptr, cgm_s);
  return 1;
}
/* Character Orientation */
static int corient(unsigned char *dat_ptr,struct cmd_info_s *cmd_ptr,
		   cgm_s_type *cgm_s)
{
  int i;
  
  /* just 4 VDC's to grab */
  switch (cgm_s->vdctype) {
  case VDC_INT:	for (i=0; i<4; ++i) 
    cgm_s->corient[i].i = b_gsint(&dat_ptr, cgm_s->vdcintprec);
    break;
  case VDC_REAL:	for (i=0; i<4; ++i) 
    cgm_s->corient[i].r = b_real(&dat_ptr, &(cgm_s->vdcrprec));
    break;
  }
  return 1;
}
/* Text Path */
static int tpath(unsigned char *dat_ptr, struct cmd_info_s *cmd_ptr,
		 cgm_s_type *cgm_s)
{
  cgm_s->tpath = b_gsint(&dat_ptr, 16);
  
  return 1;
}
/* Text Alignment */
static int talign(unsigned char *dat_ptr, struct cmd_info_s *cmd_ptr,
		  cgm_s_type *cgm_s)
{
  int i;
  
  /* first the 2 enumerated types */
  for (i=0; i<2; ++i) cgm_s->talign[i].i = b_gsint(&dat_ptr, 16);
  /* now the 2 real bits */
  for (i=0; i<2; ++i) cgm_s->talign[i].r = 
    b_real(&dat_ptr, &(cgm_s->realprec));
  
  return 1;
}
/* Character Set Index */
static int csetindex(dat_ptr, cmd_ptr, cgm_s)
     unsigned char *dat_ptr;
     struct cmd_info_s *cmd_ptr;
     cgm_s_type *cgm_s;
{
  cgm_s->csetindex = b_gsint(&dat_ptr, cgm_s->indexprec);
  
  return 1;
}
/* Alternate Character Set Index */
static int altcsetindex(dat_ptr, cmd_ptr, cgm_s)
     unsigned char *dat_ptr;
     struct cmd_info_s *cmd_ptr;
     cgm_s_type *cgm_s;
{
  cgm_s->altcsetindex = b_gsint(&dat_ptr, cgm_s->indexprec);
  
  return 1;
}
/* Interior Style */
static int intstyle(dat_ptr, cmd_ptr, cgm_s)
     unsigned char *dat_ptr;
     struct cmd_info_s *cmd_ptr;
     cgm_s_type *cgm_s;
{
  cgm_s->intstyle = b_gsint(&dat_ptr, 16);
  switch (cgm_s->intstyle) {
  case 0: /* hollow, implementation dependent, make it the same as empty */
  case 4: /* empty, don't fill */
    XSetFunction(display, fillGC, GXnoop);
    break;
  case 1: /* solid */
  case 2: /* pattern */
  case 3: /* hatch, fill all of these */
    XSetFunction(display, fillGC, GXcopy);
    XSetFillStyle(display, fillGC, FillSolid);
    break;
  default: /* do nothing */
    break;
  }
  
  return 1;
}
/* Fill Colour */
static int fillcolr(dat_ptr, cmd_ptr, cgm_s)
     unsigned char *dat_ptr;
     struct cmd_info_s *cmd_ptr;
     cgm_s_type *cgm_s;
{
  int i;
  
  /* may be in direct colour mode or indexed colour mode */
  switch (cgm_s->colselmode) {
  case D_C_M:	for (i=0; i<3; ++i) cgm_s->fillcolr[i] = 
    b_guint(&dat_ptr, cgm_s->colprec);
    setRGB(cgm_s, cgm_s->fillcolr, fillGC);
    break;
  case I_C_M:	cgm_s->fillcolr[3] = b_guint(&dat_ptr, cgm_s->cindprec);
    setIndex(cgm_s, cgm_s->fillcolr[3], fillGC);
    break;
  }
  
  return 1;
}
/* Hatch Index */
static int hatchindex(dat_ptr, cmd_ptr, cgm_s)
     unsigned char *dat_ptr;
     struct cmd_info_s *cmd_ptr;
     cgm_s_type *cgm_s;
{
  cgm_s->hatchindex = b_gsint(&dat_ptr, cgm_s->indexprec);
  
  return 1;
}
/* Pattern Index */
static int patindex(dat_ptr, cmd_ptr, cgm_s)
     unsigned char *dat_ptr;
     struct cmd_info_s *cmd_ptr;
     cgm_s_type *cgm_s;
{
  cgm_s->patindex = b_gsint(&dat_ptr, cgm_s->indexprec);
  
  return 1;
}
/* Edge Type */
static int etype(dat_ptr, cmd_ptr, cgm_s)
     unsigned char *dat_ptr;
     struct cmd_info_s *cmd_ptr;
     cgm_s_type *cgm_s;
{
  cgm_s->etype = b_gsint(&dat_ptr, cgm_s->indexprec);
  return setLineType(cgm_s->etype, edgeGC);
}
/* Edge Width */
static int ewidth(dat_ptr, cmd_ptr, cgm_s)
     unsigned char *dat_ptr;
     struct cmd_info_s *cmd_ptr;
     cgm_s_type *cgm_s;
{
  switch (cgm_s->edwidspecmode) {
  case ABSOLUTE:
    switch (cgm_s->vdctype) {
    case VDC_INT: cgm_s->ewidth.i = xScale * b_gsint(&dat_ptr, cgm_s->vdcintprec);
      break;
    case VDC_REAL: cgm_s->ewidth.r = xScale * b_real(&dat_ptr, &(cgm_s->vdcrprec));
      cgm_s->ewidth.i = cgm_s->ewidth.r;
      break;
    }
    break;
  case SCALED:	cgm_s->ewidth.s = b_real(&dat_ptr, &(cgm_s->realprec));
    cgm_s->ewidth.i = cgm_s->ewidth.s; /* scale from 1 */
    break;
  }
  /* some servers can't handle widths of 1 */
  myStruct.line_width = (cgm_s->ewidth.i < 2) ? 0 : cgm_s->ewidth.i;
  XChangeGC(display, edgeGC, GCLineWidth, &myStruct);
  return 1;
}
/* Edge Colour */
static int ecolr(dat_ptr, cmd_ptr, cgm_s)
     unsigned char *dat_ptr;
     struct cmd_info_s *cmd_ptr;
     cgm_s_type *cgm_s;
{
  int i;
  
  /* may be in direct colour mode or indexed colour mode */
  switch (cgm_s->colselmode) {
  case D_C_M:	for (i=0; i<3; ++i) cgm_s->ecolr[i] = 
    b_guint(&dat_ptr, cgm_s->colprec);
    setRGB(cgm_s, cgm_s->ecolr, edgeGC);
    break;
  case I_C_M:	cgm_s->ecolr[3] = b_guint(&dat_ptr, cgm_s->cindprec);
    setIndex(cgm_s, cgm_s->ecolr[3], edgeGC);
    break;
  }
  
  return 1;
}
/* Edge Visibility */
static int evis(dat_ptr, cmd_ptr, cgm_s)
     unsigned char *dat_ptr;
     struct cmd_info_s *cmd_ptr;
     cgm_s_type *cgm_s;
{
  cgm_s->evis = b_gsint(&dat_ptr, 16);
  if (cgm_s->evis) { /* visible edge */
    XSetFunction(display, edgeGC, GXcopy);
  } else { /* invisible edge */
    XSetFunction(display, edgeGC, GXnoop);
  }
  return 1;
}
/* Colour Table */
static int coltab(dat_ptr, cmd_ptr, cgm_s)
     unsigned char *dat_ptr;
     struct cmd_info_s *cmd_ptr;
     cgm_s_type *cgm_s;
{
  int i, j, first_index, no_entries, iColrs[3];
  
  /* get the starting index */
  first_index = b_guint(&dat_ptr, cgm_s->cindprec);
  
  /* how many entries do we have ? */
  no_entries = (cmd_ptr->p_len * 8 - cgm_s->cindprec) / 
    (3 * cgm_s->colprec);
  
  for (i=0; (i < no_entries) && ((i+first_index) < MAXCOLRS); ++i) {
    for (j=0; j<3; ++j) {
      iColrs[j] = b_guint(&dat_ptr, cgm_s->colprec);
    }
    coltabPixels[i+first_index] = getPixel(cgm_s, iColrs);
  }
  
  return 1;
}
/* this is the external entry point */
int do_b_cmd(struct cmd_info_s *cmd_ptr, unsigned char *dat_ptr,
	     cgm_s_type *cgm_s)
{
  /* for convenience define and argument list macro */
#define ARG_LIST dat_ptr,cmd_ptr, cgm_s
    
    /* split it up by classes, and then by element */
    switch (cmd_ptr->Class) {
      /* the Delimiters (Class 0) */
    case B_DELIMCLASS:	switch (cmd_ptr->element) {
    case B_NOOP:	return 1;
    case B_BMF:		return(bmf(ARG_LIST));
    case B_EMF:		return(emf(ARG_LIST));
    case B_BPIC:	return(bpic(ARG_LIST));
    case B_BPICBODY:	return(bpicbody(ARG_LIST));
    case B_EPIC:	return(epic(ARG_LIST));
    default:	burp(stderr, "illegal Delimiter element %d\n", 
		     cmd_ptr->element);
      return(0);
    }
      /* the Metafile descriptor elements (Class 1) */
    case B_MDESCLASS:	switch (cmd_ptr->element) {
    case B_MFVERSION:	return(mfversion(ARG_LIST));
    case B_MFDESCRIP:	return(mfdescrip(ARG_LIST));
    case B_VDCTYPE:	return(vdctype(ARG_LIST));
    case B_INTPREC:	return(intprec(ARG_LIST));
    case B_REALPREC:	return(realprec(ARG_LIST));
    case B_INDEXPREC:	return(indexprec(ARG_LIST));
    case B_COLPREC:	return(colprec(ARG_LIST));
    case B_CINDPREC:	return(cindprec(ARG_LIST));
    case B_MAXCIND:	return(maxcind(ARG_LIST));
    case B_CVEXTENT:	return(cvextent(ARG_LIST));
    case B_MFELLIST:	return(mfellist(ARG_LIST));
    case B_MFDEFREP:	return(mfdefrep(ARG_LIST));	/* special */
    case B_FONTLIST:	return(fontlist(ARG_LIST));
    case B_CHARLIST:	return(charlist(ARG_LIST));
    case B_CHARANNOUNCE:	return(charannounce(ARG_LIST));
    default:	burp(stderr, "illegal Metafile descriptor element %d\n", 
		     cmd_ptr->element);
      return(0);
    }
      
      /* the Picture descriptor elements (Class 2) */
    case B_PDESCLASS:	switch (cmd_ptr->element) {
    case B_SCALMODE:		return(scalmode(ARG_LIST));
    case B_COLSELMODE:		return(colselmode(ARG_LIST));
    case B_LWIDSPECMODE:	return(lwidspecmode(ARG_LIST));
    case B_MARKSIZSPECMODE:	return(marksizspecmode(ARG_LIST));
    case B_EDWIDSPECMODE:	return(edwidspecmode(ARG_LIST));
    case B_VDCEXTENT:		return(vdcextent(ARG_LIST));
    case B_BACKCOLR:		return(backcolr(ARG_LIST));
    default:	burp(stderr, "illegal Picture Descriptor element %d\n", 
		     cmd_ptr->element);
      return(0);
    }
      /* the Control elements (Class 3) */
    case B_CTRLCLASS:	switch( cmd_ptr->element) {
    case B_VDCINTPREC:	return(vdcintprec(ARG_LIST));
    case B_VDCRPREC:	return(vdcrprec(ARG_LIST));
    case B_AUXCOLR:	return(auxcolr(ARG_LIST));
    case B_TRANSP:	return(transp(ARG_LIST));
    case B_CLIPRECT:	return(cliprect(ARG_LIST));
    case B_CLIPINDIC:	return(clipindic(ARG_LIST));
    default:	burp(stderr, "illegal Control element %d\n", 
		     cmd_ptr->element);
      return(0);
    }
      /* the Graphical Primitives (Class 4) */
    case B_GPRIMCLASS:	switch (cmd_ptr->element) {
    case B_POLYLINE:	return(polyline(ARG_LIST));
    case B_DISPOLY:	return(dispoly(ARG_LIST));
    case B_POLYMARKER:	return(polymarker(ARG_LIST));
    case B_TEXT:	return(text(ARG_LIST));
    case B_RESTEXT:	return(restext(ARG_LIST));
    case B_APPTEXT:	return(apptext(ARG_LIST));
    case B_POLYGON:	return(polygon(ARG_LIST));
    case B_POLYSET:	return(polyset(ARG_LIST));
    case B_CELLARRAY:	return(cellarray(ARG_LIST));
    case B_GENDPRIM:	return 1; /* not used here */
    case B_RECTANGLE:	return(rectangle(ARG_LIST));
    case B_CIRCLE:	return(circle(ARG_LIST));
    case B_CIRC3:	return(circ3(ARG_LIST));
    case B_CIRC3CLOSE:	return(circ3close(ARG_LIST));
    case B_CIRCCENTRE:	return(circcentre(ARG_LIST));
    case B_CIRCCCLOSE:	return(circcclose(ARG_LIST));
    case B_ELLIPSE:	return(ellipse(ARG_LIST));
    case B_ELARC:	return(elarc(ARG_LIST));
    case B_ELARCCLOSE:	return(elarcclose(ARG_LIST));
    default:	burp(stderr, "illegal Graphical Primitive element %d\n",
		     cmd_ptr->element);
      return(0);
    }
      /* the Attribute elements (Class 5) */
    case B_ATTRCLASS:	switch (cmd_ptr->element) {
    case B_LBINDEX:		return 1;
    case B_LTYPE:		return(ltype(ARG_LIST));
    case B_LWIDTH:		return(lwidth(ARG_LIST));
    case B_LCOLR:		return(lcolr(ARG_LIST));
    case B_MBINDEX:		return 1;
    case B_MTYPE:		return(mtype(ARG_LIST));
    case B_MSIZE:		return(msize(ARG_LIST));
    case B_MCOLR:		return(mcolr(ARG_LIST));
    case B_TBINDEX:		return 1;
    case B_TFINDEX:		return(tfindex(ARG_LIST));
    case B_TPREC:		return(tprec(ARG_LIST));
    case B_CEXPFAC:		return(cexpfac(ARG_LIST));
    case B_CSPACE:		return(cspace(ARG_LIST));
    case B_TCOLR:		return(tcolr(ARG_LIST));
    case B_CHEIGHT:		return(cheight(ARG_LIST));
    case B_CORIENT:		return(corient(ARG_LIST));
    case B_TPATH:		return(tpath(ARG_LIST));
    case B_TALIGN:		return(talign(ARG_LIST));
    case B_CSETINDEX:		return(csetindex(ARG_LIST));
    case B_ALTCSETINDEX:	return(altcsetindex(ARG_LIST));
    case B_FILLBINDEX:		return 1;
    case B_INTSTYLE:		return(intstyle(ARG_LIST));
    case B_FILLCOLR:		return(fillcolr(ARG_LIST));
    case B_HATCHINDEX:		return(hatchindex(ARG_LIST));
    case B_PATINDEX:		return(patindex(ARG_LIST));
    case B_EDBINDEX:		return 1;
    case B_ETYPE:		return(etype(ARG_LIST));
    case B_EWIDTH:		return(ewidth(ARG_LIST));
    case B_ECOLR:		return(ecolr(ARG_LIST));
    case B_EVIS:		return(evis(ARG_LIST));
    case B_FILLREF:		return 1;
    case B_PATTAB:		return 1;
    case B_PATSIZE:		return 1;
    case B_COLTAB:		return(coltab(ARG_LIST));
    case B_ASPSFLAGS:		return 1;
    default:	burp(stderr, "illegal Attribute element %d\n", 
		     cmd_ptr->element);
      return(0);
    }
      /* the Escape element (Class 6) */
    case B_ESCCLASS: return 1;
      /* the External elements (Class 7) */
    case B_EXTCLASS: return 1;
      /* any other class */
    default:	burp(stderr, "illegal class %d\n", cmd_ptr->Class);
      return(0);
    }
#undef ARG_LIST
}
#undef UINT
