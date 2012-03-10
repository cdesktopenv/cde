/*
 * $XConsortium: macro.h /main/4 1996/07/18 14:59:08 drk $
 *
 * Copyright (c) 1993 HAL Computer Systems International, Ltd.
 * All rights reserved.  Unpublished -- rights reserved under
 * the Copyright Laws of the United States.  USE OF A COPYRIGHT
 * NOTICE IS PRECAUTIONARY ONLY AND DOES NOT IMPLY PUBLICATION
 * OR DISCLOSURE.
 * 
 * THIS SOFTWARE CONTAINS CONFIDENTIAL INFORMATION AND TRADE
 * SECRETS OF HAL COMPUTER SYSTEMS INTERNATIONAL, LTD.  USE,
 * DISCLOSURE, OR REPRODUCTION IS PROHIBITED WITHOUT THE
 * PRIOR EXPRESS WRITTEN PERMISSION OF HAL COMPUTER SYSTEMS
 * INTERNATIONAL, LTD.
 * 
 *                         RESTRICTED RIGHTS LEGEND
 * Use, duplication, or disclosure by the Government is subject
 * to the restrictions as set forth in subparagraph (c)(l)(ii)
 * of the Rights in Technical Data and Computer Software clause
 * at DFARS 252.227-7013.
 *
 *          HAL COMPUTER SYSTEMS INTERNATIONAL, LTD.
 *                  1315 Dell Avenue
 *                  Campbell, CA  95008
 * 
 */



#ifndef _macro_h
#define _macro_h 1

/* bit manipulation macros */
#define BIT_TEST(x, y)  ( ((x) & (y)) == (y) )
#define RESET_BIT(x, y)  x &= (~(y)) 
#define SET_BIT(x, y)    x |= (y)  
#define BITS_IN(TYPE)    ( 8*sizeof(TYPE) )

/* comparison macros */
#define INRANGE(x, low, high) ((x>=low) && (x<=high))  

#undef MIN
#undef MAX

#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#define MAX(x, y) (((x) > (y)) ? (x) : (y))

#define TOBOOLEAN(x) ( ((x) == 1 ) ? true : false )
#define TOBIT(x) ( ((x) == true ) ? 1 : 0 )

/* little endian to/from big endian swap macros. */

#ifndef __osf__
#define ORDER_SWAP_LONG(x) 			\
{ 						\
   long tmp_long = x; 				\
   ((unsigned char*)&x)[0] = ((unsigned char*)&tmp_long)[3];	\
   ((unsigned char*)&x)[1] = ((unsigned char*)&tmp_long)[2];	\
   ((unsigned char*)&x)[2] = ((unsigned char*)&tmp_long)[1];	\
   ((unsigned char*)&x)[3] = ((unsigned char*)&tmp_long)[0];	\
} 
#else
#define ORDER_SWAP_LONG(x)                      \
{                                               \
   long tmp_long = x;                           \
   ((unsigned char*)&x)[0] = ((unsigned char*)&tmp_long)[7];    \
   ((unsigned char*)&x)[1] = ((unsigned char*)&tmp_long)[6];    \
   ((unsigned char*)&x)[2] = ((unsigned char*)&tmp_long)[5];    \
   ((unsigned char*)&x)[3] = ((unsigned char*)&tmp_long)[4];    \
   ((unsigned char*)&x)[4] = ((unsigned char*)&tmp_long)[3];    \
   ((unsigned char*)&x)[5] = ((unsigned char*)&tmp_long)[2];    \
   ((unsigned char*)&x)[6] = ((unsigned char*)&tmp_long)[1];    \
   ((unsigned char*)&x)[7] = ((unsigned char*)&tmp_long)[0];    \
}
#endif

#define ORDER_SWAP_FLOAT(x) 			\
{ 						\
   float tmp_float = x; 			\
   ((unsigned char*)&x)[0] = ((unsigned char*)&tmp_float)[3];	\
   ((unsigned char*)&x)[1] = ((unsigned char*)&tmp_float)[2];	\
   ((unsigned char*)&x)[2] = ((unsigned char*)&tmp_float)[1];	\
   ((unsigned char*)&x)[3] = ((unsigned char*)&tmp_float)[0];	\
} 

#define ORDER_SWAP_INT(x) 			\
{ 						\
   int tmp_uint = x; 				\
   ((unsigned char*)&x)[0] = ((unsigned char*)&tmp_uint)[3];	\
   ((unsigned char*)&x)[1] = ((unsigned char*)&tmp_uint)[2];	\
   ((unsigned char*)&x)[2] = ((unsigned char*)&tmp_uint)[1];	\
   ((unsigned char*)&x)[3] = ((unsigned char*)&tmp_uint)[0];	\
} 

#define ORDER_SWAP_UINT(x) 			\
{ 						\
   unsigned int tmp_uint = x; 			\
   ((unsigned char*)&x)[0] = ((unsigned char*)&tmp_uint)[3];	\
   ((unsigned char*)&x)[1] = ((unsigned char*)&tmp_uint)[2];	\
   ((unsigned char*)&x)[2] = ((unsigned char*)&tmp_uint)[1];	\
   ((unsigned char*)&x)[3] = ((unsigned char*)&tmp_uint)[0];	\
} 

#define ORDER_SWAP_USHORT(x) 			\
{ 						\
   unsigned short tmp_ushort = x;		\
   ((unsigned char*)&x)[0] = ((unsigned char*)&tmp_ushort)[1];	\
   ((unsigned char*)&x)[1] = ((unsigned char*)&tmp_ushort)[0];	\
} 

#endif

