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
/* $XConsortium: ildecompg4.h /main/3 1995/10/23 15:45:43 rswiston $ */
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



/* ====================================================================================
        -------------------- Macros --------------------
   ==================================================================================== */

#define READ32_LSB(address) \
   (* ((unsigned char *)(address))         + \
   (*(((unsigned char *)(address))+1)<< 8) + \
   (*(((unsigned char *)(address))+2)<<16) + \
   (*(((unsigned char *)(address))+3)<<24))


#define READ32_MSB(address) \
   (ilBitReverseTable[* ((unsigned char *)(address))]         + \
   (ilBitReverseTable[*(((unsigned char *)(address))+1)] << 8) + \
   (ilBitReverseTable[*(((unsigned char *)(address))+2)]<<16) + \
   (ilBitReverseTable[*(((unsigned char *)(address))+3)]<<24) )



#define GET_VALUE_LSB(value,base, offset, mask) {  \
    if (offset > pPriv->maxSrcPos) return IL_ERROR_COMPRESSED_DATA; \
    value = (READ32_LSB((base) + ((offset) >> 3)) >> ((offset) & 7) & (mask)); \
}

#define GET_VALUE_MSB(value,base, offset, mask) {  \
    if (offset > pPriv->maxSrcPos) return IL_ERROR_COMPRESSED_DATA; \
    value = (READ32_MSB((base) + ((offset) >> 3)) >> ((offset) & 7) & (mask)); \
}
 



            
/* ====================================================================================
        -------------------- Code table constants --------------------
   ==================================================================================== */

#define G4K_BlackTableSize      8192
#define G4K_WhiteTableSize      4096
#define G4K_DecodeTableSize     4096
#define G4K_MinSrcLen             14    /* Defines the maximum codeword size in bits */
#define G4M_WhiteRun          0x0FFF
#define G4M_BlackRun          0x1FFF
#define G4M_Codeword          0x0FFF



/* ====================================================================================
        -------------------- Decoding mode constants --------------------
   ==================================================================================== */

#define G4K_ModeNewScan            1
#define G4K_ModeCodeword           2
#define G4K_ModeFirstRun           3
#define G4K_ModeSecondRun          4


/*
    These constants define the usage of the change lists. If the change list
    index is even, a white run is described. If a black run is being defined,
    the changelist index will be odd.
*/
#define	G4K_WhiteRun               1
#define G4K_BlackRun               0


/* Code type constants */

#define G4K_CodetypeTerminator     2
#define G4K_CodetypeEol           -1
#define G4K_CodetypeMakeup         1


/* Code value constants */

#define G4K_CodevaluePass          4
#define G4K_CodevalueHoriz         5
#define G4K_CodevalueV0            0
#define G4K_CodevalueVR1           1
#define G4K_CodevalueVL1          -1
#define G4K_CodevalueVR2           2
#define G4K_CodevalueVL2          -2
#define G4K_CodevalueVR3           3
#define G4K_CodevalueVL3          -3



/* ====================================================================================
        -------------------- Status code definitions --------------------
   ==================================================================================== */

#define G4K_StsEol                -1
#define G4K_StsDcdErr             -2
#define G4K_StsSrcExh             -3
#define G4K_StsDstFul             -4
#define G4K_AlgorithmAbort        62

/* define to find out the value of the bit (1 or 0) in the image line      */
#define	PIXEL(buf,ix)	((((buf)[(ix)>>3]) >> (7-((ix)&7))) & 1)             

/* define to find out the value of the bit (1 or 0) in the image line      */
#define	PIXEL_LSB(buf,ix)	((ilBitReverseTable[((buf)[(ix)>>3])] >> (7-((ix)&7))) & 1)             
                 



/* ========================================================================
   Decompression private data structure definition
   ======================================================================== */

typedef struct {
   int                  width;         /* width of the image              */   
   long                 compFlags;     /* flags denoting G4 comp. options */
   ilBool               white;         /* value of the white pixel 0 or 1 */
   ilPtr                ImageP;        /* ptr. to the Source Image begin  */
   int                  srcpos;        /* bit position is Src image start */
   ilBool               Is_Lsb_First;  /* true if  LSB is desired         */
   ilPtr                gpRefLine;     /* ptr.to refrence line for 2d coding */
   long                 nDstLineBytes; /* no.of bytes in the Image        */
   long                 maxSrcPos;     /* size of the src Image in bits   */
}  ilDecompG3G4PrivRec, *ilDecompG3G4PrivPtr;

/* ====================================================================================
   -------------------- Huffman decode table structure definition --------------------
   ==================================================================================== */

typedef struct {
    short           value;
    unsigned char   length;
    char            type;
} ilDecompG4HuffTableRec, *ilDecompG4HuffTablePtr;

typedef	const ilDecompG4HuffTableRec* 	ilDecompG4HuffTablePtrConst;

IL_EXTERN const ilDecompG4HuffTableRec ilArFax1DDecodeWhite[G4K_WhiteTableSize];

IL_EXTERN const ilDecompG4HuffTableRec ilArFax1DDecodeBlack[G4K_BlackTableSize];

IL_EXTERN const ilDecompG4HuffTableRec ilArFax2DDecodeTable[G4K_DecodeTableSize];

IL_EXTERN ilError   _ilDecompG4Line(
register ilDecompG3G4PrivPtr  pPriv,
ilPtr          pRefLine,
ilPtr          dstImageP
);
