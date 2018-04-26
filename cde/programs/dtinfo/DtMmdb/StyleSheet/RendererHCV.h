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
/* $XConsortium: RendererHCV.h /main/3 1996/06/11 17:08:17 cde-hal $ */

#ifndef _RendererHCV_h
#define _RendererHCV_h 1

#include "Renderer.h"
#include "HardCopy/autoNumberFP.h"

//   RendererHCV class is a simplified hardcopy renderer responsible 
//   for evaluating feature values specific to hardcopy engine. The
//   feature values are used by the validator.

class RendererHCV : public Renderer 
{
public:
  RendererHCV()  {};
  ~RendererHCV() ;

  FeatureSet *initialize() { return 0; };

  unsigned int
    BeginElement(const Element       &element,
		 const FeatureSet    &featureset,
		 const FeatureSet    &complete,
		 const FeatureSet    &parentComplete) { return 0; };


  void data(const char *data, unsigned int size) {} ;
  void EndElement(const Symbol &element_name) {};
  void Begin() {};
  void End() {};

// These two functions allow a renderer to set up internal variable table.
  unsigned int accept(const char*, const Expression*) ;
  FeatureValue* evaluate(const char*) ;
  void preEvaluate(const Element &) {};
  void postEvaluate(const Element &) {}; 

protected:
  autoNumberFP f_autoNumberFP;
};

#endif

