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
//%%  $XConsortium: tt_client_isam_file.C /main/3 1995/10/23 09:59:12 rswiston $ 			 				
/*
 * tt_client_isam_file.cc - Defines the TT ISAM file class.  This class
 * 	simplifies opening, closing, reading and writing an ISAM file.
 *
 * Copyright (c) 1992 by Sun Microsystems, Inc.
 */

#include "db/tt_client_isam.h"
#include "db/tt_client_isam_file.h"
#include "db/tt_old_db_consts.h"

_Tt_client_isam_file::_Tt_client_isam_file (const _Tt_string        &file,
			      int                      mode,
			      const _Tt_db_client_ptr &db_connection)
{
  dbConnection = db_connection;

  setTtISAMFileDefaults();
  fileName = file;
  fileMode = mode;

  if ((fileDescriptor = dbConnection->isopen(file, mode)) != -1) {
    getISAMFileInfo();
  }
  else {
    getStatusInfo();
  }

  maxRecordLength = 0;
  minRecordLength = 0;
}

_Tt_client_isam_file
::_Tt_client_isam_file (const _Tt_string            &file,
		 int                          max_record_length,
		 int                          min_record_length,
		 _Tt_client_isam_key_descriptor_ptr  primary_key_descriptor,
		 int                          mode,
		 const _Tt_db_client_ptr     &db_connection)
{
  dbConnection = db_connection;

  setTtISAMFileDefaults();
  fileName = file;
  fileMode = mode;

  // Because a old TT client may be talking with the old DB, make sure
  // files are opened with a manual lock, not an exclusive lock.
  if ((fileDescriptor =
       dbConnection->isopen(file, mode&(~ISEXCLLOCK)|ISMANULOCK)) == -1) {
    newFlag = TRUE;
    dbConnection->isreclen = min_record_length;
    fileDescriptor = dbConnection->isbuild(file,
					   (max_record_length > TT_OLD_MAX_RECORD_LENGTH ?
				            TT_OLD_MAX_RECORD_LENGTH : max_record_length),
					   primary_key_descriptor->getKeyDescriptor(),
					   mode);
  }
  
  if (fileDescriptor != -1) {
    currentRecordLength = 0;
    currentRecordNumber = 0;
    getISAMFileInfo();
  }
  else {
    errorStatus = dbConnection->iserrno;
    currentRecordLength = -1;
    currentRecordNumber = -1;
  }
  maxRecordLength = 0;
  minRecordLength = 0;
}

void _Tt_client_isam_file::setTtISAMFileDefaults ()
{
  eraseFlag = FALSE;
  fileName = (char *)NULL;
  fileMode = 0;
  keyDescriptorList = new _Tt_client_isam_key_descriptor_list;
  newFlag = FALSE;

  dbConnection->iserrno = 0;
}

_Tt_client_isam_file::~_Tt_client_isam_file ()
{
  if (eraseFlag) {
    (void)dbConnection->iserase(fileName);
  }
  else {
    (void)dbConnection->isclose(fileDescriptor);
  }
}

void _Tt_client_isam_file::setErase (bool_t flag)
{
  eraseFlag = flag;
}

int _Tt_client_isam_file::addIndex (_Tt_client_isam_key_descriptor_ptr key_descriptor)
{
  dbConnection->iserrno = 0;

  int results = dbConnection->isaddindex(fileDescriptor, key_descriptor->getKeyDescriptor());

  errorStatus = dbConnection->iserrno;
  if (!results) {
    getISAMFileInfo();
  }

  return errorStatus;
}

_Tt_client_isam_record_ptr _Tt_client_isam_file::getEmptyRecord ()
{
  return (new _Tt_client_isam_record(keyDescriptorList,
			      maxRecordLength,
			      minRecordLength));
}

int _Tt_client_isam_file
::findStartRecord (const _Tt_client_isam_key_descriptor_ptr &key_descriptor,
		   int                                length,
		   const _Tt_client_isam_record_ptr         &record,
		   int                                mode)
{
  dbConnection->iserrno = 0;

  dbConnection->isreclen = record->getLength();
  (void)dbConnection->isstart(fileDescriptor,
			      key_descriptor->getKeyDescriptor(),
			      length,
			      (char *)record->getRecord(),
			      mode);

  getStatusInfo();
  return errorStatus;
}

_Tt_client_isam_record_ptr _Tt_client_isam_file::readRecord (int mode)
{
  dbConnection->iserrno = 0;

  _Tt_client_isam_record_ptr record = (_Tt_client_isam_record *)NULL;
  _Tt_string          record_buffer(maxRecordLength);
  dbConnection->isreclen = maxRecordLength;
  int                 results = dbConnection->isread(fileDescriptor,
						     (char *)record_buffer,
						     mode);

  getStatusInfo();
  if (!results) {
    record = getFullRecord(record_buffer);
  }

  return record;
}

int _Tt_client_isam_file::readRecord (int                        mode,
			       const _Tt_client_isam_record_ptr &record)
{
  dbConnection->iserrno = 0;

  dbConnection->isreclen = record->getLength();
  int results = dbConnection->isread(fileDescriptor, (char *)record->getRecord(), mode);

  getStatusInfo();
  if (!results) {
    record->setLength(currentRecordLength);
  }

  return errorStatus;
}

int
_Tt_client_isam_file::updateRecord (long recnum, const _Tt_client_isam_record_ptr &record)
{
  dbConnection->iserrno = 0;

  dbConnection->isreclen = record->getLength();
  (void)dbConnection->isrewrec(fileDescriptor, recnum, (char *)record->getRecord());

  getStatusInfo();
  return errorStatus;
}

int _Tt_client_isam_file::writeRecord (const _Tt_client_isam_record_ptr &record)
{
  dbConnection->iserrno = 0;

  dbConnection->isreclen = record->getLength();
  (void)dbConnection->iswrite(fileDescriptor, (char *)record->getRecord());

  getStatusInfo();
  return errorStatus;
}

int
_Tt_client_isam_file::deleteRecord (long                              recnum,
				    const _Tt_client_isam_record_ptr &record)
{
  dbConnection->iserrno = 0;

  dbConnection->isreclen = record->getLength();
  (void)dbConnection->isdelrec(fileDescriptor,
			       recnum,
			       (char *)record->getRecord());

  getStatusInfo();
  return errorStatus;
}

int _Tt_client_isam_file::writeMagicString (const _Tt_string &magic_string)
{
  dbConnection->iserrno = 0;

  (void)dbConnection->iscntl(fileDescriptor,
			     ISCNTL_APPLMAGIC_WRITE,
			     (char *)magic_string);

  errorStatus = dbConnection->iserrno;
  return errorStatus;
}

_Tt_string _Tt_client_isam_file::readMagicString ()
{
  dbConnection->iserrno = 0;

  _Tt_string magic_string_bytes(ISAPPLMAGICLEN);
  memset((char *)magic_string_bytes, '\0', ISAPPLMAGICLEN);
  
  (void)dbConnection->iscntl(fileDescriptor,
			     ISCNTL_APPLMAGIC_READ,
			     (char *)magic_string_bytes);

  errorStatus = dbConnection->iserrno;

  _Tt_string magic_string = (char *)magic_string_bytes;
  return magic_string;
}

_Tt_client_isam_record_ptr
_Tt_client_isam_file::getFullRecord (const _Tt_string &record_buffer)
{
  _Tt_client_isam_record_ptr record_ptr = new _Tt_client_isam_record(keyDescriptorList,
						       maxRecordLength,
						       minRecordLength);
  record_ptr->setBytes(0, currentRecordLength, record_buffer);
  record_ptr->setLength(currentRecordLength);

  return record_ptr;
}

void _Tt_client_isam_file::getISAMFileInfo ()
{
  keyDescriptorList->flush();

  if (!strcmp(TT_OLD_DB_PROPERTY_TABLE_FILE,
	      (char *)fileName+
	              fileName.len()-
	              strlen(TT_OLD_DB_PROPERTY_TABLE_FILE))) {
    maxRecordLength = TT_OLD_MAX_RECORD_LENGTH;
    minRecordLength = TT_OLD_DB_KEY_LENGTH+TT_OLD_DB_MAX_PROPERTY_NAME_LENGTH;

    _Tt_client_isam_key_descriptor_ptr key_descriptor =
                                new _Tt_client_isam_key_descriptor;

    key_descriptor->addKeyPart(TT_OLD_DB_FIRST_KEY_OFFSET,
			       TT_OLD_DB_KEY_LENGTH,
			       BINTYPE);
    key_descriptor->addKeyPart(TT_OLD_DB_PROPERTY_NAME_OFFSET,
			       TT_OLD_DB_MAX_PROPERTY_NAME_LENGTH,
			       CHARTYPE);
    key_descriptor->setDuplicates(TRUE);
    
    keyDescriptorList->append(key_descriptor);
  }
  else if (!strcmp(TT_OLD_DB_ACCESS_TABLE_FILE,
		   (char *)fileName+
		           fileName.len()-
		           strlen(TT_OLD_DB_ACCESS_TABLE_FILE))) {
    maxRecordLength = minRecordLength = TT_OLD_DB_KEY_LENGTH+
                                        3*TT_OLD_DB_SHORT_SIZE;

    _Tt_client_isam_key_descriptor_ptr key_descriptor =
                                new _Tt_client_isam_key_descriptor;

    key_descriptor->addKeyPart(TT_OLD_DB_FIRST_KEY_OFFSET,
			       TT_OLD_DB_KEY_LENGTH,
			       BINTYPE);

    keyDescriptorList->append(key_descriptor);
  }
  else if (!strcmp(TT_OLD_DB_FILE_TABLE_FILE,
		   (char *)fileName+
		           fileName.len()-
		           strlen(TT_OLD_DB_FILE_TABLE_FILE))) {
    maxRecordLength = TT_OLD_DB_KEY_LENGTH+MAXPATHLEN;
    minRecordLength = TT_OLD_DB_KEY_LENGTH+TT_OLD_DB_MAX_KEY_LENGTH;

    _Tt_client_isam_key_descriptor_ptr key_descriptor =
                                new _Tt_client_isam_key_descriptor;
    key_descriptor->addKeyPart(TT_OLD_DB_FIRST_KEY_OFFSET,
			       TT_OLD_DB_KEY_LENGTH,
			       BINTYPE);
    keyDescriptorList->append(key_descriptor);

    key_descriptor = new _Tt_client_isam_key_descriptor;
    key_descriptor->addKeyPart(TT_OLD_DB_FILE_PATH_OFFSET,
			       TT_OLD_DB_MAX_KEY_LENGTH,
			       CHARTYPE);
    keyDescriptorList->append(key_descriptor);
  }
  // TT_OLD_DB_FILE_OBJECT_MAP_FILE
  else {
    maxRecordLength = minRecordLength = 2*TT_OLD_DB_KEY_LENGTH;

    _Tt_client_isam_key_descriptor_ptr key_descriptor =
                                new _Tt_client_isam_key_descriptor;
    key_descriptor->addKeyPart(TT_OLD_DB_FIRST_KEY_OFFSET,
			       TT_OLD_DB_KEY_LENGTH,
			       BINTYPE);
    keyDescriptorList->append(key_descriptor);

    key_descriptor = new _Tt_client_isam_key_descriptor;
    key_descriptor->addKeyPart(TT_OLD_DB_SECOND_KEY_OFFSET,
			       TT_OLD_DB_KEY_LENGTH,
			       CHARTYPE);
    key_descriptor->setDuplicates(TRUE);
    keyDescriptorList->append(key_descriptor);
  }

  errorStatus = dbConnection->iserrno = 0;
}
