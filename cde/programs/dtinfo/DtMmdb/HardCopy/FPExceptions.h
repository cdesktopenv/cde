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
/* $TOG: FPExceptions.h /main/4 1998/04/17 11:46:25 mgreess $ */


#ifndef _FPExceptions_h
#define _FPExceptions_h

#include "Exceptions.hh"

/////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////
class hardCopyRendererException : public Exception
{
public:
  DECLARE_EXCEPTION(hardCopyRendererException, Exception)
};

#define CASTHCREXCEPT

#endif /* _FPExceptions_h */
/* DO NOT ADD ANY LINES AFTER THIS #endif */
