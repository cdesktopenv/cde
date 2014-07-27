/*	copyright (c) 1996 FUJITSU LIMITED	*/
/*	All Rights Reserved			*/

/*
 * $XConsortium: QueryTerm.hh /main/4 1996/05/29 12:23:46 rcs $
 *
 * Copyright (c) 1992 HAL Computer Systems International, Ltd.
 * All rights reserved.  Unpublished -- rights reserved under
 * the Copyright Laws of the United States.  USE OF A COPYRIGHT
 * NOTICE IS PRECAUTIONARY ONLY AND DOES NOT IMPLY PUBLICATION
 * OR DISCLOSURE.
 * 
 * THIS SOFTWARE CONTAINS CONFIDENTIAL INFORMATION AND TRADE
 * SECRETS OF HAL COMPUTER SYSTEMS INTERNATIONAL, LTD.  USE,
 * DISCLOSURE, OR REPRODUCTION IS PROHIBITED WITHOUT THE
 * PRIOR EXPRESS WRITTEN PERMISSION OF HAL COMPUTER SYSTEMS
 * INTERNATIONAL, LTD.
 * 
 *                         RESTRICTED RIGHTS LEGEND
 * Use, duplication, or disclosure by the Government is subject
 * to the restrictions as set forth in subparagraph (c)(l)(ii)
 * of the Rights in Technical Data and Computer Software clause
 * at DFARS 252.227-7013.
 *
 *          HAL COMPUTER SYSTEMS INTERNATIONAL, LTD.
 *                  1315 Dell Avenue
 *                  Campbell, CA  95008
 * 
 */


class QueryGroup;


enum connective_t { C_NONE, C_OR, C_XOR, C_AND, C_NEAR, C_BEFORE,
   OPT_PROXIMITY, OPT_WEIGHT, OPT_SCOPE, OPT_NOT, OPT_COMPLETE, OPT_PHRASE };

class QueryTerm : public FolioObject
{
public:
  QueryTerm (QueryGroup *parent, QueryTerm *previous, QueryTerm *next);
  ~QueryTerm();
  enum prefix_t { PFX_CONTAIN = 0, PFX_NOT_BITS = 1, PFX_NOT_CONTAIN = 1,
	 PFX_COMPLETE_BITS = 2, PFX_COMPLETE = 2, PFX_NOT_COMPLETE = 3 };
  enum { TYPE_SIMPLE, TYPE_GROUP };
  enum { CONNECTIVE_CHANGED = FolioObject::_LAST };

  prefix_t prefix() const
    { return ((prefix_t) f_prefix); }
  void prefix (u_int which)
    { f_prefix = which; }
  connective_t connective() const
    { return ((connective_t) f_connective); }
  void connective (u_int which)
    { f_connective = which; notify (CONNECTIVE_CHANGED); }
  QueryTerm *previous() const
    { return (f_previous); }
  QueryTerm *next() const
    { return (f_next); }
  u_char term_type() const
    { return (f_type); }
  QueryGroup *group() const
    { return (f_group_term); }
  char *term_string()
    { if (!f_term_string_fixed) cleanup_term_string();
      return (f_term_string); }
  // Term owns string passed into this method. 
  void term_string (char *s)
    { free (f_term_string); f_term_string_fixed = FALSE; f_term_string = s; }
  char *weight()
    { return (f_weight); }
  void weight (char *s)
    { free (f_weight); f_weight = s; }
  char *proximity()
    { return (f_proximity); }
  void proximity (char *s)
    { free (f_proximity); f_proximity = s; }
  char *scope()
    { return ((char*)""); }
  static unsigned int avail_caps() { return f_caps; }
  static void avail_caps(unsigned int caps) { f_caps = caps; }

private:
  void cleanup_term_string();

private:
  QueryGroup   *f_parent;
  QueryTerm    *f_previous;
  QueryTerm    *f_next;
  union {
    QueryGroup *f_group_term;
    char       *f_term_string;
  };
  u_char  f_prefix;
  u_char  f_connective;
  char   *f_weight;
  char   *f_proximity;
  bool f_term_string_fixed;
  u_char f_type;
  // may need some way to verify the scope... 

  static unsigned int f_caps; 
};
