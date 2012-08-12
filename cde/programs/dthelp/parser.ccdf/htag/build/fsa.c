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
/* $XConsortium: fsa.c /main/3 1995/11/08 11:12:14 rswiston $ */
/*
              Copyright 1986 Tandem Computers Incorporated.
This product and information is proprietary of Tandem Computers Incorporated.
                   Copyright (c) 1986, 1987, 1988, 1989 Hewlett-Packard Co.
*/

/* Fsa.c contains the procedures used by program BUILD to convert a tree
   representation of a content model to an FSA */

#include <stdlib.h>
#include "build.h"
#include "context.h"
#include "delim.h"

/* Addarc adds an arc from FSA state <from> to state <to> setting other
   fields as indicated by the other parameters.*/
#if defined(M_PROTO)
int addarc(STATE *from, STATE *to, ELTSTRUCT *label, ANDGROUP *and, LOGICAL optional, int id, LOGICAL minim, ELTSTRUCT **errelt)
#else
int addarc(from, to, label, and, optional, id, minim, errelt)
  STATE *from, *to ;
  ELTSTRUCT *label ;
  ANDGROUP *and ;
  LOGICAL optional ;
  int id ;
  LOGICAL minim ;
  ELTSTRUCT **errelt ;
#endif
  {
    ARC *parc, *qarc ;
    int determ ;

    determ = checkdfsa(from, label, and, id, errelt) ;
    parc = from->first ;
    qarc = (ARC *) m_malloc(sizeof(ARC), "arc") ;
    from->first = qarc ;
    qarc->label = label ;
    qarc->optional = optional ;
    qarc->minim = minim ;
    qarc->group = and ;
    qarc->to = to ;
    qarc->next = parc ;
    qarc->id = id ;
    return(determ) ;
    }

/*checkand is used to verify nondeterminism from start and final states
  of FSA's generated from and groups*/
void checkand(andstart, andptr, start, root, errelt)
  ANDGROUP *andstart, *andptr ;
  STATE *start ;
  TREE *root ;
  ELTSTRUCT **errelt ;    
  {
    ARC *parc ;
    ANDGROUP *pand ;
    int c ;

    for (parc = start->first ; parc ; parc = parc->next) {
      if (parc->group) {
        if (parc->group != andstart)
          for (pand = parc->group ; pand ; pand = pand->next)
            checkand(andstart, andptr, pand->start, root, errelt) ;
        }
      else if (c = checkdfsa(andptr->start,
                             parc->label,
                             parc->group,
                             parc->id,
                             errelt))
        nondeterm(root, c, *errelt) ;
      }
    }

/*Checkdfsa is called when adding an arc to an FSA in order to verify that
no existing arc from the same state (or from a start state of an and-group
FSA labelling an arc from the same state) has the same label. */
int checkdfsa(from, label, and, id, errelt)
  STATE *from ;
  ELTSTRUCT *label ;
  ANDGROUP *and ;
  int id ;
  ELTSTRUCT **errelt ;
  {
    int c ;
    ARC *parc ;
    ANDGROUP *group ;

    for (parc = from->first ; parc ; parc = parc->next) {
      if (parc->group) {
        if (and == parc->group) return(ANDCONFLICT) ;
        for (group = parc->group ; group ; group = group->next)
          if (c = checkdfsa(group->start, label, and, id, errelt))
            return(c) ;
        }
      else if (! and && label == parc->label && parc->id != id) {
        if (label) {
          *errelt = label ;
          return(ELTCONFLICT) ;
          }
        return(DATACONFLICT) ;
        }
      }
    return(FALSE) ;
    }

/* Check use of repeated models with and groups */
int checkrepeat(from, and, errelt)
  STATE *from ;
  ANDGROUP *and ;
  ELTSTRUCT **errelt ;
  {
    ARC *parc ;
    int c ;

    for (; and ; and = and->next)
      for (parc = and->start->first ; parc ; parc = parc->next) {
        if (parc->group)
          if (c = checkrepeat(from, parc->group, errelt)) return(c) ;
          else ;
        else
          if (c = checkdfsa(from,
                            parc->label,
                            M_NULLVAL,
                            parc->id,
                            errelt))
            return(c) ;
          else ;
        }
    return(FALSE) ;
    }

/* Copyintolist copies one list of states into another */
void copyintolist(from, to)
  STATELIST *from, **to ;
  {
    STATELIST **new, *old ;

    old = *to ;
    new = to ;
    for ( ; from ; from = from->next) {
      if (notinlist(from, old)) {
        *new = (STATELIST *) m_malloc(sizeof(STATELIST), "state list") ;
        (*new)->value = from->value ;
        (*new)->level = from->level ;
        new = &(*new)->next ;
        }
      }
    *new = old ;
    }

/* Dellist deletes a list of states */
void dellist(list)
  STATELIST **list ;
  {
    STATELIST *p, *q ;

    for (p = *list ; p ; ) {
      q = p ;
      p = p->next ;
      m_free(q, "state list") ;
      }
    *list = NULL ;
    }

/* Delstartarcs deletes the contents of the starta list of arcs from start
   states of a submodel */
void delstartarcs(M_NOPAR)
  {
    ARC *arcptr ;
    ARC *discard ;

    for (arcptr = top->starta ; arcptr ; ) {
      discard = arcptr ;
      arcptr = arcptr->next ;
      m_free(discard, "arc") ;
      }
    top->starta = NULL ;
    }

/* Getand allocates and initializes a new andgroup structure */
ANDGROUP *getand(M_NOPAR)
  {
    ANDGROUP *new ;

    new = (ANDGROUP *) m_malloc(sizeof(ANDGROUP), "and group") ;
    new->nextptr = new->next = NULL ;
    new->count = ++andused ;
    *nextand = new ;
    nextand = &new->nextptr ;
    return(new) ;
    }    

/* Getstate obtains an FSA state */
STATE *getstate(M_NOPAR)
  {
    STATE *new ;

    new = (STATE *) m_malloc(sizeof(STATE), "state") ;
    new->final = FALSE ;
    new->datacontent = FALSE ;
    new->frompcdata = FALSE ;
    new->first = NULL ;
    new->count = ++stateused ;
    new->next = NULL ;
    *nextstate = new ;
    nextstate = &new->next ;
    return(new) ;
    }

/* Makeand processes a submodel whose connector is & */
void makeand(canbenull, root, optional)
  LOGICAL *canbenull ;
  TREE *root ;
  int optional ;
  {
    TREE *child ;
    STATELIST *start, *final ;
    LOGICAL groupbenull ;
    ANDGROUP *andptr, *saveand, *otherand ;
    STATELIST *index ;
    ELTSTRUCT *errelt ;

    for (child = root->first ; child ; child = child->right) {
      if (child == root->first) {
        *canbenull = TRUE ;
        andptr = getand() ;
        saveand = andptr ;
        push() ;
        copyintolist(top->oldtop->allfinal, &top->allfinal) ;
        }
      else {
        andptr->next = getand() ;
        andptr = andptr->next ;
        }
      andptr->start = startfsa(child, &groupbenull) ;
      if (andptr->start->datacontent)
        for (index = top->oldtop->starts ; index ; index = index->next)
          index->value->datacontent = TRUE ;
      if (! groupbenull) *canbenull = FALSE ;
      /* Check for ambiguity between start state of branch just completed
         and start state of the and-group (i.e., parent of branch just
         completed) */
      for (start = top->oldtop->starts ; start ; start = start->next)
        checkand(saveand, andptr, start->value, root, &errelt) ;
      /* Check for ambiguity between start state of branch just completed
         and final states of previous branches */
      for (final = top->allfinal ; final ; final = final->next)
        checkand(saveand, andptr, final->value, root, &errelt) ;
      copyintolist(top->finals, &top->allfinal) ;
      copyintolist(top->newfinal, &top->allfinal) ;
      copyintolist(top->finals, &top->oldtop->newfinal) ;
      copyintolist(top->newfinal, &top->oldtop->newfinal) ;
      dellist(&top->finals) ;
      dellist(&top->newfinal) ;
      /* Check for ambiguity between start states of branch just completed
         and start states of previous branches */
      for (otherand = saveand ; otherand != andptr ;
           otherand = otherand->next)
        checkand(saveand, andptr, otherand->start, root, &errelt) ;
      }
    pop() ;
    if (*canbenull) optional = stacklevels + 1 ;
    simplebranch(root, M_NULLVAL, saveand, optional) ;
    if (*canbenull) copyintolist(top->starts, &top->finals) ;
    for (final = top->finals ; final ; final = final->next)
      final->level = stacklevels ;
    }

/* Makefsa builds the portion of an FSA that corresponds to an entire
   submodel (i.e., a subtree of the tree representation of the rule).
   The value returned indicates whether the submodel can be null (i.e.,
   whether all its elements are optional).  The parameters are a pointer
   to the root of the subtree and an integer indicating the level of
   nesting (if any) of submodels at which the subtree became optional.
   Note that as used here, "optional" means "not contextually required" in
   the terminology of the Standard rather than "contextually optional". 

   Makefsa is a recursive procedure.  As the FSA is built, a stack is
   maintained of the nested content models that have been encountered
   but not yet terminated.  For each open model on the stack, starts is
   a list of its start states (i.e., FSA states from which transitions
   correspond to elements which can occur at the beginning of the rule
   being processed), finals is a list of its final states, starta is a
   list of arcs emanating from the start states of the model, and
   allfinal and newfinal are lists of final states used in checking for
   determinism when and-groups are used.  In more detail, allfinal is a
   list of final states of FSA's in and-groups that may occur just prior
   to the current context; i.e., 1) when starting a new FSA in an and-group,
   the set of final states of already-constructed FSA's in the same group
   (or final states of FSA's in submodel and-groups that end such FSA's)
   or 2) the set of final states of FSA's in an and-group that precedes
   the current context (e.g., the final states of the and-group FSA's
   when processing 'x' in ((a&b),x)). At each stage in the parse (or level
   on the stack), newfinal is the set of states to be added to those in
   allfinal as a result of processing at that level.  Information in
   allfinal is passed from model to submodel; information in newfinal
   goes from submodel to model.
   */
LOGICAL makefsa(root, optional)
  TREE *root ;
  int optional ;
  {
    LOGICAL canbenull ;

    canbenull = FALSE ;
    if (root->occurrence == OPT || root->occurrence == REP)
      optional = stacklevels + 1 ;
    /* The branch consists of a single element name */
    if (root->terminal)
      simplebranch(root, root->value, M_NULLVAL, optional) ;
    /* The submodel's connector is SEQ (,) */
    else if (root->connector == SEQ)
      makeseq(&canbenull, root, optional) ;
    /* The submodel's connector is OR (|) */
    else if (root->connector == OR)
      makeor(&canbenull, root) ;
    /* The submodel's connector is AND (&) */
    else if (root->connector == AND)
      makeand(&canbenull, root, optional) ;
    /* The submodel is a single item in parentheses */
    else canbenull = makefsa(root->first, optional) ;

    /* The FSA is built, now repeat if occurrence indicator so indicates */
    if (root->occurrence == OPT || root->occurrence == REP) canbenull = TRUE ;
    if (root->occurrence == OPT) copyintolist(top->starts, &top->finals) ;
    else if (root->occurrence == REP) {
      repeat(root) ;
      copyintolist(top->starts, &top->finals) ;
      }
    else if (root->occurrence == PLUS) repeat(root) ;
    return(canbenull) ;
    }

/* Makeor processes a submodel whose connector is | */
void makeor(canbenull, root)
  LOGICAL *canbenull ;
  TREE *root ;
  {
    TREE *child ;
    STATELIST *final ;

    push() ;
    copyintolist(top->oldtop->starts, &top->starts) ;
    copyintolist(top->oldtop->allfinal, &top->allfinal) ;
    for (child = root->first ; child ; child = child->right) {
      if (makefsa(child, stacklevels)) *canbenull = TRUE ;
      savestartarcs() ;
      delstartarcs() ;
      copyintolist(top->finals, &top->oldtop->finals ) ;
      dellist(&top->finals) ;
      }
    copyintolist(top->newfinal, &top->oldtop->newfinal) ;
    pop() ;
    for (final = top->finals ; final ; final = final->next)
      final->level = stacklevels ;
    }

/* Makeseq processes a submodel whose connector is , */
void makeseq(canbenull, root, optional)
  LOGICAL *canbenull ;
  TREE *root ;
  int optional ;
  {
    LOGICAL branchnull ;
    STATELIST *keepfinal = NULL, *final ;
    TREE *child ;

    push() ;
    *canbenull = TRUE ;
    copyintolist(top->oldtop->starts, &top->starts) ;
    copyintolist(top->oldtop->allfinal, &top->allfinal) ;
    for (child = root->first ; child ; child = child->right) {
      branchnull = makefsa(child, optional) ;
      if (*canbenull) savestartarcs() ;
      if (! branchnull) {
        *canbenull = FALSE ;
        dellist(&top->allfinal) ;
        dellist(&keepfinal) ;
        }
      copyintolist(top->newfinal, &top->allfinal) ;
      copyintolist(top->newfinal, &keepfinal) ;
      dellist(&top->newfinal) ;
      delstartarcs() ;
      dellist(&top->starts) ;
      copyintolist(top->finals, &top->starts) ;
      dellist(&top->finals) ;
      if (! child->occurrence || child->occurrence == PLUS)
        optional = FALSE ;
      }
    copyintolist(top->starts, &top->oldtop->finals) ;
    copyintolist(keepfinal, &top->oldtop->newfinal) ;
    dellist(&keepfinal) ;
    pop() ;
    for (final = top->finals ; final ; final = final->next)
      final->level = stacklevels ;
    }

/* Nondeterm issues a diagnostic when a nondeterministic model is
   encountered */
void nondeterm(root, c, eltp)
  TREE *root ;
  int c ;
  ELTSTRUCT *eltp ;
  {
  M_WCHAR *wtemp;

    switch (c) {
      case ANDCONFLICT:
	wtemp = MakeWideCharString(and);
        warning2("Error in model for %s: Conflict in use of '%s'",
                 thisrule,
		 wtemp) ;
	m_free(wtemp, "wide character string");
        break ;
      case DATACONFLICT:
	wtemp = MakeWideCharString(rnicdata);
        warning2("Error in model for %s: Conflict in use of '%s'",
                 thisrule,
		 wtemp) ;
	m_free(wtemp, "wide character string");
        break ;  
      case ELTCONFLICT:
        warning2("Error in model for %s: Conflict in use of '%s'",
                 thisrule,
		 eltp->enptr) ;
        break ;
      }
    regenerate(ruletree, root) ;
    msgline(" . . .\n") ;
    }

/* Notinlist returns TRUE iff item is not in list.  If item is in list,
   it makes sure that the stored nesting level is the smaller of the two */
LOGICAL notinlist(item, list)
  STATELIST *item, *list ;
  {
    for ( ; list ; list = list->next)
      if (list->value == item->value) {
        if (item->level < list->level) list->level = item->level ;
        return(FALSE) ;
        }
    return(TRUE) ;
    }

/* Returns true if the arc is labeled #PCDATA or with an and-group that
   has an arc labelled #PCDATA from a start state */
LOGICAL permitspcd(a)
  ARC *a ;
  {
    ANDGROUP *pand ;
    ARC *b ;

    if (a->group) {
      for (pand = a->group ; pand ; pand = pand->next)
        for (b = pand->start->first ;
             b ;
             b = b->next)
          if (permitspcd(b)) return(TRUE) ;
      return(FALSE) ;
      }
    /* Not an and-group */
    if (a->label) return(FALSE) ;
    return(TRUE) ;
    }

/* Pop pops the submodel stack when the end of the current submodel is
   encountered */
void pop(M_NOPAR)
  {
    STACK *discard ;

    dellist(&top->starts) ;
    dellist(&top->finals) ;
    dellist(&top->allfinal) ;
    dellist(&top->newfinal) ;
    delstartarcs() ;
    stacklevels-- ;
    discard = top ;
    top = top->oldtop ;
    m_free((M_POINTER) discard, "stack entry") ;
    }

/* Push pushes the submodel stack when a new group is encountered */
void push(M_NOPAR)
  {
    STACK *new ;

    new = (STACK *) m_malloc(sizeof(STACK), "stack entry") ;
    new->oldtop = top ;
    top = new ;
    stacklevels++ ;
    top->starts = top->finals = top->newfinal = top->allfinal = NULL ;
    top->starta = M_NULLVAL ;
    }

/* Regenerate is used in error processing to print the portion of a grammar
   rule preceding an error */
LOGICAL regenerate(start, stop)
TREE *start, *stop ;
{
TREE *child ;

if (start == stop) return(TRUE) ;

if (start->terminal)
    {
    char *mb_enptr;

    if (start->value)
	mb_enptr = MakeMByteString(start->value->enptr);
    else
	mb_enptr = NULL;
    msg1line("%s", mb_enptr ? mb_enptr : rnicdata) ;
    if (mb_enptr)
	m_free(mb_enptr,"multi-byte string");
    }
else
    {
    msgline("(") ;
    for (child = start->first ; child ; child = child->right)
	{
	if (regenerate(child, stop)) return(TRUE) ;
	if (child->right)
	    {
	    if (start->connector == SEQ) msg1line("%s", seq) ;
	    if (start->connector == OR) msg1line("%s", or) ;
	    if (start->connector == AND) msg1line("%s", and) ;
	    }
	}
    msgline(")") ;
    }

if (start->occurrence == OPT) msg1line("%s", opt) ;
if (start->occurrence == PLUS) msg1line("%s", plus) ;
if (start->occurrence == REP) msg1line("%s", rep) ;

return(FALSE) ;
}

/* Repeat is called after a partial FSA is built for a submodel whose
   occurrence indicator is RPT (*) or PLUS (+).  It handles repetition
   by adding arcs from all the final states to all the states reachable
   in one transition from a start state, labelling them as arcs from
   start states are labelled. */
void repeat(root)
  TREE *root ;
  {
    STATELIST *final ;
    ARC *a ;
    int c ;
    ELTSTRUCT *errelt ;
    M_WCHAR *wtemp;

    copyintolist(top->newfinal, &top->allfinal) ;
    dellist(&top->newfinal) ;
    for (a = top->starta ; a ; a = a->next) {
      for (final = top->allfinal ; final ; final = final->next) {
        if (a->group)
          if (c = checkrepeat(final->value, a->group, &errelt)) {
	    wtemp = MakeWideCharString(root->occurrence == PLUS ? plus : rep);
            warning1("Conflict in use of %s", wtemp);
	    m_free(wtemp, "wide character string");
            nondeterm(root, c, errelt) ;
            }
          else
            ;
        else
          if (c = checkdfsa(final->value,
                            a->label,
                            a->group,
                            a->id,
                            &errelt))
            nondeterm (root, c, errelt) ;
          else
            ;
        }
      for (final = top->finals ; final ; final = final->next) {
        if (samelabelarc(a, final->value)) continue ;
        if (a->group)
          if (c = checkrepeat(final->value, a->group, &errelt))
            nondeterm(root, c, errelt) ;
        if (a->label ||
            a->group ||
            ! final->value->frompcdata) {
          if (c = addarc(final->value, a->to, a->label,
                         a->group, TRUE, a->id,
                         a->minim, &errelt))
            nondeterm(root, c, errelt) ;
          if (permitspcd(a)) final->value->datacontent = TRUE ;
          }
        }
      }
    }

/* Used during processing of occurrence indicators in content models such
   as (a+)+ to prohibit duplicate arcs */
LOGICAL samelabelarc(a, s)
  ARC *a ;
  STATE *s ;
  {
    ARC *b ;

    for (b = s->first ; b ; b = b->next)
      if (b->id == a->id) return(TRUE) ;
    return(FALSE) ;
    }

/* Saves the name of an element appearing on the left-hand side of a
   grammar rule */
#if defined(M_PROTO)
void savelhs(LOGICAL param)
#else
void savelhs(param)
  LOGICAL param ;
#endif
  {
    STATE *end ;
    ELTSTRUCT *errelt ;
    ELTSTRUCT *thiselt ;

    *nextlhs = (LHS *) m_malloc(sizeof(LHS), "lhs") ;
    (*nextlhs)->next = NULL ;
    thiselt = ntrelt(name) ;
    (*nextlhs)->elt = thiselt ;
    nextlhs = &(*nextlhs)->next ;
    if (! startstate) {
      startstate = getstate() ;
      end = getstate() ;
      addarc(startstate, end, thiselt, M_NULLVAL, FALSE, 1, FALSE, &errelt) ;
      end->final = TRUE ;
      }
    if (param && thiselt->parptr) {
      m_err1("Parameters for %s already defined", thiselt->enptr) ;
      return ;
      }
    if (! param && thiselt->model)
      warning1("Duplicate model for element %s", thiselt->enptr) ;
    }

/* Called when arcs are added to the start state of a submodel that is
   also a start state of the parent model to set the parent model's
   starta list */
void savestartarcs(M_NOPAR)
  {
    ARC *carcptr, *parcptr ;

    for (carcptr = top->starta ; carcptr ; carcptr = carcptr->next) {
      parcptr = (ARC *) m_malloc(sizeof(ARC), "arc") ;
      parcptr->label = carcptr->label ;
      parcptr->optional = carcptr->optional ;
      parcptr->minim = carcptr->minim ;
      parcptr->group = carcptr->group ;
      parcptr->to = carcptr->to ;
      parcptr->next = top->oldtop->starta ;
      parcptr->id = carcptr->id ;
      top->oldtop->starta = parcptr ;
      }
    }

/* Simplebranch adds a new state and transition to it in an FSA when a
   submodel consists of a single element or of an and group */
void simplebranch(root, value, group, optional)
  TREE *root ;
  ELTSTRUCT *value ;
  ANDGROUP *group ;
  int optional ;
  {
    STATE *new = NULL ;
    STATELIST *index ;
    int c ;
    ELTSTRUCT *errelt ;

    /* Check for ambiguity between an arc to be added and arcs from final
       states of and-groups that terminate at the start state of the new
       arc */       
    for (index = top->allfinal ; index ; index = index->next)
      if (c = checkdfsa(index->value, value, group, root->eltid, &errelt))
        nondeterm(root, c, errelt) ;
    for (index = top->starts ; index ; index = index->next) {
      if (! group && ! value && index->value->frompcdata)
        continue ;
      if (! new) {
        new = getstate() ;
        new->frompcdata = (LOGICAL) (! group && ! value) ;
        }
      c = addarc(index->value, new, value, group,
                 (LOGICAL) (optional > index->level),
                 root->eltid, root->minim, &errelt) ;
      if (c) nondeterm(root, c, errelt) ;
      if (! group && ! value) index->value->datacontent = TRUE ;
      }
    if (new) {
      top->finals = (STATELIST *) m_malloc(sizeof(STATELIST), "state list") ;
      top->finals->value = new ;
      top->finals->level = stacklevels ;
      top->finals->next = NULL ;
      top->starta = (ARC *) m_malloc(sizeof(ARC), "arc") ;
      top->starta->label = value ;
      top->starta->optional = FALSE ;
      top->starta->minim = root->minim ;
      top->starta->group = group ;
      top->starta->to = new ;
      top->starta->next = NULL ;
      top->starta->id = root->eltid ;
      }
    else copyintolist(top->starts, &top->finals) ;
    }

/* Startfsa creates a new FSA.  It is called once for each content model and
   once for each and group.  Its parameters are the root of the
   subtree in the tree representing the grammar rule being processed and
   the pointer to a flag that is set to indicate whether or not the
   submodel can be null. */
STATE *startfsa(root, canbenull)
  TREE *root ;
  LOGICAL *canbenull ;
  {
    STATELIST *item ;
    STATE *first ;

    top->starts = (STATELIST *) m_malloc(sizeof(STATELIST), "state list") ;
    first = getstate() ;
    top->starts->value = first ;
    top->starts->level = stacklevels ;
    top->starts->next = NULL ;
    *canbenull = makefsa(root, FALSE) ;
    for (item = top->finals ; item ; item = item->next)
      item->value->final = TRUE ;
    dellist(&top->starts) ;
    delstartarcs() ;
    return(first) ;
    }

