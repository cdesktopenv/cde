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
/* $XConsortium: DtFindD.h /main/3 1995/11/06 09:35:06 rswiston $ */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */

#ifndef DTFINDD_H
#define DTFINDD_H

#include "Dialog.h"
#include "Container.h"

class Button;
class LabelObj;
class Prompt;
class Container;
class DtMainW;
class DtFindD;

class DtJobList : public Container
{

 private:

   DtFindD *findD;
   void NotifySelected(BaseUI *obj);

 public:

   DtJobList(AnyUI *parent);

};

class DtFindD : public Dialog {

   friend class DtJobList;

   friend void StartCB(void *);
   friend void StopCB(void *);
   friend void CancelCB(void *);
   friend void GotoCB(void *);
   friend void CancelJobsCB(void *);
   friend void HelpCB(void *);
   friend void CheckQueue(BaseUI *, void *);

 private:

   boolean _has_been_posted;
   int _cur_obj;

   DtMainW *mainw;

   // dialog buttons
   Button *start;
   Button *stop;
   Button *close_it;
   Button *help;
   Button *cancel_jobs;
   Button *goto_job;
   Button *ignore_case;
   Button *exact_match;
   Button *match_any_user;
   DtJobList *found_container;
   LabelObj *field1;
   LabelObj *field2;
   LabelObj *field3;
   LabelObj *field4;
   Prompt *prompt;
   SelectProc _select_proc;
   boolean _working;
   boolean _prev_opened;
   boolean _prev_visible;

   static void StartCB(void *);
   static void StopCB(void *);
   static void CancelCB(void *);
   static void GotoCB(void *);
   static void CancelJobsCB(void *);
   static void HelpCB(void *);
   static void CheckQueue(BaseUI *, void *);

   boolean SetVisiblity(boolean flag);

   void CloseCB();
   void Cancel();
   void Stop();
   void Start();
   void UpdateQueue();
   BaseUI *FindJob(BaseUI *obj);
   boolean HandleHelpRequest();

 public:

   DtFindD(AnyUI *parent, char *name, SelectProc select_proc);
   boolean Working() { return _working; }
   boolean MatchAnyUser();
   void UpdateMatchAnyUser();
   void DeleteJobFromList(BaseUI *);
   void UpdatePrinter(BaseUI *printer);
   void UpdatePositions(BaseUI *printer);
   virtual ~DtFindD();

};

#endif /* DTFINDD_H */
