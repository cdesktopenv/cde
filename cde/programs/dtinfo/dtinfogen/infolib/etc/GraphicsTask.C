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
/* $XConsortium: GraphicsTask.cc /main/8 1996/08/14 16:59:38 rcs $ */
#include <assert.h>
#include <sstream>
#include <iostream>
using namespace std;

#include <sys/types.h>
#include <netinet/in.h>

#include "GraphicsTask.h"
#include "Dispatch.h"
#include "FlexBuffer.h"
#include "OLAF.h"
#include "OL-Data.h"
#include "Token.h"
#include "SearchEng.h"
#include "BookTasks.h"
#include "NodeData.h"
#include "NodeTask.h"
#include "DataBase.h"
#include "BookCaseDB.h"
#ifndef DtinfoClient
#include "PostScript.h"
#endif
#include "SearchPath.h"
#include "utility/funcs.h"

#include <X11/XWDFile.h>

GR_TYPE gtype;
extern FILE *graphics_taskin;
extern void get_type ();

const int GR_ENCODING_TEXT = 1;

//-------------------------------------------------------------------------
#if __GNUC__ == 4 && __GNUC_MINOR__ == 4
GR_TYPE __attribute__((optimize("O0")))
#else
GR_TYPE
#endif
GraphicsTask::graphics_type( const char *gname )
{

  gtype = GR_TYPE_UNKNOWN;
  if ( !(graphics_taskin  = fopen( gname, "r" )) ) {
    throw(PosixError( errno, form("cannot open graphics file %s\n", gname)));
  }

  get_type();

  if (gtype == GR_TYPE_UNKNOWN) {
    if ((strstr(gname, ".cgm") != NULL) ||
        (strstr(gname, ".CGM") != NULL))
    {
      gtype = GR_TYPE_CGM;
    }
  }

  fclose(graphics_taskin); 

  // If the file is not cgm and if the lexer couldn't detect the graphics type,
  // see if the graphic is an XWD file.
  if (gtype == GR_TYPE_UNKNOWN) {
    // Create header container
    CARD32 header[sz_XWDheader];
    XWDFileHeader *hdrptr;
    FILE *fp;

    if ( !(fp = fopen( gname, "r" )) ) {
      throw(PosixError( errno, 
			form("cannot reopen graphics file %s\n", gname)));
    }
    
    hdrptr = (XWDFileHeader *)(void*)header;
    // Initialize the structure
    for (CARD32 ndx = 0; ndx < sz_XWDheader; ndx++) {
      header[ndx] = (CARD32) 0;
    }

    if ( fread( &header[0], sizeof(*hdrptr), 1, fp ) == 1 ) {
       
	// Normal fields into host byte-order
	hdrptr->file_version = ntohl(hdrptr->file_version);
	hdrptr->header_size = ntohl(hdrptr->header_size);
	hdrptr->pixmap_width = ntohl(hdrptr->pixmap_width);
	hdrptr->pixmap_height = ntohl(hdrptr->pixmap_height);

	// Check some of the structure members to confirm the file type.
	if ((hdrptr->file_version == XWD_FILE_VERSION) &&
	    (hdrptr->header_size > 0) &&
	    (hdrptr->pixmap_width > 0) &&
	    (hdrptr->pixmap_height > 0)) {
	    gtype = GR_TYPE_XWD;
	}
    }
    fclose( fp );
  }

  return ( gtype );
}

//-------------------------------------------------------------------------
GraphicsTask::GraphicsTask( SearchEngine *parent, const Token &t )
{
  f_base = t.level();
  graphics_data = NULL;
  f_parent = parent;
  termsbuf = NULL;
  f_title  = 0;

  if ( Dispatch::OutsideIgnoreScope() ) {
    graphics_data = new OL_Data ( t, OLAF::OL_Graphic );
    addSubTask( graphics_data );

    if ( t.LookupAttr( OLAF::OL_Title ) ) {
      f_title = new OL_Data(t, OLAF::OL_Title, IGNORE_ON );
      if ( !f_title->DataWillBeAvailable() ) {
	delete f_title;
	f_title = 0;
      }
      else {
	addSubTask(f_title);
      }
    }
      
  }
}

//-------------------------------------------------------------------------
void
GraphicsTask::markup( const Token &t )
{
  if ( f_base > 0 ) {

    ComplexTask::markup( t );

    if ( t.type() == START && !f_title ) {

      if ( t.LookupAttr( OLAF::OL_Title ) ) {
	f_title = new OL_Data(t, OLAF::OL_Title, IGNORE_ON );
	if ( !f_title->DataWillBeAvailable() ) {
	  delete f_title;
	  f_title = 0;
	}
	else {
	  addSubTask(f_title);
	}
      }
    }

    if ( t.type() == END ) {

      if ( Dispatch::OutsideIgnoreScope() ) {
	if ( f_base == t.level() ) {
	  write_record( t );
	  KILLSUBTASK( graphics_data );
	  f_base  = -1;
	}
      }
    }

  }

}

//-------------------------------------------------------------------------
void
GraphicsTask::write_record( const Token &t )
{


  /*
   * It is up to the client of graphics_data to determine how to 
   * interpret the content() , which will be treated as file name 
   */

  SearchPath *spath = Dispatch::GetSearchPath();
  const char *file_name = graphics_data->content();
  const char *grFileName = spath->get_real_path( file_name );

  const char *bounding_box = "0.0.0.0";
  GR_TYPE gr_type = GR_TYPE_UNKNOWN;

  /*
   * assuming that grFileName is actually corresponding to the
   * final file where the graphical content is stored
   */

  FlexBuffer graphics_buffer;
  const char *size_info = NULL;
  int graphics_available = 0;
  
    
  FILE *gp;

  if ( grFileName ) {
    gp = fopen ( grFileName, "r" );
    if ( gp ) {

      graphics_available=1;
      int c;
      while ( ( c=getc(gp) ) != EOF ) {
	graphics_buffer.put(c);
      }
      
      fclose(gp);

      
      switch ( (gr_type = graphics_type( grFileName )) ) {
	
      case GR_TYPE_UNKNOWN :
	{
	  cerr << "(WARNING) Unsupported graphics type found in " 
	       << grFileName << endl
	       << "          It is included in file " << t.file() << endl
	       << "                         at line " << t.line() << endl;
	}
	break;
	  
	
      case GR_TYPE_POSTSCRIPT :
	{
#ifndef DtinfoClient
	  PostScript  *ps_store = new PostScript( grFileName );
	  
	  bounding_box  = ps_store->GetBoundingBox();
	  if ( ps_store->HasSearchTerms() ) {
	    termsbuf = ps_store->GetTermsBuffer();
	  }

	  delete ps_store;
#else
	  cerr << "(WARNING) Unsupported PostScript graphic found in " 
	       << grFileName << endl
	       << "          It is included in file " << t.file() << endl
	       << "                         at line " << t.line() << endl;
#endif
	}
	break;

      default:
	break;

      }

      size_info = form("%d.%d.%s",
		       gr_type,
		       GR_ENCODING_TEXT,
		       bounding_box );
    }
  }
  else {
    cerr << "(WARNING)   graphical file  = " << file_name << endl
	 << "          included in file  = " << t.file() << endl
	 << "                   at line  = " << t.line() << endl
	 << "          is not found, a zero length graphical data is used\n\n";

  }

  
  /* dump the graphics record */
  NodeData *nodeData = f_parent->node_data();
  const char *gid = nodeData->graphics_id();
  BookTask *book = nodeData->node()->book();
  const char *bookLocator = book->locator();
  
  DBTable *gtab = book->bookcase()->table(BookCaseDB::Graphics);
  
  gtab->insert( STRING_CODE, bookLocator,
		STRING_CODE, gid,
		STRING_CODE, "",
		STRING_CODE, "v0",
		STRING_CODE, graphics_available?size_info : "0.0.0.0.0.0",
		-STRING_CODE, graphics_buffer.GetBuffer(),
		              (size_t)graphics_buffer.GetSize(),
		STRING_CODE, f_title?f_title->content():"",
		NULL);

   /*
     if ( compress ) {
        gtab->insert(STRING_CODE, gid,
		 STRING_CODE, "@@ graphcis name",
		 STRING_CODE, "v0",
		 STRING_CODE, "@@ type info",
		 COMPRESSED_STRING_CODE, "@@ CompressedAgent",
		 -STRING_CODE, graphics_buffer.GetBuffer(),
			       (size_t)graphics_buffer.GetBSize(),
		 STRING_CODE, "@@ graphics title" );
      }

      */
  
}

       
