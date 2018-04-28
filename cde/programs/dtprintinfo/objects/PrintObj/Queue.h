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
/* $XConsortium: Queue.h /main/3 1995/11/06 09:47:44 rswiston $ */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */

#ifndef QUEUE_H
#define QUEUE_H

#include "BaseObj.h"
#include "dtprintinfomsg.h"

// Object Class Name
extern const char *QUEUE;

// Actions
extern const char *START_QUEUE;
extern const char *STOP_QUEUE;
#ifndef aix
extern const char *START_PRINTING;
extern const char *STOP_PRINTING;
#endif

// Attributes
extern const char *ICON_NAME;
extern const char *PRINTER_QUEUE;
extern const char *QUEUE_DEVICE;

// Status Commands
extern const char *GET_QUEUE_STATUS;
extern const char *GET_DEVICE_STATUS;

class Queue : public BaseObj {

   friend int Start(BaseObj *, char **output, BaseObj *requestor);
   friend int Stop(BaseObj *, char **output, BaseObj *requestor);
#ifndef aix
   friend int StartPrint(BaseObj *, char **output, BaseObj *requestor);
   friend int StopPrint(BaseObj *, char **output, BaseObj *requestor);
#endif

 protected:

   static int Start(BaseObj *, char **output, BaseObj *requestor);
   static int Stop(BaseObj *, char **output, BaseObj *requestor);
#ifndef aix
   static int StartPrint(BaseObj *, char **output, BaseObj *requestor);
   static int StopPrint(BaseObj *, char **output, BaseObj *requestor);
#endif

   boolean _loaded_attributes;

   void InitChildren();
   void LoadAttributes(int numAttributes, Attribute **attributes);
   void ParseOutput(char *, int);

   void ProcessJobs(char *jobs = NULL);

   boolean is_remote;
   boolean remote_up;
#ifdef aix
   char **local_devices;
   int n_devices;
#endif
   char *remote_server;
   char *remote_printer;

 public:

   Queue(BaseObj *parent, char *name);
   virtual ~Queue();

   const char *Server();
   const char *RemotePrinter();
#ifdef aix
   char *Device(int index = 0);
   int NumberDevices();
#endif
   boolean IsRemote();
   boolean RemoteUp() { return remote_up; }
   void ParseRemoteStatus(char *output);

   virtual const char *const ObjectClassName() { return QUEUE; }

};

#endif // QUEUE_H
