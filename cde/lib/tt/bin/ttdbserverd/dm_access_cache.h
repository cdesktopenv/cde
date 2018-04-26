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
/*%%  $XConsortium: dm_access_cache.h /main/3 1995/10/20 16:40:47 rswiston $ 			 				 */
/*
 *  Tool Talk Database Manager - dm_access_cache.h
 *
 *  Copyright (c) 1989 Sun Microsystems, Inc.
 *
 *  This file contains class declarations for the oid access info cache.
 *
 */

#if !defined(_DM_ACCESS_CACHE_H)
#define _DM_ACCESS_CACHE_H

#include <util/tt_object.h>
#include <util/tt_string.h>
#include <tt_const.h>
#include <sys/param.h>

#include <limits.h>
#if !defined(NGROUPS)
#define NGROUPS	NGROUPS_MAX
#endif

#define DM_OID_ACCESS_BUCKETS 97
#define DM_MAX_ACCESS_ELEMS  (5 * DM_OID_ACCESS_BUCKETS)

/*
 *  OID keys' access info cache
 */

class _Tt_oid_access : public _Tt_object {
      public:
	_Tt_oid_access() { _user = 0; _group = 0; _mode = 0; }
	_Tt_oid_access(const char *key, uid_t user, gid_t group, mode_t mode);
	_Tt_oid_access(char *ku);
	~_Tt_oid_access();
	const char	*key() const { return _key; }
	uid_t		user() const { return _user; }
	gid_t		group() const { return _group; }
	mode_t		mode() const { return _mode; }
	void		set_user(uid_t user) { _user = user; }
	void		set_group(gid_t group) { _group = group; }
	void		set_mode(mode_t mode) { _mode = mode; }
	int		reclen() const { return OID_KEY_LENGTH +
					 sizeof(uid_t) + sizeof(gid_t) +
					 sizeof(mode_t); }
	char		*rec();
	void		print(FILE *fs = stdout) const;
      private:
	char		_key[OID_KEY_LENGTH];
	uid_t		_user;
	gid_t		_group;
	mode_t		_mode;
};

declare_ptr_to(_Tt_oid_access)

class _Tt_oid_access_elem;
declare_ptr_to(_Tt_oid_access_elem)

class _Tt_oid_access_elem : public _Tt_object {
      public:
	_Tt_oid_access_elem() {}
	_Tt_oid_access_elem(_Tt_oid_access_ptr oa, _Tt_oid_access_elem_ptr next);
	_Tt_oid_access_ptr	oa() { return _oa; }
	_Tt_oid_access_elem_ptr	next() { return _next; }
	void			set_next(_Tt_oid_access_elem_ptr next)
	{ _next = next; }
	void			print(FILE *fs = stdout) const;
      private:
	_Tt_oid_access_ptr	_oa;
	_Tt_oid_access_elem_ptr	_next;
};

class _Tt_oid_access_queue : public _Tt_object {
      public:
	_Tt_oid_access_queue();
	~_Tt_oid_access_queue();
	void			enqueue(_Tt_oid_access_ptr oa);
	_Tt_oid_access_ptr	lookup(const char *key);
	void			remove(_Tt_oid_access_ptr oa);
	void			promote(_Tt_oid_access_ptr oa);
	void			print(FILE *fs = stdout) const;
      private:
	void			dequeue();
	int			hash(const char *key);

	_Tt_oid_access_elem_ptr	_head;
	_Tt_oid_access_elem_ptr	_tail;
	int			_len;
	_Tt_oid_access_elem_ptr	_table[DM_OID_ACCESS_BUCKETS];
};

declare_ptr_to(_Tt_oid_access_queue)


/*
 *  LINK keys' access info cache
 */

class _Tt_link_access : public _Tt_object {
      public:
	_Tt_link_access() { _user = 0; _group = 0; _mode = 0; }
	_Tt_link_access(const char *key, uid_t user, gid_t group, mode_t mode);
	_Tt_link_access(char *ku);
	~_Tt_link_access();
	const char	*key() const { return _key; }
	uid_t		user() const { return _user; }
	gid_t		group() const { return _group; }
	mode_t		mode() const { return _mode; }
	void		set_user(uid_t user) { _user = user; }
	void		set_group(gid_t group) { _group = group; }
	void		set_mode(mode_t mode) { _mode = mode; }
	int		reclen() const { return OID_KEY_LENGTH +
					 sizeof(uid_t) + sizeof(gid_t) +
					 sizeof(mode_t); }
	char		*rec();
	void		print(FILE *fs = stdout) const;
      private:
	char		_key[OID_KEY_LENGTH];
	uid_t		_user;
	gid_t		_group;
	mode_t		_mode;
};

declare_ptr_to(_Tt_link_access)

class _Tt_link_access_elem;
declare_ptr_to(_Tt_link_access_elem)

class _Tt_link_access_elem : public _Tt_object {
      public:
	_Tt_link_access_elem() {}
	_Tt_link_access_elem(_Tt_link_access_ptr oa, _Tt_link_access_elem_ptr next);
	_Tt_link_access_ptr	oa() { return _oa; }
	_Tt_link_access_elem_ptr	next() { return _next; }
	void			set_next(_Tt_link_access_elem_ptr next)
	{ _next = next; }
	void			print(FILE *fs = stdout) const;
      private:
	_Tt_link_access_ptr	_oa;
	_Tt_link_access_elem_ptr	_next;
};

class _Tt_link_access_queue : public _Tt_object {
      public:
	_Tt_link_access_queue();
	~_Tt_link_access_queue();
	void			enqueue(_Tt_link_access_ptr oa);
	_Tt_link_access_ptr	lookup(const char *key);
	void			remove(_Tt_link_access_ptr oa);
	void			promote(_Tt_link_access_ptr oa);
	void			print(FILE *fs = stdout) const;
      private:
	void			dequeue();
	int			hash(const char *key);

	_Tt_link_access_elem_ptr	_head;
	_Tt_link_access_elem_ptr	_tail;
	int				_len;
	_Tt_link_access_elem_ptr	_table[DM_OID_ACCESS_BUCKETS];
};

declare_ptr_to(_Tt_link_access_queue)

// Here follow a bunch of common declarations for dbserver components.
// They really should have their own file. (dbserver_commmon.h?)
// They got stuck in dm_access_cache since they have do do with
// internal dbserver caching (but for fd's.)
#define _TT_MAX_ISFD  128

/* structure containing info open ISAM files: full path and opener's uids */
struct _Tt_db_info {
	_Tt_string	db_path;
	uid_t		opener_uid;
	int		open_mode;	// mode file was opened with
	int		client_has_open; // 1 iff we think somebodys using it
	int		server_has_open; // 1 iff we opened it
	int		reftime;	 // "time" of last reference for LRU
};

int  cached_isopen(const char *filepath, int mode);
int  cached_isclose(int isfd);
void isgarbage_collect();


#endif /* _DM_ACCESS_CACHE_H */
