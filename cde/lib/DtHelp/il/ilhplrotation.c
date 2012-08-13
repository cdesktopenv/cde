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

static char rcsid[] = "$XConsortium: ilhplrotation.c /main/3 1995/10/23 15:48:35 rswiston $";
static char version[] = "$XConsortium: ilhplrotation.c /main/3 1995/10/23 15:48:35 rswiston $";
static char vdate[] = "$XConsortium: ilhplrotation.c /main/3 1995/10/23 15:48:35 rswiston $";

# include <stdio.h>
# include <stdlib.h>
# include <math.h>
# include <limits.h>        /* Get definition of */
# include <float.h>         /*     FLX_MAX       */
# include "ilhplrotation.h"

#ifdef MSDOS
# define P16BIT 1
#endif

#ifdef P16BIT               /* for 16bit compilers */
# define OPERAND  short
# define OPERAND2 unsigned short
# define PIXEL_UNIT 16384       /* pixel width, used to convert floating point
                                   operations into integer operations */
# define PIXEL_BITS    14       /* log(PIXEL_UNIT), must be <= 14 (16-2) */
# define INTER_LEVELS 256       /* # diff. interpolated values */
# define INTER_BITS     8       /* # diff. interp. values = 2^INTER_BITS */
# define PIX_SHIFT      6       /* must = PIXEL_BITS - INTER_BITS */
#else                       /* for 32bit compilers */
# define OPERAND  long
# define OPERAND2 long
# define PIXEL_UNIT 1073741824L /* pixel width, used to convert floating point
                                   operations into integer operations */
# define PIXEL_BITS     30      /* log(PIXEL_UNIT), must be <= 30 (32-2) */
# define INTER_LEVELS  256      /* # diff. interpolated values */
# define INTER_BITS      8      /* log(INTER_LEVELS), 10 is sufficient */
# define PIX_SHIFT      22      /* must = PIXEL_BITS - INTER_BITS */
#endif

# define PI 3.14159265359

typedef enum {Q_FIRST, Q_SECOND, Q_THIRD, Q_FOURTH, Q_0_180, Q_90_270}
    QUADRANT_TYPE;
/* 1st quad., 2nd quad., 3rd quad., 4th quad., 0 or 180 degree, 90 or 270 */


/*
 * Build_Mult_Table
 * Build Multiplication Table: table[i][j] = (i*j) >> INTER_BITS where
 *     0 <= i < INTER_LEVELS, -255 <= j <= 255.
 * Return 0 if successful, -1 if memory can't be allocated.
 */
static short Build_Mult_Table(table)
OPERAND *table[INTER_LEVELS];
{
    short i, j;
    OPERAND value, *p;

    /* Allocate Memory */
    for (i = 0; i < INTER_LEVELS; i++) {
        if ((table[i] = (OPERAND *) malloc(511*sizeof(OPERAND)))==NULL) {
            for (j = 0; j < i; j++) {
                free(table[j]); table[j] = NULL;
            }
            return(-1);
        }
    }

    /* Fill in Data */
    for (i = 0; i < INTER_LEVELS; i++) {
        for (p = table[i],value = -255*i, j= -255; j <256; j++, value += i) {
            *p++ = value >> INTER_BITS;
        }
    }

    /* Shift all table[j] pointers */
    for (i = 0; i < INTER_LEVELS; i++) {
        table[i] += 255;
    }

    return(0);
}


/*
 * Free_Mult_Table
 * Free space of Multiplication Table.
 */
static void Free_Mult_Table(table)
OPERAND *table[INTER_LEVELS];
{
    short i;

    /* Shift then free all table[j] pointers */
    for (i = 0; i < INTER_LEVELS; i++) {
        if (table[i] != NULL) {
            free(table[i] - 255);
            table[i] = NULL;
        }
    }
}

/* Static Variables */
static short r_input_w , r_input_h;    /* dimension of input image */
static short r_output_w, r_output_h;   /* dimension of output image */
static short r_image_bytes;            /* number of bytes per image pixel */
static short r_rotation_mode;          /* rotation mode */
static unsigned char r_bg_red,         /* background color, r_bg_red is background */
        r_bg_green, r_bg_blue;         /*     gray level if r_image_bytes == 1 */
static float r_angle;                  /* rotation angle */
static float r_sinA, r_cosA;           /* sin, cos of rotation angle */
static float r_tanA, r_cotA;           /* tan, cotan of rotation angle */
static QUADRANT_TYPE r_quadrant;       /* which quadrant that r_angle belongs to */
static DATA_IN_PTR r_buf = NULL;       /* contains image data(RGB interl. if color)*/
static DATA_IN_PTR r_data;             /* points to 1st pixel in r_buf */
static OPERAND r_next_row_out;         /* next output row number */
static OPERAND r_next_row_in ;         /* next  input row number */
static OPERAND r_first_mark;           /* These 2 variables are used to compute */
static OPERAND r_last_mark;            /*     First_Pixel and Last_Pixel */
static short   r_mul_tbls_valid = 0;   /* validity of multiplication tables below */
static OPERAND *r_mul_tbls[INTER_LEVELS];


static void Free_All_Buffers()
{
    if (r_buf != NULL) free(r_buf);
    r_buf = NULL;

    if (r_mul_tbls_valid) {
        Free_Mult_Table(r_mul_tbls);
        r_mul_tbls_valid = 0;
    }
}


static OPERAND First_Mark()
{
    if (r_rotation_mode == 0) {
        return((OPERAND) (r_input_w*r_sinA - 0.5));
    } else {
        return((OPERAND) (r_input_w*r_sinA));
    }
}


static OPERAND First_Pixel(row_num)
OPERAND row_num;
{
    if ((r_angle ==   0.0) || (r_angle ==  90.0) ||
        (r_angle == 180.0) || (r_angle == 270.0)) {
        return(0);
    }

    if (r_rotation_mode == 0) {
        if (row_num <= r_first_mark) {
            return(r_input_w*r_cosA - (row_num+0.5)*r_cotA + 0.5);
        } else {
            return((row_num+0.5 - r_input_w*r_sinA)*r_tanA + 0.5);
        }
    } else {
        if (row_num < r_first_mark) {
            return(r_input_w*r_cosA - (row_num+1)*r_cotA);
        } else if (row_num == r_first_mark) {
            return(0);
        } else {
            return((row_num - r_input_w*r_sinA)*r_tanA);
        }
    }
}


static OPERAND Last_Mark()
{
    if (r_rotation_mode == 0) {
        return((OPERAND) (r_input_h*r_cosA - 0.5));
    } else {
        return((OPERAND) (r_input_h*r_cosA));
    }
}


static OPERAND Last_Pixel(row_num)
OPERAND row_num;
{
    if ((r_angle == 0.0) || (r_angle == 180.0)) {
        return(r_input_w);
    } else if ((r_angle == 90.0) || (r_angle == 270.0)) {
        return(r_input_h);
    }

    if (r_rotation_mode == 0) {
        if (row_num <= r_last_mark) {
            return(r_input_w*r_cosA + (row_num+0.5)*r_tanA - 0.5);   
        } else {
            return(r_input_w*r_cosA + r_input_h/r_sinA -
                (row_num+0.5)*r_cotA - 0.5);
        }
    } else {
        if (row_num < r_last_mark) {
            return(r_input_w*r_cosA + (row_num+1)*r_tanA);   
        } else if (row_num == r_last_mark) {
            return(r_output_w - 1);
        } else {
            return(r_input_w*r_cosA + r_input_h/r_sinA - row_num*r_cotA);
        }
    }
}


static short Output_Width()
{
    if (r_rotation_mode == 0) {
        return((short) (r_input_w*r_cosA + r_input_h*r_sinA + 0.5));
    } else {
        return((short) ceil(r_input_w*r_cosA + r_input_h*r_sinA));
    }
}


static short Output_Height()
{
    if (r_rotation_mode == 0) {
        return((short) (r_input_w*r_sinA + r_input_h*r_cosA + 0.5));
    } else {
        return((short) ceil(r_input_w*r_sinA + r_input_h*r_cosA));
    }
}

/*
 * IR_Rotate_Image_Begin
 * Returns a pointer to the location where the image data of the first row
 * must be read into, or NULL if memory can not be allocated.
 */
DATA_IN_PTR _il_Rotate_Image_Begin(
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
)
{
    double angleRad;
    DATA_IN_PTR top, bot;
    short i;
    
    /* Initialize global variables */
    Free_All_Buffers();
    if (rotation_mode == 0) {
        r_buf = (DATA_IN_PTR) malloc((size_t)input_w*(size_t)input_h*
            input_bytes*sizeof(DATA_IN));
        r_data = r_buf;
    } else {
        /* Add 2 pix above, below, to the left & right of the orig. image */
        r_buf = (DATA_IN_PTR) malloc((size_t)(input_w+4)*(size_t)(input_h+4)*
            input_bytes*sizeof(DATA_IN));
        r_data = r_buf + ((input_w+4)*2 + 2)*input_bytes;
    }
    if (r_buf == NULL) {
        Free_All_Buffers();
        return(NULL);
    }

    r_input_w = input_w; r_input_h = input_h;
    r_image_bytes = input_bytes;
    r_rotation_mode = rotation_mode;
    r_bg_red = bg_red; r_bg_green = bg_green; r_bg_blue = bg_blue;

    /* Normalize angle to the range [0, 360) */
    r_angle = angle;
    while (r_angle <    0.0) r_angle += 360.0;
    while (r_angle >= 360.0) r_angle -= 360.0;
    if      (r_angle ==   0.0) r_quadrant = Q_0_180;
    else if (r_angle <   90.0) r_quadrant = Q_FIRST;
    else if (r_angle ==  90.0) r_quadrant = Q_90_270;
    else if (r_angle <  180.0) r_quadrant = Q_SECOND;
    else if (r_angle == 180.0) r_quadrant = Q_0_180;
    else if (r_angle <  270.0) r_quadrant = Q_THIRD;
    else if (r_angle == 270.0) r_quadrant = Q_90_270;
    else                       r_quadrant = Q_FOURTH;
    angleRad = ((double) angle*PI) / 180.0;
    r_sinA = sin(angleRad); r_cosA = cos(angleRad);
    if ((r_angle == 0.0) || (r_angle == 180.0)) {
        r_tanA = 0.0; r_cotA = FLT_MAX;
    } else if ((r_angle == 90.0) || (r_angle == 270.0)) {
        r_tanA = FLT_MAX; r_cotA = 0.0;
    } else {
        r_tanA = tan(angleRad); r_cotA = 1.0/r_tanA;
    }
    r_next_row_out = 0;
    r_next_row_in  = 0;

    /* These calls must follow the init. of r_input_w, r_rotation_mode... */
    r_first_mark = First_Mark();
    r_last_mark  = Last_Mark();
    *output_w = r_output_w = Output_Width();
    *output_h = r_output_h = Output_Height();

    if (rotation_mode != 0) {
        /* Fill in the extra pixels with background color */
        top = r_buf; bot = r_buf + ((input_h+2)*(input_w+4) - 2)*input_bytes;
        if (input_bytes == 1) {
            for (i = (input_w+4)*2 + 2; i > 0; --i) {
                *top++ = bg_red;
                *bot++ = bg_red;
            }
        } else {
            for (i = (input_w+4)*2 + 2; i > 0; --i) {
                *top++ = bg_red; *top++ = bg_green; *top++ = bg_blue;
                *bot++ = bg_red; *bot++ = bg_green; *bot++ = bg_blue;
            }
        }
        r_mul_tbls_valid = !Build_Mult_Table(r_mul_tbls);
    }

    return(r_data);
}

/*
 * IR_Rotate_Send_Row
 */
short _il_Rotate_Send_Row(
DATA_IN_PTR *input_data
)
{
    DATA_IN_PTR data;
    short i;

    data = *input_data + r_input_w*r_image_bytes;
    if (r_rotation_mode != 0) {
        if (r_image_bytes == 1) {
            for (i = 0; i < 4; i++) {
                *data++ = r_bg_red;
            }
        } else {
            for (i = 0; i < 4; i++) {
                *data++ = r_bg_red;
                *data++ = r_bg_green;
                *data++ = r_bg_blue;
            }
        }
    }

    *input_data = data;
    if (++r_next_row_in == r_input_h) {
        return(r_output_h);
    } else {
        return(0);
    }
}

/*
 * IR_Rotate_Get_Row_Mode0
 */
static void _il_Rotate_Get_Row_Mode0(out)
DATA_OUT_PTR out;
{
    OPERAND first_pix, last_pix, col;
    float Y, x, y;
    OPERAND ex, ey, dx, dy;
    register DATA_IN_PTR data;

    /* Find coord of first & last pixels in the new image coord. system */
    first_pix = First_Pixel(r_next_row_out);
    last_pix  = Last_Pixel (r_next_row_out);

    /* Find coord of first pixel in the original image coord. system */
    Y = r_next_row_out + 0.5 - r_input_w*r_sinA;
    x = (first_pix + 0.5)*r_cosA - Y*r_sinA;
    y = (first_pix + 0.5)*r_sinA + Y*r_cosA;

    /*
       ex, ey are the distances from x, y respectively to the upper left
       corner of the pixel surrounding the point (x,y), i.e.
       ________
       |      |   ^
       |      |  ey
       |    x |-- v
       |______|
       |<ex>|
    */

    ex = (x - (long) x)*(double)PIXEL_UNIT;
    ey = (y - (long) y)*(double)PIXEL_UNIT;

    /* Do Not round off, Truncate instead to avoid overflow */
    dx = (OPERAND)(r_cosA*PIXEL_UNIT) - PIXEL_UNIT;
    dy = (OPERAND)(r_sinA*PIXEL_UNIT) - PIXEL_UNIT;

    if (r_image_bytes == 3) {
        size_t input_row_size;

        for (col = 0; col < first_pix; col++) {
            *out++ = r_bg_red; *out++ = r_bg_green; *out++ = r_bg_blue;
        }

        input_row_size = r_input_w*(size_t)3;
        data = r_data + (size_t)3*(((long)y-1)*r_input_w + ((long)x));
        for (; col <= last_pix; col++, ex += dx, ey += dy) {
            if (ex < 0) {
                ex += PIXEL_UNIT;
                data -= 3;
            }
            if (ey >= 0) {
                data += input_row_size;
            } else {
                ey += PIXEL_UNIT;
            }
            *out++ = *data++; *out++ = *data++; *out++ = *data++;
        }

        for (; col < r_output_w; col++) {
            *out++ = r_bg_red; *out++ = r_bg_green; *out++ = r_bg_blue;
        }
    } else {
        for (col = 0; col < first_pix; col++) {
            *out++ = r_bg_red;
        }

        data = r_data + ((long)y-1)*r_input_w + ((long)x-1);
        for (; col <= last_pix; col++, ex += dx, ey += dy) {
            if (ex >= 0) {
                ++data;
            } else {
                ex += PIXEL_UNIT;
            }
            if (ey >= 0) {
                data += r_input_w;
            } else {
                ey += PIXEL_UNIT;
            }
            *out++ = *data;
        }

        for (; col < r_output_w; col++) {
            *out++ = r_bg_red;
        }
    }

    ++r_next_row_out;
}


/*
 * IR_Rotate_Get_Row_Mode1
 */
static void _il_Rotate_Get_Row_Mode1(out)
DATA_OUT_PTR out;
{
    OPERAND first_pix, last_pix, col;
    float Y, x, y;
    OPERAND ex, ey, dx, dy;
    register DATA_IN_PTR data, data2;
    OPERAND val1, val2, x1, y1, t;
    size_t input_row_size;

    /* Find coord of first & last pixels in the new image coord. system */
    first_pix = First_Pixel(r_next_row_out);
    last_pix  = Last_Pixel (r_next_row_out);

    /* Find coord of first pixel in the original image coord. system */
    Y = r_next_row_out + 0.5 - r_input_w*r_sinA;
    x = (first_pix + 0.5)*r_cosA - Y*r_sinA;
    y = (first_pix + 0.5)*r_sinA + Y*r_cosA;

    /*
       ex, ey are the distances from x, y respectively to the center of
       the pixel right above and to the left of the point(x,y) , i.e.

       *      *   ^
                 ey
            x  -- v        (the center of the upper left pixel is at
                            (floor(x-0.5)+0.5, floor(y-0.5)+0.5) )
       *      *
       |<ex>|
    */
 
    ex = (x - (floor(x-0.5)+0.5))*(double)PIXEL_UNIT;
    ey = (y - (floor(y-0.5)+0.5))*(double)PIXEL_UNIT;

    /* Do Not round off, Truncate instead to avoid overflow */
    dx = (OPERAND)(r_cosA*PIXEL_UNIT) - PIXEL_UNIT;
    dy = (OPERAND)(r_sinA*PIXEL_UNIT) - PIXEL_UNIT;

    input_row_size = (r_input_w+4)*(size_t)r_image_bytes;

    if (r_image_bytes == 3) {
        for (col = 0; col < first_pix; col++) {
            *out++ = r_bg_red; *out++ = r_bg_green; *out++ = r_bg_blue;
        }

        if (r_mul_tbls_valid) {
            OPERAND *tex, *tey;

            data = r_data + (((size_t)floor(y-0.5)-1)*(r_input_w+4) +
                (size_t)floor(x-0.5))*r_image_bytes;
            data2 = data + input_row_size;
            for (; col <= last_pix; col++, ex += dx, ey += dy) {
                if (ex < 0) {
                    ex += PIXEL_UNIT;
                    data  -= 3;
                    data2 -= 3;
                }
                if (ey >= 0) {
                    data  += input_row_size;
                    data2 += input_row_size;
                } else {
                    ey += PIXEL_UNIT;
                }
                tex = r_mul_tbls[ex >> PIX_SHIFT];
                tey = r_mul_tbls[ey >> PIX_SHIFT];
                x1 = *data++;
                val1 = *(tex + ((size_t)(*(data +2)) - x1)) + x1;
                x1 = *data2++;
                val2 = *(tex + ((size_t)(*(data2+2)) - x1)) + x1;
                *out++ = *(tey + (val2 - val1)) + val1;
                x1 = *data++;
                val1 = *(tex + ((size_t)(*(data +2)) - x1)) + x1;
                x1 = *data2++;
                val2 = *(tex + ((size_t)(*(data2+2)) - x1)) + x1;
                *out++ = *(tey + (val2 - val1)) + val1;
                x1 = *data++;
                val1 = *(tex + ((size_t)(*(data +2)) - x1)) + x1;
                x1 = *data2++;
                val2 = *(tex + ((size_t)(*(data2+2)) - x1)) + x1;
                *out++ = *(tey + (val2 - val1)) + val1;
            }
        } else {

#ifdef P16BIT
            data = r_data + (((size_t)floor(y-0.5)-1)*(r_input_w+4) +
                ((size_t)floor(x-0.5)-1))*r_image_bytes;
            for (; col <= last_pix; col++, ex += dx, ey += dy) {
                if (ex >= 0) {
                    data += 3;
                } else {
                    ex += PIXEL_UNIT;
                }
                if (ey >= 0) {
                    data += input_row_size;
                } else {
                    ey += PIXEL_UNIT;
                }
                data2 = data + input_row_size;
                x1 = PIXEL_UNIT - ex;
                y1 = PIXEL_UNIT - ey;
                val1 = (*(data +3)*ex + *(data ) * x1) >> PIXEL_BITS;
                val2 = (*(data2+3)*ex + *(data2) * x1) >> PIXEL_BITS;
                *out++ = (val2*ey + val1*y1) >> PIXEL_BITS;
                val1 = (*(data +4)*ex + *(data +1)*x1) >> PIXEL_BITS;
                val2 = (*(data2+4)*ex + *(data2+1)*x1) >> PIXEL_BITS;
                *out++ = (val2*ey + val1*y1) >> PIXEL_BITS;
                val1 = (*(data +5)*ex + *(data +2)*x1) >> PIXEL_BITS;
                val2 = (*(data2+5)*ex + *(data2+2)*x1) >> PIXEL_BITS;
                *out++ = (val2*ey + val1*y1) >> PIXEL_BITS;
            }
#else
            data = r_data + (((size_t)floor(y-0.5)-1)*(r_input_w+4) +
                (size_t)floor(x-0.5))*r_image_bytes;
            for (; col <= last_pix; col++, ex += dx, ey += dy) {
                if (ex < 0) {
                    ex += PIXEL_UNIT;
                    data -= 3;
                }
                if (ey >= 0) {
                    data += input_row_size;
                } else {
                    ey += PIXEL_UNIT;
                }
                data2 = data + input_row_size;
                x1 = ex >> PIX_SHIFT;
                y1 = ey >> PIX_SHIFT;
                t = *data++;
                val1 = ((((OPERAND)(*(data +2))-t)*x1) >> INTER_BITS) + t;
                t = *data2++;
                val2 = ((((OPERAND)(*(data2+2))-t)*x1) >> INTER_BITS) + t;
                *out++ = (((val2 - val1)*y1) >> INTER_BITS) + val1;
                t = *data++;
                val1 = ((((OPERAND)(*(data +2))-t)*x1) >> INTER_BITS) + t;
                t = *data2++;
                val2 = ((((OPERAND)(*(data2+2))-t)*x1) >> INTER_BITS) + t;
                *out++ = (((val2 - val1)*y1) >> INTER_BITS) + val1;
                t = *data++;
                val1 = ((((OPERAND)(*(data +2))-t)*x1) >> INTER_BITS) + t;
                t = *data2;
                val2 = ((((OPERAND)(*(data2+3))-t)*x1) >> INTER_BITS) + t;
                *out++ = (((val2 - val1)*y1) >> INTER_BITS) + val1;
            }
#endif
        }

        for (; col < r_output_w; col++) {
            *out++ = r_bg_red; *out++ = r_bg_green; *out++ = r_bg_blue;
        }
    } else {
        for (col = 0; col < first_pix; col++) {
            *out++ = r_bg_red;
        }

        if (r_mul_tbls_valid) {
            OPERAND *tex;

            data = r_data + ((size_t)floor(y-0.5)-1)*input_row_size +
                (size_t)floor(x-0.5);
            data2 = data + input_row_size;
            for (; col <= last_pix; col++, ex += dx, ey += dy) {
                if (ex < 0) {
                    ex += PIXEL_UNIT;
                    --data;
                    --data2;
                }
                if (ey >= 0) {
                    data += input_row_size;
                    data2 += input_row_size;
                } else {
                    ey += PIXEL_UNIT;
                }
                tex = r_mul_tbls[ex >> PIX_SHIFT];
                x1 = *data++;
                val1 = *(tex + ((size_t)(*data ) - x1)) + x1;
                x1 = *data2++;
                val2 = *(tex + ((size_t)(*data2) - x1)) + x1;
                *out++ = *(r_mul_tbls[ey >>PIX_SHIFT] + (val2 - val1)) + val1;
            }
        } else {

#ifdef P16BIT
            data = r_data + ((size_t)floor(y-0.5)-1)*input_row_size +
                ((size_t)floor(x-0.5)-1);
            data2 = data + input_row_size;
            for (; col <= last_pix; col++, ex += dx, ey += dy) {
                if (ex >= 0) {
                    ++data;
                    ++data2;
                } else {
                    ex += PIXEL_UNIT;
                }
                if (ey >= 0) {
                    data += input_row_size;
                    data2 += input_row_size;
                } else {
                    ey += PIXEL_UNIT;
                }
                val1 =(*(data +1)*ex + *(data )*(PIXEL_UNIT-ex)) >>PIXEL_BITS;
                val2 =(*(data2+1)*ex + *(data2)*(PIXEL_UNIT-ex)) >>PIXEL_BITS;
                *out++ =(val2*ey + val1*(PIXEL_UNIT-ey)) >> PIXEL_BITS;
            }
#else
            data = r_data + ((size_t)floor(y-0.5)-1)*input_row_size +
                (size_t)floor(x-0.5);
            data2 = data + input_row_size;
            for (; col <= last_pix; col++, ex += dx, ey += dy) {
                if (ex < 0) {
                    ex += PIXEL_UNIT;
                    --data;
                    --data2;
                }
                if (ey >= 0) {
                    data += input_row_size;
                    data2 += input_row_size;
                } else {
                    ey += PIXEL_UNIT;
                }
                x1 = ex >> PIX_SHIFT;
                t = *data++;
                val1 = ((((OPERAND)(*data) - t)*x1) >> INTER_BITS) + t;
                t = *data2++;
                val2 = ((((OPERAND)(*data2)- t)*x1) >> INTER_BITS) + t;
                *out++ = (((val2-val1)*(ey >> PIX_SHIFT)) >>INTER_BITS) +val1;
            }
#endif
        }

        for (; col < r_output_w; col++) {
            *out++ = r_bg_red;
        }
    }

    ++r_next_row_out;
}


/*
 * IR_Rotate_Get_Row
 */
void _il_Rotate_Get_Row(
DATA_OUT_PTR out
)
{
    if (r_rotation_mode == 0) _il_Rotate_Get_Row_Mode0(out);
    else                      _il_Rotate_Get_Row_Mode1(out);
}


void _il_Rotate_Image_End()
{
    Free_All_Buffers();
}
