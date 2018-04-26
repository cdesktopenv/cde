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
/* $XConsortium: Expression.h /main/3 1996/08/21 15:46:44 drk $ */
#ifndef __CRec_hdr__
#define __CRec_hdr__

enum OL_DATA_TYPE {
  INVALID_OL_TYPE,
  CONTENT,
  CONCAT,
  FIRSTOF,
  GENERIC_ID,
  LITERAL,
  REFERENCE
};

class OL_Expression {

friend class OL_Data;
friend class FirstOf;
friend class Concat;  
friend int ol_dataparse();
friend class ContentType;
friend class ExprList;
  
private:
  OL_DATA_TYPE   data_type;
  int            ename;
  void           *value_list;  
  OL_Expression  *next;

public:
  OL_Expression( OL_DATA_TYPE dtype=INVALID_OL_TYPE,
	      int dname=-1,
	      void *vlist=0) {
    data_type = dtype; ename = dname; value_list = vlist; next = 0;
  }
  
  ~OL_Expression();
  
  OL_DATA_TYPE type()       const { return data_type; }
  int          name()       const { return ename;      }
  void         *data_list() const { return value_list; }

};

#endif
