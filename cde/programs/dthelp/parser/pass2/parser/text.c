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
/* $XConsortium: text.c /main/3 1995/11/08 11:00:29 rswiston $ */
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

#define M_BUFLEN (32)

/* When a text character occurs */
#if defined(M_PROTO)
void m_textaction(M_WCHAR m_textchar)
#else
void m_textaction(m_textchar)
  M_WCHAR m_textchar ;
  #endif
{
    char buffer[M_BUFLEN] ;
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
      snprintf(buffer, M_BUFLEN - 1, "%d", m_textchar) ;
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


