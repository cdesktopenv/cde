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
/*%%  $XConsortium: copier.h /main/3 1995/10/20 16:36:05 rswiston $ 			 				 */
/*
 * copier.h - Interface to copier, an LS/TT-aware cp(1).
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 *
 */

#ifndef _COPIER_H
#define _COPIER_H

#include <api/c/tt_c.h>
#include <util/tt_string.h>

class copier : public _Tt_object {
    public:
	copier( char *arg0 );
	virtual ~copier();

	int			do_cp();
	Tt_status		do_ttcp();
	bool_t			can_cp( _Tt_string from_path );
	Tt_status		open_tt();
	Tt_status		close_tt();
	void			parse_args( int argc, char **argv );
	void			usage( FILE *fs = stderr ) const;
	bool_t			should_cp() { return _should_cp; }
	bool_t			tt_opened() { return _tt_opened; }

    private:
	void			_parse_arg( char *arg );

	_Tt_string		_process_name;
	_Tt_string		_prog_name;
	_Tt_string		_process_id;
	_Tt_string_list_ptr	_args;
	bool_t			_should_cp;
	bool_t			_recurse;
	bool_t			_preserve;
	bool_t			_tt_opened;
	_Tt_string_list_ptr	_from_paths;
	_Tt_string		_to_path;
	bool_t			_to_path_is_dir;
	bool_t			_clonedir_mode;		/* see parse_args() */
};

#endif /* _COPIER_H */
