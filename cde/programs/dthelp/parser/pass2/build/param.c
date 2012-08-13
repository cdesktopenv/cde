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
/* $XConsortium: param.c /main/3 1995/11/08 10:43:22 rswiston $ */
/*
              Copyright 1986 Tandem Computers Incorporated.
This product and information is proprietary of Tandem Computers Incorporated.
                   Copyright (c) 1986, 1987, 1988, 1989 Hewlett-Packard Co.
*/

/* Param.c contains procedures related to parameters for program BUILD */

#include <stdlib.h>
#include <string.h>
#include "build.h"
#include "context.h"
#include "delim.h"

/* Add a keyword to the list of possible values of a keyword parameter for
   the current element
*/
void addkeyword(M_NOPAR)
{
int length ;
PARAMETER *par ;
PTYPE *kw ;

/* Check if duplicate keyword for this parameter */
for (kw = newpar->ptypep ; kw ; kw = kw->nextptr)
    if (w_strcmp(kw->keyword, name) == 0)
	{
	warning3("Warning: Repeated keyword %s in parameter %s of %s",
		 name,
		 newpar->paramname,
		 thisrule) ;
	return ;
	}

/* Check if duplicate keyword within parameters of this element */
for (par = plist ; par != newpar ; par = par->next)
    for (kw = par->ptypep ; kw ; kw = kw->nextptr)
	{
	if (w_strcmp(kw->keyword, name) == 0)
	    warning4("Keyword %s used in parameters %s and %s of %s",
	             name,
		     par->paramname,
		     newpar->paramname,
		     thisrule) ;
	if (! kw->next) break ;
	}
   
*nextptype = (PTYPE *) m_malloc(sizeof(PTYPE), "ptype") ;
if (newpar->kwlist) thisptype->next = ptypelen + 1 ;
else
    {
    newpar->kwlist = ptypelen + 1 ;
    newpar->ptypep = *nextptype ;
    }
thisptype = *nextptype ;
thisptype->next = M_NULLVAL ;
thisptype->nextptr = NULL ;
nextptype = &(thisptype->nextptr) ;
length = w_strlen(name) + 1 ;
thisptype->keyword = (M_WCHAR *) m_malloc(length, "keyword") ;
w_strcpy(thisptype->keyword, name) ;
kwlen += length ;
ptypelen++ ;
}

/* Add a parameter to the current element */
void addpar(M_NOPAR)
{
PARAMETER *paramp, *last ;
int length ;

parcount++ ;
pcount++ ;
for (paramp = plist ; paramp ; paramp = paramp->next)
    {
    if (w_strcmp(name, paramp->paramname) == 0)
    warning2("Multiple definition of parameter %s for element %s",
             name,
	     thisrule) ;
    last = paramp ;
    }
newpar = (PARAMETER *) m_malloc(sizeof(PARAMETER), "parameter") ;
if (! plist)
    plist = newpar ;
else
    last->next = newpar ;
*nextpar = newpar ;
nextpar = &newpar->nextptr ;
*nextpar = NULL ;
newpar->next = NULL ;
newpar->deftype = NULLDEF ;
newpar->defval = M_NULLVAL ;
newpar->defstring = NULL ;
newpar->kwlist = M_NULLVAL ;
length = w_strlen(name) + 1 ;
pnamelen += length ;
newpar->paramname = (M_WCHAR *) m_malloc(length, "parameter name") ;
w_strcpy(newpar->paramname, name) ;
newpar->ptypep = NULL ;
}


/* Check that specified default value is legal parameter value */
#if defined(M_PROTO)
LOGICAL checkdefault( const M_WCHAR *string )
#else
LOGICAL checkdefault(string)
M_WCHAR *string ;
#endif /* M_PROTO */
{
const M_WCHAR *p ;
int len ;
LOGICAL first ;

if (newpar->type == CDATA) return(TRUE) ;
if (m_allwhite(string)) return(FALSE) ;

first = TRUE ;
switch (newpar->type)
    {
    case IDREF:
    case NAMETYPE:
    case ENTATT:
    case NMTOKEN:
    case NUMBER:
    case NUTOKEN:
    /* Check length of default */
    if (w_strlen(string) > M_NAMELEN) return(FALSE) ;

    for (p = string ; *p ; p++)
	{
	if (m_cttype(*p) == M_NONNAME) return(FALSE) ;
	else if (first || newpar->type == NUMBER)
	switch (newpar->type)
	    {
	    case IDREF:
	    case NAMETYPE:
	    case ENTATT:
	      if (m_cttype(*p) != M_NMSTART) return(FALSE) ;
	      break ;
	    case NUMBER:
	    case NUTOKEN:
	      if (m_cttype(*p) != M_DIGIT) return(FALSE) ;
	      break ;
	    }
	first = FALSE ;
	}
    return(TRUE) ;
    case IDREFS:
    case NAMES:
    case NMTOKENS:
    case NUMBERS:
    case NUTOKENS:
    len = 1 ;
    for (p = string ; *p ; p++)
	{
	if (m_cttype(*p) == M_NONNAME)
	    {
	    if (! m_whitespace(*p)) return(FALSE) ;
	    len = 1 ;
	    }
	else if (len == 1) 
	  switch (newpar->type)
	      {
	      case IDREFS:
	      case NAMES:
		if (m_cttype(*p) != M_NMSTART) return(FALSE) ;
		break ;
	      case NUMBERS:
	      case NUTOKENS:
		if (m_cttype(*p) != M_DIGIT) return(FALSE) ;
		break ;
	      }
	else if (newpar->type == NUMBERS && 
		 m_cttype(*p) != M_DIGIT) return(FALSE) ;
	if (len++ > M_NAMELEN) return(FALSE) ;
	}
    break ;
    }
}

/* Normalize parameter default.  Change tabs and RE's to spaces, capitalize
   all letters (unless type is CDATA), remove extra space in lists */
#if defined(M_PROTO)
void normdefault(M_WCHAR *string )
#else
void normdefault(string)
M_WCHAR *string ;
#endif /* M_PROTO */
{
M_WCHAR *p, *q ;
int i ;

switch (newpar->type)
    {
    case IDREF:
    case NAMETYPE:
    case NMTOKEN:
    case NUTOKEN:
    case ENTATT:
      for ( ; *string ; string++)
	  *string = m_ctupper(*string) ;
      return ;
    case IDREFS:
    case NAMES:
    case NMTOKENS:
    case NUTOKENS:
    case NUMBERS:
      /* Remove leading spaces */
      for (p = string; *p ; p++)
	  if (! m_whitespace(*p)) break ;
      w_strcpy(string, p) ;

      /* Capitalize and normalize white space */
      for (p = string, i = 0 ; *p ; p++, i++)
	  if (m_whitespace(*p))
	      {
	      mbtowc(p, " ", 1);
	      for (q = p + 1 ; m_whitespace(*q); q++) ;
	      w_strcpy(p + 1, q) ;
	      }
	  else
	      *p = m_ctupper(*p) ;

      if (i && m_whitespace(string[i - 1])) string[i - 1] = M_EOS ;
      return ;

    default:
      for ( ; *string ; string++)
	  {
	  char mbyte[32]; /* bigger than the biggest multibyte char */
	  int  length;

	  length = wctomb(mbyte, *string);
	  if (length < 0)
	      {
	      m_error("Invalid wide character seen");
	      mbyte[0] = ' ';
	      mbyte[1] = 0;
	      }
	  if ((length == 1) && (*mbyte == '\n' || *mbyte == '\t'))
	    mbtowc(string, " ", 1);
	  }
      return ;
    }
}

/* Called at end of parameter attribute list rule */
void prulend(M_NOPAR)
{
LHS *lhsp ;
LHS *discard ;

if (pcount > maxpar) maxpar = pcount ;
for (lhsp = lhs ; lhsp ; )
    {
    if (lhsp->elt->parptr)
      m_err1(
	"Program error: prulend for %s, element with predefined parameters",
	lhsp->elt->enptr) ;
    lhsp->elt->parptr = plist ;
    lhsp->elt->paramcount = pcount ;
    lhsp->elt->parindex = parcount - pcount + 1 ;
    discard = lhsp ;
    lhsp = lhsp->next ;
    m_free((M_POINTER) discard, "lhs") ;
    }
ruleinit() ;
}

/* Make string the default for the current parameter of the current element */
#if defined(M_PROTO)
void setdefault(const M_WCHAR *string)
#else
void setdefault()
M_WCHAR *string;
#endif /* M_PROTO */
{
PTYPE *kw ;
int keycount ;
int length ;

/* Process default for non-keyword parameter*/
if (newpar->type != GRPO)
    {
    /* First check it is a legitimate value */
    if (! checkdefault(string))
	{
	m_err2("\"%s\": Not a valid default value for parameter %s",
	       string,
	       newpar->paramname) ;
	return ;
	}

    length = w_strlen(string) + 1 ;
    newpar->defstring = (M_WCHAR *) m_malloc(length, "default") ;
    w_strcpy(newpar->defstring, string) ;
    deflen += length ;

    /* Value is valid.  Change tabs and RE's to spaces, if value is not
       CDATA, put it in all caps, remove extra white space. */
    normdefault(newpar->defstring) ;
    }
else
    {
    /* It is a keyword parameter, store a pointer to the value */
    for (kw = newpar->ptypep ; kw ; kw = kw->nextptr)
	{
	if (! m_wcupstrcmp(string, kw->keyword))
	    {
	    for (keycount = 0 ; kw ; kw = kw->nextptr)
	      keycount += w_strlen(kw->keyword) + 1 ;
	    newpar->defval = kwlen - keycount + 1;
	    return ;
	    }
	}
    m_err2("Invalid default: %s not a valid value for %s",
           string,
	   newpar->paramname) ;
    }
}
