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
 * $XConsortium: asciiIn_filters.cc /main/4 1996/06/11 17:26:22 cde-hal $
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



#include "oliasdb/asciiIn_filters.h"
#include "oliasdb/node_hd.h"


static char oid_in_string_buf[BUFSIZ];

char* get_oid(info_lib* infolib_ptr, char* base_str, char* locator_line)
{
    char* locator = strchr(locator_line, '\t') + 1;

    if ( strcmp(locator, "	0000000000000000000000") == 0 ||
         strcmp(locator, "0") == 0 ) 
    {
        return (char*)"0.0";
    }

    return get_oid_2(infolib_ptr, base_str, locator);
}

char* get_oid_2(info_lib* infolib_ptr, char* base_str, char* locator)
{
    node_smart_ptr x(infolib_ptr, base_str, locator);

    const char *this_node_locator = x.locator();

    if ( strcmp( this_node_locator, locator) == 0 ) {
        ostringstream strout(oid_in_string_buf, ios::out);
        x.its_oid().asciiOut(strout);
        strcpy(oid_in_string_buf, strout.str().c_str());
    } else {
         cerr << "dangling locator " << locator << "\n"
              << "mismatches with " << this_node_locator << "\n"
              << "of node " << x.its_oid().asciiOut(cerr) << "\n";
         throw(stringException("dangling locator"));
    }

    return oid_in_string_buf;
}

