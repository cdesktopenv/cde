/* $XConsortium: Element.h /main/5 1996/08/21 15:50:08 drk $ */
#ifndef _Element_h
#define _Element_h

#ifndef CDE_NEXT
#else
#include "dti_cc/CC_Slist.h"
#endif

#include "SymTab.h"

/* **************************************************************
   class Element
   
   this is the structure passed from the node parser to the Resolver.
   It contains the Element GI, and the attribute list for the element
   ************************************************************** */

class Attribute;
class AttributeList;

class Element
{
public:
  Element(const Element&);
  Element(const Symbol &gi,
	  unsigned int	sibling_number = 0,
	  AttributeList *attrs = 0,
	  AttributeList *olias_attrs = 0,
	  unsigned int	relative_sibling_number = 0
	 );
  ~Element();
  
  const Symbol       &gi() const { return f_gi ; }

  const Attribute	*get_attribute(const Symbol &name) const ;
  const Attribute	*get_olias_attribute(const Symbol &name) const;

  unsigned int		sibling_number() const { return f_sibling_number ; }
  unsigned int		relative_sibling_number() const 
	{ return f_relative_sibling_number; }

  int		last_child() const { return f_last_child; }
  int		relatively_last_child() const 
	{ return f_relatively_last_child; }

  ostream &print(ostream &) const ;

private:
  unsigned int		f_freeAttrLists;
  unsigned int		f_sibling_number ; // counting all children of a parent
  unsigned int		f_relative_sibling_number ; // counting all 
						    // consecutive children
						    // of same types of a
						    // parent
  int			f_last_child;
  int			f_relatively_last_child;
  Symbol		f_gi ;
  AttributeList	       *f_attributes;
  AttributeList	       *f_olias_attributes;
};

inline
ostream &operator<<(ostream &o, const Element &e)
{
  return e.print(o);
}


#endif /* _Element_h */
/* DO NOT ADD ANY LINES AFTER THIS #endif */
