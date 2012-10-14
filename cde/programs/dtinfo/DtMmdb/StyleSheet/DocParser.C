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
/*	Copyright (c) 1995 FUJITSU LIMITED	*/
/*	All Rights Reserved			*/

/* $TOG: DocParser.C /main/16 1998/04/17 11:48:07 mgreess $ */
#ifdef DEBUG
#include "assert.h"
#endif
#include "Debug.h"
#include "StyleSheetExceptions.h"
#include "DocParser.h"
#include "Resolver.h"
#include "Element.h"
#include "AttributeList.h"

#define DATA_BUF_SIZ 4096

#if defined(SC3) || defined(__osf__)
static ostrstream& terminate(ostrstream& ost)
{
    char* string = ost.str();
    *(string + ost.pcount()) = 0;

    return ost;
}
#endif

DocParser::DocParser(Resolver &r)
: f_resolver(r),
#if defined(SC3) || defined(__osf__)
  f_buffer(new char[DATA_BUF_SIZ]),
  f_output(f_buffer, DATA_BUF_SIZ)
#else
  f_streambuf(new stringbuf()),
  f_output(f_streambuf)
#endif
{
}

DocParser::~DocParser()
{
#if defined(SC3) || defined(__osf__)
  if (f_buffer) delete[] f_buffer;
#else
  // this causes a free memory read when f_output is deleted as part of this
  // object...nothing we can do about it
  delete f_streambuf ;
#endif
}

unsigned int
DocParser::parse(istream &input)
{
   f_resolver.Begin();
   unsigned int ok = rawParse(input);
   f_resolver.End();
   return ok;
}

unsigned int
DocParser::rawParse(istream &input)
{
  input.unsetf(ios::skipws);

  f_ignoring_element = 0 ;

  switch(read_tag(input, f_output))
    {
    case StartTag:
      {
#if defined(SC3) || defined(__osf__)
	Symbol name(gElemSymTab->intern(terminate(f_output).str()));
	f_output.rdbuf()->freeze(0);
#else
	char *data = (char *)f_streambuf->str().c_str();

/*
MESSAGE(cerr, "StartTag case:");
debug(cerr, f_streambuf->pcount());
debug(cerr, data);
*/

#if !defined(SC3) && !defined(__osf__)
	data[f_streambuf->str().size()] = 0;
#endif
	Symbol name(gElemSymTab->intern(data));
#endif
	process(input, f_output, name, 1, 1);
      }
      break;
    case EndTag:
    case AttributeSection:
    case OliasAttribute:
      throw(CASTDPUTEXCEPT docParserUnexpectedTag());
      break;
    case NoTag:
      throw(CASTDPUDEXCEPT docParserUnexpectedData());
      break;
    }      
  return 1;
}

		
void
update_last_seen_child_name(Symbol*& last_seen_child_name, unsigned int& child_relative_sibling_number, const Symbol& new_child_name)
{
   if ( last_seen_child_name == 0 || 
        !(*last_seen_child_name == Symbol(new_child_name)) 
      ) 
   {
     delete last_seen_child_name ;
     last_seen_child_name = new Symbol(new_child_name);
     child_relative_sibling_number= 1;
   } else
     child_relative_sibling_number++;

   return;
}

void
DocParser::process(istream &input, ostream &output,
		   const Symbol &name,
		   unsigned int sibling_number, unsigned int this_sibling_number)
{
  ON_DEBUG(cerr << "process(" << name << ") -> " << sibling_number << endl);

  Symbol* last_seen_child_name = 0; 
	    
  unsigned int child_relative_sibling_number = 0;

  unsigned int child = 1 ;	// sibling numbers for child elements 

  char c ;
  while ((input >> c) && (c == '\n'));
  input.putback(c);

  if (input.eof())
    throw(CASTDPUEEXCEPT docParserUnexpectedEof());

  int ignore = 0 ;

  mtry
    {
      // process whatever comes right after start tag 
      TagType tt = read_tag(input, output);
      switch (tt)
	{
	case StartTag:
	  {
	    ON_DEBUG(cerr << "beginElement" << endl);
	    // have to begin this element before processing child elements 
	    if (!f_ignoring_element)
	      {
		ignore = f_resolver.beginElement(new Element(name,
							     sibling_number, 0,
							     0,
							     this_sibling_number)); 
		f_ignoring_element = ignore ;
	      }

               
/////////////////////////////
// first child of this node
/////////////////////////////
#if defined(SC3) || defined(__osf__)
	    Symbol name(gElemSymTab->intern(terminate(f_output).str()));

	    update_last_seen_child_name(last_seen_child_name, 
				 child_relative_sibling_number, name);

	    f_output.rdbuf()->freeze(0);

	    process(input, output, name, child++, child_relative_sibling_number);
#else
	    char *data = (char *)f_streambuf->str().c_str();
#if !defined(SC3)  && !defined(__osf__)
	    data[f_streambuf->str().size()] = 0;
#endif
	    Symbol name(gElemSymTab->intern(data));
	    update_last_seen_child_name(last_seen_child_name, 
				 child_relative_sibling_number, name);

	    process(input, output, name,
		    child++, child_relative_sibling_number);
#endif
	  }
	  break;
	case EndTag:
	  // hit an end tag right after start tag 
#ifdef DEBUG
	  {
#if defined(SC3) || defined(__osf__)
	    char *data = terminate(f_output).str();
	    f_output.rdbuf()->freeze(0);
#else
	    char *data = (char*)f_streambuf->str().c_str();
//#ifdef _IBMR2
#if !defined(SC3)  && !defined(__osf__)
	    data[f_streambuf->str().size()] = 0;
#endif
#endif
	    cerr << "EndTag: " << data << endl;
	    assert(gElemSymTab->intern(data) == name);
	  }
#endif

// this node
	  if (!f_ignoring_element)
	    {
	      int ignore = f_resolver.beginElement(new Element(name,
							       sibling_number,
							       0, 0, this_sibling_number)); 
	      if (!ignore)
		f_resolver.endElement(name);
	    }
	  return ;		// EXIT FUNCTION 
	  break;
	case AttributeSection:
	  {
#if !defined(SC3) && \
    !defined(__osf__) && \
    !defined(_IBMR2) && \
    !defined(__uxp__) && \
    !defined(USL) && \
    !defined(linux) && \
    !defined(CSRG_BASED)
	    volatile
#endif
	    AttributeList *attrs = 0;
#if !defined(SC3) &&  \
    !defined(__osf__) && \
    !defined(_IBMR2) && \
    !defined(__uxp__) && \
    !defined(USL) && \
    !defined(linux) && \
    !defined(CSRG_BASED)
	    volatile
#endif
	    AttributeList *olias_attrs = 0;

	    mtry
	      {
		process_attributes(input, output, attrs, olias_attrs);

		if (!f_ignoring_element)
		  {
//////////////////////////////
// this node with attributes
//////////////////////////////
		    ignore = f_resolver.beginElement(new Element(name,
								 sibling_number,
								 attrs,
								 olias_attrs, 
								 this_sibling_number
							));
		    f_ignoring_element = ignore ;
		  }
	      }
	    mcatch_any()
	      {
/*
		delete attrs ;
		delete olias_attrs ;
*/
		attrs = 0 ;
		olias_attrs = 0 ;
	      }
	    end_try;
	  }
	  break;
	case OliasAttribute:
	  throw(CASTDPUTEXCEPT docParserUnexpectedTag());
	  break;

	case NoTag:
	  {
	    if (!f_ignoring_element)
	      {
// this node
		ignore = f_resolver.beginElement(new Element(name,
							     sibling_number,
							     0, 0, this_sibling_number)); 
		f_ignoring_element = ignore ;
	      }
	    // process data 
	    read_data(input, output);

	    if (!f_ignoring_element)
	      {
		//  the str() call seems to add the null byte to the stream
		//  and increment the pcount, so we must make sure it gets
		//  called first
#if defined(SC3) || defined(__osf__)
		char *string = terminate(f_output).str();
		int   size = f_output.pcount();
		f_resolver.data(string, size);
		f_output.rdbuf()->freeze(0);
#else
		char *string = (char *)f_streambuf->str().c_str();
//#ifdef _IBMR2
#if !defined(SC3) && !defined(__osf__)
		string[f_streambuf->str().size()] = 0;
		int   size = f_streambuf->str().size() ;
#else
		int   size = f_streambuf->pcount() - 1 ;
#endif
		f_resolver.data(string, size);
#endif
	      }
	  }
	  break;
	}
      
      while ((tt = read_tag(input, output)) != EndTag)
	switch (tt)
	  {
	  case StartTag:
	    {
/////////////////////////////
// second child and beyond.
/////////////////////////////
#if defined(SC3) || defined(__osf__)
	      char *data = f_output.str();
	      *(data + f_output.pcount()) = 0;
	      f_output.rdbuf()->freeze(0);
#else
	      char *data = (char *)f_streambuf->str().c_str();
//#ifdef _IBMR2
#if !defined(SC3) && !defined(__osf__)
              data[f_streambuf->str().size()] = 0;
#endif
#endif

/*
MESSAGE(cerr, "StartTag case2");
debug(cerr, data);
debug(cerr, f_streambuf->pcount ());
*/

	      Symbol name(gElemSymTab->intern(data));
	      update_last_seen_child_name(last_seen_child_name, 
				 child_relative_sibling_number, name);

	      process(input, output, name, child++, child_relative_sibling_number);
	    }
	    break;
	  case EndTag:		// should never get this 
	    break;
	    // we have already processed these for this tag
	  case AttributeSection:
	  case OliasAttribute: 
	    throw(CASTDPUTEXCEPT docParserUnexpectedTag());
	    break;
	  case NoTag:
	    {
	      read_data(input, output);

	      if (!f_ignoring_element)
		{
		  //  the str() call seems to add the null byte to the stream
		  //  and increment the pcount, so we must make sure it gets
		  //  called first
#if defined(SC3) || defined(__osf__)
		  char *string = f_output.str();
		  int   size = f_output.pcount();
		  *(string + size) = 0;
		  f_resolver.data(string, size);
		  f_output.rdbuf()->freeze(0);
#else
		  char *string = (char *)f_streambuf->str().c_str();
//#ifdef _IBMR2
#if !defined(SC3) && !defined(__osf__)
		  string[f_streambuf->str().size()] = 0;
		  int   size = f_streambuf->str().size() ;
#else
		  int   size = f_streambuf->pcount() - 1 ;
#endif
		  f_resolver.data(string, size);
#endif
		}
	    }
	  }
#ifdef DEBUG
      {
#if defined(SC3) || defined(__osf__)
	char *data = terminate(f_output).str();
	f_output.rdbuf()->freeze(0);
#else
	char *data = (char*)f_streambuf->str().c_str();
//#ifdef _IBMR2
#if !defined(SC3) && !defined(__osf__)
	data[f_streambuf->str().size()] = 0;
#endif
#endif
	cerr << "EndTag: " << data << endl;
	assert(gElemSymTab->intern(data) == name);
      }
#endif
      // hit end tag, end processing
      if (!f_ignoring_element)
	f_resolver.endElement(name);

      // if we set ignore flag, unset it 
      if (ignore)
	f_ignoring_element = 0;
    }
  mcatch_any()
    {
      rethrow;
    }
  end_try;
  ON_DEBUG(cerr << "exit process: " << name << endl);
  delete last_seen_child_name; 
}


void
DocParser::process_attributes(istream &input, ostream &output,
			      AttributeList *&attrs,
			      AttributeList *&olias_attrs)
{
  TagType tt ;

  Attribute* newAttribute = 0;

  AttributeList* orig_attrs = attrs;
  AttributeList* orig_olias_attrs = olias_attrs;

  char *theData = 0;

  mtry {
     while ((tt = read_tag(input,output)) != NoTag)
       {
         switch (tt)
   	{
   	case StartTag:
          {
//#ifdef _IBMR2
#if !defined(SC3) && !defined(__osf__)
	  theData = (char *)f_streambuf->str().c_str();
          theData[f_streambuf->str().size()] = 0;
#endif
   	  if (!attrs)
   	    attrs = new AttributeList ;

          newAttribute = 
   		process_attribute(input, output,
#if defined(SC3) || defined(__osf__)
				  gSymTab->intern(terminate(f_output).str()),
#else
//#ifdef _IBMR2
#if !defined(SC3)  && !defined(__osf__)
   				  gSymTab->intern(theData),
#else
   				  gSymTab->intern(f_streambuf->str()),
#endif
#endif
       				  StartTag
				 );
   	  attrs->add(newAttribute);
   	  break;
          }
   	case EndTag:
   	  return ;		// EXIT FUNCTION
   
   	case AttributeSection:
   	  throw(CASTDPUTEXCEPT docParserUnexpectedTag());
   	  break;
   	case OliasAttribute:
//#ifdef _IBMR2
#if !defined(SC3) && !defined(__osf__)
	  theData = (char *)f_streambuf->str().c_str();
          theData[f_streambuf->str().size()] = 0;
#endif
   	  // mirrors attribute 
   	  if (!olias_attrs)
   	    olias_attrs = new AttributeList ;

          newAttribute = 
   		process_attribute(input, output,
#if defined(SC3) || defined(__osf__)
				  gSymTab->intern(terminate(f_output).str()),
#else
//#ifdef _IBMR2
#if !defined(SC3)  && !defined(__osf__)
   				  gSymTab->intern(theData),
#else
   				  gSymTab->intern(f_streambuf->str()),
#endif
#endif
       				  OliasAttribute
   				 );

   	  olias_attrs->add(newAttribute);
   	  break;
   	case NoTag:
   	  throw(CASTDPUDEXCEPT docParserUnexpectedData());
   	  break;
   	}
       }
   }
   mcatch_any()
   {
     delete newAttribute;

     if ( orig_attrs == 0 ) {
        delete attrs;
        attrs = 0;
     }

     if ( orig_olias_attrs == 0 ) {
        delete olias_attrs;
        olias_attrs = 0;
     }

     rethrow;
   }
   end_try;
}

Attribute *
DocParser::process_attribute(istream &input, ostream &output,
			     const Symbol &name, TagType tt)
{
  //ON_DEBUG(cerr << "process_attribute: " << name << endl);

// If the attribute is OLIAS internal, we use DocParser's 
// read_data(). This is to prevent the attribte value 
// from change in a call to specific renderer engine's 
// read_data().
//
// Example: LoutDocparser::read_data() quotes any '.' char
// which changes the graphic locator value if the element
// is OLIAS internal attribute #GRAPHIC.

  if ( tt == OliasAttribute ) {
    DocParser::read_data(input, output);
  } else 
    (void)read_data(input, output);
#if defined(SC3) || defined(__osf__)
  char *data = f_output.str();
  *(data + f_output.pcount()) = 0;
  f_output.rdbuf()->freeze(0);
#else
  char *data = (char *)f_streambuf->str().c_str();
//#ifdef _IBMR2
#if !defined(SC3)  && !defined(__osf__)
  data[f_streambuf->str().size()] = 0;
#endif
#endif
  Attribute *attr = new Attribute(name, strdup(data));

  switch (read_tag(input, output))
    {
    case StartTag:
    case AttributeSection:
    case OliasAttribute:
      delete attr ;
      throw(CASTDPUTEXCEPT docParserUnexpectedTag());
      break;
    case NoTag:
      delete attr;
      throw(CASTDPUDEXCEPT docParserUnexpectedData());
      break;
    case EndTag:
      break;
    }

  return attr ;
  
}


DocParser::TagType
DocParser::read_tag(istream &input, ostream &output)
{
  output.seekp(streampos(0));

  TagType tt = StartTag;

  char c ;

  // strip newlines before/after tags
  while ((input >> c) && (c == '\n'));
  if (input.eof())
    throw(CASTDPUEEXCEPT docParserUnexpectedEof());

  if (c != '<')
    {
      input.putback(c);
      return NoTag;
    }


  input >> c ;

  switch (c)
    {
    case '/':
      tt = EndTag ;
      break;
    case '#':
      input >> c;
      if (c == '>')
	return AttributeSection ; // EXIT 
      else
	{
	  tt = OliasAttribute ;
	  output << c;		// keep char we just read 
	}
      break;
    case '>':
      throw(CASTUTEXCEPT unknownTagException());
      // NOT REACHED 
      break;
    default:
      output << c ;		// keep char we just read 
      break;
    }


  // get (remainder of) tag name 
  while ((input >> c) && (c != '>'))
    output << c ;

  return tt ;
}


void
DocParser::read_data(istream &input, ostream &output)
{
  char c ;

  output.seekp(streampos(0));

  while ((input >> c) && (c != '<'))
    {
      // handle entities 
      if (c == '&')
	{
	  char tmpbuf[64];
	  unsigned int tmplen = 0;
	  while ((input >> c ) && (c != ';'))
	    {
	      tmpbuf[tmplen++] = c ;
	      if (tmplen > 63)
		{
		  cerr << "Temp Buf overflow (ampersand problem)" << endl;
		  throw(CASTEXCEPT Exception());
		}
	    }
	  if (input.eof())
	    throw(CASTDPUEEXCEPT docParserUnexpectedEof());
	    
	  tmpbuf[tmplen] = 0 ;

#ifdef ENTITY_DEBUG
	  cerr << "Entity: " << tmpbuf << endl;
#endif

	  if ((!strcmp(tmpbuf, "hardreturn")) ||
	      (!strcmp(tmpbuf, "lnfeed")))
	    c = '\n';
	  else
	    if ((!strcmp(tmpbuf, "lang")) ||
		(!strcmp(tmpbuf, "lt")))
	      c = '<' ;
	    else
	      if (!strcmp(tmpbuf, "amp"))
		c = '&' ;
	      else
		if (!strcmp(tmpbuf, "nbsp")) // non-break space 
		  c = (char)0xA0 ;
		else
		  c = ' ';

	}

      output << c;
    }

  // can never run out of input while reading data, tags must be balanced
  if (input.eof())
    throw(CASTDPUEEXCEPT docParserUnexpectedEof());
	    
  input.putback(c);

}
