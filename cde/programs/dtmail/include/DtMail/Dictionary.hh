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
/*
 *+SNOTICE
 *
 *
 *	$XConsortium: Dictionary.hh /main/4 1996/04/21 19:44:36 drk $
 *
 *	RESTRICTED CONFIDENTIAL INFORMATION:
 *	
 *	The information in this document is subject to special
 *	restrictions in a confidential disclosure agreement bertween
 *	HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
 *	document outside HP, IBM, Sun, USL, SCO, or Univel wihtout
 *	Sun's specific written approval.  This documment and all copies
 *	and derivative works thereof must be returned or destroyed at
 *	Sun's request.
 *
 *	Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 *
 *+ENOTICE
 */

#ifndef _DTM_DICTIONARY_HH
#define _DTM_DICTIONARY_HH

#include "DtVirtArray.hh"

class DtMailDictionaryImpl : public DtCPlusPlusAllocator {
  public:
    // The hash_size must be <= 256. Setting to smaller sizes
    // will reduce the memory used by the dictionary, but may increase
    // the hash collisions resulting in slower performance.
    //
    DtMailDictionaryImpl(int hash_size = 256);
    ~DtMailDictionaryImpl(void);

    void set(const char * key, const void * value);
    const void * lookup(const char * key);
    void remove(const char * key);

  private:
    struct Entry : public DtCPlusPlusAllocator {
	char *			key;
	const void *		value;
    };

    typedef DtVirtArray<Entry *> * HashTable_t;

    HashTable_t		*_hash_table;
    int			_hash_size;
    void		*_obj_mutex;

    int hashValue(const char * key);
    void locate(int hash_value, const char * key, Entry ** entry);
};

template <class Element>
class DtMailDictionary : public DtCPlusPlusAllocator {
  public:
    DtMailDictionary(int hash_size = 256) : _dict(hash_size) { }
    ~DtMailDictionary(void) { }

    void set(const char * key, const Element value) { _dict.set(key, value); }

    const Element lookup(const char * key) {
	return((const Element)_dict.lookup(key));
    }

    void remove(const char * key) { _dict.remove(key); }

  private:
    DtMailDictionaryImpl	_dict;
    
};

#endif
