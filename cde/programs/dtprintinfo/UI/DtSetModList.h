/* $XConsortium: DtSetModList.h /main/3 1995/11/06 09:36:56 rswiston $ */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */

#ifndef DTSETMODLIST_H
#define DTSETMODLIST_H

#include "Dialog.h"

class Button;
class Container;
class LabelObj;
class DtMainW;

class DtSetModList : public Dialog {

   friend void OkCB(void *);
   friend void ApplyCB(void *);
   friend void CancelCB(void *);
   friend void HelpCB(void *);
   friend void SelectAllCB(void *);
   friend void UnSelectAllCB(void *);

 private:

   boolean _has_been_posted;
   Container *_container;
   DtMainW *mainw;
   Container *_workarea;
   LabelObj *_info;
   Button *select_all;
   Button *unselect_all;

   // dialog buttons
   Button *ok;
   Button *apply;
   Button *cancel;
   Button *help;

   static void OkCB(void *);
   static void ApplyCB(void *);
   static void CancelCB(void *);
   static void HelpCB(void *);
   static void SelectAllCB(void *);
   static void UnSelectAllCB(void *);

   boolean SetVisiblity(boolean flag);
   ButtonCallback _filterCB;
   void CloseCB();

   void Apply();
   void Cancel();
   boolean HandleHelpRequest();

 public:

   DtSetModList(AnyUI *parent, char *name, BaseUI *container,
		ButtonCallback filterCB);
   virtual ~DtSetModList();
   void Reset();

};

#endif /* DTSETMODLIST_H */
