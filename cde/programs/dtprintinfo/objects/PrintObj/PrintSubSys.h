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
/* $XConsortium: PrintSubSys.h /main/3 1995/11/06 09:47:33 rswiston $ */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */

#ifndef PRINTSUBSYS_H
#define PRINTSUBSYS_H

#include "BaseObj.h"
#include "dtprintinfomsg.h"

// Object Class Name
extern const char *PRINTSUBSYSTEM;

// List Children command;
extern const char *LIST_QUEUES;

class PrintSubSystem : public BaseObj {

 protected:

   void InitChildren();

 public:

   PrintSubSystem(BaseObj *parent);
   virtual ~PrintSubSystem();

   virtual const char *const ObjectClassName() { return PRINTSUBSYSTEM; }

};

#endif // PRINTSUBSYS_H
