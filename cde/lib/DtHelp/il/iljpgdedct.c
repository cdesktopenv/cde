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
/* $XConsortium: iljpgdedct.c /main/3 1995/10/23 15:55:19 rswiston $ */
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

#include "iljpgdecodeint.h"
#include <math.h>
#include <stdlib.h>

    /*  Macros to check if "clipValue" (an int) is  outside range 0..255, and
        to branch to point named by second macro if so, which clips and returns.
        This is done to avoid taking a branch on the common case of value
        not out of range - significant speedup on RISC machine.
    */
#define ILJPG_CLIP_256(_gotoLabel, _returnLabel) \
    if ((clipValue) >> 8) goto _gotoLabel; \
_returnLabel:

#define ILJPG_CLIP_256_LABEL(_gotoLabel, _returnLabel) \
_gotoLabel: \
    if ((clipValue) > 255) clipValue = 255; else clipValue = 0; \
    goto _returnLabel;


    /* compute 2-D DCT descaling matrix */
static void _il_fwft_rev_scale (
    iljpgPtr q,                /* pointer to quantization matrix */
    float   *s                 /* pointer to pointer to descaling matrix */
    )
{
  register int   i, j, prevValue, value, QIndex;
  double         a, c0, b[8], pi;
  register iljpgPtr qptr;
  register float *sptr;

  pi = 4.0 * atan(1.0);
  c0 = 1.0 / (2.0 * 0.707106718);
  a =  1.0 / (2 * c0);
  b[0] = a / 2.0;
  for (i = 1; i < 8; i++) {
      a = cos (i * pi / 16.0) / 2.0;
      b[i] = a;
  }

  /* descaling matrix including dequantization effects */
  /* Note: given Q table is zigzag'd, as in JIF stream.  De-zigzag *qptr.
     Also: if an entry in the Q table is zero, then: if it is the first entry, 
     store 16 (?) in the Q table, otherwise store the previous Q table value.
  */

  sptr = s;
  qptr = q;
  prevValue = 16;               /* in case 1st Q table value is 0 */
  QIndex = 0;

  for (i = 0; i < 8; i++) {
      for (j = 0; j < 8; j++) {
          value = qptr[_iljpgZigzagTable[QIndex++]];
          if (value == 0)
              value = prevValue;
          *sptr++ = b[i] * b[j] * value;
          prevValue = value;
      }
  }
}

    /*  -------------------- _iljpgDeDCTInit -------------------------- */
    /*  Called by iljpgDecode() to init for DCT decoding.
    */
    ILJPG_PRIVATE_EXTERN
iljpgError _iljpgDeDCTInit (
    iljpgDecodePrivPtr  pPriv
    )
{
    iljpgDataPtr        pData;
    register int        i;

    pData = pPriv->pData;

        /*  Build a "rev scale" table for each QTable; store into private */
    for (i = 0; i < 4; i++)
        pPriv->DCTRevScaleTables[i] = (float *)NULL;

    for (i = 0; i < 4; i++) {
        if (pData->QTables[i]) {
            if (!(pPriv->DCTRevScaleTables[i] = (float *)ILJPG_MALLOC(sizeof(float) * 64)))
                return ILJPG_ERROR_DECODE_MALLOC;
            _il_fwft_rev_scale (pData->QTables[i], pPriv->DCTRevScaleTables[i]);
            }
        }

    return 0;
}


    /*  -------------------- _iljpgDeDCTCleanup -------------------------- */
    /*  Called by iljpgDecode() to cleanup after DCT decoding.
    */
    ILJPG_PRIVATE_EXTERN
iljpgError _iljpgDeDCTCleanup (
    iljpgDecodePrivPtr  pPriv
    )
{
    register int        i;

        /*  Free any "rev scale" tables that were allocated in iljpgDeDCTInit() */
    for (i = 0; i < 4; i++) {
        if (pPriv->DCTRevScaleTables[i])
            ILJPG_FREE (pPriv->DCTRevScaleTables[i]);
        }
    
    return 0;
}


/*
 NAME
               float implementation of inverse quantize and inverse dct.
               input and output in integer format.
               Output is clamped to the range 0-255.
 PURPOSE
  perform inverse quantization and inverse DCT of a 8x8 array. During
  inverse quantization, the input array is processed in dezigzag order.
 
 NOTES
  includes the +128 shift in inverse DCT as per JPEG spec. This shift is
  accomplished by shifting the DC value by 128 prior to performing the
  inverse DCT.

  The constants b1,..,b5 used by the fast DCT method are declared as
  register variables; this allows the 'c89' compiler to perform the
  multplies as floating point 32 bit multiplies rather than promoting
  to double followed by a double to float conversion.
  (4/29/92, V. Bhaskaran)

 METHOD
   8x8 INVERSE DCT
  
   This implementation is based on computation of a 8 point inverse DCT
   using a 16 point Winograd Fourier Transform.
   The winograd fourier transform method requires 5 multiplies and 29
   additions for a 8 point inverse dct.
  
   Let T[] be a 8x8 input DCT array for which we need to perform the
   inverse DCT.
  
         | t00 t01 t02 t03 t04 t05 t06 t07 |
         | t10 t11 t12 t13 t14 t15 t16 t17 |
         | t20 t21 t22 t23 t24 t25 t26 t27 |
   T[] = | t30 t31 t32 t33 t34 t35 t36 t37 |
         | t40 t41 t42 t43 t44 t45 t46 t47 |
         | t50 t51 t52 t53 t54 t55 t56 t57 |
         | t60 t61 t62 t63 t64 t65 t66 t67 |
         | t70 t71 t72 t73 t74 t75 t76 t77 |
  
   1. The T[] matrix values are descaled by the inverse DCT denormalization
      values and the quantization matrix values.
      If we denote T[] = {t(i,j)}, S[] = {s(i,j)}, and X[] = {y(i,j)},
      then,
             X(i,j) = s(i,j) * t(i,j)
      Here, S[] is the descaling matrix and includes quantization matrix.
  
      Define a[n] as
         a[n] = cos (n pi / 16) / 2 C[n],   pi = 3.1415....
         and,
                C[0] = 1 / sqrt(2), C[1] = C[2] = ... = C[7] = 1,
      Define b[n] as
         b0 = a[0]
         bi = 2 a[i], i = 1,2,...,7
  
    Then, descaling matrix S[] has the following structure
    | b0b0q00 b1b0q01 b2b0q02 b3b0q03 b4b0q04 b5b0q05 b6b0q06 b7b0q07 |
    | b0b1q10 b1b1q11 b2b1q12 b3b1q13 b4b1q14 b5b1q15 b6b1q16 b7b1q17 |
    | b0b2q20 b1b2q21 b2b2q22 b3b2q23 b4b2q24 b5b2q25 b6b2q26 b7b2q27 |
    | b0b3q30 b1b3q31 b2b3q32 b3b3q33 b4b3q34 b5b3q35 b6b3q36 b7b3q37 |
    | b0b4q40 b1b4q41 b2b4q42 b3b4q43 b4b4q44 b5b4q45 b6b4q46 b7b4q47 |
    | b0b5q50 b1b5q51 b2b5q52 b3b5q53 b4b5q54 b5b5q55 b6b5q56 b7b5q57 |
    | b0b6q60 b1b6q61 b2b6q62 b3b6q63 b4b6q64 b5b6q65 b6b6q66 b7b6q67 |
    | b0b7q70 b1b7q71 b2b7q72 b3b7q73 b4b7q74 b5b7q75 b6b7q76 b7b7q77 |
  
    Note that q00,q01,...,q60,..,q77 is the quantization matrix specified
    during the compression stage.
    Note that in the above descaling matrix description,
    bibj = bjbi, and bibjqji implies multiplying bi,bj and qji.
    The descaling matrix can be precomputed at the start of the component
    scan (JPEG terminology).
  
   2. After T[] has been descaled, for each column of the descaled matrix,
      X[], perform a 8 point inverse dct and write results back to X[] in
      column order.
      Note that input to 8 point inverse dct is out of order, i.e. x0, x4,
      x2, x6, x5, x1, x7, x3, but output is in order.
      y0, y1, y2, y3, y4, y5, y6, y7
   3. After all eight column inverse dcts have been computed, perform
      8 point inverse dct on each row of X[] and write results to Y[]
      in row order.
   4. The Y[] matrix values must be rounded to meet the specifications of
      the input data that was compressed. Typically, for image data, this
      implies restricting Y[] to take on values only in the range 0 - 255.  

  NOTES
    Author: V. Bhaskaran, HPL, PaloAlto. Telnet: 7-7153.
            bhaskara@hplvab.hpl.hp.com
    Version: 0 (5-29-92). 
*/


    /*  -------------------- _iljpgDeDCTFull -------------------------- */
    /*  Do a full 8x8 inverse DCT, from *pSrc to *pDst, where each "scan line"
        in the 8x8 block pointed to by pDst is "nBytesPerRow" bytes away.
        pRevScale is from DCTRevScaleTables[i], where i is the Q table index
        for this component.
    */
    ILJPG_PRIVATE_EXTERN
void _iljpgDeDCTFull (
    register int       *pSrc,
    long                nBytesPerRow,
    iljpgPtr            ix,                 /* RETURNED */
    register float      *pRevScale
    )
{
  register int   i;
  register int   *zptr;
  float           ox[64];
  register float in0, in1, in2, in3, in4, in5, in6, in7;
  register float tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7;
  register float tmp;
  register int   clipValue;
  register iljpgPtr ixaddr;
  register float *oxaddr;
  /* Constants needed by the 16 point Winograd Fourier Transform for inv. DCT */
  register float b1 =  1.41421356;  
  register float b2 = -2.61312587;
  register float b3 =  1.41421356;  
  register float b4 =  1.08239220;  
  register float b5 =  0.76536686;
  

  oxaddr = ox;
  zptr  = &_iljpgZigzagTable[0]; /* zigzag scanning order */
  tmp = 128.0;                   /* JPEG +128 shift in spatial domain for DCT output */

          /* do 1-D inverse DCT along columns. setup input data, which is out of order */
  for (i = 0; i < 8; i++, oxaddr++, zptr++, pRevScale++, tmp = 0.0) { 

      in0 = *(pSrc + zptr[0])  * pRevScale[0] + tmp;  /* Add 128.0 to first DCT only */
      in1 = *(pSrc + zptr[32]) * pRevScale[32];
      in2 = *(pSrc + zptr[16]) * pRevScale[16];
      in3 = *(pSrc + zptr[48]) * pRevScale[48];
      in4 = *(pSrc + zptr[40]) * pRevScale[40];
      in5 = *(pSrc + zptr[8])  * pRevScale[8];
      in6 = *(pSrc + zptr[56]) * pRevScale[56];
      in7 = *(pSrc + zptr[24]) * pRevScale[24];

      /* Stage 0 */ 
      tmp4 = in4 - in7;
      tmp5 = in5 + in6;
      tmp6 = in5 - in6;
      tmp7 = in4 + in7;

      /* Stage 1 */
      tmp2 = in2 - in3;
      tmp3 = in2 + in3;
      in5  = tmp5 - tmp7;
      in7  = tmp5 + tmp7;

      /* Stage 1.1 */
      tmp  = tmp4 - tmp6;
      tmp  = b5 * tmp;
  
      /* Stage 2 */
      in2  = b1 * tmp2;
      in4  = b2 * tmp4;
      tmp5 = b3 * in5;
      in6  = b4 * tmp6;
      tmp0 = in0 + in1;
      tmp1 = in0 - in1;
      in2 -= tmp3;
      in4 += tmp;
      in6 -= tmp;  

      /* Stage 3 */
      in6  -= in7;
      tmp5 -= in6;

      /* Stage 4 */
      in0   = tmp0 + tmp3;
      in1   = tmp1 + in2;
      tmp2  = tmp1 - in2;
      in3   = tmp0 - tmp3;
      tmp4  = in4 + tmp5;

      /* Stage 5 */
      *oxaddr       = in0  + in7;    /* y0 */
      *(oxaddr+8)   = in1  + in6;    /* y1 */
      *(oxaddr+16)  = tmp2 + tmp5;   /* y2 */
      *(oxaddr+24)  = in3  - tmp4;   /* y3 */
      *(oxaddr+32)  = in3  + tmp4;   /* y4 */
      *(oxaddr+40)  = tmp2 - tmp5;   /* y5 */
      *(oxaddr+48)  = in1  - in6;    /* y6 */
      *(oxaddr+56)  = in0  - in7;    /* y7 */
  }

  ixaddr = ix;  
  oxaddr = ox;
  for (i = 0; i < 8; i++) { /* do 1-D inverse DCT along rows */

      /* setup input data - input data is out of order */
      in0 = *oxaddr;
      in1 = *(oxaddr+4);
      in2 = *(oxaddr+2);
      in3 = *(oxaddr+6);
      in4 = *(oxaddr+5);
      in5 = *(oxaddr+1);
      in6 = *(oxaddr+7);
      in7 = *(oxaddr+3);

      /* Stage 0 */ 
      tmp4 = in4 - in7;
      tmp5 = in5 + in6;
      tmp6 = in5 - in6;
      tmp7 = in4 + in7;

      /* Stage 1 */
      tmp2 = in2 - in3;
      tmp3 = in2 + in3;
      in5  = tmp5 - tmp7;
      in7  = tmp5 + tmp7;

      /* Stage 1.1 */
      tmp  = tmp4 - tmp6;
      tmp  = b5 * tmp;
  
      /* Stage 2 */
      tmp0 = in0 + in1;
      tmp1 = in0 - in1;
      in2  = b1 * tmp2;
      in4  = b2 * tmp4 + tmp;
      tmp5 = b3 * in5;
      in6  = b4 * tmp6 - tmp;
      in2 -= tmp3;

      /* Stage 2.1 */
      in6  -= in7;
      tmp5 -= in6;

      /* Stage 3 */
      in0   = tmp0 + tmp3;
      in1   = tmp1 + in2;
      tmp2  = tmp1 - in2;
      in3   = tmp0 - tmp3;
      tmp4  = in4 + tmp5;

      /* Stage 4: clip values to 0..255 and store */
      clipValue = (int)(in0 + in7);     /* y0 */
      ILJPG_CLIP_256 (fClipG0, fClipR0)
      *ixaddr = clipValue;
      clipValue = (int)(in1 + in6);     /* y1 */
      ILJPG_CLIP_256 (fClipG1, fClipR1)
      *(ixaddr+1) = clipValue;
      clipValue = (int)(tmp2 + tmp5);   /* y2 */
      ILJPG_CLIP_256 (fClipG2, fClipR2)
      *(ixaddr+2) = clipValue;
      clipValue = (int)(in3  - tmp4);   /* y3 */
      ILJPG_CLIP_256 (fClipG3, fClipR3)
      *(ixaddr+3) = clipValue;
      clipValue = (int)(in3  + tmp4);   /* y4 */
      ILJPG_CLIP_256 (fClipG4, fClipR4)
      *(ixaddr+4) = clipValue;
      clipValue = (int)(tmp2 - tmp5);   /* y5 */
      ILJPG_CLIP_256 (fClipG5, fClipR5)
      *(ixaddr+5) = clipValue;
      clipValue = (int)(in1  - in6);    /* y6 */
      ILJPG_CLIP_256 (fClipG6, fClipR6)
      *(ixaddr+6) = clipValue;
      clipValue = (int)(in0  - in7);    /* y7 */
      ILJPG_CLIP_256 (fClipG7, fClipR7)
      *(ixaddr+7) = clipValue;

      oxaddr += 8;
      ixaddr += nBytesPerRow;
  }

        /*  Goto points for above clip macros */
  return;

  ILJPG_CLIP_256_LABEL (fClipG0, fClipR0)
  ILJPG_CLIP_256_LABEL (fClipG1, fClipR1)
  ILJPG_CLIP_256_LABEL (fClipG2, fClipR2)
  ILJPG_CLIP_256_LABEL (fClipG3, fClipR3)
  ILJPG_CLIP_256_LABEL (fClipG4, fClipR4)
  ILJPG_CLIP_256_LABEL (fClipG5, fClipR5)
  ILJPG_CLIP_256_LABEL (fClipG6, fClipR6)
  ILJPG_CLIP_256_LABEL (fClipG7, fClipR7)
} 
  


/*
                float implementation of inverse quantize and inverse dct.
                Assumes that only the first 4x4 submatrix of DCT 
                coefficients is non-zero.
                Input and output in integer format.

               This reduces the fast inverse DCT multiply and add count
               from 80 multiplies, 464 additions to 60 multiplies,
               252 additions.
 PURPOSE
  perform inverse quantization and inverse DCT of a 8x8 array.
  Data is dezigzagged during the inverse quantization process.
   
 NOTES
  includes the +128 shift in inverse DCT as per JPEG spec. This shift is
  accomplished by shifting the DC value by 128 prior to performing the
  inverse DCT.
 METHOD
   8x8 INVERSE DCT
  
   This implementation is based on computation of a 8 point inverse DCT
   using a 16 point Winograd Fourier Transform.
   The winograd fourier transform method requires 5 multiplies and 29
   additions for a 8 point inverse dct.
  
   Let T[] be a 8x8 input DCT array for which we need to perform the
   inverse DCT. 
   Note that the matrix shown below suggests that the DCT matrix has
   only 16 non-zero coefficients and the coefficient locations are
   as indicated in the matrix shown below.  
  
         | t00 t01 t02 t03 --- --- --- --- |
         | t10 t11 t12 t13 --- --- --- --- | 
         | t20 t21 t22 t23 --- --- --- --- |
   T[] = | t30 t31 t32 t33 --- --- --- --- | 
         | --- --- --- --- --- --- --- --- | 
         | --- --- --- --- --- --- --- --- | 
         | --- --- --- --- --- --- --- --- |
         | --- --- --- --- --- --- --- --- |
  
   1. The T[] matrix values are descaled by the inverse DCT denormalization
      values and the quantization matrix values.
      If we denote T[] = {t(i,j)}, S[] = {s(i,j)}, and X[] = {y(i,j)},
      then,
             X(i,j) = s(i,j) * t(i,j)
      Here, S[] is the descaling matrix and includes quantization matrix.
      Since only 1/4th of the T[] matrix is non-zero we need to descale
      only these components. The rest are simply accounted for by setting
      the corresponding locations in X[] to zero.
  
      Define a[n] as
         a[n] = cos (n pi / 16) / 2 C[n],   pi = 3.1415....
         and,
                C[0] = 1 / sqrt(2), C[1] = C[2] = ... = C[7] = 1,
      Define b[n] as
         b0 = a[0]
         bi = 2 a[i], i = 1,2,...,7
  
    Then, descaling matrix S[] has the following structure
    | b0b0q00 b1b0q01 b2b0q02 b3b0q03 b4b0q04 b5b0q05 b6b0q06 b7b0q07 |
    | b0b1q10 b1b1q11 b2b1q12 b3b1q13 b4b1q14 b5b1q15 b6b1q16 b7b1q17 |
    | b0b2q20 b1b2q21 b2b2q22 b3b2q23 b4b2q24 b5b2q25 b6b2q26 b7b2q27 |
    | b0b3q30 b1b3q31 b2b3q32 b3b3q33 b4b3q34 b5b3q35 b6b3q36 b7b3q37 |
    | b0b4q40 b1b4q41 b2b4q42 b3b4q43 b4b4q44 b5b4q45 b6b4q46 b7b4q47 |
    | b0b5q50 b1b5q51 b2b5q52 b3b5q53 b4b5q54 b5b5q55 b6b5q56 b7b5q57 |
    | b0b6q60 b1b6q61 b2b6q62 b3b6q63 b4b6q64 b5b6q65 b6b6q66 b7b6q67 |
    | b0b7q70 b1b7q71 b2b7q72 b3b7q73 b4b7q74 b5b7q75 b6b7q76 b7b7q77 |
  
    Note that q00,q01,...,q60,..,q77 is the quantization matrix specified
    during the compression stage.
    Note that in the above descaling matrix description,
    bibj = bjbi, and bibjqji implies multiplying bi,bj and qji.
    The descaling matrix can be precomputed at the start of the component
    scan (JPEG terminology).
  
   2. After T[] has been descaled, for each column of the descaled matrix,
      X[], perform a 8 point inverse dct and write results back to X[] in
      column order. A pruned dct which uses 4 input points and generates
      8 output points is used.
      Note that input to 8 point inverse dct is out of order, i.e. x0, x4,
      x2, x6, x5, x1, x7, x3, but output is in order.
      y0, y1, y2, y3, y4, y5, y6, y7
      Since we know that only the first four components of each column is
      nonzero, we exploit this in pruning our fast DCT computation, thus,
      reducing the multiplies/adds by a factor of two.
 
   3. After the first four column inverse dcts have been computed, perform
      8 point inverse dct on each row of X[] and write results to Y[]
      in row order.
      Since we know that the first four components along each row could
      be non-zero (the remaining have to be zero as per the structure of
      the X[] matrix), we can exploit this in pruning the fast DCT
      computation, and thereby, reducing the multiplies/adds by a factor
      of two.
   4. The Y[] matrix values must be rounded to meet the specifications of
      the input data that was compressed. Typically, for image data, this
      implies restricting Y[] to take on values only in the range 0 - 255.

   NOTES   
    Author: V. Bhaskaran, HPL, PaloAlto. Telnet: 7-7153.
    Version: 0 (5-29-92).
*/

    /*  -------------------- _iljpgDeDCT4x4 -------------------------- */
    /*  Do an inverse DCT, from *pSrc to *pDst, each a ptr to 64 ints.
        Assumes that only the top-left 4x4 DCT coefficients are non-zero.
        pRevScale is from DCTRevScaleTables[i], where i is the Q table index
        for this component.
    */
    ILJPG_PRIVATE_EXTERN
void _iljpgDeDCT4x4 (
    register int       *pSrc,
    long                nBytesPerRow,
    iljpgPtr            ix,                 /* RETURNED */
    register float      *pRevScale
    )
{
  register int   i;
  register int   *zptr;
  float           ox[64];
  register float in0, in2, in3, in4, in5, in7;
  register float tmp0, tmp1, tmp2, tmp5, tmp6, tmp7;
  register float tmp;
  register float *oxaddr;
  register int   clipValue;
  register iljpgPtr ixaddr;
  /* Constants needed by the 16 point Winograd Fourier Transform for inv. DCT */
  register float b1 =  1.41421356;  
  register float b2 = -2.61312587;
  register float b3 =  1.41421356;  
  register float b4 =  1.08239220;  
  register float b5 =  0.76536686;

#ifdef NOTDEF
  orxptr = ox;
  rsptr  = pRevScale;
  rzptr  = &_iljpgZigzagTable[0];
  /* descale-dequantize dct coefficients - need to do this for 4x4 submatrix */
  for (i = 0; i < 4; i++, orxptr += 8, rsptr += 8, rzptr += 8) {
      for (j = 0, ocxptr = orxptr, csptr = rsptr, czptr = rzptr; 
           j < 4; j++, ocxptr++, czptr++) {
           k = *(pSrc + *czptr);
           *ocxptr   = (k * *csptr++);
      }                 
  }

  *ox += 128.0; /* JPEG +128 shift in spatial domain for DCT output */
#endif

  oxaddr = ox;
  zptr  = &_iljpgZigzagTable[0]; /* zigzag scanning order */
  tmp = 128.0;                   /* JPEG +128 shift in spatial domain for DCT output */

          /* do 1-D inverse DCT along columns. setup input data, which is out of order */
  for (i = 0; i < 4; i++, oxaddr++, zptr++, pRevScale++, tmp = 0.0) {

      in0 = *(pSrc + zptr[0])  * pRevScale[0] + tmp;  /* Add 128.0 to first DCT only */
      in2 = *(pSrc + zptr[16])  * pRevScale[16];
      in5 = *(pSrc + zptr[8])  * pRevScale[8];
      in7 = *(pSrc + zptr[24])  * pRevScale[24];

#ifdef NOTDEF
      in0 = *oxaddr;
      in2 = *(oxaddr+16);
      in5 = *(oxaddr+8);
      in7 = *(oxaddr+24);
#endif

      /* Stage 1 */
      tmp5 = in5 - in7;
      tmp7 = in5 + in7;

      /* Stage 2 */
      tmp  = -in7 -in5;
      tmp  = b5 * tmp;
  
      tmp2  = b1 * in2;
      tmp6  = b4 * in5;
      in4   = b2 * -in7;
      in5   = b3 * tmp5;
      in4  += tmp;
      tmp6 -= tmp;  
      tmp2 -= in2; 

      /* Stage 3 */
      tmp6 -= tmp7;
      in5  -= tmp6;

      /* Stage 4 */
      tmp0  = in0 + in2;
      tmp1  = in0 + tmp2;
      in3   = in0 - in2;
      in4  += in5;
      tmp2  = in0 - tmp2;

      /* Stage 5 */
      *oxaddr       = tmp0 + tmp7;  /* y0 */
      *(oxaddr+8)   = tmp1 + tmp6;  /* y1 */
      *(oxaddr+16)  = tmp2 + in5;   /* y2 */
      *(oxaddr+24)  = in3  - in4;   /* y3 */
      *(oxaddr+32)  = in3  + in4;   /* y4 */
      *(oxaddr+40)  = tmp2 - in5;   /* y5 */
      *(oxaddr+48)  = tmp1 - tmp6;  /* y6 */
      *(oxaddr+56)  = tmp0 - tmp7;  /* y7 */
  }

  ixaddr = ix;  
  oxaddr = ox;
  for (i = 0; i < 8; i++) { /* do 1-D inverse DCT along rows */

      /* setup input data - input data is out of order */
      in0 = *oxaddr;
      in2 = *(oxaddr+2);
      in5 = *(oxaddr+1);
      in7 = *(oxaddr+3);

      /* Stage 1 */
      tmp5 = in5 - in7;
      tmp7 = in5 + in7;

      /* Stage 2 */
      tmp  = -in7 -in5;
      tmp  = b5 * tmp;
  
      tmp2 = b1 * in2;
      tmp6 = b4 * in5;
      in4  = b2 * -in7;
      in5  = b3 * tmp5;
      in4  += tmp;
      tmp6 -= tmp;  

      /* Stage 3 */
      tmp2 -= in2; 

      /* Stage 3.1 */
      tmp6 -= tmp7;
      in5  -= tmp6;

      /* Stage 4 */
      tmp0  = in0 + in2;
      tmp1  = in0 + tmp2;
      in3   = in0 - in2;
      in4  += in5;
      tmp2  = in0 - tmp2;

      /* Stage 5 */
       
      clipValue = (int)(tmp0  + tmp7);    /* y0 */
      ILJPG_CLIP_256 (pClipG0, pClipR0)
      *ixaddr = clipValue;
      clipValue = (int)(tmp1 + tmp6);   /* y1 */
      ILJPG_CLIP_256 (pClipG1, pClipR1)
      *(ixaddr+1) = clipValue;
      clipValue = (int)(tmp2 + in5);    /* y2 */
      ILJPG_CLIP_256 (pClipG2, pClipR2)
      *(ixaddr+2) = clipValue;
      clipValue = (int)(in3  - in4);    /* y3 */
      ILJPG_CLIP_256 (pClipG3, pClipR3)
      *(ixaddr+3) = clipValue;
      clipValue = (int)(in3  + in4);    /* y4 */
      ILJPG_CLIP_256 (pClipG4, pClipR4)
      *(ixaddr+4) = clipValue;
      clipValue = (int)(tmp2 - in5);    /* y5 */
      ILJPG_CLIP_256 (pClipG5, pClipR5)
      *(ixaddr+5) = clipValue;
      clipValue = (int)(tmp1 - tmp6);   /* y6 */
      ILJPG_CLIP_256 (pClipG6, pClipR6)
      *(ixaddr+6) = clipValue;
      clipValue = (int)(tmp0 - tmp7);   /* y7 */
      ILJPG_CLIP_256 (pClipG7, pClipR7)
      *(ixaddr+7) = clipValue;

      ixaddr += nBytesPerRow;
      oxaddr += 8;

  }

        /*  Goto points for above clip macros */
  return;

  ILJPG_CLIP_256_LABEL (pClipG0, pClipR0)
  ILJPG_CLIP_256_LABEL (pClipG1, pClipR1)
  ILJPG_CLIP_256_LABEL (pClipG2, pClipR2)
  ILJPG_CLIP_256_LABEL (pClipG3, pClipR3)
  ILJPG_CLIP_256_LABEL (pClipG4, pClipR4)
  ILJPG_CLIP_256_LABEL (pClipG5, pClipR5)
  ILJPG_CLIP_256_LABEL (pClipG6, pClipR6)
  ILJPG_CLIP_256_LABEL (pClipG7, pClipR7)
} 
  
/*
  float implementation of inverse quantize and inverse dct.
 PURPOSE
  perform inverse quantization and inverse DCT of a 8x8 array. 
  Assumes that all coefficients of 8x8 DCT matrix except DC coefficient 
  are zero. 
  Input and output in integer format.
 NOTES
  includes the +128 shift in inverse DCT as per JPEG spec. This shift is
  accomplished by shifting the DC value by 128 prior to performing the
  inverse DCT.
 METHOD
   8x8 INVERSE DCT
  
   Let T[] be a 8x8 input DCT array for which we need to perform the
   inverse DCT. 
   Note that the matrix shown below suggests that the DCT matrix has
   only 16 non-zero coefficients and the coefficient locations are
   as indicated in the matrix shown below.  
  
         | t00 --- --- --- --- --- --- --- |
         | --- --- --- --- --- --- --- --- | 
         | --- --- --- --- --- --- --- --- |
   T[] = | --- --- --- --- --- --- --- --- | 
         | --- --- --- --- --- --- --- --- | 
         | --- --- --- --- --- --- --- --- | 
         | --- --- --- --- --- --- --- --- |
         | --- --- --- --- --- --- --- --- |
  
   1. The T[] matrix values are descaled by the inverse DCT denormalization
      values and the quantization matrix values.
      If we denote T[] = {t(i,j)}, S[] = {s(i,j)}, and X[] = {y(i,j)},
      then,
             X(i,j) = s(i,j) * t(i,j)
      Here, S[] is the descaling matrix and includes quantization matrix.
      Since only 1/4th of the T[] matrix is non-zero we need to descale
      only these components. The rest are simply accounted for by setting
      the corresponding locations in X[] to zero.
  
      Define a[n] as
         a[n] = cos (n pi / 16) / 2 C[n],   pi = 3.1415....
         and,
                C[0] = 1 / sqrt(2), C[1] = C[2] = ... = C[7] = 1,
      Define b[n] as
         b0 = a[0]
         bi = 2 a[i], i = 1,2,...,7
  
    Then, descaling matrix S[] has the following structure
    | b0b0q00 b1b0q01 b2b0q02 b3b0q03 b4b0q04 b5b0q05 b6b0q06 b7b0q07 |
    | b0b1q10 b1b1q11 b2b1q12 b3b1q13 b4b1q14 b5b1q15 b6b1q16 b7b1q17 |
    | b0b2q20 b1b2q21 b2b2q22 b3b2q23 b4b2q24 b5b2q25 b6b2q26 b7b2q27 |
    | b0b3q30 b1b3q31 b2b3q32 b3b3q33 b4b3q34 b5b3q35 b6b3q36 b7b3q37 |
    | b0b4q40 b1b4q41 b2b4q42 b3b4q43 b4b4q44 b5b4q45 b6b4q46 b7b4q47 |
    | b0b5q50 b1b5q51 b2b5q52 b3b5q53 b4b5q54 b5b5q55 b6b5q56 b7b5q57 |
    | b0b6q60 b1b6q61 b2b6q62 b3b6q63 b4b6q64 b5b6q65 b6b6q66 b7b6q67 |
    | b0b7q70 b1b7q71 b2b7q72 b3b7q73 b4b7q74 b5b7q75 b6b7q76 b7b7q77 |
  
    Note that q00,q01,...,q60,..,q77 is the quantization matrix specified
    during the compression stage.
    Note that in the above descaling matrix description,
    bibj = bjbi, and bibjqji implies multiplying bi,bj and qji.
    The descaling matrix can be precomputed at the start of the component
    scan (JPEG terminology).
  
   2. After T[] has been descaled, compute y(0,0) = t(0,0) * s(0,0) + 128.
      set y(i,j) = y(0,0), for all i = 0,7, j = 0,7.  
   4. The Y[] matrix values must be rounded to meet the specifications of
      the input data that was compressed. Typically, for image data, this
      implies restricting Y[] to take on values only in the range 0 - 255.  
   NOTES 
    Author: V. Bhaskaran, HPL, PaloAlto. Telnet: 7-7153.
    Version: 0 (5-29-92).
*/

    /*  -------------------- _iljpgDeDCTDCOnly -------------------------- */
    /*  Do an inverse DCT, from *pSrc to *pDst, each a ptr to 64 ints.
        Assumes that only the top-left coefficient (the DC) is non-zero.
        pRevScale is from DCTRevScaleTables[i], where i is the Q table index
        for this component.
    */
    ILJPG_PRIVATE_EXTERN
void _iljpgDeDCTDCOnly (
    int                *pSrc,
    long                nBytesPerRow,
    register iljpgPtr   pDst,               /* RETURNED */
    float              *pRevScale
    )
{
  register int   i, dc;
  register int   j;

  j       = *pSrc;
  j = (j < -2047) ? -2047 : ((j > 2047) ? 2047 : j);
  dc      = (int)(j * *pRevScale + 128.0);
  if (dc < 0) dc = 0; else if (dc > 255) dc = 255;

  /* 
     since only DC value is nonzero, inverse DCT is simply a copy of the
     descaled and dequantized DC value copied to rest of 8x8 array. 
  */
  for (i = 0; i < 8; i++, pDst += nBytesPerRow) {
    pDst[0] = dc; 
    pDst[1] = dc; 
    pDst[2] = dc; 
    pDst[3] = dc; 
    pDst[4] = dc; 
    pDst[5] = dc; 
    pDst[6] = dc; 
    pDst[7] = dc; 
    }
}


