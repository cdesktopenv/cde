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
/* $XConsortium: XInterface.c /main/37 1996/11/11 12:28:19 cde-hp $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:	   XInterface.c
 **
 **   Project:     Cde Help System
 **
 **   Description: This is the UI dependant layer to the Help System.
 **		   It recieves requests from the Core Engine for
 **		   information or to do rendering.  It does the
 **		   appropriate work based on a Motif and X Lib
 **		   interface.
 **
 **  (c) Copyright 1987, 1988, 1989, 1990, 1991, 1992 Hewlett-Packard Company
 **
 **  (c) Copyright 1993, 1994 Hewlett-Packard Company
 **  (c) Copyright 1993, 1994 International Business Machines Corp.
 **  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 **  (c) Copyright 1993, 1994 Novell, Inc.
 **
 ****************************************************************************
 ************************************<+>*************************************/

/*
 * system includes
 */
#include <stdlib.h>
#include <limits.h>
#include <errno.h>
#include <X11/Xlib.h>

#include <X11/Xos.h>
#ifdef X_NOT_STDC_ENV
extern int errno;
#endif

/*
 * Canvas Engine
 */
#include "CanvasP.h"
#include "CanvasSegP.h"

/*
 * private includes
 */
#include "AsciiSpcI.h"
#include "DisplayAreaP.h"
#include "HelpErrorP.h"
#include "FontAttrI.h"
#include "FontI.h"
#include "GraphicsI.h"
#include "RegionI.h"
#include "StringFuncsI.h"
#include "XInterfaceI.h"

#include <X11/bitmaps/root_weave>

#ifdef NLS16
#endif

/********    Private Function Declarations    ********/
static void	DADrawSpc (
			_DtCvPointer	client_data,
			_DtCvPointer	spc_handle,
			_DtCvUnit	x,
			_DtCvUnit	y,
			_DtCvUnit	box_x,
			_DtCvUnit	box_y,
			_DtCvUnit	box_height,
			int		link_type,
			_DtCvFlags	old_flags,
			_DtCvFlags	new_flags );
/********    End Private Function Declarations    ********/

/*****************************************************************************
 *		Private Defines
 *****************************************************************************/
#define	GROW_SIZE	3

/*****************************************************************************
 *		Private Macros
 *****************************************************************************/
#define IsHyperLink(x)		((x) & _DtCvLINK_FLAG)
#define IsHyperBegin(x)		((x) & _DtCvLINK_BEGIN)
#define IsHyperEnd(x)		((x) & _DtCvLINK_END)
#define IsPopUp(x)		((x) & _DtCvLINK_POP_UP)
#define IsNewWindow(x)		((x) & _DtCvLINK_NEW_WINDOW)

#define HasTraversal(x)		((x) & _DtCvTRAVERSAL_FLAG)
#define IsTraversalBegin(x)	((x) & _DtCvTRAVERSAL_BEGIN)
#define IsTraversalEnd(x)	((x) & _DtCvTRAVERSAL_END)

#define IsSelected(x)		((x) & _DtCvSELECTED_FLAG)

#define IsMarkOn(x)		(((x) & _DtCvMARK_FLAG) && ((x) & _DtCvMARK_ON))

#define	TocIsOn(x)		((x) & _DT_HELP_TOC_ON)
#define	TocIsOff(x)		(!((x) & _DT_HELP_TOC_ON))

#define	TurnTocOn(x)		(x) |= _DT_HELP_TOC_ON
#define	TurnTocOff(x)		(x) &= ~(_DT_HELP_TOC_ON)

#define IsEndOfLine(x)		((x) & _DtCvEND_OF_LINE)

/*****************************************************************************
 *		Private Variables
 *****************************************************************************/
static	const char	IsoString[]    = "ISO-8859-1";
static	const char	SymbolString[] = "DT-SYMBOL-1";
static	const char	DefaultStr[]   = "default_spc";

#define	MAX_STR_LEN	5

typedef	struct {
	const char	*symbol;
	const char	 spc_string[MAX_STR_LEN];
	const char	*spc_value;
} SpecialSymbolTable;


static	SpecialSymbolTable SpcTable[] =
{ 
 {"[aacute]", {'\341','\0'}, IsoString   }, /*ISOlat1, small a, acute accent */ 
 {"[acirc ]", {'\342','\0'}, IsoString   }, /*ISOlat1, small a, circumflex   */ 
 {"[aelig ]", {'\346','\0'}, IsoString   }, /*ISOlat1, small ae diphthong    */ 
 {"[agrave]", {'\340','\0'}, IsoString   }, /*ISOlat1, small a, grave accent */ 
 {"[alpha ]", {'\141','\0'}, SymbolString}, /*ISOgrk3, Greek, small alpha    */ 
 {"[aleph ]", {'\300','\0'}, SymbolString}, /*ISOtech, aleph, Hebrews        */ 
 {"[amp   ]", {  '&' ,'\0'}, IsoString   }, /*ISOnum , ampersand             */ 
 {"[and   ]", {'\331','\0'}, SymbolString}, /*ISOtech, wedge, logical and    */ 
 {"[ang   ]", {'\320','\0'}, SymbolString}, /*ISOamso, angle                 */ 
 {"[angst ]", {'\305','\0'}, IsoString   }, /*ISOtech, Angstrom, cap A, ring */ 
 {"[ap    ]", {'\273','\0'}, SymbolString}, /*ISOtech, approximately equal   */ 
 {"[apos  ]", { '\'' ,'\0'}, IsoString   }, /*ISOnum , apostrophe            */ 
 {"[aring ]", {'\345','\0'}, IsoString   }, /*ISOlat1, small a, ring         */ 
 {"[ast   ]", {  '*' ,'\0'}, IsoString   }, /*ISOnum , asterisk              */ 
 {"[atilde]", {'\343','\0'}, IsoString   }, /*ISOlat1, small a, tilde        */ 
 {"[auml  ]", {'\344','\0'}, IsoString   }, /*ISOlat1, small a, umlaut mark  */ 
 {"[beta  ]", {'\142','\0'}, SymbolString}, /*ISOgrk3, Greek, small beta     */ 
 {"[blank ]", {  ' ' ,'\0'}, IsoString   }, /*ISOpub , significant blank     */ 
 {"[bottom]", {'\136','\0'}, SymbolString}, /*ISOtech, bottom symbol         */ 
 {"[brvbar]", {'\246','\0'}, IsoString   }, /*ISOnum , broken vertical bar   */ 
 {"[bsol  ]", { '\\' ,'\0'}, IsoString   }, /*ISOnum , backslash, rev solidus*/ 
 {"[bull  ]", {'\267','\0'}, SymbolString}, /*ISOpub , round bullet,filled   */ 
 {"[cap   ]", {'\307','\0'}, SymbolString}, /*ISOtech, intersection          */ 
 {"[ccedil]", {'\347','\0'}, IsoString   }, /*ISOlat1, small c, cedilla      */ 
 {"[cent  ]", {'\242','\0'}, IsoString   }, /*ISOnum , cent sign             */ 
 {"[chi   ]", {'\143','\0'}, SymbolString}, /*ISOgrk3, Greek, small chi      */ 
 {"[clubs ]", {'\247','\0'}, SymbolString}, /*ISOpub , clubs suit symbol     */ 
 {"[colon ]", {  ':' ,'\0'}, IsoString   }, /*ISOnum , colon                 */ 
 {"[comma ]", {  ',' ,'\0'}, IsoString   }, /*ISOnum , comma                 */ 
 {"[commat]", {  '@' ,'\0'}, IsoString   }, /*ISOnum , commercial at         */ 
 {"[cong  ]", {'\100','\0'}, SymbolString}, /*ISOtech, congruent with        */ 
 {"[copy  ]", {'\323','\0'}, SymbolString}, /*ISOnum , copyright symbol      */ 
 {"[cup   ]", {'\310','\0'}, SymbolString}, /*ISOtech, union or logical sum  */ 
 {"[curren]", {'\244','\0'}, IsoString   }, /*ISOnum , general currency sign */ 
 {"[darr  ]", {'\257','\0'}, SymbolString}, /*ISOnum , downward arrow        */ 
 {"[dArr  ]", {'\337','\0'}, SymbolString}, /*ISOamsa, down double arrow     */ 
 {"[dash  ]", {  '-' ,'\0'}, IsoString   }, /*ISOpub , dash                  */ 
 {"[deg   ]", {'\260','\0'}, SymbolString}, /*ISOnum , degree sign           */ 
 {"[delta ]", {'\144','\0'}, SymbolString}, /*ISOgrk3, Greek, small delta    */ 
 {"[diams ]", {'\250','\0'}, SymbolString}, /*ISOpub , diamond suit symbol   */ 
 {"[divide]", {'\270','\0'}, SymbolString}, /*ISOnum , divide sign           */ 
 {"[dollar]", {  '$' ,'\0'}, IsoString   }, /*ISOnum , dollar sign           */ 
 {"[eacute]", {'\351','\0'}, IsoString   }, /*ISOlat1, small e, acute accent */ 
 {"[ecirc ]", {'\352','\0'}, IsoString   }, /*ISOlat1, small e, circumflex   */ 
 {"[egrave]", {'\350','\0'}, IsoString   }, /*ISOlat1, small e, grave accent */ 
 {"[empty ]", {'\370','\0'}, IsoString   }, /*ISOamso, empty string          */ 
 {"[epsiv ]", {'\145','\0'}, SymbolString}, /*ISOgrk3, Greek,sm epsilon, var */ 
 {"[equals]", {  '=' ,'\0'}, IsoString   }, /*ISOnum , equals sign           */ 
 {"[equiv ]", {'\272','\0'}, SymbolString}, /*ISOtech, identical with        */ 
 {"[eta   ]", {'\150','\0'}, SymbolString}, /*ISOgrk3, Greek, small eta      */ 
 {"[eth   ]", {'\360','\0'}, IsoString   }, /*ISOlat1, small eth, Icelandic  */ 
 {"[euml  ]", {'\353','\0'}, IsoString   }, /*ISOlat1, small e, umlaut mark  */ 
 {"[excl  ]", {  '!' ,'\0'}, IsoString   }, /*ISOnum , exclamation mark      */ 
 {"[exist ]", {'\044','\0'}, SymbolString}, /*ISOtech, at least one exists   */ 
 {"[forall]", {'\042','\0'}, SymbolString}, /*ISOtech, for all               */ 
 {"[frac12]", {'\275','\0'}, IsoString   }, /*ISOnum , fraction one-half     */ 
 {"[frac13]", {'1','/','3','\0'},
			     IsoString   }, /*ISOnum , fraction one-third    */ 
 {"[frac14]", {'\274','\0'}, IsoString   }, /*ISOnum , fraction one-quarter  */ 
 {"[frac15]", {'1','/','5','\0'},
			     IsoString   }, /*ISOnum , fraction one-fifth    */ 
 {"[frac16]", {'1','/','6','\0'},
			     IsoString   }, /*ISOnum , fraction one-sixth    */ 
 {"[frac18]", {'1','/','8','\0'},
			     IsoString   }, /*ISOnum , fraction one-eight    */ 
 {"[frac23]", {'2','/','3','\0'},
			     IsoString   }, /*ISOnum , fraction two-thirds   */ 
 {"[frac25]", {'2','/','5','\0'},
			     IsoString   }, /*ISOnum , fraction two-fifths   */ 
 {"[frac34]", {'\276','\0'}, IsoString   }, /*ISOnum , fraction three-quarter*/ 
 {"[frac35]", {'3','/','5','\0'},
			     IsoString   }, /*ISOnum , fraction three-fifths */ 
 {"[frac38]", {'3','/','8','\0'},
			     IsoString   }, /*ISOnum , fraction three-eights */ 
 {"[frac45]", {'4','/','5','\0'},
			     IsoString   }, /*ISOnum , fraction four-fifths  */ 
 {"[frac56]", {'5','/','6','\0'},
			     IsoString   }, /*ISOnum , fraction five-sixths  */ 
 {"[frac58]", {'5','/','8','\0'},
			     IsoString   }, /*ISOnum , fraction five-eights  */ 
 {"[frac78]", {'7','/','8','\0'},
			     IsoString   }, /*ISOnum , fraction seven-eights */ 
 {"[gamma ]", {'\147','\0'}, SymbolString}, /*ISOgrk3, Greek, small gamma    */ 
 {"[ge    ]", {'\263','\0'}, SymbolString}, /*ISOtech, greater-than-or-equal */ 
 {"[gt    ]", {  '>' ,'\0'}, IsoString   }, /*ISOnum , greater than sign     */ 
 {"[half  ]", {'\275','\0'}, IsoString   }, /*ISOnum , fraction one-half     */ 
 {"[harr  ]", {'\253','\0'}, SymbolString}, /*ISOamsa, left & right arrow    */ 
 {"[hArr  ]", {'\333','\0'}, SymbolString}, /*ISOamsa, l & r double arrow    */ 
 {"[hearts]", {'\251','\0'}, SymbolString}, /*ISOpub , hearts suit symbol    */ 
 {"[hellip]", {'\274','\0'}, SymbolString}, /*ISOpub , ellipsis(horizontal)  */ 
 {"[horbar]", {'\276','\0'}, SymbolString}, /*ISOnum , horizontal bar        */ 
 {"[hyphen]", {  '-' ,'\0'}, IsoString   }, /*ISOnum , hyphen                */ 
 {"[iacute]", {'\355','\0'}, IsoString   }, /*ISOlat1, small i, acute accent */ 
 {"[icirc ]", {'\356','\0'}, IsoString   }, /*ISOlat1, small i, circumflex   */ 
 {"[iexcl ]", {'\241','\0'}, IsoString   }, /*ISOnum , inverted ! mark       */ 
 {"[igrave]", {'\354','\0'}, IsoString   }, /*ISOlat1, small i, grave accent */ 
 {"[image ]", {'\301','\0'}, SymbolString}, /*ISOamso,imaginary number symbol*/ 
 {"[infin ]", {'\245','\0'}, SymbolString}, /*ISOtech, infinity              */ 
 {"[int   ]", {'\246','\0'}, SymbolString}, /*ISOtech, intergral operator    */ 
 {"[iota  ]", {'\151','\0'}, SymbolString}, /*ISOgrk3, Greek, small iota     */ 
 {"[iquest]", {'\277','\0'}, IsoString   }, /*ISOnum , inverted ? mark       */ 
 {"[isin  ]", {'\316','\0'}, SymbolString}, /*ISOtech, set membership        */ 
 {"[iuml  ]", {'\357','\0'}, IsoString   }, /*ISOlat1, small i, umlaut mark  */ 
 {"[kappa ]", {'\153','\0'}, SymbolString}, /*ISOgrk3, Greek, small kappa    */ 
 {"[lambda]", {'\154','\0'}, SymbolString}, /*ISOgrk3, Greek, small lambda   */ 
 {"[lang  ]", {'\341','\0'}, SymbolString}, /*ISOtech, left angle bracket    */ 
 {"[laquo ]", {'\253','\0'}, IsoString   }, /*ISOnum , left angle quotation  */ 
 {"[larr  ]", {'\254','\0'}, SymbolString}, /*ISOnum , leftward arrow        */ 
 {"[lArr  ]", {'\334','\0'}, SymbolString}, /*ISOtech, is implied by         */ 
 {"[lcub  ]", {  '{' ,'\0'}, IsoString   }, /*ISOnum , left curly brace      */ 
 {"[ldquo ]", {  '"' ,'\0'}, IsoString   }, /*ISOnum , left double quote     */ 
 {"[le    ]", {'\243','\0'}, SymbolString}, /*ISOtech, less-than-or-equal    */ 
 {"[lowbar]", {  '_' ,'\0'}, IsoString   }, /*ISOnum , low line              */ 
 {"[lpar  ]", {  '(' ,'\0'}, IsoString   }, /*ISOnum , left parenthesis      */ 
 {"[lsqb  ]", {  '[' ,'\0'}, IsoString   }, /*ISOnum , left square bracket   */ 
 {"[lsquo ]", {  '`' ,'\0'}, IsoString   }, /*ISOnum , left single quote     */ 
 {"[lsquor]", {  ',' ,'\0'}, IsoString   }, /*ISOnum,rising single quote(low)*/ 
 {"[lt    ]", {  '<' ,'\0'}, IsoString   }, /*ISOnum , less-than sign        */ 
 {"[mdash ]", {'\276','\0'}, SymbolString}, /*ISOpub , em dash(long dash)    */ 
 {"[micro ]", {'\155','\0'}, SymbolString}, /*ISOnum , micro                 */ 
 {"[middot]", {'\267','\0'}, IsoString   }, /*ISOnum , middle dot            */ 
 {"[minus ]", {  '-' ,'\0'}, IsoString   }, /*ISOtech, minus sign            */ 
 {"[mldr  ]", {'.','.','.','.','\0'},
			     IsoString   }, /*ISOpub , em leader             */
 {"[mnplus]", {'\140','\b','+','\0'},
			     SymbolString}, /*ISOtech, minus-or-plus sign    */
 {"[mu    ]", {'\155','\0'}, SymbolString}, /*ISOgrk3, Greek, small mu       */ 
 {"[nabla ]", {'\321','\0'}, SymbolString}, /*ISOtech, del, Hamilton operator*/ 
 {"[nbsp  ]", {  ' ' ,'\0'}, IsoString   }, /*ISOnum , no break space        */ 
 {"[ndash ]", {  '-' ,'\0'}, IsoString   }, /*ISOpub , en dash(short dash)   */ 
 {"[ne    ]", {'\271','\0'}, SymbolString}, /*ISOtech, not equal             */ 
 {"[ni    ]", {'\047','\0'}, SymbolString}, /*ISOtech, contains              */ 
 {"[nldr  ]", {'.','.','\0'},IsoString   }, /*ISOpub , double baseline dot   */
 {"[not   ]", {'\330','\0'}, SymbolString}, /*ISOnum , not                   */ 
 {"[notin ]", {'\317','\0'}, SymbolString}, /*ISOtech, negated set membership*/ 
 {"[ntilde]", {'\361','\0'}, IsoString   }, /*ISOlat1, small N, tilde        */ 
 {"[nu    ]", {'\156','\0'}, SymbolString}, /*ISOgrk3, Greek, small nu       */ 
 {"[num   ]", {  '#' ,'\0'}, IsoString   }, /*ISOnum , number sign           */ 
 {"[oacute]", {'\363','\0'}, IsoString   }, /*ISOlat1, small o, acute accent */ 
 {"[ocirc ]", {'\364','\0'}, IsoString   }, /*ISOlat1, small o, circumflex   */ 
 {"[ograve]", {'\362','\0'}, IsoString   }, /*ISOlat1, small o, grave accent */ 
 {"[ohm   ]", {'\127','\0'}, SymbolString}, /*ISOnum , ohm                   */ 
 {"[omega ]", {'\167','\0'}, SymbolString}, /*ISOgrk3, Greek, small omega    */ 
 {"[oplus ]", {'\305','\0'}, SymbolString}, /*ISOamsb, plus sign in circle   */ 
 {"[or    ]", {'\332','\0'}, SymbolString}, /*ISOtech, vee, logical or       */ 
 {"[ordf  ]", {'\252','\0'}, IsoString   }, /*ISOnum , ordinal indicator, fem*/ 
 {"[ordm  ]", {'\272','\0'}, IsoString   }, /*ISOnum , ordinal indicator,male*/ 
 {"[oslash]", {'\370','\0'}, IsoString   }, /*ISOlat1, small o, slash        */ 
 {"[osol  ]", {'\306','\0'}, SymbolString}, /*ISOamsb, slash in circle       */ 
 {"[otilde]", {'\365','\0'}, IsoString   }, /*ISOlat1, small o, tilde        */ 
 {"[otimes]", {'\304','\0'}, SymbolString}, /*ISOamsb,multiply sign in circle*/ 
 {"[ouml  ]", {'\366','\0'}, IsoString   }, /*ISOlat1, small o, umlaut mark  */ 
 {"[over  ]", {'\140','\0'}, SymbolString}, /*made up, over symbol           */ 
 {"[par   ]", {'\275', '\275', '\0'},
			     SymbolString}, /*ISOtech, parallel              */ 
 {"[para  ]", {'\266','\0'}, IsoString   }, /*ISOnum , paragraph sign        */ 
 {"[part  ]", {'\266','\0'}, SymbolString}, /*ISOtech, partial differential  */ 
 {"[percnt]", {  '%' ,'\0'}, IsoString   }, /*ISOnum , percent sign          */ 
 {"[period]", {  '.' ,'\0'}, IsoString   }, /*ISOnum , full stop, period     */ 
 {"[perp  ]", {'\136','\0'}, SymbolString}, /*ISOtech, perpendicular         */ 
 {"[phis  ]", {'\146','\0'}, SymbolString}, /*ISOgrk3, Greek, small phi      */ 
 {"[pi    ]", {'\160','\0'}, SymbolString}, /*ISOgrk3, Greek, small pi       */ 
 {"[piv   ]", {'\166','\0'}, SymbolString}, /*ISOgrk3, Greek, small pi, var  */ 
 {"[plus  ]", {  '+' ,'\0'}, IsoString   }, /*ISOnum , plus sign             */ 
 {"[plusmn]", {'\261','\0'}, SymbolString}, /*ISOnum , plus or minus sign    */ 
 {"[pound ]", {'\243','\0'}, IsoString   }, /*ISOnum , pound sign            */ 
 {"[prime ]", {'\242','\0'}, SymbolString}, /*ISOtech, prime or minute       */ 
 {"[prop  ]", {'\265','\0'}, SymbolString}, /*ISOtech, proportional to       */ 
 {"[psi   ]", {'\171','\0'}, SymbolString}, /*ISOgrk3, Greek, small psi      */ 
 {"[quest ]", {  '?' ,'\0'}, IsoString   }, /*ISOnum , question mark         */ 
 {"[quot  ]", { '\'' ,'\0'}, IsoString   }, /*ISOnum , quote mark            */ 
 {"[radic ]", {'\326','\0'}, SymbolString}, /*ISOtech, radical               */ 
 {"[rang  ]", {'\361','\0'}, SymbolString}, /*ISOtech, right angle bracket   */ 
 {"[raquo ]", {'\273','\0'}, IsoString   }, /*ISOnum , right angle quotation */ 
 {"[rarr  ]", {'\256','\0'}, SymbolString}, /*ISOnum , rightward arrow       */ 
 {"[rArr  ]", {'\336','\0'}, SymbolString}, /*ISOtech, rt dbl arrow,implies  */ 
 {"[rcub  ]", {  '}' ,'\0'}, IsoString   }, /*ISOnum , right curly brace     */ 
 {"[rdquo ]", {  '"' ,'\0'}, IsoString   }, /*ISOnum , right double quote    */ 
 {"[real  ]", {'\302','\0'}, SymbolString}, /*ISOamso, real number symbol    */ 
 {"[reg   ]", {'\322','\0'}, SymbolString}, /*ISOnum,circledR,registered sign*/ 
 {"[rho   ]", {'\162','\0'}, SymbolString}, /*ISOgrk3, Greek, small rho      */ 
 {"[rpar  ]", {  ')' ,'\0'}, IsoString   }, /*ISOnum , right parenthesis     */ 
 {"[rsqb  ]", {  ']' ,'\0'}, IsoString   }, /*ISOnum , right square bracket  */ 
 {"[rsquo ]", { '\'' ,'\0'}, IsoString   }, /*ISOnum , right single quote    */ 
 {"[sect  ]", {'\247','\0'}, IsoString   }, /*ISOnum , section sign          */ 
 {"[semi  ]", {  ';' ,'\0'}, IsoString   }, /*ISOnum , semicolon             */ 
 {"[shy   ]", {'\255','\0'}, IsoString   }, /*ISOnum , soft hypen            */ 
 {"[sigma ]", {'\163','\0'}, SymbolString}, /*ISOgrk3, Greek, small sigma    */ 
 {"[sim   ]", {  '~' ,'\0'}, IsoString   }, /*ISOtech, similar to            */ 
 {"[sime  ]", {'\100','\0'}, SymbolString}, /*ISOtech, similar, equals       */ 
 {"[sol   ]", {  '/' ,'\0'}, IsoString   }, /*ISOnum , solidus               */ 
 {"[spades]", {'\252','\0'}, SymbolString}, /*ISOpub , spades suit symbol    */ 
 {"[sub   ]", {'\314','\0'}, SymbolString}, /*ISOtech, subset/is implied by  */ 
 {"[sube  ]", {'\315','\0'}, SymbolString}, /*ISOtech, subset, equals        */ 
/*"[sung  ]", { ??   ,'\0'},             }, /*ISOnum , musical note(sung txt)*/ 
 {"[sup   ]", {'\311','\0'}, SymbolString}, /*ISOtech, superset or implies   */ 
 {"[sup1  ]", {'\271','\0'}, IsoString   }, /*ISOnum , superscript one       */ 
 {"[sup2  ]", {'\262','\0'}, IsoString   }, /*ISOnum , superscript two       */ 
 {"[sup3  ]", {'\263','\0'}, IsoString   }, /*ISOnum , superscript three     */ 
 {"[supe  ]", {'\312','\0'}, SymbolString}, /*ISOtech, superset, equals      */ 
 {"[szlig ]", {'\337','\0'}, IsoString   }, /*ISOlat1, small sharp s, German */ 
 {"[tau   ]", {'\164','\0'}, SymbolString}, /*ISOgrk3, Greek, small tau      */ 
 {"[there4]", {'\134','\0'}, SymbolString}, /*ISOtech, therefore             */ 
 {"[thetas]", {'\161','\0'}, SymbolString}, /*ISOgrk3, Greek, small theta    */ 
 {"[thetav]", {'\112','\0'}, SymbolString}, /*ISOgrk3, Greek, small theta,var*/ 
 {"[times ]", {'\264','\0'}, SymbolString}, /*ISOnum , multipy sign          */ 
 {"[thorn ]", {'\376','\0'}, IsoString   }, /*ISOlat1, small thorn, Icelandic*/ 
 {"[tprime]", {'\242', '\242', '\242', '\0'},
			     SymbolString}, /*ISOtech, triple prime          */ 
 {"[trade ]", {'\324','\0'}, SymbolString}, /*ISOnum , trade mark sign       */ 
 {"[uarr  ]", {'\255','\0'}, SymbolString}, /*ISOnum , upward arrow          */ 
 {"[uArr  ]", {'\335','\0'}, SymbolString}, /*ISOamsa, up double arrow       */ 
 {"[uacute]", {'\372','\0'}, IsoString   }, /*ISOlat1, small u, acute accent */ 
 {"[ucirc ]", {'\373','\0'}, IsoString   }, /*ISOlat1, small u, circumflex   */ 
 {"[ugrave]", {'\371','\0'}, IsoString   }, /*ISOlat1, small u, grave accent */ 
 {"[upsi  ]", {'\165','\0'}, SymbolString}, /*ISOgrk3, Greek, small upsilon  */ 
 {"[uuml  ]", {'\374','\0'}, IsoString   }, /*ISOlat1, small u, umlaut mark  */ 
 {"[vellip]", {':','\n',':','\0'},
			     SymbolString}, /*ISOpub , vertical ellipsis     */ 
 {"[verbar]", {'\174','\0'}, IsoString   }, /*ISOnum , vertical bar          */ 
 {"[weierp]", {'\303','\0'}, SymbolString}, /*ISOamso, Weierstrass p         */ 
 {"[xi    ]", {'\170','\0'}, SymbolString}, /*ISOgrk3, Greek, small xi       */ 
 {"[yacute]", {'\375','\0'}, IsoString   }, /*ISOlat1, small y, acute accent */ 
 {"[yen   ]", {'\245','\0'}, IsoString   }, /*ISOnum , yen sign              */ 
 {"[yuml  ]", {'\377','\0'}, IsoString   }, /*ISOlat1, small y, umlaut mark  */ 
 {"[zeta  ]", {'\172','\0'}, SymbolString}, /*ISOgrk3, Greek, small zeta     */ 

 {"[Aacute]", {'\301','\0'}, IsoString   }, /*ISOlat1, capital a,acute accent*/ 
 {"[Acirc ]", {'\302','\0'}, IsoString   }, /*ISOlat1, capital a,circumflex  */ 
 {"[AElig ]", {'\306','\0'}, IsoString   }, /*ISOlat1, capital ae diphthong  */ 
 {"[Agrave]", {'\300','\0'}, IsoString   }, /*ISOlat1, capital a,grave accent*/ 
 {"[Aring ]", {'\305','\0'}, IsoString   }, /*ISOlat1, capital a,ring        */ 
 {"[Atilde]", {'\303','\0'}, IsoString   }, /*ISOlat1, capital a,tilde       */ 
 {"[Auml  ]", {'\304','\0'}, IsoString   }, /*ISOlat1, capital a,umlaut mark */ 
 {"[Ccedil]", {'\307','\0'}, IsoString   }, /*ISOlat1, capital c, cedilla    */ 
 {"[Delta ]", {'\104','\0'}, SymbolString}, /*ISOgrk3, Greek, large delta    */ 
 {"[Dot   ]", {'\250','\0'}, IsoString   }, /*ISOtech, dieresis or umlaut mrk*/ 
 {"[DotDot]", {'\250', '\250', '\0'},
			     IsoString   }, /*ISOtech, four dots above       */ 
 {"[Eacute]", {'\311','\0'}, IsoString   }, /*ISOlat1, capital E,acute accent*/ 
 {"[Ecirc ]", {'\312','\0'}, IsoString   }, /*ISOlat1, capital E,circumflex  */ 
 {"[Egrave]", {'\310','\0'}, IsoString   }, /*ISOlat1, capital E,grave accent*/ 
 {"[ETH   ]", {'\320','\0'}, IsoString   }, /*ISOlat1, capital Eth, Icelandic*/ 
 {"[Euml  ]", {'\313','\0'}, IsoString   }, /*ISOlat1, capital E,umlaut mark */ 
 {"[Gamma ]", {'\107','\0'}, SymbolString}, /*ISOgrk3, Greek, large gamma    */ 
 {"[Iacute]", {'\315','\0'}, IsoString   }, /*ISOlat1, capital I,acute accent*/ 
 {"[Icirc ]", {'\316','\0'}, IsoString   }, /*ISOlat1, capital I,circumflex  */ 
 {"[Igrave]", {'\314','\0'}, IsoString   }, /*ISOlat1, capital I,grave accent*/ 
 {"[Iuml  ]", {'\317','\0'}, IsoString   }, /*ISOlat1, capital I,umlaut mark */ 
 {"[Lambda]", {'\114','\0'}, SymbolString}, /*ISOgrk3, Greek, large lambda   */ 
 {"[Ntilde]", {'\321','\0'}, IsoString   }, /*ISOlat1, capital N, tilde      */ 
 {"[Oacute]", {'\323','\0'}, IsoString   }, /*ISOlat1, capital O,acute accent*/ 
 {"[Ocirc ]", {'\324','\0'}, IsoString   }, /*ISOlat1, capital O,circumflex  */ 
 {"[Ograve]", {'\322','\0'}, IsoString   }, /*ISOlat1, capital O,grave accent*/ 
 {"[Oslash]", {'\330','\0'}, IsoString   }, /*ISOlat1, capital O, slash      */ 
 {"[Otilde]", {'\325','\0'}, IsoString   }, /*ISOlat1, capital O, tilde      */ 
 {"[Omega ]", {'\127','\0'}, SymbolString}, /*ISOgrk3, Greek, large omega    */ 
 {"[Ouml  ]", {'\326','\0'}, IsoString   }, /*ISOlat1, capital O,umlaut mark */ 
 {"[Pi    ]", {'\120','\0'}, SymbolString}, /*ISOgrk3, Greek, large pi       */ 
 {"[Prime ]", {'\262','\0'}, SymbolString}, /*ISOtech, double prime/second   */ 
 {"[Phi   ]", {'\106','\0'}, SymbolString}, /*ISOgrk3, Greek, large phi      */ 
 {"[Psi   ]", {'\131','\0'}, SymbolString}, /*ISOgrk3, Greek, large psi      */ 
 {"[Sigma ]", {'\123','\0'}, SymbolString}, /*ISOgrk3, Greek, large sigma    */ 
 {"[THORN ]", {'\336','\0'}, IsoString   }, /*ISOlat1,capital THORN,Icelandic*/ 
 {"[Theta ]", {'\121','\0'}, SymbolString}, /*ISOgrk3, Greek, large theta    */ 
 {"[Uacute]", {'\332','\0'}, IsoString   }, /*ISOlat1, capital U,acute accent*/ 
 {"[Ucirc ]", {'\333','\0'}, IsoString   }, /*ISOlat1, capital U,circumflex  */ 
 {"[Ugrave]", {'\331','\0'}, IsoString   }, /*ISOlat1, capital U,grave accent*/ 
 {"[Upsi  ]", {'\125','\0'}, SymbolString}, /*ISOgrk3, Greek, large upsilon  */ 
 {"[Uuml  ]", {'\334','\0'}, IsoString   }, /*ISOlat1, capital U,umlaut mark */ 
 {"[Verbar]", {'\174', '\174', '\0'},
			     IsoString   }, /*ISOtech, dbl vertical bar      */ 
 {"[Xi    ]", {'\130','\0'}, SymbolString}, /*ISOgrk3, Greek, large xi       */ 
 {"[Yacute]", {'\335','\0'}, IsoString   }, /*ISOlat1, capital Y,acute accent*/ 
 { NULL     , {' '   ,'\0'}, IsoString   }, /* default character to use      */
};


/*****************************************************************************
 *		Semi-Private Variables
 *****************************************************************************/
/*****************************************************************************
 *		Private Functions
 *****************************************************************************/
/*****************************************************************************
 * Function:	_DtCvUnit GetTocIndicator(pDAS);
 *
 * Parameters:
 *
 * Returns:
 *
 * Purpose:
 *
 *****************************************************************************/
static	_DtHelpFontHints	DefFontAttrs = {
		NULL, NULL,
		10, 10, NULL,
		NULL, NULL, NULL, NULL,
		NULL, NULL, NULL, NULL,
		_DtHelpFontStyleSerif,
		_DtHelpFontSpacingProp,
		_DtHelpFontWeightMedium,
		_DtHelpFontSlantRoman,
		_DtHelpFontSpecialNone };

static void
GetTocIndicator (
    DtHelpDispAreaStruct	*pDAS)
{
    _DtCvUnit	ascent;

    pDAS->toc_width  = 0;
    pDAS->toc_height = 0;
    pDAS->toc_indicator = NULL;
    if (_DtHelpDAResolveSpc (pDAS, "C", IsoString, DefFontAttrs, "[rArr  ]",
			     &(pDAS->toc_indicator), &(pDAS->toc_width),
			     &(pDAS->toc_height), &ascent))
	_DtHelpDAResolveSpc (pDAS, "C", IsoString, DefFontAttrs, "[ndash ]",
			     &(pDAS->toc_indicator), &(pDAS->toc_width),
			     &(pDAS->toc_height), &ascent);
}

static void
ClearTocMarker (
    DtHelpDispAreaStruct	*pDAS,
    _DtCvFlags			 flag)
{
    _DtCvUnit clearHeight;
    _DtCvUnit clearWidth;
    _DtCvUnit clearY      = pDAS->toc_y + pDAS->decorThickness
					- pDAS->firstVisible;
    _DtCvUnit clearX      = pDAS->decorThickness - pDAS->virtualX;

    clearHeight = pDAS->toc_height;
    clearWidth  = pDAS->toc_width;

    if (clearY < pDAS->decorThickness)
      {
	clearHeight = clearHeight - pDAS->decorThickness + clearY;
	clearY      = pDAS->decorThickness;
      }
    if (clearX < pDAS->decorThickness)
      {
	clearWidth = clearWidth - pDAS->decorThickness + clearX;
	clearX     = pDAS->decorThickness;
      }

    if (clearWidth > 0 && clearHeight > 0)
      {
	if (IsSelected(flag))
	    XFillRectangle (XtDisplay(pDAS->dispWid), XtWindow(pDAS->dispWid),
				pDAS->invertGC,
				clearX, clearY, clearWidth, clearHeight);
	else
            XClearArea(XtDisplay(pDAS->dispWid), XtWindow(pDAS->dispWid),
			clearX, clearY, clearWidth, clearHeight, False);
      }
}

/*****************************************************************************
 * Function:	void DrawTocIndicator ();
 *
 * Parameters:
 *
 * Returns:
 *
 * Purpose:
 *
 *****************************************************************************/
static void
DrawTocIndicator (
    DtHelpDispAreaStruct *pDAS,
    _DtCvFlags	flag)
{

    if (XtIsRealized(pDAS->dispWid)
	&& TocIsOn(pDAS->toc_flag)
	&& ((int)(pDAS->toc_y+pDAS->toc_height)) >= ((int) pDAS->firstVisible)
	&& ((int)pDAS->toc_y) < ((int)(pDAS->firstVisible+pDAS->dispUseHeight))
	&& pDAS->toc_indicator)
      {
        _DtHelpDARegion *regInfo = (_DtHelpDARegion *) pDAS->toc_indicator;

        DADrawSpc (((_DtCvPointer) pDAS), regInfo->handle,
			0, pDAS->toc_base,
			0, pDAS->toc_y,
			pDAS->toc_height, 0, 0, 0);
      }
}

/*****************************************************************************
 * Function:	void DrawTraversal ();
 *
 * Parameters:
 *		pDAS	Specifies the display area.
 *		dst_x	Specifies the inside boundary of the segment.
 *			This is where the segment starts, so the
 *			traversal box must end at dst_x - 1.
 *		dst_y	Specifies the upper pixel that the traversal
 *			will cover. 
 *		width	Specifies the inner width of the traversal box.
 *		height  Specifies the outer height of the traversal box
 *			including the leading. But since the leading is
 *			also used for the bottom traversal line, its
 *			okay.
 *		flag	Specifies whether to turn the traversal box on
 *			or off.
 *
 * Returns: Nothing.
 *
 * Purpose: Draw the traversal box.
 *
 *****************************************************************************/
static void
DrawTraversal (
    DtHelpDispAreaStruct	*pDAS,
    _DtCvUnit	dst_x,
    _DtCvUnit	dst_y,
    _DtCvUnit	width,
    _DtCvUnit	height,
    _DtCvFlags	flag)
{
    int     copyY1;
    int     copyY2;

    int     lineX1;
    int     lineY1;
    int     lineX2;
    int     lineY2;

    int     onLeftTop;
    int     onRightBot;
    Display	*dpy = XtDisplay(pDAS->dispWid);
    Window	 win = XtWindow(pDAS->dispWid);

    if (pDAS->neededFlags & _DT_HELP_FOCUS_FLAG)
      {
	XGCValues saved, gc_values;
/*
 * SYSTEM
 * see the SYSTEM note in XUICreate.c
 */
	/*
	 * how many pixels will be above or to the left of the
	 * destination position.
	 */
        onLeftTop = pDAS->lineThickness / 2;

	/*
	 * how many pixels will be on and inside the destination.
	 */
	onRightBot = pDAS->lineThickness - onLeftTop;

	/*
	 * calculate the y position of the traversal lines
	 * for the top and bottom lines.
	 */
	lineY1 = dst_y + onLeftTop;
	lineY2 = dst_y + height - onRightBot;

	/*
	 * get the extreme y positions.
	 */
	copyY1 = dst_y;
	copyY2 = dst_y + height;

	/*
	 * adjust the source and destination positions.
	 */
	lineX1 = dst_x;
	if (IsTraversalBegin (flag))
	    lineX1 = lineX1 - onRightBot;

	lineX2 = dst_x + width;
	if (IsTraversalEnd (flag))
	    lineX2 = lineX2 + onLeftTop;

	XGetGCValues(dpy, pDAS->normalGC, GCLineStyle | GCForeground, &saved);
	gc_values.line_style = saved.line_style;
	if (HasTraversal(flag)) {
	    if (flag & _DtCvAPP_FLAG2) {
		XSetForeground (dpy, pDAS->normalGC, pDAS->searchColor);
		gc_values.line_style = LineOnOffDash;
	    }
	    else
		XSetForeground (dpy, pDAS->normalGC, pDAS->traversalColor);
	}
	else
	    XSetForeground (dpy, pDAS->normalGC, pDAS->backgroundColor);

	if (gc_values.line_style != saved.line_style)
	    XChangeGC(dpy, pDAS->normalGC, GCLineStyle, &gc_values);

	/*
	 * draw the top and bottom lines
	 */
	if (lineX1 - onLeftTop < ((int) pDAS->dispWidth) &&
						lineX2 + onRightBot - 1 > 0)
	  {
	    /*
	     * draw the top line
	     */
	    if (copyY1 < ((int) pDAS->dispHeight) &&
		      copyY1 + ((int)pDAS->lineThickness) >
						((int)pDAS->decorThickness))
	        XDrawLine(dpy,win,pDAS->normalGC,lineX1,lineY1,lineX2,lineY1);

	    /*
	     * draw the bottom line
	     */
	    if (copyY2 - ((int)pDAS->lineThickness) < ((int)pDAS->dispHeight)
				&& copyY2 > ((int)pDAS->decorThickness))
	        XDrawLine(dpy,win,pDAS->normalGC,lineX1,lineY2,lineX2,lineY2);
	  }

	/*
	 * draw the left end
	 */
	if (IsTraversalBegin(flag)
		&& lineX1 - onLeftTop < ((int)pDAS->dispWidth)
		&& lineX1 + onRightBot - 1 > 0
		&& copyY1 < ((int) pDAS->dispHeight)
		&& copyY2 > ((int)pDAS->decorThickness))
	    XDrawLine(dpy, win, pDAS->normalGC, lineX1, copyY1, lineX1, copyY2);

	/*
	 * draw the right end
	 */
	if (IsTraversalEnd(flag)
		&& lineX2 - onLeftTop < ((int) pDAS->dispWidth)
		&& lineX2 + onRightBot - 1 > 0
		&& copyY1 < ((int) pDAS->dispHeight)
		&& copyY2 > ((int) pDAS->decorThickness))
	    XDrawLine(dpy, win, pDAS->normalGC, lineX2, copyY1, lineX2, copyY2);

	XSetForeground (dpy, pDAS->normalGC, saved.foreground);
	XSetFillStyle(dpy, pDAS->normalGC, FillSolid);

	if (gc_values.line_style != saved.line_style)
	    XChangeGC(dpy, pDAS->normalGC, GCLineStyle, &saved);
      }
}

static void
DrawSearchTraversal(DtHelpDispAreaStruct* pDAS,
			_DtCvUnit dst_x, _DtCvUnit dst_y,
			_DtCvUnit width, _DtCvUnit height, _DtCvFlags flag)
{
    short save = pDAS->neededFlags;

    pDAS->neededFlags |= _DT_HELP_FOCUS_FLAG;

    DrawTraversal(pDAS, dst_x, dst_y, width, height, flag);

    pDAS->neededFlags = save;
}

/*****************************************************************************
 * Function:	void MyDrawString (Display *dpy, Drawable d, GC gc,
 *				int font_index,
 *				int x, int y, char *string, int length,
 *				int wc, DtHelpDAFontInfo font_info)
 *
 * Parameters:
 *		dpy		Specifies the connection to the X Server.
 *		d		Specifies the drawable.
 *		gc		Specifies the graphic's context.
 *		font_index	Specifies the font structure or set to use.
 *				If the value is positive, it indicates a
 *				font structure. If negative, it indicates
 *				a font set.
 *		x		Specifies the x coordinate.
 *		y		Specifies the y coordinate.
 *		string		Specifies the string to render.
 *		length		Specifies the number of bytes in 'string'
 *				to render.
 *		wc		Flag to designate wide character data
 *
 * Return Value:	Nothing
 *
 * Purpose:	Renders an text string to the specified drawable.
 *
 *****************************************************************************/
static void
MyDrawString (
    Display	*dpy,
    Drawable	 d,
    GC		 gc,
    int		 font_index,
    int		 x,
    int		 y,
    const void	*string,
    int		 length,
    int          wc,
    int		 image,
    DtHelpDAFontInfo	font_info)
{

/*
 * If the 'font_index' is a negative number, it indicates a font set was
 * opened for this string. Use the X11R5 I18N routines to render the
 * glyphs. Otherwise use the standard X11 drawing routines.
 */

    if (font_index < 0)
      {
	XFontSet  mySet;

	mySet = __DtHelpFontSetGet(font_info, font_index);

        if (wc) {
	    if (image)
		XwcDrawImageString(dpy, d, mySet, gc, x, y,
						(wchar_t*)string, length);
	    else
		XwcDrawString (dpy, d, mySet, gc, x, y,
						(wchar_t*)string, length);
	}
        else {
	    /*
	     * have to use XmbDrawString here instead of XDrawString
	     * because we need to use a font set to render the string.
	     */
	    if (image)
		XmbDrawImageString (dpy, d, mySet, gc, x, y, string, length);
	    else
		XmbDrawString (dpy, d, mySet, gc, x, y, string, length);
	}
      }
    else
      {
	XFontStruct  *myFont;

	myFont = __DtHelpFontStructGet(font_info, font_index);
	XSetFont (dpy, gc, myFont->fid);

	if (image)
	    XDrawImageString (dpy, d, gc, x, y, string, length);
	else
	    XDrawString (dpy, d, gc, x, y, string, length);
      }
}

/*****************************************************************************
 * Function:	void ResolveFont ();
 *
 * Parameters:
 *
 * Returns:
 *
 * Purpose:
 *
 *****************************************************************************/
static int
ResolveFont (
    _DtCvPointer	 client_data,
    char		*lang,
    const char		*charset,
    _DtHelpFontHints	 font_attr,
    int			*ret_idx )
{
    int    result = -1;
    char  *xlfdSpec = font_attr.xlfd;
    char   buffer[10];
    DtHelpDispAreaStruct *pDAS = (DtHelpDispAreaStruct *) client_data;
    XrmName	xrmList[_DtHelpFontQuarkNumber];

    _DtHelpCopyDefaultList(xrmList);

    if (font_attr.spacing != _DtHelpFontSpacingProp)
        xrmList[_DT_HELP_FONT_SPACING] = XrmStringToQuark("m");

    sprintf(buffer, "%d", font_attr.pointsz);
    xrmList[_DT_HELP_FONT_SIZE]    = XrmStringToQuark(buffer);

    if (font_attr.slant != _DtHelpFontSlantRoman)
      {
        xrmList[_DT_HELP_FONT_ANGLE]  = XrmStringToQuark("italic");
	if (font_attr.xlfdi != NULL)
	    xlfdSpec = font_attr.xlfdi;
      }

    if (font_attr.weight == _DtHelpFontWeightBold)
      {
        xrmList[_DT_HELP_FONT_WEIGHT] = XrmStringToQuark("bold");
	if (xrmList[_DT_HELP_FONT_ANGLE] == XrmStringToQuark("italic"))
	  {
	    if (font_attr.xlfdib != NULL)
	        xlfdSpec = font_attr.xlfdib;
	  }
	else if (font_attr.xlfdb != NULL)
	    xlfdSpec = font_attr.xlfdb;
      }

    if (font_attr.style == _DtHelpFontStyleSerif)
        xrmList[_DT_HELP_FONT_TYPE] = XrmStringToQuark("serif");
    else if (font_attr.style == _DtHelpFontStyleSymbol)
        xrmList[_DT_HELP_FONT_TYPE] = XrmStringToQuark("symbol");

    xrmList[_DT_HELP_FONT_LANG_TER] = XrmStringToQuark ("C");
    if (lang != NULL)
	xrmList[_DT_HELP_FONT_LANG_TER] = XrmStringToQuark(lang);

    if (charset != NULL)
        xrmList[_DT_HELP_FONT_CHAR_SET] = XrmStringToQuark(charset);

    if (NULL == xlfdSpec ||
		_DtHelpGetExactFontIndex(pDAS,lang,charset,xlfdSpec,ret_idx)!=0)
        result = __DtHelpFontIndexGet (pDAS, xrmList, ret_idx);

    return result;
}

/*****************************************************************************
 * Function:	void DADrawLine ();
 *
 * Parameters:
 *
 * Returns:
 *
 * Purpose:
 *
 *****************************************************************************/
static void
DADrawLine (
    _DtCvPointer client_data,
    _DtCvUnit	 x1,
    _DtCvUnit	 y1,
    _DtCvUnit	 x2,
    _DtCvUnit	 y2,
    _DtCvUnit	 line_width)
{
    int		 offset;
    Display	*dpy;
    Window	 win;
    DtHelpDispAreaStruct *pDAS = (DtHelpDispAreaStruct *) client_data;
    XGCValues	 saved, gcValues;

    gcValues.line_width = line_width;
    offset = line_width / 2;
    dpy    = XtDisplay(pDAS->dispWid);
    win    = XtWindow(pDAS->dispWid);

    /*
     * get the current value of the gc.
     */
    XGetGCValues(dpy, pDAS->normalGC, GCLineWidth, &saved);

    /*
     * does the old line width equal the width we want?
     * If no, then change it to the desired width.
     */
    if (line_width != saved.line_width)
	XChangeGC(dpy, pDAS->normalGC, GCLineWidth, &gcValues);

    /*
     * drawing a horizontal line?
     */
    if (x1 == x2)
      {
	x1 += offset;
	x2  = x1;
      }
    else
      {
	y1 += offset;
	y2  = y1;
      }

    y1 = y1 + pDAS->decorThickness - pDAS->firstVisible;
    x1 = x1 + pDAS->decorThickness - pDAS->virtualX;

    y2 = y2 + pDAS->decorThickness - pDAS->firstVisible;
    x2 = x2 + pDAS->decorThickness - pDAS->virtualX;

    XDrawLine (dpy, win, pDAS->normalGC, x1, y1, x2, y2);

    /*
     * did we change the gc's line_width? If so, change it back.
     */
    if (line_width != saved.line_width)
	XChangeGC(dpy, pDAS->normalGC, GCLineWidth, &saved);
}

/*****************************************************************************
 * Function:	void DADrawString ();
 *
 * Parameters:
 *		client_data	Specifies the client specific data
 *				associated with this canvas.
 *		x		Specifies the x unit at which the
 *				string is to be rendered.
 *		y		Specifes the y unit at which the baseline
 *				of the string is to be rendered.
 *		string		Specifies the string to render.
 *		byte_len	Specifies the number of bytes of the
 *				string to render.
 *		char_len	Specifies the number of bytes that
 *				comprise one character.
 *		font_ptr	Specifies the font associated with this
 *				string.
 *		box_x,box_y	Specifies the upper left hand corner of
 *				the bounding box for this string. This
 *				includes any traversal and/or link offsets
 *				relevent for the line this segment occupies.
 *		box_height	Specifies the maximum height of the bounding
 *				box for this string. This includes any
 *				traversal and/or offsets relevent to this
 *				line.
 *		old_flags	Specifies .............
 *		new_flags	Specifies .............
 *
 * Returns: Nothing
 *
 * Purpose:
 *
 *****************************************************************************/
static void
DADrawString (
    _DtCvPointer client_data,
    _DtCvPointer data,
    _DtCvUnit	 x,
    _DtCvUnit	 y,
    const void	*string,
    int		 byte_len,
    int		 wc,
    _DtCvPointer font_ptr,
    _DtCvPointer scd,
    _DtCvUnit	 box_x,
    _DtCvUnit	 box_y,
    _DtCvUnit	 box_height,
    _DtCvFlags	 old_flags,
    _DtCvFlags	 new_flags )
{
    Boolean	 image_flag = False;
    int		 strWidth = 0;
    int		 width;
    Display	*dpy;
    Window	 win;
    GC		 drawGC;
    GC		 fillGC;
    DtHelpDispAreaStruct *pDAS = (DtHelpDispAreaStruct *) client_data;
    _DtCvStringClientData* pSCD = (_DtCvStringClientData*)scd;

    unsigned long mask1 = 0x00; /* to be replaced with highlight attrs */
    unsigned long mask2 = 0x00; /* to be replaced with stippling attrs */
    XGCValues gcValues;
    XGCValues saved1, saved2;

    /*
     * if this is outside our display area, ignore it.
     */
    if (box_y + box_height < pDAS->firstVisible ||
			box_y > pDAS->firstVisible + pDAS->dispHeight)
	return;

    y = y + pDAS->decorThickness - pDAS->firstVisible;
    x = x + pDAS->decorThickness - pDAS->virtualX;

    box_x = box_x + pDAS->decorThickness - pDAS->virtualX;
    box_y = box_y + pDAS->decorThickness - pDAS->firstVisible;

    if (byte_len)
        strWidth = _DtHelpDAGetStrWidth(client_data, _DtCvSTRING_TYPE, data);

    width = strWidth + (x - box_x);

    dpy = XtDisplay(pDAS->dispWid);
    win = XtWindow(pDAS->dispWid);

    /*
     * initial colors for rendering.
     */
    drawGC = pDAS->normalGC;
    fillGC = pDAS->invertGC;

    /*
     * is this a search hit or does this string have its own color?
     */
    if (strWidth && pDAS->dtinfo &&
	((new_flags & _DtCvSEARCH_FLAG) || 
		(NULL != pSCD && (pSCD->fg_color || pSCD->bg_color))))
      {
	XColor screen, exact;

	/*
	 * indicate that we are going to change the foreground and background.
	 */
	mask1 = (GCForeground | GCBackground);
	mask2 = (GCForeground | GCBackground);

	/*
	 * remember the current values of the gc.
	 */
	XGetGCValues(dpy, drawGC, mask1, &saved1);
	XGetGCValues(dpy, fillGC, mask2, &saved2);

	/* search highlight should precede stylesheet-driven highlights */
	/*
	 * is the search flag set? Set the foreground to the search color.
	 */
	if (new_flags & _DtCvSEARCH_FLAG)
	  {
	    XSetForeground(dpy, drawGC, pDAS->searchColor);
	    XSetBackground(dpy, fillGC, pDAS->searchColor);
	  }
	/*
	 * is there a foreground color specified?
	 */
	else if (pSCD->fg_color)
	  {
	    /*
	     * does the foreground color need allocating?
	     */
	    if (pSCD->fg_pixel == (unsigned long)-1)
	      {
		if (XAllocNamedColor(dpy, pDAS->colormap, pSCD->fg_color,
							&screen, &exact))
		  {
		    XSetForeground(dpy, drawGC, screen.pixel);
		    XSetBackground(dpy, fillGC, screen.pixel);
		  }
		else /* the allocation went badly */
		  {
		    mask1 &= ~GCForeground;
		    mask2 &= ~GCBackground;
		  }
	      }
	    else /* dtinfo has calculated pSCD->fg_pixel */
	      {
		XSetForeground(dpy, drawGC, pSCD->fg_pixel);
		XSetBackground(dpy, fillGC, pSCD->fg_pixel);
	      }
	  }
	else /* no foreground color specified */
	  {
	    mask1 &= ~GCForeground;
	    mask2 &= ~GCBackground;
	  }

	/*
	 * was a background color specified?
	 */
	if (pSCD->bg_color)
	  {
	    /*
	     * does the background color need allocating?
	     */
	    if (pSCD->bg_pixel == (unsigned long)-1)
	      {
		if (XAllocNamedColor(dpy, pDAS->colormap, pSCD->bg_color,
							&screen, &exact))
		  {
		    XSetBackground(dpy, drawGC, screen.pixel);
		    XSetForeground(dpy, fillGC, screen.pixel);
		  }
		else /* the allocation went badly */
		  {
		    mask1 &= ~GCBackground;
		    mask2 &= ~GCForeground;
		  }
	      }
	    else /* the background color's been allocated */
	      {
		XSetBackground(dpy, drawGC, pSCD->bg_pixel);
		XSetForeground(dpy, fillGC, pSCD->bg_pixel);
	      }
	  }
	else  /* no background color specified */
	  {
	    mask1 &= ~GCBackground;
	    mask2 &= ~GCForeground;
	  }
      }

    /*
     * if selected, reverse the foreground and background colors
     */
    if (IsSelected(new_flags))
      {
        unsigned long	tmpMsk;
        GC		tmpGC;

	/*
	 * swap the gc's
	 */
	tmpGC  = drawGC;
	drawGC = fillGC;
	fillGC = tmpGC;

	/*
	 * swap the saved values!
	 */
	if (mask1 || mask2)
	  {
	    /*
	     * swap the masks!
	     */
	    tmpMsk = mask1;
	    mask1  = mask2;
	    mask2  = tmpMsk;

	    /*
	     * swap the saved values
	     */
	    gcValues = saved1;
	    saved1   = saved2;
	    saved2   = gcValues;
	  }
      }

    /*
     * if the segment had the hypertext/mark traversal, turn it off
     * this has the added benefit of also turning of the search hit
     * traversal.
     */
    if (HasTraversal(old_flags))
	DrawTraversal (pDAS, x, box_y, strWidth, box_height,
					(old_flags & (~_DtCvTRAVERSAL_FLAG)));

    /*
     * band-aid for Search traversal support (since it isn't there).
     * simply draw a negative search traversal (if the link traversal
     * hasn't already blanked it out) for *every* search hit because
     * it may have had the current search flag and now doesn't.
     */
    else if (pDAS->dtinfo && (new_flags & _DtCvSEARCH_FLAG))
	DrawSearchTraversal(pDAS, x, box_y, strWidth, box_height,
				(new_flags & (~_DtCvTRAVERSAL_FLAG)) |
				_DtCvTRAVERSAL_BEGIN | _DtCvTRAVERSAL_END);

    /*
     * if the item was selected or is selected, we need to draw the
     * inverse video rectangle.
     */
    if (IsSelected(new_flags) || IsSelected(old_flags)
			      || IsMarkOn(new_flags) || IsMarkOn(old_flags))
	image_flag = True;

    /*
     * do we need to allocate the stipple pattern?
     */
    if (IsMarkOn(new_flags) && None == pDAS->stipple)
	pDAS->stipple = XCreateBitmapFromData(dpy, win, root_weave_bits,
					  root_weave_width, root_weave_height);


    /*
     * do we need to draw a rectangle?
     */
    if (image_flag)
      {
	_DtCvUnit eff_x, eff_width;

	/*
	 * do we need to stipple the rectangle for marks?
	 */
	if (pDAS->dtinfo && IsMarkOn(new_flags) && None != pDAS->stipple)
	  {
	    /*
	     * include the fill style and stipple in the mask.
	     * it will be used later to restore the gc.
	     *
	     * Don't bother to reset the stipple pattern. As long
	     * as FillStyle is set to FillSolid (or FillTiled)
	     * the stipple is ignored.
	     */
	    mask2 = mask2 | GCFillStyle;
	    XSetFillStyle(dpy, fillGC, FillOpaqueStippled);
	    XSetStipple(dpy, fillGC, pDAS->stipple);

	    /*
	     * initialize the saved fill_style and stipple fields
	     * to what they should be restored to.
	     */
	    saved2.fill_style = FillSolid;
	  }

	if (HasTraversal(new_flags) && IsTraversalEnd(new_flags))
	    width += pDAS->lineThickness;

	eff_x     = box_x;
	eff_width = width;

	if (pDAS->dtinfo) {
	    /*
	     * search rending info
	     */
	    if (new_flags & _DtCvAPP_FLAG3) {
		eff_x     += pDAS->lineThickness;
		eff_width -= pDAS->lineThickness;
	    }
	    else if (new_flags & _DtCvAPP_FLAG4)
		eff_width += pDAS->lineThickness;
	}

	XFillRectangle (dpy, win, fillGC, eff_x, (int) box_y,
					eff_width, box_height);
      }

    if (strWidth)
      {
        MyDrawString (dpy, win, drawGC,
			(int) font_ptr,
			(int) x, (int) y, string, byte_len,
			wc, False, pDAS->font_info);

	if (pDAS->dtinfo && NULL != pSCD) {
	    _DtCvUnit line_y;
	    if (pSCD->hilite_type & hilite_underline) {
		line_y = box_y + box_height - pDAS->lineThickness / 2;
		XDrawLine (dpy, win, drawGC, x, line_y, x + strWidth, line_y);
	    }
	    if (pSCD->hilite_type & hilite_strikethrough) {
		line_y = box_y + box_height / 2;
		XDrawLine (dpy, win, drawGC, x, line_y, x + strWidth, line_y);
	    }
	    if (pSCD->hilite_type & hilite_overline) {
		line_y = box_y + pDAS->lineThickness / 2;
		XDrawLine (dpy, win, drawGC, x, line_y, x + strWidth, line_y);
	    }
	}

        if (IsHyperLink(new_flags))
          {
	    y   = y + pDAS->underLine;

	    if (IsPopUp(new_flags))
	      {
	        gcValues.line_style = LineOnOffDash;
	        XChangeGC (dpy, drawGC, GCLineStyle, &gcValues);
	      }

            XDrawLine (dpy, win, drawGC, (int) x, (int) y,
						(int) (x + strWidth), (int) y);

	    if (IsPopUp(new_flags))
	      {
	        gcValues.line_style = LineSolid;
	        XChangeGC (dpy, drawGC, GCLineStyle, &gcValues);
	      }
          }
      }

    /*
     * draw the traversal first
     */
    if (HasTraversal(new_flags))
	DrawTraversal (pDAS, x, box_y, strWidth, box_height, new_flags);

    /*
     * draw the search traversal
     */
    if (pDAS->dtinfo && (new_flags & _DtCvAPP_FLAG2)) {
	_DtCvFlags eff_flags = new_flags;
	eff_flags |= _DtCvTRAVERSAL_FLAG;
	if (new_flags & _DtCvSEARCH_BEGIN)
	    eff_flags |= _DtCvTRAVERSAL_BEGIN;
	else
	    eff_flags &= ~_DtCvTRAVERSAL_BEGIN;

	if (new_flags & _DtCvSEARCH_END)
	    eff_flags |= _DtCvTRAVERSAL_END;
	else
	    eff_flags &= ~_DtCvTRAVERSAL_END;

	DrawSearchTraversal(pDAS, x, box_y, strWidth, box_height, eff_flags);
    }

    /*
     * restore the gcs.
     */
    if (mask1)
	XChangeGC(dpy, drawGC, mask1, &saved1);
    if (mask2)
	XChangeGC(dpy, fillGC, mask2, &saved2);
}

/*****************************************************************************
 * Function:	void DADrawSpc ();
 *
 * Parameters:
 *		client_data	Specifies the client specific data
 *				associated with this canvas.
 *		spc_handle	Specifies the handle of the special
 *				character to render.
 *		x		Specifies the x unit at which the
 *				spc is to be rendered.
 *		y		Specifes the y unit at which the baseline
 *				of the spc is to be rendered.
 *		box_x,box_y	Specifies the upper left hand corner of
 *				the bounding box for this string. This
 *				includes any traversal and/or link offsets
 *				relevent for the line this segment occupies.
 *		box_height	Specifies the maximum height of the bounding
 *				box for this string. This includes any
 *				traversal and/or offsets relevent to this
 *				line.
 *		link_type	Specifies the link type if this spc is
 *				part of a link.
 *		old_flags	Specifies .............
 *		new_flags	Specifies .............
 *
 * Returns:
 *
 * Purpose:
 *
 *****************************************************************************/
static void
DADrawSpc (
    _DtCvPointer client_data,
    _DtCvPointer spc_handle,
    _DtCvUnit	 x,
    _DtCvUnit	 y,
    _DtCvUnit	 box_x,
    _DtCvUnit	 box_y,
    _DtCvUnit	 box_height,
    int		 link_type,
    _DtCvFlags	 old_flags,
    _DtCvFlags	 new_flags )
{
    int			  i;
    int			  spcLstIdx = (int) spc_handle;
    _DtCvUnit		  ascent;
    _DtCvUnit		  descent;
    const char		 *spcStr;
    DtHelpDispAreaStruct *pDAS      = (DtHelpDispAreaStruct *) client_data;
    _DtCvStringInfo       strInfo;

    if (pDAS->max_spc == 0)
	return;

    spcStr    = SpcTable[pDAS->spc_chars[spcLstIdx].spc_idx].spc_string;
    _DtHelpDAGetFontMetrics(client_data, pDAS->spc_chars[spcLstIdx].font_ptr,
					&ascent, &descent, NULL, NULL, NULL);

    do
      {
	/*
	 * display the string up to the next newline or backspace
	 */
	i = 0;
	while ('\0' != spcStr[i] && '\n' != spcStr[i] && '\b' != spcStr[i])
	    i++;

	strInfo.string   = (void *) spcStr;
	strInfo.byte_len = i;
	strInfo.wc       = 0;
	strInfo.font_ptr = (_DtCvPointer)(pDAS->spc_chars[spcLstIdx].font_ptr);

        DADrawString (client_data, (_DtCvPointer) &strInfo, x, y,
			spcStr, i, 0,
			pDAS->spc_chars[spcLstIdx].font_ptr, NULL,
			box_x, box_y, box_height, old_flags,
			new_flags);

	/*
	 * if we are looking at a newline character, that means that
	 * the special character takes up several 'lines' of height.
	 * So increment the next y position to go to the next 'line'.
	 */
	if ('\n' == spcStr[i])
	    y  = y + ascent + descent;

	/*
	 * if we are looking at a newline or a backspace, skip over it.
	 */
	if ('\n' == spcStr[i] || '\b' == spcStr[i])
	    i++;

	/*
	 * reset the string pointer.
	 */
	spcStr += i;

	/*
	 * do this while there is any string to display.
	 */
      } while ('\0' != *spcStr);
}

/*****************************************************************************
 * Function:	void DADrawGraphic ();
 *
 * Parameters:
 *
 * Returns:
 *
 * Purpose:
 *
 *****************************************************************************/
static void
DADrawGraphic (
    _DtCvPointer client_data,
    _DtCvPointer graphic_ptr,
    _DtCvUnit	 x,
    _DtCvUnit	 y,
    _DtCvUnit	 box_width,
    _DtCvUnit	 box_height,
    _DtCvFlags	 old_flags,
    _DtCvFlags	 new_flags )
{
    int    width;
    int    height;
    int    old_x;
    int    old_y;
    int    srcX;
    int    srcY;

    DtHelpDispAreaStruct *pDAS = (DtHelpDispAreaStruct *) client_data;
    DtHelpGraphicStruct	 *pGS  = (DtHelpGraphicStruct *)     graphic_ptr;
    Display		 *dpy  = XtDisplay(pDAS->dispWid);
    Window		  win  = XtWindow(pDAS->dispWid);
    XGCValues	 gcValues;

    /*
     * adjust for the scrolled window.
     */
    y = y + pDAS->decorThickness - pDAS->firstVisible;
    x = x + pDAS->decorThickness - pDAS->virtualX;

    /*
     * save away.
     */
    old_x = x;
    old_y = y;

    /*
     * adjust to get the correct y position for the graphic.
     */
    y = y + box_height - pGS->height - 1;

    /*
     * add the line thickness of the traversal to the height.
     */
    if (HasTraversal(old_flags))
	DrawTraversal (pDAS, old_x, old_y, pGS->width, box_height,
			(old_flags & (~_DtCvTRAVERSAL_FLAG)));

    srcX   = 0;
    srcY   = 0;
    width  = pGS->width;
    height = 0;

    /*
     * Check and adjust the width of the graphic if it is
     * to be displayed
     */
    if (x < 0)
      {
	srcX  = srcX - x;
	width = width + x;
	x     = 0;
      }

    if (width > 0)
      {
	height = pGS->height;

	if (y < pDAS->decorThickness)
	  {
	    srcY    = pDAS->decorThickness - y;
	    height -= srcY;
	    y       = pDAS->decorThickness;
	  }
	if (y + height > ((int)(pDAS->dispHeight - pDAS->decorThickness)))
	    height = pDAS->dispHeight - pDAS->decorThickness - y;
      }

    /*
     * If we found something to write out,
     * height and width will/should be greater than zero.
     */
    if (height > 0 && width > 0)
	XCopyArea (dpy, pGS->pix, win, pDAS->normalGC,
					srcX, srcY, width, height, x, y);


    if (HasTraversal(new_flags))
	DrawTraversal (pDAS, old_x, old_y, pGS->width, box_height, new_flags);

    else if (IsHyperLink(new_flags))
      {
	int x,y;

	XSetFillStyle(dpy, pDAS->normalGC, FillTiled);

	if (IsPopUp(new_flags))
	  {
	    gcValues.line_style = LineOnOffDash;
	    XChangeGC (dpy, pDAS->normalGC, GCLineStyle, &gcValues);
	  }

	x   = old_x + 2;
	y   = old_y + pDAS->lineThickness / 2;

	XDrawLine(dpy, win, pDAS->normalGC, x, y, x + box_width - 4, y);

	y   = old_y + box_height - pDAS->lineThickness
						+ pDAS->lineThickness / 2;

	XDrawLine(dpy, win, pDAS->normalGC, x, y, x + box_width - 4, y);

	y   = old_y + 2;
	x   = old_x - pDAS->lineThickness + pDAS->lineThickness / 2;

	XDrawLine(dpy, win, pDAS->normalGC, x, y, x, y + box_height - 4);

	x   = old_x + box_width + pDAS->lineThickness / 2;

	XDrawLine(dpy, win, pDAS->normalGC, x, y, x, y + box_height - 4);

	if (IsPopUp(new_flags))
	  {
	    gcValues.line_style = LineSolid;
	    XChangeGC (dpy, pDAS->normalGC, GCLineStyle, &gcValues);
	  }
	XSetFillStyle(dpy, pDAS->normalGC, FillSolid);
      }
}

/*****************************************************************************
 * Function:	_DtCvPointer DABuildSelectedString ();
 *
 * Parameters:
 *
 * Returns:
 *
 * Purpose:
 *
 *****************************************************************************/
static _DtCvStatus
DABuildSelectedString (
    _DtCvPointer	 client_data,
    _DtCvPointer	*prev_info,
    const char		*string,
    int			 byte_len,
    int			 wc,
    _DtCvPointer	 font_ptr,
    _DtCvUnit		 space,
    _DtCvFlags		 flags )
{
    int    i;
    int    j;
    int    mbLen;
    int    totalLen;
    int    spaceNum  = 0;
    int    stringLen = 0;
    long   spaceLong = space * 10;
    char  *myString  = (char *)(*prev_info);
    DtHelpDispAreaStruct *pDAS = (DtHelpDispAreaStruct *) client_data;

    if (spaceLong > 0)
	spaceNum = (int) (spaceLong / pDAS->charWidth +
		((spaceLong % pDAS->charWidth) < pDAS->charWidth / 2 ? 0 : 1));

    if (string == NULL)
	byte_len = 0;

    /*
     * if we are processing a wide character string, we need to
     * allocate MB_CUR_MAX bytes per character
     */
    mbLen = 1;
    if (0 != wc)
	mbLen = MB_CUR_MAX;

    totalLen = byte_len * mbLen + spaceNum + 1 + (IsEndOfLine(flags) ? 1 : 0);

    /*
     * allocate the string
     */
    if (myString == NULL)
	myString = (char *) malloc (sizeof(char) * totalLen);
    else
      {
	stringLen = strlen(myString);
	myString  = (char *) realloc ((void *) myString,
				(sizeof(char) * (stringLen + totalLen)));
      }

    if (myString == NULL)
	return _DtCvSTATUS_BAD;

    /*
     * tack on the leading spaces.
     */
    for (i = stringLen; spaceNum > 0; spaceNum--)
	myString[i++] = ' ';

    /*
     * now tack on the characters.
     */
    if (0 != wc)
      {
	/*
	 * have to translate the wide character into a multi-byte
	 * character.
	 */
	wchar_t  *wStr = (wchar_t *) string;

	for (j = 0; 0 < byte_len; byte_len--)
	  {
	    /*
	     * convert the next character
	     */
	    mbLen = wctomb(&myString[i], wStr[j++]);

	    /*
	     * if a valid character, increase the destination pointer
	     */
	    if (0 <= mbLen)
		i += mbLen;
	  }
      }
    else
	/*
	 * transfer the single byte string to the destination string
	 */
        for (j = 0; byte_len > 0 && string[j] != '\0'; byte_len--)
	    myString[i++] = string[j++];

    if (flags)
	myString[i++] = '\n';

    myString[i] = '\0';

    *prev_info = (_DtCvPointer) myString;

    return _DtCvSTATUS_OK;
}

/*****************************************************************************
 * Function:	_DtCvPointer DABuildSelectedSpc ();
 *
 * Parameters:
 *
 * Returns:
 *
 * Purpose:
 *
 *****************************************************************************/
static _DtCvStatus
DABuildSelectedSpc (
    _DtCvPointer	 client_data,
    _DtCvPointer	*prev_info,
    _DtCvPointer	 spc_handle,
    _DtCvUnit		 space,
    _DtCvFlags		 flags )
{
    int    spcIdx = (int) spc_handle;
    int    i;
    int    totalLen;
    int    spaceNum  = 0;
    int    stringLen = 0;
    long   spaceLong = space * 10;
    const char *spcStr;
    char  *myString  = (char *)(*prev_info);
    DtHelpDispAreaStruct *pDAS = (DtHelpDispAreaStruct *) client_data;

    /*
     * find the ascii version of this spc
     */
    spcIdx = pDAS->spc_chars[spcIdx].spc_idx;
    spcStr = _DtHelpCeResolveSpcToAscii(SpcTable[spcIdx].symbol);

    if (spcStr == NULL)
	return _DtCvSTATUS_BAD;

    /*
     * get the length to realloc
     */
    totalLen = strlen(spcStr);

    if (spaceLong > 0)
	spaceNum = (int)(spaceLong / pDAS->charWidth +
		((spaceLong % pDAS->charWidth) < pDAS->charWidth / 2 ? 0 : 1));

    totalLen = totalLen + spaceNum + 1 + (IsEndOfLine(flags) ? 1 : 0);

    if (myString == NULL)
	myString = (char *) malloc (sizeof(char) * totalLen);
    else
      {
	stringLen = strlen(myString);
	myString  = (char *) realloc ((void *) myString,
				(sizeof(char) * (stringLen + totalLen)));
      }

    if (myString == NULL)
	return _DtCvSTATUS_BAD;

    for (i = stringLen; spaceNum > 0; spaceNum--)
	myString[i++] = ' ';

    while (*spcStr != '\0')
	myString[i++] = *spcStr++;

    if (flags)
	myString[i++] = '\n';

    myString[i] = '\0';

    *prev_info = (_DtCvPointer) myString;
    return _DtCvSTATUS_OK;
}

/*****************************************************************************
 *		Semi-Public Functions
 *****************************************************************************/
/*****************************************************************************
 * Function:	char * _DtHelpDAGetSpcString ();
 *
 * Parameters:
 *
 * Returns:
 *
 * Purpose:
 *
 *****************************************************************************/
const char *
_DtHelpDAGetSpcString (
    int		idx)
{
    return (SpcTable[idx].spc_string);
}

/*****************************************************************************
 * Function:	void _DtHelpDAResolveFont ();
 *
 * Parameters:
 *
 * Returns:
 *
 * Purpose:
 *
 *****************************************************************************/
void
_DtHelpDAResolveFont (
    _DtCvPointer	 client_data,
    char		*lang,
    const char		*charset,
    _DtHelpFontHints	 font_attr,
    _DtCvPointer	*ret_font )
{
    /*
     * note - if the mod_string comes in with a NULL pointer,
     * it came from our own call rather than the Core Engine.
     */
    int idx;

    (void) ResolveFont(client_data, lang, charset, font_attr, &idx);
    *ret_font = (_DtCvPointer) idx;
}

/*****************************************************************************
 * Function:	_DtCvStatus _DtHelpDAResolveSpc ();
 *
 * Parameters:
 *
 * Returns:
 *
 * Purpose:
 *
 *****************************************************************************/
_DtCvStatus
_DtHelpDAResolveSpc (
    _DtCvPointer	 client_data,
    char		*lang,
    const char		*charset,
    _DtHelpFontHints	 font_attr,
    const char		*spc_symbol,
    _DtCvPointer	*ret_handle,
    _DtCvUnit		*ret_width,
    _DtCvUnit		*ret_height,
    _DtCvUnit		*ret_ascent)
{
    register int          i = 0;
    int			  result = -1;
    int			  fontIdx;
    int			  spcTbIdx = 0;
    _DtCvUnit		  ascent;
    _DtCvUnit		  descent;
    const char		 *newSet;
    const char		 *spcStr;
    DtHelpDispAreaStruct *pDAS = (DtHelpDispAreaStruct *) client_data;
    _DtCvStringInfo       strInfo;

    while (SpcTable[spcTbIdx].symbol != NULL &&
			strcmp(SpcTable[spcTbIdx].symbol, spc_symbol) != 0)
	spcTbIdx++;

    newSet = SpcTable[spcTbIdx].spc_value;
    spcStr = SpcTable[spcTbIdx].spc_string;

    result = ResolveFont(client_data, lang, newSet, font_attr, &fontIdx);

    if (result == 0)
      {
	int		 spcLstIdx = 0;
	_DtCvUnit	 maxWidth;
	_DtHelpDARegion *pReg;

        pReg = (_DtHelpDARegion *) malloc (sizeof(_DtHelpDARegion));
	if (NULL == pReg)
	    return -1;

	while (spcLstIdx < pDAS->cur_spc &&
				pDAS->spc_chars[spcLstIdx].spc_idx != -1)
	    spcLstIdx++;

	if (spcLstIdx == pDAS->cur_spc)
	  {
	    if (pDAS->cur_spc >= pDAS->max_spc)
	      {
		pDAS->max_spc += GROW_SIZE;
		if (pDAS->cur_spc > 0)
		    pDAS->spc_chars = (DtHelpSpecialChars *) realloc(
				(void *)pDAS->spc_chars,
				(sizeof(DtHelpSpecialChars)*pDAS->max_spc));
		else
		    pDAS->spc_chars = (DtHelpSpecialChars *) malloc(
				(sizeof(DtHelpSpecialChars)*pDAS->max_spc));

		if (pDAS->spc_chars == NULL)
		    return -1;

		for (i = pDAS->cur_spc; i < pDAS->max_spc; i++)
		    pDAS->spc_chars[i].spc_idx = -1;
	      }
	    pDAS->cur_spc++;
	  }

	pDAS->spc_chars[spcLstIdx].font_ptr = (_DtCvPointer)fontIdx;
	pDAS->spc_chars[spcLstIdx].spc_idx  = spcTbIdx;

	/*
	 * fill in the region information
	 */
	pReg->inited = True;
	pReg->type   = _DtHelpDASpc;
	pReg->handle = (_DtCvPointer) spcLstIdx;

	/*
	 * fill out the return information
	 */
	*ret_handle = (_DtCvPointer) pReg;

	*ret_height = 0;
	*ret_width  = 0;

	/*
	 * get the maximum ascent and descent for the font. Set the return
	 * ascent.
	 */
	_DtHelpDAGetFontMetrics(client_data, (_DtCvPointer)fontIdx,
					&ascent, &descent, NULL, NULL, NULL);
	*ret_ascent = ascent;

	/*
	 * now go through the string and get the maximum width
	 * and total height of the special character.
	 */
	do
	  {
	    /*
	     * find the first 'break' in the string sequence.
	     */
	    i = 0;
	    while ('\0' != spcStr[i] && '\n' != spcStr[i] && '\b' != spcStr[i])
		i++;

	    /*
	     * set the height if it is zero or if this is a multi-height
	     * special character.
	     */
	    if ('\n' == spcStr[i] || 0 == *ret_height)
	        *ret_height = *ret_height + ascent + descent;

	    /*
	     * now get the maximum width of the special character.
	     */
	    strInfo.string   = spcStr;
	    strInfo.byte_len = i;
	    strInfo.wc       = 0;
	    strInfo.font_ptr = (_DtCvPointer)fontIdx;

	    maxWidth  = _DtHelpDAGetStrWidth (client_data, _DtCvSTRING_TYPE,
						(_DtCvPointer) &strInfo);
	    if (*ret_width < maxWidth)
		*ret_width = maxWidth;

	    /*
	     * if this is a multi-height or a composite character
	     * skip the newline or backspace.
	     */
	    if ('\n' == spcStr[i] || '\b' == spcStr[i])
		i++;

	    /*
	     * reset the string pointer
	     */
	    spcStr += i;
	  } while (*spcStr != '\0');
	result = 0;
      }
    else if (spc_symbol != DefaultStr)
	result = _DtHelpDAResolveSpc (client_data, lang, charset,
					font_attr, DefaultStr,
					ret_handle, ret_width,
					ret_height, ret_ascent);
    return result;
}


/*****************************************************************************
 * Function:	_DtCvStatus GraphicLoad ();
 *
 * Parameters:
 *
 * Returns:
 *
 * Purpose:
 *
 *****************************************************************************/
_DtCvStatus
_DtHelpDALoadGraphic (
    _DtCvPointer client_data,
    char	*vol_xid,
    char	*topic_xid,
    char	*file_xid,
    char	*format,
    char	*method,
    _DtCvUnit	*ret_width,
    _DtCvUnit	*ret_height,
    _DtCvPointer *ret_region)
{
    DtHelpDispAreaStruct *pDAS = (DtHelpDispAreaStruct *) client_data;
    DtHelpGraphicStruct	 *pGS;
    _DtHelpDARegion      *pReg;
    Widget		  wid;
    Window		  win;
    char		 *ptr;
    char		 *extptr;
    char		 *fileName = file_xid;        
    Screen               *retScr;
    int                  screen;

    pGS  = (DtHelpGraphicStruct *) malloc (sizeof(DtHelpGraphicStruct));
    pReg = (_DtHelpDARegion     *) malloc (sizeof(_DtHelpDARegion));
    if (pGS == NULL || NULL == pReg)
      {
	if (NULL != pGS)
	    free(pGS);
	if (NULL != pReg)
	    free(pReg);
        return -1;
      }

    wid = pDAS->dispWid;
    retScr = XtScreen(wid);
    screen = XScreenNumberOfScreen(retScr);

    while (!XtIsRealized(wid) && XtParent(wid) != NULL)
	wid = XtParent(wid);

    win = XtWindow(wid);
    if (!XtIsRealized(wid))
	win = RootWindow(XtDisplay(pDAS->dispWid), screen);

    if (fileName != NULL && *fileName != '/')
      {
	fileName = (char *) malloc (strlen(vol_xid) + strlen (file_xid) + 2);
	if (fileName == NULL)
	    return -1;

	strcpy(fileName, vol_xid);

	if (_DtHelpCeStrrchr(fileName, "/", MB_CUR_MAX, &ptr) != -1)
	    *ptr = '\0';

	strcat(fileName, "/");
	strcat(fileName, file_xid);
      }

    /*
     * Find out if this is a X Pixmap graphic and set flag if it is.
     * This will be used later when/if colors need to be freed.
     */
    if (fileName != NULL && _DtHelpCeStrrchr(fileName, ".", MB_CUR_MAX, &extptr) != -1)
    	if (strcmp (extptr, ".xpm") == 0 || strcmp (extptr, ".pm") == 0)
		pGS->used = -1;

    if (pDAS->context == NULL)
    {
        pDAS->context = (_DtGrContext *) XtMalloc(sizeof(_DtGrContext));
        pDAS->context->image_type = NULL;
        pDAS->context->context = NULL;
    }

    pGS->pix = _DtHelpProcessGraphic (XtDisplay(pDAS->dispWid), win,
				XScreenNumberOfScreen(XtScreen(pDAS->dispWid)),
				pDAS->depth,
				pDAS->pixmapGC,
				&(pDAS->def_pix),
				&(pDAS->def_pix_width),
				&(pDAS->def_pix_height),
				pDAS->context,
				pDAS->colormap,
				pDAS->visual,
				pDAS->foregroundColor,
				pDAS->backgroundColor, fileName,
				pDAS->media_resolution,
				&(pGS->width), &(pGS->height), &(pGS->mask),
				&(pGS->pixels), &(pGS->num_pixels));

    if (pDAS->context->image_type == NULL)
    {
        XtFree((char *) pDAS->context);
        pDAS->context = NULL;
    }

    if (fileName != file_xid)
	free (fileName);

    if (pGS->pix == 0)
      {
	free(pReg);
	free(pGS);
	return -1;
      }

    /*
     * fill in the region information
     */
    pReg->inited = True;
    pReg->type   = _DtHelpDAGraphic;
    pReg->handle = (_DtCvPointer) pGS;

    /*
     * fill out the return information
     */
    *ret_width  = pGS->width;
    *ret_height = pGS->height;
    *ret_region = (_DtCvPointer) pReg;

    return 0;
}

/*****************************************************************************
 * Function:	void _DtHelpDADestroyGraphic ();
 *
 * Parameters:
 *
 * Returns:
 *
 * Purpose:
 *
 *****************************************************************************/
void
_DtHelpDADestroyGraphic (
    _DtCvPointer	client_data,
    _DtCvPointer	graphic_ptr )
{
    Arg   args[5];

    Colormap colormap;

    DtHelpDispAreaStruct *pDAS = (DtHelpDispAreaStruct *) client_data;
    DtHelpGraphicStruct	 *pGS  = (DtHelpGraphicStruct *)     graphic_ptr;
    Display		 *dpy  = XtDisplay(pDAS->dispWid);

    if (pGS->pix != pDAS->def_pix)
	XFreePixmap(dpy, pGS->pix);

    if (pGS->mask != None)
	XFreePixmap(dpy, pGS->mask);

    if (pGS->num_pixels)
      {
	XtSetArg (args[0], XmNcolormap, &colormap);
	XtGetValues (pDAS->dispWid, args, 1);

	/*
	 * This will only be set to -1 if the original graphic was an X Pixmap,
	 * <filename>.pm or <filename>.xpm.
	 */
	if (pGS->used != -1)
		XFreeColors (dpy, colormap, pGS->pixels, pGS->num_pixels, 0);

	free((void *) pGS->pixels);
      }

    free((char *) pGS);
}

/*****************************************************************************
 * Function:	void _DtHelpDADestroySpc ();
 *
 * Parameters:
 *
 * Returns:
 *
 * Purpose:
 *
 *****************************************************************************/
void
_DtHelpDADestroySpc (
    _DtCvPointer	client_data,
    _DtCvPointer	spc_handle)
{
    int			  spc_index = (int) spc_handle;
    DtHelpDispAreaStruct *pDAS      = (DtHelpDispAreaStruct *) client_data;

    if (pDAS->max_spc == 0)
	return;

    pDAS->spc_chars[spc_index].spc_idx = -1;
}

/*****************************************************************************
 * Function:	void _DtHelpDADestroyRegion ();
 *
 * Parameters:
 *
 * Returns:
 *
 * Purpose:
 *
 *****************************************************************************/
void
_DtHelpDADestroyRegion (
    _DtCvPointer	client_data,
    _DtCvPointer	reg_info)
{
    _DtHelpDARegion      *pReg = (_DtHelpDARegion *) reg_info;

    if (True == pReg->inited)
      {
	if (_DtHelpDASpc == pReg->type)
	    _DtHelpDADestroySpc(client_data, pReg->handle);
	else if (_DtHelpDAGraphic == pReg->type)
	    _DtHelpDADestroyGraphic(client_data, pReg->handle);
      }
    else if (False == pReg->inited && _DtHelpDASpc == pReg->type)
      {
	_DtHelpDASpcInfo *pSpc = (_DtHelpDASpcInfo *) pReg->handle;

	free(pSpc->name);
	_DtHelpFreeFontHints(&(pSpc->spc_fonts));
	free(pReg->handle);
      }

    free (pReg);
}

/*****************************************************************************
 * Function:	Widget _DtHelpDATocMarker ();
 *
 * Parameters:
 *		client_data	Specifies the display area pointer.
 *
 * Returns: nothing
 *
 * Purpose: Turns on/off the toc marker. This includes setting the correct
 *          bit in pDAS->toc_flag and doing a physical XDraw or XClearArea.
 *
 *****************************************************************************/
void
_DtHelpDATocMarker (
    XtPointer	 client_data,
    Boolean	 draw)
{
    DtHelpDispAreaStruct *pDAS = (DtHelpDispAreaStruct *) client_data;

    if (pDAS->toc_flag & _DT_HELP_SHADOW_TRAVERSAL)
      {
        if (False == draw)
          {
	    if (XtIsRealized(pDAS->dispWid))
	        ClearTocMarker(pDAS, 0);
	    TurnTocOff(pDAS->toc_flag);
          }
        else if (True == draw)
          {
	    TurnTocOn(pDAS->toc_flag);
	    DrawTocIndicator(pDAS, 0);
          }
      }
}

/*****************************************************************************
 *		Virtual UI Functions
 *****************************************************************************/
/*****************************************************************************
 * Function:	void _DtHelpDAGetCvsMetrics ();
 *
 * Parameters:
 *
 * Returns:
 *
 * Purpose:
 *
 *****************************************************************************/
void
_DtHelpDAGetCvsMetrics (
    _DtCvPointer		 client_data,
    _DtCvElemType	elem_type,
    _DtCvPointer	ret_metrics)
{
    DtHelpDispAreaStruct *pDAS = (DtHelpDispAreaStruct *) client_data;
    _DtCvSpaceMetrics	 *retSpace = (_DtCvSpaceMetrics *) ret_metrics;

    if (_DtCvCANVAS_TYPE == elem_type)
      {
	_DtCvMetrics  *retCanvas = (_DtCvMetrics *) ret_metrics;

        retCanvas->width          = pDAS->dispUseWidth;
        retCanvas->height         = pDAS->dispUseHeight;
	retCanvas->top_margin     = pDAS->marginHeight;
	retCanvas->side_margin    = pDAS->marginWidth;
	retCanvas->line_height    = pDAS->lineHeight;
	retCanvas->horiz_pad_hint = pDAS->charWidth / 10;
      }

    else if (_DtCvLINK_TYPE == elem_type)
      {
	retSpace->space_before = 0;
	retSpace->space_after  = 0;
	retSpace->space_above  = 0;
	retSpace->space_below  = 0;
      }
    else if (_DtCvTRAVERSAL_TYPE == elem_type)
      {
	retSpace->space_before = pDAS->lineThickness;
	if (pDAS->toc_flag & _DT_HELP_SHADOW_TRAVERSAL)
	  {
	    if (pDAS->toc_flag & _DT_HELP_NOT_INITIALIZED)
	      {
	        GetTocIndicator(pDAS);
	        pDAS->toc_flag &= (~(_DT_HELP_NOT_INITIALIZED));
	      }
	    retSpace->space_before += pDAS->toc_width;
	  }

	retSpace->space_after  = pDAS->lineThickness;
	retSpace->space_above  = pDAS->lineThickness;
	retSpace->space_below  = 0;
      }
    else if (_DtCvLOCALE_TYPE == elem_type)
      {
	_DtCvLocale  *retLocale = (_DtCvLocale *) ret_metrics;

	retLocale->line_wrap_mode = _DtCvModeWrapNone;
	retLocale->cant_begin_chars = pDAS->cant_begin_chars;
	retLocale->cant_end_chars   = pDAS->cant_end_chars;
      }
}

/*****************************************************************************
 * Function:	void _DtHelpDAGetFontMetrics ();
 *
 * Parameters:
 *
 * Returns:
 *
 * Purpose:
 *
 *****************************************************************************/
void
_DtHelpDAGetFontMetrics (
    _DtCvPointer	 client_data,
    _DtCvPointer	 font_ptr,
    _DtCvUnit		*ret_ascent,
    _DtCvUnit		*ret_descent,
    _DtCvUnit		*ret_width,
    _DtCvUnit		*ret_super,
    _DtCvUnit		*ret_sub)
{
    DtHelpDispAreaStruct *pDAS = (DtHelpDispAreaStruct *) client_data;

    __DtHelpFontMetrics (pDAS->font_info, (int) font_ptr,
		ret_ascent, ret_descent, ret_width, ret_super, ret_sub);
}

/*****************************************************************************
 * Function:	_DtCvUnit _DtHelpDAGetStrWidth ();
 *
 * Parameters:
 *
 * Returns:
 *
 * Purpose:
 *
 *****************************************************************************/
_DtCvUnit
_DtHelpDAGetStrWidth (
    _DtCvPointer	client_data,
    _DtCvElemType	elem_type,
    _DtCvPointer	data)
{
    DtHelpDispAreaStruct *pDAS = (DtHelpDispAreaStruct *) client_data;
    int   length;
    int   font_index;
    _DtCvStringInfo *strInfo;

    if (elem_type != _DtCvSTRING_TYPE)
	return 0;

    /*
     * cast the data to a string information structure.
     */
    strInfo = (_DtCvStringInfo *) data;

    /*
     * If the 'font_index' is a negative number, it indicates a font set
     * was opened for this string.  Use the X11R5 I18N routine to figure
     * out its length.  Otherwise use the standard X11 text width call.
     */
    font_index = (int) strInfo->font_ptr;
    if (font_index < 0)
      {
	if (0 == strInfo->wc)
	length = XmbTextEscapement(
			__DtHelpFontSetGet(pDAS->font_info, font_index),
				(char *) strInfo->string, strInfo->byte_len);
    else
	    length = XwcTextEscapement(
			__DtHelpFontSetGet(pDAS->font_info, font_index),
				(wchar_t *) strInfo->string, strInfo->byte_len);
      }
    else
        length = XTextWidth(__DtHelpFontStructGet(pDAS->font_info, font_index),
				(char *) strInfo->string, strInfo->byte_len);

   return ((_DtCvUnit) length);
}

/*****************************************************************************
 * Function:	void _DtHelpDARenderElem ();
 *
 * Parameters:
 *		client_data	Specifies the client specific data
 *				associated with this canvas.
 *		elem_type	Specifies the element type that will be
 *				rendered.
 *		x		Specifies the x unit at which the
 *				string is to be rendered.
 *		y		Specifes the y unit at which the baseline
 *				of the string is to be rendered.
 *		link_type	Specifies the link type if this element is
 *				part of a link.
 *		old_flags	Specifies the old flags.
 *		new_flags	Specifies the new flags.
 *		data		Contains a pointer to the element specific
 *				structures required for rendering.
 *
 * Returns: Nothing
 *
 * Purpose:
 *
 *****************************************************************************/
void
_DtHelpDARenderElem (
    _DtCvPointer	client_data,
    _DtCvElemType	elem_type,
    _DtCvUnit		x,
    _DtCvUnit		y,
    int			link_type,
    _DtCvFlags		old_flags,
    _DtCvFlags		new_flags,
    _DtCvElemType	trav_type,
    _DtCvPointer	trav_data,
    _DtCvPointer	data )
{
    _DtCvStringInfo *strInfo;
    _DtCvLineInfo   *lnInfo  = (_DtCvLineInfo *) data;
    _DtCvRenderInfo *posInfo = (_DtCvRenderInfo *) data;

    /*
     * nothing is done with trav_type or trav_data for DtHelp.
     */

    /*
     * render the element based on its type.
     */
    if (_DtCvSTRING_TYPE == elem_type)
      {
	strInfo = (_DtCvStringInfo *) posInfo->info;
	DADrawString(client_data, posInfo->info, x, y, strInfo->string,
				strInfo->byte_len, strInfo->wc,
				strInfo->font_ptr, strInfo->csd,
				posInfo->box_x, posInfo->box_y,
				posInfo->box_height,
				old_flags, new_flags);
      }
    else if (_DtCvLINE_TYPE == elem_type)
      {
	DADrawLine(client_data, x, y, lnInfo->x2, lnInfo->y2, lnInfo->width);
      }
    else if (_DtCvREGION_TYPE == elem_type)
      {
	_DtHelpDARegion *regInfo = (_DtHelpDARegion *) posInfo->info;

	if (_DtHelpDAGraphic == regInfo->type)
	  {
	    DADrawGraphic(client_data, regInfo->handle,
				posInfo->box_x,
				posInfo->box_y,
				posInfo->box_width,
				posInfo->box_height,
				old_flags, new_flags);
	  }
	else if (_DtHelpDASpc == regInfo->type)
	  {
	    DADrawSpc (client_data, regInfo->handle, x, y,
				posInfo->box_x   ,
				posInfo->box_y   , posInfo->box_height,
				link_type        , old_flags,
				new_flags);
	  }
      }
}

/*****************************************************************************
 * Function:	_DtCvStatus _DtHelpDABuildSelection ();
 *
 * Parameters:
 *		client_data	Specifies the client specific data
 *				associated with this canvas.
 *		elem_type	Specifies the element type that will be
 *				rendered.
 *		x		Specifies the x unit at which the
 *				string is to be rendered.
 *		y		Specifes the y unit at which the baseline
 *				of the string is to be rendered.
 *		link_type	Specifies the link type if this element is
 *				part of a link.
 *		old_flags	Specifies the old flags.
 *		new_flags	Specifies the new flags.
 *		data		Contains a pointer to the element specific
 *				structures required for rendering.
 *
 * Returns: Nothing
 *
 * Purpose:
 *
 *****************************************************************************/
_DtCvStatus
_DtHelpDABuildSelection (
    _DtCvPointer	 client_data,
    _DtCvElemType	 elem_type,
    unsigned int	 mask,
    _DtCvPointer	*prev_info,
    _DtCvUnit		 space,
    _DtCvUnit		 width,
    _DtCvFlags		 flags,
    _DtCvPointer	 data)
{
    _DtCvStatus	result = _DtCvSTATUS_NONE;

    if (_DtCvSTRING_TYPE == elem_type)
      {
        _DtCvStringInfo *strInfo = (_DtCvStringInfo *) data;

	result = DABuildSelectedString(client_data, prev_info,
				strInfo->string  , strInfo->byte_len,
				strInfo->wc,
				strInfo->font_ptr, space, flags);
      }
    else if (_DtCvREGION_TYPE == elem_type)
      {
	_DtHelpDARegion *regInfo = (_DtHelpDARegion *) data;

	if (_DtHelpDASpc == regInfo->type)
	    result = DABuildSelectedSpc (client_data, prev_info,
				regInfo->handle, space, flags);
      }

    return result;
}
