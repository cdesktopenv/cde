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
/* $XConsortium: HardCopyFP.h /main/3 1996/06/11 17:04:22 cde-hal $ */

#ifndef _fp_h
#define _fp_h 1

#include <iostream>
using namespace std;
#include "FPExceptions.h"
#include "StyleSheet/Feature.h"
#include "StyleSheet/FeatureValue.h"
#include "StyleSheet/Element.h"
#include "StyleSheet/Const.h"
#include "StyleSheet/Debug.h"
#include "utility/funcs.h"

enum BLOCK_MODE { NON_BLOCK, BLOCK };
extern enum BLOCK_MODE g_block_mode;

class featureProcessor : public Destructable
{

protected:
   const char* f_name;

   virtual const char* stringToCharPtr(const FeatureValue*) = 0;
   virtual unsigned int  dimensionToFloat(float& Y, 
			FeatureValueDimension::Unit& UnitOfY,
			const FeatureValue*,
		        FeatureValueDimension::Unit) = 0;
   virtual const char* dimensionToCharPtr(const FeatureValue*,
		       FeatureValueDimension::Unit) = 0;

// turn to Hard Copy engine specific literal
   virtual const char* convertToLiteral(const char* str) = 0;

public:
   featureProcessor(featureProcessor& x): f_name(x.f_name) {};
   featureProcessor(const char* name): f_name(name) {};
   virtual ~featureProcessor() {};

   virtual featureProcessor* clone() = 0;

   virtual void handleBegElement(const Element &element, const FeatureSet&, const FeatureSet&, const FeatureSet&, const FeatureSet& initialFeatureSet, ostream&) = 0;
   virtual void handleEndElement(ostream&) = 0;
   virtual void handleData(const char *data, unsigned int size, ostream&)=0;

   const char* name() { return f_name; };

// evaluate the variable (in x.y.z form) to a feature value.
   virtual FeatureValue* evaluate(const char* variableName) = 0;
   virtual unsigned int accept(const char* nm, const Expression*) = 0;

   virtual void preEvaluate(const Element&) = 0;
   virtual void postEvaluate(const Element&) = 0;

   virtual void clear() = 0;

////////////////////////////////////////
// functions for getting feature values.
//
// Sample usage:
//   int x;
//   if ( getFeatureValue(x, FeatureSet.deep_lookup(FONT, FONT_SIZE)) ) {
//     //use x here
//   }
////////////////////////////////////////

//
// default value (NONE) means using whatever unit is available
// Otherwise, a conversion is performed
//
   unsigned int    getDimensionValue(float& y, 
		   const Feature*, 
		   FeatureValueDimension::Unit = FeatureValueDimension::NONE);

   const char     *getDimensionValue(const Feature*, 
		   FeatureValueDimension::Unit = FeatureValueDimension::NONE);

   const char     *getFeatureValue(const Feature*);
   unsigned int    getFeatureValue(int&, const Feature*);
   unsigned int    getFeatureValue(float&, const Feature*);
   FeatureValueArray* getFeatureValueArray(const Feature*);

   const FeatureSet *getFeatureSetValue(const Feature*);
};

typedef featureProcessor* featureProcessorPtr;

class loutFeatureProcessor : public featureProcessor
{
protected:
   unsigned int f_change;

protected:
   char* empty_string();
   char* prepend(const char*, const char*);

// turn to Hard Copy engine specific literal
   const char* convertToLiteral(const char* str);
   
public:
   loutFeatureProcessor(loutFeatureProcessor& x) : 
    featureProcessor(x), f_change(x.f_change) {};

   loutFeatureProcessor(const char* name) : 
     featureProcessor(name), f_change(false) {};
   ~loutFeatureProcessor() {};

   virtual void handleEndElement(ostream& out) {
      if ( f_change == true )
         out << "}";
   };
   virtual void handleData(const char *data, unsigned int size, ostream&);

   const char* stringToCharPtr(const FeatureValue*);
   unsigned int dimensionToFloat(float&, 
		FeatureValueDimension::Unit& unitOfY,
		const FeatureValue*,
		FeatureValueDimension::Unit);
   const char* dimensionToCharPtr(const FeatureValue*,
		FeatureValueDimension::Unit);

   virtual FeatureValue* evaluate(const char* variableName) ;
   virtual unsigned int accept(const char* nm, const Expression*) ;

   virtual void preEvaluate(const Element&) ;
   virtual void postEvaluate(const Element&) ;

   virtual void clear() ;
};

#endif
