/* $XConsortium: text.c /main/3 1995/11/08 11:33:41 rswiston $ */
/*
              Copyright 1986 Tandem Computers Incorporated.
This product and information is proprietary of Tandem Computers Incorporated.
                   Copyright 1986, 1987, 1988, 1989 Hewlett-Packard Co.
*/

/* Text.c executes text-code specified in the interface. */

#include "userinc.h"
#include "globdec.h"

/* Include generated file */
#include "tfile.c"

/* When a text character occurs */
#if defined(M_PROTO)
void m_textaction(M_WCHAR m_textchar)
#else
void m_textaction(m_textchar)
  M_WCHAR m_textchar ;
  #endif
{
    char buffer[10] ;
    char    mb_re;
    M_WCHAR wc_re;

    mb_re = M_RE;
    mbtowc(&wc_re, &mb_re, 1);

    if (m_stacktop->holdre) {
      m_stacktop->holdre = FALSE ;
      m_textaction(wc_re) ;
      }
    if (m_cdtrace) {
      m_trace("Text character '") ;
      buffer[0] = m_textchar ;
      buffer[1] = M_EOS ;
      m_trace(buffer) ;
      m_trace("' (") ;
      sprintf(buffer, "%d", m_textchar) ;
      m_trace(buffer) ;
      m_trace(")\n") ;
      }
    if (m_tagtrace)
      m_toptstat = m_textchar == M_SPACE || m_textchar == M_TAB ?
                   M_WHITESPACE : M_OTHER ;
    if (m_textchar != wc_re) m_stacktop->linestat = M_DCORCET ;
    m_stackpar = m_stacktop->cdparam ;
    (*m_ttable[m_stacktop->cdcase])(m_textchar) ;
    }


