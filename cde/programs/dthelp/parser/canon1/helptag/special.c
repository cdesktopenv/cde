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
/* $XConsortium: special.c /main/3 1995/11/08 09:32:32 rswiston $ */
/****************************************************************************/
/*                                                                          */
/*   This program generates either one or both of two include files needed  */
/*   for other programs.                                                    */
/*                                                                          */
/*   For the portion of HP Tag/Vectra that transforms characters into       */
/*   their TeX equivalents, this program produces a file with entries that  */
/*   consist of a comment field and either a 0 or a string entry.  Within   */
/*   the comment field is the character's eight-bit character code plus     */
/*   the name of the character, as:                                         */
/*                                                                          */
/*       0: ^@  NUL                                                         */
/*       60: <                                                              */
/*       65: A                                                              */
/*      159: undefined                                                      */
/*      161: A   grave                                                      */
/*                                                                          */
/*   and so on.  The string entry is of the form:                           */
/*                                                                          */
/*     "\`A",                                                               */
/*     "\^A",                                                               */
/*    "{\bslash}",                                                          */
/*                                                                          */
/*   and so on.                                                             */
/*                                                                          */
/*   For the collating program used with indexing, this program produces a  */
/*   file with entries that consist of a comment field, a number            */
/*   representing the collating position of the character, a "handling"     */
/*   field, and the character code of the lowercase representation of       */
/*   those characters that also have uppercase representations.  The        */
/*   "handling" field is one of NORMAL, SPECIAL, or UNSUPP.  Here are       */
/*   examples showing the fields:                                           */
/*                                                                          */
/*   *  63: ?                               *  54, SPECIAL,  63,            */
/*   *  64: @                               *  55, SPECIAL,  64,            */
/*   *  65: A                               *  92, NORMAL,  97,             */
/*   * 161: A   grave                       *  94, NORMAL, 200,             */
/*   * 162: A   circumflex                  *  93, NORMAL, 192,             */
/*   * 200: a   grave                       *  86, NORMAL, 200,             */
/*                                                                          */
/*   The question mark has ROMAN8 character code 63.  Its collating         */
/*   position as defined for the indexing program is 54.  (That is, it      */
/*   collates just before the at-sign.)  It is given SPECIAL handling by    */
/*   the collating program.  Since it has no lowercase representation, the  */
/*   number in this position is the same as its ROMAN8 character code.      */
/*   The uppercase A has ROMAN8 character code 65.  Its collating position  */
/*   as defined for the indexing program is 92.  (That is, it collates      */
/*   just before A circumflex.)  It is given NORMAL handling by the         */
/*   collating program.  Its lowercase representation is 200, the same as   */
/*   lowercase a grave.                                                     */
/*                                                                          */
/*   The collating order for this program is specified in SPECIAL.H.  The   */
/*   collating order to be used by the indexing program can be changed by   */
/*   modifying SPECIAL.H and recompiling and rerunning this program.  For   */
/*   example, at the moment, two of the lines in SPECIAL.H are:             */
/*                                                                          */
/*    162, "A   circumflex",                  NORMAL, "\\^A", 192,          */
/*    161, "A   grave",                       NORMAL, "\\`A", 200,          */
/*                                                                          */
/*   Thus, A circumflex is specified to collate before A grave.  If this    */
/*   changes, and it is determined that A grave should collate first, it    */
/*   would be necessary only to switch the lines, so they look like this:   */
/*                                                                          */
/*    161, "A   grave",                       NORMAL, "\\`A", 200,          */
/*    162, "A   circumflex",                  NORMAL, "\\^A", 192,          */
/*                                                                          */
/*   In SPECIAL.H, only those characters that have ROMAN8 representations   */
/*   are listed.  The program generates the undefined positions in both     */
/*   outputs.                                                               */
/*                                                                          */
/*   To output only the include file for generating TeX characters, run     */
/*   this program with the "s" switch:                                      */
/*                                                                          */
/*     SPECIAL S > output_file_name                                         */
/*                                                                          */
/*   To output only the include file for generating the collating           */
/*   sequence, run this program with the "c" switch:                        */
/*                                                                          */
/*     SPECIAL C > output_file_name                                         */
/*                                                                          */
/*   To output both, run this program with the "b" switch, or with no       */
/*   specification:                                                         */
/*                                                                          */
/*     SPECIAL B > output_file_name                                         */
/*     SPECIAL > output_file_name                                           */
/*                                                                          */
/*   In all cases, if you specify no redirected file name, the output goes  */
/*   to the Vectra's screen.                                                */
/*                                                                          */
/****************************************************************************/

#define NORMAL  1
#define SPECIAL 2
#define UNSUPP  3

#define TRUE    1
#define FALSE   0

#include <stdio.h>
#include <stdlib.h>

typedef wchar_t M_WCHAR;

struct {
  int charcode;
  char *name ;
  int handling;
  char *texexpand ;
  int lower;
  } specials[] = {
#include "special.h"
    } ;

#define NOSPECIALS sizeof(specials)/sizeof(specials[0])
#define NO8BITCHARS 256
int invert[NO8BITCHARS], emptycell;

int
main(int argc, char *argv[])
{
int i, both = FALSE, spec = FALSE, coll = FALSE, error = FALSE ;

if (argc < 3)
    { /*  Either one argument or none; not more */
    if (argc == 2)
	{
	if (!stricmp(argv[1], "b")) both = TRUE;
	else if (!stricmp(argv[1], "s")) spec = TRUE;
	else if (!stricmp(argv[1], "c")) coll = TRUE;
	else error = TRUE;
	}
    else both = TRUE;
    }
else error = TRUE;
if (error)
    {
    fprintf(stderr,"Usage: SPECIAL [b | s | c]\n");
    exit (0);
    }
for (i = 0 ; i < NOSPECIALS ; i++)
invert[specials[i].charcode] = i + 1;
if (both || spec)
    {
    printf("/* Char Name                        TeX Expansion */\n");     
    printf("/*                                                */\n");     
    for (i = 0 ; i < NO8BITCHARS ; i++)
	{
	if (invert[i])
	    {
	    printf("/* %3d: %-31s */ ", i, specials[invert[i] - 1].name) ;
	    if (specials[invert[i] - 1].texexpand)
		printf("\"%s\"", specials[invert[i] - 1].texexpand) ;
	    else putchar('0') ;
	    }
	else printf("/* %3d: undefined                       */ 0", i) ;
	if (i < NO8BITCHARS - 1) putchar(',') ;
	putchar('\n') ;
	}
    }
if (both || coll)
    {
    printf(
    "/*                                                             */\n");     
    printf(
    "/* Char Name                               Coll Status   Lower */\n");     
    printf(
    "/*                                                             */\n");     
    for (i = 0, emptycell = NOSPECIALS + 1; i < NO8BITCHARS ; i++)
	{
	if (invert[i])
	    {
	    printf("/* %3d: %-31s */ %3d, ",
	      specials[invert[i] - 1].charcode,
	      specials[invert[i] - 1].name, invert[i]) ;
	    switch (specials[invert[i] - 1].handling)
		{
		    case NORMAL:   
			printf("NORMAL,");
			break;
		    case SPECIAL:   
			printf("SPECIAL,");
			break;
		    case UNSUPP:   
			printf("UNSUPP,");
			break;
		}
	    if (specials[invert[i] - 1].lower)
		printf(" %3d", specials[invert[i] - 1].lower);
	    else    
		printf(" %3d", specials[invert[i] - 1].charcode);
	    }
	else
	    printf(
	      "/* %3d: undefined                       */ %3d, UNSUPP, %3d",
		   i,
		   emptycell++,
		   i) ;
	if (i < NO8BITCHARS - 1) putchar(',') ;
	putchar('\n') ;
	}
    }
return 0;
}
