/*
 * CDE - Common Desktop Environment
 *
 * Copyright (c) 1993-2012, The Open Group. All rights reserved.
 *
 * These libraries and programs are free software; you can
 * redistribute them and/or modify them under the terms of the GNU
 * Lesser General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * These libraries and programs are distributed in the hope that
 * they will be useful, but WITHOUT ANY WARRANTY; without even the
 * implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with these librararies and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
/*
 * $XConsortium: TOC_Element.C /main/7 1996/09/04 14:29:22 rcs $
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

#define C_TOC_Element
#define C_OutlineList
#define L_Basic

#include "Prelude.h"

#include <iostream>
using namespace std;

INIT_CLASS (TOC_Element);

TOC_Element::TOC_Element (const UAS_Pointer<UAS_Common> &toc)
: f_toc (toc) 
{
     UAS_Pointer<UAS_Common> *temp_common = (UAS_Pointer<UAS_Common> *)&toc;
     fDisplayAs = (*temp_common)->title();
}


const char *
TOC_Element::display_as()
{
  return (char *) fDisplayAs;
}


OutlineList *
TOC_Element::children()
{
  // If the children list hasn't been created yet, create it now. 
  if (f_children == NULL)
    {
      UAS_List<UAS_Common> kids = f_toc->children();
      unsigned int num_children = kids.length();
      if (num_children > 0)
	{
	  // Allocate an OutlineList for the kids and fill it in. 
	  f_children = new OutlineList (num_children);
	  unsigned int i;
	  for (i = 0; i < num_children; i++)
	    f_children->append (new TOC_Element (kids[i]));
	}
    }
  children_valid (TRUE);
  return (f_children);
}


void
TOC_Element::set_children (OutlineList *)
{
  // Attempting to set the children is a programming error. 
  abort();
}


void
TOC_Element::display()
{
#ifdef SUBTREE_SIZE_DEBUG
  cerr << "subtree size = " << f_toc->subtree_size() << endl;
#endif
    f_toc->retrieve((void *)0);
}


void
TOC_Element::xprint()
{
    f_toc->retrieve((void *)1);
}


bool
TOC_Element::has_children_internal()
{
  UAS_List<UAS_Common> kids = f_toc->children();
  return (kids.length() > 0);
}
