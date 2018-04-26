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
 *	$TOG: DtEditor.C /main/11 1998/02/03 10:28:15 mgreess $
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

#ifndef I_HAVE_NO_IDENT
#else
#endif

#ifdef DTEDITOR

#include <Xm/Xm.h>
#include <Xm/Form.h>
#include <Xm/Text.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <X11/IntrinsicP.h>
#include <Xm/Text.h>
#include <Xm/CutPaste.h>

#include "Help.hh"
#include "RoamApp.h"
#include "DtEditor.hh"
#include "MailMsg.h"               // DT_catd defined here

CDEM_DtWidgetEditor::CDEM_DtWidgetEditor(
    Widget parent,
    DtMailEditor *owner_of_editor
)
{
    my_parent   = parent;
    my_owner	= owner_of_editor;
    my_text     = NULL;
    my_text_core = NULL;
    _modified_text = NULL;
    _modified_text_buflen = 0;

    begin_ins_bracket = NULL;
    indent_str = NULL;
    end_ins_bracket = NULL;
    _auto_show_cursor = FALSE;

    _buffer     = NULL;
    _buf_len    = (unsigned long) 0;
    text_already_selected = FALSE;
}

CDEM_DtWidgetEditor::~CDEM_DtWidgetEditor() 
{
    if (my_text) {
	// No DtEditor API equivalent
	// Remove the callbacks first.

	XtRemoveCallback(my_text, DtNtextSelectCallback,
	    &CDEM_DtWidgetEditor::text_selected_callback, this);
	XtRemoveCallback(my_text, DtNtextDeselectCallback,
	    &CDEM_DtWidgetEditor::text_unselected_callback, this);
        XtRemoveCallback( my_text, XmNhelpCallback, HelpTexteditCB, this ) ;

	XtDestroyWidget(my_text);
    }
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
CDEM_DtWidgetEditor::initialize()
{
    
    int i = 0;

    Arg args[10];
    int n = 0;

#if 0
    short rows, cols;
    DtMailEnv  error;
    DtMail::Session * d_session = theRoamApp.session()->session();
    DtMail::MailRc * mailrc = d_session->mailRc(error);

    const char * value = NULL;
    mailrc->getValue(error, "popuplines", &value);
    if (error.isSet()) {
	value = strdup("24");
    }
    rows = strtol(value, NULL, 10);
    if (NULL != value)
      free((void*) value);

    // If toolcols is set, overwrite the column width with "toolcols" value.
    // Otherwise, default resource value will be used.
    value = NULL;
    mailrc->getValue(error, "toolcols", &value);
    if (!error.isSet()){
        cols = strtol(value, NULL, 10);      
        XtSetArg(args[i], DtNcolumns, cols); i++;
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

        cols = strtol(value, NULL, 10);      
        XtSetArg(args[i], DtNcolumns, cols); i++;
    }
#endif

    XtSetArg(args[i], DtNeditable, FALSE); i++;
    XtSetArg(args[i], DtNrows, 24); i++;
    if ( MB_CUR_MAX == 1 ) {
      XtSetArg(args[i], DtNcolumns, 80); i++;
    } else {
      XtSetArg(args[i], DtNcolumns, 40); i++;
    }
    XtSetArg(args[i], DtNcursorPositionVisible, FALSE); i++;

    my_text = DtCreateEditor(my_parent, "Text", args, i);

    update_display_from_props();
    XtAddCallback(my_text, DtNtextSelectCallback,
 	   &CDEM_DtWidgetEditor::text_selected_callback, this);
    XtAddCallback(my_text, DtNtextDeselectCallback,
 	   &CDEM_DtWidgetEditor::text_unselected_callback, this);
    XtAddCallback( my_text, XmNhelpCallback, HelpTexteditCB, this ) ;

    XtAddEventHandler(my_text, ButtonPressMask,
                        FALSE, MenuButtonHandler, 
			(XtPointer) this);

    XtManageChild(my_text);

}


char*
CDEM_DtWidgetEditor::get_contents()
{
    
    DtEditorErrorCode status;
    static DtEditorContentRec content;

    content.type = DtEDITOR_TEXT;
    
    // Get the contents with hardCarriageReturns = TRUE and
    // markContentsAsSaved = TRUE.

    /*
     * If hardCarriageReturns = TRUE, the performace of DtEditorGetContents()
     * suffers since according to man 3 DtEditorGetContents,
     *
     *    The hardCarriageReturns argument, if set to True, indicates
     *    that the DtEditor widget should replace any soft line feeds
     *    (word wraps) with <newline>s when saving the data.  When
     *    hardCarriageReturns is set to False, any line wrapped
     *    because it reaches the right edge of the window is saved as
     *    one complete line.
     * 
     * And current default value of DtNwordWrap is TRUE. See dtmail/Dtmail.
     * My temporary and non-good solution is
     *    - Change default to False.
     *    - If DtNwordWarp == TRUE,
     *          call DtEditorGetContents(my_text, &content, TRUE, TRUE)
     *      if not,
     *          call DtEditorGetContents(my_text, &content, False, TRUE)
     * This value can be controllable by a resource file or Format menu.
     *
     * Goofy ? but...................;-(
     */
    Arg     args[1];
    Boolean ww;

    XtSetArg( args[0], DtNwordWrap, &ww );
    XtGetValues( my_text, args, 1 );
    status = DtEditorGetContents(my_text, &content, ww, TRUE);
    
    return(content.value.string);
}

void
CDEM_DtWidgetEditor::set_contents(
    const char *contents,
    const unsigned long len
)
{
    DtEditorContentRec content;
    DtEditorErrorCode status;

    this->my_owner->needBuf(&_buffer, &_buf_len, len + 1);
    this->my_owner->stripCRLF(&_buffer, contents, len);

    content.type = DtEDITOR_TEXT;
    content.value.string = _buffer;
    status = DtEditorSetContents(my_text, &content);

}

void
CDEM_DtWidgetEditor::set_contents(
    const char *path
)
{
   DtEditorSetContentsFromFile(my_text, (char *)path);
}

void    
CDEM_DtWidgetEditor::clear_contents()
{

// Doesn't work yet.  Work around with setting an empty string...
//    DtEditorReset(my_text);

    DtEditorContentRec content;
    DtEditorErrorCode status;

    content.type = DtEDITOR_TEXT;
    content.value.string = NULL;
    status = DtEditorSetContents(my_text, &content);

}

void
CDEM_DtWidgetEditor::append_to_contents(
    const char *contents,
    const unsigned long len
)
{
    DtEditorContentRec rec;

    rec.type = DtEDITOR_TEXT;

    if ( contents[len - 1] == 0 ) {
	rec.value.string = (char *)contents;
    } else {
	this->my_owner->needBuf(&_buffer, &_buf_len, len + 1);
	this->my_owner->stripCRLF(&_buffer, contents, len);
	rec.value.string = _buffer;
    }

    DtEditorInsert(my_text, &rec);
}

void
CDEM_DtWidgetEditor::append_to_contents(
    const char *path
)
{

    DtEditorAppendFromFile(my_text, (char *)path);
    
}

void
CDEM_DtWidgetEditor::append_at_cursor(
    const char *path
)
{
    DtEditorInsertFromFile(my_text, (char *) path);
}

void
CDEM_DtWidgetEditor::append_at_cursor(
    const char *contents,
    const unsigned long len
)
{
    DtEditorContentRec rec;

    rec.type = DtEDITOR_TEXT;

    if ( contents[len - 1] == 0 ) {
	rec.value.string = (char *)contents;
    } else {
	this->my_owner->needBuf(&_buffer, &_buf_len, len + 1);
	this->my_owner->stripCRLF(&_buffer, contents, len);
	rec.value.string = _buffer;
    }
    //DtEditorAppend(my_text, &rec);
    // Fix for the defect 179186 05-25-95
    // The above API will insert "contents" to the end of the buffer
    // (appending). It should change to DtEditorInsert which insert 
    // string to the current position (the cursor's position)
    DtEditorInsert(my_text, &rec);
}

Widget
CDEM_DtWidgetEditor::get_text_widget()
{
    // We actually need to return the text widget contained
    // within DtEditor.  For now, just return the DtEditor.

    return(my_text);
}

Pixel
CDEM_DtWidgetEditor::get_text_foreground()
{
    Pixel fg;
    
    XtVaGetValues(my_text,
	DtNtextForeground, &fg,
	NULL);

    return(fg);
}


// DtEditor returns the bg color of the Form widget, not the 
// text widget that the Form contains.
// This explains why the attachment pane color is that of the scroll bar...
// DtEditor needs to return the color of its text widget.
// OBTW, DtNtextBackground and DtNtextForeground don't work.  They
// return uninitialized values

Pixel
CDEM_DtWidgetEditor::get_text_background()
{
    Pixel bg;
    
    XtVaGetValues(my_text,
	DtNtextBackground, &bg,
	NULL);
    
    return(bg);
}

XmFontList
CDEM_DtWidgetEditor::get_text_fontList()
{
    XmFontList fl;
    
    XtVaGetValues(my_text,
	DtNtextFontList, &fl,
	NULL);

    return(fl);
}

Dimension
CDEM_DtWidgetEditor::get_text_width()
{
    Dimension wid;
    
    XtVaGetValues(my_text,
        XmNwidth, &wid,
	NULL);
    return (wid);
}


Widget
CDEM_DtWidgetEditor::get_editor()
{
    return(my_text);
}

int
CDEM_DtWidgetEditor::get_columns()
{
  short ncolumns;
  XtVaGetValues(my_text, DtNcolumns, &ncolumns, NULL);
  return (int) ncolumns;
}


int
CDEM_DtWidgetEditor::get_rows()
{
  short nrows;
  XtVaGetValues(my_text, DtNrows, &nrows, NULL);
  return (int) nrows;
}

void
CDEM_DtWidgetEditor::set_editable(Boolean bval)
{
   XtVaSetValues(my_text,
		DtNeditable, bval,
		DtNcursorPositionVisible, bval,
		NULL);
}


void
CDEM_DtWidgetEditor::set_columns(int ncolumns)
{
  XtVaSetValues(my_text, DtNcolumns, ncolumns, NULL);
}


void
CDEM_DtWidgetEditor::set_rows(int nrows)
{
  XtVaSetValues(my_text, DtNrows, nrows, NULL);
}

void
CDEM_DtWidgetEditor::undo_edit()
{

    DtEditorUndoEdit(my_text);

}

void
CDEM_DtWidgetEditor::cut_selection()
{

    DtEditorCutToClipboard(my_text);

}

void
CDEM_DtWidgetEditor::copy_selection()
{

    DtEditorCopyToClipboard(my_text);

}

void
CDEM_DtWidgetEditor::paste_from_clipboard()
{

    DtEditorPasteFromClipboard(my_text);

}

void
CDEM_DtWidgetEditor::paste_special_from_clipboard(
    Editor::InsertFormat format
)
{
    int status;
    unsigned long length, recvd;
    char *clipboard_data;
    Display *dpy = XtDisplayOfObject(my_text);
    Window window = XtWindowOfObject(my_text);

    do {
	status = XmClipboardInquireLength(dpy, window, "STRING", &length);
    } while (status == ClipboardLocked);

    if (length == 0) {
	return;
    }
    
    clipboard_data = XtMalloc((unsigned)length);
    
    do {
	status = XmClipboardRetrieve(
			dpy, window, "STRING", clipboard_data, 
			length,  &recvd, NULL
			);
    } while (status == ClipboardLocked);
    
    if (status != ClipboardSuccess || recvd != length) {
	// Couldn't get all
	
	XtFree(clipboard_data);
	return;
    }

    // Now modify the data such that the necessary formatting occurs
    // within it.  Bracketting will cause a line at the beginning and
    // end of the data.  Indenting will prepend a ">" before each line,
    // realigning the lines if necessary.
    // The results are stored in _modified_text so clipboard_data can 
    // be freed immediately after this call.

    this->modifyData(clipboard_data, (unsigned) length, format);
    XtFree(clipboard_data);

    // Now copy the modified data stripped of CRLFs to a buffer.
    // Put that buffer into the structure appropriate for DtEditor.

    DtEditorContentRec rec;

    rec.type = DtEDITOR_TEXT;
    
    // Length needs to be reset since the text now contains
    // new characters that do the necessary formatting.

    length = _modified_text->length;

    if ( _modified_text->ptr[(unsigned) length - 1] == 0 ) {
	rec.value.string = (char *)_modified_text->ptr;
    } else {
	this->my_owner->needBuf(
			&_buffer, &_buf_len, 
			(unsigned) length + 1
			);
	this->my_owner->stripCRLF(
			&_buffer, _modified_text->ptr, 
			(unsigned) length);
	rec.value.string = _buffer;
    }
    
    DtEditorInsert(my_text, &rec);
}


void
CDEM_DtWidgetEditor::clear_selection()
{

    DtEditorClearSelection(my_text);

}

void
CDEM_DtWidgetEditor::delete_selection()
{
    DtEditorDeleteSelection(my_text);
}

void
CDEM_DtWidgetEditor::set_word_wrap(
    Boolean bval
)
{
   XtVaSetValues(my_text, DtNwordWrap, bval, NULL);
}

void
CDEM_DtWidgetEditor::set_to_top()
{
    XtVaSetValues(my_text, 
	DtNtopCharacter, 0, 
	DtNcursorPosition, 0,
	NULL);
}


void
CDEM_DtWidgetEditor::text_selected_callback(
    Widget,
    void * clientData,
    void *
)
{

    CDEM_DtWidgetEditor  *obj=(CDEM_DtWidgetEditor *) clientData;

    obj->text_selected();

}

void
CDEM_DtWidgetEditor::text_unselected_callback(
    Widget,
    void * clientData,
    void *
)
{

    CDEM_DtWidgetEditor  *obj=(CDEM_DtWidgetEditor *) clientData;

    obj->text_unselected();

}

void
CDEM_DtWidgetEditor::text_selected()
{

    if (!text_already_selected) {
	text_already_selected = TRUE;
	my_owner->owner()->text_selected();
    }
}

void
CDEM_DtWidgetEditor::text_unselected()
{

    my_owner->owner()->text_unselected();
    text_already_selected = FALSE;

}

void
CDEM_DtWidgetEditor::find_change()
{
   DtEditorInvokeFindChangeDialog(my_text);
}

void
CDEM_DtWidgetEditor::spell()
{
   DtEditorInvokeSpellDialog(my_text);
}

void
CDEM_DtWidgetEditor::format()
{
   DtEditorInvokeFormatDialog(my_text);
}

void
CDEM_DtWidgetEditor::auto_show_cursor_off()
{
}

void
CDEM_DtWidgetEditor::auto_show_cursor_restore()
{
}

void
CDEM_DtWidgetEditor::select_all()
{
   DtEditorSelectAll(my_text);
}

void
CDEM_DtWidgetEditor::set_to_bottom()
{
}

int
CDEM_DtWidgetEditor::no_text()
{
   char *text = get_contents();   
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
CDEM_DtWidgetEditor::disable_redisplay()
{
    DtEditorDisableRedisplay(my_text);
}

void
CDEM_DtWidgetEditor::enable_redisplay()
{

    DtEditorEnableRedisplay(my_text);

}



/*
 * This fucntion modifies the pasted data
 * with an indent prefix before each new line or brackets it.
 */

void
CDEM_DtWidgetEditor::modifyData(
    char *sp,  // source pointer to the insert string 
    int length, // length does not include '\0' char
    Editor::InsertFormat insert_format
)
{
    if(_modified_text == NULL)
	_modified_text = (XmTextBlockRec *)calloc(1,sizeof(XmTextBlockRec));

    char *maxsp = sp  + length; // maxmimum source ptr

    // Allocate memory rounded off to the nearest BUFINC
    size_t size_req = (size_t)(((length/BUFINC)+1)*BUFINC);
    if((_modified_text_buflen < size_req)	||
       ((_modified_text_buflen > CDEM_DtWidgetEditor::MAXBUFSZ) && 
	(size_req <  CDEM_DtWidgetEditor::MAXBUFSZ)) 	)
	reallocPasteBuf(size_req);
    
    if(_modified_text->ptr == NULL)
	    return; // No memory available

    switch( insert_format) {
      case IF_INDENTED:	
      {
	  DtMailEnv error;
	  int ip = 0;
	  
		// Get the indent prefix string
          DtMail::Session *m_session = theRoamApp.session()->session();
	  m_session->mailRc(error)->getValue(error,"indentprefix", &indent_str);
	  if (error.isSet() || NULL == indent_str) 
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
	    begin_ins_bracket = GETMSG(DT_catd, 1, 201,
		"\n------------- Begin Included Message -------------\n"); 
	if(!end_ins_bracket) 
	    end_ins_bracket = GETMSG(DT_catd, 1, 202,
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
}

void
CDEM_DtWidgetEditor::MenuButtonHandler(
    Widget ,
    XtPointer cd,
    XEvent *event,
    Boolean *)
{
    CDEM_DtWidgetEditor *obj = (CDEM_DtWidgetEditor *)cd;

    if(event->xany.type != ButtonPress)
	return;

    XButtonEvent *be = (XButtonEvent *)event;

    if(be->button == Button3)
	obj->my_owner->owner()->postTextPopup(event);
}


#endif
