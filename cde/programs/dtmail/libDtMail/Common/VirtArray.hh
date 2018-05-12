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
/*
 *+SNOTICE
 *
 *	$TOG: VirtArray.hh /main/5 1997/12/22 16:35:24 bill $
 *
 *	RESTRICTED CONFIDENTIAL INFORMATION:
 *	
 *	The information in this document is subject to special
 *	restrictions in a confidential disclosure agreement between
 *	HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
 *	document outside HP, IBM, Sun, USL, SCO, or Univel without
 *	Sun's specific written approval.  This document and all copies
 *	and derivative works thereof must be returned or destroyed at
 *	Sun's request.
 *
 *	Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 *
 *+ENOTICE
 */

#ifndef _VIRTARRAY_HH
#define _VIRTARRAY_HH

#include <DtMail/DtLanguages.hh>

class DtVirtArrayImpl : public DtCPlusPlusAllocator {
  public:
    DtVirtArrayImpl(const int size);
    ~DtVirtArrayImpl(void);
    
    int length(void);
    void * operator[](const int at);
    
    int indexof(void * handle);
    void append(void * handle);
    void insert(void * handle, const int at);
    void remove(const int at);
    
  private:
    void make_slot(const int at);
    void grow(void);

    void	**_elements;
    int		_size;
    int		_count;

    void	*_mutex;
};

template <class Element> 
class DtVirtArray : public DtCPlusPlusAllocator {
  public:
    DtVirtArray(const int size) 
    : my_array(size) {}

    ~DtVirtArray(void) {}

    int length(void) { return(my_array.length()); }

    Element operator[](const int at) { return((Element)my_array[at]); }
    int indexof(Element handle) { return(my_array.indexof(handle)); }
    void append(Element handle) { my_array.append(handle); }
    void insert(Element handle, const int at) { my_array.insert(handle, at); }
    void remove(const int at) { my_array.remove(at); }

  private:
    DtVirtArrayImpl	my_array;
};

#endif
