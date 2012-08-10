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
//%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			
//%%  (c) Copyright 1993, 1994 International Business Machines Corp.	
//%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			
//%%  (c) Copyright 1993, 1994 Novell, Inc. 				
//%%  $TOG: tt_file_system.C /main/5 1998/03/19 18:59:48 mgreess $ 			 				
//%% 	restrictions in a confidential disclosure agreement between	
//%% 	HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this	
//%% 	document outside HP, IBM, Sun, USL, SCO, or Univel without	
//%% 	Sun's specific written approval.  This document and all copies	
//%% 	and derivative works thereof must be returned or destroyed at	
//%% 	Sun's request.							
//%% 									
//%% 	Copyright 1993, 1994 Sun Microsystems, Inc.  All rights reserved.	
//%% 									
/* @(#)tt_file_system.C	1.40 95/01/06
 *
 * Tool Talk Utility
 *
 * Copyright (c) 1990, 1994 by Sun Microsystems, Inc.
 *
 * Defines a class for browsing the file system mount table.
 *
 */

#include <stdio.h>
#include <string.h>
#if defined(linux)
# include <sys/poll.h>
#else
# include <poll.h>
#endif
#include <sys/stat.h>
#include <fcntl.h>
#include "tt_options.h"
#include "util/tt_port.h"
#include "util/tt_host_equiv.h"

#if defined(OPT_SVR4_GETMNTENT)
#	include <sys/mnttab.h>
#	include <sys/mntent.h>
#	define TtMntTab			MNTTAB
#	define TtMntEntry		struct mnttab
#	define ttOpenMntTbl(path,mode)	fopen(path,mode)
#	define ttFsType(e)		(e).mnt_fstype
#	define ttFsName(e)		(e).mnt_special
#	define ttMountPt(e)		(e).mnt_mountp
#	define ttCloseMntTbl(f)		fclose(f)
#elif defined(_AIX)
	extern "C" int mntctl(int,int,char *);
#	include <sys/vfs.h>
#	define MNTTYPE_NFS		"nfs"
#	define ttOpenMntTbl(path,mode)	((FILE *) 1)
#	define TtMntEntry		struct vmount *
#	define ttFsType(e)		\
				(((e)->vmt_gfstype == MNT_JFS) ? "jfs" \
				: (((e)->vmt_gfstype == MNT_NFS) ? "nfs" \
				 : (((e)->vmt_gfstype == MNT_CDROM) ? "cdrom" \
				  : (((e)->vmt_gfstype == MNT_AIX) ? "oaix" \
				   : "unknown"))))
#	define ttFsName(e) vmt2dataptr(e,VMT_OBJECT)
#	define ttMountPt(e) vmt2dataptr(e,VMT_STUB)
#	define ttCloseMntTbl(f)		free(tmpbuf)    
#elif defined(__osf__) || defined(CSRG_BASED)
#       include <sys/types.h>
#       include <sys/mount.h>
# ifdef __osf__
        extern "C" int getfsstat(struct statfs *, long, int);
# endif
#       define MNTTYPE_NFS              "nfs"
#       define ttOpenMntTbl(path,mode)  ((FILE *) 1)
#       define TtMntEntry               struct statfs *
# ifdef __osf__
#       define ttFsType(e)              \
                                (((e)->f_type == MOUNT_UFS) ? "ufs" \
                                : (((e)->f_type == MOUNT_NFS3) ? "nfs" \
                                : (((e)->f_type == MOUNT_NFS) ? "nfs" \
                                 : (((e)->f_type == MOUNT_CDFS) ? "cdfs" \
                                  : (((e)->f_type == MOUNT_PROCFS) ? "procfs" \
                                   : "unknown")))))
# elif defined(CSRG_BASED)
#	define ttFsType(e)		(e->f_fstypename)
# endif
#       define ttFsName(e)              ((char *)((e)->f_mntfromname))
#       define ttMountPt(e)             ((char *)((e)->f_mntonname))
#       define ttCloseMntTbl(f)         free(buf)
#else
#	include <mntent.h>
#	define TtMntTab			MOUNTED
#	define MNTTYPE_RFS		"rfs"
#	define TtMntEntry		struct mntent *
#	define ttOpenMntTbl(path,mode)	setmntent(path,mode)
#	define ttFsType(e)		(e)->mnt_type
#	define ttFsName(e)		(e)->mnt_fsname
#	define ttMountPt(e)		(e)->mnt_dir
#	define ttCloseMntTbl(f)		endmntent(f)
#endif
#include "util/tt_file_system_entry_utils.h"
#include "util/tt_file_system.h"
#include "util/tt_file_system_entry.h"
#include "util/tt_path.h"
#include "util/tt_port.h"
#include "util/tt_host.h"
#include "util/tt_global_env.h"

#if defined(OPT_BUG_SUNOS_5)
// Bug 1116904 filed on the absence of hasmntopt from the
// header file.  Fixed in SunOS 5.3, but we leave this in so 
// we can compile on 5.1 and 5.2.
extern "C" {
	extern char *hasmntopt(struct mnttab *mnt, char *opt);
};
#endif

time_t				_Tt_file_system::lastMountTime		= 0;

_Tt_file_system::
_Tt_file_system ()
{
	// It would be simpler just to have a static instance of 
	// _Tt_file_system_entry_list here, but static class instances
	// are verboten if we want to be called by C programs.
	if (_tt_global->fileSystemEntries.is_null()) {
		_tt_global->fileSystemEntries = new _Tt_file_system_entry_list;
		lastMountTime = 0;
	}
}

_Tt_file_system::
~_Tt_file_system ()
{
	lastMountTime = 0;
}

static _Tt_file_system_entry *
createFileSystemEntry(TtMntEntry entry)
{
	_Tt_string hostname;
	_Tt_string mount_point;
	_Tt_string partition;
	int        local_flag = 1;
	int        loop_back_flag = 0;
	//
	// not all platforms have the IGNORE option, but if it
	// is defined, be sure to ignore mount entries with it
	// set, as they are things like automounter entries
	// that will just confuse the mapping.
	//
#if defined(MNTOPT_IGNORE)
	if (0!=hasmntopt(&entry, MNTOPT_IGNORE)) {
		return (_Tt_file_system_entry *)0;
	}
#endif
	if (   (strcmp( ttFsType(entry), MNTTYPE_NFS) == 0)
#if defined(MNTTYPE_RFS)
	    || (strcmp( ttFsType(entry), MNTTYPE_RFS) == 0)
#endif
	   )
	{
#if defined(_AIX)
                // AIX is different; at least it\'s arguably better.
                hostname = vmt2dataptr(entry,VMT_HOSTNAME);
		partition = ttFsName(entry);
#else /* AIX */
	
		// Search for a '.' to separate the hostname and
		// the domain name
		char *endOfHostNamePos =
			strchr(ttFsName(entry), '.');

		// Search for the ':' that separates the
		// hostname and the rest of the mtab field.

		char *justBeforePartitionPos =
			strchr(ttFsName(entry), ':');

		// If there was no domain name or the dot
		// found above was after the ':' (the dot was
		// just part of a directoty name), then the
		// hostname ends at the ':' position.

		if (!endOfHostNamePos ||
		    (justBeforePartitionPos < endOfHostNamePos)) {
			endOfHostNamePos = justBeforePartitionPos;
		}

		// NULL terminate the hostname part of the field
		if (endOfHostNamePos) {
			*endOfHostNamePos = '\0';
		}

		if (justBeforePartitionPos) {
			partition = justBeforePartitionPos+1;
		}

		hostname = ttFsName(entry);
#endif	/* AIX */	
		local_flag = FALSE;
		loop_back_flag = FALSE;
	} else {
		partition = ttFsName(entry);
		_Tt_host_ptr localHost;
		_tt_global->get_local_host( localHost );
		hostname = localHost->name();
		local_flag = TRUE;
		loop_back_flag = FALSE;

#if defined(MNTTYPE_LOFS)
		if (!strcmp( ttFsType(entry), MNTTYPE_LOFS)) {
			loop_back_flag = TRUE;
		}
#endif
	}
	mount_point = ttMountPt(entry);

	return new _Tt_file_system_entry( hostname, mount_point, partition,
					  local_flag, loop_back_flag );
}


/*
 * Finds the best match mount table entry to the specified local path
 * resolving loop back mounts to the true mount point.
 */
_Tt_file_system_entry_ptr _Tt_file_system::
bestMatchToPath (const _Tt_string &path)
{
	_Tt_string real_path = _tt_realpath(path);

	updateFileSystemEntries();

	_Tt_file_system_entry_ptr max_match_entry;
	bool_t 		    loop_back_flag = TRUE;
	_Tt_string	    current_loop_back_mount_point;
	_Tt_string	    first_loop_back_mount_point;
	bool_t		    first_time = TRUE;

	while (loop_back_flag) {
		max_match_entry = findBestMountPoint(real_path,
						  loop_back_flag,
						  current_loop_back_mount_point);

		if (loop_back_flag && first_time) {
			first_loop_back_mount_point =
				current_loop_back_mount_point;
		}

		first_time = FALSE;
	}

	max_match_entry->loopBackMountPoint = first_loop_back_mount_point;
	return max_match_entry;
}

/*
 * Finds the best matching mount pount to the specified local path.
 * If the best match is a loop back then the matching portion of the
 * specified path is updated to the loop back partition and the loop
 * back flag parameter is set to TRUE.
 */
_Tt_file_system_entry_ptr _Tt_file_system::
findBestMountPoint (_Tt_string &path,
		    bool_t     &loop_back_flag,
		    _Tt_string &loop_back_mount_point)
{
	updateFileSystemEntries();

	_Tt_string                current_mount_point;
	int                       current_mount_point_length;
	_Tt_file_system_entry_ptr max_match_entry;
	int                       max_match_length = 0;
	int                       path_length = path.len();

	_Tt_file_system_entry_list_cursor
		entries_cursor(_tt_global->fileSystemEntries);

	while (entries_cursor.next()) {
		current_mount_point = entries_cursor->getMountPoint();
		current_mount_point_length = current_mount_point.len();
		if (path_length < current_mount_point_length) {
			continue;
		}

		if (current_mount_point_length > max_match_length) {
			if (!memcmp((char *)current_mount_point,
				    (char *)path,
				    current_mount_point_length)) {
				max_match_length = current_mount_point_length;
				max_match_entry = *entries_cursor;
			}  
		}

		if (max_match_length == path_length) {
			break;
		}
	}

	if (max_match_entry->isLoopBack ()) {
		loop_back_flag = TRUE;

		_Tt_string mount_point = max_match_entry->getMountPoint();
		_Tt_string partition = max_match_entry->getPartition();

		// Get the path info after the mount point path
		path = path.right(path.len()-mount_point.len());

		// Prepend the loop back partition onto the rest of the path
		path = partition.cat(path);

		// Return the loop back mount point
		loop_back_mount_point = mount_point;
	} else {
		loop_back_flag = FALSE;
	}

	return max_match_entry;
}

/*
 * Finds the mount table entry corresponding to the specified network path.
 * The specified path must be of the form:
 *
 *		hostname:/path
 */
_Tt_file_system_entry_ptr _Tt_file_system::
findMountEntry (const _Tt_string &network_path)
{
	updateFileSystemEntries();

	_Tt_string			current_hostname;
	_Tt_string			current_partition;
	int				current_partition_length;
	_Tt_file_system_entry_ptr	entry;

	_Tt_string hostname;
	_Tt_string path;

	path = network_path;
	path = path.split(':', hostname);

	int path_length = path.len();

	_Tt_file_system_entry_list_cursor
		entries_cursor(_tt_global->fileSystemEntries);

	_Tt_host_equiv_ptr eq_p = new _Tt_host_equiv();
	while (entries_cursor.next()) {
		current_hostname = entries_cursor->getHostname();

#ifdef notdef
printf("DEBUG findMountEntry: hostname = %s, current_hostname = %s\n",
	(char *) hostname, (char *) current_hostname);
#endif

		if (eq_p->hostname_equiv(hostname, current_hostname) == 1) {
			current_partition = entries_cursor->getPartition();
			current_partition_length = current_partition.len();

#ifdef notdef
printf("DEBUG findMountEntry: found hostname equivalence between %s and %s\n",
	(char *) hostname, (char *) current_hostname);
#endif

			if (path_length >= current_partition_length) {
				if (!memcmp((char *)path, (char *)current_partition,
					    current_partition_length)) {
					entry = *entries_cursor;
#ifdef notdef
printf("DEBUG findMountEntry: found PATH equivalence between %s and %s\n",
	(char *) hostname, (char *) current_hostname);
#endif

					break;
				}
			}
		}
	}
	return entry;
}

void _Tt_file_system::
updateFileSystemEntries ()
{
	static int firsttime = 1;
        static int since_last = 0;

        if (!firsttime) {
                if (since_last < _tt_global->event_counter) {
                        since_last = _tt_global->event_counter;
                } else {
                        return;
                }
        }

// AIX  doesn\'t have a mount table file as such.
#ifdef TtMntTab

	struct stat mount_table_stat;
	if (stat(TtMntTab, &mount_table_stat)) {
		return;
	}

	if (!firsttime && mount_table_stat.st_mtime <= lastMountTime) {
		return;
	}

	firsttime = 0;
	
	// XXX Due to bug #1126575 - MNTTAB temporarily goes to
	//     size 0 during automounter updates.  The file stats
	//     OK, but has no data in it.
	pollfd poll_fd;
	while (mount_table_stat.st_size == 0) {
		(void)poll (&poll_fd, 0, 100);
		// Must use lstat here; mtab is often a symlink
		if (lstat(TtMntTab, &mount_table_stat)) {
			return;
		}
	}

	FILE *mount_table = ttOpenMntTbl(TtMntTab, "r");

	if (! mount_table) {
		return;
	}
	fcntl(fileno(mount_table), F_SETFD, 1);		// Close on exec.

	lastMountTime = mount_table_stat.st_mtime;
#endif

	_tt_global->fileSystemEntries->flush();
	_Tt_file_system_entry_ptr fse;	
	TtMntEntry entry;
#if defined(_AIX)
	int	rc, sz = BUFSIZ;
	char	*tmpbuf = (char *)malloc( sz );

	while ((rc = mntctl(MCTL_QUERY, sz, tmpbuf)) == 0)
	{
		// increase buffer size
		sz = *((int *) tmpbuf);
		free( tmpbuf );
		tmpbuf = (char *)malloc( sz );
	}

	for (entry = (TtMntEntry)tmpbuf; rc > 0; --rc,
	     entry = (TtMntEntry)((char *) entry + entry->vmt_length))
#elif defined(__osf__) || defined(CSRG_BASED)
        int             numfs,i;
        struct statfs   *buf;
        long            bufsize;
        int             flags = MNT_NOWAIT;
        char            *s, *host, path[MNAMELEN] ;
 
        numfs = getfsstat ( (struct statfs *)0, 0, 0 );

        bufsize = numfs * sizeof ( struct statfs );
        buf = (struct statfs *) malloc ( bufsize );
	memset ((void *)buf,0,bufsize);

        getfsstat ( buf, bufsize, flags );

        for ( i=0; i<numfs; i++ )
        {
            // convert path@host to host:/path
            s = strchr(buf[i].f_mntfromname,'@');
            if (s != NULL) {
                host = s + 1 ;
		 memset((char*)path,0,MNAMELEN);
                strncpy(path,buf[i].f_mntfromname, (strlen(buf[i].f_mntfromname)
 - strlen(s))) ;
                strcpy(buf[i].f_mntfromname,host) ;
                strcat(buf[i].f_mntfromname,":") ;
                strcat(buf[i].f_mntfromname,path) ;
            }
            entry = &buf[i];

#elif defined(OPT_SVR4_GETMNTENT)
	int rc;     
        while (! (rc = getmntent(mount_table, &entry)))
#else		    
	while (entry = getmntent(mount_table))
#endif
#if !defined(__osf__) && !defined(CSRG_BASED)
	{
#endif
		fse =  createFileSystemEntry( entry );
		if (!fse.is_null()) {
#if defined(TT_DEBUG_FNM)
fprintf(stderr,"File name mapping: %s:%s on %s; %s %s\n",
	(char *)fse->getHostname(),
	(char *)fse->getPartition(),
	(char *)fse->getMountPoint(),
	fse->isLocal()?"local":"",
	fse->isLoopBack()?"loopback":"");
#endif			
			_tt_global->fileSystemEntries->append(fse);
                }
        }

        ttCloseMntTbl(mount_table);
}

void _Tt_file_system::
flush ()
{
	if (!_tt_global->fileSystemEntries.is_null()) {
		_tt_global->fileSystemEntries->flush();
	}

	lastMountTime = 0;
}
/* Local Variables : */
/* c++-indent-level: 2 */
/* End: */
