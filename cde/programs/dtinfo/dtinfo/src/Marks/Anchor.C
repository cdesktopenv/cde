/*
 * $TOG: Anchor.C /main/5 1998/04/17 11:37:31 mgreess $
 *
 * Copyright (c) 1993 HAL Computer Systems International, Ltd.
 * All rights reserved.  Unpublished -- rights reserved under
 * the Copyright Laws of the United States.  USE OF A COPYRIGHT
 * NOTICE IS PRECAUTIONARY ONLY AND DOES NOT IMPLY PUBLICATION
 * OR DISCLOSURE.
 * 
 * THIS SOFTWARE CONTAINS CONFIDENTIAL INFORMATION AND TRADE
 * SECRETS OF HAL COMPUTER SYSTEMS INTERNATIONAL, LTD.  USE,
 * DISCLOSURE, OR REPRODUCTION IS PROHIBITED WITHOUT THE
 * PRIOR EXPRESS WRITTEN PERMISSION OF HAL COMPUTER SYSTEMS
 * INTERNATIONAL, LTD.
 * 
 *                         RESTRICTED RIGHTS LEGEND
 * Use, duplication, or disclosure by the Government is subject
 * to the restrictions as set forth in subparagraph (c)(l)(ii)
 * of the Rights in Technical Data and Computer Software clause
 * at DFARS 252.227-7013.
 *
 *          HAL COMPUTER SYSTEMS INTERNATIONAL, LTD.
 *                  1315 Dell Avenue
 *                  Campbell, CA  95008
 * 
 */


#define C_Anchor
#define L_Marks

#include <Prelude.h>

#include <string.h>
#include <memory.h>

Anchor::~Anchor()
{
  if (f_location) delete[] f_location;
  if (f_format)   delete[] f_format;
}


void
Anchor::set (const char *location, const char *format, bool keep)
{
  unsigned int l;

  if (f_location) {
    delete[] f_location;
    f_location = NULL;
  }
  if (f_format) {
    delete[] f_format;
    f_format = NULL;
  }

  if (keep)
    {
      f_location = (char *) location;
      f_format = (char *) format;
    }
  else
    {
      l = strlen (location);
      f_location = new char[l + 1];
      memcpy (f_location, location, l+1);

      l = strlen (format);
      f_format = new char[l + 1];
      memcpy (f_format, format, l+1);
    }
}

Anchor &
Anchor::operator=(Anchor &anchor)
{
  if (anchor != 0)
    set (anchor.location(), anchor.format());
  else
    set(0, 0, 1);

  return *this ;
}

bool
Anchor::operator!=(int )
{
  
  // if location or format are non-zero then this is not a NULL object 
  return f_location || f_format ;
}

