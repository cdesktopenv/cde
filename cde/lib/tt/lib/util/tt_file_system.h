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
/*%%  $XConsortium: tt_file_system.h /main/3 1995/10/23 10:38:25 rswiston $ 			 				 */
/* @(#)tt_file_system.h	1.13 93/09/07
 * Tool Talk Utility - tt_file_system.h
 *
 * Copyright (c) 1992,1993 by Sun Microsystems, Inc.
 *
 * Declares a class for browsing the file system mount table.
 *
 */

#ifndef  _TT_FILE_SYSTEM_H
#define  _TT_FILE_SYSTEM_H

#include <netdb.h>
#include <sys/param.h>
#include "util/tt_new.h"
#include "util/tt_file_system_entry_utils.h"
#include "util/tt_string.h"
#include "util/tt_host_utils.h"

class _Tt_file_system : public _Tt_allocated
{
      public:
	_Tt_file_system();
	~_Tt_file_system();

	_Tt_file_system_entry_ptr bestMatchToPath (const _Tt_string&);

	_Tt_file_system_entry_ptr findMountEntry (const _Tt_string&);

	static void flush ();

      private:
	static time_t				lastMountTime;

	_Tt_file_system_entry_ptr findBestMountPoint (_Tt_string&,
						      bool_t&,
						      _Tt_string&);

	void updateFileSystemEntries ();

};

#endif /* _TT_FILE_SYSTEM_H */
