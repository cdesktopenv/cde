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
/* $XConsortium: MotifThread.h /main/3 1995/11/06 09:43:54 rswiston $ */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */

#ifndef MOTIF_THREAD_H
#define MOTIF_THREAD_H

#include "MotifUI.h"

class MotifThread;
class MotifUI;

typedef void (*MotifThreadCallback) (MotifThread *, BaseUI *, ThreadCallback);

class MotifThread
{

   friend void GetOutputCB(XtPointer, int *fid, XtInputId *id);

 private:
   MotifThreadCallback cb;
   ThreadCallback cb1;
   XtInputId inputID;
   pid_t pid;
   int buf_len;
   int fd;
   int len;
   int ctr;
   char *out1;
   char *out2;
   MotifUI *obj;

   static void GetOutputCB(XtPointer, int *fid, XtInputId *id);
   void GetOutput();
   void CreateThread(MotifUI *, const char *, int, int, 
		     MotifThreadCallback, ThreadCallback, int);

 public:
   char *output;
   int status;
   MotifThread(MotifUI *, const char *cmd, MotifThreadCallback, ThreadCallback,
	       int buf_len);
   MotifThread(MotifUI *, int pid, int fd, MotifThreadCallback, ThreadCallback,
	       int buf_len);
   MotifThread(MotifUI *, int socket, MotifThreadCallback, ThreadCallback,
	       int buf_len);
   ~MotifThread();
   void Halt();
};

#endif // MOTIF_THREAD_H
