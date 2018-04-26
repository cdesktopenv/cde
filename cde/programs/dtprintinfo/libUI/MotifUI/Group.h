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
/* $XConsortium: Group.h /main/3 1995/11/06 09:40:43 rswiston $ */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */

#ifndef GROUP_H
#define GROUP_H

#include "MotifUI.h"

class Group : public MotifUI {

 private:
   
   Widget _label;
   Widget _rc;
   GroupType _group_type;

 public:

   Group(MotifUI *parent,
	 char *name,
	 GroupType group_type = RADIO_GROUP);

   const Widget InnerWidget()         { return _rc; }
   const UI_Class UIClass()           { return GROUP; }
   const int UISubClass()             { return _group_type; }
   const char *const UIClassName()    { return "Group"; }

   // Override SetName, need to set label name, not BaseWidget
   boolean SetName(char *);

};

#endif /* GROUP_H */
