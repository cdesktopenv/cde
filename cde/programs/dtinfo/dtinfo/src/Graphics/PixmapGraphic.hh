/*
 * $XConsortium: PixmapGraphic.hh /main/5 1996/09/06 15:32:15 cde-hal $
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

#include "UAS.hh"
// #include "Managers/GraphicsMgr.hh"

class GraphicsMgr;
class GraphicAgent ;
class GraphicsHandler ;
class Graphic;

#include <X11/X.h>
#include <X11/Intrinsic.h>

#include <DtI/GraphicsP.h>

class PixmapGraphic ;
// Message Classed for informing ViewportAgents about attaching and detaching
// their graphics 
class GraphicsMessage
{
public:
  GraphicsMessage( UAS_Pointer<Graphic> &gr )
    : f_graphic(gr)
    {}

  UAS_Pointer<Graphic> &graphic() 
    { return f_graphic ; }

private:
  UAS_Pointer<Graphic>	f_graphic ;
};

class DetachGraphic : public GraphicsMessage
{
public:
  DetachGraphic(UAS_Pointer<Graphic> &gr,
		GraphicAgent *ga)
    : GraphicsMessage(gr),
      f_graphic_agent(ga)
    {}

  GraphicAgent *agent()	{ return f_graphic_agent ; }

private:
  GraphicAgent  *f_graphic_agent ;
};

class ReAttachGraphic : public GraphicsMessage
{

public:
  ReAttachGraphic( UAS_Pointer<Graphic> &gr)
    : GraphicsMessage(gr)
    {}
};

class DisplayGraphic
{
public:
  DisplayGraphic( UAS_Pointer<Graphic> &g)
    : f_graphic(g)
    {}
  UAS_Pointer<Graphic>	&graphic() { return f_graphic ;}
private:
  UAS_Pointer<Graphic>	f_graphic ;
};

class PixmapGraphic : public UAS_Sender<DetachGraphic>,
		      public UAS_Sender<ReAttachGraphic>
{
public:
  enum PixmapFlag { NORMAL, DETACHED_PIXMAP, DEFAULT_PIXMAP, PRESERVE } ;

  PixmapGraphic (Pixmap pixmap,
		 Dimension width, Dimension height,
		 PixmapFlag);

  virtual ~PixmapGraphic();

  void set_colormap( Colormap, int, Pixel *, _DtGrContext * );

  virtual bool isPostScriptGraphic() const { return FALSE ; } 

  // member access functions 
  Pixmap pixmap() const
    { return f_pixmap; }

  Dimension width() const
    { return f_width; }

  Dimension height() const
    { return f_height; }

  void destroy() ;

  bool is_detached() const { return f_flag == DETACHED_PIXMAP ; }

  void set_type(PixmapFlag f) { f_flag = f ; }

  void register_handler(GraphicsHandler *);

  GraphicAgent *agent() { return f_agent; }
  GraphicAgent *agent(GraphicAgent *agent)
    { f_agent = agent ; return agent; }

protected:
  // copy constructor
  PixmapGraphic(const PixmapGraphic *);
  
protected: // variables
  Pixmap      		f_pixmap;
  Dimension   		f_width;
  Dimension   		f_height;
  PixmapFlag		f_flag ; 
  GraphicAgent	       *f_agent ;

  // colormap info allocated for the above f_pixmap:
  Colormap		f_colormap;	// containing colormap
  int			f_num_colors;	// num colors allocated
  Pixel		       *f_colors;	// colors allocated
  _DtGrContext	       *f_tiff_context; // colors if tiff type graphic

};

