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
 * $XConsortium: funcs.h /main/12 1996/09/13 20:48:55 cde-hal $
 *
 * Copyright (c) 1993 HAL Computer Systems International, Ltd.
 * All rights reserved.  Unpublished -- rights reserved under
 * the Copyright Laws of the United States.  USE OF A COPYRIGHT
 * NOTICE IS PRECAUTIONARY ONLY AND DOES NOT IMPLY PUBLICATION
 * OR DISCLOSURE.
 * 
 * THIS SOFTWARE CONTAINS CONFIDENTIAL INFORMATION AND TRADE
 * SECRETS OF HAL COMPUTER SYSTEMS INTERNATIONAL, LTD.  USE,
 * DISCLOSURE, OR REPRODUCTION IS PROHIBITED WITHOUT THE
 * PRIOR EXPRESS WRITTEN PERMISSION OF HAL COMPUTER SYSTEMS
 * INTERNATIONAL, LTD.
 * 
 *                         RESTRICTED RIGHTS LEGEND
 * Use, duplication, or disclosure by the Government is subject
 * to the restrictions as set forth in subparagraph (c)(l)(ii)
 * of the Rights in Technical Data and Computer Software clause
 * at DFARS 252.227-7013.
 *
 *          HAL COMPUTER SYSTEMS INTERNATIONAL, LTD.
 *                  1315 Dell Avenue
 *                  Campbell, CA  95008
 * 
 */



#ifndef _funcs_h
#define _funcs_h 1

#if !defined(USL) && !defined(__osf__) && !defined(linux) && \
    !defined(CSRG_BASED) && !defined(sun)
#include <libc.h>
#endif
#if defined(hpux) || defined(sgi) || defined(USL) ||defined(__osf__) || \
    defined(linux) || defined(CSRG_BASED) || defined(sun)
#include <unistd.h>
#else
#include <sysent.h>
#endif
#include <time.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#ifdef C_API
#include "utility/c_stream.h"
#include "utility/c_fstream.h"
#include "utility/c_stringstream.h"
#else
#include <assert.h>
#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;
#endif

#include <math.h>

#ifdef __CENTERLINE__
// centerline does not define these
#define S_ISDIR(m)      (((m)&S_IFMT) == S_IFDIR)
#define S_ISREG(m)      (((m)&S_IFMT) == S_IFREG)

#endif

#include "utility/debug.h"
#include "utility/types.h"
#include "utility/const.h"
#include "utility/macro.h"
#include "utility/mmdb_exception.h"

#ifdef USL
int _DtMmdbStrcasecmp(const char *s1, const char *s2);
int _DtMmdbStrncasecmp(const char *s1, const char *s2, int n);
#define strcasecmp(s1,s2) _DtMmdbStrcasecmp(s1,s2)
#define strncasecmp(s1,s2,n) _DtMmdbStrncasecmp(s1,s2,n)
#endif
#ifdef _IBMR2
/* no C++ version of <strings.h>, C version causes conflicts */
extern "C" {
extern int  strcasecmp(const char *, const char *);
extern int  strncasecmp(const char *, const char *, size_t);
}
#endif
#ifdef mips
int gethostname(char* name, int namelen);
#endif

int compare_stream(ostringstream& x, ostringstream& y);

char * cuserid(char *s);


inline float flog2(unsigned int x) {
   return (float)(log((double)x) / log((double)2));
#if defined(__osf__)
}     // return log_2(x) 
#else
};     // return log_2(x) 
#endif /* (__osf__) */

inline float flog2(const float x) {
   return (float)log((double)x) / (float)log((float)2);
#if defined(__osf__)
}
#else
};
#endif /* (__osf__) */

inline int pow2(const int x) {
   return (int)pow((double)2, (double)x);
#if defined(__osf__)
}     // x's power of 2
#else
};     // x's power of 2
#endif /* (__osf__) */

inline int pow2(const float x) {
   return (int)pow((double)2, (double)x);
#if defined(__osf__)
}    // x's power of 2
#else
};    // x's power of 2
#endif /* (__osf__) */

int pos_of_LSB(const unsigned int x); // position of the MSB

int ceiling(const float);    // ceiling of x
unsigned getbits(unsigned, unsigned, unsigned);

inline void char_swap(char& c1, char& c2) {
   char tmp = c1; c1 = c2; c2 = tmp;
#if defined(__osf__)
}  // switch two chars
#else
};  // switch two chars
#endif /* (__osf__) */

inline void short_swap(short& s1, short& s2) {
   short tmp = s1; s1 = s2; s2 = tmp;
#if defined(__osf__)
}// switch two shorts
#else
};// switch two shorts
#endif /* (__osf__) */

inline void int_swap(int& i1, int& i2) {
   int tmp = i1; i1 = i2; i2 = tmp;
#if defined(__osf__)
}     // switch two ints
#else
};     // switch two ints
#endif /* (__osf__) */

// file functions

int del_file(const char* file_nm, const char* path_nm = 0);

Boolean copy_file(const char* source, const char* sink);
Boolean copy_file(const char* path, const char* filenm,
                  const char* source_ext, const char* target_ext);

Boolean exist_file(const char* name, const char* path = 0);
Boolean cat_file(const char* source1, const char* source2, 
                    const char* target);
int check_file(istream&, const char* msg = "");

Boolean exist_dir(const char* path);
Boolean check_and_create_dir(const char* path);

int open_file_prot();
int open_dir_prot();


Boolean cc_is_digit(istream&); // "cc" stands for current char

unsigned long disk_space(const char* path);
char* access_info( char* request );

Boolean int_eq(void*, void*);
Boolean int_ls(void*, void*);

// return an lease largest int of x, the returned value is 
// also a multiple of sizeof(void*)
int ll4(int x);

/*
enum lock_t { SHARED, EXCLUSIVE };

Boolean fcntl_lock( int fd, lock_t lt );
Boolean fcntl_unlock( int fd );

Boolean timed_lock( int fd, lock_t lt, int seconds = 5);
Boolean timed_unlock( int fd, int seconds = 5);

void onalarm(int);
*/

#ifdef C_API
int bytes(int fd);
int bytes(fstream&);
#else
int bytes(fstream*);
#endif
int bytes(char* file_name);

char*  form(const char* ...);

// lsb is considered as the 0th bit
void lsb_putbits(unsigned& target, unsigned position_from_lsb, 
             unsigned bits, unsigned source);
unsigned lsb_getbits(unsigned source, unsigned position_from_lsb, unsigned bits);

Boolean writeToTmpFile(char* unique_nm, char* str, int size);

#endif

