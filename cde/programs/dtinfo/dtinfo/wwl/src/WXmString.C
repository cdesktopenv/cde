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
/*
 * $XConsortium: WXmString.C /main/5 1996/11/07 16:12:41 rcs $
 *
 * Copyright (c) 1992 HaL Computer Systems, Inc.  All rights reserved.
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

#include <Xm/Xm.h>
#include <WWL/WXmString.h>

//-----------------------------------------------------------------------------
// Create a copy of a WXmString, substituting the current font with the
// supplied font
WXmString
WXmString::CopyUsingFont( char* newtag )
{
   XmStringContext context;
   if( XmStringInitContext(&context,*this)){
      char* text = NULL;
      XmStringCharSet tag;
      XmStringDirection direction;
      Boolean separator;

      if( XmStringGetNextSegment(context,&text,&tag,&direction,&separator) ){
	 WXmString NewString(text, newtag);
	 delete text;
         XmStringFreeContext(context);
	 return NewString;
      }
      XmStringFreeContext(context);
   }
   WXmString NullString;
   return NullString;
}

extern "C" {
extern XtPointer _XmStringUngenerate (XmString string,
                        XmStringTag tag,
                        XmTextType tag_type,
                        XmTextType output_type);
}

Boolean
XmStringUngenerate(XmString string, XmStringCharSet tag, char** ret)
{

    *ret = (char *)_XmStringUngenerate(string, NULL, XmCHARSET_TEXT, XmCHARSET_TEXT);
    if (*ret) {
	return True;
    }
    else {
	return False;
    }
    
#if 0
  XmStringContext context;

  if (! XmStringInitContext(&context, string)) {
    *ret = NULL;
    return False;
  }

  int bufsize, buflen;
  char* buf = XtMalloc(bufsize = 128);
  buf[0] = 0;
  buflen  = 1;

  char* text = NULL;
  XmStringDirection dir;
  Boolean sep;

  while (XmStringGetNextSegment(context, &text, &tag, &dir, &sep)) {
    if (text) {
      len = strlen(text);
      int textlen = len + (sep? 1 : 0);
      { // buffer size adjustment
	int bufsize_required = bufsize;
	while (buflen + textlen > bufsize_required)
	  bufsize_required += 128;
	if (bufsize_required > bufsize) {
	  buf = XtRealloc(buf, bufsize_required);
	  bufsize = bufsize_required;
	}
      }
      slen = strlen(buf);
      *((char *) memcpy(buf + slen, text, len) + len) = '\0';
      if (sep)
	*((char *) memcpy(buf + slen + 1, "\n", 1) + 1) = '\0';
      buflen += textlen;

      XtFree(text);
    }
    text = NULL;
  }

  XmStringFreeContext(context);

  *ret = buf;

  return True;

#endif
}

