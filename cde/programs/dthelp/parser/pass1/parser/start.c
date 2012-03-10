/* $XConsortium: start.c /main/3 1995/11/08 10:25:42 rswiston $ */
/*
              Copyright 1986 Tandem Computers Incorporated.
This product and information is proprietary of Tandem Computers Incorporated.
                   Copyright 1986, 1987, 1988, 1989 Hewlett-Packard Co.
*/

/* Start.c executes start-code specified in the interface. */

#include "userinc.h"
#include "globdec.h"

/* Include generated file */
#include "sfile.c"

/* Perform the m_action-th start-code in the interface */
void m_strtcase(m_action)
  int m_action ;
  {
    (*m_stable[m_action])() ;
    }

