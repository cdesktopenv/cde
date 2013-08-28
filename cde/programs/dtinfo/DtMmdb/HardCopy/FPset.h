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
/* $XConsortium: FPset.h /main/3 1996/06/11 17:04:12 cde-hal $ */

#ifndef _FPset_h
#define _FPset_h

#include "StyleSheet/Renderer.h"
#include "StyleSheet/Const.h"
#include "StyleSheet/Element.h"

#include "HardCopy/FPExceptions.h"
#include "HardCopy/HardCopyFP.h"
#include "HardCopy/lout_limits.h"

class FPset
{
public:
/*
  FPset(const FPset& fps) : ptrs(fps.ptrs), count(fps.count), f_QuaddingString(0) {};
  FPset(featureProcessorPtr* p, int s) : ptrs(p), count(s), f_QuaddingString(0) {};
*/
  FPset(int s, const Element&, const FeatureSet&, 
	const FeatureSet&, const FeatureSet&);
  ~FPset();

  const Element& element() { return *f_element; };
  const FeatureSet& local() { return *f_local; };
  const FeatureSet& complete() { return *f_complete; };
  const FeatureSet& parent() { return *f_parent; };

  FPset operator=(const FPset&);
  unsigned int operator==(const FPset&);

  featureProcessorPtr* ptrs;
  int count;

  const char* f_QuaddingString;

  const Element* f_element;
  const FeatureSet* f_local;
  const FeatureSet* f_complete;
  const FeatureSet* f_parent;
};

#endif
