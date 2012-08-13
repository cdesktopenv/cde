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
/*
 *+SNOTICE
 *
 *
 *	$XConsortium: HashTable.C /main/3 1995/11/06 16:37:14 rswiston $
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

#ifndef I_HAVE_NO_IDENT
#endif

#include <string.h>
#include <DtMail/HashTable.hh>

HashTableImpl::HashTableImpl(int table_size)
{
    _table_size = table_size;
    _hash_table = new HashEntry[table_size];
    memset(_hash_table, 0, table_size * sizeof(HashEntry));
}

HashTableImpl::~HashTableImpl(void)
{
  // Scan the entire hash table, deleting all keys and chains, and
  // eventually delete the hash table itself
  //
  for (int slot = 0; slot < _table_size; slot++) {
    if (_hash_table[slot].key != NULL) {
      HashEntry * chain;
      HashEntry * chainHead;
      for (chainHead = chain = &_hash_table[slot]; chain; chain = chain->next) {
	if (chain->key)
	  delete chain->key;
	if (chain != chainHead)
	  delete chain;
      }
    }
  }
  delete _hash_table;
}

void *
HashTableImpl::lookup(ObjectKey & key)
{
    short hash_key = key.hashValue();
    HashEntry *chain;

    int slot = hash_key % _table_size;

    // Search the slot looking for the value. Return NULL if there
    // are no objects matching this key.
    //
    for (chain = &_hash_table[slot]; chain; chain = chain->next) {
	if (chain->key && key == *(chain->key)) {
	    break;
	}
    }

    if (chain) {
	return(chain->value);
    }

    return(NULL);
}

void
HashTableImpl::set(ObjectKey & key, void * value)
{
    short hash_key = key.hashValue();
    int slot = hash_key % _table_size;
    HashEntry *chain;

    // See if we have already filled the slot.
    //
    if (_hash_table[slot].key == NULL) {
	// Simple, put it in the slot.
	//
	_hash_table[slot].key = &key;
	_hash_table[slot].value = value;
	return;
    }

    // We either have a collision or a duplicate. In the case of duplicates
    // we simply replace the value.
    //
    for (chain = &_hash_table[slot]; chain->next; chain = chain->next) {
	// If this item is already stored then update the value.
	//
	if (key == *(chain->key)) {
	    chain->value = value;
	    return;
	}
    }

    HashEntry * new_ent = new HashEntry;
    new_ent->key = &key;
    new_ent->value = value;
    new_ent->next = NULL;

    chain->next = new_ent;
}

void *
HashTableImpl::remove(ObjectKey & key)
{
    short hash_val = key.hashValue();
    int slot = hash_val % _table_size;
    void * removed_val = NULL;
    HashEntry *chain;

    // See if we even have this object.
    //
    if (!_hash_table[slot].key) {
	// Obviously not.
	//
	return(removed_val);
    }

    // Try to find it in the chain.
    //
    HashEntry * last = NULL;
    for (chain = &_hash_table[slot]; chain; chain = chain->next) {
	if (key == *(chain->key)) {
	    break;
	}
	last = chain;
    }

    if (!chain) { // Not found
	return(removed_val);
    }

    if (last) {
	last->next = chain->next;
	delete chain->key;
	removed_val = chain->value;
	delete chain;
    }
    else {
	// This is the first entry. In this case we copy the next entry
	// into this memory and through away the next item. If we have
	// no next, then simply zero the slot.
	//
	removed_val = chain->value;
	delete chain->key;
	if (chain->next) {
	    *chain = *(chain->next);
	    delete chain->next;
	}
	else {
	    memset(chain, 0, sizeof(HashEntry));
	}
    }
    return(removed_val);
}

#if !defined(HPUX) && !defined(__uxp__)
void
HashTableImpl::forEach(HashImplIterator iterator, void * client_data)
{
    // Scan the entire hash table, passing valid entries to the
    // iterator.
    //
    for (int slot = 0; slot < _table_size; slot++) {
	if (_hash_table[slot].key == NULL) {
	    continue;
	}

	for (HashEntry * chain = &_hash_table[slot]; chain; chain = chain->next) {
	    int cont = 0;
	    cont = iterator(*chain->key, chain->value, client_data);
	    if (!cont) {
		return;
	    }
	}
    }
}
#endif
