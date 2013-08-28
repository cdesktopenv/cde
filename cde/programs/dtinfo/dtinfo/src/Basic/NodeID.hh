/*
 * $XConsortium: NodeID.hh /main/3 1996/06/11 16:20:47 cde-hal $
 *
 * Copyright (c) 1991 HaL Computer Systems, Inc.  All rights reserved.
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

#include <string.h>

class NodeID {
  public:
    NodeID(const char *filename);
    ~NodeID();
    const char *filename()	{ return f_filename ; }
    const char *node_name (node_name_style_t)
      // NOTE: temp hack
      { return f_filename; }

  private:
    char	*f_filename ;
};

inline
NodeID::NodeID(const char *filename)
{
    int len = strlen(filename);
    f_filename = new char[len + 1] ;
    *((char *) memcpy(f_filename, filename, len) + len) = '\0';
}

inline 
NodeID::~NodeID()
{
    delete f_filename ;
}
