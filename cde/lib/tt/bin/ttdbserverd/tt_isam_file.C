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
//%%  $XConsortium: tt_isam_file.C /main/3 1995/10/20 16:43:39 rswiston $ 			 				
/*
 * tt_isam_file.cc - Defines the TT ISAM file class.  This class simplifies
 *                   opening, closing, reading and writing an ISAM file.
 *
 * Copyright (c) 1992 by Sun Microsystems, Inc.
 */

#include "tt_isam_file.h"
#include "util/tt_port.h"
#include "dm_access_cache.h"

extern FILE *errstr;

bool_t _Tt_isam_file::isamFatalErrorHandlerSet = FALSE;

_Tt_isam_file::_Tt_isam_file (const _Tt_string &file, int mode)
{
  setTtISAMFileDefaults();
  fileName = file;
  fileMode = mode;

  if ((fileDescriptor = cached_isopen(file, mode)) != -1) {
    getISAMFileInfo();
  }
  else {
    getStatusInfo();
  }

  maxRecordLength = 0;
  minRecordLength = 0;
}

_Tt_isam_file
::_Tt_isam_file (const _Tt_string            &file,
		 int                          max_record_length,
		 int                          min_record_length,
		 _Tt_isam_key_descriptor_ptr  primary_key_descriptor,
		 int                          mode)
{
  setTtISAMFileDefaults();
  fileName = file;
  fileMode = mode;

  if ((fileDescriptor = cached_isopen(file, mode)) == -1) {
    newFlag = TRUE;
    isreclen = min_record_length;
    fileDescriptor = isbuild(file,
			     max_record_length,
			     primary_key_descriptor->getKeyDescriptor(),
			     mode);
  }
  
  if (fileDescriptor != -1) {
    getISAMFileInfo();
  }
  else {
    errorStatus = iserrno;
    currentRecordLength = -1;
    currentRecordNumber = -1;
  }

  maxRecordLength = 0;
  minRecordLength = 0;
}

void _Tt_isam_file::setTtISAMFileDefaults ()
{
  eraseFlag = FALSE;
  fileName = (char *)NULL;
  fileMode = 0;
  keyDescriptorList = new _Tt_isam_key_descriptor_list;
  newFlag = FALSE;

  if (!isamFatalErrorHandlerSet) {
    isamFatalErrorHandlerSet = TRUE;
    (void)iscntl(ALLISFD, ISCNTL_FATAL, &_Tt_isam_file::isamFatalErrorHandler);
  }

  iserrno = 0;
}

_Tt_isam_file::~_Tt_isam_file ()
{
  if (eraseFlag) {
    (void)iserase(fileName);
  }
  else {
    (void)cached_isclose(fileDescriptor);
  }
}

void _Tt_isam_file::setErase (bool_t flag)
{
  eraseFlag = flag;
}

int _Tt_isam_file::sync ()
{
  iserrno = 0;

  (void)isfsync(fileDescriptor);

  errorStatus = iserrno;
  return errorStatus;
}

// 
// islock and isunlock are not supported by mini-isam
// 
//int _Tt_isam_file::lock ()
//{
//  iserrno = 0;
//
//  (void)islock(fileDescriptor);
//
//  errorStatus = iserrno;
//  return errorStatus;
//}
//
//int _Tt_isam_file::unlock ()
//{
//  iserrno = 0;
//
//  (void)isunlock(fileDescriptor);
//
//  errorStatus = iserrno;
//  return errorStatus;
//}

int _Tt_isam_file::rename (const _Tt_string &new_file)
{
  iserrno = 0;

  int results = isrename(fileName, new_file);

  errorStatus = iserrno;
  if (!results) {
    fileName = new_file;
    newFlag = FALSE;

    if ((fileDescriptor = cached_isopen(new_file, fileMode)) != -1) {
      getISAMFileInfo();
    }
    else {
      getStatusInfo();
    }
  }

  return errorStatus;
}

int _Tt_isam_file::addIndex (_Tt_isam_key_descriptor_ptr key_descriptor)
{
  iserrno = 0;

  int results = isaddindex(fileDescriptor, key_descriptor->getKeyDescriptor());

  errorStatus = iserrno;
  if (!results) {
    getISAMFileInfo();
  }

  return errorStatus;
}

_Tt_isam_record_ptr _Tt_isam_file::getEmptyRecord ()
{
  return (new _Tt_isam_record(keyDescriptorList,
			      maxRecordLength,
			      minRecordLength));
}

int _Tt_isam_file
::findStartRecord (const _Tt_isam_key_descriptor_ptr &key_descriptor,
		   int                                length,
		   const _Tt_isam_record_ptr         &record,
		   int                                mode)
{
  iserrno = 0;

  (void)isstart(fileDescriptor,
		key_descriptor->getKeyDescriptor(),
		length,
		(char *)record->getRecord(),
		mode);

  getStatusInfo();
  return errorStatus;
}

_Tt_isam_record_ptr _Tt_isam_file::readRecord (int mode)
{
  iserrno = 0;

  _Tt_isam_record_ptr record = (_Tt_isam_record *)NULL;
  _Tt_string          record_buffer(maxRecordLength);
  int                 results = isread(fileDescriptor,
				       (char *)record_buffer,
				       mode);

  getStatusInfo();
  if (!results) {
    record = getFullRecord(record_buffer);
  }

  return record;
}

int _Tt_isam_file::readRecord (int                        mode,
			       const _Tt_isam_record_ptr &record)
{
  iserrno = 0;

  int results = isread(fileDescriptor, (char *)record->getRecord(), mode);

  getStatusInfo();
  if (!results) {
    record->setLength(currentRecordLength);
  }

  return errorStatus;
}

int _Tt_isam_file::updateCurrentRecord (const _Tt_isam_record_ptr &record)
{
  iserrno = 0;

  isreclen = record->getLength();
  (void)isrewcurr(fileDescriptor, (char *)record->getRecord());

  getStatusInfo();
  return errorStatus;
}

int
_Tt_isam_file::updateRecord (long recnum, const _Tt_isam_record_ptr &record)
{
  iserrno = 0;

  isreclen = record->getLength();
  (void)isrewrec(fileDescriptor, recnum, (char *)record->getRecord());

  getStatusInfo();
  return errorStatus;
}

int _Tt_isam_file::writeRecord (const _Tt_isam_record_ptr &record)
{
  iserrno = 0;

  isreclen = record->getLength();
  (void)iswrite(fileDescriptor, (char *)record->getRecord());

  getStatusInfo();
  return errorStatus;
}

int _Tt_isam_file::deleteCurrentRecord ()
{
  iserrno = 0;

  (void)isdelcurr(fileDescriptor);

  getStatusInfo();
  return errorStatus;
}

int _Tt_isam_file::deleteRecord (long recnum)
{
  iserrno = 0;

  (void)isdelrec(fileDescriptor, recnum);

  getStatusInfo();
  return errorStatus;
}

int _Tt_isam_file::writeMagicString (const _Tt_string &magic_string)
{
  iserrno = 0;

  (void)iscntl(fileDescriptor,
	       ISCNTL_APPLMAGIC_WRITE,
	       (char *)magic_string);

  errorStatus = iserrno;
  return errorStatus;
}

_Tt_string _Tt_isam_file::readMagicString ()
{
  iserrno = 0;

  _Tt_string magic_string_bytes(ISAPPLMAGICLEN);
  memset((char *)magic_string_bytes, '\0', ISAPPLMAGICLEN);
  
  (void)iscntl(fileDescriptor,
	       ISCNTL_APPLMAGIC_READ,
	       (char *)magic_string_bytes);

  errorStatus = iserrno;

  _Tt_string magic_string = (char *)magic_string_bytes;
  return magic_string;
}

int _Tt_isam_file::setFatalErrorHandler (FatalErrorHandlerFunction function)
{
  iserrno = 0;

  (void)iscntl(fileDescriptor, ISCNTL_FATAL, function);

  errorStatus = iserrno;
  return errorStatus;
}

_Tt_isam_record_ptr
_Tt_isam_file::getFullRecord (const _Tt_string &record_buffer)
{
  _Tt_isam_record_ptr record_ptr = new _Tt_isam_record(keyDescriptorList,
						       maxRecordLength,
						       minRecordLength);
  record_ptr->setBytes(0, currentRecordLength, record_buffer);
  record_ptr->setLength(currentRecordLength);

  return record_ptr;
}

void _Tt_isam_file::getISAMFileInfo ()
{
  iserrno = 0;

  struct dictinfo file_info;
  struct keydesc  key_descriptor;
  short           num_keys;
  int             results = isindexinfo(fileDescriptor,
					(keydesc *)&file_info,
					0);

  getStatusInfo();
  if (!results) {
    keyDescriptorList->flush();

    // If the file has var length records, the MSB is set in the di_nkeys
    // field.  This guarantees to turn the bit off...
    num_keys = file_info.di_nkeys & ~DICTVARLENBIT;

    if (num_keys == 1) {
      (void)isindexinfo(fileDescriptor, &key_descriptor, 1);
      if (key_descriptor.k_nparts == 0) {
	num_keys = 0;
      }
      else {
	num_keys = 1;
      }
    }

    maxRecordLength = file_info.di_recsize;
    minRecordLength = isreclen;
    
    for (int i=1; i <= num_keys; i++) {
      isindexinfo(fileDescriptor, &key_descriptor, i);

      _Tt_isam_key_descriptor_ptr key_descriptor_ptr =
	                          new _Tt_isam_key_descriptor;
      key_descriptor_ptr->keyDescriptor = key_descriptor;

      keyDescriptorList->append(key_descriptor_ptr);
    }

    errorStatus = iserrno;
  }
}

int _Tt_isam_file::isamFatalErrorHandler (char *msg)
{
	_tt_syslog(errstr, LOG_ERR, "NetISAM: %s", msg);
	return 1;
}
