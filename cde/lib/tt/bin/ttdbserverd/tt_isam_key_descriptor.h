/*%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			 */
/*%%  (c) Copyright 1993, 1994 International Business Machines Corp.	 */
/*%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			 */
/*%%  (c) Copyright 1993, 1994 Novell, Inc. 				 */
/*%%  $XConsortium: tt_isam_key_descriptor.h /main/3 1995/10/20 16:44:29 rswiston $ 			 				 */
/*
 * tt_isam_key_descriptor.h - Defines the TT ISAM key descriptor class.
 *                 This class is used to hold the information required
 *                 to create a NetISAM key descriptor.  It also makes
 *                 it very easy to construct the descriptor.
 *
 * Copyright (c) 1992 by Sun Microsystems, Inc.
 */

#ifndef  _TT_ISAM_KEY_DESCRIPTOR_H
#define  _TT_ISAM_KEY_DESCRIPTOR_H

#include "util/tt_object.h"
#include "tt_isam.h"

class _Tt_isam_key_descriptor : public _Tt_object {
public:
  _Tt_isam_key_descriptor ();
  ~_Tt_isam_key_descriptor ();

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

private:
  struct keydesc keyDescriptor;

  keydesc *getKeyDescriptor ()
    {
      return &keyDescriptor;
    }
  
friend class _Tt_isam_file;
};

#endif  /* _TT_ISAM_KEY_DESCRIPTOR_H */
