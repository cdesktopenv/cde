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
/* $XConsortium: fproto.h /main/3 1995/11/08 10:08:24 rswiston $ */
/* Copyright (c) 1988, 1989, 1990 Hewlett-Packard Co. */
/* Function prototypes for HP Tag/TeX translator */

void assert_hometopic_exists(void);

/* conflict with prototype defined for basename() in <string.h> on osf, 
   so use fbasename */
void fbasename(void);

void chapstart(M_WCHAR *id);

void checkgloss(void);

M_WCHAR *checkid(M_WCHAR *id);

void checkmsghead (void);

void dumpxref(void);

void echohead(M_WCHAR *p);

void mb_echohead(char *p);

void endhead(void);

void esoutchar(M_WCHAR textchar);

void vexoutchar(M_WCHAR textchar);

void exoutchar(M_WCHAR textchar);

void imoutchar(M_WCHAR textchar);

int getqualified (M_WCHAR *qualname, M_WCHAR *unqualname);

void handle_link_and_graphic(M_WCHAR *parent, M_WCHAR *gentity, M_WCHAR *gposition, M_WCHAR *ghyperlink,
  M_WCHAR *glinktype, M_WCHAR *gdescription);

void idstring(M_WCHAR *string);

void indexchar(M_WCHAR textchar);

void Item(M_WCHAR *id);

char *m_itoa(int n, char *s);

LOGICAL m_letter(M_WCHAR c);

void loadxref(void);

int m_lower(int c);

void options(LOGICAL filelenonly);

void outchar(M_WCHAR textchar, FILE *outfile );

void outpi(int enttype, M_WCHAR *pi, M_WCHAR *entname);

void realoutchar(M_WCHAR textchar, FILE *outfile);

void rsectstart(M_WCHAR *id);

void rseqend(void);

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

void starthelpnode(M_WCHAR *ssi,
  M_WCHAR *id,
  int      level);

void mb_starthelpnode(char    *ssi,
  char    *id,
  int      level);

void StartLabList(M_WCHAR *spacing,
  M_WCHAR *longlabel);

void StartList(M_WCHAR *type, M_WCHAR *order, M_WCHAR *spacing, M_WCHAR *cont);

void EndList(void);

void strcode(M_WCHAR *string, FILE *outfile);

void mb_strcode(char *string, FILE *outfile);

char *strstr(const char *s1, const char *s2);

void svhdstring(M_WCHAR *string);

void svtcstring(M_WCHAR *string);

void termchar(M_WCHAR textchar);

void termpi(int m_enttype, M_WCHAR *m_pi, M_WCHAR *m_entname);

void texinit(void);

int vextextsize(const M_WCHAR *keyword);

int vgloss(const M_WCHAR *keyword);

int vcenter(const M_WCHAR *keyword);

int vnumber(const M_WCHAR *keyword);

LOGICAL vstack(const M_WCHAR *keyword);

int vordertype(const M_WCHAR *keyword);

int vlonglabel(const M_WCHAR *keyword);

int vspacing(const M_WCHAR *keyword);

int vtype(const M_WCHAR *keyword);

void xrefexpand(M_WCHAR *id);

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
  M_WCHAR *gdescription);

void figure(     M_WCHAR *number,
       M_WCHAR *tonumber,
       M_WCHAR *id,
       M_WCHAR *file,
       M_WCHAR *figpos,
       M_WCHAR *cappos,
       M_WCHAR *ghyperlink,
       M_WCHAR *glinktype,
       M_WCHAR *gdescription);

void endterm(     M_WCHAR *base,
       M_WCHAR *gloss,
       char    *linktype);

M_WCHAR wc_toupper(     M_WCHAR wc);

M_WCHAR *wc_stringtoupper(     M_WCHAR *wcp);

int NextId(void);

char *GetLanguage(void);

char *GetCharset(void);

void HandleLink(  M_WCHAR *hyperlink,
    M_WCHAR *type,
    M_WCHAR *description);

char *mb_realloc(  char *ptr, long size);

char *mb_malloc(  long size);

void mb_free(  char **pptr);

void Add2ToRowVec(  int *length, char **rowvec, char *id1, char *id2);

void StartEx(M_WCHAR *notes, M_WCHAR *lines, M_WCHAR *textsize);

void EndEx(void);

void StartNCW(char *which);

void StartBlock(char *pclass, char *ssi, char *id);

void AddToSNB(char *id, char *iconName);

void StartNCWtext(char *which, char *iconFile, char *headingString);

void IncludeToss(void);

M_WCHAR *CycleEnt(  LOGICAL init,
    unsigned char *type,
    M_WCHAR ***content,
    unsigned char *wheredef);

void ModifyEntities(void);

void PushForm(char *class,
  char *ssi,
  char *id);

void PushForm2(char *class,
  char *ssi,
  char *id1,
  char *id2);

void PopForm(void);

void PopForm2(void);

void PopFormMaybe(void);

void EmitSavedAnchors(void);

void CloseVirpage(void);
