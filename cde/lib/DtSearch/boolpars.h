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
/* $XConsortium: boolpars.h /main/1 1996/03/29 17:03:47 cde-ibm $
 *
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1996 Hewlett-Packard Company.
 * (c) Copyright 1996 International Business Machines Corp.
 * (c) Copyright 1996 Sun Microsystems, Inc.
 * (c) Copyright 1996 Novell, Inc. 
 * (c) Copyright 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 */
/*
 *   COMPONENT_NAME: austext
 *
 *   FUNCTIONS: none
 *
 *   ORIGINS: 27
 *
 *
 *   (C) COPYRIGHT International Business Machines Corp. 1996
 *   All Rights Reserved
 *   Licensed Materials - Property of IBM
 *   US Government Users Restricted Rights - Use, duplication or
 *   disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 */
#ifndef _boolpars_h
#define _boolpars_h
/******************** BOOLPARS.H ********************
 * $Id: boolpars.h /main/1 1996/03/29 17:03:47 cde-ibm $
 * February 1996.
 * Header for boolpars.c AusText/DtSearch yacc-based boolean query parser.
 *
 * FORMAT OF TRUTH TABLE:
 * Pointer to TRUTHTAB struct is main yacc stack value.
 * Represents 256 unsigned chars, every possible permutation of 8 (max)
 * query stems.  The unsigned chars ("permutes") are bit vectors.
 * Bit #0 (lowest order) is for first word in stemsarray stems[0],
 * bit #1 is stems[1], etc.
 * Example: truthtab for query consisting of a single word,
 * 	call it 'A' (assigned bit #0), is the 128 chars 1, 3, 5, 7, ...,
 * 	ie all bitvector permutations with low order bit turned on.
 * B:      (bit #1) is the 128 chars 2, 3, 6, 7, ..., ie all permutations
 * 	   with 2nd bit turned on.
 * A & B:  Intersection of truth tables: the 64 chars 3, 7, ...,
 * 	   all the bit vector permutations they have in common.
 * A | B:  Union of truth tables:  the 192 permutes 1,2,3, 5,6,7, 9,10,11...
 * ~B:     Complement of truth table, U - B: the 128 chars 0, 1, 4, 5, ...,
 * 	   ie every bitvector permutation NOT in B.
 * 
 * The zero permute (all bits zero) refers to the entire
 * balance of the database containing none of the query stems.
 * Needless to say searches with the zero permute in their
 * truth table will take an incredibly long time!
 *
 * The 'universal set' U is all 256 possible permutes.
 * The 'empty set' is indicated by truthtab.pmsz == 0;
 *
 * The permutes in a truth table are maintained in
 * ascending order for ease of comparisons.
 *
 * Truth table manipulations require and depend on
 * DtSrMAX_STEMCOUNT being defined only as 8.
 *
 * Newly allocated truth tables are maintained on the 'ttlist'
 * using the 'next' field.  This is to ensure they are eventually
 * freed to prevent memory leaks.
 *
 * If 'stemno' >= 0 it points to the word in usrblk.stems to which
 * it applies.  Stemno == -1 means truth table is not directly
 * associated with a word in usrblk.stems.
 * 
 * $Log$
 * Revision 1.1  1996/03/05  15:52:06  miker
 * Initial revision
 *
 */

#define COLLOC_STEM_FORMAT	"@%02d%02d%d"

/****************************************/
/*					*/
/*		TRUTHTAB		*/
/*					*/
/****************************************/
/* Truth table returned from boolean_parse() */
typedef struct tttag {
    struct tttag   *next;
    int		   stemno;	/* -1 means not assoc with stems[] array */
    int		   pmsz;	/* number of bytes in permutes array */
    unsigned char  *permutes;	/* array of bit vector permutations */
    }	TRUTHTAB;

/****************************************/
/*					*/
/*		 Globals		*/
/*					*/
/****************************************/
extern int	qry_has_no_NOTs;
extern int	qry_is_all_ANDs;
extern int	parser_invalid_wordcount;
extern TRUTHTAB	final_truthtab;

/****************************************/
/*					*/
/*	    Function Prototypes		*/
/*					*/
/****************************************/
extern void	add_syntax_errmsg (int msgno);
extern TRUTHTAB	*boolyac_AND (TRUTHTAB *tt1, TRUTHTAB *tt2);
extern TRUTHTAB	*boolyac_COLLOC (TRUTHTAB *tt1, int colval, TRUTHTAB *tt2);
extern TRUTHTAB	*boolyac_NOT (TRUTHTAB *tt1);
extern TRUTHTAB	*boolyac_OR (TRUTHTAB *tt1, TRUTHTAB *tt2);
extern TRUTHTAB *copy_final_truthtab (TRUTHTAB *tt);

/******************** BOOLPARS.H *********************/
#endif /* _boolpars_h */
