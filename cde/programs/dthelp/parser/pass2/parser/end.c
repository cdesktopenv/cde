/* $XConsortium: end.c /main/3 1995/11/08 10:50:46 rswiston $ */
/*
              Copyright 1986 Tandem Computers Incorporated.
This product and information is proprietary of Tandem Computers Incorporated.
                   Copyright 1986, 1987, 1988, 1989 Hewlett-Packard Co.
*/

/* End.c executes end-code specified in the interface. */

#include "userinc.h"
#include "globdec.h"

/* Include generated file */
#include "efile.c"


/* Perform the m_action-th end-code in the interface */
void m_endcase(m_action)
  int m_action ;
  {
    (*m_etable[m_action])() ;
    }

