/*
 * $XConsortium: GraphicAgentMotif.hh /main/11 1996/11/21 11:55:31 cde-hal $
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

#include <WWL/WTopLevelShell.h>
#include "UAS.hh"

class WXmPushButton;
class WXmLabel;
class WPixmap;
class WXawPanner ;
class WXmPulldownMenu ;
class WXmToggleButton;

class PixmapGraphic;
template <class T> class UAS_Pointer;

class GraphicAgent : public Agent,
		     public UAS_Receiver<DisplayGraphic>,
		     public UAS_Receiver<ReAttachGraphic>,
		     public UAS_Receiver<UAS_LibraryDestroyedMsg>
{

public:
  GraphicAgent (UAS_Pointer<UAS_Common> &node_ptr,
		UAS_Pointer<Graphic> &);
  ~GraphicAgent();
  
  void display();
  void popup();

  void receive(DisplayGraphic &, void *client_data);
  void receive(ReAttachGraphic &, void *client_data);
  void receive(UAS_LibraryDestroyedMsg &, void *);

  UAS_Pointer<Graphic> &graphic() { return f_graphic ; }

  void attach_graphic();
  void set_graphic(Graphic *gr) { f_graphic = gr; }
  UAS_Pointer<UAS_Common> node_ptr() { return f_node_ptr; }

private:

  void create_ui();

  void visit_node();
  void close_window();
  void reattach();
  void ui_destroyed();

  void porthole_report(WCallback *);
  void panner_report(WCallback *);

  void adjust_panner(unsigned int state);

  void set_panner(WCallback *);
  void set_zoom(unsigned int scale);

  void zoom_callback(WCallback *);
  void fit_graphic_to_window_callback(WCallback*);
  void fit_window_to_graphic_callback(WCallback*);
  void fit_window_to_graphic();

  void install_new_picture(PixmapGraphic *);

  void refresh(const UAS_Pointer<Graphic> &);

  void unset_scale_buttons();

private:
  WTopLevelShell  	f_shell;
  UAS_Pointer<UAS_Common>	f_node_ptr ;
  UAS_Pointer<Graphic>	f_graphic ;
  WXawPanner           *f_panner;
  WPixmap	       *f_pixmap_widget ;
  unsigned int		f_panner_state ;
  unsigned int          f_current_scale;
  WXmPulldownMenu      *f_view_menu ;
  WXmLabel	       *f_message_area ;
  WXmToggleButton      *f_scale_button ;
  WXmToggleButton      *f_custom_scale ;
  int			f_setcustom ;
};
