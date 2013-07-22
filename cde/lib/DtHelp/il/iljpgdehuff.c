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
/* $TOG: iljpgdehuff.c /main/5 1999/10/14 13:19:16 mgreess $ */
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

#include <stdlib.h>

#include "iljpgdecodeint.h"

/*
   utilities for fast table-based huffman decoder

  NOTES
   Author: G. Seroussi, HPL, PaloAlto. May 21, 1992
   Modifications: V. Bhaskaran, HPL, PaloAlto. May 29, 1992.
                  email: bhaskara@hplvab.hpl.hp.com
*/

typedef struct tree_node {
	int	c;
	struct tree_node *left;
	struct tree_node *right;
} tree_node;

typedef int LOOKUP;

struct table_set {
	LOOKUP	*lookup_symb;
	LOOKUP	*lookup_len;
	tree_node *tree_nodes;
};

typedef unsigned int BITBUF;

#define LOOKUP_BITS 9
#define FOUND_EOI 0x80

#define ABSIZE 256       /* max size of alphabet for Huffman encoding */
#define NOLEN  0    
#define BLOCK_COEFFS    64      /* number of coefficients per block */
#define NOLEAF  (BITBUF)(-1)
#define BITBUFSIZE (8*sizeof(BITBUF))
#define SHIFTBUF ( BITBUFSIZE - 8 )
#define BUFMSB   ( ((BITBUF) 1) << ( BITBUFSIZE - 1 ) )

        /*  Data private to this file.  Pointed to by iljpgDecodePrivRec.pHuffPriv */
typedef struct {
    unsigned int        bitbuf;
    int                 bitsleft;
    int                 resetDone;
    struct table_set    table_set_dc[4];
    struct table_set    table_set_ac[4];
    } iljpgDehuffRec, *iljpgDehuffPtr;


/* Characteristc vector of the upper 4x4 block in zig-zag order */
static int fourx4_table[] = 
{ 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 0, 1, 1, 1, 0, 0,
  0, 1, 1, 0, 0, 0, 0, 0,
  1, 0, 0, 0, 0, 0, 0, 0, 
  0, 0, 0, 0, 0, 0, 0, 0, 
  0, 0, 0, 0, 0, 0, 0, 0, 
  0, 0, 0, 0, 0, 0, 0, 0, 
  0, 0, 0, 0, 0, 0, 0, 0
};


    /*  Build a lookup table and a partial binary tree for the Huffman dictionary.
        Return absize of table; = 0 if tables are invalid.
    */

/* The lookup tables are used by the Huffman decoding engine as follows:
        a fixed number N of bits are read from the encoded stream (N is
        given by the "lookup_bits" parameter). Let P denote the binary
        number formed by the N bits read.
        P is used as an index into two lookup tables: lookup_symb[]
        and lookup_len[]. If P uniquely identifies a Huffman code (i.e. if its 
        N-bit pattern contains a Huffman code as a prefix), then the encoded 
        symbol is given by lookup_symb[P], and the length of the corresponding
        Huffman code is given by lookup_len[P]. The latter is used to determine
        how many bits from the input stream were "used". 
        If P does not uniquely identify a Huffman code (i.e. it is a prefix of 
        a Huffman code), then lookup_len[P] = NOLEN (currently set to 0),
        and lookup_symb[P] is an index into an array of tree nodes, giving the
        Huffman subtree determined by the prefix P. The rest of the Huffman
        code is obtained by traversing this subtree according to the input bits
        following P in the encoded stream.

        Notice that a given Huffman code may have many entries in the lookup
        tables. For example, if N=9 and 01 is a Huffman code,
        then all the entries with indices 01XXXXXXX will contain the same
        symbol (the one represented by the code 01) and length.

 */

static int build_intermediate_tables (
    iljpgPtr bits,
    iljpgPtr vals,
    int      max_absize,
    int     *hufvals,               /* must be of length max_absize+1; all zeros */
    int     *hufcodes,              /* ditto */
    int     *huflen                 /* ditto */
    )
{
        long int i, j, k, code, si;
        int  absize;
        int  length, sumLengths;

        /* generate code lengths: return 0 (error) if sum > 256 */
        for (i = 0, k = 0, sumLengths = 0;  i < 16; i++) {
            length = bits[i];
            if ((sumLengths += length) > max_absize)
                return 0;               /* table too long; return error */
            for (j = 0; j < length; j++)
                huflen[k++] = i+1;
            }

        /*  generate values until huflen[i] == 0 - at least one is, because array
            is "max_absize+1" entries long and zeroed, and above set at most max_absize.
        */
        for ( i=0; huflen[i]; i++ )
                hufvals[i] = vals[i];

        absize = i;          /* actual number of Huffman-encoded symbols */

        /* now generate codes using the JPEG document procedure */
        k=0;
        code = 0;
        si = huflen[0];

        while (1) {
                do {
                    hufcodes[k] = code;
                    code ++;
                    k ++;

                }
                while ( huflen[k] == si );

                if ( huflen[k] == 0 )
                        break;   /* done */
                
                do { 
                        code <<= 1;
                        si ++;
                }
                while ( huflen[k] != si );
        }

        return absize;
}


static iljpgError build_huffman_tables (
    iljpgPtr    bits,           /* "Bits" table as specified in the JPEG standard */
    iljpgPtr    vals,           /* "Vals" table as specified in the JPEG standard */
    int         lookup_bits,    /* # of Huffman bits that are looked up in the tables */
    struct table_set *table_setp /* pointer to a struct of pointers to lookup tables */
    )
{
        int     next_node = 0,          /* index to next available tree node */
                c, len, s, i, j, n;
        long    numnodes,               /* max number nodes in Huffman tree */
                lookup_index,
                lookup_size,
                code, mask;
        tree_node *tree_nodes,          /* pointer to pool of tree nodes */
                  *nodep, *leafp, **followp;  /* misc aux. tree pointers */
        LOOKUP    *lookup_symb, *lookup_len; /* pointers to lookup tables */
        int       absize; /* actual number of Huffman-encoded symbols */
        int      *hufvals, *hufcodes, *huflen; /* intermediate tables */
        iljpgError error;


            /*  Init all ptrs to allocated data to null. If any allocation fails 
                deallocate all and return malloc error.
            */
        hufvals = hufcodes = huflen = (int *)NULL;
        lookup_symb = lookup_len = (LOOKUP *)NULL;
        tree_nodes = (tree_node *)NULL;

        error = ILJPG_ERROR_DECODE_MALLOC;             /* assumed malloc error for now */
        if (!(hufvals = (int *)ILJPG_MALLOC_ZERO ((ABSIZE+1) * sizeof(int))))
            goto BuildTablesError;
        if (!(hufcodes = (int *)ILJPG_MALLOC_ZERO ((ABSIZE+1) * sizeof(int))))
            goto BuildTablesError;
        if (!(huflen = (int *)ILJPG_MALLOC_ZERO ((ABSIZE+1) * sizeof(int))))
            goto BuildTablesError;
                
        absize = build_intermediate_tables(bits, vals, ABSIZE, hufvals,
                                                            hufcodes, huflen);
        if (absize <= 0) {
            error = ILJPG_ERROR_DECODE_DCAC_TABLE;
            goto BuildTablesError;
            }

        lookup_size = ( 1L << lookup_bits );

        /* allocate lookup tables */
        if (!(lookup_symb = (LOOKUP *)ILJPG_MALLOC_ZERO (lookup_size * sizeof(LOOKUP))))
            goto BuildTablesError;
        if (!(lookup_len = (LOOKUP *)ILJPG_MALLOC_ZERO (lookup_size * sizeof(LOOKUP))))
            goto BuildTablesError;

        /* allocate tree nodes */
        numnodes = 2*absize + 1;  /* the max number of leaves is absize+1, since
                                     a leave is created for code 111...111 even
                                     though JPEG doesn't use it */

        if (!(tree_nodes = (tree_node *)ILJPG_MALLOC_ZERO (numnodes * sizeof(tree_node))))
            goto BuildTablesError;

        /* initialize lookup tables */
        for ( i=0; i<lookup_size; i++ ) {
                lookup_symb[i] = -1;
                lookup_len[i] = NOLEN;
        }

        /* now go thru the Huffman table, build a lookup table,
           and also parts of the Huffman tree for codes that
           are not fully in the lookup table */
        for ( s = 0; s < absize; s++ ) {

                c    = hufvals[s];
                code = hufcodes[s];
                len  = huflen[s];

                if ( len == NOLEN )  /* no entry for this character */
                        continue;

                if ( len <= lookup_bits ) { /* enter in lookup table */
                        lookup_index = code << ( lookup_bits - len );
                        /* number of entries for this code */
                        n = 1 << ( lookup_bits - len );
                        for ( j=0; j<n; j++ ) {
                                lookup_symb[ lookup_index + j ] = c;
                                lookup_len[ lookup_index + j ] = len;
                        }

                } 
                else {
                /* build the part of the Huffman tree for the 
                  end of this code */
                        lookup_index = code >> ( len - lookup_bits );
                        
                        /* check that the lookup entry is not occupied by
                                   a full coded symb */
                        if ( lookup_len[lookup_index] != NOLEN )  {
#if DEBUG
                                fprintf(stderr,"*** ERROR: lookup_len[%ld] = %d\n",lookup_index,lookup_len[lookup_index]);
#endif
                                error = ILJPG_ERROR_DECODE_INTERNAL;
                                goto BuildTablesError;
                        }
                        /* see if the prefix is already in the table */
                        if ( (j = lookup_symb[lookup_index]) != -1  ) {
                                /* it's there */
                                nodep = tree_nodes + j; /* get tree pointer */
                        }
                        else {
                            /* create a tree node */
                            /* check that there are available nodes
                               (this should never fail) */
                            if ( next_node >= numnodes ) {
#if DEBUG
                                fprintf(stderr,"build_huffman_tables: *** ERROR: ran out of tree nodes (a)\n");
#endif
                                error = ILJPG_ERROR_DECODE_INTERNAL;
                                goto BuildTablesError;
                            }
                            lookup_symb[lookup_index] = next_node;
                            nodep = tree_nodes + next_node;
                            nodep->left = NULL;
                            nodep->right = NULL;
                            next_node ++;
                            lookup_len[lookup_index] = NOLEN;
                        }


                        mask = (1L<<(len - lookup_bits - 1));

                        /* now go down the tree branch, building as needed */
                        for ( i = lookup_bits; i < len; i++ ) {
                            followp = (code & mask) ? &nodep->left : &nodep->right;
                            if ( *followp == NULL ) {
                                /* build a new node */
                                /* check that there are available nodes
                                   (this should never fail) */
                                if ( next_node >= numnodes ) {
#if DEBUG
                                    fprintf(stderr,"build_huffman_tables: *** ERROR: ran out of tree nodes (b)\n");
#endif
                                    error = ILJPG_ERROR_DECODE_INTERNAL;
                                    goto BuildTablesError;
                                }
                                *followp = &tree_nodes[next_node++];
                                leafp = *followp;     /* remember leaf */

                                (*followp)->left = NULL;
                                (*followp)->right= NULL;
                                (*followp)->c    = -1;
                            }
                            nodep = *followp;
                            mask >>= 1;
                        }

                        leafp->c = c;  /* write symbol in leaf */
                }
        }
        /* return pointers to lookup tables */
        table_setp->lookup_symb = lookup_symb;
        table_setp->lookup_len = lookup_len;
        table_setp->tree_nodes = tree_nodes;

        /* deallocate temporary data and return success */
        ILJPG_FREE (hufvals);
        ILJPG_FREE (hufcodes);
        ILJPG_FREE (huflen);
        return 0;

    /*  Goto point on error: deallocate all non-null and return "error". */
BuildTablesError:
        if (lookup_symb) ILJPG_FREE (lookup_symb);
        if (lookup_len) ILJPG_FREE (lookup_len);
        if (tree_nodes) ILJPG_FREE (tree_nodes);
        if (hufvals) ILJPG_FREE (hufvals);
        if (hufcodes) ILJPG_FREE (hufcodes);
        if (huflen) ILJPG_FREE (huflen);
        return error;
}



    /*  -------------------- _iljpgDehuffInit -------------------------- */
    /*  Called by iljpgDecodeInit() to init for Huffman decoding.
    */
    ILJPG_PRIVATE_EXTERN
iljpgError _iljpgDehuffInit (
    iljpgDecodePrivPtr  pPriv
    )
{
    iljpgDehuffPtr      pHuffPriv;
    iljpgDataPtr        pData;
    iljpgPtr            pTable;
    iljpgError          error;
    int                 i;

        /*  Allocate Huffman private area and point to it in decode private */
    pData = pPriv->pData;
    pHuffPriv = (iljpgDehuffPtr)ILJPG_MALLOC_ZERO (sizeof (iljpgDehuffRec));
    if (!pHuffPriv)
        return ILJPG_ERROR_DECODE_MALLOC;
    pPriv->pHuffPriv = (iljpgPtr)pHuffPriv;

        /*  Clear buffer and count of input bits, and flag that reset was done */
    pHuffPriv->bitbuf = 0;
    pHuffPriv->bitsleft = 0;
    pHuffPriv->resetDone = 0;

        /*  Build lookup tables from DC/AC tables from caller (*pPriv->pData) */
    for (i = 0; i < 4; i++) {
        if ((pTable = pData->DCTables[i])) {
            if ((error = build_huffman_tables (pTable, (pTable+16), LOOKUP_BITS,
                               &pHuffPriv->table_set_dc[i])))
                return error;
            }
        if ((pTable = pData->ACTables[i])) {
            if ((error = build_huffman_tables (pTable, (pTable+16), LOOKUP_BITS,
                               &pHuffPriv->table_set_ac[i])))
                return error;
            }
        }

    return 0;
}

                       
    /*  -------------------- _iljpgDehuffCleanup -------------------------- */
    /*  Called by iljpgDecodeCleanup() to cleanup after Huffman decoding.
    */
    ILJPG_PRIVATE_EXTERN
iljpgError _iljpgDehuffCleanup (
    iljpgDecodePrivPtr  pPriv
    )
{
    iljpgDehuffPtr      pHuffPriv;
    int                 i;

        /*  Free the Huffman decode private data including lookup tables */
    pHuffPriv = (iljpgDehuffPtr)pPriv->pHuffPriv;
    if (pHuffPriv) {
        for (i = 0; i < 4; i++) {
            if (pHuffPriv->table_set_dc[i].lookup_symb)
                ILJPG_FREE (pHuffPriv->table_set_dc[i].lookup_symb);
            if (pHuffPriv->table_set_dc[i].lookup_len)
                ILJPG_FREE (pHuffPriv->table_set_dc[i].lookup_len);
            if (pHuffPriv->table_set_dc[i].tree_nodes)
                ILJPG_FREE (pHuffPriv->table_set_dc[i].tree_nodes);
            if (pHuffPriv->table_set_ac[i].lookup_symb)
                ILJPG_FREE (pHuffPriv->table_set_ac[i].lookup_symb);
            if (pHuffPriv->table_set_ac[i].lookup_len)
                ILJPG_FREE (pHuffPriv->table_set_ac[i].lookup_len);
            if (pHuffPriv->table_set_ac[i].tree_nodes)
                ILJPG_FREE (pHuffPriv->table_set_ac[i].tree_nodes);
            }
        ILJPG_FREE (pHuffPriv);
        }

    return 0;
}


    /*  -------------------- _iljpgDehuffReset -------------------------- */
    /*  Reset for Huffman decoding, when a restart marker is seen or at the
        beginning of a strip, which implicitly is a restart.
    */
    ILJPG_PRIVATE_EXTERN
iljpgError _iljpgDehuffReset (
    iljpgDecodePrivPtr  pPriv
    )
{
    iljpgDehuffPtr      pHuffPriv;
    int                 comp;

        /*  Clear buffer and count of input bits */
    pHuffPriv = (iljpgDehuffPtr)pPriv->pHuffPriv;
    pHuffPriv->bitbuf = 0;
    pHuffPriv->bitsleft = 0;

        /*  Flag that reset done, to signal DehuffExecute to eat restart marker */
    pHuffPriv->resetDone = 1;

        /*  Clear set lastDC to 0, as per JPEG spec on a restart marker */
    for (comp = 0; comp < pPriv->pData->nComps; comp++)
        pPriv->compData[comp].lastDC = 0;

    return 0;
}


    /*  -------------------- _iljpgDehuffExecute -------------------------- */
    /*  Decode 64 bytes of huffman bits from "stream" into "mb", for component
        index "comp".  Return a code (e.g. HUFF_FULL) for what is non-zero in the 
        block to "*pBlockType".
    */
    ILJPG_PRIVATE_EXTERN
iljpgError _iljpgDehuffExecute (
    iljpgDecodePrivPtr  pPriv,
    ILJPG_DECODE_STREAM stream,
    int                 comp,
    int                *mb,
    unsigned int       *pBlockType              /* RETURNED */
    )
{
        iljpgDehuffPtr  pHuffPriv;
        int             coeff_ct = 0;
#define         is_dc   (!coeff_ct)     /* First coefficient is DC */
        int             delta;
        int             len, zrun; 
        register BITBUF bitbuf, ch, c;
        register int    bitsleft;
        iljpgError      error = 0;      /* assume no error */
        int             index;
        int             is_fourx4 = 1, is_dconly = 0;
        int             coeff;
        tree_node      *nodep;
        struct table_set *pTableSet;
        LOOKUP          *lookup_symb, *lookup_len;
        tree_node       *tree_nodes;
        BITBUF          markerValue = 0;    /* 0 or code for marker seen */

            /* macros for outputting coefficients to a memory buffer */
#       define  WRITE_COEFF(x)  (*mb++ = (int)x)
#       define  WRITE_ZRUN(n)   ( mb += n )

            /*  Get the next byte from stream into "_byte".  If a 0xff, 
                eat next char ifit is a null, otherwise it is a marker:
                if a restart marker ignore it; else store in markerValue, which if 
                != 0 when this is invoked means a marker has already been seen (error).
                    If an end-of-data error, set markerValue to non-zero.
                Thus any invocation of this macro after end-of-data or
                a non-restart marker seen is an error - but the first one is allowed.
                Apparently the code here fetches one byte ahead.
            */
#       define DECODE_GET_BYTE(_byte) {                                    \
            if (markerValue) {                                             \
                error = ILJPG_ERROR_DECODE_DATA;                           \
                goto out_mainloop;                                         \
                }                                                          \
            if (!ILJPG_DECODE_GET_BYTE (stream, _byte, error)) {           \
                if (error == ILJPG_ERROR_DECODE_EOD) {                     \
                    markerValue = 1;                                       \
                    _byte = 0xff;                                          \
                    error = 0;                                             \
                    }                                                      \
                else goto out_mainloop;                                    \
                }                                                          \
            else if (_byte == 0xff) {                                      \
                if (!ILJPG_DECODE_GET_BYTE (stream, markerValue, error)) { \
                    if (error == ILJPG_ERROR_DECODE_EOD) {                 \
                        markerValue = 1;                                   \
                        error = 0;                                         \
                        }                                                  \
                    else goto out_mainloop;                                \
                    }                                                      \
                if ((markerValue & ~7) == ILJPGM_RST0)                     \
                    markerValue = 0;                                       \
                }                                                          \
            }


            /* clear output buffer */
        memset(mb, 0, BLOCK_COEFFS*sizeof(int));

            /*  Get bit buffer and count from private */
        pHuffPriv = (iljpgDehuffPtr)pPriv->pHuffPriv;
        bitbuf = pHuffPriv->bitbuf;
        bitsleft = pHuffPriv->bitsleft;

            /*  If a reset was just done, eat the next marker if present and get
                next byte (which can't be a marker or error!); set into bitbuf.
            */
        if (pHuffPriv->resetDone) {
            pHuffPriv->resetDone = 0;
            if (!ILJPG_DECODE_GET_BYTE (stream, ch, error))
                return error;                   /* must be more bytes after restart */
            if (ch == 0xff) {                   /* marker or 0xff,0 = 0xff */
                if (!ILJPG_DECODE_GET_BYTE (stream, ch, error))
                    return error;
                if (ch == 0)
                    ch = 0xff;                  /* ff,0 => ff in data stream */
                else if ((ch & ~7) == ILJPGM_RST0) {
                        /*  Restart marker; get another byte, or two if ff */
                    if (!ILJPG_DECODE_GET_BYTE (stream, ch, error))
                        return error;
                    if (ch == 0xff) {
                        if (!ILJPG_DECODE_GET_BYTE (stream, ch, error))
                            return error;
                        if (ch == 0)
                            ch = 0xff;          /* ff,0 => ff in data stream */
                        }
                    }
                else return ILJPG_ERROR_DECODE_DATA;    /* other marker; error */
                }
            bitbuf = ( ch << SHIFTBUF );
            bitsleft = 8;                       /* one byte; bitsleft 0 after restart */
            }

            /*  Point to DC tables for this component ("comp"). */
        pTableSet = &pHuffPriv->table_set_dc[pPriv->pData->comp[comp].DCTableIndex];
        lookup_symb = pTableSet->lookup_symb;
        lookup_len = pTableSet->lookup_len;
        tree_nodes = pTableSet->tree_nodes;

        while ( coeff_ct < BLOCK_COEFFS ) {

                while ( bitsleft < LOOKUP_BITS ) {
                        DECODE_GET_BYTE (ch)
                        ch <<= (SHIFTBUF - bitsleft);
                        bitbuf += ch;
                        bitsleft += 8;
                }

                index = bitbuf >>  ( BITBUFSIZE - LOOKUP_BITS );
                ch = *(lookup_symb + index);
                len = *(lookup_len + index);

                if ( len != NOLEN ) {  /* can get value from lookup table */
                        bitbuf <<= len;
                        bitsleft -= len;
                }
                else {   /* has to go down the tree */
                        bitbuf <<= LOOKUP_BITS;
                        bitsleft -= LOOKUP_BITS;
                        /* point to a node in the tree */
                        nodep = tree_nodes + ch;
                        while ( 1 ) {
                                if ( !bitsleft ) {
                                        DECODE_GET_BYTE (ch)
                                        bitbuf = ( ch << SHIFTBUF );
                                        bitsleft = 8;
                                }
                                if ( bitbuf & BUFMSB )  /* next bit is a one */
                                        nodep = nodep->left;
                                else
                                        nodep = nodep->right;
                                bitbuf <<= 1;
                                bitsleft --;

                                    /*  if nodep is null, error in data */
                                if (!nodep)
                                    return ILJPG_ERROR_DECODE_DATA;
                                if ( (ch = nodep->c) != NOLEAF ) {
                                        break;
                                }
                        }
                }

                if ( ! is_dc ) {
                /* Handling of AC coefficients */

                /* at this point, ch contains RRRRSSSS, where
                   RRRR = length of a run of zero coeffs.
                   SSSS = number of the "bin" containing the next coeff.
                */

                /* first handle RRRR: */
                    zrun = ch >> 4;

                    if ( zrun ) {
                        WRITE_ZRUN(zrun);
                        coeff_ct += zrun;
                        ch &= 0x0f;
                    } 

                }
                else { /* we've done a DC case: set pointers for next
                          coefficient to AC tables.                  */
                        pTableSet = &pHuffPriv->table_set_ac
                                        [pPriv->pData->comp[comp].ACTableIndex];
                        lookup_symb = pTableSet->lookup_symb;
                        lookup_len = pTableSet->lookup_len;
                        tree_nodes = pTableSet->tree_nodes;
                }

                /* rest of AC case is identical to DC case */

                /* at this point, ch contains the "bin" number:
                   read that number of bits from the input stream */    
                if ( ch ) {
                    /* SSSS is not zero */
                    while ( bitsleft < ch ) {
                            DECODE_GET_BYTE (c)
                            bitbuf = bitbuf + 
                                     ( c << (SHIFTBUF - bitsleft) );
                            bitsleft += 8;
                    }

                    /* the desired number is now at the MS part of bitbuf */
                        
                    /* NOTE: Following code should be broken into a big 
                       SWITCH statement, with one case per value of ch
                       between 1 and 11 */

                    /* shift to lower part */        
                    coeff = bitbuf >> (BITBUFSIZE - ch); 
                    if ( (bitbuf & BUFMSB) == 0  )  /* leading bit is a zero */
                            coeff -= (( 1L << ch ) - 1 );
                    bitbuf <<= ch;                       
                    bitsleft -= ch;                      
                    
                    WRITE_COEFF(coeff);    /* write a nonzero coefficient */

                    /* check if the nonzero coefficient is inside the 
                       upper left 4x4 sub-block */
                    if ( is_fourx4 )
                        is_fourx4 &= fourx4_table[ coeff_ct ];

                    coeff_ct ++;
                }
                else {
                        /* SSSS is zero */
                        if ( !is_dc && zrun == 0 ) {
                        /* we got RRRRSSSS = 00000000, pad rest of the
                                           block with zeroes */

                                delta = BLOCK_COEFFS-coeff_ct;
                                WRITE_ZRUN(delta);
                                coeff_ct  += delta;
                                is_dconly = ( delta == BLOCK_COEFFS-1);
                                        
                        }
                        else {
                            /* write an isolated zero coefficient */
                            WRITE_ZRUN(1);
                            coeff_ct ++;
                        }
                }
        }

    /*  Goto point for exiting: "error" must contain current error code */
out_mainloop:
            /*  Store bit buffer and count into private */
        pHuffPriv->bitbuf = bitbuf;
        pHuffPriv->bitsleft = bitsleft;

        if ( is_dconly )
             *pBlockType = HUFF_DC_ONLY;
        else if ( is_fourx4 )
             *pBlockType = HUFF_FOURX4;
        else *pBlockType = HUFF_FULL;
        return error;
}




