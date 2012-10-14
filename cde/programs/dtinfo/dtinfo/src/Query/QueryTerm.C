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
/*	copyright (c) 1994,1995,1996 FUJITSU LIMITED	*/
/*	All Rights Reserved				*/

/*
 * $XConsortium: QueryTerm.cc /main/6 1996/07/05 16:16:19 rws $
 *
 * Copyright (c) 1992 HAL Computer Systems International, Ltd.
 * All rights reserved.  Unpublished -- rights reserved under
 * the Copyright Laws of the United States.  USE OF A COPYRIGHT
 * NOTICE IS PRECAUTIONARY ONLY AND DOES NOT IMPLY PUBLICATION
 * OR DISCLOSURE.
 * 
 * THIS SOFTWARE CONTAINS CONFIDENTIAL INFORMATION AND TRADE
 * SECRETS OF HAL COMPUTER SYSTEMS INTERNATIONAL, LTD.  USE,
 * DISCLOSURE, OR REPRODUCTION IS PROHIBITED WITHOUT THE
 * PRIOR EXPRESS WRITTEN PERMISSION OF HAL COMPUTER SYSTEMS
 * INTERNATIONAL, LTD.
 * 
 *                         RESTRICTED RIGHTS LEGEND
 * Use, duplication, or disclosure by the Government is subject
 * to the restrictions as set forth in subparagraph (c)(l)(ii)
 * of the Rights in Technical Data and Computer Software clause
 * at DFARS 252.227-7013.
 *
 *          HAL COMPUTER SYSTEMS INTERNATIONAL, LTD.
 *                  1315 Dell Avenue
 *                  Campbell, CA  95008
 * 
 */


#define C_QueryTerm
#define C_QueryGroup
#define L_Query

#include "Prelude.h"

#include <string.h>
#include <ctype.h>

#if defined(sun)
#if defined(SVR4)
#define SunOS5
#else
#define SunOS4
#endif
#endif

#if defined(UseWideChars)
# if !defined(SunOS5)
#  define wslen(a) wcslen(a)
# endif
# if defined(SunOS4)
#   define mbstowcs(a,b,c) Xmbstowcs(a,b,c)
#   define wcstombs(a,b,c) Xwcstombs(a,b,c)
# elif defined(_IBMR2)
#   include <wcstr.h>
# endif
#endif

#include <wchar.h>
#if defined(USL) || defined(linux) || defined(CSRG_BASED)
#include <wctype.h>
#endif

#define TML_CHAR_TYPE wchar_t

#include <assert.h>

unsigned int QueryTerm::f_caps = 0;

// /////////////////////////////////////////////////////////////////
// class constructor
// /////////////////////////////////////////////////////////////////

QueryTerm::QueryTerm (QueryGroup *parent, QueryTerm *previous, QueryTerm *next)
: f_parent (parent), f_previous (previous), f_next (next),
  f_prefix (PFX_CONTAIN), f_connective (C_NONE),
  f_weight (strdup("")), f_proximity (strdup("")),
  f_term_string_fixed (TRUE),
  f_type (TYPE_SIMPLE)
{
    f_term_string = strdup(""); 
    // Link it into the list. 
    if (previous != NULL)
	f_previous->f_next = this;
    else
	parent->f_term_list = this;
    if (next)
	f_next->f_previous = this;
}


// /////////////////////////////////////////////////////////////////
// class destructor
// /////////////////////////////////////////////////////////////////

QueryTerm::~QueryTerm()
{
  if (f_previous != NULL)
    f_previous->f_next = f_next;
  else
    f_parent->f_term_list = f_next;
  if (f_next != NULL)
    f_next->f_previous = f_previous;
  free (f_weight);
  free (f_proximity);
  if (f_type == TYPE_GROUP)
    delete f_group_term;
  else
    free (f_term_string);
}


// /////////////////////////////////////////////////////////////////
// cleanup_term_string - parse out leading and trailing spaces
// /////////////////////////////////////////////////////////////////

void
QueryTerm::cleanup_term_string()
{
#ifdef UseQSearch
  // NOTE: full-text search is a major feature of QSearch, so do not
  //	   cut off surrounding spaces - 9/22/94 kamiya

  char* p = f_term_string;

  int n_bytes;

  for (char* last = p + strlen(p); p < last; p += n_bytes) {
	n_bytes = mblen(p, MB_CUR_MAX);
	assert( n_bytes > 0 );
	if (n_bytes == 1 && isspace(*p))
	    *p = ' ';
  }

  assert( p == last );

#else 
  // NOTE: this used to be done in place, but is now done in separate buffer
  // for wide char support 
  TML_CHAR_TYPE *first, *last;
  TML_CHAR_TYPE *wcbuf;
  int length, wclen;
  length = strlen(f_term_string);

  wcbuf = new TML_CHAR_TYPE[length + 1];

#ifdef UseWideChars
  wclen = mbstowcs(wcbuf, f_term_string, length + 1);
  assert( *(wcbuf + wclen) == (TML_CHAR_TYPE)'\0' );
#else
  strcpy((char *) wcbuf, f_term_string);
  wclen = length ;
#endif

  first = wcbuf;

  // Strip leading white space 
  while (iswspace (*first))
    first++;

  // Skip to the end of the string if not already there.
  last = wcbuf + wclen - 1;
  while ((last > first ) && iswspace (*last)) // strip trailing spaces
    last--;

  // Truncate whitespace if there was any. 
  if (last < wcbuf + wclen -1)
    {
      last++;          // advance to first trailing space 
      *last = (TML_CHAR_TYPE)'\0';    // truncate it
    }

  // Copy back over space, if any. 
  if ((first != wcbuf) || (last != wcbuf + wclen))
    {
#ifdef UseWideChars
      wcstombs(f_term_string, first, length + 1);
#else
      strcpy(f_term_string, (const char *) first);
#endif
    }
  delete[] wcbuf;
  f_term_string_fixed = TRUE;
#endif
}
