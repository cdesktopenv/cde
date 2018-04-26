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
// $XConsortium: testparser.C /main/4 1996/08/21 15:51:08 drk $

#include "PathTable.h"
#include "Debug.h"
#include "Const.h"
#include "StyleSheetExceptions.h"
#include "VariableTable.h"
#include "StyleSheet.h"
#include "Resolver.h"

Renderer* gRenderer = 0;
      
extern int styleparse();
extern int stylerestart(FILE*);
extern FILE *stylein;

test1()
{
   BitVector v1(33, 0);
   v1.setBitTo(0, 1);
   v1.setBitTo(1, 1);
   v1.setBitTo(5, 1);
   v1.setBitTo(32, 1);
   debug(cerr, v1);
   v1.shiftRightOneBit();

   BitVector v2(33, 0);
   v2.setBitTo(3, 1);
   debug(cerr, v2);
  
   MESSAGE(cerr, "v2 | v1");
   v2 |= v1;
   debug(cerr, v2);
  
   BitVector v3(33, 0);
   v3.setBitTo(8, 1);
   debug(cerr, v3);
   v3 &= v1;
   MESSAGE(cerr, "v3 & v1");
   debug(cerr, v3);
  
   BitVector v4(33, 0);
   v4.setBitTo(1, 1);
   v4.setBitTo(8, 1);
   debug(cerr, v4);
   v4 ^= v1;
   MESSAGE(cerr, "v4 ^ v1");
   debug(cerr, v4);
}


test2( char* argv[] )
{
   SSPath t(argv[1], false);

   SSPath p(argv[2], true);


   EncodedPath et(&t);
   EncodedPath ep(&p, true);

   debug(cerr, ep.match(et, 0, 0));
}

test3( int argc, char* argv[] )
{
/*
   PathTable pt;
   SSPath *px;

   for ( int i=1; i<argc-1; i++ ) {
       px = new SSPath(argv[i], (FeatureSet*)i);
       pt.addPathFeatureSet(px);
   }


   SSPath pq(argv[argc-2], false);

   debug(cerr, int(pt.getFeatureSet(pq)));
*/
}

test4( int argc, char* argv[] )
{
   if ( argc >= 2 ) {
      stylein = fopen(argv[1], "r");
      if ( stylein == 0 ) {
         MESSAGE(cerr, "open file failed");
         return 1;
      }
      styleparse();

      debug(cerr, *gVariableTable);

      debug(cerr, *gPathTab);

      fclose(stylein);
   } else
      MESSAGE(cerr, "no file argument");
}

struct XmappingTable_t {
   char* FeatureName;
   char* SubFeatureNameList;
   char  Value; // d: directly from the value()
                // i: indrectly from the value()
   char* Source; // l: local FeatureSet
                 // p: parent FeatureSet
                 // c: Combined FeatureSet
                 // can be a list of l, p, c
   char* LoutBeginTag;
};

main( int argc, char* argv[] )
{

   INIT_EXCEPTIONS();

   StyleSheet ss;

   if ( strcmp(argv[1], "test1") == 0 )
     test1;
   else
   if ( strcmp(argv[1], "test2") == 0 )
     test2(&argv[1]);
   else
   if ( strcmp(argv[1], "test3") == 0 )
     test3(argc-1, &argv[1]);
   else
   if ( strcmp(argv[1], "test4") == 0 )
     test4(argc-1, &argv[1]);
}

void styleerror( char* errorstr )
{
   MESSAGE(cerr, errorstr);
   return;
}

