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
/* $XConsortium: filebuf.C /main/9 1996/10/04 10:44:52 drk $ */

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

#if !defined(hpux) && !defined(__osf__) && !defined(USL) && !defined(linux) && !defined(CSRG_BASED)
#include <sysent.h>
#endif

#include <fcntl.h>
#include <string.h>

#include "utility/c_filebuf.h"

#include <sys/stat.h>

filebuf::~filebuf() 
{
   close();
   delete _name;
}

/////////////////////////////////////////////
// only current_pos is used.
/////////////////////////////////////////////

filebuf::filebuf(int __fd) :
   _fd(__fd), _prev_action(-1), _name(0),
   current_pos(0), new_pos(0), default_new_pos(0)
{
   _mode = 0;

   struct stat statbuf;
   if ( fstat(_fd, &statbuf ) == 0 ) {
     if ( BIT_TEST( statbuf.st_mode, S_IRUSR) )
        _mode |= ios::in;

     if ( BIT_TEST( statbuf.st_mode, S_IWUSR) )
        _mode |= ios::out;
   } else {
     _mode = 0;
   }
}

filebuf::filebuf(const char* name, int mode, int protect) :
   _fd(-1), _prev_action(-1), _name(strdup(name)),
   current_pos(0), new_pos(0), default_new_pos(0)
{
   open(name, mode, protect);
}

int filebuf::open(const char* name, int mode, int protect)
{
   if ( _fd != -1 && 
        _name && strcmp(name, _name) == 0 && 
        mode == _mode 
   )
      return 0;


   close();

   delete _name; _name = strdup(name);

   _mode = mode;

   int flag = 0;

   if ( BIT_TEST(mode, ios::app) || BIT_TEST(mode, ios::out) ) {
      if ( BIT_TEST(mode, ios::in) )
         flag |= O_RDWR;
      else
         flag |= O_WRONLY;
   } else {
      if ( BIT_TEST(mode, ios::in) )
         flag |= O_RDONLY;
   }

   if ( BIT_TEST(mode, ios::trunc) ||
        (
         BIT_TEST(mode, ios::trunc) && 
         !(BIT_TEST(mode, ios::in)||BIT_TEST(mode, ios::app))
        ) 
      ) 
      flag |= O_TRUNC;
  
//fprintf(stderr, "flag=%x\n", flag);

   _fd = ::open(name, flag);

   if ( _fd < 0 ) {
//fprintf(stderr, "use O_CREAT\n");
      flag |= O_CREAT;
      _fd = ::open(name, flag, protect);
   }

   if ( _fd >= 0 ) {
//fprintf(stderr, "filebuf::open OK, name = %s, fd = %d, this = %d\n", name, _fd, (void*)this);
      return 0;
   } else
      return EOF;
}

int filebuf::close()
{
   if ( _prev_action == streambuf::PUT ) 
      flush();
   
   ::close(_fd);
   _fd = -1;
   return 0;
}

int filebuf::is_open()
{
   return (_fd>=0) ? 1 : 0;
}

void filebuf::notify(int action_t) 
{
      if ( _prev_action == -1 ) {
         _prev_action = action_t;
      } else
        if ( _prev_action != action_t )
          _notify(action_t);
}

void filebuf::_notify(int action_t)
{
   switch ( action_t ) {
     case streambuf::GET:
//////////////////////////////////////////////////////////////
// previous action was PUT. Now calculate the number of chars
// that have been put and write them out. 
//
// Note: put_ptr always starts at base. And at this time, 
//       get_ptr == base.
//
//////////////////////////////////////////////////////////////
       overflow();
       break;

     case streambuf::PUT:
//////////////////////////////////////////////////////////////
// previous action was a GET. 
//
// Note: get_ptr always starts at base.
//
//////////////////////////////////////////////////////////////
       empty_buffer(); 
       break;

     default:
       return;
   }
   _prev_action = action_t;
}

int filebuf::_write(char* ptr, int size)
{
//fprintf(stderr, "_write() size = %d, fd = %d\n", size, _fd);

   int _written_size = ::write(_fd, ptr, size);
   if ( _written_size != size ) {
      //fprintf(stderr, "fwrite only writes %d bytes\n.", size - _written_size);
      return EOF;
   } 
   return _written_size;
}

int filebuf::overflow()
{
// write to the file if possible and clean the buffer

   if ( !BIT_TEST(_mode, ios::out) && !BIT_TEST(_mode, ios::app) )
      return EOF;

   if ( _size == 0 ) return 0;

   if ( BIT_TEST(_mode, ios::app) ) {
      if ( _seek(0L, SEEK_END) != 0 ) return EOF;
   } else
      if ( _seek() != 0 ) return EOF;

   int ok;

   if ( get_ptr + _size <= end ) {
      ok = _write(get_ptr, _size);
   } else {
      int l = end - get_ptr;

      ok = _write(get_ptr, l);

      if ( ok != EOF ) 
         ok = _write(base, _size - l);
   }

   if ( ok == EOF ) return EOF;

   current_pos += _size;

   empty_buffer(); 

   return ok;
}

int filebuf::underflow() 
{
// get more from the file if possible

   if ( !BIT_TEST(_mode, ios::in) )
      return EOF;

   if ( _seek() != 0 ) return EOF;

   _size = ::read(_fd, base, _capacity);

//fprintf(stderr, "read in _underflow() this = %d, _size = %d, current_pos = %d, fd = %d\n", (void*)this, _size, current_pos, _fd);

   if (_size <=0)
      return EOF;
   else {
      get_ptr = base;
      put_ptr = base + _size;
   }

   current_pos += _size;

   return 0;
}

int filebuf::_seek() 
{
//fprintf(stderr, "_seek() [1] current_pos= %d\n", current_pos);
   if ( ::lseek(_fd, current_pos, SEEK_SET) == -1 ) {
      fprintf(stderr, "lseek failed. current_pos= %ld\n", current_pos);
      return EOF;
   } else 
      return 0;
}

int filebuf::_seek(streampos pos, int whence) 
{
      
//fprintf(stderr, "_seek() [2]: pos= %d, whence=%d\n", pos, whence);
   current_pos = pos;
   if ( ::lseek(_fd, pos, whence) == -1 ) {
      fprintf(stderr, "lseek failed. pos= %ld, whence=%d\n", pos, whence);
      return EOF;
   } else 
      return 0;
}

int filebuf::seekg(long delta) 
{
   if ( _prev_action == streambuf::PUT )
      overflow();
   else
      empty_buffer(); 

   current_pos = delta;

   return 0;
}

int filebuf::flush() 
{
   if ( overflow() == EOF )
      return EOF;

   if ( ::fsync(_fd) != 0 )
      return EOF;
   else
      return 0;
}


