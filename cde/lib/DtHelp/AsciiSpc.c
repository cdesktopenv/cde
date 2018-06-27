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
/* $TOG: AsciiSpc.c /main/6 1999/02/24 12:04:30 mgreess $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:	 AsciiSpc.c
 **
 **   Project:   Cde Help Library
 **
 **   Description: This file resolves an <spc> spec into an ascii 
 **		   representation of the special character.
 **
 **  (c) Copyright 1987, 1988, 1989, 1990, 1991, 1992 Hewlett-Packard Company
 **
 **  (c) Copyright 1993, 1994 Hewlett-Packard Company
 **  (c) Copyright 1993, 1994 International Business Machines Corp.
 **  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 **  (c) Copyright 1993, 1994 Novell, Inc.
 ****************************************************************************
 ************************************<+>*************************************/

/*
 * system includes
 */
#include <string.h>

/*
 * private includes
 */
#include "AsciiSpcI.h"

#ifdef NLS16
#endif

/********    Private Function Declarations    ********/

/********    End Public Function Declarations    ********/

/******************************************************************************
 *
 * Private variables and defines.
 *
 *****************************************************************************/
typedef	struct {
	const char	*symbol;
	const char	*spc_string;
} SpecialSymbolTable;


static	SpecialSymbolTable SpcTable[] =
{ 
/*
 * comment out the entries that map to a blank.
 * The last entry will catch them and map them to a blank
 * anyway. This will shorten the table by quite a bit.
 */
      /*{"[aacute]", " "         },  *ISOlat1, small a, acute accent */
      /*{"[acirc ]", " "         },  *ISOlat1, small a, circumflex   */
      /*{"[aelig ]", " "         },  *ISOlat1, small ae diphthong    */
      /*{"[agrave]", " "         },  *ISOlat1, small a, grave accent */
      /*{"[alpha ]", " "         },  *ISOgrk3, Greek, small alpha    */
      /*{"[aleph ]", " "         },  *ISOtech, aleph, Hebrews        */
        {"[amp   ]", "&"         }, /*ISOnum , ampersand             */
        {"[and   ]", "^"         }, /*ISOtech, wedge, logical and    */ 
      /*{"[ang   ]", " "         },  *ISOamso, angle                 */
      /*{"[angst ]", " "         },  *ISOtech, Angstrom, cap A, ring */
        {"[ap    ]", "~="        }, /*ISOtech, approximately equal   */ 
        {"[apos  ]", "'"         }, /*ISOnum , apostrophe            */
      /*{"[aring ]", " "         },  *ISOlat1, small a, ring         */
        {"[ast   ]", "*"         }, /*ISOnum , asterisk              */
      /*{"[atilde]", " "         },  *ISOlat1, small a, tilde        */
      /*{"[auml  ]", " "         },  *ISOlat1, small a, umlaut mark  */
      /*{"[beta  ]", " "         },  *ISOgrk3, Greek, small beta     */
      /*{"[blank ]", " "         },  *ISOpub , significant blank     */
      /*{"[bottom]", " "         },  *ISOtech, bottom symbol         */
        {"[bsol  ]", "\\"        }, /*ISOnum , backslash, rev solidus*/
        {"[bull  ]", "*"         }, /*ISOpub , round bullet,filled   */ 
      /*{"[brvbar]", "|"         },  *ISOnum , broken vertical bar   */
      /*{"[cap   ]", " "         },  *ISOtech, intersection          */
      /*{"[ccedil]", " "         },  *ISOlat1, small c, cedilla      */
      /*{"[cent  ]", " "         },  *ISOnum , cent sign             */
      /*{"[chi   ]", " "         },  *ISOgrk3, Greek, small chi      */
      /*{"[clubs ]", " "         },  *ISOpub , clubs suit symbol     */
        {"[colon ]", ":"         }, /*ISOnum , colon                 */
        {"[comma ]", ","         }, /*ISOnum , comma                 */
        {"[commat]", "@"         }, /*ISOnum , commercial at         */
      /*{"[cong  ]", " "         },  *ISOtech, congruent with        */
      /*{"[copy  ]", " "         },  *ISOnum , copyright symbol      */
        {"[cup   ]", "U"         }, /*ISOtech, union or logical sum  */ 
      /*{"[curren]", " "         },  *ISOnum , general currency sign */
      /*{"[darr  ]", " "         },  *ISOnum , downward arrow        */
      /*{"[dArr  ]", " "         },  *ISOamsa, down double arrow     */
        {"[dash  ]", "-"         }, /*ISOpub , dash                  */
      /*{"[deg   ]", " "         },  *ISOnum , degree sign           */
      /*{"[delta ]", " "         },  *ISOgrk3, Greek, small delta    */
      /*{"[diams ]", " "         },  *ISOpub , diamond suit symbol   */
        {"[divide]", "/"         }, /*ISOnum , divide sign           */ 
        {"[dollar]", "$"         }, /*ISOnum , dollar sign           */
      /*{"[eacute]", " "         },  *ISOlat1, small e, acute accent */
      /*{"[ecirc ]", " "         },  *ISOlat1, small e, circumflex   */
      /*{"[egrave]", " "         },  *ISOlat1, small e, grave accent */
      /*{"[empty ]", " "         },  *ISOamso, empty string, o/slash */
      /*{"[epsiv ]", " "         },  *ISOgrk3, Greek,sm epsilon, var */
        {"[equals]", "="         }, /*ISOnum , equals sign           */
      /*{"[equiv ]", " "         },  *ISOtech, identical with        */
      /*{"[eta   ]", " "         },  *ISOgrk3, Greek, small eta      */
      /*{"[eth   ]", " "         },  *ISOlat1, small eth, Icelandic  */
      /*{"[euml  ]", " "         },  *ISOlat1, small e, umlaut mark  */
        {"[excl  ]", "!"         }, /*ISOnum , exclamation mark      */
      /*{"[exist ]", " "         },  *ISOtech, at least one exists   */
      /*{"[forall]", " "         },  *ISOtech, for all               */
        {"[frac12]", "1/2"       }, /*ISOnum , fraction one-half     */ 
        {"[frac13]", "1/3"       }, /*ISOnum , fraction one-third    */ 
        {"[frac14]", "1/4"       }, /*ISOnum , fraction one-quarter  */ 
        {"[frac15]", "1/5"       }, /*ISOnum , fraction one-fifth    */ 
        {"[frac16]", "1/6"       }, /*ISOnum , fraction one-sixth    */ 
        {"[frac18]", "1/8"       }, /*ISOnum , fraction one-eight    */
        {"[frac23]", "2/3"       }, /*ISOnum , fraction two-thirds   */ 
        {"[frac25]", "2/5"       }, /*ISOnum , fraction two-fifths   */ 
        {"[frac34]", "3/4"       }, /*ISOnum , fraction three-quarter*/ 
        {"[frac35]", "3/5"       }, /*ISOnum , fraction three-fifths */ 
        {"[frac38]", "3/8"       }, /*ISOnum , fraction three-eights */
        {"[frac45]", "4/5"       }, /*ISOnum , fraction four-fifths  */ 
        {"[frac56]", "5/6"       }, /*ISOnum , fraction five-sixths  */ 
        {"[frac58]", "5/8"       }, /*ISOnum , fraction five-eights  */
        {"[frac78]", "7/8"       }, /*ISOnum , fraction seven-eights */
      /*{"[gamma ]", " "         },  *ISOgrk3, Greek, small gamma    */
        {"[ge    ]", ">="        }, /*ISOtech, greater-than-or-equal */ 
        {"[gt    ]", ">"         }, /*ISOnum , greater than sign     */
        {"[half  ]", "1/2"       }, /*ISOnum , fraction one-half     */ 
        {"[harr  ]", "<-->"      }, /*ISOamsa, left & right arrow    */ 
        {"[hArr  ]", "<<==>>"    }, /*ISOamsa, l & r double arrow    */ 
      /*{"[hearts]", " "         },  *ISOpub , hearts suit symbol    */
        {"[hellip]", "..."       }, /*ISOpub , ellipsis(horizontal)  */ 
        {"[horbar]", "--"        }, /*ISOnum , horizontal bar        */ 
        {"[hyphen]", "-"         }, /*ISOnum , hyphen                */
      /*{"[iacute]", " "         },  *ISOlat1, small i, acute accent */
      /*{"[icirc ]", " "         },  *ISOlat1, small i, circumflex   */
      /*{"[iexcl ]", " "         },  *ISOnum , inverted ! mark       */
      /*{"[igrave]", " "         },  *ISOlat1, small i, grave accent */
      /*{"[image ]", " "         },  *ISOamso,imaginary number symbol*/
      /*{"[infin ]", " "         },  *ISOtech, infinity              */
      /*{"[int   ]", " "         },  *ISOtech, intergral operator    */
      /*{"[iota  ]", " "         },  *ISOgrk3, Greek, small iota     */
      /*{"[iquest]", " "         },  *ISOnum , inverted ? mark       */
      /*{"[isin  ]", " "         },  *ISOtech, set membership        */
      /*{"[iuml  ]", " "         },  *ISOlat1, small i, umlaut mark  */
      /*{"[kappa ]", " "         },  *ISOgrk3, Greek, small kappa    */
      /*{"[lambda]", " "         },  *ISOgrk3, Greek, small lambda   */
      /*{"[lang  ]", " "         },  *ISOtech, left angle bracket    */
        {"[laquo ]", "<<"        }, /*ISOnum , left angle quotation  */ 
        {"[larr  ]", "<--"       }, /*ISOnum , leftward arrow        */ 
        {"[lArr  ]", "<=="       }, /*ISOtech, is implied by         */ 
        {"[lcub  ]", "{"         }, /*ISOnum , left curly brace      */
        {"[ldquo ]", "\""        }, /*ISOnum , left double quote     */ 
        {"[le    ]", "=<"        }, /*ISOtech, less-than-or-equal    */ 
        {"[lowbar]", "_"         }, /*ISOnum , low line              */
        {"[lpar  ]", "("         }, /*ISOnum , left parenthesis      */
        {"[lsqb  ]", "["         }, /*ISOnum , left square bracket   */
        {"[lsquo ]", "`"         }, /*ISOnum , left single quote     */ 
        {"[lsquor]", ","         }, /*ISOnum,rising single quote(low)*/ 
        {"[lt    ]", "<"         }, /*ISOnum , less-than sign        */
        {"[mdash ]", "--"        }, /*ISOpub , em dash(long dash)    */ 
      /*{"[micro ]", " "         },  *ISOnum , micro                 */
      /*{"[middot]", " "         },  *ISOnum , middle dot            */
        {"[minus ]", "-"         }, /*ISOtech, minus sign            */ 
        {"[mldr  ]", "...."      }, /*ISOpub , em leader             */
        {"[mnplus]", "-/+"       }, /*ISOtech, minus-or-plus sign    */
      /*{"[mu    ]", " "         },  *ISOgrk3, Greek, small mu       */
      /*{"[nabla ]", " "         },  *ISOtech, del, Hamilton operator*/
        {"[nbsp  ]", " "         }, /*ISOnum , no break space        */
        {"[ndash ]", "-"         }, /*ISOpub , en dash(short dash)   */ 
        {"[ne    ]", "!="        }, /*ISOtech, not equal             */ 
      /*{"[ni    ]", " "         },  *ISOtech, contains              */
        {"[nldr  ]", ".."        }, /*ISOpub , double baseline dot   */
      /*{"[not   ]", " "         },  *ISOnum , not                   */
      /*{"[notin ]", " "         },  *ISOtech, negated set membership*/
      /*{"[ntilde]", " "         },  *ISOlat1, small N, tilde        */
      /*{"[nu    ]", " "         },  *ISOgrk3, Greek, small nu       */
        {"[num   ]", "#"         }, /*ISOnum , number sign           */
      /*{"[oacute]", " "         },  *ISOlat1, small o, acute accent */
      /*{"[ocirc ]", " "         },  *ISOlat1, small o, circumflex   */
      /*{"[ograve]", " "         },  *ISOlat1, small o, grave accent */
      /*{"[ohm   ]", " "         },  *ISOnum , ohm                   */
      /*{"[omega ]", " "         },  *ISOgrk3, Greek, small omega    */
      /*{"[oplus ]", " "         },  *ISOamsb, plus sign in circle   */
        {"[or    ]", "V"         }, /*ISOtech, vee, logical or       */ 
      /*{"[ordf  ]", " "         },  *ISOnum , ordinal indicator, fem*/
      /*{"[ordm  ]", " "         },  *ISOnum , ordinal indicator,male*/
      /*{"[oslash]", " "         },  *ISOlat1, small o, slash        */
      /*{"[osol  ]", " "         },  *ISOamsb, slash in circle       */
      /*{"[otilde]", " "         },  *ISOlat1, small o, tilde        */
      /*{"[otimes]", " "         },  *ISOamsb,multiply sign in circle*/
      /*{"[ouml  ]", " "         },  *ISOlat1, small o, umlaut mark  */
      /*{"[over  ]", " "         },  *made up, over symbol           */
        {"[par   ]", "||"        }, /*ISOtech, parallel              */ 
      /*{"[para  ]", " "         },  *ISOnum , paragraph sign        */
      /*{"[part  ]", " "         },  *ISOtech, partial differential  */
        {"[percnt]", "%"         }, /*ISOnum , percent sign          */
        {"[period]", "."         }, /*ISOnum , full stop, period     */
      /*{"[perp  ]", " "         },  *ISOtech, perpendicular         */
      /*{"[phis  ]", " "         },  *ISOgrk3, Greek, small phi      */
      /*{"[pi    ]", " "         },  *ISOgrk3, Greek, small pi       */
      /*{"[piv   ]", " "         },  *ISOgrk3, Greek, small pi, var  */
        {"[plus  ]", "+"         }, /*ISOnum , plus sign             */
        {"[plusmn]", "+/-"       }, /*ISOnum , plus or minus sign    */ 
        {"[pound ]", "#"         }, /*ISOnum , pound sign            */ 
        {"[prime ]", "\'"        }, /*ISOtech, prime or minute       */ 
      /*{"[prop  ]", " "         },  *ISOtech, proportional to       */
      /*{"[psi   ]", " "         },  *ISOgrk3, Greek, small psi      */
        {"[quest ]", "?"         }, /*ISOnum , question mark         */
        {"[quot  ]", "'"         }, /*ISOnum , quote mark            */ 
      /*{"[radic ]", " "         },  *ISOtech, radical               */
      /*{"[rang  ]", " "         },  *ISOtech, right angle bracket   */
        {"[raquo ]", ">>"        }, /*ISOnum , right angle quotation */ 
        {"[rarr  ]", "-->"       }, /*ISOnum , rightward arrow       */ 
        {"[rArr  ]", "==>>"      }, /*ISOtech, right double arrow    */ 
        {"[rcub  ]", "}"         }, /*ISOnum , right curly brace     */
        {"[rdquo ]", "\""        }, /*ISOnum , right double quote    */
      /*{"[real  ]", " "         },  *ISOamso, real number symbol    */
      /*{"[reg   ]", " "         },  *ISOnum,circledR,registered sign*/
      /*{"[rho   ]", " "         },  *ISOgrk3, Greek, small rho      */
        {"[rpar  ]", ")"         }, /*ISOnum , right parenthesis     */
        {"[rsqb  ]", "]"         }, /*ISOnum , right square bracket  */
        {"[rsquo ]", "'"         }, /*ISOnum , right single quote    */
      /*{"[sect  ]", " "         },  *ISOnum , section sign          */
        {"[semi  ]", ";"         }, /*ISOnum , semicolon             */
        {"[shy   ]", "-"         }, /*ISOnum , soft hypen            */ 
      /*{"[sigma ]", " "         },  *ISOgrk3, Greek, small sigma    */
        {"[sim   ]", "~"         }, /*ISOtech, similar to            */ 
        {"[sime  ]", "~="        }, /*ISOtech, similar, equals       */ 
        {"[sol   ]", "/"         }, /*ISOnum , solidus               */
      /*{"[spades]", " "         },  *ISOpub , spades suit symbol    */
      /*{"[sub   ]", " "         },  *ISOtech, subset/is implied by  */
      /*{"[sung  ]", " "         },  *ISOnum , musical note(sung txt)*/
      /*{"[sube  ]", " "         },  *ISOtech, subset, equals        */
      /*{"[sup   ]", " "         },  *ISOtech, superset or implies   */
      /*{"[sup1  ]", " "         },  *ISOnum , superscript one       */
      /*{"[sup2  ]", " "         },  *ISOnum , superscript two       */
      /*{"[sup3  ]", " "         },  *ISOnum , superscript three     */
      /*{"[supe  ]", " "         },  *ISOtech, superset, equals      */
      /*{"[szlig ]", " "         },  *ISOlat1, small sharp s, German */
      /*{"[tau   ]", " "         },  *ISOgrk3, Greek, small tau      */
      /*{"[there4]", " "         },  *ISOtech, therefore             */
      /*{"[thetas]", " "         },  *ISOgrk3, Greek, small theta    */
      /*{"[thetav]", " "         },  *ISOgrk3, Greek, small theta,var*/
      /*{"[thorn ]", " "         },  *ISOlat1, small thorn, Icelandic*/
        {"[times ]", "x"         }, /*ISOnum , multipy sign          */ 
        {"[tprime]", "'''"       }, /*ISOtech, triple prime          */ 
      /*{"[trade ]", " "         },  *ISOnum , trade mark sign       */
      /*{"[uacute]", " "         },  *ISOlat1, small u, acute accent */
      /*{"[ucirc ]", " "         },  *ISOlat1, small u, circumflex   */
      /*{"[ugrave]", " "         },  *ISOlat1, small u, grave accent */
      /*{"[uarr  ]", " "         },  *ISOnum , upward arrow          */
      /*{"[uArr  ]", " "         },  *ISOamsa, up double arrow       */
      /*{"[upsi  ]", " "         },  *ISOgrk3, Greek, small upsilon  */
      /*{"[uuml  ]", " "         },  *ISOlat1, small u, umlaut mark  */
        {"[vellip]", ".\n.\n.\n" }, /*ISOpub , vertical ellipsis     */ 
        {"[verbar]", "|"         }, /*ISOnum , vertical bar          */ 
      /*{"[weierp]", " "         },  *ISOamso, Weierstrass p         */
      /*{"[xi    ]", " "         },  *ISOgrk3, Greek, small xi       */
      /*{"[yacute]", " "         },  *ISOlat1, small y, acute accent */
      /*{"[yen   ]", " "         },  *ISOnum , yen sign              */
      /*{"[yuml  ]", " "         },  *ISOlat1, small y, umlaut mark  */
      /*{"[zeta  ]", " "         },  *ISOgrk3, Greek, small zeta     */
      /*{"[Aacute]", " "         },  *ISOlat1, capital a,acute accent*/
      /*{"[Acirc ]", " "         },  *ISOlat1, capital a,circumflex  */
      /*{"[AElig ]", " "         },  *ISOlat1, capital ae diphthong  */
      /*{"[Agrave]", " "         },  *ISOlat1, capital a,grave accent*/
      /*{"[Aring ]", " "         },  *ISOlat1, capital a,ring        */
      /*{"[Atilde]", " "         },  *ISOlat1, capital a,tilde       */
      /*{"[Auml  ]", " "         },  *ISOlat1, capital a,umlaut mark */
      /*{"[Ccedil]", " "         },  *ISOlat1, capital c, cedilla    */
      /*{"[Delta ]", " "         },  *ISOgrk3, Greek, large delta    */
      /*{"[Dot   ]", " "         },  *ISOtech, dieresis or umlaut mrk*/
      /*{"[DotDot]", " "         },  *ISOtech, four dots above       */
      /*{"[Eacute]", " "         },  *ISOlat1, capital E,acute accent*/
      /*{"[Ecirc ]", " "         },  *ISOlat1, capital E,circumflex  */
      /*{"[Egrave]", " "         },  *ISOlat1, capital E,grave accent*/
      /*{"[ETH   ]", " "         },  *ISOlat1, capital Eth, Icelandic*/
      /*{"[Euml  ]", " "         },  *ISOlat1, capital E,umlaut mark */
      /*{"[Gamma ]", " "         },  *ISOgrk3, Greek, large gamma    */
      /*{"[Iacute]", " "         },  *ISOlat1, capital I,acute accent*/
      /*{"[Icirc ]", " "         },  *ISOlat1, capital I,circumflex  */
      /*{"[Igrave]", " "         },  *ISOlat1, capital I,grave accent*/
      /*{"[Iuml  ]", " "         },  *ISOlat1, capital I,umlaut mark */
      /*{"[Lambda]", " "         },  *ISOgrk3, Greek, large lambda   */
      /*{"[Ntilde]", " "         },  *ISOlat1, capital N, tilde      */
      /*{"[Oacute]", " "         },  *ISOlat1, capital O,acute accent*/
      /*{"[Ocirc ]", " "         },  *ISOlat1, capital O,circumflex  */
      /*{"[Ograve]", " "         },  *ISOlat1, capital O,grave accent*/
      /*{"[Omega ]", " "         },  *ISOgrk3, Greek, large omega    */
      /*{"[Oslash]", " "         },  *ISOlat1, capital O, slash      */
      /*{"[Otilde]", " "         },  *ISOlat1, capital O, tilde      */
      /*{"[Ouml  ]", " "         },  *ISOlat1, capital O,umlaut mark */
      /*{"[Pi    ]", " "         },  *ISOgrk3, Greek, large pi       */
        {"[Prime ]", "\""        }, /*ISOtech, double prime/second   */ 
      /*{"[Phi   ]", " "         },  *ISOgrk3, Greek, large phi      */
      /*{"[Psi   ]", " "         },  *ISOgrk3, Greek, large psi      */
      /*{"[Sigma ]", " "         },  *ISOgrk3, Greek, large sigma    */
      /*{"[Theta ]", " "         },  *ISOgrk3, Greek, large theta    */
      /*{"[THORN ]", " "         },  *ISOlat1,capital THORN,Icelandic*/
      /*{"[Uacute]", " "         },  *ISOgrk3, Greek, large theta    */
      /*{"[Ucirc ]", " "         },  *ISOlat1, capital U,acute accent*/
      /*{"[Ugrave]", " "         },  *ISOlat1, capital U,circumflex  */
      /*{"[Upsi  ]", " "         },  *ISOgrk3, Greek, large upsilon  */
      /*{"[Uuml  ]", " "         },  *ISOlat1, capital U,umlaut mark */
        {"[Verbar]", "||"        }, /*ISOtech, dbl vertical bar      */ 
      /*{"[Xi    ]", " "         },  *ISOgrk3, Greek, large xi       */
      /*{"[Yacute]", " "         },  *ISOlat1, capital Y,acute accent*/
        { NULL     , " "         }, /* default character to use      */
};

/******************************************************************************
 *
 * Semi Public functions
 *
 ******************************************************************************/
/*****************************************************************************
 * Function:	const char *_DtHelpCeResolveSpcToAscii ();
 *
 * Parameters:
 *
 * Returns:
 *
 * Purpose:
 *
 *****************************************************************************/
const char *
_DtHelpCeResolveSpcToAscii (
    const char	*spc_symbol)
{
    int          i = 0;

    if (NULL == spc_symbol)
      return NULL;

    while (NULL != SpcTable[i].symbol &&
	   0 != strcmp(SpcTable[i].symbol, spc_symbol))
      i++;

    if (SpcTable[i].symbol != NULL)
      return (SpcTable[i].spc_string);

    return NULL;
}
