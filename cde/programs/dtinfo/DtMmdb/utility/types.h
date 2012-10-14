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
 * $XConsortium: types.h /main/5 1996/08/21 15:55:48 drk $
 *
 * Copyright (c) 1992 HaL Computer Systems, Inc.  All rights reserved.
 * UNPUBLISHED -- rights reserved under the Copyright Laws of the United
 * States.  Use of a copyright notice is precautionary only and does not
 * imply publication or disclosure.
 * 
 * This software contains confidential information and trade secrets of HaL
 * Computer Systems, Inc.  Use, disclosure, or reproduction is prohibited
 * without the prior express written permission of HaL Computer Systems, Inc.
 * 
 *                         RESTRICTED RIGHTS LEGEND
 * Use, duplication, or disclosure by the Government is subject to
 * restrictions as set forth in subparagraph (c)(l)(ii) of the Rights in
 * Technical Data and Computer Software clause at DFARS 252.227-7013.
 *                        HaL Computer Systems, Inc.
 *                  1315 Dell Avenue, Campbell, CA  95008
 * 
 */



#ifndef _types_h
#define _types_h 1

#ifdef C_API
#include "utility/c_iostream.h"
#else
#include <iostream>
using namespace std;
#endif

#define true  1
#define false 0

typedef char Boolean;
typedef void* voidPtr;
typedef char* charPtr;

typedef short 		s_int16;
typedef unsigned short	u_int16;

typedef int		s_int32;
typedef unsigned int	u_int32;

typedef long 		s_long32;

typedef float 		s_float32;

enum io_status { done, fail };

class root;
typedef Boolean (*cmp_func_ptr_t)(const void*, const void*);
typedef void (*app_func_ptr_t)(const void*);
typedef void (*print_func_ptr_t)(ostream&, const void*);

//enum Boolean { true, false };

#ifndef __osf__
typedef long mmdb_pos_t;
#else
typedef int mmdb_pos_t;
#endif


#endif
