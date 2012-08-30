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
/* $XConsortium: cmnutils.c /main/4 1995/11/01 16:11:47 rswiston $ */
/***************************************************************************/
/*                                                                         */
/*  Utility Functions                                                      */
/*                                                                         */
/***************************************************************************/

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <Xm/Xm.h>
#include <Xm/RowColumnP.h>
#include <Xm/MessageB.h>
#include <Xm/Text.h>
#include "cmnutils.h"


/********************************************************************************/
/* countItems - counts the number of items in a null terminated array           */
/* INPUT: char **items - null terminated array                                  */
/* OUTPUT: int lcv - number of items in array                                   */
/********************************************************************************/
int countItems (char **items)
{
  int lcv = 0;
  /*
  while (items[lcv]) {
     lcv++;
  }
  */
  if (items) {
     for (lcv = 0; items[lcv]; lcv++);
  }
  return (lcv);
}

/********************************************************************************/
/* TextStringsToXmStrings - Given an array of C text strings returns an         */
/*                          array of XmStrings.                                 */
/* INPUT:  char **text_strings - array of C style strings                       */
/* OUTPUT: XmStringTable  xmstrings - an array Motif compound strings           */
/********************************************************************************/
XmStringTable TextStringsToXmStrings (char **text_strings)
{
XmStringTable xmstrings = NULL;
int count, lcv;

if (text_strings) {
    count = countItems (text_strings);
    xmstrings = (XmStringTable) calloc (sizeof(XmString), (count));
    for (lcv = 0; lcv < count; lcv++)
        xmstrings[lcv] = (XmString) XmStringCreateSimple (text_strings[lcv]);
}
return ((XmStringTable)xmstrings);
}

/********************************************************************************/
/* XmStringToText - Given an XmString returns a C character text string.        */
/* INPUT:  XmString  xmstring - a Motif compound string                         */
/*  OUTPUT: char *text_string - C style string                                  */
/********************************************************************************/
char *XmStringToText (XmString xmstring)
{
XmStringContext   context;
XmStringCharSet   charset;
XmStringDirection direction;
Boolean           separator;
char              *text_string = NULL, *temp = NULL;

text_string = (char *)calloc (1, sizeof (char));

if (xmstring) {
    if (!XmStringInitContext (&context, xmstring)) {
      printf("Can't convert compound string.\n");
      return (NULL);
    }
    while  (XmStringGetNextSegment (context, &temp, &charset,
                                    &direction, &separator)) {
      text_string = (char *)realloc (text_string, strlen (temp)+1);
      if (text_string == NULL) {
        printf("Can't allocate space for file name.\n");
        return (NULL);
      }
      text_string = strcpy(text_string, temp);
    }

    XmStringFreeContext(context);
    }

return (text_string);
}

/********************************************************************************/
/* delete_all_list_items - removes all items from a list box                    */
/* INPUT: Widget list - id of list widget                                       */
/* OUTPUT: none                                                                 */
/********************************************************************************/
void delete_all_list_items (Widget list)
{
int item_count = 0;

XtVaGetValues (list, XmNitemCount, &item_count, NULL);
if (item_count > 0) {
  XmListDeleteItemsPos (list, item_count, 1);
}
return;
}

/********************************************************************************/
/* clear_text_field - removes any text from a text field                        */
/* INPUT: Widget textfield - id of text widget                                  */
/* OUTPUT: none                                                                 */
/********************************************************************************/
void clear_text_field (Widget textfield)
{
XmTextPosition last_pos;
char *empty = "";

last_pos = XmTextGetLastPosition (textfield);
XmTextReplace (textfield, 0, last_pos, empty);

return;
}

/********************************************************************************/
/* clear_text - removes any text from a text widget                             */
/* INPUT: Widget textwid - id of text widget                                    */
/* OUTPUT: none                                                                 */
/********************************************************************************/
void clear_text (Widget textwid)
{
XmTextPosition last_pos;
char *empty = "";

last_pos = XmTextGetLastPosition (textwid);
XmTextReplace (textwid, 0, last_pos, empty);

return;
}
