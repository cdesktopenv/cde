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
/* $XConsortium: restore.C /main/2 1996/11/11 11:10:03 drk $
 *
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1996 Hewlett-Packard Company.
 * (c) Copyright 1996 International Business Machines Corp.
 * (c) Copyright 1996 Sun Microsystems, Inc.
 * (c) Copyright 1996 Novell, Inc. 
 * (c) Copyright 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 */

#include <fstream>
#include <sstream>
using namespace std;

#include "utility/const.h"
#include "utility/randomize.h"

main(int argc, char** argv)
{
  if (argc < 2) {
    cerr << "usage: " << *argv << " filename\n" << flush;
    return 1;
  }

  char schema_header[LBUFSIZ];
  fstream fstr(*(argv+1), ios::in);

  fstr.getline(schema_header, 1024);

  int sz = bytes(*(argv+1)) - strlen(schema_header) - 1;

  char* buf = new char[sz];

  if ( !fstr.read(buf, sz) )
    return 0;

  fstr.close();

  buffer orig(0);
  orig.set_chunk(buf, sz);
  orig.set_content_sz(sz);

  randomize rd(233);
  rd.restore(orig);

  cout << buf << flush;
}

