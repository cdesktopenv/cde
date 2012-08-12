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
/* $XConsortium: param.c /main/3 1995/11/08 11:26:48 rswiston $ */
/*
              Copyright 1986 Tandem Computers Incorporated.
This product and information is proprietary of Tandem Computers Incorporated.
                   Copyright 1986, 1987, 1988, 1989 Hewlett-Packard Co.
*/

/* Param.c has procedures relevant to parameters (attributes). */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#if defined(MSDOS)
#include <process.h>
#endif
#include "basic.h"
#include "trie.h"
#include "delim.h"
#include "context.h"
#include "dtdext.h"
#include "parser.h"
#include "entext.h"

/* Process the value for the parameter whose index number was previously
   saved in m_ppsave */
void m_attval(string)
  M_WCHAR *string ;
  {
    const M_WCHAR *p ;

    if (p = m_partype(m_ppsave, string)) {
      if (m_parameter[m_ppsave - 1].type == M_KEYWORD)
        m_poccur[m_psave] = (M_WCHAR *) p ;
      else {
        m_poccur[m_psave] = (M_WCHAR *)
	  m_malloc(w_strlen(p) + 1, "parameter value") ;
        w_strcpy(m_poccur[m_psave], p) ;
        m_parupper(m_ppsave, m_poccur[m_psave]) ;
        }
      m_updatedefault(m_ppsave, p) ;
      }
    else m_err2("\"%s\" -- error in value for parameter %s",
                string,
		&m_pname[m_parameter[m_ppsave - 1].paramname]) ;
    }

/* Process a string that is a parameter value not prefixed by the parameter
   name and value indicator */
LOGICAL m_attvonly(string)
  M_WCHAR *string ;
  {
    const M_WCHAR *p ;
    int par, i ;

    if (! m_scanel) {
      m_error("Program error in m_attvonly") ;
      m_exit(TRUE) ;
      }
    /* First check is it a valid keyword? */
    for (par = m_element[m_scanel - 1].parptr, i = 0 ;
         i < m_element[m_scanel - 1].parcount ;
         par++, i++)
        if (m_parameter[par - 1].type == M_KEYWORD)
          if (p = m_partype(par, string)) {
            if (m_poccur[i])
              m_err2(
                "Redefinition of parameter %s. Discarding old value '%s'.",
                &m_pname[m_parameter[par - 1].paramname],
		m_poccur[i]) ;
            m_poccur[i] = (M_WCHAR *) p ;
            m_updatedefault(par, p) ;
            return(TRUE) ;
            }
    /* It wasn't a keyword.  Check for valid value for some other
       parameter whose value has not yet been specified. */
    for (par = m_element[m_scanel - 1].parptr, i = 0 ;
         i < m_element[m_scanel - 1].parcount ;
         par++, i++)
      if (! m_poccur[i]) {
        m_poccur[i] = (M_WCHAR *) m_partype(par, string) ;
        if (m_poccur[i]) {
          m_parupper(par, string) ;
          m_poccur[i] = (M_WCHAR *)
	    m_malloc(w_strlen(string) + 1, "parameter value") ;
          w_strcpy(m_poccur[i], string) ;
          m_updatedefault(par, m_poccur[i]) ;
          return(TRUE) ;
          }
        }
    m_err2("%s: impossible value for any parameters of %s",
           string,
	   m_nameofelt(m_scanel)) ;
    return(FALSE) ;
    }

/* Check a name previously saved in m_saveatt to see if it is the name of
   a valid parameter for the current start tag */
void m_findatt(M_NOPAR)
  {
    int par, i ;

    if (! m_scanel) {
      m_error("Program error in m_findatt") ;
      m_exit(TRUE) ;
      }
    for (par = m_element[m_scanel - 1].parptr, i = 0 ;
         i < m_element[m_scanel - 1].parcount ;
         par++, i++)
        if (! m_wcupstrcmp(&m_pname[m_parameter[par - 1].paramname],
                         m_saveatt)) {
          if (m_poccur[i])
            m_err2("Redefinition of parameter %s. Discarding old value '%s'.",
                   &m_pname[m_parameter[par - 1].paramname],
		   m_poccur[i]) ;
          m_psave = i ;
          m_ppsave = par ;
          return ;
          }
    m_attvonly(m_saveatt) ;
    if (! m_wholetag) {
      M_WCHAR *wc_tagc;

      wc_tagc  = MakeWideCharString(m_tagc);
      m_err3("No %s parameter for %s (possibly missing %s)",
             m_saveatt,
	     m_nameofelt(m_scanel),
	     wc_tagc) ;
      m_free(wc_tagc,"multi-byte string");
      }
    m_undodelim(m_dlmptr[M_VI - 1], TRUE) ;
    m_stcomplete() ;
    m_curcon = START ;
    }

/* Free the parameter storage associated with an element on the parse stack */
void m_freeparam(stackelt)
  M_PARSE *stackelt ;
  {
    int i ;
    int par ;

    if (stackelt->param) {
      for (i = 0, par = m_element[stackelt->element - 1].parptr - 1 ;
           i < m_element[stackelt->element - 1].parcount ; i++, par++)
        if (m_parameter[par].type != M_KEYWORD)
          if (m_parameter[par].deftype != M_NAMEDEF ||
              stackelt->param[i] != m_parameter[par].defval)
            if (stackelt->param[i])
              m_free(stackelt->param[i], "parameter value") ;
      m_free(stackelt->param, "parameter block") ;
      }
    }

/* Force a parameter value to uppercase, if appropriate for its type.
   Also, if list-valued attribute, remove leading and trailing spaces,
   and condense white-space sequences to a single blank*/
void m_parupper(par, string)
  int par ;
  M_WCHAR *string ;
  {
    M_WCHAR *p ;
    M_WCHAR *q ;
    int i ;

    switch (m_parameter[par - 1].type) {
      case M_ID:
      case M_IDRF:
      case M_NAMEPAR:
      case M_NMTOKEN:
      case M_NUTOKEN:
      case M_ENTATT:
        for ( ; *string ; string++)
          *string = m_ctupper(*string) ;
        return ;
      case M_IDRFS:
      case M_NAMES:
      case M_NMSTOKEN:
      case M_NUSTOKEN:
      case M_NUMS:
        for (p = string; *p ; p++)
          if (! m_whitespace(*p)) break ;
        w_strcpy(string, p) ;
        for (p = string, i = 0 ; *p ; p++, i++)
          *p = m_ctupper(*p) ;
          if (m_whitespace(*p)) {
            *p = M_SPACE ;
            for (q = p + 1 ; m_whitespace(*q); q++) ;
            w_strcpy(p + 1, q) ;
            }
        if (i && m_whitespace(string[i - 1])) string[i - 1] = M_EOS ;
        return ;
      default:
        for ( ; *string ; string++)
          if (*string == M_RE || *string == M_TAB) *string = M_SPACE ;
        return ;
      }
    }

/* Set all parameters to their default values for an element included by
   tag minimization */
void m_stkdefaultparams(M_NOPAR)
  {
    int i, par = 0 ;

    m_stacktop->param = (M_WCHAR **)
      m_malloc(
        m_element[m_stacktop->element - 1].parcount * sizeof(M_WCHAR *),
        "parameter block") ;
    for (i = 0, par = m_element[m_stacktop->element - 1].parptr ;
         i < m_element[m_stacktop->element - 1].parcount ;
         i++, par++)
      m_stkonedef(par, m_stacktop->element, m_stacktop->param, i) ;
    m_strtaction(m_stacktop->element) ;
    }

/* Stack one default parameter */
void m_stkonedef(par, scanel, poccur, i)
  int par ;
  M_ELEMENT scanel ;
  M_WCHAR **poccur ;
  int i ;
  {
    if (m_parameter[par - 1].deftype == M_REQUIRED ||
        (m_parameter[par - 1].deftype == M_CURRENT &&
         ! m_parameter[par - 1].defval))
      m_err2("Missing value for %s parameter of element %s",
             &m_pname[m_parameter[par - 1].paramname],
	     m_nameofelt(scanel)) ;
    poccur[i] = m_parameter[par - 1].defval ;
    if (! m_parameter[par - 1].defval) return ;
    if (m_parameter[par - 1].type == M_KEYWORD) return ;

    /* If parameter is an entity name, the default is usable only if
       the name is that of a defined entity */
    if (m_parameter[par - 1].type == M_ENTATT)
      if (! (M_ENTITY *)
            m_lookfortrie(m_parameter[par - 1].defval, m_enttrie)) {
        m_err1("Interface error: Default entity %s undefined", poccur[i]) ;
        poccur[i] = NULL ;
        return ;
        }

    /* Non-null, non-keyword current default must go on stack, since
       default could change in a subelement */
    if (m_parameter[par - 1].deftype != M_CURRENT &&
        m_parameter[par - 1].deftype != M_CHANGEDCUR) return ;
    poccur[i] = (M_WCHAR *)
      m_malloc(w_strlen(poccur[i]) + 1, "parameter value") ;
    w_strcpy(poccur[i], m_parameter[par - 1].defval) ;
    }

/* Stack parameter values */
void m_stkparams(M_NOPAR)
  {
    int i, par ;

    m_stacktop->param = (M_WCHAR **)
      m_malloc(
        m_element[m_stacktop->element - 1].parcount * sizeof(M_WCHAR *),
        "parameter block") ;
    for (i = 0, par = m_element[m_scanel - 1].parptr ;
         i < m_element[m_scanel - 1].parcount ;
         i++, par++) {
      if (! m_poccur[i]) m_stkonedef(par, m_scanel, m_stacktop->param, i) ;
      else m_stacktop->param[i] = m_poccur[i] ;
      }
    m_strtaction(m_stacktop->element) ;
    }

/* Update the default of a parameter whose default is #CURRENT */
#if defined(M_PROTO)
void m_updatedefault(const int par , const M_WCHAR *string )
#else
void m_updatedefault(par, string)
  int par ;
  M_WCHAR *string ;
#endif /* M_PROTO */
  {
    if (m_parameter[par - 1].deftype != M_CURRENT &&
        m_parameter[par - 1].deftype != M_CHANGEDCUR) return ;

    /* For keyword parameters, the string is already saved in the
       keyword array; for entities, it is saved with the entity structure */
    /* In light of the above comment, why does the following if statement
       not test for parameters of type entity? */
    if (m_parameter[par - 1].type == M_KEYWORD) 
      m_parameter[par - 1].defval = (M_WCHAR *) string ;

    /* Save value in allocated storage */
    else {
      if (m_parameter[par - 1].deftype == M_CHANGEDCUR)
        m_free(m_parameter[par - 1].defval, "updateable default") ;
      m_parameter[par - 1].defval = (M_WCHAR *)
	m_malloc(w_strlen(string) + 1, "updateable default") ;
      w_strcpy(m_parameter[par - 1].defval, string) ;
      }

    m_parameter[par - 1].deftype = M_CHANGEDCUR ;
    }

/* Check to see if a string that occurs after the element name in a start
   tag is a valid parameter name or value; if not, assume tag is ended */
LOGICAL m_validinpar(string)
  M_WCHAR *string ;
  {
    int par ;
    M_WCHAR *p ;
    int i ;
    M_WCHAR *wc_tagc;

    if (! m_scanel) {
      m_error("Program error in m_validinpar") ;
      m_exit(TRUE) ;
      }
    for (par = m_element[m_scanel - 1].parptr, i = 0 ;
         i < m_element[m_scanel - 1].parcount ;
         par++, i++) {
      /* Check if valid name of a parameter */
      if (! m_wcupstrcmp(&m_pname[m_parameter[par - 1].paramname], string))
        return(TRUE) ;
      /* Check if possible value of a keyword parameter or of a non-keyword
         parameter that has not yet occurred*/
      if (m_partype(par, string) &&
          (m_parameter[par - 1].type == M_KEYWORD || ! m_poccur[i]))
        return(TRUE) ;
      }
    wc_tagc = MakeWideCharString(m_tagc);
    if (! m_wholetag) m_err1("Invalid parameter or missing %s", wc_tagc) ;
    m_free(wc_tagc,"wide character string");
    m_stcomplete() ;
    m_curcon = POUNDCDATA ;
    for (p = string ; *p ; p++) ;
    for (p-- ; p >= string ; p--) m_ungetachar((int) *p, M_NORMAL, TRUE) ;
    return(FALSE) ;
    }

#include "paramu.c"

