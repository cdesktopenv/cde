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
// $XConsortium: Tml_TextRenderer.C /main/7 1996/09/10 16:03:56 cde-hal $
/*	Copyright (c) 1995,1996 FUJITSU LIMITED		*/
/*	All Rights Reserved				*/

#include <assert.h>

#include "Tml_TextRenderer.hh"
#include "DtSR_SearchZones.hh"

#include "StyleSheet/FeatureValue.h"

Tml_TextRenderer::Tml_TextRenderer(ostringstream &ostr, UAS_SearchZones &zones) :
	f_ostr(ostr), f_zones(zones)
{
}

Tml_TextRenderer::~Tml_TextRenderer()
{
}

FeatureSet *
Tml_TextRenderer::initialize()
{
    // NOTE: You don't have to keep track of default_features.
    //       Resolver will take care of the rest (i.e. delete).
    FeatureSet* default_features = new FeatureSet;

    default_features->add(new Feature(gSymTab->intern("IGNORE"),
					new FeatureValueInt(0)));

    return default_features;
}

void
Tml_TextRenderer::Begin()
{
    f_current_level = 0;

    for (; f_marked_level.entries(); f_marked_level.pop());
    f_marked_level.push(f_current_level);

    for (; f_effect.entries(); f_effect.pop());

    char base_effect;
    if (effective_zone(DtSR_SearchZones::uas_bodies))
	base_effect = ShiftIn;
    else
	base_effect = ShiftOut;

    f_effect.push(base_effect);
    f_ostr << f_effect.top();

    f_ignore_stack.push(0);
}

void
Tml_TextRenderer::End()
{
    f_ostr << flush;

    if (f_ignore_stack.entries())
	f_ignore_stack.pop();
}

int
Tml_TextRenderer::effective_zone(UAS_SearchZones::uas_zones zone)
{
    switch (zone) {
	case UAS_SearchZones::uas_titles :
	    return f_zones.titles();
	    break;
	case UAS_SearchZones::uas_bodies :
	    return f_zones.bodies();
	    break;
	case UAS_SearchZones::uas_examples :
	    return f_zones.examples();
	    break;
	case UAS_SearchZones::uas_indexes :
	    return f_zones.indexes();
	    break;
	case UAS_SearchZones::uas_tables :
	    return f_zones.tables();
	    break;
	case UAS_SearchZones::uas_graphics :
	    return f_zones.graphics();
	    break;
	default:
	    fprintf(stderr, "(ERROR) unknown zone found\n");
	    break;
    }

    return False;
}

unsigned int
Tml_TextRenderer::BeginElement(const Element	&element,
		  const FeatureSet&, const FeatureSet& complete,
		  const FeatureSet&)
{
    f_current_level++;

    const Attribute *scope_attr =
	element.get_attribute(gSymTab->intern("OLIAS.SCOPE"));

    if (scope_attr) { // scope attribute specified

	f_marked_level.push(f_current_level);

#ifdef DEBUG
	fprintf(stderr, "(DEBUG) %s specifies OLIAS.SCOPE=\"%s\"\n",
			element.gi().name(), scope_attr->value());
#endif
        char effect;
	if (effective_zone(DtSR_SearchZones::
					zonename2zone(scope_attr->value()))) {
	    effect = ShiftIn;
 	}
	else {
	    effect = ShiftOut;
	}

	f_effect.push(effect);
	f_ostr << f_effect.top();
    }

    const Attribute *terms_attr =
	element.get_olias_attribute(gSymTab->intern("TERMS"));

    CC_TPtrSlistIterator<Feature> iter(*(FeatureSet*)&complete);
    while (++iter) {
	const Symbol &feature = iter.key()->name();
	if (feature == gSymTab->intern("IGNORE")) {
	    Feature *ignore = iter.key();
	    f_ignore_stack.push((int)*(ignore->value()));
	}
    }

    if (terms_attr) {
	const char* terms = terms_attr->value();
	f_ostr << terms << '\n';
	return True;
    }

    return False;
}

void
Tml_TextRenderer::EndElement(const Symbol &)
{
    f_ostr << '\n';

    if (f_current_level == f_marked_level.top()) {

	f_effect.pop();
	f_ostr << f_effect.top();

	f_marked_level.pop();
    }

    f_current_level--;

    if (f_ignore_stack.entries())
	f_ignore_stack.pop();
}

void
Tml_TextRenderer::data(const char *data, unsigned int)
{
    if (! f_ignore_stack.top())
	f_ostr << data;
}

