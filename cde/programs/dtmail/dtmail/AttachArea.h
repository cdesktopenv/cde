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
 *	$XConsortium: AttachArea.h /main/3 1995/11/06 16:04:04 rswiston $
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

#ifndef ATTACHAREA_H
#define ATTACHAREA_H

#ifndef I_HAVE_NO_IDENT
#endif

#define TYPETEXT 0		/* unformatted text */
#define TYPEMULTIPART 1		/* multiple part */
#define TYPEMESSAGE 2		/* encapsulated message */
#define TYPEAPPLICATION 3	/* application data */
#define TYPEAUDIO 4		/* audio */
#define TYPEIMAGE 5		/* static image */
#define TYPEVIDEO 6		/* video */
#define TYPEOTHER 7		/* unknown */

#include <Xm/Xm.h>
#include "UIComponent.h"
#include "UndoCmd.h"
#include "CmdList.h"
#include "MenuBar.h"
#include "DtMailHelp.hh"

#include <DtMail/DtMailError.hh>
#include <DtMail/DtMail.hh>

typedef enum { NOTSET, ADD, SAVEAS } FSState;

class Attachment;
class DtMailEditor;

class RoamMenuWindow;
class SendMsgDialog;
class ViewMsgDialog;

class AttachArea : public UIComponent {

  private:

    enum   AAreaSState {AA_SEL_NONE=0, AA_SEL_SINGLE=1, AA_SEL_ALL=2};
    static void inputCallback( Widget, XtPointer, XtPointer );
    static void MenuButtonHandler(Widget, XtPointer, XEvent *, Boolean *); 
    static void resizeCallback( Widget, XtPointer, XtPointer );

    void   popupAttachmentMenu(XEvent *event);

    void resize(Dimension);	// Called when the user click a button
    unsigned int 	  _iconCount;
    unsigned int   	  _iconSelectedCount;
    unsigned int   	  _deleteCount;
    Attachment      **_attachmentList;// The list of attachments

    FSState      	  _fsState;	// State of the File Selection Box
    Widget       	  _fsDialog;
    Widget       	  _renameDialog;
    Widget       	  _descriptionDialog;
    Widget       	  _no_attachments_label;
    Widget       	  _attachments_label;
    Widget       	  _size_attachments_label;
    Widget       	  _no_selected_label;
    Widget       	  _selected_label;
    DtMailEditor	  *_myOwner;		// The class that owns it
    Widget		  _parent;		// The parent widget
    Widget		rowOfAttachmentsStatus;
    Widget		_attachments_status;
    Widget		_attachments_summary;
    RoamMenuWindow	*_myRMW;
    ViewMsgDialog	*_myVMD;
    SendMsgDialog	*_mySMD;
    Boolean		_pendingAction;
    int			_numPendingActions;

		// Selection State of Attach Area
    AAreaSState		_attach_area_selection_state; 
    Attachment		*_cache_single_attachment; // single selected attachment 
    DtMail::MailBox		  *_mailbox;
    Dimension          _attachAreaWidth;	// The width of the AA
    Dimension          _attachAreaHeight;	// The height of the AA
    void	   valueChanged( XtPointer );
    static void	   valueChangedCallback( Widget, XtPointer, XtPointer );
    void		   dragSlider( XtPointer );
    static void 	   dragCallback( Widget, XtPointer, XtPointer );
    unsigned int       _lastRow;		// The current last row
    unsigned int       _currentRow;		// The current row
    unsigned int	   _attachmentsSize;	// Size of all attachments
    unsigned int	   _selectedAttachmentsSize;	// Size of all 
    XtPointer	   _clientData;
			// selected attachments
    char *		    calcKbytes(unsigned int);

  protected:

    Widget		 _rc;	// The RowColumn widget that manages 
				// the attachments
    Widget		 _sw;	// The ScrolledWindow widget
    Widget		 _vsb;	// The vertical scrollbar of the ScrolledWindow
    Widget		 _message;
    Widget		 _clipWindow;
    Cmd		*_open;
    Cmd		*_saveas;
    Cmd		*_selectall;
    Cmd		*_unselectall;
    MenuBar		*_menuBar;
    Pixel		_background;
    Pixel		_foreground;
    Pixel		_appBackground;
    Pixel		_appForeground;
    CmdList*	_fileCmdList;
    CmdList*	_editCmdList;
    Widget       	_size_selected_label;
    Widget       	_format_button;

  public:
    AttachArea (Widget, DtMailEditor*, char *);
    virtual 		~AttachArea();

    void		initialize();
    void		addToRowOfAttachmentsStatus();
    void		attachment_summary(int, int);
	// Inline functions
    Cmd* 			openCmd()	{ return ( _open ); }
    Widget 			getVerticalSB() { return ( _vsb ); }
    unsigned int 		getIconCount()	{ return ( _iconCount ); }
    unsigned int		getUndeletedIconCount()
					{ return (_iconCount - _deleteCount); }
    unsigned int 		getIconSelectedCount()
					{ return ( _iconSelectedCount ); }

    unsigned int 		getDeleteCount(){ return ( _deleteCount ); }
    void 			incIconCount()	{ _iconCount++; }
    void 			decIconCount()	{ _iconCount--; }
    void 			incIconSelectedCount()
					{ _iconSelectedCount++; }
    void 			decIconSelectedCount()
						{ _iconSelectedCount--; }
    void 			incDeleteCount(){ _deleteCount++; }
    void 			decDeleteCount(){ _deleteCount--; }
    Attachment**		getList() { return ( _attachmentList ); }

    FSState      		getFsState()	{ return (_fsState ); }
    void         		setFsState( FSState state )
						{ ( _fsState = state ); }
    void         		setFsDialog( Widget w )
						{  _fsDialog = w; }
    void         		setRenameDialog( Widget w )
						{  _renameDialog = w; }
    void         		setDescriptionDialog( Widget w )
						{  _descriptionDialog = w; }
    Widget       		getFsDialog()	{ return (_fsDialog ); }
    Widget       		getRenameDialog()  { return (_renameDialog ); }
  
    Widget       		getDescriptionDialog()	
					{ return (_descriptionDialog );}
    virtual const char *const className()	{ return ( "AttachArea" ); }


    void 			addToList( Attachment * );
    void			addToDeletedList( Attachment *);

#ifdef DEAD_WOOD
    void			undeleteAllDeletedAttachments(DtMailEnv &);
#endif /* DEAD_WOOD */
    void			undeleteLastDeletedAttachment(DtMailEnv &);

    void			removeLastDeletedAttachment();

    virtual void		attachmentFeedback(Boolean);

    virtual void 		activateDeactivate();
    Widget      		getRc()	{ return (_rc ); }
    Widget      		getClipWindow()	{ return (_clipWindow ); }
    Widget      		getSWWindow()	{ return (_sw ); }
    Pixel      		getBackground()	{ return (_background ); }
    Pixel      		getForeground()	{ return (_foreground ); }
    Pixel      		getAppBackground(){ return (_appBackground ); }
    Pixel      		getAppForeground(){ return (_appForeground ); }
    Dimension		getAAWidth()	{ return (_attachAreaWidth); }
    Dimension		getAAHeight()	{ return (_attachAreaHeight); }
    DtMailEditor*	owner()   { return ( _myOwner ); }
    
    Attachment*		addAttachment(
				      DtMail::Message *, 
				      DtMail::BodyPart *, 
				      char *filename,
				      char *
				      );

    Attachment*		addAttachment(DtMail::Message *,
					DtMail::BodyPart *,
					String,
					DtMailBuffer);

    Attachment*		addAttachment(String name, DtMail::BodyPart *);

    void		addAttachmentActions(
				    char **,
				    int
			   );
    DtMail::MailBox*	get_mailbox() 	{ return ( _mailbox ); }
    String      	getRenameMessageString()
				{ return ("RenameMessageString"); }
    String      	getDescriptionMessageString()
				{ return ("DescriptionMessageString"); }
    unsigned int 	getAttachmentsSize()
					{ return ( _attachmentsSize ); }
    unsigned int 	getSelectedAttachmentsSize()
				{ return ( _selectedAttachmentsSize ); }
    void         	setAttachmentsSize( unsigned int size )
					{ ( _attachmentsSize = size ); }
    void         	setSelectedAttachmentsSize( unsigned int size )
				{ ( _selectedAttachmentsSize = size ); }

    void		selectAllAttachments();

    Attachment*		getSelectedAttachment();
    void		unselectOtherSelectedAttachments(Attachment *);
#ifdef DEAD_WOOD
    void			deleteAttachments();
#endif /* DEAD_WOOD */
    void 			manageList();
    void 			CalcSizeOfAttachPane( );
#ifdef DEAD_WOOD
    void 			CalcAttachmentPosition(Attachment *);
#endif /* DEAD_WOOD */
    void			CalcAllAttachmentPositions();
    void			DisplayAttachmentsInRow(unsigned int);
    void			calculate_attachment_position(
					Attachment *, 
					Attachment *);
    void			CalcLastRow();
    unsigned int		getLastRow()	{ return ( _lastRow); }
    unsigned int		getCurrentRow()	{ return ( _currentRow); }
    void			SetScrollBarSize(unsigned int );
    void			AdjustCurrentRow();
    void 			initialize(Widget);
    void 			initialize_send(Widget);
    void 			initialize_view(Widget);
#ifdef DEAD_WOOD
    void			setAttachmentsLabel( );
#endif /* DEAD_WOOD */
    int				getSelectedIconCount();
#ifdef DEAD_WOOD
    void			CalcAttachmentsSize( );
    void			add_attachment( Attachment * );
#endif /* DEAD_WOOD */

    // Methods for parsing the attachments in a DtMail::Message

    void		parseAttachments(
				       DtMailEnv &,
				       DtMail::Message *, 
					   Boolean, 
				       int);

    // SR -- Added methods below

    void		attachmentSelected(Attachment *);
    void		manage();
    void		unmanage();
    void		removeCurrentAttachments();
    void		clearAttachArea();
#ifdef DEAD_WOOD
    void		saveAttachmentToFile(DtMailEnv &, char *);
#endif /* DEAD_WOOD */
    void		deleteSelectedAttachments(DtMailEnv &);
    Widget		ownerShellWidget();
    void		setOwnerShell(RoamMenuWindow *);
    void		setOwnerShell(ViewMsgDialog  *);
    void		setOwnerShell(SendMsgDialog  *);
    Boolean		isOwnerShellEditable();

    void		setPendingAction(Boolean);
    void		resetPendingAction();
    int			getNumPendingActions();

    XmString		getSelectedAttachName();
    void		setSelectedAttachName(XmString );

    int			handleQuestionDialog(char *title, 
						char *buf,
						char *helpId = DTMAILHELPERROR);

    int			handleErrorDialog(char *title, 
					    char *buf, 
					    char *helpId = DTMAILHELPERROR);
};
#endif

