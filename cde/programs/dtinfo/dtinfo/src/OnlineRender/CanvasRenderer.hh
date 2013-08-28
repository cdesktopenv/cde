// $XConsortium: CanvasRenderer.hh /main/31 1996/11/26 15:29:35 rcs $
#ifndef _CanvasRenderer_hh
#define _CanvasRenderer_hh

#include "Renderer.h"
#include "FontCache.hh"

// get DtCanvas structure defns
#include <Dt/CanvasSegP.h>

#include "dti_cc/CC_Stack.h"

// Symbol Constants required
#define GRAPHIC_attr	0
#define INLGRAPHIC	1
#define ISMAP		2
#define OLID		3
#define OLIDREF		4
#define REMOTE		5
#define TABLE		6
#define TERMS		7

#define ALIGN		8 
#define BOTTOM		9 
#define LINEBREAK	10
#define CHARSETS	11
#define SET		12
#define BGCOLOR		13
#define FGCOLOR		14
#define FONT		15
#define FONTCATALOG	16
#define FONTFAMILY	17
#define HIGHLIGHT	18
#define IGNORE		19
#define LEFT		20
#define MARGIN		21
#define SUFFIX		22
#define PREFIX		23
#define PREVIEW		24
#define RIGHT		25
#define TABSTOPS	26
#define CONTENT		27
#define TOP		28
#define OVERLINE	29
#define STRIKETHROUGH	30
#define UNDERLINE	31
#define WRAP		32
#define XLFD		33
#define GRAPHIC_feature 34
#define FAMILY		35
#define WEIGHT		36
#define SLANT		37
#define SIZE		38
#define CHARSET		39
#define REVERSEVIDEO	40
#define AT		41
#define EVERY		42
/* ---- */
#define POSITION	43
#define HORIZ		44
#define VERT		45
/* ---- */
#define LAYOUT		46
#define ASPACE		47
#define BSPACE		48
#define LEADING		49
#define FINDENT		50
#define LINDENT		51
#define RINDENT		52
#define FLOW		53
#define JUSTIFY		54
#define VJUSTIFY	55
#define BORDER		56
#define THICKNESS	57
#define ROW		58
#define COLS		59
#define COLFORMAT	60
#define CHARALIGN	61
#define SPANCOLS	62
#define MOREROWS	63
#define CELL		64
#define WIDTH		65
#define FALLBACK	66
#define FOUNDRY		67
#define NAME		68
#define DISPLAY		69
#define COLREF		70
#define SUBSUPER	71
#define COLSEP		72
#define ROWSEP		73
#define TGROUP		74
#define FRAME		75

// print symbols

#define MEDIA           76
#define PAGEBREAK       77
#define FOOTERS         78
#define HEADERS         79
#define ORIENTATION     80

#define REND_SYMBOLS	81

//-------- External References ---------------------------------

extern const char *f_printOrientation;

int point2pixel(int size);

class FeatureSet ;
class Symbol ;

#include "OnlineRender/SegClientData.hh"
#include "OnlineRender/TableDefn.hh"

class Margins
{
  
public:

  Margins();

  int	has_margins();

  int	first()		{ return f_first ; }
  int	first (int f)	{ f_first = f ; return f ; }
  int	left()		{ return f_left ; }
  int	left (int l)	{ f_left = l ; return l ; }
  int	right()		{ return f_right ; }
  int	right (int r)	{ f_right = r ; return r ; }
  int	top()		{ return f_top ; }
  int	top (int t)	{ f_top = t ; return t ; }
  int	bottom()	{ return f_bottom ; }
  int	bottom (int b)	{ f_bottom = b ; return b ; }

  
private:
  int	f_first ;
  int	f_left  ;
  int	f_right ;
  int	f_top ;
  int	f_bottom ;
};

class Posn
{
public:
  Posn () : f_horiz (_DtCvOPTION_BAD), f_vert (_DtCvOPTION_BAD) {}
  Posn (Posn &posn) : f_horiz (posn.f_horiz), f_vert (posn.f_vert)
    {}


  unsigned has_position()	{ return f_horiz || f_vert ; }

  _DtCvFrmtOption horiz ()			{ return f_horiz ; }
  _DtCvFrmtOption horiz (_DtCvFrmtOption h)	{ f_horiz = h ; return h ; }
  _DtCvFrmtOption vert ()			{ return f_vert ; }
  _DtCvFrmtOption vert (_DtCvFrmtOption v)	{ f_vert = v ; return v ; } 

private:

  _dtCvFrmtOption	f_horiz ;
  _dtCvFrmtOption	f_vert ;

};

class CanvasRenderer;

class Layout
{
public:
  Layout (CanvasRenderer *);
  Layout (Layout &);

  unsigned has_layout(int affix);

  int aspace ()		{ return f_aspace ; }
  int aspace (int a)	{ f_aspace = a ; return a ; }

  int bspace ()		{ return f_bspace ; }
  int bspace (int a)	{ f_bspace = a ; return a ; }

  int leading ()	{ return f_leading ; }
  int leading (int a)	{ f_leading = a ; return a ; }

  int findent ()	{ return f_findent ; }
  int findent (int a)	{ f_findent = a ; return a ; }

  int rindent ()	{ return f_rindent ; }
  int rindent (int a)	{ f_rindent = a ; return a ; }

  int lindent ()	{ return f_lindent ; }
  int lindent (int a)	{ f_lindent = a ; return a ; }

  _DtCvFrmtOption	flow ()		{ return f_flow ; }
  _DtCvFrmtOption	flow (_DtCvFrmtOption f)	{ f_flow = f; return f ; }

  _DtCvFrmtOption	wrap ()		{ return f_wrap ; }
  _DtCvFrmtOption	wrap (_DtCvFrmtOption f)	{ f_wrap = f; return f ; }

  _DtCvFrmtOption	justify ()		{ return f_justify ; }
  _DtCvFrmtOption	justify (_DtCvFrmtOption f)	{ f_justify = f; return f ; }

private:
  CanvasRenderer* f_renderer;
  int	f_aspace ;
  int	f_bspace ;
  int	f_leading ;
  int	f_findent ;
  int	f_rindent ;
  int	f_lindent ;

  _DtCvFrmtOption	f_flow ;
  _DtCvFrmtOption	f_justify ;
  _DtCvFrmtOption	f_wrap ;
  
};

class Cell
{
public:
  Cell() : 
  f_has_cell (0), 
  f_spanrows (1),
  f_colref (NULL),
  f_colstart (NULL),
  f_colend (NULL),
  f_justify(_DtCvOPTION_BAD),
  f_vjustify(_DtCvOPTION_BAD),
  f_colsep(CRSEP_NOT_SPECIFIED),
  f_rowsep(CRSEP_NOT_SPECIFIED),
  f_char_align('\0')
    {}

  int	has_cell()	{ return f_has_cell ; }
  int	has_cell (int tf){ f_has_cell = tf ; return tf ; }

  int	spanrows () { return f_spanrows ; }
  int	spanrows (int rows) { f_spanrows = rows ; return rows ; }

  char *colref(char *r)	  { f_colref = r; return r; }
  char *colref()	  { return f_colref; }
  void  colstart(char* s) { f_colstart = s; }
  char* colstart()	  { return f_colstart; }
  void  colend(char* s)	  { f_colend = s; }
  char* colend()	  { return f_colend; }

  _DtCvFrmtOption justify() { return f_justify; }
  _DtCvFrmtOption justify(_DtCvFrmtOption c) { f_justify = c ; return c ; }

  _DtCvFrmtOption vjustify() { return f_vjustify; }
  _DtCvFrmtOption vjustify(_DtCvFrmtOption c) { f_vjustify = c ; return c ; }

  int	colsep() { return f_colsep; }
  void	colsep(int v) { f_colsep = v; }
  int	rowsep() { return f_rowsep; }
  void	rowsep(int v) { f_rowsep = v; }

  char char_align(const char *s) {
    f_char_align = *s; return f_char_align;
  }
  char char_align() { return f_char_align; }

private:
  int   f_has_cell ;
  int   f_spanrows ;
  char *f_colref ;
  char *f_colstart ;
  char *f_colend ;
  _DtCvFrmtOption f_justify;
  _DtCvFrmtOption f_vjustify;
  int	f_colsep;
  int	f_rowsep;
  char  f_char_align;
};

class Highlight
{
public:
  Highlight() : f_hilite(0) {}
  Highlight(Highlight& src) : f_bg_color(src.f_bg_color),
			      f_fg_color(src.f_fg_color),
			      f_hilite(src.f_hilite) {}

  int overline() { return (f_hilite & hilite_overline); }
  int strikethrough() { return (f_hilite & hilite_strikethrough); }
  int underline() { return (f_hilite & hilite_underline); }

  UAS_String& bg_color() { return f_bg_color; }
  UAS_String& fg_color() { return f_fg_color; }

  void overline(unsigned int);
  void strikethrough(unsigned int);
  void underline(unsigned int);

  void bg_color(const char* col) { f_bg_color = col; }
  void bg_color(UAS_String& col) { f_bg_color = col; }
  void fg_color(const char* col) { f_fg_color = col; }
  void fg_color(UAS_String& col) { f_fg_color = col; }

private:
  UAS_String f_bg_color;
  UAS_String f_fg_color;
  unsigned int f_hilite;
};

inline void
Highlight::overline(unsigned int v)
{
    if (v)
	f_hilite |= hilite_overline;
    else
	f_hilite &= ~hilite_overline;
}

inline void
Highlight::strikethrough(unsigned int v)
{
    if (v)
	f_hilite |= hilite_strikethrough;
    else
	f_hilite &= ~hilite_strikethrough;
}

inline void
Highlight::underline(unsigned int v)
{
    if (v)
	f_hilite |= hilite_underline;
    else
	f_hilite &= ~hilite_underline;
}

#define LINEBREAK_NONE   0
#define LINEBREAK_BEFORE 1
#define LINEBREAK_AFTER  2
#define LINEBREAK_BOTH   (LINEBREAK_BEFORE | LINEBREAK_AFTER)

#define PAGEBREAK_NONE   0
#define PAGEBREAK_BEFORE 1
#define PAGEBREAK_AFTER  2
#define PAGEBREAK_BOTH   (PAGEBREAK_BEFORE | PAGEBREAK_AFTER)

class PartialElementFeatures
{
  // element features acceptible by prefix and suffix
public:
  PartialElementFeatures(CanvasRenderer *);
  PartialElementFeatures (PartialElementFeatures &);
  virtual ~PartialElementFeatures();

  virtual int requires_container(int affix);
  virtual int ignore_linebreak(int affix);

  UAS_String font () { return f_font ; }
  void set_font (const char *f) { f_font = f ; }  

  const unsigned pagebreak()		{ return f_pagebreak ; }
  const unsigned pagebreak (unsigned b)	{ f_pagebreak = b ; return b ; }

  const unsigned linebreak()		{ return f_linebreak ; }
  const unsigned linebreak (unsigned b)	{ f_linebreak = b ; return b ; }

  const char *text ()			{ return f_text ; }
  const char *text(const char *t)	{ f_text = t ; return t ; }

  Posn &position()	{ return f_position ; } 

  Layout &layout ()	{ return f_layout ; }

  _DtCvSegment *graphic (_DtCvSegment *g)	{ f_graphic = g ; return g ; }
  _DtCvSegment *graphic ()			{ return f_graphic ; } 

  Highlight& highlight() { return f_highlight; }

  enum { baseline, subscript, superscript } ;
  void subsuper(int v)	{ f_subsuper = v; }
  int  subsuper()	{ return f_subsuper; }

  void ignore(int v)	{ f_ignore = v; }
  int  ignore()		{ return f_ignore; }

  const char *orientation() { return f_orientation ; }

  const char *orientation(const char * b) { 
      f_printOrientation = b; 
      f_orientation = b ; 
      return b ; 
  }

private:
  const char   *f_text ;
  UAS_String    f_font ;
  int		f_subsuper;
  Highlight	f_highlight;
  unsigned	f_linebreak;
  unsigned	f_pagebreak;
  Posn		f_position;
  Layout	f_layout ;
  _DtCvSegment *f_graphic ;
  int		f_ignore;
  const char   *f_orientation;
};

class ElementFeatures : public PartialElementFeatures
{
  // all the features required to format an element
public:
  ElementFeatures(CanvasRenderer *);
  ~ElementFeatures();

  int requires_container(int affix);
  int ignore_linebreak(int affix);

  PartialElementFeatures &prefix()
    { return f_prefix ; }
  PartialElementFeatures &suffix()
    { return f_suffix ; }

  Margins	&margin()	{ return f_margin ; }

  _DtCvFrmtOption	border()  { return f_border ; }
  _DtCvFrmtOption	border(_DtCvFrmtOption b) { f_border = b ; return b ; }
  int		border_width() { return f_border_width; }
  void		border_width(int w) { f_border_width = w; }

  TableDefn	*table()	{ return f_table; }
  TableDefn	*table(TableDefn *t) { f_table = t; return f_table; }

  TGDefn	*tgroup()	{ return f_tgroup ; }
  TGDefn	*tgroup(TGDefn *t) { f_tgroup = t ; return f_tgroup ; }

  ColFormat	*col_format()	{ return f_colformat ; }
  ColFormat	*col_format (ColFormat *f) { f_colformat = f ; return f ; }

  Cell		&cell()		{ return f_cell ; }

  RowDefn*	row()		{ return f_row ; }
  RowDefn*	row(RowDefn* d)	{ f_row = d ; return d ; }

  int		link_idx ()	{ return f_link_idx ; }
  int		link_idx (int li){ f_link_idx = li; return li ; }

  char		*locator ()	{ return f_locator ; }
  char		*locator(char *l){ f_locator = l ; return l ; }

  char		*xref ()	{ return f_xref ; }
  char		*xref(char *l){ f_xref = l ; return l ; }

private:
  RowDefn*	f_row ;
  unsigned	f_graphic;

  _DtCvFrmtOption	f_border;
  int			f_border_width;

  TableDefn	*f_table;
  TGDefn	*f_tgroup ;
  ColFormat	*f_colformat ;
  Cell		f_cell ;
  Margins	f_margin ;
  PartialElementFeatures	f_prefix ;
  PartialElementFeatures	f_suffix ;

  /* --- olias attributes ---*/
  char	       *f_locator ; // if this element is identified with a locator
  char	       *f_xref ;
  int	        f_link_idx ; // index of where this guy links to
};

class CanvasRenderer : public Renderer
{
public:
  CanvasRenderer(int font_scale);
  ~CanvasRenderer();
  
  // inherited virtuals to be overridden
  FeatureSet   *initialize();

  void		Begin();
  void		End();

  unsigned int	BeginElement(const Element     &element,
			     const FeatureSet  &local,
			     const FeatureSet  &complete,
			     const FeatureSet  &parentComplete);

  void		data(const char *data, unsigned int size);
  void		_data(const char *data, unsigned int size, Boolean count_vcc);

  void		EndElement(const Symbol &name);

  friend class Layout; // let Layout to look into leading stacks and level
  int current_leading();

  // local stuff 

private:
  void	handle_olias_attributes(ElementFeatures &,
				const Element &,
				const FeatureSet &local,
				const FeatureSet &complete,
				const FeatureSet &parentComplete);

  void  setup_container(_DtCvSegment *container,
			PartialElementFeatures &features,
			int affix);
  void	setup_container(_DtCvSegment *container,
			ElementFeatures &features);

  const char *get_pattern(const char *fallback, const char *weight, const char *slant, int size);
  void	dofont(PartialElementFeatures	&return_features, const FeatureSet &, Symbol **);
  void	dofontlist(ElementFeatures	&return_features,const FeatureSet &, Symbol **);
  void	dounderline(ElementFeatures	&return_features,const FeatureSet &, Symbol **);
  void	domargin(ElementFeatures	&return_features, const FeatureSet &, Symbol **);
  void	dowrap(ElementFeatures	&return_features, const FeatureSet &, Symbol **);
  void	dohighlight(ElementFeatures	&return_features,const FeatureSet &, Symbol **);
  void	docolor(ElementFeatures	&return_features, const FeatureSet &, Symbol **);
  void	doposition(PartialElementFeatures &return_features, const FeatureSet &, Symbol**);
  void	doborder(ElementFeatures &return_features, const FeatureSet &, Symbol**);
  void	dolayout(PartialElementFeatures &return_features, const FeatureSet &local,
		 const FeatureSet &complete, Symbol**);
  void  dotable(ElementFeatures &return_features, const FeatureSet &, Symbol**);
  void  dotgroup(ElementFeatures &return_features, const FeatureSet &, Symbol**);
  void  docolformat(ElementFeatures &return_features, const FeatureSet &, Symbol**);
  void  docell(ElementFeatures &return_features, const FeatureSet &, Symbol**);
  void  dorow(ElementFeatures &return_features, const FeatureSet &, Symbol**);

    // print features

#ifdef NOTIMPLEMENTED
  void  dofooter(ElementFeatures &return_features, const FeatureSet &, Symbol**);
  void  doheader(ElementFeatures &return_features, const FeatureSet &, Symbol**);
#endif

  void  domedia(ElementFeatures &return_features, const FeatureSet &, Symbol**);
  void	dopage(PartialElementFeatures	&return_features, const FeatureSet
		&local, const FeatureSet &complete, Symbol **); 

  void	do_unsupported_feature(const Symbol &);

  void	doprefix(ElementFeatures	&return_features, const
		 FeatureSet &local, const FeatureSet &complete, Symbol**);  
  void	dosuffix(ElementFeatures	&return_features,const
		 FeatureSet &local, const FeatureSet &complete,
		 Symbol**);   
  void	dobreak(PartialElementFeatures	&return_features, const FeatureSet
		&local, const FeatureSet &complete, Symbol **); 

  void do_features(ElementFeatures	&return_features,
		   const FeatureSet	&local,
		   const FeatureSet 	&complete,
		   Symbol 	       **symbols);

  _DtCvSegment *create_container(_DtCvSegment *parent);

  _DtCvSegment *insert_string (_DtCvSegment *container, const char *font, 
			       const char *data, unsigned int size,
			       Boolean count_vcc = True);
  _DtCvSegment *really_insert_string (_DtCvSegment *container, const char *font, 
			       const char *data, unsigned int size,
			       Boolean count_vcc = True);
  _DtCvSegment *insert_literal(_DtCvSegment *container, const char *font,
			       const char *data, unsigned int size);

  void          setup_cascaded_container (_DtCvSegment*);
  void          setup_cascaded_tgroup_container (_DtCvSegment*, _DtCvSegment*);
  _DtCvSegment* create_cascaded_container(_DtCvSegment*);

private:
  static const char* const f_hcf_sans;
  static const char* const f_hcf_serif;
  static const char* const f_hcf_mono;
  static const char* const f_hcf_symbol;

  static const char* const f_hcf_weight;
  static const char* const f_hcf_slant;
  static const char* const f_hcf_fallback;
  static const int	   f_hcf_size;

  char *_dofont(const FeatureSet &, Symbol **);

  // Virtual Character Count
  unsigned int	f_vcc ;

  _DtCvSegment *f_current_container ;
  _DtCvSegment *f_current_displayable ; // for next_disp links

  TGDefn    *f_current_tgroup ; // for building tables

  UAS_String    f_font ;		// current font
  int		f_link_idx ;		// current link index

  int		f_font_scale ;

  // I have implemented multiple stacks, instead of one stack with a
  // class or structure, because, with the exception of the suffixes
  // stack, every other stack contains single word entries, hence no
  // excessive dynamic memory allocation, and no worries about
  // releasing the memory at the correct time. This may or may not be
  // optimal, but was how the incremental development produced
  // it. This should be simple to change. - brad

  Stack<PartialElementFeatures *> f_suffixes ; // keep track of
					       // suffixes to be applied
  Stack<UAS_String>   f_fstack ;	       // font stack
  Stack<_DtCvSegment *> f_stack ;	       // current container
  Stack<int>		f_link_stack ;	       // current link
  Stack<TableDefn *>	f_table_stack ;        // tables
  Stack<TGDefn *>	f_tgroup_stack ;       // tgroups
  Stack<SegClientData*> f_scd_stack;	// highlight info stack
  Stack<int>		f_subsuper_stack;      // sub/super script
  Stack<int>		f_ignore_stack;	       // ignore state
  Stack<int>		f_leading_stack;       // leading values
  Stack<int>		f_leading_level_stack; // leading levels

  // default fonts 
  const char   *f_sans;
  const char   *f_serif;
  const char   *f_mono;
  const char   *f_symbol;

  // do not destroy ... owned by resolver
  FeatureSet   *f_default_features ;

  // feature symbols
  Symbol       *f_symbols[REND_SYMBOLS] ;

  // element symbols 
  Symbol	fBogusSymbol;

  // local Feature symbol table 
  SymbolTable	f_symtab ;

  // FontCache 
  FontCache	f_fontcache ;

  // default border width
  int	f_border_width;

  int f_level;
};





#endif /* _CanvasRenderer_hh */
/* DO NOT ADD ANY LINES AFTER THIS #endif */


