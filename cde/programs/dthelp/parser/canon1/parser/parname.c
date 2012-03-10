/* $XConsortium: parname.c /main/3 1995/11/08 09:40:42 rswiston $ */
/*
                   Copyright 1989 Hewlett-Packard Co.
*/

#include <stdio.h>
#include "basic.h"
#include "trie.h"
#include "dtdext.h"
#include "parser.h"

/* Function callable by interface designers.  Returns name of nth parameter
   of specified element (NULL indicates error). */
M_WCHAR *m_parname(elt, n)
  M_WCHAR *elt ;
  int n ;
  {
    M_ELEMENT eltid ;

    if (! (eltid = m_packedlook(m_entree, elt))) return(NULL) ;
    if (n < 1 || n > m_element[eltid - 1].parcount) return(NULL) ;
    return(&m_pname[m_parameter[(n - 1) + m_element[eltid - 1].parptr - 1].
                    paramname]) ;
    }

M_WCHAR *m_mbparname(elt, n)
char *elt ;
int n ;
{
M_WCHAR *wc_elt, *retval;

wc_elt = MakeWideCharString(elt);
retval = m_parname(wc_elt, n);
m_free(wc_elt,"wide character string");
}

