/*
 * $XConsortium: Anchor.hh /main/3 1996/06/11 16:28:47 cde-hal $
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


struct Anchor
{
public:
  Anchor (const char *location, const char *format, bool keep = FALSE)
    : f_location (NULL), f_format (NULL)
    { set (location, format, keep); }
  Anchor (const Anchor &anchor)
    : f_location (NULL), f_format (NULL)
    { set (anchor.f_location, anchor.f_format, FALSE); }
  virtual ~Anchor();

  Anchor &operator= (Anchor &);
  bool   operator!=(int i);	// check vs NULL

  const char *location() const
    { return (f_location); }
  const char *format() const
    { return (f_format); }

  void set (const char *location, const char *format, bool keep = FALSE);

protected:
    Anchor()
    : f_location (NULL), f_format (NULL)
    { }

private:
  char *f_location;
  char *f_format;
};
