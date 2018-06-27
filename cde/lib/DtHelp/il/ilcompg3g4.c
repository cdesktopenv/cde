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
/* $XConsortium: ilcompg3g4.c /main/3 1995/10/23 15:42:12 rswiston $ */
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
#include "ilpipelem.h"
#include "ilcompress.h"
#include "ilcodec.h"
#include "ilerrors.h"

/* ========================================================================
     Compression private data structure definition
     Used in Both G3 & G4 Compression ...
   ======================================================================== */


typedef struct {
   int                  width;          /* width of the image              */
   unsigned long        compData;       /* flags for G4 Compression Options*/
   ilBool               white;          /* value of the white pixel 0 or 1 */
   unsigned int         bitCount;       /* # of bits waiting to be output  */
/* compatibility problem with long and unsigned long data fields */
   CARD32               bits;           /* bits waiting to be output       */
   ilPtr                pDstByte;       /* ptr to spot for next byte in output buffer */
   ilPtr                pDstBufferEnd;  /* ptr _past_ last available spot in buffer */
   ilImageInfo          *pDstImage;     /* ptr to dst image structure */
   ilBool               Is_Lsb_First;   /* true if  LSB is desired    */
   ilPtr                gpRefLine;      /* ptr.to refrence line for 2d coding */
   long                 nDstLineBytes;  /* no.of bytes in the Image        */
} ilCompressG3G4PrivRec, *ilCompressG3G4PrivPtr;
         

/* define to find out the value of the bit (1 or 0) in the image line      */
#define	PIXEL(buf,ix)	((((buf)[(ix)>>3]) >> (7-((ix)&7))) & 1)             

     /* max bytes written after check for room in compressed buffer */
#define                 G3_G4_MAX_BUFFER_WRITE 10


/* ========================================================================
   The following are the encode tables for CCITT.  The tables are broken
   down by terminating codes, make-up codes, and additional make-up codes.
   There are separate tables for black and white terminating codes and
   make-up codes.  The first two columns are for documentation only.  The
   first column is the run length represented by the entry.  The second
   column is the bit code for the run length, left justified.  The third
   column is the hex number for the bit code.  The fourth column is the
   number of bits in the code. There is also a table with one entry for
   the end-of-line (eol) code.
   ======================================================================== */

static unsigned short _ilEncodeWhiteTerms[64][2] = { /* white terminating codes */

    	/*    0:    0011 0101 0000 0000 */    0x3500,    8,
	    /*    1:    0001 1100 0000 0000 */    0x1C00,    6,
	    /*    2:    0111 0000 0000 0000 */    0x7000,    4,
	    /*    3:    1000 0000 0000 0000 */    0x8000,    4,
	    /*    4:    1011 0000 0000 0000 */    0xB000,    4,
	    /*    5:    1100 0000 0000 0000 */    0xC000,    4,
	    /*    6:    1110 0000 0000 0000 */    0xE000,    4,
	    /*    7:    1111 0000 0000 0000 */    0xF000,    4,
	    /*    8:    1001 1000 0000 0000 */    0x9800,    5,
	    /*    9:    1010 0000 0000 0000 */    0xA000,    5,
	    /*   10:    0011 1000 0000 0000 */    0x3800,    5,
	    /*   11:    0100 0000 0000 0000 */    0x4000,    5,
	    /*   12:    0010 0000 0000 0000 */    0x2000,    6,
	    /*   13:    0000 1100 0000 0000 */    0x0C00,    6,
	    /*   14:    1101 0000 0000 0000 */    0xD000,    6,
	    /*   15:    1101 0100 0000 0000 */    0xD400,    6,
	    /*   16:    1010 1000 0000 0000 */    0xA800,    6,
	    /*   17:    1010 1100 0000 0000 */    0xAC00,    6,
	    /*   18:    0100 1110 0000 0000 */    0x4E00,    7,
	    /*   19:    0001 1000 0000 0000 */    0x1800,    7,
	    /*   20:    0001 0000 0000 0000 */    0x1000,    7,
	    /*   21:    0010 1110 0000 0000 */    0x2E00,    7,
	    /*   22:    0000 0110 0000 0000 */    0x0600,    7,
	    /*   23:    0000 1000 0000 0000 */    0x0800,    7,
	    /*   24:    0101 0000 0000 0000 */    0x5000,    7,
	    /*   25:    0101 0110 0000 0000 */    0x5600,    7,
	    /*   26:    0010 0110 0000 0000 */    0x2600,    7,
	    /*   27:    0100 1000 0000 0000 */    0x4800,    7,
	    /*   28:    0011 0000 0000 0000 */    0x3000,    7,
	    /*   29:    0000 0010 0000 0000 */    0x0200,    8,
	    /*   30:    0000 0011 0000 0000 */    0x0300,    8,
	    /*   31:    0001 1010 0000 0000 */    0x1A00,    8,
	    /*   32:    0001 1011 0000 0000 */    0x1B00,    8,
	    /*   33:    0001 0010 0000 0000 */    0x1200,    8,
	    /*   34:    0001 0011 0000 0000 */    0x1300,    8,
	    /*   35:    0001 0100 0000 0000 */    0x1400,    8,
	    /*   36:    0001 0101 0000 0000 */    0x1500,    8,
	    /*   37:    0001 0110 0000 0000 */    0x1600,    8,
	    /*   38:    0001 0111 0000 0000 */    0x1700,    8,
	    /*   39:    0010 1000 0000 0000 */    0x2800,    8,
	    /*   40:    0010 1001 0000 0000 */    0x2900,    8,
	    /*   41:    0010 1010 0000 0000 */    0x2A00,    8,
	    /*   42:    0010 1011 0000 0000 */    0x2B00,    8,
	    /*   43:    0010 1100 0000 0000 */    0x2C00,    8,
	    /*   44:    0010 1101 0000 0000 */    0x2D00,    8,
	    /*   45:    0000 0100 0000 0000 */    0x0400,    8,
	    /*   46:    0000 0101 0000 0000 */    0x0500,    8,
	    /*   47:    0000 1010 0000 0000 */    0x0A00,    8,
	    /*   48:    0000 1011 0000 0000 */    0x0B00,    8,
	    /*   49:    0101 0010 0000 0000 */    0x5200,    8,
	    /*   50:    0101 0011 0000 0000 */    0x5300,    8,
	    /*   51:    0101 0100 0000 0000 */    0x5400,    8,
	    /*   52:    0101 0101 0000 0000 */    0x5500,    8,
	    /*   53:    0010 0100 0000 0000 */    0x2400,    8,
	    /*   54:    0010 0101 0000 0000 */    0x2500,    8,
	    /*   55:    0101 1000 0000 0000 */    0x5800,    8,
	    /*   56:    0101 1001 0000 0000 */    0x5900,    8,
	    /*   57:    0101 1010 0000 0000 */    0x5A00,    8,
	    /*   58:    0101 1011 0000 0000 */    0x5B00,    8,
	    /*   59:    0100 1010 0000 0000 */    0x4A00,    8,
	    /*   60:    0100 1011 0000 0000 */    0x4B00,    8,
	    /*   61:    0011 0010 0000 0000 */    0x3200,    8,
	    /*   62:    0011 0011 0000 0000 */    0x3300,    8,
	    /*   63:    0011 0100 0000 0000 */    0x3400,    8
};


static unsigned short _ilEncodeBlackTerms[64][2] = { /* black terminating codes */

	    /*    0:    0000 1101 1100 0000 */    0x0DC0,    10,
	    /*    1:    0100 0000 0000 0000 */    0x4000,    3,
	    /*    2:    1100 0000 0000 0000 */    0xC000,    2,
	    /*    3:    1000 0000 0000 0000 */    0x8000,    2,
	    /*    4:    0110 0000 0000 0000 */    0x6000,    3,
	    /*    5:    0011 0000 0000 0000 */    0x3000,    4,
	    /*    6:    0010 0000 0000 0000 */    0x2000,    4,
	    /*    7:    0001 1000 0000 0000 */    0x1800,    5,
	    /*    8:    0001 0100 0000 0000 */    0x1400,    6,
	    /*    9:    0001 0000 0000 0000 */    0x1000,    6,
	    /*   10:    0000 1000 0000 0000 */    0x0800,    7,
	    /*   11:    0000 1010 0000 0000 */    0x0A00,    7,
	    /*   12:    0000 1110 0000 0000 */    0x0E00,    7,
	    /*   13:    0000 0100 0000 0000 */    0x0400,    8,
	    /*   14:    0000 0111 0000 0000 */    0x0700,    8,
	    /*   15:    0000 1100 0000 0000 */    0x0C00,    9,
	    /*   16:    0000 0101 1100 0000 */    0x05C0,    10,
	    /*   17:    0000 0110 0000 0000 */    0x0600,    10,
	    /*   18:    0000 0010 0000 0000 */    0x0200,    10,
	    /*   19:    0000 1100 1110 0000 */    0x0CE0,    11,
	    /*   20:    0000 1101 0000 0000 */    0x0D00,    11,
	    /*   21:    0000 1101 1000 0000 */    0x0D80,    11,
	    /*   22:    0000 0110 1110 0000 */    0x06E0,    11,
	    /*   23:    0000 0101 0000 0000 */    0x0500,    11,
	    /*   24:    0000 0010 1110 0000 */    0x02E0,    11,
	    /*   25:    0000 0011 0000 0000 */    0x0300,    11,
	    /*   26:    0000 1100 1010 0000 */    0x0CA0,    12,
	    /*   27:    0000 1100 1011 0000 */    0x0CB0,    12,
	    /*   28:    0000 1100 1100 0000 */    0x0CC0,    12,
	    /*   29:    0000 1100 1101 0000 */    0x0CD0,    12,
	    /*   30:    0000 0110 1000 0000 */    0x0680,    12,
	    /*   31:    0000 0110 1001 0000 */    0x0690,    12,
	    /*   32:    0000 0110 1010 0000 */    0x06A0,    12,
	    /*   33:    0000 0110 1011 0000 */    0x06B0,    12,
	    /*   34:    0000 1101 0010 0000 */    0x0D20,    12,
	    /*   35:    0000 1101 0011 0000 */    0x0D30,    12,
	    /*   36:    0000 1101 0100 0000 */    0x0D40,    12,
	    /*   37:    0000 1101 0101 0000 */    0x0D50,    12,
	    /*   38:    0000 1101 0110 0000 */    0x0D60,    12,
	    /*   39:    0000 1101 0111 0000 */    0x0D70,    12,
	    /*   40:    0000 0110 1100 0000 */    0x06C0,    12,
	    /*   41:    0000 0110 1101 0000 */    0x06D0,    12,
	    /*   42:    0000 1101 1010 0000 */    0x0DA0,    12,
	    /*   43:    0000 1101 1011 0000 */    0x0DB0,    12,
	    /*   44:    0000 0101 0100 0000 */    0x0540,    12,
	    /*   45:    0000 0101 0101 0000 */    0x0550,    12,
	    /*   46:    0000 0101 0110 0000 */    0x0560,    12,
	    /*   47:    0000 0101 0111 0000 */    0x0570,    12,
	    /*   48:    0000 0110 0100 0000 */    0x0640,    12,
	    /*   49:    0000 0110 0101 0000 */    0x0650,    12,
	    /*   50:    0000 0101 0010 0000 */    0x0520,    12,
	    /*   51:    0000 0101 0011 0000 */    0x0530,    12,
	    /*   52:    0000 0010 0100 0000 */    0x0240,    12,
	    /*   53:    0000 0011 0111 0000 */    0x0370,    12,
	    /*   54:    0000 0011 1000 0000 */    0x0380,    12,
	    /*   55:    0000 0010 0111 0000 */    0x0270,    12,
	    /*   56:    0000 0010 1000 0000 */    0x0280,    12,
	    /*   57:    0000 0101 1000 0000 */    0x0580,    12,
	    /*   58:    0000 0101 1001 0000 */    0x0590,    12,
	    /*   59:    0000 0010 1011 0000 */    0x02B0,    12,
	    /*   60:    0000 0010 1100 0000 */    0x02C0,    12,
	    /*   61:    0000 0101 1010 0000 */    0x05A0,    12,
	    /*   62:    0000 0110 0110 0000 */    0x0660,    12,
	    /*   63:    0000 0110 0111 0000 */    0x0670,    12
};


static unsigned short _ilEncodeWhiteMakeupCodes[27][2] = { /* white make-up codes */

     	/*   64:    1101 1000 0000 0000 */    0xD800,    5,
	    /*  128:    1001 0000 0000 0000 */    0x9000,    5,
	    /*  192:    0101 1100 0000 0000 */    0x5C00,    6,
	    /*  256:    0110 1110 0000 0000 */    0x6E00,    7,
	    /*  320:    0011 0110 0000 0000 */    0x3600,    8,
	    /*  384:    0011 0111 0000 0000 */    0x3700,    8,
	    /*  448:    0110 0100 0000 0000 */    0x6400,    8,
	    /*  512:    0110 0101 0000 0000 */    0x6500,    8,
	    /*  576:    0110 1000 0000 0000 */    0x6800,    8,
	    /*  640:    0110 0111 0000 0000 */    0x6700,    8,
	    /*  704:    0110 0110 0000 0000 */    0x6600,    9,
	    /*  768:    0110 0110 1000 0000 */    0x6680,    9,
	    /*  832:    0110 1001 0000 0000 */    0x6900,    9,
	    /*  896:    0110 1001 1000 0000 */    0x6980,    9,
	    /*  960:    0110 1010 0000 0000 */    0x6A00,    9,
	    /* 1024:    0110 1010 1000 0000 */    0x6A80,    9,
	    /* 1088:    0110 1011 0000 0000 */    0x6B00,    9,
	    /* 1152:    0110 1011 1000 0000 */    0x6B80,    9,
	    /* 1216:    0110 1100 0000 0000 */    0x6C00,    9,
	    /* 1280:    0110 1100 1000 0000 */    0x6C80,    9,
	    /* 1344:    0110 1101 0000 0000 */    0x6D00,    9,
	    /* 1408:    0110 1101 1000 0000 */    0x6D80,    9,
	    /* 1472:    0100 1100 0000 0000 */    0x4C00,    9,
	    /* 1536:    0100 1100 1000 0000 */    0x4C80,    9,
	    /* 1600:    0100 1101 0000 0000 */    0x4D00,    9,
	    /* 1664:    0110 0000 0000 0000 */    0x6000,    6,
	    /* 1728:    0100 1101 1000 0000 */    0x4D80,    9
};


static unsigned short _ilEncodeBlackMakeupCodes[27][2] = { /* black make-up codes */

    	/*   64:    0000 0011 1100 0000 */    0x03C0,    10,
	    /*  128:    0000 1100 1000 0000 */    0x0C80,    12,
	    /*  192:    0000 1100 1001 0000 */    0x0C90,    12,
	    /*  256:    0000 0101 1011 0000 */    0x05B0,    12,
	    /*  320:    0000 0011 0011 0000 */    0x0330,    12,
	    /*  384:    0000 0011 0100 0000 */    0x0340,    12,
	    /*  448:    0000 0011 0101 0000 */    0x0350,    12,
	    /*  512:    0000 0011 0110 0000 */    0x0360,    13,
	    /*  576:    0000 0011 0110 1000 */    0x0368,    13,
	    /*  640:    0000 0010 0101 0000 */    0x0250,    13,
	    /*  704:    0000 0010 0101 1000 */    0x0258,    13,
	    /*  768:    0000 0010 0110 0000 */    0x0260,    13,
	    /*  832:    0000 0010 0110 1000 */    0x0268,    13,
	    /*  896:    0000 0011 1001 0000 */    0x0390,    13,
	    /*  960:    0000 0011 1001 1000 */    0x0398,    13,
	    /* 1024:    0000 0011 1010 0000 */    0x03A0,    13,
	    /* 1088:    0000 0011 1010 1000 */    0x03A8,    13,
	    /* 1152:    0000 0011 1011 0000 */    0x03B0,    13,
	    /* 1216:    0000 0011 1011 1000 */    0x03B8,    13,
	    /* 1280:    0000 0010 1001 0000 */    0x0290,    13,
	    /* 1344:    0000 0010 1001 1000 */    0x0298,    13,
	    /* 1408:    0000 0010 1010 0000 */    0x02A0,    13,
	    /* 1472:    0000 0010 1010 1000 */    0x02A8,    13,
	    /* 1536:    0000 0010 1101 0000 */    0x02D0,    13,
	    /* 1600:    0000 0010 1101 1000 */    0x02D8,    13,
	    /* 1664:    0000 0011 0010 0000 */    0x0320,    13,
	    /* 1728:    0000 0011 0010 1000 */    0x0328,    13
};


static unsigned short _ilEncodeAdditionalMakeupCodes[13][2] = { /* additional make-up codes */


    	/* 1792:    0000 0001 0000 0000 */    0x0100,    11,
	    /* 1856:    0000 0001 1000 0000 */    0x0180,    11,
	    /* 1920:    0000 0001 1010 0000 */    0x01A0,    11,
	    /* 1984:    0000 0001 0010 0000 */    0x0120,    12,
	    /* 2048:    0000 0001 0011 0000 */    0x0130,    12,
	    /* 2112:    0000 0001 0100 0000 */    0x0140,    12,
	    /* 2176:    0000 0001 0101 0000 */    0x0150,    12,
	    /* 2240:    0000 0001 0110 0000 */    0x0160,    12,
	    /* 2304:    0000 0001 0111 0000 */    0x0170,    12,
	    /* 2368:    0000 0001 1100 0000 */    0x01C0,    12,
	    /* 2432:    0000 0001 1101 0000 */    0x01D0,    12,
	    /* 2496:    0000 0001 1110 0000 */    0x01E0,    12,
	    /* 2560:    0000 0001 1111 0000 */    0x01F0,    12
};


static unsigned short _ilEncodeEndOfLineCode[1][2] = { /* End-of-line code */

	/*  eol:    0000 0000 0001 0000 */    0x0010,    12
};

/*  Table containing (array) the number of consecutive zeros 
    from the start (msb first), in chars  for 0x00 to 0xff 
    for e.g  number of consecutive zeros in 0x00 = 8 
                                         in 0x0f = 4               */

static unsigned char zeroruns[256] = {
	8, 7, 6, 6, 5, 5, 5, 5, 4, 4, 4, 4, 4, 4, 4, 4,	/* 0x00 - 0x0f */
	3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,	/* 0x10 - 0x1f */
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,	/* 0x20 - 0x2f */
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,	/* 0x30 - 0x3f */
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,	/* 0x40 - 0x4f */
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,	/* 0x50 - 0x5f */
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,	/* 0x60 - 0x6f */
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,	/* 0x70 - 0x7f */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/* 0x80 - 0x8f */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/* 0x90 - 0x9f */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/* 0xa0 - 0xaf */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/* 0xb0 - 0xbf */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/* 0xc0 - 0xcf */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/* 0xd0 - 0xdf */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/* 0xe0 - 0xef */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/* 0xf0 - 0xff */
};


/*  Table containing (array) the number of consecutive ones 
    from the start (msb first), in chars  for 0x00 to 0xff         
    for e.g  number of consecutive ones  in 0x00 = 0 
                                         in 0xff = 8               */


static unsigned char oneruns[256] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/* 0x00 - 0x0f */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/* 0x10 - 0x1f */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/* 0x20 - 0x2f */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/* 0x30 - 0x3f */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/* 0x40 - 0x4f */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/* 0x50 - 0x5f */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/* 0x60 - 0x6f */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/* 0x70 - 0x7f */
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,	/* 0x80 - 0x8f */
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,	/* 0x90 - 0x9f */
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,	/* 0xa0 - 0xaf */
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,	/* 0xb0 - 0xbf */
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,	/* 0xc0 - 0xcf */
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,	/* 0xd0 - 0xdf */
	3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,	/* 0xe0 - 0xef */
	4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 7, 8,	/* 0xf0 - 0xff */
};



/* ========================================================================

        --------------------  _ilGetAbsDiff(). --------------------
  Input : the pointer to the pointer to the strings (image Line)
          startPixel from which the diff. is to be found
          endPixel i.e upto which the diff can be checked.
          color of the current pixel (startPixel)
          nTimes - no. of times the counting is to be repeated i.e for 
                   how many changing elements, the operation is to be
                   performed.

  Does : 
    Calculates the count of pixels of the same color  and returns the absolute
    position of the next    changing element and Returns the Absolute Position 
    of the element

      **  used for G4,G3-2d compression and de-comression    
   ======================================================================== */

static int
_ilGetAbsDiff(	unsigned char *sByte,
int startPixel,
int endPixel,
int color,
int nTimes)
{
	int rel_diff, abs_diff;

	unsigned char *bp;
	int ini_diff;
	int n, fin_diff;
	unsigned char *table ;

	bp = sByte;

	bp += startPixel>> 3;			/* adjust byte offset */

	do {

		if ((startPixel == -1) ) {
			fin_diff = 1;
			bp       = sByte;
			goto done;
		}
		else {

			table = (color ? oneruns : zeroruns );
			ini_diff = endPixel- startPixel;

			/*  Find difference in the partial byte on the current Byte */

			if (ini_diff > 0 && (n = (startPixel & 7))) {
				fin_diff = table[(*bp << n) & 0xff];

				if (fin_diff > 8-n)		/* Consecutive bits extend beyond the current byte */
					fin_diff = 8-n;


				/* return the initial differece, if the current byte happens
                   to be the last byte of the Imageline and cosecutive bits
                   extend beyound that.                                      */

				if (fin_diff > ini_diff)
					fin_diff = ini_diff;

				if (n + fin_diff < 8 )	/* Consecutive bits does not  go upto the edge, so return the diff */
					goto done;
				ini_diff -= fin_diff;
				bp++;
			} else
				fin_diff = 0;

			/*  Count in the bytes, till opp. color is found
                i.e while the diff >= 8                          */

			while (ini_diff >= 8) {
				n = table[*bp];
				fin_diff += n;
				ini_diff -= n;
				if (n < 8)		/* end of run */
					goto done;

				bp++;
			}

			/*  Find difference in the partial byte on RHS */

			if (ini_diff > 0) {
				n = table[*bp];
				fin_diff += (n > ini_diff ? ini_diff : n);
			}
		}
done:
		startPixel +=  fin_diff;
		color       = !color;

	} while (--nTimes > 0 );

	return(startPixel);

}
/* --------------- End of  _ilGetAbsDiff()  ---------------- */

/* ========================================================================
     --------------------- _ilCompressG3G4ReallocBuffer ------------------- 
   Reallocate the output (compressed) buffer and reset pPriv->pDst(BufferEnd).
   ======================================================================== */

static ilBool _ilCompressG3G4ReallocBuffer (
ilCompressG3G4PrivPtr  pPriv
)
{
	unsigned long  offset;

	offset = pPriv->pDstByte - pPriv->pDstImage->plane[0].pPixels;
	if (!_ilReallocCompressedBuffer (pPriv->pDstImage, 0, offset + G3_G4_MAX_BUFFER_WRITE))
		return FALSE;
	pPriv->pDstByte = pPriv->pDstImage->plane[0].pPixels + offset;
	pPriv->pDstBufferEnd = pPriv->pDstImage->plane[0].pPixels +
	    (pPriv->pDstImage->plane[0].bufferSize - G3_G4_MAX_BUFFER_WRITE);
	return TRUE;
}


/* ========================================================================

        --------------------  _ilPutData() --------------------
   Input : the pointer to the private record ilCompressG3G4PrivPtr;
           i.e the destination Image structre, the coded bits, the counts;
   Does  : adjusts the bit (integer to which the codes are put) and puts
           them interms of bytes into the Destination Image.
           Checks for the number of bits coded, and adds the bytes
           accordingly. If LSB first is desired, reverses the bits and puts.
           Checks for sufficient room in the destination buffer.
   Returns : IL_OK, if does not encouter any error in allocating space.

      **  used in  G4 compression also.
   ======================================================================== */
static ilError
_ilPutData ( 
 ilCompressG3G4PrivPtr pPriv)

{

   /* use local variables here, copy the values from pPriv to local & copy them
      back at the end of the function; this is expected to be faster                */

   unsigned int         bitCount;       /* # of bits waiting to be output  */
/* compatibility problem with long and unsigned long data fields */
   CARD32               bits;           /* bits waiting to be output       */
   ilPtr                pDstByte;       /* ptr to spot for next byte in output buffer */
   ilBool               Is_Lsb_First;

	/*  Output 3 bytes; check for room in buffer; realloc if not room.
            */
	if (pPriv->pDstByte >= pPriv->pDstBufferEnd)
		if (!_ilCompressG3G4ReallocBuffer (pPriv))
			return IL_ERROR_MALLOC;


    pDstByte = pPriv->pDstByte ;
    bits     = pPriv->bits;
    bitCount = pPriv->bitCount;               
    Is_Lsb_First = pPriv->Is_Lsb_First;


	if ( bitCount > 24 ){

		/* If LSbit first is Required, reverse bit order for the Output */
		if ( Is_Lsb_First ) {
			*pDstByte = ilBitReverseTable [(unsigned char)(( bits >> 24) & 0xff)];
			 pDstByte++;
			*pDstByte = ilBitReverseTable [(unsigned char)(( bits >> 16) & 0xff)];
			 pDstByte++;
			*pDstByte = ilBitReverseTable [(unsigned char)(( bits >> 8) & 0xff)];
			 pDstByte++;

		}

		else {
			*pDstByte = (unsigned char)(( bits >> 24) & 0xff);
			 pDstByte++;
			*pDstByte = (unsigned char)(( bits >> 16) & 0xff);
			 pDstByte++;
			*pDstByte = (unsigned char)(( bits >> 8) & 0xff);
			 pDstByte++;
		}

		 bits    <<= 24;
		 bitCount -= 24;
	}                                   /* > 24 */

	else if (  bitCount > 16 ) {

		/* If LSbit first is Required, reverse bit order for the Output */
		if ( Is_Lsb_First ) {
			*pDstByte = ilBitReverseTable [(unsigned char)(( bits >> 24) & 0xff)];
			 pDstByte++;
			*pDstByte = ilBitReverseTable [(unsigned char)(( bits >> 16) & 0xff)];
			 pDstByte++;

		}

		else {
			*pDstByte = (unsigned char)(( bits >> 24) & 0xff);
			 pDstByte++;
			*pDstByte = (unsigned char)(( bits >> 16) & 0xff);
			 pDstByte++;
		}

		 bits    <<= 16;
		 bitCount -= 16;
	}                                   /* > 16 */

	else if  (  bitCount > 8 ) {

		/* If LSbit first is Required, reverse bit order for the Output */
		if ( Is_Lsb_First ) {
			*pDstByte = ilBitReverseTable [(unsigned char)(( bits >> 24) & 0xff)];
			 pDstByte++;
		}

		else {
			*pDstByte = (unsigned char)(( bits >> 24) & 0xff);
			 pDstByte++;
		}

		 bits    <<= 8;
		 bitCount -= 8;
	}                                   /* > 8 */

    pPriv->pDstByte  =   pDstByte ;
    pPriv->bits      =   bits     ;
    pPriv->bitCount  =   bitCount ;

	return(IL_OK);
}

/* ========================================================================

        --------------------  _ilGetNdPutNewBits() --------------------
   Input : the pointer to the private record ilCompressG3G4PrivPtr;
           i.e the destination Image structre, the coded bits, the counts;
           Count: the number of pixels in the source image for which
                  appropriate code is to be found from the tables;
           pixelValue : Color of the pixel : white 0 , black 1 ;

   Does  : depending upon the pixelValue, retrieves the bit sequence
           and the number of bits (from the encodeWhite or Black tables)
           and adds them to the running Bit stream(pPriv->bits);
           The number of maximum bits for a run length could turn out
           to be 37 i.e may exceed 32 the max. no of bits that could
           be put into a unsigned long int; So care is taken to send
           the bits to the destination, if the count exceeds 16, before
           retrieveing the makeup codes.

   Returns : IL_OK, if does not encouter any error .

      ** used in G4 compression also.
   ======================================================================== */
static ilError
_ilGetNdPutNewBits( 
 ilCompressG3G4PrivPtr pPriv,
int count,
ilBool pixelValue
)

{

	ilBool termFlag ;
	int    index    ;
	int    tnumBits ;
/* compatibility problem with long and unsigned long data fields */
        CARD32 tnewBits;
	ilError error;

	/* find the new bit pattern and the number of bits in the tables */
	termFlag = FALSE;

	while (termFlag == FALSE) {

		if (count > 2560) {                                        /* write out longest available */
			tnewBits = _ilEncodeAdditionalMakeupCodes[12][0];
			tnumBits = _ilEncodeAdditionalMakeupCodes[12][1];
			count  -= 2560;
		}

		else {

			if (count > 1791) {                                    /* use additional make-up codes */
				index   = (count - 1792) / 64;
				tnewBits = _ilEncodeAdditionalMakeupCodes[index][0];
				tnumBits = _ilEncodeAdditionalMakeupCodes[index][1];
				count   = (count - 1792 - (index * 64));
			}

			else {

				if (count > 63) {
					index = (count - 64) / 64;
					if (!pixelValue) {                         /* use white make-up codes */
						tnewBits = _ilEncodeWhiteMakeupCodes[index][0];
						tnumBits = _ilEncodeWhiteMakeupCodes[index][1];
					}

					else {                                         /* use black make-up codes */
						tnewBits = _ilEncodeBlackMakeupCodes[index][0];
						tnumBits = _ilEncodeBlackMakeupCodes[index][1];
					}
					count = count - 64 - index * 64;
				}

				else {                                             /* count <= 63 */

					if (!pixelValue ) {                         /* use white terminating codes */
						tnewBits = _ilEncodeWhiteTerms[count][0];
						tnumBits = _ilEncodeWhiteTerms[count][1];
					}

					else {                                         /* use black terminating codes */
						tnewBits = _ilEncodeBlackTerms[count][0];
						tnumBits = _ilEncodeBlackTerms[count][1];
					}
					termFlag = TRUE;
					count   = 0;
				}
			}
		}

		tnewBits  = tnewBits << 16;                    /* shift the new bits to the left end      */
		tnewBits  = tnewBits >> pPriv->bitCount ;      /* right shift by the current bitCount     */
		pPriv->bits     = pPriv->bits     | tnewBits;  /* add the new bits to the existing bits   */
		pPriv->bitCount = pPriv->bitCount + tnumBits;  /* reduce the bitCount by no. of bits added*/

		if (pPriv->bitCount > 16)                      /* put the data, if count is more          */
			if (error =  _ilPutData(pPriv))
				return error;
	}

	return(IL_OK);

}


/*  ========================= G3 Compression ========================================= */


/* ========================================================================
       -------------------- _ilCompressG3Init -------------------
   Routine defined in ilCompressG3 for initializing CCITT Group3  
   compression when the pipe gets executed.
   ======================================================================== */

static ilError _ilCompressG3Init(
ilCompressG3G4PrivPtr   pPriv,
ilImageInfo            *pSrcImage,
ilImageInfo            *pDstImage
)
{
	/* Allocate space for Reference line, needed for 2 dimensional coding */

	pPriv->gpRefLine = (ilPtr)IL_MALLOC(pPriv->nDstLineBytes );
	if (!pPriv->gpRefLine)
		return IL_ERROR_MALLOC;
	return IL_OK;

}
/* End _ilCompressG3Init() */

/* ========================================================================
       -------------------- _ilCompressG3Cleanup -------------------
   Routine defined in ilCompressG3 for Cleaning up CCITT Group3  
   compression when the pipe gets executed.
   ======================================================================== */

static ilError _ilCompressG3Cleanup(
ilCompressG3G4PrivPtr   pPriv,
ilImageInfo            *pSrcImage,
ilImageInfo            *pDstImage
)
{

	if (pPriv->gpRefLine)
		IL_FREE( (ilPtr)pPriv->gpRefLine);
	return IL_OK;

}
/* End _ilCompressG3Cleanup() */

/* ========================================================================
        -------------------- _ilCompressG3Line()  --------------------
   Input : the pointer to the private record ilCompressG3G4PrivPtr;
           Pointer to the Source Image Line 
   Does  : calculates the successive no. of white pixels & black pixels
           and put the Codes appropriately into the Destination image.

   Returns : IL_OK, if does not encouter any error in compression.
   ======================================================================== */
static ilError
_ilCompressG3Line( 
 ilCompressG3G4PrivPtr pPriv,
 ilPtr                 pSrc
)
{
    int        a0, b2;             /* changing elements pertaining to the pSrcLine   */
    ilBool     pixelValue;         /* value of the pixel 0 or 1                      */
    ilError             error;
    long       width;              /* width of the image */
    ilBool     white;              /* value of the white pixel */
#ifndef NOINLINEG3
	int ini_diff;
	int n, fin_diff;
	unsigned char *table ;
    ilBool termFlag ;
    int    index;
    int    tnumBits ;
    int    count;
    CARD32          tnewBits;
#endif

    width = pPriv->width;
    white = pPriv->white;
	a0 = 0;

	/*   G3 1 D Compression ...
         For eachline, count the number of  white pixels, if it is Zero, then put the
         code for zero white Run... The line should always start with a White Run.
         Then count the number or successive black Pixels and put the appropriate
         code..
         Continue this series of white & black runs until the end of line is Reached..
    */

        /*  Alternate counting white and black pixels, starting with white */
    pixelValue = 0;
	for (;;) {

            /*  Count the white Pixels .... (formerly:
                    b2 = _ilGetRelDiff(&pSrcLine, a0, width,white); 
            */
	ini_diff = width - a0;
	table = (white ? oneruns : zeroruns );

	/*  Find difference in the partial byte on the current Byte  */

	if (ini_diff > 0 && (n = (a0 & 7))) {
		b2 = table[(*pSrc << n) & 0xff];
		if (b2 > 8-n)		/* Consecutive bits extend beyond the current byte */
			b2 = 8-n;

	/* return the initial differece, if the current byte happens
     to be the last byte of the Imageline and cosecutive bits
     extend beyound that.                                      */

		if (b2 > ini_diff)
			b2 = ini_diff;

		if (n + b2 < 8 )	/* Consecutive bits does not  go upto the edge, so return the diff */
			goto done;
		ini_diff -= b2;
		pSrc++;
	} else
		b2 = 0;

	/*  Count in the bytes, till opp. color is found i.e while the diff >= 8 */

	while (ini_diff >= 8) {
		n = table[*pSrc];
		b2 += n;
		ini_diff -= n;
		if (n < 8)		/* end of run */
			goto done;
		pSrc++;
	}

	/*  Find difference in the partial byte on RHS */

	if (ini_diff > 0) {
		n = table[*pSrc];
		b2 += (n > ini_diff ? ini_diff : n);
	}
done:

        /*  Inline code for: 
                if (error = _ilGetNdPutNewBits( pPriv,b2,pixelValue))
                    return error;
        */

	/* find the new bit pattern and the number of bits in the tables */
	termFlag = FALSE;
    count = b2;

	while (termFlag == FALSE) {

		if (count > 2560) {                                        /* write out longest available */
			tnewBits = _ilEncodeAdditionalMakeupCodes[12][0];
			tnumBits = _ilEncodeAdditionalMakeupCodes[12][1];
			count  -= 2560;
		}

		else {

			if (count > 1791) {                                    /* use additional make-up codes */
				index   = (count - 1792) / 64;
				tnewBits = _ilEncodeAdditionalMakeupCodes[index][0];
				tnumBits = _ilEncodeAdditionalMakeupCodes[index][1];
				count   = (count - 1792 - (index * 64));
			}

			else {

				if (count > 63) {
					index = (count - 64) / 64;
					if (!pixelValue) {                         /* use white make-up codes */
						tnewBits = _ilEncodeWhiteMakeupCodes[index][0];
						tnumBits = _ilEncodeWhiteMakeupCodes[index][1];
					}

					else {                                         /* use black make-up codes */
						tnewBits = _ilEncodeBlackMakeupCodes[index][0];
						tnumBits = _ilEncodeBlackMakeupCodes[index][1];
					}
					count = count - 64 - index * 64;
				}

				else {                                             /* count <= 63 */

					if (!pixelValue ) {                         /* use white terminating codes */
						tnewBits = _ilEncodeWhiteTerms[count][0];
						tnumBits = _ilEncodeWhiteTerms[count][1];
					}

					else {                                         /* use black terminating codes */
						tnewBits = _ilEncodeBlackTerms[count][0];
						tnumBits = _ilEncodeBlackTerms[count][1];
					}
					termFlag = TRUE;
					count   = 0;
				}
			}
		}

		tnewBits  = tnewBits << 16;                    /* shift the new bits to the left end      */
		tnewBits  = tnewBits >> pPriv->bitCount ;      /* right shift by the current bitCount     */
		pPriv->bits     = pPriv->bits     | tnewBits;  /* add the new bits to the existing bits   */
		pPriv->bitCount = pPriv->bitCount + tnumBits;  /* reduce the bitCount by no. of bits added*/

		if (pPriv->bitCount > 16)                      /* put the data, if count is more          */
			if (error =  _ilPutData(pPriv))
				return error;
	}

		a0 += b2;
		if (a0 >= width)
			break;

        white = !white;                     /* reverse sense of white and pixelValue */
        pixelValue = !pixelValue;
	}

	return(IL_OK);
}


/* ========================================================================
     -------------------- ilCompressG3Execute() -------------------
   Routine defined in ilCompressG3 for executing CCITT Group3
   compression when the pipe gets executed.
  

   ======================================================================== */

static ilError _ilCompressG3Execute(
ilExecuteData      *pData,
unsigned long       dstLine,
unsigned long      *pNLines
)
{


   /* ========================================================================
   ilCompressG3Execute() definitions
   ======================================================================== */

#define g3_1d  1                       /* Tag bit denoting G3 1 d compression   */
#define g3_2d  0                       /* Tag bit denoting G3 2 d compression   */

    /* Macro for putting a Byte aligned EOL into the compressed data streame    */

#define    PUT_BYTE_ALIGNED_EOL                       \
    if (pPriv->bitCount < 5) {                        \
       pPriv->bits     = pPriv->bits | (0x001 << 16); \
       pPriv->bitCount = 16;                          \
    }                                                 \
    else if (pPriv->bitCount < 13) {                  \
       pPriv->bits     = pPriv->bits | (0x001 << 8);  \
       pPriv->bitCount = 24;                          \
    }                                                 \
    else {                                            \
       pPriv->bits     = pPriv->bits | 0x001;         \
       pPriv->bitCount = 32;                          \
    }                         

    /* Macro for putting the Tag Bit after the  EOL for G3 2 Dimensional Compression        */

#define PUT_TAG_BIT    \
       pPriv->bits     = pPriv->bits | (tag_bit << ( 31 - pPriv->bitCount)); \
       pPriv->bitCount += 1; 

   /* ========================================================================
   ilCompressG3Execute() Declarations
   ======================================================================== */

	unsigned short      pixelValue;    /* Current Pixel value, white = 0, black = 1           */
	long                nLines;        /* Number of lines per source image strip              */
	ilPtr               pSrcLine;      /* Pointer to source image data first byte of line     */
	unsigned long       srcNBytes;     /* Number of source image bytes per row                */
	ilImagePlaneInfo   *pSrcPlane;     /* Pointer to the Source Image Plane                   */
	ilCompressG3G4PrivPtr pPriv;       /* Pointer to private image data                       */

	ilPtr               pRefLine;      /* Pointer to  first byte of Reference Line            */
	int                 temp;          /* some temp var..                                     */
	ilError             error;         /* returned error                                      */
	ilBool              Is_2DCoding;   /* G3 2 D coding is required, if True                  */
	int                 k,kmax;        /* K factors used for 2 D coding                       */
	int                 tag_bit;       /* Tag bit, used if 2 D coding is required             */
	ilBool              Is_EOLs;       /* EOL markers are set, and will have to be put        */

   /* ========================================================================
   Set up for execution of compression algorithm code
   ======================================================================== */

	pPriv               = (ilCompressG3G4PrivPtr) pData->pPrivate;

	/* Number of lines of source image data contained in the current strip */
	nLines              = *pNLines;
	if (nLines <= 0)      return IL_OK;

	pSrcPlane           = &pData->pSrcImage->plane[0];
	srcNBytes           = pSrcPlane->nBytesPerRow;
	pSrcLine            = pSrcPlane->pPixels + pData->srcLine * srcNBytes;


	pPriv->pDstImage = pData->pDstImage;
	if (pPriv->pDstImage->plane[0].bufferSize < G3_G4_MAX_BUFFER_WRITE)
		if (!_ilReallocCompressedBuffer (pPriv->pDstImage, 0, G3_G4_MAX_BUFFER_WRITE))
			return IL_ERROR_MALLOC;


	pPriv->pDstByte = pPriv->pDstImage->plane[0].pPixels + *pData->compressed.pDstOffset;
	pPriv->pDstBufferEnd = pPriv->pDstImage->plane[0].pPixels + 
	    (pPriv->pDstImage->plane[0].bufferSize - G3_G4_MAX_BUFFER_WRITE);

	Is_EOLs              =   ( pPriv->compData  & IL_G3M_EOL_MARKERS);
	Is_2DCoding          =   ( pPriv->compData  & IL_G3M_2D);

	/* For G3 - 2D files, EOLs must be present, if not Error in the Compressed
       flags... so check for that                                                */
	if ( (Is_2DCoding) && (!(Is_EOLs)) )
		return        IL_ERROR_COMPRESSION ;


	kmax = 0;

	/* If 2 D mode of Compression is required, then allocate space for the Reference
       line, equal to the size of one image line, and set all the bits to that of
       white pixels in the Image.

       Find out the K factor and set it. K specifies that, in 2 D coding, after coding
       one line 1 dimensionally, the next (K-1) lines should be coded 2 dimensionally,
       and again one line 1 dimen... & (K-1) 2 dimen... ;

       The default value of  K is assumed to be 4 , for every 1 d coding, after that,
       there will be 3 2d coded lines.
       Setting IL_G3M_K_FACTOR_2 in the compData mask bits , will take K = 2.
                                                                                          */
	if (Is_2DCoding) {

		if (pPriv->white)
			memset(pPriv->gpRefLine,0xff,(pSrcPlane->nBytesPerRow ));
		else
			memset(pPriv->gpRefLine,0x00,(pSrcPlane->nBytesPerRow ));
		pRefLine = pPriv->gpRefLine;

		if ( pPriv->compData  & IL_G3M_K_FACTOR_2 )
			kmax = 2;
		else kmax = 4;
		k = kmax - 1 ;
	}


	tag_bit = g3_1d;
	pPriv->Is_Lsb_First      =   ( pPriv->compData  & IL_G3M_LSB_FIRST) ;
	pPriv->bitCount          = 0;
	pPriv->bits              = 0;


	/* Compress the Image Strip Using the CCITT Group3 algorithm             */
	/* The flow is ...
      for each of the line to be compressed, before compressing, put the 
      EOL markers aligned or unaligned as appropriately, And then decide 
      whether 1 D coding or 2 D coding should be done, and call 
      ilCompressG3Line() or ilCompressG4Line() accordingly.                     

      After successful return from ilCompress..Line(), increment the
      Source image & Destn. image .
                                                                            */

	while (nLines-- > 0) {          /* for each line in the Src Image Strip */
                            		/* call _ilCompressG4Line               */

		if ( Is_EOLs  ) {

			/* add EOL code (0x001) for NON byte-aligned EOL support (CCITT Group3 Type3) */
			if (pPriv->compData & IL_G3M_EOL_UNALIGNED) {
				pPriv->bits      = (pPriv->bits | (0x00100000 >> pPriv->bitCount));
				pPriv->bitCount += 12;
			}
			/* add EOL code (0x[xxx xxxx0000 00000001]) for byte-aligned EOL support (Class F) */
			else {
				PUT_BYTE_ALIGNED_EOL 
			}
			if (error =  _ilPutData(pPriv))
				return error;

		}


		if ( Is_2DCoding ) {                                 /* if 2DCoding is required         */

			PUT_TAG_BIT                                       /* add the tag bit after EOL ...   */
			if ( tag_bit == g3_1d ) {                         /* next line must be 1 D coded     */
				if (error = _ilCompressG3Line( pPriv,pSrcLine ))             /* call 1 D Coding   */
					return error;
				tag_bit = g3_2d ;
			} else {                                           /* next line must be 2 D coded    */
				if (error = _ilCompressG4Line( pPriv,pSrcLine,pRefLine ))    /* call 2 D Coding   */
					return error;
				k-- ;                                           /* substract 1 line for 2d coding */
			}

			if ( k == 0 ) {                                    /* next line must be 1 D coded    */
				tag_bit = g3_1d ;
				k       = kmax - 1;
			} else
				pRefLine  = pSrcLine ;                /*   set the Current Line as Reference Line */

		} else {                                    /* Only 1 D coding is Required              */

			if ( !(Is_EOLs) ) {                     /* if EOLs not specified, then next row of compressed
                                                       data must start from the next byte boudary, so offset */
				if  ( ( temp = pPriv->bitCount % 8) != 0 ) {
					pPriv->bits      = (pPriv->bits | (0x0000 >> pPriv->bitCount));
					pPriv->bitCount += (8-temp) ;
				}
			}                                                             /* Block  !(Is_EOLs) */
			if (error = _ilCompressG3Line( pPriv,pSrcLine ))                /* call 1 D Coding   */
				return error;
		}                                                                 /* Block  Is_2DCoding*/

		pSrcLine += srcNBytes;                                /* increment the Source Line   */
		if (pPriv->bitCount > 16)
			if (error =  _ilPutData(pPriv))                     /* Put data, if count > 16   */
				return error;
	}                                                      /*   while (nLines-- > 0 ) loop  */
	if (error =  _ilPutData(pPriv))
		return error;

	/* All data (bits > 8) would have been added to Dstn, by _ilPutData()
       In case if more bits are remaining, add to the Dst Image              */

	if (pPriv->bitCount > 0) {

		/* If LSbit first is the special of the day, reverse bit order */
		if ( pPriv->Is_Lsb_First  ) {
			*pPriv->pDstByte = ilBitReverseTable [(unsigned char)((pPriv->bits >> 24) & 0xff)];
			pPriv->pDstByte++;
			pPriv->bits <<= 8;
		}
		else {
			*pPriv->pDstByte = (unsigned char)((pPriv->bits >> 24) & 0xff);
			pPriv->pDstByte++;
			pPriv->bits <<= 8;
		}

	}

	/*  Return the number of bytes written, = dst ptr - beginning of dst buffer */
	*pData->compressed.pNBytesWritten = pPriv->pDstByte -
	    (pPriv->pDstImage->plane[0].pPixels + *pData->compressed.pDstOffset);
	return IL_OK;
}
/* End ilCompressG3Execute() */



/* ========================================================================

        -------------------- ilCompressG3() --------------------
    Main body of code for CCITT Group3 .  This includes
    image descriptor parameter error checking and function calls for:
    strip handler initialization, adding the filter element to the pipe, 
    pipe initialization and execution, compression algorithm.....

   ======================================================================== */

IL_PRIVATE
ilBool _ilCompressG3 (
ilPipe              pipe,
ilPipeInfo         *pinfo,                              
ilImageDes         *pimdes,
ilImageFormat      *pimformat,
ilSrcElementData   *pSrcData,
ilPtr              pCompData 
)
{
	ilDstElementData        dstdata;
	ilCompressG3G4PrivPtr   pPriv;
	unsigned long           compData;

	/*  Validate that image is bitonal */
	if (pimdes->type != IL_BITONAL)
		return ilDeclarePipeInvalid (pipe, IL_ERROR_IMAGE_TYPE);

	/*
        Check for Group3, uncompressed, or any undefined bits on.  These
        are not supported!
    */

	compData = (pCompData) ? *((unsigned long *)pCompData) : 0;
	if (compData & IL_G3M_UNCOMPRESSED )
		return ilDeclarePipeInvalid (pipe, IL_ERROR_NOT_IMPLEMENTED);

	/*  From check in ilCompress(), pipe image is either uncompressed or G3, in
            which case check variants: if they match, exit with success, otherwise
            force decompression.
        */
	if (pimdes->compression == IL_G3) {
    	if (compData  == pimdes->compInfo.g3.flags ) {
			pipe->context->error = IL_OK;
			return TRUE;
		}
		ilGetPipeInfo (pipe, TRUE, pinfo, pimdes, pimformat);   /* decompress */
	}

	/*  Validate image bits per pixel */
	if (pimformat->nBitsPerSample[0] != 1)
		if (!ilConvert (pipe, (ilImageDes *)NULL, IL_FORMAT_BIT, 0, (ilPtr)NULL))
			return FALSE;


	/* ========================================================================
   The following strip handler initialize code for BITONAL compression
   formats is common for all compression types, yet duplicated where used.
   ======================================================================== */

	/* dstdata describes strips being output to next pipe element */
	dstdata.producerObject  =  (ilObject) NULL;
	pimdes->compression     =  IL_G3;
	pimdes->compInfo.g3.flags = compData;
	dstdata.pDes            =  pimdes;
	dstdata.pFormat         =  IL_FORMAT_BIT;
	dstdata.width           =  pinfo->width;
	dstdata.height          =  pinfo->height;
	dstdata.stripHeight     =  pSrcData->stripHeight;
	dstdata.constantStrip   =  pSrcData->constantStrip;
	dstdata.pPalette        =  (unsigned short *)NULL;

	dstdata.pCompData       =  (ilPtr)NULL;    /* des.compInfo.g3.flags describes it */


	/* ========================================================================
   Add the filter to the pipeline
   ======================================================================== */

	pPriv = (ilCompressG3G4PrivPtr) ilAddPipeElement(pipe, IL_FILTER,
     	    sizeof(ilCompressG3G4PrivRec),
	        0, pSrcData, &dstdata,_ilCompressG3Init,
    	    _ilCompressG3Cleanup, IL_NPF, _ilCompressG3Execute, 0);
	if (!pPriv) return FALSE;

	/* save private data */
	pPriv->width            =  pinfo->width;
	pPriv->compData         =  compData;
	pPriv->white            =  ( pimdes->blackIsZero ? 1 : 0 );
	pPriv->nDstLineBytes    = (pPriv->width + 7) / 8;
	return TRUE;
}


/*  ============================== G4 Compression ================================= */

/* Code for the Modes for Group 4 compression  
   Horizontal Mode, Pass Mode Vertical Mode    */

static unsigned short  horizcode[1][2] =
	/* 001      */            { 0x1,3 };	
static unsigned short  passcode [1][2] =
	/* 0001     */            { 0x1,4 };	
static unsigned short  vcodes[7][2] = {
	/* 0000 011 */            {  0x03,7 },
	/* 0000 11  */            {  0x03,6 },
	/* 011      */            {  0x03,3 },
	/* 1        */            {  0x1 ,1 },	
	/* 010      */            {  0x2,3 },	
	/* 0000 10  */            {  0x02,6 },
	/* 0000 010 */            {  0x02,7 }	
                       };


/* ========================================================================
       -------------------- _ilCompressG4Init -------------------
   Routine defined in ilCompressG4 for initializing CCITT Group3  
   compression when the pipe gets executed.

   ======================================================================== */

static ilError _ilCompressG4Init(
ilCompressG3G4PrivPtr       pPriv,
ilImageInfo            *pSrcImage,
ilImageInfo            *pDstImage
)
{
	/* Allocate space for Reference line, needed for 2 dimensional coding */

	pPriv->gpRefLine = (ilPtr)IL_MALLOC(pPriv->nDstLineBytes );
	if (!pPriv->gpRefLine)
		return IL_ERROR_MALLOC;
	return IL_OK;

}
/* End _ilCompressG4Init() */
/* ========================================================================
       -------------------- _ilCompressG4Cleanup -------------------
   Routine defined in ilCompressG4 for Cleaning up CCITT Group3  
   compression when the pipe gets executed.
   ======================================================================== */

static ilError _ilCompressG4Cleanup(
ilCompressG3G4PrivPtr       pPriv,
ilImageInfo            *pSrcImage,
ilImageInfo            *pDstImage
)
{

	if (pPriv->gpRefLine)
		IL_FREE( (ilPtr)pPriv->gpRefLine);
	return IL_OK;

}
/* End _ilCompressG4Cleanup() */



/* ========================================================================

        -------------------- _ilCompressG4Line()  --------------------
   Input : the pointer to the private record ilCompressG3G4PrivPtr;
           i.e the destination Image structre, the coded bits, the counts;
            
           pSrcLine : Pointer to the first byte of the Current source
                      image line, that is to be coded.
           pRefLine : Pointer to the first byte of the Reference Line
                      used for coding the current line.

   Does  : From the Source line and Reference line, calculates the
           Changing elements viz. a0,a1,a2,b1,b2;
           Identifies the Mode to any one of
           Horizontal Mode or Vertical Mode or Pass Mode;
           Gets the codes for these modes and adds to the bitstream
           (pPriv->bits, and increments pPriv->bitCount)
           If Horizontal Mode, further gets the codes for the white
           and black run lengths, and puts the code to the stream;
           Calculates new values for a0,a1,a2,b1,b2 and goes on till
           the value of a0 reaches the end of the Source Line.
           Puts the data to the destination Image (calls _ilPutData())


   Returns : IL_OK, if does not encouter any error .

      **  used in G3 compression for 2 Dimensional coding .
   ======================================================================== */
ilError
_ilCompressG4Line( 
 ilCompressG3G4PrivPtr pPriv,
ilPtr pSrcLine,
ilPtr pRefLine
)
{
	int                 a0, a1, a2;     /* changing elements pertaining to the pSrcLine            */
	int                 b1,b2;          /* changing elements pertaining to the pRefLine            */
	int                 d ;
	short               numBits;        /* the number of bits for the current run length            */
/* compatibility problem with long and unsigned long data fields */
        CARD32              newBits;        /* value of the bit code for the run length, left justified */
	ilBool              pixelValue;     /* value of the pixel 0 or 1                                */
	ilError             error;


	/*************************************************************************
    ** From Page 27 of spec:
    ** 4.2.1.3.1  Definition of changing picture elements
    **  A changing element is defined as an element whose "colour" (i.e., B or W) is
    ** different from the of the previous element along the same scan line.
    **  a0  The reference or starting changing element on the coding line.
    **      At the start of the coding line, a0 is set on an imaginary white
    **      changing element siturated just before the first element on the line.
    **      During the coding of the coding line, the position of a0 is defined by
    **      the previous coding mode.
    **  a1  The next changing element to the right of a0 on the coding line.
    **  a2  The next changing element to the right of a1 on the coding line.
    **  b1  The first chaning element on the reference line to the right of
    **      a0 and of opposite colour to a0.
    **  b2  The next changing element to the right of b1 on the reference line.
    ****************************************************************************/


	a0 = 0;
	a1 = (PIXEL(pSrcLine, 0) != pPriv->white ? 0 : 
	    _ilGetAbsDiff(pSrcLine, 0, pPriv->width,pPriv->white,1));
	a2 = 0;
	b1 = (PIXEL(pRefLine, 0) != pPriv->white ? 0 : 
	    _ilGetAbsDiff(pRefLine, 0, pPriv->width,pPriv->white,1));
	b2 = 0;

	for (;;) {

		b2 = _ilGetAbsDiff(pRefLine, b1, pPriv->width,PIXEL(pRefLine,b1),1);

		if (b2 >= a1) {                     /* either Horiz mode or Vert mode   */
                                			/* b2 is not to the left of a1      */
			d = b1 - a1;                    /* diff of b1 & a1, this determines */
                                			/* whether H or V mode              */


			if (!(-3 <= d && d <= 3)) {   	/* horizontal mode deducted         */

				a2 = _ilGetAbsDiff(pSrcLine, a1, pPriv->width,PIXEL(pSrcLine,a1),1);
				newBits = horizcode[0][0];                      /* get the code for Horiz Mode */
				numBits = horizcode[0][1];
				newBits = newBits << ( 32 - ( numBits + pPriv->bitCount)) ;
				pPriv->bits     = pPriv->bits     | newBits;    /* add the new bits to the stream */
				pPriv->bitCount = pPriv->bitCount + numBits;

				if (pPriv->bitCount > 16)                       /* put data to the destination    */
					if (error =  _ilPutData(pPriv))
						return error;

				if (a0+a1 == 0 || PIXEL(pSrcLine, a0) == pPriv->white ) {

					pixelValue = 0;                                /* first white run length 0 for white */
					if (error = _ilGetNdPutNewBits( pPriv,a1-a0,pixelValue))   /* get & put white for a1-a0 */
						return error;
					pixelValue = 1;
					if (error = _ilGetNdPutNewBits( pPriv,a2-a1,pixelValue))   /* get & put black for a2-a1 */
						return error;
				} else {

					pixelValue = 1;                               /* first black run length 0 for black */
					if (error = _ilGetNdPutNewBits( pPriv,a1-a0,pixelValue))   /* get & put black for a1-a0 */
						return error;
					pixelValue = 0;
					if (error = _ilGetNdPutNewBits( pPriv,a2-a1,pixelValue))   /* get & put white for a2-a1 */
						return error;
				}
				a0 = a2;                                       /* set a0 to a2 for further coding    */
			} else {	                                       /* vertical mode deducted  a1b1 <= 3  */

				newBits = vcodes[d+3][0];                      /* get the code for Vertical  Mode    */
				numBits = vcodes[d+3][1];
				newBits = newBits << ( 32 - ( numBits + pPriv->bitCount)) ;
				pPriv->bits     = pPriv->bits     | newBits;   /* add the new bits to the stream     */
				pPriv->bitCount = pPriv->bitCount + numBits;
				if (pPriv->bitCount > 16)                       /* put data to the destination    */
					if (error =  _ilPutData(pPriv))
						return error;

				a0 = a1;                                       /* set a0 to a1 for further coding    */
			}
		} else {			                                   /* pass mode deducted    b2 > a1      */


			newBits = passcode[0][0];                          /* get the code for Vertical  Mode    */
			numBits = passcode[0][1];
			newBits = newBits << ( 32 - ( numBits + pPriv->bitCount)) ;
			pPriv->bits     = pPriv->bits     | newBits;   /* add the new bits to the stream     */
			pPriv->bitCount = pPriv->bitCount + numBits;
			if (pPriv->bitCount > 16)                       /* put data to the destination    */
				if (error =  _ilPutData(pPriv))
					return error;

			a0 = b2;                                       /* set a0 to b2 for further coding    */
		}

		if (a0 >= pPriv->width)                                /* end of the line, break             */
			break;
		a1 = _ilGetAbsDiff(pSrcLine, a0, pPriv->width,PIXEL(pSrcLine,a0),1);
		b1 = _ilGetAbsDiff(pRefLine, a0, pPriv->width,PIXEL(pRefLine,a0),1);
		if (PIXEL(pRefLine, b1) == PIXEL(pSrcLine, a0))
			b1 = _ilGetAbsDiff(pRefLine, b1, pPriv->width,PIXEL(pRefLine,b1),1);

		newBits = 0;
		numBits = 0;
	}                                                          /* for (;;) loop                      */


	return(IL_OK);
}

/* ========================================================================

     -------------------- _ilCompressG4Execute() -------------------
   Routine defined in ilCompressG4 for executing CCITT Group4 
   compression when the pipe gets executed.

   ======================================================================== */

static ilError _ilCompressG4Execute(
ilExecuteData      *pData,
unsigned long       dstLine,
unsigned long      *pNLines
)
{


	/* ========================================================================
   ilCompressG4Execute() Declarations
   ======================================================================== */


	unsigned short      pixelValue;    /* Current Pixel value, white = 0, black = 1           */
	long                nLines;        /* Number of lines per source image strip              */
	ilPtr               pSrcLine;      /* Pointer to source image data first byte of line     */
	unsigned long       srcNBytes;     /* Number of source image bytes per row                */
	ilImagePlaneInfo   *pSrcPlane;     /* Pointer to the Source Image Plane                   */
	ilCompressG3G4PrivPtr pPriv;       /* Pointer to private image data                       */
	ilPtr               pRefLine;      /* Pointer to  first byte of Reference Line            */
	ilError             error;         /* returned error                                      */

	/* ========================================================================
   Set up for execution of compression algorithm code
   ======================================================================== */

	pPriv               = (ilCompressG3G4PrivPtr) pData->pPrivate;

	/* Number of lines of source image data contained in the current strip */
	nLines              = *pNLines;
	if (nLines <= 0)      return IL_OK;

	pSrcPlane           = &pData->pSrcImage->plane[0];
	srcNBytes           = pSrcPlane->nBytesPerRow;
	pSrcLine            = pSrcPlane->pPixels + pData->srcLine * srcNBytes;

	/*      Make sure dst compressed bufferSize is min # writes in size.
            Set pDst to beginning of dst buffer + dst offset (= 0 unless writing
            to an image, in which case = byte past where last strip was written).
            Set pDstBufferEnd to begin of buffer + bufferSize - max # bytes written,
            so that pDst <= pDstBufferEnd can check for room for writing max # bytes.
        */


	pPriv->pDstImage = pData->pDstImage;
	if (pPriv->pDstImage->plane[0].bufferSize < G3_G4_MAX_BUFFER_WRITE)
		if (!_ilReallocCompressedBuffer (pPriv->pDstImage, 0, G3_G4_MAX_BUFFER_WRITE))
			return IL_ERROR_MALLOC;


	pPriv->pDstByte = pPriv->pDstImage->plane[0].pPixels + *pData->compressed.pDstOffset;
	pPriv->pDstBufferEnd = pPriv->pDstImage->plane[0].pPixels + 
	    (pPriv->pDstImage->plane[0].bufferSize - G3_G4_MAX_BUFFER_WRITE);


	/* Set the Reference Line to have white pixels. If the value of
       white pixel is 1, set the line to have bits 1, else set the
       line with zeros; For each strip or block, first, the reference
       line must be taken as white; after coding one line, the coded
       line becomes the reference line for the next line.               */

	if (pPriv->white)
		memset(pPriv->gpRefLine,0xff,(pSrcPlane->nBytesPerRow ));
	else
		memset(pPriv->gpRefLine,0x00,(pSrcPlane->nBytesPerRow ));

	pRefLine = pPriv->gpRefLine;

	pPriv->Is_Lsb_First  =  ( ( pPriv->compData  & IL_G4M_LSB_FIRST) ? 1 : 0 );

	/* Compress the Image Strip Using the G4 algorithm                 */
	/* G4 Compression uses Two - dimensional coding 

     * Two  dimensional  coding for bi-level  images :  - line-by-line
       coding  method in which the position of each  changing  picture
       element on the current coding line is coded with respect to the
       position  of a  corresponding  reference  element  situated  on
       either  the  coding  line  or  the  reference  line,  which  is
       immediately above the coding line.

     - depending upon the values of the relative positions of changing
       elements,  one of the three modes are identified,  which are
       Pass mode,  Horizontal  mode & Vertical mode.  Codes for these
       modes  along with run  length  codes  (only in case of  Horizontal
       mode) are put. And the  run-length  codes  are  same  as  used 
       in  Group 3 scheme
                         
       _ilCompressG4Line(.. ) takes the input from pSrcLine & pRefLine
       and compresses/encodes the line and put the output to DstBuffer
       in pPriv record. This function will be used in G3 Compression
       for 2d option G3-2d.

       After coding all the lines, code for End Of Fascimile Block(EOFB)
       is put, which is two EOL codes.
                                                                          */


	pPriv->bitCount          = 0;
	pPriv->bits              = 0;

	while (nLines-- > 0) {          /* for each line in the Src Image Strip */
                            		/* call _ilCompressG4Line               */

		if (error = _ilCompressG4Line( pPriv,pSrcLine,pRefLine ))
			return error;

		if (pPriv->bitCount > 16)
			if (error =  _ilPutData(pPriv))            /*   Put data, if count > 16                */
				return error;

		pRefLine  = pSrcLine ;                /*   set the Current Line as Reference Line */
		pSrcLine += srcNBytes;                /*   Increment the Source Line              */

	}                                         /*   while (nLines-- > 0 ) loop             */


	/* Add the End Of Fascimile Block (EOFB)  code to the Destination Image               */

	pPriv->bits =  (pPriv->bits | (_ilEncodeEndOfLineCode[0][0] << (16 - pPriv->bitCount) ));
	pPriv->bitCount += _ilEncodeEndOfLineCode[0][1];

	if (error =  _ilPutData(pPriv))
		return error;

	pPriv->bits =  (pPriv->bits | (_ilEncodeEndOfLineCode[0][0] << (16 - pPriv->bitCount) ));
	pPriv->bitCount += _ilEncodeEndOfLineCode[0][1];

	if (error =  _ilPutData(pPriv))
		return error;

	/* All data (bits > 8) would have been added to Dstn, by _ilPutData()
       In case if more bits are remaining, add to the Dst Image              */

	if (pPriv->bitCount > 0) {

		/* If LSbit first is Required, reverse bit order for the Output */
		if ( pPriv->Is_Lsb_First  ) {
			*pPriv->pDstByte = ilBitReverseTable [(unsigned char)((pPriv->bits >> 24) & 0xff)];
			pPriv->pDstByte++;
			pPriv->bits <<= 8;
		}
		else {
			*pPriv->pDstByte = (unsigned char)((pPriv->bits >> 24) & 0xff);
			pPriv->pDstByte++;
			pPriv->bits <<= 8;
		}

	}

	/*  Return the number of bytes written, = dst ptr - beginning of dst buffer */
	*pData->compressed.pNBytesWritten = pPriv->pDstByte -
	    (pPriv->pDstImage->plane[0].pPixels + *pData->compressed.pDstOffset);

	return IL_OK;
}

/* End ilCompressG4Execute() */


/* ========================================================================

        -------------------- ilCompressG4() --------------------
    Main body of code for CCITT Group4 .  This includes
    image descriptor parameter error checking and function calls for:
    strip handler initialization, adding the filter element to the pipe, 
    pipe initialization and execution, compression algorithm.....

   ======================================================================== */

IL_PRIVATE
ilBool _ilCompressG4 (
ilPipe              pipe,
ilPipeInfo         *pinfo,                              
ilImageDes         *pimdes,
ilImageFormat      *pimformat,
ilSrcElementData   *pSrcData,
ilPtr              pCompData 
)
{
	ilDstElementData      dstdata;
	ilCompressG3G4PrivPtr   pPriv;
	unsigned long         compData;

	/*  Validate that image is bitonal */
	if (pimdes->type != IL_BITONAL)
		return ilDeclarePipeInvalid (pipe, IL_ERROR_IMAGE_TYPE);

	/*
        Check for Group4, uncompressed, or any undefined bits on.  These
        are not supported!
    */

	compData = (pCompData) ? *((unsigned long *)pCompData) : 0;
	if (compData & IL_G4M_UNCOMPRESSED )
		return ilDeclarePipeInvalid (pipe, IL_ERROR_NOT_IMPLEMENTED);

	/*  From check in ilCompress(), pipe image is either uncompressed or G4, in
            which case check variants: if they match, exit with success, otherwise
            force decompression.
        */
	if (pimdes->compression == IL_G4) {
        if ((compData & IL_G4M_LSB_FIRST) == 
            (pimdes->compInfo.g4.flags & IL_G4M_LSB_FIRST)) {
			pipe->context->error = IL_OK;
			return TRUE;
		}
		ilGetPipeInfo (pipe, TRUE, pinfo, pimdes, pimformat);   /* decompress */
	}

	/*  Validate image bits per pixel */
	if (pimformat->nBitsPerSample[0] != 1)
		if (!ilConvert (pipe, (ilImageDes *)NULL, IL_FORMAT_BIT, 0, (ilPtr)NULL))
			return FALSE;


	/* ========================================================================
   The following strip handler initialize code for BITONAL compression
   formats is common for all compression types, yet duplicated where used.
   ======================================================================== */

	/* dstdata describes strips being output to next pipe element */
	dstdata.producerObject  =  (ilObject) NULL;
    pimdes->compression     =  IL_G4;
    pimdes->compInfo.g4.flags = compData;
    dstdata.pDes            =  pimdes;
	dstdata.pFormat         =  IL_FORMAT_BIT;
	dstdata.width           =  pinfo->width;
	dstdata.height          =  pinfo->height;
	dstdata.stripHeight     =  pSrcData->stripHeight;
	dstdata.constantStrip   =  pSrcData->constantStrip;
	dstdata.pPalette        =  (unsigned short *)NULL;
	dstdata.pCompData       =  (ilPtr)NULL;    /* des.compInfo.g4.flags describes it */


	/* ========================================================================
   Add the filter to the pipeline
   ======================================================================== */

	pPriv = (ilCompressG3G4PrivPtr) ilAddPipeElement(pipe, IL_FILTER,
	    sizeof(ilCompressG3G4PrivRec),
	    0, pSrcData, &dstdata,_ilCompressG4Init,
	    _ilCompressG4Cleanup, IL_NPF, _ilCompressG4Execute, 0);
	if (!pPriv) return FALSE;

	/* save private data */
	pPriv->width            =  pinfo->width;
	pPriv->compData         =  compData;
    pPriv->white            =  ( pimdes->blackIsZero ? 1 : 0 );
	pPriv->nDstLineBytes = (pPriv->width + 7) / 8;
	return TRUE;
}



