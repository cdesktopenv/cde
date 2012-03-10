/* $XConsortium: startdoc.c /main/3 1995/11/08 10:59:27 rswiston $ */
/*
Copyright 1989 Hewlett-Packard Co.
*/

#include "userinc.h"
#include "globdec.h"

/* Default function called at start of document instance.  Allows interface
   control after all entity declarations have been processed.  Not needed
   in valid documents (when it is called just prior to global start-code),
   but may be useful in some applications nevertheless. */
void m_startdoc(M_NOPAR)
  {
    }


