/* $XConsortium: getsign.c /main/3 1995/11/08 10:19:25 rswiston $ */
/*
Copyright 1988, 1989 Hewlett-Packard Co.
*/

/* Issue interface-generated sign-on message */

#include <stdio.h>
#include "basic.h"
#include "trie.h"
#include "dtdext.h"
#include "parser.h"
#include "signonx.h"

void m_getsignon(M_NOPAR)
  {
    m_errline(m_signon) ;
    }


