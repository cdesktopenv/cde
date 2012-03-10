/* $XConsortium: code.c /main/3 1995/11/08 11:21:27 rswiston $ */
/*
              Copyright 1986 Tandem Computers Incorporated.
This product and information is proprietary of Tandem Computers Incorporated.
                   Copyright 1986, 1987, 1988, 1989 Hewlett-Packard Co.
*/

/* Code.c executes code entities specified in the interface. */

#include "userinc.h"
#include "globdef.h"
#include <time.h>

/* Include generated file */
#include "entfile.c"

/* When a reference to a CODE entity is encountered */
void m_codeent(m_ent)
  int m_ent ;
  {
    (*m_ctable[m_ent])() ;
  }

