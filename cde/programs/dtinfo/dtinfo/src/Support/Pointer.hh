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
/*	Copyright (c) 1995 FUJITSU LIMITED	*/
/*	All Rights Reserved			*/

/*
 * $XConsortium: Pointer.hh /main/5 1996/09/27 19:02:25 drk $
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

#ifndef _Pointer_hh
#define _Pointer_hh

#ifndef NULL
#define NULL 0
#endif

#include "Exceptions.hh"
#include <stdio.h>

template <class T>
class Pointer : public Destructable
{
public:
  Pointer()
    : f_pointer (NULL)
    { ON_DEBUG (printf ("Pointer (null) @ 0x%p\n", this)); }

  // Copy constructor 
  Pointer (const Pointer<T> &pointer);
  Pointer (T *pointer);
  // inline Pointer (T &object);
  ~Pointer();

  // Member access operators.
  // Precondition: f_pointer != NULL
  T *operator->()
    { return (f_pointer); }
  const T *operator->() const
    { return (f_pointer); }

  // Assignment operator
  // Implemented at T level to support inheritance.
  // Client code should not access the T object directly. 
  operator T*() const
    { return (f_pointer); }
  Pointer<T> &operator= (T *pointer);
  Pointer<T> &operator= (const Pointer<T> &pointer);

  // Equality operator
  // Two pointers are equal if the point to the same document.
  bool operator== (const Pointer<T> &pointer) const;
  bool operator!= (const Pointer<T> &pointer) const;
  bool operator== (const int) const;
  bool operator!= (const int) const;

#if 0
  // NULL testing operator. 
  operator void*() const
    { return ((void *) (f_pointer != NULL)); }
#endif

private:
  T *f_pointer;
};

#ifdef EXPAND_TEMPLATES
#include "Pointer.C"
#endif

#endif /* _Pointer_hh */
/* DO NOT ADD ANY LINES AFTER THIS #endif */
