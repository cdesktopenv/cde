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
 * $XConsortium: abs_storage.cc /main/6 1996/07/18 14:52:12 drk $
 *
 * Copyright (c) 1992 HAL Computer Systems International, Ltd.
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


#include "storage/abs_storage.h"

abs_storage::abs_storage( char* file_path, char* file_name,
                          c_code_t c_id, rep_policy* p ) : 
root(c_id), index_num(-1), policy(p), v_swap_order(false)
{
   int len = MIN(strlen(file_path), PATHSIZ - 1);
   *((char *) memcpy(path, file_path, len) + len) = '\0';
   len = MIN(strlen(file_name), PATHSIZ - 1);
   *((char *) memcpy(name, file_name, len) + len) = '\0';
}

abs_storage::~abs_storage()
{
}

int abs_storage::byte_order()
{
   unsigned int x;
   unsigned char *p;

   x = 0x01020304;
   p = (unsigned char *)&x;

   switch (*p) {
    case 1:
       return (mmdb_big_endian);
    case 4:
       return (mmdb_little_endian);
    default:
       throw(stringException("neither BIG_ENDIAN or LITTLE_ENDIAN machine"));
   }
}


