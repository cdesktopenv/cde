/*
 * $XConsortium: OutlineString.hh /main/3 1996/06/11 16:21:35 cde-hal $
 *
 * Copyright (c) 1992 HAL Computer Systems International, Ltd.
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

#include <string.h>

class OutlineString : public OutlineElement
{
public: // functions
  OutlineString (const char *string = NULL)
    {
      if (string != NULL)
	{
	  int len = strlen(string);
	  f_string = new char [len + 1];
	  *((char *) memcpy(f_string, string, len) + len) = '\0';
	}
      else
	f_string = NULL;
    }

  ~OutlineString ()
    { delete [] f_string; }

  const char *display_as() 
    { return (f_string); }

  void display()
    { }

protected: // variables
  char *f_string;
};
