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
 * $TOG: funcs.C /main/16 1998/04/17 11:51:14 mgreess $
 *
 * Copyright (c) 1992 HaL Computer Systems, Inc.  All rights reserved.
 * UNPUBLISHED -- rights reserved under the Copyright Laws of the United
 * States.  Use of a copyright notice is precautionary only and does not
 * imply publication or disclosure.
 * 
 * This software contains confidential information and trade secrets of HaL
 * Computer Systems, Inc.  Use, disclosure, or reproduction is prohibited
 * without the prior express written permission of HaL Computer Systems, Inc.
 * 
 *                         RESTRICTED RIGHTS LEGEND
 * Use, duplication, or disclosure by the Government is subject to
 * restrictions as set forth in subparagraph (c)(l)(ii) of the Rights in
 * Technical Data and Computer Software clause at DFARS 252.227-7013.
 *                        HaL Computer Systems, Inc.
 *                  1315 Dell Avenue, Campbell, CA  95008
 * 
 */



#include "utility/funcs.h"
#include "unique_id.h"

#define X_INCLUDE_TIME_H
#define XOS_USE_XT_LOCKING
#include <X11/Xos_r.h>

/* Imake stuff defines SYSV; this code uses SVR4 ... here's the quick-fix */
#if defined(SYSV) && ! defined(SVR4)
#define SVR4
#endif

#ifdef SVR4
#include <sys/utsname.h>
#endif

#if defined(linux)
#include <sys/vfs.h>
#include <stdarg.h>
#elif defined(_AIX)
#include <sys/vfs.h>
#include <sys/statfs.h>
#include <sys/statvfs.h>
#else
#include <sys/statvfs.h>
#include <stdio.h>
#include <stdarg.h>
#endif
#ifdef __osf__
extern "C"
{
int statvfs(const char *, struct statvfs *);
int getdomainname(char *, int);
}
#endif /* __osf__ */
#include <sys/stat.h>

#define BUFLEN 512

#ifdef USL

int _DtMmdbStrcasecmp(register const char* s1, register const char* s2)
{
    register int c1, c2;

    while (*s1 && *s2) {
	c1 = isupper(*s1) ? tolower(*s1) : *s1;
	c2 = isupper(*s2) ? tolower(*s2) : *s2;
	if (c1 != c2)
	    return (c1 - c2);
	s1++;
	s2++;
    }
    return (int) (*s1 - *s2);
}

int _DtMmdbStrncasecmp(register const char* s1,
		       register const char* s2,
		       register int count)
{
    register int c1, c2;

    if (!count)
      return 0;

    while (*s1 && *s2) {
	c1 = isupper(*s1) ? tolower(*s1) : *s1;
	c2 = isupper(*s2) ? tolower(*s2) : *s2;
	if ((c1 != c2) || (! --count))
	    return (c1 - c2);
	s1++;
	s2++;
    }
    return (int) (*s1 - *s2);
}

#endif

#ifdef mips
#include <sys/utsname.h>

int gethostname(char* name, int namelen)
{
   struct utsname myuname;
   if ( uname(&myuname) != 0 ) {
      MESSAGE(cerr, "gethostname(): uname() failed");
      throw(Exception());
   }

   int l = strlen(myuname.nodename);
   if ( l >= namelen ) {
      cerr << "gethostname(): name array too short.\n";
      throw(Exception());
   } else
      memcpy(name, myuname.nodename, l);
   
   return 0;
}
#endif

int compare_stream(ostringstream& x, ostringstream& y)
{
   string xstr = x.str();
   string ystr = y.str();

   if ( xstr.size() != ystr.size() ) {
      cerr << xstr.size() << "---" << ystr.size() << endl;
//debug(cerr, xstr.c_str());
//debug(cerr, ystr.c_str());
      return 1;
   } else {

     char* u = (char *)xstr.c_str();
     char* v = (char *)ystr.c_str();

//debug(cerr, u);
//debug(cerr, v);
//fprintf(stderr, "u=%s, pcount() = %d\n", u, x.pcount());
//fprintf(stderr, "v=%s, pcount() = %d\n", v, y.pcount());

     if ( memcmp(u, v, xstr.size()) != 0 ) {
       STDERR_MESSAGE("two streams do not match.");
debug(cerr, u);
debug(cerr, v);
       return 1;
     } else {
       //STDERR_MESSAGE("two streams match.");
       return 0;
     }
   }
}

/*
float flog2(unsigned int x)
{
   return (float)(log((double)x) / log((double)2));
}

float flog2(const float x)
{
   return (float)log((double)x) / (float)log((float)2);
}

int pow2(const int x)
{
   return (int)pow((double)2, (double)x);
}

int pow2(const float x)
{
   return (int)pow((double)2, (double)x);
}

int bits(const int x)
{
   return (int)flog2((unsigned int)x);
}
*/

int pos_of_LSB(const unsigned int y)
{
   switch (y) {

   case 8192: return 13;
   case 1024: return 10;

   default:
    {
      unsigned int x = y;
   
   //debug(cerr, x);
   //debug(cerr, hex(x));
   
      unsigned int i;
      for ( i =0; i<sizeof(x); i++ ) {
         if ( ( 0x000000ff & x) == 0 ) 
            x >>= 8;
         else
            break;
      }
   
   //debug(cerr, i);
   
      int j;
      for ( j =1; j<=8; j++ )
         if ( (0x00000001 & x) == 0 ) 
            x >>= 1;
         else
            break;
   
   //debug(cerr, j);
   //debug(cerr, i*8+j);
   
      return i*8 + j;
     }
   }
   
}

/*
void char_swap(char& c1, char& c2)
{
   char tmp = c1;
   c1 = c2;
   c2 = tmp;
}

void short_swap(short& c1, short& c2)
{
   short tmp = c1;
   c1 = c2;
   c2 = tmp;
}

void int_swap(int& c1, int& c2)
{
   int tmp = c1;
   c1 = c2;
   c2 = tmp;
}
*/

int ceiling(const float x)
{
   if ( int(x) > x )
      return int(x)+1;
   else
      return int(x);
}

unsigned getbits(unsigned x, unsigned p, unsigned n)
{
   return((x>> (p-n)) & ~(~0 << n));
}

int del_file(const char* filename, const char* pathname)
{
   unsigned int len, slen;
   static char buf[BUFLEN];

   int ok;

   if ( pathname == 0 )
       ok = unlink(filename);
   else {

       if ( strlen(filename) + strlen(pathname) > (BUFLEN - 1) )
          throw(boundaryException(1, BUFLEN,
				  strlen(filename) + strlen(pathname)));

       buf[0] = 0;
       len = MIN(strlen(pathname), BUFLEN - 1);
       *((char *) memcpy(buf, pathname, len) + len) = '\0';
       slen = len;
       len = MIN(1, BUFLEN - 1 - slen);
       *((char *) memcpy(buf + slen, "/", len) + len) = '\0';
       slen += len;
       len = MIN(strlen(filename), BUFLEN - 1 - slen);
       *((char *) memcpy(buf + slen, filename, len) + len) = '\0';
       ok = unlink(buf);
   }
   
   if ( ok == -1 ) {
      debug(cerr, pathname);
      debug(cerr, filename);
      MESSAGE(cerr, form("unlink %s/%s failed", pathname, filename));
      throw(systemException(errno));
   }

   return 0;
}

Boolean copy_file(const char* source, const char* sink)
{
   fstream in(source, ios::in);
   fstream out(sink, ios::out);

   if ( !in || ! out )
      return false;

   int c;
   while ( (c=in.get()) != EOF ) {
     out.put((unsigned char)c);
   }

   in.close();

   if ( out.fail() ) 
      return false;
   else
      return true;
}

Boolean
copy_file(const char* path, const char* file,
          const char* source_ext, const char* target_ext)
{
   char source[PATHSIZ];
   char target[PATHSIZ];

   snprintf(source, sizeof(source), "%s/%s.%s", path, file, source_ext);
   snprintf(target, sizeof(target), "%s/%s.%s", path, file, target_ext);

   return copy_file(source, target) ;
}


Boolean exist_file(const char* filename, const char* pathname)
{
   int ok;

   struct stat stat_info;

    if ( pathname ) 
       ok = stat(form("%s/%s", pathname, filename), &stat_info);
    else
       ok = stat( filename, &stat_info );


    if ( ok == 0 )
       return S_ISREG(stat_info.st_mode) ? true : false ;

    switch (errno) {
       case ENOENT: 
          return false;
       default: 
          MESSAGE(cerr, "exist_file(): stat() failed. an exception");
          throw(systemException(errno));
    }

}

int check_file(istream& in, const char* msg)
{
   char c;
   in.get(c);
   in.putback(c);
   cerr << c << " " << (int)c << " <---" << msg << "\n";
   return 0;
}

Boolean
cat_file(const char* source1, const char* source2, const char* target)
{
/*
MESSAGE(cerr, "in cat_file");
debug(cerr, source1);
debug(cerr, source2);
debug(cerr, target);
*/

   fstream in1(source1, ios::in);
   fstream out(target, ios::out);

   if ( !in1 || ! out )
      return false;

   char buf[BUFSIZ];
   while ( in1.getline(buf, BUFSIZ) ) {
     out << buf;

     if ( in1.gcount() < BUFSIZ - 1 )
        out << '\n';

   }

   in1.close();

   fstream in2(source2, ios::in);

   if ( !in2 )
      return false;

   while ( in2.getline(buf, BUFSIZ) ) {
     out << buf;

     if ( in2.gcount() < BUFSIZ - 1 )
        out << '\n';

   }

   in2.close();
   out.close();

   return ( out.fail() ) ? false : true;
}


Boolean exist_dir(const char* pathname)
{
   struct stat stat_info;

   if ( stat( pathname, &stat_info ) == 0 ) 
      return S_ISDIR(stat_info.st_mode) ? true : false ;

   switch ( errno ) {
      case ENOENT: 
         return false;
      default:
         MESSAGE(cerr, "exist_dir() failed");
         debug(cerr, pathname);
         throw(systemException(errno));
   }
}

Boolean check_and_create_dir(const char* path)
{
   if ( exist_dir(path) == true )
      return true;

   const char* path_tail = path + 1; // skip the first '/'
   char* slash_ptr;

// create the subdirecties
   while ( path_tail[0] != 0 &&
           ( slash_ptr = (char *)strchr(path_tail, '/') ) ) {

       path_tail = slash_ptr + 1; // set for the next check
       slash_ptr[0] = 0;          // temp. set the slash to 0.

//debug(cerr, path);
       if ( exist_dir(path) == false ) {
          if ( mkdir(path, 0777) != 0 ) {
             debug(cerr, path);
             slash_ptr[0] = '/';         //reset to '/'
             perror(0);
             MESSAGE(cerr, form( "mkdir failed on path %s", path));
             throw(systemException(errno));
          }
       }
       slash_ptr[0] = '/';         //reset to '/'
   }

// create the full path
   if ( mkdir(path, 0777) != 0 ) {
      cerr << "mkdir failed on path " << path << "\n";
      throw(systemException(errno));
   } 

   return true;
}

static
int open_prot(int min, int def)
{
   int prot;

   umask(prot = umask(0));

   prot = min | (def & ~(prot & 0777));
   
   return prot;
}

int open_file_prot()
{
   return open_prot(0600,0666);
}

int open_dir_prot()
{
   return open_prot(0700,0777);
}

Boolean int_eq(void* x, void* y)
{
   if ( *(int*)x == *(int*)y )
       return true;
   else
       return false;
}

Boolean int_ls(void* x, void* y)
{
   if ( *(int*)x < *(int*)y )
       return true;
   else
       return false;
}

int ll4(int x)
{
   int u = sizeof(void*);
   int delta = x % u  ;
   return ( delta == 0 ) ? x : x + u - delta;
}

//Boolean fcntl_lock( int fd, lock_t lt )
//{
//   flock flock_record;
//
//   switch ( lt ) {
//     case SHARED:
//         flock_record.l_type = F_RDLCK;
//         break;
//     case EXCLUSIVE:
//         flock_record.l_type = F_WRLCK;
//         break;
//     default:
//         perror("fcntl_lock(): unknown lock type");
//         exit(-2);
//   }
//
///****************************/
//// the entire file is locked
///****************************/
//   flock_record.l_whence = SEEK_SET,
//   flock_record.l_start = 0;
//   flock_record.l_len  = 0;
//
//   if ( fcntl(fd, F_SETLKW, (int)&flock_record) != -1 ) {
//      return true;
//   } else {
//      return false;
//   }
//}
//      
//Boolean fcntl_unlock( int fd )
//{
//   flock flock_record;
//
///****************************/
//// the entire file is unlocked
///****************************/
//   flock_record.l_type = F_UNLCK;
//   flock_record.l_whence = SEEK_SET;
//   flock_record.l_start = 0;
//   flock_record.l_len  = 0;
//
//   if ( fcntl(fd, F_SETLKW, (int)&flock_record) != -1 ) {
//      return true;
//   } else
//      return false;
//}
//
//static Boolean time_out;
//
//Boolean timed_lock(int fd, lock_t lt, int seconds)
//{
//   signal(SIGALRM, (SIG_PF)onalarm);
//   alarm(seconds);
//   time_out = false;
//
//   while ( fcntl_lock(fd, lt) == false ) {
//      switch ( errno ) {
//        case EINTR:
//
//           if ( time_out == true ) {
//#ifdef DEBUG
//              MESSAGE(cerr, "time out after");
//              debug(cerr, seconds);
//#endif
//              return false;
//           }
//
//           break;
//
//        default:
//#ifdef DEBUG
//           MESSAGE(cerr, "error in fcntl_lock()");
//           perror(0);
//           debug(cerr, fd);
//#endif
//           return false;
//      }
//   }
//   signal(SIGALRM, SIG_IGN);
//   return true;
//}
//
//Boolean timed_unlock(int fd, int seconds)
//{
//   signal(SIGALRM, (SIG_PF)onalarm);
//   alarm(seconds);
//   time_out = false;
//
//   while ( fcntl_unlock(fd) == false ) {
//      switch ( errno ) {
//        case EINTR:
//
//           if ( time_out == true )
//              return false;
//
//           break;
//
//        default:
//           return false;
//      }
//   }
//   signal(SIGALRM, SIG_IGN);
//   return true;
//}
//
//void onalarm(int)
//{
//   time_out = true;
//}

static
char* time_stamp(_Xctimeparams *ctime_buf)
{
   time_t x;
   time(&x);
   return _XCtime(&x, *ctime_buf);
}

#ifdef C_API
int bytes(fstream& fs)
{
   struct stat file_info;

   if ( fstat( fs.rdbuf() -> fd(), &file_info) != 0 )
      return 0;
   else
      return int(file_info.st_size);
}

int bytes(int fd)
{
   struct stat file_info;

   if ( fstat( fd, &file_info) != 0 )
      return 0;
   else
      return int(file_info.st_size);
}
#else
int bytes(fstream* fs)
{
   streampos begin, current, end;
   int total_bytes;

   current = fs->tellg();
   fs->seekg(ios::beg);
   begin = fs->tellg();
   fs->seekg(ios::end);
   end = fs->tellg();
   fs->seekg(current);
   total_bytes = end - begin;
   return int(total_bytes);
}
#endif

int bytes(char * file_name)
{
   struct stat file_info;

   if ( stat( file_name, &file_info) != 0 )
      return 0;
   else
      return int(file_info.st_size);
}

char* form(const char* fmt, ...)
{
   static char formbuf[BUFSIZ];
   char tempbuf[BUFSIZ];
   va_list args;
   int len;

   va_start(args, fmt);

   len = MIN(strlen(formbuf), BUFSIZ - 1);
   *((char *) memcpy(tempbuf, formbuf, len) + len) = '\0';
   (void) vsnprintf(tempbuf, sizeof(tempbuf), fmt, args);

   va_end(args);

   len = MIN(strlen(tempbuf), BUFSIZ - 1);
   *((char *) memcpy(formbuf, tempbuf, len) + len) = '\0';
   return formbuf;
}

static char info_buf[BUFSIZ];

char* access_info( char* request )
{
#ifndef SVR4
   char dm_name[PATHSIZ];
   int dm_name_sz = PATHSIZ;

   if ( getdomainname(dm_name, dm_name_sz) == -1 ) {
      MESSAGE(cerr, "getdomainname() failed");
      throw(systemException(errno));
   }
#endif

#ifdef SVR4
   struct utsname name ;
   uname(&name);
#else
   char host_name[PATHSIZ];
   int host_name_sz = PATHSIZ;
   if ( gethostname(host_name, host_name_sz) == -1 ) {
      MESSAGE(cerr, "gethostname() failed");
      throw(systemException(errno));
   }
#endif

   _Xctimeparams ctime_buf;
   char* x = time_stamp(&ctime_buf);
   x[strlen(x)-1] = 0;

#ifndef SVR4
   snprintf(info_buf, sizeof(info_buf), "%s-%s-%ld-%s-%s",
           host_name, dm_name,
           /* getenv("USER"), */
           (long)getpid(), x, request
          );
#else
   char userid[L_cuserid];
   snprintf(info_buf, sizeof(info_buf), "%s-%s-%ld-%s-%s",
           name.nodename, 
           ( cuserid(userid)[0] == 0 ) ? "???" : userid,
           /* getenv("USER"), */
           (long)getpid(), x, request
          );
#endif

   return info_buf;
}

void lsb_putbits(unsigned& target, unsigned position_from_lsb, 
             unsigned bits, unsigned source)
{
   target |= ((source & ~( ~0 << bits )) << position_from_lsb) ;
}

unsigned lsb_getbits(unsigned source, unsigned position_from_lsb, unsigned bits)
{
   return ( ( source >> position_from_lsb ) & ~( ~0 << bits ) );
}

Boolean cc_is_digit(istream& in)
{
  int c = in.get();
  int ok = isdigit(c);
  in.putback(c);

  return ( ok ) ? true : false;
}

unsigned long disk_space(const char* path)
{
#if defined(__osf__) || defined (hpux) || defined (SVR4) || defined(CSRG_BASED)
   struct statvfs statfs_buf;
#else
   struct statfs statfs_buf;
#endif

   long free_bytes;

#if defined(__osf__) || defined (hpux) || defined (SVR4) || defined(CSRG_BASED)
   if ( statvfs(path, &statfs_buf) == 0 ) {
      free_bytes = statfs_buf.f_bavail * statfs_buf.f_frsize ;
#else
   if ( statfs(path, &statfs_buf) == 0 ) {
      free_bytes = statfs_buf.f_bavail * statfs_buf.f_bsize ;
#endif

   } else {
      throw(stringException(form("statfs failed on %s", path)));
   }

   return free_bytes;
}

Boolean writeToTmpFile(char* unique_nm, char* str, int size)
{
    Boolean ok = false;
    fstream *out = 0;
    char* tmp_dir_tbl[4];
    int len;
    tmp_dir_tbl[0] = getenv("TMPDIR");
    tmp_dir_tbl[1] = (char*)"/tmp";
    tmp_dir_tbl[2] = (char*)"/usr/tmp";
    tmp_dir_tbl[3] = getenv("HOME");

    int tmp_dir_tbl_size = 4;

    const char* uid = unique_id();

    for ( int i=0; i<tmp_dir_tbl_size; i++ ) {

       if ( tmp_dir_tbl[i] == 0 )
          continue;

       len = MIN(strlen(tmp_dir_tbl[i]) + strlen(uid) + 5, PATHSIZ - 1);
       *((char *) memcpy(unique_nm,
			 form("%s/tmp.%s", tmp_dir_tbl[i], uid),
			 len) + len) = '\0';

       mtry {
//debug(cerr, tmp_dir_tbl[i]);
//debug(cerr, disk_space(tmp_dir_tbl[i]));
          if ( disk_space(tmp_dir_tbl[i]) <= (unsigned long) size )
            continue;

          out = new fstream(unique_nm, ios::out);

          if ( !(*out) ) {
             delete out;
             continue;
          }
   
          if ( ! (out->write(str, size) ) ) {
             out -> close();
             delete out;
             del_file(unique_nm);
             continue;
          } else {
             ok = true;
             out -> close();
             delete out;
             break;
          }
   
       }
       mcatch_any()
       {
         continue;
       }
       end_try;

    }
   return ok;
}
