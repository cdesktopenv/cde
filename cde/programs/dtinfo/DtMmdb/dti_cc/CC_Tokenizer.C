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
// $XConsortium: CC_Tokenizer.C /main/5 1996/11/21 19:47:27 drk $
#include <string.h>
#include "CC_Tokenizer.h"

//--------------------------------------------------------------
CC_Tokenizer::CC_Tokenizer( const CC_String &s )
{
  int len = s.length();
  str_ = new char [len + 1];
  *((char *) memcpy(str_, s.data(), len) + len) = '\0';
  current_ptr = str_;
  touched = FALSE;
}


//--------------------------------------------------------------
CC_Boolean
CC_Tokenizer::operator()()
{

  if ( !touched ) {
    current_ptr = _XStrtok(str_, " \t\n", strtok_buf);
    touched = TRUE;
  }
  else {
    current_ptr = _XStrtok(NULL, " \t\n", strtok_buf);
  }

  return ( current_ptr != NULL );
}
    
  
  
  
