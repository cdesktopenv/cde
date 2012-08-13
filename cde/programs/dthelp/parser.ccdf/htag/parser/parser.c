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
/* $XConsortium: parser.c /main/3 1995/11/08 11:28:19 rswiston $ */
/*
              Copyright 1986 Tandem Computers Incorporated.
This product and information is proprietary of Tandem Computers Incorporated.
                   Copyright 1986, 1987, 1988, 1989 Hewlett-Packard Co.
*/

/*  Parser.c contains the main procedure for program PARSER */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "basic.h"
#include "trie.h"
#include "dtdext.h"
#include "context.h"
#include "delim.h"
#include "parser.h"
#include "if.h"
#include "entext.h"

/* Main procedure */
void main(argc, argv)
  int argc ;
  char **argv ;
  {
    static char parserr[] = "\nM_token=%d, m_prevcon=%d, m_scanval=%d\n" ;
    static char sopt[] =
      "prevcon=%d,token=%d,curcon=%d,scanval='%c'(%d),line=%d,netlevel=%d\n" ;
    static char name[] = "m_name = '%s'\n" ;
    static char literal[] = "m_literal = '%s'\n" ;
#if defined(hpux) || defined(_AIX) || defined(sun) || defined(USL) || defined(__uxp__)
    char buffer[M_LITLEN + 80] ;
#else
#define max4(a,b,c,d) (a>b&&a>c&&a>d) ? a : ((b>c&&b>d) ? b : (c>d ? c : d))
    M_WCHAR buffer[max4(sizeof(parserr) + 3,
                     sizeof(sopt) + 6,
                     sizeof(name) + M_NAMELEN - 2,
                     sizeof(literal) + M_LITLEN - 2)] ;
#endif

    m_argc = argc ;
    m_argv = argv ;

    m_strtcase(1) ;
    m_globss() ;
    m_setoptions() ;
    m_initialize() ;
    while (TRUE) {
      m_token = m_scan() ;
      m_prevcon = m_curcon ;
      m_curcon = m_newcon(m_prevcon - 1, m_token - 1) ;
      if (! m_curcon) {
        if (m_token != M_ENDFILE) {
          m_error("Parsing table error") ;
          sprintf(buffer, parserr, m_token, m_prevcon, m_scanval) ;
          m_errline(buffer) ;
          }
        else {
          m_error("Unexpected end of document") ;
          m_showcurelt() ;
          }
        m_done() ;
        }
      if ((! m_stacktop->oldtop) &&
          m_start &&
          ! m_aftereod &&
          m_state[m_stacktop->fsastack->current - 1].final &&
          (m_token != M_ENDFILE) &&
          (m_token != M_TEXT || ! m_whitespace((M_WCHAR) m_scanval))
          ) {
        m_error("Expecting end of document") ;
        m_aftereod = TRUE ;
        }
      if (m_scantrace) {
        sprintf(buffer, sopt,
          m_prevcon, m_token, m_curcon, m_scanval ? m_scanval : ' ',
          m_scanval, m_line[m_sysecnt], m_netlevel) ;
        m_trace(buffer) ;
        if (m_token == M_NAME) {
          sprintf(buffer, name, m_name) ;
          m_trace(buffer) ;
          }
        if (m_token == M_LITERAL) {
          sprintf(buffer, literal, m_literal) ;
          m_trace(buffer) ;
          }
        }
#include "case.c"
      m_adjuststate() ;
      if (m_token == M_ENDFILE) break ;
      }
    /* At EOF */
    m_done() ;
    }

