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
/* $XConsortium: Feature.h /main/4 1996/08/21 15:50:21 drk $ */
#ifndef _Feature_h
#define _Feature_h

#include <sstream>

#ifndef CDE_NEXT


#else
#include "dti_cc/CC_Slist.h"
#include "dti_cc/cc_povec.h"
#endif

#include "SymTab.h"

class Element;
class Expression;
class FeatureValue;
class VariableTable;
class ResolverStackElement ;
class Symbol;

/*
#ifndef CDE_NEXT
class dlist_array<Symbol> ;
#else
class dlist_array<Symbol> ;
#endif
*/


/* **************************************************************
 * class Feature

 a symbol, value pairing where symbol is the feature name
 * ************************************************************** */


class Feature
{
public:
  Feature(const Symbol &, FeatureValue *);
  Feature(const Feature &);
  ~Feature();


  const Symbol       &name()	const	{ return f_name ; }
  const FeatureValue *value()	const	{ return f_value ; }	

  FeatureValue *evaluate() const;

  // destructively modifies f_value 
  void merge(const Feature &);

  ostream &print(ostream &) const ;

  unsigned int operator==(const Feature&);

private:
  void assembleChainFeatures(FeatureValue* x);

private:
  Symbol	f_name ;
  FeatureValue *f_value;

};

/* **************************************************************
 * class FeatureSet

 FeatureSet is is a list of features and their values. Features are
 represented by symbols. Values can be strings, numbers (int or real)
 or FeatureSet
 * ************************************************************** */

#ifndef CDE_NEXT
class FeatureSet : public CC_TPtrSlist<Feature>
#else
class FeatureSet : public CC_TPtrSlist<Feature>
#endif
{
public:
  FeatureSet();
  FeatureSet(const FeatureSet &); /* copy */
  FeatureSet(const FeatureSet &,
	     const FeatureSet &); /* merge */
  virtual ~FeatureSet();

  void			add(Feature *);
  const Feature	       *lookup(const Symbol *) const ;
  const Feature	       *lookup(const Symbol &) const;
  const Feature	       *lookup(const char *) const;

  // find a chain, eg for "prefix.font.size" use */
  // deep_lookup("prefix","font","size", 0); 
  // returns 0 if not found 
  const Feature	       *deep_lookup(const char * ...) const ;
  const Feature	       *deep_lookup(const Symbol * ...) const ;


#ifndef CDE_NEXT
  const Feature	       *deep_lookup(const dlist_array<Symbol> &) const ;
#else
  const Feature	       *deep_lookup(const dlist_array<Symbol> &) const ;
#endif

// remve a feature that is specified by a chain
  void removeFeature(const char * ...) ;

  // returns new feature set with all unresolved expressions resolved
  FeatureSet *evaluate() const; 
  // evaluate self and place answers into result_set, returns result set
  FeatureSet *evaluate(FeatureSet *result_set) const ;

  unsigned int operator == (const FeatureSet &) const ;

  ostream &print(ostream &) const ;

private:

  static unsigned int	f_print_indent_level ;
  
};

/* **************************************************************
 * Printing
 * ************************************************************** */

ostream &operator << (ostream &o, const Feature &f);
ostream &operator << (ostream &o, const FeatureSet &f);


#endif /* _Feature_h */
/* DO NOT ADD ANY LINES AFTER THIS #endif */
