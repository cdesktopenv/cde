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
//%%  $XConsortium: tt_db_rpc_routines.C /main/3 1995/10/23 10:05:24 rswiston $ 			 				
/* %W @(#)
 * tt_db_rpc_routines.cc - Defines routines for converting TT DB classes to
 *                         RPC arguments.
 *
 * Copyright (c) 1992 by Sun Microsystems, Inc.
 */

#include <string.h>

#include "util/tt_string.h"
#include "util/tt_xdr_utils.h"
#include "db/tt_db_rpc_routines.h"
#include "db/tt_db_access.h"
#include "db/tt_db_property.h"

// If this file is being compiled with -g, then DEFINE_NEW_AND_DELETE
// must be defined in order to prevent a dependency on the libC library.
// The dependency is due to the fact that cfront generates an extern
// reference to vec_new and vec_delete if it sees any access to arrays
// of calsses or structures.  In the case of this file, tt_message_list
// contains a pointer to a structure that causes the references to be
// generated.
#ifdef DEFINE_NEW_AND_DELETE
extern "C" {
void *__nw__FUi (unsigned int)
{ return (void *)NULL; }
 
void __dl__FPv (void *)
{}
 
void *__vec_new (void *, int, int, void *)
{ return (void *)NULL; }

void __vec_delete (void *, int, int, void *, int, int)
{}
}
#endif

void _tt_free_rpc_message (const _tt_message &rpc_message)
{
  if (rpc_message.body.body_val) {
    free(rpc_message.body.body_val);
  }
}

void _tt_free_rpc_messages (const _tt_message_list &rpc_messages)
{
  if (rpc_messages.messages_val) {
    for (unsigned int i=0; i < rpc_messages.messages_len; i++) {
      _tt_free_rpc_message(rpc_messages.messages_val [i]);
    }

    free((MALLOCTYPE *)rpc_messages.messages_val);
  }
}

void _tt_free_rpc_strings (const _tt_string_list &rpc_strings)
{
  if (rpc_strings.values_val) {
    for (unsigned int i=0; i < rpc_strings.values_len; i++) {
      if (rpc_strings.values_val [i].value) {
	free(rpc_strings.values_val [i].value);
      }
    }
    
    free((MALLOCTYPE *)rpc_strings.values_val);
  }
}

void _tt_free_rpc_property (const _tt_property &rpc_prop)
{
  if (rpc_prop.name) {
    free(rpc_prop.name);
  }
  
  if (rpc_prop.values.values_val) {
    for (unsigned int i=0; i < rpc_prop.values.values_len; i++) {
      if (rpc_prop.values.values_val [i].value.value_val) {
	free(rpc_prop.values.values_val [i].value.value_val);
      }
    }

    free((MALLOCTYPE *)rpc_prop.values.values_val);
  }
}

void _tt_free_rpc_properties (const _tt_property_list &rpc_props)
{
  if (rpc_props.properties_val) {
    for (unsigned int i=0; i < rpc_props.properties_len; i++) {
      _tt_free_rpc_property(rpc_props.properties_val [i]);
    }

    free((MALLOCTYPE *)rpc_props.properties_val);
  }
}

void _tt_get_rpc_access (const _tt_access  &rpc_access,
			 _Tt_db_access_ptr &access)
{
  access = new _Tt_db_access;
  access->user = rpc_access.user;
  access->group = rpc_access.group;
  access->mode = rpc_access.mode;
}		   

void _tt_get_rpc_strings (const _tt_string_list &rpc_strings,
			  _Tt_string_list_ptr   &strings)
{
  strings = new _Tt_string_list;

  if (rpc_strings.values_len) {
    for (unsigned int i=0; i < rpc_strings.values_len; i++) {
      (void)strings->append(_Tt_string(rpc_strings.values_val [i].value));
    }
  }
}

void _tt_get_rpc_property (const _tt_property  &rpc_prop,
			   _Tt_db_property_ptr &prop)
{
  prop = new _Tt_db_property;

  if (rpc_prop.name) {
    prop->name = rpc_prop.name;
    
    for (unsigned int i=0; i < rpc_prop.values.values_len; i++) {
      int prop_value_length = rpc_prop.values.values_val [i].value.value_len;
      if (prop_value_length) {
        _Tt_string prop_value(prop_value_length);

        memcpy((char *)prop_value,
	       rpc_prop.values.values_val [i].value.value_val,
	       prop_value_length);
        (void)prop->values->append(prop_value);
      }
    }
  }
}

void _tt_get_rpc_properties (const _tt_property_list  &rpc_props,
			     _Tt_db_property_list_ptr &props)
{
  props = new _Tt_db_property_list;

  if (rpc_props.properties_len) {
    for (unsigned int i=0; i < rpc_props.properties_len; i++) {
      _Tt_db_property_ptr prop;
      
      _tt_get_rpc_property (rpc_props.properties_val [i], prop);
      if (prop->name.len()) {
        (void)props->append(prop);
      }
    }
  }
}

void _tt_set_rpc_access (const _Tt_db_access_ptr &access,
			 _tt_access              &rpc_access)
{
  if (access.is_null()) {
    rpc_access.user = (uid_t)-1;
    rpc_access.group = (gid_t)-1;
    rpc_access.mode = (mode_t)-1;
  }
  else {
    rpc_access.user = access->user;
    rpc_access.group = access->group;
    rpc_access.mode = access->mode;
  }
}   

void _tt_set_rpc_strings (const _Tt_string_list_ptr &strings,
			  _tt_string_list           &rpc_strings)
{
  if (strings.is_null()) {
    rpc_strings.values_val = (_tt_string *)NULL;
    rpc_strings.values_len = 0;
  }
  else {
    int strings_count = strings->count ();

    rpc_strings.values_val = (_tt_string *)
                             malloc(sizeof(_tt_string) * strings_count);
    rpc_strings.values_len = strings_count;

    int i = 0;
    _Tt_string_list_cursor strings_cursor(strings);
    while (strings_cursor.next()) {
      rpc_strings.values_val [i].value = strdup((char *)*strings_cursor);
      i++;
    }
  }
}
  
void _tt_set_rpc_property (const _Tt_db_property_ptr &prop,
			   _tt_property              &rpc_prop)
{
  if (prop.is_null()) {
    rpc_prop.name = (char *)NULL;
    rpc_prop.values.values_len = 0;
    rpc_prop.values.values_val = (_tt_property_value *)NULL;
  }
  else {
    int prop_values_count = prop->values->count ();

    rpc_prop.name = strdup((char *)prop->name);
    rpc_prop.values.values_len = prop_values_count;
    rpc_prop.values.values_val = (_tt_property_value *)
			         malloc (sizeof(_tt_property_value) *
				         prop_values_count);

    int i = 0;
    _Tt_string_list_cursor values_cursor(prop->values);
    while (values_cursor.next()) {
      _tt_property_value *temp_value = rpc_prop.values.values_val + i;
      int                 temp_value_length = (*values_cursor).len();

      temp_value->value.value_val = (char *)malloc(temp_value_length+1);
      temp_value->value.value_len = temp_value_length;
      (void)memcpy (temp_value->value.value_val,
		    (char *)*values_cursor,
		    temp_value_length);
      temp_value->value.value_val[temp_value_length] = '\0';
      i++;
    }
  }
}

void _tt_set_rpc_properties (const _Tt_db_property_list_ptr &props,
			     _tt_property_list              &rpc_props)
{
  if (props.is_null()) {
    rpc_props.properties_len = 0;
    rpc_props.properties_val = (_tt_property *)NULL;
  }
  else {
    int props_count = rpc_props.properties_len = props->count();

    if (!props_count) {
      rpc_props.properties_val = (_tt_property *)NULL;
    }
    else {
      rpc_props.properties_val = (_tt_property *)
			         malloc(sizeof(_tt_property) * props_count);

      int i = 0;
      _Tt_db_property_list_cursor props_cursor(props);
      while (props_cursor.next()) {
        _tt_set_rpc_property (*props_cursor, rpc_props.properties_val [i]);
        i++;
      }
    }
  }
}
