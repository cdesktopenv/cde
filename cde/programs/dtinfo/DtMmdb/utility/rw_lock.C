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
 * $XConsortium: rw_lock.cc /main/3 1996/06/11 17:38:50 cde-hal $
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


#include "atomic_lock.h"
#include "rw_lock.h"

Boolean read_lock(char* lock_file_path,
                  char* writing_lock_file_path, 
                  char* ai_path, 
                  char* reader_info, int& offset,
                  char*& ai_info
                 )
{
   atomic_lock l(lock_file_path);

   if ( l.lock() == false ) {
      MESSAGE(cerr, "read_lock(): can't do atomic locking");
      return false;
   }

   Boolean ok;

   if ( false == exist_file( writing_lock_file_path ) ) {

      fstream x(ai_path, ios::app);
      if ( !x ) {
         MESSAGE(cerr, "read_lock(): can't open lock file");
         throw(streamException(x.rdstate()));
      }

#ifdef C_API
      offset = bytes(x.rdbuf() -> fd());
#else
      offset = bytes(ai_info);
#endif
      x << "A-" << reader_info << "\n";

      x.close();
      ok = true;

   } else {

      fstream x(ai_path, ios::in);
      if ( !x ) {
         MESSAGE(cerr, "read_lock(): can't open lock file");
         throw(streamException(x.rdstate()));
      }

#ifdef C_API
      int sz = bytes(x.rdbuf() -> fd());
#else
      int sz = bytes(ai_info);
#endif

      ai_info = new char[sz+1];
      ai_info[0] = 0;

      x.getline(ai_info, sz);

      x.close();

      ok = false;
   }

   if ( l.unlock() == false ) {
     MESSAGE(cerr, "read_lock(): can't do atomic unlocking");
     return false;
   }

   return ok;
}

Boolean read_unlock(char* lock_file_path, char* ai_path, int offset)
{
   atomic_lock l(lock_file_path);

   if ( l.lock() == false ) {
      MESSAGE(cerr, "read_lock(): can't do atomic locking");
      return false;
   }

   Boolean ok ;

   fstream x(ai_path, ios::in|ios::out);
   if ( !x ) {
      MESSAGE(cerr, "read_unlock(): can't open lock file");
      throw(streamException(x.rdstate()));
   }

   x.seekg( offset, ios::beg );
   x.put('I');

///////////////////////////////////////////////
// truncate the info_file if no active readers
// and the file size is over 1k
///////////////////////////////////////////////

#ifdef C_API
   if ( bytes(x.rdbuf() -> fd()) > 1024 ) {
#else
   if ( bytes(ai_path) > 1024 ) {
#endif

      ok = false;
      char buf[BUFSIZ];
/////////////////////////////////////////
// scan the info file for active readers
/////////////////////////////////////////
      while ( x.getline(buf, BUFSIZ) ) {
         if ( buf[0] == 'A' ) {
            ok = true;
            break;
         }
      }

      if ( ok == false )
         if ( truncate(ai_path, 0) != 0 ) {
            MESSAGE(cerr, "read_unlock(): can't truncate");
            throw(systemException(errno));
         }
   }

   x.close();

   if ( l.unlock() == false ) {
     MESSAGE(cerr, "read_lock(): can't do atomic locking");
     return false;
   }

   return true;
}


Boolean write_lock(char* lock_file_path, 
                   char* writing_lock_path, 
                   char* ai_path, char* writer_info,
                   char*& ai_info
                  )
{
   unsigned int len, slen;
   atomic_lock l(lock_file_path);

   if ( l.lock() == false ) {
      MESSAGE(cerr, "write_lock(): can't do atomic locking");
      return false;
   }

   Boolean ok = true;

   fstream x(ai_path, ios::in|ios::out); 

   if (!x) {
      MESSAGE(cerr, "write_lock(): can't open info file");
      throw(streamException(x.rdstate()));
   }

   char buf[BUFSIZ];

#ifdef C_API
   int sz = bytes(x.rdbuf() -> fd());
#else
   int sz = bytes(ai_path);
#endif
   ai_info = new char[sz+1]; 
   ai_info[0] = 0;
   
/////////////////////////////////////////
// scan the info file for active readers
/////////////////////////////////////////
   while ( x.getline(buf, BUFSIZ) ) {
      if ( buf[0] == 'A' ) {
         ok = false;

         slen = strlen(ai_info);
         len = MIN(strlen(buf+1), BUFSIZ - 1 - slen);
         *((char *) memcpy(ai_info + slen, buf+1, len) + len) = '\0';

         slen = strlen(ai_info);
         len = MIN(1, BUFSIZ - 1 - slen);
         *((char *) memcpy(ai_info + slen, "\n", len) + len) = '\0';
      }
   }
   
   x.close();

   if ( exist_file( writing_lock_path ) == false ) {

      if ( ok == true ) {
   
         delete ai_info;
/////////////////////////////////////////
// create the access info file
/////////////////////////////////////////
         if(truncate(ai_path, 0) != 0 ) {
            throw(systemException(errno));
         }
         fstream x(ai_path, ios::out);
         x << "A-" << writer_info << "\n";
   
/////////////////////////////////////////
// create the writing lock file
/////////////////////////////////////////
         if ( creat(writing_lock_path, 0755) == -1 )
            ok = false;
     }

   } else 
      ok = false;

   if ( l.unlock() == false ) {
      MESSAGE(cerr, "write_lock(): can't do atomic unlocking");
      return false;
   }

   return ok;
}

Boolean write_unlock(char* lock_file_path, char* writing_lock_path, 
                     char* ai_path 
                    )
{
   atomic_lock l(lock_file_path);

   if ( l.lock() == false ) {
      MESSAGE(cerr, "write_unlock(): can't do atomic locking");
      return false;
   }

   Boolean ok; 

   if ( del_file(writing_lock_path) == 0 &&
        del_file(ai_path) == 0 
      ) 
      ok = true; 
   else
      ok = false; 

   if ( l.unlock() == false ) {
      MESSAGE(cerr, "write_unlock(): can't do atomic unlocking");
      return false;
   }

   return ok;
}
