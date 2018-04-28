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
/* $XConsortium: VariableTable.h /main/4 1996/08/21 15:51:00 drk $ */
#ifndef _VariableTable_h
#define _VariableTable_h

#include "SymTab.h"
#include "Expression.h"

#ifndef CDE_NEXT

#else
#include "dti_cc/cc_hdict.h"
#endif

class VariableTable : private hashTable<Symbol, Expression>
{
public:
  VariableTable();
  ~VariableTable();

  unsigned int	exists(const Symbol &name) const; 
  void		enter (const Symbol &name, Expression *value);
  const Expression  &lookup(const Symbol &name) const;

  ostream &print(ostream &) const;

};

ostream &operator <<(ostream &o, const VariableTable &v);

#endif /* _VariableTable_h */
/* DO NOT ADD ANY LINES AFTER THIS #endif */
