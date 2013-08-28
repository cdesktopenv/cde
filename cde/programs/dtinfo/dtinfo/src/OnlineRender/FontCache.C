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
 *  $XConsortium: FontCache.C /main/4 1996/08/30 11:52:26 rcs $
 *
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1996 Hewlett-Packard Company.
 * (c) Copyright 1996 International Business Machines Corp.
 * (c) Copyright 1996 Sun Microsystems, Inc.
 * (c) Copyright 1996 Novell, Inc. 
 * (c) Copyright 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 *
 */

#include <stdio.h>
#include <X11/Xlib.h>
#include <utility/funcs.h>
#include "FontCache.hh"

#if defined DEBUG || defined SMALL_DEBUG || defined FONT_DEBUG
#include <iostream>
using namespace std;
#endif

#define C_List
#define C_TOC_Element
#define L_Basic

#define C_WindowSystem
#define L_Other
#include "Prelude.h"

static unsigned
hFun(const FontEntry &e)
{
  return e.hash();
}

static unsigned
flhFun(const CC_String &s)
{
  return s.hash();
}


FontCache::FontCache()
: f_cache(hFun),
  f_fontlist(flhFun)
{
}

FontCache::~FontCache()
{
  f_cache.clearAndDestroy();
  f_fontlist.clearAndDestroy();
}

const char *
FontCache::lookup(const char *family, const char *weight, const char *slant,
		int ptsize, const char* charset, int scale_factor, const char *fallback)
{
  FontEntry key(family, weight, slant, charset, ptsize);

  const char *xlfd = f_cache.findValue(&key);

  if (!xlfd)
    {
      xlfd = getxlfd(family, !strcasecmp(weight,"bold"),
		     strcasecmp(slant,"roman"), charset,
		     ptsize + scale_factor*2, fallback);
      if (xlfd)
	f_cache.insertKeyAndValue(new FontEntry(key), strdup(xlfd));
    }
  return xlfd ;
}

// /////////////////////////////////////////////////////////////////////////
// class FontEntry
// /////////////////////////////////////////////////////////////////////////

FontEntry::FontEntry(const char *family,
		     const char *weight,
		     const char *slant,
		     const char *charset,
		     int	ptsize)
: f_family(family),
  f_weight(weight),
  f_slant(slant),
  f_charset(charset),
  f_ptsize(ptsize)
{
}

FontEntry::FontEntry(const FontEntry &fe)
: f_family(fe.f_family),
  f_weight(fe.f_weight),
  f_slant(fe.f_slant),
  f_charset(fe.f_charset),
  f_ptsize(fe.f_ptsize)
{
}

FontEntry::~FontEntry()
{
}

unsigned
FontEntry::hash() const
{
  return f_family.hash() + f_weight.hash() + f_slant.hash() +
	 f_charset.hash() + f_ptsize ;
}

bool
FontEntry::operator==(const FontEntry &fe) const
{
  return
    (f_family == fe.f_family) && (f_weight == fe.f_weight) &&
    (f_slant  == fe.f_slant)  && (f_charset == fe.f_charset) &&
    (f_ptsize == fe.f_ptsize) ;
}

// yanked from connolly code in the RichText widget

const char *
FontCache::getxlfd(const char *family, int bold,
		   int italic, const char *charset, int size, const char *fallback)
{
    Display *display;

    if (window_system().printing() == TRUE) {
	display = window_system().printDisplay();
    }
    else {
	display = window_system().display();
    }
    
    // get cache of font family
    static char pattern[256];
    if (charset && *charset) {
	snprintf(pattern, sizeof(pattern),
			"-*-%s-*-*-*-*-*-*-*-*-*-*-%s", family, charset);
    }
    else if (family && *family) {
	snprintf(pattern, sizeof(pattern),
			"-*-%s-*-*-*-*-*-*-*-*-*-*-*-*", family);
    }
    else {
	int len = MIN(strlen(fallback), 256 - 1);
	*((char *) memcpy(pattern, fallback, len) + len) = '\0';
    }
    
  CC_String _l_pattern(pattern);
  FontList *fontlist = f_fontlist.findValue(&_l_pattern);
  if (!fontlist)
  {
    XFontStruct *newfont = 0;
#if DO_SCALEABLE_FONTS
    // first check for scalable 
    char scaled_pattern[256];
    snprintf(scaled_pattern, sizeof(scaled_pattern),
		"-*-%s-%s-%s-*-*-0-0-*-*-*-*-*-*", family, weight, slant);
    
    newfont = XLoadQueryFont(display, pattern);
#endif
    if (newfont)
      {
	char **names = new char*[1] ;
	names[0] = strdup(pattern);
	fontlist = new FontList; // empty fontlist with scalable flag set 
	f_fontlist.insertKeyAndValue(new CC_String(pattern), fontlist);
#ifdef SMALL_DEBUG	
	cerr << "scaleable: " << pattern << endl;
#endif
      }
    else
      {
#ifdef SMALL_DEBUG
	cerr << "non scaleable: " << pattern << endl;
#endif
	int  count ;	
	char **names = XListFonts(display,
				  pattern,
				  100, // number of fonts to retrieve...sounds good
				  &count); // number actually retrieved ;
#ifdef DEBUG    
	{
	  cerr << "retrieved: " << count << " for: " << pattern ;
	  for (int i = 0 ; i < count ; i++ )
	    cerr << names[i] << endl;
	}
#endif
	
	fontlist = new FontList(count, (const char**)names);
	f_fontlist.insertKeyAndValue(new CC_String(pattern), fontlist);
      }
  }

#ifdef DO_SCALEABLE_FONTS
  if (fontlist->scaleable())
    {
      snprintf(pattern, sizeof(pattern),
	"-*-%s-%s-%s-*-*-%d-*-*-*-*-*-*-*", family, weight, slant, size);
    
      return pattern ;
    }
#endif
    
  int qty = fontlist->count();
  const char** cand_names = fontlist->names();
  const char* font_name;

  if(qty > 0){
    int best_score = 100000; //MAGIC - smaller scores are better
    int best_index = 0;

    for(int i = 0; i<qty; i++){
      ON_DEBUG(printf("scoring: %s\n", cand_names[i]));

      int score = 0;
      const char *scan = cand_names[i];
      if((scan = strchr(scan, '-')) &&
	 (scan = strchr(scan+1, '-')) &&
	 (scan = strchr(scan+1, '-'))){
	if(bold == (strncmp(scan + 1, "medium", 6) == 0))
	  score += 500; //HEURISTIC
	if((scan = strchr(scan+1, '-'))){
	  if(italic == (strncmp(scan+1, "r", 1) == 0))
	    score += 500; //HEURISTIC
	  if((scan = strchr(scan+1, '-')) &&
	     (scan = strchr(scan+1, '-')) &&
	     (scan = strchr(scan+1, '-'))){
	    int pixels = atoi(scan+1);

// 	    int diff = (int)((half_points * ydpmm() *254/1440) -  pixels);
	    int diff = size - pixels ;
	    score += diff * diff;
	  }else
	    score += 2000;
	}else
	  score += 2000;
      }else
	score += 2000; //HEURISTIC

      if(score < best_score){
	best_score = score;
	best_index = i;
      }
    }
    font_name = cand_names[best_index];
  }else
    font_name = 0 ;

#ifdef FONT_DEBUG
  char *btype = bold ? "\tbold" : "\tmedium";
  char *itype = italic ? "\titalic" : "\tnormal";
  cerr << family
       << btype
       << itype
       << '\t' << size << endl;
  cerr << font_name << endl;
#endif

  return font_name ;

}


FontList::FontList(int count, const char **names)
: f_names(names),
  f_count(count)
{
}
FontList::~FontList()
{
  XFreeFontNames((char **)f_names);
}
