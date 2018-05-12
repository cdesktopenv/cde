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
/*      Copyright (c) 1994 FUJITSU LIMITED      */
/*      All Rights Reserved                     */

/*
 * $XConsortium: PrefAgentMotif.hh /main/3 1996/06/11 16:15:29 cde-hal $
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

#include <WWL/WTopLevelShell.h>
#include <WWL/WXmPushButton.h>
#include <WWL/WXmTextField.h>
#include <WWL/WXmToggleButton.h>
#ifdef UseWideChars
#include <WWL/WXmToggleButtonGadget.h>
#endif

class ChangeData;
class WTimeOut;
class WindowGeometry;

class PrefAgent : public Agent
{
public:
  PrefAgent();
  ~PrefAgent();

  void display();

private:
  void create_ui();

  bool change_field (ChangeData *);

  void init_values();
  void set_update (bool);
  void set_auto_display (bool);
#ifdef UseWideChars
  void set_hyphen (XtPointer); // 4/8/94 kamiya
#endif
  void set_lock (bool);
  void set_geometry (WXmTextField &, WindowGeometry &);
  void set_integer (WXmTextField &, int);

  /* -------- callbacks -------- */

  void something_changed();
  void switch_to_window (WCallback *);
  void arm_arrow (WCallback *);
  void disarm_arrow();
  void change_cb (WCallback *);
  void change_timeout (WTimeOut *);
  void lock_toggle (WCallback *);
  void update_toggle (WCallback *);
  void adisplay_toggle (WCallback *);
#ifdef UseWideChars
  void hyphen_toggle (WCallback *); // 4/8/94 kamiya
#endif
  void get_geometry (WCallback *);

  void ok();
  void apply();
  void reset();
  void cancel();

  static void dialog_mapped(Widget, XtPointer, XEvent *, Boolean *);

private:
  WTopLevelShell f_shell;

  WXmPushButton f_ok;
  WXmPushButton f_apply;
  WXmPushButton f_reset;

  Widget        f_top_panel;

  WXmTextField     f_browse_geo;
  WXmTextField     f_fs_field;
  WXmToggleButton  f_lock_toggle;
  WXmTextField     f_max_field;
  WXmTextField     f_map_geo;
  WXmTextField     f_nh_field;
  WXmTextField     f_sh_field;
  WXmToggleButton  f_update_toggle;
#ifdef UseWideChars
  WXmTextField     f_hyphen_level_field; // hyphenation level - 4/8/94 kamiya
  WXmToggleButtonGadget f_hyphen_up; // wrap-up hyphenation method - 4/11/94 kamiya
  WXmToggleButtonGadget f_hyphen_down; // wrap-down hyphenation method - 4/11/94 kamiya
#endif
  WXmTextField     f_max_hits_field;
  WXmToggleButton  f_adisplay_toggle;

  WTimeOut *f_timeout;
};
