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
 * $XConsortium: mmdb_btree.cc /main/3 1996/06/11 17:14:32 cde-hal $
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


#include "btree/mmdb_btree.h"


btree::btree(const char* store_name)
{
// let the package figure out all these parameters
   btree_info.flags = 0;
   btree_info.cachesize = 0;
   btree_info.maxkeypage = 0;
   btree_info.minkeypage = 0;
   btree_info.psize = 0;
   btree_info.compare = NULL;
   btree_info.prefix = NULL;
   btree_info.lorder = 0;


   int mode = O_CREAT|O_RDWR;

   //btree_DB = dbopen(store_name, mode, 0640, DB_BTREE, &btree_info);
   btree_DB = dbopen(store_name, mode, 0640, DB_BTREE, NULL);

   if ( btree_DB == 0 )
      throw(stringException("btree dbopen failed"));
}

btree::~btree()
{
   if ( btree_DB->sync(btree_DB, 0) == RET_ERROR )
      throw(stringException("btree sync failed"));

   if ( btree_DB->close(btree_DB) == RET_ERROR )
      throw(stringException("btree close failed"));
}

void btree::clean()
{
   throw(stringException("void btree::clean(): not implemented yet"));
}

void btree::data_t_2_DBT(data_t& w)
{
   switch (w.flag) {
     case data_t::INT:
      key_DBT.data = &w.key.int_key;
      key_DBT.size = sizeof(w.key.int_key);
      break;

     case data_t::STRING:
      key_DBT.data = w.key.str_key;
      key_DBT.size = strlen(w.key.str_key);
      break;

     case data_t::VOID:
        throw(stringException("btree data_t_2_DBT: unknow key type"));
   }
}

Boolean btree::insert(data_t& w)
{
   data_t_2_DBT(w);

   DBT data_DBT;
   data_DBT.data = &w.dt;
   data_DBT.size = sizeof(w.dt);

   //int status = btree_DB->put(btree_DB, &key_DBT, &data_DBT, R_NOOVERWRITE);
   int status = btree_DB->put(btree_DB, &key_DBT, &data_DBT, 0);

   switch (status) {
     case RET_ERROR:
        throw(stringException("btree put failed"));
	break;

     case RET_SPECIAL:
        throw(stringException("btree put: dup key"));
	break;

     case RET_SUCCESS:
        return true;
   }

   return false;
}

Boolean btree::remove(data_t& w)
{
   data_t_2_DBT(w);

   int status = btree_DB->del(btree_DB, &key_DBT, 0);

   switch (status) {
     case RET_ERROR:
        throw(stringException("btree delete failed"));
	break;

     case RET_SPECIAL:
     case RET_SUCCESS:
        return true;
   }

   return false;
}

Boolean btree::member(data_t& w) 
{
   data_t_2_DBT(w);
   DBT data_DBT;

   int status = btree_DB->get(btree_DB, &key_DBT, &data_DBT, 0);

   switch (status) {
     case RET_ERROR:
        throw(stringException("btree get failed"));
	break;

     case RET_SPECIAL:
        return false;

     case RET_SUCCESS:
        if ( data_DBT.size != sizeof(w.dt) )
           throw(stringException("btree get: tree corrupted"));

        memcpy((char*)&w.dt, data_DBT.data, data_DBT.size);
        return true;
   }

   return false;
}

ostream& btree::asciiOut(ostream& out)
{
   return out;
}

istream& btree::asciiIn(istream& in)
{
   return in;
}

