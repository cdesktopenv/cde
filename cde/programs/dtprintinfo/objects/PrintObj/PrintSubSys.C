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
/* $XConsortium: PrintSubSys.C /main/4 1996/01/17 18:02:47 lehors $ */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */

#include "PrintSubSys.h"
#include "Queue.h"

#include <string.h>

#ifdef aix
const char *LIST_QUEUES = "lsallq | grep -v '^bsh$' | sort";
#else
#ifdef hpux
const char *LIST_QUEUES = "LANG=C lpstat -v | "
			    "awk '"
			    " $2 == \"for\" "
			    "   { "
			    "      x = match($3, /:/); "
			    "      print substr($3, 1, x-1)"
			    "   }' | sort";
#else
const char *LIST_QUEUES = "LANG=C lpstat -v | "
			    "nawk '"
			    " $2 == \"for\" "
			    "   { "
			    "      x = match($3, /:/); "
			    "      print substr($3, 1, x-1)"
			    "   }' | sort";
#endif
#endif

// Object Class Name
const char *PRINTSUBSYSTEM = "PrintSubSystem";

PrintSubSystem::PrintSubSystem(BaseObj *parent)
	: BaseObj(parent, "PrintSubSystem")
{
   _displayName = strdup(MESSAGE(PrinterMenuL));
   _details = strdup("Status   Number   Owner      Date       Time       Size");
}

PrintSubSystem::~PrintSubSystem()
{
   // Empty
}

void PrintSubSystem::InitChildren()
{
   char *std_out;
   if (RunCommand(LIST_QUEUES, &std_out))
    {
      Error("InitChildren method could not list queues.");
    }
   else
    {
      char *queue = strtok(std_out, " \n");
      while (queue && *queue)
       {
         new Queue(this, queue);
         queue = strtok(NULL, " \n");
       }
    }
   delete std_out;
}
