/* $XConsortium: DtProps.h /main/3 1995/11/06 09:36:21 rswiston $ */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */

#ifndef DTPROPS_H
#define DTPROPS_H

#include "Dialog.h"
#include "BaseObj.h"

class Button;
class Container;
class Prompt;
class DtMainW;

class DtProps : public Dialog {

   friend void OkCB(void *data);
   friend void ApplyCB(void *data);
   friend void CancelCB(void *data);
   friend void ResetCB(void *data);
   friend void HelpCB(void *data);

   static void OkCB(void *data);
   static void ApplyCB(void *data);
   static void CancelCB(void *data);
   static void ResetCB(void *data);
   static void HelpCB(void *data);

 private:

   DtMainW *mainw;

   // dialog buttons
   Button *ok;
   Button *apply;
   Button *cancel;
   Button *reset;
   Button *help;
   Container *rc;
   char *_location_id;
   boolean _has_been_posted;

   void CloseCB();
   boolean SetVisiblity(boolean);
   boolean HandleHelpRequest();

 public:

   DtProps(AnyUI *parent,
	   char *name,
	   char *location_id,
	   boolean editable,
	   int n_attrs,
	   Attribute **attrs);
   virtual ~DtProps();
   void Apply();
   void Reset();

};

#endif // DTPROPS_H
