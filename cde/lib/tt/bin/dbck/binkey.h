/*%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			 */
/*%%  (c) Copyright 1993, 1994 International Business Machines Corp.	 */
/*%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			 */
/*%%  (c) Copyright 1993, 1994 Novell, Inc. 				 */
/*%%  $XConsortium: binkey.h /main/3 1995/10/20 16:25:01 rswiston $ 			 				 */
/*
 *
 * binkey.h
 * 
 * Simple class for manipulating 16-byte keys.
 * This class simply encapsulates the storage and (lexical) comparison
 * of 16-byte binary keys.  Generally, _Tt_db_key is the class to
 * use to hold and manipulate keys; only code which depends on the
 * lexical ordering of keys (which should only be the inspect-and-
 * repair tools) should use this class.
 *
 * Copyright (c) 1991 by Sun Microsystems, Inc.
 */

#ifndef _BINKEY_H
#define _BINKEY_H

#include "tt_const.h"
#include "util/tt_object.h"
#include "util/tt_string.h"
#include "db/tt_db_key.h"
#include "db/tt_db_key_utils.h"
#include <stdio.h>
#include <memory.h>

class Binkey : public _Tt_object {
      private:
	unsigned char	_binkey[OID_KEY_LENGTH];
	_Tt_db_key_ptr	_key;
      public:
	static Binkey smallest;
	static Binkey largest;
	Binkey();
	Binkey(const unsigned char *);
	~Binkey() {};
	operator _Tt_string() const {return _key->string();};
	operator char*() const {return (char *)_binkey;};
	_Tt_db_key_ptr key() const {return _key;};
	friend int operator==(const Binkey &a, const Binkey &b);
	friend int operator!=(const Binkey &a, const Binkey &b) {
		return !(a==b);};
	friend int operator==(const Binkey &a, const _Tt_db_key &b);
	friend int operator!=(const Binkey &a, const _Tt_db_key &b) {
		return !(a==b);};
	friend int operator<(const Binkey &a, const Binkey &b);
	friend int operator>(const Binkey &a, const Binkey &b);
	Binkey &operator=(const Binkey &k);
	Binkey &operator=(const unsigned char *);
	virtual void print(FILE *f = stdout) const;
};

declare_list_of(Binkey)

#endif /* _BINKEY_H */
