/*
 *+SNOTICE
 *
 *	$XConsortium: DialogWindow.h /main/4 1996/04/21 19:41:28 drk $
 *
 *	RESTRICTED CONFIDENTIAL INFORMATION:
 *	
 *	The information in this document is subject to special
 *	restrictions in a confidential disclosure agreement between
 *	HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
 *	document outside HP, IBM, Sun, USL, SCO, or Univel without
 *	Sun's specific written approval.  This document and all copies
 *	and derivative works thereof must be returned or destroyed at
 *	Sun's request.
 *
 *	Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 *
 *+ENOTICE
 */

#ifndef DIALOGWINDOW_H
#define DIALOGWINDOW_H

#include "UIComponent.h"

class RoamMenuWindow;

class DialogWindow : public UIComponent {

  static void popdownCallback ( Widget, XtPointer, XmAnyCallbackStruct * );
  static void popupCallback( Widget, XtPointer, XmAnyCallbackStruct * );

protected:

  Widget _workArea;
  RoamMenuWindow *_parent;
  virtual Widget createWorkArea( Widget ) = 0;

public:
  DialogWindow( char *, RoamMenuWindow * );
  virtual ~DialogWindow();
  virtual void initialize();

// Accessors

  RoamMenuWindow *parent() { return _parent; };

// Mutators

  virtual void title( char * );
  virtual void popup()=0;
  virtual void popdown()=0;
};

#endif
