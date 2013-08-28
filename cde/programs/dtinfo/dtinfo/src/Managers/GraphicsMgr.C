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
/*	Copyright (c) 1994 FUJITSU LIMITED	*/
/*	All Rights Reserved			*/

/*
 * $TOG: GraphicsMgr.C /main/29 1998/04/17 11:36:15 mgreess $
 *
 * Copyright (c) 1992 HaL Computer Systems, Inc.  All rights reserved.
 * UNPUBLISHED -- rights reserved under the Copyright Laws of the United
 * States.  Use of a copyright notice is precautionary only and does not
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

#include <DtI/GraphicsP.h>

#define C_PixmapGraphic
#define L_Graphics

#define C_GraphicsMgr
#define C_MessageMgr
#define C_EnvMgr
#define L_Managers

#ifdef UseDlOpen
#define C_XpmLib
#endif
#define C_WindowSystem
#define L_Other

#define C_Database
#define C_OString
#define L_Basic

#define C_GraphicAgent
#define C_ViewportAgent
#define L_Agents

#include "Prelude.h"

#include <iostream>
using namespace std;
extern "C"
{
#include "../cgm/spec.h"
}
#include "utility/funcs.h"

LONG_LIVED_CC(GraphicsMgr,graphics_mgr);

// ////////////////////////////////////////////////////////////
// class constructor
// ////////////////////////////////////////////////////////////

GraphicsMgr::GraphicsMgr ()
{
}

// ////////////////////////////////////////////////////////////
// class destructor
// ////////////////////////////////////////////////////////////

GraphicsMgr::~GraphicsMgr ()
{
}

// /////////////////////////////////////////////////////////////////
// get_graphic - get a pixmap given a locator and a scale factor
// /////////////////////////////////////////////////////////////////

PixmapGraphic *
GraphicsMgr::get_graphic (UAS_Pointer<Graphic> &gr)
{    
    extern AppPrintData * l_AppPrintData;
    int resolution = 0;
    String string_resolution ;

    // if printing, get print resolution from default-printer-resolution

    if (window_system().printing()) {
	string_resolution = XpGetOneAttribute(window_system().printDisplay(), 
					      l_AppPrintData->f_print_data->print_context,
					      XPDocAttr, (char*)"default-printer-resolution") ;
	resolution = atoi(string_resolution);
	XFree(string_resolution);	
    }

    // if not printing, or default-printer-resolution not defined, calcuate from 
    // the X screen resolution

    if (!resolution) {

	// calculate the display (or printer) resolution in dots per inch.
	// 25.4mm = 1 inch
	resolution = 
	    (254 * WidthOfScreen(window_system().screen()) +
	     5 * WidthMMOfScreen(window_system().screen())) /
	    (10 * WidthMMOfScreen(window_system().screen())) ;

        // cgm needs resolution to be 100--this is to fix
        // fit_graphic_to_window problem.
        UAS_String cgm_type("application/CGM");
        if(cgm_type == gr->content_type())
          resolution = 100;

    }

    return get_graphic(gr->data(), gr->data_length(), gr->content_type(), resolution);
}

// /////////////////////////////////////////////////////////////////
// get_graphic - get a pixmap given a locator
// /////////////////////////////////////////////////////////////////

PixmapGraphic *
GraphicsMgr::get_graphic (const UAS_String &imdata,
                          unsigned int length,
                          const UAS_String &imtype,
                          unsigned short scale_factor)
{
  //ON_DEBUG(cerr << "GraphicsMgr::get_graphic( "  << gr->locator() << " )" << endl);
  //ON_DEBUG(cerr << "type = " << gr->content_type() << endl);
  ON_DEBUG(cerr << "type = " << imtype << endl);

  // made changes to the interface to allow the image data to be passed in instead
  // of the graphic object. this is to allow the image data to be saved before
  // it is passed to this method. when scaling is done, need to be able to scale
  // the original data each time. this is why the image data is saved before calling
  // this method. EAM - 6/27/96
  static int initialized=0;

  if(!initialized)
  {
    _DtGrRegisterConverter((char*)"CGM", processCGM, NULL, NULL, NULL);
    initialized = 1;
  }


  UAS_String content_type(imtype);
  char *image_type;
  // if content_type is a NULL string, image type is unknown
  if (content_type == "")
  {
    image_type = (char*)"unknown";
  } 
  else
  {
    image_type = content_type;
    image_type = strchr(image_type, '/');

    if (image_type)
      image_type++;		// advance past the '/'
  }

  // this is called by a graphic to retrieve the pixmap 

  PixmapGraphic *the_pixmap = NULL ;

  Dimension width = 0 ;
  Dimension height = 0;

  //if (gr == (const int)NULL)
  if ((char*)imdata == (char*)NULL)
    {
      fprintf (stderr, "*** Graphic not found in database ***\n");
      throw (CASTEXCEPT Exception());
    }

  // setup the DtGrStream Buffer

  _DtGrStream gr_stream ;
  
  UAS_String base(imdata);

  gr_stream.type = _DtGrBUFFER ;
  gr_stream.source.buffer.base = base ;
  gr_stream.source.buffer.size = length;
  gr_stream.source.buffer.current = (char*)gr_stream.source.buffer.base ;
  gr_stream.source.buffer.end = (char *)(gr_stream.source.buffer.base + gr_stream.source.buffer.size) ;

  // get some basic graphical info to pass to the routine

  int depth ;
  Screen   *screen ;
  Visual   *visual ;
  Colormap colormap ;

  Pixel foreground, background ;


  Arg arg[6] ;
  unsigned n = 0;

  XtSetArg(arg[n], XmNdepth,    &depth)    ; n++ ;
  XtSetArg(arg[n], XmNcolormap, &colormap) ; n++ ;
  XtSetArg(arg[n], XmNscreen,   &screen)   ; n++ ;
  XtSetArg(arg[n], XmNvisual,   &visual)   ; n++ ;
  XtSetArg(arg[n], XmNforeground, &foreground) ; n++ ;
  XtSetArg(arg[n], XmNbackground, &background) ; n++ ;

  XtGetValues(window_system().toplevel(), arg, n) ;

  if (visual == CopyFromParent) {
      visual= DefaultVisualOfScreen(screen);
  }

#ifdef DEBUG
  cerr << "depth: " << depth << "\tcolormap: " << colormap <<
    "\tscreen: " << screen << "\tvisual: " << visual << endl;
#endif

  // returned values - 
  int   ret_num_colors = 0;
  Pixel *ret_colors = 0;
  Pixmap ret_pixmap = 0;
  Pixmap ret_mask = 0;
  
  if (!strcasecmp (image_type, "unknown")) {
    // If it is unknown at this point then it needs to be nulled out so the
    // graphics api will determine the type.
    image_type = NULL;
  }

  if (!strcasecmp (image_type, "XBM")) {
    // match up infolib name for XBM to DtGr library
    image_type = (char*)"BM";
  }

  if (!strcasecmp (image_type, "XPM")) {
    // match up infolib name for XPM to DtGr library
    image_type = (char*)"PM";
  }

#ifdef XWD_TEST
  if (!image_type || !strcasecmp (image_type, "unknown"))
    image_type = (char*)"XWD" ;
#endif

  // Context struct is required for TIFF data type only
  _DtGrContext *ret_context = NULL ;
  if (!strcasecmp (image_type, "TIFF")) {
    ret_context = new _DtGrContext ;
    ret_context->image_type = NULL ;
    ret_context->context    = NULL ;
  }


  _DtGrLoadStatus status = 
    _DtGrLoad(&gr_stream,	// image
	      &image_type,
	      screen,
	      depth,
	      colormap,
	      visual,
	      foreground,
	      background,
	      XDefaultGCOfScreen(screen),
	      _DtGrCOLOR,	// color_model
	      FALSE,		// don't allow reduced colors
	      &width,
	      &height,
	      scale_factor,	// media_resolution
	      &ret_pixmap,
	      &ret_mask,
	      &ret_colors,
	      &ret_num_colors,
	      ret_context);	


  if (status == _DtGrCOLOR_FAILED)
     {
#ifdef DEBUG
        printf(
       "Insufficient color cells for graphic. Retry with degraded colors.\n" );
#endif
        status =
          _DtGrLoad(&gr_stream, // image
              &image_type,
              screen,
              depth,
              colormap,
              visual,
              foreground,
              background,
              XDefaultGCOfScreen(screen),
              _DtGrCOLOR,       // color_model
              TRUE,             // allow reduced colors
              &width,
              &height,
              scale_factor,             // media_resolution
              &ret_pixmap,
              &ret_mask,
              &ret_colors,
              &ret_num_colors,
              ret_context);
     }

	      
  if ((status == _DtGrSUCCESS) ||
      (status == _DtGrCOLOR_REDUCE))
  {
      // get rid of memory the routine may have allocated for XPM,
      // but which is not needed 
      if (ret_mask) {
	  XFreePixmap (window_system().display(), ret_mask);
      }
      
      // build our pixmap object
      the_pixmap = new PixmapGraphic(ret_pixmap,
                            width, height, PixmapGraphic::NORMAL);
#ifdef DEBUG
      printf(
      "pixmap %lx used to create PixmapGraphic %p\n", ret_pixmap, the_pixmap );
#endif

      // set the associated colormap reservations so they can be freed later.
      // Note static local struct ptr ret_context is used for TIFF.
      the_pixmap->set_colormap( colormap, ret_num_colors, ret_colors,
                                ret_context );
    }
#ifdef DEBUG
  {
    const char *statusv ;
    switch (status)
      {
      case _DtGrSUCCESS:
	statusv = " _DtGrSUCCESS";
	break;
      case _DtGrCOLOR_REDUCE:
	statusv = "_DtGrCOLOR_REDUCE";
	break;
      case _DtGrCONVERT_FAILURE:
	statusv = " _DtGrCONVERT_FAILURE";
	break;
      case _DtGrOPEN_FAILED:
	statusv = "_DtGrOPEN_FAILED";
	break;
      case _DtGrFILE_INVALID:
	statusv = "_DtGrFILE_INVALID";
	break;
      case _DtGrNO_MEMORY:
	statusv = "_DtGrNO_MEMORY";
	break ;
      case _DtGrCOLOR_FAILED:
	statusv = "_DtGrCOLOR_FAILED";
	break;
      default:
	statusv = "Unknown"  ;
      }
    cerr << "status = " << statusv << " " << status << endl ;
  }
#endif  


  /* -------- Fallback by reading in default graphic. -------- */
  if (the_pixmap == NULL)
   {
     Pixmap pixmap = window_system().default_pixmap(&width, &height); 

     if (pixmap)
       the_pixmap = new PixmapGraphic(pixmap,
				      width, height,
				      PixmapGraphic::DEFAULT_PIXMAP);
   }

  return the_pixmap ;
}



// /////////////////////////////////////////////////////////////////////////
// detach
// /////////////////////////////////////////////////////////////////////////

void
GraphicsMgr::detach (UAS_Pointer<UAS_Common> &node_ptr,
		     UAS_Pointer<Graphic> &gr)
{
  // called from ViewportAgent
  // ViewportAgent will actually remove it from display

  gr->set_detached(TRUE);
  //PixmapGraphic *pgraphic = gr->pixmap_graphic();
  GraphicAgent *ga = new GraphicAgent(node_ptr, gr);

  //gr->set_agent(ga);

  // add graphic agent to list if not already there.
  add_detached(ga);

  DetachGraphic dg(gr, ga);
  //pgraphic->UAS_Sender<DetachGraphic>::send_message(dg);
  UAS_Sender<DetachGraphic>::send_message(dg);

  ga->display() ;

}

PixmapGraphic *
GraphicsMgr::detached_graphic()
{
    Dimension width, height;
  
    Pixmap pixmap = window_system().detached_pixmap (&width, &height); 
    
    return new PixmapGraphic(pixmap, width, height,
			     PixmapGraphic::DETACHED_PIXMAP);

}

void
GraphicsMgr::reattach_graphic(UAS_Pointer<Graphic> &gr)
{
  Wait_Cursor bob;
  gr->set_detached(FALSE);
  ReAttachGraphic rg(gr);
  UAS_Sender<ReAttachGraphic>::send_message(rg);
  UAS_String locator_str = gr->locator();
  GraphicAgent *ga = get_agent(locator_str);
  remove_detached(ga);

  delete ga;
}

void
GraphicsMgr::uncache(Graphic *gr)
{
  ON_DEBUG(cerr << "GraphicsMgr uncache: " << gr << endl);
  List_Iterator<Graphic* > cursor(&f_graphic_cache) ;
  for (; cursor; cursor++)
    {
      if (cursor.item() == gr)
	{
	  ON_DEBUG(cerr << "found...uncache" << endl);
	  f_graphic_cache.remove(cursor);
	  break;		// exit, return 
	}
    }
}

UAS_Pointer<Graphic>
GraphicsMgr::get(UAS_Pointer<UAS_Common> &node_ptr, const char *locator)
{

#if 0
  // iterate through the list looking for our graphic.

  List_Iterator<Graphic * > cursor (&f_graphic_cache);

  for (; cursor; cursor++)
    if (cursor.item()->locator() == locator)
      return cursor.item();
#endif

  //Graphic *gr = new Graphic (node_ptr, locator);
  UAS_Pointer<Graphic> gr(new Graphic (node_ptr, locator));

#if 0
  f_graphic_cache.insert (gr);
#endif

  if(is_detached(gr))
  {
    //init_gr(gr);
    gr->set_detached(TRUE);
  }

  return gr;
}

GraphicAgent*
GraphicsMgr::get_agent(UAS_String &locator)
{
  // iterate through the list looking for our graphic.
  // if our graphic is in the list, return it.
  // Otherwise, return NULL.
  List_Iterator<GraphicAgent*> dgl (&f_detached_list);

  for (; dgl; dgl++)
  {
    UAS_Pointer<Graphic> gr(dgl.item()->graphic());
    if (gr->locator() == locator)
      return dgl.item();
  }
  return NULL;
}

void
GraphicsMgr::add_detached(GraphicAgent *ga)
{
  UAS_Pointer<Graphic> gr(ga->graphic());
  if(!is_detached(gr))
    f_detached_list.insert(ga);
  
}

void
GraphicsMgr::remove_detached(GraphicAgent *ga)
{
  // iterate through the list looking for our graphic.
  // if our graphic is in the list, return it.
  // Otherwise, return NULL.
  List_Iterator<GraphicAgent*> dgl (&f_detached_list);
  if (ga == NULL)
  {
    return;
  }
  UAS_Pointer<Graphic> oldgr = ga->graphic();

  for (; dgl; dgl++)
  {
    UAS_Pointer<Graphic> gr(dgl.item()->graphic());
    if (gr->locator() == oldgr->locator())
    {
      f_detached_list.remove(ga);
      break;
    }
  }
}

bool
GraphicsMgr::is_detached(UAS_Pointer<Graphic> &gr)
{
  // iterate through the list looking for our graphic.
  // if our graphic is in the list, return True.
  // Otherwise, return False.
  List_Iterator<GraphicAgent*> dgl (&f_detached_list);

  for (; dgl; dgl++)
  {
   UAS_Pointer<Graphic> g = dgl.item()->graphic();
    if (gr->locator() == g->locator())
    {
      return True;
    }
  }
  return False;
}

void
GraphicsMgr::init_gr(UAS_Pointer<Graphic> &gr)
{
  //gr->detached_graphic();
  //gr->pixmap_graphic();
  gr->set_detached(TRUE);
  //ga->set_graphic(gr);
}

