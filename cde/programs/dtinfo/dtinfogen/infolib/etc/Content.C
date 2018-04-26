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
/* $XConsortium: Content.cc /main/2 1996/07/18 16:10:34 drk $ */

/* exported interfaces */
#include "Content.h"

/* imported interfaces */
#include "Dispatch.h"
#include "OL-Data.h"
#include "Token.h"
#include "SGMLName.h"

void
Content::markup( const Token &t )
{

  if ( ignore_status && !Dispatch::OutsideIgnoreScope() ) {
    return;
  }

  if ( f_base > 0 ) {

    if ( t.type() == END ) {
      if ( f_base == t.level() ) {
	f_base = -1;
	data_complete=1;
      }
    }
    else if ( t.type() == EXTERNAL_ENTITY ) {
      data_avail = 1;
      ValueBuffer.writeStr( t.getEntityFileName() );
    }
    else if ( t.type() == INTERNAL_ENTITY ) {
      data_avail = 1;
      ValueBuffer.put( '&' );
      ValueBuffer.writeStr( SGMLName::lookup( t.getEntityName() ) );
      ValueBuffer.put( ';' );
    }

  }
}

void
Content::data( const char *str, size_t sz )
{

  if ( ignore_status && !Dispatch::OutsideIgnoreScope() ) {
    return;
  }

  if ( f_base > 0 ) {
    ValueBuffer.write ( str, sz );
  }
}

	
