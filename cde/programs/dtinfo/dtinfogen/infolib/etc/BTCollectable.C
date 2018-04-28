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
/* $XConsortium: BTCollectable.cc /main/2 1996/07/18 16:09:37 drk $ */
#include <string.h>

// exported interfaces 
#include "BTCollectable.h"

//--------------------------------------------------------------------
BTCollectable::BTCollectable():f_name(0), line_num(0), value(0)
{
}

//--------------------------------------------------------------------
BTCollectable::BTCollectable( 
  const char *filename, int line_no, const char *val
  )
{

  f_name = strdup( filename );
  line_num = line_no;
  value = strdup( val );
}

//--------------------------------------------------------------------
BTCollectable::~BTCollectable()
{
  delete f_name;
  delete value;
}
