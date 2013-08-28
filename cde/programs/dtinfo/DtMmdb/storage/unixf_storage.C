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
 * $TOG: unixf_storage.C /main/8 1998/04/17 11:50:39 mgreess $
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


#include "storage/unixf_storage.h"

#ifdef _IBMR2 /* connolly 2/21/95 from the AIX fsync() manpage */
extern "C" int fsync(int fd);
#endif

/***********************************************************/
// Constructor 
/***********************************************************/
unixf_storage::
unixf_storage( char* file_path, char* file_name, 
               rep_policy* rep_p, int m
             ) :
abs_storage( file_path, file_name, UNIX_STORAGE_CODE, rep_p ), 
fstream(), mode(m),
total_bytes(-1), v_file_exist(exist_file(file_name, file_path))
{
}

/***********************************************************/
// Destructor 
/***********************************************************/
unixf_storage::~unixf_storage()
{
/*
MESSAGE(cerr, "~unixf storeage ()");
   debug(cerr, my_path());
   debug(cerr, my_name());
*/
   if ( policy )
      policy -> remove(*this);

#ifdef REPORT_IO_COUNT
#endif
}

void unixf_storage::remove()
{
#ifdef C_API
   int fd = rdbuf() -> fd();
   fsync(fd);
   ::close(fd);
#else
   rdbuf() -> close();
#endif
   del_file(my_name(), my_path());
/*
   int md = mode;
   _open(ios::trunc);
   mode = md;
   _open(mode);
*/
}

/***********************************************************/
// Open the physical file associated with this store
/***********************************************************/
int unixf_storage::_open(int new_mode)
{
   char *fmt = NULL;

   if ( ! ( *this ) ) {
#ifdef DEBUG
      fprintf(stderr, "fstream is in bad status @ %s:%d\n",
						__FILE__, __LINE__);
#endif
      throw(streamException(fstream::rdstate()));
   }

   rep_cell* replaced = 0;

   if ( policy != 0 ) {

      policy -> promote(*this, replaced);

      if ( replaced ) {
         unixf_storage* us = ((unixf_storage*)replaced);
         us -> fstream::close(); 
         if ( us -> fstream::fail() ) {
#ifdef DEBUG
	    fprintf(stderr, "Can't close db file %s/%s @ %s:%d\n",
			us->my_path(), us->my_name(), __FILE__, __LINE__);
#endif
            throw(streamException(us -> fstream::rdstate()));
         }
      }
   }

   if ( v_file_exist == false ) {
      SET_BIT(new_mode, ios::out | ios::trunc);
      v_file_exist = true;
   }

//MESSAGE(cerr, "_open");
//debug(cerr, total_bytes);

   if ( !BIT_TEST(mode, new_mode) ) {

      if ( fstream::rdbuf() -> is_open() ) {
         fstream::close();

         if ( fstream::fail() ) {
#ifdef DEBUG
	    fprintf(stderr, "Can't close db file %s/%s @ %s:%d\n",
					path, name, __FILE__, __LINE__);
#endif
            throw(streamException(fstream::rdstate()));
         }
      }
     
      SET_BIT(mode, new_mode);

      fmt = ::form("%s/%s", path, name);
#ifdef C_API
      fstream::open((const char *) fmt, mode);
#else
      fstream::open((const char *) fmt, (ios_base::openmode)mode);
#endif
//    fstream::open((const char *) fmt, mode, open_file_prot());

   } else {

      if ( ! fstream::rdbuf() -> is_open() )  {
        fmt = ::form("%s/%s", path, name);
#ifdef C_API
        fstream::open((const char *) fmt, mode);
#else
        fstream::open((const char *) fmt, (ios_base::openmode)mode);
#endif
//      fstream::open((const char *) fmt, mode, open_file_prot());
      }

   }

   if ( (!(*this)) || !fstream::rdbuf() -> is_open() ) {
      MESSAGE(cerr, "_open failed");
      debug(cerr, new_mode);
      debug(cerr, mode);
      debug(cerr, my_path());
      debug(cerr, my_name());
#ifdef DEBUG
      fprintf(stderr, "Can't close db file %s/%s @ %s:%d\n",
					path, name, __FILE__, __LINE__);
#endif
      throw(streamException(fstream::rdstate()));
   }

   return 0;
}

/***********************************************************/
// Read a string from the store. Use internal buffer.
/***********************************************************/
int 
unixf_storage::readString(mmdb_pos_t loc, char* base, int len, int str_off)
{
   _open(ios::in);
     
   int offset = int(loc) + str_off;

   if ( seekg( offset, ios::beg ) == 0 ) {
      MESSAGE(cerr, "seekg failed");
      throw(streamException(fstream::rdstate()));
   }

   if ( read( base, len ) == 0 || len != fstream::gcount() ) {
      MESSAGE(cerr, "read() failed");
      throw(streamException(fstream::rdstate()));
   }

   return 0;
}

/***********************************************************/
// Write a string to the store. Use external buffer.
/***********************************************************/
int unixf_storage::updateString(mmdb_pos_t loc, const char* base, int len, int string_ofst, Boolean flush_opt)
{
/*
debug(cerr, len);
debug(cerr, loc);
debug(cerr, int(base));
debug(cerr, string_ofst);
debug(cerr, int(flush_opt));
*/

/*
fprintf(stderr, "updateString():");
fprintf(stderr, "len=%d, ", len);
fprintf(stderr, "loc=%d, ", loc);
fprintf(stderr, "string_ofst=%d, ", string_ofst);
fprintf(stderr, "flush option=%d\n", flush_opt);
*/

   _open(ios::out);

#ifdef C_API
   if ( ! seekg(loc+string_ofst, ios::beg) ) {
      MESSAGE(cerr, form("seek() failed on %s", my_name()));
      throw(streamException(fstream::rdstate()));
   }
#else
   if ( ! seekp(loc+string_ofst, ios::beg) ) {
      MESSAGE(cerr, form("seek() failed on %s", my_name()));
      throw(streamException(fstream::rdstate()));
   }
#endif

   if ( ! write( base, len ) ) {
#ifdef DEBUG
      fprintf(stderr, "write() failed on %s @ %s:%d\n",
				my_name(), __FILE__, __LINE__);
#endif
      throw(streamException(fstream::rdstate()));
   }

   if ( flush_opt == true ) 
      flush();

#ifdef DEBUG
   fprintf(stderr, "%d bytes have been written at offset %ld in %s/%s @ %s:%d\n", len, loc+string_ofst, path, name, __FILE__, __LINE__);
#ifndef C_API
   {
     char fname[64];
     snprintf(fname, sizeof(fname), "%s.%ld-%d", name, loc+string_ofst, len);
     ofstream output(fname);
     output.write(base, len);      
     output.flush();
   }
#endif
#endif

   if ( !(*this) ) {
#ifdef DEBUG
      fprintf(stderr, "write() failed on %s @ %s:%d\n",
				my_name(), __FILE__, __LINE__);
#endif
      throw(streamException(fstream::rdstate()));
   }

   total_bytes = MAX(total_bytes, int(loc)+len);

   return 0;
}

/***********************************************************/
// insert a string.
/***********************************************************/
int unixf_storage::appendString(mmdb_pos_t, const char* base, int len, Boolean flush_opt)
{
   mmdb_pos_t loc = bytes();
   updateString(loc, base, len, 0, flush_opt);
   return 0;
}

/***********************************************************/
// Return the number of bytes in the store.
/***********************************************************/
int unixf_storage::bytes() 
{
   if ( total_bytes == -1 ) {

#ifdef C_API
      _open(ios::in);

      if ( !good() ) 
         clear();

      total_bytes = ::bytes(rdbuf() -> fd());
#else
      char* info_lib_file = form("%s/%s", path, name);

      total_bytes = ::bytes(info_lib_file);
#endif
   }

   return total_bytes;
}
   
Boolean unixf_storage::io_mode(int test_mode)
{
   Boolean opened = false;

   if ( fstream::rdbuf() -> is_open() ) {
      opened = true ;
      fstream::close();
   }

#ifdef C_API
   fstream::open(name, test_mode);
#else
   fstream::open(name, (ios_base::openmode)test_mode);
#endif
// fstream::open(name, test_mode, open_file_prot());

   if ( ! fstream::rdbuf() -> is_open() )
      return false;
   else {
      fstream::close();
    
      if ( opened == true )
#ifdef C_API
         fstream::open(name, mode, open_file_prot());
#else
         fstream::open(name, (ios_base::openmode)mode);
#endif
//       fstream::open(name, mode, open_file_prot());

      return true;
   }
}

int unixf_storage::truncate(int target_length_in_bytes)
{
   char *fmt = NULL;

   _open(ios::out);

   if ( total_bytes > target_length_in_bytes ) {
      fmt = ::form("%s/%s", path, name);
      if ( ::truncate((const char *) fmt, target_length_in_bytes) != 0 ) {
         throw(systemException(errno));
      }

      total_bytes = target_length_in_bytes;
   }


   return 0;
}
