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
/* $XConsortium: Sep.h /main/3 1995/11/06 09:44:53 rswiston $ */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */

#ifndef SEP_H
#define SEP_H

#include "MotifUI.h"

class Sep : public MotifUI {

 private:

   SeparatorStyle _style;

 public:

   Sep(MotifUI * parent,
       SeparatorStyle style = SHADOW_ETCHED_IN);
   Sep(char *category,
       MotifUI * parent,
       SeparatorStyle style = SHADOW_ETCHED_IN);

   const UI_Class UIClass()           { return SEPARATOR; }
   const int UISubClass()             { return _style; }
   const char *const UIClassName()    { return "Sep"; }

};

#endif /* SEP_H */
