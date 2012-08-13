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
/*%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			 */
/*%%  (c) Copyright 1993, 1994 International Business Machines Corp.	 */
/*%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			 */
/*%%  (c) Copyright 1993, 1994 Novell, Inc. 				 */
/*%%  $XConsortium: isdatconv.c /main/3 1995/10/23 11:37:16 rswiston $ 			 				 */
#ifndef lint
static char sccsid[] = "@(#)isdatconv.c 1.5 89/07/17 Copyr 1988 Sun Micro";
#endif
/*
 * Copyright (c) 1988 by Sun Microsystems, Inc.
 */

/*
 * isdatconv.c
 *
 * Description:
 *      Conversion function between machine dependent and the X/OPEN 
 *	machine	independent formats.
 *
 * Some pieces of code may not be very "structured", but they result in
 * optimized code with the -O compiler option.
 *
 */

#include "isam_impl.h"

#define BLANK	' '

/* conversion functions for sparc architecture */

/* ldlong() - Load a long integer from a potentially  unaligned address */

long 
ldlong(p)
    register char     	*p;
{
    int i;
#if LONG_BIT == 64
    register unsigned long val;
#else
    register unsigned int val;
#endif

    val = 0;
    for (i=0; i<LONGSIZE ; i++)  
        val = (val << 8) + *((unsigned char *)p++);
    
    return ((long)val);
}

/* stlong() - Store a long integer at a potentially unaligned address */

int
stlong(val, p)
    register long	val;
    register char     	*p;
{
    int i;
    p += LONGSIZE;

    for (i=0; i<LONGSIZE ; i++)
        *--p = (val >> 8*i) & 255;

    return(0);
}

/* ldint() - Load a short integer from a potentially  unaligned address */

short
ldint(p)
    register char     	*p;
{
    register unsigned int val;

    val = *((unsigned char *)p++);
    val = (val << 8) + *((unsigned char *)p++);

    return ((short)val);
}


/* ldunshort - load a unshort integer : for 64K record length */

u_short
ldunshort(p)
    register char     	*p;
{
    register unsigned int val;

    val = *((unsigned char *)p++);
    val = (val << 8) + *((unsigned char *)p++);

    return ((u_short)val);
}

/* stint() - Store a short integer at a potentially unaligned address */

int
stint(val, p)
    register short	val;
    register char     	*p;
{
    p += SHORTSIZE;
    *--p = val & 255;
    *--p = (val >> 8) & 255;

    return(0);
}

/* ldchar() - Load character field */

int
ldchar(src, len, dst)
    char		*src;
    register char	*dst;
    int			len;
{
    register char	*p;

    if (len <= 0)
	return;

    /* Load the entire string. */
    memcpy((void *) dst, (const void *) src, len);

    /* Remove trailing blanks. */
    p = dst + len;
    while (--p >= dst) {
	if (*p != BLANK) {
	    break;
	}
    }

    *++p = '\0';
}

int
stchar(src, dst, len)
    register char	*src;
    register char	*dst;
    register int	len;
{
    register char	c;

    if (len <= 0)
	return;

    /* Copy up to NULL character. */
    do {
	if ((c = *src++) == '\0') 
	    break;
	*dst++ = c;
    } while (--len > 0);
    
    /* Pad with blanks. */
    if (len > 0)
	(void) memset((void *) dst, BLANK, len);
}

/* ldchar2() - Load character field (C style, NULL padded) */
 
int
ldchar2(src, len, dst)
    char                *src;
    register char       *dst;
    int                 len;
{
    register char       *p;
 
    if (len <= 0)
        return;
 
    /* Load the entire string. */
    memcpy((void *) dst, (const void *) src, len);
    *(dst + len) = '\0';
}
 
int
stchar2(src, dst, len)
    register char       *src;
    register char       *dst;
    register int        len;
{
    register char       c;
 
    if (len <= 0)
        return;
 
    /* Copy up to a NULL character. */
    do {
        if ((c = *src++) == '\0')
            break;
        *dst++ = c;
    } while (--len > 0);
    
    /* Pad with NULLs. */
    if (len > 0)
        memset(dst, 0, len);
}

/* ldfloat() - Load a float number from a potentially  unaligned address */

float
ldfloat(p)
    register char     	*p;
{
    union {
	float fval;
	int   ival;
    } uval;
    register unsigned int val;

    val = *((unsigned char *)p++);
    val = (val << 8) + *((unsigned char *)p++);
    val = (val << 8) + *((unsigned char *)p++);
    val = (val << 8) + *((unsigned char *)p++);

    uval.ival = val;
    return (uval.fval);
}

/* stfloat() - Store a float number at a potentially unaligned address */

int
stfloat(f, p)
    float		f;		     /* Bug - it is passed as double */
    register char     	*p;
{
    register unsigned  	val;
    union {
	float fval;
	int   ival;
    } uval;

    uval.fval = f;			     /* This fixes compiler bug */
    val = uval.ival;

    p += LONGSIZE;
    *--p = val & 255;
    *--p = (val >> 8) & 255;
    *--p = (val >> 16) & 255;
    *--p = (val >> 24) & 255;

    return(0);
}

#if sparc | mc68000    /* MRJ */

/* ldbld() - Load a double float number from a potentially unaligned address */

double
lddbl(p)
    register char     	*p;
{
    double val;

    memcpy((void *)&val, (const void *) p, DOUBLESIZE);
    return (val);
}

/* stdbl() - Store a double float number at a potentially unaligned address */

int
stdbl(val, p)
    double		val;
    register char     	*p;
{
    memcpy ( p,(char *)&val, DOUBLESIZE);
}

#else      /* 386i -- do it the long way round....  */

/* ldbld() - Load a double float number from a potentially unaligned address */

double
lddbl(p)
    register char     	*p;
{
    union {
        double rval;
	char   sval[DOUBLESIZE];
    } x;

    char  *q;
    int  i;

    q  =  x.sval;
    p +=  DOUBLESIZE;
   
    for (i=0; i<DOUBLESIZE; i++)
    	*q++ = *--p;
    return (x.rval);
}

/* stdbl() - Store a double float number at a potentially unaligned address */

int
stdbl(val, p)
    double		val;
    register char     	*p;
{
    union {
        double rval;
	char   sval[DOUBLESIZE];
    } x;

    char  *q;
    int  i;

    x.rval = val;
    q  =  x.sval;
    p +=  DOUBLESIZE;
   
    for (i=0; i<DOUBLESIZE; i++)
    	*--p = *q++ ;

    return(0);
}


#endif    /* sparc */


