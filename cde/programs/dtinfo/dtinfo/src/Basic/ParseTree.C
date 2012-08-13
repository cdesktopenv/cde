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
/*
 * $XConsortium: ParseTree.cc /main/3 1996/06/11 16:21:40 cde-hal $
 *
 * Copyright (c) 1991 HaL Computer Systems, Inc.  All rights reserved.
 * UNPUBLISHED -- rights reserved under the Copyright Laws of the United
 * States.  Use of a copyright notice is precautionary only and does not
 * imply publication or disclosure.
 * 
 * This software contains confidential information and trade secrets of HaL
 * Computer Systems, Inc.  Use, disclosure, or reproduction is prohibited
 * without the prior express written permission of HaL Computer Systems, Inc.
 * 
 *                         RESTRICTED RIGHTS LEGEND
 * Use, duplication, or disclosure by the Government is subject to
 * restrictions as set forth in subparagraph (c)(l)(ii) of the Rights in
 * Technical Data and Computer Software clause at DFARS 252.227-7013.
 *                        HaL Computer Systems, Inc.
 *                  1315 Dell Avenue, Campbell, CA  95008
 * 
 */

#define C_ParseTree
#define L_Basic

#include "../Prelude.h"
#include <string.h>
#include <stdlib.h>

#ifdef SUN_CPP
#define InitBase(T) \
  const ClassType T::T/**/Class = (ClassType *) &T::T/**/Class
#else
#define InitBase(T) \
  const ClassType T::T##Class = (ClassType *) &T::T##Class
#endif

#define InitLeaf(T,P) \
  InitBase (T); const int T::f_precedence = P
#define InitLeafS(T,P,S) \
  InitBase (T); const int T::f_precedence = P; char *T::f_symbol = S

bool PNode::f_recursive_delete = FALSE;

InitBase  (PNode);
InitBase  (PBooleanOp);
InitBase  (PRelation);
InitBase  (PPrefixOp);
InitBase  (PPostfixOp);
InitBase  (PText);
InitBase  (PRoot);

InitLeafS (PWeight,     7,  " weight "  );
InitLeafS (PProximity,  7,  " within "  );
InitLeafS (PNotBoth,    7,  " not_both" );
InitLeafS (PStart,      0,  "("         );
InitLeafS (PEnd,        0,  ")"         );
InitLeaf  (PGroup,      0               );
InitLeafS (PNot,        5,  "not "      );
InitLeafS (PNear,       4,  " near "    );
InitLeafS (PBefore,     4,  " before "  );
InitLeafS (PAnd,        3,  " and "     );
InitLeafS (POr,         2,  " or "      );
InitLeaf  (PString,     8               );
InitLeaf  (PNumber,     8               );

// /////////////////////////////////////////////////////////////////
// Constructors
// /////////////////////////////////////////////////////////////////

PNode::PNode (PNode *parent, PNode *previous, PNode *next)
: f_parent (parent), f_previous (previous), f_next (next)
{
  /* Modify previous and next links of previous and next */
  if (f_previous != NULL)
    f_previous->f_next = this;
  if (f_next != NULL)
    f_next->f_previous = this;
}

PText::PText (PNode *parent, PNode *previous, PNode *next, char *str)
: PNode (parent, previous, next)
{
  if (str[1] == '?')
    f_empty = TRUE;
  else
    f_empty = FALSE;

  f_symbol_len = strlen (str);
  if (f_symbol_len < 32)
    f_symbol_space = 32;
  else
    f_symbol_space = f_symbol_len;

  f_symbol = (char *) malloc (sizeof(char) * (f_symbol_space + 1));
  strcpy (f_symbol, str);
}

  
PString::PString (PNode *parent, PNode *previous, PNode *next, char *str)
: PText (parent, previous, next, str)
{
}

PString::PString (char *str)
: PText (NULL, NULL, NULL, str)
{ }

PNumber::PNumber (PNode *parent, PNode *previous, PNode *next, char *str)
: PText (parent, previous, next, str)
{ }

PGroup::PGroup (PNode *parent, PNode *previous, PNode *next, PNode *subexpr)
: PNode (parent, NULL, NULL),
  f_left (new PStart (this, previous, subexpr)),
  f_subexpr (subexpr),
  f_right (new PEnd (this, subexpr, next))
{
  subexpr->f_parent = this;
}

PRoot::PRoot (PNode *subexpr)
: PGroup (NULL, NULL, NULL, subexpr), f_length (0)
{
  f_space = 16;
  f_char_of = (char *) malloc (sizeof (char) * (f_space + 1));
  f_node_of_real = (PNode **) malloc (sizeof (PNode *) * (f_space + 2));
  f_node_of = f_node_of_real + 1;
}

PPrefixOp::PPrefixOp (PNode *rhs)
: PNode (rhs->f_parent, rhs->f_previous, rhs)
{
  /* fix up the parent and previous of rhs */
  rhs->f_parent = this;
}

PNot::PNot (PNode *rhs)
: PPrefixOp (rhs)
{
  f_operand = rhs;
}

PBooleanOp::PBooleanOp ()
: PNode (NULL, NULL, NULL)
{}

// NOTE: clean up PNode constructors later (make NULL constructor, etc.)
void
PBooleanOp::init (PRoot *root, long pos)
{
  PNode *lhs = root->f_node_of[pos-1];
  PNode *rhs = root->f_node_of[pos];

  /* Expects eother lhs or rhs to be NULL, to indicate where to insert
     a blank string. */
  /* The new string will be on the lhs of the op and it's
     previous will become the previous of the existing rhs.
     Eg:
          previous-thing <cursor> rhs                <--- Before
	  previous-thing new-lhs op <cursor> rhs     <--- After

     The next of rhs does not change.  The next of the new-lhs
     and the previous of rhs will be set in the PNode constructor
     when the op is created.  The second case below is exactly
     opposite to this one, eg:

	  lhs <cursor> next-thing                <--- Before
	  lhs op new-rhs <cursor> next-thing     <--- After       
  */

  // First check to see of right side is valid operand
  if (rhs->isa (PString::PStringClass))
    {
      // Create lhs string
      f_left = new PString (this, rhs->f_previous, this);

      // Make rhs a child and fix next/previous pointers.
      f_next = rhs;
      rhs->f_previous = this;

      // Become a child of the rhs' parent
      rhs->f_parent->replace_child (rhs, this);

      // Make rhs a child of this
      rhs->f_parent = this;
      f_right = rhs;
    }
  // Now see if left side is valid operand
  else if (lhs->isa (PString::PStringClass))
    {
      // Create rhs string
      f_right = new PString (this, this, lhs->f_next);

      // Make lhs a child and fix previous/next pointers.
      f_previous = lhs;
      lhs->f_next = this;

      // Become a child of the lhs' parent
      lhs->f_parent->replace_child (lhs, this);

      // Make the lhs a child of this
      lhs->f_parent = this;
      f_left = lhs;
    }
  else
    {
      puts ("ERROR: lhs and rhs both non NULL in Boolean Op constructor");
      abort ();
    }

  /* -------- Handle precedence -------- */

  while (f_parent->isa (PBooleanOp::PBooleanOpClass) &&
	 (f_parent->precedence() > precedence()))
	 
    {
      // Reparent based on the side of parent this is on
      PBooleanOp *old_parent = (PBooleanOp *) f_parent;
      // My f_parent pointer changed by next function call
      old_parent->f_parent->replace_child (old_parent, this);
     
      if (old_parent->f_left == this)
	{
	  old_parent->replace_child (this, f_right);
	  f_right = old_parent;
	  old_parent->f_parent = this;
	}
      else if (old_parent->f_right == this)
	{
	  old_parent->replace_child (this, f_left);
	  f_left = old_parent;
	  old_parent->f_parent = this;
	}
      else
	{
	  puts ("ERROR: bogousity in precedence handling");
	  abort ();
	}
    }
}


// /////////////////////////////////////////////////////////////////
// Desctructors
// /////////////////////////////////////////////////////////////////

PNode::~PNode ()
{
  /* fix up next / previous links */
  if (f_previous)
    f_previous->f_next = f_next;
  if (f_next)
    f_next->f_previous = f_previous;
}

PString::~PString ()
{
  if (f_symbol)
    free (f_symbol);
}

PGroup::~PGroup ()
{
  delete f_left;
  delete f_right;
  if (f_recursive_delete)
    delete f_subexpr;
}

PRoot::~PRoot ()
{
  if (f_space > 0)
    {
      free ((char *) f_char_of);
      free ((char *) f_node_of_real);
    }
}

PNot::~PNot ()
{
  // Stick the child of not in not's parent (in not's slot)
  f_parent->replace_child (this, f_operand);
}

void
PNot::delete_self (delete_request_t request)
{
  PNode *previous = f_previous;
  delete this;
  if (request == OBJECT_REQUEST)
    previous->delete_self (OBJECT_REQUEST);
}

PText::~PText ()
{
  // Don't let parent keep a pointer to me
  f_parent->replace_child (this, NULL);
}


// NOTE: need to add check for deleting first thing before a group.
// NOTE: may need a way to return "failed"
void
PText::delete_self (delete_request_t request)
{
  PNode *previous = f_previous;
  delete this;
  if (request == USER_REQUEST)
    previous->delete_self (OBJECT_REQUEST);
}

void
PBooleanOp::delete_self (delete_request_t request)
{
  if (request == USER_REQUEST)
    f_previous->delete_self (OBJECT_REQUEST);

  // Take this out of next/previous chain
  f_next->f_previous = f_previous;
  f_previous->f_next = f_next;
  // f_parent->replace_child (this, NULL);

  /* When an operator is deleted part of the parse tree is split.
     To reconstruct the parse tree, the former left and right side
     must be merged.  */

  // The first operator needing a child will be the parent of the
  // deleted operand.

  // (Yes, operater is spelled wrong, but only because the correct spelling
  //  is a C++ reserved word...)

  PNode  *operater, *operand;

  /* -------- Figure out operand and operators. -------- */

  if (f_left == NULL)
    operand = f_right;
  else if (f_right == NULL)
    operand = f_left;
  else
    {
      if (f_next->isa (PBooleanOp::PBooleanOpClass))
	{
	  operater = f_next;
	  // operand was on left of operater, find new operand
	  operand = this->f_left;
	  while (operand->isa (PBooleanOp::PBooleanOpClass))
	    operand = ((PBooleanOp *) operand)->f_right;
	}
      else if (f_previous->isa (PBooleanOp::PBooleanOpClass))
	{
	  operater = f_previous;
	  // operand was on right of operater, find new operand
	  operand = this->f_right;
	  while (operand->isa (PBooleanOp::PBooleanOpClass))
	    operand = ((PBooleanOp *) operand)->f_left;
	}
    }
     

  /* -------- Merge subtrees until only one tree remains -------- */

  while (f_left != NULL && f_right != NULL)
    {
      // Now find enclosing subtree of operand based on precedence.
      // We can't move the operand to the operator if it is already
      // under an operator with higher precedence.
      while (operand->f_parent != this
	     && operand->f_parent->precedence() >= operater->precedence())
	// Move up the tree
	operand = operand->f_parent;

      // Now operand points to the subtree we can safely reparent
      PNode *old_parent = operand->f_parent;

      // Empty the spot where the subtree was
      operand->f_parent->replace_child (operand, NULL);

      // Replace hole in operator with this subtree
      operater->replace_child (NULL, operand);

      // The new operand is the old operator...
      operand = operater->f_parent;

      // and the new operator is the old operand because it just lost a subtree
      operater = old_parent;
    }

  // Finally get rid of this
  if (f_left == NULL)
    f_parent->replace_child (this, f_right);
  else
    f_parent->replace_child (this, f_left);
  f_previous = f_next = NULL;
  delete this;
}

// /////////////////////////////////////////////////////////////////
// PText functions
// /////////////////////////////////////////////////////////////////

void
PText::insert_chars (int where, char *text, int length)
{
  int new_len = f_symbol_len + length;
  f_empty = FALSE;

  /* -------- allocate memory if necessary -------- */
  if (new_len > f_symbol_space)
    {
      // keep doubling until we have enough space
      while (new_len > f_symbol_space)
	f_symbol_space *= 2;
      // NOTE: check malloc result
      f_symbol = (char *) realloc ((char *) f_symbol,
				   sizeof (char) * (f_symbol_space + 1));
    }

  /* -------- make room for new stuff -------- */
  bcopy (&f_symbol[where],              // from
	 &f_symbol[where+length],	// to
	 f_symbol_len - where + 1);	// length

  /* -------- insert new stuff -------- */
  bcopy (text, &f_symbol[where], length);
  f_symbol_len += length;
}

void
PText::remove_chars (int where, int len)
{
  /* Just copy stuff after hole back over it. */
  bcopy (&f_symbol[where+len], &f_symbol[where],
	 f_symbol_len - (where + len) + 1);
  f_symbol_len -= len;
  if (f_symbol_len == 2)  // ie: just two quotes
    f_empty = TRUE;
}

// /////////////////////////////////////////////////////////////////
// self insert code
// /////////////////////////////////////////////////////////////////

#define LEFT_SIDE  (root->f_node_of[pos-1])
#define RIGHT_SIDE (root->f_node_of[pos])
#define LEFT_ISA(X) ((root->f_node_of[pos-1])->isa (X))
#define RIGHT_ISA(X) ((root->f_node_of[pos])->isa (X))

void
PNot::insert (PRoot *root, long pos)
{
  puts ("Inserting a not");
  PNode *rhs = RIGHT_SIDE;
  PNot *not = new PNot (rhs);
  not->f_parent->replace_child (rhs, not);
}

void
POr::insert (PRoot *root, long pos)
{
  new POr (root, pos);
}

void
PAnd::insert (PRoot *root, long pos)
{
  new PAnd (root, pos);
}

void
PNear::insert (PRoot *root, long pos)
{
  new PNear (root, pos);
}

void
PBefore::insert (PRoot *root, long pos)
{
  new PBefore (root, pos);
}

void
PNode::insert (PRoot *, long)
{
  puts ("Tried to insert something with no insert method.");
}

// /////////////////////////////////////////////////////////////////
// can_insert
// /////////////////////////////////////////////////////////////////

bool
PNode::can_insert (PRoot *, long)
{
  return (FALSE);
}

bool
PBooleanOp::can_insert (PRoot *root, long pos)
{
  /* If one side or the other is text, but not both, can do */
  if ((LEFT_ISA (PText::PTextClass) && !RIGHT_ISA (PText::PTextClass)) ||
      (!LEFT_ISA (PText::PTextClass) && RIGHT_ISA (PText::PTextClass)))
    return (TRUE);
  return (FALSE);
}

bool
PWeight::can_insert (PRoot *root, long pos)
{
  return (FALSE);// tmp fix
  /* If left side is string and the right side isn't can do */
  if (LEFT_ISA (PString::PStringClass) &&
      !RIGHT_ISA (PText::PTextClass) &&
      !RIGHT_ISA (PWeight::PWeightClass))
    return (TRUE);

  return (FALSE);
}

bool
PNot::can_insert (PRoot *root, long pos)
{
  /* "not" insertable if rhs is a string and lhs isn't */
  if (LEFT_ISA (PText::PTextClass) || LEFT_ISA (PNot::PNotClass))
    return (FALSE);
  if (RIGHT_ISA (PString::PStringClass) || RIGHT_ISA (PStart::PStartClass))
    return (TRUE);
  return (FALSE);
}

// /////////////////////////////////////////////////////////////////
// replace child functions
// /////////////////////////////////////////////////////////////////

void
PBooleanOp::replace_child (PNode *old, PNode *replacement)
{
  if (f_left == old)
    f_left = replacement;
  else if (f_right == old)
    f_right = replacement;
  else
    {
      puts ("ERROR: invalid old child passed to PBooleanOp::replace_child");
      abort ();
    }
  if (replacement != NULL)
    replacement->f_parent = this;
}

// /////////////////////////////////////////////////////////////////
// Other stuff
// /////////////////////////////////////////////////////////////////

void
PRoot::generate ()
{
  PNode *p;
  static char *s;
  int pos = 0;

  /* go through the list of displayable elements */
  for (p = f_left->f_next; p != f_right; p = p->f_next)
    {
      /* add the symbol for this element to the string */
      printf ("adding symbol: <%s>\n", p->symbol());
      for (s = p->symbol(); *s != '\0'; s++)
	{
	  if (pos >= f_space)
	    {
	      f_space *= 2;
	      f_char_of = (char *)
		realloc ((char *) f_char_of, sizeof(char) * (f_space + 1));
	      f_node_of_real = (PNode **)
		realloc ((char *) f_node_of_real,
			 sizeof(PNode *) * (f_space + 2));
	      f_node_of = f_node_of_real + 1;
	    }
	  f_char_of[pos] = *s;
	  f_node_of[pos] = p;
	  pos++;
	}
    }
  /* fill in boundries */
  f_char_of[pos] = '\0';
  f_node_of[-1] = f_left;
  f_node_of[pos] = f_right;
  f_old_length = f_length;
  f_length = pos;
  // ON_DEBUG (printf ("Generated: <%s>\n", f_char_of);)
}
