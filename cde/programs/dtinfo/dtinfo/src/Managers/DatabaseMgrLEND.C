/*
 * $XConsortium: DatabaseMgrLEND.cc /main/3 1996/06/11 16:24:25 cde-hal $
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

#define C_DatabaseMgr
#define L_Managers

#include "Prelude.h"

LONG_LIVED_CC(DatabaseMgr,database_mgr);

DatabaseMgr::DatabaseMgr()
: f_infolib_open(FALSE)
{
  f_olias_server = new olias_server (true);
}

DatabaseMgr::~DatabaseMgr()
{
  delete f_olias_server;
  ON_DEBUG (puts ("DatabaseMgr destructed"));
}

olias_server *
DatabaseMgr::olias_db()
{
  if (!f_infolib_open)
    f_olias_server->init_infolib();
  return (f_olias_server);
}
