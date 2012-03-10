/* $XConsortium: ComboBoxObj.h /main/3 1995/11/06 09:39:32 rswiston $ */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */

#ifndef COMBOBOXOBJ_H
#define COMBOBOXOBJ_H

#include "MotifUI.h"

class ComboBoxObj;

typedef void (*ComboBoxCallback) (ComboBoxObj *, char *item, int item_position);

class ComboBoxObj : public MotifUI {

  friend void SelectCB(Widget, XtPointer, XtPointer);

 private:

  static void SelectCB(Widget, XtPointer, XtPointer);

  Widget _label;
  Widget _list;
  Widget _combo_box;
  ComboBoxCallback _callback;

 public:

#if defined(USL) || defined(__uxp__)
   ComboBoxObj(MotifUI * parent, void (*)(ComboBoxObj *, char *, int),
	       char *name, char **items, int n_items);
#else
   ComboBoxObj(MotifUI * parent, ComboBoxCallback, char *name, 
	       char **items, int n_items);
#endif

   void Add(char *item);
   void Delete(char *item);
   char * Item(char *item);
   char * Item(int index);

   const Widget InnerWidget()         { return _combo_box; }
   const UI_Class UIClass()           { return COMBO_BOX; }
   const char *const UIClassName()    { return "ComboBox"; }

};

#endif /* COMBOBOXOBJ_H */
