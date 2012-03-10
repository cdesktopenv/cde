/* $XConsortium: pardef.c /main/3 1995/11/08 10:55:48 rswiston $ */
/*
                   Copyright 1989 Hewlett-Packard Co.
*/

#include <stdio.h>
#include "basic.h"
#include "trie.h"
#include "dtdext.h"
#include "parser.h"

/* Function callable by interface designers.  Returns pointer to default
   value of a parameter of an element. Type is set to indicate type of
   default.  Error indication is type is 0 and function returns NULL. */
M_WCHAR *m_pardefault(elt, param, type)
  M_WCHAR *elt ;
  M_WCHAR *param ;
  unsigned char *type ;
  {
    M_ELEMENT eltid ;
    int par ;
    int i ;

    *type = M_NULLVAL ;
    if (! (eltid = m_packedlook(m_entree, elt))) return(NULL) ;

    for (par = m_element[eltid - 1].parptr, i = 0 ;
         i < m_element[eltid - 1].parcount ;
         par++, i++)
      if (! m_wcupstrcmp(&m_pname[m_parameter[par - 1].paramname], param)) {
        *type = (unsigned char) m_parameter[par - 1].deftype ;
        if (*type == M_REQUIRED || *type == M_NULLDEF) return(NULL) ;
        return(m_parameter[par -1].defval) ;
        }

    /* Can't find the parameter */
    return(NULL) ;
    }

