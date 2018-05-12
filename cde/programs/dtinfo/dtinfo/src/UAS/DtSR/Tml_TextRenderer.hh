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
// $XConsortium: Tml_TextRenderer.hh /main/7 1996/08/21 15:44:59 drk $
/*	Copyright (c) 1995,1996 FUJITSU LIMITED		*/
/*	All Rights Reserved				*/

#ifndef _TML_TEXTRUNCOUNTER_HH_
#define _TML_TEXTRUNCOUNTER_HH_

#include <StyleSheet/Attribute.h>
#include <StyleSheet/Element.h>
#include <StyleSheet/Renderer.h>

#include <sstream>
using namespace std;

#include "dti_cc/CC_Stack.h"

#include "UAS_SearchZones.hh"

#ifndef True
#define True	1
#endif
#ifndef False
#define False	0
#endif

#define ShiftIn		(char)0x0E
#define ShiftOut	(char)0x0F

class Tml_TextRenderer : public Renderer
{
  public:

    Tml_TextRenderer(ostringstream &, UAS_SearchZones &);
    ~Tml_TextRenderer();

    FeatureSet * initialize();

    unsigned int   BeginElement(const Element     &element,
				const FeatureSet  &local,
				const FeatureSet  &complete,
				const FeatureSet  &parentComplete);

    void	data(const char *data, unsigned int size);

    void	EndElement(const Symbol &name);

    void	Begin();
    void	End();

  private:

    int effective_zone(UAS_SearchZones::uas_zones);

    ostringstream	&f_ostr;
    UAS_SearchZones	&f_zones;

    int f_current_level;
    Stack<int> f_marked_level;

    Stack<char> f_effect;
    Stack<int>  f_ignore_stack;
};

#endif
