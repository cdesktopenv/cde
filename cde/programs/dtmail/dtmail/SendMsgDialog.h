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
 * $TOG: SendMsgDialog.h /main/17 1999/03/25 13:43:01 mgreess $
 *
 * RESTRICTED CONFIDENTIAL INFORMATION:
 * 
 * The information in this document is subject to special
 * restrictions in a confidential disclosure agreement between
 * HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
 * document outside HP, IBM, Sun, USL, SCO, or Univel without
 * Sun's specific written approval.  This document and all copies
 * and derivative works thereof must be returned or destroyed at
 * Sun's request.
 *
 * Copyright 1993, 1994, 1995 Sun Microsystems, Inc.  All rights reserved.
 *
 *+ENOTICE
 */

#ifndef SENDMSGDIALOG_H
#define SENDMSGDIALOG_H

//#include "Editor.hh"
//#include "CDEM_CoeEd.hh"
//#include "XmTextEditor.h"

#include "DtMailEditor.hh"
#include "MenuWindow.h"
#include "NoUndoCmd.h"
#include "RoamApp.h"
#include "RoamCmds.h"
#include "RoamMenuWindow.h"
#include "DtMailGenDialog.hh"
#include "DtVirtArray.hh"

class MailBox;
class Cmd;

// This is the Compose window.
class SendMsgDialog : public MenuWindow, public AbstractEditorParent {
  public:
    enum ShowState {
	SMD_ALWAYS,
	SMD_SHOWN,
	SMD_HIDDEN,
	SMD_NEVER
	};
    
    struct HeaderList {
	char  		*label;
	char  		*header;
	char		*value;
	ShowState	show;
	Widget		form_widget;
	Widget		label_widget;
	Widget		field_widget;
	HeaderList(void);
	HeaderList(const HeaderList &);
	~HeaderList(void);
	};

    Widget GetMainWin() { return _main_form; }
    void   setTitle(char *title);
    
  private:
    
    Widget	_main_form;
    DtMailEditor  *_my_editor;
    DtMailGenDialog *_genDialog; 

    Boolean _takeDown;
    Boolean _first_time;
    Boolean _already_sending;  // Flag to make sure we don't send more than once.
    Boolean _show_attach_area;

    int     _confirm_attachment_threshold;
    int     confirm_add_attachment(char*, int);
    int     get_confirm_attachment_threshold(void);
    
    Cmd *_send_button;
    Cmd *_close_button;

    Cmd *_separator;
    
    // File Menu items
    
    UnifiedSelectFileCmd *_file_include;
    SaveAsTextCmd *_file_save_as;
    Cmd *_file_log;
    Cmd *_file_send;
    Cmd *_file_sendAs[10];   // Assume 10 transports.
    Cmd *_file_close;
    
    // Edit Menu items
    
    Cmd *_edit_undo;
    Cmd *_edit_cut;
    Cmd *_edit_copy;
    Cmd *_edit_paste;
    Cmd *_edit_paste_special[2];
    Cmd *_edit_clear;
    Cmd *_edit_delete;
    Cmd *_edit_select_all;

    // Alias Popup Menus
    
    DtVirtArray <PropStringPair *> *_aliasList;
    CmdList *_bccPopupCmdlist;
    Widget   _bccPopupMenu;
    MenuBar *_bccPopupMenuBar;
    CmdList *_ccPopupCmdlist;
    Widget   _ccPopupMenu;
    MenuBar *_ccPopupMenuBar;
    CmdList *_toPopupCmdlist;
    Widget   _toPopupMenu;
    MenuBar *_toPopupMenuBar;
    
    // Text Popup Menu
    //
    // Found in Editor.hh
    //
    //MenuBar	*_menuPopupText;
    //Widget	_textPopupMenu;
    //
    CmdList *_textPopupMenuList;
    
    // Attachment
    Widget   _attachmentMenu;
    CmdList *_attachmentMenuList;
    CmdList *_attachmentPopupMenuList;
    CmdList *_attachmentActionsList;

    SaveAttachCmd  *_att_save;
    UnifiedSelectFileCmd *_att_add;
    Cmd *_att_delete;
    Cmd *_att_undelete;
    Cmd *_att_rename;
    Cmd *_att_select_all;
    Cmd *_att_show_pane;

    void  construct_attachment_popup(void);
    void  construct_text_popup(void);
    
    // Format
    
    Cmd	*_format_word_wrap;
    Cmd	*_format_settings;
    Cmd *_format_find_change;
    Cmd	*_format_spell;
    Cmd *_format_bcc;    // Depends on Properties setting.
    Cmd	*_format_separator;
    Widget	_format_menu;
    CmdList	*_format_cmds;
    CmdList	*_templates;
    Widget	_format_cascade;
    
    Widget _status_form;
    Widget _status_text;

    int _num_sendAs;    // Number of Transports available through Back End.
    int   _template_count; // Number of templates loaded.
    char *_templateList; // names of templates
    
    
    // The header list keeps track of the current headers on the Format
    // menu, as well as their current state in the compose header region.
    DtVirtArray<HeaderList *>	_header_list;
    DtMailBoolean		_headers_changed;
    Widget			_header_form;
    char			*_additionalfields;
    
    // Help
    Cmd *_overview;
    Cmd *_tasks;
    Cmd *_reference;
    Cmd *_on_item;
    Cmd *_using_help;
    Cmd *_about_mailer;

    // This is the internal data structure used for submission.
    // As user adds/deletes attachment, this data structure is update immediately.
    // This data structure is also updated at periodic checkpoints to store the
    // latest Compose window data.
    // Before sending, this is also updated.
    DtMail::Message *_msgHandle;   
    
    // Pointer to the message that is being included as an attachment.
    // When updateMsgHndAtt() is called, data in this handle is copied to _msgHandle.
    DtMail::Message *_inclMsgHandle;
    
    // Pointer to the last Attachment BodyPart so that newBodyPart(error, _lastAttBP)
    // returns a message body part after the last body part.  This is so that
    // attachments will be ordered properly.
    DtMail::BodyPart *_lastAttBP;
    
    // When makeMessage() is called, it creates a message handle with its
    // first body part empty.  If set to TRUE, then this boolean variable
    // indicates that the first body part of _msgHandle has already been used
    // (probably for text).  Otherwise, the first body part can be used for attachment.
    Boolean _firstBPHandled;
    
    // updateMsgHndAtt() uses this Boolean variable to determine whether the
    // current message handle to be copied contains a text body part or not.
    Boolean _inclMsgHasText;
    
    // The auto save interval id is used to stop the interval timer for auto
    // saving.
    //
    XtIntervalId _auto_save_interval;
    char * _auto_save_path;
    char * _auto_save_file;
    
    char * _dead_letter_buf;
    
    DtMailBoolean _log_msg;
    
    void   mkAutoSavePath();
    static void autoSaveCallback(XtPointer, XtIntervalId *id);
    void   doAutoSave(void);
    void   doAutoSave(char *);
    int    getAutoSaveInterval(void);
    
    void   addTemplates(CmdList*); // Create the initial template menu.
    Widget getHeaderWidget(const char *);
    static void   aliasMenuButtonHandler(
			Widget,
			XtPointer client_data,
			XEvent *event,
			Boolean *);
    void   createAliasList(DtVirtArray<PropStringPair*> *aliases);
    void   destroyAliasList(DtVirtArray<PropStringPair*> *aliases);
    Widget createAliasPopupMenu(
			Widget parent,
			MenuBar** menubar,
			CmdList** cmdlist,
			DtVirtArray<PropStringPair*> *aliases);
    void   destroyAliasPopupMenu(
			Widget  parent,
			MenuBar *menubar,
			CmdList *cmdlist,
			Widget  menu);
    void   createAliasPopupMenus();
    void   destroyAliasPopupMenus();
    void   createFormatMenu();
    void   createMenuPanes();
    Widget createWorkArea(Widget);
    void   createHeaders(Widget header);
    void   doDynamicHeaderMenus(void);
    Boolean reservedHeader(const char *label);

    void valueToAddrText(Widget, DtMailValueSeq &);
    
    // File selection Box callbacks 
    static void include_file_cb( void *, char * );
    static void add_att_cb( void *, char * );
    static void save_att_cb( void *, char * );
    
    // Return a message handle -- internal data structure of a mail message.
    DtMail::Message * makeMessage(void);
    // Clear Compose window text fields to prepare for re-use.
    void reset();

    // Find a header in the list by name.
    //
    int lookupHeader(const char * name);
    static void headerValueChanged(Widget, XtPointer, XtPointer);
    void reattachHeaders(void);
    void justifyHeaders(void);
    void forceFormResize(Widget);

    // Called when the sendmail child process completes
    static void sendmailErrorProc (int pid, int status, void *data);
    void popupMemoryError(DtMailEnv &error);

  public:
    SendMsgDialog();
    virtual ~SendMsgDialog();
    virtual const char *const className () { return "SendMsgDialog"; }
    void initialize();
    void send_message( const char *, int );

    void include_file(char *);
    void add_att(char *);
    void add_att(char *, DtMailBuffer);
    void add_att(DtMailBuffer);
    void setMsgHnd();    // initialize _msgHandle.
    void startAutoSave(void);
    void stopAutoSave(void);
    void setLogState(DtMailBoolean state) { _log_msg = state; }
    void propsChanged(void);
    Boolean isMsgValid(void);	// checks to see whether _msgHandle is NULL
    
    void setLastAttBP();
    void setLastAttBP(DtMail::BodyPart *);
    void setFirstBPHandled(Boolean);

    void attachmentFeedback(Boolean);

    // The set/get Header methods manage the dynamic header structures.
    //
    void setHeader(const char * name, const char * value);
    void setHeader(const char * name, DtMailValueSeq & value);
    void getHeader(const char * name, char ** value);
    void resetHeaders(void);
    void setInputFocus(const int mode);

    // This method returns true if the user has edited the headers.
    //
    DtMailBoolean headersChanged(void) { return _headers_changed; }

    // The load/store Headers methods will reload the pane from a message,
    // or store the header pane values to the message. Specifying NULL will
    // load the header pane from the current value for _msgHandle.
    // If load_all is true, then all headers are loaded, and fields are
    // added as necessary to the header pane. Otherwise, the list of shown
    // headers are loaded and all others are ignored.
    //
    // storeHeaders transfers the currently stored headers to the specified
    // message object, or the internal message if none is specified.
    //
    void loadHeaders(DtMail::Message * msg = NULL,
		     DtMailBoolean load_all = DTM_FALSE);
    void storeHeaders(DtMail::Message * msg = NULL);

    // Check if a message has addressees.  If a message is Sent and
    // it has no addressees, we need to barf.

    Boolean hasAddressee();

    // The changeHeaderState method toggles the dynamic headers between
    // shown an hidden. It will also update the menu label accordingly.
    //
    void changeHeaderState(const char * name);

    // unfilled_headers returns TRUE if no header has a value.
    // FaLSE if any header has a value

    Boolean unfilled_headers();

    // The set/clear Status methods will set and clear the status line.
    //
    void setStatus(const char * str);
    void clearStatus(void);

    // XSMP support
    static void		restoreSession(char*);
    virtual int		smpSaveSessionGlobal();
    virtual void	smpSaveSessionLocal();
    
    // These are public so that the check point routine can call
    // updateMsgHnd, and include/forward routines can call updateMsgHndAtt.
    void updateMsgHnd();
    void updateMsgHndAtt();
    
    char *text();
    
    DtMailEditor* get_editor() {return _my_editor; }
    
    // Mutators
    
    void setInclMsgHnd(DtMail::Message *, Boolean);
    void text( const char * );
    void append( const char * );
    void quit(Boolean delete_win = FALSE);
    void panicQuit();

    // Method to check if self has content in it.
    Boolean checkDirty();
    Boolean handleQuitDialog();
    void    goAway(Boolean);

    // Add the specified file (first parameter) as attachment.
    void inclAsAttmt( char *, char *);
    // Add the content of the buffer as attachment.
    void inclAsAttmt( unsigned char *, int, char *);
    // Parse the buffer and fill the Compose window with data.
    void parseNplace( char *, int );
    // Parse the file and fill the Compose window with data.
    void parseNplace(const char * path);
    
    // Load the dead letter file.
    void loadDeadLetter(const char * path);
    
    // SR - Text-selection callbacks.
    
    virtual void text_selected();
    virtual void text_unselected();
    void attachment_selected();
    void all_attachments_deselected();
    void all_attachments_selected();
    void selectAllAttachments(); 
    
    
    DtMailGenDialog *genDialog() { return _genDialog; }  
    
    void showAttachArea();
    void hideAttachArea();
    
    void activate_default_attach_menu();
    void deactivate_default_attach_menu();
    
    // Activate and deactivate paste stuff
    void activate_edit_paste() { _edit_paste->activate(); }
    void activate_edit_paste_indented() {_edit_paste_special[0]->activate();}
    void activate_edit_paste_bracketed() {_edit_paste_special[1]->activate();}
    
    void delete_selected_attachments();
    
    void undelete_last_deleted_attachment();
    
    void save_selected_attachment(char *);
    void save_selected_msg_text(char *);
    
    Boolean renameAttachmentOK();
    
    void addAttachmentActions(char **, int);
    void removeAttachmentActions();
    void invokeAttachmentAction(int);   

    virtual void manage();
    virtual void unmanage();

};


// Manager of all compose windows.
class Compose {
    
  public:
    Compose();
    virtual ~Compose();
    virtual const char *const className () { return "Compose"; }

    SendMsgDialog	*getWin();
    SendMsgDialog	*getUnusedWin();
    XtIntervalId	 getTimeOutId() { return _timeout_id; }
    int			 numCreatedWindows() { return _num_created; }
    int			 numUnusedWindows() { return _not_in_use; }
    void		 putWin(SendMsgDialog*, Boolean);
    void		 putTimeOutId(XtIntervalId id) { _timeout_id = id; }
    void		 Self_destruct(XtPointer, XtIntervalId*);
  
    
  private:
    struct Compose_Win
    {
	SendMsgDialog *win;
	struct Compose_Win *next;
	Boolean in_use;
    };

    Compose_Win	*_compose_head;	// List of compose windows.
    int		 _not_in_use;	// Number of unused compose windows.
    int		 _num_created;	// Total number of compose windows created.
    XtIntervalId _timeout_id;   // Self destruct id.
};

extern Compose theCompose;

#ifdef DTMAIL_TOOLTALK
extern int started_by_tt;
#endif

#endif


