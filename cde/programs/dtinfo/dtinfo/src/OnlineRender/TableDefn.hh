// $XConsortium: TableDefn.hh /main/13 1996/11/06 16:52:17 cde-hal $

#define CRSEP_NOT_SPECIFIED	-1
#define CRSEP_NO		0
#define CRSEP_YES		1

class TableDefn;

class ColFormat
{
public:
  ColFormat();
  ~ColFormat();

  char		char_align ()		{ return f_char_align  ; }
  char		char_align (const char *s)
    { f_char_align = s[0] ; return f_char_align ; }

  const	char   *name ()			{ return f_name ; }
  const char   *name (const char *);

  _DtCvFrmtOption justify ()		
    { return f_justify  ; }
  _DtCvFrmtOption justify (_DtCvFrmtOption c)	
    { f_justify = c ; return c ; }

  unsigned	width ()		{ return f_width  ; }
  unsigned	width (int w)		{ f_width = w ; return w ; }

  int		colsep()		{ return f_colsep; }
  void		colsep(int v)		{ f_colsep = v; }
  int		rowsep()		{ return f_rowsep; }
  void		rowsep(int v)		{ f_rowsep = v; }

  bool operator==(const ColFormat &c) const
    { return &c == this ; }

private:
  char			f_char_align ; // alignment character
  char			*f_name ;
  _DtCvFrmtOption	f_justify ;    // 
  unsigned		f_width ;      // percentage of total width
  int			f_colsep;
  int			f_rowsep;
};

class ColDefn
{
public:
  ColDefn(_DtCvSegment *par, _DtCvSegment *gpar,
	  unsigned rows, char *colref, char* colstart, char* colend,
	  _DtCvFrmtOption justify, _DtCvFrmtOption vjustify,
	  int colsep, int rowsep, char char_align)
    : f_parseg (par),
  f_gparseg(gpar),
  f_spanrows (rows),
  f_colref (colref),
  f_colstart (colstart),
  f_colend (colend),
  f_justify (justify),
  f_vjustify (vjustify),
  f_colsep(colsep),
  f_rowsep(rowsep),
  f_char_align(char_align)
      {}

  ~ColDefn () {
	if (f_colref)	delete f_colref;
	if (f_colstart)	delete f_colstart;
	if (f_colend)	delete f_colend;
  }

  _DtCvSegment  *parseg()  const { return f_parseg; }
  _DtCvSegment  *gparseg() const { return f_gparseg; }

  unsigned spanrows () const { return f_spanrows ; } 

  const char *colref(char *r) { f_colref = r ; return r ; }
  const char *colref()        { return f_colref ; }

  void	      colstart(char *s) { f_colstart = s; }
  const char* colstart() const  { return f_colstart; }

  void	      colend(char *e) { f_colend = e; }
  const char* colend()        { return f_colend; }

  bool operator==(const ColDefn &c) const
    { return &c == this ; }

  _DtCvFrmtOption justify() { return f_justify; }
  _DtCvFrmtOption justify(_DtCvFrmtOption c) { f_justify = c ; return c ; }

  _DtCvFrmtOption vjustify() { return f_vjustify; }
  _DtCvFrmtOption vjustify(_DtCvFrmtOption c) { f_vjustify = c ; return c ; }

  int	colsep()	{ return f_colsep; }
  void	colsep(int v)	{ f_colsep = v; }
  int	rowsep()	{ return f_rowsep; }
  void	rowsep(int v)	{ f_rowsep = v; }

  void char_align(char c) { f_char_align = c; }
  char char_align() { return f_char_align; }

private:
  _DtCvSegment	*f_parseg ;
  _DtCvSegment	*f_gparseg ;
  unsigned	 f_spanrows ;
  char		*f_colref ;	// column format name
  char		*f_colstart;
  char		*f_colend;
  _DtCvFrmtOption f_justify;
  _DtCvFrmtOption f_vjustify;
  int		f_colsep;
  int		f_rowsep;
  char		f_char_align;
};


class RowDefn
{
public:
  RowDefn() ;
  ~RowDefn() ;

  void append(ColDefn *cd) { f_columns.append (cd); } 

  CC_TPtrSlist<ColDefn> &columns() { return f_columns ; }

  bool operator==(const RowDefn &r) const
    { return &r == this ; }

  _DtCvFrmtOption vjustify ()		
    { return f_vjustify  ; }
  _DtCvFrmtOption vjustify (_DtCvFrmtOption c)	
    { f_vjustify = c ; return c ; }

  int	rowsep()	{ return f_rowsep; }
  void	rowsep(int v)	{ f_rowsep = v; }

private:
  CC_TPtrSlist<ColDefn> f_columns ;

  _DtCvFrmtOption	f_vjustify ;
  int			f_rowsep;
} ;

class TGDefn
{
public:
  TGDefn (_DtCvFrmtOption justify, _DtCvFrmtOption vjustify);
  ~TGDefn();
  
  void  set_segment (_DtCvSegment *seg) { f_segment = seg ; } 

  void  add_row(RowDefn *);
  void	add (ColFormat *);
  void  add (ColDefn *);
  
  unsigned	numcols ()	{ return f_numcols ; }
  unsigned	numcols (int n)	{ f_numcols = n ; return n ; }

  void build();		// fill in the Canvas Segment data

  void setup_cell(_DtCvSegment*, ColDefn*, RowDefn*, ColFormat*, ColFormat*);

  bool operator==(const TGDefn& t) const
    { return &t == this ; } 

  _DtCvFrmtOption justify() { return f_justify; }
  _DtCvFrmtOption justify(_DtCvFrmtOption c) { f_justify = c ; return c ; }

  _DtCvFrmtOption vjustify() { return f_vjustify  ; }
  _DtCvFrmtOption vjustify(_DtCvFrmtOption c) { f_vjustify = c ; return c ; }

  int	colsep()	{ return f_colsep; }
  void	colsep(int v)	{ f_colsep = v; }
  int	rowsep()	{ return f_rowsep; }
  void	rowsep(int v)	{ f_rowsep = v; }

  TableDefn* table() { return f_table; }
  void table(TableDefn* t) { f_table = t; }

  void char_align(const char* s) { f_char_align = *s; }
  char char_align() const { return f_char_align; }

private:
  ColFormat *find_format (const char *name, int* index = NULL) ;


private:
  unsigned			f_numcols ;
  _DtCvSegment		       *f_segment ;
  CC_TPtrSlist<ColFormat>	f_colformats ;
  CC_TPtrSlist<RowDefn>		f_rows ;
  _DtCvFrmtOption		f_justify;
  _DtCvFrmtOption		f_vjustify;
  int				f_colsep;
  int				f_rowsep;
  char				f_char_align;
  TableDefn*			f_table; // table which contains this tgroup
};

class TableDefn
{
public:
  typedef enum {
    table_frame_default = -1,
    table_frame_none    = 0,
    table_frame_top     = 1 << 0,
    table_frame_bottom  = 1 << 1,
    table_frame_left    = 1 << 2,
    table_frame_right   = 1 << 3,
    table_frame_topbot  = table_frame_top | table_frame_bottom,
    table_frame_sides   = table_frame_left | table_frame_right,
    table_frame_all     = table_frame_topbot | table_frame_sides
  } table_frame_t;

  TableDefn(table_frame_t frame_tok, int colsep = CRSEP_NOT_SPECIFIED,
				     int rowsep = CRSEP_NOT_SPECIFIED) :
    f_frame(frame_tok), f_colsep(colsep), f_rowsep(rowsep) {}
  TableDefn(const char* frame_str, int colsep = CRSEP_NOT_SPECIFIED,
				   int rowsep = CRSEP_NOT_SPECIFIED);
  ~TableDefn();

  table_frame_t frame() { return f_frame; }
  void frame(table_frame_t frame_tok) { f_frame = frame_tok; }
  void frame(const char* frame_str);

  int	colsep()	{ return f_colsep; }
  void	colsep(int v)	{ f_colsep = v; }
  int	rowsep()	{ return f_rowsep; }
  void	rowsep(int v)	{ f_rowsep = v; }

protected:

  table_frame_t string_to_token(const char* frame_str);

private:

  table_frame_t f_frame;
  int		f_colsep;
  int		f_rowsep;

};
