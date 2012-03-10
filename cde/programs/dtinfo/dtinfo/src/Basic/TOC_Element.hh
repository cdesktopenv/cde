/*
 * $XConsortium: TOC_Element.hh /main/4 1996/07/10 09:37:56 rcs $
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

#include "UAS.hh"

class OutlineList;

class TOC_Element : public OutlineElement
{
public:
  DEF_CLASS (TOC_Element);

  TOC_Element (const UAS_Pointer<UAS_Common> &toc);
  
  const char *display_as();
  OutlineList *children();
  void display();
  void xprint();
  UAS_Pointer<UAS_Common> toc()
    { return (f_toc); }

  // Can't set the children, since object will automatically generate. 
  void set_children (OutlineList *);
  bool expand_hint();

private:
  bool has_children_internal();

private:
  UAS_Pointer<UAS_Common>  f_toc;
  UAS_String fDisplayAs;
};
