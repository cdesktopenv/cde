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
/* $XConsortium: SGMLDefn.cc /main/2 1996/06/04 16:44:32 rcs $
 *
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1996 Hewlett-Packard Company.
 * (c) Copyright 1996 International Business Machines Corp.
 * (c) Copyright 1996 Sun Microsystems, Inc.
 * (c) Copyright 1996 Novell, Inc. 
 * (c) Copyright 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 */

#include <assert.h>
#include <string.h>

#include "SGMLName.h"
#include "SGMLDefn.h"

SGMLDefn::SGMLDefn()
{
  sys_id = NULL;
  pub_id = NULL;
  file_name = NULL;
  name      = -1;
  next      = NULL;
  type      = INVALID;
}

SGMLDefn::~SGMLDefn()
{
  delete sys_id;
  delete pub_id;
  delete file_name;
}

void
SGMLDefn::store_sys_id( char *sid )
{
  if ( sid ) {
    delete sys_id;

    int len = strlen(sid);
    sys_id = new char [len + 1];
    assert(sys_id != NULL);
    
    *((char *) memcpy(sys_id, sid, len) + len) = '\0';
  }
}

void
SGMLDefn::store_pub_id( char *pid )
{
  if ( pid ) {
    delete pub_id;

    int len = strlen(pid);
    pub_id = new char [len + 1];
    assert(pub_id != NULL);
    
    *((char *) memcpy(pub_id, pid, len) + len) = '\0';
  }
}

void
SGMLDefn::store_defn( DEFN_TYPE dt, char *defnStr )
{
  if ( dt == ENTITY_TYPE ) {
    type = dt;
    char *tmp = strtok( defnStr, "\n\t ");

    name = SGMLName::intern(tmp);
  }

}

void
SGMLDefn::store_file_name( char *fname )
{
  if ( fname ) {
    delete file_name;

    int len = strlen(fname);
    file_name = new char [len + 1];
    assert(file_name != NULL);

    *((char *) memcpy(file_name, fname, len) + len) = '\0';
  }
}

SGMLDefn &
SGMLDefn::operator=( SGMLDefn &defn )
{
  store_sys_id( defn.sys_id );
  store_pub_id( defn.pub_id );
  store_file_name( defn.file_name );
  type = defn.type;
  name = defn.name;
  next = NULL;

  return (*this);
}
    



