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
// $XConsortium: PathTable.cc /main/4 1996/06/11 17:07:57 cde-hal $
#include "PathTable.h"
#include "Debug.h"
#include "Feature.h"
#include "utility/debug.h"

extern SymbolTable* gElemSymTab;

unsigned int letterHash(const LetterType& x)
{
   return x;
}

EncodedPath::~EncodedPath()
{
  delete [] f_array ;
  f_SVectors.clearAndDestroy();
  delete f_copyOfS;
}

EncodedPath::EncodedPath(SSPath* p, unsigned int asPattern) :
   f_size(p -> entries()), f_patternSize(0), f_SVectors(letterHash),
   f_wildCard(gElemSymTab -> wildCardId()), 
   f_unlimitedWildCard(gElemSymTab -> unlimitedWildCardId()),
   f_copyOfS(new BitVector(WORD_SIZE, 0))
{

    f_array = new LetterType[f_size];

    CC_TPtrDlistIterator<PathTerm> l_pathIter(*p);

    BitVector *l_bv = 0;

    int i = 0;

    while (++l_pathIter) {

//
// Count pattern size, excluding "*". 
//
      f_array[i] = (LetterType)((l_pathIter.key() -> symbol()).id());
      if ( f_array[i] != f_unlimitedWildCard )
         f_patternSize++;
      i++;
    }

    if ( asPattern == true ) {

//
// Compute the S arrays. 
// Examples  pat = a b a c
//           S(a) = 1 0 1 0
//           S(b) = 0 0 0 0
//           S(c) = 0 0 0 1
//
// 	     pat = a ? a c
//           S(a) = 1 1 1 0
//           S(c) = 0 1 0 1
//           S(?) = 0 1 0 0
//
// 	     pat = a * a c
//           S(a) = 1 1 0
//           S(c) = 0 0 1
//           S(*) = 1 0 0
//
       l_pathIter.reset();
       int j = patternLength()-1;
    
       LetterType l_id;

// i records each PathTerm position in the list 
       int i=0;
   
       while (++l_pathIter) {
         l_id = (LetterType)((l_pathIter.key() -> symbol()).id());
   
         l_bv = f_SVectors.findValue(&l_id);
         if ( l_bv == 0 ) {
            l_bv = new BitVector(patternLength(), 0);
            f_SVectors.insertKeyAndValue(new LetterType(l_id), l_bv);
         }
   

///////////////////////////////////////////////////
// reverse the order in the bit vector:
//  MSB position ==> set LSB bit in the bit vector
///////////////////////////////////////////////////
         if ( l_id != f_unlimitedWildCard ) {
            l_bv -> setBitTo(j, 1);

//////////////////////////////////////////////////
// only set position when an expr is attatched 
// to the term
//////////////////////////////////////////////////
            if ( l_pathIter.key() -> pqexpr() && l_id != f_wildCard ) {
/*
MESSAGE(cerr, "==========");
debug(cerr, (void*)l_bv);
debug(cerr, *l_bv);
debug(cerr, i);
debug(cerr, j);
*/
               l_bv -> recordPositions(i, j);

            }

            j--;

         } else {
///////////////////////////////////////////////////
// do not set bits for unlimitedWildCards that 
// begin or end the pattern
///////////////////////////////////////////////////
            if ( j < patternLength() - 1 ) 
               l_bv -> setBitTo(j+1, 1);    
         }
         i++;
      }


///////////////////////////////////////////////////
// special treatment to the ? symbols:
// OR S(?) back to each S's.
///////////////////////////////////////////////////
       BitVector* l_BitVectorWildCard = f_SVectors.findValue(&f_wildCard);
       hashTableIterator<LetterType, BitVector> l_SVIter(f_SVectors);

       if ( l_BitVectorWildCard ) {
          while (++l_SVIter) {
             if ( *l_SVIter.key() != f_wildCard && 
                  *l_SVIter.key() != f_unlimitedWildCard 
                ) 
             {
                (*l_SVIter.value()) |= (*l_BitVectorWildCard);
             }
          }
       }
/*
#ifdef DEBUG
       l_SVIter.reset();
       while (++l_SVIter) {
          debug(cerr, (*l_SVIter.key()));
          debug(cerr, (*l_SVIter.value()));
       }
#endif
*/
    }
}



unsigned int
_match(LetterType* pattern, int n,
       LetterType* text, int m,
       LetterType wildCard,
       LetterType unlimitedWildCard
       )
{
// A naive (brute force) string matching algorithm that handles 
// wild card (?) and unlimited wild card (*) symbols.
   unsigned int findMisMatch = false;

   for ( int i=0; i<n; i++ ) {  // over the "text" string

      for ( int j=0; j<m; j++ ) {    // over the pattern
         if ( pattern[j] == wildCard ) {
            continue;
         } else
         if ( pattern[j] == unlimitedWildCard ) {

             for (;;) {

               j++;

               if ( j == m )
                 return true;

               if ( pattern[j] != unlimitedWildCard ) 
                 break;
             } 

             for ( int x=i+1; x<n; x++ ) {
                if ( text[x] == pattern[j] )
                   if (
                   _match(
                        &pattern[j], m-j,
                        &text[x], n-x,
                        wildCard, unlimitedWildCard
                         ) == true 
                      )
                      return true;
                
             }

         } else {
            if ( pattern[j] != text[i] ) {
               findMisMatch = true;
               break;
            }
         }
      }
      if ( findMisMatch == false )
         return true;  
   }
   return false;  
}


unsigned int
EncodedPath::match(EncodedPath& text, SSPath* patternPath, SSPath* elementPath)
{
////////////////////////////////////////////////
// text: the encoded string for the element path
// elementPath: the element path
//
// this: the encoded string for the pattern string
// patternPath: the pattern path
////////////////////////////////////////////////


////////////////////////////////////////////////
// the unlimited wildcard vector
////////////////////////////////////////////////
   BitVector* l_U = f_SVectors.findValue(&f_unlimitedWildCard); 

   if ( l_U && patternLength() == 0 )
     return true;

////////////////////////////////////////////////
// the wildcard vector
////////////////////////////////////////////////
   BitVector* l_W = f_SVectors.findValue(&f_wildCard); 

// the S vector of each Letter, including that for '?'
   BitVector* l_S = 0; 

// the accumulated result vector
   BitVector l_R(patternLength(), 0); 

// placeholder for '*''s contribution
   BitVector l_I(patternLength(), 0); 

// hole position record of each path term in the pattern path
   posRecord l_pr; 

// expr pointer of each path term in the pattern path
   PQExpr* expr = 0; 

   CC_TPtrDlistIterator<PathTerm>* elementPathNextPtr = 0;

   if ( elementPath )
      elementPathNextPtr = new CC_TPtrDlistIterator<PathTerm>(*elementPath);

// loop over text string
   for ( int i=0; i<text.length(); i++) {

      if ( elementPath )
          ++(*elementPathNextPtr);

	//MESSAGE(cerr, "===================");
	//debug(cerr, text.f_array[i]);

////////////////////////////////////////////////
// get this character's vector, including '?'s
////////////////////////////////////////////////
      l_S = f_SVectors.findValue(&text.f_array[i]);
   
      if ( l_S ) {
	//debug(cerr, (void*)l_S);
	//debug(cerr, *l_S);
	//MESSAGE(cerr, "checking qualifies");
//////////////////////
// check qualifies.
//////////////////////
         if ( patternPath && l_S -> positionArray() ) {

////////////////////
// get a copy of S  
////////////////////
            f_copyOfS -> setTo(*l_S);

	    //debug(cerr, *patternPath);

            positionArrayIteratorT l_positionNext(*(l_S -> positionArray()));
            while (++ l_positionNext) {
                
               l_pr = l_positionNext.key();

//cerr <<  " calling patternPath -> fastGetAt():  " << (void*)patternPath << "l_pr.pathTermPos= " << l_pr.pathTermPos << endl;

               expr = patternPath -> fastGetAt(l_pr.pathTermPos) -> pqexpr();
/*
debug(cerr, int(expr));
if ( expr ) {
   MESSAGE(cerr, "qualify checking is needed.");
   debug(cerr, *(elementPathNext -> key())); 
}
*/

               if ( expr && 
                    expr->evaluate(elementPathNextPtr->key()->element()) == PQFalse ) 
               {
		  //MESSAGE(cerr, form("set position %d to 0", l_pr.bitPos));
                  f_copyOfS -> setBitTo(l_pr.bitPos, 0);
               }
            }
/////////////////////////////////////////
// make l_S point at its modified copy
/////////////////////////////////////////
            l_S = f_copyOfS;
         }
      } else
         l_S = l_W;

////////////////////////////////////////////////
// get unlimited wildcard's vector.
////////////////////////////////////////////////
      if ( l_U ) {
         l_I.setTo(l_R);
         l_I &= (*l_U);
	 //MESSAGE(cerr, "l_I");
	 //debug(cerr, l_I);

      }

////////////////////////////////////////////////
// shift the partial result vector right once
////////////////////////////////////////////////
      l_R.shiftRightOneBit();
      //MESSAGE(cerr, "after l_R >> 1");
      //debug(cerr, l_R);

////////////////////////////////////////////////
// AND in this character's vector
////////////////////////////////////////////////
      if ( l_S ) {
         l_R &= (*l_S);
         //debug(cerr, *l_S);
         //MESSAGE(cerr, "after AND with l_S");
         //debug(cerr, l_R);
      } else {
///////////////////////////////////////////////
// this branch is impossible to reach.
///////////////////////////////////////////////
	 //MESSAGE(cerr, "l_R set all bits to 0");
         l_R.setAllBitsTo(0);
      }


///////////////////////////////////////////////
// OR in unlimited wild char's vector.
///////////////////////////////////////////////
      if ( l_U ) {
         l_R |= l_I;
	 //MESSAGE(cerr, "after OR with l_I");
	 //debug(cerr, l_R);
      }
	 //debug(cerr, l_R);
	 //MESSAGE(cerr, "===================");
      
///////////////////////////////////////////////
// Use this test to get the first matched position
//  if ( l_R.getBit(0) == 1 )
//    return true;
///////////////////////////////////////////////

   }
   delete elementPathNextPtr;

///////////////////////////////////////////////
//
// the last symbol must match
//
///////////////////////////////////////////////
   if ( l_R.getBit(0) == 1 )
      return true;
   else
      return false;

}

basePathFeature::~basePathFeature()
{
   delete f_path;
   delete f_featureSet;
}

PathFeature::~PathFeature()
{
  delete f_encodedPath ;
}

unsigned int basePathFeature::operator==(const basePathFeature& pf) const
{
   cerr << "Warning: basePathFeature::operator==() called\n";
   return true;
}

unsigned int PathFeature::operator==(const PathFeature& pf) const
{
   cerr << "Warning: PathFeature::operator==() called\n";
   return true;
}

void pathSelector(BitVector& bv)
{
}

unsigned int
PathFeature::match(SSPath& p)
{
   EncodedPath l_ep(&p);

  if ( f_path -> containSelector() == false )
    return f_encodedPath -> match(l_ep, 0, 0);
  else 
    return f_encodedPath -> match(l_ep, f_path, &p);
}


// /////////////////////////////////////////////////////////////////////////
// 
// 	class PathTable
// 
// /////////////////////////////////////////////////////////////////////////


unsigned symHashFunc(const Symbol& s)
{
   return s.hash();
}

PathTable::PathTable()
: f_lastSymIndex(0),
  f_lastSymIndexCount(0)
{
}

PathTable::~PathTable()
{
  f_pathFeatureList.clearAndDestroy();
  for (unsigned int i = 0 ; i < f_lastSymIndexCount; i++) {
    //f_lastSymIndex[i].clearAndDestroy();
    f_lastSymIndex[i] -> clear();
    delete f_lastSymIndex[i];
   }
   delete f_lastSymIndex;
}

LetterType PathTable::findIndex(SSPath& p)
{
  return (LetterType) ((p.last() -> symbol()).id());
}

void PathTable::initLastSymIndex()
{
   f_lastSymIndexCount = gElemSymTab -> IdsAssigned()+1;
   f_lastSymIndex = new CC_TPtrDlist_PathFeature_Ptr_T[f_lastSymIndexCount];
   for (unsigned int i=0; i<f_lastSymIndexCount; i++)
      f_lastSymIndex[i] = new CC_TPtrDlist<PathFeature>;
      

   CC_TPtrDlistIterator<PathFeature> l_pfIter(f_pathFeatureList);
   PathFeature* l_pathFeature = 0;

   LetterType x;

   while ( ++l_pfIter ) {
      l_pathFeature = l_pfIter.key();
      x = findIndex( *(l_pathFeature->path()) );
      //f_lastSymIndex[x] -> prepend(l_pathFeature); // backward order
      f_lastSymIndex[x] -> append(l_pathFeature); // select the matching rule
						  // in the same order rules
						  // appear in the 
						  // stylesheet
   }
}

FeatureSet* PathTable::getFeatureSet(SSPath& p)
{
   if ( f_lastSymIndex == 0 ) {
     initLastSymIndex();
   }

   int pids[3];
   FeatureSet* fs[3];

   fs[0] = getFeatureSet(findIndex(p), p, pids[0]);
   fs[1] = getFeatureSet(gElemSymTab -> wildCardId(), p, pids[1]);
   fs[2] = getFeatureSet(gElemSymTab -> unlimitedWildCardId(), p, pids[2]);

   int index = 0;
   int x = pids[0];

   for ( int i=1; i<3; i++ ) {
     if ( pids[i] > x )
       index = i;
   }

   return fs[index];
}

FeatureSet* 
PathTable::getFeatureSet(int bucketIndex, SSPath& p, int& pathId)
{
   CC_TPtrDlistIterator<PathFeature> l_pathFeatureIter(*f_lastSymIndex[bucketIndex]);
 
   l_pathFeatureIter.reset();

   PathFeature* l_pathFeature = 0;

   while ( ++l_pathFeatureIter ) {
      
      l_pathFeature = l_pathFeatureIter.key();

//debug(cerr, *(l_pathFeature->path()));
//debug(cerr, *(l_pathFeature->featureSet()));

      if ( l_pathFeature -> match(p) ) {
//MESSAGE(cerr, "match");
         pathId = l_pathFeature -> id();
         return l_pathFeature -> featureSet();
      }
   }

   pathId = 0;
   return 0;
}

void PathTable::addPathFeatureSet(PathFeature* x)
{
//MESSAGE(cerr, "addPathFeatureSet():");
//debug(cerr, *(x->path()));
//debug(cerr, *(x->featureSet()));

   if ( x -> path() -> containSelector() )
      x -> path() -> fastGetIndex();

   EncodedPath* l_epath = new EncodedPath(x -> path(), true);
   unsigned int id = f_pathFeatureList.entries()+1;

   x -> setEncodedPath(l_epath);
   x -> setID(id);

   f_pathFeatureList.insert(x);

}

ostream& operator<<(ostream& out, PathTable& pt)
{
   CC_TPtrDlistIterator<PathFeature> l_pfIter(pt.f_pathFeatureList);
   PathFeature* l_pathFeature = 0;

   while ( ++l_pfIter ) {
      l_pathFeature = l_pfIter.key();
    
      out << *(l_pathFeature)->path() << ' ' << *(l_pathFeature->featureSet())
	  << endl;

#ifdef DEBUG
      // this will cause errors if gTopOfStack is not set properly 
      FeatureSet *set = l_pathFeature->featureSet()->evaluate();
      out << *set << endl;
      delete set ;
#endif
   }

   
   return out;
}

void PathFeatureList::appendList(PathFeatureList& list)
{
   PathFeatureListIterator l_Iter(list);
   while ( ++ l_Iter ) {
      append(l_Iter.key());
   }
}

PathFeatureList::~PathFeatureList()
{
  clearAndDestroy();
}
