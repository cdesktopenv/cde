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
/* $XConsortium: ContentRec.h /main/2 1996/07/18 16:41:34 drk $ */
#ifndef __CRec_hdr__
#define __CRec_hdr__

#include "VarElementList.h"

class ContentRec {

friend class OL_Data;
friend int ol_dataparse();
friend class ContentType;
  
private:
  int            data_type;
  int            attr_name;
  VarElementList var_list;  
  ContentRec *next;

public:
  void Init ( int dtype, int aname=0, VarElement *vList=0 );
  ContentRec() { attr_name = -1; data_type=-1, next = 0; }
};

/*--------------------------------------------------------*/
inline
void
ContentRec::Init( int dtype, int aname, VarElement *vlist )
{
  data_type = dtype;
  attr_name = aname;
  var_list.insert ( vlist );
}

#endif
