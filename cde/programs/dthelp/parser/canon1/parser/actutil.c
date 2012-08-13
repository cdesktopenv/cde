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
/* $XConsortium: actutil.c /main/3 1995/11/08 09:34:00 rswiston $ */
/*
              Copyright 1986 Tandem Computers Incorporated.
This product and information is proprietary of Tandem Computers Incorporated.
                   Copyright 1986, 1987, 1988, 1989 Hewlett-Packard Co.
*/

/* Actutil.c contains utility procedures for processing actions specified
in the interface definition.*/

#include <stdio.h>
#include <stdlib.h>
#if defined(MSDOS)
#include <process.h>
#endif
#include <string.h>
#include "basic.h"
#include "trie.h"
#include "dtdext.h"
#include "parser.h"
#define M_IFDEF
#include "if.h"
#include "delim.h"
#include "context.h"
#include "signon.h"

/* When an explicit or implied end-tag occurs */
void m_endaction(m_elt)
  M_ELEMENT m_elt;
  {
    M_ELEMENT m_action;
    char buffer[2*MAXD+M_NAMELEN+1];

    if (m_tagtrace) {
      if (m_element[m_elt - 1].content != M_NONE) {
        if (m_toptstat == M_OTHER) {
          strcpy(buffer, "\n");
          m_trace(buffer);
          }
        sprintf(buffer,
		"%s%s%s",
		m_etago,
		m_nameofelt(m_elt),
		m_tagc);
        m_trace(buffer);
        }
      m_toptstat = M_OTHER;
      }
    if (m_action = m_findact(m_elt, m_ecarray)) {
      m_stackpar = m_stacktop;
      m_endcase(m_action);
      }
    if (m_action = m_findact(m_elt, m_etarray)) {
      m_stackpar = m_stacktop->stparam;
      m_textout(&m_string[m_action - 1], FALSE, TRUE);
      }
    }

/* Find appropriate action according to current stack */
int m_findact(elt, array)
  M_ELEMENT elt;
  int *array;
  {
    int chainlen = 0;
    int index;

    if (! array[elt - 1]) return(FALSE);
    if (m_stacktop->element != elt) {
      m_error("Program error in findact");
      m_exit(TRUE);
      }
    /* There is an action for this element with no context specification */
    if (m_action[array[elt - 1] - 1].data) {
      chainlen = 1;
      index = array[elt - 1];
      }
    /* Only actions for this element have context specified */
    else {
      chainlen = 0;
      index = 0;
      }
    m_findchain(m_stacktop->oldtop, array[elt - 1], chainlen, &chainlen,
                &index, FALSE);
    return(index ? m_action[index - 1].data : FALSE);
    }

/* Recursive procedure called by findact() to search m_action */
#if defined(M_PROTO)
void m_findchain(M_PARSE *stackptr, int start, int chainin, int *chainout, int *index, LOGICAL wild)
#else
void m_findchain(stackptr, start, chainin, chainout, index, wild)
  M_PARSE *stackptr;
  int start;
  int chainin;
  int *chainout;
  int *index;
  LOGICAL wild;
#endif
{
    int node;
    M_PARSE *stackp;

    for (node = m_action[start - 1].son ; node;
         node = m_action[node - 1].next) {
      if (m_action[node - 1].element == 1) 
        m_findchain(stackptr, node, chainin, chainout, index, TRUE);
      else for (stackp = stackptr;
                stackp->oldtop;
                stackp = stackp->oldtop) {
        if (stackp->element == m_action[node - 1].element - 1) {
          if (m_action[node - 1].data) 
            if (chainin + 1 > *chainout ||
                (chainin + 1 == *chainout && node < *index)) {
              *chainout = chainin + 1;
              *index = node;
              }
          m_findchain(stackp->oldtop, node, chainin + 1, chainout,
                      index, FALSE);
          }
        if (! wild) break;
        }
      }
    }

/* Process global end string.  In separate procedure to keep all references
   to if.h in one source file and minimize recompilation if interface
   changes. */
void m_globes(M_NOPAR)
{
M_WCHAR *wc_string;

wc_string = MakeWideCharString(&m_string[m_ges]);
if (m_ges) m_stcaction(wc_string, FALSE, TRUE);
m_free(wc_string,"wide character string");
}    

/* Process global start string.  In separate procedure to keep all references
   to if.h in one source file and minimize recompilation if interface
   changes. */
void m_globss(M_NOPAR)
{
M_WCHAR *wc_string;

wc_string = MakeWideCharString(&m_string[m_gss]);
if (m_gss) m_stcaction(wc_string, TRUE, FALSE);
m_free(wc_string,"wide character string");
}    

/* When an explicit or implied start-tag occurs */
void m_strtaction(m_elt)
  M_ELEMENT m_elt;
  {
    int m_par, m_i;
    M_WCHAR *m_p;
    M_ELEMENT m_action;
    static char newpar[] = "\n   ";
    static char quote[] = " = \"";
    char buffer[M_NAMELEN + 1 +
      (sizeof(quote) + sizeof(newpar) - 2 > MAXD ?
         sizeof(quote) + sizeof(newpar) - 2 : 
         MAXD)
      ];

    m_start = TRUE;
    m_getline(&m_stacktop->file, &m_stacktop->line);
    if (m_tagtrace) {
      sprintf(buffer, "%s%s", m_stago, m_nameofelt(m_elt));
      m_trace(buffer);
      for (m_i = 0, m_par = m_element[m_elt - 1].parptr;
           m_i < m_element[m_elt - 1].parcount;
           m_i++, m_par++)
        if (m_stacktop->param[m_i]) {
          sprintf(buffer, "%s%s%s",
                  newpar, &m_pname[m_parameter[m_par - 1].paramname], quote);
          m_trace(buffer);
          buffer[1] = M_EOS;
          for (m_p = m_stacktop->param[m_i] ; *m_p ; m_p++)
            if (*m_p != '"') {
              buffer[0] = *m_p;
              m_trace(buffer);
              }
            else {
              sprintf(buffer, "%s%d", m_cro, '"');
              m_trace(buffer);
              buffer[1] = M_EOS;
              }
          buffer[0] = '"';
          m_trace(buffer);
          }
      if (m_element[m_elt - 1].parcount) {
        buffer[0] = '\n';
        buffer[1] = M_EOS;
        m_trace(buffer);
        }
      sprintf(buffer, "%s\n", m_tagc);
      m_trace(buffer);
      m_toptstat = M_TOPTSTARTTAG;
      }
    if (m_action = m_findact(m_elt, m_scarray)) {
      m_stackpar = m_stacktop;
      m_strtcase(m_action);
      }
    if (m_action = m_findact(m_elt, m_stcarray)) {
      m_stacktop->stccase = m_action;
      m_stacktop->stparam = m_stacktop;
      }
    if (m_action = m_findact(m_elt, m_starray)) {
      m_stackpar = m_stacktop->stparam;
      m_textout(&m_string[m_action - 1], TRUE, FALSE);
      }
    if (m_action = m_findact(m_elt, m_tcarray)) {
      m_stacktop->cdcase = m_action;
      m_stacktop->cdparam = m_stacktop;
      }
    if (m_action = m_findact(m_elt, m_pcarray)) {
      m_stacktop->picase = m_action;
      m_stacktop->piparam = m_stacktop;
      }
    }

/* Output a start-string or end-string */
#if defined(M_PROTO)
void m_textout(char *format, LOGICAL start, LOGICAL end)
#else
void m_textout(format, start, end)
  char *format;
  LOGICAL start;
  LOGICAL end;
#endif
{
M_WCHAR name[M_NAMELEN + 1];
int i, par;
LOGICAL found;
M_WCHAR *string;
M_WCHAR *p;
M_WCHAR *q;
M_WCHAR *r;
M_WCHAR *s;
M_WCHAR *new;
int stringlen;
int changelen;
int unused;

stringlen = strlen(format) + 1;
unused = 0;
string = (M_WCHAR *) m_malloc(stringlen, "string space");
for (p = string ; *format ; )
    {
    if (*format == M_ESCAPECHAR)
	{
	for (i = 0, format++ ; i < M_NAMELEN ; i++, format++)
	    {
	    mbtowc(&name[i], format, 1);
	    if (m_cttype(name[i]) == M_NONNAME ||
		(m_cttype(name[i]) != M_NMSTART && i == 0)
	       ) break;
	    }
	if (! i)
	    {
	    char mb;

	    mb = M_ESCAPECHAR;
	    mbtowc(p, &mb, 1);
	    p++;
	    /* Double escape character used to insert a single escape character
	       in the output string */
	    if (*format == M_ESCAPECHAR) format++;
	    continue;
	    }
	name[i] = M_EOS;
	for (found = FALSE, i = 0,
		 par = m_element[m_stacktop->element - 1].parptr;
		 i < m_element[m_stacktop->element - 1].parcount;
		 i++, par++)
	    if (! m_wcupstrcmp(&m_pname[m_parameter[par - 1].paramname], name))
		{
		q = m_stacktop->param[i];
		if (! q)
		    {
		    found = TRUE;
		    unused += w_strlen(name) + 1;
		    break;
		    }
		changelen = w_strlen(q) - w_strlen(name) - 1 - unused;
		if (changelen > 0)
		    {
		    new = (M_WCHAR *) m_malloc(stringlen + changelen,
					       "string space");
		    for (r = string, s = new ; r < p ; ) *s++ = *r++;
		    m_free(string, "string space");
		    string = new;
		    stringlen = stringlen + changelen;
		    p = s;
		    unused = 0;
		    }
		else if (changelen < 0) unused = -changelen;
		found = TRUE;
		break;
		}
	if (! found)
	    {
	    char mb;

	    mb = M_ESCAPECHAR;
	    mbtowc(p, &mb, 1);
	    p++;
	    q = name;
	    }
	if (q) while (*q) *p++ = *q++;
	}
    else *p++ = *format++;
    }
*p = M_EOS;
m_stcaction(string, start, end);
m_free(string, "string space");
}
