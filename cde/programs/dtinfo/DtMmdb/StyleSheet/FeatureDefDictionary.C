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
// $TOG: FeatureDefDictionary.C /main/4 1998/04/17 11:48:58 mgreess $


#include "StyleSheet/FeatureDefDictionary.h"
#include "StyleSheet/Debug.h"
#include "StyleSheet/StyleSheetExceptions.h"
#include "utility/const.h"
#include "utility/funcs.h"
#include <iostream>
using namespace std;

featureDefDictionary* g_FeatureDefDictionary = 0;

extern void report_error_location();

extern def_list_t* g_def_list;
extern FILE *defParserin;
extern int defParserparse();
istream* g_defParserin = 0;

unsigned g_validation_mode = false;
unsigned g_hasSemanticError = false;


ostream& out_tabs(ostream& out, int tabs) 
{
   for (int i=0; i<tabs; i++)
      out << "\t";

   return out;
}

TypeValues::TypeValues(char* t, defv_t* dv) : 
	f_type(t), f_default_values(dv)
{
}

TypeValues::~TypeValues()
{
   if ( f_default_values ) {
      f_default_values -> clearAndDestroy();
      delete f_default_values;
   }
}

unsigned int TypeValues::operator==(const TypeValues& def)
{
   return !f_type.compareTo(def.f_type, CC_String::ignoreCase);
}

ostream& operator <<(ostream& out, TypeValues& tvs) 
{
   return tvs.print(out, 0);
}

unsigned TypeValues::check(const FeatureValue* fv)
{
/*
MESSAGE(cerr, "TypeValues::check()");
debug(cerr, fv -> type());
*/

// type check
   switch ( fv -> type() ) {
     case FeatureValue::real:
     case FeatureValue::integer:
//debug(cerr, f_type.data());
       if ( strcasecmp(f_type.data(), "REAL") == 0 ||
            strcasecmp(f_type.data(), "INTEGER") == 0 
       )
          break;
       else
          return false;

     case FeatureValue::string:
//debug(cerr, f_type.data());
       if ( strcasecmp(f_type.data(), "STRING") == 0 ||
            strcasecmp(f_type.data(), "STRING_PREFIX") == 0 
       )
          break;
       else
          return false;

     case FeatureValue::symbol:
     case FeatureValue::expression:
     case FeatureValue::featureset:
          return false;

//debug(cerr, f_type.data());
     case FeatureValue::dimension:
       if ( strcasecmp(f_type.data(), "DIMENSION") == 0 ||
            strcasecmp(f_type.data(), "DIMENSION_PIXEL") == 0 
       )
          break;
       else
          return false;

     case FeatureValue::array:
//debug(cerr, f_type.data());
       if ( strcasecmp(f_type.data(), "ARRAY") == 0 )
          break;
       else
          return false;

   }

// value check
   if ( f_default_values == 0 ) {
//MESSAGE(cerr, "check() Passed");
     return true;
   }

   defv_iterator_t next(*f_default_values);
   const char* x, *y; 

   while (++next) {

/////////////////////////////////////////////////////////////
// trap string type as FeatureValueString::operator==() uses
// case sensitive comparsion
/////////////////////////////////////////////////////////////
     if ( fv -> type() == FeatureValue::string &&
          next.key() -> type() == FeatureValue::string ) 
     {
        x = *((FeatureValueString*)next.key());
        y = *(FeatureValueString*)fv;

//debug(cerr, x);
//debug(cerr, y);

        if ( strcasecmp(f_type.data(), "STRING_PREFIX") == 0 ) {
          if ( strncasecmp(x, y, strlen(x)) == 0 )
             return true;
        } else {
          if ( strcasecmp(x, y) == 0 )
             return true;
        }

     } else

     if ( next.key() -> operator==(*fv) == true ) {
//MESSAGE(cerr, "check() Passed");
       return true ;
     }
   }

   return false;
}

ostream& TypeValues::print(ostream& out, int tabs) const
{
   out_tabs(out, tabs) << f_type << "\n";

   if ( f_default_values ) {
      defv_iterator_t NextValue (*f_default_values);
      while (++NextValue) {
             out_tabs(out, tabs+1) << *(NextValue.key()) << "\n";
      }
   }

   return out;
}

unsigned hash(const FeatureDef& key)
{
   return key.name() -> hash();
}

FeatureDef::FeatureDef(const char* name) : f_name(name)
{
}

FeatureDef::~FeatureDef()
{
}

ostream& operator << (ostream& out, FeatureDef& def)
{
   return def.print(out, 0);
}

unsigned int FeatureDef::operator==(const FeatureDef& def)
{
//debug(cerr, f_name);
//debug(cerr, def.f_name);
//   unsigned ok = ! f_name.compareTo(def.f_name, CC_String::ignoreCase);
//debug(cerr, ok);
//return ok;

   return !f_name.compareTo(def.f_name, CC_String::ignoreCase);
}

FeatureDefComposite::FeatureDefComposite(const char* name, def_list_t* dl) :
   FeatureDef(name), f_components(dl)
{
}

FeatureDefComposite::~FeatureDefComposite()
{
   if ( f_components ) {
      f_components -> clearAndDestroy();
      delete f_components;
   }
}

CC_Boolean compareFunc(FeatureDef* fd, void* nm)
{
   if ( strcasecmp( fd -> name() -> data(), (char*)nm ) == 0 )
      return TRUE;
   else
      return FALSE;
}

// return true:
//    fv confirms to the spec of a component of this def.
// return false:
//    otherwise
unsigned FeatureDefComposite::checkContent(const Feature* fv) const
{
/*
   const FeatureDef* def = getComponentDef((fv->name()).name());

   if ( def == 0 )
     return false;

   if ( def -> type() != FeatureDef::PRIMITIVE )
     return false;

   return ((FeatureDefPrimitive*)def) -> checkContent(fv);
*/
   return true;
}

const FeatureDef* FeatureDefComposite::getComponentDef(const char* nm) const
{
   if ( f_components == 0 )
      return 0;
   else {
      FeatureDef* def = f_components -> find(compareFunc, (void*)nm);
    
      if ( def )
         return def;
      else {
         if ( f_components -> first() -> type() == WILDCARD )
            return f_components -> first();
         else
            return 0;
      }
//return f_components -> find(compareFunc, "*");
   }
}

ostream& FeatureDefComposite::print(ostream& out, int tabs) const
{
    out_tabs(out, tabs) << f_name << "\n";

    if ( f_components ) {
       def_list_iterator_t NextComponent(*f_components);
       while (++NextComponent) {
               NextComponent.key() -> print(out, tabs+1) << "\n";
       }
    }
   return out;
}

FeatureDefPrimitive::FeatureDefPrimitive(const char* name, type_values_list_t* tvl) :
   FeatureDef(name), f_typeValuesList(tvl)
{
}

FeatureDefPrimitive::~FeatureDefPrimitive()
{
   if ( f_typeValuesList ) {
      f_typeValuesList -> clearAndDestroy();
      delete f_typeValuesList;
   }
}

unsigned FeatureDefPrimitive::checkContent(const Feature* f) const
{
/*
MESSAGE(cerr, "FeatureDefPrimitive::checkContent");
f -> print(cerr);
MESSAGE(cerr, "");
debug(cerr, *(this -> name()));
MESSAGE(cerr, "");
*/

   if ( f_typeValuesList == 0 ) {
     report_error_location();
     cerr << "No type definition.\n";
     return false;
   }

   FeatureValue * fv = 0;
   mtry {
     fv = f -> evaluate();
   }
   mcatch (undefinedAttributeException&, e) {
      return true;
   }

   mcatch (undefinedVariableException&, e) {
      report_error_location();
      cerr << "Undefined variable error.\n";
      return false;
   }

/*
   mcatch (badCastException&, e) {
      report_error_location();
      cerr << "Evaluating expression error.\n";
      return false;
   }

   mcatch (badEvaluationException&, e) {
      report_error_location();
      cerr << "Evaluating expression error.\n";
      return false;
   }
*/

   mcatch_any() {
      //report_error_location();
      //cerr << "There might be an error in the expression.\n";
      return true;
   }
   end_try;
   
/*
debug(cerr, int(fv));
fv -> print(cerr);
MESSAGE(cerr, "");
*/

   if ( fv == 0 ) {
      report_error_location();
      cerr << "Error in evaluating an expression.\n";
      return false;
   }

   type_values_list_iterator_t next(*f_typeValuesList);

   while ( ++ next ) {
      if ( next.key() -> check(fv) == true ) {
        delete fv;
        return true ;
      }
   }

   delete fv;
   report_error_location();
   cerr << form("Unmatched feature type or illegal feature value: %s.\n", 
                (f -> name()).name()
               );
   return false;
}

ostream& FeatureDefPrimitive::print(ostream& out, int tabs) const
{
    out_tabs(out, tabs) << f_name << "\n";

    if ( f_typeValuesList ) {
       type_values_list_iterator_t NextValue (*f_typeValuesList);
       while (++NextValue) {
               NextValue.key() -> print(out, tabs+1) << "\n";
       }
    }
    return out;
}

unsigned FeatureDefReference::checkContent(const Feature* fv) const
{
   return false;
}

ostream& FeatureDefReference::print(ostream& out, int tabs) const
{
    out_tabs(out, tabs) << f_name << "\n";
    return out;
}

unsigned FeatureDefWildCard::checkContent(const Feature* fv) const
{
   return true;
}

ostream& FeatureDefWildCard::print(ostream& out, int tabs) const
{
    out_tabs(out, tabs) << f_name << "\n";
    return out;
}

featureDefDictionary::featureDefDictionary() : f_def_list(0)
{
}

featureDefDictionary::~featureDefDictionary()
{
   if ( f_def_list ) {
     f_def_list -> clearAndDestroy();
     delete f_def_list;
   }
}
	
void 
featureDefDictionary::addFeatureDefs(def_list_t* fdefs)
{
   f_def_list = fdefs;

   if ( f_def_list == 0 )
      throw(CASTEXCEPT Exception());
}
    
const FeatureDef* 
featureDefDictionary::getDef(const char* nm)
{
//debug(cerr, nm);

   FeatureDefReference key((char*)nm);
   return f_def_list -> find(&key);

/*
 FeatureDef* def = 0;
 def = f_def_list -> find(&key);

debug(cerr, int(def));
   return def;
*/
}

const FeatureDef* 
featureDefDictionary::getDef(const Feature* f)
{
   return getDef((f -> name()).name());
}

unsigned
featureDefDictionary::checkSemantics(const FeatureSet* fs)
{
/*
MESSAGE(cerr, "check feature set:");
fs -> print(cerr);
MESSAGE(cerr, "");
*/

  const FeatureDef* def = 0;
  const Feature *f = 0;

  CC_TPtrSlistIterator<Feature> next(*(CC_TPtrSlist<Feature>*)fs);
  while (++next) {
     f = next.key();
     def = getDef(f);

     if ( def == 0 ) {
        report_error_location();
        cerr << form("Unknown feature name %s.\n", 
                     ((next.key()) -> name()).name());
        return false;
     }

     if ( _checkSemantics(f, def) == false )
       return false;
  }
  return true;
}

unsigned
featureDefDictionary::_checkSemantics(const Feature* f, const FeatureDef* def)
{
   const FeatureSet *featureset = 0;
   const Feature *child_f= 0;

/*
MESSAGE(cerr, "_checkSemantics");
f -> print(cerr);
MESSAGE(cerr, "");
def -> print(cerr, 0);
MESSAGE(cerr, "");
*/

        
   const FeatureDef* child_def = 0;

   if ( def -> checkContent(f) == false ) {
      return false;
   }

   if (f -> value()->type() == FeatureValue::featureset &&
       def -> type() == FeatureDef::COMPOSITE )
   {
//MESSAGE(cerr, "it is a feature set");


      featureset = 
        ((const FeatureValueFeatureSet *)(f -> value()))->value();

      CC_TPtrSlistIterator<Feature> next(*(CC_TPtrSlist<Feature>*)featureset);

      const char* nm;

      while ( ++ next ) {

        nm = ((next.key()) -> name()).name();

/*
debug(cerr, nm);
report_error_location();
*/

        child_def = ((FeatureDefComposite*)def) -> getComponentDef(nm);

//debug(cerr, int(child_def));
        if ( child_def == 0 ) {
          report_error_location();
          cerr << form("%s is a undefined feature.\n", nm);
          return false;
        }

        switch ( child_def -> type() ) {
          case FeatureDef::REFERENCE:
           child_def = getDef(child_def -> name() -> data());
           break;
          case FeatureDef::WILDCARD:
           child_def = getDef(nm);
           break;
          default:
           break;
        }

        child_f = next.key();

        if ( _checkSemantics(child_f, child_def) == false )
           return false;

      }

      return true;
   } 
   return true;
}

istream& operator >>(istream& in, featureDefDictionary& dict)
{
   g_defParserin = &in;

   int ok = defParserparse();
   if ( ok != 0 ) {
      MESSAGE(cerr, "bad feature definition file");
      throw(CASTEXCEPT Exception());
   }

   dict.addFeatureDefs(g_def_list);

   return in;
}

ostream& operator <<(ostream& out, featureDefDictionary& dict)
{
   def_list_iterator_t Next(*dict.f_def_list);

   while (++Next) {
//debug(cerr, int(Next.key()));
      out << *Next.key() << "\n"; 
   }
   return out;
}
