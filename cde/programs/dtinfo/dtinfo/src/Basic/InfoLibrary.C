/*
 * $XConsortium: InfoLibrary.cc /main/3 1996/06/11 16:19:32 cde-hal $
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

#define C_OutlineList
#define C_InfoLibrary
#define C_InfoBase
#define C_Database
#define L_Basic

#define C_DatabaseMgr
#define C_MessageMgr
#define L_Managers

#include "Prelude.h"

INIT_CLASS (InfoLibrary);

void
InfoLibrary::display()
{
}

// /////////////////////////////////////////////////////////////////
// create_children
// /////////////////////////////////////////////////////////////////

void
InfoLibrary::create_children()
{
  info_lib &il = *(olias_db()->infolib_ptr());
  int i = il.first();

  int num_bases = il.num_of_bases();
  Xassert (num_bases != 0);

  f_children = new OutlineList (num_bases);

  while (i != 0)
    {
      f_children->append (new InfoBase (il(i)));
      il.next (i);
    }
}
