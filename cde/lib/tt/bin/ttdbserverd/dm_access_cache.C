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
//%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			
//%%  (c) Copyright 1993, 1994 International Business Machines Corp.	
//%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			
//%%  (c) Copyright 1993, 1994 Novell, Inc. 				
//%%  $XConsortium: dm_access_cache.C /main/3 1995/10/20 16:40:39 rswiston $ 			 				
/*
 *  Tool Talk Database Manager - dm_access_cache.h
 *
 *  Copyright (c) 1989 Sun Microsystems, Inc.
 *
 *  This file contains class implementation for the oid access info cache.
 *
 */

#include "dm_access_cache.h"
#include <memory.h>
#include <stdlib.h>
#include <isam.h>

implement_ptr_to(_Tt_oid_access)
implement_ptr_to(_Tt_oid_access_elem)
implement_ptr_to(_Tt_oid_access_queue)
implement_ptr_to(_Tt_link_access)
implement_ptr_to(_Tt_link_access_elem)
implement_ptr_to(_Tt_link_access_queue)

static char _tt_access_record[ISMAXRECLEN];

/*
 *  class _Tt_oid_access
 */

_Tt_oid_access::_Tt_oid_access(char *ku)
{
	memcpy(_key, ku, OID_KEY_LENGTH);
	memcpy((char *)&_user, ku + OID_KEY_LENGTH, sizeof(uid_t));
	memcpy((char *)&_group, ku + OID_KEY_LENGTH + sizeof(uid_t),
	       sizeof(gid_t));
	memcpy((char *)&_mode,
	       ku + OID_KEY_LENGTH + sizeof(uid_t) + sizeof(gid_t),
	       sizeof(mode_t));
}

_Tt_oid_access::_Tt_oid_access(const char *key, uid_t user, gid_t group,
			       mode_t mode)
{
	memcpy(_key, key, OID_KEY_LENGTH);
	_user = user;
	_group = group;
	_mode = mode;
}

_Tt_oid_access::~_Tt_oid_access()
{
}

char *
_Tt_oid_access::rec()
{
	memcpy(_tt_access_record, _key, OID_KEY_LENGTH);
	memcpy(_tt_access_record + OID_KEY_LENGTH, (char *)&_user,
	       sizeof(uid_t));
	memcpy(_tt_access_record + OID_KEY_LENGTH + sizeof(uid_t),
	       (char *)&_group, sizeof(gid_t));
	memcpy(_tt_access_record + OID_KEY_LENGTH + sizeof(uid_t) + sizeof(gid_t),
	       (char *)&_mode, sizeof(mode_t));
	return _tt_access_record;
}

void
_Tt_oid_access::print(FILE *fs) const
{
	fprintf(fs, "oid-access entry: ");
	fprintf(fs, "key - <%d, %d, %d, %d>, user = %d\n",
		*((short *) ((char *)_key)),
		*((int *) ((char *)_key + 4)), *((int *) ((char *)_key + 8)),
		*((int *) ((char *)_key + 12)), _user);
}

/*
 *  class _Tt_oid_access_elem
 */

_Tt_oid_access_elem::_Tt_oid_access_elem(_Tt_oid_access_ptr oa,
					 _Tt_oid_access_elem_ptr next)
{
	_oa = oa;
	_next = next;
}

void
_Tt_oid_access_elem::print(FILE *fs) const
{
	_oa->print(fs);
}

/*
 *  class _Tt_oid_access_queue
 */

_Tt_oid_access_queue::_Tt_oid_access_queue()
{
	_head = _tail = 0;
	_len = 0;
	for (int i = 0; i < DM_OID_ACCESS_BUCKETS; i++) {
		_table[i] = 0;
	}
}

_Tt_oid_access_queue::~_Tt_oid_access_queue()
{
}

void
_Tt_oid_access_queue::enqueue(_Tt_oid_access_ptr oa)
{
	if (_len == DM_MAX_ACCESS_ELEMS) {
		dequeue();
	}
	/* put the new element on the LRU list */
	_Tt_oid_access_elem_ptr oae = new _Tt_oid_access_elem(oa, _head);
	if (_head.is_null()) {
		_tail = oae;
	}
	_head = oae;
	/* put the new element in the lookup table */
	int bucket = hash(oa->key());
	_table[bucket] = new _Tt_oid_access_elem(oa, _table[bucket]);
	_len++;
}

_Tt_oid_access_ptr
_Tt_oid_access_queue::lookup(const char *key)
{
	if (!key) {	/* erroneous condition, read by record number needs key */
		return 0;
	}
	int bucket_no = hash(key);
	_Tt_oid_access_elem_ptr e = _table[bucket_no];
	while (!e.is_null()) {
		_Tt_oid_access_ptr oa = e->oa();
		if (memcmp(oa->key(), key, OID_KEY_LENGTH) == 0) {
			return oa;
		}
		e = e->next();
	}
	return 0;
}

/*
 *  remove - remove the oid access element from both the LRU list and the lookup
 *  table.  Does not delete the element.
 */

void
_Tt_oid_access_queue::remove(_Tt_oid_access_ptr oa)
{
	/* remove from the LRU list */
	_Tt_oid_access_elem_ptr prev = 0;
	_Tt_oid_access_elem_ptr cur = _head;
	while (!cur.is_null()) {
		if (cur->oa().is_eq(oa)) {
			if (prev.is_null()) {
				_head = _head->next();
			} else {
				prev->set_next(cur->next());
			}
			if (cur.is_eq(_tail)) {
				_tail = prev;
			}
			break;
		} else {
			prev = cur;
			cur = cur->next();
		}
	}
	/* remove from the lookup table */
	int bucket = hash(oa->key());
	prev = 0;
	cur = _table[bucket];
	while (!cur.is_null()) {
		if (cur->oa().is_eq(oa)) {
			if (prev.is_null()) {
				_table[bucket] = _table[bucket]->next();
			} else {
				prev->set_next(cur->next());
			}
			break;
		} else {
			prev = cur;
			cur = cur->next();
		}
	}
	--_len;
}

/*
 *  promote - promote the oid access element to the front of the LRU list.
 */

void
_Tt_oid_access_queue::promote(_Tt_oid_access_ptr oa)
{
	if (_head->oa().is_eq(oa)) {
		return;
	}
	/* remove from the LRU list */
	_Tt_oid_access_elem_ptr prev = _head;
	_Tt_oid_access_elem_ptr cur = prev->next();
	while (!cur.is_null()) {
		if (cur->oa().is_eq(oa)) {
			prev->set_next(cur->next());
			if (cur.is_eq(_tail)) {
				_tail = prev;
			}
			cur->set_next(_head);
			_head = cur;
			break;
		} else {
			prev = cur;
			cur = cur->next();
		}
	}
}

/*
 *  Remove the LRU element from the LRU list and lookup table and deletes it.
 */

void
_Tt_oid_access_queue::dequeue()
{
	if (!_tail.is_null()) {
		remove(_tail->oa());
	}
}

int
_Tt_oid_access_queue::hash(const char *key)
{
	int hash_value = 0;
	for (int i = 0; i < OID_KEY_LENGTH; i++) {
		hash_value += key[i] & 0177;	/* ignore sign */
	}
	hash_value = abs(hash_value);
	return (hash_value % DM_OID_ACCESS_BUCKETS);
}

void
_Tt_oid_access_queue::print(FILE *fs) const
{
	fprintf(fs, "\nOID-ACCESS QUEUE list (len = %d):\n", _len);
	_Tt_oid_access_elem_ptr e = _head;
	while (!e.is_null()) {
		e->print(fs);
		e = e->next();
	}
	fprintf(fs, "OID-ACCESS QUEUE table:\n");
	for (int i = 0; i < DM_OID_ACCESS_BUCKETS; i++) {
		e = _table[i];
		if (!e.is_null()) {
			fprintf(fs, "bucket %d:\n", i);
			while (!e.is_null()) {
				e->print(fs);
				e = e->next();
			}
		}
	}
	fprintf(fs, "\n");
}

/*
 *  class _Tt_link_access
 */

_Tt_link_access::_Tt_link_access(char *kp)
{
	memcpy(_key, kp, OID_KEY_LENGTH);
	memcpy((char *)&_user, kp + OID_KEY_LENGTH, sizeof(uid_t));
	memcpy((char *)&_group, kp + OID_KEY_LENGTH + sizeof(uid_t),
	       sizeof(gid_t));
	memcpy((char *)&_mode,
	       kp + OID_KEY_LENGTH + sizeof(uid_t) + sizeof(gid_t),
	       sizeof(mode_t));
}

_Tt_link_access::_Tt_link_access(const char *key, uid_t user, gid_t group,
				 mode_t mode)
{
	memcpy(_key, key, OID_KEY_LENGTH);
	_user = user;
	_group = group;
	_mode = mode;
}

_Tt_link_access::~_Tt_link_access()
{
}

char *
_Tt_link_access::rec()
{
	memcpy(_tt_access_record, _key, OID_KEY_LENGTH);
	memcpy(_tt_access_record + OID_KEY_LENGTH, (char *)&_user,
	       sizeof(uid_t));
	memcpy(_tt_access_record + OID_KEY_LENGTH + sizeof(uid_t),
	       (char *)&_group, sizeof(gid_t));
	memcpy(_tt_access_record + OID_KEY_LENGTH + sizeof(uid_t) + sizeof(gid_t),
	       (char *)&_mode, sizeof(mode_t));
	return _tt_access_record;
}

void
_Tt_link_access::print(FILE *fs) const
{
	fprintf(fs, "link-access entry: ");
	fprintf(fs, "key - <%d, %d, %d, %d>, user = %d\n",
		*((short *) ((char *)_key)),
		*((int *) ((char *)_key + 4)), *((int *) ((char *)_key + 8)),
		*((int *) ((char *)_key + 12)), _user);
}

/*
 *  class _Tt_link_access_elem
 */

_Tt_link_access_elem::_Tt_link_access_elem(_Tt_link_access_ptr oa,
					 _Tt_link_access_elem_ptr next)
{
	_oa = oa;
	_next = next;
}

void
_Tt_link_access_elem::print(FILE *fs) const
{
	_oa->print(fs);
}

/*
 *  class _Tt_link_access_queue
 */

_Tt_link_access_queue::_Tt_link_access_queue()
{
	_head = _tail = 0;
	_len = 0;
	for (int i = 0; i < DM_OID_ACCESS_BUCKETS; i++) {
		_table[i] = 0;
	}
}

_Tt_link_access_queue::~_Tt_link_access_queue()
{
}

void
_Tt_link_access_queue::enqueue(_Tt_link_access_ptr oa)
{
	if (_len == DM_MAX_ACCESS_ELEMS) {
		dequeue();
	}
	/* put the new element on the LRU list */
	_Tt_link_access_elem_ptr oae = new _Tt_link_access_elem(oa, _head);
	if (_head.is_null()) {
		_tail = oae;
	}
	_head = oae;
	/* put the new element in the lookup table */
	int bucket = hash(oa->key());
	_table[bucket] = new _Tt_link_access_elem(oa, _table[bucket]);
	_len++;
}

_Tt_link_access_ptr
_Tt_link_access_queue::lookup(const char *key)
{
	if (!key) {	/* erroneous condition, read by record number needs key */
		return 0;
	}
	_Tt_link_access_elem_ptr e = _table[hash(key)];
	while (!e.is_null()) {
		_Tt_link_access_ptr oa = e->oa();
		if (memcmp(oa->key(), key, OID_KEY_LENGTH) == 0) {
			return oa;
		}
		e = e->next();
	}
	return 0;
}

/*
 *  remove - remove the oid access element from both the LRU list and the lookup
 *  table.  Does not delete the element.
 */

void
_Tt_link_access_queue::remove(_Tt_link_access_ptr oa)
{
	/* remove from the LRU list */
	_Tt_link_access_elem_ptr prev = 0;
	_Tt_link_access_elem_ptr cur = _head;
	while (!cur.is_null()) {
		if (cur->oa().is_eq(oa)) {
			if (prev.is_null()) {
				_head = _head->next();
			} else {
				prev->set_next(cur->next());
			}
			if (cur.is_eq(_tail)) {
				_tail = prev;
			}
			break;
		} else {
			prev = cur;
			cur = cur->next();
		}
	}
	/* remove from the lookup table */
	int bucket = hash(oa->key());
	prev = 0;
	cur = _table[bucket];
	while (!cur.is_null()) {
		if (cur->oa().is_eq(oa)) {
			if (prev.is_null()) {
				_table[bucket] = _table[bucket]->next();
			} else {
				prev->set_next(cur->next());
			}
			break;
		} else {
			prev = cur;
			cur = cur->next();
		}
	}
	--_len;
}

/*
 *  promote - promote the oid access element to the front of the LRU list.
 */

void
_Tt_link_access_queue::promote(_Tt_link_access_ptr oa)
{
	if (_head->oa().is_eq(oa)) {
		return;
	}
	/* remove from the LRU list */
	_Tt_link_access_elem_ptr prev = _head;
	_Tt_link_access_elem_ptr cur = prev->next();
	while (!cur.is_null()) {
		if (cur->oa().is_eq(oa)) {
			prev->set_next(cur->next());
			if (cur.is_eq(_tail)) {
				_tail = prev;
			}
			cur->set_next(_head);
			_head = cur;
			break;
		} else {
			prev = cur;
			cur = cur->next();
		}
	}
}

/*
 *  Remove the LRU element from the LRU list and lookup table and deletes it.
 */

void
_Tt_link_access_queue::dequeue()
{
	if (!_tail.is_null()) {
		remove(_tail->oa());
	}
}

int
_Tt_link_access_queue::hash(const char *key)
{
	int hash_value = 0;
	for (int i = 0; i < OID_KEY_LENGTH; i++) {
		hash_value += key[i] & 0177;  /* ignore sign */
	}
	hash_value = abs(hash_value);
	return (hash_value % DM_OID_ACCESS_BUCKETS);
}

void
_Tt_link_access_queue::print(FILE *fs) const
{
	fprintf(fs, "\nLINK-ACCESS QUEUE list (len = %d):\n", _len);
	_Tt_link_access_elem_ptr e = _head;
	while (!e.is_null()) {
		e->print(fs);
		e = e->next();
	}
	fprintf(fs, "LINK-ACCESS QUEUE table:\n");
	for (int i = 0; i < DM_OID_ACCESS_BUCKETS; i++) {
		e = _table[i];
		if (!e.is_null()) {
			fprintf(fs, "bucket %d:\n", i);
			while (!e.is_null()) {
				e->print(fs);
				e = e->next();
			}
		}
	}
	fprintf(fs, "\n");
}
