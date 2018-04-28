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
/* $XConsortium: xmcmds.h /main/6 1996/04/01 17:54:16 rswiston $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        xmcmds.h
 **
 **   Project:     CDE
 **
 **   Description: Public include file for xmcmds.c
 **
 **
 **   (c) Copyright 1987, 1988, 1989, 1990, 1991, 1992
 **       by Hewlett-Packard Company
 **
 **
 **
 ****************************************************************************
 ************************************<+>*************************************/

#ifndef _Dtksh_xmcmds_h
#define _Dtksh_xmcmds_h



extern int toolkit_initialize( 
                        int argc,
                        char *argv[]) ;
extern int toolkit_special_resource( 
                        char *arg0,
                        XtResourceList res,
                        wtab_t *w,
                        wtab_t *parent,
                        classtab_t *class,
                        char *resource,
                        char *val,
                        XtArgVal *ret,
                        int *freeit,
                        Boolean postponePixmaps) ;
extern int do_XmCreateArrowButton( 
                        int argc,
                        char *argv[]) ;
extern int do_XmCreateArrowButtonGadget( 
                        int argc,
                        char *argv[]) ;
extern int do_XmCreateBulletinBoard( 
                        int argc,
                        char *argv[]) ;
extern int do_XmCreateBulletinBoardDialog( 
                        int argc,
                        char *argv[]) ;
extern int do_XmCreateCascadeButton( 
                        int argc,
                        char *argv[]) ;
extern int do_XmCreateCascadeButtonGadget( 
                        int argc,
                        char *argv[]) ;
extern int do_XmCreateComboBox( 
                        int argc,
                        char *argv[]) ;
extern int do_XmCreateCommand( 
                        int argc,
                        char *argv[]) ;
extern int do_XmCreateDialogShell( 
                        int argc,
                        char *argv[]) ;
extern int do_XmCreateDrawingArea( 
                        int argc,
                        char *argv[]) ;
extern int do_XmCreateDrawnButton( 
                        int argc,
                        char *argv[]) ;
extern int do_XmCreateErrorDialog( 
                        int argc,
                        char *argv[]) ;
extern int do_XmCreateFileSelectionBox( 
                        int argc,
                        char *argv[]) ;
extern int do_XmCreateFileSelectionDialog( 
                        int argc,
                        char *argv[]) ;
extern int do_XmCreateForm( 
                        int argc,
                        char *argv[]) ;
extern int do_XmCreateFormDialog( 
                        int argc,
                        char *argv[]) ;
extern int do_XmCreateFrame( 
                        int argc,
                        char *argv[]) ;
extern int do_XmCreateInformationDialog( 
                        int argc,
                        char *argv[]) ;
extern int do_XmCreateLabel( 
                        int argc,
                        char *argv[]) ;
extern int do_XmCreateLabelGadget( 
                        int argc,
                        char *argv[]) ;
extern int do_XmCreateList( 
                        int argc,
                        char *argv[]) ;
extern int do_XmCreateMainWindow( 
                        int argc,
                        char *argv[]) ;
extern int do_XmCreateMenuBar( 
                        int argc,
                        char *argv[]) ;
extern int do_XmCreateMenuShell( 
                        int argc,
                        char *argv[]) ;
extern int do_XmCreateMessageBox( 
                        int argc,
                        char *argv[]) ;
extern int do_XmCreateMessageDialog( 
                        int argc,
                        char *argv[]) ;
extern int do_XmCreateOptionMenu( 
                        int argc,
                        char *argv[]) ;
extern int do_XmCreatePanedWindow( 
                        int argc,
                        char *argv[]) ;
extern int do_XmCreatePopupMenu( 
                        int argc,
                        char *argv[]) ;
extern int do_XmCreatePromptDialog( 
                        int argc,
                        char *argv[]) ;
extern int do_XmCreatePulldownMenu( 
                        int argc,
                        char *argv[]) ;
extern int do_XmCreatePushButton( 
                        int argc,
                        char *argv[]) ;
extern int do_XmCreatePushButtonGadget( 
                        int argc,
                        char *argv[]) ;
extern int do_XmCreateQuestionDialog( 
                        int argc,
                        char *argv[]) ;
extern int do_XmCreateRadioBox( 
                        int argc,
                        char *argv[]) ;
extern int do_XmCreateRowColumn( 
                        int argc,
                        char *argv[]) ;
extern int do_XmCreateScale( 
                        int argc,
                        char *argv[]) ;
extern int do_XmCreateScrollBar( 
                        int argc,
                        char *argv[]) ;
extern int do_XmCreateScrolledList( 
                        int argc,
                        char *argv[]) ;
extern int do_XmCreateScrolledText( 
                        int argc,
                        char *argv[]) ;
extern int do_XmCreateScrolledWindow( 
                        int argc,
                        char *argv[]) ;
extern int do_XmCreateSelectionBox( 
                        int argc,
                        char *argv[]) ;
extern int do_XmCreateSelectionDialog( 
                        int argc,
                        char *argv[]) ;
extern int do_XmCreateSeparator( 
                        int argc,
                        char *argv[]) ;
extern int do_XmCreateSeparatorGadget( 
                        int argc,
                        char *argv[]) ;
extern int do_XmCreateText( 
                        int argc,
                        char *argv[]) ;
extern int do_XmCreateTextField( 
                        int argc,
                        char *argv[]) ;
extern int do_XmCreateToggleButton( 
                        int argc,
                        char *argv[]) ;
extern int do_XmCreateToggleButtonGadget( 
                        int argc,
                        char *argv[]) ;
extern int do_XmCreateWarningDialog( 
                        int argc,
                        char *argv[]) ;
extern int do_XmCreateWorkArea( 
                        int argc,
                        char *argv[]) ;
extern int do_XmCreateWorkingDialog( 
                        int argc,
                        char *argv[]) ;
extern int do_DtCreateHelpDialog( 
                        int argc,
                        char *argv[]) ;
extern int do_DtCreateHelpQuickDialog( 
                        int argc,
                        char *argv[]) ;
extern int do_DtHelpReturnSelectedWidgetId( 
                        int argc,
                        char *argv[]) ;
extern int do_DtHelpSetCatalogName( 
                        int argc,
                        char *argv[]) ;
extern int do_DtHelpQuickDialogGetChild( 
                        int argc,
                        char *argv[]) ;
extern int do_DtCreatePrintSetupBox( 
                        int argc,
                        char *argv[]) ;
extern int do_DtCreatePrintSetupDialog( 
                        int argc,
                        char *argv[]) ;
extern int do_DtCreatePDMJobSetup( 
                        int argc,
                        char *argv[]) ;
extern int do_XmListAddItem( 
                        int argc,
                        char *argv[]) ;
extern int do_XmListAddItemUnselected( 
                        int argc,
                        char *argv[]) ;
extern int do_XmListAddItems( 
                        int argc,
                        char *argv[]) ;
extern int do_XmListAddItemsUnselected( 
                        int argc,
                        char *argv[]) ;
extern int do_XmListReplaceItemsPos( 
                        int argc,
                        char *argv[]) ;
extern int do_XmListReplaceItemsPosUnselected( 
                        int argc,
                        char *argv[]) ;
extern int do_XmListDeleteAllItems( 
                        int argc,
                        char *argv[]) ;
extern int do_XmListDeselectAllItems( 
                        int argc,
                        char *argv[]) ;
extern int do_XmListUpdateSelectedList( 
                        int argc,
                        char *argv[]) ;
extern int do_XmListDeleteItem( 
                        int argc,
                        char *argv[]) ;
extern int do_XmListDeselectItem( 
                        int argc,
                        char *argv[]) ;
extern int do_XmListSetBottomItem( 
                        int argc,
                        char *argv[]) ;
extern int do_XmListSetItem( 
                        int argc,
                        char *argv[]) ;
extern int do_XmListDeletePos( 
                        int argc,
                        char *argv[]) ;
extern int do_XmListDeselectPos( 
                        int argc,
                        char *argv[]) ;
extern int do_XmListSetBottomPos( 
                        int argc,
                        char *argv[]) ;
extern int do_XmListSetHorizPos( 
                        int argc,
                        char *argv[]) ;
extern int do_XmListSetPos( 
                        int argc,
                        char *argv[]) ;
extern int do_XmListDeleteItemsPos( 
                        int argc,
                        char *argv[]) ;
extern int do_XmListDeleteItems( 
                        int argc,
                        char *argv[]) ;
extern int do_XmListDeletePositions( 
                        int argc,
                        char *argv[]) ;
extern int do_XmListGetKbdItemPos( 
                        int argc,
                        char *argv[]) ;
extern int do_XmListGetSelectedPos( 
                        int argc,
                        char *argv[]) ;
extern int do_XmListGetMatchPos( 
                        int argc,
                        char *argv[]) ;
extern int do_XmListItemExists( 
                        int argc,
                        char *argv[]) ;
extern int do_XmListItemPos( 
                        int argc,
                        char *argv[]) ;
extern int do_XmListPosSelected( 
                        int argc,
                        char *argv[]) ;
extern int do_XmListPosToBounds( 
                        int argc,
                        char *argv[]) ;
extern int do_XmListSelectItem( 
                        int argc,
                        char *argv[]) ;
extern int do_XmListSelectPos( 
                        int argc,
                        char *argv[]) ;
extern int do_XmListSetAddMode( 
                        int argc,
                        char *argv[]) ;
extern int do_XmListSetKbdItemPos( 
                        int argc,
                        char *argv[]) ;
extern int do_XmMainWindowSetAreas( 
                        int argc,
                        char *argv[]) ;
extern int do_XmMainWindowSep1( 
                        int argc,
                        char *argv[]) ;
extern int do_XmMainWindowSep2( 
                        int argc,
                        char *argv[]) ;
extern int do_XmMainWindowSep3( 
                        int argc,
                        char *argv[]) ;
extern int do_XmProcessTraversal( 
                        int argc,
                        char *argv[]) ;
extern int do_XmInternAtom( 
                        int argc,
                        char *argv[]) ;
extern int do_XmGetAtomName( 
                        int argc,
                        char *argv[]) ;
extern int do_XmGetColors( 
                        int argc,
                        char *argv[]) ;
extern int do_XmUpdateDisplay( 
                        int argc,
                        char *argv[]) ;
extern int do_XmAddWMProtocols( 
                        int argc,
                        char **argv) ;
extern int do_XmRemoveWMProtocols( 
                        int argc,
                        char **argv) ;
extern int do_XmAddWMProtocolCallback( 
                        int argc,
                        char **argv) ;
extern int do_XmRemoveWMProtocolCallback( 
                        int argc,
                        char **argv) ;
extern int do_XmMenuPosition( 
                        int argc,
                        char *argv[]) ;
extern int do_XmCommandAppendValue( 
                        int argc,
                        char *argv[]) ;
extern int do_XmCommandError( 
                        int argc,
                        char *argv[]) ;
extern int do_XmCommandSetValue( 
                        int argc,
                        char *argv[]) ;
extern int do_XmCommandGetChild( 
                        int argc,
                        char *argv[]) ;
extern int do_XmMessageBoxGetChild( 
                        int argc,
                        char *argv[]) ;
extern int do_XmFileSelectionBoxGetChild( 
                        int argc,
                        char *argv[]) ;
extern int do_XmSelectionBoxGetChild( 
                        int argc,
                        char *argv[]) ;
extern int do_XmIsTraversable( 
                        int argc,
                        char *argv[]) ;
extern int do_XmScaleGetValue( 
                        int argc,
                        char *argv[]) ;
extern int do_XmScaleSetValue( 
                        int argc,
                        char *argv[]) ;
extern int do_XmScrollBarGetValues( 
                        int argc,
                        char *argv[]) ;
extern int do_XmScrollBarSetValues( 
                        int argc,
                        char *argv[]) ;
extern int do_XmScrollVisible( 
                        int argc,
                        char *argv[]) ;
extern int do_XmToggleButtonGetState( 
                        int argc,
                        char *argv[]) ;
extern int do_XmToggleButtonGadgetGetState( 
                        int argc,
                        char *argv[]) ;
extern int do_XmToggleButtonSetState( 
                        int argc,
                        char *argv[]) ;
extern int do_XmToggleButtonGadgetSetState( 
                        int argc,
                        char *argv[]) ;
extern int do_catopen( 
                        int argc,
                        char **argv) ;
extern int do_catclose( 
                        int argc,
                        char **argv) ;
extern int do_catgets( 
                        int argc,
                        char **argv) ;
extern int do_XmTextDisableRedisplay( 
                        int argc,
                        char *argv[]) ;
extern int do_XmTextEnableRedisplay( 
                        int argc,
                        char *argv[]) ;
extern int do_XmTextPaste( 
                        int argc,
                        char *argv[]) ;
extern int do_XmTextGetEditable( 
                        int argc,
                        char *argv[]) ;
extern int do_XmTextRemove( 
                        int argc,
                        char *argv[]) ;
extern int do_XmTextGetTopCharacter( 
                        int argc,
                        char *argv[]) ;
extern int do_XmTextGetBaseline( 
                        int argc,
                        char *argv[]) ;
extern int do_XmTextGetInsertionPosition( 
                        int argc,
                        char *argv[]) ;
extern int do_XmTextGetLastPosition( 
                        int argc,
                        char *argv[]) ;
extern int do_XmTextGetMaxLength( 
                        int argc,
                        char *argv[]) ;
extern int do_XmTextGetSelection( 
                        int argc,
                        char *argv[]) ;
extern int do_XmTextGetString( 
                        int argc,
                        char *argv[]) ;
extern int do_XmTextSetEditable( 
                        int argc,
                        char *argv[]) ;
extern int do_XmTextSetAddMode( 
                        int argc,
                        char *argv[]) ;
extern int do_XmTextScroll( 
                        int argc,
                        char *argv[]) ;
extern int do_XmTextSetInsertionPosition( 
                        int argc,
                        char *argv[]) ;
extern int do_XmTextSetTopCharacter( 
                        int argc,
                        char *argv[]) ;
extern int do_XmTextSetMaxLength( 
                        int argc,
                        char *argv[]) ;
extern int do_XmTextSetString( 
                        int argc,
                        char *argv[]) ;
extern int do_XmTextShowPosition( 
                        int argc,
                        char *argv[]) ;
extern int do_XmTextClearSelection( 
                        int argc,
                        char *argv[]) ;
extern int do_XmTextCopy( 
                        int argc,
                        char *argv[]) ;
extern int do_XmTextCut( 
                        int argc,
                        char *argv[]) ;
extern int do_XmTextGetSelectionPosition( 
                        int argc,
                        char *argv[]) ;
extern int do_XmTextInsert( 
                        int argc,
                        char *argv[]) ;
extern int do_XmTextPosToXY( 
                        int argc,
                        char *argv[]) ;
extern int do_XmTextReplace( 
                        int argc,
                        char *argv[]) ;
extern int do_XmTextSetSelection( 
                        int argc,
                        char *argv[]) ;
extern int do_XmTextXYToPos( 
                        int argc,
                        char *argv[]) ;
extern int do_XmTextSetHighlight( 
                        int argc,
                        char *argv[]) ;
extern int do_XmTextFindString( 
                        int argc,
                        char *argv[]) ;
extern int do_XmOptionButtonGadget( 
                        int argc,
                        char *argv[]) ;
extern int do_XmOptionLabelGadget( 
                        int argc,
                        char *argv[]) ;
extern int do_XmGetVisibility( 
                        int argc,
                        char *argv[]) ;
extern int do_XmGetTearOffControl( 
                        int argc,
                        char *argv[]) ;
extern int do_XmGetTabGroup( 
                        int argc,
                        char *argv[]) ;
extern int do_XmGetPostedFromWidget( 
                        int argc,
                        char *argv[]) ;
extern int do_XmGetFocusWidget( 
                        int argc,
                        char *argv[]) ;
extern int do_XmFileSelectionDoSearch( 
                        int argc,
                        char *argv[]) ;



#endif /* _Dtksh_xmcmds_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
