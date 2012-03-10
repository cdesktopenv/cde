/*
 *+SNOTICE
 *
 *	$TOG: DialogShell.h /main/5 1998/01/28 18:35:07 mgreess $
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

#ifndef DIALOGSHELL_H
#define DIALOGSHELL_H

#include <Xm/DialogS.h>
#include "UIComponent.h"

class RoamMenuWindow;

class DialogShell : public UIComponent {

  public:
    DialogShell(
		char		*name,
		RoamMenuWindow	*parent,
		WidgetClass	wc = xmDialogShellWidgetClass);
    virtual ~DialogShell();
    virtual void initialize();
    
// Accessors
    
    RoamMenuWindow *parent() { return _parent; };
    Widget work_area(){ return _workArea; }
    
// Mutators
    
    virtual void title( char * );
    virtual void popped_up()=0;
    virtual void popped_down()=0;
    virtual void manage();
    virtual void busyCursor();
    virtual void normalCursor();
    
    virtual void quit()=0;
    
  protected:

    virtual Widget createWorkArea( Widget ) = 0;
    static void popdownCallback ( Widget, XtPointer, XmAnyCallbackStruct * );
    static void popupCallback( Widget, XtPointer, XmAnyCallbackStruct * );
    Widget _workArea;

  private:

    static void quitCallback( Widget, XtPointer, XmAnyCallbackStruct * );    

    RoamMenuWindow *_parent;
    WidgetClass	_widgetClass;

};

#endif
