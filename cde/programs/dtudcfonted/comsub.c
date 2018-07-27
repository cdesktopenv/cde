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
/* comsub.c 1.36 - Fujitsu source for CDEnext    96/09/09 15:30:40      */
/* $XConsortium: comsub.c /main/8 1996/09/19 19:36:37 cde-fuj $ */
/*
 *  (c) Copyright 1995 FUJITSU LIMITED
 *  This is source code modified by FUJITSU LIMITED under the Joint
 *  Development Agreement for the CDEnext PST.
 *  This is unpublished proprietary source code of FUJITSU LIMITED
 */


#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <math.h>
#include <locale.h>

#include <X11/Intrinsic.h>
#include "fssxeg.h"
#include "falfont.h"
#include "ufontrsrc.h"

static void writePtn(char *ptn, int width, int height, FILE *fp);
static void sig_receive(void);
char	*get_cmd_path(char *path, char *cmd) ;

extern Resource resource;

/***********************************************************************
 manage the character patterns
 **********************************************************************/

#define	GUADDPTNN	100	/* number of charcters for memory allocation  */
/* this structure corresopnds to single font file */
typedef struct {		/* area to manage character patterns 	*/
    char	flag;		/* invalid(=0)/valid(=1)		*/
    int		width;		/* pattern width 			*/
    int		height;		/* pattern height 			*/
    int		nptn;		/* number of characters in editting	*/
    int		nmaxptn;	/* maximum number of characters in editting */
    int		ptnsize;	/* size per byte of single pattern 	*/
    unsigned int ptnmemsize;	/* size of memory of whole patterns	*/
    unsigned short *ntoc;	/* character list / relative code	*/
    unsigned short *ctop;	/* relative code / bitmap area 		*/
    char	*ptn;		/* pointer to the bimap area		*/
    int	cptnnum;		/* current pattern number 		*/
} PATTERN_MNG;

static PATTERN_MNG	pm;		/* Pattern_Manage */

/* selection window for XLFD */
extern  FalFontData     fullFontData;
extern  FalFontData     copyFontData;
extern int CodePoint;

char			*fullpath=NULL;
int			begin_code=0, last_code=0;

FalCodeRegion	CodeArea[10];
char		AreaStr[160];

#define FAL_ERROR_STR resource.falerrmsg[((fal_utyerrno & 0xff) > 25) ? 0 : (fal_utyerrno & 0xff)]

FalFontID		font_id;

static char *
get_locale(void)
{
    char *loc;

    if ((loc = getenv("LANG")) != NULL) {
	return(loc);
    } else {
	return("C");
    }
}

/* contents :	convert the sequential number in the editor into
 * 		relative code in the system area
 * values   : 	-1   : terminated abnormally
 *		else : relative code
 */

int
ptnSqToNo( int num /*sequential number */)
{
    if( (pm.flag == 0) || (num >= pm.nptn) )
	return( -1 );

    return( pm.ntoc[num] );
}




/* contents :	convert the relative code in the system area into
 *		sequential number in the editor
 * values   : 	-1   : terminated abnormally
 *		else : sequential number
 */

int
ptnNoToSq( int ncode /* relative code */)
{
    int		sq;

    if( pm.flag == 0 )
	return( -1 );

    for( sq=0   ; sq < pm.nptn   ; sq++ ) {
	if( pm.ntoc[sq] == ncode )
	    return( sq );
    }

    return( -1 );
}




/*
 * contents : convert the character code into relative code
 * values   : relative code
 * caution  : There is no security against unusal character code.
 */

int
codeToNo( int code /* character code */)
{
    return( code - begin_code);
}




/*
 * contents : convert the relative code into character code
 * values   : character code
 * caution  : There is no security against unusal character code.
 */

int
noToCode( int sno /* relative code */)
{
    return( sno + begin_code);
}


/*
 * contents : judge the input code with the system defined character area
 * values   : 0 : in the system defined area
 *	     -1 : not in the system defined area
 */

int
codeCheck( int code )
{
    if (code < begin_code || code > last_code) {
	return( -1 );
    }
    return( 0 );
}

/*
 * contents : allocate memories for character patterns
 *
 * values   : 	ID : sequential number of the area
 *		-1 : terminated abnormally
 */

static int
ptnOpen(
int	n,			/* initial number of charcters	*/
int	maxc,
int	width,			/* pattern width 		*/
int	height			/* pattern height 		*/
)
{
    int		i;
    int		fpsize;

    if( (n < 0) || (width <= 0) || (height <= 0) )
	return( -1 );

    if( pm.flag != 0 ) {
	return( -1 );
    }

    pm.width   = width;
    pm.height  = height;
    pm.nptn    = 0;
    pm.nmaxptn = n + GUADDPTNN;
    pm.ptnsize = height * ((width+7)/8);
    fpsize = pm.ptnsize + 1;
    pm.ptnmemsize = (unsigned int) ( fpsize * pm.nmaxptn ) ;

    if(( pm.ntoc = (unsigned short *)calloc(maxc, sizeof(unsigned short)) ) == NULL) {
	return( -1 );
    }
    if(( pm.ctop = (unsigned short *)calloc(maxc, sizeof(unsigned short)) ) == NULL ) {
	free( pm.ntoc );
	return( -1 );
    }
    if(( pm.ptn  = (char   *)malloc(fpsize*pm.nmaxptn) ) == NULL ) {
	free( pm.ctop );
	free( pm.ntoc );
	return( -1 );
    }

    for( i=0   ; i < maxc   ; i++ ) {
	pm.ntoc[i] = 0xFFFF;
	pm.ctop[i] = 0xFFFF;
    }
    /* set null to each character pattern area */
    for( i=0   ; i < pm.nmaxptn   ; i++ ) {
	pm.ptn[fpsize * i] = 0;
    }
    pm.cptnnum = 0;
    pm.flag = 1;

    return( 0 );
}




/*
 * contents : 	set free memories for character patterns
 * values   : 	0  : terminated normally
 *		-1 : terminated abnormally
 */

int
ptnClose(void)
{
    if( pm.flag == 0 ) {
	return( -1 );
    }
    pm.flag = 0;
    free( pm.ntoc );
    free( pm.ctop );
    free( pm.ptn );

    return( 0 );
}




/*
 * contents : 	add character pattern to the character management area
 *
 * values   : 	0  : same code was exists and replaced it
 *		1  : add new pattern
 *		-1 : terminated abnormally
 */

int
ptnAdd(
int	code,			/* code to be add 	*/
char	*ptn)			/* pointer to the pattern */
{
    int		fpsize;
    int		ncode;
    unsigned short	pno;
    char	*pf;
    char	*pp;
    int		i;
    char	*re_ptn;
    unsigned int	re_ptnmemsize;
    int		cpn;

    if( pm.flag == 0 )
	return( -1 );

    fpsize = pm.ptnsize + 1;
    ncode = codeToNo( code );

    if( (pno = pm.ctop[ncode]) != 0xFFFF ) {
	pf = pm.ptn + fpsize*pno;
	pp = pf + 1;
	for( i=0   ; i < pm.ptnsize   ; i++ )
	    pp[i] = ptn[i];
	return( 0 );
    }

    if( pm.nptn >= pm.nmaxptn ) {
	re_ptnmemsize = pm.ptnmemsize + GUADDPTNN*fpsize;
	if(pm.ptn == NULL) {
	    re_ptn = malloc( re_ptnmemsize );
	} else {
	    re_ptn = realloc(pm.ptn, re_ptnmemsize);
	}
	if (re_ptn == NULL) {
	    return( -1 );
	}
	pm.ptnmemsize = re_ptnmemsize;
	pm.ptn     = re_ptn;
	for( i=pm.nmaxptn   ; i < pm.nmaxptn + GUADDPTNN   ; i++ ) {
	    pm.ptn[fpsize * i] = 0;
	}
	pm.nmaxptn += GUADDPTNN;
    }

    cpn = pm.cptnnum + 1;
    for( i=0   ; i < pm.nmaxptn   ; i++, cpn++ ) {
	if( cpn >= pm.nmaxptn ) {
	    cpn = 0;
	}
	if( pm.ptn[fpsize * cpn] == 0 ) {
	    break;
	}
    }
    pm.cptnnum     = cpn;

    pm.ctop[ncode] = (unsigned short) cpn;
    pf = pm.ptn + fpsize*cpn;
    pp = pf + 1;
    pf[0] = 1;
    for( i=0   ; i < pm.ptnsize   ; i++ )
	pp[i] = ptn[i];

    for( i=pm.nptn   ; i >=0   ; i-- ) {
	if( ncode > (int)pm.ntoc[i] ) {
	    break;
	}
	pm.ntoc[i+1] = pm.ntoc[i];
    }
    pm.ntoc[i+1] = (unsigned short) ncode;
    pm.nptn      += 1;

    return( 1 );
}


/*
 * contents : 	get a character pattern from the character management area
 * values   : 	0  : terminated normally
 *		-1 : terminated abnormally
 */

int
ptnGet( int code, char *ptn )
{
    int		ncode;
    int		fpsize;
    unsigned short pno;
    char	*pf;
    char	*pp;
    int		i;

    if( (pm.flag == 0) || (codeCheck(code) == -1) )
	return( -1 );

    ncode  = codeToNo( code );
    fpsize = pm.ptnsize +1;

    if( (pno = pm.ctop[ncode]) == 0xFFFF )
	return( -1 );

    pf = pm.ptn + pno*fpsize;
    pp = pf +1;

    for( i=0   ; i < pm.ptnsize   ; i++ )
	ptn[i] = pp[i];

    return( 0 );
}


/*
 * contents : 	judge the character pattern exist or not
 * values   : 	1  : exists
 *		0  : not exists
 *		-1 : terminated abnormally
 */

int
ptnSense( int code )
{
    if( (pm.flag == 0) || (codeCheck(code) == -1) )
	return( -1 );

    if( pm.ctop[ codeToNo(code) ] == 0xFFFF )
	return( 0 );
    else
	return( 1 );
}


/*
 * contents : 	delete the character pattern
 * values   : 	1  : deleted
 *		0  : the pattern does not exist
 *		-1 : terminated abnormally
 */

int
ptnDel( int code )
{
    int		ncode;
    int		fpsize;
    unsigned short pno;
    char	*pf;
    int		i;

    if( (pm.flag == 0) || (codeCheck(code) == -1) )
	return( -1 );

    ncode  = codeToNo( code );
    fpsize = pm.ptnsize +1;

    if( (pno = pm.ctop[ncode]) == 0xFFFF ) {
	return( 0 );
    }

    pf = pm.ptn + pno*fpsize;

    pf[0] = 0;
    pm.ctop[ncode] = 0xFFFF;

    for( i=0   ; i < pm.nptn   ; i++ ) {
	if( (int)pm.ntoc[i] == ncode )
	    break;
    }
    for(      ; i < pm.nptn   ; i++ )
	pm.ntoc[i] = pm.ntoc[i+1];

    pm.nptn --;
    return( 1 );
}


/*
 * contents : 	get the infomation of the character
 * values   : 	0  : terminated normally
 *		-1 : terminated abnormally
 */

int
ptnGetInfo(
int	*n,			/* the number of characters in editting */
int	*width,			/* pattern width 	*/
int	*height)		/* pattern height 	*/
{
    if( pm.flag == 0 )
	return( -1 );

    *n = pm.nptn;
    *width = pm.width;
    *height = pm.height;

    return( 0 );
}


/********************************************************************
 *    handle bit map file(1)                                        *
 ********************************************************************/

struct {			/* infomation of character pattern */
    int		width;		/* pattern width */
    int		height;		/* pattern height */
    int		xbytesize;
} bitInfo = { 16, 16 ,2 };


/*
 * contents : set infomation for handling bit-mapped files
 */

static void
bitSetInfo(
int	width,			/* pattern width 	*/
int	height)			/* pattern height 	*/
{
    bitInfo.width     = width;
    bitInfo.height    = height;
    bitInfo.xbytesize = (width+7)/8;

    return;
}


/*
 * contents : set the bit at specefied coordinate on
 */

void
bitSet( char *ptn, int cx, int cy )
{
    if((cx < 0) || (bitInfo.width <= cx) || (cy < 0) || (bitInfo.height <= cy))
	return;
    ptn[ bitInfo.xbytesize*cy + cx/8 ] |= (char)( 0x80 >> (cx%8) );
}


/*
 * contents : set the bit at specefied coordinate off
 */

void
bitReset( char *ptn, int cx, int cy )
{
    if((cx < 0) || (bitInfo.width <= cx) || (cy < 0) || (bitInfo.height <= cy))
	return;
    ptn[ bitInfo.xbytesize*cy + cx/8 ] &= ~(char)( 0x80 >> (cx%8) );
}


/*
 * contents : returns 1 ( bit on ) or 0 ( bit off )
 */

int
bitRead( char *ptn, int cx, int cy )
{
    if((cx < 0) || (bitInfo.width <= cx) || (cy < 0) || (bitInfo.height <= cy))
	return( 0 );
    return( ptn[ bitInfo.xbytesize*cy + cx/8 ]
		&
	   (char)( 0x80 >> (cx%8) ) ?   1   :   0
    );
}


/*
 * contents : clear the specefied bitmap file
 */

void
bitPtnClear( char *ptn )
{
    int		i;

    for( i=0   ; i < bitInfo.height * bitInfo.xbytesize    ; i++ )
	ptn[i] = 0;
}


/*
 * contents : copy the bit-mapped file
 */

void
bitPtnCopy(
char	*d_ptn,			/* pointer of the destination file 	*/
char	*s_ptn)			/* pointer of the source file 		*/
{
    int	i;

    for( i=0   ; i < bitInfo.height * bitInfo.xbytesize   ; i++ )
	d_ptn[i] = s_ptn[i];
}



/************************************************************************
 *    handle bit map file(2)                                       	*
 *    caution : You must call bitSetInfo() before use these functions.	*
 *									*
 ************************************************************************/

/*
 * contents : 	draw lines between desired two points
 * values   : 	0  : terminated normally
 *		-1 : outside of the edtting pane
 */

int
bitDrawLine(
char	*ptn,			/* pointer of the bit map file */
int	x1,
int	y1,
int	x2,
int	y2,
int	mode)			/* 0: erase 1: draw	*/
{
    float	dx, dy;
    float	x, y;
    float	delta;
    int		i;

    if( (x1 < 0) || (x2 < 0) || (y1 < 0) || (y2 < 0) ||
	(bitInfo.width  <= x1) || (bitInfo.width <= x2) ||
	(bitInfo.height <= y1) || (bitInfo.height <= y2) )
	return( -1 );

    dx = x2 - x1;
    dy = y2 - y1;
     x = x1 + 0.5;
     y = y1 + 0.5;

    delta = abs((int)dx) > abs((int)dy) ?   dx  :   dy;
    if( delta < 0 )
	delta = -delta;
    if( delta == 0)
	delta = 0.75;
    dx /= delta;
    dy /= delta;

    for(i=0   ; i <= (int)delta   ; i++ ) {
	mode == 0 ? bitReset(ptn,(int)x,(int)y)   :   bitSet(ptn,(int)x,(int)y);
	x += dx;
	y += dy;
    }
    return( 0 );
}


/*
 * contents : 	draw circle
 * values   : 	0  : terminated normally
 *		-1 : terminated abnormally
 */

int
bitDrawCircle(
char	*ptn,
int	x1,
int	y1,
int	x2,
int	y2,
int	mode)
{
    int		dx, dy;
    int		i,x;
    double	rad, half;

    if( (x1 < 0) || (y1 < 0) || (x2 < 0.0) || ( y2 < 0.0) ) {
	return( -1 );
    }

    dx = abs(x2 - x1);
    dy = abs(y2 - y1);
    rad = sqrt((double)(dx * dx + dy * dy)) + 0.5;
    half = rad * sqrt(2.0)/2;
    for (i = 0; i <= (int)half; i++) {
	x = (int)sqrt(rad * rad - (double)(i*i));
	if( mode ) {
	    bitSet  (ptn, x1 - x, y1 - i);
	    bitSet  (ptn, x1 - x, y1 + i);
	    bitSet  (ptn, x1 + x, y1 - i);
	    bitSet  (ptn, x1 + x, y1 + i);
	    bitSet  (ptn, x1 - i, y1 - x);
	    bitSet  (ptn, x1 - i, y1 + x);
	    bitSet  (ptn, x1 + i, y1 - x);
	    bitSet  (ptn, x1 + i, y1 + x);
	} else {
	    bitReset(ptn, x1 - x, y1 - i);
	    bitReset(ptn, x1 - x, y1 + i);
	    bitReset(ptn, x1 + x, y1 - i);
	    bitReset(ptn, x1 + x, y1 + i);
	    bitReset(ptn, x1 - i, y1 - x);
	    bitReset(ptn, x1 - i, y1 + x);
	    bitReset(ptn, x1 + i, y1 - x);
	    bitReset(ptn, x1 + i, y1 + x);
	}
    }
    return( 0 );
}


/*
 * contents : 	draw rectangle
 * values   : 	0  : terminated normally
 *		-1 : terminated abnormally
 */

int
bitDrawRect(
char	*ptn,
int	x,
int	y,
int	width,
int	height,
int	mode)
{
    int		i;

    width --;
    height --;

    if( (x < 0) || (y < 0) || (width < 0) || (height < 0) ||
	(bitInfo.width <= width +x) || (bitInfo.height <= height +y) )
	return( -1 );

    for( i=x   ; i <= width+x   ; i++ ) {
	if( mode == 0 ) {
	    bitReset( ptn, i, y );
	    bitReset( ptn, i, height+y );
	}
	else {
	    bitSet( ptn, i, y );
	    bitSet( ptn, i, height+y );
	}
    }
    for( i=y+1   ; i < height +y   ; i++ ) {
	if( mode == 0 ) {
	    bitReset( ptn, x, i );
	    bitReset( ptn, x+width, i );
	}
	else {
	    bitSet( ptn, x, i );
	    bitSet( ptn, x +width, i );
	}
    }

    return( 0 );
}


/*
 * contents : 	clear the inside of the specefied rectangle
 * values   : 	0  : terminated normally
 *		-1 : terminated abnormally
 */

int
bitDrawCls(
char	*ptn,
int	x,
int	y,
int	width,
int	height)
{
    int		i, j;

    width --;
    height --;

    if( (x < 0) || (y < 0) || (width < 0) || (height < 0) ||
	(bitInfo.width <= width +x) || (bitInfo.height <= height +y) )
	return( -1 );

    for( i=x   ; i <= width +x   ; i++ ) {
	for( j=y   ; j <= height +y   ; j++ )
	    bitReset( ptn, i, j );
    }

    return( 0 );
}


/*
 * contents : 	paint out the inside of the desired rectangle
 * values   : 	0  : terminated normally
 *		-1 : terminated abnormally
 */

int
bitDrawSet(
char	*ptn,
int	x,
int	y,
int	width,
int	height)
{
    int		i, j;

    width --;
    height --;

    if( (x < 0) || (y < 0) || (width < 0) || (height < 0) ||
	(bitInfo.width <= width +x) || (bitInfo.height <= height +y) )
	return( -1 );

    for( i=x   ; i <= width +x   ; i++ ) {
	for( j=y   ; j <= height +y   ; j++ )
	    bitSet( ptn, i, j );
    }
    return(0);
}


/*
 * contents : 	reverse the inside of the desired rectangle
 * values   : 	0  : terminated normally
 *		-1 : terminated abnormally
 */

int
bitDrawRev(
char	*ptn,
int	x,
int	y,
int	width,
int	height)
{
    int		i, j;

    width --;
    height --;

    if( (x < 0) || (y < 0) || (width < 0) || (height < 0) ||
	(bitInfo.width <= width +x) || (bitInfo.height <= height +y) )
	return( -1 );

    for( i=x   ; i <= width +x   ; i++ ) {
	for( j=y   ; j <= height +y   ; j++ ) {
	    if( bitRead( ptn, i, j ) == 0 )
		bitSet ( ptn, i, j );
	    else
		bitReset( ptn, i, j );
	}
    }

    return( 0 );
}


static char cut_buffer[MAXPTNBYTE];
static int cut_buffer_w=0;
static int cut_buffer_h=0;
/*
 * contents : 	copy the specified rectangle area
 * values   : 	0  : terminated normally
 *		-1 : terminated abnormally
 */

int
bitDrawCpy(
char	*ptn,
int	sx,
int	sy,
int	width,
int	height,
int	cut_flag)
{
    int		i, j;

    if ((sx < 0) || (sy < 0) || (width < 2) || (height < 2) ||
	(bitInfo.width < width +sx) || (bitInfo.height < height +sy))
	return( -1 );

    cut_buffer_w = width;
    cut_buffer_h = height;

    for (i=0; i < width; i++) {
	for (j=0; j < height; j++) {
	    if (bitRead(ptn, i + sx, j + sy) != 0)
		bitSet(cut_buffer, i, j);
	    else
		bitReset(cut_buffer, i, j);
	    if (cut_flag)
		bitReset(ptn, i + sx, j + sy);
	}
    }
    return( 0 );
}

int
bitDrawPaste(char *ptn, int dx, int dy)
{
    int		i, j;
    int		width, height;

    if((dx < 0) || (dy < 0) || (cut_buffer == 0) ||
			(cut_buffer_w == 0) || (cut_buffer_h == 0))
	return( -1 );

    width  = (cut_buffer_w <= bitInfo.width - dx)
				? cut_buffer_w : bitInfo.width - dx;
    height = (cut_buffer_h <= bitInfo.height - dy)
				? cut_buffer_h : bitInfo.height - dy;

    for (i=0; i < width; i++) {
	for (j=0; j < height; j++) {
	    if( bitRead(cut_buffer, i, j) != 0 )
		bitSet (ptn, i + dx, j + dy);
	    else
		bitReset(ptn, i + dx, j + dy);
	}
    }
    return( 0 );
}



/*
 * contents : 	rotate the inside of the area specified
 * values   :  	0  : terminated normally
 *		-1 : terminated abnormally
 */

int
bitDrawRoll(char *ptn, int x, int y, int width, int height)
{
    char *point;
    int xx, yy;
    int count;

    if( (x < 0) || (y < 0) || (width < 0) || (height < 0) ||
	(bitInfo.width < width +x) || (bitInfo.height < height +y) )
	return( -1 );

    point = (char *)malloc((width * height) * sizeof(int));

    for(count=0,yy=0; yy < height; yy++) {
	for(xx=0; xx < width; xx++ ) {
	    point[count++] = bitRead(ptn, x + xx, y + yy);
	    bitReset(ptn, x + xx, y + yy);
	}
    }

    for(count=0,yy=0; yy < height; yy++) {
	for(xx=0; xx < width; xx++ ) {
	    point[count++] == 0 ? bitReset(ptn, x + height - yy - 1, y + xx)
				: bitSet(ptn, x + height - yy - 1, y + xx);
	}
    }

    free(point);
    return( 0 );
}


/*
 * contents : 	reverse the top-bottom symmetry of the rectangle specified
 * values   : 	0  : terminated normally
 *		-1 : terminated abnormally
 */

int
bitDrawSymV( char *ptn, int x, int y, int width, int height )
{
    int		k, j;
    int		d1, d2;

    width --;
    height --;

    if( (x < 0) || (y < 0) || (width < 0) || (height < 0) ||
	(bitInfo.width <= width +x) || (bitInfo.height <= height +y) )
	return( -1 );

    for( k=0   ; k <= width   ; k++ ) {
	for( j=0   ; j <(height+1)/2   ; j++ ) {
	    d1 = bitRead( ptn, x +k,        y +j );
	    d2 = bitRead( ptn, x +k, y + height -j );

	    d1 == 0 ? bitReset( ptn, x +k, y +height -j )
		      : bitSet( ptn, x +k, y +height -j );
	    d2 == 0 ? bitReset( ptn, x +k,        y +j )
		      : bitSet( ptn, x +k,        y +j );
	}
    }
    return( 0 );
}


/*
 * contents : 	reverse the left-right symmetry of the rectangle specified
 * values   : 	0  : terminated normally
 *		-1 : terminated abnormally
 */

int
bitDrawSymH( char *ptn, int x, int y, int width, int height )
{
    int		k, j;
    int		d1, d2;

    width --;
    height --;

    if( (x < 0) || (y < 0) || (width < 0) || (height < 0) ||
	(bitInfo.width <= width +x) || (bitInfo.height <= height +y) )
	return( -1 );

    for( k=0   ; k < (width+1)/2   ; k++ ) {
	for( j=0   ; j <= height   ; j++ ) {
	    d1 = bitRead( ptn, x +k,        y +j );
	    d2 = bitRead( ptn, x +width -k, y +j );

	    d1 == 0 ? bitReset( ptn, x +width -k, y +j )
		      : bitSet( ptn, x +width -k, y +j );
	    d2 == 0 ? bitReset( ptn, x +k,        y +j )
		      : bitSet( ptn, x +k,        y +j );
	}
    }
    return( 0 );
}


static char *
char_set(char *str)
{
    int i, count;
    char *p;
    for (i=strlen(str),p=&(str[i]),count=0; i && count < 2; i--,p--) {
	if (*p == '-')
	    count ++;
    }
    if (count == 2)
	return(p + 2);
    else
	return(str);
}


/****************************************************************
 *   read and write SNF file					*
 *								*
 *								*
 ****************************************************************/

/*
 * contents : read character patterns from specified SNF file
 * values   : number of the area that was allocated to manage font file
 *		-1 : terminated abnormally
 *		-2 : file is locked
 */
int
readSNF(FalFontData **fdata, int *width, int *height, char *err)
{
    FalFontinfo		finfo;
    int			start, end;
    int			code;
    int			mask;
    int			i;
    char		*ptn;
    static FalFontDataList	*fulllist;
    char		*mes;

    FalGlyphRegion	*gr, *gr_p;
    int			num_gr;
    char		tmp[16];
    unsigned int	s, e;


    if (fulllist) {
	FalFreeFontList(fulllist);
	fulllist = NULL;
    }
    mask =  FAL_FONT_MASK_XLFDNAME | FAL_FONT_MASK_UPDATE |
		FAL_FONT_MASK_DEFINED | FAL_FONT_MASK_UNDEFINED;
    if (fullFontData.cd_set != -1) {
	mask |= FAL_FONT_MASK_CODE_SET;
    }
    if (! CodePoint) {
	mask |= FAL_FONT_MASK_GLYPH_INDEX;
    }
    if (FalGetFontList(&fullFontData, mask, &fulllist) == FAL_ERROR) {
	strcpy(err, FAL_ERROR_STR);
	return(-1);
    }
    if(fulllist == NULL) {
	strcpy(err, resource.mn_no_font);
	return(-1);
    }
    if (fulllist->num != 1) {
	FalFreeFontList(fulllist);
	fulllist = NULL;
	strcpy(err, resource.mn_plural_font);
	return(-1);
    }
    *fdata = &(fulllist->list[0]);
    font_id = FalOpenSysFont(&fullFontData, mask, &fulllist);
    mes = 0;
    if (font_id == 0 && (fal_utyerrno & 0xff) == FAL_ERR_FNT_OPN) {
	mes = (char *) malloc(strlen(resource.mn_no_perm)+strlen(fal_err_file)+10);
	if (mes == NULL) {
	    strcpy(err, resource.mn_no_mem);
	    FalFreeFontList(fulllist);
	    return( -1 );
	}
	sprintf(mes, "%sfile: %s", resource.mn_no_perm, fal_err_file);

	mask &= ~FAL_FONT_MASK_UPDATE;
        font_id = FalOpenSysFont(&fullFontData, mask, &fulllist);
        if (font_id == 0) {
	    free(mes);
	    strcpy(err, FAL_ERROR_STR);
	    FalFreeFontList(fulllist);
	    return( -1 );
	}
    } else if (font_id == 0) {
	strcpy(err, FAL_ERROR_STR);
	FalFreeFontList(fulllist);
	return( -1 );
    }
    if (fullpath != NULL)
	FalFree(fullpath);
    if (FalFontIDToFileName(font_id, &fullpath) == FAL_ERROR) {
	strcpy(err, FAL_ERROR_STR);
	if (mes)
	    free(mes);
	return( -1 );
    }

    if( FalQueryFont( font_id, &finfo ) == FAL_ERROR ) {
	strcpy(err, FAL_ERROR_STR);
	FalCloseFont( font_id );
	if (mes)
	    free(mes);
	return( -1 );
    }
    *width = finfo.width;
    *height = finfo.height;

    fullFontData.cd_set = (* fdata)->cd_set;
    if (CodePoint) {
	if (FalGetUDCCPArea(get_locale(),(* fdata)->cd_set,
		char_set(( *fdata)->xlfdname), (FalCodeRegion **) &gr, &num_gr) == FAL_ERROR) {
	    strcpy(err, FAL_ERROR_STR);
	    FalCloseFont( font_id );
	    if (mes)
	        free(mes);
	    return( -1 );
	}
    } else {
	if (FalGetUDCGIArea(get_locale(),(* fdata)->cd_set,
		char_set(( *fdata)->xlfdname), &gr, &num_gr) == FAL_ERROR) {
	    strcpy(err, FAL_ERROR_STR);
	    FalCloseFont( font_id );
	    if (mes)
	        free(mes);
	    return( -1 );
	}
    }
    if (! num_gr) {
	FalCloseFont( font_id );
	strcpy(err, resource.mn_not_udc);
	if (mes)
	    free(mes);
	return( -1 );
    }
    if (mes) {
	Error_message2(NULL, mes);
	free(mes);
	fullFontData.prm = True;
    }
    start = gr->start;
    end = gr->end;
    for (i=0, gr_p=gr; i < num_gr; i++, gr_p++) {
	CodeArea[i].start = gr_p->start;
	CodeArea[i].end = gr_p->end;
	s = (gr_p->start & 0xff00) >> 8;
	e = (gr_p->end & 0xff00) >> 8;
	if (i == 0) {
	    sprintf(tmp, "0x%x:0x%x", s, e);
	    strcpy(AreaStr, tmp);
	} else {
	    sprintf(tmp, ",0x%x:0x%x", s, e);
	    strcat(AreaStr, tmp);
	}
	if (start > gr_p->start)
	    start = gr_p->start;
	if (end < gr_p->end)
	    end = gr_p->end;
    }
    CodeArea[i].start = -1;
    begin_code = start;
    last_code = end;

    if (start > end) {
	FalCloseFont( font_id );
	sprintf(err, "%s\nStart address = %x End address = %x\n", resource.mn_illegal_area, start, end);
	return( -1 );
    }

    if( ptnOpen(1, end - start + 1, finfo.width, finfo.height) < 0 ) {
	FalCloseFont( font_id );
	strcpy(err, resource.mn_no_mem);
	return( -1 );
    }
    bitSetInfo( finfo.width, finfo.height );

    for (i=0, gr_p=gr; i < num_gr; i++, gr_p++) {
	if (start > gr_p->start)
	    gr_p->start = start;
	if (end < gr_p->end)
	    gr_p->end = end;
	start = gr_p->end;

	if (! CodePoint) {
		for (code=gr_p->start; code <= gr_p->end; code++) {
		    if (code & 0x80) {
			code &= 0xff00;
			code += 0x100;
			continue;
		    }

		    ptn  = (char *)FalReadFont( font_id, code, finfo.width, finfo.height );
		    if( ptn == (char *)FAL_ERROR ) {
			strcpy(err, FAL_ERROR_STR);
			FalFree(gr);
			FalCloseFont( font_id );

			ptnClose();
			return( -1 );
		    }
		    if( EXISTS_FLAG == 0 ) {
			if( ptnAdd( code, ptn ) != 1 ) {
			    FalFree(gr);
			    strcpy(err, resource.mn_no_read);
			    FalCloseFont( font_id );

			    ptnClose();
			    return( -1 );
			}
		    }
		}
	} else {
		for (code=gr_p->start; code <= gr_p->end; code++) {
		    if (! (code & 0x80)) {
			code &= 0xff00;
			code |= 0x80;
			continue;
		    }

		    ptn  = (char *)FalReadFont( font_id, code, finfo.width, finfo.height );
		    if( ptn == (char *)FAL_ERROR ) {
			strcpy(err, FAL_ERROR_STR);
			FalFree(gr);
			FalCloseFont( font_id );

			ptnClose();
			return( -1 );
		    }
		    if( EXISTS_FLAG == 0 ) {
			if( ptnAdd( code, ptn ) != 1 ) {
			    FalFree(gr);
			    strcpy(err, resource.mn_no_read);
			    FalCloseFont( font_id );

			    ptnClose();
			    return( -1 );
			}
		    }
		}
	}
    }
    FalFree(gr);
    *err = 0;
    return( 0 );
}

static void
bicopy(char *s1, char *s2, int size)
{
    int i;
    for(i=size; i; i--, s1++, s2++)
	*s2 = *s1;
}

int
copySNF(int start, int end, char ***ptn, int *num, char *err)
{
    FalFontID		fid;
    int			mask;
    char		** _ptn, **p;
    int			i;
    static FalFontDataList	*copylist;
    int			count;
    char		*point;
    int			size;

    mask =  FAL_FONT_MASK_XLFDNAME | FAL_FONT_MASK_DEFINED |
						FAL_FONT_MASK_UNDEFINED;
    if (copyFontData.cd_set != -1) {
	mask |= FAL_FONT_MASK_CODE_SET;
    }
    if (! CodePoint) {
	mask |= FAL_FONT_MASK_GLYPH_INDEX;
    }

    fid = FalOpenSysFont(&copyFontData, mask, &copylist);
    if (fid == 0) {
	strcpy(err, FAL_ERROR_STR);
	FalFreeFontList(copylist);
	return( -1 );
    }

    _ptn = (char **)malloc(sizeof(char *) * (end - start + 1));
    if (! CodePoint) {
        for (i = start, count = 0, p = _ptn; i <= end; i++) {
	    if (i & 0x80) {
		i &= 0xff00;
		i += 0x100;
		continue;
	    }
	    point  = (char *)FalReadFont( fid, i, pm.width, pm.height );
	    if( point == (char *)FAL_ERROR ) {
		strcpy(err, FAL_ERROR_STR);
		FalCloseFont( fid );
		return( -1 );
	    }
	    if (EXISTS_FLAG != 0)
		continue;
	    size = pm.height * ((pm.width+7)/8);
	    *p  = (char *) XtMalloc(size);
	    bicopy(point, *p, size);
	    count++; p++;
        }
    } else {
        for (i = start, count = 0, p = _ptn; i <= end; i++) {
	    if (! (i & 0x80)) {
		i &= 0xff00;
		i |= 0x80;
		continue;
	    }
	    point  = (char *)FalReadFont( fid, i, pm.width, pm.height );
	    if( point == (char *)FAL_ERROR ) {
		strcpy(err, FAL_ERROR_STR);
		FalCloseFont( fid );
		return( -1 );
	    }
	    if (EXISTS_FLAG != 0)
		continue;
	    size = pm.height * ((pm.width+7)/8);
	    *p  = (char *) XtMalloc(size);
	    bicopy(point, *p, size);
	    count++; p++;
        }
    }
    *num = count;
    *ptn = _ptn;
    FalCloseFont( fid );
    if (count == 0) {
	strcpy(err, resource.mg_non_code);
	return( -1 );
    }
    return( 0 );
}

void
freeSNF(char **addr, int count)
{
    char **p;
    for (p=addr; count; count--, p++)
	XtFree(*p);
    XtFree((char *)addr);
}

static int	sig_flg = 0;		/* flag for signal		*/

/*
 * contents : 	write character patterns to the specified SNF file
 * values   : 	0  : terminated normally
 *		-1 : terminated abnormally
 *	      1xxx : process is not completed ¡Êxxx:persents finished¡Ë
 *
 *
 */

int
writeSNF(
int		restart,	/* ON:continue OFF:initial	*/
int		*err)		/* errors : 			*/
{
    static int		pfstdi[2];
    static FILE		*fstdi;
    static int		nptn;
    static int		width, height;
    static int		n;
    static int		persents = 0;
    char		*argv[8];
    int			code;
    int			endst;
    int			persents_;

    char	ptn[MAXPTNBYTE];
    char	code_set[2];

    FalGIInf	*gi;
    int		num_gi;

    char	*command;

    /* get command path */
    if (!(command = (char *)get_cmd_path(getenv("PATH"), resource.l_ptog_cmd))){
        command = resource.l_ptog;
    }

    if (fullFontData.cd_set == FAL_FONT_CS0)
	sprintf(code_set, "0");
    else if (fullFontData.cd_set == FAL_FONT_CS1)
	sprintf(code_set, "1");
    else if (fullFontData.cd_set == FAL_FONT_CS2)
	sprintf(code_set, "2");
    else if (fullFontData.cd_set == FAL_FONT_CS3)
	sprintf(code_set, "3");

    /* signal set */
    signal( SIGPIPE, sig_receive );
    sig_flg = 0;

    if( restart == 0 ) {

	persents = 0;

	if( ptnGetInfo( &nptn, &width, &height ) != 0 ) {
	    *err = 100;
	    return( -1 );
	}

	if( pipe(pfstdi) !=0 ) {
	    *err = 101;
	    return( -1 );
	}

	switch( fork() ) {
	case -1:
	    close( pfstdi[0] );
	    close( pfstdi[1] );
	    *err = 102;
	    return( -1 );
	case 0:
	    close( 0 );			/** 0 ... stdin **/

	    if( dup(   pfstdi[0] ) == -1){
		return -1;
	    } else {
		close( pfstdi[0] );
		close( pfstdi[1] );
		argv[0] = resource.l_ptog_cmd;
		argv[1] = "-codeset";
		argv[2] = code_set;
		argv[3] = "-xlfd";
		argv[4] = fullFontData.xlfdname;
		argv[5] = "-init";
		argv[6] = "-f";
		argv[7] = NULL;
		execv (command, argv );
		exit( 103 );
	    }
	}


	close( pfstdi[0] );
	fstdi = (FILE *)fdopen( pfstdi[1], "w" );
	fprintf( fstdi, "numfonts:%d\n", nptn );
	fprintf( fstdi, "width:%d\n",    width );
	fprintf( fstdi, "height:%d\n",   height );

	bitSetInfo( width, height );
	n = 0;
    }

    while( (n < nptn) && ( sig_flg == 0) ) {
	code = noToCode( ptnSqToNo(n) );
	if( ptnGet(code, ptn ) != 0 ) {
	    fclose( fstdi );
	    close( pfstdi[1] );
	    *err = 104;
	    return( -1 );
	}

        if (CodePoint) {
	    fal_code_to_glyph(get_locale(), code, &gi, &num_gi);
	    fprintf( fstdi, "code:0x%x\n", gi[0].glyph_index );
	    FalFreeGI(gi, num_gi);
	} else {
	    fprintf( fstdi, "code:0x%x\n", code );
	}
	writePtn(ptn, width, height, fstdi);

	n++;
	if ( sig_flg != 0 )
	    break;
	if( n == nptn ) {
	    persents = 0;
	    return( 1101 );
	}
	if( (persents_ = (n*100)/nptn) != persents ) {
		persents = persents_;
		return( 1000 + persents );
	}
    }

    if(sig_flg == 0 )    fprintf( fstdi,  "enddata\n" );

    fclose( fstdi );
    close( pfstdi[1] );

    wait( &endst );


    if ( WIFEXITED(endst) && !WEXITSTATUS(endst) ) {
	*err = 0;
	return( 0 );
    }
    else {
	if ( WIFEXITED(endst) )
	    *err = WEXITSTATUS(endst);
	else{
	    if ( WIFSTOPPED(endst) )
		*err = ( WTERMSIG(endst) << 8 );
	}
	return( -1 );
    }
}


/*
 * contents : set flag when dtudcfonted received signal
 */

static void
sig_receive(void)
{
    sig_flg = 1;
    return;
}


/*
 * contents : convert bit map file into SNF file
 */

static void
writePtn(char *ptn, int width, int height, FILE *fp)
{
    int		i, j, k;
    int		nbyte;
    int		tw;
    char	p, *pbuf;
    static char	buf[ (MAXPTNSIZE+1)*MAXPTNSIZE+1 ];

    nbyte = (width + 7) / 8;

    pbuf = buf;
    for (i=0   ; i < height   ; i++) {
	for (j=0, tw=width   ; j < nbyte   ; j++ ) {
	    p = *ptn++;
	    for ( k=0   ; (k < 8) && (tw > 0)   ; k++, tw--) {
		if (p & 0x80)
		    *pbuf++ = '0';
		else
		    *pbuf++ = '-';
		p = p << 1;
	    }
	}
	*pbuf++ = '\n';
    }
    *pbuf = '\0';
    fprintf(fp, "%s", buf);
}
