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
/* $XConsortium: buildutl.c /main/3 1995/11/08 10:41:58 rswiston $ */
/*
              Copyright 1986 Tandem Computers Incorporated.
This product and information is proprietary of Tandem Computers Incorporated.
                   Copyright (c) 1986, 1987, 1988, 1989 Hewlett-Packard Co.
*/

/* Buildult.c contains the main supporting utilities for program BUILD */

#include <stdlib.h>
#include "context.h"
#include "build.h"
#include "delim.h"

#include "version.h"

/* Verify that <PARAM>, <MIN> or <USEMAP> has not previously occurred in
   this rule */
void found(flag, delim)
LOGICAL *flag;
char *delim;
{
M_WCHAR wcbuff[129];
ssize_t  length;

wcbuff[128] = 0;
length = mbstowcs(wcbuff,delim,128);
if (length < 0)
    {
    m_error("An invalid multibyte character was seen");
    wcbuff[0] = 0;
    }

if (*flag)
    {
    m_err1("A rule can contain only one %s clause", wcbuff) ;
    return ;
    }
*flag = TRUE ;
}

/* Program initialization */
void initialize(M_NOPAR)
{
char    **mb_delims;
M_WCHAR **wc_delims;

ifile = stdin ;
m_openchk(&m_errfile, "error", "w") ;

fprintf(stderr, "MARKUP System - BUILD %s\n", M_VERSION) ;
fprintf(stderr, "Copyright (c) 1986, 1987, 1988, 1989 Hewlett-Packard Co.\n") ;
if (! standard)
    warning("Info: optional enhancements of SGML enabled.") ;

mb_delims = mb_dlmptr;
wc_delims = m_dlmptr;

while (*mb_delims)
    {
    *wc_delims++ = MakeWideCharString(*mb_delims);
    mb_delims++;
    }
*wc_delims = 0;

ruleinit() ;
}


/* Initialization before starting a new rule */
void ruleinit(M_NOPAR)
  {
    /* Reinitialize for next rule */
    lhs = NULL ;
    nextlhs = &lhs ;
    eltsinrule = 0 ;
    plist = NULL ;
    pcount = 0 ;
    dellist(&bot.finals) ;
    dellist(&bot.allfinal) ;
    dellist(&bot.newfinal) ;
    poccur = uoccur = moccur = FALSE ;
    smin = emin = FALSE ;
    srefp = 0 ;
    useoradd = TRUE ;
    egensuf = 1 ;
    idoccur = FALSE ;
    }

/* Rule end processing */
void rulend(M_NOPAR)
  {
    STATE *fsa = NULL ;
    LOGICAL canbenull ;
    LHS *lhsp ;
    LHS *discard ;
    int inptr ;
    int exptr ;

    if (contype == GRPO) fsa = startfsa(ruletree, &canbenull) ;
    if (pcount > maxpar) maxpar = pcount ;
    inptr = lhs->elt->inptr ;
    exptr = lhs->elt->exptr ;
    for (lhsp = lhs ; lhsp ; ) {
      lhsp->elt->model = contype == GRPO ? fsa : (STATE *) M_NULLVAL ;
      lhsp->elt->content = contype ;
      lhsp->elt->inptr = inptr ;
      lhsp->elt->exptr = exptr ;
      if (plist) {
        if (lhsp->elt->parptr)
          warning1("Redefining parameter list for element %s",
            lhsp->elt->enptr) ;
        lhsp->elt->parptr = plist ;
        lhsp->elt->paramcount = pcount ;
        lhsp->elt->parindex = parcount - pcount + 1 ;
        }
      lhsp->elt->stmin = smin ;
      lhsp->elt->etmin = emin ;
      lhsp->elt->srefptr = srefp ;
      lhsp->elt->useoradd = useoradd ;
      discard = lhsp ;
      lhsp = lhsp->next ;
      m_free((M_POINTER) discard, "lhs") ;
      }
    if (ruletree) freetree(ruletree) ;
    ruletree = NULL ;
    ruleinit() ;
    }

/* Skip rest of statement after an error */
void skiptoend(M_NOPAR)
  {
    int i ;
    static int errlev = 0 ;

    if (! errlev++) {
      curcon = ERROR ;
      for (i = scan() ; ; i = scan())
        if ((i == REND && ! entrule) ||
            (i == TAGC && entrule) ||
            i == ENDFILE) break ;
      curcon = RULE ;
      entrule = FALSE ;
      }
    errlev-- ;
    ruleinit() ;
  }

