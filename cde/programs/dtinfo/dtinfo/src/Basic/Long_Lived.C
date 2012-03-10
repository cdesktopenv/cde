/*
 * $XConsortium: Long_Lived.cc /main/3 1996/06/11 16:20:14 cde-hal $
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


#define C_Long_Lived
#define L_Basic

#include "Prelude.h"

// Final TRUE means that all list elements are deleted when
// the list is destructed on exit from the browser. 
List Long_Lived::g_long_lived_list (20, 10, List::GROW_ADD, TRUE);

// /////////////////////////////////////////////////////////////////
// class constructor
// /////////////////////////////////////////////////////////////////

Long_Lived::Long_Lived()
{
  g_long_lived_list.append (this);
}


// /////////////////////////////////////////////////////////////////
// class destructor
// /////////////////////////////////////////////////////////////////

Long_Lived::~Long_Lived()
{
  g_long_lived_list.remove (this);
}
