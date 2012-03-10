/*
 * $XConsortium: Atomizer.cc /main/3 1996/06/11 16:17:33 cde-hal $
 *
 * Copyright (c) 1992 HaL Computer Systems, Inc.  All rights reserved.
 * UNPUBLISHED -- rights reserved under the Copyright Laws of the United
 * States.  Use of a copyright notice is precautionary only and does not
 * imply publication or disclosure.
 * 
 * This software contains confidential information and trade secrets of HaL
 * Computer Systems, Inc.  Use, disclosure, or reproduction is prohibited
 * without the prior express written permission of HaL Computer Systems, Inc.
 * 
 *                         RESTRICTED RIGHTS LEGEND
 * Use, duplication, or disclosure by the Government is subject to
 * restrictions as set forth in subparagraph (c)(l)(ii) of the Rights in
 * Technical Data and Computer Software clause at DFARS 252.227-7013.
 *                        HaL Computer Systems, Inc.
 *                  1315 Dell Avenue, Campbell, CA  95008
 * 
 */

#define C_Atomizer
#define L_Basic

#include "Prelude.h"

#include <string.h>

// Initial size is 10, grow by 10.
List Atomizer::f_pool (10, 10);

// /////////////////////////////////////////////////////////////////
// private string storage class
// /////////////////////////////////////////////////////////////////

class PoolString : public FolioObject
{
public: // functions
  PoolString (const char *string)
    {
      f_string = new char[strlen(string) + 1];
      strcpy (f_string, string);
    }
	
  ~PoolString ()
    { delete [] f_string; }

  int equals (const char *string)
    { return (strcmp (string, f_string) == 0); }

public: // variables
  char *f_string;
};

// /////////////////////////////////////////////////////////////////
// constructor
// /////////////////////////////////////////////////////////////////

Atomizer::Atomizer (const char *string)
{
  // Try to find the string in the pool of strings
  register int i;

  for (i = 0; i < f_pool.length(); i++)
    if (((PoolString *)f_pool[i])->equals (string))
      break;

  if (i < f_pool.length())
    {
      // We've found it.
      f_value = ((PoolString *)f_pool[i])->f_string;
    }
  else
    {
      // Not there, so add it.
      PoolString *ps = new PoolString (string);
      f_value = ps->f_string;
      f_pool.append (*ps);
    }
}
