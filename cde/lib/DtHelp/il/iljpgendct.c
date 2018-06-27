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
/* $XConsortium: iljpgendct.c /main/3 1995/10/23 15:56:59 rswiston $ */
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

#include "iljpgencodeint.h"
#include <math.h>

/* floating point implementation of dct and quantization.
   input and output in integer format.
   Output is clamped to the range -2047 to 2047.
  PURPOSE
   compute 8x8 2-D DCT and quantize the DCT results.
  NOTES 
   DCT includes preshifting the input by -128; in our DCT, this is
   equivalent to preshifting the DC value by 8192 prior to scaling and
   quantizing the DCT output.

   Quantized DCT values in 8x8 array is output in zigzag order as per
   JPEG's zigzag scanning order.
 
   To use the code modules, the routine 'fwft_fwd_scale' must be
   invoked at the start of the compression process and during compression
   process, each time a new quantizer specification is required.
   
  METHOD
   This implementation is based on computation of a 8 point DCT using
   a 16 point Winograd Fourier Transform.
   The winograd fourier transform method requires 5 multiplies and 25
   additions for a 8 point dct.
  
   Let X[] be a 8x8 input array for which we need to perform the DCT
   and quantization.
  
         | x00 x01 x02 x03 x04 x05 x06 x07 |
         | x10 x11 x12 x13 x14 x15 x16 x17 |
         | x20 x21 x22 x23 x24 x25 x26 x27 |
   X[] = | x30 x31 x32 x33 x34 x35 x36 x37 |
         | x40 x41 x42 x43 x44 x45 x46 x47 |
         | x50 x51 x52 x53 x54 x55 x56 x57 |
         | x60 x61 x62 x63 x64 x65 x66 x67 |
         | x70 x71 x72 x73 x74 x75 x76 x77 |
  
   1. For each row of X[], perform a 8 point dct and replace the row in X[]
      by the dct output.
      Note that input to 8 point dct is in order, i.e. x0, x1, ..., x7, but,
      output is out of order, i.e. y0, y4, y2, y6, y5, y1, y7, y3.
   2. After all eight row dcts have been computed, for each column perform
      a 8 point dct and write it to 8x8 array T[] in column order. 
   3. After all eight column dcts have been performed, the resulting array
      is post-multiplied, point-by-point by a scaling matrix S[] to yield
      a quantized dct output for X[] as Y[].
      If we denote T[] = {t(i,j)}, S[] = {s(i,j)}, and Y[] = {y(i,j)},
      then,
             y(i,j) = s(i,j) * t(i,j)
  
      Define a[n] as
         a[n] = 2 C[n] / cos (n pi / 16),   pi = 3.1415....
         and,
                C[0] = 1 / sqrt(2), C[1] = C[2] = ... = C[7] = 1,
      Define b[n] as
         b0 = a[0] / 8
         bi = a[i] / 16, i = 1,2,...,7
  
    Then, scaling matrix S[] has the following structure
    | b0b0/q00 b1b0/q01 b2b0/q02 b3b0/q03 b4b0/q04 b5b0/q05 b6b0/q06 b7b0/q07 |
    | b0b1/q10 b1b1/q11 b2b1/q12 b3b1/q13 b4b1/q14 b5b1/q15 b6b1/q16 b7b1/q17 |
    | b0b2/q20 b1b2/q21 b2b2/q22 b3b2/q23 b4b2/q24 b5b2/q25 b6b2/q26 b7b2/q27 |
    | b0b3/q30 b1b3/q31 b2b3/q32 b3b3/q33 b4b3/q34 b5b3/q35 b6b3/q36 b7b3/q37 |
    | b0b4/q40 b1b4/q41 b2b4/q42 b3b4/q43 b4b4/q44 b5b4/q45 b6b4/q46 b7b4/q47 |
    | b0b5/q50 b1b5/q51 b2b5/q52 b3b5/q53 b4b5/q54 b5b5/q55 b6b5/q56 b7b5/q57 |
    | b0b6/q60 b1b6/q61 b2b6/q62 b3b6/q63 b4b6/q64 b5b6/q65 b6b6/q66 b7b6/q67 |
    | b0b7/q70 b1b7/q71 b2b7/q72 b3b7/q73 b4b7/q74 b5b7/q75 b6b7/q76 b7b7/q77 |
  
    Note that q00,q01,...,q60,..,q77 is the quantization matrix specified
    during the compression stage.
    Note that in the above scaling matrix description,
    bibj = bjbi, and bibj implies multiplying bi with bj.
    The scaling matrix can be precomputed at the start of the component
    scan (JPEG terminology).
 
   4. The Y[] matrix values must be rounded to meet JPEG specifications,
      i.e. if X[] is comprised of unsigned 8 bit values, Y[] must be adjusted
      to signed 12 bit values. (i.e. numbers in the range -2047, 2047).
*/


    /*  ------------------------ _iljpgEnDCTScale ------------------------------- */
    /*  Called by iljpgEncodeInit() to scale the Q table pointed to by "pSrc"
        into "pDst", each a 64 entry table.
    */
ILJPG_PRIVATE iljpgError _iljpgEnDCTScale (
    iljpgPtr            pSrc,
    float              *pDst
    )
{
  int   i, j, k, temp;
  double         a, c0, b[8], pi;
  float *sptr;
  iljpgPtr qptr;
  int   *cptr;
   
  pi   = 4.0 * atan(1.0);
  c0   = 1.0 / (2.0 * 0.707106718);
  a    =  2 * c0;
  b[0] = a / 4.0;
  for (i = 1; i < 8; i++) {
       a    = 2 / cos (i * pi / 16.0);
       b[i] = a / 8.0;
  }

    /*  scale and quantize dct output after computing 2-D DCT.
        The Q table is in zigzag order: de-zigzag while accessing.
    */
  sptr = pDst;
  qptr = pSrc;
  for (i = 0, k = 0; i < 8; i++) {
      for (j = 0; j < 8; j++) {
          temp = qptr[_iljpgZigzagTable[k++]];
          if (temp == 0)
            return ILJPG_ERROR_ENCODE_Q_TABLE;
          *sptr++ = b[i] * b[j] / (float)temp;
      }
  }

#ifdef DEBUG
  fprintf(fp_dbg,"\nforward dct routine.. post-scaling matrix\n");
  for (i = 0; i < 64; i++) { 
      if ((i % 8) == 0) fprintf(fp_dbg,"\n");
      fprintf(fp_dbg,"%8.4f ",s[i]);
  }
#endif

  return 0;
} 


    /*  ------------------------ fwfwddct_8x8 ------------------------------------- */
static void fwfwddct_8x8 (
    int   *ix, /* pointer to 8x8 input array whose DCT is to be computed */
    float *ox  /* pointer to 8x8 output array containing DCT coefficients */
    )
{
  float in0, in1, in2, in3, in4, in5, in6, in7;
  float tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7;
  float tmp;
  int   *ixaddr;
  float *oxaddr;
  int            i;
  int            j;
  /* constants needed by the 16 point Winograd Fourier Transform method */
  float a1 =  0.707106718;
  float a2 = -0.541196100;
  float a3 =  0.707106718;
  float a4 =  1.306562963;
  float a5 =  0.382683432;

#ifdef DEBUG
  ixaddr = ix;
  fprintf(fp_dbg,"\nforward dct routine.. before row dct\n");
  for (j = 0; j < 64; j++, ixaddr++) { 
      if ((j % 8) == 0) fprintf(fp_dbg,"\n");
      fprintf(fp_dbg,"%8.4f ",(float)(*ixaddr));
  }
#endif

  ixaddr  = ix;
  oxaddr  = ox;
  for (i = 0; i < 8; i++) {

      /* setup input data  - 8 bit range */
      in0 = *ixaddr;
      in1 = *(ixaddr+1);
      in2 = *(ixaddr+2);
      in3 = *(ixaddr+3);
      in4 = *(ixaddr+4);
      in5 = *(ixaddr+5);
      in6 = *(ixaddr+6);
      in7 = *(ixaddr+7);

#ifdef DEBUG
      fprintf(fp_dbg,"\nstart: %8.4f %8.4f %8.4f %8.4f\n", in0, in1, in2, in3);
      fprintf(fp_dbg,"         %8.4f %8.4f %8.4f %8.4f\n", in4, in5, in6, in7);
#endif
 
      /* Stage 0  - 9bits max, 1bit sign */ 
      tmp0 = in0 + in7;
      tmp1 = in1 + in6;
      tmp2 = in5 + in2;
      tmp3 = in3 + in4;
      tmp4 = in3 - in4;
      tmp5 = in2 - in5;
      tmp6 = in1 - in6;
      tmp7 = in0 - in7;

#ifdef DEBUG
      fprintf(fp_dbg,"s0: %8.4f %8.4f %8.4f %8.4f\n", tmp0, tmp1, tmp2, tmp3);
      fprintf(fp_dbg,"s0: %8.4f %8.4f %8.4f %8.4f\n", tmp4, tmp5, tmp6, tmp7);
#endif
 
      /* Stage 1  - 10bits max, 1 bit sign */
      in0 = tmp0 + tmp3;
      in1 = tmp1 + tmp2;
      in2 = tmp1 - tmp2;
      in3 = tmp0 - tmp3;
      in4 =  - tmp4 - tmp5;
      in5 =  tmp5 + tmp6;
      in6 =  tmp6 + tmp7;

#ifdef DEBUG
      fprintf(fp_dbg,"s1: %8.4f %8.4f %8.4f %8.4f\n", in0, in1, in2, in3);
      fprintf(fp_dbg,"s1: %8.4f %8.4f %8.4f %8.4f\n", in4, in5, in6, tmp7);
#endif

      /* Stage 2 - 11bits max, 1 bit sign */
      *oxaddr       = in0 + in1;  /* y0 */
      *(oxaddr+4)   = in0 - in1;  /* y4 */
      in2           = in2 + in3;

#ifdef DEBUG
      fprintf(fp_dbg,"s2: %8.4f %8.4f %8.4f %8.4f\n", 
                     *oxaddr, *(oxaddr+4), in2, in3);
      fprintf(fp_dbg,"s2: %8.4f %8.4f %8.4f %8.4f\n", in4, in5, in6, in7);
#endif

      /* Stage 2.1  - 12bit for partial add, 1bit sign */
      tmp   = (in6 + in4);
      tmp   = a5 * tmp;


      /* Stage 3 - 13 bit, 1 bit sign */
      tmp2  = a1 * in2;
      tmp4  = a2 * in4 - tmp;
      tmp5  = a3 * in5;
      tmp6  = a4 * in6 - tmp;

#ifdef DEBUG
      fprintf(fp_dbg,"s0: %8.4f %8.4f %8.4f %8.4f\n", 
              *oxaddr, *(oxaddr+4), tmp2, in3);
      fprintf(fp_dbg,"s0: %8.4f %8.4f %8.4f %8.4f\n", tmp4, tmp5, tmp6, tmp7);
#endif
 
      /* Stage 4 */
      *(oxaddr+2) =  tmp2 + in3;  /* y2 */
      *(oxaddr+6) =  in3 - tmp2;  /* y6 */
      in5         =  tmp5 + tmp7;
      in7         =  tmp7 - tmp5;

#ifdef DEBUG
      fprintf(fp_dbg,"s0: %8.4f %8.4f %8.4f %8.4f\n", 
              *oxaddr, *(oxaddr+4), *(oxaddr+2), *(oxaddr+6));
      fprintf(fp_dbg,"s0: %8.4f %8.4f %8.4f %8.4f\n", 
              tmp4, in5, tmp6, in7);
#endif

      /* Stage 5 */
      *(oxaddr+5) = tmp4 + in7;  /* y5 */
      *(oxaddr+1) = in5 + tmp6;  /* y1 */
      *(oxaddr+7) = in5 - tmp6;  /* y7 */
      *(oxaddr+3) = in7 - tmp4;  /* y3 */

#ifdef DEBUG
      fprintf(fp_dbg,"s0: %8.4f %8.4f %8.4f %8.4f\n", 
              *oxaddr, *(oxaddr+4), *(oxaddr+2), *(oxaddr+6));
      fprintf(fp_dbg,"s0: %8.4f %8.4f %8.4f %8.4f\n", 
              *(oxaddr+5), *(oxaddr+1), *(oxaddr+7), *(oxaddr+3));
#endif

      oxaddr += 8;               /* pointer to next row of output */
      ixaddr += 8;               /* pointer to next row of input */

  }

#ifdef DEBUG
  fprintf(fp_dbg,"\nforward dct routine.. after row dct\n");
  oxaddr = ox;
  for (j = 0; j < 64; j++, oxaddr++) { 
      if ((j % 8) == 0) fprintf(fp_dbg,"\n");
      fprintf(fp_dbg,"%8.4f ",*oxaddr);
  }
#endif 

  oxaddr = ox;
  for (i = 0; i < 8; i++) {     /* perform 1-D DCT along columns */

      /* setup input data  - 8 bit range */
      in0 = *oxaddr;
      in1 = *(oxaddr+8);
      in2 = *(oxaddr+16);
      in3 = *(oxaddr+24);
      in4 = *(oxaddr+32);
      in5 = *(oxaddr+40);
      in6 = *(oxaddr+48);
      in7 = *(oxaddr+56);

#ifdef DEBUG
      fprintf(fp_dbg,"\nstart: %8.4f %8.4f %8.4f %8.4f\n", in0, in1, in2, in3);
      fprintf(fp_dbg,"\nstart: %8.4f %8.4f %8.4f %8.4f\n", in4, in5, in6, in7);
#endif
 
      /* Stage 0  - 9bits max, 1bit sign */ 
      tmp0 = in0 + in7;
      tmp1 = in1 + in6;
      tmp2 = in5 + in2;
      tmp3 = in3 + in4;
      tmp4 = in3 - in4;
      tmp5 = in2 - in5;
      tmp6 = in1 - in6;
      tmp7 = in0 - in7;

#ifdef DEBUG
      fprintf(fp_dbg,"s0: %8.4f %8.4f %8.4f %8.4f\n", tmp0, tmp1, tmp2, tmp3);
      fprintf(fp_dbg,"s0: %8.4f %8.4f %8.4f %8.4f\n", tmp4, tmp5, tmp6, tmp7);
#endif
 
      /* Stage 1  - 10bits max, 1 bit sign */
      in0 = tmp0 + tmp3;
      in1 = tmp1 + tmp2;
      in2 = tmp1 - tmp2;
      in3 = tmp0 - tmp3;
      in4 =  - tmp4 - tmp5;
      in5 =  tmp5 + tmp6;
      in6 =  tmp6 + tmp7;

#ifdef DEBUG
      fprintf(fp_dbg,"s1: %8.4f %8.4f %8.4f %8.4f\n", in0, in1, in2, in3);
      fprintf(fp_dbg,"s1: %8.4f %8.4f %8.4f %8.4f\n", in4, in5, in6, tmp7);
#endif
  
      /* Stage 2 - 11bits max, 1 bit sign */
      *oxaddr       = in0 + in1;  /* y0 */
      *(oxaddr+32)  = in0 - in1;  /* y4 */
      in2         += in3;

#ifdef DEBUG
      fprintf(fp_dbg,"s2: %8.4f %8.4f %8.4f %8.4f\n", *oxaddr, *(oxaddr+32),
              in2, in3);
      fprintf(fp_dbg,"s2: %8.4f %8.4f %8.4f %8.4f\n", in4, in5, in6, tmp7);
#endif
  
      /* Stage 2.1  - 12bit for partial add, 1bit sign */
      tmp   = (in6 + in4);
      tmp   = a5 * tmp;


      /* Stage 3 - 13 bit, 1 bit sign */
      tmp2  = a1 * in2;
      tmp4  = a2 * in4 - tmp;
      tmp5  = a3 * in5;
      tmp6  = a4 * in6 - tmp;

#ifdef DEBUG
      fprintf(fp_dbg,"s3: %8.4f %8.4f %8.4f %8.4f\n", 
             *oxaddr, *(oxaddr+32), tmp2, in3);
      fprintf(fp_dbg,"s3: %8.4f %8.4f %8.4f %8.4f\n", 
              tmp4, tmp5, tmp6, tmp7);
#endif

      /* Stage 4 */
      *(oxaddr+16) =  tmp2 + in3;  /* y2 */
      *(oxaddr+48) =  in3 - tmp2;  /* y6 */
      in5         =  tmp5 + tmp7;
      in7         =  tmp7 - tmp5;

#ifdef DEBUG
      fprintf(fp_dbg,"s4: %8.4f %8.4f %8.4f %8.4f\n", 
              *oxaddr, *(oxaddr+32), *(oxaddr+16), *(oxaddr+48));
      fprintf(fp_dbg,"s4: %8.4f %8.4f %8.4f %8.4f\n",
              tmp4, in5,tmp6,in7);
#endif

      /* Stage 5 */
      *(oxaddr+40) = tmp4 + in7;  /* y5 */
      *(oxaddr+8)  = in5 + tmp6;  /* y1 */
      *(oxaddr+56) = in5 - tmp6;  /* y7 */
      *(oxaddr+24) = in7 - tmp4;  /* y3 */

#ifdef DEBUG
      fprintf(fp_dbg,"s5: %8.4f %8.4f %8.4f %8.4f\n",
              *oxaddr, *(oxaddr+32), *(oxaddr+16), *(oxaddr+48)); 
      fprintf(fp_dbg,"s5: %8.4f %8.4f %8.4f %8.4f\n", 
              *(oxaddr+40), *(oxaddr+8), *(oxaddr+56), *(oxaddr+24));
#endif

      oxaddr += 1;               /* pointer to next column */
   }

#ifdef DEBUG
  fprintf(fp_dbg,"\nforward dct routine.. after column dct\n");
  oxaddr = ox;
  for (j = 0; j < 64; j++, oxaddr++) { 
      if ((j % 8) == 0) fprintf(fp_dbg,"\n");
      fprintf(fp_dbg,"%8.4f ",*oxaddr);
  }
#endif 

} 


    /*  -------------------------------- _iljpgEnDCT ------------------------------- */
    /*  DCT encode the 64 int matrix pointed to by "pSrc", storing the results back
        into the same matrix.  "pScale" must point to the 64 float scaled/clipped 
        Q table, as setup by _iljpgEnDCTInit().
    */
ILJPG_PRIVATE _iljpgEnDCT (
    int        *pSrc,
    float      *pScale
    )
{
  int   i, value;
  float *sptr;
  int   *ixptr;
  float *oxptr;
  int   *zptr;
  float  fvalue;
  float           ox[64];   /* DCT coefficient buffer */


  fwfwddct_8x8(pSrc,ox); /* perform 1-D DCT along rows, then columns */ 

  /* scale dct and quantize dct values */
  oxptr = ox;
  sptr  = pScale;
  ixptr  = pSrc;

  *oxptr -= 8192; /* for JPEG shift by -128 */
  
  /* scale and quantize DC value. JPEG expects rounding during this process */
  /* clip quantized values to the range -2047 to 2047 */ 
  fvalue = *oxptr++ * *sptr++;
  if (fvalue < 0.0) fvalue -= 0.5;
  else fvalue += 0.5;
  value = (int)fvalue;
  if (value < -2047) value = -2047;
  else if (value > 2047) value = 2047;
  *ixptr = value;

  /* scale and quantize AC values. JPEG expects rounding during this process */
  /* clip quantized values to the range -1023 to 1023 */ 
  zptr    = &_iljpgZigzagTable[1];       /* zigzag addressing included */
  for (i = 1; i < 64; i++) {
      fvalue = *oxptr++ * *sptr++;
      if (fvalue < 0.0) fvalue -= 0.5;
      else fvalue += 0.5;
      value = (int)fvalue;
      if (value < -1023) value = -1023;
      else if (value > 1023) value = 1023;
      *(ixptr + *zptr++) = value;
  }
  
#ifdef DEBUG

  fprintf(fp_dbg,"\nforward dct routine.. after scaling\n");
  ixptr = pSrc;
  for (i = 0; i < 64; i++, ixptr++) { 
      if ((i % 8) == 0) fprintf(fp_dbg,"\n");
      fprintf(fp_dbg,"%8.4f ",(float)(*ixptr));
  }

#endif
}

