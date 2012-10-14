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
// $XConsortium: docparser.C /main/4 1996/08/21 15:51:05 drk $
#include "Debug.h"
#include "DocParser.h"
#include "Element.h"
#include "PathTable.h"
#include "Renderer.h"
#include "Resolver.h"
#include "StyleSheet.h"
#include "StyleSheetExceptions.h"
#include "VariableTable.h"
#include <iostream>
using namespace std;

Renderer *gRenderer = 0;
class TestRenderer : public Renderer
{
public:

  // inherited virtuals

  void Begin()	{} ;
  void End()	{} ;

  FeatureSet *initialize();
  unsigned int BeginElement(const Element       &element,
			    const FeatureSet    &featureset,
			    const FeatureSet    &complete,
			    const FeatureSet    &parentComplete);

  void data(const char *data, unsigned int size);

  void EndElement(const Symbol &element_name);
};

FeatureSet *
TestRenderer::initialize()
{
  return new FeatureSet;
}
unsigned int
TestRenderer::BeginElement(const Element &element,
			   const FeatureSet    &localset,
			   const FeatureSet    &complete,
			   const FeatureSet    &/* parentComplete */)
{
  ON_DEBUG(cerr << "TestRenderer::BeginElement()" << endl);

  ON_DEBUG(cerr << localset<< endl);
  ON_DEBUG(cerr << complete << endl);

  if (localset.lookup(gSymTab->intern("ignore")))
    return 1 ; // ignore 

  cout << element << endl;

  return 0 ; // do not ignore 
}
void
TestRenderer::data(const char * data, unsigned int /* size */)
{
  ON_DEBUG(cerr << "TestRenderer::data()" << endl);
  cout << data ;
}
void
TestRenderer::EndElement(const Symbol &name)
{
  ON_DEBUG(cerr << "TestRenderer::EndElement(" << name << ')' << endl);
  cout << "</" << name << '>';
}
void
styleerror(char *errorstr)
{
  cerr << errorstr ;
}

// extern FILE *stylein;
extern int styleparse();

extern PathTable       *gPathTab;
extern VariableTable   *gVariableTable ;

extern istream *g_stylein;

main(int argc, char **argv)
{
  INIT_EXCEPTIONS();

  StyleSheet ss ;

  ifstream stylestream(argv[1]);
  g_stylein = &stylestream;
  g_stylein->unsetf(ios::skipws);
  styleparse();

  try
    {
      TestRenderer	renderer ;
      Resolver resolver(*gPathTab, renderer);
      DocParser docparser(resolver);
      docparser.parse(cin);
    }
  catch_any()
    {
      cerr << "docparser.C: exception thrown" << endl;
      rethrow;
    }
  end_try;

  cout << endl;
  
  exit (0);
}
