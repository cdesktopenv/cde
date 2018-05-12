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
 *	$TOG: Editor.hh /main/11 1998/02/03 10:28:56 mgreess $
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

#ifndef EDITOR_H
#define EDITOR_H

#include <Xm/Xm.h>
#include "MenuBar.h"

// Get all the Bento-related API and types (CMContainer, etc.)

#include <DtMail/DtMail.hh>
#include "UIComponent.h"

extern int use_XmTextEditor;

class DtMailEditor;

class Editor : public UIComponent {
    
  public:	
    Editor();
    virtual ~Editor();

    virtual void	initialize() = 0;

    virtual void	set_contents(
				     const char *contents,
				     const unsigned long len
				     ) = 0;

    virtual void	set_contents(const char * path) = 0;

    virtual char *	get_contents() = 0;

    virtual void	append_to_contents(
					   const char *new_contents,
					   const unsigned long len
					   ) = 0;

    virtual void	append_to_contents(const char * path) = 0;

    virtual void	append_at_cursor(const char *path) = 0;
    virtual void	append_at_cursor(
				     const char *contents, 
				     const unsigned long len
				     ) = 0;

    enum InsertFormat {
	IF_NONE,	// No special format
	IF_INDENTED,	// Indented with indent_prefix
	IF_BRACKETED	// Bracketed
    };

    enum BracketFormat {
	BF_NONE,	// No bracketing
	BF_INCLUDE,	// As included message,
	BF_FORWARD	// As forwarded message
	};

    enum HeaderFormat {
	HF_NONE,	// Do not insert headers in message body.
	HF_FULL,	// Insert all headers.
	HF_ABBREV	// Do not insert ignored headers.
	};

    virtual void	append_newline_to_contents();
    virtual DtMailBoolean	set_message(
					DtMail::Message * msg,
					char ** status_string,
					HeaderFormat header_format = HF_ABBREV,
					InsertFormat format = IF_NONE,
					BracketFormat brackets = BF_NONE);

    virtual void		set_attachment(
					DtMail::BodyPart * body_part,
					InsertFormat format = IF_NONE,
					BracketFormat brackets = BF_NONE);

    virtual void	clear_contents() = 0;
    virtual int		no_text()	 = 0;

    virtual Widget	get_editor() = 0;
    virtual Widget	get_text_widget() = 0;
    virtual Pixel	get_text_foreground() = 0;
    virtual Pixel	get_text_background() = 0;
    virtual Dimension   get_text_width() = 0;    
    virtual XmFontList	get_text_fontList() = 0;

    virtual int		get_columns() = 0;
    virtual int		get_rows() = 0;
    virtual void	set_columns(int ncolumns) = 0;
    virtual void	set_editable(Boolean value) = 0;
    virtual void	set_rows(int nrows) = 0;
    virtual void	update_display_from_props(void);

    virtual void        auto_show_cursor_off() = 0;
    virtual void        auto_show_cursor_restore() = 0;

    virtual void	set_to_top() = 0;
    virtual void	set_to_bottom() = 0;

	virtual void    find_change() = 0;
	virtual void    spell() = 0;
	virtual void    format() = 0;
	virtual void    set_word_wrap(Boolean value) = 0;
	virtual void    undo_edit() = 0;
    virtual void	cut_selection() = 0;
    virtual void	copy_selection() = 0;
    virtual void	paste_from_clipboard() = 0;
    virtual void	paste_special_from_clipboard(InsertFormat) = 0;
    virtual void	clear_selection() = 0;
    virtual void	delete_selection() = 0;
    virtual void	select_all() = 0;

    virtual void	disable_redisplay() = 0;
    virtual void	enable_redisplay() = 0;

};

class AbstractEditorParent {
  protected:
	Widget _attachmentPopupMenu;
	Widget _textPopupMenu;
	MenuBar *_menuPopupAtt;
	MenuBar *_menuPopupText;
  public:

    AbstractEditorParent();
    virtual ~AbstractEditorParent();

    virtual     const char *const className() 
				{ return "AbstractEditorParent"; }

    virtual DtMailEditor *  get_editor() = 0;

    // Text/attachment (de)selection methods

    virtual void text_selected()   = 0;
    virtual void text_unselected() = 0;
    virtual void attachment_selected() = 0;
    virtual void all_attachments_deselected() = 0;
    virtual void all_attachments_selected() = 0;
    virtual void add_att(char *) = 0;
    virtual void add_att(char *, DtMailBuffer) = 0;
    virtual void add_att(DtMailBuffer) = 0;

    virtual void postAttachmentPopup(XEvent *event);
    virtual void postTextPopup(XEvent *event);
    
    virtual void addAttachmentActions(
			char **,
			int
		) = 0;
    virtual void invokeAttachmentAction(int) = 0;
    virtual void removeAttachmentActions() = 0;

    virtual void selectAllAttachments() = 0;

    virtual void	showAttachArea() = 0;
    virtual void	hideAttachArea() = 0;

    virtual void	attachmentFeedback(Boolean) = 0;

};

    
#endif // EDITOR_HH
