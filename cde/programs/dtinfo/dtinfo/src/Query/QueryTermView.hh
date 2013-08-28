/*	Copyright (c) 1994 FUJITSU LIMITED	*/
/*	All Rights Reserved			*/

/*
 * $XConsortium: QueryTermView.hh /main/4 1996/08/06 09:20:28 rcs $
 *
 * Copyright (c) 1992 HAL Computer Systems International, Ltd.
 * All rights reserved.  Unpublished -- rights reserved under
 * the Copyright Laws of the United States.  USE OF A COPYRIGHT
 * NOTICE IS PRECAUTIONARY ONLY AND DOES NOT IMPLY PUBLICATION
 * OR DISCLOSURE.
 * 
 * THIS SOFTWARE CONTAINS CONFIDENTIAL INFORMATION AND TRADE
 * SECRETS OF HAL COMPUTER SYSTEMS INTERNATIONAL, LTD.  USE,
 * DISCLOSURE, OR REPRODUCTION IS PROHIBITED WITHOUT THE
 * PRIOR EXPRESS WRITTEN PERMISSION OF HAL COMPUTER SYSTEMS
 * INTERNATIONAL, LTD.
 * 
 *                         RESTRICTED RIGHTS LEGEND
 * Use, duplication, or disclosure by the Government is subject
 * to the restrictions as set forth in subparagraph (c)(l)(ii)
 * of the Rights in Technical Data and Computer Software clause
 * at DFARS 252.227-7013.
 *
 *          HAL COMPUTER SYSTEMS INTERNATIONAL, LTD.
 *                  1315 Dell Avenue
 *                  Campbell, CA  95008
 * 
 */


class QueryTerm;
class QueryGroupView;

#include <WWL/WXmForm.h>
#include <WWL/WXmTextField.h>
#include <WWL/WXmToggleButton.h>
#include <WWL/WXmArrowButton.h>
#include <WWL/WXmLabelGadget.h>
#include <WWL/WXmMenu.h>

class QueryTermView : public WWL, public FolioObject
{
public:
  QueryTermView (QueryTerm *query_term, QueryGroupView *parent,
		 QueryTermView *previous, QueryTermView *next);
  ~QueryTermView();
  
  void destroy_widgets()
    { f_form.Destroy(); }

  void invert_colors();

  QueryTerm *query_term()
    { return (f_query_term); }

  void traverse_here()
    {   f_form.InitialFocus (f_term_widget);
	XmProcessTraversal (f_term_widget, XmTRAVERSE_CURRENT); }

//#ifdef UseFJMTF
#if 1
  QueryTermView* next_term_view() const { return f_next; }
#endif

private:
  void create_ui();
  void create_fields();
  void select_toggle (WCallback *);
  void pw_activate (WCallback *);
  static void _select (Widget, XEvent *, String *, Cardinal *);
  void register_actions();
  void set_prefix (WCallback *);
  void set_connective (WCallback *);
  void verify_term (WCallback *);
  void term_changed();
#ifdef UseQSearch
  void weight_modified(WCallback *);
#else
  void proximity_changed();
#endif
  void weight_changed();
  void set_pws_label();
#ifndef UseQSearch
  void prev_connective_changed();
#endif
  void verify_numeric (WCallback *wcb);

  // This function takes advantage of the fact that the first thing in a
  // widget is a pointer to itself.  So if the union contains a widget,
  // dereferencing it gives it's address.  This cannot be true of the
  //  QueryGroupView object.  Therefore we avoid an extra type field. 
  enum term_type_t { TYPE_TERM, TYPE_GROUP };
  term_type_t term_type()
    { return ((f_term_widget == (Widget)*f_term_field) ? TYPE_TERM : TYPE_GROUP); }

private:
#ifdef UseQSearch
  QueryTerm      *f_query_term;
  WXmForm         f_form;
  WXmForm         f_pws_form;
  WXmTextField    f_weight_field;
  WXmLabelGadget  f_weight_label;
  QueryGroupView *f_parent;
  QueryTermView  *f_previous;
  QueryTermView  *f_next;
  WXmOptionMenu   f_prefix;
  union {
    WXmTextField   *f_term_field;
    Widget          f_term_widget;
    QueryGroupView *f_group_view;
  };
  WXmArrowButton  f_pw_button;
  WXmOptionMenu   f_connective;
#else
  QueryTerm      *f_query_term;
  WXmForm         f_form;
  WXmForm         f_pws_form;
  WXmLabelGadget  f_proximity_label;
  WXmTextField    f_proximity_field;
  QueryGroupView *f_parent;
  QueryTermView  *f_previous;
  QueryTermView  *f_next;
  WXmToggleButton f_select_toggle; // What is this for? - 6/28/94 kamiya
  WXmOptionMenu   f_prefix;
  union {
    WXmTextField   *f_term_field;
    Widget          f_term_widget;
    QueryGroupView *f_group_view;
  };
  WXmArrowButton  f_pw_button;
  WXmOptionMenu   f_connective;
  WXmTextField    f_weight_field;
  WXmTextField    f_scope_field;
#endif
};
