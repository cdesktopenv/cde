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

    sys_id = new char [ strlen(sid) + 1 ];
    assert(sys_id != NULL);
    
    strcpy ( sys_id , sid );
  }
}

void
SGMLDefn::store_pub_id( char *pid )
{
  if ( pid ) {
    delete pub_id;

    pub_id = new char [ strlen(pid) + 1 ];
    assert(pub_id != NULL);
    
    strcpy ( pub_id, pid );
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

    file_name = new char [ strlen(fname) + 1 ];
    assert(file_name != NULL);

    strcpy( file_name, fname );
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
    



