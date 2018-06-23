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
/* $XConsortium: input.c /main/3 1995/11/08 09:38:12 rswiston $ */
/*
              Copyright 1986 Tandem Computers Incorporated.
This product and information is proprietary of Tandem Computers Incorporated.
                   Copyright 1986, 1987, 1988, 1989 Hewlett-Packard Co.
*/

/* Input.c contains procedures that deal with the interface between the
   scanner and parser for PARSER */

#include <stdio.h>
#include "basic.h"
#include "trie.h"
#include "dtdext.h"
#include "delim.h"
#include "context.h"
#include "parser.h"

/* M_etcomplete is called when a complete end-tag has been read */
void m_etcomplete(void)
  {
    if (! m_scanel) {
      M_WCHAR *wc_etago, *wc_tagc;

      wc_etago = MakeWideCharString(m_etago);
      wc_tagc  = MakeWideCharString(m_tagc);
      m_err3("%s%s%s ignored: element undefined", wc_etago, m_name, wc_tagc) ;
      m_free(wc_etago,"multi-byte string");
      m_free(wc_tagc,"multi-byte string");
      }
    else
      if (! m_ckend(m_scanel, FALSE)) {
        m_err1("End tag for %s unexpected", m_nameofelt(m_scanel)) ;
        m_expecting() ;
        m_showcurelt() ;
        if (m_element[m_stacktop->element - 1].content == M_CDATA ||
            m_element[m_stacktop->element - 1].content == M_RCDATA)
          m_ckend(m_stacktop->element, FALSE) ;
        m_frcend(m_scanel) ;
        }
    }

/* M_frcend is called after a syntax error to end element VAL even
   if more content for that element is expected */
void m_frcend(M_ELEMENT val)
  {
    M_PARSE *stackptr ;
    M_ELEMENT poppedval ;

    for (stackptr = m_stacktop ;
         stackptr->oldtop && stackptr->element != val ;
         stackptr = stackptr->oldtop) ;
    if (! stackptr->oldtop) {
      m_err1("No %s element currently opened", m_nameofelt(val)) ;
      if (m_stacktop->oldtop)
        if (m_element[m_stacktop->element].content == M_CDATA ||
            m_element[m_stacktop->element].content == M_RCDATA)
          m_ckend(m_stacktop->element, FALSE) ;
      return ;
      }
    while (TRUE) {
      poppedval = m_stacktop->element ;
      m_endaction(m_stacktop->element) ;
      m_pop() ;
      if (poppedval == val) return ;
      }
    }

/* M_frcstart is called to force the start of an element when that element
   cannot legally occur in the current context but does appear in the text*/
void m_frcstart(void)
  {
    m_push(m_scanel, m_element[m_scanel - 1].start, m_scannet) ;
    if (m_scannet) m_netlevel++ ;
    m_stacktop->contextual = FALSE ;
    m_stkparams() ;
    if (m_element[m_scanel - 1].content == M_CDATA ||
        m_element[m_scanel - 1].content == M_RCDATA) {
      m_stacktop->intext = TRUE ;
      m_curcon = m_element[m_scanel - 1].content == M_CDATA ?
                 CDATAEL : RCDATAEL ;
      m_stacktop->thisent = m_eopencnt ;
      }
    }

/* M_holdproc processes a RE that was saved to test if it was the last
   RE in an element's content */
void m_holdproc(void)
  {
    if (m_stacktop->holdre) {
      m_stacktop->holdre = FALSE ;
      if (m_stacktop->intext) m_textaction(M_RE) ;
      else {
        m_strtcdata((int) M_RE) ;
        if (m_curcon == NETDATACON) m_curcon = NETCDATA ;
        else if (m_curcon == DATACON) m_curcon = POUNDCDATA ;
        }
      }
    }

/* M_stcomplete is called when a complete start tag has been recognized */
void m_stcomplete(void)
  {
    if (! m_scanel) return ;
    if (m_strtproc(m_scanel))
      m_stkparams() ;
    else {
      if (m_stacktop->holdre) m_holdproc() ;
      /* M_strtproc should not return TRUE as long as #PCDATA is considered
         optional, but this code should work if the standard is changed */
      if (m_strtproc(m_scanel)) m_stkparams() ;
      else {
        if (m_stacktop->oldtop) {
          m_err2("%s not allowed at this point in %s",
                 m_nameofelt(m_scanel),
		 m_nameofelt(m_stacktop->element)) ;
          m_expecting() ;
          m_showcurelt() ;
          }
        else if (! m_start) {
	  M_WCHAR *wc_stago, *wc_tagc;

	  wc_stago = MakeWideCharString(m_stago);
	  wc_tagc  = MakeWideCharString(m_tagc);
          m_err6("Expecting %s%s%s instead of %s%s%s to start document",
                 wc_stago,
		 m_nameofelt(1),
		 wc_tagc,
		 wc_stago,
		 m_nameofelt(m_scanel),
                 wc_tagc) ;
	  m_free(wc_stago,"multi-byte string");
	  m_free(wc_tagc,"multi-byte string");
	  }
        m_frcstart() ;
        }
      }
    if (m_element[m_scanel - 1].content == M_NONE) m_endtag(m_scanel) ;
    }
