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
// $XConsortium: StyleSheet.cc /main/3 1996/06/11 17:09:10 cde-hal $

#include "StyleSheet.h"
#include "Types.h"
#include "Feature.h"

PathTable* 	gPathTab;
VariableTable* 	gVariableTable;
SymbolTable* 	gSymTab;
SymbolTable* 	gElemSymTab;
unsigned int	gGI_CASE_SENSITIVE;
const Element	       *gCurrentElement	;
const FeatureSet       *gCurrentLocalSet;
const FeatureSet       *gParentCompleteSet;

StyleSheet::StyleSheet(const char* nm)
: f_GI_CASE_SENSITIVE(false),
  f_TopOfStack(0,new FeatureSet(),new FeatureSet())
{
  if (nm==0) {
     f_name = new char[1];
     f_name[0] = 0;
  } else
     f_name = strdup(nm);

  use();
}

StyleSheet::~StyleSheet()
{
   delete f_name;
}

void
StyleSheet::use()
{
  gSymTab = &f_SymTab;
  gVariableTable = &f_VarTab;
  gPathTab = &f_PathTab;
  gElemSymTab = &f_ElemSymTab;
  gGI_CASE_SENSITIVE = f_GI_CASE_SENSITIVE;
}
