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
// $XConsortium: create_macros.hh /main/3 1996/06/11 16:17:20 cde-hal $
#ifndef _create_macros_hh
#define _create_macros_hh

#define DECL(TYPE,VAR,PARENT,NAME) TYPE VAR (PARENT, NAME)
#define DECLC(TYPE,VAR,PARENT,NAME) TYPE VAR (PARENT, (char*)NAME)
#define DECLM(TYPE,VAR,PARENT,NAME) TYPE VAR (PARENT, NAME, WAutoManage)
#define DECLMC(TYPE,VAR,PARENT,NAME) TYPE VAR (PARENT, (char*)NAME, WAutoManage)
#define ASSN(TYPE,VAR,PARENT,NAME) VAR = TYPE (PARENT, NAME)
#define ASSNM(TYPE,VAR,PARENT,NAME) VAR = TYPE (PARENT, NAME, WAutoManage)

#define SET_CALLBACK(WOBJ,CBACK,FUNC) \
  (WOBJ).CONCAT3(Set,CBACK,Callback) (this, (WWL_FUN) &CLASS::FUNC)
#define SET_CALLBACK_D(WOBJ,CBACK,FUNC,DATA) \
  (WOBJ).CONCAT3(Set,CBACK,Callback) (this, (WWL_FUN) &CLASS::FUNC, \
				      (void *) DATA)

#define ON_ACTIVATE(WOBJ,FUNC) SET_CALLBACK(WOBJ,Activate,FUNC)
#define ON_ACTIVATE_D(WOBJ,FUNC,DATA) SET_CALLBACK_D(WOBJ,Activate,FUNC,DATA)

#define CALL_DATA(TYPE,NAME) \
  TYPE *NAME = (TYPE *) wcb->CallData()

#ifndef CLASS
  #error "CLASS macro must be defined!"
#endif

#endif /* _create_macros_hh */
/* DO NOT ADD ANY LINES AFTER THIS #endif */
