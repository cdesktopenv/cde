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
 *	$TOG: IO.hh /main/10 1999/01/29 14:43:20 mgreess $
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

#ifndef _IO_HH
#define _IO_HH

#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <utime.h>
#include <pwd.h>
#include <grp.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <stdio.h>

#include <DtMail/DtMailError.hh>

//
// The following I/O routines are wrappers for the normal routines,
// but they deal with EINTR, and partial read/write situations.
//
int	SafeAccess(const char * path, int amode);
int	SafeClose(int fd);
int	SafeDup2(int fd1, int fd2);
int	SafeExecvp(const char * file, char *const *argv);
int	SafeFchown(int fd, uid_t owner, gid_t group);
int	SafeFChmod(int fd, mode_t mode);
int	SafeFStat(int fd, struct stat * buf);
int	SafeFTruncate(int fd, off_t len);
int	SafeGuaranteedStat(const char * path, struct stat * buf);
int	SafeLStat(const char * path, struct stat * buf);
int	SafeLink(const char * existingPath, const char * newPath);
int	SafeLockf(int fd, int func, long size);
int	SafeOpen(const char * path, int oflag, mode_t mode = 0644);
void    SafePathIsAccessible(DtMailEnv &error, const char * path);
ssize_t	SafeRead(int	fd, void * buf, size_t bytes);
int	SafeRename(const char * oldPath, const char * newPath);
int	SafeRemove(const char * path);
int	SafeStat(const char * path, struct stat * buf);
int	SafeTruncate(const char * path, off_t len);
int	SafeUTime(const char * path, utimbuf * ntime);
int	SafeUnlink(const char * path);
ssize_t	SafeWrite(int fd, const void * buf, size_t bytes);
unsigned long
	SafeWritev(int	fd, struct iovec *iov, int iovcnt);
ssize_t	SafeWriteStrip(int fd, const void * buf, size_t bytes);
int	SafeWaitpid(pid_t proc, int * status, int options);
int	SafeWait(int * status);

// The time routines deal with MT and reentrant issues in a platform
// independent way.
//
void	SafeCtime(const time_t *clock, char * buf, int buflen = 26);
void	SafeLocaltime(const time_t *clock, tm & result);
time_t	SafeMktime(tm * timeptr);
size_t	SafeStrftime(
		char * buf, size_t buf_size,
		const char * format, const tm * timeptr);

//
// Interface routines to AF_INET, SOCK_STREAM sockets with line-based
// buffering.
//
void	*SockOpen(char *host, int clientPort, char **errorstring);
char	*SockGets(char *buf, int len, void *sockfp);
int	SockPrintf(void *sockfp, char *format, ...);
int     SockRead(char *buf, int size, int nels, void *sockfp);
int	SockWrite(char *buf, int size, int nels, void *sockfp);
void	SockClose(void *sockfp);

//
// The group and password routines will retrieve the group and
// password names in an MT-Safe way.
//
int	FileSystemSpace(const char *file_path, size_t bytes, char **fsname);
void	GetGroupName(char * grp_name);
gid_t	GetIdForGroupName(char * grp_name);
void	GetPasswordEntry(passwd & result);
int	isSetMailGidNeeded(const char * mailboxPath);

#endif
