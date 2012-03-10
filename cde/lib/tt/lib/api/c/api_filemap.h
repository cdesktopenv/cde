/*%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			 */
/*%%  (c) Copyright 1993, 1994 International Business Machines Corp.	 */
/*%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			 */
/*%%  (c) Copyright 1993, 1994 Novell, Inc. 				 */
/*%%  $XConsortium: api_filemap.h /main/3 1995/10/23 09:52:36 rswiston $ 			 				 */

/*
 *
 * @(#)api_filemap.h	1.5 94/11/17
 *
 * Copyright (c) 1990, 1993 by Sun Microsystems, Inc.
 *
 * This file implements the api filename mapping calls to map from
 * an absolute pathname of the form /pathname to canonical names
 * of the format hostname:/pathname.
 */
#ifndef _TT_API_FILEMAP_H
#define _TT_API_FILEMAP_H

#include "api/c/tt_c.h"
#include "util/tt_string.h"

class _Tt_api_filename_map : public _Tt_object  {
	public:
		_Tt_api_filename_map() { };
		~_Tt_api_filename_map() { };

		_Tt_string set_filename(const _Tt_string & filename);

		_Tt_string parse_netfilename(const _Tt_string & netfilename);

		_Tt_string canonical_path_val();

		_Tt_string lpath_val();

		_Tt_string rpath_val();

		_Tt_string hostname_val();


	private:
		// local name of the file by the
		// machine that hosts it.
		_Tt_string _canonical_path;

		// local name of the file by the
		// machine that hosts it.
		_Tt_string _lpath;

		// exported name of the file by the
		// machine that hosts it.
		_Tt_string _rpath;

		// name of the host which exports
		// the file.
		_Tt_string _hostname;
};

declare_ptr_to(_Tt_api_filename_map)

#endif                          /* _TT_API_FILEMAP_H */
