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
/* $TOG: EncryptedTextFieldUiItem.C /main/2 1997/11/13 13:25:10 mgreess $ */
/*
 *+SNOTICE
 *
 *	RESTRICTED CONFIDENTIAL INFORMATION:
 *	
 *	The information in this document is subject to special
 *	restrictions in a confidential disclosure agreement bertween
 *	HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
 *	document outside HP, IBM, Sun, USL, SCO, or Univel wihtout
 *	Sun's specific written approval.  This documment and all copies
 *	and derivative works thereof must be returned or destroyed at
 *	Sun's request.
 *
 *	Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 *
 *+ENOTICE
 */
#include <assert.h>
#include <sys/param.h>
#include <Xm/TextF.h>

#include <DtMail/options_util.h>
#include <DtMail/EncryptedTextFieldUiItem.hh>
#include <DtMail/PropUi.hh>
#include <DtMail/DtMailServer.hh>
#include <DtMail/DtMailTypes.h>

////////////////////////////////////////////////////////////////////
EncryptedTextFieldUiItem::EncryptedTextFieldUiItem(
					Widget		w,
					int		source,
					char		*search_key,
					PropUiCallback	validator,
					void		*validator_data)
: PropUiItem(w, source, search_key, validator, validator_data)
{
    _loading = DTM_FALSE;
    _maxtextlen = 256;
    _text = (char*) malloc(_maxtextlen);
    assert(NULL!=_text);
    _writeAllowed = DTM_FALSE;

    XtVaSetValues(
		w,
		XmNeditable, False,
		XmNvalue, "",
		NULL);
    XtAddCallback(
		w,
		XmNmodifyVerifyCallback, EncryptedTextFieldUiItem::verifyCB,
		(XtPointer) this);

    options_field_init(w, &(this->dirty_bit));
}

///////////////////////////////////////////////////////////////////
void EncryptedTextFieldUiItem::writeAllowed(DtMailBoolean allowed)
{
    Widget	w = getWidget();

    if (_writeAllowed == allowed) return;

    _writeAllowed = allowed;
    if (DTM_TRUE == _writeAllowed)
    {
	char	*s;
	char	*value = strdup(_text);

	for (s=value; *s; s++) *s = '*';

        _loading = DTM_TRUE;
        XtVaSetValues(w, XmNeditable, True, XmNsensitive, True, NULL);
        options_field_set_value(w, value, this->dirty_bit);
        _loading = DTM_FALSE;
	setDirtyBit(True);
	free(value);
    }
    else
    {
        _loading = DTM_TRUE;
        XtVaSetValues(w, XmNeditable, False, XmNsensitive, False, NULL);
        options_field_set_value(w, "", this->dirty_bit);
        _loading = DTM_FALSE;
	setDirtyBit(True);
    }
}


// Takes values in the UI and puts them into the source DB
///////////////////////////////////////////////////////////////////
void EncryptedTextFieldUiItem::writeFromUiToSource()
{
    if (DTM_TRUE == _writeAllowed)
      prop_source->setValue(_text, DTM_TRUE);
    else
      prop_source->setValue(DTMAS_PROPDFLT_PASSWORD, DTM_TRUE);
}

// Takes values in the UI and puts them into the source DB
///////////////////////////////////////////////////////////////////
void EncryptedTextFieldUiItem::writeFromSourceToUi()
{
    int i;
    char *s, *value;
    Widget w = this->getWidget();

    value = (char *)prop_source->getValue(DTM_TRUE);
    if (! strcmp(value, PropSourceDEFAULTVALUE)) return;

    validateLength(strlen(value));
    strcpy(_text, value);

    for (i=0, s=value; i<strlen(value); i++, s++)
      *s = '*';

    _loading = DTM_TRUE;
    options_field_set_value(w, value, this->dirty_bit);
    free((void*) value);
    _loading = DTM_FALSE;
}

// Verifies that the _text array is sufficiently long for a string
// of the specified size.
///////////////////////////////////////////////////////////////////
void EncryptedTextFieldUiItem::validateLength(int length)
{
    length++;	// Account for the '\0'
    if (length >= _maxtextlen)
    {
	_maxtextlen *= 2;
	_text = (char*) realloc((void*) _text, (size_t) _maxtextlen);
	assert(NULL!=_text);
    }
}

///////////////////////////////////////////////////////////////////
void EncryptedTextFieldUiItem::verify(XmTextVerifyPtr cbs)
{
    int			i;
    static char		buffer[MAXPATHLEN];
    char	*s, *t;

#if defined(ENCRYPTED_TEXTFIELD_DEBUG)
    printf(
	    "currInsert=%d newInsert=%d startPos=%d endPos=%d\n",
	    cbs->currInsert,cbs->newInsert,cbs->startPos, cbs->endPos);
    if (cbs->text->ptr) printf("text->ptr=%s\n", cbs->text->ptr);
    printf("_text->ptr=%s\n", _text);
#endif

    for (i=0, s=buffer, t=_text; (*t && i<cbs->startPos); i++, s++, t++)
      *s = *t;

    if (cbs->text->ptr)
    {
        strcpy(s, cbs->text->ptr);
	s += cbs->text->length;
    }
    else
      *s = '\0';

    if (strlen(_text) >= cbs->endPos)
    {
        t = _text+cbs->endPos;
	if (strlen(t))
          strcpy(s, t);
    }

    validateLength(strlen(buffer));
    strcpy(_text, buffer);

    if (cbs->text->ptr)
      for (i=0, s=cbs->text->ptr; i<cbs->text->length; i++, s++)
        *s = '*';

#if defined(ENCRYPTED_TEXTFIELD_DEBUG)
    printf("text=%s\n", _text);
#endif
}

// ValueChangedCallback for the text field.  Saves the user input
// and echos *'s.
///////////////////////////////////////////////////////////////////
void EncryptedTextFieldUiItem::verifyCB(
					Widget		w,
					XtPointer	client_data,
					XtPointer	call_data)
{
    EncryptedTextFieldUiItem	*etf = (EncryptedTextFieldUiItem*) client_data;
    XmTextVerifyCallbackStruct	*cbs = (XmTextVerifyCallbackStruct*) call_data;

    if (DTM_FALSE == etf->_loading) etf->verify(cbs);
}
