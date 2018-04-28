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
/* $XConsortium: StyleSheet.h /main/3 1996/06/11 17:09:16 cde-hal $ */

#ifndef _StyleSheet_h
#define _StyleSheet_h

#include "SymTab.h"
#include "VariableTable.h"
#include "PathTable.h"
#include "ResolverStack.h"

class StyleSheet 
{

private:
   SymbolTable 		f_SymTab;
   VariableTable 	f_VarTab;
   PathTable 		f_PathTab;
   SymbolTable  	f_ElemSymTab;
   unsigned int		f_GI_CASE_SENSITIVE;
   ResolverStackElement f_TopOfStack;

   char* f_name;

public:
   StyleSheet(const char* name = 0); 
   ~StyleSheet();

   const char* name() { return f_name; };

   void use();
};

#endif /* _StyleSheet_h */
/* DO NOT ADD ANY LINES AFTER THIS #endif */
