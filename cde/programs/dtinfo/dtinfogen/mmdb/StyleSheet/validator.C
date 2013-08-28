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
// $XConsortium: validator.C /main/6 1996/08/21 15:51:12 drk $

#include "StyleSheet/Debug.h"
#include "StyleSheet/Element.h"
#include "StyleSheet/PathTable.h"
#include "StyleSheet/Resolver.h"
#include "StyleSheet/StyleSheet.h"
#include "StyleSheet/StyleSheetExceptions.h"
#include "StyleSheet/VariableTable.h"
#include "StyleSheet/Feature.h"
#include "StyleSheet/FeatureDefDictionary.h"
#include "StyleSheet/RendererHCV.h"
#include "HardCopy/autoNumberFP.h"
#include <iostream>
using namespace std;
#include <stdarg.h>

     
extern featureDefDictionary* g_FeatureDefDictionary;
Renderer* gRenderer = 0;
fstream* defStream = 0;

extern unsigned g_validation_mode;
extern unsigned g_hasSemanticError;

extern void report_error_location();

/*
void styleerror(char *errorstr)
{
  cerr << errorstr ;
}
*/

extern FILE *stylein;
extern int styleparse();
extern void stylerestart(FILE *);
extern istream* g_stylein;

int stylewrap()
{
   return 0;
}

void report_error()
{
   MESSAGE(cerr, "\nStyle sheet has error.\n");
}

void quit(int code)
{
   delete g_FeatureDefDictionary;
   delete gRenderer;
   delete defStream;
   exit(code);
}

int main(int argc, char** argv )
{
  INIT_EXCEPTIONS();

  StyleSheet ss ;

  g_validation_mode = true;

  if ( argc != 4 && argc != 3 ) {
        cerr << form("usage: %s online|hardcopy styleSheetFile [FeatureDefinitionFile]\n", argv[0]);
        quit(1);
  }

  mtry {

     if ( strcasecmp(argv[1], "hardcopy") == 0 )
        gRenderer = new RendererHCV();
     else
      if ( strcasecmp(argv[1], "online") == 0 )
        gRenderer = 0;
     else {
        cerr << form("bad validation option: %s\n", argv[1]);
        quit(1);
     }



     if ( argc == 4 ) {
        defStream = new fstream(argv[3], ios::in);
        if ( !( *defStream ) ) {
           cerr << form("bad feature definition file name: %s\n", argv[3]);
           quit(1);
        }
     } else {
        char* path = getenv("DTINFO_HOME");
        if ( path == 0 ) {
           cerr << "DTINFO_HOME is undefined.\n";
           quit(1);
        }

        char* spec_file_path = form("%s/infolib/etc/%s.feature.spec", path, argv[1]);

        defStream = new fstream(spec_file_path, ios::in);

        if ( !( *defStream ) ) {
           cerr << form("bad feature definition file name: %s", spec_file_path);
           quit(1);
        }
         
     }

     defStream -> unsetf(ios::skipws);

     g_FeatureDefDictionary = new featureDefDictionary();
     *defStream >> *g_FeatureDefDictionary;
//cerr << *g_FeatureDefDictionary;
  }
  mcatch_any()
  {
      cerr << "\nfeature definition has error.\n";
      quit(1);
  }
  end_try;

  mtry {

     fstream* styleStream = new fstream(argv[2], ios::in);
     if ( !(*styleStream) ) {
        cerr << form("bad stylesheet file name: %s\n", argv[2]);
        quit(1);
     }


     styleStream -> unsetf(ios::skipws);
     g_stylein = styleStream;


     int ok = styleparse();
     
     delete styleStream;

     if ( ok != 0 || g_hasSemanticError == true ) {
        report_error_location(); 
        quit(1);
      }

//debug(cerr, *gPathTab);

  }

  mcatch_any()
  {
      report_error_location(); 
      quit(1);
  }
  end_try;

  MESSAGE(cerr, "\nThe style sheet is ok.");
  quit(0);
}
