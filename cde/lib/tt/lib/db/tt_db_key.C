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
 * License along with these librararies and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
//%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			
//%%  (c) Copyright 1993, 1994 International Business Machines Corp.	
//%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			
//%%  (c) Copyright 1993, 1994 Novell, Inc. 				
//%%  $XConsortium: tt_db_key.C /main/3 1995/10/23 10:02:59 rswiston $ 			 				
/*
 * @(#)tt_db_key.C	1.10 93/09/07
 + Implements the TT db server key class.
 *
 * Copyright (c) 1992 by Sun Microsystems, Inc.
 */

#include <util/tt_base64.h>
#include <sys/types.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>

#include "util/tt_base64.h"
#include "util/tt_port.h"
#include "db/tt_db_key.h"

_Tt_db_key::_Tt_db_key (short version_number)
{
  static unsigned int last_time_sec = 0;
  static long counter = 0;

  key.version  = version_number;
  key.padding  = 0;
  key.hostid   = _tt_gethostid();
  key.time_sec = time ((time_t *) NULL);

  if (key.time_sec != last_time_sec) {
    last_time_sec = key.time_sec;
    counter       = getpid() * 10000;
  }
  key.counter = counter++;
}

_Tt_db_key::_Tt_db_key (const _Tt_string &string)
{
  // If this is an actual key...
  if ((string.len() == TT_DB_KEY_LENGTH) && (string[0] < '0')) {
    (void)memcpy((char *)&key, (char *)string, TT_DB_KEY_LENGTH);
  }
  // Else, assume this is an objid...
  else {
    _Tt_string key_string = (char *)string;
    _Tt_string temp_string;

    // Get the version number
    key_string = key_string.split('|', temp_string);
    key.version = (unsigned short)_tt_base64_decode(temp_string);
    
    // In the future, if the key format changes, it may be necessary
    // to check the version here and decode according to the version
    // number.
    
    // Set the padding to 2 bytes of 0
    key.padding = (unsigned short)0;
    
    // Get the host ID
    key_string = key_string.split('|', temp_string);
    key.hostid = _tt_base64_decode(temp_string);
    
    // Get the first time component
    key_string = key_string.split('|', temp_string);
    key.time_sec = _tt_base64_decode(temp_string);
    
    // Get the last time component
    if (key_string.index(':') == -1) {
      key.counter = _tt_base64_decode(key_string);
    }
    else {
      key_string = key_string.split(':', temp_string);
      key.counter = _tt_base64_decode(temp_string);
    }
  }
}

_Tt_db_key::~_Tt_db_key ()
{
}

_Tt_string _Tt_db_key::string () const
{
  _Tt_string result(_tt_base64_encode(key.version));
  result = result.cat("|");
  result = result.cat(_tt_base64_encode(key.hostid)).cat("|");
  result = result.cat(_tt_base64_encode(key.time_sec)).cat("|");
  result = result.cat(_tt_base64_encode(key.counter));
  return result;
}

int _Tt_db_key::
operator== (const _Tt_db_key &otherkey) const
{
	// Very minor optimization:: compare the most likely differences
	// first
	return (key.time_sec==otherkey.key.time_sec &&
		key.counter==otherkey.key.counter && 
		key.version==otherkey.key.version &&
		key.padding==otherkey.key.padding &&
		key.hostid==otherkey.key.hostid);
}
