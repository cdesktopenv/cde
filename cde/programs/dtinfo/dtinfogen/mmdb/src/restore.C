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

#include <fstream.h>
#include <strstream.h>

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

  int sz = bytes(fstr) - strlen(schema_header) - 1;

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

