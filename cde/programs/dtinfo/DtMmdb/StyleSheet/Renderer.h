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
/* $XConsortium: Renderer.h /main/3 1996/06/11 17:08:07 cde-hal $ */

#ifndef _Renderer_h
#define _Renderer_h 1

#include "Feature.h"
#include "Exceptions.hh"

/* **************************************************************
 * class Renderer
 
   the Renderer is responsible for taking the style sheet features
   matched with an ELEMENT and using them to guide the drawing of the
   ELEMENT data on the page
   ************************************************************** */

/* **************************************************************
   The symbol table supplied is common to the Node Parser,
   Style Sheet parser and the Renderer.
 * ************************************************************** */

//class FeatureSet;

class Renderer : public Destructable
{
public:
  Renderer() {};
  virtual ~Renderer() {};

  virtual FeatureSet *initialize() = 0;	/* return default feature set */

  /* -------- Begin Element -------- */
  /* ************************************************************
   * called when a new ELEMENT is found in the document
   * FeatureSet is set of features and values that match with the
   * ELEMENT as described in the style sheet 
   * ************************************************************ */

  /* returns a non-zero value if element is to be ignored */

  virtual unsigned int
    BeginElement(const Element       &element,
		 const FeatureSet    &featureset,
		 const FeatureSet    &complete,
		 const FeatureSet    &parentComplete) = 0;


  /* -------- Data -------- */
  /* ************************************************************
   * ELEMENT data passed in (usually text to be displayed)
   * embedded ELEMENT children are passed in with BeginElement...
   * ************************************************************ */

  virtual void
    data(const char *data, unsigned int size) = 0;


  /* -------- End Element -------- */
  /* ************************************************************
   * End of ELEMENT processing (eg. postfixes, reset pointers etc.)
   * ************************************************************ */

  virtual void
    EndElement(const Symbol &element_name) = 0 ;

  /* ************************************************************
   * Called BEFORE any data processing begins
   * ************************************************************ */

  virtual void Begin() = 0;

/* **************************************************************
 * called after all data has been processed
 * ************************************************************** */

  virtual void End() = 0;


  // These two functions allow a renderer to set up internal variable table.
 
/* **************************************************************
 * pass a variable and a Expression to the renderer and let it decide
 * whether to accept the variable and the Expression and put them into
 * the internal variable table.
 *
 * non zero return value means that it has been entered into the internal
 * table 
 *
 * If gRenderer is set to null while style sheet is parsed, the 
 * accept() function will not be called from style.y.
 * ************************************************************** */
  virtual unsigned int accept(const char*, const Expression*) { return 0 ;}

  /* ************************************************************
   *
   * evaluate the variable to a FeatureValue
   *
   * If gRenderer is set to null while the renderer is running, the 
   * evaluate() function will not be called. 
   * ************************************************************ */
  virtual FeatureValue* evaluate(const char*) { return 0 ; }

/* **************************************************************
 * notifies renderer that the local feature set will be evaluated.
 * ************************************************************** */
  virtual void preEvaluate(const Element &) {}

/* **************************************************************
 * notifies renderer that the local feature set has been evaluated.
 * ************************************************************** */
  virtual void postEvaluate(const Element &) {}


protected:
};

#endif

