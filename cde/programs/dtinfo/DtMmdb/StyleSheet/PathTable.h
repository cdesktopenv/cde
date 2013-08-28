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
/* $XConsortium: PathTable.h /main/4 1996/08/21 15:50:37 drk $ */
#ifndef _PathTable_h
#define _PathTable_h

#ifndef CDE_NEXT


#else
#include "dti_cc/CC_Dlist.h"
#include "dti_cc/cc_hdict.h"
#endif

#include <sstream>
#include "Types.h"
#include "SymTab.h"
#include "SSPath.h"
#include "BitVector.h"

class Feature;
class FeatureSet;

#define OP_ONE "?"
#define OP_MANY "*"
typedef unsigned int LetterType;

class EncodedPath 
{
   int f_size;
   int f_patternSize;
   LetterType* f_array;

   hashTable<LetterType, BitVector> f_SVectors;

   LetterType f_wildCard;
   LetterType f_unlimitedWildCard;

   BitVector* f_copyOfS; // copy of S vector, used in match()

public:
   EncodedPath(SSPath* p, unsigned int asPattern = false);
   ~EncodedPath();

   int length() { return f_size; };
   int patternLength() { return f_patternSize; };

   unsigned int match(EncodedPath& p, SSPath* Pattern, SSPath* Elements);
}
;

class basePathFeature 
{

protected:
  SSPath* f_path;
   FeatureSet* f_featureSet;

public:

   basePathFeature(SSPath* p=0, FeatureSet* f=0): 
	f_path(p), f_featureSet(f) {};
   ~basePathFeature();

   unsigned int operator==(const basePathFeature&) const;

   SSPath* path() { return f_path; };
   FeatureSet* featureSet() { return f_featureSet; };

   void setPath(SSPath* p) { f_path = p; };
   void setFeatureSet(FeatureSet* fs) { f_featureSet = fs; };
};

class PathFeature : public basePathFeature
{
   unsigned int f_id;
   EncodedPath* f_encodedPath;

public:
   PathFeature(SSPath* p,FeatureSet* f,EncodedPath* e=0, unsigned int id=0): 
      basePathFeature(p, f), f_id(id), f_encodedPath(e) {};
   ~PathFeature();

   EncodedPath* encodedPath() { return f_encodedPath; };
   unsigned int id() { return f_id; };

   void setEncodedPath(EncodedPath* e) { f_encodedPath = e; };
   void setID(int x) { f_id = x; };

   unsigned int operator==(const PathFeature&) const;

   unsigned int match(SSPath& p);
};

class PathFeatureList : public CC_TPtrDlist<PathFeature> 
{

public:
   PathFeatureList() {};
   virtual ~PathFeatureList();

   void appendList(PathFeatureList&);
};

typedef CC_TPtrDlistIterator<PathFeature> PathFeatureListIterator;


// /////////////////////////////////////////////////////////////////////////
// 
// 	class PathTable
//
// /////////////////////////////////////////////////////////////////////////

typedef CC_TPtrDlist<PathFeature>* CC_TPtrDlist_PathFeature_Ptr_T;

class PathTable
{
public:
  PathTable();
  ~PathTable();

// add a path and associated raw feature set 
// this should only be called by the style sheet parser to build this internal
// table  
//
// Assume that the paths are passed in in left to right order. i.e., 
// in the order   
//       "TITLE SECTION" 
//       "TITLE CHAPTER" 
// with the example shown above.
//
  //void addPathFeatureSet(Path*, FeatureSet *rawFeatureSet);
  void addPathFeatureSet(PathFeature*);

// returns a NULL value if no feature set is available at this point
// this "new"s a new feature set, and the caller is responsible for
// deleting the object
  FeatureSet* getFeatureSet(SSPath&);

  friend ostream& operator<<(ostream&, PathTable&);

private:
  CC_TPtrDlist<PathFeature> f_pathFeatureList;
  CC_TPtrDlist_PathFeature_Ptr_T *f_lastSymIndex;
  unsigned int		    f_lastSymIndexCount ;

private:
  void initLastSymIndex();
  unsigned int findIndex(SSPath&);
  FeatureSet* getFeatureSet(int bucketIndex, SSPath&, int& pathId);
};

extern PathTable* gPathTab;

#endif /* _PathTable_h */
/* DO NOT ADD ANY LINES AFTER THIS #endif */
