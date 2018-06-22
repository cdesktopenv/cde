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

void appstart(M_WCHAR *id, M_WCHAR *letter);

void assert_hometopic_exists(void);

void badgraphic(int filestat);

void basename(void);

void calcdisp (M_WCHAR *file, M_WCHAR *startrow, M_WCHAR *endrow,
  M_WCHAR *clip, M_WCHAR *margin);

void callndbeg (void);

void callndend (void);

/* used by ccompspec(), cmenlspec(), and cwmenlspec() */
char *ccharspec(M_WCHAR textchar);

void ccmpiooutchar (M_WCHAR textchar);

void ccompesoutchar (M_WCHAR textchar, FILE *outfile);

void ccompexoutchar (M_WCHAR textchar, FILE *outfile);

void ccompoutchar (M_WCHAR textchar, FILE *outfile);

/* special computer expansions for dot matrix(computer) in calculator style */
char *ccompspec(M_WCHAR textchar);

void chapstart(M_WCHAR *id, M_WCHAR *number, char type[]);

M_WCHAR *checkdimen(M_WCHAR *val, M_WCHAR *paramname, M_WCHAR *elt);

M_WCHAR *mb_checkdimen(M_WCHAR *val, char *paramname, char *elt);

M_WCHAR *checkent(M_WCHAR *entcontent);

void checkgloss(void);

M_WCHAR *checkid(M_WCHAR *id);

void checkmsghead (void);

M_WCHAR *checkreal (M_WCHAR *val, M_WCHAR *paramname, LOGICAL msgout, M_WCHAR *elt);

LOGICAL chkplottype(M_WCHAR *val, int *p1x, int *p1y, int *p2x, int *p2y, LOGICAL prnt );

void cmloutchar (M_WCHAR textchar, FILE *outfile);

/* special menu lable character expansions */
char *cmenlspec(M_WCHAR textchar);

void cspotend(M_WCHAR *file, int graphics, M_WCHAR *xwidth, M_WCHAR *xheight);

void cspotstart(M_WCHAR *file, int graphics, M_WCHAR *xwidth, M_WCHAR *xheight);

void csubtc (M_WCHAR textchar, FILE *outfile, M_WCHAR *parent);

void csuptc (M_WCHAR textchar, FILE *outfile, M_WCHAR *parent);

/* special white menu label character expansions */
char *cwmenlspec(M_WCHAR textchar);

void cwmloutchar(M_WCHAR textchar, FILE *outfile);

void defxref(FILE *xfile, M_WCHAR *id, struct xref *xref);

int do_esc_seq(void);

void dumpxref(void);

void echohead(M_WCHAR *p);

void mb_echohead(char *p);

void endhead(void);

void endhelpnode(void);

void esoutchar(M_WCHAR textchar);

void exoutchar(M_WCHAR textchar);

void exvexend(M_WCHAR *textsize);

void exvexstart(M_WCHAR *type, M_WCHAR *textsize, char *listinfo);

void figend (M_WCHAR *id);

int figext (M_WCHAR *fname);

int figlist (char *fnin);

void figraster(M_WCHAR *file,    M_WCHAR *magnify,
M_WCHAR *width,   M_WCHAR *depth,        M_WCHAR *hadjust,  M_WCHAR *vadjust,
M_WCHAR *mirror,  M_WCHAR *margin,       M_WCHAR *clip,
M_WCHAR *penwidth,M_WCHAR *plottype,     M_WCHAR *callout,  M_WCHAR *textsize);

void figstart(M_WCHAR *number,  M_WCHAR *tonumber,  M_WCHAR *id,          M_WCHAR *file,
M_WCHAR *type,    M_WCHAR *oldtype,
M_WCHAR *xmagnify,M_WCHAR *xwidth,    M_WCHAR *xdepth,      M_WCHAR *xhadjust,
M_WCHAR *xvadjust,M_WCHAR *border,    M_WCHAR *video,       M_WCHAR *strip,
M_WCHAR *mirror,  M_WCHAR *margin,      M_WCHAR *clip,
M_WCHAR *penwidth,M_WCHAR *snap,      M_WCHAR *autoscale,   M_WCHAR *plottype,
M_WCHAR *callout, M_WCHAR *textsize);

void figvector(M_WCHAR *file,     M_WCHAR *magnify,
M_WCHAR *width,    M_WCHAR *depth,        M_WCHAR *hadjust,  M_WCHAR *vadjust,
M_WCHAR *mirror,   M_WCHAR *margin,       M_WCHAR *clip,
M_WCHAR *penwidth, M_WCHAR *autoscale,    M_WCHAR *plottype, M_WCHAR *callout,
M_WCHAR *textsize);

unsigned get2b(FILE *fh);

unsigned long get4b(FILE *fh);

unsigned long getPCLdims (char *fn);

int getqualified (M_WCHAR *qualname, M_WCHAR *unqualname);

int mb_getqualified (char *qualname, char *unqualname);

void handle_link_and_graphic(M_WCHAR *parent, M_WCHAR *gentity, M_WCHAR *gposition, M_WCHAR *ghyperlink,
  M_WCHAR *glinktype, M_WCHAR *gdescription);     

void keyesoutchar (M_WCHAR textchar);

void idstring(M_WCHAR *string);

void indexchar(M_WCHAR textchar);

void item(M_WCHAR *id);

char *m_itoa(int n, char *s);

void itoletter(int n, char start, char *dest, int length);

LOGICAL m_letter(M_WCHAR c);

void loadxref(void);

int m_lower(int c);

char *makecsname(M_WCHAR *id);

void manustart(M_WCHAR *language, M_WCHAR *idxvol, M_WCHAR *status);

void notimp(char *eltname, char *option);

M_WCHAR *okdimen (M_WCHAR *val);

void onlygraphic(char    *param,    M_WCHAR *magnify, M_WCHAR *video,     M_WCHAR *mirror,
  M_WCHAR *margin,   M_WCHAR *clip,    M_WCHAR *penwidth,  M_WCHAR *autoscale,
  M_WCHAR *plottype, M_WCHAR *hadjust, M_WCHAR *vadjust);

void open_new_helpfile(void);
		       
void notallow(char *param1, char *param2);

void options(LOGICAL filelenonly);

void outcall(M_WCHAR textchar, FILE *outfile);

void outchar(M_WCHAR textchar, FILE *outfile);

void outpi(int enttype, M_WCHAR *pi, M_WCHAR *entname);

void parspace(void);

LOGICAL pushndok(LOGICAL val);

LOGICAL popndok(void);

void prfigborder(int val);

void prfigclip(M_WCHAR figmargin [NFIGSIDES] [M_NAMELEN+1]);

void prfigheight(char *val);

void prfigmagnify(char *val);

void prfigmargin(M_WCHAR figmargin [NFIGSIDES] [M_NAMELEN+1]);

void prfigpenwds(long penwdarray[MAXPENS]);

void prfigwidth(char *val);

LOGICAL print_vectors(FILE * hOutFile, PTWTXT **Points, int *maxcalltxt, FILE *hCalFile, int CalNo);

void prplottype(int p1x, int p1y, int p2x, int p2y);

float readcoord(FILE *fh);

float readinches(FILE *fh);

void realoutchar(M_WCHAR textchar, FILE *outfile);

void rsectstart(M_WCHAR *id, M_WCHAR *pagebreak);

void rseqend(void);

void rshnewclear(void);

M_WCHAR *searchforfile(M_WCHAR *file);

void setid(M_WCHAR *id,
  LOGICAL where,
  LOGICAL csensitive,
  LOGICAL inchapter,
  char *chapter,
  M_WCHAR *xrffile,
  int xrfline,
  LOGICAL xrefable);

void setopt(char *string, LOGICAL filelenonly);

LOGICAL setvalopt(int thisopt, char *string, LOGICAL filelenonly);

void shchar(M_WCHAR textchar,
  int *len,
  int max,
  M_WCHAR *string,
  void (*proc)(M_WCHAR *string),
  char *msg,
  LOGICAL *errflg);

void mb_shchar(char textchar,
  int *len,
  int max,
  M_WCHAR *string,
  void (*proc)(M_WCHAR *string),
  char *msg,
  LOGICAL *errflg);

void shstring(M_WCHAR *addstring,
  int *len,
  int max,
  M_WCHAR *storestring,
  char *msg,
  LOGICAL *errflg);

void mb_shstring(char *addstring,
  int *len,
  int max,
  M_WCHAR *storestring,
  char *msg,
  LOGICAL *errflg);

void starthelpnode(M_WCHAR *id,
  LOGICAL suppress_topic_map);

void mb_starthelpnode(char *id,
  LOGICAL suppress_topic_map);

void startlablist(M_WCHAR *longlabel, M_WCHAR *width, M_WCHAR *spacing);

void startlist(M_WCHAR *type, M_WCHAR *order, M_WCHAR *spacing, M_WCHAR *cont);

void strcode(M_WCHAR *string, FILE *outfile);

void mb_strcode(char *string, FILE *outfile);

#if defined(hpux) || defined(__aix) || defined(sun)
char *strstr(const char *s1, const char *s2);
#endif

void svhdstring(M_WCHAR *string);

void svtcstring(M_WCHAR *string);

void taboutre(void);

void termchar(M_WCHAR textchar);

void termpi(int m_enttype, M_WCHAR *m_pi, M_WCHAR *m_entname);

void testindexfile(FILE *first);

void texinit(void);

void unimp(M_WCHAR *eltname);

void mb_unimp(char *eltname);

int m_upstrcmp(char *p, char *q);

int vextextsize(const M_WCHAR *keyword);

int vgloss(const M_WCHAR *keyword);

int vcenter(const M_WCHAR *keyword);

int vnumber(const M_WCHAR *keyword);

int vordertype(const M_WCHAR *keyword);

int vrsectpage(const M_WCHAR *keyword);

int vspacing(const M_WCHAR *keyword);

int vtype(const M_WCHAR *keyword);

void xrefexpand(M_WCHAR *id);

void rtrim(char *s);

double usertopt(M_WCHAR *s);

double mb_usertopt(char *s);

long usertosp(char *s);

void whereneedused(void);

void t_addnotes(M_WCHAR *tnoteid);

void t_prntnotes(void);

LOGICAL t_want_a_Q(int span_count);   


void t_xrefnotes(M_WCHAR *tnoteid);

int t_getnum(char *s);

void t_insertcellmarkup(void);

void t_newpage(void);

void t_startcolh(void);

void t_start_tabhead_colh(void);

void t_nextcell(void);

void t_checkcaption(void);

void t_preamble(void);

void t_getstyle(void);

void t_dospan(LOGICAL in_colh);

void t_startcell(M_WCHAR *span, M_WCHAR *vspan, char *msg);

void t_endcell(M_WCHAR *span, M_WCHAR *vspan);

void t_startrow(void);

void t_endrow(M_WCHAR *taglevone);

void sizetofloat(int column, char *s);

void t_tabstart(M_WCHAR *style, M_WCHAR *id, M_WCHAR *number, M_WCHAR *box, M_WCHAR *rule, 
  M_WCHAR *vrule, M_WCHAR *hrule, M_WCHAR *vcount, M_WCHAR *hcount,
  M_WCHAR *spacing, M_WCHAR *width, M_WCHAR *position, 
  M_WCHAR *tonumber, M_WCHAR *divide, M_WCHAR *textsize);

void t_tablebody(void);

void t_sendout_rowrule(void);

void showmemavail(void);

int m_upper(int c);

char *GetDefaultHeaderString(char          *elementName,
  unsigned char  desiredType,
  char          *defaultString);


void SetDefaultLocale(void);

char *MakeMByteString(const M_WCHAR *from);

M_WCHAR *MakeWideCharString(const char *from);

void paragraph(M_WCHAR *indent,
  M_WCHAR *id,
  M_WCHAR *gentity,
  M_WCHAR *gposition,
  M_WCHAR *ghyperlink,
  M_WCHAR *glinktype,
  M_WCHAR *gdescription,
  char    *listinfo);

void figure(M_WCHAR *number,
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
       char    *listinfo);

void endterm(M_WCHAR *base,
       M_WCHAR *gloss,
       int      linktype);

M_WCHAR wc_toupper(M_WCHAR wc);
