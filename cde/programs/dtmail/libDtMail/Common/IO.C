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
 *	$TOG: IO.C /main/33 1999/01/29 14:45:00 mgreess $
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

#define X_INCLUDE_TIME_H
#define X_INCLUDE_GRP_H
#define X_INCLUDE_PWD_H
#define XOS_USE_NO_LOCKING
#include <X11/Xos_r.h>

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
//
// START Order dependent for AIX
//
#include <sys/socket.h>
#if defined(_aix)
#include <sys/socketvar.h>
#if defined(BIG_ENDIAN)
#undef BIG_ENDIAN
#endif
#endif
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
//
// END Order dependent for AIX
//
#include <sys/stat.h>
#if !defined(linux)
#  include <sys/statvfs.h>
#endif
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/uio.h>
#include <unistd.h>

#ifdef _AIX
#include <sys/mntctl.h>
#include <sys/statfs.h>
#include <sys/vfs.h>
#define IOVMAX MSG_MAXIOVLEN
extern "C"
{
          fchmod(int, mode_t);
          statfs(const char *, struct statfs *);
          mntctl(int, int, char *);
  ssize_t writev(int, const struct iovec *, int);
}
#if (OSMAJORVERSION==4) && (OSMINORVERSION==2)
/* Temporary hack till the /usr/lpp/xlC/include/unistd.h file is fixed. */
extern "C" { int lockf(int, int, off_t); }
#endif
#endif /* _AIX */

#ifdef __osf__
extern "C" { int statvfs(const char *, struct statvfs *); }
#endif /* __osf__ */

#include <DtMail/DtMail.hh>
#include <DtMail/DtMailError.hh>
#include <DtMail/IO.hh>
#include <DtMail/Threads.hh>

#if !defined(IOV_MAX)
  #if !defined(linux)
    #include <sys/stream.h>
  #endif
  #if !defined(DEF_IOV_MAX)
    #define DEF_IOV_MAX 16
  #endif
  #define IOV_MAX DEF_IOV_MAX
#endif

#define IOVEC_IOVBASE_INCREMENTOR(iov, bytes) \
	((caddr_t)((size_t)iov->iov_base+bytes))

// The following I/O routines are wrappers for the normal routines,
// but they deal with EINTR, and partial read/write situations.
//
//
int
SafeOpen(const char * path, int oflag, mode_t mode)
{
    int status;
    
    do {
	status = open(path, oflag, mode);
    } while (status < 0 && errno == EINTR);

    return(status);
}

int
SafeClose(int fd)
{
    int status;
    do {
	status = close(fd);
    } while (status < 0 && errno == EINTR);

    return(status);
}

int
SafeLink(const char * existingPath, const char * newPath)
{
  int status;
  do {
    status = link(existingPath, newPath);
  } while (status < 0 && errno == EINTR);

  return(status);
}

int
SafeRename(const char * oldPath, const char * newPath)
{
  int status;
  do {
    status = rename(oldPath, newPath);
  } while (status < 0 && errno == EINTR);

  return(status);
}

int
SafeRemove(const char *path)
{
    int status;
    do {
      status = remove(path);
    } while (status < 0 && errno == EINTR);

    return(status);
}

int
SafeUnlink(const char *path)
{
    int status;
    do {
      status = unlink(path);
    } while (status < 0 && errno == EINTR);

    return(status);
}

ssize_t
SafeRead(int fd, void * buf, size_t bytes)
{
    ssize_t status = 0;
    do {
	status = read(fd, buf, bytes);
    } while(status < 0 && errno == EINTR);

    return(status);
}

ssize_t
SafeWrite(int fd, const void * buf, size_t bytes)
{
    ssize_t status = 0;
    do {
	status = write(fd, buf, bytes);
    } while(status < 0 && errno == EINTR);
    
    return(status);
}

// SafeWritev -- safe multiply vectored write
// Description:
//   SafeWritev provides an interrupt safe way to call the writev() system
//   call. In addition, it removes the IOV_MAX limitation on the size of the
//   iovec structure, and it will not return until either all bytes specified
//   by the iovec structure are written or writev() returns an error.
// Arguments:
//   int fd	-- file descriptor to write to
//   struct iovec *iov -- iovec structure describing writes to be done
//   int iovcnt	-- number of elements in passed in iov structure
// Outputs:
//   Will effectively destroy the contents of the passed in iovec structure.
//   The caller must deallocate the storage associated with the structure
//   upon regaining control.
// Returns:
//   == -1 : error returned from writev() - errno set to specific error number
//   != -1 : number of bytes actually written to the file

unsigned long
SafeWritev(int fd, struct iovec *iov, int iovcnt)
{
  ssize_t status;
  unsigned long bytesWritten = 0;

  // outer loop: starting with the first write vector, as long as there is
  // at least one vector left to feed writev(), do so
  //
  for(int curIov = 0; curIov < iovcnt; ) {

    // inner loop: feed writev() allowing for interrupted system call
    do {
      status = writev(
		fd,
		&iov[curIov],
		(iovcnt-curIov) > IOV_MAX ? IOV_MAX : (iovcnt-curIov));
    } while (status < 0 && errno == EINTR);

    if (status == (ssize_t)-1)		// true error from writev??
      return((unsigned long)-1);	// yes: bail at this point to caller
    bytesWritten += status;		// bump # bytes written count

    // must now "walk through" the io vector until we are up the to point
    // indicated by the number of bytes written by writev() - any leftover
    // in status indicates a partial write of a vector
    //
    while ((status > 0) && (curIov < iovcnt) && (iov[curIov].iov_len <= status)) {
      status -= iov[curIov++].iov_len;
    }

    // Check to see if we have reached the end of the io vector array; also
    // check to see if more bytes were written than called for; as crazy as
    // this sounds, in at least one instance when we finish spinning through
    // the io vectors we still had bytes left that had been written but not
    // accounted for in the io vectors (status > 0 && curIov >= iovcnt)
    //
    if (curIov >= iovcnt) {		// out of IO vectors?
      if (status > 0) {			// yes: all data accounted for?
	DtMailEnv error;		// NO:: log error condition
	error.logError(
		DTM_TRUE,
		"SafeWritev: writev(): too many bytes written (%d/%d)\n",
		status, bytesWritten);
      }
      continue;				// continue and let for loop exit
    }
    
    // Check for a partial write: if the current vector contains more data
    // than what has been written, writev() bailed in the middle of writing
    // a vector - adjust the vector and and feed it back to writev() again
    // OTHERWISE writev() ended with the current vector so move on to the next
    //
    if (iov[curIov].iov_len == status)	// full write of this vector?
      curIov++;				// yes: move on to the next vector
    else if (status != 0) {		// no: adjust this vector and retry
      iov[curIov].iov_len -= status;
      iov[curIov].iov_base = IOVEC_IOVBASE_INCREMENTOR((&iov[curIov]), status);
    }
  }
  return(bytesWritten);
}

#define SWS_BUFFERSIZE 1024
ssize_t
SafeWriteStrip(int fd, const void * buf, size_t bytes)
{
    ssize_t status = 0;
    int i, j;
    char *ptr = (char*)buf, *writebuf; 
    
    // bug 5856: dont write out control M 
    // make a finite size buffer for writing
    writebuf = (char*) malloc(bytes < SWS_BUFFERSIZE ? bytes : SWS_BUFFERSIZE); 

    for (i = 0, j = 0; i < bytes; i++, ptr++) {
	if (*ptr == '\r' && *(ptr+1) == '\n')
		continue;
	writebuf[j++] = *ptr;
	if (j == SWS_BUFFERSIZE || i == (bytes-1)) {
    		do {
			status = write(fd, writebuf, j);
    		} while(status < 0 && errno == EINTR);
		j = 0;
	}
    }
    free(writebuf);
    
    return(status);
}

int
SafeStat(const char * path, struct stat * buf)
{
    int status;
    do {
	status = stat(path, buf);
    } while (status < 0 && errno == EINTR);

    return(status);
}

int
SafeFchown(int fd, uid_t owner, gid_t group)
{
  int status;
  do {
    status = fchown(fd, owner, group);
  } while (status < 0 && errno == EINTR);

  return(status);
}

int
SafeLStat(const char * path, struct stat * buf)
{
    int status;
    do {
	status = lstat(path, buf);
    } while (status < 0 && errno == EINTR);

    return(status);
}

int
SafeFStat(int fd, struct stat * buf)
{
    int status;
    do {
	status = fstat(fd, buf);
    } while (status < 0 && errno == EINTR);

    return(status);
}

// SafeGuaranteedStat - return stat info for object given path name
// If NFS attribute caching is enabled (which is the default), a
// stat/fstat of a NFS file may not return the correct true size of the
// mailbox if it has been appended to since the last time it was read.
// To get around this problem, this function will perform a open(),
// read() of 1 byte, fstat(), close() which will force the attributes
// for the named file to be retrieved directly from the server.
// 
int
SafeGuaranteedStat(const char * path, struct stat * buf)
{
  int saveErrno;

#ifndef O_RSYNC
  int tempFd = SafeOpen(path, O_RDONLY|O_SYNC);
#else
  int tempFd = SafeOpen(path, O_RDONLY|O_RSYNC|O_SYNC);
#endif /* O_RSYNC */

  if (tempFd == -1) {
    return(-1);
  }

  char tempBuf;  
  if (SafeRead(tempFd, &tempBuf, 1) == -1) {
    saveErrno = errno;
    (void) SafeClose(tempFd);
    errno = saveErrno;
    return(-1);
  }

  if (SafeFStat(tempFd, buf) == -1) {
    saveErrno = errno;
    (void) SafeClose(tempFd);
    errno = saveErrno;
    return(-1);
  }

  (void) SafeClose(tempFd);

  return(0);
}

int
SafeTruncate(const char * path, off_t len)
{
    int status;
    do {
	status = truncate((char *)path, len);	// The cast is for AIX
    } while (status < 0 && errno == EINTR);

    return(status);
}

int
SafeFTruncate(int fd, off_t len)
{
    int status;
    do {
	status = ftruncate(fd, len);
    } while (status < 0 && errno == EINTR);

    return(status);
}

int
SafeAccess(const char * path, int amode)
{
    int status;
    do {
	status = access(path, amode);
    } while (status < 0 && errno == EINTR);

    return(status);
}

#if defined(sun)
#define LOCKF_SIZE_TYPE	long
#else
#define LOCKF_SIZE_TYPE	off_t
#endif

int
SafeLockf(int fd, int func, long size)
{
    int status;
    do {
	status = lockf(fd, func, (LOCKF_SIZE_TYPE) size);
    } while (status < 0 && errno == EINTR);

    return(status);
}

int
SafeFChmod(int fd, mode_t mode)
{
    int status;
    do {
	status = fchmod(fd, mode);
    } while (status < 0 && errno == EINTR);

    return(status);
}

int
SafeDup2(int fd1, int fd2)
{
    int status;
    do {
	status = dup2(fd1, fd2);
    } while (status < 0 && errno == EINTR);

    return(status);
}

int
SafeExecvp(const char * file, char *const *argv)
{
    int status;
    do {

	status = execvp(file, (char * const *)argv);

    } while (status < 0 && errno == EINTR);

    return(status);
}

int
SafeWaitpid(pid_t proc, int * p_stat, int options)
{
    pid_t status;
    do {
	status = waitpid(proc, p_stat, options);
    } while (status < 0 && errno == EINTR);

    return((int)status);
}

int
SafeWait(int * p_stat)
{
    pid_t status;
    do {
	status = wait(p_stat);
    } while (status < 0 && errno == EINTR);

    return((int)status);
}

int
SafeUTime(const char * path, utimbuf * ntime)
{
    int status;
    do {
	status = utime(path, ntime);
    } while (status < 0 && errno == EINTR);

    return(status);
}

void
SafePathIsAccessible(DtMailEnv &error, const char *path)
{
    int status;

    status = SafeAccess(path, F_OK);
    if (-1 == status)
    {
	if (EACCES == errno)
	  error.vSetError(DTME_PathElementPermissions, DTM_FALSE, NULL, path);
	else if (ENOTDIR == errno)
	  error.vSetError(DTME_PathElementNotDirectory, DTM_FALSE, NULL, path);
	else if (ENOENT == errno)
	{
	    char *s, *t;

	    t = strdup(path);
	    s = strrchr(t, '/');
	    if (s && s != t)
	    {
		*s = '\0';
		status = SafeAccess(t, F_OK);
		if (-1 == status)
		  error.vSetError(
				DTME_PathElementDoesNotExist,
				DTM_FALSE, NULL,
				path);
	    }
	}
    }
}

#if defined(POSIX_THREADS)
static void * time_mutex = NULL;
#endif

void
SafeCtime(const time_t *clock, char * buf, int buflen)
{
  _Xctimeparams ctime_buf;
  char *result;

  memset((void*) &ctime_buf, 0, sizeof(_Xctimeparams));
  result = _XCtime(clock, ctime_buf);
  if (result)
    strncpy(buf, result, buflen);
  else
    *buf = '\0';
}

void
SafeLocaltime(const time_t *clock, tm & result)
{
  struct tm	*time_ptr;
  _Xltimeparams	localtime_buf;

  memset((void*) &localtime_buf, 0, sizeof(_Xltimeparams));
  time_ptr = _XLocaltime(clock, localtime_buf);
  result = *time_ptr;
}

time_t
SafeMktime(tm * timeptr)
{
#if defined(POSIX_THREADS)

    if (!time_mutex) {
	time_mutex = MutexInit();
    }

    MutexLock lock_scope(time_mutex);
#endif

    return(mktime(timeptr));
}

size_t
SafeStrftime(char * buf, size_t buf_size,
	     const char * format, const tm * timeptr)
{
#if defined(POSIX_THREADS)

    if (!time_mutex) {
	time_mutex = MutexInit();
    }

    MutexLock lock_scope(time_mutex);
#endif

    return(strftime(buf, buf_size, format, timeptr));
}

#define  SockINTERNAL_BUFSIZE	2048

void *SockOpen(char *host, int clientPort, char **errorstring)
{
    int sockfd;
#if defined(__osf__)
    in_addr_t inaddr;
#else
    unsigned long inaddr;
#endif
    struct sockaddr_in ad;
    struct hostent *hp;
    DtMailEnv error;
    char *errorfmt = NULL;

    memset(&ad, 0, sizeof(ad));
    ad.sin_family = AF_INET;

    inaddr = inet_addr(host);
    if (inaddr != -1)
      memcpy(&ad.sin_addr, &inaddr, sizeof(inaddr));
    else
    {
        hp = gethostbyname(host);
        if (hp == NULL)
	{
	    if (NULL != errorstring)
	    {
	        errorfmt = DtMailEnv::getMessageText(
						SockErrorSet, 2,
						"Unknown host:  %s");
	        if (NULL == *errorstring) *errorstring = (char*) malloc(BUFSIZ);
	        sprintf(*errorstring, errorfmt, host);
	    }
            return (FILE*) NULL;
	}
        memcpy(&ad.sin_addr, hp->h_addr, hp->h_length);
    }
    ad.sin_port = htons(clientPort);
    
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
	if (NULL != errorstring)
	{
	    errorfmt = DtMailEnv::getMessageText(
						SockErrorSet, 3,
						"Error creating socket: %s");
	    if (NULL == *errorstring) *errorstring = (char*) malloc(BUFSIZ);
	    sprintf(*errorstring, errorfmt, error.errnoMessage());
	}
        return (FILE*) NULL;
    }
    if (connect(sockfd, (struct sockaddr *) &ad, sizeof(ad)) < 0)
    {
	if (NULL != errorstring)
	{
	    errorfmt = DtMailEnv::getMessageText(
					SockErrorSet, 3,
					"Error connecting to socket:  %s");
	    if (NULL == *errorstring) *errorstring = (char*) malloc(BUFSIZ);
	    sprintf(*errorstring, errorfmt, error.errnoMessage());
	}
	close(sockfd);
        return (FILE*) NULL;
    }

#if defined(USE_SOCKSTREAM)
    FILE *sockfp = fdopen(sockfd, "r+");
    setvbuf(sockfp, NULL, _IONBF, SockINTERNAL_BUFSIZE);
    return (void*) sockfp;
#else
    return (void*) sockfd;
#endif
}

int SockPrintf(void *sockfp, char* format, ...)
{
    va_list ap;
    char *buf = new char[8192];
    int i;

    va_start(ap, format);
    vsprintf(buf, format, ap);
    va_end(ap);
    i = SockWrite(buf, 1, strlen(buf), sockfp);
    delete [] buf;
    return i;
}

char *SockGets(char *buf, int len, void *sockfp)
{
#if defined(USE_SOCKSTREAM)
    char *in = fgets(buf, len, (FILE*) sockfp);
    fseek((FILE*) sockfp, 0L, SEEK_CUR);	/* required by POSIX */
    return in;
#else
    size_t n;
    char *bufp;
    
    n = 0;
    bufp = buf-1;
    do
    {
	bufp++;
        read((int) sockfp, (void*) bufp, 1);
	n++;
    } while (*bufp != '\n');
    *(bufp+1) = '\0';
    return buf;
#endif
}

int SockRead(char *buf, int size, int len, void *sockfp)
{
#if defined(USE_SOCKSTREAM)
    int n = fread(buf, size, len, (FILE*) sockfp);
    fseek((FILE*) sockfp, 0L, SEEK_CUR);	/* required by POSIX */
#else
    int n = (int) read((int) sockfp, (void*) buf, (size_t) size * len);
#endif
    return n;
}

int SockWrite(char *buf, int size, int len, void *sockfp)
{
    int n;

#if defined(USE_SOCKSTREAM)
    n = fwrite(buf, size, len, (FILE*) sockfp);
    fseek((FILE*) sockfp, 0L, SEEK_CUR);	/* required by POSIX */
#else
    n = write((int) sockfp, buf, size * len);
#endif

    return n;
}

void SockClose(void *sockfp)
{
#if defined(USE_SOCKSTREAM)
    fclose((FILE*) sockfp);
#else
    close((int) sockfp);
#endif
}

void
GetGroupName(char * grp_name)
{
    struct group *grp;
    _Xgetgrparams grp_buf;

    memset((void*) &grp_buf, 0, sizeof(_Xgetgrparams));
    grp = _XGetgrgid(getegid(), grp_buf);

    if (grp) {
	strcpy(grp_name, grp->gr_name);
    }
    else {
	strcpy(grp_name, "UNKNOWN_GROUP");
    }

}

gid_t
GetIdForGroupName(char * grp_name)
{
  assert(grp_name != NULL);
  struct group *grp;
  _Xgetgrparams grp_buf;
  
  memset((void*) &grp_buf, 0, sizeof(_Xgetgrparams));
  grp = _XGetgrnam(grp_name, grp_buf);
    
  return(grp ? grp->gr_gid : (gid_t)-1);
}

void
GetPasswordEntry(passwd & result)
{
  static struct passwd passwordEntry;
  static int oneTimeFlag = 0;

  if (!oneTimeFlag) {
    _Xgetpwparams pw_buf;
    struct passwd *tresult;

    memset((void*) &pw_buf, 0, sizeof(_Xgetpwparams));
#if defined(_AIX)
    _Xos_processLock;
    tresult = getpwuid(getuid());
    _Xos_processUnlock;
#else
    tresult = _XGetpwuid(getuid(), pw_buf);
#endif

    assert(tresult != NULL);
    memcpy(&passwordEntry, tresult, sizeof(struct passwd));
    passwordEntry.pw_name = strdup(passwordEntry.pw_name);
    passwordEntry.pw_passwd = strdup(passwordEntry.pw_passwd);
#if !defined(_AIX) && !defined(linux) && !defined(CSRG_BASED)
#ifndef __osf__
    passwordEntry.pw_age = strdup(passwordEntry.pw_age);
#endif
    passwordEntry.pw_comment = strdup(passwordEntry.pw_comment);
#endif
    passwordEntry.pw_gecos = strdup(passwordEntry.pw_gecos);
    passwordEntry.pw_dir = strdup(passwordEntry.pw_dir);
    passwordEntry.pw_shell = strdup(passwordEntry.pw_shell);
    oneTimeFlag++;
  }

  memcpy(&result, &passwordEntry, sizeof(struct passwd));
  return;
}
#if defined(MAILGROUP_REQUIRED)
int isSetMailGidNeeded(const char * mailboxPath)
{

  assert(mailboxPath);

    char inbox_path[1024];
    char mbox_path[1024];
    struct passwd *pw;
    struct stat buf;
    struct stat buf1;
    _Xgetpwparams pw_buf;

    memset((void*) &pw_buf, 0, sizeof(_Xgetpwparams));
    pw = _XGetpwuid(getuid(), pw_buf);

    // construct the lockfile name for the user.
    char *lock_file = (char *)calloc(1,strlen(pw->pw_name) + 6);
    strcpy(lock_file,pw->pw_name);
    strcat(lock_file,".lock");

    // parse the mailfolder name from the path
    char *p = strrchr(mailboxPath,'/');
    int len = (NULL!=0) ? strlen(mailboxPath)-strlen(p) : strlen(mailboxPath);
    char *str = (char *) calloc(1, len+1);
    strncpy(str, mailboxPath, len);
    str[len] = '\0';
    
    stat(str,&buf);

    // Default or system mailbox dir name.
    strcpy(inbox_path, "/var/spool/mail");
    stat(inbox_path,&buf1);
    free(str);

    if( ( (buf.st_dev == buf1.st_dev) && (buf.st_ino == buf1.st_ino) ) )
    {
        if( !strcmp((p+1),pw->pw_name) || !strcmp((p+1),lock_file) )
        {  
           if( access(mailboxPath,R_OK) == -1 )
           {
             free(lock_file);
             return (1);
           }
        }
        free(lock_file);
  	return ( 0 ); 
    }
    else
    {
        free(lock_file);
  	return ( 1 ); 
    }
}
#endif

#ifdef _AIX
/*
 * NAME: get_stat
 *
 * FUNCTION:
 *      gather mount status for all virtual mounts for this host.
 *
 * EXECUTION ENVIRONMENT: Part of user command.
 *
 * ON ENTRY:
 *      vmountpp        pointer to a buffer in which to put mount info
 *
 * NOTES:
 *      get_stat() was lifted and slightly modified from
 *      AIX Version 3 df.c.
 *
 * RETURNS:
 *      < 0     error in mntctl()
 *      > 0     for number of struct vmounts (mount points) in
 *              buffer which is pointed to by pointer left at
 *              *vmountpp.
 */
int get_stat(
struct vmount   **vmountpp)     /* place to tell where buffer is */
{
  size_t                size;
  register struct vmount        *vm;
  int                   nmounts;
  int                   count;

  size = BUFSIZ;                /* initial default size */
  count = 10;                   /* don't try forever    */

  while (count--) {             /* don't try it forever */
        if ((vm = (struct vmount*) malloc(size)) == NULL) {
                return(-1);
        }

/*
 * perform the QUERY mntctl - if it returns > 0, that is the
 * number of vmount structures in the buffer.  If it returns
 * -1, an error occured.  If it returned 0, then look in
 * first word of buffer for needed size.
 */
        if ((nmounts = mntctl(MCTL_QUERY, size, (caddr_t)vm)) > 0) {
                *vmountpp = vm;         /* OK, got it, now return */
                return(nmounts);
        } else {
                if (nmounts == 0) {
                        size = *(int *)vm; /* the buffer wasn't big enough */
                        free((void *)vm);   /* get required buffer size     */
                } else {
                        free((void *)vm);/* some other kind of error occurred*/
                        return(-1);
                }
        }
  }
  return(-1);
}

/*
 * NAME: get_vmount
 *
 * FUNCTION:
 *        Determines, via the filesystems vmount structures,
 *        the vmount id of the the filesystem id provided as
 *        an argument (enables ultimate access to the actual
 *        name of the filesystem).
 *
 * EXECUTION ENVIRONMENT: Part of user command.
 *
 * RETURNS:
 *      ptr to structure with vmount id of filesystem or NULL
 */
struct vmount *get_vmount(fsid_t *fsid)
{
    struct vmount *inu_vmount_p=NULL;
    int inu_vmount_num;
    register struct vmount *vm;
    int nmount;

    /* make sure we have all the virtual mount status of this host */
    if(inu_vmount_p == NULL)
        inu_vmount_num = get_stat(&inu_vmount_p);

    /* get the number of struct vmount in the vmount buffer */
    nmount = inu_vmount_num;

    /* go thru all the structures in the vmount buffer */
    for (vm = inu_vmount_p; nmount; nmount--,
            vm = (struct vmount *)((char *)vm + vm->vmt_length)) {
         if(( vm->vmt_fsid.fsid_dev == fsid->fsid_dev ) &&
            ( vm->vmt_fsid.fsid_type == fsid->fsid_type ))
             return(vm);
    }
    return((struct vmount *)NULL);
}
#endif /* _AIX */

int FileSystemSpace(const char *file_path, size_t bytes, char **fsname)
{
    int			fserror=FALSE;
    struct stat		stat_buf;
    size_t		req_space = 0;
#if !defined(linux)
    struct statvfs	statvfs_buf;
#endif

    if (stat(file_path,&stat_buf) < 0) return 0;
#if !defined(linux)
    if (statvfs(file_path,&statvfs_buf) < 0) return 0;
#endif

#ifdef _AIX
    struct statfs	statfs_buf;

    if (statfs(file_path,&statfs_buf) < 0) return 0;
    if (statfs_buf.f_vfstype == MNT_NFS)
    {
        struct vmount *vm;
        vm = get_vmount(&(statfs_buf.f_fsid));
        strcpy (statfs_buf.f_fname, vmt2dataptr (vm, VMT_STUB));
    }
#endif /* _AIX */

    if (bytes != 0)
    {

        // The following code became redundant as the writeMailBox method now
        // checks for No space from the SafeWritev call.
#if 0
        // CDExc20314
        // This check does not work (HP, IBM, Sun return -1; DEC returns 0)
        // Since the calling code has already created the file and is only
        // check that there is enough space, we don't need to check that
        // there are enough inodes.
        //
        // special case where the filesystem is out of inodes.
        if(statvfs_buf.f_ffree < 10) fserror = TRUE; 
#endif

        if (! fserror)
        {
            req_space = (size_t) ((bytes > stat_buf.st_size) ?
				  (bytes-stat_buf.st_size) :
				  0);
#if !defined(linux)
            if ( (statvfs_buf.f_bfree*statvfs_buf.f_bsize) >
	         (req_space + statvfs_buf.f_bsize) )
	      return 1;
            else
              fserror = TRUE;
#endif
        }
    }
    else fserror = TRUE;

    if (fserror)
    {
#ifdef _AIX
	*fsname = (char*) calloc(1, strlen(statfs_buf.f_fname)+1);
        strcpy (*fsname, statfs_buf.f_fname);
#else
	*fsname = (char*) calloc(1, strlen(file_path)+1);
        strcpy (*fsname, file_path);
#endif
        return 0;
    }
    return 1;
}
