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
/* $XConsortium: fproto.h /main/3 1995/11/08 09:30:01 rswiston $ */
/* Copyright (c) 1988, 1989, 1990 Hewlett-Packard Co. */
/* Function prototypes for HP Tag/TeX translator */

void assert_hometopic_exists(
#if defined(M_PROTO)
  M_NOPAR
#endif
  ) ;

/* osf has basename() defined in system header file, change
 * basename() to fbasename(). --XPG4 standard.
 */
void fbasename(
#if defined(M_PROTO)
  M_NOPAR
#endif
  ) ;

void chapstart(
#if defined(M_PROTO)
  M_WCHAR *id
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

void esoutchar(
#if defined(M_PROTO)
  M_WCHAR textchar
#endif
  ) ;

void vexoutchar(
#if defined(M_PROTO)
  M_WCHAR textchar
#endif
  ) ;

void exoutchar(
#if defined(M_PROTO)
  M_WCHAR textchar
#endif
  ) ;

void imoutchar(
#if defined(M_PROTO)
  M_WCHAR textchar
#endif
  ) ;

int getqualified (
#if defined(M_PROTO)
  M_WCHAR *qualname, M_WCHAR *unqualname
#endif
  ) ;

void handle_link_and_graphic(
#if defined(M_PROTO)
  M_WCHAR *parent, M_WCHAR *gentity, M_WCHAR *gposition, M_WCHAR *ghyperlink,
  M_WCHAR *glinktype, M_WCHAR *gdescription
#endif
  );			     

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

void Item(
#if defined(M_PROTO)
  M_WCHAR *id
#endif
  ) ;

char *m_itoa(
#if defined(M_PROTO)
  int n, char *s
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

void options(
#if defined(M_PROTO)
  LOGICAL filelenonly
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

void realoutchar(
#if defined(M_PROTO)
  M_WCHAR textchar, FILE *outfile
#endif
  ) ;

void rsectstart(
#if defined(M_PROTO)
  M_WCHAR *id
#endif
  ) ;

void rseqend(
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
  M_WCHAR *ssi,
  M_WCHAR *id,
  int      level
#endif
  ) ;

void mb_starthelpnode(
#if defined(M_PROTO)
  char    *ssi,
  char    *id,
  int      level
#endif
  ) ;

void StartLabList(
#if defined(M_PROTO)
  M_WCHAR *spacing,
  M_WCHAR *longlabel
#endif
  ) ;

void StartList(
#if defined(M_PROTO)
  M_WCHAR *type, M_WCHAR *order, M_WCHAR *spacing, M_WCHAR *cont
#endif
  ) ;

void EndList(
#if defined(M_PROTO)
  M_NOPAR
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

char *strstr(
#if defined(M_PROTO)
  const char *s1, const char *s2
#endif
  ) ;

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

void texinit(
#if defined(M_PROTO)
  M_NOPAR
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

LOGICAL vstack(
#if defined(M_PROTO)
  const M_WCHAR *keyword
#endif
  ) ;

int vordertype(
#if defined(M_PROTO)
  const M_WCHAR *keyword
#endif
  ) ;

int vlonglabel(
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
  M_WCHAR *gdescription
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
       M_WCHAR *ghyperlink,
       M_WCHAR *glinktype,
       M_WCHAR *gdescription
#endif
  );

void endterm(
#if defined(M_PROTO)
       M_WCHAR *base,
       M_WCHAR *gloss,
       char    *linktype
#endif
  );

M_WCHAR wc_toupper(
#if defined(M_PROTO)
       M_WCHAR wc
#endif
  );

M_WCHAR *wc_stringtoupper(
#if defined(M_PROTO)
       M_WCHAR *wcp
#endif
  );

int NextId(
#if defined(M_PROTO)
  M_NOPAR
#endif
);

char *GetLanguage(
#if defined(M_PROTO)
  M_NOPAR
#endif
);

char *GetCharset(
#if defined(M_PROTO)
  M_NOPAR
#endif
);

void HandleLink(
#if defined(M_PROTO)
    M_WCHAR *hyperlink,
    M_WCHAR *type,
    M_WCHAR *description
#endif
  );

char *mb_realloc(
#if defined(M_PROTO)
    char *ptr, long size
#endif
  );

char *mb_malloc(
#if defined(M_PROTO)
    long size
#endif
  );

void mb_free(
#if defined(M_PROTO)
    char **pptr
#endif
  );

void Add2ToRowVec(
#if defined(M_PROTO)
    int *length, char **rowvec, char *id1, char *id2
#endif
  );

void StartEx(
#if defined(M_PROTO)
M_WCHAR *notes, M_WCHAR *lines, M_WCHAR *textsize
#endif
  );

void EndEx(
#if defined(M_PROTO)
  M_NOPAR
#endif
  );

void StartNCW(
#if defined(M_PROTO)
  char *which
#endif
  );

void StartBlock(
#if defined(M_PROTO)
  char *pclass, char *ssi, char *id
#endif
  );

void AddToSNB(
#if defined(M_PROTO)
  char *id, char *iconName
#endif
  );

void StartNCWtext(
#if defined(M_PROTO)
  char *which, char *iconFile, char *headingString
#endif
  );

void IncludeToss(
#if defined(M_PROTO)
  M_NOPAR
#endif
  );

M_WCHAR *CycleEnt(
#if defined(M_PROTO)
    LOGICAL init,
    unsigned char *type,
    M_WCHAR ***content,
    unsigned char *wheredef
#endif
  );

void ModifyEntities(
#if defined(M_PROTO)
  M_NOPAR
#endif
  );

void PushForm(
#if defined(M_PROTO)
  char *class,
  char *ssi,
  char *id
#endif
  ) ;

void PushForm2(
#if defined(M_PROTO)
  char *class,
  char *ssi,
  char *id1,
  char *id2
#endif
  ) ;

void PopForm(
#if defined(M_PROTO)
  M_NOPAR
#endif
  ) ;

void PopForm2(
#if defined(M_PROTO)
  M_NOPAR
#endif
  ) ;

void PopFormMaybe(
#if defined(M_PROTO)
  M_NOPAR
#endif
  ) ;

void EmitSavedAnchors(
#if defined(M_PROTO)
  M_NOPAR
#endif
  ) ;

void CloseVirpage(
#if defined(M_PROTO)
  M_NOPAR
#endif
  ) ;
