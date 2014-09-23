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
/* $XConsortium: context.c /main/3 1995/11/08 11:03:02 rswiston $ */
/*
              Copyright 1986 Tandem Computers Incorporated.
This product and information is proprietary of Tandem Computers Incorporated.
               Copyright (c) 1986, 1987, 1988, 1989 Hewlett-Packard Co.
*/

/*
Context.c contains the main procedure for program CONTEXT.

CONTEXT builds scanning and parsing tables for a simple
state-transition recognizer.  Input consists of two files, delimiter
declarations in file "delim.dat" and state tables with corresponding
actions in file "context.dat". If a parameter is present on the
command line, a sparse matrix technique is used to encode some of the
output arrays.

Data lines in "delim.dat" contain the name of the delimiter followed
by the text of the delimiter (one or more non-white space
characters).  The delimiter name must begin in the first column.  
Lines with white-space in the first column are considered comments.

Data in "context.dat" consists of free-form multi-line declarations.
Each declaration is terminated by a semi-colon. It begins with the
name of a token followed by any number of pairs of contexts (parser
states).  The context pairs are separated by a colon and may
optionally be followed by some C code enclosed in braces.  In each
pair, the first context is one in which the delimiter is recognized,
the second is the state that will result from an occurrence of the
delimiter in the first context. If code is specified, it is to be
executed by the parser when the delimiter occurs in the first context
of the pair. If successive pairs of contexts are separated by a
comma, code which follows is common to all the pairs.

The tokens specified in "context.dat" can either be delimiters defined in
"delim.dat" or other tokens (e.g., number, name, end_of_file) that will
be recognized by the user-defined scanner.

For example, suppose "delim.dat" contains the line

               grpo (

and "context.dat" includes

               grpo element:namegroup
                    content:model {openmod() ;}
                    excon:openex
                    pcon:type ;

               name namegroup:namesep {eltname() ;}
                    model:connector,
                    model2:connector
                      {storemod(nstring) ;}

These declarations specify that the "grpo" delimiter is "(" and is
recognized in contexts "element", "content", "excon", and "pcon". 
Procedure openmod() is called when "grpo" occurs in "content" and,
beyond the state change, no code is executed in other contexts when
"grpo" occurs.  The "name" token is valid in contexts "namegroup",
"model", and "model2"; in the first, the context is set to "namesep"
and procedure eltname() is called, in the last two, the context is
set to "connector" and storemod(nstring) is called.

The program has several output files: case.c, delim.h, context.h, and
error.  Case.c contains nested switch statements to execute the code
specified with the context transitions.  Delim.h contains C
declarations related to delimiters; context.h has declarations
related to contexts.  Programs that include delim.h should define
M_DELIMDEF for definition form of the declarations and not define it
for the declaration form.  Programs that include context.h should
define M_CONDEF analogously.

The program numbers the delimiter names and the contexts as
encountered and outputs corresponding #define instructions with all
letters in the names forced to uppercase to delim.h and context.h.
In addition, an array named for each delimiter (with all letters
forced to lowercase) is initialized to the delimiter text.  The first
declaration in the above example, for instance, would result in the
following lines written to delim.h:

               #define GRPO 1
               M_DELIMEXTERN char grpo[] M_DELIMINIT("(") ;

These lines would be written to context.h:

               #define ELEMENT 1
               #define NAMEGROUP 2
               #define CONTENT 3
               #define MODEL 4
               #define EXCON 5
               #define OPENEX 6
               #define PCON 7
               #define TYPE 8
               #define NAMESEP 9
               #define CONNECTOR 10
               #define MODEL2 11

Delim.h also contains an array, m_dlmptr, which contains pointers to the
names of all the defined delimiters.  In addition, delim.h contains the
declaration of a tree representation of the delimiters allowed in each
context. Internally, the program represents the delimiter strings with a
tree.  The children of the root of this tree represent the set of characters
with which delimiters can start. Children of other nodes represent
characters that can follow the character of their parent.  Leaf nodes
are associated with the null character, indicating the end of a
delimiter.  A path from the root to a leaf thus represents a valid
delimiter.

The form in which the tree is represented within this program is not
quite the same as the output data structure.  In the latter, each
element consists of a flag called 'more' indicating whether it is the last
child of its parent; a character, called 'symbol'; and an integer,
called 'index'. All sons of a node are stored in adjacent elements
ordered in increaing value of their 'symbol' fields. 'Symbol'
contains the character associated with the node (0 if the node is a
leaf node), with the sign bit turned on if the node is the last child
of its parent.  'Index' contains the array index of the first son of
the node, if the node is not a leaf.  If the node is a leaf, 'index'
is the number of the represented delimiter (i.e., 1 for "grpo" in the
above example).

As the tree is constructed, it is not feasible to store brothers in adjacent
elements, so the internal data structure is different.  It contains a 'next'
field which points to the first brother of a given node. Because the 'next'
field can be used to detect the last child of a parent, it is not necessary
to use the sign bit of 'symbol' to indicate this special case.

Two other arrays are written to context.h.  Contree[i] points to the root
of the delimiter tree for the ith context within the array tree whose
declaration is in delim.h; nextcon[i][j] indicates the state
resulting from an occurrence of delimiter j+1 in state i+1.  Finally,
a #define of MAXD to the length of the longest delimiter is written to
delim.h.
*/

#include <stdio.h>
#include <stdlib.h>
#if defined(MSDOS)
#include <process.h>
#endif
#include "basic.h"
#include "trie.h"

#include "common.h"
#include "chartype.h"
#define CONTDEF 1
#include "cont.h"

/* Main procedure */
int main(argc, argv)
  int argc ;
  char **argv ;
{
int n ;

m_openchk(&delim, "delim.h", "w") ;
m_openchk(&context, "context.h", "w") ;
m_openchk(&fcase, "case.c", "w") ;
m_openchk(&cdat, "context.dat", "r") ;
m_openchk(&ddat, "delim.dat", "r") ;
m_openchk(&m_errfile, "error", "w") ;
fputs("    switch(m_token) {\n", fcase) ; /* keep the "}" balanced */

fputs("#if defined(M_DELIMDEF)\n", delim) ;
fputs("#define M_DELIMEXTERN\n", delim) ;
fputs("#define M_DELIMINIT(a) = a\n", delim) ;
fputs("#else\n", delim) ;
fputs("#define M_DELIMEXTERN extern\n", delim) ;
fputs("#define M_DELIMINIT(a)\n", delim) ;
fputs("#endif\n\n", delim) ;

fputs("#if defined(M_CONDEF)\n", context) ;
fputs("#define M_CONEXTERN\n", context) ;
fputs("#else\n", context) ;
fputs("#define M_CONEXTERN extern\n", context) ;
fputs("#endif\n\n", context) ;

contree = (M_TRIE **) calloc(NUMCON, sizeof(M_TRIE *)) ;
xtransit = (int *) calloc(NUMCON * NUMDELIM, sizeof(int)) ;
contexts = (M_WCHAR **) calloc(NUMCON, sizeof(char *)) ;
dlmptr = (char**) calloc(NUMDELIM, sizeof(char*)) ;

loaddelim() ;
while (getdname())
    {
    casestarted = FALSE ;
    while ((n = getContext()) >= 0)
	{
	if (withdelim) enterdelim(n) ;
	getcolon() ;
	if (transit(n, curdelim))
	    {
	    if (! withdelim) 
		{
		char *mb_dname, *mb_contexts;

		mb_dname = MakeMByteString(dname);
		mb_contexts = MakeMByteString(contexts[n]);
		warning2(
		    "Duplicate assignment to token \"%s\" in context \"%s\"",
			 mb_dname,
			 mb_contexts) ;
		m_free(mb_dname,"multi-byte string");
		m_free(mb_contexts,"multi-byte string");
		}
	    }
	transit(n, curdelim) = getContext() + 1 ;
	getcode(n) ;
	}
    if (casestarted)
	{
	fprintf(fcase, "          default:\n            break ;\n") ;
	/* keep the "{" balanced */
	fprintf(fcase, "          }\n        break ;\n") ;
	}
    }
/* keep the "{" balanced */
fprintf(fcase, "      default:\n        break ;\n      }\n") ;
fprintf(context, "#define MAXD %d\n", maxd) ;
dumptree((LOGICAL) (argc > 1)) ;
fprintf(stderr, "NUMCON set to %d\n", NUMCON) ;
fprintf(stderr, "NUMDELIM set to %d\n", NUMDELIM) ;
exit(errexit) ;
}
