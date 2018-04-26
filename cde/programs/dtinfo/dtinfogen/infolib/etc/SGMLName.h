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
/* $XConsortium: SGMLName.h /main/2 1996/07/18 15:19:02 drk $ */

#ifndef __SGMLName_h
#define __SGMLName_h

class SGMLName{

 public:
  static int intern(const char*, int upcase = 0);

  static const char *lookup(int) /* throw(KeyError) */;

  enum DeclaredValue { IMPLIED, CDATA, NOTATION, TOKEN, ENTITY, qty };

  static void init();
};

#endif
