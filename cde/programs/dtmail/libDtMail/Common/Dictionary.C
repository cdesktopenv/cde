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
 *	$TOG: Dictionary.C /main/5 1997/12/22 16:32:11 bill $
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

#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <DtMail/Threads.hh>
#include <DtMail/Dictionary.hh>

DtMailDictionaryImpl::DtMailDictionaryImpl(int size)
{
    _hash_size = size;

    _hash_table = (HashTable_t *)malloc(sizeof(HashTable_t) * _hash_size);

    for (int h = 0; h < _hash_size; h++) {
	_hash_table[h] = NULL;
    }

    _obj_mutex = MutexInit();
}

DtMailDictionaryImpl::~DtMailDictionaryImpl(void)
{
    MutexLock lock_scope(_obj_mutex);

    if (_hash_table) {
	for (int h = 0; h < _hash_size; h++) {
	    if (_hash_table[h]) {
		delete _hash_table[h];
	    }
	}
	free(_hash_table);
	_hash_table = NULL;
    }

    lock_scope.unlock_and_destroy();
}

void
DtMailDictionaryImpl::set(const char * key, const void * value)
{
    MutexLock lock_scope(_obj_mutex);

    int hash = hashValue(key);

    // See if this key is already there. If so, simply update the value.
    //
    Entry * ent;
    locate(hash, key, &ent);

    if (ent) {
	ent->value = value;
	return;
    }

    // New key/value. Add it to the appropriate slot.
    //
    if (_hash_table[hash] == NULL) {
	_hash_table[hash] = new DtVirtArray<Entry *>(16);
    }

    DtVirtArray<Entry *> * slot = _hash_table[hash];

    ent = new Entry;
    ent->key = strdup(key);
    ent->value = value;

    slot->append(ent);
    return;
}

const void *
DtMailDictionaryImpl::lookup(const char * key)
{
    MutexLock lock_scope(_obj_mutex);

    int hash = hashValue(key);

    Entry * ent;
    locate(hash, key, &ent);
    const void * value = NULL;
    if (ent) {
	value = ent->value;
    }

    return(value);
}

void
DtMailDictionaryImpl::remove(const char * key)
{
    MutexLock lock_scope(_obj_mutex);

    int hash = hashValue(key);

    // Locate the entry. This approach will cause us to search
    // the hash list twice, but that should be cheap enough that
    // we can afford to do it.
    //
    Entry * ent;
    locate(hash, key, &ent);
    if (ent == NULL) {
	// Not here, just return.
	return;
    }

    DtVirtArray<Entry *> * slot = _hash_table[hash];
    int loc = slot->indexof(ent);
    slot->remove(loc);

    return;
}

int
DtMailDictionaryImpl::hashValue(const char * key)
{
    int hash = 0;
    for (const char * cur = key; *cur; cur++) {
	hash ^= *cur;
    }

    hash %= _hash_size;

    return(hash);
}

void
DtMailDictionaryImpl::locate(int hash, const char * key, Entry ** ent)
{
    *ent = NULL;

    DtVirtArray<Entry *> * slot = _hash_table[hash];

    if (slot == NULL) {
	return;
    }

    for (int col = 0; col < slot->length(); col++) {
	Entry * t_ent = (*slot)[col];

	if (strcmp(t_ent->key, key) == 0) {
	    *ent = t_ent;
	    break;
	}
    }

    return;
}
