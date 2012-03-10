/* $XConsortium: DocParser.h /main/6 1996/06/11 17:06:11 cde-hal $ */
/*	Copyright (c) 1995 FUJITSU LIMITED	*/
/*	All Rights Reserved			*/

#include <iostream.h>
#include <strstream.h>

#include "SymTab.h"
#include "Exceptions.hh"

class Resolver;
class Attribute;
class AttributeList;

// parse SGML like documents

// attributes follow elements in an olias section

// <ELEMENT>
// <#><attr_1>value</>
//    <attr_2>value</>
//    <#olias_attr>value</>
// </>
// Element data here
// </ELEMENT>

class Element ;

class DocParser : public Destructable
{
public:
  enum TagType { StartTag, EndTag, AttributeSection, OliasAttribute, NoTag };

  DocParser(Resolver &);
  ~DocParser();
  
  // returns a boolean  
  unsigned int	parse(istream &);

  // parse without calling Begin() and End() on the renderer.
  unsigned int	rawParse(istream &);

protected:
  virtual void read_data(istream &, ostream &);

private:
  
  void		process(istream &, ostream &, const Symbol &tagname, 
			unsigned int sibling_number,
			unsigned int relative_sibling_number);
  TagType	read_tag(istream &, ostream &);
  void		process_entity(istream &, ostream &);

  void		process_attributes(istream &, ostream &,
				   AttributeList *&attrs,
				   AttributeList *&olias_attrs);
  Attribute    *process_attribute(istream &, ostream &, const Symbol &name, TagType);
  
private:
  unsigned int	f_ignoring_element ;
#if defined(SC3) || defined(__osf__)
  char* const	f_buffer;
  ostrstream	f_output;
#else
  strstreambuf *f_streambuf ;
  ostream	f_output;
#endif
  Resolver     &f_resolver;
};
