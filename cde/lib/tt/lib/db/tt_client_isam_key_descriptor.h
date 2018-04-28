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
/*%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			 */
/*%%  (c) Copyright 1993, 1994 International Business Machines Corp.	 */
/*%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			 */
/*%%  (c) Copyright 1993, 1994 Novell, Inc. 				 */
/*%%  $XConsortium: tt_client_isam_key_descriptor.h /main/3 1995/10/23 09:59:50 rswiston $ 			 				 */
/*
 * tt_client_isam_key_descriptor.h - Defines the TT ISAM key descriptor class.
 *                 This class is used to hold the information required
 *                 to create a NetISAM key descriptor.  It also makes
 *                 it very easy to construct the descriptor.
 *
 * Copyright (c) 1992 by Sun Microsystems, Inc.
 */

#ifndef  _TT_CLIENT_ISAM_KEY_DESCRIPTOR_H
#define  _TT_CLIENT_ISAM_KEY_DESCRIPTOR_H

#include "db/tt_client_isam.h"
#include "util/tt_object.h"

class _Tt_client_isam_key_descriptor : public _Tt_object {
public:
  _Tt_client_isam_key_descriptor ();
  ~_Tt_client_isam_key_descriptor ();

  void setCompress (bool_t flag)
    {
      if (flag) {
	keyDescriptor.k_flags |= COMPRESS;
      }
      else {
	keyDescriptor.k_flags &= (short) (0xFFFF - COMPRESS);
      }
    }

  bool_t isCompressSet ()
    {
      return ((keyDescriptor.k_flags & COMPRESS) ? TRUE : FALSE);
    }

  void setDuplicates (bool_t flag)
    {
      if (flag) {
	keyDescriptor.k_flags &= (short) (0xFFFF - ISNODUPS);
	keyDescriptor.k_flags |= ISDUPS;
      }
      else {
	keyDescriptor.k_flags &= (short) (0xFFFF - ISDUPS);
	keyDescriptor.k_flags |= ISNODUPS;
      }
    }

  bool_t isDuplicatesSet ()
    {
      return ((keyDescriptor.k_flags & ISDUPS) ? TRUE : FALSE);
    }

  short addKeyPart (short start, short length, short type);
  short setKeyPart (short index, short start, short length, short type);

  short getKeyPart (short  index,
		    short &start,
		    short &length,
		    short &type) const;

  short getNumberOfParts () const
    {
      return keyDescriptor.k_nparts;
    }

  keydesc *getKeyDescriptor ()
    {
      return &keyDescriptor;
    }
  
private:
  struct keydesc keyDescriptor;

friend class _Tt_client_isam_file;
};

#endif  /* _TT_CLIENT_ISAM_KEY_DESCRIPTOR_H */
