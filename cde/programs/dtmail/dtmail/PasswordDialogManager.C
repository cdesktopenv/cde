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
 *	$XConsortium: PasswordDialogManager.C /main/4 1996/04/21 19:42:53 drk $
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

#include "PasswordDialogManager.h"
#include <Xm/SelectioB.h>
#include <Xm/TextF.h>
#include <Xm/Label.h>
#include <Xm/RowColumn.h>

PasswordDialogManager *thePasswordDialogManager = 
    new PasswordDialogManager ( "PasswordDialog" );


PasswordDialogManager::PasswordDialogManager ( char   *name ) : 
                                 PromptDialogManager ( name )
{
    // Empty
    _pwd[0] = 0;
    _user = NULL;
    _password = NULL;
}

Widget PasswordDialogManager::createDialog ( Widget parent )
{
   
  Widget dialog = XmCreatePromptDialog ( parent, _name, NULL, 0);
    
  XtVaSetValues ( dialog,
		  XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL,
		  NULL );

  XtUnmanageChild( XmSelectionBoxGetChild( dialog,
				      XmDIALOG_TEXT ) );

  XtUnmanageChild( XmSelectionBoxGetChild( dialog,
				      XmDIALOG_SELECTION_LABEL ) );

  Widget rc = XtCreateManagedWidget ( "PasswordArea",
				      xmRowColumnWidgetClass,
				      dialog,
				      NULL, 0 );

  Widget _user_label = XtCreateManagedWidget
    ( "UserLabel",
      xmLabelWidgetClass,
      rc,
      NULL, 0);

  _user = XtCreateManagedWidget
    ( "User",
      xmTextFieldWidgetClass,
      rc,
      NULL, 0);

  Widget _password_label = XtCreateManagedWidget
    ( "PasswordLabel",
      xmLabelWidgetClass,
      rc,
      NULL, 0);

  _password = XtCreateManagedWidget
    ( "Password",
      xmTextFieldWidgetClass,
      rc,
      NULL, 0);

  XtManageChild( rc );
  XtAddCallback ( _password,
		  XmNmodifyVerifyCallback,
		  (XtCallbackProc )
		  &PasswordDialogManager::modifyVerifyCallback,
		  ( XtPointer ) this );
  return dialog;
}

#ifdef DEAD_WOOD
char *
PasswordDialogManager::userName(){
  return ( XmTextFieldGetString( _user ) );
}

char *
PasswordDialogManager::password(){
//  return ( XmTextFieldGetString( _password ) );
  return _pwd;
}
#endif /* DEAD_WOOD */

void
PasswordDialogManager::modifyVerifyCallback( Widget w,
		      XtPointer clientData,
		      XmTextVerifyCallbackStruct *cbs ) {
  PasswordDialogManager *pdm=( PasswordDialogManager * ) clientData;
  pdm->modifyVerify( w, cbs );
}
		      
void
PasswordDialogManager::modifyVerify( Widget ,
				     XmTextVerifyCallbackStruct *cbs )
{
 int len;

  if ( cbs->text->ptr == NULL ) // Backspace
    return;
  for ( len=0; len< cbs->text->length; len++ ) {
    strncat(_pwd, &cbs->text->ptr[len], 1);
    cbs->text->ptr[len] = '*';
  }
}

#ifdef DEAD_WOOD
void
PasswordDialogManager::resetPassword()
{
    _pwd[0] = 0;
}
#endif /* DEAD_WOOD */
