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
/* $XConsortium: struct.c /main/3 1995/11/08 10:26:13 rswiston $ */
/*
              Copyright 1986 Tandem Computers Incorporated.
This product and information is proprietary of Tandem Computers Incorporated.
                   Copyright 1986, 1987, 1988, 1989 Hewlett-Packard Co.
*/

/* Struct.c contains functions relevant to parsing document structure for
   program PARSER */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#if defined(MSDOS)
#include <process.h>
#endif
#include "basic.h"
#include "trie.h"
#include "dtdext.h"
#include "delim.h"
#include "context.h"
#include "parser.h"
#include "entext.h"
#include "if.h"

/* M_checkstart tests to see if the element (or #PCDATA) indicated by VAL
   is valid input. It returns TRUE, FALSE, or M_NONCONTEXTUAL respectively
   if the element is allowed by content, not allowed, or allowed by an
   inclusion exception. */
int m_checkstart(val)
  M_ELEMENT val ;
  {
    M_PARSE *stackptr ;
    int except ;

    /* Check for applicable exclusions */
    if (m_excluded(val)) return(FALSE) ;

    /* Check for declared content in element currently at top of stack */
    if (m_stacktop->oldtop) {
      if (m_element[m_stacktop->element - 1].content == M_ANY) return(TRUE) ;
      if (m_element[m_stacktop->element - 1].content == M_CDATA ||
          m_element[m_stacktop->element - 1].content == M_RCDATA)
        if (! val) return(TRUE) ;
        else return(FALSE) ;
      }

    /* Check content model */
    if (m_transition(val, TRUE)) return(TRUE) ;

    /* Check for inclusions */
    for (stackptr = m_stacktop ;
         stackptr->oldtop ;
         stackptr = stackptr->oldtop)
      for (except = m_element[stackptr->element - 1].inptr ;
           except ;
           except = m_exception[except - 1].next)
      if (m_exception[except - 1].element == val) return(M_NONCONTEXTUAL) ;

    /* Nothing left to try, val is not allowed */
    return(FALSE) ;
    }

/* M_ckend verifies that element VAL can be legally ended at the
   current state of the parse, by an end tag or NET as indicated by NEEDNET.
   If VAL is not the element at the top of the parse stack, m_ckend
   checks to see if the end of VAL can validly end nested
   elements as well. */
#if defined(M_PROTO)
LOGICAL m_ckend(M_ELEMENT val, LOGICAL neednet)
#else
LOGICAL m_ckend(val, neednet)
  M_ELEMENT val ;
  LOGICAL neednet ;
  #endif
{
    M_PARSE *stackptr ;
    M_OPENFSA *fsastack ;
    M_ANDLIST *usedand ;
    M_ANDGROUP pand ;
    M_MIN *minend ;
    M_MIN *discard ;

    m_minend = NULL ;
    m_nextme = &m_minend ;
    /* Go down the stack, checking that each element can end until
       element val occurs */       
    for (stackptr = m_stacktop ; stackptr ; stackptr = stackptr->oldtop) {
      /* If the element at stackptr has a content model, make sure each
         open fsa is in a final state and that all required submodels of
         open and-groups have occurred */
      for (fsastack = stackptr->fsastack ;
           fsastack ;
           fsastack = fsastack->oldtop) {
        if (! m_state[fsastack->current - 1].final) {
          m_freemin(m_minend, "end-tag minimization") ;
          return(FALSE) ;
          }
        for (pand = fsastack->andgroup ;
             pand ;
             pand = m_andgroup[pand - 1].next) {
          /* Don't bother checking if optional submodel of an and-group
             has occurred */
          if (m_state[m_andgroup[pand - 1].start - 1].final) continue ;
          for (usedand = fsastack->usedand ;
               usedand ;
               usedand = usedand->next)
            if (usedand->group == pand) break ;
          if (! usedand) {
            /* Didn't find a required submodel */
            m_freemin(m_minend, "end-tag minimization") ;
            return(FALSE) ;
            }  
          } /* End for pand */
        } /* End for fsastack */
      /* Have confirmed that the element indicated by stackptr can end now */
      if (stackptr->element == val) break ;
      *m_nextme = (M_MIN *) m_malloc(sizeof(M_MIN), "end-tag minimization") ;
      (*m_nextme)->next = NULL ;
      (*m_nextme)->val = stackptr->element ;
      m_nextme = &(*m_nextme)->next ;
      } /* End for stackptr */
    if (! stackptr) {
      m_freemin(m_minend, "end-tag minimization") ;
      return(FALSE) ;
      }
    for (minend = m_minend ; minend ; ) {
      discard = minend ;
      minend = minend->next ;
      m_free(discard, "end-tag minimization") ;
      if (m_stacktop->neednet && ! neednet) {
	M_WCHAR *wc_net;

	wc_net = MakeWideCharString(m_net);
        m_err2("Expecting %s to end %s",
               wc_net,
	       m_nameofelt(m_stacktop->element)) ;
	m_free(wc_net,"wide character string");
        m_showcurelt() ;
        }
      if (! m_element[m_stacktop->element - 1].emin) {
        m_err1("Missing end tag for %s", m_nameofelt(m_stacktop->element)) ;
        m_showcurelt() ;
        }
      m_endtag(m_stacktop->element) ;
      }
    if (m_stacktop->neednet != neednet) {
      M_WCHAR *wc_etago, *wc_tagc, *wc_mnet, *wc_stago, *wc_net;

      wc_etago = MakeWideCharString(m_etago);
      wc_stago = MakeWideCharString(m_stago);
      wc_tagc  = MakeWideCharString(m_tagc);
      wc_net   = MakeWideCharString(m_net);
      if (neednet)
        m_err4("Expecting %s%s%s instead of %s",
               wc_etago,
	       m_nameofelt(m_stacktop->element),
	       wc_tagc,
	       wc_net) ;
      else
        m_err4("Expecting %s to end %s%s%s",
               wc_net,
	       wc_stago,
	       m_nameofelt(m_stacktop->element),
	       wc_tagc) ;
      m_showcurelt() ;
      m_free(wc_etago,"wide character string");
      m_free(wc_stago,"wide character string");
      m_free(wc_tagc,"wide character string");
      m_free(wc_net,"wide character string");
      }
    m_endtag(val) ;
    return(TRUE) ;
    } /*end m_ckend*/

/* Make a copy of the stack entry at the top of the parse stack in a scratch
   area */
M_PARSE *m_copystackelt(M_NOPAR)
  {
    M_OPENFSA *oldfsa ;
    M_OPENFSA **newfsa ;
    M_ANDLIST *oldand ;
    M_ANDLIST **newand ;
    M_PARSE *copy ;

    copy = (M_PARSE *) m_malloc(sizeof(M_PARSE), "stack element") ;
    memcpy((char *) copy, (char *) m_stacktop, sizeof(M_PARSE)) ;
    copy->param = NULL ;
    for (oldfsa = m_stacktop->fsastack, newfsa = &copy->fsastack ;
         oldfsa ;
         oldfsa = oldfsa->oldtop, newfsa = &(*newfsa)->oldtop) {
      *newfsa = (M_OPENFSA *) m_malloc(sizeof(M_OPENFSA), "FSA") ;
      memcpy((char *) *newfsa, (char *) oldfsa, sizeof(M_OPENFSA)) ;
      for (oldand = oldfsa->usedand, newand = &(*newfsa)->usedand ;
           oldand ;
           oldand = oldand->next, newand = &(*newand)->next) {
        *newand = (M_ANDLIST *) m_malloc(sizeof(M_ANDLIST), "and list") ;
        (*newand)->group = oldand->group ;
        (*newand)->next = NULL ;
        }
      }
    return(copy) ;
    }

/* End of document */
void m_done(M_NOPAR)
  {
    M_ELEMENT lastelt ;

    while (m_stacktop->oldtop) {
      lastelt = m_stacktop->element ;
      if (! m_ckend(m_stacktop->element, FALSE)) {
        m_err1("More content expected in element %s at end of document",
               m_nameofelt(m_stacktop->element)) ;
        m_expecting() ;
        m_showcurelt() ;
        m_frcend(m_stacktop->element) ;
        }
      else if (! m_element[lastelt - 1].emin)
        m_err1("Missing end tag for %s", m_nameofelt(lastelt)) ;
      }
    m_endcase(1) ;
    m_globes() ;
    m_exit(m_errexit) ;
    }

/* Process the endtag (implied, abbreviated, or explicit) for element C*/
void m_endtag(c)
  M_ELEMENT c ;
  {
    m_endaction(c) ;
    m_pop() ;
    if (m_stacktop->intext) {
      m_curcon = POUNDCDATA ;
      if (m_netlevel) m_curcon = NETCDATA ;
      }
    }

/* Check that the identified element is not prohibited in the current context
   by an exclusion exception */
LOGICAL m_excluded(elt)
  M_ELEMENT elt ;
  {
    M_PARSE *stackptr ;
    int except ;

    if (! elt) return(FALSE) ;
    for (stackptr = m_stacktop ;
         stackptr->oldtop ;
         stackptr = stackptr->oldtop)
      for (except = m_element[stackptr->element - 1].exptr ;
           except ;
           except = m_exception[except - 1].next)
      if (m_exception[except - 1].element == elt) return(TRUE) ;
    return(FALSE) ;
    }

/* Free the OPEN FSA substructures associated with an element on
   the parse stack */
void m_freeFSA(stackelt)
  M_PARSE *stackelt ;
  {
    M_OPENFSA *fsastack ;
    M_ANDLIST *usedand ;

    while (stackelt->fsastack) {
      fsastack = stackelt->fsastack ;
      if (fsastack == &m_botfsa) return ;
      while (fsastack->usedand) {
        usedand = fsastack->usedand ;
        fsastack->usedand = usedand->next ;
        m_free(usedand, "and list") ;
        }
      stackelt->fsastack = fsastack->oldtop ;
      m_free(fsastack, "FSA") ;
      }
    }

/* Free storage used for tentative chain of tag minimizations */
void m_freemin(min, msg)
  M_MIN *min ;
  char *msg ;
  {
    M_MIN *discard ;

    for ( ; min ;) {
      discard = min ;
      min = min->next ;
      m_free(discard, msg) ;
      }
    }

/* M_nextand returns TRUE iff element label is allowed at the current point
   in the current content model by starting a new submodel of the and-group
   indicated by fsastack, or (if the and-group is within a seq-group) by
   continuing past the and-group */
LOGICAL m_nextand(thisfsa, label)
  M_OPENFSA *thisfsa ;
  M_ELEMENT label ;
  {
    M_ANDLIST *newgroup ;
    M_ANDGROUP pand ;
    M_ANDLIST *plist ;
    M_OPENFSA *savefsa ;
    M_OPENFSA *fsa ;
    LOGICAL required = FALSE ;
    LOGICAL last ;

    /* Verify currently within an and-group and in final state of this
       branch */
    if (! m_state[thisfsa->current - 1].final) return(FALSE) ;
    if (! thisfsa->oldtop) return(FALSE) ;
    savefsa = m_stacktop->fsastack ;

    /* Check possibility of starting a new branch*/
    m_stacktop->fsastack =
      (M_OPENFSA *) m_malloc(sizeof(M_OPENFSA), "FSA") ;
    m_stacktop->fsastack->oldtop = thisfsa->oldtop ;
    m_stacktop->fsastack->andgroup = M_NULLVAL ;
    m_stacktop->fsastack->usedand = NULL ;
    newgroup = (M_ANDLIST *) m_malloc(sizeof(M_ANDLIST), "and list") ;
    newgroup->next = thisfsa->oldtop->usedand ;
    thisfsa->oldtop->usedand = newgroup ;
    for (pand = thisfsa->oldtop->andgroup ;
         pand ;
         pand = m_andgroup[pand - 1].next) {
      for (plist = newgroup->next ; plist ; plist = plist->next)
        if (pand == plist->group) break ;
      if (! plist) {
        newgroup->group = pand ;
        m_stacktop->fsastack->current = m_andgroup[pand - 1].start ;
        if (! m_state[m_stacktop->fsastack->current - 1].final)
          required = TRUE ;
        if (m_transition(label, FALSE)) {
          for (fsa = savefsa ; TRUE ; fsa = fsa->oldtop) {
            for (plist = fsa->usedand ; plist ; plist = plist->next)
              m_free(plist, "and list") ;
	    if (fsa == thisfsa) {
	      m_free(fsa, "FSA") ;
	      break;
	      }
            m_free(fsa, "FSA") ;
            }
          return(TRUE) ;
          }
        }
      }
  
    /* Couldn't start a new branch.  Restore parse stack */
    thisfsa->oldtop->usedand = newgroup->next ;
    m_free(newgroup, "and list") ;
    m_free(m_stacktop->fsastack, "FSA") ;
    m_stacktop->fsastack = savefsa ;

    /* Have all required branches occurred? */
    if (required) return(FALSE) ;

    /* Can we continue past this and-group in a containing seq-group? */
    m_stacktop->fsastack =
      (M_OPENFSA *) m_malloc(sizeof(M_OPENFSA), "FSA") ;
    m_stacktop->fsastack->oldtop = thisfsa->oldtop->oldtop ;
    m_stacktop->fsastack->andgroup = M_NULLVAL ;
    m_stacktop->fsastack->usedand = NULL ;
    m_stacktop->fsastack->current = thisfsa->oldtop->current ;
    if (m_transition(label, FALSE)) {
      /* Free temporary FSA storage used to test transition */
      for (fsa = savefsa, last = FALSE ; TRUE ; fsa = fsa->oldtop) {
        for (plist = fsa->usedand ; plist ; plist = plist->next)
          m_free(plist, "and list") ;
        m_free(fsa, "FSA") ;
        if (last) break ;
        if (fsa == thisfsa) last = TRUE ;
        }
      return(TRUE) ;
      }
    m_free(m_stacktop->fsastack, "FSA") ;
    m_stacktop->fsastack = savefsa ;

    /* Can we continue in a containing and-group? */
    if (m_nextand(thisfsa->oldtop, label)) return(TRUE) ;
    return(FALSE) ;
    }

/* Pops the parse stack*/
void m_pop(M_NOPAR)
  {
    M_PARSE *stackelt ;

    if (! m_stacktop->oldtop) {
      m_error("Program error: attempt to pop empty stack") ;
      m_exit(TRUE) ;
      }

    if (m_stacktop->map && m_stacktop->map != m_stacktop->oldtop->map)
      m_free(m_stacktop->map, "short reference map") ;
    m_freeparam(m_stacktop) ;
    m_freeFSA(m_stacktop) ;

    if (m_stacktop->neednet) m_netlevel-- ;
    stackelt = m_stacktop ;
    m_stacktop = stackelt->oldtop ;
    m_free(stackelt, "stack element") ;
    }

/* Pushes a new item onto the parse stack, setting its element, current,
   and neednet fields as indicated by the parameters*/
#if defined(M_PROTO)
void m_push(M_ELEMENT elt, M_STATE current, LOGICAL need)
#else
void m_push(elt, current, need)
  M_ELEMENT elt ;
  M_STATE current ;
  LOGICAL need ;
  #endif
{
    M_PARSE *newstack ;

    m_aftereod = FALSE ;
    newstack = (M_PARSE *) m_malloc(sizeof(M_PARSE), "stack element") ;
    newstack->oldtop = m_stacktop ;
    newstack->element = elt ;
    newstack->param = NULL ;
    if (m_element[elt - 1].content == M_REGEXP) {
      newstack->fsastack = (M_OPENFSA *) m_malloc(sizeof(M_OPENFSA), "FSA") ;
      newstack->fsastack->oldtop = NULL ;
      newstack->fsastack->current = current ;
      newstack->fsastack->andgroup = M_NULLVAL ;
      newstack->fsastack->usedand = NULL ;
      }
    else newstack->fsastack = NULL ;
    newstack->thisent = 0 ;
    newstack->neednet = need ;
    newstack->firstre = FALSE ;
    newstack->contextual = TRUE ;
    newstack->intext = FALSE ;
    newstack->linestat = M_NOTHING ;
    newstack->holdre = FALSE ;
    newstack->map = m_stacktop->map ;
    newstack->cdcase = m_stacktop->cdcase ;
    newstack->picase = m_stacktop->picase ;
    newstack->stccase = m_stacktop->stccase ;
    newstack->cdparam = m_stacktop->cdparam ;
    newstack->piparam = m_stacktop->piparam ;
    newstack->stparam = m_stacktop->stparam ;
    newstack->file = m_stacktop->file ;
    newstack->line = m_stacktop->line ;
    newstack->ifdata = NULL ;
    m_stacktop = newstack ;
    if (m_element[elt - 1].srefptr)
      m_setmap(m_element[elt - 1].srefptr,
               (LOGICAL) m_element[elt - 1].useoradd) ;
    }

/* Process first character of a segment of character data.  The first
   character is treated differently so that if character data is not
   allowed in the current context, an error message is issued with the
   first character only and not with every character. */
void m_strtcdata(scanval)
  int scanval ;
  {
    if (! m_strtproc(M_NULLVAL))
      if (m_whitespace((M_WCHAR) scanval)) {
        m_curcon = m_prevcon ;
        return ;
        }
      else {
        if (m_stacktop->oldtop) {
          m_err1("Data characters not allowed at this point in %s",
                 m_nameofelt(m_stacktop->element)) ;
          m_expecting() ;
          m_showcurelt() ;
          }
        else if (! m_start) {
          m_error("Document may not start with data characters") ;
          m_expecting() ;
          }
        }
    m_start = TRUE ;
    m_textaction((M_WCHAR) scanval) ;
    m_stacktop->firstre = TRUE ;
    m_stacktop->intext = TRUE ;
    }

/* M_strtproc checks that the next starttag or beginning of the next
   #PCDATA segment is valid, processing omitted start and endtags as needed.
   (Since m_endtag may reset the current context if the stack is popped down
   to an element that was within #PCDATA, m_strtproc saves the current context
   and restores it after returning from the last call to m_endtag.)
*/
LOGICAL m_strtproc(scanval)
  M_ELEMENT scanval ;
  {
    int check ;
    M_PARSE *original ;
    int savecontext ;
    int savenet ;
    M_PARSE *savestack ;
    M_PARSE *starttagomit ;
    M_MIN *min ;
    M_MIN *discard ;

    /* The algorithms used here involve making a copy of the stack entry
       at the top of the stack before testing for the possibility of
       start-tag omission.  Values of cdparam, piparam, and stparam
       are not used while testing for markup minimization and therefore
       are not set.  However, the original entry and the copy may differ
       in the accuracy of these values, so care must be taken to keep
       the version in which they are correct when the stack is manipulated
       for the final time */

    /* Is scanval allowed without tag omission? */
    savestack = m_stacktop ;
    original = m_stacktop ;
    m_stacktop = m_copystackelt() ;
    if (check = m_checkstart(scanval)) {
      if (scanval && m_stacktop->holdre && check != M_NONCONTEXTUAL) {
        m_freeFSA(m_stacktop) ;
        m_free(m_stacktop, "stack element") ;
        m_stacktop = original ;
        m_holdproc() ;
        return(m_strtproc(scanval)) ;
        }
      m_freeFSA(m_stacktop) ;
      m_free(m_stacktop, "stack element") ;
      m_stacktop = original ;
      if (scanval && check != M_NONCONTEXTUAL) {
        m_stacktop->linestat = M_DCORCET ;
        m_stacktop->firstre = TRUE ;
        }
      m_strttag(scanval, m_scannet) ;
      if (check == M_NONCONTEXTUAL) m_stacktop->contextual = FALSE ;
      else if (scanval) m_stacktop->oldtop->intext = FALSE ;
      return(TRUE) ;
      }
    
    /* Check for start- and end-tag omission */
    savecontext = m_curcon ;
    savenet = m_netlevel ;
    m_minstart = m_minend = NULL ;
    m_nextms = &m_minstart ;
    m_nextme = &m_minend ;
    starttagomit = m_stacktop ;
    while (TRUE) {
      if (m_omitstart()) {
        if (check = m_checkstart(scanval)) break ;
        else continue ;
        }
      m_freemin(m_minstart, "start-tag minimization") ;
      m_minstart = NULL ;
      m_nextms = &m_minstart ;
      while (m_stacktop != starttagomit) m_pop() ;
      m_freeFSA(m_stacktop) ;
      m_free(m_stacktop, "stack element") ;
      m_stacktop = original ;
      if (m_omitend()) {
        original = m_stacktop->oldtop ;
        m_stacktop = m_stacktop->oldtop ;
        m_stacktop = m_copystackelt() ;
        starttagomit = m_stacktop ;
        if (check = m_checkstart(scanval)) break ;
        else continue ;
        }
      m_freemin(m_minend, "end-tag minimization") ;
      m_freemin(m_minstart, "start-tag minimization") ;
      m_stacktop = savestack ;
      m_netlevel = savenet ;
      m_curcon = savecontext ;
      return(FALSE) ;
      }

    /* Have determined a sequence of omitted tags.  Process them */
    /* Undo all stack changes that were made tentatively, so they can
       be redone with invocation of interface as appropriate */
    while (m_stacktop != starttagomit) m_pop() ;
    m_freeFSA(m_stacktop) ;
    m_free(m_stacktop, "stack element") ;
    m_stacktop = savestack ;
    m_netlevel = savenet ;
    if (m_minend) m_stacktop->holdre = FALSE ;
    else if (m_stacktop->holdre && check != M_NONCONTEXTUAL) {
      m_freemin(m_minstart, "start-tag minimization") ;
      m_holdproc() ;
      if (scanval) return(m_strtproc(scanval)) ;
      else return(TRUE) ;
      }
    for (min = m_minend ; min ;) {
      if (m_stacktop->neednet) {
	M_WCHAR *wc_net;

	wc_net = MakeWideCharString(m_net);
        m_err2("Expecting %s to end %s",
               wc_net,
	       m_nameofelt(m_stacktop->element)) ;
	m_free(wc_net,"wide character string");
        m_showcurelt() ;
        }
      if (! m_element[m_stacktop->element - 1].emin) {
        m_err1("Missing end tag for %s", m_nameofelt(m_stacktop->element)) ;
        m_showcurelt() ;
        }
      m_endtag(min->val) ;
      discard = min ;
      min = min->next ;
      m_free(discard, "end-tag minimization") ;
      }
    for (min = m_minstart ; min ;) {
      m_checkstart(min->val) ;
      m_strttag(min->val, FALSE) ;
      if (! m_element[min->val - 1].smin)
        m_err1("Missing start tag for %s", m_nameofelt(min->val)) ;
      m_stkdefaultparams() ;
      discard = min ;
      min = min->next ;
      m_free(discard, "start-tag minimization") ;
      }
    check = m_checkstart(scanval) ;
    if (scanval && check != M_NONCONTEXTUAL) {
      m_stacktop->linestat = M_DCORCET ;
      m_stacktop->firstre = TRUE ;
      }
    m_strttag(scanval, m_scannet) ;
    if (check == M_NONCONTEXTUAL) m_stacktop->contextual = FALSE ;
    else if (scanval) m_stacktop->oldtop->intext = FALSE ;
    m_curcon = savecontext ;
    if (m_element[m_stacktop->element - 1].content == M_CDATA)
      m_curcon = CDATAEL ;
    if (m_element[m_stacktop->element - 1].content == M_RCDATA) {
      m_curcon = RCDATAEL ;
      m_stacktop->thisent = m_eopencnt ;
      }
    return(TRUE) ;
    }

/* Processes explicit or implied start tag*/
#if defined(M_PROTO)
void m_strttag(M_ELEMENT val, LOGICAL net)
#else
void m_strttag(val, net)
  M_ELEMENT val ;
  LOGICAL net ;
#endif
{
    m_transition(val, TRUE) ;
    if (val) {
      m_push(val, m_element[val - 1].start, net) ;
      if (net) m_netlevel++ ;
      if (m_element[val - 1].content == M_CDATA ||
          m_element[val - 1].content == M_RCDATA) {
        m_stacktop->intext = TRUE ;
        m_curcon = m_element[val - 1].content == M_CDATA ?
                   CDATAEL : RCDATAEL ;
        }
      }
    }

/* Returns indication of whether or not parsed data characters are allowed
   (without tag minimization) in the current context.  Used to distinguish
   mixed content from element content. (Assuming the definition of
   mixed content is a context where #PCDATA can occur, not that the current
   content model contains #PCDATA at some point.  The former definition
   makes more sense, is used by MARKUP, and is under consideration by the
   Standards committee; the latter is the current definition in the Standard
   */
LOGICAL m_textpermitted(M_NOPAR)
  {
    M_ANDGROUP pand ;
    M_OPENFSA *fsastack ;
    M_ANDLIST *usedand ;
    LOGICAL morebranches = FALSE ;

    if (! m_stacktop->oldtop) return(FALSE) ;
    /* If element has declared content (other than EMPTY), data is allowed.
       But EMPTY elements don't stay on the stack long enough to call this
       function */
    if (m_element[m_stacktop->element - 1].content != M_REGEXP) return(TRUE) ;
    /* If within #PCDATA, more text can be entered */
    if (m_stacktop->intext) return(TRUE) ;
    /* If current state emits an arc labelled #PCDATA, text can be
       entered */
    for (fsastack = m_stacktop->fsastack ;
         fsastack ;
         fsastack = fsastack->oldtop) {
      for (pand = fsastack->andgroup ;
           pand ;
           pand = m_andgroup[pand - 1].next) {
        for (usedand = fsastack->usedand ;
             usedand ;
             usedand = usedand->next)
          if (usedand->group == pand) break ;
        if (! usedand) {
          if (m_state[m_andgroup[pand - 1].start - 1].datacontent)
            return(TRUE) ;
          if (! m_state[m_andgroup[pand - 1].start - 1].final)
            morebranches = TRUE ;
          }
        }
      if (morebranches) return(FALSE) ; 
      if (m_state[fsastack->current - 1].datacontent) return(TRUE) ;
      if (! m_state[fsastack->current - 1].final) return(FALSE) ;
      }
    return(FALSE) ;
    } /* End m_textpermitted() */

/* Returns TRUE iff LABEL allowed in the current state of the current
   element (without expanding any minimization).  May result in changes
   to the stack of FSA's for this element if and-groups open or close. */
#if defined(M_PROTO)
LOGICAL m_transition(M_ELEMENT label, LOGICAL recur)
#else
LOGICAL m_transition(label, recur)
  M_ELEMENT label ;
  LOGICAL recur ;
#endif
{
    M_ARC parc ;
    M_OPENFSA *newfsa ;
    M_ANDGROUP pand ;

    if (m_stacktop->oldtop &&
        m_element[m_stacktop->element - 1].content != M_REGEXP)
      return(FALSE) ;
    for (parc = m_state[m_stacktop->fsastack->current - 1].first ;
         parc ;
         parc = m_arc[parc - 1].next) {
      if (m_arc[parc - 1].group) {
        newfsa = (M_OPENFSA *) m_malloc(sizeof(M_OPENFSA), "FSA") ;
        newfsa->oldtop = m_stacktop->fsastack ;
        newfsa->andgroup = M_NULLVAL ;
        newfsa->usedand = NULL ;
        m_stacktop->fsastack = newfsa ;
        for (pand = m_arc[parc - 1].group ; pand ;
             pand = m_andgroup[pand - 1].next) {
          newfsa->current = m_andgroup[pand - 1].start ; 
          if (m_transition(label, FALSE)) {
            newfsa->oldtop->andgroup = m_arc[parc - 1].group ;
            newfsa->oldtop->usedand =
              (M_ANDLIST *) m_malloc(sizeof(M_ANDLIST), "and list") ;
            newfsa->oldtop->usedand->group = pand ;
            newfsa->oldtop->usedand->next = NULL ;
            newfsa->oldtop->current = m_arc[parc - 1].to ;
            return(TRUE) ;
            }
          }
        m_stacktop->fsastack = newfsa->oldtop ;
        m_free(newfsa, "FSA") ;
        }
      else if (label == m_arc[parc - 1].label) {
        m_stacktop->fsastack->current = m_arc[parc - 1].to ;
        return(TRUE) ;
        }
      } /* End for parc */
    if (recur && m_nextand(m_stacktop->fsastack, label)) return(TRUE) ;
    return(FALSE) ;
    } /* End transition */

