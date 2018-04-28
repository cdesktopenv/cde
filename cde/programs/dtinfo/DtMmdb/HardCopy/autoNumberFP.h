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
/* $XConsortium: autoNumberFP.h /main/7 1996/10/08 19:25:47 cde-hal $ */

#ifndef _autoNumber_fp_h
#define _autoNumber_fp_h 1

#ifndef CDE_NEXT

#else
#include "dti_cc/CC_String.h"
#include "dti_cc/cc_hdict.h"
#endif

#include "StyleSheet/SymTab.h"
#include "StyleSheet/Element.h"
#include "StyleSheet/FeatureValue.h"
#include "HardCopy/autoNumber.h"
   
#ifndef CDE_NEXT
typedef hashTable<CC_String, autoNumber> f_autoNumberSet_t ;
typedef hashTable<CC_String, autoNumberListT> f_resetControlList_t;
typedef hashTable<CC_String, autoNumberListT> f_registerList_t;
#else
typedef hashTable<CC_String, autoNumber> f_autoNumberSet_t ;
typedef hashTable<CC_String, autoNumberListT> f_resetControlList_t;
typedef hashTable<CC_String, autoNumberListT> f_registerList_t;
#endif

class autoNumberFP 
{
private:
// autoNumber FPs that are known through their names

   f_autoNumberSet_t f_autoNumberSet;

// Inverted reset control list.
// A reset control list records a list of tags that 
// once are met, the autonumber the list is associated with
// will be reset to its initial value.

// Each <String, autoNumberList> pair in the dictionary has the
// meaning that when the tag with String GI is encounted in the doucument,
// all autonumber in the autoNumberList will reset.

   f_resetControlList_t f_resetControlList;

// Each <String, autoNumberList> pair in the dictionary has the
// meaning that all autonumber in the autoNumberList are defined
// in the style sheet under tag with String GI.

   f_registerList_t f_registerList;

private:
   void pushAutoNumbers(const char *);
   void popAutoNumbers (const char *);
   void updateAutoNumbers(const char*);
	
   void defineAutoNumber(const char*, const FeatureValue*);

   void setSeenTagStatus(const char* tagName);

   const char* stringToCharPtr(const FeatureValue* f);

public:
   autoNumberFP();
   ~autoNumberFP();

   FeatureValue* evaluate(const char* varName) ;
   unsigned int accept(const char*, const Expression*);

   void beginElement(const Element &);
   void endElement  (const Symbol  &);

   void clear();

   void resetAllAutoNumbers();
};

extern autoNumberFP gAutoNumberFP;

#endif
