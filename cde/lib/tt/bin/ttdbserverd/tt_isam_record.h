/*%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			 */
/*%%  (c) Copyright 1993, 1994 International Business Machines Corp.	 */
/*%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			 */
/*%%  (c) Copyright 1993, 1994 Novell, Inc. 				 */
/*%%  $XConsortium: tt_isam_record.h /main/3 1995/10/20 16:45:07 rswiston $ 			 				 */
/*
 * tt_isam_record.h - Defines the TT ISAM record class.  This class simplifies
 *                    putting data in a record.  To get an empty record ready
 *                    to be filled with data, call the _Tt_isam_file::getEmptyRecord.
 *
 * Copyright (c) 1992 by Sun Microsystems, Inc.
 */

#ifndef  _TT_ISAM_RECORD_H
#define  _TT_ISAM_RECORD_H

#include "util/tt_object.h"
#include "util/tt_string.h"
#include "tt_isam_key_descriptor_utils.h"

class _Tt_isam_record : public _Tt_object {
public:
  // Dummy constructor needed to make tt_isam_record_utils.cc happy
  _Tt_isam_record () {}

  // Real constructor
  _Tt_isam_record (const _Tt_isam_key_descriptor_list_ptr &key_descriptor_list,
		   int                                     max_record_length,
		   int                                     min_record_length);
  ~_Tt_isam_record ();

  int                         getNumberOfKeys () const;
  _Tt_isam_key_descriptor_ptr getKeyDescriptor (int) const;

  _Tt_string getKeyPartValue (int, int) const;
  void       setKeyPartValue (int, int, const _Tt_string&);

  _Tt_string getBytes (int, int) const;
  void       setBytes (int, const _Tt_string&);
  void       setBytes (int, int, const _Tt_string&);

  void setLength (int length)
    {
      currentLength = length;
    }

  int getLength () const
    {
      return currentLength;
    }

  int getMaxLength () const
    {
      return maxLength;
    }

  int getMinLength () const
    {
      return minLength;
    }

  const _Tt_string &getRecord () const
    {
      return buffer;
    }

private:
  _Tt_string                       buffer;
  _Tt_isam_key_descriptor_list_ptr keyDescriptorList;
  int                              currentLength;
  int			           maxLength; 
  int			           minLength; 
};

#endif  /* _TT_ISAM_RECORD_H */
