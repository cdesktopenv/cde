/* $XConsortium: Options.h /main/3 1995/11/03 12:29:53 rswiston $ */
/*******************************************************************
**  (c) Copyright Hewlett-Packard Company, 1990, 1991, 1992, 1993.
**  All rights are reserved.  Copying or other reproduction of this
**  program except for archival purposes is prohibited without prior
**  written consent of Hewlett-Packard Company.
********************************************************************
****************************<+>*************************************/

#ifndef _OPTIONS_H_
#define _OPTIONS_H_

#include "cstring.h"

class Options {
 public:
  Options (unsigned int, char **);
 ~Options () {}

  int       Verbose () const         { return (flags & 1); }
  int       Retain () const          { return (flags & 2); }

 private:
  unsigned char flags;
};

#endif
