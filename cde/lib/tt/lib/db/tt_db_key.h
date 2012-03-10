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
#ifdef __osf__
  /* These fields need to be: <host id:4> <time sec:4> <counter:4> */
  unsigned int hostid;
  unsigned int time_sec;
  unsigned int counter;
#else
  unsigned long hostid;
  unsigned long time_sec;
  unsigned long counter;
#endif /* __osf__ */
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
