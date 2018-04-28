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
/* $XConsortium: ScaleObj.h /main/3 1995/11/06 09:44:42 rswiston $ */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */

#ifndef SCALEOBJ_H
#define SCALEOBJ_H

#include "MotifUI.h"

class ScaleObj : public MotifUI {

 private:

   ScaleType _style;
   int _value;
   int _numDecimalPoints;
   int _max;
   int _min;
   int _scaleMultiple;
   boolean _showValue;
   Widget _scale;
   Widget _minLabel;
   Widget _maxLabel;

   void SetString(Widget w, int value);
   void CheckValues(boolean);
   void CreateScale(MotifUI *, char *, int, int, int, int, int, ScaleType,
	            boolean);

   boolean SetName(char *name);

 public:

   ScaleObj(MotifUI * parent,
	    char *title,
            int value, 
            int numDecimalPoints = 0, 
            int max = 100, 
            int min = 1, 
            int scaleMultiple = 5, 
            ScaleType style = VERTICAL_SCALE,
	    boolean showValue = false);
   ScaleObj(char *category,
            MotifUI * parent,
	    char *title,
            int value, 
            int numDecimalPoints = 0, 
            int max = 100, 
            int min = 1, 
            int scaleMultiple = 5, 
            ScaleType style = VERTICAL_SCALE,
	    boolean showValue = false);

   void Style(ScaleType);
   void Value(int);
   void NumDecimalPoints(int);
   void Maximum(int);
   void Minumum(int);
   void Multiple(int);
   void ShowValue(boolean);

   ScaleType Style()       { return _style; }
   int Value();
   int NumDecimalPoints()  { return _numDecimalPoints; }
   int Maximum()           { return _max; }
   int Minumum()           { return _min; }
   int Multiple()          { return _scaleMultiple; }
   boolean ShowValue()     { return _showValue; }

   const Widget InnerWidget()         { return _scale; }
   const UI_Class UIClass()           { return SCALE; }
   const int UISubClass()             { return _style; }
   const char *const UIClassName()    { return "Scale"; }

};

#endif /* SCALEOBJ_H */
