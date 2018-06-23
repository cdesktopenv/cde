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
/* $XConsortium: minim.c /main/3 1995/11/08 10:54:22 rswiston $ */
/*
              Copyright 1986 Tandem Computers Incorporated.
This product and information is proprietary of Tandem Computers Incorporated.
                   Copyright 1986, 1987, 1988, 1989 Hewlett-Packard Co.
*/

/* Minim.c contains procedures relevant to tag minimization */

#include <stdio.h>
#include <string.h>
#include "basic.h"
#include "trie.h"
#include "dtdext.h"
#include "parser.h"
#include "delim.h"
#include "context.h"

/* M_expecting reports to the user the possible valid content at a particular
   state in the parse of the document */
void m_expecting(void)
  {
    LOGICAL expstart = TRUE ;
    M_PARSE *stackptr ;
    M_OPENFSA *fsastack ;
    M_ANDLIST *usedand ;
    M_ANDGROUP pand ;
    LOGICAL required = FALSE ;
    LOGICAL data = FALSE ;

    m_expcount = 0 ;
    if (m_stacktop->intext) m_expline(&expstart, &data, M_NULLVAL) ;
    for (stackptr = m_stacktop ; stackptr ; stackptr = stackptr->oldtop) {
      if (m_explimit && m_expcount > M_EXPLIMIT) return ;
      if (m_start && ! stackptr->oldtop) return ;
      /* First check for possible start-tags.
         Begin by testing if at start of document or not within a
           CDATA or RCDATA element. */
      if (! stackptr->oldtop ||
          m_element[stackptr->element - 1].content == M_REGEXP) {
        /* Note the following statement, which checks the type of the
           element at the top of the stack, is not a repeat of the previous
           one, which checks the type of an element embedded in the stack.
           The second comparison prevents traversing paths from
           a parent of an RCDATA or CDATA element and still allows displaying
           the end-tag of the parent */
        if (! stackptr->oldtop ||
            m_element[m_stacktop->element - 1].content == M_REGEXP)
          for (fsastack = stackptr->fsastack ;
               fsastack ;
               fsastack = fsastack->oldtop) {
            for (pand = fsastack->andgroup ;
                 pand ;
                 pand = m_andgroup[pand - 1].next) {
              for (usedand = fsastack->usedand ;
                   usedand ;
                   usedand = usedand->next) 
                if (usedand->group == pand) break ;
              if (! usedand)
                m_expexpand(&expstart, m_andgroup[pand - 1].start, &required,
                            &data) ;
              }
            if (required) return ;
            m_expexpand(&expstart, fsastack->current, &required, &data) ;
            if (! m_state[fsastack->current - 1].final) return ;
            }
        }
      else if (m_element[stackptr->element - 1].content == M_CDATA ||
               m_element[stackptr->element - 1].content == M_RCDATA)
        m_expline(&expstart, &data, M_NULLVAL) ;
      if (m_explimit && m_expcount > M_EXPLIMIT) return ;
      /* Now report the end-tag */
      m_exptend(&expstart, stackptr) ;
      if (! m_element[stackptr->element - 1].emin) return ;
      }
    }

/* Recursive procedure first called from expecting() to display
   names of elements reachable from a particular node */
void m_expexpand(LOGICAL *expstart, M_STATE node, LOGICAL *required, LOGICAL *data)
  {
    M_ARC parc ;
    M_ANDGROUP pand ;

    for (parc = m_state[node - 1].first ;
         parc ;
         parc = m_arc[parc - 1].next) {
      if (m_explimit && m_expcount > M_EXPLIMIT) return ;
      if (m_arc[parc - 1].group)
        for (pand = m_arc[parc - 1].group ;
             pand ;
             pand = m_andgroup[pand - 1].next)
          m_expexpand(expstart, m_andgroup[pand - 1].start, required, data) ;
      else {
        if (! m_state[node - 1].final) *required = TRUE ;
        m_expline(expstart, data, m_arc[parc - 1].label) ;
        }
      }
    }

/* M_expline writes one line for m_expecting() */
void m_expline(LOGICAL *expstart, LOGICAL *data, M_ELEMENT label)
  {
    char buffer[M_NAMELEN + 2*MAXD + 1] ;

    if (! label && *data) return ;
    if (m_excluded(label)) return ;
    if (*expstart) {
      sprintf(buffer, "Expecting ") ;
      m_errline(buffer) ;
      *expstart = FALSE ;
      }
    else {
      sprintf(buffer, "   or     ") ;
      m_errline(buffer) ;
      }
    if (m_explimit && m_expcount == M_EXPLIMIT) {
      sprintf(buffer, ". . .\n") ;
      m_errline(buffer) ;
      }
    else if (label) {
      char *mb_enptr;

      mb_enptr = MakeMByteString(&m_ename[m_element[label - 1].enptr]);
      sprintf(buffer, "%s%s%s\n", m_stago, mb_enptr, m_tagc) ;
      m_free(mb_enptr,"multi-byte string");
      m_errline(buffer) ;
      }
    else {
      sprintf(buffer, "data characters\n") ;
      m_errline(buffer) ;
      *data = TRUE ;
      }
    m_expcount++ ;
    }

/* M_exptend is called from m_expecting to inform the user after an
   error if an end tag is permitted */
void m_exptend(LOGICAL *expstart, M_PARSE *stackptr)
  {
    char buffer[M_NAMELEN + 2*MAXD + 1] ;

    if (*expstart) {
      sprintf(buffer, "Expecting ") ;
      m_errline(buffer) ;
      *expstart = FALSE ;
      }
    else {
      sprintf(buffer, "   or     ") ;
      m_errline(buffer) ;
      }
    if (m_explimit && m_expcount == M_EXPLIMIT) {
      sprintf(buffer, ". . .\n") ;
      m_errline(buffer) ;
      }
    else if (stackptr->neednet) {
      sprintf(buffer, "%s\n", m_net) ;
      m_errline(buffer) ;
      }
    else {
      char *mb_enptr;

      mb_enptr =
	  MakeMByteString(&m_ename[m_element[stackptr->element - 1].enptr]);
      sprintf(buffer, "%s%s%s\n", m_etago, mb_enptr, m_tagc) ;
      m_free(mb_enptr,"multi-byte string");
      m_errline(buffer) ;
      }
    m_expcount++ ;
    }

/* M_findunique is used to test for start tag minimization.  If the current
   parse state permits at least one element with explicit start-tag
   minimization, the left-most such element to occur in the content model
   is returned.  Otherwise, the contextually-required element, if any,
   is returned.  Finally, if the parse state permits a unique valid element,
   and the flag for conformance to ISO 8879 is not set, the unique valid
   element is returned by m_findunique.

   Before returning, m_findunique verifies that the element to be returned
   permits start-tag minimization.  If not, the value is returned only if
   conformance to ISO 8879 is set.

   Actually m_findunique returns 1 greater than the index of the unique
   element, 1 if character data is expected, and 0 (FALSE) if there is
   no unique element. 
*/
M_ELEMENT m_findunique(M_STATE from, int *newleft)
  {
    M_ARC parc ;
    M_ELEMENT cr = 0, minim = 0;
    int leftmost = M_BIGINT ;
    int testleft = M_BIGINT ;
    int testminim ;
    M_ANDGROUP pand ;

    for (parc = m_state[from - 1].first ;
         parc ;
         parc = m_arc[parc - 1].next) {
      if (m_arc[parc - 1].group) {
        if (! m_conform)
          for (pand = m_arc[parc - 1].group ;
               pand ;
               pand = m_andgroup[pand - 1].next) {
            testminim = m_findunique(m_andgroup[pand - 1].start, &testleft) ;
            if (testminim && testleft < leftmost) {
              minim = testminim ;
              leftmost = testleft ;
              }
            }
        }
      else {
        if (! m_conform) {
          if (m_arc[parc - 1].minim &&
              m_arc[parc - 1].minim < leftmost &&
              ! m_excluded(m_arc[parc - 1].label)) {
            /* Save the explicitly minimizable element plus its position
               in the content model */
            leftmost = m_arc[parc - 1].minim ;
            minim = m_arc[parc - 1].label + 1 ;
            } /* End arc.minim > leftmost */
          else if (m_arc[parc - 1].optional &&
                   parc == m_state[from - 1].first &&
                   ! m_arc[parc - 1].next &&
                   m_element[m_arc[parc - 1].label -1].smin &&
                   ! m_excluded(m_arc[parc - 1].label))
            /* Save the only element that can occur */
            cr = m_arc[parc - 1].label ;
          } /* End if (! m_conform) */
        /* Save the contextually-required element */
        if (! m_arc[parc - 1].optional && ! m_excluded(m_arc[parc - 1].label))
          cr = m_arc[parc - 1].label ;
        } /* End if (! group) */
      } /* End for parc */
    *newleft = leftmost ;
    if (minim) return(minim) ;
    if (cr) return(cr + 1) ;
    return(FALSE) ;
    }

/* M_nullendtag is called when a null end tag is encountered; i.e., at the
   end of a short element */
void m_nullendtag(void)
  {
    LOGICAL foundnet ;

    while (m_stacktop->oldtop) {
      foundnet = m_stacktop->neednet ;
      if (! foundnet && ! m_element[m_stacktop->element - 1].emin) {
        m_err1("Missing end tag for %s",
	       m_nameofelt(m_stacktop->element)) ;
        m_showcurelt() ;
        }
      if (! m_ckend(m_stacktop->element, foundnet)) {
	M_WCHAR *wc_found;

	wc_found = MakeWideCharString(foundnet ? "Null" : "Implied");
        m_err2("%s end tag for %s unexpected",
	       wc_found,
	       m_nameofelt(m_stacktop->element)) ;
	m_free(wc_found,"wide character string");
        m_expecting() ;
        m_showcurelt() ;
        m_frcend(m_stacktop->element) ;
        }
      if (foundnet) return ;
      }
    m_error("Internal error: Invalid stack in Nullendtag") ;
    m_exit(TRUE) ;
    }

/* Tests to see if an end tag may have been omitted at this point in the
   parse.*/
LOGICAL m_omitend(void)
  {
    M_ANDGROUP pand ;
    M_OPENFSA *fsastack ;
    M_ANDLIST *usedand ;

    if (! m_stacktop->oldtop) return(FALSE) ;
    if (m_element[m_stacktop->element - 1].content != M_REGEXP) return(TRUE) ;
    for (fsastack = m_stacktop->fsastack ;
         fsastack ;
         fsastack = fsastack->oldtop) {
      for (pand = fsastack->andgroup ;
           pand ;
           pand = m_andgroup[pand - 1].next) {
        /* Doesn't matter if optional submodel of and-group has occurred */
        if (m_state[m_andgroup[pand - 1].start - 1].final) continue ;
        for (usedand = fsastack->usedand ;
             usedand ;
             usedand = usedand->next)
          if (usedand->group == pand) break ;
        /* Required submodel of and-group has not occurred */
        if (! usedand) return(FALSE) ;
        }
      /* Current FSA not in final state */
      if (! m_state[fsastack->current - 1].final) return(FALSE) ;
      }
    *m_nextme = (M_MIN *) m_malloc(sizeof(M_MIN), "end-tag minimization") ;
    (*m_nextme)->val = m_stacktop->element ;
    (*m_nextme)->next = NULL ;
    m_nextme = &(*m_nextme)->next ;
    return(TRUE) ;
    }

/* Tests to see if a start tag may have been omitted at this point of
   the parse.  If so, saves the element name in the MINVAL array*/
LOGICAL m_omitstart(void)
  {
    M_ELEMENT c = M_NULLVAL ;

    /* int par ;  (used in commented-out code below) */
    M_OPENFSA *fsastack ;
    M_ANDLIST *usedand ;
    M_ANDGROUP pand ;
    int leftmost = M_BIGINT ;
    int newleft = M_BIGINT ;
    M_ELEMENT newc = M_NULLVAL ;
    LOGICAL required = FALSE ;
    M_MIN *min ;

    /* Make sure are in an element that has a content model */
    if (m_stacktop->oldtop &&
        m_element[m_stacktop->element - 1].content != M_REGEXP)
      return(FALSE) ;

    /* Test for unique element expected, or only allowed token is #PCDATA */
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
          if (! m_state[m_andgroup[pand - 1].start - 1].final)
            required = TRUE ;
          newc = m_findunique(m_andgroup[pand - 1].start, &newleft) ;
          if (newleft < leftmost) {
            leftmost = newleft ;
            c = newc ;
            }
          }
        }
      if (! required) {
        newc = m_findunique(fsastack->current, &newleft) ;
        if (newleft < leftmost) {
          leftmost = newleft ;
          c = newc ;
          }
        }
      if (c > 1) break ;
      if (fsastack == m_stacktop->fsastack && newc) {
        c = newc ;
        break ;
        }
      if (m_conform) return(FALSE) ;
      if (! m_state[fsastack->current - 1].final) return(FALSE) ;
      }
    if (! c) return(FALSE) ;

    /* Have found a unique element.  Can its start-tag be omitted? */
    c-- ;
    if (m_element[c - 1].content == M_NONE) return(FALSE) ;
    if (m_element[c - 1].content == M_CDATA) return(FALSE) ;
    if (m_element[c - 1].content == M_RCDATA) return(FALSE) ;

    /* Following code allows start-tag to be omitted only if all required
       parameters are specified:
    for (par = m_element[c - 1].parptr ; par ;
         par = m_parameter[par - 1].next)
      if (m_parameter[par - 1].deftype == M_REQUIRED) return(FALSE) ;
    */

    /* Check for recursive sequences of omitted tags */
    for (min = m_minstart ; min ; min = min->next)
      if (c == min->val) return(FALSE) ;

    m_push(c, m_element[c - 1].start, FALSE) ;
    *m_nextms = (M_MIN *) m_malloc(sizeof(M_MIN), "start-tag minimization") ;
    (*m_nextms)->val = m_stacktop->element ;
    (*m_nextms)->next = NULL ;
    m_nextms = &(*m_nextms)->next ;
    return(TRUE) ;
    }
