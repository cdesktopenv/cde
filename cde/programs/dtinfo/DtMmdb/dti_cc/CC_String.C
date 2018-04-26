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
 * License along with these libraries and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
// $XConsortium: CC_String.cc /main/3 1996/06/11 16:55:54 cde-hal $
#include "CC_String.h"
#include <ctype.h>

#include "utility/atoi_fast.h"

unsigned int 
CC_String::hash() const
{
  static atoi_fast converter(65535, 256);
  return converter.atoi(f_string);
}

CC_String::CC_String (const CC_String& x)
{
   f_string = strdup(x.f_string);
}

//-----------------------------------------------------
int
CC_String::compareTo(const char *str, caseCompare CaseSensitive ) const
{
  if ( CaseSensitive == exact ) {
    return (strcmp(f_string, str));
  }
  else {
    const char *ptr1 = f_string;
    const char *ptr2 = str;
    for ( ; tolower(*ptr1) == tolower(*ptr2) ; ptr1++, ptr2++ ) {
      if ( *ptr1 == '\0' ) {
	return (0);
      }
    }

    return ( tolower(*ptr1) - tolower(*ptr2) );
  }
}

	
      
      
    
