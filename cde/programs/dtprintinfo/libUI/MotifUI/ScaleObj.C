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
/* $XConsortium: ScaleObj.C /main/2 1995/07/17 14:07:10 drk $ */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */

#include "ScaleObj.h"

#include <Xm/Scale.h>
#include <Xm/Form.h>
#include <Xm/Label.h>

ScaleObj::ScaleObj(MotifUI *parent,
		   char *title,
		   int value,
		   int numDecimalPoints,
		   int max,
		   int min,
		   int scaleMultiple,
	           ScaleType style,
		   boolean showValue)
	: MotifUI(parent, title, NULL)
{
    CreateScale(parent, title, value, numDecimalPoints, max, min, scaleMultiple,
		style, showValue);
}

ScaleObj::ScaleObj(char *category,
	           MotifUI *parent,
		   char *title,
		   int value,
		   int numDecimalPoints,
		   int max,
		   int min,
		   int scaleMultiple,
	           ScaleType style,
		   boolean showValue)
	: MotifUI(parent, title, category)
{
    CreateScale(parent, title, value, numDecimalPoints, max, min, scaleMultiple,
		style, showValue);
}

void ScaleObj::CreateScale(MotifUI *parent, char *title, int value,
		           int numDecimalPoints, int max, int min,
		           int scaleMultiple, ScaleType style, boolean showValue)
{
   _style = style;
   _value = value;
   _numDecimalPoints = numDecimalPoints;
   _max = max;
   _min = min;
   _scaleMultiple = scaleMultiple;
   _style = style;
   _showValue = showValue;
   CheckValues(false);

   int orientation;
   short points = _numDecimalPoints; 
   if (_style == VERTICAL_SCALE)
      orientation = XmVERTICAL;
   else
      orientation = XmHORIZONTAL;

   XmString xm_string = StringCreate(title);
   _w = XtVaCreateManagedWidget(title, xmFormWidgetClass,
				parent->InnerWidget(), NULL);
   _minLabel = XtVaCreateWidget(title, xmLabelWidgetClass, _w,
				XmNtopAttachment, XmATTACH_FORM,
				XmNbottomAttachment, XmATTACH_FORM,
				XmNleftAttachment, XmATTACH_FORM, NULL);
   _maxLabel = XtVaCreateWidget(title, xmLabelWidgetClass, _w,
				XmNtopAttachment, XmATTACH_FORM,
				XmNbottomAttachment, XmATTACH_FORM,
				XmNrightAttachment, XmATTACH_FORM, NULL);
   _scale = XtVaCreateManagedWidget(title, xmScaleWidgetClass, _w,
				    XmNmaximum, _max,
                                    XmNminimum, _min, XmNvalue, _value,
				    XmNscaleMultiple, _scaleMultiple,
                                    XmNdecimalPoints, points,
				    XmNtitleString, xm_string,
				    XmNshowValue, _showValue,
				    XmNtopAttachment, XmATTACH_FORM,
				    XmNbottomAttachment, XmATTACH_FORM,
				    XmNorientation, orientation, NULL);
   StringFree(xm_string);
   SetString(_minLabel, _min);
   SetString(_maxLabel, _max);
   ShowValue(_showValue);
}

boolean ScaleObj::SetName(char *name)
{
   XmString xm_string = StringCreate(name);
   XtVaSetValues(_w, XmNtitleString, xm_string, NULL);
   StringFree(xm_string);
   return true;
}

void ScaleObj::CheckValues(boolean do_it)
{
   if (_min > _max)
    {
      int tmp = _min;
      _min = _max;
      _max = tmp;
    }
   if ((_max - _min) == 0)
      _max += 1;
   if (_numDecimalPoints < 0)
      _numDecimalPoints = 0;
   if (_value > _max)
      _value = _max;
   else if (_value < _min)
      _value = _min;
   if (_scaleMultiple < 0)
      _scaleMultiple = 1;
   else if (_scaleMultiple > (_max - _min))
    {
      if ((_scaleMultiple = (_max - _min) / 10) == 0)
         _scaleMultiple = 1;
    }
   if (do_it)
    {
      short points = _numDecimalPoints;
      XtVaSetValues(_scale, XmNmaximum, _max, XmNminimum, _min,
		    XmNscaleMultiple, _scaleMultiple,
                    XmNdecimalPoints, points, XmNvalue, _value, NULL);
    }
}

void ScaleObj::SetString(Widget w, int value)
{
   char number[20];
   if (_numDecimalPoints == 0)
      sprintf(number, "%d", value);
   else
    {
      char fmt[10];
      sprintf(fmt, "%%.%df", _numDecimalPoints);
      int n = 1, i;
      for (i = 0; i < _numDecimalPoints; i++)
	 n *= 10;
      sprintf(number, fmt, (float) value / n);
    }
   XmString xm_string = StringCreate(number);
   XtVaSetValues(w, XmNlabelString, xm_string, NULL);
   StringFree(xm_string);
}

void ScaleObj::Style(ScaleType value)
{
   int orientation;

   if (value == VERTICAL_SCALE)
      orientation = XmVERTICAL;
   else
      orientation = XmHORIZONTAL;
   XtVaSetValues(_scale, XmNorientation, orientation, NULL);
}

int ScaleObj::Value()
{
   XtVaGetValues(_scale, XmNvalue, &_value, NULL);
   return _value;
}

void ScaleObj::Value(int value)
{
   if (_value == value)
      return;
   _value = value;
   CheckValues(true);
}

void ScaleObj::NumDecimalPoints(int value)
{
   _numDecimalPoints = value;
   CheckValues(true);
}

void ScaleObj::Maximum(int value)
{
   if (_max == value)
      return;
   _max = value;
   int old_min = _min;
   CheckValues(true);
   SetString(_maxLabel, _max);
   if (old_min != _min)
      SetString(_minLabel, _min);
}

void ScaleObj::Minumum(int value)
{
   if (_min == value)
      return;
   _min = value;
   int old_max = _max;
   CheckValues(true);
   SetString(_minLabel, _min);
   if (old_max != _max)
      SetString(_maxLabel, _max);
}

void ScaleObj::Multiple(int value)
{
   _scaleMultiple = value;
   CheckValues(true);
}

void ScaleObj::ShowValue(boolean value)
{
   _showValue = value;
   if (_showValue)
    {
       XtManageChild(_minLabel);
       XtManageChild(_maxLabel);
       XtVaSetValues(_scale, XmNshowValue, _showValue, XmNleftWidget, _minLabel,
		     XmNleftAttachment, XmATTACH_WIDGET, 
		     XmNrightAttachment, XmATTACH_WIDGET, 
		     XmNrightWidget, _maxLabel, NULL);
    }
   else
    {
       XtUnmanageChild(_minLabel);
       XtUnmanageChild(_maxLabel);
       XtVaSetValues(_scale, XmNshowValue, _showValue,
		     XmNleftAttachment, XmATTACH_FORM, 
		     XmNrightAttachment, XmATTACH_FORM, NULL);
    }
}
