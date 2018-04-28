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
/******************** LANGMAP.C ********************
 * $XConsortium: langmap.c /main/3 1996/05/07 13:38:58 drk $
 * January 1996.
 * Contains European single byte character maps for
 * teskey parser and Paice/Porter stemmers.
 * Formerly coded in lang.c.
 *
 * Contains two code pages: ascii and iso-latin-1.
 * From F. N. Teskey (Principles of Text Processing, Elis Horwood Ltd,
 * 1982), a word is "a finite sequence of concordable and optionally
 * concordable characters delimited by either a nonconcordable character or an
 * optionally concordable character adjacent to a nonconcordable or optionally
 * concordable one."  Optionally concordable characters are symbols like
 * "./-" which may or may not occur within words (TCP/IP, file.dos, co-op).
 * Concordable characters are alphas and numbers.
 * Nonconcordables are whitespace, punctuation, control chars, etc.
 *
 * Lowest order byte = 'uppercase' of indexing char.
 * Next higher order byte = mask for teskey parse bits.
 * 
 * $Log$
 * Revision 1.3  1996/03/13  22:58:28  miker
 * Added prolog.
 *
 * Revision 1.2  1996/03/05  16:35:04  miker
 * Changed usage of WHITESPACE to replace isspace() function calls.
 *
 * Revision 1.1  1996/02/01  19:15:52  miker
 * Initial revision
 */
#include "SearchP.h"

/************************************************/
/*						*/
/*		 latin_charmap			*/
/*						*/
/************************************************/
/* Teskey character map for iso-latin-1 character set. */
int	latin_charmap [] = {
     NON_CONCORD | 		 0	/* NULL, Teskey EOT char */
    ,NON_CONCORD | 		 1	/* CTRL-A */
    ,NON_CONCORD | 		 2	/* CTRL-B */
    ,NON_CONCORD | 		 3	/* CTRL-C */
    ,NON_CONCORD | 		 4	/* CTRL-D */
    ,NON_CONCORD | 		 5	/* CTRL-E */
    ,NON_CONCORD | 		 6	/* CTRL-F */
    ,NON_CONCORD | 		 7	/* BEL \a */
    ,NON_CONCORD | 		 8	/* BS  \b */
    ,NON_CONCORD | WHITESPACE |	 9	/* TAB \t */
    ,NON_CONCORD | WHITESPACE |	10	/* LF  \n */ 
    ,NON_CONCORD | WHITESPACE |	11	/* VT  \v */
    ,NON_CONCORD | WHITESPACE |	12	/* FF  \f */
    ,NON_CONCORD | WHITESPACE |	13	/* CR  \r */
    ,NON_CONCORD | 		14	/* CTRL-N */
    ,NON_CONCORD | 		15	/* CTRL-O */
    ,NON_CONCORD | 		16	/* CTRL-P */
    ,NON_CONCORD | 		17	/* CTRL-Q */
    ,NON_CONCORD | 		18	/* CTRL-R */
    ,NON_CONCORD | 		19	/* CTRL-S */
    ,NON_CONCORD | 		20	/* CTRL-T */
    ,NON_CONCORD | 		21	/* CTRL-U */
    ,NON_CONCORD | 		22	/* CTRL-V */
    ,NON_CONCORD | 		23	/* CTRL-W */
    ,NON_CONCORD | 		24	/* CTRL-X */
    ,NON_CONCORD | 		25	/* CTRL-Y */
    ,NON_CONCORD | 		26	/* CTRL-Z (EOF) */
    ,NON_CONCORD | 		27	/* ESC */
    ,NON_CONCORD | 		28
    ,NON_CONCORD | 		29
    ,NON_CONCORD | 		30
    ,NON_CONCORD | 		31
    ,NON_CONCORD | WHITESPACE |	32	/* SPACE (blank) */
    ,NON_CONCORD |		33	/* ! */
    ,NON_CONCORD |		34	/* " */
    ,NON_CONCORD |		35	/* # */
    ,NON_CONCORD |		36	/* $ */
    ,NON_CONCORD |		37	/* % */
    ,NON_CONCORD |		38	/* & */
    ,NON_CONCORD |		39	/* ' (apostrophe) */
    ,NON_CONCORD |		40	/* ( */
    ,NON_CONCORD |		41	/* ) */
    ,NON_CONCORD |		42	/* * */
    ,NON_CONCORD |		43	/* + */
    ,NON_CONCORD |		44	/* , (comma) */

    ,OPT_CONCORD |		45	/* - (dash) */
    ,OPT_CONCORD |		46	/* . (period) */
    ,OPT_CONCORD |		47	/* / (slash) */

    ,CONCORDABLE | NUMERAL |	48	/* 0 */
    ,CONCORDABLE | NUMERAL |	49	/* 1 */
    ,CONCORDABLE | NUMERAL |	50	/* 2 */
    ,CONCORDABLE | NUMERAL |	51	/* 3 */
    ,CONCORDABLE | NUMERAL |	52	/* 4 */
    ,CONCORDABLE | NUMERAL |	53	/* 5 */
    ,CONCORDABLE | NUMERAL |	54	/* 6 */
    ,CONCORDABLE | NUMERAL |	55	/* 7 */
    ,CONCORDABLE | NUMERAL |	56	/* 8 */
    ,CONCORDABLE | NUMERAL |	57	/* 9 */

    ,NON_CONCORD |		58	/* : */
    ,NON_CONCORD |		59	/* ; */
    ,NON_CONCORD |		60	/* < */
    ,NON_CONCORD |		61	/* = */
    ,NON_CONCORD |		62	/* > */
    ,NON_CONCORD |		63	/* ? */
    ,NON_CONCORD |		64	/* @ */

    ,CONCORDABLE | VOWEL |	65	/* A */
    ,CONCORDABLE | CONSONANT |	66	/* B */
    ,CONCORDABLE | CONSONANT |	67	/* C */
    ,CONCORDABLE | CONSONANT |	68	/* D */
    ,CONCORDABLE | VOWEL |	69	/* E */
    ,CONCORDABLE | CONSONANT |	70	/* F */
    ,CONCORDABLE | CONSONANT |	71	/* G */
    ,CONCORDABLE | CONSONANT |	72	/* H */
    ,CONCORDABLE | VOWEL |	73	/* I */
    ,CONCORDABLE | CONSONANT |	74	/* J */
    ,CONCORDABLE | CONSONANT |	75	/* K */
    ,CONCORDABLE | CONSONANT |	76	/* L */
    ,CONCORDABLE | CONSONANT |	77	/* M */
    ,CONCORDABLE | CONSONANT |	78	/* N */
    ,CONCORDABLE | VOWEL |	79	/* O */
    ,CONCORDABLE | CONSONANT |	80	/* P */
    ,CONCORDABLE | CONSONANT |	81	/* Q */
    ,CONCORDABLE | CONSONANT |	82	/* R */
    ,CONCORDABLE | CONSONANT |	83	/* S */
    ,CONCORDABLE | CONSONANT |	84	/* T */
    ,CONCORDABLE | VOWEL |	85	/* U */
    ,CONCORDABLE | CONSONANT |	86	/* V */
    ,CONCORDABLE | CONSONANT |	87	/* W */
    ,CONCORDABLE | CONSONANT |	88	/* X */
    ,CONCORDABLE | CONSONANT |	89	/* Y */
    ,CONCORDABLE | CONSONANT |	90	/* Z */

    ,NON_CONCORD |		91	/* [ */
    ,NON_CONCORD |		92	/* \ (backslash) */
    ,NON_CONCORD |		93	/* ] */
    ,NON_CONCORD |		94	/* ^ */

    ,OPT_CONCORD |		95	/* _ (underscore) */

    ,NON_CONCORD |		96	/* ` (grave accent) */

    ,CONCORDABLE | VOWEL |	'A'	/*  97 = lowercase a */
    ,CONCORDABLE | CONSONANT |	'B'	/*  98 = lowercase b */
    ,CONCORDABLE | CONSONANT |	'C'	/*  99 = lowercase c */
    ,CONCORDABLE | CONSONANT |	'D'	/* 100 = lowercase d */
    ,CONCORDABLE | VOWEL |	'E'	/* 101 = lowercase e */
    ,CONCORDABLE | CONSONANT |	'F'	/* 102 = lowercase f */
    ,CONCORDABLE | CONSONANT |	'G'	/* 103 = lowercase g */
    ,CONCORDABLE | CONSONANT |	'H'	/* 104 = lowercase h */
    ,CONCORDABLE | VOWEL |	'I'	/* 105 = lowercase i */
    ,CONCORDABLE | CONSONANT |	'J'	/* 106 = lowercase j */
    ,CONCORDABLE | CONSONANT |	'K'	/* 107 = lowercase k */
    ,CONCORDABLE | CONSONANT |	'L'	/* 108 = lowercase l */
    ,CONCORDABLE | CONSONANT |	'M'	/* 109 = lowercase m */
    ,CONCORDABLE | CONSONANT |	'N'	/* 110 = lowercase n */
    ,CONCORDABLE | VOWEL |	'O'	/* 111 = lowercase o */
    ,CONCORDABLE | CONSONANT |	'P'	/* 112 = lowercase p */
    ,CONCORDABLE | CONSONANT |	'Q'	/* 113 = lowercase q */
    ,CONCORDABLE | CONSONANT |	'R'	/* 114 = lowercase r */
    ,CONCORDABLE | CONSONANT |	'S'	/* 115 = lowercase s */
    ,CONCORDABLE | CONSONANT |	'T'	/* 116 = lowercase t */
    ,CONCORDABLE | VOWEL |	'U'	/* 117 = lowercase u */
    ,CONCORDABLE | CONSONANT |	'V'	/* 118 = lowercase v */
    ,CONCORDABLE | CONSONANT |	'W'	/* 119 = lowercase w */
    ,CONCORDABLE | CONSONANT |	'X'	/* 120 = lowercase x */
    ,CONCORDABLE | CONSONANT |	'Y'	/* 121 = lowercase y */
    ,CONCORDABLE | CONSONANT |	'Z'	/* 122 = lowercase z */

    ,NON_CONCORD |		123	/* { */
    ,NON_CONCORD |		124	/* | (virgule, vertical line) */
    ,NON_CONCORD |		125	/* } */
    ,NON_CONCORD |		126	/* ~ */
    ,NON_CONCORD | 		127	/* DEL */

    ,NON_CONCORD |		128
    ,NON_CONCORD |		129
    ,NON_CONCORD |		130
    ,NON_CONCORD |		131
    ,NON_CONCORD |		132
    ,NON_CONCORD |		133
    ,NON_CONCORD |		134
    ,NON_CONCORD |		135

    ,NON_CONCORD |		136
    ,NON_CONCORD |		137
    ,NON_CONCORD |		138
    ,NON_CONCORD |		139
    ,NON_CONCORD |		140
    ,NON_CONCORD |		141
    ,NON_CONCORD |		142
    ,NON_CONCORD |		143

    ,NON_CONCORD |		144
    ,NON_CONCORD |		145
    ,NON_CONCORD |		146
    ,NON_CONCORD |		147
    ,NON_CONCORD |		148
    ,NON_CONCORD |		149
    ,NON_CONCORD |		150
    ,NON_CONCORD |		151

    ,NON_CONCORD |		152
    ,NON_CONCORD |		153
    ,NON_CONCORD |		154
    ,NON_CONCORD |		155
    ,NON_CONCORD |		156
    ,NON_CONCORD |		157
    ,NON_CONCORD |		158
    ,NON_CONCORD |		159

    ,CONCORDABLE |		160	/* RSP, 'Reqd SPace' (nonbreak?) */ 
    ,NON_CONCORD |		161	/* inverted exclamation point */
    ,NON_CONCORD |		162	/* cent sign, C slash */
    ,NON_CONCORD |		163	/* pounds sterling */
    ,NON_CONCORD |		164	/* generic international currency */
    ,NON_CONCORD |		165	/* yen */
    ,NON_CONCORD |		166	/* broken vertical line */
    ,NON_CONCORD |		167	/* section/paragraph symbol */

    ,NON_CONCORD |		168	/* diaeresis-umlaut */
    ,NON_CONCORD |		169	/* copyright symbol, c circle */
    ,NON_CONCORD |		170	/* 'a' small superscript, feminine */
    ,NON_CONCORD |		171	/* left angle quotes */
    ,NON_CONCORD |		172	/* logical not, eol */
    ,CONCORDABLE |		173	/* SHY, 'Syllable HYphen' (nonbreak?) */
    ,NON_CONCORD |		174	/* registered trademark, r circle */
    ,NON_CONCORD |		175	/* overline */

    ,NON_CONCORD |		176	/* degree symbol, xB0 */
    ,NON_CONCORD |		177	/* plus-minus symbol */
    ,NON_CONCORD |		178	/* squared, '2' superscript */
    ,NON_CONCORD |		179	/* cubed, '3' superscript */
    ,NON_CONCORD |		180	/* acute accent */
    ,NON_CONCORD |		181	/* micro symbol (greek mu) */
    ,NON_CONCORD |		182	/* paragraph symbol */
    ,NON_CONCORD |		183	/* middle dot */

    ,NON_CONCORD |		184	/* cedilla */
    ,NON_CONCORD |		185	/* '1' superscript */
    ,NON_CONCORD |		186	/* 'o' small superscript, masculine */
    ,NON_CONCORD |		187	/* right angle quotes */
    ,NON_CONCORD |		188	/* one fourth */
    ,NON_CONCORD |		189	/* one half */
    ,NON_CONCORD |		190	/* three fourths */
    ,NON_CONCORD |		191	/* inverted question mark */

    ,CONCORDABLE | VOWEL |	192	/* uppercase 'A' grave accent, xC0 */
    ,CONCORDABLE | VOWEL |	193	/* uppercase 'A' acute accent */
    ,CONCORDABLE | VOWEL |	194	/* uppercase 'A' circumflex */
    ,CONCORDABLE | VOWEL |	195	/* uppercase 'A' tilde */
    ,CONCORDABLE | VOWEL |	196	/* uppercase 'A' diaeresis-umlaut */
    ,CONCORDABLE | VOWEL |	197	/* uppercase 'A' overcircle */
    ,CONCORDABLE | VOWEL |	198	/* uppercase 'AE' diphthong */
    ,CONCORDABLE | CONSONANT |	199	/* uppercase 'C' cedilla */

    ,CONCORDABLE | VOWEL |	200	/* uppercase 'E' grave accent */
    ,CONCORDABLE | VOWEL |	201	/* uppercase 'E' acute accent */
    ,CONCORDABLE | VOWEL |	202	/* uppercase 'E' circumflex */
    ,CONCORDABLE | VOWEL |	203	/* uppercase 'E' diaeresis-umlaut */
    ,CONCORDABLE | VOWEL |	204	/* uppercase 'I' grave accent */
    ,CONCORDABLE | VOWEL |	205	/* uppercase 'I' acute accent */
    ,CONCORDABLE | VOWEL |	206	/* uppercase 'I' circumflex */
    ,CONCORDABLE | VOWEL |	207	/* uppercase 'I' diaeresis-umlaut */

    ,CONCORDABLE | CONSONANT |	208	/* uppercase 'D' stroke (eth), xD0 */
    ,CONCORDABLE | CONSONANT |	209	/* uppercase 'N' tilde */
    ,CONCORDABLE | VOWEL |	210	/* uppercase 'O' grave accent */
    ,CONCORDABLE | VOWEL |	211	/* uppercase 'O' acute accent */
    ,CONCORDABLE | VOWEL |	212	/* uppercase 'O' circumflex */
    ,CONCORDABLE | VOWEL |	213	/* uppercase 'O' tilde */
    ,CONCORDABLE | VOWEL |	214	/* uppercase 'O' diaeresis-umlaut */
    ,NON_CONCORD |		215	/* multiply sign 'x' */

    ,CONCORDABLE | VOWEL |	216	/* uppercase 'O' slash */
    ,CONCORDABLE | VOWEL |	217	/* uppercase 'U' grave accent */
    ,CONCORDABLE | VOWEL |	218	/* uppercase 'U' acute accent */
    ,CONCORDABLE | VOWEL |	219	/* uppercase 'U' circumflex */
    ,CONCORDABLE | VOWEL |	220	/* uppercase 'U' diaeresis-umlaut */
    ,CONCORDABLE | VOWEL |	221	/* uppercase 'Y' acute accent */
    ,CONCORDABLE | CONSONANT |	222	/* uppercase Thorn (Iceland) */
    ,CONCORDABLE | CONSONANT |	223	/* lowercase s sharp (German ss) */

    ,CONCORDABLE | VOWEL |	192	/* 224, lcase 'a' grave accent, xE0 */
    ,CONCORDABLE | VOWEL |	193	/* lowercase 'a' acute accent */
    ,CONCORDABLE | VOWEL |	194	/* lowercase 'a' circumflex */
    ,CONCORDABLE | VOWEL |	195	/* lowercase 'a' tilde */
    ,CONCORDABLE | VOWEL |	196	/* lowercase 'a' diaeresis-umlaut */
    ,CONCORDABLE | VOWEL |	197	/* lowercase 'a' overcircle */
    ,CONCORDABLE | VOWEL |	198	/* lowercase 'ae' diphthong */
    ,CONCORDABLE | CONSONANT |	199	/* lowercase 'c' cedilla */

    ,CONCORDABLE | VOWEL |	200	/* 232, lowercase 'e' grave accent */
    ,CONCORDABLE | VOWEL |	201	/* lowercase 'e' acute accent */
    ,CONCORDABLE | VOWEL |	202	/* lowercase 'e' circumflex */
    ,CONCORDABLE | VOWEL |	203	/* lowercase 'e' diaeresis-umlaut */
    ,CONCORDABLE | VOWEL |	204	/* lowercase 'i' grave accent */
    ,CONCORDABLE | VOWEL |	205	/* lowercase 'i' acute accent */
    ,CONCORDABLE | VOWEL |	206	/* lowercase 'i' circumflex */
    ,CONCORDABLE | VOWEL |	207	/* lowercase 'i' diaeresis-umlaut */

    ,CONCORDABLE | CONSONANT |	208	/* 240, lcase 'd' stroke (eth), xF0 */
    ,CONCORDABLE | CONSONANT |	209	/* lowercase 'n' tilde */
    ,CONCORDABLE | VOWEL |	210	/* lowercase 'o' grave accent */
    ,CONCORDABLE | VOWEL |	211	/* lowercase 'o' acute accent */
    ,CONCORDABLE | VOWEL |	212	/* lowercase 'o' circumflex */
    ,CONCORDABLE | VOWEL |	213	/* lowercase 'o' tilde */
    ,CONCORDABLE | VOWEL |	214	/* lowercase 'o' diaeresis-umlaut */
    ,NON_CONCORD |		247	/* divide sign */

    ,CONCORDABLE | VOWEL |	216	/* 248, lowercase 'o' slash */
    ,CONCORDABLE | VOWEL |	217	/* lowercase 'u' grave accent */
    ,CONCORDABLE | VOWEL |	218	/* lowercase 'u' acute accent */
    ,CONCORDABLE | VOWEL |	219	/* lowercase 'u' circumflex */
    ,CONCORDABLE | VOWEL |	220	/* lowercase 'u' diaeresis-umlaut */
    ,CONCORDABLE | VOWEL |	221	/* lowercase 'y' acute accent */
    ,CONCORDABLE | CONSONANT |	222	/* lowercase thorn (Icelandic) */
    ,CONCORDABLE | CONSONANT |	255	/* lowercase 'y' diaeresis-umlaut */

    ,NON_CONCORD			/* 256, an extra one... */
    }; /* latin_charmap[] */


/************************************************/
/*						*/
/*		 ascii_charmap			*/
/*						*/
/************************************************/
/* Teskey character map for 7-bit ascii
 * character set in 8-bit octets.
 * Also accessed in packed EUC parsing in jpn.c.
 */
int	ascii_charmap [] = {
     NON_CONCORD |		 0	/* NULL, Teskey EOT char */
    ,NON_CONCORD |		 1	/* CTRL-A */
    ,NON_CONCORD |		 2	/* CTRL-B */
    ,NON_CONCORD |		 3	/* CTRL-C */
    ,NON_CONCORD |		 4	/* CTRL-D */
    ,NON_CONCORD |		 5	/* CTRL-E */
    ,NON_CONCORD |		 6	/* CTRL-F */
    ,NON_CONCORD |		 7	/* BEL \a */
    ,NON_CONCORD |		 8	/* BS  \b */
    ,NON_CONCORD | WHITESPACE |	 9	/* TAB \t */
    ,NON_CONCORD | WHITESPACE |	10	/* LF  \n */ 
    ,NON_CONCORD | WHITESPACE |	11	/* VT  \v */
    ,NON_CONCORD | WHITESPACE |	12	/* FF  \f */
    ,NON_CONCORD | WHITESPACE |	13	/* CR  \r */
    ,NON_CONCORD |		14	/* CTRL-N */
    ,NON_CONCORD |		15	/* CTRL-O */
    ,NON_CONCORD |		16	/* CTRL-P */
    ,NON_CONCORD |		17	/* CTRL-Q */
    ,NON_CONCORD |		18	/* CTRL-R */
    ,NON_CONCORD |		19	/* CTRL-S */
    ,NON_CONCORD |		20	/* CTRL-T */
    ,NON_CONCORD |		21	/* CTRL-U */
    ,NON_CONCORD |		22	/* CTRL-V */
    ,NON_CONCORD |		23	/* CTRL-W */
    ,NON_CONCORD |		24	/* CTRL-X */
    ,NON_CONCORD |		25	/* CTRL-Y */
    ,NON_CONCORD |		26	/* CTRL-Z (EOF) */
    ,NON_CONCORD |		27	/* ESC */
    ,NON_CONCORD |		28
    ,NON_CONCORD |		29
    ,NON_CONCORD |		30
    ,NON_CONCORD |		31
    ,NON_CONCORD | WHITESPACE |	32	/* SPACE (blank) */
    ,NON_CONCORD |		33	/* ! */
    ,NON_CONCORD |		34	/* " */
    ,NON_CONCORD |		35	/* # */
    ,NON_CONCORD |		36	/* $ */
    ,NON_CONCORD |		37	/* % */
    ,NON_CONCORD |		38	/* & */
    ,NON_CONCORD |		39	/* ' (apostrophe) */
    ,NON_CONCORD |		40	/* ( */
    ,NON_CONCORD |		41	/* ) */
    ,NON_CONCORD |		42	/* * */
    ,NON_CONCORD |		43	/* + */
    ,NON_CONCORD |		44	/* , (comma) */

    ,OPT_CONCORD |		45	/* - (dash) */
    ,OPT_CONCORD |		46	/* . (period) */
    ,OPT_CONCORD |		47	/* / (slash) */

    ,CONCORDABLE | NUMERAL |	48	/* 0 */
    ,CONCORDABLE | NUMERAL |	49	/* 1 */
    ,CONCORDABLE | NUMERAL |	50	/* 2 */
    ,CONCORDABLE | NUMERAL |	51	/* 3 */
    ,CONCORDABLE | NUMERAL |	52	/* 4 */
    ,CONCORDABLE | NUMERAL |	53	/* 5 */
    ,CONCORDABLE | NUMERAL |	54	/* 6 */
    ,CONCORDABLE | NUMERAL |	55	/* 7 */
    ,CONCORDABLE | NUMERAL |	56	/* 8 */
    ,CONCORDABLE | NUMERAL |	57	/* 9 */

    ,NON_CONCORD |		58	/* : */
    ,NON_CONCORD |		59	/* ; */
    ,NON_CONCORD |		60	/* < */
    ,NON_CONCORD |		61	/* = */
    ,NON_CONCORD |		62	/* > */
    ,NON_CONCORD |		63	/* ? */
    ,NON_CONCORD |		64	/* @ */

    ,CONCORDABLE | VOWEL |	65	/* A */
    ,CONCORDABLE | CONSONANT |	66	/* B */
    ,CONCORDABLE | CONSONANT |	67	/* C */
    ,CONCORDABLE | CONSONANT |	68	/* D */
    ,CONCORDABLE | VOWEL |	69	/* E */
    ,CONCORDABLE | CONSONANT |	70	/* F */
    ,CONCORDABLE | CONSONANT |	71	/* G */
    ,CONCORDABLE | CONSONANT |	72	/* H */
    ,CONCORDABLE | VOWEL |	73	/* I */
    ,CONCORDABLE | CONSONANT |	74	/* J */
    ,CONCORDABLE | CONSONANT |	75	/* K */
    ,CONCORDABLE | CONSONANT |	76	/* L */
    ,CONCORDABLE | CONSONANT |	77	/* M */
    ,CONCORDABLE | CONSONANT |	78	/* N */
    ,CONCORDABLE | VOWEL |	79	/* O */
    ,CONCORDABLE | CONSONANT |	80	/* P */
    ,CONCORDABLE | CONSONANT |	81	/* Q */
    ,CONCORDABLE | CONSONANT |	82	/* R */
    ,CONCORDABLE | CONSONANT |	83	/* S */
    ,CONCORDABLE | CONSONANT |	84	/* T */
    ,CONCORDABLE | VOWEL |	85	/* U */
    ,CONCORDABLE | CONSONANT |	86	/* V */
    ,CONCORDABLE | CONSONANT |	87	/* W */
    ,CONCORDABLE | CONSONANT |	88	/* X */
    ,CONCORDABLE | CONSONANT |	89	/* Y */
    ,CONCORDABLE | CONSONANT |	90	/* Z */

    ,NON_CONCORD |		91	/* [ */
    ,NON_CONCORD |		92	/* \ (backslash) */
    ,NON_CONCORD |		93	/* ] */
    ,NON_CONCORD |		94	/* ^ */

    ,OPT_CONCORD |		95	/* _ (underscore) */

    ,NON_CONCORD |		96	/* ` (grave accent) */

    ,CONCORDABLE | VOWEL |	'A'	/*  97 = lowercase a */
    ,CONCORDABLE | CONSONANT |	'B'	/*  98 = lowercase b */
    ,CONCORDABLE | CONSONANT |	'C'	/*  99 = lowercase c */
    ,CONCORDABLE | CONSONANT |	'D'	/* 100 = lowercase d */
    ,CONCORDABLE | VOWEL |	'E'	/* 101 = lowercase e */
    ,CONCORDABLE | CONSONANT |	'F'	/* 102 = lowercase f */
    ,CONCORDABLE | CONSONANT |	'G'	/* 103 = lowercase g */
    ,CONCORDABLE | CONSONANT |	'H'	/* 104 = lowercase h */
    ,CONCORDABLE | VOWEL |	'I'	/* 105 = lowercase i */
    ,CONCORDABLE | CONSONANT |	'J'	/* 106 = lowercase j */
    ,CONCORDABLE | CONSONANT |	'K'	/* 107 = lowercase k */
    ,CONCORDABLE | CONSONANT |	'L'	/* 108 = lowercase l */
    ,CONCORDABLE | CONSONANT |	'M'	/* 109 = lowercase m */
    ,CONCORDABLE | CONSONANT |	'N'	/* 110 = lowercase n */
    ,CONCORDABLE | VOWEL |	'O'	/* 111 = lowercase o */
    ,CONCORDABLE | CONSONANT |	'P'	/* 112 = lowercase p */
    ,CONCORDABLE | CONSONANT |	'Q'	/* 113 = lowercase q */
    ,CONCORDABLE | CONSONANT |	'R'	/* 114 = lowercase r */
    ,CONCORDABLE | CONSONANT |	'S'	/* 115 = lowercase s */
    ,CONCORDABLE | CONSONANT |	'T'	/* 116 = lowercase t */
    ,CONCORDABLE | VOWEL |	'U'	/* 117 = lowercase u */
    ,CONCORDABLE | CONSONANT |	'V'	/* 118 = lowercase v */
    ,CONCORDABLE | CONSONANT |	'W'	/* 119 = lowercase w */
    ,CONCORDABLE | CONSONANT |	'X'	/* 120 = lowercase x */
    ,CONCORDABLE | CONSONANT |	'Y'	/* 121 = lowercase y */
    ,CONCORDABLE | CONSONANT |	'Z'	/* 122 = lowercase z */

    ,NON_CONCORD |		123	/* { */
    ,NON_CONCORD |		124	/* | (virgule, vertical line) */
    ,NON_CONCORD |		125	/* } */
    ,NON_CONCORD |		126	/* ~ */
    ,NON_CONCORD |		127	/* DEL */

    ,NON_CONCORD |		128
    ,NON_CONCORD |		129
    ,NON_CONCORD |		130
    ,NON_CONCORD |		131
    ,NON_CONCORD |		132
    ,NON_CONCORD |		133
    ,NON_CONCORD |		134
    ,NON_CONCORD |		135

    ,NON_CONCORD |		136
    ,NON_CONCORD |		137
    ,NON_CONCORD |		138
    ,NON_CONCORD |		139
    ,NON_CONCORD |		140
    ,NON_CONCORD |		141
    ,NON_CONCORD |		142
    ,NON_CONCORD |		143

    ,NON_CONCORD |		144
    ,NON_CONCORD |		145
    ,NON_CONCORD |		146
    ,NON_CONCORD |		147
    ,NON_CONCORD |		148
    ,NON_CONCORD |		149
    ,NON_CONCORD |		150
    ,NON_CONCORD |		151

    ,NON_CONCORD |		152
    ,NON_CONCORD |		153
    ,NON_CONCORD |		154
    ,NON_CONCORD |		155
    ,NON_CONCORD |		156
    ,NON_CONCORD |		157
    ,NON_CONCORD |		158
    ,NON_CONCORD |		159

    ,NON_CONCORD |		160
    ,NON_CONCORD |		161
    ,NON_CONCORD |		162
    ,NON_CONCORD |		163
    ,NON_CONCORD |		164
    ,NON_CONCORD |		165
    ,NON_CONCORD |		166
    ,NON_CONCORD |		167

    ,NON_CONCORD |		168
    ,NON_CONCORD |		169
    ,NON_CONCORD |		170
    ,NON_CONCORD |		171
    ,NON_CONCORD |		172
    ,NON_CONCORD |		173
    ,NON_CONCORD |		174
    ,NON_CONCORD |		175

    ,NON_CONCORD |		176
    ,NON_CONCORD |		177
    ,NON_CONCORD |		178
    ,NON_CONCORD |		179
    ,NON_CONCORD |		180
    ,NON_CONCORD |		181
    ,NON_CONCORD |		182
    ,NON_CONCORD |		183

    ,NON_CONCORD |		184
    ,NON_CONCORD |		185
    ,NON_CONCORD |		186
    ,NON_CONCORD |		187
    ,NON_CONCORD |		188
    ,NON_CONCORD |		189
    ,NON_CONCORD |		190
    ,NON_CONCORD |		191

    ,NON_CONCORD |		192
    ,NON_CONCORD |		193
    ,NON_CONCORD |		194
    ,NON_CONCORD |		195
    ,NON_CONCORD |		196
    ,NON_CONCORD |		197
    ,NON_CONCORD |		198
    ,NON_CONCORD |		199

    ,NON_CONCORD |		200
    ,NON_CONCORD |		201
    ,NON_CONCORD |		202
    ,NON_CONCORD |		203
    ,NON_CONCORD |		204
    ,NON_CONCORD |		205
    ,NON_CONCORD |		206
    ,NON_CONCORD |		207

    ,NON_CONCORD |		208
    ,NON_CONCORD |		209
    ,NON_CONCORD |		210
    ,NON_CONCORD |		211
    ,NON_CONCORD |		212
    ,NON_CONCORD |		213
    ,NON_CONCORD |		214
    ,NON_CONCORD |		215

    ,NON_CONCORD |		216
    ,NON_CONCORD |		217
    ,NON_CONCORD |		218
    ,NON_CONCORD |		219
    ,NON_CONCORD |		220
    ,NON_CONCORD |		221
    ,NON_CONCORD |		222
    ,NON_CONCORD |		223

    ,NON_CONCORD |		224
    ,NON_CONCORD |		225
    ,NON_CONCORD |		226
    ,NON_CONCORD |		227
    ,NON_CONCORD |		228
    ,NON_CONCORD |		229
    ,NON_CONCORD |		230
    ,NON_CONCORD |		231

    ,NON_CONCORD |		232
    ,NON_CONCORD |		233
    ,NON_CONCORD |		234
    ,NON_CONCORD |		235
    ,NON_CONCORD |		236
    ,NON_CONCORD |		237
    ,NON_CONCORD |		238
    ,NON_CONCORD |		239

    ,NON_CONCORD |		240
    ,NON_CONCORD |		241
    ,NON_CONCORD |		242
    ,NON_CONCORD |		243
    ,NON_CONCORD |		244
    ,NON_CONCORD |		245
    ,NON_CONCORD |		246
    ,NON_CONCORD |		247

    ,NON_CONCORD |		248
    ,NON_CONCORD |		249
    ,NON_CONCORD |		250
    ,NON_CONCORD |		251
    ,NON_CONCORD |		252
    ,NON_CONCORD |		253
    ,NON_CONCORD |		254
    ,NON_CONCORD |		255

    ,NON_CONCORD		/* 256 = an extra one... */
    }; /* ascii_charmap[] */

/******************** LANGMAP.C ********************/

