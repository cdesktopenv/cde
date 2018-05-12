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
/*	copyright (c) 1996 FUJITSU LIMITED	*/
/*	All Rights Reserved			*/

/*
 * $XConsortium: NodeViewInfo.hh /main/9 1996/10/18 19:47:58 cde-hal $
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

#include <X11/Xlib.h>
#include <X11/Intrinsic.h>

#include <Dt/CanvasP.h>
#include <Dt/CanvasSegP.h>

#include "dti_cc/CC_Dlist.h"

#include "UAS/DtSR/Util_Classes/Dict.hh"

class Graphic;

// wrap the canvas segment for search hit usage
class sr_DtCvSegment
{
public:
  _DtCvSegment* hitseg;
  sr_DtCvSegment( _DtCvSegment* pseg) { hitseg = pseg ; };
  ~sr_DtCvSegment() {};
  int operator==(const sr_DtCvSegment& psg) const
     { return psg.hitseg == hitseg; }
  int operator!=(const sr_DtCvSegment& psg) const
     { return psg.hitseg != hitseg; }
};

class NodeViewInfo
{
public:
  NodeViewInfo (UAS_Pointer<UAS_Common> &node_ptr, _DtCvTopicInfo *topic = 0);
  ~NodeViewInfo();

  UAS_Pointer<UAS_Common> &node_ptr()
    { return (f_node_ptr); }

  _DtCvTopicInfo *topic()	{ return f_topic; }

  _DtCvTopicInfo *topic(_DtCvTopicInfo *topic)
    { f_topic = topic ; return topic; }

#ifdef NotDefined
  HashTbl &locator_table()
    { return (f_locator_table); }
#endif
  
  void add_graphic(UAS_Pointer<Graphic> &);

  xList<UAS_Pointer<Graphic> > &graphics()
    { return f_pixmap_list ; } 

  void clear_search_hits();
  void set_search_hits(UAS_Pointer<UAS_List<UAS_TextRun> > &);

  enum trav_dir_t { PREV, NEXT };
  enum trav_status_t { SUCCESS, NOT_MOVED, REACH_LIMIT };
  trav_status_t  adjust_current_search_hit(trav_dir_t);

  int hit_entries() const;
  int search_hit_idx();

  void comp_pixel_values(Display* dpy, Colormap& cmap);

  // retrieve the segment which matches to vcc
  _DtCvSegment* get_segment(unsigned int vcc);

  static unsigned int segment_to_vcc(_DtCvSegment* seg);

private:

  UAS_Pointer<UAS_Common>  f_node_ptr;        /* pointer to the associated node       */
  _DtCvTopicInfo *f_topic;	    /* pointer to the TML model of it       */

#ifdef NotDefined
  HashTbl        f_locator_table;   /* quick lookup table for node locators */
#endif
  xList<UAS_Pointer<Graphic> > f_pixmap_list; /* graphics processing */

  CC_TPtrDlist<sr_DtCvSegment> f_hits;
  sr_DtCvSegment* f_current_hit;

  _DtCvSegment* top_container();

  UAS_String    f_def_key;
  unsigned long f_def_val;
  Dict<UAS_String, unsigned long> f_color_dict;
  void comp_pixel_values_traverse(_DtCvSegment*, Display*, Colormap&);
};
