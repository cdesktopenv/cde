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
/* $XConsortium: proto.h /main/3 1995/11/08 09:42:21 rswiston $ */
int m_actgetc(
#if defined(M_PROTO)
  M_NOPAR
#endif
  ) ;

void m_adjuststate(
#if defined(M_PROTO)
  M_NOPAR
#endif
  ) ;

LOGICAL m_allwhite(
#if defined(M_PROTO)
  const M_WCHAR *string
#endif
  ) ;

void m_attval(
#if defined(M_PROTO)
  M_WCHAR *string
#endif
  ) ;

LOGICAL m_attvonly(
#if defined(M_PROTO)
  M_WCHAR *string
#endif
  ) ;

int m_checkstart(
#if defined(M_PROTO)
  M_ELEMENT val
#endif
  ) ;

LOGICAL m_ckend(
#if defined(M_PROTO)
  M_ELEMENT val, LOGICAL neednet
#endif
  ) ;

void m_ckmap(
#if defined(M_PROTO)
  M_WCHAR *name, LOGICAL useoradd
#endif
  ) ;

void m_closent(
#if defined(M_PROTO)
  void *m_ptr
#endif
  ) ;

void m_codeent(
#if defined(M_PROTO)
  int m_ent
#endif
  ) ;

M_PARSE *m_copystackelt(
#if defined(M_PROTO)
  M_NOPAR
#endif
  ) ;

M_WCHAR *m_cyclent(
#if defined(M_PROTO)
  LOGICAL init, unsigned char *type, M_WCHAR **content, 
  unsigned char *wheredef
#endif
  ) ;

void m_dispcurelt(
#if defined(M_PROTO)
  M_WCHAR *file, int line
#endif
  ) ;

void m_done(
#if defined(M_PROTO)
  M_NOPAR
#endif
  ) ;

void m_dumpline(
#if defined(M_PROTO)
  M_WCHAR *file, int line
#endif
  ) ;

void m_eduptype(
#if defined(M_PROTO)
  int type
#endif
  ) ;

M_ELEMENT m_eltname(
#if defined(M_PROTO)
  M_NOPAR
#endif
  ) ;

void m_endaction(
#if defined(M_PROTO)
  M_ELEMENT m_elt
#endif
  ) ;

void m_endcase(
#if defined(M_PROTO)
  int m_action
#endif
  ) ;

void m_endtag(
#if defined(M_PROTO)
  M_ELEMENT c
#endif
  ) ;

void m_entexpand(
#if defined(M_PROTO)
  M_ENTITY *openent
#endif
  ) ;

void m_eprefix(
#if defined(M_PROTO)
  M_NOPAR
#endif
  ) ;

void m_err1(
#if defined(M_PROTO)
  const char *text, const M_WCHAR *arg
#endif
  ) ;

void m_err2(
#if defined(M_PROTO)
  const char *text, const M_WCHAR *arg1, const M_WCHAR *arg2
#endif
  ) ;

void m_err3(
#if defined(M_PROTO)
  char *text, M_WCHAR *arg1, M_WCHAR *arg2, M_WCHAR *arg3
#endif
  ) ;

void m_err4(
#if defined(M_PROTO)
  char *text, M_WCHAR *arg1, M_WCHAR *arg2, M_WCHAR *arg3, M_WCHAR *arg4
#endif
  ) ;

void m_err5(
#if defined(M_PROTO)
  char *text, M_WCHAR *arg1, M_WCHAR *arg2, M_WCHAR *arg3,
	      M_WCHAR *arg4, M_WCHAR *arg5
#endif
  ) ;

void m_err6(
#if defined(M_PROTO)
  char *text, M_WCHAR *arg1, M_WCHAR *arg2, M_WCHAR *arg3,
	      M_WCHAR *arg4, M_WCHAR *arg5, M_WCHAR *arg6
#endif
  ) ;

void m_err7(
#if defined(M_PROTO)
  char *text, M_WCHAR *arg1, M_WCHAR *arg2, M_WCHAR *arg3,
	      M_WCHAR *arg4, M_WCHAR *arg5, M_WCHAR *arg6, M_WCHAR *arg7
#endif
  ) ;

void m_errline(
#if defined(M_PROTO)
  char *p
#endif
  ) ;

void m_error(
#if defined(M_PROTO)
  char *text
#endif
  ) ;

void m_esuffix(
#if defined(M_PROTO)
  M_NOPAR
#endif
  ) ;

void m_exit(
#if defined(M_PROTO)
  int status
#endif
  ) ;

void m_etcomplete(
#if defined(M_PROTO)
  M_NOPAR
#endif
  ) ;

LOGICAL m_excluded(
#if defined(M_PROTO)
  M_ELEMENT elt
#endif
  ) ;

void m_expecting(
#if defined(M_PROTO)
  M_NOPAR
#endif
  ) ;

void m_expexpand(
#if defined(M_PROTO)
  LOGICAL *expstart, M_STATE node, LOGICAL *required, LOGICAL *data
#endif
  ) ;

void m_expline(
#if defined(M_PROTO)
  LOGICAL *expstart, LOGICAL *data, M_ELEMENT label
#endif
  ) ;

void m_exptend(
#if defined(M_PROTO)
  LOGICAL *expstart, M_PARSE *stackptr
#endif
  ) ;

int m_findact(
#if defined(M_PROTO)
  M_ELEMENT elt, int *array
#endif
  ) ;

void m_findatt(
#if defined(M_PROTO)
  M_NOPAR
#endif
  ) ;

void m_findchain(
#if defined(M_PROTO)
  M_PARSE *stackptr, int start, int chainin, int *chainout, int *index, 
  LOGICAL wild
#endif
  ) ;

int m_findpar(
#if defined(M_PROTO)
  const char *elt, const char *param, const M_WCHAR *value
#endif
  ) ;

M_ELEMENT m_findunique(
#if defined(M_PROTO)
  M_STATE from, int *newleft
#endif
  ) ;

void m_frcend(
#if defined(M_PROTO)
  M_ELEMENT val
#endif
  ) ;

void m_frcstart(
#if defined(M_PROTO)
  M_NOPAR
#endif
  ) ;

void m_free(
#if defined(M_PROTO)
  void *block, char *msg
#endif
  ) ;

void m_freeFSA(
#if defined(M_PROTO)
  M_PARSE *stackelt
#endif
  ) ;

void m_freemin(
#if defined(M_PROTO)
  M_MIN *min
  , char *msg
#endif
  ) ;

void m_freeparam(
#if defined(M_PROTO)
  M_PARSE *stackelt
#endif
  ) ;

LOGICAL m_gendelim(
#if defined(M_PROTO)
  int srlen, int context
#endif
  ) ;

int m_getachar(
#if defined(M_PROTO)
  M_HOLDTYPE *dchar
#endif
  ) ;

int m_getc(
#if defined(M_PROTO)
  void *m_ptr
#endif
  ) ;

int mb_getwc(
#if defined(M_PROTO)
  void *m_ptr
#endif
  ) ;

void *m_getdata(
#if defined(M_PROTO)
  int n, LOGICAL *flag
#endif
  ) ;

void m_getline(
#if defined(M_PROTO)
  M_WCHAR **file, int *line
#endif
  ) ;

void m_getname(
#if defined(M_PROTO)
  M_WCHAR first
#endif
  ) ;

void m_getsignon(
#if defined(M_PROTO)
  M_NOPAR
#endif
  ) ;

int m_gettoken(
#if defined(M_PROTO)
  int *c, M_HOLDTYPE *dchar, int context
#endif
  ) ;

void m_globes(
#if defined(M_PROTO)
  M_NOPAR
#endif
  ) ;

void m_globss(
#if defined(M_PROTO)
  M_NOPAR
#endif
  ) ;

void m_holdproc(
#if defined(M_PROTO)
  M_NOPAR
#endif
  ) ;

void m_inctest(
#if defined(M_PROTO)
  int *count, int limit, char *message
#endif
  ) ;

void m_initctype(
#if defined(M_PROTO)
  M_NOPAR
#endif
  ) ;

void m_initialize(
#if defined(M_PROTO)
  M_NOPAR
#endif
  ) ;

void m_lastchars(
#if defined(M_PROTO)
  M_NOPAR
#endif
  ) ;

int m_level(
#if defined(M_PROTO)
  M_WCHAR *elt
#endif
  ) ;

int m_mblevel(
#if defined(M_PROTO)
  char *elt
#endif
  ) ;

void m_litproc(
#if defined(M_PROTO)
  int delim
#endif
  ) ;

void m_longent(
#if defined(M_PROTO)
  int context
#endif
  ) ;

LOGICAL m_lookent(
#if defined(M_PROTO)
  M_WCHAR *name, unsigned char *type, M_WCHAR **content, 
  unsigned char *wheredef
#endif
  ) ;

int main(
#if defined(M_PROTO)
  int argc, char **argv
#endif
  ) ;

void *m_malloc(
#if defined(M_PROTO)
  int size, char *msg
#endif
  ) ;

void m_missingtagc(
#if defined(M_PROTO)
  int c, M_HOLDTYPE dchar, LOGICAL start
#endif
  ) ;

LOGICAL m_nextand(
#if defined(M_PROTO)
  M_OPENFSA *thisfsa, M_ELEMENT label
#endif
  ) ;

void m_nextdelimchar(
#if defined(M_PROTO)
  int *n, int i, LOGICAL *linestart, LOGICAL newlinestart, LOGICAL skipblank, 
  unsigned char type
#endif
  ) ;

void m_ntrent(
#if defined(M_PROTO)
  M_WCHAR *p
#endif
  ) ;

void m_nullendtag(
#if defined(M_PROTO)
  M_NOPAR
#endif
  ) ;

LOGICAL m_omitend(
#if defined(M_PROTO)
  M_NOPAR
#endif
  ) ;

LOGICAL m_omitstart(
#if defined(M_PROTO)
  M_NOPAR
#endif
  ) ;

void m_openchk(
#if defined(M_PROTO)
  FILE **ptr, char *name, char *mode
#endif
  ) ;

void *m_openent(
#if defined(M_PROTO)
  M_WCHAR *entcontent
#endif
  ) ;

void *m_openfirst(
#if defined(M_PROTO)
  M_NOPAR
#endif
  ) ;

void m_optstring(
#if defined(M_PROTO)
  char *p
#endif
  ) ;

int m_packedlook(
#if defined(M_PROTO)
  M_PTRIE *xptrie, M_WCHAR *name
#endif
  ) ;

int m_parcount(
#if defined(M_PROTO)
  M_WCHAR *elt
#endif
  ) ;

M_WCHAR *m_pardefault(
#if defined(M_PROTO)
  M_WCHAR *elt, M_WCHAR *param, unsigned char *type
#endif
  ) ;

M_WCHAR *m_parent(
#if defined(M_PROTO)
  int n
#endif
  ) ;

M_WCHAR *m_parname(
#if defined(M_PROTO)
  M_WCHAR *elt, int n
#endif
  ) ;

M_WCHAR *m_mbparname(
#if defined(M_PROTO)
  char *elt, int n
#endif
  ) ;

const M_WCHAR *m_partype(
#if defined(M_PROTO)
  const int par, const M_WCHAR *string
#endif
  ) ;

void m_parupper(
#if defined(M_PROTO)
  int par, M_WCHAR *string
#endif
  ) ;

LOGICAL m_parvalok(
#if defined(M_PROTO)
  M_WCHAR *elt, M_WCHAR *param, const M_WCHAR *value
#endif
  ) ;

void m_piaction(
#if defined(M_PROTO)
  M_WCHAR *m_pi, M_WCHAR *m_entname, int m_enttype
#endif
  ) ;

void m_pop(
#if defined(M_PROTO)
  M_NOPAR
#endif
  ) ;

void m_push(
#if defined(M_PROTO)
  M_ELEMENT elt, M_STATE current, LOGICAL need
#endif
  ) ;

LOGICAL m_putdata(
#if defined(M_PROTO)
  void *data, int n
#endif
  ) ;

void m_readcomments(
#if defined(M_PROTO)
  M_NOPAR
#endif
  ) ;

void *m_realloc(
#if defined(M_PROTO)
  void *ptr, int size, char *msg
#endif
  ) ;

int m_scan(
#if defined(M_PROTO)
  LOGICAL prolog
#endif
  ) ;

void m_setmap(
#if defined(M_PROTO)
  int map, LOGICAL useoradd
#endif
  ) ;

void m_setoptions(
#if defined(M_PROTO)
  M_NOPAR
#endif
  ) ;

void m_setparam(
#if defined(M_PROTO)
  M_WCHAR **cvar, int par
#endif
  ) ;

void m_shortref(
#if defined(M_PROTO)
  int context
#endif
  ) ;

void m_showcurelt(
#if defined(M_PROTO)
  M_NOPAR
#endif
  ) ;

void m_signmsg(
#if defined(M_PROTO)
  char *p
#endif
  ) ;

void m_sigre(
#if defined(M_PROTO)
  M_NOPAR
#endif
  ) ;

void m_startdoc(
#if defined(M_PROTO)
  M_NOPAR
#endif
  ) ;

void m_startmsg(
#if defined(M_PROTO)
  M_NOPAR
#endif
  ) ;

void m_stcaction(
#if defined(M_PROTO)
  M_WCHAR *m_string, LOGICAL M_START, LOGICAL M_END
#endif
  ) ;

void m_stcomplete(
#if defined(M_PROTO)
  M_NOPAR
#endif
  ) ;

void m_stkdefaultparams(
#if defined(M_PROTO)
  M_NOPAR
#endif
  ) ;

void m_stkonedef(
#if defined(M_PROTO)
  int par, M_ELEMENT scanel, M_WCHAR **poccur, int i
#endif
  ) ;

void m_stkparams(
#if defined(M_PROTO)
  M_NOPAR
#endif
  ) ;

void m_strtaction(
#if defined(M_PROTO)
  M_ELEMENT m_elt
#endif
  ) ;

void m_strtcase(
#if defined(M_PROTO)
  int m_action
#endif
  ) ;

void m_strtcdata(
#if defined(M_PROTO)
  int scanval
#endif
  ) ;

LOGICAL m_strtproc(
#if defined(M_PROTO)
  M_ELEMENT scanval
#endif
  ) ;

void m_strttag(
#if defined(M_PROTO)
  M_ELEMENT val, LOGICAL net
#endif
  ) ;

void m_textaction(
#if defined(M_PROTO)
  M_WCHAR m_textchar
#endif
  ) ;

void m_textout(
#if defined(M_PROTO)
  char *format, LOGICAL start, LOGICAL end
#endif
  ) ;

LOGICAL m_textpermitted(
#if defined(M_PROTO)
  M_NOPAR
#endif
  ) ;

M_WCHAR *m_thisfile(
#if defined(M_PROTO)
  M_NOPAR
#endif
  ) ;

int m_thisline(
#if defined(M_PROTO)
  M_NOPAR
#endif
  ) ;

void m_trace(
#if defined(M_PROTO)
  char *p
#endif
  ) ;

void m_wctrace(
#if defined(M_PROTO)
  M_WCHAR *p
#endif
  ) ;

LOGICAL m_transition(
#if defined(M_PROTO)
  M_ELEMENT label, LOGICAL recur
#endif
  ) ;

void m_undodelim(
#if defined(M_PROTO)
  M_WCHAR *delim, LOGICAL flag
#endif
  ) ;

void m_ungetachar(
#if defined(M_PROTO)
  int c, M_HOLDTYPE dchar, LOGICAL preread
#endif
  ) ;

void m_updatedefault(
#if defined(M_PROTO)
  const int par, const M_WCHAR *string
#endif
  ) ;

int m_wcupstrcmp(
#if defined(M_PROTO)
  const M_WCHAR *p, const M_WCHAR *q
#endif
  ) ;

int m_wcmbupstrcmp(
#if defined(M_PROTO)
  const M_WCHAR *p, const char *q
#endif
  ) ;

int m_mbmbupstrcmp(
#if defined(M_PROTO)
  const char *p, const char *q
#endif
  ) ;

LOGICAL m_validinpar(
#if defined(M_PROTO)
  M_WCHAR *string
#endif
  ) ;

LOGICAL m_vldentref(
#if defined(M_PROTO)
  M_NOPAR
#endif
  ) ;

LOGICAL m_whitespace(
#if defined(M_PROTO)
  M_WCHAR c
#endif
  ) ;

