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
//%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			
//%%  (c) Copyright 1993, 1994 International Business Machines Corp.	
//%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			
//%%  (c) Copyright 1993, 1994 Novell, Inc. 				
//%%  $XConsortium: tt_client_isam_key_descriptor.C /main/3 1995/10/23 09:59:42 rswiston $ 			 				
/*
 * tt_client_isam_key_descriptor.cc - Defines the TT ISAM key descriptor class.
 *                  This class is used to hold the information required
 *                  to create a NetISAM key descriptor.  It also makes
 *                  it very easy to construct the descriptor.
 *
 * Copyright (c) 1992 by Sun Microsystems, Inc.
 */

#include "db/tt_client_isam_key_descriptor.h"

_Tt_client_isam_key_descriptor::_Tt_client_isam_key_descriptor ()
{
  keyDescriptor.k_flags = 0;
  keyDescriptor.k_nparts = 0;
}

_Tt_client_isam_key_descriptor::~_Tt_client_isam_key_descriptor ()
{
}

short _Tt_client_isam_key_descriptor::addKeyPart (short start, short length, short type)
{
  int index = -1;

  if (keyDescriptor.k_nparts < NPARTS-1) {
    index = keyDescriptor.k_nparts;

    (&keyDescriptor.k_part_0 + index)->kp_start = start;
    (&keyDescriptor.k_part_0 + index)->kp_leng = length;
    (&keyDescriptor.k_part_0 + index)->kp_type = type;

    keyDescriptor.k_nparts++;
  }

  return index;
}

short _Tt_client_isam_key_descriptor::setKeyPart (short index,
					   short start,
					   short length,
					   short type)
{
  short error = 0;

  if ((index > -1) && (index < keyDescriptor.k_nparts)) {
    (&keyDescriptor.k_part_0 + index)->kp_start = start;
    (&keyDescriptor.k_part_0 + index)->kp_leng = length;
    (&keyDescriptor.k_part_0 + index)->kp_type = type;
  }
  else {
    error = -1;
  }

  return error;
}

short _Tt_client_isam_key_descriptor::getKeyPart (short  index,
					   short &start,
					   short &length,
					   short &type) const
{
  short error = 0;

  if ((index > -1) && (index < keyDescriptor.k_nparts)) {
    

    start = (&keyDescriptor.k_part_0 + index)->kp_start;
    length = (&keyDescriptor.k_part_0 + index)->kp_leng;
    type = (&keyDescriptor.k_part_0 + index)->kp_type;
  }
  else {
    start = -1;
    length = -1;
    type = -1;
    error = -1;
  }

  return error;
}
