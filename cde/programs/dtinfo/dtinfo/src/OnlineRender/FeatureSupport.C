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
 *  $XConsortium: FeatureSupport.C /main/43 1996/12/03 18:22:13 rcs $
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


#define C_Stack
#define L_Support

#define C_TOC_Element
#define L_Basic

#define C_NodeMgr
#define L_Managers
#define C_WindowSystem
#define L_Other

#include <Prelude.h>

#include <limits.h>
#include "Feature.h"
#include "FeatureValue.h"
#include "StyleSheetExceptions.h"
#include <utility/funcs.h>
#include <sstream>
using namespace std;

#include <Dt/CanvasP.h>
#include "CanvasRenderer.hh"

#ifdef DEBUG
#define HIGHLIGHT_DEBUG
#endif

#if defined(UseWideChars) && defined(sun) && !defined(SVR4)
#define mbstowcs(a,b,c) Xmbstowcs(a,b,c)
#define wcstombs(a,b,c) Xwcstombs(a,b,c)
#endif

int left_margin = 0;
int right_margin = 0;
int top_margin = 0;
int bottom_margin = 0;

int point2pixel(int size) {
    return(XmConvertUnits(window_system().toplevel(), XmHORIZONTAL, XmPOINTS,
		   size, XmPIXELS));  
}

void
CanvasRenderer::dofontlist(ElementFeatures&, const FeatureSet&, Symbol**) 
{
  // This member function needs to be removed from header and herein.
}

void
CanvasRenderer::dofont(PartialElementFeatures	&return_features,
		       const FeatureSet &complete,
		       Symbol **symbols)
{
    const Feature *fontfeature = complete.lookup(symbols[FONT]);
    if (fontfeature == NULL)
	return;
    
    const FeatureValue* fv = fontfeature->value();
    assert( fv->type() == FeatureValue::featureset );
    
    const FeatureSet* fs = ((FeatureValueFeatureSet*)fv)->value();
    char* xlfd = _dofont(*fs, symbols);
    
    if (xlfd)
    {
	return_features.set_font (xlfd);
	delete[] xlfd;
	xlfd = NULL;
    }
#ifndef NDEBUG
    else
	cerr << "(WARNING) could not resolve font\n";
#endif
    
    const Feature *subsuperF = fs->lookup(symbols[SUBSUPER]);
    if (subsuperF) {
	const char* subsuper = (const char *)*subsuperF->value();
	if (subsuper) {
	    if (strcasecmp(subsuper, "sub") == 0 ||
		strcasecmp(subsuper, "subscript") == 0)
	    {
		return_features.subsuper(PartialElementFeatures::subscript);
	    }
	    else if (strcasecmp(subsuper, "super") == 0 ||
		     strcasecmp(subsuper, "superscript") == 0)
	    {
		return_features.subsuper(PartialElementFeatures::superscript);
	    }
	    else
		return_features.subsuper(PartialElementFeatures::baseline);
	}
    }
}

const char *
CanvasRenderer::get_pattern(const char *fallback, const char *weight, const char *slant, int size)
{
    const char *ptr;
    const char *foundry_str = "*";
    const char *family_str = "*";
    const char *weight_str = weight;
    const char *slant_str = slant;
    const char *setwidth_name_str = "*";
    const char *style_str = "*";
    int  point_size = size;
    const char *spacing_str = "*";
    const char *charset_str = "*-*";
    const char *xlfd;

    Display *display;

    // set the display variable based on if we are printing or displaying online

    if (window_system().printing() == TRUE) {
	display = window_system().printDisplay();
    }
    else {
	display = window_system().display();
    }
    


#ifdef FONT_DEBUG
    fprintf(stderr, "(DEBUG) fallback=\"%s\" is being tried...",
	    fallback);
#endif
    char *dupfallback = strdup(fallback);
    if (!dupfallback) {
	goto pattern_done;
    }
    
    // sample font: 
    // -dt-application-medium-r-normal-sans-8-80-75-75-p-46-iso8859-1
    
    //  hack the foundry name from the front of the xlfd
    ptr = strtok(dupfallback, "-");
    if (!ptr) {
	goto pattern_done;
    }
    else {
	foundry_str = ptr;
    }
#ifdef FONT_DEBUG
    fprintf(stderr, "(DEBUG)foundry = %s.\n", foundry_str);
    fflush(stderr);
#endif
    
    // get family name
    ptr = strtok(NULL, "-");
    if (!ptr) {
	goto pattern_done;
    }	
    else {
	family_str = ptr;
    }
#ifdef FONT_DEBUG
    fprintf(stderr, "(DEBUG)family = %s.\n", family_str);
    fflush(stderr);
#endif
    
    // get weight from fallback string if not specified by
    // style sheet element
    ptr = strtok(NULL, "-");
    
    // if no more token in fallback pattern is done
    if (!ptr) {
	goto pattern_done;
    }
    
    // if the string is a wildcard then no style sheet weight 
    // was specified so use xlfd field
    if (weight_str && (strcmp(weight_str, "*") == 0)) {
	weight_str = ptr;
    }
#ifdef FONT_DEBUG
    fprintf(stderr, "(DEBUG)weight = %s.\n", weight_str);
    fflush(stderr);
#endif
    
    //  get the slant from the fallback string if not specified
    // by the style sheet element
    ptr = strtok(NULL, "-");
    
    // if no more tokens in fallback then the pattern is done
    if (!ptr) {
	goto pattern_done;
    }
    
    // if the string is a wildcard then no style sheet weight 
    // was specified so use xlfd field
    if (slant_str && (strcmp(slant_str, "*") == 0)) {
	slant_str = ptr;
    }
#ifdef FONT_DEBUG
    fprintf(stderr, "(DEBUG)slant = %s.\n", slant_str);
    fflush(stderr);
#endif
    
    // if slant is specified as ROMAN or ITALIC change to "i" or "r"
    if (strcasecmp(slant_str, "roman") == 0) {
	slant_str = "r";
    }
    else if (strcasecmp(slant_str, "italic") == 0) {
	slant_str = "i";
    }
    
    //  get the setwidth name from the front of the xlfd
    ptr = strtok(NULL, "-");
    // if no more tokens in fallback then the pattern is done
    if (!ptr) {
	goto pattern_done;
    }
#ifdef FONT_DEBUG
    fprintf(stderr, "(DEBUG)setwidth = %s.\n", ptr);
    fflush(stderr);
#endif
    
    //  get the add style name from the front of the xlfd
    ptr = strtok(NULL, "-");
    // if no more tokens in fallback then the pattern is done
    if (!ptr) {
	goto pattern_done;
    }
    else {
	style_str = ptr;
    }
#ifdef FONT_DEBUG
    fprintf(stderr, "(DEBUG)style_str = %s.\n", style_str);
    fflush(stderr);
#endif
    
    //  get the pixel size from the front of the xlfd
    ptr = strtok(NULL, "-");
    // if no more tokens in fallback then the pattern is done
    if (!ptr) {
	goto pattern_done;
    }
#ifdef FONT_DEBUG
    fprintf(stderr, "(DEBUG)pixel size = %s.\n", ptr);
    fflush(stderr);
#endif
    
    //  get the point size from the front of the xlfd
    ptr = strtok(NULL, "-");
    
    // make sure there is a token
    if (ptr) {
	
	// a point size of -1 indicates that no style sheet size
	// was specified.  in this case, we'll convert the
	// xlfd field and use that size
	
	// ??? should put a check here to make sure ptr is an integer value
	if (point_size == -1) {
	    point_size = atoi(ptr);
	}
	point_size = 10 * point_size;
	
#ifdef FONT_DEBUG
	fprintf(stderr, "(DEBUG)point size = %d.\n", point_size);
	fflush(stderr);
#endif
    }
    // else no more tokens in fallback then the pattern is done
    else {	
	// make sure we replace default of -1 with something
	// before making the pattern
	if (point_size == -1) {
	    point_size = 140 + (20 * f_font_scale);;
	}
	goto pattern_done;
    }	    
    
    // apply scaling factor
    point_size = point_size + (20 * f_font_scale);
    
    //  get the resolution x from the front of the xlfd
    ptr = strtok(NULL, "-");
    // if no more tokens in fallback then the pattern is done
    if (!ptr) {
	goto pattern_done;
    }
#ifdef FONT_DEBUG
    fprintf(stderr, "(DEBUG)x resolution = %s.\n", ptr);
    fflush(stderr);
#endif
    
    //  get the resolution y from the front of the xlfd
    ptr = strtok(NULL, "-");
    // if no more tokens in fallback then the pattern is done
    if (!ptr) {
	goto pattern_done;
    }
#ifdef FONT_DEBUG
    fprintf(stderr, "(DEBUG)y resolution = %s.\n", ptr);
    fflush(stderr);
#endif
    
    //  get the spacing from the front of the xlfd
    ptr = strtok(NULL, "-");
    // if no more tokens in fallback then the pattern is done
    if (!ptr) {
	goto pattern_done;
    }
    else {
	spacing_str = ptr;
    }
#ifdef FONT_DEBUG
    fprintf(stderr, "(DEBUG)setwidth = %s.\n", ptr);
    fflush(stderr);
#endif
    
    //  get the average width from the front of the xlfd
    ptr = strtok(NULL, "-");
    // if no more tokens in fallback then the pattern is done
    if (!ptr) {
	goto pattern_done;
    }
#ifdef FONT_DEBUG
    fprintf(stderr, "(DEBUG)average width = %s.\n", ptr);
    fflush(stderr);
#endif
    
    //  get the charset registery and encoding 
    //  if a font really does include a $ i'm screwed
    
    ptr = strtok(NULL, "$");
    // if no more tokens in fallback then the pattern is done
    if (!ptr) {
	goto pattern_done;
    }
    else {
	charset_str = ptr;
    }
    
#ifdef FONT_DEBUG
    fprintf(stderr, "(DEBUG)charset = %s.\n", charset_str);
    fflush(stderr);
#endif
pattern_done:
    
    char pattern[256];
    // -dt-application-medium-r-normal-sans-8-80-75-75-p-46-iso8859-1
    sprintf (
	pattern, 
	"-%s-%s-%s-%s-normal-%s-*-%d-*-*-%s-*-%s", 
	foundry_str ? foundry_str : "*",
	family_str  ? family_str  : "*",
	weight_str  ? weight_str  : "*",
	slant_str   ? slant_str   : "*",
	style_str   ? style_str   : "*",
	point_size,
	spacing_str ? spacing_str : "*",
	charset_str ? charset_str : "*-*"
	);
    
    
    //  If the font exists, return a copy
    if (XLoadQueryFont(display, pattern)) {
#ifdef FONT_DEBUG
	printf("pattern = %s.\n", pattern);
#endif
	xlfd = strdup(pattern);
	return(xlfd);
    }
    
    // determine if the font is a standard application font name
    if ( (strcmp(foundry_str, "dt") == 0) &&  
	 (strcmp(family_str, "application") == 0)) {
	
	// round the point size to the nearest match
	if (point_size < 90 ) point_size = 80;
	else if (point_size < 110) point_size = 100;
	else if (point_size < 130) point_size = 120;
	else if (point_size < 160) point_size = 140;
	else if (point_size < 210) point_size = 180;
	else point_size = 240;

#ifdef FONT_DEBUG
	printf("point size = %d.\n", point_size);
#endif
	
	sprintf (
	    pattern, 
	    "-%s-%s-%s-%s-normal-%s-*-%d-*-*-%s-*-%s", 
	    foundry_str ? foundry_str : "*",
	    family_str  ? family_str  : "*",
	    weight_str  ? weight_str  : "*",
	    slant_str   ? slant_str   : "*",
	    style_str   ? style_str   : "*",
	    point_size,
	    spacing_str ? spacing_str : "*",
	    charset_str ? charset_str : "*-*"
	    );
	
	//  If font does not exist, try using the font cache 
	if (XLoadQueryFont(display, pattern)) {
#ifdef FONT_DEBUG
	    printf("pattern = %s.\n", pattern);
#endif
	    xlfd = strdup(pattern);
	    return(xlfd);	
	}
    } // end if desktop font
	    
	
    // if still not found,  lookup string in cache.  
    // point_size and scale are extracted first.

    xlfd = f_fontcache.lookup(family_str,
			      weight_str,
			      slant_str,
			      (point_size - (20 * f_font_scale)) / 10,
			      charset_str,
			      f_font_scale, 
			      fallback);    

#ifdef FONT_DEBUG
    printf("xlfd = %s.\n", xlfd);
#endif
    return(xlfd);
			      
}

char*
CanvasRenderer::_dofont(const FeatureSet &fs, Symbol** symbols)
{
    const char *fallback = NULL;
    
    const Feature *fallbackF = fs.lookup(symbols[FALLBACK]);
    if (fallbackF) {
	if (fallback = *fallbackF->value()) {
	    if (strcasecmp(fallback, "sans") == 0)
		fallback = f_sans ;
	    else if (strcasecmp(fallback, "serif") == 0)
		fallback = f_serif ;
	    else if (strcasecmp(fallback, "mono") == 0)
		fallback = f_mono ;
	    else if (strcasecmp(fallback, "symbol") == 0)
		fallback = f_symbol ;
	    else 
		fallback = f_sans;
	}
	
#ifdef FONT_DEBUG
	if (fallback)
	    fprintf(stderr, "(DEBUG) fallback=\"%s\"\n", fallback);
	else
	    cerr << "(WARNING) invalid fallback \"" << fallback <<
		"\" specified." << endl;
#endif
    }
    
    const Feature *weightF	= fs.lookup(symbols[WEIGHT]);
    const Feature *slantF	= fs.lookup(symbols[SLANT]);
    const Feature *sizeF	= fs.lookup(symbols[SIZE]);
    
    const char *name = NULL, *foundry = NULL, *charset = NULL;
    
    // need to add something for spacing here
    //const Feature *spacingF	= fs.lookup(symbols[SPACING]);
    const char* font;    
    char* xlfd = NULL;
    
    int i;
    for (i = 0; i < fs.entries(); i++) {
	
	Feature* entry;
	if ((entry = fs.at(i)) == NULL)
	    continue;
	
	if (entry->name() == *symbols[FAMILY]) {
	    
	    const FeatureSet* familyFS;
	    if ((familyFS = *entry->value()) == NULL)
		continue;
	    
	    // resolve name,foundry,charset
	    const Feature* nameF = familyFS->lookup(symbols[NAME]);
	    if (nameF) {
		name = *nameF->value();
	    }
	    else {
#ifdef FONT_DEBUG
		cerr << "(WARNING) You need to specify either family name "
		    "or font fallback in stylesheet." << endl;
#endif
		continue;
	    }
	    
	    const Feature* foundryF = familyFS->lookup(symbols[FOUNDRY]);
	    if (foundryF) {
		foundry = *foundryF->value();
	    }
	    const Feature* charsetF = familyFS->lookup(symbols[CHARSET]);
	    if (charsetF)
		charset = *charsetF->value();
	    else {
#ifdef FONT_DEBUG
		cerr << "(WARNING) You need to specify charset if you "
		    "specify family name in stylesheet." << endl;
#endif
		continue;
	    }
	    
	    assert( name && charset );
	    
	    font = f_fontcache.lookup(name,
				      (const char *)*weightF->value(),
				      (const char *)*slantF->value(),
				      (int)*sizeF->value(),
				      charset,
				      f_font_scale); // scale factor 
	    
	    if (font == NULL && fallback) {
		
		name = fallback;
		
		if ((font = f_fontcache.lookup(name,
					       (const char *)*weightF->value(),
					       (const char *)*slantF->value(),
					       (int)*sizeF->value(),
					       charset,
					       f_font_scale) // scale factor 
		    ) == NULL) {
		    continue;
		}
	    }
	    
	    // if xlfd already defined, create a font list
	    if (xlfd) {
		xlfd = (char*)realloc(xlfd, strlen(xlfd) + strlen(font) + 3);
		strcat(xlfd, ",");
		strcat(xlfd, font);
	    }
	    // otherwise, just dup the font streing
	    else {
		xlfd = strdup(font);
	    }
	    
	}  // end if entry name is a font family

    } // end for each feature support entry
    
    //  if we have a font definition at this point where almost home
    
    if (xlfd) {
	
	//  if a comma appears in the xlfd string, then then it is a font
	//  list so append a colon to the end of the font list string
	
	if (strchr(xlfd, ',')) {
	    strcat(xlfd, ":");
	}
    }
    
    // if no font families were specified, just use what we have from 
    // the fallback and whatever font attributes were specified
    
    else if (fallback) {
	
	font = get_pattern(fallback, 
			   (const char *)*weightF->value(), 
			   (const char *)*slantF->value(),
			   (int)*sizeF->value());

	if (font && *font) {
	    xlfd = strdup(font);
	}

	// if we still can't find one go with whatever.

	else {
	    xlfd = (char*)"-*-*-*-*-*-*-*-*-*-*-*-*-*";
	}
#ifdef FONT_DEBUG
	fprintf(stderr, "resulting in \"%s\".\n", xlfd);
#endif
	
    } // else if fallback


	
#ifdef JBM
	// see if we have a 2 part font family (separated by comma)
	const char *family = *familyF->value();
	const char *p = family ;
    while (*p && (*p != ','))
	p++ ;
    char *fallback = 0;
    
    // p points to comma or end of string 
    if (*p == ',')
    {
	int len = p - family ;
	fallback = new char[len + 1] ;
	strncpy(fallback, family, len);
	fallback[len] = 0 ;
	
	do p++; while (isspace(*p));
	family = p ;
    }
    
#endif
    
#ifdef JBM_FONT_DEBUG
    cerr << "name:     " << name << endl;
    cerr << "foundry:  " << foundry << endl;
    cerr << "charset:  " << charset << endl;
    if (familyF)
	cerr << "family:   (yes)" << endl;
    else
	cerr << "family:   (no)" << endl;
    
    if (weightF)
	cerr << "weight:   " << (const char *)*weightF->value() << endl;
    else
	cerr << "weight:   (no)" << endl;
    
    if (slantF)
	cerr << "slant:    " << (const char *)*slantF->value() << endl;
    else
	cerr << "slant:    (no)" << endl;
    
    if (sizeF)
	cerr << "size:     " << (int)*sizeF->value() << endl;
    else
	cerr << "size:     (no)" << endl;
    
    cerr << "scale:    " << f_font_scale << endl;
    cerr << "fallback: " << fallback << endl;
#endif
    
#if defined FONT_DEBUG || defined FONT_DEBUG_XLFD
    if (xlfd)
	cerr << "xlfd: " << xlfd << endl;
    else
	cerr << "xlfd: (nil)" << endl;
#endif
    
    return xlfd;
}

void
CanvasRenderer::dounderline(ElementFeatures&, const FeatureSet&, Symbol**)
{
  cerr << "dounderline: called " << endl;
}

#ifdef NOTIMPLEMENTED
void
CanvasRenderer::dofooter(ElementFeatures&, const FeatureSet&, Symbol**)
{
  cerr << "dofooter: called " << endl;
}

void
CanvasRenderer::doheader(ElementFeatures&, const FeatureSet&, Symbol**)
{
  cerr << "doheader: called " << endl;
}
#endif

void
CanvasRenderer::domedia(ElementFeatures &return_features, 
			const FeatureSet &complete, Symbol **symbols)
{
  cerr << "domedia: called " << endl;

  const FeatureSet *mset = *complete.lookup(symbols[MEDIA])->value();

  const Feature *orient = mset->lookup(symbols[ORIENTATION]);

  if (orient)  {

      const char *orientation =  *orient->value();

      if (strcasecmp(orientation, "portrait") == 0) {
	  return_features.orientation("portrait");
      }
      else if (strcasecmp(orientation, "landscape") == 0) {
	  return_features.orientation("landscape");
      }
  } // end if orientation feature

}

void
CanvasRenderer::dobreak(PartialElementFeatures	&return_features,
			const FeatureSet &local,
			const FeatureSet& /*complete*/, Symbol **symbols)
{
  const char *breakvalue =  *local.lookup (*symbols[LINEBREAK])->value();

#ifdef BREAK_DEBUG
  cerr << "linebreak: " << breakvalue << endl;
#endif  
  if (strcasecmp (breakvalue, "before") == 0)
    return_features.linebreak (LINEBREAK_BEFORE);
  else
    if (strcasecmp (breakvalue, "after") == 0)
      return_features.linebreak (LINEBREAK_AFTER);
  else
    if (strcasecmp (breakvalue, "both") == 0)
      return_features.linebreak (LINEBREAK_BOTH);


#ifdef JBM
  node *new_spnode = 0 ;
  const char *breakvalue = *local.lookup(*symbols[BREAK])->value();
  
  if (strcasecmp(breakvalue, "line") == 0)
    {
      // get interparagraph spacing 
      const Feature *spacefp = complete.deep_lookup(symbols[MARGIN],
						    symbols[TOP], 0); 

      int spacing = 10 ;	// default value 

      if (spacefp)
	spacing = point2pixel(*spacefp->value());

#ifdef DEBUG
      cerr << "break: spacing = " << spacing << endl ;
#endif

      // insert a space between paragraphs 
#ifdef TML_NO_THIS_ASSIGNMENT
      model* to_model = current_node->get_model();
      new_spnode = new(to_model) space_node(spacing, to_model);
#else
      new_spnode = new space_node(spacing, current_node->get_model());
#endif
      new_spnode->vcc(current_node->vcc());
    }
  return new_spnode ;
#endif
}


void
CanvasRenderer::dopage(PartialElementFeatures	&return_features,
			const FeatureSet &local,
		       const FeatureSet& /*complete*/, Symbol **symbols)
{
    const char *breakvalue =  *local.lookup (*symbols[PAGEBREAK])->value();
    
    cerr << "pagebreak: " << breakvalue << endl;

    if (strcasecmp (breakvalue, "before") == 0) {
	return_features.pagebreak (PAGEBREAK_BEFORE);
    }
    else {
	if (strcasecmp (breakvalue, "after") == 0) {
	    return_features.pagebreak (PAGEBREAK_AFTER);
	}
	else {
	    if (strcasecmp (breakvalue, "both") == 0) {
		return_features.pagebreak(PAGEBREAK_BOTH);
	    }
	}
    }
}

void
CanvasRenderer::domargin(ElementFeatures	&return_features,
			 const FeatureSet &complete,
			 Symbol **symbols)
{
  const FeatureSet *marginset = *complete.lookup(symbols[MARGIN])->value();  

  int first = 0;
  int left  = 0;
  int right = 0;
  int top   = 0;
  int bottom= 0;

  mtry
  {
    //const Feature *f = marginset->lookup(symbols[FIRST]);
    const Feature *l = marginset->lookup(symbols[LEFT]);
    const Feature *r = marginset->lookup(symbols[RIGHT]);
    const Feature *t = marginset->lookup(symbols[TOP]);
    const Feature *b = marginset->lookup(symbols[BOTTOM]);


    if (l) left = point2pixel(*l->value());
    if (r) right = point2pixel(*r->value());
    if (t) top = point2pixel(*t->value());
    if (b) bottom = point2pixel(*b->value());

    // external systems for print form

    left_margin = left;
    right_margin = right;
    top_margin = top;
    bottom_margin = bottom;

  }
  mcatch_noarg (StyleSheetException &)
    {
#ifdef DEBUG
      cerr << "MarginFPtml: style sheet exception" << endl;
#endif
    }
  end_try ;

#ifdef DEBUG_MARGINS
  cout << "[ " << left <<  ", " << right << "] [ " 
       << top << ", " << bottom << "]" << endl; 

#endif

  return_features.margin().first (first);
  return_features.margin().left (left);
  return_features.margin().right (right);
  return_features.margin().top (top);
  return_features.margin().bottom (bottom);


#ifdef JBM
#ifdef TML_NO_THIS_ASSIGNMENT
  model* to_model = current_node->get_model();
  prop_vec *pvec = new(to_model) prop_vec(to_model) ;
#else
  prop_vec *pvec = new prop_vec(current_node->get_model()) ;
#endif
  
  pvec->attach(make_margin_prop(left, right, top, bottom,
				current_node->get_model())); 
  
  current_node->prop_attach(pvec);

#endif
}

void
CanvasRenderer::doborder (ElementFeatures &return_features,
			  const FeatureSet &local,
			  Symbol **symbols)
{
  const Feature *feature = local.lookup(symbols[BORDER]);
  if (feature == NULL)
    return ;

  const FeatureValue *fv = feature->value();

  assert (fv->type() == FeatureValue::featureset);

  if (fv->type() == FeatureValue::featureset)
    {
      const FeatureSet *fs = ((FeatureValueFeatureSet*)fv)->value();

      const Feature *display_feature = fs->lookup(symbols[DISPLAY]);

      if (display_feature) {
	const char *border  = *display_feature->value();

	// valid values are one of:
	// All Bottom Horiz Left Right Top Vert

	if (strcasecmp (border, "all") == 0)
	  return_features.border (_DtCvBORDER_FULL);
	else if (strcasecmp (border, "bottom") == 0)
	  return_features.border (_DtCvBORDER_BOTTOM);
	else if (strcasecmp (border, "horiz") == 0)
	  return_features.border (_DtCvBORDER_HORZ);
	else if (strcasecmp (border, "left") == 0)
	  return_features.border (_DtCvBORDER_LEFT);
	else if (strcasecmp (border, "right") == 0)
	  return_features.border (_DtCvBORDER_RIGHT);
	else if (strcasecmp (border, "top") == 0)
	  return_features.border (_DtCvBORDER_TOP);
	else if (strcasecmp (border, "vert") == 0)
	  return_features.border (_DtCvBORDER_VERT);
      }
      else { // default to _DtCvBORDER_FULL (hard-coded default)
	  return_features.border (_DtCvBORDER_FULL);
      }

      const Feature *width_feature = fs->lookup(symbols[THICKNESS]);

      if (width_feature)
	return_features.border_width(point2pixel(*width_feature->value()));
    }  
}


void
CanvasRenderer::doposition(PartialElementFeatures  &return_features,
			   const FeatureSet &local, 
			   Symbol **symbols)

{
  const FeatureSet *pset = *local.lookup(symbols[POSITION])->value();

  const Feature *horiz = pset->lookup(symbols[HORIZ]);
  const Feature *vert  = pset->lookup(symbols[VERT]);

  if (horiz)
    {
      const char *hvalue = *horiz->value();
      if (strcasecmp (hvalue, "lcorner") == 0)
	return_features.position().horiz(_DtCvJUSTIFY_LEFT_CORNER) ;
      else
      if (strcasecmp (hvalue, "left") == 0)
	return_features.position().horiz(_DtCvJUSTIFY_LEFT) ;
      else
      if (strcasecmp (hvalue, "lmargin") == 0)
	return_features.position().horiz(_DtCvJUSTIFY_LEFT_MARGIN) ;
      else
      if (strcasecmp (hvalue, "rcorner") == 0)
	return_features.position().horiz(_DtCvJUSTIFY_RIGHT_CORNER) ;
      else
      if (strcasecmp (hvalue, "right") == 0)
	return_features.position().horiz(_DtCvJUSTIFY_RIGHT) ;
      else
      if (strcasecmp (hvalue, "rmargin") == 0)
	return_features.position().horiz(_DtCvJUSTIFY_RIGHT_MARGIN) ;
      else
      if (strcasecmp (hvalue, "center") == 0)
	return_features.position().horiz(_DtCvJUSTIFY_CENTER) ;
    }

  if (vert)
  {
      const char *vvalue = *vert->value();
      if (strcasecmp (vvalue, "top") == 0)
	return_features.position().vert(_DtCvJUSTIFY_TOP) ;
      else
      if (strcasecmp (vvalue, "bottom") == 0)
	return_features.position().vert(_DtCvJUSTIFY_BOTTOM) ;
      else
      if (strcasecmp (vvalue, "middle") == 0)
	return_features.position().vert(_DtCvJUSTIFY_CENTER) ;
    }

}

void
CanvasRenderer::dolayout(PartialElementFeatures  &return_features,
			 const FeatureSet &local,
			 const FeatureSet &complete, 
			 Symbol **symbols)
{
  const FeatureSet *locallayoutset = *local.lookup (symbols[LAYOUT])->value();
  const FeatureSet *layoutset = *complete.lookup (symbols[LAYOUT])->value();

  Layout &layout = return_features.layout() ;

  const Feature *feature ;

  feature = locallayoutset->lookup (symbols[ASPACE]) ;
  if (feature != NULL)
    layout.aspace(point2pixel(*feature->value()));

  feature = locallayoutset->lookup (symbols[BSPACE]) ;
  if (feature != NULL)
    layout.bspace(point2pixel(*feature->value()));

  feature = layoutset->lookup (symbols[LEADING]) ;
  if (feature != NULL)
    layout.leading(point2pixel(*feature->value()));

  feature = locallayoutset->lookup (symbols[FINDENT]) ;
  if (feature != NULL)
    layout.findent(point2pixel(*feature->value()));

  feature = locallayoutset->lookup (symbols[LINDENT]) ;
  if (feature != NULL)
    layout.lindent(point2pixel(*feature->value()));

  feature = locallayoutset->lookup (symbols[RINDENT]) ;
  if (feature != NULL)
    layout.rindent(point2pixel(*feature->value()));

  // type

  feature = layoutset->lookup (symbols[FLOW]);
  if (feature)
    {
      const char *flow = *feature->value();
      if (flow)
	{
	  if (strcasecmp (flow, "verbatim") == 0)
	    layout.flow (_DtCvLITERAL);
	  else if (strcasecmp (flow, "filled") == 0) 
	    layout.flow (_DtCvDYNAMIC);
	}
    }
  // justify
  feature = layoutset->lookup (symbols[JUSTIFY]);
  if (feature)
    {
      const char *justify = *feature->value();
      if (justify)
	{
	  if (strcasecmp (justify, "left") == 0)
	    layout.justify (_DtCvJUSTIFY_LEFT);
	  else if (strcasecmp (justify, "right") == 0)
	    layout.justify (_DtCvJUSTIFY_RIGHT);
	  else if (strcasecmp (justify, "center") == 0)
	    layout.justify (_DtCvJUSTIFY_CENTER);
	}
    }
  // wrap
  feature = layoutset->lookup (symbols[WRAP]) ;
  if (feature)
    {
      const char *wrap = *feature->value();
      if (wrap)
	{
	  if (strcasecmp (wrap, "block") == 0)
	    layout.wrap (_DtCvWRAP);
	  else if (strcasecmp (wrap, "join") == 0)
	    layout.wrap (_DtCvWRAP_JOIN);
	  else if (strcasecmp (wrap, "none") == 0)
	    layout.wrap(_DtCvWRAP_NONE);
	}
    }

}

void
CanvasRenderer::dowrap(ElementFeatures &, const FeatureSet &, Symbol **) 
{}

void
CanvasRenderer::dohighlight(ElementFeatures	&return_features,
			    const FeatureSet &complete,
			    Symbol **symbols) 
{
  const FeatureSet *hiliteset = *complete.lookup(symbols[HIGHLIGHT])->value();
#ifdef HILITE_DEBUG
  cerr << "HIGHLIGHT feature";
  CC_TPtrSlistIterator<Feature> iter(*(FeatureSet*)hiliteset);
  Feature* item;
  for(; item = iter(); ) {
    cerr << ' ' << (const char*)item->name().name();
  }
  cerr << " found" << endl;
#endif

  const Feature *underlineF = hiliteset->lookup(symbols[UNDERLINE]);
  int underline = 0;
  if (underlineF)
    underline = *underlineF->value();
  if (underline)
    return_features.highlight().underline(True);
  else
    return_features.highlight().underline(False);
    
  const Feature *strikethroughF = hiliteset->lookup(symbols[STRIKETHROUGH]);
  int strikethrough = 0;
  if (strikethroughF)
    strikethrough = *strikethroughF->value();
  if (strikethrough)
    return_features.highlight().strikethrough(True);
  else
    return_features.highlight().strikethrough(False);

  const Feature *overlineF = hiliteset->lookup(symbols[OVERLINE]);
  int overline = 0;
  if (overlineF)
    overline = *overlineF->value();
  if (overline)
    return_features.highlight().overline(True);
  else
    return_features.highlight().overline(False);

  const Feature *bgcolorF = hiliteset->lookup(symbols[BGCOLOR]);
  if (bgcolorF)
    return_features.highlight().bg_color(*bgcolorF->value());

  const Feature *fgcolorF = hiliteset->lookup(symbols[FGCOLOR]);
  if (fgcolorF)
    return_features.highlight().fg_color(*fgcolorF->value());
}

void
CanvasRenderer::doprefix(ElementFeatures  &return_features,
			 const FeatureSet &local,
			 const FeatureSet &complete,
			 Symbol **symbols)
{
  // NOTE: need to inform find_search_hits that vcc counting is to skip this
  // node...perhaps because vcc is 0 or maybe set a flag?

#ifdef PREFIX_TESTING
  cerr << "doprefix(): " <<  endl;
#endif

  const Feature *prefix_text = local.deep_lookup(symbols[PREFIX],
						 symbols[CONTENT],0);
  if (prefix_text)
    {
      const char *text = *prefix_text->value();

      if (text)
	{
	  PartialElementFeatures &features = return_features.prefix();

	  features.text (text);

	  const Feature    *prefixF = local.lookup(*symbols[PREFIX]);
	  const FeatureSet *prefix_set = ((FeatureValueFeatureSet*)prefixF->value())->value () ;
	  FeatureSet *merged = new FeatureSet(complete, *prefix_set);
  
#ifdef PREFIX_DEBUG
	  cerr << "Prefix Set: " << *prefix_set << endl;
#endif

	  CC_TPtrSlistIterator<Feature> next(*(FeatureSet*)prefix_set);
	  while (++next)
	    {
	      // for each feature, look up the id in the local symbol table, and use
	      // that id as an index into a the table of processing objects which
	      // handle each feature
	      
	      // get id and do lookup 
	      
	      const Symbol &feature = next.key()->name();
	      
#ifdef FEATURE_DEBUG
	      cerr << "\tfeature = " << feature.name() << endl;
#endif	      
	      
	      if (feature == *symbols[LINEBREAK]) 
		dobreak(features, *prefix_set, complete, symbols);
	      else 
		if (feature == *symbols[FONT])
		  dofont(features, *merged, symbols); 
	      else if (feature == *symbols[POSITION])
		doposition (features, *prefix_set, symbols);
	      else if (feature == *symbols[LAYOUT])
		dolayout (features, *prefix_set, *merged, symbols);
	    }
	  delete merged ;
	}
    }
}

void
CanvasRenderer::dosuffix(ElementFeatures	&return_features,
			 const FeatureSet &local,
			 const FeatureSet &complete,
			 Symbol **symbols)
{
  
  const Feature *suffix_text = local.deep_lookup(symbols[SUFFIX],
						 symbols[CONTENT],0);
  if (suffix_text)
    {
      const char *text = *suffix_text->value();
      
      if (text)
	{
	  PartialElementFeatures &features = return_features.suffix();
	  
	  const Feature    *suffixF = local.lookup(*symbols[SUFFIX]);
	  const FeatureSet *suffix_set = ((FeatureValueFeatureSet*)suffixF->value())->value () ;
	  FeatureSet *merged = new FeatureSet(complete, *suffix_set);
	  
	  CC_TPtrSlistIterator<Feature> next(*(FeatureSet*)suffix_set);
	  while (++next)
	    {
	      // for each feature, look up the id in the local symbol table, and use
	      // that id as an index into a the table of processing objects which
	      // handle each feature
	      
	      // get id and do lookup 
	      
	      const Symbol &feature = next.key()->name();
	      
#ifdef FEATURE_DEBUG
	      cerr << "\tfeature = " << feature.name() << endl;
#endif	      
	      
	      if (feature == *symbols[LINEBREAK]) 
		dobreak(features, *suffix_set, complete, symbols);
	      else 
		if (feature == *symbols[FONT])
		  dofont(features, *merged, symbols); 
	      else if (feature == *symbols[POSITION])
		doposition (features, *suffix_set, symbols);
	      else if (feature == *symbols[LAYOUT])
		dolayout (features, *suffix_set, *merged, symbols);
	    }
	  delete merged ;
	  
	  features.text (text);
	}
    }
#ifdef JBM
  // NOTE: need to inform find_search_hits that vcc counting is to skip this
  // node...perhaps because vcc is 0 or maybe set a flag?
  
  // NOTE: need to handle other suffix things too
  // (space, font, wrap, tab, margins, highlight, underline, color)
  
  node *suffix_node = 0 ;
  gnode *new_gnode = 0;
  const Feature *suffix_text = local.deep_lookup(symbols[SUFFIX],
						 symbols[CONTENT],0);
  if (suffix_text)
    {
      const char *text = *suffix_text->value();
#ifdef TML_NO_THIS_ASSIGNMENT
      model* to_model = current_node->get_model();
      new_gnode = new(to_model) gnode(to_model);
#else
      new_gnode = new gnode(current_node->get_model());
#endif
#ifdef UseWideChars
      int size = strlen(text);
      TML_CHAR_TYPE* buffer = new TML_CHAR_TYPE[ size + 1 ];
      int nc = mbstowcs(buffer, text, size + 1);
      assert( nc >= 0 );
      new_gnode->attach_data(buffer, nc);
      delete[] buffer;
#else 
      new_gnode->attach_data((void*)text, strlen(text));
#endif
      
      // process suffix features recursively
      const Feature *suffixF = local.lookup(*symbols[SUFFIX]);
      
      const FeatureSet *suffix_set = ((FeatureValueFeatureSet*)suffixF->value())->value();
      
#ifdef TML_NO_THIS_ASSIGNMENT
      to_model = current_node->get_model();
      suffix_node = new(to_model) bnode(to_model) ;
#else
      suffix_node = new bnode(current_node->get_model()) ;
#endif
      
      // set vcc to ULONG_MAX. When looking for search hits, if we are down
      // this branch then the main trunk (actual data) contains the vcc we are
      // looking for. This large vcc prevents the search from continuing to
      // the right into any prefix nodes.
      
      suffix_node->vcc(ULONG_MAX); 
      
      node_dir dir = n_down ;
      node *root = suffix_node;
      
      // NOTE: ordering of merge parameters counts 
      FeatureSet *merged = new FeatureSet(complete, *suffix_set);
      do_features(root, dir, *suffix_set, *merged, symbols); // recursive call 
      delete merged ;
      
      root->connect_node(new_gnode, dir);
    }
  return suffix_node ;
#endif
}

void
CanvasRenderer::docolor(ElementFeatures	&, const FeatureSet &, Symbol **)
{}

void
CanvasRenderer::do_unsupported_feature(const Symbol &feature_name)
{
  // NOTE: maybe give warning in Author mode 
#ifdef TRUE
  cerr << "Unsupported feature: " << feature_name << endl;
#endif  
}

void
CanvasRenderer::do_features(ElementFeatures	&return_features,
			    const FeatureSet 	&local,
			    const FeatureSet 	&complete,
			    Symbol 	       **symbols)
{
  // NOTE: notice args passed by reference 
  // this routine modifies the calling arguments current_node and connect_dir
  // in the calling routine if breaks, prefix or suffix occur in the feature
  // set  
#ifdef JBM
  node *break_node = 0;
  node *prefix_node = 0;
  node *suffix_node = 0;
#endif
  
  CC_TPtrSlistIterator<Feature> next(*(FeatureSet*)&local);
  while (++next)
    {
      // for each feature, look up the id in the local symbol table, and use
      // that id as an index into a the table of processing objects which
      // handle each feature
      
      // get id and do lookup 
      
#ifdef JBM_IGNORE
      // check for ignore symbol 
      // NOTE: special hack - jbm - needs to be fixed to do proper vcc counting 
      if (next.key()->name() == *f_symbols[IGNORE])
	{
	  connect_dir = n_right ;
	  return 1 ;
	}
#endif
      const Symbol &feature = next.key()->name();

#ifdef FEATURE_DEBUG      
      cerr << "feature = " << feature.name() << endl;
#endif      
      
      if (feature == *symbols[LINEBREAK]) 
	dobreak(return_features, local, complete, symbols);
      else if (feature == *symbols[FONT])
	dofont(return_features, complete, symbols); 
      else if (feature == *symbols[MARGIN])
	domargin(return_features, complete, symbols);
      else if (feature == *symbols[POSITION])
	doposition (return_features, local, symbols);
      else if (feature == *symbols[LAYOUT])
	dolayout (return_features, local, complete, symbols);
      else if (feature == *symbols[BORDER])
	doborder (return_features, local, symbols);
      else if (feature == *symbols[TABLE])
	dotable (return_features, local, symbols);
      else if (feature == *symbols[TGROUP])
	dotgroup (return_features, local, symbols);
      else if (feature == *symbols[COLFORMAT])
	docolformat (return_features, local, symbols);
      else if (feature == *symbols[CELL])
	docell (return_features, local, symbols);
      else if (feature == *symbols[ROW])
	dorow (return_features, local, symbols);
      else if (feature == *symbols[PREFIX])
	doprefix(return_features, local, complete, symbols);
      else if (feature == *symbols[SUFFIX])
	dosuffix(return_features, local, complete, symbols);
#ifdef JBM
      /*
	 else if (feature == *symbols[UNDERLINE])
	 dounderline(return_features, current_node, local, symbols);
	 */
      else if (feature == *symbols[WRAP])
	dowrap(return_features, current_node, local, symbols);
      
      /*
	 else if (feature == *symbols[COLOR])
	 docolor(return_features, current_node, local, symbols);
	 */
#endif
      else {

	  // if printing in progress, check for print features
	  if (window_system().printing()) {

	      // media
	      if (feature == *symbols[MEDIA]) {
		  domedia(return_features, complete, symbols); 
	      }

	      // page break
	      else if (feature == *symbols[PAGEBREAK]) {
		  dopage(return_features, local, complete, symbols);
	      }
	      // otherwise features are unsupported
	      else {
		  do_unsupported_feature(feature);
	      }
	  }
      }
    }

    CC_TPtrSlistIterator<Feature> iter(*(FeatureSet*)&complete);
    while (++iter) {
      const Symbol &feature = iter.key()->name();
      if (feature == *symbols[HIGHLIGHT])
	dohighlight(return_features, complete, symbols);
      else if (feature == *symbols[IGNORE]) {
	Feature *ignore = iter.key();
#ifdef IGNORE_DEBUG
	fprintf(stderr, "IGNORE value=%d\n", (int)*(ignore->value()));
#endif
	return_features.ignore((int)*(ignore->value()));
      }
    }
  
#ifdef JBM
  if (prefix_node)
    {
      current_node->connect_node(prefix_node, connect_dir);
      current_node = prefix_node ;
      connect_dir  = n_right ;
    }
  if (break_node)
    {
      current_node->connect_node(break_node, connect_dir);
      current_node = break_node ;
      connect_dir  = n_right ;
    }
  if (suffix_node)
    {
      current_node->connect_node(suffix_node, connect_dir);
      current_node = suffix_node ;
      connect_dir  = n_left ;
    }
  
#endif
}

void
CanvasRenderer::dotable (ElementFeatures  &return_features,
			 const FeatureSet &local, 
			 Symbol **symbols)
{
#ifdef TABLE_DEBUG
  cerr << "dotable" << endl;
#endif

  const FeatureSet *tset = *local.lookup (symbols[TABLE])->value();

  const Feature* frame_feature = tset->lookup(symbols[FRAME]);

  TableDefn* table;

  if (frame_feature)
    table = new TableDefn((const char*)(*frame_feature->value()));    
  else
    table = new TableDefn((TableDefn::table_frame_t)
				TableDefn::table_frame_default);

  const Feature *colsep_feature = tset->lookup(symbols[COLSEP]);
  if (colsep_feature) {
    int colsep = point2pixel(*colsep_feature->value());
    table->colsep(colsep);
#ifdef COLSEP_DEBUG
    cerr << "COLSEP specified in table, number=" << colsep << endl;
#endif
  }
  
  const Feature *rowsep_feature = tset->lookup(symbols[ROWSEP]);
  if (rowsep_feature) {
    int rowsep = point2pixel(*rowsep_feature->value());
    table->rowsep(rowsep);
#ifdef COLSEP_DEBUG
    cerr << "ROWSEP specified in table, number=" << rowsep << endl;
#endif
  }

  return_features.table(table);
}

void
CanvasRenderer::dotgroup (ElementFeatures  &return_features,
			  const FeatureSet &local, 
			  Symbol **symbols)
{
#ifdef TABLE_DEBUG
  cerr << "dotgroup" << endl;
#endif

  const FeatureSet *tset = *local.lookup (symbols[TGROUP])->value();

  const Feature* justify_feature = tset->lookup(symbols[JUSTIFY]);

  _DtCvFrmtOption justify = _DtCvOPTION_BAD;

  if (justify_feature) {
    const char *str = *justify_feature->value();

    if (str && *str) {
      if (strcasecmp(str, "left") == 0)
	justify = _DtCvJUSTIFY_LEFT;
      else if (strcasecmp(str, "right") == 0)
	justify = _DtCvJUSTIFY_RIGHT;
      else if (strcasecmp(str, "center") == 0)
	justify = _DtCvJUSTIFY_CENTER;
    }
  }

  const Feature* vjustify_feature = tset->lookup(symbols[VJUSTIFY]);

  _DtCvFrmtOption vjustify = _DtCvOPTION_BAD;
  
  if (vjustify_feature) {
    const char *str = *vjustify_feature->value();

    if (str && *str) {
      if (strcasecmp(str, "top") == 0)
	vjustify = _DtCvJUSTIFY_TOP;
      else if (strcasecmp(str, "bottom") == 0)
	vjustify = _DtCvJUSTIFY_BOTTOM;
      else if (strcasecmp(str, "middle") == 0)
	vjustify = _DtCvJUSTIFY_CENTER;
    }
  }

  TGDefn *tgroup = new TGDefn(justify, vjustify);

  const Feature *colsepF = tset->lookup(symbols[COLSEP]);
  if (colsepF) {
    int colsep = point2pixel(*colsepF->value());
    tgroup->colsep(colsep);
#ifdef COLSEP_DEBUG
    cerr << "COLSEP specified in tgroup, number=" << colsep << endl;
#endif
  }

  const Feature *rowsepF = tset->lookup(symbols[ROWSEP]);
  if (rowsepF) {
    int rowsep = point2pixel(*rowsepF->value());
    tgroup->rowsep(rowsep);
#ifdef COLSEP_DEBUG
    cerr << "ROWSEP specified in tgroup, number=" << rowsep << endl;
#endif
  }

  return_features.tgroup(tgroup);

  const Feature *char_alignF = tset->lookup(symbols[CHARALIGN]);
  if (char_alignF)
    tgroup->char_align(*char_alignF->value());
}

void
CanvasRenderer::docolformat(ElementFeatures	&return_features,
			    const FeatureSet	&local,
			    Symbol **symbols)
{
#ifdef TABLE_DEBUG
  cerr << "docolformat" << endl;
#endif
  // have to iterate over colformat attributes
  
  const Feature *feature = local.lookup(symbols[COLFORMAT]);
  const FeatureValue *fv = feature->value();
  
  const FeatureSet *set = ((FeatureValueFeatureSet*)fv)->value();
  ColFormat *colf = new ColFormat ;
  

  mtry
    {
      const Feature *widthF = set->lookup (symbols[WIDTH]) ;
      if (widthF)
	colf->width(point2pixel(*widthF->value()));

      const Feature *nameF = set->lookup (symbols[NAME]) ;
      if (nameF)
	colf->name (*nameF->value());
  
      const Feature *justifyF = set->lookup(symbols[JUSTIFY]); 
      if (justifyF)
	{
	  _DtCvFrmtOption justify = _DtCvOPTION_BAD ;
	  const char *justify_name = *justifyF->value();

	  if (!strcasecmp (justify_name, "left"))
	    {
	      justify = _DtCvJUSTIFY_LEFT;
	    }
	  else
	  if (!strcasecmp (justify_name, "right"))
	    {
	      justify = _DtCvJUSTIFY_RIGHT ;
	    }
	  else
	  if (!strcasecmp (justify_name, "center"))
	    {
	      justify = _DtCvJUSTIFY_CENTER ;
	    }
	  else
	  if (!strcasecmp (justify_name, "char"))
	    {
	      justify = _DtCvJUSTIFY_CHAR ;

	      // CharAlign is effective only for _DtCvJUSTIFY_CHAR
	      const Feature *char_alignF = set->lookup (symbols[CHARALIGN]);
	      if (char_alignF)
		colf->char_align (*char_alignF->value()) ;
	    }
	  colf->justify (justify);
	}
    }
  mcatch_any()
    {
      abort();
    }
  end_try ;

  const Feature *colsepF = set->lookup(symbols[COLSEP]);
  if (colsepF) {
    int colsep = point2pixel(*colsepF->value());
    colf->colsep(colsep);
#ifdef COLSEP_DEBUG
    cerr << "COLSEP specified in colformat, number=" << colsep << endl;
#endif
  }

  const Feature *rowsepF = set->lookup(symbols[ROWSEP]);
  if (rowsepF) {
    int rowsep = point2pixel(*rowsepF->value());
    colf->rowsep(rowsep);
#ifdef COLSEP_DEBUG
    cerr << "ROWSEP specified in colformat, number=" << rowsep << endl;
#endif
  }

  return_features.col_format (colf);	// remember to add into table
}
void
CanvasRenderer::docell (ElementFeatures  &return_features,
			const FeatureSet &local, 
			Symbol **symbols)
{
#ifdef TABLE_DEBUG_X
  static int cell_count = 0 ;
  cerr << "docell: " << cell_count << endl;
  cell_count++ ;
#endif  
  const FeatureSet *cset = *local.lookup (symbols[CELL])->value() ;
  
  const Feature *mr = cset->lookup (symbols[MOREROWS]);
  
  if (mr)
    return_features.cell().spanrows (int(point2pixel(*mr->value())) + 1);

  const Feature *cref = cset->lookup (symbols[COLREF]);
  if (cref)
    return_features.cell().colref (strdup (*cref->value()));

  const Feature *vjustify_feature = cset->lookup(symbols[VJUSTIFY]);
  if (vjustify_feature) {
    const char *vjustify = *vjustify_feature->value();

    if (vjustify && *vjustify) {
#ifdef VJUSTIFY_DEBUG
      cerr << "cell vjustify = " << vjustify << endl;
#endif
      if (strcasecmp(vjustify, "top") == 0)
	return_features.cell().vjustify(_DtCvJUSTIFY_TOP);
      else if (strcasecmp(vjustify, "middle") == 0)
	return_features.cell().vjustify(_DtCvJUSTIFY_CENTER);
      else if (strcasecmp(vjustify, "bottom") == 0)
	return_features.cell().vjustify(_DtCvJUSTIFY_BOTTOM);
      else
	return_features.cell().vjustify(_DtCvOPTION_BAD);
    }
  }

  const Feature *justify_feature = cset->lookup(symbols[JUSTIFY]);
  if (justify_feature) {
    const char *justify = *justify_feature->value();

    if (strcasecmp(justify, "left") == 0)
      return_features.cell().justify(_DtCvJUSTIFY_LEFT);
    else if (strcasecmp(justify, "right") == 0)
      return_features.cell().justify(_DtCvJUSTIFY_RIGHT);
    else if (strcasecmp(justify, "center") == 0)
      return_features.cell().justify(_DtCvJUSTIFY_CENTER);
    else if (strcasecmp(justify, "char") == 0) {
      return_features.cell().justify(_DtCvJUSTIFY_CHAR);

      // CharAlign is effective only for _DtCvJUSTIFY_CHAR
      const Feature *char_alignF = cset->lookup (symbols[CHARALIGN]);
      if (char_alignF) {
	return_features.cell().char_align(*char_alignF->value());
      }
    }
    else
      return_features.cell().justify(_DtCvOPTION_BAD);
  }

  const Feature *colsepF = cset->lookup(symbols[COLSEP]);
  if (colsepF) {
    int colsep = point2pixel(*colsepF->value());
    return_features.cell().colsep(colsep);
#ifdef COLSEP_DEBUG
    cerr << "COLSEP specified in cell, number=" << colsep << endl;
#endif
  }

  const Feature *rowsepF = cset->lookup(symbols[ROWSEP]);
  if (rowsepF) {
    int rowsep = point2pixel(*rowsepF->value());
    return_features.cell().rowsep(rowsep);
#ifdef COLSEP_DEBUG
    cerr << "ROWSEP specified in cell, number=" << rowsep << endl;
#endif
  }

  return_features.cell().has_cell(True);
}
void
CanvasRenderer::dorow (ElementFeatures  &return_features,
		       const FeatureSet &local, 
		       Symbol **symbols)
{
#ifdef TABLE_DEBUG
  cerr << "dorow" << endl;
#endif

  RowDefn* rowdefn = new RowDefn;

  const FeatureSet* features;

  mtry
    {
      features = *local.lookup(symbols[ROW])->value();
    }
  mcatch_any()
    {
      features = NULL;
    }
  end_try;

  if (features) {

    const Feature *vjustify_feature = features->lookup(symbols[VJUSTIFY]);

    if (vjustify_feature) {
      const char *vjustify = *vjustify_feature->value();

      if (vjustify && *vjustify) {
#ifdef VJUSTIFY_DEBUG
	cerr << "vjustify = " << vjustify << endl;
#endif
	if (strcasecmp(vjustify, "top") == 0)
	  rowdefn->vjustify(_DtCvJUSTIFY_TOP);
	else if (strcasecmp(vjustify, "middle") == 0)
	  rowdefn->vjustify(_DtCvJUSTIFY_CENTER);
	else if (strcasecmp(vjustify, "bottom") == 0)
	  rowdefn->vjustify(_DtCvJUSTIFY_BOTTOM);
	else
	  rowdefn->vjustify(_DtCvOPTION_BAD);
      }
    }

    const Feature *rowsepF = features->lookup(symbols[ROWSEP]);
    if (rowsepF) {
      int rowsep = point2pixel(*rowsepF->value());
      rowdefn->rowsep(rowsep);
#ifdef COLSEP_DEBUG
      cerr << "ROWSEP specified in row, number=" << rowsep << endl;
#endif
    }
  }

  return_features.row (rowdefn) ;
}
