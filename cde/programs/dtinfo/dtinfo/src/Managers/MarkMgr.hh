/*
 * $XConsortium: MarkMgr.hh /main/3 1996/06/11 16:26:32 cde-hal $
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

#include "UAS.hh"


class Mark;
class MarkBase;
class Anchor;
class MarkListView;

class MarkMgr : public Long_Lived
{
public:
  MarkMgr();
  ~MarkMgr();

  STATIC_SENDER_HH (MarkCreated);
  STATIC_SENDER_HH (MarkMoved);
  
  UAS_Pointer<Mark> create_mark (UAS_Pointer<UAS_Common> &, const Anchor &,
			     const char *name, const char *notes = "");
  UAS_Pointer<Mark> move_mark (UAS_Pointer<UAS_Common> &, const Anchor &,
			   UAS_Pointer<Mark> &old_mark);

  void open_base (const char *filename);

  void get_marks (UAS_Pointer<UAS_Common> &doc_ptr, xList<UAS_Pointer<Mark> > &list);

  void get_all_marks (xList<UAS_Pointer<Mark> > &list);


  void display_mark_list();

  void finalize ();

  /* Eventually:
     close_base
     set_save_base
  */

private:
  xList<MarkBase *> f_base_list;
  unsigned int      f_num_bases;
  bool              f_can_create;

  MarkListView     *f_mark_list_view;

  LONG_LIVED_HH (MarkMgr,mark_mgr);
};

LONG_LIVED_HH2 (MarkMgr,mark_mgr);
