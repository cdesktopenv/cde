/*
 * $XConsortium: OutlineElement.hh /main/4 1996/08/30 17:21:04 drk $
 *
 * Copyright (c) 1992 HaL Computer Systems, Inc.  All rights reserved.
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

class OutlineList;

class OutlineElement : public FolioObject
{
friend class OutlineList;
  
public: // functions
  DEF_CLASS (OutlineElement);

  OutlineElement (OutlineList *children = NULL)
    : f_children (children),
      f_xm_string (NULL), f_children_flag (0),
      f_string_creator (0), f_display_flag (1),
      f_level (0)
  { children_valid ((children != NULL) ? TRUE : FALSE); }

  ~OutlineElement();

  inline bool has_children();

  virtual OutlineList *children()
    { return (f_children); }
  virtual void set_children (OutlineList *children)
    { f_children = children;
      children_valid (TRUE); }

  bool children_cached()
    { return (children_valid()); }

  virtual void display() = 0;

  unsigned char level()
    { return (f_level); }
  void level (unsigned char level)
    { f_level = level; }
  
  bool is_selected (BitHandle handle)
    { return f_selected.is_set (handle); }

  void set_selected (BitHandle handle)
    { f_selected.set (handle) ; }

  void unset_selected (BitHandle handle)
    { f_selected.unset (handle); }

  bool is_expanded (BitHandle handle)
    { return f_expanded.is_set (handle); }

  bool is_contracted (BitHandle handle)
    { return !f_expanded.is_set (handle) ; }

  void set_expanded (BitHandle handle)
    { f_expanded.set (handle); }

  void set_contracted (BitHandle handle)
    { f_expanded.unset (handle); }
  
  void set_xm_string (void *xm_string)
    { f_xm_string = xm_string; }
  void *xm_string()
    { return (f_xm_string); }

  void string_creator (u_int creator_id)
    { f_string_creator = creator_id; }
  u_int string_creator()
    { return (f_string_creator); }

#ifdef DEBUG
  void print();
#endif

  unsigned char display_flag()
    { return (f_display_flag); }
  void display_flag (unsigned char flag)
    { f_display_flag = flag; }

protected: // functions
  bool children_valid()
    { return ((f_children_flag & CHILDREN_VALID_BIT) != 0); }
  void children_valid (bool setting)
    { f_children_flag = (setting ? f_children_flag | CHILDREN_VALID_BIT :
  		                   f_children_flag & ~CHILDREN_VALID_BIT);
    }
  bool has_children_valid()
    { return ((f_children_flag & HAS_CHILDREN_VALID_BIT) != 0); }
  void has_children_valid (bool setting)
    { f_children_flag = (setting ? f_children_flag | HAS_CHILDREN_VALID_BIT :
		                   f_children_flag & ~HAS_CHILDREN_VALID_BIT);
    }
  virtual bool has_children_internal()
    { return (children() != NULL); }
  void has_children (bool setting)
    { f_children_flag = (setting ? f_children_flag | HAS_CHILDREN_BIT :
		                   f_children_flag & ~HAS_CHILDREN_BIT);
    }

protected: // variables
  OutlineList *f_children;  // Each outline element may have a list of children
  
  BitField	f_selected ;
  BitField	f_expanded ;

  // NOTE: xm_string temporary until R2
  void        *f_xm_string;
  unsigned char f_children_flag;
  enum { CHILDREN_VALID_BIT = 0x01,
         HAS_CHILDREN_VALID_BIT = 0x02, HAS_CHILDREN_BIT = 0x04 };
  u_int f_string_creator;

  unsigned char f_display_flag;  // Able to be displayed on activate [rtp]
  unsigned char f_level;    // Level this element is at, zero-based
};


inline bool
OutlineElement::has_children()
{
  // Only calculate value of has_children once since it might be expensive. 
  if (!has_children_valid())
    {
      has_children (has_children_internal());
      has_children_valid (TRUE);
    }

  return ((f_children_flag & HAS_CHILDREN_BIT) != 0);
}

