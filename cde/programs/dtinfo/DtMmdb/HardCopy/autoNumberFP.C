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
/* $TOG: autoNumberFP.C /main/7 1998/04/17 11:47:29 mgreess $ */

#include "autoNumberFP.h"
#include "StyleSheet/Const.h"
#include "StyleSheet/Expression.h"

autoNumberFP gAutoNumberFP;

#ifndef CDE_NEXT
typedef CC_TPtrSlistIterator<autoNumber> autoNumberListIteratorT;
unsigned ANP_StringHash(const CC_String& str)
{
   return str.hash();
}
#else
typedef CC_TPtrSlistIterator<autoNumber> autoNumberListIteratorT;
unsigned ANP_StringHash(const CC_String& str)
{
   return str.hash();
}
#endif


///////////////////////////////////////////////
// static member initialization
///////////////////////////////////////////////

/*
f_autoNumberSet_t autoNumberFP::f_autoNumberSet(ANP_StringHash);
f_resetControlList_t autoNumberFP::f_resetControlList(ANP_StringHash);
f_registerList_t autoNumberFP::f_registerList(ANP_StringHash);
*/

///////////////////////////////////////////////
//
///////////////////////////////////////////////


autoNumberFP::autoNumberFP() :
f_autoNumberSet(ANP_StringHash),
f_resetControlList(ANP_StringHash),
f_registerList(ANP_StringHash)
{
}

autoNumberFP::~autoNumberFP() 
{
   f_autoNumberSet.clearAndDestroy();
   f_resetControlList.clearAndDestroy();
   f_registerList.clearAndDestroy();
}

const char* autoNumberFP::stringToCharPtr(const FeatureValue* f)
{
   if ( f ) {
      if ( f -> type() == FeatureValue::string ) {
         const char* x = *f;
         return x;
      } else
         throw(CASTHCREXCEPT hardCopyRendererException());
   }
   return 0;
}

#if 0
// reset autonumbers
void autoNumberFP::resetAutoNumbers(const char* name)
{
   CC_String key(name);

   autoNumberListT* anList = f_resetControlList.findValue(&key);

   if ( anList == 0 ) 
      return;

   autoNumberListIteratorT l_iter(*anList);  

   while ( ++l_iter ) {
      l_iter.key() -> reset();
   }
}
#endif

void
autoNumberFP::pushAutoNumbers(const char* giname)
{
  if (giname && *giname)
  {
    CC_String key(giname);

    autoNumberListT* anList = f_resetControlList.findValue(&key);

    if (anList)
    {
      autoNumberListIteratorT l_iter(*anList);
      while ( ++l_iter )
	l_iter.key()->push();
    }
  }
}

void
autoNumberFP::popAutoNumbers(const char* giname)
{
  if (giname && *giname)
  {
    CC_String key(giname);

    autoNumberListT* anList = f_resetControlList.findValue(&key);

    if (anList)
    {
      autoNumberListIteratorT l_iter(*anList);
      while ( ++l_iter )
	l_iter.key()->pop();
    }
  }
}

// update autonumbers
void autoNumberFP::updateAutoNumbers(const char* name)
{
   CC_String key(name);
   autoNumberListT* anList = f_registerList.findValue(&key);

   if ( anList == 0 )
      return;

   autoNumberListIteratorT l_iter(*anList);

   while ( ++l_iter ) {
      l_iter.key() -> setNextValue();
   }
}


//////////////////////////////////////////////////////////////////////////
//
// Array fields:
// Type, InitValue, Delta, RegisterList, ControlList, [Prefix], [PostFix]
//
// Type, Pretfix and Postfix are strings.
// RegisterList and ControlList are array.
// InitValue and Delta are integers.
// Prefix and Postfix are optional
//////////////////////////////////////////////////////////////////////////
void 
autoNumberFP::defineAutoNumber(const char* nm, const FeatureValue* f)
{
   if ( f -> type() != FeatureValue::array ) {
      debug(cerr, f -> type());
      cerr << "Autonumber: should use an array to define.\n";
      throw(CASTHCREXCEPT hardCopyRendererException());
   }

   FeatureValueArray* fa = (FeatureValueArray*)f;

   if ( fa -> length() != 5 ) {
      cerr << "Autonumber: invalid number of arguments.\n";
      throw(CASTHCREXCEPT hardCopyRendererException());
   }

// name
   const char* name = nm;

// type
   if ( (*fa)[0] -> type() != FeatureValue::string ) {
      cerr << "Autonumber: type should be a string.\n";
      throw(CASTHCREXCEPT hardCopyRendererException());
   } 

   const char* type = stringToCharPtr((*fa)[0]);

// init value
   if ( (*fa)[1] -> type() != FeatureValue::string ) {
      cerr << "Autonumber: initial value should be a string.\n";
      throw(CASTHCREXCEPT hardCopyRendererException());
   } 
   const char* initvalue = stringToCharPtr((*fa)[1]);

// delta
   if ( (*fa)[2] -> type() != FeatureValue::string ) {
      cerr << "Autonumber: delta value should be a string.\n";
      throw(CASTHCREXCEPT hardCopyRendererException());
   } 
   int delta = atoi(stringToCharPtr((*fa)[2]));


// register list
   FeatureValueArray* registerList = 0; 
   if ( (*fa)[3] -> type() != FeatureValue::array ) {
      cerr << "Autonumber: counter list should be an array\n.";
      throw(CASTHCREXCEPT hardCopyRendererException());
   } else {
      registerList = (FeatureValueArray*)(*fa)[3]; 
   }

// control list
   FeatureValueArray* controlList = 0; 
   if ( (*fa)[4] -> type() != FeatureValue::array ) {
      cerr << "Autonumber: reset list should be an array\n.";
      throw(CASTHCREXCEPT hardCopyRendererException());
   } else {
      controlList = (FeatureValueArray*)(*fa)[4]; 
   }

// prefix
   const char* prefix = "";

// postfix
   const char* postfix = "";

//////////////////////////////////
// create the autonumber object
//////////////////////////////////
   autoNumber* an = 0;
   if ( strcasecmp(type, AUTO_NUMBER_NUMERIC) == 0 )
      an = new autoNumberNumeric(
		name, delta, atoi(initvalue), prefix, postfix
				);
   else
   if ( strcasecmp(type, AUTO_NUMBER_ALPHABETIC_UPPERCASE) == 0 ) {
      an = new autoNumberAlphabetic(
	   name, delta, autoNumberCased::UPPER, initvalue, prefix, postfix
				   );
   } else
   if ( strcasecmp(type, AUTO_NUMBER_ALPHABETIC_LOWERCASE) == 0 ) {
      an = new autoNumberAlphabetic(
	   name, delta, autoNumberCased::LOWER, initvalue, prefix, postfix
				   );
   } else
   if ( strcasecmp(type, AUTO_NUMBER_ROMAN_UPPERCASE) == 0 )
      an = new autoNumberRoman(
	   name, delta, autoNumberCased::UPPER, initvalue, prefix, postfix
			      );
   else 
   if ( strcasecmp(type, AUTO_NUMBER_ROMAN_LOWERCASE) == 0 )
      an = new autoNumberRoman(
	   name, delta, autoNumberCased::LOWER, initvalue, prefix, postfix
			      );
   else {
      MESSAGE(cerr, form("unknown type: %s", type));
      cerr << form("Autonumber: unknown type %s.", type) << "\n";
      throw(CASTHCREXCEPT hardCopyRendererException());
   }

//////////////////////////
// log the new autonumber
//////////////////////////
   CC_String *key = new CC_String(name);

   if ( f_autoNumberSet.findValue(key) ) {
      delete key;
      delete an;
      return;
   } else
      f_autoNumberSet.insertKeyAndValue(key, an);
   
///////////////////////////////
// log into reset control list
///////////////////////////////

   const char* gi = 0;
   autoNumberListT* anList = 0;
   int i;
   for (i=0; i<controlList -> length(); i++ ) {

      gi = stringToCharPtr((*controlList)[i]);
      key = new CC_String(gi);

      anList = f_resetControlList.findValue(key);

      if ( anList == 0 ) {
        anList = new autoNumberListT();
        f_resetControlList.insertKeyAndValue(key, anList);
      } else
        delete key;

      anList -> append(an);
   }

////////////////////////////////
// log into register list
////////////////////////////////
   for (i=0; i<registerList -> length(); i++ ) {

      gi = stringToCharPtr((*registerList)[i]);
      key = new CC_String(gi);

      anList = f_registerList.findValue(key);

      if ( anList == 0 ) {
        anList = new autoNumberListT();
        f_registerList.insertKeyAndValue(key, anList);
      } else
         delete key;

      anList -> append(an);
   }
}

void autoNumberFP::setSeenTagStatus(const char* tagName)
{
   CC_String key(tagName);
   autoNumberListT* anList = f_registerList.findValue(&key);

   if ( anList == 0 ) 
     return;

   autoNumberListIteratorT next(*anList);
   while ( ++next ) {
      next.key() -> setNumTagsSeen();
   }
}


FeatureValue* 
autoNumberFP::evaluate(const char* varName) 
{
   CC_String key(varName);
   autoNumber* an = f_autoNumberSet.findValue(&key);

   if ( an == 0 ) {
      MESSAGE(cerr, form("Warning: unknown autonumber name %s.", varName));
      return 0;
   } else
      return new FeatureValueString(an -> getValue());
}

unsigned int 
autoNumberFP::accept(const char* name, const Expression* expr) 
{
   FeatureValue* fv = 0;

   mtry
    {
      fv = expr -> evaluate();
    }
   mcatch_any()
    {
      return false;
    }
   end_try;

/*
debug(cerr, name);
fv -> print(cerr);
*/

   if ( fv -> type() != FeatureValue::array ) {
     delete fv;
     return false;
   }

   FeatureValueArray* fvArray = (FeatureValueArray*) fv;

/*
debug(cerr, fvArray -> length());
debug(cerr, fvArray -> name());
*/

   if ( fvArray -> length() >= 1 &&
        strcasecmp(fvArray -> name(), AUTO_NUMBER) == 0 
      ) 
   {
      defineAutoNumber(name, fv);
      delete fv;
      return true;
   } else {
      delete fv;
      return false;
   }
}

void
autoNumberFP::beginElement(const Element& element)
{
  const char* giname = element.gi().name();

  if (giname && *giname)
  {
    pushAutoNumbers(giname);

    setSeenTagStatus(giname);
    updateAutoNumbers(giname);
  }
#ifdef DEBUG
  else
    abort();
#endif
}

void
autoNumberFP::endElement(const Symbol& sym)
{
  const char* giname = sym.name();

  if (giname && *giname)
  {
    popAutoNumbers(giname);
  }
#ifdef DEBUG
  else
    abort();
#endif
}

#if 0
void autoNumberFP::preEvaluate(const Element& element)
{
   setSeenTagStatus(element.gi().name());
   updateAutoNumbers(element.gi().name());
}

void autoNumberFP::postEvaluate(const Element& element)
{
   resetAutoNumbers(element.gi().name());
}
#endif

void autoNumberFP::clear()
{
   f_autoNumberSet.clearAndDestroy();
   f_resetControlList.clearAndDestroy();
   f_registerList.clearAndDestroy();
}

// reset All autonumbers
void autoNumberFP::resetAllAutoNumbers()
{
   hashTableIterator<CC_String, autoNumberListT> l_rc_iterator(f_resetControlList);

   while ( ++l_rc_iterator ) {
     
      autoNumberListT* anList = l_rc_iterator.value();

      autoNumberListIteratorT l_iter(*anList);

      while ( ++l_iter ) {
         l_iter.key() -> reset();
      }
   }
}

