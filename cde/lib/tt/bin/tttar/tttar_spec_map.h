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
/*%%  $XConsortium: tttar_spec_map.h /main/3 1995/10/20 17:01:17 rswiston $ 			 				 */
/*
 * tttar_spec_map.h - Interface to spec maps for the LS/TT archive tool
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 *
 */

#ifndef _LSTAR_SPEC_MAP_H
#define _LSTAR_SPEC_MAP_H

#include <util/tt_object.h>
#include <util/tt_list.h>
#include <util/tt_string.h>
#include <util/tt_table.h>

/*
 * SET() - Set a private member to the formal parameter of its set method.
 */
#define SET(identifier)				\
{						\
	name2(_,identifier) = identifier;	\
}

class Lstar_spec_map : public _Tt_object {
    public:
	Lstar_spec_map();
	~Lstar_spec_map();

	_Tt_string	old_id() {return _old_id;};
	_Tt_string	new_id() {return _new_id;};
	_Tt_string	path() {return _path;};
	void		old_id_set( _Tt_string old_id )	SET(old_id);
	void		new_id_set( _Tt_string new_id )	SET(new_id);
	void		path_set(   _Tt_string path   )	SET(path);
	void		print(FILE *fs = stdout) const;

    private:
	_Tt_string	_old_id;
	_Tt_string	_new_id;
	_Tt_string	_path;
};

declare_list_of(Lstar_spec_map)
declare_table_of(Lstar_spec_map,old_id,_Tt_string)

#endif /* _LSTAR_SPEC_MAP_H */
