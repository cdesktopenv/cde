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
/*%%  $XConsortium: tt_isam_file.h /main/3 1995/10/20 16:43:47 rswiston $ 			 				 */
/*
 * tt_isam_file.h - Defines the TT ISAM file class.  This class simplifies
 *                  opening, closing, reading and writing an ISAM file.
 *
 * Copyright (c) 1992 by Sun Microsystems, Inc.
 */

#ifndef  _TT_ISAM_FILE_H
#define  _TT_ISAM_FILE_H

#include "util/tt_object.h"
#include "tt_isam.h"
#include "tt_isam_key_descriptor_utils.h"
#include "tt_isam_record_utils.h"

typedef int (*FatalErrorHandlerFunction) (char *);

class _Tt_isam_file : public _Tt_object {
public:
  // Dummy constructor needed to make _tt_isam_file_utils.cc happy
  _Tt_isam_file () { currentRecordLength = 0; currentRecordNumber = 0; eraseFlag = 0; errorStatus = 0;
                     fileDescriptor = 0; fileMode = 0; maxRecordLength = 0; minRecordLength = 0; newFlag = 0; }

  // Real constructors
  _Tt_isam_file (const _Tt_string &file, int mode);
  _Tt_isam_file (const _Tt_string            &file,
		 int                          max_record_length,
		 int                          min_record_length,
		 _Tt_isam_key_descriptor_ptr  primary_key_descriptor,
		 int                          mode);
  ~_Tt_isam_file ();

  void setErase (bool_t);
  int  sync ();

//
// islock and isunlock are not supported by mini-isam
//
//  int  lock ();
//  int  unlock ();

  int  rename (const _Tt_string&);

  int addIndex (_Tt_isam_key_descriptor_ptr);
  _Tt_isam_record_ptr getEmptyRecord ();

  int findStartRecord (const _Tt_isam_key_descriptor_ptr &key_descriptor,
		       int                                length,
		       const _Tt_isam_record_ptr         &record,
		       int                                mode);

  _Tt_isam_record_ptr readRecord (int mode);
  int                 readRecord (int mode, const _Tt_isam_record_ptr&);
  int                 updateCurrentRecord (const _Tt_isam_record_ptr&);
  int                 updateRecord (long recnum, const _Tt_isam_record_ptr&);
  int                 writeRecord (const _Tt_isam_record_ptr&);
  int                 deleteCurrentRecord ();
  int                 deleteRecord (long recnum);

  int        writeMagicString (const _Tt_string&);
  _Tt_string readMagicString ();

  int setFatalErrorHandler (FatalErrorHandlerFunction);

  long getLastRecordNumber () const
    {
      return currentRecordNumber;
    }

  int getFileDescriptor () const
    {
      return fileDescriptor;
    }

  const _Tt_string &getName () const
    {
      return fileName;
    }

  int getErrorStatus () const
    {
      return errorStatus;
    }

  bool_t isNew () const
    {
      return newFlag;
    }

private:
  int                              currentRecordLength;
  long                             currentRecordNumber;
  bool_t		           eraseFlag;
  int                              errorStatus;
  int                              fileDescriptor;
  _Tt_string                       fileName;
  int                              fileMode;
  _Tt_isam_key_descriptor_list_ptr keyDescriptorList;
  int                              maxRecordLength;
  int                              minRecordLength;
  bool_t		           newFlag;

  void		      setTtISAMFileDefaults ();
  _Tt_isam_record_ptr getFullRecord (const _Tt_string&);
  void                getISAMFileInfo ();

  void		      getStatusInfo ()
    {
      currentRecordLength = isreclen;
      currentRecordNumber = isrecnum;
      errorStatus = iserrno;
    }

  static bool_t	      isamFatalErrorHandlerSet;
  static int	      isamFatalErrorHandler (char *message);
};

#endif  /* _TT_ISAM_FILE_H */
