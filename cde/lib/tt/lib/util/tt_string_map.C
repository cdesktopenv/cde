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
//%%  $TOG: tt_string_map.C /main/10 1999/10/14 18:42:44 mgreess $ 

/* @(#)tt_string_map.C	1.10 95/01/06
 *
 * Tool Talk Utility
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 *
 * Defines a class for holding string maps.  This
 * class is used for 2 different things at this time:
 *
 *	_Tt_db_hostname_redirection_map
 *	_Tt_db_partition_redirection_map
 */
#include <fcntl.h>
#include "util/tt_string_map.h"
#include "util/tt_map_entry.h"

/* Included after "util/tt_string.h" to avoid index/strchr conflicts. */
#define X_INCLUDE_STRING_H
#define XOS_USE_NO_LOCKING
#if defined(linux) || defined(CSRG_BASED)
#define index
#define rindex
#endif
#include <X11/Xos_r.h>
#if defined(linux) || defined(CSRG_BASED)
#undef index
#undef rindex
#endif

_Tt_string_map::
_Tt_string_map (_Tt_object_table_keyfn key_function)
{
	mapEntries = new _Tt_map_entry_table(key_function);
}

_Tt_string_map::
_Tt_string_map ()
{
	mapEntries = new _Tt_map_entry_table((_Tt_object_table_keyfn)
					     &_Tt_map_entry::getAddress);
}

_Tt_string_map::
~_Tt_string_map()
{
}

void _Tt_string_map::
loadFile (const _Tt_string &file)
{
	const int   MAX_BUFFER_LEN = 1023;
	const char *SEPARATORS = " \t";

	mapEntries->flush();

	FILE *fp = fopen(file, "r");

	if (fp) {
	  
		_Tt_string buffer(MAX_BUFFER_LEN);

		bool_t throw_away = FALSE;

		fcntl(fileno(fp), F_SETFD, 1);	/* Close on exec */

		while (fgets(buffer, MAX_BUFFER_LEN+1, fp)) {
			int new_line_index = buffer.index('\n');

			// Throw away the characters read because no
			// new-line was found in the last buffer and
			// we only handle the first 1023 characters in a line
			if (throw_away) {
				// If there is a new-line, then keep the
				// next buffer
				if (new_line_index > -1) {
					throw_away = FALSE;
				}
				continue;
			}

			// If the current buffer has no new-line in it,
			// remember to throw away the next buffer
			if (new_line_index == -1) {
				throw_away = TRUE;
			}
			else {
				buffer [new_line_index] = '\0';
			}

			_Xstrtokparams strtok_buf;
		        memset((char*) &strtok_buf, 0, sizeof(_Xstrtokparams));

			char *address_token = 
			  _XStrtok(buffer, SEPARATORS, strtok_buf);
			if (address_token) {
				// Make sure this is not a comment line
				if (*address_token == '#') {
					continue;
				}

				char *data_token = 
				  _XStrtok(NULL, SEPARATORS, strtok_buf);
				if (data_token) {
					_Tt_map_entry_ptr entry = new _Tt_map_entry;
					entry->address = address_token;
					entry->data = data_token;
					mapEntries->insert(entry);
				}
			}
		}
		
		(void)fclose(fp);
	}
	return;
}
  
_Tt_string _Tt_string_map::findEntry(const _Tt_string &address)
{
	_Tt_map_entry_table_ptr repeat_reference_map =
	  new _Tt_map_entry_table((_Tt_object_table_keyfn)
				  &_Tt_map_entry::getAddress);

	_Tt_map_entry_ptr repeat_entry;
	_Tt_map_entry_ptr current_entry = new _Tt_map_entry;
	_Tt_map_entry_ptr next_entry = mapEntries->lookup(address);

	// Chain the references to the entries in the map
	while (!next_entry.is_null()) {
		repeat_reference_map->insert(next_entry);

		current_entry = next_entry;
		next_entry = mapEntries->lookup(current_entry->data);

		if (!next_entry.is_null()) {
			// Check if we have seen this entry before. If we
			// have, then we have a reference loop in the table,
			// so return a NULL string...
			repeat_entry
			  = repeat_reference_map->lookup(next_entry->address);
			if (!repeat_entry.is_null()) {
				return _Tt_string((char *)NULL);
			}
		}
	}
	return current_entry->data;
}
