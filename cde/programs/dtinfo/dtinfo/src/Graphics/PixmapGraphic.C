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
/*
 * $XConsortium: PixmapGraphic.C /main/8 1996/10/04 11:07:06 drk $
 *
 * Copyright (c) 1992 HaL Computer Systems, Inc.  All rights reserved.
 * UNPUBLISHED -- rights reserved under the Copyright Laws of the United
 * States. Use of a copyright notice is precautionary only and does not
 * imply publication or disclosure.
 * 
 * This software contains confidential information and trade secrets of HaL
 * Computer Systems, Inc.  Use, disclosure, or reproduction is prohibited
 * without the prior express written permission of HaL Computer Systems, Inc.
 * 
 *                         RESTRICTED RIGHTS LEGEND
 * Use, duplication, or disclosure by the Government is subject to
 * restrictions as set forth in subparagraph (c)(l)(ii) of the Rights in
 * Technical Data and Computer Software clause at DFARS 252.227-7013.
 *                        HaL Computer Systems, Inc.
 *                  1315 Dell Avenue, Campbell, CA  95008
 * 
 */

#define C_TOC_Element
#define L_Basic

#define C_WindowSystem
#define L_Other

#define C_PixmapGraphic
#define L_Graphics
#define C_GraphicsMgr
#define L_Managers

// for GraphicsHandler 
#define C_NodeWindowAgent
#define C_GraphicAgent
#define L_Agents

#include "Prelude.h"

// /////////////////////////////////////////////////////////////////////////
// for tml
// /////////////////////////////////////////////////////////////////////////

Drawable
PixmapFromGraphic (void *gr)
{
  return ((Graphic*) gr)->graphic()->pixmap();
}

// /////////////////////////////////////////////////////////////////////////
// regular constructor
// /////////////////////////////////////////////////////////////////////////


PixmapGraphic::PixmapGraphic(Pixmap pixmap, Dimension width, Dimension height,
			     PixmapFlag flag)
: f_pixmap (pixmap),
  f_width (width),
  f_height (height),
  f_flag (flag),
  f_agent(0),
  f_colormap(0),
  f_num_colors(0),
  f_colors(NULL),
  f_tiff_context(NULL)
{
}

// /////////////////////////////////////////////////////////////////////////
// copy constructor
// /////////////////////////////////////////////////////////////////////////

PixmapGraphic::PixmapGraphic(const PixmapGraphic *pg)
: f_pixmap(pg->f_pixmap),
  f_width(pg->f_width),
  f_height(pg->f_height),
  f_flag(pg->f_flag),
  f_agent(pg->f_agent)
{
  // do not want these to be copied
  abort();

}

// /////////////////////////////////////////////////////////////////
// class destructor
// /////////////////////////////////////////////////////////////////

PixmapGraphic::~PixmapGraphic()
{
  if (f_pixmap && (f_flag == NORMAL))
  {
    XFreePixmap (window_system().display(), f_pixmap);
    if (f_colors)
    {
      XFreeColors (window_system().display(), f_colormap,
                   f_colors, f_num_colors, 0 );
      XFree (f_colors);
#ifdef DEBUG
      printf( "pixmap %lx and its colors freed in PixmapGraphic %p\n",
               f_pixmap, this );
#endif
    }
    if (f_tiff_context)
    {
#ifdef DEBUG
      printf( "pixmap %lx and TIFF context %p freed in PixmapGraphic %p\n",
               f_pixmap, f_tiff_context, this );
#endif
      _DtGrDestroyContext(f_tiff_context);
      delete f_tiff_context ;	// free the struct memory we own
    }
  }
  assert(!f_agent);
}

// hold colormap info allocated for the f_pixmap we are maintaining
void
PixmapGraphic::set_colormap( Colormap cm,
                             int      num,
                             Pixel   *pixels,
                             _DtGrContext *tc )
{
  f_colormap = cm;		// the containing colormap
  f_num_colors = num;		// num colors allocated
  f_colors = pixels;		// colors allocated

  f_tiff_context = tc;		// tiff only, in lieu of above values
}

void
PixmapGraphic::register_handler(GraphicsHandler *graphics_handler)
{
  UAS_Sender<DetachGraphic>::request(graphics_handler);
  UAS_Sender<ReAttachGraphic>::request(graphics_handler);

  if (f_agent != NULL)
    graphics_handler->UAS_Sender<DisplayGraphic>::request(f_agent);

}
