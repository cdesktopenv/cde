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
