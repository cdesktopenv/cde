/* $XConsortium: trieplk.c /main/3 1995/11/08 11:45:01 rswiston $ */
/*
                   Copyright 1986, 1987, 1988, 1989 Hewlett-Packard Co.
*/

/* Trie.c contains procedures for maintaining the tree structure
   used to store element names, delimiter strings, short reference strings,
   etc. */

#include <stdio.h>
#include "basic.h"
#include "common.h"
#include "trie.h"

extern M_CHARTYPE m_ctarray[M_CHARSETLEN] ;

int m_packedlook(
#if defined(M_PROTO)
  M_PTRIE *xptrie,
  M_WCHAR *name
#endif
  ) ;

/* Look for the string NAME in the packed trie PTRIE */
int m_packedlook(xptrie, name)
M_PTRIE *xptrie ;
M_WCHAR *name ;
{
int current = 0 ;
int i ;
int c ;

for ( ; TRUE ; name++)
    {
    c = m_ctupper(*name) ;
    for (i = current ;
	 (int) ((xptrie + i)->symbol) < c && (xptrie + i)->more ;
	 i++) ;
    if ((int) ((xptrie + i)->symbol) == c)
	{
	if (! c) return((xptrie + i)->index) ;
	current = (xptrie + i)->index ;
	}
    else return(FALSE) ;
    }
}
