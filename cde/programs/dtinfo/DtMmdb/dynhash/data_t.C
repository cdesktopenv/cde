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
 * $XConsortium: data_t.cc /main/4 1996/07/18 14:31:45 drk $
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

   
#include "dynhash/data_t.h"

//memory_pool data_t::data_t_space_pool;

extern memory_pool g_memory_pool;

#ifdef C_API
atoi_larson* data_t::larson_convertor_ptr;
atoi_pearson* data_t::pearson_convertor_ptr;
#else
atoi_larson data_t::larson_convertor;
atoi_pearson data_t::pearson_convertor(SHRT_MAX, 256);
#endif

data_t::data_t(data_t& d)
{
   flag = d.flag;
   dt = d.dt;

   switch ( flag ) {
      case INT:
      {
        key.int_key = d.key.int_key ;
        flag = INT;
        break;
      }
      case STRING:
      {
        int sz = strlen(d.key.str_key);
        key.str_key = new char[sz+1];
        *((char *) memcpy(key.str_key, d.key.str_key, sz) + sz) = '\0';
        key.str_key[sz] = 0;

        flag = STRING;
        break;
      }
      case VOID:
        flag = VOID;
   }
}

   
/* // inline
data_t::data_t(int t, voidPtr d) : dt(d)
{ 
   flag = data_t::INT;
   key.int_key = t;
}
*/
   
data_t::data_t(const char* str, int sz, voidPtr d) : dt(d)
{ 
   flag = data_t::STRING;

   key.str_key = new char[sz+1];

   if ( sz > 0 ) {
      int len = MIN(strlen(str), (unsigned int) sz);
      *((char *) memcpy(key.str_key, str, len) + len) = '\0';
   }

   key.str_key[sz] = 0;
}

/*
int data_t::size()
{
   switch ( flag ) {
      case INT:
      case VOID:
        return 2 + strlen(form("%d", key.int_key)) + sizeof(dt);

      case STRING:
        return 2 + strlen(key.str_key) + sizeof(dt);
   }
}
*/

   
int data_t::operator==(data_t& d)
{
   if ( flag != d.flag ) {

      debug(cerr, flag);
      debug(cerr, d.flag);

      debug(cerr, *this);
      debug(cerr, d);

      throw(stringException("data_t type mismatches in operator==()"));
   }

   switch (flag) {
      case INT:
        return key.int_key == d.key.int_key ;
      case STRING:
        return strcmp(key.str_key, d.key.str_key) == 0 ;
      case VOID:
        throw(stringException("VOID type in operator==()"));
   }

   return 0;
}

data_t& data_t::operator =(data_t& d) 
{  
   flag = d.flag;
   dt = d.dt;

   switch ( flag ) {
      case INT:
        key.int_key = d.key.int_key ;
        flag = INT;
        break;

      case STRING:
        {
        unsigned int d_sz = strlen(d.key.str_key);
        if ( strlen(key.str_key) < d_sz ) {
           delete key.str_key;
           key.str_key = new char[d_sz+1];
        }
        *((char *) memcpy(key.str_key, d.key.str_key, d_sz) + d_sz) = '\0';
        flag = STRING;
        break;
        }
      case VOID:
        flag = VOID;
   }
   return *this;
}

data_t::~data_t() 
{
   switch ( flag ) {
      case INT:
        break;
      case STRING:
        delete key.str_key;
        break;
      case VOID:
        break;
   }
}

   
static char buf[BUFSIZ];

istream& operator >>(istream& i, data_t& d)
{
   if ( !i.getline(buf, BUFSIZ) || buf[0] == '\n' )
      return i;

   char* key_ptr = strchr(buf, '\t');
   char* voidPtr_ptr = strrchr(buf, '\t');

   *key_ptr = 0;
   *voidPtr_ptr = 0;

   key_ptr++;
   voidPtr_ptr++;

//////////////////////////
// delete the string
//////////////////////////
   if ( d.flag == data_t::STRING ) 
      delete d.key.str_key;

   d.flag = (data_t::flag_type)atoi(buf);
   d.dt = (voidPtr)(size_t)atoi(voidPtr_ptr);

   if ( d.flag == data_t::INT )
      d.key.int_key = atoi(key_ptr);
   else
   if ( d.flag == data_t::STRING ) {
      int sz = strlen(key_ptr);
      d.key.str_key = new char[sz+1];
      *((char *) memcpy(d.key.str_key, key_ptr, sz) + sz) = '\0';
   }

   return i;
}

ostream& operator <<(ostream& o, data_t& d) 
{
   if ( d.flag == data_t::VOID )
      return o;

   o << d.flag << "	";

   switch ( d.flag ) {
      case data_t::INT:
        o << d.key.int_key;
        break;
      case data_t::STRING:
        o << d.key.str_key;
        break;
      default:
        break;
   }

   o << "	" << (long)(d.dt);
   return o;
}

Boolean data_t::binaryIn(buffer& buf)
{
   char c; buf.get(c);
   flag = (data_t::flag_type)c;

   unsigned int sz;

   switch ( flag ) {
      case data_t::VOID:
        break;

      case data_t::INT:
        buf.get(*(unsigned int*)&key.int_key);
        break;

      case data_t::STRING:
        
        buf.get(sz);
        key.str_key = new char[sz+1];
        buf.get(key.str_key, sz);
        key.str_key[sz] = 0;

        break;
   }
   buf.get(*(long*)&dt);
   return true;
}

Boolean data_t::binaryOut(buffer& buf)
{
   buf.put((char)flag);
   unsigned int sz;

   switch ( flag ) {
      case data_t::VOID:
        break;

      case data_t::INT:
        buf.put((unsigned int)key.int_key);
        break;

      case data_t::STRING:
        sz = strlen(key.str_key);

        buf.put(sz);
        buf.put(key.str_key, sz);


        break;
   }
   buf.put((long)dt);
   return true;
}

ostream& data_t::asciiOut(ostream& o, print_func_ptr_t print_f)
{
   print_f(o, this);
   return o;
}

int data_t::bucket_num(int k, int p, int M)
{ 
   switch ( flag ) {
      case INT:
        return abs( key.int_key * k ) % p % M ;
      case STRING:
        //return abs( larson_convertor.atoi(key.str_key) * k ) % p % M ;
        //return abs( pearson_convertor.atoi(key.str_key, strlen(key.str_key), k
        return abs( pearson_convertor.atoi(key.str_key, strlen(key.str_key), k
, p) )  % M ;
      default:
        throw(stringException("VOID type in bucket_num()"));
   }
}

int data_t::slot_num(int k, int rotate, int prime, int M )
{ 
   int x;
   switch ( flag ) {
      case INT:
        x = ( abs( k * key.int_key ) % prime + rotate ) % M ;
        break;
      case STRING:
        x = ( abs( pearson_convertor.atoi(key.str_key, strlen(key.str_key), k, prime)) + rotate  ) % M ;
        break;
      default:
        throw(stringException("VOID type in slot_num()"));
   }
   return x;
}

void* data_t::operator new( size_t x )
{
   //return (void*)data_t_space_pool.alloc(x);
   return (void*)g_memory_pool.alloc(x);
}

void data_t::operator delete( void* ptr )
{
   //data_t_space_pool.free((char*)ptr);
   g_memory_pool.free((char*)ptr);
}

