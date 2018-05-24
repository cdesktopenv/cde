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
 *	$TOG: XmTextEditor.C /main/13 1998/02/03 10:29:51 mgreess $
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

#include <EUSCompat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#if defined(NEED_MMAP_WRAPPER)
extern "C" {
#endif

#include <sys/types.h>
#include <sys/mman.h>

#if defined(NEED_MMAP_WRAPPER)
}
#endif

#include <Xm/Form.h>
#include <Xm/Text.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "XmTextEditor.h"
#include <RoamApp.h>
#include <MailMsg.h>

XmTextEditor::XmTextEditor(
    Widget parent,
    DtMailEditor *owner_of_editor
)
{
    my_parent   = parent;
    my_owner	= owner_of_editor;
    _w    = NULL;
	my_text = NULL;

    _buffer = NULL;
    _buf_len = 0;
    _modified_text = NULL;
    _modified_text_buflen = 0;
    begin_ins_bracket = NULL;
    indent_str = NULL;
    end_ins_bracket = NULL;
    _auto_show_cursor = FALSE;
    text_already_selected = FALSE;
}

XmTextEditor::~XmTextEditor() 
{
    if (_buffer) {
	delete _buffer;
	_buffer = NULL;
    }

    if(_modified_text )  {
	if(_modified_text->ptr)  {
		free(_modified_text->ptr);
		_modified_text->ptr = NULL;
	}
	free(_modified_text);
	_modified_text = NULL;
    }
    if (NULL != indent_str)
      free((void*) indent_str);
}

void
XmTextEditor::initialize()
{
    Arg args[10];
    int n = 0;

#if 0
    short rows, cols;
    DtMailEnv  error;
    DtMail::Session * d_session = theRoamApp.session()->session();
    DtMail::MailRc * mailrc = d_session->mailRc(error);

    const char * value;
    mailrc->getValue(error, "popuplines", &value);
    if (error.isSet()) {
	value = strdup("24");
    }
    rows = (short) strtol(value, NULL, 10);
    if (NULL != value)
      free((void*) value);

    // If toolcols is set, overwrite the column width with "toolcols" value.
    // Otherwise, default resource value will be used.
    value = NULL;
    mailrc->getValue(error, "toolcols", &value);
    if (!error.isSet()){
        cols = (short) strtol(value, NULL, 10);
        XtSetArg(args[n], XmNcolumns, cols); n++;
        if (NULL != value)
          free((void*) value);
    } else {
	/*
	 * Default XmNcolumns
	 * MB_CUR_MAX == 1 : SingleByteLanguage
	 * MB_CUR_MAX >  1 : MultiByteLanguage
	 */
	if ( MB_CUR_MAX == 1 )
	    value = "80";
	else
	    value = "40";
    }
#endif

    XtSetArg(args[n], XmNeditable, FALSE); n++;
    XtSetArg(args[n], XmNrows, 24); n++;
    if ( MB_CUR_MAX == 1 ) {
      XtSetArg(args[n], XmNcolumns, 80); n++;
    } else {
      XtSetArg(args[n], XmNcolumns, 40); n++;
    }

    _w = XmCreateScrolledText(my_parent, "Text", args, n );

    update_display_from_props();
    XtManageChild(_w);
    XtAddEventHandler(XtParent(_w), ButtonPressMask,
                        FALSE, MenuButtonHandler, 
			(XtPointer) this);
}

void
XmTextEditor::set_contents(
			   const char *contents,
			   const unsigned long len
			   )
{
    if (contents[len - 1] == 0) {
	XmTextSetString(_w, (char *)contents);
    }
    else {
	this->my_owner->needBuf(&_buffer, &_buf_len, len + 1);
	this->my_owner->stripCRLF(&_buffer, contents, len);
	XmTextSetString(_w, _buffer);
    }
}

void
XmTextEditor::set_contents(const char * path)
{
    loadFile(path, 0);
}

char*
XmTextEditor::get_contents()
{
    return(XmTextGetString(_w));
}

void
XmTextEditor::append_to_contents(
    const char *contents,
    const unsigned long len
)
{

    if (contents[len - 1] == 0) {
  	XmTextInsert( _w, XmTextGetLastPosition( _w ), 
		      (char *)contents);
    }
    else {
	// Not null terminated, and most likely has crlf instead of lf.
	//
	this->my_owner->needBuf(&_buffer, &_buf_len, len + 1);
	this->my_owner->stripCRLF(&_buffer, contents, len);
	XmTextInsert(_w, XmTextGetLastPosition(_w), _buffer);
    }
}

void
XmTextEditor::append_to_contents(const char * path)
{
    loadFile(path, (const int) XmTextGetLastPosition(_w));
}

void
XmTextEditor::append_at_cursor(
				 const char *contents,
				 const unsigned long len
				 )
{
    if (contents[len - 1] == 0) {
  	XmTextInsert(
		_w, 
		XmTextGetInsertionPosition(_w), 
		(char *)contents
		);
    }
    else {
	// Not null terminated, and most likely has crlf instead of lf.
	//
	this->my_owner->needBuf(&_buffer, &_buf_len, len + 1);
	this->my_owner->stripCRLF(&_buffer, contents, len);
	XmTextInsert(
		_w, 
		XmTextGetInsertionPosition(_w), 
		_buffer);
    }
}

void
XmTextEditor::append_at_cursor(const char * path)
{
    loadFile(path, (const int)XmTextGetInsertionPosition(_w));
}

void    
XmTextEditor::clear_contents()
{

    XmTextSetString(_w, "");

}

Widget
XmTextEditor::get_text_widget()
{
    return _w;
}

Pixel
XmTextEditor::get_text_foreground()
{
    Pixel fg;
    
    XtVaGetValues(_w,
	XmNforeground, &fg,
	NULL);
    return(fg);
}


Pixel
XmTextEditor::get_text_background()
{
    Pixel bg;
    
    XtVaGetValues(_w,
	XmNbackground, &bg,
	NULL);
    return(bg);
}

XmFontList
XmTextEditor::get_text_fontList()
{
    XmFontList fl;
    
    XtVaGetValues(_w,
	XmNfontList, &fl,
	NULL);
    return(fl);
}

Dimension
XmTextEditor::get_text_width()
{
    Dimension wid;
    
    XtVaGetValues(_w,
        XmNwidth, &wid,
	NULL);
    return (wid);
}

Widget
XmTextEditor::get_editor()
{
    return XtParent(_w);
}

void
XmTextEditor::set_editable(
	Boolean bval
)
{
  XmTextSetEditable( _w, bval);

  // If not editable, turn off the cursor?
  XtVaSetValues(_w, XmNcursorPositionVisible, bval, NULL);
}


int
XmTextEditor::get_columns()
{
  short ncolumns = 0;;
  XtVaGetValues(_w, XmNcolumns, &ncolumns, NULL);
  return ncolumns;
}


int
XmTextEditor::get_rows()
{
  short nrows = 0;
  XtVaGetValues(_w, XmNrows, &nrows, NULL);
  return nrows;
}

void
XmTextEditor::set_columns(int ncolumns)
{
  XtVaSetValues(_w, XmNcolumns, ncolumns, NULL);
}


void
XmTextEditor::set_rows(int nrows)
{
  XtVaSetValues(_w, XmNrows, nrows, NULL);
}


// TOGO void
// TOGO XmTextEditor::set_container(
// TOGO    CMContainer
// TOGO )
// TOGO {
// TOGO    // Extract text and display?
// TOGO }

// TOGO CMContainer
// TOGO XmTextEditor::get_container()
// TOGO {
// TOGO    return(NULL);    
// TOGO }

void
XmTextEditor::auto_show_cursor_off()
{
    // Get the original value of XmNautoShowCursorPosition
    XtVaGetValues(_w,
		  XmNautoShowCursorPosition, &_auto_show_cursor,
		  NULL);

    // Set it to false so we don't scroll with the cursor.
    XtVaSetValues(_w,
		  XmNautoShowCursorPosition, FALSE,
		  NULL);
}

void
XmTextEditor::auto_show_cursor_restore()
{
    // Restore the original value of XmNautoShowCursorPosition.
    XtVaSetValues(_w,
		  XmNautoShowCursorPosition, _auto_show_cursor,
		  NULL);
}

void
XmTextEditor::set_to_top()
{

    XmTextShowPosition(_w, 0);
    XmTextSetInsertionPosition(_w, 0);

}

void
XmTextEditor::set_to_bottom()
{
	XmTextSetInsertionPosition( _w, XmTextGetLastPosition(_w) );
}

#ifdef DEAD_WOOD
void
XmTextEditor::focus_callback(
    Widget,
    void *clientData,
    void *
)
{
 
    XmTextEditor  *obj=(XmTextEditor *) clientData;

    obj->obtained_focus();

}
#endif /* DEAD_WOOD */

void
XmTextEditor::obtained_focus()
{
    // If there is text already selected, then the highlighted
    // text is unhighlighted when the widget gets the focus
    // next.  Need to disable the parent's menu items now.

    if (XmTextGetSelection(_w) != NULL) {
	text_already_selected = TRUE;
    }

    if (text_already_selected)
	this->text_unselected();
}

void
XmTextEditor::text_selected_callback(
    Widget,
    void *clientData,
    void *
)
{

    XmTextEditor  *obj=(XmTextEditor *) clientData;

    obj->text_selected();

}

void
XmTextEditor::text_unselected_callback(
    Widget,
    void *,
    void *
)
{

//    XmTextEditor  *obj=(XmTextEditor *) clientData;

//    obj->text_unselected();

}

void
XmTextEditor::text_selected()
{
    if (!text_already_selected) {
	text_already_selected = TRUE;
	my_owner->owner()->text_selected();
    }
}

void
XmTextEditor::text_unselected()
{
    text_already_selected = FALSE;
    my_owner->owner()->text_unselected();
}

int
XmTextEditor::no_text()
{
	char *text = get_contents();;
	int text_len = strlen(text);
	int result = 1;

	for ( int k = 0;  k < text_len;  k++ ) {
	   if ( isgraph(text[k]) ) {
		  result = 0;
		  break;
	   }
	}
	XtFree(text);
	return result;
}

void
XmTextEditor::undo_edit()
{
	// This is to be consistent with DtEditor.
    // Do nothing since Motif XmText does not support this.
}

void
XmTextEditor::set_word_wrap(Boolean)
{
	// This is to be consistent with DtEditor.
    // Do nothing since Motif XmText does not support this.
}

void
XmTextEditor::find_change()
{
	// This is to be consistent with DtEditor.
    // Do nothing since Motif XmText does not support this.
}

void
XmTextEditor::spell()
{
	// This is to be consistent with DtEditor.
    // Do nothing since Motif XmText does not support this.
}

void
XmTextEditor::format()
{
	// This is to be consistent with DtEditor.
    // Do nothing since Motif XmText does not support this.
}

void
XmTextEditor::cut_selection()
{
	// Shouldn't really use CurrentTime
	XmTextCut( _w, CurrentTime );
}

void
XmTextEditor::copy_selection()
{
	// Shouldn't really use CurrentTime
	XmTextCopy( _w, CurrentTime );
}

void
XmTextEditor::paste_from_clipboard()
{
	XmTextPaste( _w );
}

void
XmTextEditor::paste_special_from_clipboard(Editor::InsertFormat format)
{
	PSClientData cd;

	cd.obj = this;
	cd.insert_format = format;

	XtAddCallback(_w, XmNmodifyVerifyCallback, 
			modify_verify_callback, (XtPointer)&cd);

	XmTextPaste( _w );

	XtRemoveCallback(_w, XmNmodifyVerifyCallback, 
			modify_verify_callback, (XtPointer)&cd);
}


// Removes the selection and leaves the resulting white space.
void
XmTextEditor::clear_selection()
{
	// Shouldn't really use CurrentTime
	// XmTextClearSelection( _w, CurrentTime );

	// BUG in XmTextClearSelection.  Selection is not cleared.  Only the cursor
	// is advanced to the last selected position.  Therefore need the following
	// workaround.

	XmTextPosition left, right;

	if ( XmTextGetSelectionPosition( _w, &left, &right ) ) {
	   char *sel = XmTextGetSelection( _w );
	   // NOTE:  Modifying buffer returned by XmTextGetSelection.
	   // Future Motif implementation might return read only buffer.
	   if (sel != NULL) {
	   	memset( sel, ' ', strlen(sel) );
	   	// XmTextInsert( _w, left, sel );
	   	XmTextReplace( _w, left, right, sel );
	   	XtFree(sel);
	   }
	}
}

// Removes the selection and compresses the resulting white space.
void
XmTextEditor::delete_selection()
{
	XmTextRemove( _w );
}

void
XmTextEditor::select_all()
{
	XmTextSetSelection( _w, 0, XmTextGetLastPosition(_w), CurrentTime );
}

void
XmTextEditor::disable_redisplay(void)
{
    XmTextDisableRedisplay(_w);
}

void
XmTextEditor::enable_redisplay(void)
{
    XmTextEnableRedisplay(_w);
}

void
XmTextEditor::loadFile(const char * path, const int pos)
{
    int fd = open(path, O_RDONLY);
    if (fd < 0) {
	return;
    }

    struct stat info;
    if (fstat(fd, &info) < 0) {
	close(fd);
	return;
    }

    int page_size = (int)sysconf(_SC_PAGESIZE);
    size_t map_size = (size_t)(info.st_size + 
				(page_size - (info.st_size % page_size)));
    char * map;

    map = (char *) mmap(0, map_size, PROT_READ, MAP_PRIVATE, fd, 0);

    if (map == (char *)-1) {
	// We could not map it for some reason. Let's just read it into
	// _buffer and pass it to XmText.
	//
	this->my_owner->needBuf(&_buffer, &_buf_len, info.st_size + 1);
	if (read(fd, _buffer, (unsigned int) info.st_size) < 0) {
	    close(fd);
	    return;
	}
	_buffer[info.st_size] = 0;
	XmTextInsert(_w, pos, _buffer);
    }
    else {
	// We now have a mapped file. XmText wants a zero terminated
	// buffer. We get luck with mmap because unless the file is
	// an even page size, we will have some zero fill bytes that
	// are legal to access.
	//
	// Of course in the case of an even page size file we must
	// copy the buffer, terminate it and then give it to XmText.
	//
	if (info.st_size < map_size) {
	    XmTextInsert(_w, pos, map);
	}
	else {
	    this->my_owner->needBuf(&_buffer, &_buf_len, info.st_size + 1);
	    this->my_owner->stripCRLF(&_buffer, map, info.st_size);
	    XmTextInsert(_w, pos, _buffer);
	}
	munmap(map, map_size);
    }

    close(fd);
}

void 
XmTextEditor::modify_verify_callback(
	Widget , XtPointer client, XtPointer call_data)
{
	PSClientData *cd = (PSClientData *)client;
	XmTextVerifyCallbackStruct *modify_info = 
			(XmTextVerifyCallbackStruct *)call_data;

	// Make sure we are being called programmatically
	if(modify_info->event != (XEvent *)NULL)
		return;

	cd->obj->modifyPasteData(modify_info, cd->insert_format);
}

/*
 * This fucntion modifies the pasted data
 * with an indent prefix before each new line or brackets it.
 */

void
XmTextEditor::modifyPasteData(
	XmTextVerifyCallbackStruct *modify_info,
	Editor::InsertFormat insert_format)
{
	// The toolkit does not use this any more, this must be weird stuff.
	if(modify_info->text->format == XmFMT_16_BIT)
		return;

	if(_modified_text == NULL)
		_modified_text = (XmTextBlockRec *)calloc(1,sizeof(XmTextBlockRec));

	char *sp = modify_info->text->ptr; // source pointer to the insert string 
	int length = modify_info->text->length; // length does not include '\0' char
	char *maxsp = sp  + length; // maxmimum source ptr

	// Allocate memory rounded off to the nearest BUFINC
	size_t size_req = (size_t)(((length/BUFINC)+1)*BUFINC);
	if((_modified_text_buflen < size_req)	||
		  ((_modified_text_buflen > XmTextEditor::MAXBUFSZ) && 
			(size_req <  XmTextEditor::MAXBUFSZ)) 	)
		reallocPasteBuf(size_req);

	if(_modified_text->ptr == NULL)
		return; // No memory available

	switch( insert_format) {
	case IF_INDENTED:	
		{
		DtMailEnv error;
		int ip = 0;

		// Get the indent prefix string
		DtMail::Session *m_session = 
				theRoamApp.session()->session();
		m_session->mailRc(error)->getValue(error, 
				"indentprefix", &indent_str);
		if ( error.isSet() || !indent_str) 
			indent_str = strdup("> ");

		size_t indlen = strlen(indent_str);

		// Copy the src buf into dest, inserting indent before '\n'
		while(sp < maxsp) {

			// Make sure there is enough space
			// for an indent prefix, one char and a terminating '\0'
			if(!((ip+indlen+2) < _modified_text_buflen) ) {
				size_req = (size_t)((((_modified_text_buflen + 
						indlen+2)/BUFINC)+1)*BUFINC);
				reallocPasteBuf(size_req);
				if(_modified_text->ptr == NULL)
					return; // No memory available
			}

			// Copy the indent string at the beginning 
			if(!ip) { 
				memcpy(_modified_text->ptr, indent_str, indlen);
                		ip += indlen;
			}

			// Copy the next byte and check for new line
			_modified_text->ptr[ip++] = *sp++; 
			if(*(sp-1) == '\n') {
				memcpy(&_modified_text->ptr[ip], indent_str, indlen);
				ip += indlen;
			}

		}
		_modified_text->ptr[ip] = '\0'; // terminate with a null char
		_modified_text->length = ip; // Do not include '\0' char in len
		}
		break;
	case IF_BRACKETED:
		{

		if( !begin_ins_bracket)
			begin_ins_bracket = GETMSG(DT_catd, 1, 199,
				"\n------------- Begin Included Message -------------\n"); 
		if(!end_ins_bracket) 
			end_ins_bracket = GETMSG(DT_catd, 1, 200,
				"\n------------- End Included Message -------------\n"); 
		
		size_t begin_len = strlen(begin_ins_bracket); 
		size_t end_len = strlen(end_ins_bracket); 

		// Make sure there is enough space
		if((size_req = length + begin_len + end_len + 1) > 
					_modified_text_buflen) {
			size_req = (size_t) ((((size_req)/BUFINC)+1)*BUFINC);
			reallocPasteBuf(size_req);
		}

		if(_modified_text->ptr == NULL)
			return;

		strcpy(_modified_text->ptr, begin_ins_bracket);
		strncat(_modified_text->ptr,sp,length);
		strcat(_modified_text->ptr, end_ins_bracket); 
		_modified_text->length = end_len + begin_len + length;
		}
		break;
	default:
		break;
	}
			
	_modified_text->format = modify_info->text->format;

	// Stuff the modified text block ptr in the return call data
	modify_info->text = _modified_text;
}

void
XmTextEditor::MenuButtonHandler(
    Widget ,
    XtPointer cd,
    XEvent *event,
    Boolean *)
{
	XmTextEditor *obj = (XmTextEditor *)cd;

	if(event->xany.type != ButtonPress)
		return;

	XButtonEvent *be = (XButtonEvent *)event;

	if(be->button == Button3)
		obj->my_owner->owner()->postTextPopup(event);
}

