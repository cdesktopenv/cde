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
/*%%  $XConsortium: ce.h /main/3 1995/10/23 11:48:35 rswiston $ 			 				 */
/* @(#)ce.h	1.3 %%
 * 
 * XXX: This is a copy of the OW 3.1 CE include file.  It's a horrible
 * thing to do, but we copy this into the ToolTalk build tree because
 * of the way the OW,  DeskSet, and ToolTalk builds are intertwined.
 * Currently ToolTalk must be built before DeskSet and after Classing
 * Engine, because DeskSet uses ToolTalk and ToolTalk uses Classing
 * Engine.  But, Classing Engine is built as part of DeskSet!  To 
 * break this vicious circle, we copy out the CE include we need to
 * compile ToolTalk.  Since we expect to completely get off of
 * Classing Engine in the next feature release of ToolTalk, this
 * is only a temporary measure.
 */


/*
 * Classing Engine data types
 */


#ifndef ce_hdr_DEFINED
#define ce_hdr_DEFINED
#include <xview/xv_c_types.h>

/* CE API flags */
#define	CE_FLAG_TEST_ALL		0
#define	CE_FLAG_TEST_PERMISSIONS	1

/* Name Space handle  - an index in the global table of name spaces */
typedef void *CE_NAMESPACE;

/* Entry handle */
typedef void *CE_ENTRY;

/* Attribute handle */
typedef void *CE_ATTRIBUTE;

#if defined(__cplusplus) || defined(__STDC__)
typedef void *(*CE_NAMESPACE_MAP_FUNCTION)(CE_NAMESPACE, void *);
typedef void *(*CE_ENTRY_MAP_FUNCTION)(CE_NAMESPACE, CE_ENTRY, void *);
typedef void *(*CE_ATTR_MAP_FUNCTION)(CE_ATTRIBUTE, char *, void *);
#else
typedef void *(*CE_NAMESPACE_MAP_FUNCTION)();
typedef void *(*CE_ENTRY_MAP_FUNCTION)();
typedef void *(*CE_ATTR_MAP_FUNCTION)();
#endif

/* the extern routines for the classing engine */

EXTERN_FUNCTION( int ce_abort_write, (int) );
EXTERN_FUNCTION( int ce_add_attribute,
		(CE_NAMESPACE, CE_ENTRY *, CONST char *, CONST char *, CONST char *, int) );
EXTERN_FUNCTION( int ce_add_entry, (CE_NAMESPACE, CE_ENTRY) );
EXTERN_FUNCTION( int ce_add_namespace, (char *, CE_NAMESPACE *) );
EXTERN_FUNCTION( int ce_alloc_entry, (CE_NAMESPACE, CE_ENTRY *) );
EXTERN_FUNCTION( int ce_alloc_ns_entry, (CE_NAMESPACE, CE_ENTRY *) );
EXTERN_FUNCTION( int ce_commit_write, (int) );
EXTERN_FUNCTION( int ce_begin, (void *) );
EXTERN_FUNCTION( int ce_clone_namespace, (CE_NAMESPACE *) );
EXTERN_FUNCTION( int ce_db_changed, (_VOID_) );
EXTERN_FUNCTION( int ce_end, (_VOID_) );
EXTERN_FUNCTION(char *ce_get_attribute,
		(CE_NAMESPACE, CE_ENTRY, CE_ATTRIBUTE) );
EXTERN_FUNCTION( CE_ATTRIBUTE ce_get_attribute_id, (CE_NAMESPACE, CONST char *) );
EXTERN_FUNCTION( char *ce_get_attribute_name, (CE_ATTRIBUTE) );
EXTERN_FUNCTION( int ce_get_attribute_size,
		(CE_NAMESPACE, CE_ENTRY, CE_ATTRIBUTE) );
EXTERN_FUNCTION( char *ce_get_attribute_type,
		(CE_NAMESPACE, CE_ENTRY, CE_ATTRIBUTE) );
EXTERN_FUNCTION( int ce_get_dbs, (int *, char ***, char ***) );
EXTERN_FUNCTION( CE_ENTRY ce_get_entry, (CE_NAMESPACE, int, DOTDOTDOT) );
EXTERN_FUNCTION( int ce_get_entry_db_info,
		(CE_NAMESPACE, CE_ENTRY, char **, char **) );
EXTERN_FUNCTION( CE_NAMESPACE ce_get_namespace_id, (char *) );
EXTERN_FUNCTION( char *ce_get_namespace_name, (CE_NAMESPACE) );
EXTERN_FUNCTION( CE_ENTRY ce_get_ns_entry, (CE_NAMESPACE) );
EXTERN_FUNCTION(void *ce_map_through_attrs,
		(CE_NAMESPACE, CE_ENTRY, CE_ATTR_MAP_FUNCTION, void *) );
EXTERN_FUNCTION( void *ce_map_through_entries,
		(CE_NAMESPACE, CE_ENTRY_MAP_FUNCTION, void *) );
EXTERN_FUNCTION( void *ce_map_through_namespaces,
		(CE_NAMESPACE_MAP_FUNCTION, void *) );
EXTERN_FUNCTION( void *ce_map_through_ns_attrs,
		(CE_NAMESPACE, CE_ATTR_MAP_FUNCTION, void *) );
EXTERN_FUNCTION( int ce_modify_attribute,
		(CE_NAMESPACE, CE_ENTRY *, char *, char *, char *, int) );
EXTERN_FUNCTION( int ce_remove_attribute,
		(CE_NAMESPACE, CE_ENTRY, CE_ATTRIBUTE) );
EXTERN_FUNCTION( int ce_remove_entry, (CE_NAMESPACE, CE_ENTRY) );
EXTERN_FUNCTION( int ce_remove_namespace, (CE_NAMESPACE) );
EXTERN_FUNCTION( int ce_start_write, (char *) );
EXTERN_FUNCTION( int ce_test_ok_to_write, (char *, int) );

#endif ce_hdr_DEFINED
