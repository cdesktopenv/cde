/* $XConsortium: DtFindSet.h /main/3 1995/11/06 09:35:17 rswiston $ */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */

#ifndef DTFINDSET_H
#define DTFINDSET_H

#include "Dialog.h"
#include "Prompt.h"

class Button;
class DtMainW;
class LabelObj;
class Container;
class ComboBoxObj;
class FindSetIcon;
class HelpSystem;

typedef void (*CallerCallback)(BaseUI *caller, char *iconFile);

typedef struct
{
   boolean read_it;
   FindSetIcon **icons;
   int n_icons;
} FileNamesStruct, *FileNames, **FileNamesList;

class DtFindSet : public Dialog {

   friend void OkCB(void *data);
   friend void ApplyCB(void *data);
   friend void CancelCB(void *data);
   friend void ResetCB(void *data);
   friend void HelpCB(void *data);
   friend void ComboBoxCB(ComboBoxObj *, char *, int);
   friend void InitComboBox(BaseUI *, void *data);

   static void OkCB(void *data);
   static void ApplyCB(void *data);
   static void CancelCB(void *data);
   static void ResetCB(void *data);
   static void HelpCB(void *data);
   static void ComboBoxCB(ComboBoxObj *, char *, int);
   static void InitComboBox(BaseUI *, void *data);

 private:

   DtMainW *mainw;
   HelpSystem *helpSystem;

   // dialog buttons
   Button *ok;
   Button *apply;
   Button *cancel;
   Button *reset;
   Button *help;

   char **dirs;
   int n_dirs;
   int last_position;
   BaseUI *caller;
   CallerCallback callback;
   FileNamesList filenames;
   ComboBoxObj *comboBox;
   Container *icons;
   LabelObj *empty;
   LabelObj *icon_label;
   boolean _has_been_posted;

   void CloseCB();
   boolean SetVisiblity(boolean);
   boolean HandleHelpRequest();

 public:

   DtFindSet(DtMainW *, char *name, CallerCallback callback);
   ~DtFindSet();
   void Caller(BaseUI *obj) { caller = obj; }
   void Apply();
   void Reset();

};

#endif // DTFINDSET_H
