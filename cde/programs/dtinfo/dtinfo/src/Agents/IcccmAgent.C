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
 * License along with these librararies and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
// $TOG: IcccmAgent.C /main/8 1998/04/17 11:33:22 mgreess $
/*	Copyright (c) 1994,1995,1996 FUJITSU LIMITED	*/
/*	All Rights Reserved				*/

#define C_TOC_Element
#define L_Basic

#define C_IcccmAgent
#define L_Agents

#define C_WindowSystem
#define L_Other

#define C_MessageMgr
#define L_Managers

#include <X11/Xmu/Atoms.h>
#include <X11/Xmu/StdSel.h>

#include <Prelude.h>

#include "Other/XmStringLocalized.hh"
#include "Managers/CatMgr.hh"

#include "Registration.hh"

Time		IcccmAgent::f_paste_activated_time;
IcccmAgent*	IcccmAgent::f_selection_owner;
Atom		IcccmAgent::f_owning_selection;
Widget		IcccmAgent::f_owning_widget;
int		IcccmAgent::f_getting_selection,
		IcccmAgent::f_voluntary_disown;

IcccmAgent::IcccmAgent(void* real_object,
		data_handler_t string_handler, data_exporter_t string_exporter)
	: f_real_object(real_object),
	  f_string_handler(string_handler), f_string_exporter(string_exporter)
{
    ON_DEBUG(cout << "***** IcccmAgent *****" << endl;);
}

IcccmAgent::IcccmAgent(void* real_object, data_handler_t string_handler)
	: f_real_object(real_object),
	  f_string_handler(string_handler),
	  f_string_exporter((data_exporter_t) NULL)
{
    ON_DEBUG(cout << "***** IcccmAgent *****" << endl;);
}

IcccmAgent::IcccmAgent(void* real_object, data_exporter_t string_exporter)
	: f_real_object(real_object),
	  f_string_handler((data_handler_t) NULL),
	  f_string_exporter(string_exporter)
{
    ON_DEBUG(cout << "***** IcccmAgent *****" << endl;);
}

IcccmAgent::~IcccmAgent()
{
    ON_DEBUG(cout << "***** ~IcccmAgent *****" << endl;);
    // if I am the owner, relinquishes the selection
    if (f_selection_owner == this) {
	    assert( selection_owner() );
	    disown_selection();
	    assert( selection_owner() == NULL );
    }
}

IcccmAgent*
IcccmAgent::selection_owner()
{
#ifndef NDEBUG
    if (f_selection_owner)
	assert( f_owning_selection != None && f_owning_widget != NULL );
    else
	assert( f_owning_selection == None && f_owning_widget == NULL );
#endif
    return f_selection_owner;
}

Atom
IcccmAgent::value_handler(Widget w, XtPointer ia, Atom *selection,
	Atom *type, XtPointer value, unsigned long *length, int *format)
{
    if (*type == XT_CONVERT_FAIL) {
        ON_DEBUG( cerr << "(ERROR) valueCB: conversion failed" << endl; );
        return None;
    }

    if ((*type == None) || (*length == 0)) {
#ifdef DEBUG
        cerr << "(ERROR) valueCB: this guy contradicted himself!" << endl;
#endif
        return None;
    }

    if (*type == XA_TEXT(XtDisplay(w)) || *type == XA_STRING) {
#ifdef DEBUG   
        if (*type == XA_STRING)
            cout << "(DEBUG) valueCB: import data as XA_STRING" << endl;
        else
            cout << "(DEBUG) valueCB: import data as XA_TEXT" << endl;
#endif
#ifdef Internationalize
        if (*type == XA_STRING) { // Latin1
            const char* p = extract_ascii((char*)value);
            XtFree((char*)value);
            value = (XtPointer)p;
        }
        else { // XA_TEXT
            char* mbs = (char *)value;
            wchar_t *wcs = (wchar_t*)malloc(strlen(mbs) + 1);
            // check if value string can be valid in current locale
            if (mbstowcs(wcs, mbs, strlen(mbs) + 1) < 0) { // invalid
                const char* p = extract_ascii((char*)value);
                XtFree((char*)value);
                value = (XtPointer)p;
            }
            free(wcs);
        }
#endif
        (((IcccmAgent*)ia)->*f_string_handler)((char*)value, *length);
        XtFree((char*)value);
    }
    else if (*type == XA_COMPOUND_TEXT(XtDisplay(w))) {
#ifdef DEBUG
        cerr << "(DEBUG) valueCB: import data as XA_COMPOUND_TEXT" << endl;
#endif
        XTextProperty prop;
        prop.value = (unsigned char*)value;
        prop.encoding = XA_COMPOUND_TEXT(XtDisplay(w));
        prop.format = 8;
        prop.nitems = *length;
        char** string_list;
        int count, len, slen;
        XmbTextPropertyToTextList(XtDisplay(w), &prop, &string_list, &count);
        char *mbs = strdup("");
        for (int i=0; i<count; i++) {
            slen = strlen(mbs);
            len = strlen(string_list[i]);
            mbs = (char*)realloc(mbs, slen + len + 1);
            *((char *) memcpy(mbs, string_list[i], len) + len) = '\0';
        }
        XwcFreeStringList((wchar_t**)string_list);
        (((IcccmAgent*)ia)->*f_string_handler)(mbs, strlen(mbs));
        free((void*)mbs);
    }
    else {
#ifdef DEBUG
        cerr << "(ERROR) valueCB: import data as unsupported Atom" << endl;
#endif
	return None;
    }
    return *type;
}

Atom
IcccmAgent::target_handler(Widget w, XtPointer cdata, Atom *selection,
	Atom *type, XtPointer value, unsigned long *length, int *format)
{
#define targetCB_return		{ nest--; return best_target; }

    static int nest = 0;
    nest++;

    Atom best_target = None;

    if (*type == XT_CONVERT_FAIL) {
	if (nest < 5) { // retry
	    ON_DEBUG( cerr << "(ERROR) targetCB: failed, retry" << endl; );
	    XtGetSelectionValue(w, *selection, XA_TARGETS(XtDisplay(w)),
				targetCB, cdata, f_paste_activated_time);
	}
	ON_DEBUG( cerr << "(ERROR) targetCB: failed" << endl; );
	targetCB_return
    }

    // There is no owner or the owner doesn't understand XA_TARGETS!
    // Hopefully, XA_TEXT will work.
    if ((*type == None) || (*length == 0)) {
#ifdef DEBUG
	cerr << "(WARNING) targetCB: "
	     << "this guy doesn't understand XA_TARGETS, "
	     << "just try XA_TEXT" << endl;
#endif
	best_target = XA_TEXT(XtDisplay(w));
    }
    else
	best_target = pickup_target(w, (Atom *)value, *length);

    if (best_target == None)
	targetCB_return
    else
	XtGetSelectionValue(w, *selection, best_target, valueCB,
			    cdata, f_paste_activated_time);

    targetCB_return

#undef targetCB_return
}

void
IcccmAgent::get_selection_value(Widget w, Atom selection,
				IcccmAgent* ia_this, Time as_of)
{
    f_paste_activated_time = as_of;

    XtGetSelectionValue(w, selection, XA_TARGETS(XtDisplay(w)),
				targetCB, (XtPointer)ia_this, as_of);
}

void
IcccmAgent::targetCB(Widget w, XtPointer ia, Atom *selection,
	Atom *type, XtPointer value, unsigned long *length, int *format)
{
    Atom atom;
    atom = ((IcccmAgent*)ia)->target_handler(w, ia, selection, type, value,
							length, format);
    if (atom == None) // failed to resolve the best atom
	message_mgr().info_dialog ((char*)
		UAS_String(CATGETS(Set_Messages, 4,
				"There is no text selected.\n"
				"Select some text, then try again.")));
}

void
IcccmAgent::valueCB(Widget w, XtPointer ia, Atom *selection,
	Atom *type, XtPointer value, unsigned long *length, int *format)
{
    Atom atom;
    atom = ((IcccmAgent*)ia)->value_handler(w, ia, selection, type, value,
							length, format);

    if (atom == None) // failed to resolve the best atom
	message_mgr().info_dialog ((char*)
		UAS_String(CATGETS(Set_Messages, 4,
				"There is no text selected.\n"
				"Select some text, then try again.")));
}

Atom
IcccmAgent::pickup_target(Widget w, Atom *targets, unsigned long length)
{
    Atom best = None;
    Display* display = XtDisplay(w);

    Atom *iter;
    // search XA_COMPOUND_TEXT
    for (iter=targets; iter<targets+length; iter++) {
	if (*iter == XA_COMPOUND_TEXT(display)) {
	    best = *iter;
	    ON_DEBUG( cout << "(DEBUG) pickup XA_COMPOUND_TEXT" << endl; );
	    break;
	}
    }
    if (best != None)
	return best;

    for (iter=targets; iter<targets+length; iter++) {
        if (*iter == XA_TEXT(display)) {
            best = *iter;
            ON_DEBUG( cout << "(DEBUG) pickup XA_TEXT" << endl; );
            break;
        }
    }
    if (best != None)
        return best;

    for (iter=targets; iter<targets+length; iter++) {
        if (*iter == XA_STRING) {
            best = *iter;
            ON_DEBUG( cout << "(DEBUG) pickup XA_STRING" << endl; );
            break;
        }
        else
            continue;
    }
#ifdef DEBUG
    if (best == None)
        cout << "(DEBUG) no proper Atoms found" << endl;
#endif

    return best;
}

// set new string_handler, returns old one
data_handler_t
IcccmAgent::set_string_handler(data_handler_t handler)
{
    data_handler_t old_handler = f_string_handler;
    f_string_handler = handler;
    return old_handler;
}

// set new string_exporter, returns old one
data_exporter_t
IcccmAgent::set_string_exporter(data_exporter_t exporter)
{
    data_exporter_t old_exporter = f_string_exporter;
    f_string_exporter = exporter;
    return old_exporter;
}

const char*
IcccmAgent::extract_ascii(const char* mbs)
{
    if (mbs == NULL)
        return NULL;
    unsigned char* asciis = (unsigned char*)
                                XtMalloc((strlen(mbs) + 1) * sizeof(char));

    const unsigned char* src = (const unsigned char*)mbs;
    unsigned char* dest = asciis;
    for (; *src; src++) {
        if ((*src & 0x80) == 0) // msb off
            *dest++ = *src;
    }
    *dest = '\0';
    return (char*)asciis;
}

void
IcccmAgent::own_selection(Widget w, Atom selection, IcccmAgent* ia,
			Time as_of, own_success_CB_t fs, own_fail_CB_t ff)
{
    ON_DEBUG(cout << "***** IcccmAgent::own_selection *****" << endl;);
    f_getting_selection = True;

#ifdef DEBUG
    if (f_selection_owner && f_selection_owner != this)
	cout << "(DEBUG) own_selection: "
	     << "Another IcccmAgent already has selection" << endl;
#endif
    // same owner, same widget, same selection
    if (f_selection_owner == this &&
	f_owning_selection == selection &&
	f_owning_widget == w) {
#ifdef DEBUG
	cout << "(DEBUG) own_selection: same combination, do nothing" << endl;
#endif
#if 0
	(((IcccmAgent*)f_real_object)->*fs)();
#endif
    }
    else {
    // DoneProc not supported yet
	if (XtOwnSelection(w, selection, as_of,
					convertCB, loseCB, NULL) == True) {
		assert( selection_owner() == NULL );
		f_selection_owner = this;
		f_owning_selection = selection;
		f_owning_widget = w;
#if defined(__osf__)
		if (fs != NULL)
#else
		if (fs)
#endif /* __osf__ */
#if defined(SC3) || defined (__osf__)
		    (ia->*fs)();
#else
		    (((IcccmAgent*)f_real_object)->*fs)();
#endif
	}
	else {
#if defined(__osf__)
	    if (ff != NULL)
#else
	    if (ff)
#endif /* __osf__ */
#if defined(SC3) || defined (__osf__)
		(ia->*ff)();
#else
		(((IcccmAgent*)f_real_object)->*ff)();
#endif
	}
    }

    f_getting_selection = False;
}

Boolean
IcccmAgent::convertCB(Widget w, Atom* selection, Atom* target,
		Atom* type_return, XtPointer* value_return,
		unsigned long* length_return, int* format_return)
{
    assert( f_selection_owner );
    return f_selection_owner->
		convert_handler(w, *selection, *target, *type_return,
				*value_return, *length_return, *format_return);
}

// convert multibyte string to compound text, returns number of bytes in
// compound text
// NOTE: This routine allocates some space on heap, it's caller's
//       responsibility to free it.
static int
mbstoct(const char* mbs, const char*& ct)
{
    if (mbs == NULL) {
        ct = NULL;
        return -1;
    }

    XTextProperty prop;
    if (XmbTextListToTextProperty(window_system().display(), (char**)&mbs, 1,
                                        XCompoundTextStyle, &prop) != Success) {
        ct = NULL;
        return -2;
    }

    assert( prop.encoding == XA_COMPOUND_TEXT(window_system().display()) );
    ct = (char*)prop.value;
    return ((prop.nitems + 1) * prop.format / 8);
}

// extract latin1 characters from a multibyte string
// NOTE: This routine allocates some space on heap, it's caller's
//       responsibility to free it using XtFree().
static const char*
extract_latin1(const char* mbs)
{
    if (mbs == NULL)
        return NULL;
    char* latin1s = XtMalloc((strlen(mbs) + 1) * sizeof(char));

    const char* src = mbs;
    char* dest = latin1s;
    while (*src) {
        int mb_len = mblen(src, MB_CUR_MAX);
        assert( mb_len > 0 );
        if (mb_len == 1)
            *dest++ = *src++;
        else
            src += mb_len;
    }
    *dest = '\0';
    return latin1s;
}

Boolean
IcccmAgent::convert_handler(Widget w, Atom selection, Atom target,
		Atom& type_return, XtPointer& value_return,
		unsigned long& length_return, int& format_return)
{
  Atom* target_list;
  int* length;

  XSelectionRequestEvent* req = XtGetSelectionRequest(w, selection, NULL);

  if (target == XA_TARGETS(XtDisplay(w))) {
    ON_DEBUG( cout << "selection requested as compound targets" << '\n' << flush; );
    Atom* xmu_targets;
    unsigned long xmu_length;

    XmuConvertStandardSelection(w, req->time, &selection, &target, &type_return,
				(caddr_t*)&xmu_targets, &xmu_length,
				&format_return);
    assert( type_return == XA_ATOM );
    assert( format_return == sizeof(Atom) * 8 );
    // put our targets in the list
#define OLIAS_SUPPORT_TARGETS	4
    length_return = xmu_length + OLIAS_SUPPORT_TARGETS;
    target_list = (Atom*)XtMalloc(sizeof(Atom) * length_return);
    target_list[0] = XA_COMPOUND_TEXT(XtDisplay(w));
    target_list[1] = XA_STRING;
    target_list[2] = XA_TEXT(XtDisplay(w));
    target_list[3] = XA_LENGTH(XtDisplay(w));
    for (unsigned int i = OLIAS_SUPPORT_TARGETS; i < length_return; i++)
	target_list[i] = xmu_targets[i - OLIAS_SUPPORT_TARGETS];
    XtFree((char*)xmu_targets);
    xmu_targets = NULL;
    value_return = (XtPointer)target_list;
    return True;				
  }

  if (target == XA_COMPOUND_TEXT(XtDisplay(w))) {
    ON_DEBUG( cout << "selection requested as compound text" << '\n' << flush; );
    const char* ct;
#if 0
    if ((length_return = mbstoct(selection_text(), ct)) <= 0)
#else
    if ((length_return = mbstoct((((IcccmAgent*)f_real_object)->*f_string_exporter)(), ct)) <= 0)
#endif
	return False;
    type_return = XA_COMPOUND_TEXT(XtDisplay(w));
    value_return = (XtPointer)ct;
    format_return = 8;
    return True;
  }

#ifdef Internationalize
  if (target == XA_STRING) {
    ON_DEBUG( cout << "selection requested as string" << '\n' << flush; );
    type_return = XA_STRING;
#if 0
    value_return = selection_text();
#else
    value_return = (XtPointer)
			(((IcccmAgent*)f_real_object)->*f_string_exporter)();
#endif
    const char* ct = extract_latin1((char*)&value_return);
    XtFree((char*)&value_return);
    value_return = (XtPointer)ct;
    length_return = strlen(ct);
    format_return = 8;
    return True;
  }

  if (target == XA_TEXT(XtDisplay(w))) {
    ON_DEBUG( cout << "selection requested as text" << '\n' << flush; );
    type_return = XA_TEXT(XtDisplay(w));
#if 0
    length_return = selection_text((char*)*value_return);
#else
    value_return = (XtPointer)
			(((IcccmAgent*)f_real_object)->*f_string_exporter)();
    length_return = strlen((char*)value_return);
#endif
    format_return = 8;
    return True;
  }
#else
  if (target == XA_STRING ||
      target == XA_TEXT (XtDisplay (w))) {
    ON_DEBUG( cout << "selection requested as string or text" << '\n' << flush; );
    type_return = XA_STRING;
#if 0
    length_return = selection_text((char*)*value_return);
#else
    //
    //  SWM 950427 -- selection_text is undefined. Looking at the
    //  above code, looks like strlen will do the job.
    //
    length_return = strlen ((char *) value_return);
#endif
    format_return = 8;
    return True;
  }
#endif

  if (target == XA_LENGTH(XtDisplay(w))) {
    ON_DEBUG( cout << "selection requested as length" << '\n' << flush; );

    const char* ct;
    length = (int *)XtMalloc(sizeof(int)); 
    // we should return the length of compound text
    *length = mbstoct((((IcccmAgent*)f_real_object)->*f_string_exporter)(), ct);
    XtFree((char*)ct);

    type_return = XA_INTEGER;
    value_return = (XtPointer)length; 
    length_return = 1;
    format_return = sizeof(int) * 8;
    return True;
  }

  ON_DEBUG( cout << "selection requested as others..." << '\n' << flush; );

  // We couldn't do it, maybe Xmu can
  return(XmuConvertStandardSelection(w, req->time, &selection, &target,
			&type_return, (caddr_t*)value_return, &length_return,
			&format_return));
}

// relinquish the selection voluntarily
void
IcccmAgent::disown_selection(Boolean call_app_handler)
{
    ON_DEBUG(cout << "***** disown_selection *****" << endl;);
    f_voluntary_disown = True; // guard not to execute loseCB

    if (f_selection_owner == this) {
#ifdef DEBUG
	cout << "(DEBUG) disown_selection: call XtDisownSelection" << endl;
#endif
	XtDisownSelection(f_owning_widget, f_owning_selection, CurrentTime);

	if (call_app_handler == True) // application specific handler
	    f_selection_owner->IcccmAgent::lose_selection(f_owning_selection);

	reset_selection_info();
    }
    f_voluntary_disown = False; // reset the guard
}

void
IcccmAgent::reset_selection_info()
{
    f_selection_owner = NULL;
    f_owning_selection = None;
    f_owning_widget = NULL;
}

// dispatch post processing function according to who is getting the selection
void
IcccmAgent::lose_selection(Atom atom)
{
    if (ia_getting_selection() == True)
	turn_over_selection(atom);
    else {
	// NOTE: lose_selection is virtual, if derived does not provide
	//	 lose_selection, it ends in infinite loops!
	lose_selection(atom);
    }
}

// True if some IcccmAgent is about to own selection
Boolean
IcccmAgent::ia_getting_selection()
{
    return (Boolean)f_getting_selection;
}

// loseCB is a callback that is called by XtDisownSelection
void
IcccmAgent::loseCB(Widget, Atom*)
{
    ON_DEBUG(cout << "(DEBUG) ***** loseCB *****" << endl;);

    // if this XtDisownSelection is voluntary, leave things to disown_selection
    if (f_voluntary_disown == True)
	return;

    if (f_selection_owner) {
	// perform application specific post processing
	f_selection_owner->IcccmAgent::lose_selection(f_owning_selection);

	f_selection_owner->reset_selection_info();
    }
    else {
#ifdef DEBUG
	cerr << "(WARNING) loseCB: no selection owner exists" << endl;
#endif
    }
}

Boolean
IcccmAgent::is_selection_owner()
{
    return (f_selection_owner == this);
}
