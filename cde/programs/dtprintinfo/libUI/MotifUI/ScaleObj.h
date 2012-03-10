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
