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
/* $XConsortium: proto.h /main/3 1995/11/08 11:30:37 rswiston $ */
int m_actgetc(void);

void m_adjuststate(void);

LOGICAL m_allwhite(const M_WCHAR *string);

void m_attval(M_WCHAR *string);

LOGICAL m_attvonly(M_WCHAR *string);

int m_checkstart(M_ELEMENT val);

LOGICAL m_ckend(M_ELEMENT val, LOGICAL neednet);

void m_ckmap(M_WCHAR *name, LOGICAL useoradd);

void m_closent(void *m_ptr);

void m_codeent(int m_ent);

M_PARSE *m_copystackelt(void);

M_WCHAR *m_cyclent(LOGICAL init, unsigned char *type, M_WCHAR **content, 
  unsigned char *wheredef);

void m_dispcurelt(M_WCHAR *file, int line);

void m_done(void);

void m_dumpline(M_WCHAR *file, int line);

void m_eduptype(int type);

M_ELEMENT m_eltname(void);

void m_endaction(M_ELEMENT m_elt);

void m_endcase(int m_action);

void m_endtag(M_ELEMENT c);

void m_entexpand(M_ENTITY *openent);

void m_eprefix(void);

void m_err1(const char *text, const M_WCHAR *arg);

void m_err2(const char *text, const M_WCHAR *arg1, const M_WCHAR *arg2);

void m_err3(char *text, M_WCHAR *arg1, M_WCHAR *arg2, M_WCHAR *arg3);

void m_err4(char *text, M_WCHAR *arg1, M_WCHAR *arg2, M_WCHAR *arg3, M_WCHAR *arg4);

void m_err5(char *text, M_WCHAR *arg1, M_WCHAR *arg2, M_WCHAR *arg3,
	      M_WCHAR *arg4, M_WCHAR *arg5);

void m_err6(char *text, M_WCHAR *arg1, M_WCHAR *arg2, M_WCHAR *arg3,
	      M_WCHAR *arg4, M_WCHAR *arg5, M_WCHAR *arg6);

void m_err7(char *text, M_WCHAR *arg1, M_WCHAR *arg2, M_WCHAR *arg3,
	      M_WCHAR *arg4, M_WCHAR *arg5, M_WCHAR *arg6, M_WCHAR *arg7);

void m_errline(char *p);

void m_error(char *text);

void m_esuffix(void);

void m_exit(int status);

void m_etcomplete(void);

LOGICAL m_excluded(M_ELEMENT elt);

void m_expecting(void);

void m_expexpand(LOGICAL *expstart, M_STATE node, LOGICAL *required, LOGICAL *data);

void m_expline(LOGICAL *expstart, LOGICAL *data, M_ELEMENT label);

void m_exptend(LOGICAL *expstart, M_PARSE *stackptr);

int m_findact(M_ELEMENT elt, int *array);

void m_findatt(void);

void m_findchain(M_PARSE *stackptr, int start, int chainin, int *chainout, int *index, 
  LOGICAL wild);

int m_findpar(const char *elt, const char *param, const M_WCHAR *value);

M_ELEMENT m_findunique(M_STATE from, int *newleft);

void m_frcend(M_ELEMENT val);

void m_frcstart(void);

void m_free(void *block, char *msg);

void m_freeFSA(M_PARSE *stackelt);

void m_freemin(M_MIN *min, char *msg);

void m_freeparam(M_PARSE *stackelt);

LOGICAL m_gendelim(int srlen, int context);

int m_getachar(M_HOLDTYPE *dchar);

int m_getc(void *m_ptr);

int mb_getwc(void *m_ptr);

void *m_getdata(int n, LOGICAL *flag);

void m_getline(M_WCHAR **file, int *line);

void m_getname(M_WCHAR first);

void m_getsignon(void);

int m_gettoken(int *c, M_HOLDTYPE *dchar, int context);

void m_globes(void);

void m_globss(void);

void m_holdproc(void);

void m_inctest(int *count, int limit, char *message);

void m_initctype(void);

void m_initialize(void);

void m_lastchars(void);

int m_level(M_WCHAR *elt);

int m_mblevel(char *elt);

void m_litproc(int delim);

void m_longent(int context);

LOGICAL m_lookent(M_WCHAR *name, unsigned char *type, M_WCHAR **content, 
  unsigned char *wheredef);

void main(int argc, char **argv);

void *m_malloc(int size, char *msg);

void m_missingtagc(int c, M_HOLDTYPE dchar, LOGICAL start);

LOGICAL m_nextand(M_OPENFSA *thisfsa, M_ELEMENT label);

void m_nextdelimchar(int *n, int i, LOGICAL *linestart, LOGICAL newlinestart, LOGICAL skipblank, 
  unsigned char type);

void m_ntrent(M_WCHAR *p);

void m_nullendtag(void);

LOGICAL m_omitend(void);

LOGICAL m_omitstart(void);

void m_openchk(FILE **ptr, char *name, char *mode);

void *m_openent(M_WCHAR *entcontent);

void *m_openfirst(void);

void m_optstring(char *p);

int m_packedlook(M_PTRIE *xptrie, M_WCHAR *name);

int m_parcount(M_WCHAR *elt);

M_WCHAR *m_pardefault(M_WCHAR *elt, M_WCHAR *param, unsigned char *type);

M_WCHAR *m_parent(int n);

M_WCHAR *m_parname(M_WCHAR *elt, int n);

M_WCHAR *m_mbparname(char *elt, int n);

const M_WCHAR *m_partype(const int par, const M_WCHAR *string);

void m_parupper(int par, M_WCHAR *string);

LOGICAL m_parvalok(M_WCHAR *elt, M_WCHAR *param, const M_WCHAR *value);

void m_piaction(M_WCHAR *m_pi, M_WCHAR *m_entname, int m_enttype);

void m_pop(void);

void m_push(M_ELEMENT elt, M_STATE current, LOGICAL need);

LOGICAL m_putdata(void *data, int n);

void m_readcomments(void);

void *m_realloc(void *ptr, int size, char *msg);

int m_scan(void);

void m_setmap(int map, LOGICAL useoradd);

void m_setoptions(void);

void m_setparam(M_WCHAR **cvar, int par);

void m_shortref(int context);

void m_showcurelt(void);

void m_signmsg(char *p);

void m_sigre(void);

void m_startdoc(void);

void m_startmsg(void);

void m_stcaction(M_WCHAR *m_string, LOGICAL M_START, LOGICAL M_END);

void m_stcomplete(void);

void m_stkdefaultparams(void);

void m_stkonedef(int par, M_ELEMENT scanel, M_WCHAR **poccur, int i);

void m_stkparams(void);

void m_strtaction(M_ELEMENT m_elt);

void m_strtcase(int m_action);

void m_strtcdata(int scanval);

LOGICAL m_strtproc(M_ELEMENT scanval);

void m_strttag(M_ELEMENT val, LOGICAL net);

void m_textaction(M_WCHAR m_textchar);

void m_textout(char *format, LOGICAL start, LOGICAL end);

LOGICAL m_textpermitted(void);

M_WCHAR *m_thisfile(void);

int m_thisline(void);

void m_trace(char *p);

void m_wctrace(M_WCHAR *p);

LOGICAL m_transition(M_ELEMENT label, LOGICAL recur);

void m_undodelim(M_WCHAR *delim, LOGICAL flag);

void m_ungetachar(int c, M_HOLDTYPE dchar, LOGICAL preread);

void m_updatedefault(const int par, const M_WCHAR *string);

int m_wcupstrcmp(const M_WCHAR *p, const M_WCHAR *q);

int m_wcmbupstrcmp(const M_WCHAR *p, const char *q);

int m_mbmbupstrcmp(const char *p, const char *q);

LOGICAL m_validinpar(M_WCHAR *string);

LOGICAL m_vldentref(void);

LOGICAL m_whitespace(M_WCHAR c);

