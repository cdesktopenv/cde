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
/*
 * $XConsortium: ParseTree.hh /main/3 1996/06/11 16:21:45 cde-hal $
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

#ifndef _ParseTree_hh
#define _ParseTree_hh

typedef void *ClassType;

/* Class Tree: */
class     PNode;
class       PStart;
class       PEnd;
class       PText;
class         PNumber;
class         PString;
class       PGroup;
class         PRoot;
class       PBooleanOp;
class         POr;
class         PAnd;
class         PRelation;
class           PNear;
class           PBefore;
class       PPrefixOp;
class         PNot;
class       PPostfixOp;
class         PWeight;
class         PProximity;
class         PNotBoth;

typedef enum { USER_REQUEST, OBJECT_REQUEST } delete_request_t;

/* -------- PNode -------- */

class PNode {
public: // variables
  static const ClassType PNodeClass;
  void *f_user_data;            // allow app to attach misc data to nodes
  /* we're dead in multi-threaded environment with statics like this */
  static bool f_recursive_delete;

  PNode *f_parent;		// parent in parse tree
  PNode *f_previous;		// next symbol in linear ordering
  PNode *f_next;		// previous symbol in linear ordering

public: // functions
  virtual ~PNode ();
  
  virtual bool isa (ClassType type)
    { return (type == PNodeClass ? TRUE : FALSE); }
  virtual const ClassType type () = 0;
  virtual char *symbol ()
    { return " *ERROR* "; }
  virtual const int precedence ()
    { abort (); return (0); }
  virtual void replace_child (PNode *, PNode *)
    { puts ("ERROR: BOGUS replace_child CALLED!!"); abort (); }
  static void insert (PRoot *root, long position);
  static bool can_insert (PRoot *root, long position);
  virtual void delete_self (delete_request_t)
    { puts ("*** Tried to delete something without delete method ***"); }

protected: // functions
  PNode (PNode *parent, PNode *previous, PNode *next);
};

/* -------- PStart -------- */

class PStart : public PNode {
public: // variables
  static const ClassType PStartClass;
  
public: // functions
  PStart (PNode *parent, PNode *previous, PNode *next)
    : PNode (parent, previous, next) { };

  virtual bool isa (ClassType type)
    { return (type == PStartClass ? TRUE : PNode::isa (type)); }
  virtual const ClassType type ()
    { return PStartClass; }
  virtual char *symbol ()
    { return f_symbol; }

protected: // variables
  static char *f_symbol;
  static const int f_precedence;
};

/* -------- PEnd -------- */

class PEnd : public PNode {
public: // variables
  static const ClassType PEndClass;

public: //functions
  PEnd (PNode *parent, PNode *previous, PNode *next)
    : PNode (parent, previous, next) { };

  virtual bool isa (ClassType type)
    { return (type == PEndClass ? TRUE : PNode::isa (type)); }
  virtual const ClassType type ()
    { return PEndClass; }
  virtual char *symbol ()
    { return f_symbol; }

protected: // variables
  static char *f_symbol;
  static const int f_precedence;
};

/* -------- PText -------- */

class PText : public PNode {
public: // variables
  static const ClassType PTextClass;
  bool f_empty;

public: // functions
  PText (PNode *parent, PNode *previous, PNode *next, char *str);
  ~PText ();
  void insert_chars (int where, char *text, int length);
  void remove_chars (int where, int len);
  virtual bool isa (ClassType type)
    { return (type == PTextClass ? TRUE : PNode::isa (type)); }
  virtual char *symbol()
    { return f_symbol; }
  virtual void delete_self (delete_request_t);

protected: // variables
  char *f_symbol;
  int f_symbol_len;
  int f_symbol_space;
};

/* -------- PNumber -------- */

class PNumber : public PText {
public: // variables
  PNumber (PNode *parent, PNode *previous, PNode *next, char *str = "#");
  static const ClassType PNumberClass;

public: // functions
  virtual bool isa (ClassType type)
    { return (type == PNumberClass ? TRUE : PText::isa (type)); }
  virtual const ClassType type ()
    { return PNumberClass; }

protected: // variables
  static const int f_precedence;
};

/* -------- PString -------- */

class PString : public PText {
public: // variables
  static const ClassType PStringClass;

public: // functions
  PString (char *str = "\"?\"");
  PString (PNode *parent, PNode *previous, PNode *next, char *str = "\"?\"");
  virtual ~PString ();

  virtual bool isa (ClassType type)
    { return (type == PStringClass ? TRUE : PText::isa (type)); }
  virtual const ClassType type ()
    { return PStringClass; }

protected: // variables
  static const int f_precedence;
};

/* -------- PGroup -------- */

class PGroup : public PNode {
public: // variables
  static const ClassType PGroupClass;

  PNode *f_left;
  PNode *f_subexpr;
  PNode *f_right;

public: // functions
  PGroup (PNode *parent, PNode *previous, PNode *next, PNode *subexpr);
  virtual ~PGroup ();
  
  virtual bool isa (ClassType type)
    { return (type == PGroupClass ? TRUE : PNode::isa (type)); }
  virtual const ClassType type ()
    { return PGroupClass; }
  virtual void replace_child (PNode *, PNode *replacement)
    { f_subexpr = replacement;
      if (replacement != NULL) replacement->f_parent = this; }
  virtual const int precedence ()
    { return f_precedence; }
  virtual char *symbol ()
    { return ("Group"); }

protected: // variables
  static const int f_precedence;
};

/* -------- PRoot -------- */
/* root of a parse tree */

class PRoot : public PGroup {
public: // variables
  static const ClassType PRootClass;

  int f_old_length;
  int f_length;
  char *f_char_of;
  PNode **f_node_of;

public: // functions
  PRoot (PNode *subexpr);
  virtual ~PRoot ();
  void generate ();
  virtual bool isa (ClassType type)
    { return (type == PRootClass ? TRUE : PGroup::isa (type)); }
  virtual const ClassType type ()
    { return PRootClass; }

protected: // variables
  int f_space;
  PNode **f_node_of_real;
};

/* -------- PBooleanOp -------- */

class PBooleanOp : public PNode {
public: // variables
  static const ClassType PBooleanOpClass;

  PNode *f_left;
  PNode *f_right;

public: // functions
  PBooleanOp ();
  virtual bool isa (ClassType type)
    { return (type == PBooleanOpClass ? TRUE : PNode::isa (type)); }
  virtual void replace_child (PNode *old, PNode *replacement);
  static bool can_insert (PRoot *root, long position);
  virtual void init (PRoot *, long pos);
  virtual void delete_self (delete_request_t);

protected: // variables
};

/* -------- POr -------- */

class POr : public PBooleanOp {
public: // variables
  static const ClassType POrClass;

  PNotBoth *f_not_both;

public: // functions
  POr (PRoot *root, long pos)
    :  f_not_both (NULL) { init (root, pos); }
  virtual bool isa (ClassType type)
    { return (type == POrClass ? TRUE : PBooleanOp::isa (type)); }
  virtual const ClassType type ()
    { return POrClass; }
  virtual char *symbol()
    { return f_symbol; }
  static void insert (PRoot *root, long pos);
  virtual const int precedence ()
    { return f_precedence; }

protected: // variables
  static char *f_symbol;
  static const int f_precedence;
};

/* -------- PAnd -------- */

class PAnd : public PBooleanOp {
public: // variables
  static const ClassType PAndClass;

public: // functions
  PAnd (PRoot *root, long pos)
    { init (root, pos); }
  virtual bool isa (ClassType type)
    { return (type == PAndClass ? TRUE : PBooleanOp::isa (type)); }
  virtual const ClassType type ()
    { return PAndClass; }
  virtual char *symbol()
    { return f_symbol; }
  static void insert (PRoot *root, long pos);
  virtual const int precedence ()
    { return f_precedence; }

protected: // variables
  static char *f_symbol;
  static const int f_precedence;
};

/* -------- PRelation -------- */

class PRelation : public PBooleanOp {
public: // variables
  static const ClassType PRelationClass;

  PPostfixOp *f_proximity;

public: // functions
  PRelation ()
    : f_proximity (NULL) { }
  virtual bool isa (ClassType type)
    { return (type == PRelationClass ? TRUE : PBooleanOp::isa (type)); }

protected: // variables
};

/* -------- PNear -------- */

class PNear : public PRelation {
public: // variables
  static const ClassType PNearClass;

public: // functions
  PNear (PRoot *root, long pos)
    { init (root, pos); }
  virtual bool isa (ClassType type)
    { return (type == PNearClass ? TRUE : PRelation::isa (type)); }
  virtual const ClassType type ()
    { return PNearClass; }
  virtual char *symbol ()
    { return f_symbol; }
  static void insert (PRoot *root, long pos);
  virtual const int precedence ()
    { return f_precedence; }

protected: // variables
  static char *f_symbol;
  static const int f_precedence;
};

/* -------- PBefore -------- */

class PBefore : public PRelation {
public: // variables
  static const ClassType PBeforeClass;

public: // functions
  PBefore (PRoot *root, long pos)
    { init (root, pos); }
  virtual bool isa (ClassType type)
    { return (type == PBeforeClass ? TRUE : PRelation::isa (type)); }
  virtual const ClassType type ()
    { return PBeforeClass; }
  virtual char *symbol ()
    { return f_symbol; }
  static void insert (PRoot *root, long pos);
  virtual const int precedence ()
    { return f_precedence; }

protected: // variables
  static char *f_symbol;
  static const int f_precedence;
};

/* -------- PPrefixOp -------- */

class PPrefixOp : public PNode {
public: // variables
  static const ClassType PPrefixOpClass;

  PNode *f_operand;

public: // functions
  PPrefixOp (PNode *rhs);
  virtual bool isa (ClassType type)
    { return (type == PPrefixOpClass ? TRUE : PNode::isa (type)); }
  virtual void replace_child (PNode *, PNode *replacement)
    { f_operand = replacement;
      if (replacement != NULL) replacement->f_parent = this; }
};

/* -------- PNot -------- */

class PNot : public PPrefixOp {
public: // variables
  static const ClassType PNotClass;

public: // functions
  PNot (PNode *rhs);
  ~PNot ();
  virtual bool isa (ClassType type)
    { return (type == PNotClass ? TRUE : PPrefixOp::isa (type)); }
  virtual const ClassType type ()
    { return PNotClass; }
  virtual char *symbol()
    { return f_symbol; }
  virtual const int precedence ()
    { return f_precedence; }
  static void insert (PRoot *root, long position);
  static bool can_insert (PRoot *root, long position);
  virtual void delete_self (delete_request_t request);

protected: // variables
  static char *f_symbol;
  static const int f_precedence;
};


/* -------- PPostfixOp -------- */
/* Postfix ops are bound by the thing they operation on. */
/* In other words, they can't apply to arbitrary things,
   so the things they apply to will point to them. */

class PPostfixOp : public PNode {
public: // variables
  static const ClassType PPostfixOpClass;

public: // functions
  virtual bool isa (ClassType type)
    { return (type == PPostfixOpClass ? TRUE : PNode::isa (type)); }
};

/* -------- PWeight -------- */

class PWeight : public PPostfixOp {
public: // variables
  static const ClassType PWeightClass;

public: // functions
  virtual bool isa (ClassType type)
    { return (type == PWeightClass ? TRUE : PPostfixOp::isa (type)); }
  virtual const ClassType type ()
    { return PWeightClass; }
  virtual char *symbol ()
    { return f_symbol; }
  static bool can_insert (PRoot *root, long position);
  virtual const int precedence ()
    { return f_precedence; }

protected: // variables
  static char *f_symbol;
  static const int f_precedence;
};

/* -------- PProximity -------- */

class PProximity : public PPostfixOp {
public: // variables
  static const ClassType PProximityClass;

public: // functions
  virtual bool isa (ClassType type)
    { return (type == PProximityClass ? TRUE : PPostfixOp::isa (type)); }
  virtual const ClassType type ()
    { return PProximityClass; }
  virtual char *symbol()
    { return f_symbol; }
  virtual const int precedence ()
    { return f_precedence; }

protected: // variables
  static char *f_symbol;
  static const int f_precedence;
};
/* -------- PProximity -------- */

class PNotBoth : public PPostfixOp {
public: // variables
  static const ClassType PNotBothClass;

public: // functions
  virtual bool isa (ClassType type)
    { return (type == PNotBothClass ? TRUE : PPostfixOp::isa (type)); }
  virtual const ClassType type ()
    { return PNotBothClass; }
  virtual char *symbol()
    { return f_symbol; }
  virtual const int precedence ()
    { return f_precedence; }
  static bool can_insert (PRoot *root, int position);
  
protected: // variables
  static char *f_symbol;
  static const int f_precedence;
};

#endif /* _ParseTree_hh */
/* DO NOT ADD ANY LINES AFTER THIS #endif */

