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
/* $XConsortium: cgmcommon.c /main/3 1996/06/11 16:09:35 cde-hal $ */
/* this is the module of common routines for the CGM interpreter */
#include "cgm.h"			/* our defs */
/* this routine sets a cgm state variable to its defaults */
int cgm_defs(cgm_s_type *cgm_s)
{
  int i;
  
  /* metafile descriptors */
  cgm_s->vdctype 	= VDC_INT;
  cgm_s->intprec 	= 16;
  cgm_s->realprec.fixed	= 1;
  cgm_s->realprec.exp	= 16;
  cgm_s->realprec.fract	= 16;
  cgm_s->indexprec 	= 16;
  cgm_s->colprec	= 8;
  cgm_s->cindprec	= 8;
  cgm_s->maxcind	= 63;
  for (i=0; i<3; ++i) {
    cgm_s->cvextent[i] = 0;
    cgm_s->cvextent[3 + i] = 255;
  }
  for (i=0; i<MAX_FONTS; ++i) {
    cgm_s->fontlist[i] 	= NULL;
    cgm_s->chartype[i]	= 0;
    cgm_s->charlist[i]	= NULL;
  }
  cgm_s->charannounce	= 0;
  /* picture descriptors */
  cgm_s->scalmode	= ABSTRACT_S;
  cgm_s->colselmode	= I_C_M;
  cgm_s->lwidspecmode	= SCALED;
  cgm_s->marksizspecmode= SCALED;
  cgm_s->edwidspecmode	= SCALED;
  for (i=0; i<2; ++i) {
    cgm_s->vdcextent[i].i	= 0;
    cgm_s->vdcextent[i].r	= 0.0;
    cgm_s->vdcextent[i + 2].i	= 32767;
    cgm_s->vdcextent[i + 2].r	= 1.0;
  }
  for (i=0; i<3; ++i) cgm_s->backcolr[i] = cgm_s->cvextent[3 + i];
  /* control elements */
  cgm_s->vdcintprec	= 16;
  cgm_s->vdcrprec.fixed	= 1;
  cgm_s->vdcrprec.exp	= 16;
  cgm_s->vdcrprec.fract	= 16;
  for (i=0; i<3; ++i) cgm_s->auxcolr[i] = cgm_s->cvextent[i];
  cgm_s->auxcolr[3]	= 1;
  cgm_s->transp		= ON;
  for (i=0; i<4; ++i) {
    cgm_s->cliprect[i].i	= cgm_s->vdcextent[i].i;
    cgm_s->cliprect[i].r	= cgm_s->vdcextent[i].r;
  }
  cgm_s->clipindic	= ON;
  cgm_s->lbindex	= 1;
  cgm_s->ltype		= SOLID_L;
  cgm_s->lwidth.r 	= 0.001;
  cgm_s->lwidth.i	= 33;
  cgm_s->lwidth.s 	= 1.0;
  for (i=0; i<3; ++i) cgm_s->lcolr[i] = cgm_s->cvextent[i];
  cgm_s->lcolr[3] 	= 1;
  cgm_s->mbindex	= 1;
  cgm_s->mtype		= 3;
  cgm_s->msize.r 	= 0.01; /* unused in XLib */
  cgm_s->msize.i	= 5; /* XLib sepcific */
  cgm_s->msize.s	= 1.0;
  for (i=0; i<3; ++i) cgm_s->mcolr[i] = cgm_s->cvextent[i];
  cgm_s->mcolr[3] 	= 1;
  cgm_s->tbindex	= 1;
  cgm_s->tfindex	= 1;
  cgm_s->tprec		= STRING_P;
  cgm_s->cexpfac	= 1.0;
  cgm_s->cspace		= 0.0;
  for (i=0; i<3; ++i) cgm_s->tcolr[i] = cgm_s->cvextent[i];
  cgm_s->tcolr[3] 	= 1;
  cgm_s->cheight.r	= 0.01; /* unused in XLib */
  cgm_s->cheight.i	= 10; /* XLib specific */
  cgm_s->corient[0].i	= 0;
  cgm_s->corient[0].r	= 0.0;
  cgm_s->corient[1].i	= 1;
  cgm_s->corient[1].r	= 1.0;
  cgm_s->corient[2].i	= 1;
  cgm_s->corient[2].r	= 1.0;
  cgm_s->corient[3].i	= 0;
  cgm_s->corient[3].r	= 0.0;
  cgm_s->tpath		= RIGHT_P;
  cgm_s->talign[0].i	= NORMAL_H;
  cgm_s->talign[1].i	= NORMAL_V;
  cgm_s->csetindex	= 1;
  cgm_s->fillbindex	= 1;
  cgm_s->intstyle	= HOLLOW_F;
  for (i=0; i<3; ++i) cgm_s->fillcolr[i] = cgm_s->cvextent[i];
  cgm_s->fillcolr[3] 	= 1;
  cgm_s->hatchindex	= 1;
  cgm_s->patindex	= 1;
  cgm_s->edbindex	= 1;
  cgm_s->etype		= SOLID_L;
  cgm_s->ewidth.r	= 0.001;
  cgm_s->ewidth.i	= 33;
  cgm_s->ewidth.s	= 1.0;
  for (i=0; i<3; ++i) cgm_s->ecolr[i] = cgm_s->cvextent[i];
  cgm_s->ecolr[3] 	= 1;
  cgm_s->evis		= OFF;
  for (i=0; i<2; ++i) {
    cgm_s->fillref[i].i	= cgm_s->vdcextent[i].i;
    cgm_s->fillref[i].r	= cgm_s->vdcextent[i].r;
  }
  cgm_s->pattab		= NULL;
  for (i=0; i<4; ++i) {
    cgm_s->patsize[i].i	= cgm_s->vdcextent[i].i;
    cgm_s->patsize[i].r	= cgm_s->vdcextent[i].r;
  }
  if (!(cgm_s->coltab = (int *) 
	malloc(3 * sizeof(int) * (cgm_s->maxcind + 1)))) {
    burp(stderr, "couldn't get memory for def colour table\n");
    return(0);
  }
  for (i=0; i<NO_ASPSF; ++i) cgm_s->aspsflags[i] = F_INDIV;
  
  return(1);
}
/* this routine sets one cgm state equal to another */
int cgm_assign(cgm_s_type *cgm_1, cgm_s_type *cgm_2)
{
  int i;
  
  /* metafile descriptors */
  cgm_2->vdctype 	= cgm_1->vdctype;
  cgm_2->intprec 	= cgm_1->intprec;
  cgm_2->realprec.fixed	= cgm_1->realprec.fixed;
  cgm_2->realprec.exp	= cgm_1->realprec.exp;
  cgm_2->realprec.fract	= cgm_1->realprec.fract;
  cgm_2->indexprec 	= cgm_1->indexprec;
  cgm_2->colprec	= cgm_1->colprec;
  cgm_2->cindprec	= cgm_1->cindprec;
  for (i=0; i<6; ++i) {
    cgm_2->cvextent[i] = cgm_1->cvextent[i];
  }
  for (i=0; i<MAX_FONTS; ++i) {
    cgm_2->fontlist[i] 	= cgm_1->fontlist[i];
    cgm_2->chartype[i]	= cgm_1->chartype[i];
    cgm_2->charlist[i]	= cgm_1->charlist[i];
  }
  cgm_2->charannounce	= cgm_1->charannounce;
  /* picture descriptors */
  cgm_2->scalmode	= cgm_1->scalmode;
  cgm_2->colselmode	= cgm_1->colselmode;
  cgm_2->lwidspecmode	= cgm_1->lwidspecmode;
  cgm_2->marksizspecmode= cgm_1->marksizspecmode;
  cgm_2->edwidspecmode	= cgm_1->edwidspecmode;
  for (i=0; i<4; ++i) {
    cgm_2->vdcextent[i].i	= cgm_1->vdcextent[i].i;
    cgm_2->vdcextent[i].r	= cgm_1->vdcextent[i].r;
  }
  for (i=0; i<3; ++i) cgm_2->backcolr[i] = cgm_1->backcolr[i];
  /* control elements */
  cgm_2->vdcintprec	= cgm_1->vdcintprec;
  cgm_2->vdcrprec.fixed	= cgm_1->vdcrprec.fixed;
  cgm_2->vdcrprec.exp	= cgm_1->vdcrprec.exp;
  cgm_2->vdcrprec.fract	= cgm_1->vdcrprec.fract;
  for (i=0; i<4; ++i) cgm_2->auxcolr[i] = cgm_1->auxcolr[i];
  cgm_2->transp		= cgm_1->transp;
  for (i=0; i<4; ++i) {
    cgm_2->cliprect[i].i	= cgm_1->cliprect[i].i;
    cgm_2->cliprect[i].r	= cgm_1->cliprect[i].r;
  }
  cgm_2->clipindic	= cgm_1->clipindic;
  cgm_2->lbindex	= cgm_1->lbindex;
  cgm_2->ltype		= cgm_1->ltype;
  cgm_2->lwidth.r 	= cgm_1->lwidth.r;
  cgm_2->lwidth.i	= cgm_1->lwidth.i;
  cgm_2->lwidth.s 	= cgm_1->lwidth.s;
  for (i=0; i<4; ++i) cgm_2->lcolr[i] = cgm_1->lcolr[i];
  cgm_2->mbindex	= cgm_1->mbindex;
  cgm_2->mtype		= cgm_1->mtype;
  cgm_2->msize.r 	= cgm_1->msize.r;
  cgm_2->msize.i	= cgm_1->msize.i;
  cgm_2->msize.s	= cgm_1->msize.s;
  for (i=0; i<4; ++i) cgm_2->mcolr[i] = cgm_1->mcolr[i];
  cgm_2->tbindex	= cgm_1->tbindex;
  cgm_2->tfindex	= cgm_1->tfindex;
  cgm_2->tprec		= cgm_1->tprec;
  cgm_2->cexpfac	= cgm_1->cexpfac;
  cgm_2->cspace		= cgm_1->cspace;
  for (i=0; i<4; ++i) cgm_2->tcolr[i] = cgm_1->tcolr[i];
  cgm_2->cheight.r	= cgm_1->cheight.r;
  cgm_2->cheight.i	= cgm_1->cheight.i;
  cgm_2->corient[0].i	= cgm_1->corient[0].i;
  cgm_2->corient[0].r	= cgm_1->corient[0].r;
  cgm_2->corient[1].i	= cgm_1->corient[1].i;
  cgm_2->corient[1].r	= cgm_1->corient[1].r;
  cgm_2->corient[2].i	= cgm_1->corient[2].i;
  cgm_2->corient[2].r	= cgm_1->corient[2].r;
  cgm_2->corient[3].i	= cgm_1->corient[3].i;
  cgm_2->corient[3].r	= cgm_1->corient[3].r;
  cgm_2->tpath		= cgm_1->tpath;
  cgm_2->talign[0].i	= cgm_1->talign[0].i;
  cgm_2->talign[1].i	= cgm_1->talign[1].i;
  cgm_2->csetindex	= cgm_1->csetindex;
  cgm_2->fillbindex	= cgm_1->fillbindex;
  cgm_2->intstyle	= cgm_1->intstyle;
  for (i=0; i<4; ++i) cgm_2->fillcolr[i] 	= cgm_1->fillcolr[i];
  cgm_2->hatchindex	= cgm_1->hatchindex;
  cgm_2->patindex	= cgm_1->patindex;
  cgm_2->edbindex	= cgm_1->edbindex;
  cgm_2->etype		= cgm_1->etype;
  cgm_2->ewidth.r	= cgm_1->ewidth.r;
  cgm_2->ewidth.i	= cgm_1->ewidth.i;
  cgm_2->ewidth.s	= cgm_1->ewidth.s;
  for (i=0; i<4; ++i) cgm_2->ecolr[i] = cgm_1->ecolr[i];
  cgm_2->evis		= cgm_1->evis;
  for (i=0; i<2; ++i) {
    cgm_2->fillref[i].i	= cgm_1->fillref[i].i;
    cgm_2->fillref[i].r	= cgm_1->fillref[i].r;
  }
  if (cgm_2->pattab) free(cgm_2->pattab);		/* fix later */
  cgm_2->pattab		= cgm_1->pattab;
  for (i=0; i<4; ++i) {
    cgm_2->patsize[i].i	= cgm_1->patsize[i].i;
    cgm_2->patsize[i].r	= cgm_1->patsize[i].r;
  }
  if (cgm_2->maxcind < cgm_1->maxcind) {
    if (cgm_2->coltab) {
      if (!(cgm_2->coltab =
	    (int *) realloc(cgm_2->coltab,
			    3 * sizeof(int) * (cgm_1->maxcind + 1)))) {
	burp(stderr, "couldn't reallocate mem for coltab\n");
	return(0);
      } 
    } else {
      if (!(cgm_2->coltab =
	    (int *) malloc(
			   3 * sizeof(int) * (cgm_1->maxcind + 1)))) {
	burp(stderr, "couldn't allocate mem for coltab\n");
	return(0);
      }
    }
  }	/* have enough memory now */
  for (i=0; i<((cgm_1->maxcind + 1) * 3); ++i) {
    cgm_2->coltab[i] = cgm_1->coltab[i];
  }
  cgm_2->maxcind 		= cgm_1->maxcind;
  for (i=0; i<NO_ASPSF; ++i) cgm_2->aspsflags[i] = cgm_1->aspsflags[i];
  
  return(1);
}
