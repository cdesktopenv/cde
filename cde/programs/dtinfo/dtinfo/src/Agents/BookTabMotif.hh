/*
 * $XConsortium: BookTabMotif.hh /main/3 1996/06/11 16:11:31 cde-hal $
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

#include "Widgets/WXyzTab.h"
#include "UAS.hh"

class NodeWindowAgent;
class NodeHandle;

class BookTab : public WWL, public WXyzTab, public FolioObject
{
public: // functions 
  BookTab (NodeWindowAgent *, const WComposite &parent,
	   UAS_Pointer<UAS_Common> &tab_ptr);
  ~BookTab();

  void set_tab (UAS_Pointer<UAS_Common> &tab_ptr);
  UAS_Pointer<UAS_Common> &tab()
    { return (f_tab_ptr); }

  void select_if_same (UAS_Pointer<UAS_Common> &);

private:  // functions
  // Callbacks
  void activate();
  void select();
  void deselect();

private: // variables
  NodeWindowAgent *f_node_window_agent;
  UAS_Pointer<UAS_Common>     f_tab_ptr;
  bool             f_selected;
};
