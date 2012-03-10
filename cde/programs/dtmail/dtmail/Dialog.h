/*
 *+SNOTICE
 *
 *	$XConsortium: Dialog.h /main/4 1996/04/21 19:41:19 drk $
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

#ifndef DIALOG_H
#define DIALOG_H

#include <UIComponent.h>

class RoamMenuWindow;

class Dialog : public UIComponent {

  public:
    Dialog(char *, RoamMenuWindow *);
    Dialog(RoamMenuWindow *);		// For custom dialogs.

    virtual ~Dialog();
    virtual void initialize();
    
// Accessors
    
    RoamMenuWindow *parent() { return _parent; };
    Widget work_area(){ return _workArea; }
    
// Mutators
    
    virtual void title( char * );
    virtual void popped_up()=0;
    virtual void popped_down()=0;
    virtual void popup()=0;
    virtual void popdown()=0;
    virtual void manage();
    virtual void busyCursor();
    virtual void normalCursor();
    

    
  protected:

    virtual Widget createWorkArea( Widget ) = 0;
    static void popdownCallback ( Widget, XtPointer, XmAnyCallbackStruct * );
    static void popupCallback( Widget, XtPointer, XmAnyCallbackStruct * );

  private:

    Widget _workArea;
    RoamMenuWindow *_parent;

};

#endif
