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
/*
 * $XConsortium: WXmString.h /main/4 1996/09/05 17:39:41 cde-hal $
 *
 * Copyright (c) 1991 HaL Computer Systems, Inc.  All rights reserved.
 * UNPUBLISHED -- rights reserved under the Copyright Laws of the United
 * States.  Use of a copyright notice is precautionary only and does not
 * imply publication or disclosure.
 * 
 * This software contains confidential information and trade secrets of HaL
 * Computer Systems, Inc.  Use, disclosure, or reproduction is prohibited
 * without the prior express written permission of HaL Computer Systems, Inc.
 * 
 *                         RESTRICTED RIGHTS LEGEND
 * Use, duplication, or disclosure by the Government is subject to
 * restrictions as set forth in subparagraph (c)(l)(ii) of the Rights in
 * Technical Data and Computer Software clause at DFARS 252.227-7013.
 *                        HaL Computer Systems, Inc.
 *                  1315 Dell Avenue, Campbell, CA  95008
 * 
 */

#ifndef WXmString_h
#define WXmString_h

extern Boolean XmStringUngenerate(XmString, XmStringCharSet, char**);

class WXmString {

protected:

   XmString	string;

public:

   enum { NO_COPY = False, COPY = True };

// Constructors

   inline	WXmString ()		{ string = 0; }
   inline	WXmString (XmString s,
			   Boolean copy = True)
     { if (copy)
	 string = XmStringCopy(s);
       else
	 string = s;
     }
   inline	WXmString (const WXmString& w)	{ string = w.Copy(); }

#if XmVersion >= 1002
   inline	WXmString (const char* t, char *tag=XmFONTLIST_DEFAULT_TAG)
   {
      if (tag == XmFONTLIST_DEFAULT_TAG)
	string = XmStringCreateLocalized((char *) t);
      else
	string = XmStringCreate ((char *) t, tag);
   }
#else
   inline	WXmString (char* t,
			   XmStringCharSet charset=XmSTRING_DEFAULT_CHARSET) {
      string = XmStringCreateLtoR (t, charset);
   }
#endif
   inline	WXmString (XmStringDirection d)	{
      string = XmStringDirectionCreate (d);
   }

// Destructor

   inline	~WXmString () {
      if (string) XmStringFree (string);
   }

// Operators

   inline WXmString&	operator= (const WXmString& w) {
      if (string) XmStringFree (string);
      string = XmStringCopy(w.string);
      return *this;
   }
   inline WXmString&	operator= (const XmString s) {
      if (string) XmStringFree (string);
      string = XmStringCopy(s);
      return *this;
   }
   inline WXmString&	operator= (char *cs) {
      if (string) XmStringFree (string);
#if XmVersion >= 1002
      string = XmStringCreateLocalized (cs);
#else
      string = XmStringCreateLtoR (cs, XmSTRING_DEFAULT_CHARSET);
#endif
      return *this;
   }

   inline		operator XmString () const { return string; }
   inline		operator XtArgVal () const { return (XtArgVal) string; }
   inline		operator char* () const	{
      char* ret;
#if XmVersion >= 1002
      XmStringUngenerate(string, XmFONTLIST_DEFAULT_TAG, &ret);
#else
      XmStringGetLtoR (string, XmSTRING_DEFAULT_CHARSET, &ret);
#endif
      return ret;
   }
   inline Boolean	operator== (XmString s) const {
      return ByteCompare (s);
   }
   inline WXmString&	operator<< (const WXmString& s)	{
      XmString n = Concat(s.string);
      XmStringFree(string);
      string = n;
      return *this;
   }
   // Beware that this operator modifies the first string!!
   inline WXmString&	operator + (const WXmString& s)	{
      XmString n = Concat(s.string);
      XmStringFree(string);
      string = n;
      return *this;
   }

// Methods

   inline void		Free ()	{
      if (string) XmStringFree (string);
      string = 0;
   }
   inline Dimension	Baseline (XmFontList fl) const {
      return XmStringBaseline (fl, string);
   }
   inline Boolean	ByteCompare (XmString s) const {
      return XmStringByteCompare (string, s);
   }
   inline Boolean	Compare (XmString s) const {
      return XmStringCompare (string, s);
   }
   // Caller responsible for deleting return value!!
   inline XmString	Concat (XmString s) const {
      return XmStringConcat (string, s);
   }
   inline XmString	Copy () const { return XmStringCopy (string); }
   inline Boolean	Empty () const { return XmStringEmpty (string); }
   inline void		Extent (XmFontList fl, Dimension* w, Dimension* h)
   const {
      XmStringExtent (fl, string, w, h);
   }
#if XmVersion >= 1002
   inline Boolean	GetLtoR (char** text, char *tag=XmFONTLIST_DEFAULT_TAG)
   const {
      return XmStringUngenerate(string, tag, text);
   }
#else
   inline Boolean	GetLtoR (char** text,
				 XmStringCharSet cs=XmSTRING_DEFAULT_CHARSET)
   const {
      return XmStringGetLtoR (string, cs, text);
   }
#endif
   inline Dimension	Height (XmFontList fl) const {
      return XmStringHeight (fl, string);
   }
   inline int		LineCount () const {
      return XmStringLineCount (string);
   }
   inline void		NConcat (XmString s, int n) {
      XmStringNConcat (string, s, n);
   }
   inline Dimension	Width (XmFontList fl) const {
      return XmStringWidth (fl, string);
   }
   inline void		Zero ()	{ string = 0; }
   inline XmString disown() { XmString s = string; string = NULL; return s; }

   WXmString  CopyUsingFont( char* newtag );

};

#endif
