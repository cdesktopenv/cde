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
/* $XConsortium: ilhplrotation.h /main/3 1995/10/23 15:48:46 rswiston $ */
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

/*
   NOTE to ALL users:

     1.See the last section of this file for an Example about how to use this
       library.

   NOTES to DOS/WINDOWS users only:

     1.Define FAR_DATA_PTR, if you want to use 32bit data pointers instead of
       the 16bit ones.  This is not necessary in Compact, Large, or Huge
       Memory Models.
*/

#ifndef NeedFunctionPrototypes
#if defined(FUNCPROTO) || defined(__STDC__) || defined(__cplusplus) || defined(c_plusplus)
#define NeedFunctionPrototypes 1
#else
#define NeedFunctionPrototypes 0
#endif /* __STDC__ */
#endif /* NeedFunctionPrototypes */


#ifdef FAR_DATA_PTR
typedef unsigned char huge * DATA_IN_PTR;
typedef unsigned char far *  DATA_OUT_PTR;
typedef unsigned char DATA_IN;
typedef unsigned char DATA_OUT;
#else
typedef unsigned char * DATA_IN_PTR;
typedef unsigned char * DATA_OUT_PTR;
typedef unsigned char DATA_IN;
typedef unsigned char DATA_OUT;
#endif

extern DATA_IN_PTR _il_Rotate_Image_Begin(
#if NeedFunctionPrototypes
    short input_w,              /* Input image width                    */
    short input_h,              /* Input image height                   */
    short input_bytes,          /* Input image bytes per pixel (1 or 3).
                                 1 : gray scale image (8 bits / pixel).
                                 3 : color image (24 bits / pixel).     */
    short rotation_mode,        /* 0 : subsampling (nearest pixel)
                                   !0 : bi-linear interpolation         */
    float angle,                /* Rotation Angle                       */
    unsigned char bg_red,       /* background color                     */
    unsigned char bg_green,     /* , if input_bytes == 1,               */ 
    unsigned char bg_blue,      /* background gray level = bg_red       */
    short *output_w,            /* Returned value for output image width*/
    short *output_h             /* Returned val. for output image height*/
#endif
    );
/*
 * Must be called before any rotation can take place.
 *
 * Returns a pointer to the location where the image data of the first row
 * must be read into, or NULL if memory can not be allocated.
 */


extern short _il_Rotate_Send(
#if NeedFunctionPrototypes
    DATA_IN_PTR *input_data     /* returned location for next input data row */
#endif
    );
/*
 * After reading the image data of the first row into the location returned by
 * IR_Rotate_Image_Begin, use this call to get the new location for the next
 * row's image data.
 *
 * Returns the number of output data rows ready to be retrieve by
 * IR_Rotate_Get_Row
 */


extern void _il_Rotate_Get_Row(
#if NeedFunctionPrototypes
    DATA_OUT_PTR row            /* output data row */
#endif
    );
/*
 * Read next output data row into the user provided buffer 'row'.
 */


extern void _il_Rotate_Image_End();
/*
 * Must be called when done with rotation to free any memory created during
 * the rotation process.
 */


/*
 * EXAMPLE:
 *
 * In order to use this rotation library, your software structure should
 * look similar to this:
 *
 * ...
 * short in_w, in_h, out_w, out_h, nrows;
 * DATA_IN_PTR data_in;
 * DATA_OUT_PTR buf;
 *
 * ...
 * data_in = IR_Rotate_Image_Begin(...,            start rotation
 *     &out_w, &out_h);
 * buf = (DATA_OUT_PTR) malloc(out_w*image_bytes); allocate output buffer
 *                                          
 * for (i = 0; i < in_h; i++) {                    for each input image row
 *     read_input_data(data_in);                     read in next data row
 *     for (nrows = IR_Rotate_Send_Row(&data_in);    get next location for
 *         nrows > 0; --nrows) {                     data_in, and for each
 *                                                   output data row
 *         IR_Rotate_Get_Row(buf);                     get output data
 *         process_data(buf);                          process it
 *     };                                            end inner for loop
 * };                                              end outer for loop
 * IR_Rotate_Image_End();                          finish rotation
 * ...
 */
