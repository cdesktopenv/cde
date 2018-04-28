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
 *	$TOG: RoamMenuWindow.h /main/29 1999/02/03 18:01:26 mgreess $
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

#ifndef ROAMMENUWINDOW_H
#define ROAMMENUWINDOW_H

//-----------------------------------------------------------------------------
    // Roam window for viewing messages
//-----------------------------------------------------------------------------

#include <DtMail/DtMail.hh>

#include "MenuWindow.h"
#include "MenuBar.h"
#include "DtMailGenDialog.hh"
#include "NoOpCmd.h"
#include "QuitCmd.h"
#include "UndoCmd.h"
#include "IconifyCmd.h"
#include "CmdList.h"
#include "RoamCmds.h"
#include "Sort.hh"
#include "ViewMsgDialog.h"
#include <DtMail/DtMailError.hh>
#include "MsgScrollingList.hh"
#include "AttachArea.h"
#include "FindDialog.h"
#include "Editor.hh"

//#include "CDEM_CoeEd.hh"

#include "DtMailEditor.hh"

#include "Undelete.hh"

/*  ROMENU_*       msgid 400 - 499
 */
#define ROMENU_NOINIT    400
#define ROMENU_NEWM      401
#define ROMENU_STAT      402
#define ROMENU_SUM       403
#define ROMENU_SEL       404
#define ROMENU_NOOPEN    405
#define ROMENU_NOCREAT   406
#define ROMENU_NOCRT     407
#define ROMENU_CREAT     408
#define ROMENU_LOAD      409
#define ROMENU_INFO      410
#define ROMENU_EMPTYC    411
#define ROMENU_NEXT      412
#define ROMENU_PREV      413
#define ROMENU_PRT       414
#define ROMENU_MVCP      415
#define ROMENU_SENDER    416
#define ROMENU_SUB       417
#define ROMENU_DATE      418
#define ROMENU_SIZE      419
#define ROMENU_OPENBOX   420
#define ROMENU_NEW       421
#define ROMENU_OPEN      422
#define ROMENU_EMPTY     423
#define ROMENU_SAVEAS    424
#define ROMENU_CLOSE     425
#define ROMENU_OPMSG     426
#define ROMENU_SAVEMSG   427
#define ROMENU_PRINT     428
#define ROMENU_DEL       429
#define ROMENU_LAST      430
#define ROMENU_LIST      431
#define ROMENU_UNDEL     432
#define ROMENU_MSG       433
#define ROMENU_UNDMSG    434
#define ROMENU_CPTO      435
#define ROMENU_COPY      436
#define ROMENU_SELALL    437
#define ROMENU_PROP      438
#define ROMENU_EDIT      439
#define ROMENU_FDMSG     440
#define ROMENU_FULL      441
#define ROMENU_ABB       442
#define ROMENU_BYDATE    443
#define ROMENU_BYSEND    444
#define ROMENU_BYSUB     445
#define ROMENU_BYSIZE    446
#define ROMENU_BYSTAT    447
#define ROMENU_VIEW      448
#define ROMENU_HDR       449
#define ROMENU_NEWMSG    450
#define ROMENU_NEWINCL   451
#define ROMENU_FORWARD   452
#define ROMENU_REPLY     453
#define ROMENU_RPYALL    454
#define ROMENU_RPYINCL   455
#define ROMENU_RPYAINCL  456
#define ROMENU_COMP      457
#define ROMENU_VAC       458
#define ROMENU_ONITEM    459
#define ROMENU_ONAPP     460
#define ROMENU_ONVER     461
#define ROMENU_HELP      462
#define ROMENU_LOADING   463
#define ROMENU_CON       464
#define ROMENU_DELMSG    465
#define ROMENU_NOEMPTY   466
#define ROMENU_EMPTY_CONT     467
#define ROMENU_EMPTY_NOFILE   468
#define ROMENU_MOVE	469
#define ROMENU_RELNOTES	470
    
class RoamMenuWindow : public MenuWindow, public AbstractEditorParent
{
  public:	
    		RoamMenuWindow(char*);
    virtual	~RoamMenuWindow();  
    virtual void initialize();
    virtual void manage();

    void	Full(Boolean);
    Widget	GetMainWin() { return _main; }
    		// Popup Menu Event Handler
    static void	MenuButtonHandler(Widget, XtPointer, XEvent*, Boolean*);
    static void	ShowErrMsg(char*, Boolean, void*);

    void	attachmentFeedback(Boolean);
    void	checkForMail(DtMailEnv &error)
		  { _we_called_newmail = TRUE; _mailbox->checkForMail(error); }
    virtual const char *const
		className() { return "RoamMenuWindow"; }
    void	create_new_container(char*);
    Widget	getDragIcon(Widget widget);
    void	load_mailbox(DtMailEnv &mail_error);
    static XtActionProc
		msgListDragStart(Widget, XEvent*, String*, Cardinal*);
    		// DND: Drop Site
    static void msgListConvertCallback( Widget, XtPointer, XtPointer);
    void	msgListDropRegister();
    void	msgListDropEnable();
    void	msgListDropDisable();
    static void msgListDragFinishCallback(Widget, XtPointer, XtPointer);
    void	msgListDragSetup();
    static Bool msgListLookForButton(Display*, XEvent*, XPointer);
    static void msgListProcessPress(Widget, XEvent*, String*, Cardinal*);
    static void	msgListTransferCallback(Widget, XtPointer, XtPointer);
    void	open(DtMailEnv &, DtMailBoolean, DtMailBoolean);
    void	open_and_load(DtMailEnv &, DtMailBoolean, DtMailBoolean);
    virtual void panicQuit();
    void	postErrorDialog(DtMailEnv&);
    void	propsChanged(void);
    virtual void quit(Boolean delete_win = FALSE);
    void	quit_silently();
    void	reopen_mail_file();
    void	removeVacationTitle(void);
    void	setVacationTitle(void);
    void	setTitle(char *suffix);
    void 	startAutoSave();
    void 	stopAutoSave();
    void 	sync_work_area_size();
    Boolean	vacation();
    void	view_mail_file(char*, DtMailBoolean create = DTM_FALSE);

	
//-------------------------------------------------------------------------
//		Accessors
//-------------------------------------------------------------------------

    DtMailEditor	*get_editor() { return(_my_editor); }
    ViewMsgDialog	*msgView()
      			{
		    	  if ( _numDialogs==0 )
	                    return 0;
	                  else
	                    return _dialogs[_numDialogs-1];
			}
    FindDialog		*get_find_dialog();
    DtMail::MailBox	*mailbox()	{ return _mailbox; }
    MenuBar		*menuBar()	{ return _menuBar; }
    MsgScrollingList	*list()		{ return _list; }
    DtMailGenDialog	*genDialog()	{ return _genDialog; }
    Widget		workArea()	{ return _workArea;} 
    Boolean		fullHeader()	{ return _full_header_resource; }
    char 		*mailboxName()	{ return _mailbox_name; }  
    unsigned int	x()		{ return _x; }
    unsigned int	y()		{ return _y; }
    unsigned int	width()		{ return _width; }
    unsigned int	height()	{ return _height; }
    char		*mailbox_fullpath() { return _mailbox_fullpath; }
	
//-----------------------------------------------------------------------
	    // Mutators
//-----------------------------------------------------------------------
	    
    void	addToRowOfButtons();
    void  	addToRowOfLabels(MsgScrollingList *msglist);
    void  	addToRowOfMessageStatus();

    void	clearStatus(void);
    void	forwardFilename(char*);
    char 	*forwardFilename();
    void	fullHeader(Boolean flag);
    ViewMsgDialog *ifViewExists(DtMailMessageHandle msg_num);
    Boolean	inbox();
    SortBy	last_sorted_by(void) { return _last_sorted_by; }
    void	last_sorted_by(SortBy type);
    void	message( char * );
    void	message_summary();
    void	message_summary(int msgn, int nmsgs, int nnew, int ndeleted);
    void	message_selected(int msgn, int nmsgs, int nnew, int ndeleted);
    void	mailboxName( char *name)
		{
		  if (NULL != _mailbox_name) free((void*)_mailbox_name);
		  _mailbox_name=strdup( name );
		}
    void	mailboxFullpath( char *path)
		{
		  if (NULL != _mailbox_fullpath) free((void*)_mailbox_fullpath);
		  _mailbox_fullpath=strdup( path );
		}
    void	registerDialog(ViewMsgDialog*);
    void	setStatus(const char *);
    void	set_find_dialog(FindDialog *dialog) { _findDialog = dialog; }
    void	showMessageFullHeaders(Boolean);
    void	unregisterDialog(ViewMsgDialog*);  
    
    void	construct_file_menu();
    void	construct_message_menu();
    void	construct_edit_menu();
    void	construct_view_menu();
    void	construct_compose_menu();
    void	construct_move_menu();
    void	construct_attachment_menu();
    void	construct_attachment_popup(void);
    void	construct_text_popup(void);
    void	construct_message_popup();
    void	construct_help_menu();

    UndelFromListDialog
		*get_undel_dialog() {return _msg_undelete_from_list->dialog();}
    DtMail::MailRc
		*get_mail_rc();
    void	clear_message();
    int		inList(char *filename, DtVirtArray<ContainerMenuCmd *> *);
    void	addToCachedContainerList(char *filename);
    void	syncCachedContainerList();
    void	newMailIndicators(void);
    void	expunge(void);
    int		queryExpunge(void);
    Boolean	requiredConversion()	{return _required_conversion;}
    
    virtual void text_selected();
    virtual void text_unselected();

    void	attachment_selected();
    void	all_attachments_deselected();
    void	all_attachments_selected();
    void	selectAllAttachments();
    void	add_att(char *) { ; }
    void	add_att(char *, DtMailBuffer)	{ ; }
    void	add_att(DtMailBuffer)		{ ; }
    void	activate_default_attach_menu();
    void	deactivate_default_attach_menu();
    void	activate_default_message_menu();
    void	deactivate_default_message_menu();	
    void	showAttachArea();
    void	hideAttachArea();
    void	addAttachmentActions(char **, int);
    void	removeAttachmentActions();
    void	invokeAttachmentAction(int);
    void	save_selected_attachment(char *);
    int		showConversionStatus(int, int);
    void	conversionFinished();
    void	convert(char *, char *);
    void	resetCacheList(int new_size);

    // syncViewAndStore() does the sync-ing of the view of a mail
    // container and the storage of that container.
    // E.g., this callback and therefore the method gets invoked every 
    // time a message gets expunged by the back end based on "timed delete".
    // 
    // The method needs to then remove the expunged message from the
    // deleted messages list, thereby syncing the view to be always
    // current with the storage.
    // Similarly, the method also gets invoked when the container store
    // has received new mail.  The view then needs to be updated....

    DtMailBoolean syncViewAndStore(DtMailCallbackOp, const char*, va_list);

    // Static public methods now.
    static DtMailBoolean syncViewAndStoreCallback(
			    DtMailCallbackOp op,
			    const char * path,
			    const char * prompt_hint,
			    void * client_data,
			    ...);
    static int		ConvertStatusCB(int current, int total, void *);
    virtual void	postMsgsPopup(XEvent *event);
    inline Boolean	IsLoaded ()	{ return _is_loaded; }

    // XSMP support
    static RoamMenuWindow
			*restoreSession(char*);
    virtual int		smpSaveSessionGlobal(void);
    virtual void	smpSaveSessionLocal(void);

 protected:
	
    Boolean		 _checkformail_when_mapped;
    Boolean		 _delete_on_quit;
    char 		*_forward_filename;
    Boolean		 _full_header_resource;
    SortBy		 _last_sorted_by;
    char		*_mail_files_resource;
    char		*_mailbox_fullpath;
    char		*_mailbox_name;
    char		*_mailbox_name_resource;
    Boolean 		 _required_conversion;
    Boolean		 _we_called_newmail;
    
    ViewMsgDialog	**_dialogs;
    int			 _numDialogs;
    FindDialog		*_findDialog;
    
    //
    // ContainerList
    //
    int			 _display_cached_list;
    char 		*_filemenu2;
    int			 _first_cached_item;
    int			 _max_cached_list_size;
    DtVirtArray<ContainerMenuCmd *>	*_user_containerlist;
    DtVirtArray<ContainerMenuCmd *>	*_cached_containerlist;

    //
    // File Menu
    //
    CmdList		*_file_cmdlist;
    Cmd			*_file_separator;
    CheckForNewMailCmd	*_file_check_new_mail;
    Cmd			*_file_open_inbox;
    UnifiedSelectMailboxCmd *_file_new_container;
    UnifiedSelectMailboxCmd *_file_open;
    Cmd			*_file_destroy_deleted_msgs;
    Cmd			*_file_quit;
    Widget		 _file_cascade;
    
    //
    // Open Cascade Menu
    //
    CmdList		*_open_container_cmdlist;
    Cmd			*_open_container_separator;
    Cmd			*_open_container_inbox;
    UnifiedSelectMailboxCmd
			*_open_container_other;

    DtVirtArray<ContainerMenuCmd *> *_open_container_containerlist;
    DtVirtArray<ContainerMenuCmd *> *_open_container_containerlist_cached;
    Widget		_opencontainerMenu;
    
    //
    // Message Menu
    //
    CmdList		*_msg_cmdlist;
    Cmd			*_msg_separator;
    Cmd			*_msg_open;
    Cmd			*_msg_save_as;
    Cmd			*_msg_print;
    Cmd			*_msg_find;
    Cmd			*_msg_select_all;
    Cmd			*_msg_delete;
    Cmd			*_msg_undelete_last;
    UndeleteCmd		*_msg_undelete_from_list;
    
    //
    // CopyTo Cascade Menu
    //
    CmdList		*_copyto_cmdlist;
    Cmd			*_copyto_separator;
    CopyToInboxCmd	*_copyto_inbox;
    Cmd			*_copyto_other;

    DtVirtArray<ContainerMenuCmd *> *_copyto_containerlist;
    DtVirtArray<ContainerMenuCmd *> *_copyto_containerlist_cached;
    Widget		_copytoMenu;
    Widget		_message_cascade;
    
    //
    // Edit Menu
    //
    CmdList		*_edit_cmdlist;
    Cmd			*_edit_copy;
    Cmd			*_edit_select_all;
    
    //
    // Attachments Menu  
    //
    CmdList		*_att_cmdlist;
    Cmd			*_att_save;
    Cmd			*_att_select_all;
    
    //
    // View Menu  
    //
    CmdList		*_view_cmdlist;
    Cmd			*_view_separator;
    Cmd			*_view_next;
    Cmd			*_view_previous;
    Cmd			*_view_abbrev_headers;
    Cmd			*_view_sortTD;
    Cmd			*_view_sortSender;	
    Cmd			*_view_sortSubject;	
    Cmd			*_view_sortSize;	
    Cmd			*_view_sortStatus;
    
    //
    // Compose Menu
    //
    CmdList		*_comp_cmdlist;
    Cmd			*_comp_separator;
    Cmd			*_comp_new;
    Cmd			*_comp_new_include;
    Cmd			*_comp_forward;
    Cmd			*_comp_replySender;
    Cmd			*_comp_replyAll;
    Cmd			*_comp_replySinclude;
    Cmd			*_comp_replyAinclude;
    
    //
    // Move Menu
    //
    CmdList		*_move_cmdlist;
    Cmd			*_move_separator;
    MoveToInboxCmd	*_move_inbox;
    Cmd			*_move_other;

    DtVirtArray<ContainerMenuCmd *> *_move_containerlist;
    DtVirtArray<ContainerMenuCmd *> *_move_containerlist_cached;
    Widget		_moveMenu;
    Widget		_move_cascade;

    //
    // Help Menu
    //
    CmdList		*_help_cmdlist;
    Cmd			*_help_separator;
    Cmd			*_help_overview;
    Cmd			*_help_tasks;
    Cmd			*_help_reference;
    Cmd			*_help_on_item;
    Cmd			*_help_using_help;
    Cmd			*_help_about_mailer;
    
    //
    // Message Popup
    //
    CmdList		*_msgsPopup_cmdlist;
    Cmd			*_msgsPopup_separator;

    MenuBar		*_menuPopupMsgs;
    Widget  		_msgsPopupMenu;
    Widget		_msgsPopupMoveMenu;

    //
    // Text Popup
    //
    CmdList		*_textPopup_cmdlist;
    Cmd			*_textPopup_separator;

    //
    // Attachments Popup
    //
    CmdList		*_attPopup_cmdlist;
    Cmd			*_attPopup_separator;
    CmdList		*_attActions_cmdlist;

    Widget		_attachmentMenu;

    //
    // Message filing interface
    //
    ConvertContainerCmd *_convertContainerCmd;
    OpenContainerCmd    *_openContainerCmd;
    
    //
    // This is the set of buttons below the scrolling list
    // They get used in the addToRowOfButtons() method
    //
    DeleteCmd		*_delete_button;
    Cmd			*_next_button;
    Cmd			*_previous_button;
    Cmd			*_replySender_button;
    Cmd			*_print_button;
    Cmd			*_move_copy_button;

    
    DtMail::MailBox	*_mailbox;
    Boolean		_inbox;
    MsgScrollingList	*_list;
    DtMailGenDialog	*_genDialog;
    
    Widget	createWorkArea ( Widget );
    void	createMenuPanes();
    void	createContainerList();
    void	createOpenContainerList(CmdList *);
    void	createCopyList(CmdList *);
    void	configurenotify(
		    unsigned int,
		    unsigned int,
		    unsigned int, 
		    unsigned int,
		    unsigned int);
    void	mapnotify();

    
  private:
    
    static XtResource	_resources[];
    static char		*_supported[];

    Pixmap		_mbox_image;
    Pixmap		_mbox_mask;

    unsigned int	_x;
    unsigned int	_y;
    unsigned int	_width;
    unsigned int	_height;
    unsigned int	_border_width;
    Dimension		_workarea_width;
    Dimension		_workarea_height;
    
    DtMailEditor	*_my_editor;
    Widget		_message;
    Widget		_message_summary;
    Widget		_rowOfLabels;
    Widget		_rowOfButtons;
    Widget		_rowOfMessageStatus;
    XtWorkProcId	_quitWorkprocID;
    Boolean		_clear_message_p;
    Boolean		_create_mailbox_file;
    Boolean		_is_loaded;
    Boolean		_open_create_flag;
    Boolean		_open_lock_flag;

    //
    // Message header list column labels
    //
    Widget		_sender_lbl;
    Widget		_subject_lbl;
    Widget		_date_lbl;
    Widget		_size_lbl;

    XmString		_sender_xms;
    XmString		_subject_xms;
    XmString		_date_xms;
    XmString		_size_xms;

    XmString		_sender_key_xms;
    XmString		_subject_key_xms;
    XmString		_date_key_xms;
    XmString		_size_key_xms;

    static void	ownselectionCallback(Widget, XtPointer, XtPointer );
    static void structurenotify(Widget, XtPointer, XEvent *, Boolean *);
    static void file_selection_callback(void *, char * );
    static void create_container_callback(void *, char * );
    static void move_callback(void *, char *);
    static void copy_callback(void *, char *);
    static void save_attachment_callback(void *, char *);    
    static void delete_attachment_callback(void *, char *);    
    static void conversionFinishedCallback(
			RoamInterruptibleCmd *,
			Boolean,
			void *);
    static void map_menu( Widget, XtPointer, XtPointer );
    static Boolean quitWorkproc(XtPointer);
};

#endif
