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
/* $XConsortium: scan.c /main/3 1995/11/08 10:58:13 rswiston $ */
/*
              Copyright 1986 Tandem Computers Incorporated.
This product and information is proprietary of Tandem Computers Incorporated.
                   Copyright 1986, 1987, 1988, 1989 Hewlett-Packard Co.
*/

/* Scan.c is the scanner for program PARSER */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#if defined(MSDOS)
#include <process.h>
#endif
#include "basic.h"
#include "trie.h"
#define M_CONDEF
#include "context.h"
#define M_DELIMDEF
#include "delim.h"
#define M_DTDDEF
#include "dtd.h"
#include "arc.h"
#define M_PARDEF
#include "parser.h"
#define M_ENTDEF
#include "entity2.h"
#include "sref.h"

/* Actually read a character from an input stream */
int m_actgetc(M_NOPAR)
  {
    int c ;

    c = m_getc(m_sysent[m_sysecnt]) ;
    m_saveline[m_svlncnt[m_sysecnt]][m_sysecnt] = c ;
    if (++m_svlncnt[m_sysecnt] >= M_LINELENGTH) {
      m_svlncnt[m_sysecnt] = 0 ;
      m_svlnwrap[m_sysecnt] = TRUE ;
      }
    return(c) ;
    }

/* Expand an entity reference */
void m_entexpand(openent)
  M_ENTITY *openent ;
  {
    M_WCHAR *p ;
    M_HOLDTYPE dchar ;
    char buffer[10] ;
    int i ;

    m_ungetachar(M_NULLVAL, M_EE, FALSE) ;
    m_eopencnt++ ;
    m_opene[m_eopencnt - 1] = openent ;

    if (m_stacktop->element &&
	m_element[m_stacktop->element - 1].content == M_RCDATA)
      m_curcon = RCNEWENT ;
    if (m_curcon == LITCON || m_curcon == LITACON)
      m_curcon = ENTINLIT ;
    if (! openent->wheredef) {
      m_eopencnt-- ;
      m_err1("%s: System error -- no definition for predeclared entity",
             openent->name) ;
      m_eopencnt++ ;
      return ;
      }
    if (m_curcon == ENTINLIT)
      if (openent->type != M_GENERAL) {
        m_eopencnt-- ;
        m_err1("%s: Typed entity not allowed in parameter value",
               openent->name) ;
        m_eopencnt++ ;
        return ;
        }
    if (m_eopencnt > M_ENTLVL) {
      m_eopencnt-- ;
      m_err1("%s: Too many nested entities", openent->name) ;
      m_eopencnt++ ;
      return ;
      }
    for (i = 0 ; i < m_eopencnt - 1; i++)
      if (m_opene[i] == openent) {
        m_eopencnt-- ;
        m_err1("Recursive call to entity %s ignored", openent->name) ;
        m_eopencnt++ ;
        return ;
        }

    /* If SDATA or PI entity (regular or CODE) at beginning of document
       instance, call m_startdoc and reset m_curcon past preamble */
    if (m_curcon == PREAMBLE &&
        (openent->type == M_SDATA ||
         openent->type == M_CODESDATA ||
         openent->type == M_PI ||
         openent->type == M_CODEPI)) {
      m_startdoc() ;
      m_curcon = START ;
      m_adjuststate() ;
      }

    /* SDATA entity */
      if (openent->type == M_SDATA || openent->type == M_CODESDATA) {
        if (! m_stacktop->intext) {
          if (! m_strtproc(M_NULLVAL)) {
            if (m_stacktop->oldtop)
              m_err1("SDATA entity not allowed at this point in %s",
                     m_nameofelt(m_stacktop->element)) ;
            else if (! m_start)
              m_error("Document may not start with SDATA entity") ;
            }
          m_start = TRUE ;
          m_stacktop->firstre = TRUE ;
          m_stacktop->intext = TRUE ;
          if (m_curcon == ELCON || m_curcon == DATACON)
            m_curcon = POUNDCDATA ;
          else if (m_curcon == NETELCON || m_curcon == NETDATACON)
            m_curcon = NETCDATA ;
          }
        m_stacktop->linestat = M_DCORCET ;
        m_holdproc() ;
        }

    /* CODE entity */
    if (openent->type == M_CODEPI || openent->type == M_CODESDATA) {
      if (openent->type == M_CODEPI)
        m_stacktop->linestat = M_SOMETHING ;
      m_codeent(openent->codeindex) ;
      return ;
      }

    /* PI or SDATA, but not CODE entity */
    if (openent->type == M_PI || openent->type == M_SDATA) {
      m_piaction(openent->content, openent->name, openent->type) ;
      return ;
      }

    /* Subordinate data file */
    if (openent->type == M_SYSTEM) {
      m_sysent[m_sysecnt + 1] = m_openent(openent->content) ;
      if (m_sysent[m_sysecnt + 1]) {
        m_sysecnt++ ;
        m_line[m_sysecnt] = 1 ;
        m_svlncnt[m_sysecnt] = 0 ;
        m_svlnwrap[m_sysecnt] = FALSE ;
        if (m_chtrace) {
          m_trace("Opening `") ;
          m_wctrace(openent->content) ;
          m_trace("'(") ;
          sprintf(buffer, "%d", m_sysecnt) ;
          m_trace(buffer) ;
          m_trace(")\n") ;
          }
        return ;
        }
      m_eopencnt-- ;
      m_err1("Unable to open file %s", openent->content) ;
      m_eopencnt++ ;
      return ;
      }

    /* An entity reference has been encountered.  Put the content of the
       entity, including any leading or trailing delimiters into the input
       stream in reverse order */
    /* Closing delimiter */
    switch (openent->type) {
      case M_STARTTAG:
      case M_ENDTAG: {
        m_undodelim(m_dlmptr[M_TAGC - 1], FALSE) ;
        break ;
	}
      case M_MD: {
        m_undodelim(m_dlmptr[M_MDC - 1], FALSE) ;
        break ;
	}
      default:
        break ;
      }
    /* Content of entity -- scan for end to reverse string */
    if (openent->type == M_CDATAENT) dchar = M_CDCHAR ;
    else dchar = M_ENTNORMAL ;
    if (p = openent->content)
      while (*p) p++;
    if (p != openent->content) {
      p-- ;
      while (TRUE) {
        m_ungetachar((int) *p, dchar, FALSE) ;
        if (p == openent->content) break ;
        p-- ;
        }
      }
    /* Opening delimiter */
    switch (openent->type) {
      case M_STARTTAG: {
        m_undodelim(m_dlmptr[M_STAGO - 1], FALSE) ;
        break ;
	}
      case M_ENDTAG: {
        m_undodelim(m_dlmptr[M_ETAGO - 1], FALSE) ;
        break ;
	}
      case M_MD: {
        m_undodelim(m_dlmptr[M_MDO - 1], FALSE) ;
        break ;
	}
      default:
        break ;
      }
    } /* End m_entexpand */

/* An srlen-character long short-reference delimiter has been found.  Verify
   that it is not the prefix of a general delimiter recognized in context*/
LOGICAL m_gendelim(srlen, context)
  int srlen ;
  int context ;
  {
    int ghold[MAXD + 1] ;
    int ucase ;
    int next ;
    int i, n = 0, current, delim[MAXD + 1], oldchars = 0 ;
    int newcharstart = 0 ;
    M_HOLDTYPE dhold[MAXD + 1], dchar ;
    LOGICAL linestart ;
    LOGICAL found ;

    if (! (current = m_contree[context - 1])) return(FALSE) ;
    linestart = TRUE ;
    for (i = 0 ; i <= srlen ; i++)
      if (m_srefchartype[i] != M_RSCHAR && m_srefchartype[i] != M_WSCHAR) {
        linestart = FALSE ;
        break ;
      }
    if (linestart) return(FALSE) ;

    current-- ;
    while (TRUE) {
      delim[n] = FALSE ;
      while (oldchars <= srlen && 
             (m_srefchartype[oldchars] == M_RSCHAR ||
              m_srefchartype[oldchars] == M_WSCHAR))
        oldchars++ ;
      if (oldchars <= srlen)
        ucase = m_hold[oldchars++] ;
      else {
        if (! newcharstart) newcharstart = n ;
        ghold[n] = m_getachar(&dhold[n]) ;
        ucase = m_ctupper(ghold[n]) ;
        if (dhold[n] != M_NORMAL && dhold[n] != M_ENTNORMAL) break ;
        }
      for (i = current ;
           (int) m_delimtrie[i].symbol < ucase && m_delimtrie[i].more ;
           i++) ;
      if ((int) m_delimtrie[i].symbol == ucase) {
        current = m_delimtrie[i].index ;
        if (! m_delimtrie[current].symbol)
          delim[n] = m_delimtrie[current].index ;
        n++ ;
        }
      else break ;
      }

    if (! newcharstart) return(FALSE) ;
    while (n >= newcharstart - 1) {
      found = FALSE ;
      if (delim[n]) {
        /* Found a delimiter. If it ends with a letter, verify
           that the following character is not a letter, in order
           to issue error messages in cases such as <!ENTITYrunon ... */
        if (m_cttype(ghold[n]) != M_NMSTART) found = TRUE ;
        else {
          next = m_getachar(&dchar) ;
          m_ungetachar(next, dchar, TRUE) ;
          if (next == EOF || m_cttype(next) != M_NMSTART)
            found = TRUE ;
          }
        }
      if (found) {
        if (delim[n] == M_ERO || delim[n] == M_STAGO ||
            delim[n] == M_ETAGO) {
          next = m_getachar(&dchar) ;
          m_ungetachar(next, dchar, TRUE) ;
          if (! (m_cttype(next) == M_NMSTART &&
                 (dchar == M_NORMAL || dchar == M_ENTNORMAL))) {
            n-- ;
            continue ;
            }
          }
        while (n >= newcharstart) {
          m_ungetachar(ghold[n], dhold[n], TRUE) ;
          n-- ;
          }
        return(TRUE) ;
        } /* End if delim[n] */
      if (n >= newcharstart) m_ungetachar(ghold[n], dhold[n], TRUE) ;
      n-- ;
      }

    return(FALSE) ;
    }

/* Reads next input character from the current source file or from an
   entity expansion */
int m_getachar(dchar)
  M_HOLDTYPE *dchar ;
  {
    int c ;
    int i ;
    char buffer[10] ;
    int length;
    M_WCHAR wc_ee, wc_re;
    char    mb_ee, mb_re;

    mb_ee = M_EE;
    mb_re = M_RE;
    mbtowc(&wc_ee, &mb_ee, 1);
    mbtowc(&wc_re, &mb_re, 1);
    if (m_toundo && m_sysecnt <= m_sourcefile[m_toundo - 1]) {
      c = m_savechar[--m_toundo] ;
      *dchar = m_savedchar[m_toundo] ;
      if (*dchar == wc_ee) m_atrs = (M_WCHAR) c;
      }
    else {
      c = m_actgetc() ;
      *dchar = M_NORMAL ;
      if (m_whitespace((M_WCHAR) c) && c != wc_re) {
        /* White space, but not RE, i.e., space or tab */
        for (m_wscount = 0 ; m_wscount < M_WSPACELEN ; m_wscount++) {
          m_wspace[m_wscount] = m_actgetc() ;
          if (! m_whitespace((M_WCHAR) m_wspace[m_wscount]) ||
              m_wspace[m_wscount] == wc_re)
            break ;
          }
        if (m_whitespace((M_WCHAR) m_wspace[m_wscount]) &&
            m_wspace[m_wscount] != wc_re) {
          m_error("Ignoring blank or tab") ;
          while (m_whitespace((M_WCHAR) m_wspace[m_wscount]) &&
                 m_wspace[m_wscount] != wc_re)
            m_wspace[m_wscount] = m_actgetc() ;
          }
        if (m_wscount > m_maxws) m_maxws = m_wscount ;
        if (m_wspace[m_wscount] == wc_re) c = wc_re ;
        else {
          for (i = 0 ; i <= m_wscount ; i++)
            m_ungetachar(m_wspace[m_wscount - i], M_NORMAL, FALSE) ;
          }
        } /* End just read a blank or tab, is it line-trailing? */
      } /* End read a character from file */

    m_oldlsindex = (m_oldlsindex + 1) % M_SAVECHAR ;
    m_oldlinestat[m_oldlsindex] = m_stacktop->linestat ;
    m_oldatrs[m_oldlsindex] = m_atrs ;
    if (c == wc_re && *dchar) {
      if (*dchar == M_NORMAL) m_line[m_sysecnt]++ ;
      m_stacktop->linestat = M_NOTHING ;
      m_atrs = TRUE ;
      }
    else if (*dchar) m_atrs = FALSE ;
    if (m_chtrace) {
      if (*dchar) {
        m_trace("get(") ;
	length = wctomb(buffer, c);
	if (length != -1)
	    {
	    buffer[length] = 0;
	    m_trace(buffer) ;
	    }
        m_trace(")[") ;
        sprintf(buffer, "%d", c) ;
        m_trace(buffer) ;
        m_trace("],") ;
        sprintf(buffer, "%d", *dchar) ;
        m_trace(buffer) ;
        m_trace("\n") ;
        }
      else m_trace("get(EE)\n") ;
      }
    return(c) ;
    }

/* Reads a name token */
#if defined(M_PROTO)
void m_getname(M_WCHAR first)
#else
void m_getname(first)
  M_WCHAR first ;
#endif
{
    M_WCHAR *p ;
    M_HOLDTYPE dchar ;
    int c ;

    *(p = m_name) = first ;
    while (TRUE) {
      c = m_getachar(&dchar) ;
      if (c == EOF) break ;
      if (dchar != M_NORMAL && dchar != M_ENTNORMAL) break ;
      if (m_cttype(c) == M_NONNAME) break ;
      *++p = (M_WCHAR) c ;
      if (p >= m_name + M_NAMELEN) {
        p-- ;
        m_error("Name too long") ;
        while ((dchar == M_NORMAL || dchar == M_ENTNORMAL) &&
               c != EOF &&
               m_cttype(c) != M_NONNAME)
          c = m_getachar(&dchar) ;
        break ;
        }
      }
    m_ungetachar(c, dchar, TRUE) ;
    *++p = M_EOS ;
    }

/* Reads the next token */
int m_gettoken(c, dchar, context)
  int *c ;
  M_HOLDTYPE *dchar ;
  int context ;
  {
    int hold[MAXD + 1], next ;
    int ucase ;
    int i, n = 0, current, delim[MAXD + 1], nexttoken ;
    M_HOLDTYPE dhold[MAXD + 1] ;
    LOGICAL found ;

    switch (context) {
      case DATACON:
      case NETDATACON:
      case POUNDCDATA:
      case NETCDATA:
      case ELCON:
      case NETELCON:
        if (m_stacktop->oldtop) m_shortref(context) ;
        break ;
      default:
        break ;
      }
    if (! (current = m_contree[context - 1])) {
      *c = m_getachar(dchar) ;
      return(M_NULLVAL) ;
      }
    current-- ;
    while (TRUE) {
      hold[n] = m_getachar(&dhold[n]) ;
      ucase = m_ctupper(hold[n]) ;
      delim[n] = FALSE ;
      if (dhold[n] != M_NORMAL && dhold[n] != M_ENTNORMAL) break ;
      for (i = current ;
           (int) m_delimtrie[i].symbol < ucase && m_delimtrie[i].more ;
           i++) ;
      if ((int) m_delimtrie[i].symbol == ucase) {
        current = m_delimtrie[i].index ;
        if (! m_delimtrie[current].symbol)
          delim[n] = m_delimtrie[current].index ;
        n++ ;
        }
      else break ;
      }

    while (n >= 0) {
      found = FALSE ;
      if (delim[n]) {
        /* Found a delimiter. If it ends with a letter, verify
           that the following character is not a letter, in order
           to issue error messages in cases such as <!ENTITYrunon ... */
        if (m_cttype(hold[n]) != M_NMSTART) found = TRUE ;
        else {
          *c = m_getachar(dchar) ;
          m_ungetachar(*c, *dchar, TRUE) ;
          if (*c == EOF || m_cttype(*c) != M_NMSTART) found = TRUE ;
          }
        }
      if (found) {
        if (delim[n] == M_CRO) {
          next = m_getachar(dchar) ;
          if ((*dchar != M_NORMAL && *dchar != M_ENTNORMAL) ||
              (m_cttype(next) != M_DIGIT)) 
            m_ungetachar(next, *dchar, TRUE) ;
          else {
            m_scanval = next - '0' ;
            while (TRUE) {
              next = m_getachar(dchar) ;
              if ((*dchar != M_NORMAL && *dchar != M_ENTNORMAL) ||
                  (m_cttype(next) != M_DIGIT)) {
                m_ungetachar(next, *dchar, TRUE) ;
                if (! m_gettoken(&next, dchar, ENTREF))
                  m_ungetachar(next, *dchar, TRUE) ;
                if (context == ELCON || context == NETELCON)
                  return(M_BLACKSPACE) ;
                else return(M_TEXT) ;
                }
              m_scanval = 10 * m_scanval + next - '0' ;
              if (m_scanval >= M_CHARSETLEN) {
                m_error("Invalid character code") ;
                m_scanval = (m_scanval - next + '0') / 10 ;
                m_ungetachar(next, *dchar, TRUE) ;
                if (context == ELCON || context == NETELCON)
                  return(M_BLACKSPACE) ;
                else return(M_TEXT) ;
                }
              } /* End loop reading digits after M_CRO */
            } /* End M_CRO followed by digit */
          } /* End delim[n] == M_CRO */
        else if (delim[n] == M_ERO) 
          if (m_vldentref())
            return(m_gettoken(c, dchar,
                            (m_curcon == RCNEWENT || m_curcon == ENTINLIT) ?
                              m_curcon : context)) ;
        /* Can be an M_ERO or M_CRO here only if not in context and hence
           should not be treated as a delimiter */
        if (delim[n] != M_STAGO && delim[n] != M_ETAGO &&
            delim[n] != M_ERO && delim[n] != M_CRO)
          return(delim[n]) ;
        /* M_STAGO and M_ETAGO recognized only if immediately followed by
           a M_NMSTART character or by an appropriate closing delimiter
           (latter is a short tag) */
        if (delim[n] == M_STAGO || delim[n] == M_ETAGO) {
          next = m_getachar(dchar) ;
          m_ungetachar(next, *dchar, TRUE) ;
          if (m_cttype(next) == M_NMSTART &&
              (*dchar == M_NORMAL || *dchar == M_ENTNORMAL))
            return(delim[n]) ;
          nexttoken = m_gettoken(&next, dchar,
            delim[n] == M_STAGO ? SELEMENT : EELEMENT) ;
          if (nexttoken) {
            m_undodelim(m_dlmptr[nexttoken - 1], TRUE) ;
            return(delim[n]) ;
            }
          else m_ungetachar(next, *dchar, TRUE) ;
          } /* End delim[n] is M_STAGO or M_ETAGO */
        } /* End if (delim[n]) */
      if (n) m_ungetachar(hold[n], dhold[n], TRUE) ;
      n-- ;
      }

    *c = *hold ;
    *dchar = *dhold ;
    return(M_NULLVAL) ;
    }

/* Reads a literal */
void m_litproc(delim)
  int delim ;
  {
    int n, i ;
    M_HOLDTYPE dchar ;
    int savecon = m_curcon ;
    int c ;
    int atentlev ;
    int atdelimcon ;
    char mb_re, mb_tab, mb_space, mb_null, mb_ee;
    M_WCHAR wc_re, wc_tab, wc_space, wc_null, wc_ee;

    mb_re = M_RE;
    mb_tab = M_TAB;
    mb_space = M_SPACE;
    mb_null = M_NULLVAL;
    mb_ee = M_EE;
    mbtowc(&wc_re, &mb_re, 1);
    mbtowc(&wc_tab, &mb_tab, 1);
    mbtowc(&wc_space, &mb_space, 1);
    mbtowc(&wc_null, &mb_null, 1);
    mbtowc(&wc_ee, &mb_ee, 1);

    m_curcon = delim == M_LIT ? LITCON : LITACON ;
    atentlev = m_eopencnt ;
    atdelimcon = m_curcon ;
    for (i = 0 ; i < M_LITLEN + 1 ; i++) {
      n = m_gettoken(&c, &dchar, m_curcon) ;
      switch (n) {
        case M_ENDFILE:
          m_ungetachar(c, dchar, TRUE) ;
          m_literal[i] = wc_null ;
          m_curcon = savecon ;
          return ;
        case M_TEXT:
          m_literal[i] = (M_WCHAR) m_scanval ;
          break ;
        case M_LIT:
        case M_LITA:
          m_literal[i] = wc_null ;
          m_curcon = savecon ;
          return ;
        case M_LITRS:
        case M_LITSCR:
          break ;
        case M_LITRE:
        case M_LITECR:
          m_literal[i] = wc_re ;
          break ;
        case M_LITSPACE:
        case M_LITCSPACE:
          m_literal[i] = wc_space ;
          break ;
        case M_LITTAB:
        case M_LITCTAB:
          m_literal[i] = wc_tab ;
          break ;
        case M_NULLVAL:
          m_literal[i] = (M_WCHAR) c ;
          if (dchar == wc_ee) {
            if (m_curcon == ENTINLIT) {
              m_eopencnt-- ;
              i-- ;
              if (m_eopencnt == atentlev) {
                m_curcon = atdelimcon ;
                break ;
                }
              }
            else {
              m_literal[i] = wc_null ;
              m_curcon = savecon ;
              m_ungetachar(wc_null, wc_ee, FALSE) ;
              return ;
              }
            }
          break ;
        default:
          m_error("Internal error processing literal") ;
          break ;
        }
      } /* End for i */
    m_error("Literal too long") ;
    m_literal[i] = wc_null ;
    m_curcon = savecon ;
    }

/* Called when a missing tagc delimiter is detected */
#if defined(M_PROTO)
void m_missingtagc(int c, M_HOLDTYPE dchar, LOGICAL start)
#else
void m_missingtagc(c, dchar, start)
  int c ;
  M_HOLDTYPE dchar ;
  LOGICAL start ;
#endif
{
    if (! m_wholetag) {
      if (start) m_mberr1("Invalid parameter or missing %s", m_tagc);
      else m_mberr1("Missing %s in end-tag", m_tagc) ;
      }
    m_ungetachar(c, dchar, TRUE) ;
    m_curcon = START ;
    m_adjuststate() ;
    }

/* Have found one character in a possible short reference delimiter.
   Prepare to look for the next one */
#if defined(M_PROTO)
void m_nextdelimchar(int *n, int i, LOGICAL *linestart, LOGICAL newlinestart,
		     LOGICAL skipblank, unsigned char type)
#else
void m_nextdelimchar(n, i, linestart, newlinestart, skipblank, type)
  int *n ;
  int i ;
  LOGICAL *linestart ;
  LOGICAL newlinestart ;
  LOGICAL skipblank ;
  unsigned char type ;
#endif
{               
    int k ;
    char mb_re,mb_seqchar, mb_rschar;
    M_WCHAR wc_re,wc_seqchar, wc_rschar;

    mb_re = M_RE;
    mbtowc(&wc_re, &mb_re, 1);
    mb_seqchar = M_SEQCHAR;
    mbtowc(&wc_seqchar, &mb_seqchar, 1);
    mb_rschar = M_RSCHAR;
    mbtowc(&wc_rschar, &mb_rschar, 1);
    m_current[*n + 1] = m_sreftree[i].index ;
    if (! m_sreftree[m_current[*n + 1]].symbol)
      m_delim[*n] = m_sreftree[m_current[*n + 1]].index ;
    *linestart = newlinestart ;
    m_srefchartype[*n] = type ;
    if (skipblank) {
      for (k = 0 ; k < M_BSEQLEN ; k++) {
        m_hold[*n + 1 + k] = m_getachar(&m_dhold[*n + 1 + k]) ;
        if (m_hold[*n + 1 + k] != ' ' && m_hold[*n + 1 + k] != '\t') {
          m_ungetachar(m_hold[*n + 1 + k], m_dhold[*n + 1 + k], TRUE) ;
          break ;
          }
        m_current[*n + 1 + k + 1] = m_current[*n + 1] ;
        m_delim[*n + 1 + k] = m_delim[*n] ;
        m_srefchartype[*n + 1 + k] = wc_seqchar ;
        }
      *n += k + 1 ;
      }
    else (*n)++ ;
    m_srefchartype[*n] = wc_rschar ;
    }

/* Scans past a comment within a markup declaration */
void m_readcomments(M_NOPAR)
  {
    int c ;
    M_HOLDTYPE dchar ;

    while (! m_gettoken(&c, &dchar, COMCON))
      if (c == EOF) {
        m_error("Document ended within a comment") ;
        m_done() ;
        }
    }

/* Scanner */
int m_scan(M_NOPAR)
  {
    int c ;
    M_HOLDTYPE dchar ;
    int n ;
    char buffer[10] ;
    char mb_ee, mb_re, mb_space, mb_tab;
    M_WCHAR wc_ee, wc_re, wc_space, wc_tab;

    mb_ee = M_EE;
    mbtowc(&wc_ee, &mb_ee, 1);
    mb_re = M_RE;
    mbtowc(&wc_re, &mb_re, 1);
    mb_space = M_SPACE;
    mbtowc(&wc_space, &mb_space, 1);
    mb_tab = M_TAB;
    mbtowc(&wc_tab, &mb_tab, 1);
    while (TRUE) {
      n = m_gettoken(&c, &dchar, m_curcon) ;
      if (n) {
        if (n != M_ENTITYEND && m_stacktop->linestat == M_NOTHING)
          m_stacktop->linestat = M_SOMETHING ;
        switch (n) {
          case M_LITRS:
          case M_LITSCR:
            m_atrs = TRUE ;
            continue ;
          case M_LITRE:
          case M_LITECR:
            m_ungetachar(wc_re, M_ENTNORMAL, FALSE) ;
            continue ;
          case M_LITSPACE:
          case M_LITCSPACE:
            m_ungetachar(wc_space, M_ENTNORMAL, FALSE) ;
            continue ;
          case M_LITTAB:
          case M_LITCTAB:
            m_ungetachar(wc_tab, M_ENTNORMAL, FALSE) ;
            continue ;
          case M_LIT:
          case M_LITA:
            m_litproc(n) ;
            return(M_LITERAL) ;
          default:
            return(n) ;
          }
        }
      /* Check for Entity End */
      if (dchar == wc_ee) {
        m_eopencnt-- ;
        if (m_stacktop->element &&
	    m_element[m_stacktop->element - 1].content == M_RCDATA) {
          if (m_eopencnt == m_stacktop->thisent) {
            if (m_netlevel) m_curcon = NETRCDATA ;
            else m_curcon = RCDATAEL ;}
          else if (m_eopencnt < m_stacktop->thisent)
            m_stacktop->thisent = m_eopencnt ;
          }
        if (m_newcon(m_curcon - 1, M_ENTITYEND - 1)) return(M_ENTITYEND) ;
        continue ;
        }
      /* Whitespace character--check if could be data.  If so,
         if it's a RE, check if its significant */
      if (m_whitespace((M_WCHAR) c)) {
        if (! m_newcon(m_curcon - 1, M_TEXT - 1)) continue ;
        if (c != wc_re || m_curcon == PROCINT || m_curcon == LITCON ||
                       m_curcon == LITENT || m_curcon == LITAENT) {
          m_scanval = c ;
          return(M_TEXT) ;
          }
        m_sigre() ;
        continue ;
        }
      if (c == EOF) {
        if (m_sysecnt) {
          m_closent(m_sysent[m_sysecnt--]) ;
          if (m_chtrace) {
            m_trace("Closing to level ") ;
            sprintf(buffer, "%d", m_sysecnt) ;
            m_trace(buffer) ;
            m_trace("\n") ;
            }
          continue ;
          }
        return(M_ENDFILE) ;
        }
      if (
          ((m_curcon == SELEMENT ||
            m_curcon == EELEMENT ||
            m_curcon == ENTNAME ||
            m_curcon == MAPNAME ||
            m_curcon == AMAPNAME)
            && m_cttype(c) == M_NMSTART) ||
          ((m_curcon == ATTNAME || m_curcon == ATTVAL ||
              m_curcon == NEEDVI) &&
            m_cttype(c) != M_NONNAME)
          ){
        m_getname((M_WCHAR) c) ;
        return(M_NAME) ;
        }
      switch (m_curcon) {
        case ATTVAL:
          m_err1("Expecting value for %s", 
                 &m_pname[m_parameter[m_ppsave - 1].paramname]) ;
          m_stcomplete() ;
          m_missingtagc(c, dchar, TRUE) ;
          continue ;
        case ATTNAME:
          m_stcomplete() ;
          m_missingtagc(c, dchar, TRUE) ;
          continue ;
        case NEEDVI:
          m_attvonly(m_saveatt) ;
          m_stcomplete() ;
          m_missingtagc(c, dchar, TRUE) ;
          continue ;
        case ETAGEND:
          if (! m_stacktop->oldtop)
            m_scanel = m_arc[m_state[0].first - 1].label ;
          else m_scanel = m_stacktop->element ;
          m_stacktop->holdre = FALSE ;
          m_etcomplete() ;
          m_missingtagc(c, dchar, FALSE) ;
          continue ;
        default:
          break ;
        }
      m_scanval = c ;
      if (! m_newcon(m_curcon - 1, M_TEXT - 1)) return(M_BLACKSPACE) ;
      return(M_TEXT) ;
      } /* End while */
    } /* End scan */


/* Process explicit or implied USEMAP or ADDMAP */
#if defined(M_PROTO)
void m_setmap(int map, LOGICAL useoradd)
#else
void m_setmap(map, useoradd)
  int map ;
  LOGICAL useoradd ;
#endif
{
    int i ;
    int sref ;

    if (! m_stacktop->oldtop) {
      m_error("Program error: attempt to set map for empty stack") ;
      m_exit(TRUE) ;
      }

    /* #EMPTY map*/
    if (map == 1) {
      if (m_stacktop->map && m_stacktop->oldtop->map != m_stacktop->map)
        m_free(m_stacktop->map, "short reference map") ;
      /* Done, if USEMAP */
      if (useoradd) {
        m_stacktop->map = NULL ;
        return ;
        }
      /* <!ADDMAP #EMPTY> restores map from beginning of element */
      m_stacktop->map = m_stacktop->oldtop->map ;
      if (m_element[m_stacktop->element - 1].srefptr)
        m_setmap(m_element[m_stacktop->element - 1].srefptr,
                 (LOGICAL) m_element[m_stacktop->element - 1].useoradd) ;
      return ;
      }

    /* Allocate and initialize a new map if needed */
    if (! m_stacktop->map || m_stacktop->map == m_stacktop->oldtop->map) {
      m_stacktop->map =
        (int *) m_malloc(sizeof(int) * M_SREFCNT, "short reference map") ;
      for (i = 0 ; i < M_SREFCNT ; i++)
        if (! useoradd && m_stacktop->oldtop->map)
          m_stacktop->map[i] = m_stacktop->oldtop->map[i] ;
        else m_stacktop->map[i] = M_NULLVAL ;
      }
    /* Clear an old map if replacing it */
    else
      if (useoradd)
        for (i = 0 ; i < M_SREFCNT ; i++)
          m_stacktop->map[i] = M_NULLVAL ;

    /* Offset into m_map is 2, 1 for 0-based indexing, 1 for #EMPTY code */
    for (sref = m_map[map - 2] ; sref ; sref = m_sref[sref - 1].next)
       m_stacktop->map[m_sref[sref - 1].sref - 1] = m_sref[sref - 1].entity ;
    }

/* Check for short reference delimiters */
void m_shortref(context)
int context ;
{
int n = 0 ;
int i ;
int c ;
LOGICAL linestart = m_atrs ;
char mb_ee;
M_WCHAR wc_ee;

mb_ee = M_EE;
mbtowc(&wc_ee, &mb_ee, 1);

/* If no short references defined, don't try to match one */
if (sizeof(m_sreftree)/sizeof(M_PTRIE) == 1) return ;

/* Can return if using MARKUP extensions and no map is active */
if (! m_conform && ! m_stacktop->map) return ;

m_current[0] = 0 ;
m_srefchartype[0] = M_RSCHAR ;
while (TRUE)
    {
    /* Search through short reference delimiter tree */
    while (TRUE)
	{
	m_delim[n] = FALSE ;

	/* Look for RS */
	if (linestart && m_srefchartype[n] >= M_RSCHAR)
	    {
	    for (i = m_current[n] ;
	    m_sreftree[i].more && m_sreftree[i].symbol < RS ;
	    i++) ;
	    if (m_sreftree[i].symbol == RS)
		{
		m_nextdelimchar(&n, i, &linestart, FALSE, FALSE, M_RSCHAR) ;
		continue ;
		}
	    }

	/* Look for white space sequence */
	if (m_srefchartype[n] >= M_WSCHAR)
	    {
	    for (i = m_current[n] ;
	    m_sreftree[i].more && m_sreftree[i].symbol < WSSEQ ;
	    i++) ;
	    if (m_sreftree[i].symbol == WSSEQ)
		{
		m_nextdelimchar(&n, i, &linestart, FALSE, TRUE, M_WSCHAR) ;
		continue ;
		}
	    }

	/* Look at next character from input stream */
	m_hold[n] = m_getachar(&m_dhold[n]) ;
	if (m_dhold[n] == wc_ee ||
	(m_dhold[n] != M_NORMAL && m_dhold[n] != M_ENTNORMAL))
	    {
	    m_srefchartype[n] = M_REGCHAR ;
	    break ;
	    }

	/* Look for blank sequence */
	if (m_srefchartype[n] >= M_BSCHAR &&
	(m_hold[n] == ' ' || m_hold[n] == '\t'))
	    {
	    for (i = m_current[n] ;
	    m_sreftree[i].more && m_sreftree[i].symbol < BLANKSEQ ;
	    i++) ;
	    if (m_sreftree[i].symbol == BLANKSEQ &&
	    (m_hold[n] == ' ' || m_hold[n] == '\t'))
		{
		m_nextdelimchar(&n, i, &linestart, FALSE, TRUE, M_BSCHAR) ;
		continue ;
		}
	    }

	/* Look for regular character */
	c = m_ctupper(m_hold[n]) ;
	if (m_cttype(c) != M_NMSTART)
	    {
	    for (i = m_current[n] ;
	         m_sreftree[i].more && (int) m_sreftree[i].symbol < c ;
	         i++) ;
	    if ((int) m_sreftree[i].symbol == c)
		{
		m_nextdelimchar(&n, i, &linestart, m_atrs, FALSE, M_REGCHAR) ;
		continue ;
		}
	    }

	m_srefchartype[n] = M_REGCHAR ;
	break ;
	} /* End search through sref delimiter tree */

    while (TRUE)
	{
	if (m_delim[n])
	    {
	    /* Found a delimiter. If letters were allowed in short references
	    would check here for runon situations such as <!ENTITYrunon ... */
	    if (m_gendelim(n, context))
		{
		for (i = n ; i >= 0 ; i--)
		    if (m_srefchartype[i] < M_WSCHAR)
			m_ungetachar(m_hold[i], m_dhold[i], TRUE) ;
		return ;
		}
	    linestart = TRUE ;
	    for (i = n ; i >= 0 ; i--)
	    if (m_srefchartype[i] < M_WSCHAR)
		{
		linestart = FALSE ;
		break ;
		}
	    if (linestart) m_atrs = FALSE ;
	    if (m_stacktop->map && m_stacktop->map[m_delim[n] - 1])
		{
		m_entexpand(
		&m_entities[m_stacktop->map[m_delim[n] - 1] - 1]) ;
		return ;
		}
	    if (m_conform)
		{
		for (i = n ; i >= 0 ; i--)
		if (m_srefchartype[i] < M_WSCHAR)
		m_ungetachar(m_hold[i], M_CDCHAR, TRUE) ;
		return ;
		}
	    }
	if (m_srefchartype[n] < M_WSCHAR)
	m_ungetachar(m_hold[n], m_dhold[n], TRUE) ;
	if (m_srefchartype[n] > M_REGCHAR)
	    {
	    m_srefchartype[n]-- ;
	    break ;
	    }
	n-- ;
	if (n < 0) return ;
	}
    }
}

/* Test for significant record ends.  Ignore RE (\n) if
      1)  It is the first RE in the content and no data character
          or contextual end tag has occurred
      2)  Something has occurred on the line but not a data character
          or contextual end tag [linestat == M_SOMETHING]
      3)  If a record end might be the last one in an element, save it
*/
void m_sigre(M_NOPAR)
  {
    /* Check for first RE in content and no preceding content */
    if (m_start &&
        (! m_stacktop->firstre && m_oldlinestat[m_oldlsindex] != M_DCORCET)) {
      m_stacktop->firstre = TRUE ;
      return ;
      }
    /* Check for line containing other than data characters or contextual
       subelements */
    if (m_start && m_oldlinestat[m_oldlsindex] == M_SOMETHING) return ;
    /* Save the RE to see what follows */
    m_holdproc() ;
    m_stacktop->holdre = TRUE ;
    return ;
    } /* End white space */

/* Returns a context-dependent delimiter string to input stream so
   characters can be reread one at a time in another context */
#if defined(M_PROTO)
void m_undodelim(M_WCHAR *delim, LOGICAL flag)
#else
void m_undodelim(delim, flag)
  M_WCHAR *delim ;
  LOGICAL flag ;
#endif
{
    M_WCHAR *p ;

    for (p = delim ; *p ; p++) ;

    p-- ;
    while (TRUE) {
      m_ungetachar((int) *p, M_NORMAL, flag) ;
      if (p == delim) return ;
      p-- ;
      }
    }

/* Place a character on the current input stream.  The character may have
   been scanned and determined not to be part of the current token or it
   may be in the expansion of an entity*/
#if defined(M_PROTO)
void m_ungetachar(int c, M_HOLDTYPE dchar, LOGICAL preread)
#else
void m_ungetachar(c, dchar, preread)
  int c ;
  M_HOLDTYPE dchar ;
  LOGICAL preread ;
#endif
{
    char buffer[10] ;
    int length;
    char mb_ee;
    M_WCHAR wc_ee;

    mb_ee = M_EE;
    mbtowc(&wc_ee, &mb_ee, 1);
    if (m_chtrace) {
      if (dchar) {
        m_trace("unget(") ;
	length = wctomb(buffer, c);
	buffer[length] = 0;
        m_trace(buffer) ;
        m_trace(")[") ;
        sprintf(buffer, "%d", c) ;
        m_trace(buffer) ;
        m_trace("],") ;
        sprintf(buffer, "%d", dchar) ;
        m_trace(buffer) ;
        m_trace("\n") ;
        }
      else m_trace("unget(EE)\n") ;
      }
    m_inctest(&m_toundo, M_SAVECHAR, "M_SAVECHAR") ;
    m_sourcefile[m_toundo - 1] = m_sysecnt ;
    m_savedchar[m_toundo - 1] = dchar ;
    m_savechar[m_toundo - 1] = dchar == wc_ee ? (int) m_atrs : c ;
    if (preread) {
      m_stacktop->linestat = m_oldlinestat[m_oldlsindex] ;
      m_atrs = m_oldatrs[m_oldlsindex] ;
      m_oldlsindex = (m_oldlsindex - 1 + M_SAVECHAR) % M_SAVECHAR ;
      }
    if (m_toundo > m_maxundo) m_maxundo = m_toundo ;
    if (c == M_RE)
      if (dchar == M_NORMAL) m_line[m_sysecnt]-- ;
    }

/* Have encountered an M_ERO.  If the entity reference is valid, process it*/
LOGICAL m_vldentref(M_NOPAR)
  {
    M_HOLDTYPE dchar ;
    int next ;
    M_ENTITY *openent ;
    char mb_ee;
    M_WCHAR wc_ee;

    mb_ee = M_EE;
    mbtowc(&wc_ee, &mb_ee, 1);
    next = m_getachar(&dchar) ;
    if (next != EOF && m_cttype(next) == M_NMSTART && dchar != wc_ee) {
      m_getname((M_WCHAR) next) ;
      if (! m_gettoken(&next, &dchar, ENTREF))
        if (next != M_RE) m_ungetachar(next, dchar, TRUE) ;
      if (openent = (M_ENTITY *) m_lookfortrie(m_name, m_enttrie))
        m_entexpand(openent) ;
      else m_err1("Reference to undefined entity '%s'", m_name) ;
      return(TRUE) ;
      }
    m_ungetachar(next, dchar, TRUE) ;
    return(FALSE) ;
    }

#if defined(sparse)
#include "sparse.c"
#endif


