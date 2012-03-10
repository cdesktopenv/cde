/* $XConsortium: DtPrtProps.h /main/3 1995/11/06 09:36:44 rswiston $ */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */

#ifndef DTPRTPROPS_H
#define DTPRTPROPS_H

#include "Dialog.h"
#include "Prompt.h"

class Button;
class DtMainW;
class Group;
class LabelObj;
class IconObj;
class DtPrinterIcon;
class DtFindSet;

class DtPrtProps : public Dialog {

   friend class DtFindIcon;
   friend void OkCB(void *data);
   friend void ApplyCB(void *data);
   friend void CancelCB(void *data);
   friend void ResetCB(void *data);
   friend void HelpCB(void *data);
   friend void FindSetCB(void *data);
   friend void CallerCB(BaseUI *, char *);

   static void OkCB(void *data);
   static void ApplyCB(void *data);
   static void CancelCB(void *data);
   static void ResetCB(void *data);
   static void HelpCB(void *data);
   static void FindSetCB(void *data);
   static void CallerCB(BaseUI *, char *);

 private:

   DtMainW *mainw;

   // dialog buttons
   Button *ok;
   Button *apply;
   Button *cancel;
   Button *reset;
   Button *help;
   Group *icon_group;
   Prompt *icon_prompt;
   Prompt *description;
   DtFindSet *findSetD;
   boolean _has_been_posted;
   char *_iconFile;
   DtPrinterIcon *printer;
   IconObj *smallIcon;
   IconObj *mediumIcon;
   IconObj *largeIcon;
   Button *find_set;

   void SetActionIcons(const char *icon);
   void CloseCB();
   boolean SetVisiblity(boolean);
   boolean HandleHelpRequest();

 public:

   DtPrtProps(DtMainW *, char *name, DtPrinterIcon *printer);
   ~DtPrtProps();
   DtPrinterIcon *Printer() { return printer; }
   void Printer(DtPrinterIcon *printer);
   const char *IconFileName() { return _iconFile; }
   char *IconLabel() { return icon_prompt->Value(); }
   void IconLabel(char *value) { icon_prompt->Value(value); }
   void Apply();
   void Reset();

};

#endif // DTPRTPROPS_H
