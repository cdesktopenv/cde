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
/* $XConsortium: CC_Tokenizer.h /main/4 1996/11/04 13:35:14 drk $ */
#ifndef __CC_Token_h
#define __CC_Token_h

#include "CC_String.h"

#define X_INCLUDE_STRING_H
#define XOS_USE_NO_LOCKING
#include <X11/Xos_r.h>

class CC_Tokenizer {

private:
  char      *current_ptr;
  char      *str_;
  CC_Boolean touched;
  _Xstrtokparams	strtok_buf;

public:
  CC_Tokenizer(const CC_String & );
  ~CC_Tokenizer() { delete [] str_; }


  /* Here is an example to use the code 
   *     CC_String b("This is a string");
   *     CC_Tokenizer next( b );
   *     while ( next() ) {
   *         cout << next.data() << endl;
   *     }
   */

  CC_Boolean operator()();  /* returns TRUE if next token exists, 
			     * FALSE if otherwise
			     */
  const char *data() { return(current_ptr); } /* returns the current token */
};
  
#endif

