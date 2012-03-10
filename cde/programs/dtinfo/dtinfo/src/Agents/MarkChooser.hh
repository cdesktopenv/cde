/*
 * $XConsortium: MarkChooser.hh /main/5 1996/06/11 16:13:43 cde-hal $
 *
 * Copyright (c) 1993 HAL Computer Systems International, Ltd.
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


#include <WWL/wwl.h>
#include <WWL/WXmForm.h>
#include <WWL/WXmList.h>
#include <WWL/WXmPushButton.h>

class MarkCanvas;

class MarkChooser : public WWL
{
public:
  MarkChooser (Widget parent, xList<MarkCanvas *> &marks,
	       const char *title_key, const char *ok_key);
  ~MarkChooser();
  
  MarkCanvas *get_choice();
  
private:  // functions
  void create_ui (Widget parent, const char *title_key, const char *ok_key);
  void update_list();
  void select (WCallback *);
  void ok();
  void cancel();
  void help();

private:  // variables 
  WXmForm           f_form;
  WXmList           f_list;
  WXmPushButton     f_ok;
  int               f_selected_item;
  bool              f_done;
  xList<MarkCanvas *> *f_mark_list;
};
