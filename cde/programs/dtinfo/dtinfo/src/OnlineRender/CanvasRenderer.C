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
// $XConsortium: CanvasRenderer.C /main/61 1996/12/03 18:21:46 rcs $
/*	Copyright (c) 1994,1995,1996 FUJITSU LIMITED	*/
/*	All Rights Reserved				*/

#define C_TOC_Element
#define C_NodeViewInfo
#define L_Basic

#define C_GraphicsMgr
#define C_PrefMgr
#define L_Managers

#define C_WindowSystem
#define L_Other

#define C_PixmapGraphic
#define L_Graphics

#define C_NamedData
#define C_OString
#define C_NodeViewInfo
#define L_Basic 
#include <Prelude.h>

#include "Managers/WString.hh"

#include <utility/funcs.h>

#include "Element.h"
#include "Attribute.h"
#include "CanvasRenderer.hh"
#include "Feature.h"
#include "FeatureValue.h"

// stylesheet spec states default leading to be 2
#define DEFAULT_CONTAINER_LEADING 2

#if defined(UseWideChars) && defined(sun) && !defined(SVR4)
#define mbstowcs(a,b,c) Xmbstowcs(a,b,c)
#define wcstombs(a,b,c) Xwcstombs(a,b,c)
#endif

#include <DtI/LinkMgrP.h>

#undef self
#include <DtI/DisplayAreaP.h>
#include <DtI/FontI.h>
#include <DtI/FontAttrI.h>
#include <DtI/RegionI.h>

// information to be passed back 
NodeViewInfo *gNodeViewInfo ;

// avoid HardCopy hack
Renderer *gRenderer = 0 ;

DtHelpDispAreaStruct *gHelpDisplayArea = 0;

struct s_entry
{
  unsigned int	index;
  const char   *name ;
};

static
struct s_entry symbols[] = 
{
  { GRAPHIC_attr,	"GRAPHIC" },
  { INLGRAPHIC,	"INLGRAPHIC" },
  { ISMAP,	"ISMAP" },
  { OLID,		"OL-ID" },
  { OLIDREF,	"OL-IDREF" },
  { REMOTE,	"REMOTE" },
  { TABLE,	"TABLE" },
  { TERMS,	"TERMS" },
  { ALIGN,	"ALIGN" },
  { BOTTOM,	"BOTTOM" },
  { LINEBREAK,	"LINEBREAK" },
  { CHARSETS,	"CHARSET" },
  { SET,		"SET" },
  { BGCOLOR,	"BGCOLOR" },
  { FGCOLOR,	"FGCOLOR" },
  { FONT,		"FONT" },
  { FONTCATALOG,	"FONTCATALOG" },
  { FONTFAMILY,	"FONTFAMILY" },
  { HIGHLIGHT,	"HIGHLIGHT" },
  { IGNORE,	"IGNORE" },
  { LEFT,		"LEFT" },
  { MARGIN,	"MARGIN" },
  { SUFFIX,	"SUFFIX" },
  { PREFIX,	"PREFIX" },
  { PREVIEW,	"PREVIEW" },
  { RIGHT,	"RIGHT" },
  { TABSTOPS,	"TABSTOP" },
  { CONTENT,	"CONTENT" },
  { TOP,		"TOP" },
  { OVERLINE,	"OVERLINE" },
  { STRIKETHROUGH,"STRIKETHROUGH" },
  { UNDERLINE,	"UNDERLINE" },
  { WRAP,		"WRAP" },
  { XLFD,		"XLFD" },
  { GRAPHIC_feature,"GRAPHIC" },
  { FAMILY,	"FAMILY" },
  { WEIGHT,	"WEIGHT" },
  { SLANT,	"SLANT" },
  { SIZE,		"SIZE" },
  { CHARSET,	"CHARSET" },
  { REVERSEVIDEO,	"REVERSE-VIDEO" },
  { AT,		"AT" },
  { EVERY,	"EVERY" },
  { POSITION,	"POSITION" },
  { HORIZ,	"HORIZ" },
  { VERT,		"VERT" },
  { LAYOUT,	"LAYOUT" },
  { ASPACE,	"ASPACE" },
  { BSPACE,	"BSPACE" },
  { LEADING,	"LEADING" },
  { FINDENT,	"FINDENT" },
  { LINDENT,	"LINDENT" },
  { RINDENT,	"RINDENT" },
  { FLOW,		"FLOW" },
  { JUSTIFY,	"JUSTIFY" },
  { VJUSTIFY,	"VJUSTIFY" },
  { BORDER,	"BORDER" },
  { THICKNESS,	"THICKNESS" },
  { ROW,		"ROW" },
  { COLS,		"COLS" },
  { COLFORMAT,	"COLFORMAT" },
  { CHARALIGN,	"CHARALIGN" },
  { SPANCOLS,	"SPANCOLS" },
  { MOREROWS,	"MOREROWS" },
  { CELL,		"CELL" },
  { WIDTH,	"WIDTH" },
  { FALLBACK,	"FALLBACK" },
  { FOUNDRY,	"FOUNDRY" },
  { NAME,		"NAME" },
  { DISPLAY,	"DISPLAY" },
  { COLREF,	"COLREF" },
  { SUBSUPER,	"POSITION" },
  { COLSEP,	"COLSEP" },
  { ROWSEP,	"ROWSEP" },
  { TGROUP,	"TGROUP" },
  { FRAME,	"FRAME" },
  { MEDIA,	"MEDIUM" },
  { PAGEBREAK,	"PAGEBREAK" },
  { FOOTERS,	"FOOTERS" },
  { HEADERS,	"HEADERS" },
  { ORIENTATION,  "ORIENTATION" }
};
// supported features

// these are last resorts, hard-coded
const char* const CanvasRenderer::f_hcf_sans = "sans";
const char* const CanvasRenderer::f_hcf_serif = "*";
const char* const CanvasRenderer::f_hcf_mono = "*";
const char* const CanvasRenderer::f_hcf_symbol = "*";

const char* const CanvasRenderer::f_hcf_weight = "*";
const char* const CanvasRenderer::f_hcf_slant = "*";
const char* const CanvasRenderer::f_hcf_fallback = "*";
const int CanvasRenderer::f_hcf_size = -1;

static _DtCvSegment *insert_break (_DtCvSegment *container,
				   unsigned long type);
static _DtCvSegment *new_segment (unsigned long type);
static void insert_segment (_DtCvSegment *container, _DtCvSegment *segment);

#ifdef CONTAINER_DEBUG
static
const char *
print_justify (unsigned value)
{
  const char *rvalue ;
  switch (value)
    {
    case _DtCvJUSTIFY_LEFT:
      rvalue =  "_DtCvJUSTIFY_LEFT" ;
      break ;
    case _DtCvJUSTIFY_RIGHT:
      rvalue =  "_DtCvJUSTIFY_RIGHT" ;
      break ;
    case _DtCvJUSTIFY_CENTER:
      rvalue =  "_DtCvJUSTIFY_CENTER" ;
      break ;
    case _DtCvJUSTIFY_TOP:
      rvalue =  "_DtCvJUSTIFY_TOP" ;
      break ;
    case _DtCvJUSTIFY_BOTTOM:
      rvalue =  "_DtCvJUSTIFY_BOTTOM" ;
      break ;
    case _DtCvJUSTIFY_LEFT_CORNER:
      rvalue =  "_DtCvJUSTIFY_LEFT_CORNER" ;
      break ;
    case _DtCvJUSTIFY_LEFT_MARGIN:
      rvalue =  "_DtCvJUSTIFY_MARGIN" ;
      break ;
    case _DtCvJUSTIFY_RIGHT_CORNER:
      rvalue =  "_DtCvJUSTIFY_RIGHT_CORNER" ;
      break ;
    case _DtCvJUSTIFY_RIGHT_MARGIN:
      rvalue =  "_DtCvJUSTIFY_RIGHT_MARGIN" ;
      break ;
    default: 
      rvalue = "Unknown" ;
    }
  return rvalue ;
}
#endif

CanvasRenderer::CanvasRenderer(int font_scale)
: Renderer(),
  f_current_tgroup (0),
  f_font (0),
  f_link_idx (-1),
  f_font_scale(font_scale),
  fBogusSymbol(gElemSymTab->intern("%BOGUS")),
  f_level(0)
{
  // make symbols 
  for ( int i=0; i < REND_SYMBOLS; i++)
    f_symbols[symbols[i].index] = new
      Symbol(gSymTab->intern(symbols[i].name)); 

  f_sans    = window_system().get_string_default("FontSans");
  f_serif   = window_system().get_string_default("FontSerif");
  f_mono    = window_system().get_string_default("FontMono");
  f_symbol  = window_system().get_string_default("FontSymbol");
  if (! (f_sans   && *f_sans))	 f_sans   = f_hcf_sans;
  if (! (f_serif  && *f_serif))	 f_serif  = f_hcf_serif;
  if (! (f_mono   && *f_mono))	 f_mono   = f_hcf_mono;
  if (! (f_symbol && *f_symbol)) f_symbol = f_hcf_symbol;

  // determine default border width
  f_border_width = window_system().get_int_default("BorderWidth");
  if (! f_border_width)
    f_border_width = 2;
}

CanvasRenderer::~CanvasRenderer()
{
  for (int i = 0 ; i < REND_SYMBOLS; i++)
    delete f_symbols[i] ;
}

// virtuals

FeatureSet *
CanvasRenderer::initialize()
{
  f_default_features = new FeatureSet ;

  FeatureSet *font_set = new FeatureSet ;

  font_set->add(new Feature(*f_symbols[WEIGHT], new FeatureValueString(f_hcf_weight)));
  font_set->add(new Feature(*f_symbols[SLANT], new FeatureValueString(f_hcf_slant)));
  font_set->add(new Feature(*f_symbols[SIZE], new FeatureValueInt(f_hcf_size)));
  font_set->add (new Feature (*f_symbols[FALLBACK], new FeatureValueString (f_hcf_fallback)));

  f_default_features->add(new Feature(*f_symbols[FONT], new FeatureValueFeatureSet(font_set)));

  // default leading
  FeatureSet* layout = new FeatureSet;

  layout->add(new Feature(*f_symbols[LEADING],
	      new FeatureValueInt(DEFAULT_CONTAINER_LEADING)));

  f_default_features->add(new Feature(*f_symbols[LAYOUT],
			  new FeatureValueFeatureSet(layout)));

#ifdef JBM
  // other default features
  FeatureSet *margin = new FeatureSet;
  margin->add (new Feature(*f_symbols[LEFT], new FeatureValueInt (20)));
  f_default_features->add(new Feature(*f_symbols[MARGIN],
				      new FeatureValueFeatureSet(margin)));
#endif
#ifdef JBM
  f_default_features->add(new Feature(*f_symbols[WRAP],
				      new FeatureValueString("word")));
  f_default_features->add(new Feature(*f_symbols[BREAK],
				      new FeatureValueString("line")));
#endif
  return f_default_features;
}

void
CanvasRenderer::Begin()
{
  // pre document initialization...called before document is processed 
  _DtCvTopicInfo	*topic = new _DtCvTopicInfo ;

  topic->link_data = _DtLinkDbCreate() ;
  topic->seg_list  = new_segment(_DtCvCONTAINER);
  topic->id_str = NULL ;
  topic->mark_list = NULL ;
  

  gNodeViewInfo->topic(topic); // NOTE: was after do_features...

  f_current_container = topic->seg_list ;
  f_current_displayable = 0 ;
  f_link_idx = -1 ;

  // set up initial parameters 
  ElementFeatures features(this) ;

  do_features(features, *f_default_features, *f_default_features,
	      f_symbols); 

  f_leading_stack.push(features.layout().leading());
  f_leading_level_stack.push(f_level);

  setup_container(f_current_container, features);

  /*
     have to apply the features here
   */

  f_vcc = 0 ;

  f_stack.push (f_current_container);
}

void
CanvasRenderer::End()
{
  // post processing...called after document ends 
#ifdef HIERARCHY_DEBUG
  void print_hierarchy(_DtCvTopicInfo*);
  print_hierarchy(gNodeViewInfo->topic());
#endif

  while (f_leading_level_stack.entries())
    f_leading_level_stack.pop();
  while (f_leading_stack.entries())
    f_leading_stack.pop();
}

static unsigned is_literal = 0 ;

// returns non-zero value if element is to be ignored 
unsigned int
CanvasRenderer::BeginElement(const Element		&element,
			     const FeatureSet	&local,
			     const FeatureSet	&complete,
			     const FeatureSet	&parentComplete)
{
  // at this point, f_current_container contains the 
  // parent of the element we are about to process

#ifdef BEGIN_ELEMENT_DEBUG
  cerr << '<' << element.gi() << '>' << endl;
#endif

  assert(f_current_container->type == _DtCvCONTAINER);

  // have to do some VCC counting here


#ifdef FEATURE_DEBUG
  cerr << element.gi() << ": \t" << local << endl;
  cerr << element.gi() << "=>\t" << complete << endl;
#endif

  // now process styles 

  f_fstack.push (f_font);
  f_stack.push(f_current_container);
  f_link_stack.push (f_link_idx);
  f_tgroup_stack.push (f_current_tgroup);

#ifdef OL_LINK_DEBUG
  cerr << "pushed link_index: " << f_link_idx << endl;
#endif

#ifdef DEBUG_USE_NAMES
  _DtCvSegment *hold_segment = f_current_container ;
#endif

  ElementFeatures features(this) ;

  do_features(features, local, complete, f_symbols); 

  if (features.ignore()) {
    if (f_tgroup_stack.entries())   f_tgroup_stack.pop();
    if (f_link_stack.entries())	    f_link_stack.pop();
    if (f_stack.entries())	    f_stack.pop();
    if (f_subsuper_stack.entries()) f_subsuper_stack.pop();

    return features.ignore();
  }

  ++f_level;

  if (features.ignore_linebreak(False)) {
    // disable linebreak features beforehand here if linebreak
    // should be ignored for this element

    if (features.prefix().linebreak() & LINEBREAK_BEFORE)
      features.prefix().linebreak(
		features.prefix().linebreak() & ~LINEBREAK_BEFORE);

    if (features.suffix().linebreak() & LINEBREAK_AFTER)
      features.suffix().linebreak(
		features.suffix().linebreak() & ~LINEBREAK_AFTER);

    features.linebreak(LINEBREAK_NONE);
  }
  
  {
    SegClientData scd(_DtCvSTRING);

    scd.hilite_type(features.highlight().overline() |
		    features.highlight().strikethrough() |
		    features.highlight().underline());
    UAS_String& bg_color(features.highlight().bg_color());
    UAS_String& fg_color(features.highlight().fg_color());
    if ((char*)bg_color && *(char*)bg_color)
      scd.bg_color(strdup((char*)bg_color));
    if ((char*)fg_color && *(char*)fg_color)
      scd.fg_color(strdup((char*)fg_color));

    f_scd_stack.push(new SegClientData(scd));
  }

  f_subsuper_stack.push(features.subsuper());

  if (features.table() == NULL && f_table_stack.entries()) {
    // semantics: f_table_stack.top should always be the current.
    //            need special care on the other end as well.
    f_table_stack.push(f_table_stack.top());
  }
  else
    f_table_stack.push(features.table());

  if (features.layout().has_layout(False) &&
      current_leading() != features.layout().leading())
  {
    f_leading_stack.push(features.layout().leading());
    f_leading_level_stack.push(f_level);
  }

  // get info on special olias stuff like links etc
  handle_olias_attributes(features, element,
			  local, complete, parentComplete); 

  // if we are not already in a link, see if we start one
  if (f_link_idx == -1)
    {
      f_link_idx = features.link_idx();
    }

  if (features.tgroup())
    {
      _DtCvSegment *tableseg = new_segment(_DtCvTABLE);

      // NOTE: have to delete this guy when he pops out the other end !
      f_current_tgroup = features.tgroup();
      f_current_tgroup->set_segment(tableseg);

      // associate tgroup with the surrounding table 
      f_current_tgroup->table(f_table_stack.top());

      if (f_current_tgroup->table() &&
	  f_current_tgroup->table()->frame() != TableDefn::table_frame_none)
	{
	  _DtCvSegment* former_current_container = f_current_container;

	  // NOTE: the purpose of the new container is just for
	  //       drawing a border around a tgroup
	  f_current_container = create_container(f_current_container);

	  setup_cascaded_tgroup_container(former_current_container,
					  f_current_container);
        }

      insert_segment(f_current_container, tableseg);

    }

  if (features.col_format()) {
#ifdef COLSPEC_DEBUG
    {
	ColFormat *colf = features.col_format();
	fprintf(stderr, "(DEBUG) ColFormat justify=%d, cols=%d, name=\"%s\", width=%d\n",
				colf->justify(), colf->cols(), colf->name(), colf->width());
    }
#endif
    f_current_tgroup->add (features.col_format());
  }

  if (features.row()) {
    f_current_tgroup->add_row(features.row());
  }

  if (features.requires_container(False) ||
      (features.prefix().text() && features.prefix().requires_container(True)) ||
      (features.suffix().text() && features.suffix().requires_container(True)))
    {
      
      if (features.cell().has_cell())
	{
	  f_current_container = create_container (NULL);

	  ColDefn* coldef = new ColDefn(f_current_container, NULL,
					features.cell().spanrows(),
					features.cell().colref(),
					features.cell().colstart(),
					features.cell().colend(),
					features.cell().justify(),
					features.cell().vjustify(),
					features.cell().colsep(),
					features.cell().rowsep(),
					features.cell().char_align()); 

	  f_current_tgroup->add(coldef);
	}
      else
	{
	  f_current_container = create_container(f_current_container);
	  setup_container (f_current_container, features);
	}
      

#ifdef CONTAINER_DEBUG
      cerr << "Container: " << 
	"\tjustify:  " << print_justify(f_current_container->handle.container.justify) << 
	  "\tvjustify: " << print_justify(f_current_container->handle.container.vjustify) << 
	  "\torient:   " << print_justify(f_current_container->handle.container.orient) << 
	  "\tvorient:   " << print_justify(f_current_container->handle.container.vorient) << 
	  "\tflow:     " << f_current_container->handle.container.flow << 
	    endl;
#endif

      f_current_container->handle.container.id = features.locator();

      if (f_link_idx != -1)
	{
	  f_current_container->link_idx = f_link_idx ;
	  f_current_container->type |= _DtCvHYPER_TEXT ;
	}

    }
  else
    {
      // may have to build a marker segment
      if (features.locator())
	{
	  _DtCvSegment *marker = new_segment (_DtCvMARKER);
	  marker->handle.marker = features.locator();
	  insert_segment (f_current_container, marker);
	}

	const char *font = features.font();
	if (font && *font)
	  f_font = features.font();
    }

  // NOTE: pagebreak implicitly adds a linebreak before the pagebreak.
  if (features.pagebreak() & PAGEBREAK_BEFORE) {
    insert_break (f_current_container, _DtCvNEW_LINE);
    insert_break (f_current_container, _DtCvPAGE_BREAK);
  }
  else if (features.linebreak() & LINEBREAK_BEFORE) {
    insert_break (f_current_container, _DtCvNEW_LINE);
  }

  // check for prefix text, because if there is no text, then we are
  // not really doing a prefix, and we just ignore it...possible
  // exception, if an <HR> is required as a prefix item

  if (features.prefix().text())
    {
      _DtCvSegment *container = f_current_container ;

      if (features.prefix().requires_container(True))
	{
	  const char *font = f_font ;
	  container = create_container (f_current_container);
	  setup_container (container, features.prefix(), True);
	  f_font = font ;
#ifdef CONTAINER_DEBUG
      cerr << "Prefix Container: " << 
	"\tjustify:  " << f_current_container->handle.container.justify << 
	  "\tvjustify: " << f_current_container->handle.container.vjustify << 
	  "\torient:   " << f_current_container->handle.container.orient << 
	  "\tvorient:   " << f_current_container->handle.container.vorient << 
	  "\tflow:     " << f_current_container->handle.container.flow << 
	    endl;
#endif
	  // create a container for the element content
	  f_current_container = create_container(f_current_container);
	}

      if (features.prefix().linebreak() & LINEBREAK_BEFORE)
	insert_break (container, _DtCvNEW_LINE);

      const char *font = features.prefix().font() ;
      if (font == NULL || *font == '\0')
	font = f_font ;


      _DtCvSegment *segment = insert_string (container,
					     font,
					     features.prefix().text(),
					     strlen(features.prefix().text()),
					     False) ;

      // indicate that this is added information 
      segment->type |= _DtCvAPP_FLAG1 ;

      if (features.prefix().requires_container(True))
	f_current_displayable = 0;

      if (features.prefix().linebreak() & LINEBREAK_AFTER) {
	if (! features.prefix().ignore_linebreak(True))
	  segment->type |= _DtCvNEW_LINE ;
      }
    }
    else if (features.prefix().linebreak() & LINEBREAK_AFTER) {
      if (! features.prefix().ignore_linebreak(True))
	insert_break(f_current_container, _DtCvNEW_LINE);
    }

  // do graphics handling
  if (features.graphic())
    {
      _DtCvSegment *grseg = features.graphic();
      insert_segment(f_current_container, grseg);
      grseg->link_idx = features.link_idx();

      if (f_current_displayable)
	f_current_displayable->next_disp = grseg;
      f_current_displayable = grseg;

#if 0
      // I have no idea what the next 2 lines are intended for. 
      // Why should prefix's linebreak take effect on the element's
      // flow-break? --- kamiya@chistech.com
      if (features.prefix().linebreak() & LINEBREAK_AFTER)
	grseg->type |= _DtCvNEW_LINE ;
#endif
    }

  PartialElementFeatures *suffixFeatures = 0;
  if (features.suffix().text())
    {
      // have to stack this guy and add it to the data during
      // EndElement 
      suffixFeatures = new PartialElementFeatures(features.suffix());
    }
  if ((features.linebreak() & LINEBREAK_AFTER) ||
      (features.pagebreak() & PAGEBREAK_AFTER))
    {
      // Add linebreak/pagebreak info to suffix; create empty suffix
      // if necessary.
      if (suffixFeatures == 0)
	suffixFeatures = new PartialElementFeatures(this);

      // if our element had a linebreak after, then include it in the suffix
      // while preserving potential suffix linebreak settings.
      if (features.linebreak() & LINEBREAK_AFTER)
	suffixFeatures->linebreak(suffixFeatures->linebreak() |
				  LINEBREAK_AFTER);

      // if our element had a pagebreak after, then include it in the suffix;
      // suffixes cannot have pagebreaks, so don't worry about preserving.
      if (features.pagebreak() & PAGEBREAK_AFTER)
	suffixFeatures->pagebreak(PAGEBREAK_AFTER);
    }
  f_suffixes.push (suffixFeatures);

  is_literal = f_current_container->handle.container.type == _DtCvLITERAL;

  if (features.xref()) {

#if 0
    if (g_xref_subsection && window_system().dtinfo_font()) {

      extern UAS_String f_dtinfo_font_name;

      UAS_String swapped_out = f_font;
      f_font = f_dtinfo_font_name;

      char buf[2];
      buf[0] = (char)0x4c;
      buf[1] = (char)0;

      _data(buf, 2, False);

      f_font = swapped_out;
    }
#endif

    _data(features.xref(), strlen(features.xref()), False);
  }

  return 0 ;			// not do not ignore content
}

void
CanvasRenderer::EndElement(const Symbol& /*name*/)
{
#ifdef END_ELEMENT_DEBUG
  cerr << "</" << name << '>' << endl;
#endif

  // take care of suffix stuff here
  PartialElementFeatures *suffix = f_suffixes.pop();

  if (suffix)
    {
      if (suffix->text())
        {
	  _DtCvSegment *container = f_current_container ;

	  if (suffix->requires_container(True))
	    {
	      container = create_container (f_current_container);
	      setup_container (container, *suffix, True);
	    }
	  else
	    {
	      if (! suffix->ignore_linebreak(True)) {
		if (suffix->linebreak() & LINEBREAK_BEFORE) {
		  insert_break (container, _DtCvNEW_LINE);
		}
	      }
	    }
	  const char *font = suffix->font() ;
	  if (font == NULL || *font == '\0')
	    font = f_font ;

	  _DtCvSegment *segment = insert_string (container,
						 font,
						 suffix->text(),
						 strlen(suffix->text()),
						 False) ;

	  // indicate that this is added information
	  segment->type |= _DtCvAPP_FLAG1 ;

	  if (suffix->linebreak() & LINEBREAK_AFTER) {
	    segment->type |= _DtCvNEW_LINE ;
	  }

	  if (suffix->requires_container(True))
	    f_current_displayable = 0;
	}
      else if (suffix->linebreak() & LINEBREAK_AFTER)
        {
	  // Element had a LINEBREAK_AFTER

	  if (f_current_displayable != 0)
	    {
	      f_current_displayable->type |= _DtCvNEW_LINE ;
	    }
	  else
	    {
	      insert_break (f_current_container, _DtCvNEW_LINE);
	    }
	}

      if (suffix->pagebreak() & PAGEBREAK_AFTER) {
	// Put page break after suffix segment.
	// NOTE: insert linebreak first if not already added.
	if (!(suffix->linebreak() & LINEBREAK_AFTER))
	  insert_break (f_current_container, _DtCvNEW_LINE);
	insert_break (f_current_container, _DtCvPAGE_BREAK);
      }
      delete suffix ;
    }

  // pop the stacks, returning state to that of current element 
  f_font = f_fstack.pop();	
  f_current_container = f_stack.pop ();
  f_link_idx = f_link_stack.pop();
  if (f_scd_stack.top())
    delete f_scd_stack.pop();
  if (f_subsuper_stack.entries())
    f_subsuper_stack.pop();

  // pop the table stack 
  // if the table that we pop is not the one we are working on, then
  // we have finished with the current table, and can build it then
  // dispose of the memory used

  TGDefn *next_tgroup = f_tgroup_stack.pop();

  if (f_current_tgroup != next_tgroup)
    {
      // convert the data we have accumulated into the _DtCvTable
      // segment that represents this table
      f_current_tgroup->build();	
      delete f_current_tgroup ;
    }
  f_current_tgroup = next_tgroup ;

  if (f_table_stack.entries()) {
    TableDefn* table = f_table_stack.pop();
    if (table) {
      if (f_table_stack.entries() == 0 || table != f_table_stack.top())
	delete table;
    }
  }

  if (f_leading_stack.entries() && f_leading_level_stack.entries())
  {
    if (f_leading_level_stack.top() == f_level)
    {
	f_leading_stack.pop();
	f_leading_level_stack.pop();
    }
  }

  --f_level;
}

void
CanvasRenderer::data(const char *data, unsigned int size)
{
  _data(data, size, True);
}

void
CanvasRenderer::_data(const char *data, unsigned int size, Boolean count_vcc)
{
  _DtCvSegment *segment ;

  if (is_literal)
    {
#ifdef LITERAL_DEBUG
      cerr << "literal: " << data << endl;
#endif
      segment = insert_literal(f_current_container, f_font, data, size);
    }
  else
    {
      segment = insert_string (f_current_container,
			       f_font, data, size, count_vcc);
    }

  if (f_link_idx != -1)
    {
      segment->link_idx = f_link_idx ;
      segment->type |= _DtCvHYPER_TEXT ;
#ifdef OL_LINK_DEBUG
      cerr << "data:( " << f_link_idx << ") " << data << endl;
#endif
    }
}

void
CanvasRenderer::handle_olias_attributes(ElementFeatures  &features,
					const Element	&element,
					const FeatureSet	& /* local */,
					const FeatureSet	&complete,
					const FeatureSet	& /*parentComplete*/
					)
{
  // check for specific olias attributes
  
  // NOTE: more efficient to iterate over attributes than to check each
  // attribute on every element (when I have time) - jbm  

  const Attribute *olxref =
		element.get_olias_attribute(gSymTab->intern("OL-XREF"));
  if (olxref) {
    UAS_String locator = olxref->value();

    UAS_Pointer<UAS_Common> target =
		gNodeViewInfo->node_ptr()->create_relative(locator);

    if (target) {
      extern char* g_mmdb_section_label;

      UAS_String title;
      if (g_mmdb_section_label && *g_mmdb_section_label)
	title = g_mmdb_section_label;
      else
	title = target->title();

      if ((char*)title && *(char*)title)
	features.xref(strdup((char*)title));

#ifdef XREF_DEBUG
    cerr << '<' << element.gi() << " xref=" << (char*)locator << '>'
	 << " target=" << (char*) (char*)target->id() << endl;
#endif
    }
  }
  
  /* -------- Locators -------- */
  const Attribute *locator = element.get_olias_attribute(*f_symbols[OLID]);
  if (locator)
    {
#ifdef LOCATOR_DEBUG
      cerr << '<' << element.gi() << " locator=" << locator->value() << '>' << endl;
#endif
      const char *locator_value = locator->value();
      
      // Need to place the locator value as an ID in the Container
      features.locator (strdup (locator_value));

    }
  
  
  /* -------- Links -------- */
  const Attribute *olidref = element.get_olias_attribute(*f_symbols[OLIDREF]);
  if (olidref)
    {
      // enter this segment into the link db

      UAS_String locator = (char*)olidref->value();

      if ((char*)locator && *(char*)locator && gNodeViewInfo) {

	// We should not really use gNodeViewInfo. CanvasRenderer should
	// a member of type UAS_Pointer<UAS_Common>

	UAS_Pointer<UAS_Common> target =
		gNodeViewInfo->node_ptr()->create_relative(locator);

	if (target) // create link only when resolved
	{
	  // hack - currently, a hint of 0 is a hypertext link and a hint
	  // of 1 is a graphic. this should really be co-ordinated with
	  // the constants used by the Canvas

	  features.link_idx (
           _DtLinkDbAddLink(
                gNodeViewInfo->topic()->link_data,		 // link db
					0,			 // id
					(char *)(olidref->value()), // spec
					_DtCvLinkType_AppDefine, // type
					0,			 // hint
					0));			 // description
	}
      }
      
#ifdef OL_LINK_DEBUG
      cerr << "idref: " << olidref->value() << " --> " << features.link_idx() << endl;
#endif

      /* -------- see if it is remote -------- */
      
      // Can we mark this anymore?
#ifdef NotDefined
      
      const Attribute *remote =
	element.get_olias_attribute(*f_symbols[REMOTE]);
      if (remote)
	{
	  // keep track of it somehow
	  // remote is only used to check for valid links at initial display
	  // time and is not used at actual link following time 
	  ON_DEBUG(cerr << "remote link: " << b_node->target() << endl);
	  b_node->remote_target(true);
	}
#ifdef DEBUG_LINK
      cerr << "link (" << b_node->target() << ") placed on: " << (void*)b_node
	<< endl ;
#endif
#endif      
    }
  
  /* -------- Graphics -------- */
  
  const Attribute *grattr =
    element.get_olias_attribute(*f_symbols[GRAPHIC_attr]);
  
  if (grattr)
    {
#ifdef GRAPHIC_DEBUG
      const Feature *graphic_display_type =
	complete.deep_lookup("GRAPHIC", "ALIGN", 0);

      cerr << "GRAPHIC" << endl;

      cerr << "Graphic( " << element.gi() << "): (" ;
      if (graphic_display_type)
	cerr << *graphic_display_type ;
      else
	cerr << "nil" ;
      cerr << ")" << endl;

      cerr << "graphic id: " << grattr->value() << endl;
#endif


      const char *graphicid = grattr->value();
      UAS_Pointer<Graphic> gr(graphics_mgr().get(gNodeViewInfo->node_ptr(), graphicid));
      PixmapGraphic *graphic = gr->graphic();

      gNodeViewInfo->add_graphic (gr); // preserve the Graphic and Pixmap objects

      // inform the PixmapGraphic that we are going to keep its pixmap
      //graphic->set_type (PixmapGraphic::PRESERVE);

      // Set up DtHelp graphic representation
      DtHelpGraphicStruct	*pGS ;
      _DtHelpDARegion		*pReg ;

      pGS = new DtHelpGraphicStruct ;
      pReg = new _DtHelpDARegion ;

      pGS->pix = graphic->pixmap();
      pGS->width = graphic->width() ;
      pGS->height = graphic->height();
      pGS->mask = 0 ;
      pGS->pixels = NULL ;
      pGS->num_pixels = 0 ;

#ifdef GRAPHIC_DEBUG
      cerr << "pixmap ( " << pGS->width << ", " << pGS->height << 
	") is: " << (void*)pGS->pix << " ==>" << (unsigned)pGS->pix << endl;   
#endif

      pReg->inited = True ;
      pReg->type = _DtHelpDAGraphic ;
      pReg->handle = (_DtCvPointer)pGS ;

      _DtCvSegment *segment = new_segment (_DtCvREGION);
      segment->handle.region.info = (_DtCvPointer)pReg ;
      segment->handle.region.width = pGS->width ;
      segment->handle.region.height = pGS->height ;
      segment->handle.region.ascent = -1 ;
      
      SegClientData scd(_DtCvREGION);
      scd.GraphicHandle((void*)(Graphic*)gr);
      // for detaching purposes
      segment->client_use = new SegClientData(scd);

#ifdef GRAPHIC_TRAVERSAL
      // if graphic is detachable, then it must be traversible
      // hack - currently, a hint of 0 is a hypertext link and a hint
      // of 1 is a graphic. this should really be co-ordinated with
      // the constants used by the Canvas
      segment->type |= _DtCvHYPER_TEXT ;
      segment->link_idx = 
	_DtLinkDbAddLink (gNodeViewInfo->topic()->link_data, // link_db
			  0,				     // id
			  grattr->value(),		     // spec
			  _DtCvLinkType_AppDefine,	     // type
			  1,				     // hint
			  0);				     // description

#endif
      // place this segment into our structure for later insertion
      // into the Canvas segment tree
      features.graphic (segment);
    }


#ifdef JBM
  
  if (grattr)
    {
      // now see if style sheet wants it inlined
      const Feature *graphic_display_type =
	complete.deep_lookup("GRAPHIC", "ALIGN", 0);
#ifdef IGRAPHIC_DEBUG
      cout << "Graphic( " << element.gi() << "): (" ;
      if (graphic_display_type)
	cout << *graphic_display_type ;
      else
	cout << "nil" ;
      cout << ")" << endl;
#endif
      
      if (graphic_display_type &&
	  !strcasecmp("inline", *graphic_display_type->value()))
	{
	  // inline graphic
	  
	  // NOTE: there may be tree problems, because graphic objects have no
	  // content, and it looks as though the tag was empty, so the tree may be
	  // built wrong after processing a graphic (especially an inline
	  // graphic). We may have to drop a branch to put this on?
	  
	  // see if it is an ismap graphic 
	  const Attribute *ismap = element.get_olias_attribute(*f_symbols[ISMAP]);
	  
	  const char *graphicid = grattr->value();
	  Pointer<Graphic> gr(graphics_mgr().get(gNodeViewInfo->node_ptr(), graphicid));
	  PixmapGraphic *graphic = gr->graphic();
	  
	  if (graphic->pixmap() != XtUnspecifiedPixmap)
	    {
	      gNodeViewInfo->add_graphic(gr);
#ifdef TML_NO_THIS_ASSIGNMENT
	      model* to_model = f_current_node->get_model();
	      igfxnode *ignode = new(to_model) igfxnode(to_model);
#else
	      igfxnode *ignode = new igfxnode(to_model);
#endif
	      if (ismap)
		ignode->set_ismap();
	      ignode->set_graphic_handle((Graphic*)gr);
	      ignode->set_graphic_dimensions(graphic->width(),
					     graphic->height());
	      f_current_node->connect_node(ignode, f_connect_dir);
	      f_current_node = ignode ;
	      f_connect_dir = n_right ;
	    }
	}
      else
	{
	  
	  // This routine puts the graphic as a child of a child of the
	  // current bnode .
	  // when we exit, current node is the new bnode, but connect dir is
	  // to the right in case there are siblings.
	  
#ifdef TML_NO_THIS_ASSIGNMENT
	  model* to_model = f_current_node->get_model();
	  node *current_node = new(to_model) bnode(to_model);
#else
	  node *current_node = new bnode(to_model);
#endif
	  b_node->connect_node(current_node, n_down);
	  current_node->vcc(f_vcc) ;
	  
	  node_dir connect_dir = n_down ;
	  
	  // space above graphic
	  
	  const Feature *spacefp = complete.deep_lookup(f_symbols[MARGIN],
							f_symbols[TOP], 0);
	  
	  int spacing = 10 ;
	  
	  if (spacefp)
	    spacing = point2pixel(*spacefp->value());
	  
	  model *the_model = current_node->get_model();
#ifdef TML_NO_THIS_ASSIGNMENT
	  space_node *spnode = new (the_model) space_node(spacing, the_model);
#else
	  space_node *spnode = new space_node(spacing, the_model);
#endif
	  current_node->connect_node(spnode, connect_dir);
	  
	  spnode->vcc(f_vcc);
	  
	  const char *graphicid = grattr->value();
	  
	  // create a gnode with one blank, so we have somewhere to attach marks 
#ifdef TML_NO_THIS_ASSIGNMENT
	  gnode *new_gnode = new (the_model) gnode(the_model);
#else
	  gnode *new_gnode = new gnode(the_model);
#endif
	  new_gnode->vcc(f_vcc);
#ifdef UseWideChars
	  wchar_t tml_blankStr[2];
	  *tml_blankStr		= (wchar_t)' ';
	  *(tml_blankStr + 1)	= (wchar_t)'\0';
					   new_gnode->attach_data(tml_blankStr,1); 
#else
	      new_gnode->attach_data(" ",1); 
#endif
	      spnode->connect_node(new_gnode, n_right);
	      
	      // now get graphic
	      Pointer<Graphic> gr(graphics_mgr().get(gNodeViewInfo->node_ptr(), graphicid));
	      PixmapGraphic *graphic = gr->graphic();
	      
	      if (graphic->pixmap() != XtUnspecifiedPixmap)
		{
		  gNodeViewInfo->add_graphic(gr);
		  
#ifdef TML_NO_THIS_ASSIGNMENT
		  gfxnode *new_gfxnode = new (the_model) gfxnode(the_model);
#else
		  gfxnode *new_gfxnode = new gfxnode(the_model);
#endif
		  
		  new_gnode->connect_node(new_gfxnode, n_right);
		  
		  
		  // NOTE: passing in REAL Graphic object
		  // this is ok because it is tracked via the node_view_info
		  new_gfxnode->set_graphic_handle((Graphic*)gr);
		  new_gfxnode->set_graphic_dimensions(graphic->width(),
						  graphic->height());
	      new_gfxnode->vcc(f_vcc);
	      
	      // insert another blank to attach end of marks
#ifdef TML_NO_THIS_ASSIGNMENT
	      gnode *tgnode = new (the_model) gnode(the_model);
#else
	      gnode *tgnode = new gnode(the_model);
#endif
	      new_gfxnode->connect_node(tgnode, n_right);
#ifdef UseWideChars
	      tgnode->attach_data(tml_blankStr,1); 
#else
	      tgnode->attach_data(" ", 1);
#endif
	      current_node = tgnode ;
	    }
	  else
	    {
	      char buf[100] ;
	      snprintf(buf, sizeof(buf),
			"Graphic \"%s\" unavailable", graphic);
#ifdef TML_NO_THIS_ASSIGNMENT
	      new_gnode = new (the_model) gnode(the_model);
#else
	      new_gnode = new gnode(the_model);
#endif
	      new_gnode->vcc(f_vcc);
#ifdef UseWideChars
	      wchar_t* buffer = new wchar_t[ strlen(buf) + 1 ];
	      int nc = mbstowcs(buffer, buf, strlen(buf) + 1);
	      assert( nc >= 0 );
	      new_gnode->attach_data(buffer, nc);
	      delete[] buffer;
#else
	      new_gnode->attach_data(buf, strlen(buf));
#endif
	      spnode->connect_node(new_gnode, n_right);
	      current_node = new_gnode;
	    }
	  // more spacing at end of graphic
#ifdef TML_NO_THIS_ASSIGNMENT
	  spnode = new (the_model) space_node(spacing, the_model);
#else
	  spnode = new space_node(spacing, the_model);
#endif
	  spnode->vcc(f_vcc);
	  current_node->connect_node(spnode, n_right);
	  
	  // set current node to child of bnode so that EndElement knows that node
	  // had content, and sets the next node to be connected to the right 
	  // the node 
	  f_current_node = b_node->get_node(n_down) ;
	  f_connect_dir = n_right ;
	  
	}
    }
#endif
  
  // Terms must come last, otherwise vcc will be screwed up for above items
  
  /* -------- Terms -------- */
  const Attribute *terms = element.get_olias_attribute(*f_symbols[TERMS]);
  if (terms)
    {
      int vcc = 0 ;
      const char *p = terms->value();
#ifdef UseWideChars
      while (*p) {
	int mb_len = mblen(p, MB_CUR_MAX);
	assert( mb_len > 0 );
	if (mb_len == 1) {
	  const unsigned char ch = (unsigned char)*p++;
	  if (ch == ' '  || // space
	      ch == '\t' || // tab
	      ch == '\n' || // newline
	      ch == 0xA0) // nbsp
	    continue;
	}
	else
	  p += mb_len;
	
	vcc++;
      }
#else
      while (*p)
	{
	  if (!((*p == ' ') || // space
		(*p == '	') || // tab
		(*p == '\n') || // newline
		(*p == (char)0xA0))) // nbsp
	    {
	      vcc++ ;
	    }
	  p++;
	}
#endif
      f_vcc += vcc;
    }
}

static
_DtCvSegment *
insert_break (_DtCvSegment *container, unsigned long type)
{
  _DtCvSegment *segment = new_segment (_DtCvNOOP);
  segment->type |= type;
  insert_segment (container, segment) ;
  return segment ;
}

void
CanvasRenderer::setup_cascaded_tgroup_container(_DtCvSegment* parent,
						_DtCvSegment* child)
{
  if (! (parent && child))
    return;
  if (! (parent->type & _DtCvCONTAINER && child->type & _DtCvCONTAINER))
    return;

  // orient,...,flow (controller specific) are irrelevant
  child->handle.container.type     = parent->handle.container.type;
  child->handle.container.leading  = parent->handle.container.leading;
  child->handle.container.fmargin  = 0;
  child->handle.container.rmargin  = 0;
  child->handle.container.tmargin  = 0;
  child->handle.container.bmargin  = 0;
  child->handle.container.bdr_info = parent->handle.container.bdr_info;
  child->handle.container.justify  = _DtCvINHERIT;
  child->handle.container.vjustify = _DtCvINHERIT;

  TableDefn::table_frame_t frame = f_current_tgroup->table()->frame();

  switch (frame) {
    case TableDefn::table_frame_top:
      child->handle.container.border   = _DtCvBORDER_TOP;
      break;
    case TableDefn::table_frame_bottom:
      child->handle.container.border   = _DtCvBORDER_BOTTOM;
      break;
    case TableDefn::table_frame_topbot:
      child->handle.container.border   = _DtCvBORDER_HORZ;
      break;
    case TableDefn::table_frame_sides:
      child->handle.container.border   = _DtCvBORDER_VERT;
      break;
    case TableDefn::table_frame_all:
    default:
      child->handle.container.border   = _DtCvBORDER_FULL;
      break;
  }
}

void
CanvasRenderer::setup_cascaded_container(_DtCvSegment*)
{}

_DtCvSegment *
CanvasRenderer::create_cascaded_container(_DtCvSegment*)
{
  return NULL;
}

static
_DtCvSegment *
new_segment(unsigned long type)
{
  _DtCvSegment *segment = new _DtCvSegment ;

  { // zero-out fields
    unsigned size = sizeof(segment->handle);
    //cerr << "Handle size = " << size << endl;
    char *p = (char *)&(segment->handle) ;
    for (unsigned int i = 0; i < size; i++, p++)
	  *p = 0;
  }
  
  segment->type		= type ;
  segment->link_idx	= -1 ;
  segment->next_seg	= NULL ;
  segment->next_disp	= NULL ;
  segment->client_use	= NULL ;
  segment->internal_use	= NULL ;
  
  switch (type)
    {
    case _DtCvCONTAINER:
      segment->handle.container.id	= 0 ;
      segment->handle.container.type	= _DtCvDYNAMIC ; 
      segment->handle.container.border  = _DtCvBORDER_NONE ;
      segment->handle.container.justify = _DtCvJUSTIFY_LEFT ;
      segment->handle.container.vjustify= _DtCvJUSTIFY_TOP;
      segment->handle.container.orient	= _DtCvJUSTIFY_LEFT_CORNER ;
      segment->handle.container.vorient = _DtCvJUSTIFY_CENTER ;
      segment->handle.container.flow	= _DtCvWRAP;
      segment->handle.container.percent = 0 ;
      segment->handle.container.leading = DEFAULT_CONTAINER_LEADING ;
      segment->handle.container.fmargin = 0 ;
      segment->handle.container.lmargin = 0 ;
      segment->handle.container.rmargin = 0 ;
      segment->handle.container.tmargin = 0 ;
      segment->handle.container.bmargin = 0 ;
      segment->handle.container.justify_char = NULL ;
#if 0
      segment->handle.container.bdr_info.width = 1 ;
#else
      segment->handle.container.bdr_info.width = 0 ;
#endif
      segment->handle.container.bdr_info.data = NULL ;
      segment->handle.container.seg_list = 0;
      break;

    case _DtCvSTRING:
      if (MB_CUR_MAX > 1)
	segment->type |= _DtCvWIDE_CHAR;
      segment->handle.string.string = NULL;
      segment->handle.string.font   = NULL;
      break;

    case _DtCvREGION:
      segment->type |= _DtCvIN_LINE;
      break;
      
    default:
      break;
    }
  
  return segment ;
}

_DtCvSegment *
CanvasRenderer::create_container(_DtCvSegment *parent)
{
  _DtCvSegment *container = new_segment(_DtCvCONTAINER);
  
  if (parent)			
    {
      _DtCvSegment *tseg = parent->handle.container.seg_list ;
  
      if (tseg)
	{
	  while (tseg->next_seg)
	    tseg = tseg->next_seg ;
      
	  tseg->next_seg = container ;
	}
      else
	parent->handle.container.seg_list = container ;
  
    }
  return container ;
  f_current_displayable = 0 ;
}

_DtCvSegment *
CanvasRenderer::insert_literal(_DtCvSegment *container,
			       const char *font, const char *data,
			       unsigned int size)
{
  // We have a literal element. This has to be broken down into
  // individual string segments, separated by DtCvNEW_LINE segments

  char *start = (char *) data;		// start of current substring

  char *cr;
  // replace all <CR> with newlines
  while ((cr = strchr(start, '\015')))
  {
    *cr = '\n'; 	// replace with newline
    start = cr ;
    start++ ;		// advancd beyond the newline
  }

  return insert_string(container, font, data, size);
}

_DtCvSegment *
CanvasRenderer::insert_string (_DtCvSegment *container,
			       const char *font, const char *data,
			       unsigned int /*size*/, Boolean count_vcc)
{
  _DtCvSegment *seg = NULL ;

  char *start = (char *)data; 
  char *newline ;

  // find the newlines, and make a string segment up to that point,
  // then advance our start pointer beyond that point

  for (; (newline = strchr(start, '\n')); start = newline + 1) {

      seg = really_insert_string(container, font, start, (newline - start), count_vcc);
      _DtCvSegment *lbseg = insert_break(container, _DtCvNEW_LINE);

      f_current_displayable->next_disp = lbseg ;
      f_current_displayable = lbseg ;

  }

  // at end of data now
  if (strlen(start) > 0)
    seg = really_insert_string(container, font, start, strlen(start), count_vcc);

  return seg ;
}

_DtCvSegment *
CanvasRenderer::really_insert_string (_DtCvSegment *container,
				      const char *font, const char *data,
				      unsigned int size, Boolean count_vcc)
{
  // put data into a string segment
  
  _DtCvSegment *strseg = new_segment(_DtCvSTRING);

  insert_segment (container, strseg);

  // set sub/super script attribute to the segment
  if (f_subsuper_stack.entries() &&
      f_subsuper_stack.top() > PartialElementFeatures::baseline) {

    if (f_subsuper_stack.top() == PartialElementFeatures::subscript)
      strseg->type |= _DtCvSUB_SCRIPT;
    else if (f_subsuper_stack.top() == PartialElementFeatures::superscript)
      strseg->type |= _DtCvSUPER_SCRIPT;
#ifdef SUBSUPER_DEBUG
    else
      cerr << "(WARNING) unknown value specified as sub/super script."
	   << endl;
#endif
  }
  
  if (f_current_displayable)
    f_current_displayable->next_disp = strseg ;
  
  f_current_displayable = strseg ;
  
  long ret_indx ;
  // calculate dthelp font index
  {
    // WARNING: this routine keeps the ptr to the xlfd_spec
    //          should be ok unless they try to delete it
    //          as it is a const and therefore static to us
    
    /* resolve current language (order of precedence : LC_ALL,LC_TYPE,LANG) */
    const char* lang;
    if ((lang = getenv("LC_ALL")) == NULL)
      if ((lang = getenv("LC_CTYPE")) == NULL)
	if ((lang = getenv("LANG")) == NULL)
	  lang = "C";

    _DtHelpGetExactFontIndex(gHelpDisplayArea, // Display Area Structure
			     lang, // lang
			     0, // char_set
			     (char*) font,	// xlfd_spec
			     &ret_indx // returned index
			     );
#ifdef FONT_INDEX_DEBUG  
    cerr << " indx: " << ret_indx << " " << font << endl;
#endif
  }
  strseg->handle.string.font = (_DtCvPointer)(size_t) ret_indx ;

   // copy data into new memory to hand over to the canvas
  char *string = new char[size + 1];
  memcpy(string, data, size);
  string[size] = '\0';

  unsigned int offset;
  if (ret_indx < 0 &&
      strcmp((char*)gNodeViewInfo->node_ptr()->locale(), "ja_JP.EUC-JP") == 0)
  {
#ifdef CR_JP_DEBUG
    cerr << "<CR> is being processed for Japanese." << endl;
    cerr << "string=" << string << endl;
#endif
    unsigned char* strp;

    while ((strp = (unsigned char*)strchr(string, '\015'))) {
#ifdef CR_JP_DEBUG
      cerr << "<CR> found...";
#endif
      unsigned char *last = NULL, *next = NULL;

      for (last = strp;
	   (char*)last >= string && *last <= (unsigned char)' ';
	   last--);
      for (next = strp;
	   (char*)next < string + size && *next <= (unsigned char)' ';
	   next++)

      // check for boundary violation
      if ((char*)last < string || (char*)next == string) {
#ifdef CR_JP_DEBUG
        cerr << "replaced with a space" << endl;
#endif
	*strp = ' ';
	continue;
      }

      if (*last < 128 || *next < 128) { // between asciis 
#ifdef CR_JP_DEBUG
        cerr << "replaced with a space" << endl;
#endif
	*strp = ' ';
      }
      else {
#ifdef CR_JP_DEBUG
        cerr << "marked as 0x90" << endl;
#endif
	*strp = 0x90; // mark to be removed later (works only for EUC)
      }
    }

    char *buf = new char[size + 1], *bufp;

    for (bufp = buf, offset = 0; offset < size && string[offset]; offset++)
    {
      unsigned char& c = (unsigned char &)string[offset];

      if (c != 0x90)
	*bufp++ = c;
    }
    *bufp = 0;

    delete[] string;
    string = buf;
  }
  else
  {
    for (offset = 0; offset < size && string[offset]; offset++)
    {
      char& pos = string[offset];
      pos = (pos != '\015') ? pos : ' ';
    }
    // no need for string termination here
  }

#ifdef DEBUG_R_INPUT_STRING
  cout << string ;
#endif

  SegClientData scd(_DtCvSTRING);
  scd.vcc(f_vcc);

  if (strseg->type & _DtCvWIDE_CHAR)
  {
    if (ret_indx < 0) {
      WString wstring(string, size);
#ifdef CRE_I18N_DEBUG
      fprintf(stderr, "(DEBUG) wstring=\"%s\"\n", wstring.get_mbstr());
#endif
      if ((wchar_t*)wstring == NULL || *(wchar_t*)wstring == 0) {
	strseg->type &= ~_DtCvWIDE_CHAR;
      }
      else {
	delete[] string;
	string = (char*)wstring.get_wstr();
      }
    }
    else { // single font being used, do not use wchar_t
      strseg->type &= ~_DtCvWIDE_CHAR;
    }
  }

  // account for vcc for this string
  scd.vclen(0);
  if (strseg->type & _DtCvWIDE_CHAR) {
    wchar_t *p;
    for (p = (wchar_t*)string; *p; p++) {
      if (*p != ' '  && *p != '\t' && *p != '\n')
	scd.vclen()++;
    }
  }
  else { // also need to exclude nbsp
    unsigned char* p;
    for (p = (unsigned char*)string; *p; p++) {
      if (*p != ' ' && *p != '\t' && *p != '\n' && *p != 0xA0)
	scd.vclen()++;
    }
  }

  // setup highlight info
  strseg->client_use = new SegClientData(*f_scd_stack.top());
  if (count_vcc) {
    f_vcc += scd.vclen();
    ((SegClientData*)strseg->client_use)->vcc  (scd.vcc());
    ((SegClientData*)strseg->client_use)->vclen(scd.vclen());
  }
  
  strseg->handle.string.string = string ;
  
  
#ifdef HILITE_DEBUG
  _DtCvStringClientData* pSCD = (_DtCvStringClientData*)strseg->client_use;
  if (pSCD) {
    fprintf(stderr, "(DEBUG) vcc=%d vclen=%d, type=0x%x\n",
			pSCD->vcc, pSCD->vclen, pSCD->hilite_type);
  }
#endif

  return strseg ;
}

void
CanvasRenderer::setup_container(_DtCvSegment *container,
				PartialElementFeatures &features,
				int affix)
{
  const char* font = (char*)features.font();
  if (font && *font)
    f_font = features.font();
  
  // position
  if (features.position().horiz() != _DtCvOPTION_BAD)
    {
      container->handle.container.orient = features.position().horiz();
      container->type |= _DtCvCONTROLLER ;
    }
  
  if (features.position().vert() != _DtCvOPTION_BAD)
    {
      container->handle.container.vorient = features.position().vert();
      container->type |= _DtCvCONTROLLER ;
    }
  
  // layout
  if (features.layout().has_layout(affix))
    {
      Layout &layout = features.layout();
      
      container->handle.container.fmargin = layout.findent();
      container->handle.container.lmargin = layout.lindent();
      container->handle.container.rmargin = layout.rindent();
      container->handle.container.tmargin = layout.aspace();
      container->handle.container.bmargin = layout.bspace();
      
      if (layout.flow() != _DtCvOPTION_BAD)
	{
	  // NOTE: container is not a controller just because this
	  // flag gets set, otherwise you will see all the objects
	  // grouped together off to one side (upper left by default)
	  container->handle.container.type = layout.flow();
	}
      if (layout.wrap() != _DtCvOPTION_BAD)
	{
	  container->handle.container.flow = layout.wrap();
	  container->type |= _DtCvCONTROLLER ;	  
	}
      if (layout.justify() != _DtCvOPTION_BAD)
	{
	  container->handle.container.justify = layout.justify();
	}
    }

    if (features.layout().has_layout(affix)) {
	container->handle.container.leading = features.layout().leading();
    }
    else if (f_leading_stack.entries()) {
	container->handle.container.leading = f_leading_stack.top();
    }
    else {
 	container->handle.container.leading = DEFAULT_CONTAINER_LEADING;
    }
}

void
CanvasRenderer::setup_container(_DtCvSegment *container, ElementFeatures &features)
{
    setup_container (container, *(PartialElementFeatures*)&features, False);
    
    // for online use, set the margins for the container
    // when printing, the margins will be set on the form

    if (!window_system().printing()) {
	
	// margins
	container->handle.container.fmargin += features.margin().first() ;
	container->handle.container.lmargin += features.margin().left() ;
	container->handle.container.rmargin += features.margin().right();
	container->handle.container.tmargin += features.margin().top(); 
	container->handle.container.bmargin += features.margin().bottom();
    }
    
    // border
    container->handle.container.border = features.border();
    if (features.border_width() < 0)
	container->handle.container.bdr_info.width = f_border_width;
    else
	container->handle.container.bdr_info.width = features.border_width();
}


PartialElementFeatures::PartialElementFeatures(CanvasRenderer* renderer)
: f_text (0),
  f_font(0),
  f_subsuper(baseline),
  f_linebreak(0),
  f_pagebreak(PAGEBREAK_NONE),
  f_layout(renderer),
  f_graphic (0),
  f_ignore(0),
  f_orientation("portrait")

{
}

PartialElementFeatures::PartialElementFeatures (PartialElementFeatures &features)
: f_text (features.f_text),
  f_font (features.f_font),
  f_subsuper (features.f_subsuper),
  f_highlight (features.f_highlight),
  f_linebreak (features.f_linebreak),
  f_pagebreak (features.f_pagebreak),
  f_position (features.f_position),
  f_layout (features.f_layout),
  f_graphic (features.f_graphic),
  f_ignore (features.f_ignore),
  f_orientation (features.f_orientation)
{
}


PartialElementFeatures::~PartialElementFeatures()
{
}

ElementFeatures::ElementFeatures(CanvasRenderer* renderer)
: PartialElementFeatures(renderer),
  f_row (NULL),
  f_graphic (0),
  f_border (_DtCvBORDER_NONE),
  f_border_width(-1), // invalid border width
  f_table (NULL),
  f_tgroup (NULL),
  f_colformat (NULL),
  f_prefix(renderer),
  f_suffix(renderer),
  f_locator (0),
  f_xref(NULL),
  f_link_idx (-1)
{}

ElementFeatures::~ElementFeatures()
{
  if (f_xref) free(f_xref);
}

int
PartialElementFeatures::ignore_linebreak(int affix)
{
  return (layout().has_layout(affix) ||
  	  position().has_position());
}

int
ElementFeatures::ignore_linebreak(int)
{
  if (prefix().text() && prefix().ignore_linebreak(True))
      return 1;

  if (suffix().text() && suffix().ignore_linebreak(True))
      return 1;

  return (PartialElementFeatures::ignore_linebreak(False) ||
	  margin().has_margins());
}

int
PartialElementFeatures::requires_container(int affix)
{
  return (layout().has_layout(affix) ||
	  position().has_position());
}

int
ElementFeatures::requires_container(int)
{
  return (PartialElementFeatures::requires_container(False) ||
	  margin().has_margins() ||
	  (border() != _DtCvBORDER_NONE) ||
	  cell().has_cell());
}


Margins::Margins()
: f_first (0),
  f_left  (0),
  f_right (0),
  f_top   (0),
  f_bottom(0)
{
}

int
Margins::has_margins()
{
  // returns a non zero (true) value if any margin setting
  // exists
  return f_first | f_left | f_right | f_top | f_bottom ;
}

Layout::Layout(CanvasRenderer* renderer)
: f_renderer(renderer),
  f_aspace (0), f_bspace (0), f_leading (-1),
  f_findent (0), f_rindent(0), f_lindent (0),
  f_flow (_DtCvOPTION_BAD),
  f_justify (_DtCvOPTION_BAD),
  f_wrap (_DtCvOPTION_BAD)
{
}

Layout::Layout (Layout &layout)
: f_renderer(layout.f_renderer),
  f_aspace (layout.f_aspace),
  f_bspace (layout.f_bspace),
  f_leading (layout.f_leading),
  f_findent (layout.f_findent),
  f_rindent (layout.f_rindent),
  f_lindent (layout.f_lindent),
  f_flow (layout.f_flow),
  f_justify (layout.f_justify),
  f_wrap (layout.f_wrap)
{
}

unsigned
Layout::has_layout(int affix)
{
  if (f_leading >= 0)
  {
    if (affix) {
      if (f_renderer->current_leading() != f_leading)
	return True;
    }
    else {
      if (f_renderer->f_level == f_renderer->f_leading_level_stack.top() ||
	f_renderer->current_leading() != f_leading)
      return True;
    }
  }

  return f_aspace || f_bspace || f_findent || f_rindent || f_lindent || 
	(f_flow != _DtCvOPTION_BAD) ||
	(f_wrap != _DtCvOPTION_BAD) ||
	  (f_justify != _DtCvOPTION_BAD) ;
}

int
CanvasRenderer::current_leading()
{
  if (f_leading_stack.entries() == 0)
    return -1;

  return f_leading_stack.top();
}

static
void
insert_segment (_DtCvSegment *container, _DtCvSegment *segment)
{
#ifdef C1_DEBUG
  if (container == segment)
    cerr << "ERROR: Containing ourself" << endl;
  cerr << (void*)container << " <-- " << (void*)segment << endl;
#endif
  _DtCvSegment *child =
    container->handle.container.seg_list ;
  
  if (!child)
    {
      // first data in this segment
      container->handle.container.seg_list = segment ;
    }
  else
    {
      while (child->next_seg)
	child = child->next_seg ;
      
      child->next_seg = segment ;
      
    }
}

RowDefn::RowDefn() : f_vjustify(_DtCvOPTION_BAD), f_rowsep(CRSEP_NOT_SPECIFIED)
{}
    
RowDefn::~RowDefn() 
{ 
  f_columns.clearAndDestroy ();
}

TableDefn::TableDefn(const char* frame_str, int colsep, int rowsep) :
	f_frame(table_frame_default), f_colsep(colsep), f_rowsep(rowsep)
{
  if (frame_str)
    f_frame = string_to_token(frame_str);
}

TableDefn::~TableDefn()
{}

void
TableDefn::frame(const char* frame_str)
{
  if (frame_str)
    f_frame = string_to_token(frame_str);
  else
    f_frame = table_frame_default;
}

TableDefn::table_frame_t
TableDefn::string_to_token(const char* frame_str)
{
  if (frame_str == NULL)
    return table_frame_default;

  table_frame_t token;

  if (! strcasecmp(frame_str, "none"))
    token = table_frame_none;
  else if (! strcasecmp(frame_str, "top"))
    token = table_frame_top;
  else if (! strcasecmp(frame_str, "bottom"))
    token = table_frame_bottom;
  else if (! strcasecmp(frame_str, "topbot"))
    token = table_frame_topbot;
  else if (! strcasecmp(frame_str, "sides"))
    token = table_frame_sides;
  else if (! strcasecmp(frame_str, "all"))
    token = table_frame_all;
  else
    token = table_frame_default;

  return token;
}

TGDefn::TGDefn(_DtCvFrmtOption justify, _DtCvFrmtOption vjustify) :
	f_numcols (1),f_segment (0), f_justify(justify), f_vjustify(vjustify),
	f_colsep(CRSEP_NOT_SPECIFIED), f_rowsep(CRSEP_NOT_SPECIFIED),
	f_char_align('\0'), f_table(NULL)
{}

TGDefn::~TGDefn()
{
  f_colformats.clearAndDestroy();
  f_rows.clearAndDestroy();
}

void
TGDefn::add_row(RowDefn *rd)
{
  if (rd)
    f_rows.append (rd);
}

void
TGDefn::add (ColFormat *cf)
{
  if (cf)
    f_colformats.append (cf);
}


void 
TGDefn::add (ColDefn *cd)
{
  if (f_rows.entries() > 0) {
    f_rows.last()->append (cd);
  }
}

#if 0
static void
add_id(char **cell_ids, unsigned row, unsigned number, _DtCvSegment *segment)
{
  char *id = segment->handle.container.id;
  char idstr[64];
  int cell_idslen;

  if (id == NULL)
    {
      snprintf(idstr, sizeof(idstr), "id%d", number);
      
      id = idstr ;
      segment->handle.container.id = strdup (id);
    }
  if (cell_ids[row] == NULL)
    {
      cell_ids[row] = strdup (id) ;
    }
  else
    {
      char *orig = cell_ids[row] ;
      cell_idslen = strlen (orig) + 1 + strlen (id) + 1 ;
      cell_ids[row] = new char [ cell_idslen ] ;
      snprintf(cell_ids[row], cell_idslen, "%s %s", orig, id);
    }
}
#endif


ColFormat::ColFormat() 
: f_char_align ('.'),
  f_name(NULL),
  f_justify (_DtCvOPTION_BAD),
  f_width (1),
  f_colsep(CRSEP_NOT_SPECIFIED),
  f_rowsep(CRSEP_NOT_SPECIFIED)
{}


ColFormat::~ColFormat()
{
  if (f_name)
    delete f_name ;
}

const char *
ColFormat::name(const char *string)
{
  f_name = strdup (string);
  return name() ;
}
     
static
_DtCvSegment *ensure_id(_DtCvSegment *segment)
{
  static unsigned id_count = 0;
  // in a table, an id cannot contain a space, so if it does
  // we wrap it in a new container
  if ((segment->handle.container.id != NULL) &&
      strchr(segment->handle.container.id, ' '))
    {
      _DtCvSegment *new_container = new_segment (_DtCvCONTAINER);
      new_container->handle.container.seg_list = segment ;
      segment = new_container ;
    }

  // generate an id if the segment does not have one
  if (segment->handle.container.id == NULL)
    {
      char buffer[16] ;
      snprintf(buffer, sizeof(buffer), "id%d", id_count++);
      segment->handle.container.id = strdup (buffer);
    }
  return segment ;
}


ColFormat *
TGDefn::find_format(const char *name, int* index)
{
  if (name == NULL || *name == '\0')
    return NULL;

  ColFormat* format = NULL;

  CC_TPtrSlistIterator<ColFormat> cf_cursor (f_colformats);
      
  int nth;
  for (nth = 0; ++cf_cursor; nth++)
    {
      if (cf_cursor.key()->name() == NULL)
	continue;

#ifdef TABLE_DEBUG_X
      cerr << "\tname: " << cf_cursor.key()->name() << endl;
      cerr << "\tchar: " << cf_cursor.key()->char_align() << endl;
      cerr << "\tjustify: " << cf_cursor.key()->justify() << endl;
      cerr << "\twidth: " << cf_cursor.key()->width() << endl;
#endif

      if (!strcmp(name, cf_cursor.key()->name())) {
	format = cf_cursor.key();
	break;
      }
    }

  if (format && index) // if found, return the index to the format
    *index = nth;

  return format;
}


void
TGDefn::build()
{
  unsigned i, len, slen, lent, leng;
  unsigned num_cells   = 0; // # of virtual cells
  unsigned num_rows    = 0; // # of physical rows
  unsigned num_columns = 0; // # of physical columns

  // calculate the actual number of items we have to distribute
  // throughout the table, as well as the widest row (in terms of
  // number of columns)

  // calculate the number of rows we really have
  // NOTE: Here we calculate the physically accurate number of
  //       rows, not # of <row>s. Some cells can have rowspan > 1
  //       which means more rows may be required than # of <row>s.
  
  CC_TPtrSlistIterator<RowDefn> row_cursor(f_rows) ;

  int current_row;
  for (current_row = 0; ++row_cursor; current_row++)
  {
    CC_TPtrSlistIterator<ColDefn> col_cursor (row_cursor.key()->columns());
    while (++col_cursor)
    {
	// extent the number of rows if this columns spans below
	// the last row calculated so far
	unsigned rowspan = col_cursor.key()->spanrows();

	unsigned potential_last_row = current_row + rowspan ;
	if (num_rows < potential_last_row)
	  num_rows = potential_last_row ;
    }
  }
#ifdef TABLE_DEBUG
  fprintf(stderr, "(DEBUG) # of physical rows = %d\n", num_rows);
#endif

  // find out how many virtual cells we actually have, and the number
  // of columns in the table
  // NOTE: Here we calculate the physically accurate number of
  //       columns, instead of using numcols of <tgroup>.

  unsigned *col_count = new unsigned[num_rows] ;
  for (i = 0 ; i < num_rows; i++)
    col_count[i] = 0 ; 

  row_cursor.reset();

  for (current_row = 0; ++row_cursor; current_row++)
    {
      unsigned entries = row_cursor.key()->columns().entries() ;
      num_cells += entries ;

      // now find out how many columns we span
      CC_TPtrSlistIterator<ColDefn> col_cursor (row_cursor.key()->columns());
      

      // adding up the spans of each column in the row
      // if we rowspan, include our count in the spanned rows

      while (++col_cursor)
	{
	  ColDefn *col = col_cursor.key() ;

	  int mth;
	  int spancols = 1;
	  if (find_format(col->colstart(), &mth)) {
	    int nth;
	    if (find_format(col->colend(), &nth)) {
	      if (mth > nth) {
		int anonym = mth;
		mth = nth;
		nth = anonym;
	      }
	      spancols += nth - mth;
	    }
	  }

	  for (i = 0; i < col->spanrows(); i++)
	      col_count[current_row + i] += spancols;
	}
    }

  for ( i = 0 ; i < num_rows; i++)
    if (num_columns < col_count[i])
      num_columns = col_count[i] ;

  delete[] col_count;

#ifdef TABLE_DEBUG
  fprintf(stderr, "(DEBUG) # of physical columns = %d\n", num_columns);
  fprintf(stderr, "(DEBUG) # of virtual cells = %d\n", num_cells);
#endif

  // create a grid of our table, then populate each box in the grid
  // we then walk the grid to build our DtCvTable structure

  // allocate row memory
  _DtCvSegment* **grid = new _DtCvSegment **[num_rows] ;

  // allocate column memory
  for (i = 0 ; i < num_rows ; i++)
    {
      grid[i] = new _DtCvSegment *[num_columns]  ;
      for (unsigned int c = 0 ; c < num_columns; c++)
	grid[i][c] = NULL ;
    }

  // allocate space in _DtCvTable for storing the segments
  f_segment->handle.table.cells = new _DtCvSegment* [num_rows*num_columns + 1];
  for (i = 0; i <= num_rows*num_columns; i++)
    f_segment->handle.table.cells[i] = NULL ; // initialize list


  if (f_colformats.entries() < num_columns) {
    int i, deficit = num_columns - f_colformats.entries();

    for (i = 0; i < deficit; i++)
	f_colformats.append(new ColFormat());

#ifdef TABLE_DEBUG
    // print warning message if num_columns exceeds # of colformat
    cerr << "(WARNING) # of physical cols exceeds that of colformats" << endl;
    cerr << "# of cols = " << num_columns << ", # of colformats = "
	 << f_colformats.entries() << endl;
#endif
  }

  // now walk the structure that we built up during parsing, and fill
  // in the grid

  unsigned cell_counter = 0 ;
  row_cursor.reset();
  for (current_row = 0; ++row_cursor; current_row++)
  {
      CC_TPtrSlist<ColDefn> columns(row_cursor.key()->columns());

      int vc, kept, count = columns.entries();
      for (vc = 0, kept = 0; vc < count; vc++) { // iterate for virtual cells
#if 0
      fprintf(stderr, "(vc,kept,count)=(%d,%d,%d)\n", vc, kept, count);
#endif

	ColDefn* colcell;
	mtry {
	  colcell = columns.at(kept);
	}
	mcatch_any() {
	  abort(); // consider it as fatal
	}
	end_try;

	int start_index;
	int spancols = 1;
	ColFormat* msformat; // most  significant format
	ColFormat* lsformat; // least significant format
	if ((msformat = find_format(colcell->colstart(), &start_index))) {
	  int end_index;
	  if ((lsformat = find_format(colcell->colend(), &end_index))) {
	    if (start_index > end_index) {
		int anonym = start_index;
		start_index = end_index;
		end_index = anonym;
	    }
	    spancols += end_index - start_index;
	  }
	  else
	    lsformat = msformat;
	}
	else if (! (msformat = find_format(colcell->colref(), &start_index))) {
	  // neither colstart nor colref are specified.
	  // Handle implicitly later.
	  ++kept;
	  continue;
	}
	else
	  lsformat = msformat;

	// make sure the segment has a valid id and add it into the 
	// _DtCvTable list of cells
	_DtCvSegment *segment = ensure_id(colcell->parseg());
	setup_cell(segment, colcell, row_cursor.key(), msformat, lsformat);

	for (int i = 0 ; i < spancols; i++) {
	  for (unsigned int j = 0; j < colcell->spanrows(); j++) {
	    // if there's entrenchment from above rows, skip it.
	    if (grid[current_row + j][start_index + i])
	      continue;
	    grid[current_row + j][start_index + i] = segment;
	  }
	}

	f_segment->handle.table.cells[cell_counter++] = segment ;

	columns.removeAt(kept);
      }

      count = columns.entries();
      for (vc = 0, kept = 0; vc < count; vc++) // iterate for virtual cells
      {
	// stick the item into each grid of the table that it
	// occupies, including spanning rows and columns
#if 0
	fprintf(stderr, "[vc,kept,count]=[%d,%d,%d]\n", vc, kept, count);
#endif
	ColDefn* colcell;
	mtry {
	  colcell = columns.at(kept);
	}
	mcatch_any() {
	  abort();
	}
	end_try;

	unsigned int i, start_index;
	for (i = 0; i < num_columns; i++) {
	  if (grid[current_row][i] == NULL) {
	    start_index = i;
	    break;
	  }
	}
	if (i == num_columns) { // all columns occupied
	  ++kept;
	  continue;
	}

	ColFormat* msformat = f_colformats.at(start_index);
	ColFormat* lsformat = msformat;

	// make sure the segment has a valid id and add it into the 
	// _DtCvTable list of cells
	_DtCvSegment *segment = ensure_id(colcell->parseg());

	setup_cell(segment, colcell, row_cursor.key(), msformat, lsformat);

	f_segment->handle.table.cells[cell_counter++] = segment ;

	for (i = 0; i < colcell->spanrows(); i++) {
	    // if there's entrenchment from above rows, skip it.
	    if (grid[current_row + i][start_index])
	      continue;
	    grid[current_row + i][start_index] = segment;
	}

	columns.removeAt(kept);
      }

      for (unsigned int c = 0; c < num_columns; c++)
      {
	if (grid[current_row][c] == NULL)
	{
	  _DtCvSegment* seg = ensure_id(new_segment(_DtCvCONTAINER));
	  setup_cell(seg, NULL, row_cursor.key(), f_colformats.at(c),
						  f_colformats.at(c));

	  grid[current_row][c] =
	  f_segment->handle.table.cells[cell_counter++] = seg;
	}
      }
  }
  
  // temp for now deal with blank spots in the table
  unsigned r;
  for (r = 0 ; r < num_rows ; r++)
    for (unsigned c = 0; c < num_columns ;  c++)
      {
	if (grid[r][c] == NULL)
	  {
#ifdef TABLE_DEBUG
	    fprintf(stderr, "(DEBUG) blank spot found in the table.\n");
#endif
	    unsigned int col;

	    if (c == 0)
	    {
	      // look for a meaningful grid
	      for (col = 0; col < num_columns && grid[r][col] == NULL; col++);

	      if (col == num_columns) // not found
		abort();
	      else
	      {
		_DtCvSegment* filler = grid[r][col];

		for (unsigned int i = c; i < col; i++)
		  grid[r][i] = filler;
	      }
	    }
	    else {
	      col = c - 1 ;
	      while (grid[r][col] == NULL)
		col-- ;

	      _DtCvSegment* filler = grid[r][col] ;
	      col++ ;
	      while (col <= c)
		grid[r][col++] = filler ;
	    }
	  }
      }

  // now create the _DtCvTable layout information
  _DtCvTable *table = &f_segment->handle.table ;

  table->num_cols = num_columns ;

  table->cell_ids = new char *[num_rows + 1] ;
  table->cell_ids[num_rows] = NULL;

  // walk the grid and insert the segment id information
  // into the cell_ids strings

  for (r = 0 ; r < num_rows ; r++)
    {    
      for (unsigned int c = 0 ; c < num_columns ; c++)
	{
	  if (c == 0)
	    {
	      // first item in row
	      table->cell_ids[r] = strdup(grid[r][c]->handle.container.id);
	    }
	  else
	    {
	      // subsequent items, append space separated id

	      lent = strlen (table->cell_ids[r]);
	      leng = strlen (grid[r][c]->handle.container.id);
	      char *new_ids = new char [lent + leng + 2] ;
	      *((char *) memcpy(new_ids, table->cell_ids[r], lent) +lent) ='\0';
	      *((char *) memcpy(new_ids + lent, " ", 1) + 1) = '\0';
	      *((char *) memcpy(new_ids + lent + 1,
			grid[r][c]->handle.container.id, leng) + leng) = '\0';
	      delete table->cell_ids[r] ;
	      table->cell_ids[r] = new_ids ;
	    }
	}
      delete grid[r] ;		// clean up column memory
    }

  delete grid ;			

  // now  apply the formats to the _DtCvTable
  { 
    _DtCvFrmtOption *justify = new _DtCvFrmtOption[num_columns] ;
    char **col_widths = new char *[num_columns] ;
    char *justify_chars = new char [num_columns + 1] ;
    
    justify_chars[0] = 0 ;

    for (unsigned int i = 0 ; i < num_columns ; i++)
      {
	justify[i]       = _DtCvOPTION_BAD;
	col_widths[i]    = NULL;
	justify_chars[i] = 0;

	char buffer[16] ;
	ColFormat *format = f_colformats.at(i);

	if (format == NULL) continue;

	justify[i] = format->justify();

	snprintf(buffer, sizeof(buffer), "%d", format->width());
	col_widths[i] = strdup(buffer);

	if (format->justify() == _DtCvJUSTIFY_CHAR)
	  {
	    char buf[2] ;
	    buf[0] = format->char_align() ;
	    buf[1] = 0 ;
	    slen = strlen(buf);
	    len = MIN(strlen(buf), num_columns - slen);
	    *((char *) memcpy(justify_chars + slen, buf, len) + len) = '\0';
	  }
      }
    table->col_justify = justify ;
#ifdef COLSPEC_DEBUG
    for (i = 0; i < num_columns; i++)
	fprintf(stderr, "col%d=%d\n", i, table->col_justify[i]);
#endif
    table->col_w = col_widths ;
    table->justify_chars = justify_chars ;
  }

#ifdef TABLE_DEBUG
  {
    cerr << "widths: " << endl;
    for (int i = 0; i < num_columns; i++)
      cerr << table->col_w[i] << endl;
  }
#endif

#ifdef TABLE_DEBUG_X
  {
    int i = 0 ;
    while (table->cell_ids[i])
      cerr << table->cell_ids[i++] << endl;

  }
#endif
}

#ifdef HIERARCHY_DEBUG

static char* seg_type_to_string(unsigned long seg_type)
{
    char* ret = NULL;
    switch (seg_type) {
	case _DtCvCONTAINER:
	    ret = "<C>";
	    break;
	case _DtCvSTRING:
	    ret = "<S>";
	    break;
	case _DtCvREGION:
	    ret = "<R>";
	    break;
	case _DtCvMARKER:
	    ret = "<M>";
	    break;
	case _DtCvLINE:
	    ret = "<L>";
	    break;
	case _DtCvNOOP:
	    ret = "<N>";
	    break;
	case _DtCvTABLE:
	    ret = "<T>";
	    break;
	default:
	    ret = "<?>";
    }

    return ret;
}

static void print_hierarchy_traversal(_DtCvSegment* seg, int depth, ostream& ostr)
{
    if (seg == NULL)
	return;

    unsigned long seg_ptype = seg->type & _DtCvPRIMARY_MASK;

    int i;
    for (i = 0; i < depth; ostr << ' ', i++);

    if (seg_ptype != _DtCvCONTAINER)
	ostr << seg_type_to_string(seg_ptype) << '\n';
    else
	ostr << seg_type_to_string(seg_ptype);

    if (seg_ptype == _DtCvCONTAINER) {
	if (seg->handle.container.border != _DtCvBORDER_NONE)
	    ostr << " border";
	ostr << " leading=" << seg->handle.container.leading;
	ostr << '\n';
	
	_DtCvSegment* subordinates = seg->handle.container.seg_list;
	if (subordinates)
	    print_hierarchy_traversal(subordinates, depth+1, ostr);
    }
    if (seg_ptype == _DtCvTABLE) {
	_DtCvTable *table = &seg->handle.table;
	_DtCvSegment** cells = table->cells;
	while (*cells)
	    print_hierarchy_traversal(*cells++, depth+1, ostr);
    }
    else if (seg_ptype == _DtCvSTRING) {
	for (i = 0; i < depth+1; ostr << ' ', i++);
	ostr << (char*)seg->handle.string.string << '\n';
    }

    // traverse siblings
    _DtCvSegment* siblings;
    if (siblings = seg->next_seg)
	print_hierarchy_traversal(siblings, depth, ostr);
}

static void print_hierarchy(_DtCvTopicInfo* topic)
{
    _DtCvSegment* top = topic->seg_list;

    if (top) {
	int depth = 0;
	ofstream ostr("SegmentStructure");
	print_hierarchy_traversal(top, depth, ostr);
	ostr.close();
    }
}

#endif

// colcell can be NULL where cell-related values are not taken
// into account.
void
TGDefn::setup_cell(_DtCvSegment* segment, ColDefn* colcell,
		   RowDefn* row, ColFormat* msformat, ColFormat* lsformat)
{
  _DtCvSegment* parseg;

  // segment is optional, but if it's specified it'll be used
  // instead of colcell->parseg().
  if (segment)
    parseg = segment;
  else
    parseg = colcell->parseg();

  if (colcell && colcell->justify() != _DtCvOPTION_BAD)
  {
    parseg->handle.container.justify = colcell->justify();

    if (colcell->justify() == _DtCvJUSTIFY_CHAR)
    {
      char charalign = '\0';

      if (colcell->char_align()) {
	charalign = colcell->char_align();
      }
      else if (msformat && msformat->char_align()) {
	charalign = msformat->char_align();
      }
      else { // always available here thanks to dtd
	charalign = char_align();
      }

      char *align = new char[2];
      align[0] = charalign;
      align[1] = '\0';
	      
      // need to deallocate container.justify_char later
      parseg->handle.container.justify_char = align;
    }
  }
  else if (msformat && msformat->justify() != _DtCvOPTION_BAD)
  {
    parseg->handle.container.justify = _DtCvINHERIT;
  }
  else if (justify() != _DtCvOPTION_BAD)
  {
    if (justify() == _DtCvJUSTIFY_CHAR)
    {
      char charalign = '\0';

      if (colcell && colcell->char_align()) {
	charalign = colcell->char_align();
      }
      else if (msformat && msformat->char_align()) {
	charalign = msformat->char_align();
      }
      else { // always available here due to dtd
	charalign = char_align();
      }

      char *align = new char[2];
      align[0] = charalign;
      align[1] = '\0';
	      
      // need to deallocate container.justify_char later
      parseg->handle.container.justify_char = align;
    }

    parseg->handle.container.justify = justify();
  }

  if (colcell && colcell->vjustify() != _DtCvOPTION_BAD) {
    parseg->handle.container.vjustify = colcell->vjustify();
  }
  else if (row->vjustify() != _DtCvOPTION_BAD) {
    parseg->handle.container.vjustify = row->vjustify();
  }
  else if (vjustify() != _DtCvOPTION_BAD) {
    parseg->handle.container.vjustify = vjustify();
  }

  int col_sep = CRSEP_NOT_SPECIFIED;
  // colsep does not apply to the last column
  if (lsformat != f_colformats.last())
  {
    // Precedence: col->colformat->tgroup->table
    if (colcell && colcell->colsep() != CRSEP_NOT_SPECIFIED) {
      col_sep = colcell->colsep();
    }
    else if (msformat && msformat->colsep() != CRSEP_NOT_SPECIFIED) {
      col_sep = msformat->colsep();
    }
    else if (colsep() != CRSEP_NOT_SPECIFIED) {
      col_sep = colsep();
    }
    else if (table() && table()->colsep() != CRSEP_NOT_SPECIFIED) {
      col_sep = table()->colsep();
    }
  }

  int row_sep = CRSEP_NOT_SPECIFIED;
  // rowsep does not apply to the last row
  if (f_rows.last() != row) {
    // Precedence: col->row->colformat->tgroup->table
    if (colcell && colcell->rowsep() != CRSEP_NOT_SPECIFIED) {
      row_sep = colcell->rowsep();
    }
    else if (row->rowsep() != CRSEP_NOT_SPECIFIED) {
      row_sep = row->rowsep();
    }
    else if (msformat && msformat->rowsep() != CRSEP_NOT_SPECIFIED) {
      row_sep = msformat->rowsep();
    }
    else if (rowsep() != CRSEP_NOT_SPECIFIED) {
      row_sep = rowsep();
    }
    else if (table() && table()->rowsep() != CRSEP_NOT_SPECIFIED) {
      row_sep = table()->rowsep();
    }
  }

  if (col_sep == CRSEP_YES && row_sep == CRSEP_YES)
    parseg->handle.container.border = _DtCvBORDER_BOTTOM_RIGHT;
  else if (col_sep == CRSEP_YES)
    parseg->handle.container.border = _DtCvBORDER_RIGHT;
  else if (row_sep == CRSEP_YES)
    parseg->handle.container.border = _DtCvBORDER_BOTTOM;
  else {
    // respect border feature of cell, do not overwrite here.
  }
}

