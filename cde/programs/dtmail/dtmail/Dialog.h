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
