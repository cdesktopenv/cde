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
 *	$TOG: DtEditor.hh /main/9 1998/02/03 10:28:23 mgreess $
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

#ifndef DTEDITOR_HH
#define DTEDITOR_HH

// Include the DtWidgetEditor .h file given by HP.

#include <Dt/Editor.h>

#include "Editor.hh"
#include "DtMailEditor.hh"

class CDEM_DtWidgetEditor : public Editor
{

  public:
    CDEM_DtWidgetEditor(
		Widget parent, 
		DtMailEditor *owner_of_editor);
    ~CDEM_DtWidgetEditor();
    
	// Pure virtual functions of class Editor
    //
    virtual void	initialize();

    virtual void	set_contents(
				const char *contents,
				const unsigned long len
				);

    virtual void	set_contents(const char *path);
				
    virtual char*	get_contents();

    virtual void	append_to_contents(
				const char *new_contents,
				const unsigned long len
				);

    virtual void	append_to_contents(const char *path);

    virtual void	append_at_cursor(const char *path);

    virtual void	append_at_cursor(
				const char *contents,
				const unsigned long len
				);

    virtual void	clear_contents();
    virtual int		no_text();

    virtual Widget	get_text_widget();
    virtual Pixel	get_text_foreground();
    virtual Pixel	get_text_background();
    virtual Dimension   get_text_width();
    virtual XmFontList	get_text_fontList();
    virtual Widget	get_editor();
    
    virtual int		get_columns();
    virtual int		get_rows();
    virtual void	set_columns(int ncolumns);
    virtual void	set_editable(Boolean value);
    virtual void	set_rows(int nrows);

    virtual void	auto_show_cursor_off();
    virtual void	auto_show_cursor_restore();

    virtual void	set_to_top();
    virtual void	set_to_bottom();

    virtual void	cut_selection();
    virtual void	copy_selection();
    virtual void	paste_from_clipboard();
    virtual void	paste_special_from_clipboard(Editor::InsertFormat);
    virtual void	clear_selection();
    virtual void	delete_selection();
    virtual void	select_all();

    virtual void	disable_redisplay();
    virtual void	enable_redisplay();

    //
	// End of Pure virtual functions for class Editor.

	// Pure virtual functions of class AbstractEditorParent
    virtual void	text_selected();
    virtual void	text_unselected();

	// Functions specific to DtEditor
    virtual void	set_word_wrap(Boolean value);
    virtual void	undo_edit();
    virtual void	find_change();
    virtual void    spell();
    virtual void    format();

    static void MenuButtonHandler(Widget, XtPointer, XEvent *, Boolean *); 
			
  protected:

    // modify verify callback used during Paste Special
    static void modify_verify_callback(Widget, XtPointer, XtPointer);

    static void text_selected_callback(Widget, void *, void *);
    static void text_unselected_callback(Widget, void *, void *);
	

  private:
    struct PSClientData {
	CDEM_DtWidgetEditor *obj;
	Editor::InsertFormat insert_format;
    };
    XmTextBlockRec	*_modified_text; 
    size_t		_modified_text_buflen;
    enum PasteSpecBuf { MAXBUFSZ = 2048, BUFINC = 512};
    void modifyData(char *, int, Editor::InsertFormat);
    void reallocPasteBuf(size_t size_req) {
		_modified_text->ptr = 
			(char *)realloc((void *)_modified_text->ptr, size_req);
		_modified_text_buflen = size_req; 
    }

    const char *indent_str;
    const char *begin_ins_bracket;
    const char *end_ins_bracket;

    Widget my_parent;
    Widget my_text;
    Widget my_text_core;

    DtMailEditor *my_owner;
    Boolean text_already_selected;
    Boolean		_auto_show_cursor;

    char * _buffer;
    unsigned long _buf_len;

};

#endif // DTEDITOR_HH
