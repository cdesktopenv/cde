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
 * License along with these libraries and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
/*
 * $XConsortium: NodeDisplayDataTml.cc /main/3 1996/06/11 16:20:36 cde-hal $
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

#define C_NodeDisplayData
#define L_Basic

#define C_NodeMgr
#define C_NodeParser
#define L_Managers

#define C_NodeObject
#define L_Odb

#include "Prelude.h"

#include <tml/tml_objects.hxx>


// /////////////////////////////////////////////////////////////////
// the_model - return the model, creating if necessary
// /////////////////////////////////////////////////////////////////

model &
NodeDisplayData::the_model()
{
#ifdef BOGUS
  // MUST start counting at 1!! DJB 
  static u_int next_serial = 1;
  if (f_model == NULL)
    {
      f_model = f_node_object.node_manager().node_parser().load (f_node_object);
      f_model_serial = next_serial++;
    }

  // NOTE: error checking required

  return (*f_model);
#endif
  return (*f_model);
}

// /////////////////////////////////////////////////////////////////
// not_displayed
// /////////////////////////////////////////////////////////////////

// FYI: this function could easily be extended to implement a model cache.

void
NodeDisplayData::not_displayed()
{
  return;  //  commented out to test code below
  // NOTE: Return for now - improves performance, but uses more memory.
  // If you remove the return statement above you MUST tell me
  // or user marks will break!!  DJB 10/28/92
#if 0
  delete f_model;
  f_model = NULL;
  f_locator_table.remove_all (TRUE);

  graphics_mgr().remove(f_node_object);
#endif
}


// /////////////////////////////////////////////////////////////////
// class destructor
// /////////////////////////////////////////////////////////////////

NodeDisplayData::~NodeDisplayData()
{
#ifdef BOGUS
  printf ("~~~ NodeDisplayData Destructed ~~~\n");
  delete f_model;
  f_locator_table.remove_all (TRUE);

  ON_DEBUG(cerr << f_node_object.locator() << endl);

  graphics_mgr().remove(f_node_object);
#endif
}


// /////////////////////////////////////////////////////////////////
// update_fonts
// /////////////////////////////////////////////////////////////////

void
NodeDisplayData::update_fonts(unsigned int serial_number)
{
#ifdef BOGUS
  // NOTE: f_font_serial number could be way less than serial_number 
  // as this could have been on the history list with no node displayed,
  // therefore f_font_serial would not have been updated
  // during other font changes, as the update_fonts is done only to visible
  // nodes

  assert(f_font_serial <= serial_number);

  //  see if we have already dealt with this model
  if (f_font_serial == serial_number)
    return ;

  f_font_serial = serial_number ;

  if (f_model)
    f_model->update_fonts();
#endif
}
