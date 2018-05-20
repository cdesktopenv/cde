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
/* $XConsortium: fproto.h /main/3 1995/11/08 11:16:08 rswiston $ */
/* Copyright (c) 1988, 1989, 1990 Hewlett-Packard Co. */
/* Function prototypes for HP Tag/TeX translator */

void appstart(
#if defined(M_PROTO)
  M_WCHAR *id, M_WCHAR *letter 
#endif
  ) ;

void assert_hometopic_exists(
#if defined(M_PROTO)
  M_NOPAR
#endif
  ) ;

void badgraphic(
#if defined(M_PROTO)
  int filestat
#endif
  ) ;

void basename(
#if defined(M_PROTO)
  M_NOPAR
#endif
  ) ;

void calcdisp (
#if defined(M_PROTO)
  M_WCHAR *file, M_WCHAR *startrow, M_WCHAR *endrow,
  M_WCHAR *clip, M_WCHAR *margin
#endif
  ) ;

void callndbeg (
#if defined(M_PROTO)
  M_NOPAR
#endif
  ) ;

void callndend (
#if defined(M_PROTO)
  M_NOPAR
#endif
  ) ;

/* used by ccompspec(), cmenlspec(), and cwmenlspec() */
char *ccharspec(
#if defined(M_PROTO)
  M_WCHAR textchar
#endif
  ) ;

void ccmpiooutchar (
#if defined(M_PROTO)
  M_WCHAR textchar
#endif
  ) ;

void ccompesoutchar (
#if defined(M_PROTO)
  M_WCHAR textchar, FILE *outfile
#endif
  ) ;

void ccompexoutchar (
#if defined(M_PROTO)
  M_WCHAR textchar, FILE *outfile
#endif
  ) ;

void ccompoutchar (
#if defined(M_PROTO)
  M_WCHAR textchar, FILE *outfile
#endif
  ) ;

/* special computer expansions for dot matrix(computer) in calculator style */
char *ccompspec(
#if defined(M_PROTO)
  M_WCHAR textchar
#endif
  ) ;

void chapstart(
#if defined(M_PROTO)
  M_WCHAR *id, M_WCHAR *number, char type[] 
#endif
  ) ;

M_WCHAR *checkdimen(
#if defined(M_PROTO)
  M_WCHAR *val, M_WCHAR *paramname, M_WCHAR *elt
#endif
  ) ;

M_WCHAR *mb_checkdimen(
#if defined(M_PROTO)
  M_WCHAR *val, char *paramname, char *elt
#endif
  ) ;

M_WCHAR *checkent(
#if defined(M_PROTO)
  M_WCHAR *entcontent
#endif
  ) ;

void checkgloss(
#if defined(M_PROTO)
  M_NOPAR
#endif
  ) ;

M_WCHAR *checkid(
#if defined(M_PROTO)
  M_WCHAR *id
#endif
  ) ;

void checkmsghead (
#if defined(M_PROTO)
  M_NOPAR
#endif
  ) ;

M_WCHAR *checkreal (
#if defined(M_PROTO)
  M_WCHAR *val, M_WCHAR *paramname, LOGICAL msgout, M_WCHAR *elt
#endif
  ) ;

LOGICAL chkplottype(
#if defined(M_PROTO)
  M_WCHAR *val, int *p1x, int *p1y, int *p2x, int *p2y, LOGICAL prnt 
#endif
  ) ;

void cmloutchar (
#if defined(M_PROTO)
  M_WCHAR textchar, FILE *outfile
#endif
  ) ;

/* special menu lable character expansions */
char *cmenlspec(
#if defined(M_PROTO)
  M_WCHAR textchar
#endif
  ) ;

void cspotend(
#if defined(M_PROTO)
  M_WCHAR *file, int graphics, M_WCHAR *xwidth, M_WCHAR *xheight
#endif
  ) ;

void cspotstart(
#if defined(M_PROTO)
  M_WCHAR *file, int graphics, M_WCHAR *xwidth, M_WCHAR *xheight
#endif
  ) ;

void csubtc (
#if defined(M_PROTO)
  M_WCHAR textchar, FILE *outfile, M_WCHAR *parent
#endif
  ) ;

void csuptc (
#if defined(M_PROTO)
  M_WCHAR textchar, FILE *outfile, M_WCHAR *parent
#endif
  ) ;

/* special white menu label character expansions */
char *cwmenlspec(
#if defined(M_PROTO)
  M_WCHAR textchar
#endif
  ) ;

void cwmloutchar(
#if defined(M_PROTO)
  M_WCHAR textchar, FILE *outfile
#endif
  ) ;

void defxref(
#if defined(M_PROTO)
  FILE *xfile, M_WCHAR *id, struct xref *xref
#endif
  ) ;

int do_esc_seq(
#if defined(M_PROTO)
  M_NOPAR
#endif
  ) ;

void dumpxref(
#if defined(M_PROTO)
  M_NOPAR
#endif
  ) ;

void echohead(
#if defined(M_PROTO)
  M_WCHAR *p
#endif
  ) ;

void mb_echohead(
#if defined(M_PROTO)
  char *p
#endif
  ) ;

void endhead(
#if defined(M_PROTO)
  M_NOPAR
#endif
  ) ;

void endhelpnode(
#if defined(M_PROTO)
  M_NOPAR
#endif
  ) ;

void esoutchar(
#if defined(M_PROTO)
  M_WCHAR textchar
#endif
  ) ;

void exoutchar(
#if defined(M_PROTO)
  M_WCHAR textchar
#endif
  ) ;

void exvexend(
#if defined(M_PROTO)
  M_WCHAR *textsize
#endif
  ) ;

void exvexstart(
#if defined(M_PROTO)
  M_WCHAR *type, M_WCHAR *textsize, char *listinfo
#endif
  ) ;

void figend (
#if defined(M_PROTO)
  M_WCHAR *id
#endif
  ) ;

int figext (
#if defined(M_PROTO)
  M_WCHAR *fname
#endif
  ) ;

int figlist (
#if defined(M_PROTO)
  char *fnin
#endif
  ) ;

void figraster(
#if defined(M_PROTO)
M_WCHAR *file,    M_WCHAR *magnify,
M_WCHAR *width,   M_WCHAR *depth,        M_WCHAR *hadjust,  M_WCHAR *vadjust,
M_WCHAR *mirror,  M_WCHAR *margin,       M_WCHAR *clip,
M_WCHAR *penwidth,M_WCHAR *plottype,     M_WCHAR *callout,  M_WCHAR *textsize
#endif
  ) ;

void figstart(
#if defined(M_PROTO)
M_WCHAR *number,  M_WCHAR *tonumber,  M_WCHAR *id,          M_WCHAR *file,
M_WCHAR *type,    M_WCHAR *oldtype,
M_WCHAR *xmagnify,M_WCHAR *xwidth,    M_WCHAR *xdepth,      M_WCHAR *xhadjust,
M_WCHAR *xvadjust,M_WCHAR *border,    M_WCHAR *video,       M_WCHAR *strip,
M_WCHAR *mirror,  M_WCHAR *margin,      M_WCHAR *clip,
M_WCHAR *penwidth,M_WCHAR *snap,      M_WCHAR *autoscale,   M_WCHAR *plottype,
M_WCHAR *callout, M_WCHAR *textsize
#endif
  ) ;

void figvector(
#if defined(M_PROTO)
M_WCHAR *file,     M_WCHAR *magnify,
M_WCHAR *width,    M_WCHAR *depth,        M_WCHAR *hadjust,  M_WCHAR *vadjust,
M_WCHAR *mirror,   M_WCHAR *margin,       M_WCHAR *clip,
M_WCHAR *penwidth, M_WCHAR *autoscale,    M_WCHAR *plottype, M_WCHAR *callout,
M_WCHAR *textsize
#endif
  ) ;

unsigned get2b(
#if defined(M_PROTO)
  FILE *fh
#endif
  ) ;

unsigned long get4b(
#if defined(M_PROTO)
  FILE *fh
#endif
  ) ;

unsigned long getPCLdims (
#if defined(M_PROTO)
  char *fn
#endif
  ) ;

int getqualified (
#if defined(M_PROTO)
  M_WCHAR *qualname, M_WCHAR *unqualname
#endif
  ) ;

int mb_getqualified (
#if defined(M_PROTO)
  char *qualname, char *unqualname
#endif
  ) ;

void handle_link_and_graphic(
#if defined(M_PROTO)
  M_WCHAR *parent, M_WCHAR *gentity, M_WCHAR *gposition, M_WCHAR *ghyperlink,
  M_WCHAR *glinktype, M_WCHAR *gdescription
#endif
  );			     

void keyesoutchar (
#if defined(M_PROTO)
  M_WCHAR textchar
#endif
  ) ;

void idstring(
#if defined(M_PROTO)
  M_WCHAR *string
#endif
  ) ;

void indexchar(
#if defined(M_PROTO)
  M_WCHAR textchar
#endif
  ) ;

void item(
#if defined(M_PROTO)
  M_WCHAR *id
#endif
  ) ;

char *m_itoa(
#if defined(M_PROTO)
  int n, char *s
#endif
  ) ;

void itoletter(
#if defined(M_PROTO)
  int n, char start, char *dest, int length
#endif
  ) ;

LOGICAL m_letter(
#if defined(M_PROTO)
  M_WCHAR c
#endif
  ) ;

void loadxref(
#if defined(M_PROTO)
  M_NOPAR
#endif
  ) ;

int m_lower(
#if defined(M_PROTO)
  int c
#endif
  ) ;

char *makecsname(
#if defined(M_PROTO)
  M_WCHAR *id
#endif
  ) ;

void manustart(
#if defined(M_PROTO)
  M_WCHAR *language, M_WCHAR *idxvol, M_WCHAR *status
#endif
  ) ;

void notimp(
#if defined(M_PROTO)
  char *eltname, char *option
#endif
  ) ;

M_WCHAR *okdimen (
#if defined(M_PROTO)
  M_WCHAR *val
#endif
  ) ;

void onlygraphic(
#if defined(M_PROTO)
  char    *param,    M_WCHAR *magnify, M_WCHAR *video,     M_WCHAR *mirror,
  M_WCHAR *margin,   M_WCHAR *clip,    M_WCHAR *penwidth,  M_WCHAR *autoscale,
  M_WCHAR *plottype, M_WCHAR *hadjust, M_WCHAR *vadjust
#endif
  ) ;

void open_new_helpfile(
#if defined(M_PROTO)   
   M_NOPAR
#endif
);
		       
void notallow(
#if defined(M_PROTO)
  char *param1, char *param2
#endif
  ) ;

void options(
#if defined(M_PROTO)
  LOGICAL filelenonly
#endif
  ) ;

void outcall(
#if defined(M_PROTO)
  M_WCHAR textchar, FILE *outfile
#endif
  ) ;

void outchar(
#if defined(M_PROTO)
  M_WCHAR textchar, FILE *outfile 
#endif
  ) ;

void outpi(
#if defined(M_PROTO)
  int enttype, M_WCHAR *pi, M_WCHAR *entname
#endif
  ) ;

void parspace(
#if defined(M_PROTO)
  M_NOPAR
#endif
  ) ;

LOGICAL pushndok(
#if defined(M_PROTO)
  LOGICAL val
#endif
  ) ;

LOGICAL popndok(
#if defined(M_PROTO)
  M_NOPAR
#endif
  ) ;

void prfigborder(
#if defined(M_PROTO)
  int val
#endif
  ) ;

void prfigclip(
#if defined(M_PROTO)
  M_WCHAR figmargin [NFIGSIDES] [M_NAMELEN+1]
#endif
  ) ;

void prfigheight(
#if defined(M_PROTO)
  char *val
#endif
  ) ;

void prfigmagnify(
#if defined(M_PROTO)
  char *val
#endif
  ) ;

void prfigmargin(
#if defined(M_PROTO)
  M_WCHAR figmargin [NFIGSIDES] [M_NAMELEN+1]
#endif
  ) ;

void prfigpenwds(
#if defined(M_PROTO)
  long penwdarray[MAXPENS]
#endif
  ) ;

void prfigwidth(
#if defined(M_PROTO)
  char *val
#endif
  ) ;

LOGICAL print_vectors(
#if defined(M_PROTO)
  FILE * hOutFile, PTWTXT **Points, int *maxcalltxt, FILE *hCalFile, int CalNo
#endif
  ) ;

void prplottype(
#if defined(M_PROTO)
  int p1x, int p1y, int p2x, int p2y
#endif
  ) ;

float readcoord(
#if defined(M_PROTO)
  FILE *fh
#endif
  ) ;

float readinches(
#if defined(M_PROTO)
  FILE *fh
#endif
  ) ;

void realoutchar(
#if defined(M_PROTO)
  M_WCHAR textchar, FILE *outfile
#endif
  ) ;

void rsectstart(
#if defined(M_PROTO)
  M_WCHAR *id, M_WCHAR *pagebreak
#endif
  ) ;

void rseqend(
#if defined(M_PROTO)
  M_NOPAR
#endif
  ) ;

void rshnewclear(
#if defined(M_PROTO)
  M_NOPAR
#endif
  ) ;

M_WCHAR *searchforfile(
#if defined(M_PROTO)
  M_WCHAR *file
#endif
  ) ;

void setid(
#if defined(M_PROTO)
  M_WCHAR *id,
  LOGICAL where,
  LOGICAL csensitive,
  LOGICAL inchapter,
  char *chapter,
  M_WCHAR *xrffile,
  int xrfline,
  LOGICAL xrefable	   
#endif
  ) ;

void setopt(
#if defined(M_PROTO)
  char *string, LOGICAL filelenonly
#endif
  ) ;

LOGICAL setvalopt(
#if defined(M_PROTO)
  int thisopt, char *string, LOGICAL filelenonly
#endif
  ) ;

void shchar(
#if defined(M_PROTO)
  M_WCHAR textchar,
  int *len,
  int max,
  M_WCHAR *string,
  void (*proc)(M_WCHAR *string),
  char *msg,
  LOGICAL *errflg
#endif
  ) ;

void mb_shchar(
#if defined(M_PROTO)
  char textchar,
  int *len,
  int max,
  M_WCHAR *string,
  void (*proc)(M_WCHAR *string),
  char *msg,
  LOGICAL *errflg
#endif
  ) ;

void shstring(
#if defined(M_PROTO)
  M_WCHAR *addstring,
  int *len,
  int max,
  M_WCHAR *storestring,
  char *msg,
  LOGICAL *errflg
#endif
  ) ;

void mb_shstring(
#if defined(M_PROTO)
  char *addstring,
  int *len,
  int max,
  M_WCHAR *storestring,
  char *msg,
  LOGICAL *errflg
#endif
  ) ;

void starthelpnode(
#if defined(M_PROTO)
  M_WCHAR *id,
  LOGICAL suppress_topic_map
#endif
  ) ;

void mb_starthelpnode(
#if defined(M_PROTO)
  char *id,
  LOGICAL suppress_topic_map
#endif
  ) ;

void startlablist(
#if defined(M_PROTO)
  M_WCHAR *longlabel, M_WCHAR *width, M_WCHAR *spacing
#endif
  ) ;

void startlist(
#if defined(M_PROTO)
  M_WCHAR *type, M_WCHAR *order, M_WCHAR *spacing, M_WCHAR *cont
#endif
  ) ;

void strcode(
#if defined(M_PROTO)
  M_WCHAR *string, FILE *outfile
#endif
  ) ;

void mb_strcode(
#if defined(M_PROTO)
  char *string, FILE *outfile
#endif
  ) ;

#if defined(hpux) || defined(__aix) || defined(sun)
char *strstr(
#if defined(M_PROTO)
  const char *s1, const char *s2
#endif
  ) ;
#endif

void svhdstring(
#if defined(M_PROTO)
  M_WCHAR *string
#endif
  ) ;

void svtcstring(
#if defined(M_PROTO)
  M_WCHAR *string
#endif
  ) ;

void taboutre(
#if defined(M_PROTO)
  M_NOPAR
#endif
  ) ;

void termchar(
#if defined(M_PROTO)
  M_WCHAR textchar
#endif
  ) ;

void termpi(
#if defined(M_PROTO)
  int m_enttype, M_WCHAR *m_pi, M_WCHAR *m_entname
#endif
  ) ;

void testindexfile(
#if defined(M_PROTO)
  FILE *first
#endif
  ) ;

void texinit(
#if defined(M_PROTO)
  M_NOPAR
#endif
  ) ;

void unimp(
#if defined(M_PROTO)
  M_WCHAR *eltname
#endif
  ) ;

void mb_unimp(
#if defined(M_PROTO)
  char *eltname
#endif
  ) ;

int m_upstrcmp(
#if defined(M_PROTO)
  char *p, char *q
#endif
  ) ;

int vextextsize(
#if defined(M_PROTO)
  const M_WCHAR *keyword
#endif
  ) ;

int vgloss(
#if defined(M_PROTO)
  const M_WCHAR *keyword
#endif
  ) ;

int vcenter(
#if defined(M_PROTO)
  const M_WCHAR *keyword
#endif
  ) ;

int vnumber(
#if defined(M_PROTO)
  const M_WCHAR *keyword
#endif
  ) ;

int vordertype(
#if defined(M_PROTO)
  const M_WCHAR *keyword
#endif
  ) ;

int vrsectpage(
#if defined(M_PROTO)
  const M_WCHAR *keyword
#endif
  ) ;

int vspacing(
#if defined(M_PROTO)
  const M_WCHAR *keyword
#endif
  ) ;

int vtype(
#if defined(M_PROTO)
  const M_WCHAR *keyword
#endif
  ) ;

void xrefexpand(
#if defined(M_PROTO)
  M_WCHAR *id
#endif
  ) ;

void rtrim(
#if defined(M_PROTO)
  char *s
#endif
  ) ;

double usertopt(
#if defined(M_PROTO)
  M_WCHAR *s
#endif
  ) ;

double mb_usertopt(
#if defined(M_PROTO)
  char *s
#endif
  ) ;

long usertosp(
#if defined(M_PROTO)
  char *s
#endif
  ) ;

void whereneedused(
#if defined(M_PROTO)
  M_NOPAR
#endif
  ) ;

void t_addnotes(
#if defined(M_PROTO)
  M_WCHAR *tnoteid
#endif
  ) ;

void t_prntnotes(
#if defined(M_PROTO)
  M_NOPAR
#endif
  ) ;

LOGICAL t_want_a_Q(
#if defined(M_PROTO)
  int span_count
#endif
  );		   


void t_xrefnotes(
#if defined(M_PROTO)
  M_WCHAR *tnoteid
#endif
  ) ;

int t_getnum(
#if defined(M_PROTO)
  char *s
#endif
  ) ;

void t_insertcellmarkup(
#if defined(M_PROTO)
  M_NOPAR
#endif
  ) ;

void t_newpage(
#if defined(M_PROTO)
  M_NOPAR
#endif
  ) ;

void t_startcolh(
#if defined(M_PROTO)
  M_NOPAR
#endif
  ) ;

void t_start_tabhead_colh(
#if defined(M_PROTO)
  M_NOPAR
#endif
  ) ;

void t_nextcell(
#if defined(M_PROTO)
  M_NOPAR
#endif
  ) ;

void t_checkcaption(
#if defined(M_PROTO)
  M_NOPAR
#endif
  ) ;

void t_preamble(
#if defined(M_PROTO)
  M_NOPAR
#endif
  ) ;

void t_getstyle(
#if defined(M_PROTO)
  M_NOPAR
#endif
  ) ;

void t_dospan(
#if defined(M_PROTO)
  LOGICAL in_colh
#endif
  ) ;

void t_startcell(
#if defined(M_PROTO)
  M_WCHAR *span, M_WCHAR *vspan, char *msg
#endif
  ) ;

void t_endcell(
#if defined(M_PROTO)
  M_WCHAR *span, M_WCHAR *vspan
#endif
  ) ;

void t_startrow(
#if defined(M_PROTO)
  M_NOPAR
#endif
  ) ;

void t_endrow(
#if defined(M_PROTO)
  M_WCHAR *taglevone
#endif
  ) ;

void sizetofloat(
#if defined(M_PROTO)
  int column, char *s
#endif
  ) ;

void t_tabstart(
#if defined(M_PROTO)
  M_WCHAR *style, M_WCHAR *id, M_WCHAR *number, M_WCHAR *box, M_WCHAR *rule, 
  M_WCHAR *vrule, M_WCHAR *hrule, M_WCHAR *vcount, M_WCHAR *hcount,
  M_WCHAR *spacing, M_WCHAR *width, M_WCHAR *position, 
  M_WCHAR *tonumber, M_WCHAR *divide, M_WCHAR *textsize
#endif
  ) ;

void t_tablebody(
#if defined(M_PROTO)
  M_NOPAR
#endif
  ) ;

void t_sendout_rowrule(
#if defined(M_PROTO)
  M_NOPAR
#endif
  ) ;

void showmemavail(
#if defined(M_PROTO)
  M_NOPAR
#endif
  );

int m_upper(
#if defined(M_PROTO)
  int c
#endif
  ) ;

char *GetDefaultHeaderString(
#if defined(M_PROTO)
  char          *elementName,
  unsigned char  desiredType,
  char          *defaultString
#endif
  ) ;


void SetDefaultLocale(
#if defined(M_PROTO)
  M_NOPAR
#endif
  );

char *MakeMByteString(
#if defined(M_PROTO)
  const M_WCHAR *from
#endif
  );

M_WCHAR *MakeWideCharString(
#if defined(M_PROTO)
  const char *from
#endif
  );

void paragraph(
#if defined(M_PROTO)
  M_WCHAR *indent,
  M_WCHAR *id,
  M_WCHAR *gentity,
  M_WCHAR *gposition,
  M_WCHAR *ghyperlink,
  M_WCHAR *glinktype,
  M_WCHAR *gdescription,
  char    *listinfo
#endif
  );

void figure(
#if defined(M_PROTO)
       M_WCHAR *number,
       M_WCHAR *tonumber,
       M_WCHAR *id,
       M_WCHAR *file,
       M_WCHAR *figpos,
       M_WCHAR *cappos,
       M_WCHAR *oldtype,
       M_WCHAR *xwidth,
       M_WCHAR *xdepth,
       M_WCHAR *xhadjust,
       M_WCHAR *xvadjust,
       M_WCHAR *border,
       M_WCHAR *type,
       M_WCHAR *xmagnify,
       M_WCHAR *video,
       M_WCHAR *strip,
       M_WCHAR *mirror,
       M_WCHAR *margin,
       M_WCHAR *clip,
       M_WCHAR *penwidth,
       M_WCHAR *snap,
       M_WCHAR *autoscale,
       M_WCHAR *plottype,
       M_WCHAR *callout,
       M_WCHAR *textsize,
       M_WCHAR *ghyperlink,
       M_WCHAR *glinktype,
       M_WCHAR *gdescription,
       char    *listinfo
#endif
  );

void endterm(
#if defined(M_PROTO)
       M_WCHAR *base,
       M_WCHAR *gloss,
       int      linktype
#endif
  );

M_WCHAR wc_toupper(
#if defined(M_PROTO)
       M_WCHAR wc
#endif
  );
