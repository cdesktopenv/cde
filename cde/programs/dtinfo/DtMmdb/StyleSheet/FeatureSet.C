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
// $XConsortium: FeatureSet.cc /main/5 1996/08/05 16:18:55 cde-hal $
#include <stdarg.h>
#include <assert.h>
#include "Feature.h"
#include "FeatureValue.h"
#include "StyleSheetExceptions.h"
#include "Debug.h"

unsigned int FeatureSet::f_print_indent_level = 0 ;

ostream &operator << (ostream &o, const FeatureSet &f)
{
  return f.print(o);
}

FeatureSet::~FeatureSet()
{
  clearAndDestroy();		// clean up memory 
}

FeatureSet::FeatureSet()
{
}

FeatureSet::FeatureSet(const FeatureSet &orig_set)
{
  // cast to non const 
  CC_TPtrSlistIterator<Feature> next(*(CC_TPtrSlist<Feature>*) &orig_set) ;

  // make a copy of each item and add it to our list 
  while (++next)
    append(new Feature(*next.key()));

}

FeatureSet *
FeatureSet::evaluate() const
{
  // I do not yet understand how this evaluate is working well. - TK
#ifdef FS_EVAL_DEBUG
  fprintf(stderr, "(DEBUG) FeatureSet::evaluate() called.\n");
#endif
  return evaluate(new FeatureSet);
}

FeatureSet *
FeatureSet::evaluate(FeatureSet *result_set) const
{
  
  CC_TPtrSlistIterator<Feature> next(*(FeatureSet*)this);
  
  // cause each feature to evaluate itself 
  while(++next)
    {
      FeatureValue *value ;
      mtry
	{
	  value = next.key()->evaluate();
	  result_set->append(new Feature(next.key()->name(),
					 value));
	}
#ifdef UXPDS
      mcatch_any()
#else
      mcatch_noarg(badEvaluationException)
#endif
	{
	  /* do nothing...we just ignore any that will not evaluate */
	}
      end_try;
    }

  return result_set ;
}

FeatureSet::FeatureSet(const FeatureSet &base,
		       const FeatureSet &mixin)
{
  Feature dummy = Feature(gSymTab->intern("FAMILY"),0);
  int contains_family = mixin.contains(&dummy);

  // first duplicate the baseline
  CC_TPtrSlistIterator<Feature> base_i(*(CC_TPtrSlist<Feature>*)&base) ;

  // make a copy of each item and add it to our list 
  while (++base_i) {
      if (! (contains_family &&
		base_i.key()->name() == gSymTab->intern("FAMILY")))
	append(new Feature(*base_i.key()));
  }

  // now merge in mixin

  CC_TPtrSlistIterator<Feature> next(*(CC_TPtrSlist<Feature>*)&mixin);
  
  while (++next)
    {
      if (next.key()->name() == gSymTab->intern("FAMILY"))
	append(new Feature(*next.key()));
      else {
	Feature* mfeature = 0;
	mfeature = find(next.key());
#if 0
	cout << "Merging: \n" <<  *next.key() << endl << "into:" << endl;
	if (mfeature)
	  cout << *mfeature << endl;
	else
	  cout << "(nil)" << endl;
#endif
	if (mfeature)
	  mfeature->merge(*next.key()); // merge it if already exists
	else
	  append(new Feature(*next.key())); // else add it if not there
      }
    }
}

ostream &
FeatureSet::print(ostream &o) const
{
  // cast to non-const to get iterator 
  CC_TPtrSlistIterator<Feature> next(*(CC_TPtrSlist<Feature>*)this);
  
  unsigned int i;
  for (i = 0 ; i < f_print_indent_level; i++)
    o << "  " ;

  o << "{" << endl;
  
  f_print_indent_level++;

  while (++next)
    {
      for (unsigned int i = 0 ; i < f_print_indent_level; i++)
	o << "  " ;
      o << *next.key() << endl ;
    }
  
  --f_print_indent_level;
  for (i = 0 ; i < f_print_indent_level ; i++)
    o << "  " ;

  o << "}" << endl;

  return o;
}

void
FeatureSet::add(Feature *f)
{
  append(f);
}


unsigned int
FeatureSet::operator==(const FeatureSet &fs) const
{
  return &fs == this ;
}

const Feature*
FeatureSet::lookup(const Symbol *symbol) const
{
  return lookup(*symbol);
}

const Feature *
FeatureSet::lookup(const Symbol &name) const
{
  Feature tmp(name, 0);
  return find(&tmp);
}


const Feature *
FeatureSet::lookup(const char *name) const
{
  Feature tmp(gSymTab->intern(name),0);
  return find(&tmp);
}

const Feature *
FeatureSet::deep_lookup(const char *first_name ...) const
{
  const Feature *feature = lookup(first_name);

  if (!feature)
    return 0; 

  const FeatureSet *featureset = 0;
  
  va_list ap;
  va_start(ap, first_name);


  for (;;)
    {
      const char *p = va_arg(ap, char*);
      if (p == 0)
	break;

      if (feature->value()->type() != FeatureValue::featureset)
	{
	  va_end(ap);
	  return 0 ;
	}

      featureset = ((const FeatureValueFeatureSet *)feature->value())->value();

      feature = featureset->lookup(p);

      if (!feature)
	{
	  va_end(ap);
	  return 0;
	}
    }
  va_end(ap);
  return feature ;
}

const Feature *
FeatureSet::deep_lookup(const Symbol *first_name ...) const
{
  const Feature *feature = lookup(*first_name);

  if (!feature)
    return 0; 

  const FeatureSet *featureset = 0;
  
  va_list ap;
  va_start(ap, first_name);


  for (;;)
    {
      const Symbol *sym = va_arg(ap, const Symbol *);
      if (sym == 0)
	break;

      if (feature->value()->type() != FeatureValue::featureset)
	{
	  va_end(ap);
	  return 0 ;
	}

      featureset = ((const FeatureValueFeatureSet *)feature->value())->value();

      feature = featureset->lookup(*sym);

      if (!feature)
	{
	  va_end(ap);
	  return 0;
	}
    }
  va_end(ap);
  return feature ;
}

const Feature *
FeatureSet::deep_lookup(const dlist_array<Symbol> &vec) const
{
  unsigned int index = 0;
  const Feature *feature = lookup(*vec[index++]);
  if (!feature)
    return 0;

  const FeatureSet *set = 0;

  unsigned int entries = vec.entries();
  for (; index < entries ; index++ )
    {
      if (feature->value()->type() != FeatureValue::featureset)
	return 0 ;
      
      set = ((const FeatureValueFeatureSet *)feature->value())->value();
      feature = set->lookup(*vec[index++]);
      if (!feature)
	return 0 ;
    }
  return feature ;
}


void
FeatureSet::removeFeature(const char *first_name ...) 
{
  const Feature *feature = lookup(first_name);

  if (!feature)
    return ; 

  FeatureSet *featureset = this;
  
  va_list ap;
  va_start(ap, first_name);


  for (;;)
    {
      const char *p = va_arg(ap, char*);
      if (p == 0)
	break;

      if (feature->value()->type() != FeatureValue::featureset)
	{
	  va_end(ap);
	  return ;
	}

      featureset = (FeatureSet*)
	(((const FeatureValueFeatureSet *)feature->value())->value());

      feature = featureset->lookup(p);

      if (!feature)
	{
	  va_end(ap);
	  return ;
	}
    }
  va_end(ap);

  delete (featureset -> remove((Feature *)feature));
}
