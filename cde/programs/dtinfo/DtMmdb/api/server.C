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
 * $XConsortium: server.cc /main/3 1996/06/11 17:11:23 cde-hal $
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


#include "api/server.h"
#include "utility/db_version.h"


server::server(char* x_infolib_path) : 
   infolibptr(0), info_base_set_names(0), info_base_list_names(0)
{
   if ( x_infolib_path == 0 ) {
      throw(stringException("NULL info lib path"));
   }

   if ( check_and_create_dir(x_infolib_path) == false )
      throw(stringException(
        form("server: infolib %s does not exist or can't be created", x_infolib_path)
                           )
           );

   int len = MIN(strlen(x_infolib_path), PATHSIZ - 1);
   *((char *) memcpy(info_lib_dir, x_infolib_path, len) + len) = '\0';

}

server::~server()
{
   delete infolibptr;

   delete info_base_set_names;
   delete info_base_list_names;
}


int server::major_code_version() 
{ 
   return MAJOR; 
}

int server::minor_code_version() 
{ 
   return MINOR; 
}

mm_version server::code_version() 
{ 
   return mm_version(MAJOR, MINOR); 
}

