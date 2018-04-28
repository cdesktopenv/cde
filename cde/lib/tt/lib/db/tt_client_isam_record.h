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
/*%%  $XConsortium: tt_client_isam_record.h /main/3 1995/10/23 10:00:19 rswiston $ 			 				 */
/*
 * tt_client_isam_record.h - Defines the TT ISAM record class.  This class simplifies
 *                    putting data in a record.  To get an empty record ready
 *                    to be filled with data, call the _Tt_client_isam_file::getEmptyRecord.
 *
 * Copyright (c) 1992 by Sun Microsystems, Inc.
 */

#ifndef  _TT_CLIENT_ISAM_RECORD_H
#define  _TT_CLIENT_ISAM_RECORD_H

#include "util/tt_object.h"
#include "util/tt_string.h"
#include "db/tt_client_isam_key_descriptor_utils.h"

class _Tt_client_isam_record : public _Tt_object {
public:
  // Dummy constructor needed to make tt_client_isam_record_utils.cc happy
  _Tt_client_isam_record ()
    {
      currentLength = 0;
      maxLength = 0;
      minLength = 0;
    }

  // Real constructor
  _Tt_client_isam_record (const _Tt_client_isam_key_descriptor_list_ptr &key_descriptor_list,
		   int                                     max_record_length,
		   int                                     min_record_length);
  ~_Tt_client_isam_record ();

  int                         getNumberOfKeys () const;
  _Tt_client_isam_key_descriptor_ptr getKeyDescriptor (int) const;

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
  _Tt_client_isam_key_descriptor_list_ptr keyDescriptorList;
  int                              currentLength;
  int			           maxLength; 
  int			           minLength; 
};

#endif  /* _TT_CLIENT_ISAM_RECORD_H */
