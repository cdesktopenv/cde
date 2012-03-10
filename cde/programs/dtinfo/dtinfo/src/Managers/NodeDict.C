/*
 * $XConsortium: NodeDict.cc /main/3 1996/06/11 16:26:50 cde-hal $
 *
 * Copyright (c) 1992 HaL Computer Systems, Inc.  All rights reserved.
 * UNPUBLISHED -- rights reserved under the Copyright Laws of the United
 * States.  Use of a copyright notice is precautionary only and does not
 * imply publication or disclosure.
 * 
 * This software contains confidential information and trade secrets of HaL
 * Computer Systems, Inc.  Use, disclosure, or reproduction is prohibited
 * without the prior express written permission of HaL Computer Systems, Inc.
 * 
 *                         RESTRICTED RIGHTS LEGEND
 * Use, duplication, or disclosure by the Government is subject to
 * restrictions as set forth in subparagraph (c)(l)(ii) of the Rights in
 * Technical Data and Computer Software clause at DFARS 252.227-7013.
 *                        HaL Computer Systems, Inc.
 *                  1315 Dell Avenue, Campbell, CA  95008
 * 
 */

#define C_NodeObject
#define L_Other

#define C_NodeDict
#define C_NodeMgr
#define C_NodeWindowMgr
#define L_Managers

#include "Prelude.h"

class NodeHandle;

// /////////////////////////////////////////////////////////////////
// get_node_handle
// /////////////////////////////////////////////////////////////////

NodeHandle &
NodeDict::get_node_handle (const ObjectId &oid)
{
  NodeHandle *node_object;

  node_object = node_exists(oid);

  if (node_object == NULL)
    {
      node_object = new NodeObject (f_node_mgr, oid);
      f_node_table.add (*(NodeHandle*)node_object);
    }

  // NOTE: Argggg!  We need exception handling!!

  return *node_object;
}

// /////////////////////////////////////////////////////////////////
// forget_node_object
// /////////////////////////////////////////////////////////////////

void
NodeDict::forget_node_object (NodeObject &node_object)
{
  f_node_table.remove (node_object);
}
