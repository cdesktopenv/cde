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
/*%%  $XConsortium: tt_db_key.h /main/4 1996/07/30 17:41:16 barstow $ 			 				 */
/*
 * tt_db_key.h - Defines the TT db server key class.  This class is used to
 *               create unique object keys and object IDs.  The key layout
 *               is:
 *
 * <00> <zero pad:2> <host id:4> <time sec:4> <counter:4>
 *
 * The padding goes before the host id to align the host id on an int boundary.
 *
 * Copyright (c) 1992 by Sun Microsystems, Inc.
 */

#ifndef  _TT_DB_KEY_H
#define  _TT_DB_KEY_H

#include "util/tt_object.h"
#include "util/tt_string.h"

struct _Tt_key_data {
  unsigned short version;
  unsigned short padding;
  unsigned long hostid;
  unsigned long time_sec;
  unsigned long counter;
};

const int TT_DB_KEY_LENGTH = sizeof(_Tt_key_data);

class _Tt_db_key : public _Tt_object {
public:
  _Tt_db_key (short version_number=0);
  _Tt_db_key (const _Tt_string&);
  ~_Tt_db_key ();

  _Tt_string string () const;
  _Tt_string binary () const
    {
      _Tt_string binary_string(TT_DB_KEY_LENGTH);
      memcpy((char *)binary_string, (char *)&key, TT_DB_KEY_LENGTH); 
      return binary_string;
    }
    int operator==(const _Tt_db_key &otherkey) const;
    int operator!=(const _Tt_db_key &otherkey) const
		{ return ! operator==(otherkey);}
    void print(FILE *fs) const
     { fprintf(fs, "%s", (char *)(this->string())); }



private:
  _Tt_key_data key;
};

#endif  /* _TT_DB_KEY_H */
