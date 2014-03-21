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
//%%  $XConsortium: tttar_api.C /main/4 1995/10/20 16:59:54 rswiston $ 			 				
/*
 * tttar_api.cc - ToolTalk object archiving interface functions.
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 *
 */

#include <errno.h>
#if defined(__osf__) || defined(linux) || defined(CSRG_BASED) || defined(sun)
#include <unistd.h>
#else
#ifndef USL
#include <osfcn.h>
#endif
#endif
#include <sys/param.h>
#include "api/c/api_api.h"
#include "api/c/tt_c.h"
#include "util/tt_path.h"
#include "util/tt_gettext.h"
#include "tttar_utils.h"
#include "tttar_file_utils.h"
#include "tttar_spec.h"
#include "tttar_api.h"

/*
 * Type definitions
 */


/*
 * Constants
 */

// Number of buckets in a hash table of interesting specs in an archive.
#define SPEC_MAP_SIZE	1000

/*
 * Private functions
 */
static bool_t		path_lstt_archive(
				_Tt_string	path,
				int		verbosity,
				XDR	       *xdrs );
bool_t			dearchive_this_path(
				char *path, void *ppaths_to_extract );
static Tt_filter_action		gather_specs( const char *spec_id, void *,
				void *specs);

/*
 * pathlist_lstt_archive() - Archive the LS/TT objects in the given paths.
 */
bool_t
pathlist_lstt_archive(
	_Tt_string_list_ptr	paths,
	bool_t			recurse,
	bool_t			follow_symlinks,
	int			verbosity,
	XDR		       *xdrs )
{
	_Tt_string_list_ptr	realpaths2tar;
	Object_kind		obj_kind;
	_Tt_string_list_ptr	paths_copy(new _Tt_string_list);
	_Tt_string_list_cursor	path_cursor( paths );
	while (path_cursor.next()) {
		paths_copy->append( *path_cursor );
	}
	
	bool_t need_preliminary_pass = follow_symlinks && recurse;
	realpaths2tar = realtrees( paths_copy, need_preliminary_pass );
	
	obj_kind = VERSION_NUM;
	int version = CURRENT_ARCHIVE_VERSION;
	if (    (! xdr_enum( xdrs, (enum_t *)&obj_kind ))
		|| (! xdr_int( xdrs, &version )))
	{
		fprintf( stderr, "%s: ! xdr_enum() || ! xdr_int()\n",
			 (char *)our_process_name );
		return FALSE;
	}
	while (! paths_copy->is_empty()) {
		_Tt_string_list_ptr	children;
		_Tt_string		path( paths_copy->top() );
		
		paths_copy->pop();
		if (! path_lstt_archive( path, verbosity, xdrs ))
		{
			return FALSE;
		}
		if (recurse) {
			children = _tt_dir_entries( path, follow_symlinks );
			children->append_destructive( paths_copy );
			paths_copy = children;
		}
	}
	obj_kind = ARCHIVE_END;
	if (! xdr_enum( xdrs, (enum_t *)&obj_kind )) {
		fprintf( stderr, "%s: ! xdr_enum()\n",
			 (char *)our_process_name );
		return FALSE;
	}
	return TRUE;
	
} /* pathlist_lstt_archive() */

/*
* pathlist_lstt_dearchive() - Extract the LS/TT objects of the given paths.
 *	If no paths are given, extract everything in the archive.
 */
bool_t
pathlist_lstt_dearchive(
	_Tt_string_list_ptr	paths_to_extract,
	Lstar_string_map_list_ptr renamings,
	_Tt_string		where_to_dearchive,
	bool_t			preserve__props,
	int			verbosity,
	XDR		       *xdrs )
{
	_Tt_string			last_path;
	char			       *this_path	= NULL;
	int				num_specs	= 0;
	int				num_links	= 0;
	bool_t				last_path_valid	= FALSE;
	Object_kind			obj_kind	= NO_KIND;
	Lstar_string_map_table_ptr	spec_map;
	int				mem_mark	= tt_mark();

	spec_map = new Lstar_string_map_table(Lstar_string_map_old_string,
					      SPEC_MAP_SIZE );
	do {
		bool_t	just_dearchived_spec = FALSE;
		bool_t	just_dearchived_link = FALSE;

		if (! xdr_enum( xdrs, (enum_t *)&obj_kind )) {
			fprintf( stderr,
				 catgets(_ttcatd, 7, 4,
					 "%s: Could not read object kind "
					 "from archive stream.\n"),
				 (char *)our_process_name );
			return FALSE;
		}
		switch (obj_kind) {
		    case VERSION_NUM:
			int version;
			if (! xdr_int( xdrs, &version)) {
				fprintf( stderr,
					 catgets(_ttcatd, 7, 5,
						 "%s: Could not read archive ver"
						 "sion from archive stream.\n"),
					 (char *)our_process_name );
				return FALSE;
			}
			if (version != CURRENT_ARCHIVE_VERSION) {
				fprintf( stderr,
					 catgets(_ttcatd, 7, 6,
						 "%s: Found archive version %d, "
						 "but expected version %d.\n"),
					 (char *)our_process_name, version,
					 CURRENT_ARCHIVE_VERSION );
				return FALSE;
			}
			break;
		    case SPEC:
			char	       *old_spec_id;
			char	       *new_spec_id;
			Tt_status	err;

			old_spec_id	= NULL;
			new_spec_id	= NULL;
			err		= TT_OK;

			if (! spec_dearchive( &old_spec_id, &new_spec_id,
					      &this_path, renamings,
					      (char *)where_to_dearchive,
					      preserve__props,
					      dearchive_this_path,
					      (void *)&paths_to_extract,
					      verbosity,
					      xdrs, &err ))
			{
				my_tt_release( mem_mark );
				return FALSE;
			}
			if (new_spec_id != NULL) {
				Lstar_string_map_ptr m = new Lstar_string_map;

				m->old_string_set( old_spec_id );
				m->new_string_set( new_spec_id );
				m->extra_set( this_path );
				spec_map->insert( m );
				num_specs++;
				just_dearchived_spec = TRUE;
			}
			break;
		    case ARCHIVE_END:
			break;
		    case NO_KIND:
		    default:
			fprintf( stderr,
				 catgets(_ttcatd, 7, 7,
					 "%s: found object of unknown kind "
					 "%d in archive.\n"),
				 (char *)our_process_name, (int)obj_kind );
			return FALSE;
		}
		if (verbosity && (    (last_path != (const char *)this_path)
				 || (obj_kind == ARCHIVE_END)))
		{
			if (last_path_valid) {
				if (just_dearchived_spec) {
					num_specs--;
				} else if (just_dearchived_link) {
					num_links--;
				}
				if (verbosity > 1) {
					fprintf( stderr, "\n" );
				}
				if ( (num_specs > 0) || (num_links > 0)) {
					fprintf( stderr, "x %s %d %s\n",
						 (char *)last_path, num_specs,
						 (   num_specs == 1
						   ? "spec" : "specs" ));
				}
				num_specs = 0;
				num_links = 0;
				if (just_dearchived_spec) {
					num_specs = 1;
				} else if (just_dearchived_link)  {
					num_links = 1;
				}
			}
			last_path = this_path;
			if (! last_path_valid) {
				last_path_valid = TRUE;
			}
		}
	} while (obj_kind != ARCHIVE_END);
	my_tt_release( mem_mark );
	return TRUE;

} /* pathlist_lstt_dearchive() */

/*
 * pathlist_lstt_archive_list() - List the LS/TT objects of the given paths.
 *	If no paths are given, list everything in the archive.
 */
bool_t
pathlist_lstt_archive_list(
	_Tt_string_list_ptr	paths_to_list,
	int			verbosity,
	XDR		       *xdrs )
{
	_Tt_string	last_path;
	_Tt_string	this_path;
	int		num_specs	= 0;
	int		num_links	= 0;
	bool_t		last_path_valid	= FALSE;
	Object_kind	obj_kind	= NO_KIND;
	Lstar_string_map_table_ptr	spec_map;
	int		mem_mark	= tt_mark();

	spec_map = new Lstar_string_map_table(Lstar_string_map_old_string,
					      SPEC_MAP_SIZE );
	do {
		Lstar_spec	spec;

		if (! xdr_enum( xdrs, (enum_t *)&obj_kind )) {
			fprintf( stderr,
				 catgets(_ttcatd, 7, 8,
					 "%s: Could not read object kind "
					 "from archive stream.\n"),
				 (char *)our_process_name );
			return FALSE;
		}
		switch (obj_kind) {
		    case VERSION_NUM:
			int version;
			if (! xdr_int( xdrs, &version)) {
				fprintf( stderr,
					 catgets(_ttcatd, 7, 9,
						 "%s: Could not read archive ver"
						 "sion from archive stream.\n"),
					 (char *)our_process_name );
				return FALSE;
			}
			if (version != CURRENT_ARCHIVE_VERSION) {
				fprintf( stderr,
					 catgets(_ttcatd, 7, 10,
						 "%s: Found archive version %d, "
						 "but expected version %d.\n"),
					 (char *)our_process_name, version,
					 CURRENT_ARCHIVE_VERSION );
				return FALSE;
			}
			break;
		    case SPEC:
			if (! spec.xdr(xdrs)) {
				my_tt_release( mem_mark );
				return FALSE;
			}
			if (dearchive_this_path( (char *)spec.path(),
						   &paths_to_list ))
			{
				Lstar_string_map_ptr m = new Lstar_string_map;

				/*
				 * Insert it into this "map" just so that
				 * we can use the map to figure out if
				 * a given link counts under paths_to_list.
				 */
				m->old_string_set( spec.id() );
				m->extra_set( spec.path() );
				spec_map->insert( m );
				this_path = spec.path();
				num_specs++;
				if (verbosity > 1) {
					spec.print( stdout );
				}
			} else {
				continue;
			}
			break;
		    case ARCHIVE_END:
			break;
		    case NO_KIND:
		    default:
			fprintf( stderr,
				 catgets(_ttcatd, 7, 11,
					 "%s: found object of unknown kind "
					 "%d in archive.\n"),
				 (char *)our_process_name, (int)obj_kind );
			return FALSE;
		}
		if (    (last_path != this_path)
		     || (obj_kind == ARCHIVE_END))
		{
			if (last_path_valid) {
				if (obj_kind == SPEC) {
					num_specs--;
				} else if (obj_kind == SUN_LINK) {
					num_links--;
				}
				printf( "%s %d %s\n",
				        (char *)last_path, num_specs,
				        (num_specs == 1 ? "spec" : "specs" ));
				num_specs = 0;
				num_links = 0;
				if (obj_kind == SPEC) {
					num_specs = 1;
				} else if (obj_kind == SUN_LINK)  {
					num_links = 1;
				}
			}
			last_path = this_path;
			if (! last_path_valid) {
				last_path_valid = TRUE;
			}
		}
	} while (obj_kind != ARCHIVE_END);
	my_tt_release( mem_mark );
	return TRUE;

} /* pathlist_lstt_archive_list() */

/*
 * path_lstt_archive() - Archive the specs on the given path. 
 */
static bool_t
path_lstt_archive(
	_Tt_string		path,
	int			verbosity,
	XDR		       *xdrs )
{
	_Tt_string_list	       *specs;
	Object_kind		obj_kind;
	int			num_specs_archived = 0;
	int			num_links_archived = 0;
	bool_t			val2return	   = TRUE;

	specs		= new _Tt_string_list;
	note_err( tt_file_objects_query( (char *)path, gather_specs, NULL, specs ));
	if (IS_TT_ERR(err_noted)) {
		delete specs;
		return TRUE;
	}
	while (! specs->is_empty()) {
		_Tt_string  spec = specs->top();
		Tt_status   tt_err;

		obj_kind = SPEC;
		specs->pop();
		if (! xdr_enum( xdrs, (enum_t *)&obj_kind )) {
			fprintf( stderr, "%s: ! xdr_enum()\n",
				 (char *)our_process_name );
			val2return = FALSE;
			break;
		}
		if (! spec_archive( (char *)spec, (char *)path, verbosity,
				    xdrs, &tt_err ))
		{
			val2return = FALSE;
			break;
		}
		num_specs_archived++;
	}
	if ((verbosity && num_specs_archived > 0 ) || (verbosity > 1)) {
		if (verbosity > 1) {
			fprintf( stderr, "\n" );
		}
		fprintf( stderr, "a %s: %d %s\n", (char *)path,
		         num_specs_archived,
		         ((num_specs_archived == 1) ? "spec" : "specs" ));
	}
	delete specs;
	return val2return;

} /* path_lstt_archive() */


/*
 * spec_archive() - Archive a spec onto the given XDR stream.
 */
bool_t
spec_archive( char *id, char *path, int verbosity, XDR *xdrs, Tt_status *err )
{
	_Tt_string	_path( path );
	_Tt_string	_id( id );

	*err = TT_OK;
	if (xdrs->x_op == XDR_ENCODE) {
		Lstar_spec spec( _id, _path );

		*err = spec.read_self();
		if (IS_TT_ERR(*err)) {
			return FALSE;
		}
		if (! spec.xdr(xdrs)) {
			return FALSE;
		}
		if (verbosity > 1) {
			spec.print( stderr );
		}
	} else {
		return FALSE;
	}
	return TRUE;

} /* spec_archive() */

/*
 * spec_dearchive() - Recreate a spec that was archived on this XDR stream.
 */
bool_t
spec_dearchive(
	char  	      **old_spec_id_ptr,
	char  	      **new_spec_id_ptr,
	char	      **path_as_archived,
	Lstar_string_map_list_ptr renamings,
	char  	       *where_to_create,
	bool_t		preserve__props,
	bool_t	      (*dearchive_this_path)(char *, void *),
	void	       *context,
	int		verbosity,
	XDR    	       *xdrs,
 	Tt_status      *err )
{
	_Tt_string	where( where_to_create );

	*err = TT_OK;
	*old_spec_id_ptr = NULL;
	*new_spec_id_ptr = NULL;
	*path_as_archived = NULL;
	if (xdrs->x_op == XDR_ENCODE) {
		return FALSE;
	} else if (xdrs->x_op == XDR_DECODE) {
		Lstar_spec spec;
		_Tt_string path;

		if (! spec.xdr(xdrs)) {
			return FALSE;
		}
		*old_spec_id_ptr = _tt_strdup( spec.id() );
		*path_as_archived = _tt_strdup( spec.path() );
		path = spec.path();
		if (    (dearchive_this_path == NULL)
		     || (dearchive_this_path( (char *)path, context )))
		{
			_Tt_string _new_name = new_name( spec.path(),
						         renamings );
			if (_new_name.len() > 0) {
				if (verbosity > 2) {
					fprintf( stderr, "%s => ",
						 (char *)spec.path() );
				}
				spec.path_set( _new_name );
				if (verbosity > 2) {
					fprintf( stderr, "%s\n",
						 (char *)spec.path() );
				}
			}
			*new_spec_id_ptr =
				spec.write_self( where, preserve__props, err );
			if (! IS_TT_ERR(*err) && (verbosity > 1)) {
				spec.print( stdout );
			}
		}
	}
	return TRUE;

} /* spec_dearchive() */


/*
 * dearchive_this_path() - Should we extract this <path> from its archive?
 *
 *	Returns true if *ppaths_to_extract is an empty list.
 */
bool_t
dearchive_this_path( char *path, void *ppaths_to_extract )
{
	_Tt_string_list_ptr	paths;
	_Tt_string		_path( path );

	if (ppaths_to_extract == NULL) {
		return TRUE;
	}
	paths = *(_Tt_string_list_ptr *)ppaths_to_extract;
	if (paths->count() <= 0) {
		return TRUE;
	}
	return is_child_in( _path, paths );

} /* dearchive_this_path() */

/*
 * gather_specs()
 */
static Tt_filter_action
gather_specs( const char *spec_id, void *, void *specs )
{
	_Tt_string id = spec_id;
	((_Tt_string_list *)specs)->push( id );
	return TT_FILTER_CONTINUE;
}
