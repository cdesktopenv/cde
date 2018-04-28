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
/* $XConsortium: FeatureDefDictionary.h /main/4 1996/08/21 15:50:25 drk $ */

#ifndef _featureNameDictionary_h
#define _featureNameDictionary_h

#ifndef CDE_NEXT



#else
//#include "StyleSheet/cde_next.h"
#include "dti_cc/CC_Slist.h"
#include "dti_cc/cc_hdict.h"
#include "dti_cc/CC_String.h"
#endif

#include "StyleSheet/Types.h"
#include "StyleSheet/SymTab.h"
#include "StyleSheet/FeatureValue.h"
#include "StyleSheet/Feature.h"
#include "StyleSheet/StyleSheetExceptions.h"

typedef CC_TPtrSlist<FeatureValue> defv_t; 
typedef CC_TPtrSlistIterator<FeatureValue> defv_iterator_t; 

class TypeValues : public Destructable
{
public:

   enum feature_t {real, integer, string, featureset, unit} ;
   enum unit_t { INCH=0, PICA=1, POINT=2, CM=3, PIXEL=4, NONE=5 };

   TypeValues(char* type, defv_t*);
   ~TypeValues();

   unsigned int operator==(const TypeValues&);

   unsigned check(const FeatureValue*);

   friend ostream& operator <<(ostream&, TypeValues&);
   ostream& print(ostream&, int tabs) const;

protected:
   CC_String f_type;
   defv_t* f_default_values;

protected:
};

class FeatureDef;

typedef CC_TPtrSlist<FeatureDef> def_list_t; 
typedef CC_TPtrSlistIterator<FeatureDef> def_list_iterator_t; 

typedef CC_TPtrSlist<TypeValues> type_values_list_t; 
typedef CC_TPtrSlistIterator<TypeValues> type_values_list_iterator_t; 
	

class FeatureDef  : public Destructable
{
public:

   FeatureDef(const char* name);
   virtual ~FeatureDef();

   unsigned int operator==(const FeatureDef&);

   friend ostream& operator <<(ostream&, FeatureDef&) ;

   enum def_type_t { PRIMITIVE, COMPOSITE, REFERENCE, WILDCARD };
   virtual unsigned type() const = 0;
   virtual const CC_String* name() const { return &f_name; };

   virtual ostream& print(ostream&, int tabs)  const= 0;
   virtual unsigned checkContent(const Feature*) const = 0;

protected:
   CC_String f_name;

protected:
};

class FeatureDefComposite : public FeatureDef
{
public:
   FeatureDefComposite(const char* name, def_list_t*);
   ~FeatureDefComposite();

   unsigned checkContent(const Feature*)const ;

   const FeatureDef* getComponentDef(const char*) const;

   unsigned type() const { return COMPOSITE; };

protected:
   def_list_t* f_components;

protected:
   virtual ostream& print(ostream&, int tabs) const;
}
;

class FeatureDefPrimitive: public FeatureDef
{
public:
   FeatureDefPrimitive(const char* name, type_values_list_t* tvslist);
   ~FeatureDefPrimitive();

   unsigned type() const { return PRIMITIVE; };

   unsigned checkContent(const Feature*)const ;

protected:
   type_values_list_t* f_typeValuesList;

protected:
   virtual ostream& print(ostream&, int tabs) const;
}
;

class FeatureDefReference : public FeatureDef
{
public:
   FeatureDefReference(const char* name) : FeatureDef(name) {};
   ~FeatureDefReference() {};

   unsigned type() const { return REFERENCE; };
   unsigned checkContent(const Feature*)const ;

protected:

protected:
   virtual ostream& print(ostream&, int tabs) const;
}
;

class FeatureDefWildCard : public FeatureDef
{
public:
   FeatureDefWildCard(const char* name) : FeatureDef(name) {};
   ~FeatureDefWildCard() {};

   unsigned type() const { return WILDCARD; };
   unsigned checkContent(const Feature*)const ;

protected:

protected:
   virtual ostream& print(ostream&, int tabs) const;
}
;

//typedef hashTable<FeatureDef, FeatureDef> featureDefDictionary_t;
//typedef hashTableIterator<FeatureDef, FeatureDef> featureDefDictionary_iterator_t;

class featureDefDictionary  : public Destructable
{
protected:
	def_list_t* f_def_list;

protected:
	const FeatureDef* getDef(const char*);
	const FeatureDef* getDef(const Feature*);
	//const FeatureDef* getDef(const Feature*, const Feature*);
        unsigned _checkSemantics(const Feature* f, const FeatureDef*);

public:
	featureDefDictionary();
	~featureDefDictionary();
	
	void addFeatureDefs(def_list_t*);

	unsigned checkSemantics(const FeatureSet*);

//format:
//
//   featureName
//	ComponentName : Type [: Default Values (',' separated)]
//	ComponentName : Type [: Default Values (',' separated)]
//   featureName
//	ComponentName : Type [: Default Values (',' separated)]
//	ComponentName : Type [: Default Values (',' separated)]
//
//   Example:
//   font
//      size : Unit 
//	weight : String : "medium", "bold"
//
	friend istream& operator >>(istream&, featureDefDictionary&);
	friend ostream& operator <<(ostream&, featureDefDictionary&);
};

#endif
