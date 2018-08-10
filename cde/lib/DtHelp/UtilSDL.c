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
/* $XConsortium: UtilSDL.c /main/10 1996/08/30 13:43:15 cde-hp $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:	   UtilSDL.c
 **
 **   Project:     Cde Help System
 **
 **   Description: Utility functions for parsing an SDL volume.
 **
 **  (c) Copyright 1987, 1988, 1989, 1990, 1991, 1992 Hewlett-Packard Company
 **
 **  (c) Copyright 1993, 1994 Hewlett-Packard Company
 **  (c) Copyright 1993, 1994 International Business Machines Corp.
 **  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 **  (c) Copyright 1993, 1994 Novell, Inc.
 **
 ****************************************************************************
 ************************************<+>*************************************/

/*
 * system includes
 */
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/*
 * Canvas Engine includes
 */
#include "CanvasP.h"
#include "CanvasSegP.h"

/*
 * private includes
 */
#include "Access.h"
#include "FontAttrI.h"
#include "AccessSDLP.h"
#include "CanvasError.h"
#include "bufioI.h"
#include "FormatUtilI.h"
#include "SDLI.h"
#include "FormatSDLI.h"
#include "StringFuncsI.h"
#include "UtilSDLI.h"

#ifdef NLS16
#endif

/********    Private Function Declarations    ********/
/********    End Private Function Declarations    ********/

/******************************************************************************
 *
 * Private defines.
 *
 *****************************************************************************/
#ifndef	FALSE
#define	FALSE	0
#endif

#ifndef	False
#define	False	0
#endif

#ifndef	TRUE
#define	TRUE	1
#endif

#ifndef	True
#define	True	1
#endif

#define	GROW_SIZE   10
#define	MAX_ELEMENT_SIZE   30

typedef struct  _classStyleMatrix
  {
    SdlOption      start;
    SdlOption      end;
    enum SdlElement     style;
  } ClassStyleMatrix;

/******************************************************************************
 *
 * Private macros.
 *
 *****************************************************************************/
#define	IsWhiteSpace(x) \
		(((x) == '\n' || (x) == ' ' || (x) == '\t') ? 1 : 0)

#define	IsNumber(x) \
		(((x) >= '0' && (x) <= '9') ? 1 : 0)

/******************************************************************************
 *
 * Private data.
 *
 *****************************************************************************/
static ClassStyleMatrix ClassToStyle[] =
  {
    { SdlClassAcro  , SdlClassUdefkey  , SdlElementKeyStyle  },
    { SdlClassLabel , SdlClassUdefhead , SdlElementHeadStyle },
    { SdlClassTable , SdlClassUdeffrmt , SdlElementFormStyle },
    { SdlClassTable , SdlClassUdeffrmt , SdlElementFrmtStyle },
    { SdlClassFigure, SdlClassUdefgraph, SdlElementGrphStyle },
    { SdlOptionBad  , SdlOptionBad     , SdlElementNone      },
  };

/******************************************************************************
 *
 * Private Functions
 *
 *****************************************************************************/
/******************************************************************************
 * Function:	static int SkipWhiteSpace (BufFilePtr f)
 *
 * Parameters:
 *
 * Returns	 0 if successfully skipped white space.
 *		-1 if errors.
 *
 * errno Values:
 *
 * Purpose:
 *
 *****************************************************************************/
static int
SkipWhiteSpace (
    BufFilePtr	 f)
{
    char   c;

    do {
        c = BufFileGet(f);
	if (c == BUFFILEEOF)
	    return -1;

    } while (IsWhiteSpace(c));

    BufFilePutBack(c, f);


    return 0;
}

/******************************************************************************
 * Function:	static int CheckSDLAttributeNameStart (char c)
 *
 * Parameters:
 *
 * Returns	 0 if starts correctly,
 *		-1 if does not start correctly for an attribute name
 *
 * errno Values:
 *
 * Purpose:
 *
 *****************************************************************************/
static int
CheckSDLAttributeNameStart (char c)
{
    if (('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z'))
	return 0;

    return -1;
}

/******************************************************************************
 * Function:	int GetSDLElement (BufFilePtr f, char **string)
 *
 * Parameters:
 *
 * Returns	 0 if got an element (subject to verification).
 *		 1 if end of file/compressed section.
 *		-1 if errors.
 *
 * errno Values:
 *
 * Memory owned:  the returned string.
 *
 * Purpose:
 *
 *****************************************************************************/
static int
GetSDLElement (
    BufFilePtr	 f,
    int		 max_allowed,
    char	*string)
{
    _DtCvValue     done     = False;
    _DtCvValue     first    = True;
    _DtCvValue     nonWhite = False;
    _DtCvValue     startEl  = False;
    int           len      = 0;
    char    c;

    do {
	c = BufFileGet(f);

	/*
	 * [chars][space]	(nonWhite == True  && IsWhiteSpace(c))
	 * <element>		(startEl  == True  && c == '>')
	 * [chars]<element..	(first    == False && c == '<')
	 */
	if (c == BUFFILEEOF || (nonWhite == True && IsWhiteSpace(c))
		|| (startEl == True  && c == '>')
		|| (first   == False && c == '<'))
	  {
	    if (c == '>'  || c == '<' || (nonWhite == True && IsWhiteSpace(c)))
		BufFilePutBack(c,f);
	    done = True;
	  }
	else
	  {
	    if (first == True && c == '<')
		startEl = True;

	    first = False;
	    if (nonWhite == False && IsWhiteSpace(c) == False)
		nonWhite = True;

	    string[len++] = c;

	    if (len >= max_allowed)
		done = True;
	  }
    } while (!done);

    string[len] = '\0';

    if (len == 0)
      {
	if (c != BUFFILEEOF)
	    return -1;

        else /* if (c == BUFFILEEOF) */
	    return 1;
      }

    return 0;
}

/******************************************************************************
 *
 * Semi Public Functions
 *
 *****************************************************************************/
/******************************************************************************
 * Function:	int _DtHelpCeMatchSdlElement (BufFilePtr my_file,
 *					const char *element_str, int sig_chars)
 *
 * Parameters:
 *
 * Returns	 0 if successfully matched element_str,
 *		 1 if did not match,
 *		-1 if errors.
 *
 * errno Values:
 *
 * Purpose:
 *
 *****************************************************************************/
int
_DtHelpCeMatchSdlElement (
    BufFilePtr	 f,
    const char	*element_str,
    int		 sig_chars)
{
    int   len;
    int   different = False;
    char  nextEl[MAX_ELEMENT_SIZE + 2];
    char *ptr;
    char  c;

    /*
     * get the next element in the stream
     */
    do {
        if (GetSDLElement(f, MAX_ELEMENT_SIZE, nextEl) != 0)
	    return -1;

        /*
         * now skip the white space
         */
        ptr = nextEl;
        while (IsWhiteSpace(*ptr)) ptr++;

      } while (*ptr == '\0');

    len = strlen(element_str);

    do {
	/*
	 * get the lowered version of the character
	 */
	c = _DtHelpCeToLower(*ptr);
	if (c != *element_str)
	    different = True;

	len--;
	sig_chars--;
	element_str++;
	ptr++;

    } while (len && sig_chars && !different && *ptr != '\0');

    if (!different && sig_chars > 0 && len > 0)
	different = True;

    return different;
}

/******************************************************************************
 * Function:	int _DtHelpCeGetSdlAttribute (BufFilePtr f,
 *					char *attribute_name)
 *
 * Parameters:
 *		f		Specifies the buf file to read.
 *		max_len		Specifies the maximum number of characters
 *				that can fit in 'attribute_name'. Does
 *				*NOT* include the terminating null byte.
 *		attribute_name	Specifies the buffer in which to place
 *				the attribute.
 *
 *
 * Returns	 0 if successfully retrieved an attribute name.
 *		 1 if there is no more attributes (eats the element end).
 *		-1 if errors.
 *
 * errno Values:
 *
 * Purpose:	Reads the an attribute.
 *		Lower cases the attribute.
 *
 *****************************************************************************/
int
_DtHelpCeGetSdlAttribute (
    BufFilePtr	 f,
    int		 max_len,
    char	*attribute_name)
{
    int   len = 0;
    char  c;

    if (SkipWhiteSpace(f) != 0)
	return -1;

    /*
     * check that the attribute name starts correctly
     */
    c = BufFileGet(f);
    if (c == '>')
	return 1;

    if (CheckSDLAttributeNameStart(c) != 0)
	return -1;

    do {
	c = _DtHelpCeToLower(c);

	attribute_name[len++] = c;

	c = BufFileGet(f);

    } while (c != BUFFILEEOF && !IsWhiteSpace(c) && c != '=' && len < max_len);

    while (c != '=')
      {
	c = BufFileGet(f);
	if (c == BUFFILEEOF)
	    return -1;
      }

    if (len == 0 || len >= max_len)
	return -1;

    attribute_name[len] = '\0';

    return 0;
}

/******************************************************************************
 * Function:	int _DtHelpCeReturnSdlElement ()
 *
 * Parameters:
 *
 *
 * Returns	 0 if matched an element in the list
 *		 1 hit eof on the file/compressed section.
 *		-1 if errors.
 *
 * errno Values:
 *
 * Purpose:
 *
 * Memory Owned By Caller:	number_str
 *
 *****************************************************************************/
int
_DtHelpCeReturnSdlElement (
    BufFilePtr		  f,
    const SDLElementInfo *el_list,
    _DtCvValue		  cdata_flag,
    enum SdlElement	 *ret_el_define,
    char	        **ret_data,
    _DtCvValue		 *ret_end_flag)
{
    int        result;
    int        lowerChar;
    _DtCvValue  endFlag   = False;
    _DtCvValue  different = True;
    char       nextEl[MAX_ELEMENT_SIZE + 2];
    char      *ptr;

    *ret_el_define = SdlElementCdata;
    *ret_end_flag  = False;
    if (ret_data)
	*ret_data = NULL;

    /*
     * get the next element
     */
    do {
        result = GetSDLElement(f, MAX_ELEMENT_SIZE, nextEl);
	if (result == 1)
	  {
	    *ret_el_define = SdlElementNone;
	    return 1;
	  }
        else if (result == -1)
	    return -1;

        /*
         * lower case the element. String dup it first incase this is data
         * and not an element. Then skip the leading white space.
         */
        ptr     = nextEl;
        while (IsWhiteSpace(*ptr) == True) ptr++;
      } while (cdata_flag == False && *ptr == '\0');

    /*
     * An element starts with a less than sign,
     * An end element starts with a less than sign and a forward slash.
     */

    /*
     * if all we've got is white space, or a non starting character,
     * then skip the loop
     */
    if (*ptr == '<')
      {
	/*
	 * skip the less than sign.
	 */
	ptr++;

	/*
	 * is this an end element?
	 */
	if (*ptr == '/')
	  {
	    endFlag = True;
	    ptr++;
	  }

	lowerChar = (int) _DtHelpCeToLower(*ptr);

        while (different && el_list->element_str != NULL)
          {
	    if (endFlag == True && el_list->element_end_str != NULL &&
		lowerChar == ((int)(el_list->element_end_str[2])) &&
		_DtHelpCeStrNCaseCmpLatin1(&(el_list->element_end_str[2]),
					ptr, el_list->end_sig_chars - 2) == 0)
	      {
		different = False;
	        *ret_end_flag = True;
	      }
	    else if (endFlag == False &&
		lowerChar == ((int)(el_list->element_str[1])) &&
		 _DtHelpCeStrNCaseCmpLatin1(&(el_list->element_str[1]), ptr,
					el_list->str_sig_chars - 1) == 0)
	        different = False;
	    else
	        el_list++;
          }
	*ret_el_define = el_list->sdl_element;
      }

    if (*ret_el_define == SdlElementCdata && ret_data != NULL)
	*ret_data = strdup(nextEl);

    return 0;
}

/******************************************************************************
 * Function:	int _DtHelpCeGetSdlCdata (BufFilePtr f, char **string)
 *
 * Parameters:
 *
 *
 * Returns	 0 if successfully retrieved data.
 *		 1 if returning because type != SdlTypeDynamic and a newline
 *		   was found.
 *		 2 if returning because ran into a non-breaking character.
 *		-1 if errors.
 *
 * errno Values:
 *
 * Purpose:
 *
 * Memory Owned By Caller:	attribute_name
 *
 *****************************************************************************/
int
_DtHelpCeGetSdlCdata (
    BufFilePtr	   f,
    SdlOption      type,
    int		   max_mb,
    _DtCvValue     nl_to_space,
    _DtCvValue    *cur_space,
    _DtCvValue    *cur_nl,
    _DtCvValue    *cur_mb,
    char	 *non_break_char,
    char	**string,
    int		 *str_max)
{
    _DtCvValue  lastWasSpace = *cur_space;
    _DtCvValue  lastWasNl    = *cur_nl;
    _DtCvValue  lastWasMb    = *cur_mb;
    _DtCvValue  ignore  = False;
    _DtCvValue  done    = False;
    _DtCvValue  escaped = False;
    int        strSize = 0;
    int        reason  = 0;
    int        len     = 0;
    int        strMB   = 1;
    char      *ptr;
    char       buf[MB_LEN_MAX + 1];
#define	ESC_STR_LEN	4
    char       escStr[ESC_STR_LEN];
    char       c;

    if (string == NULL)
	ignore = True;
    else if (*string != NULL)
      {
	strSize = strlen(*string);
	if (*str_max < strSize)
	   *str_max = strSize;
      }

    do {
	c = BufFileGet(f);

	buf[len++] = c;
	buf[len]   = '\0';

	if (c != BUFFILEEOF && max_mb != 1)
	    strMB = mblen(buf, max_mb);

	if (c == BUFFILEEOF || (escaped == False && strMB == 1 &&
		(c == '<' ||
		    ((type == SdlTypeLiteral || type == SdlTypeLined) &&
			c == '\n'))))
	  {
	    if (c == '\n')
		reason = 1;
	    else if (c == '<')
		BufFilePutBack(c,f);

	    done = True;
	  }
	else if (strMB == 1)
	  {
	    len = 0;

	    if (lastWasNl == True)
	      {
		ptr = " ";
	        if (_DtHelpCeAddCharToBuf(&ptr, string, &strSize,
							str_max, 32) == -1)
		    return -1;
		lastWasSpace = True;
	      }

	    lastWasMb = False;
	    lastWasNl = False;

	    if (escaped == True)
	      {
		if (c == '#')
		  {
		    /*
		     * process the SGML numeric character reference
		     * entity.  '&#d[d[d]][;]' - 'ddd' represent
		     * characters of '0' to '9'.  The semi-colon is
		     * required iff the next character is a numeric
		     * character of '0' to '9'.  Otherwise it is
		     * optional.
		     */
		    int  value;

		    len = 0; /* len should be zero here, but let's make sure */
		    do
		      {
			/*
			 * get the next character.
			 */
			c = BufFileGet(f);

			/*
			 * if it is the first character of the numeric entity,
			 * it has to be a numeric character.
			 */
			if (c == BUFFILEEOF ||
					(len == 0 && (c < '0' || c > '9')))
			    return -1;

			/*
			 * put the character in the buffer
			 */
			escStr[len++] = c;

		      } while (len < ESC_STR_LEN && c >= '0' && c <= '9');

		    /*
		     * the ending character of the entity has to be a
		     * non-numeric character.
		     */
		    if (c >= '0' && c <= '9')
			return -1;
		    /*
		     * if the optional semi-colon is not present, replace
		     * the terminating character back in the input buffer
		     * to be processed in the next pass.
		     */
		    else if (c != ';')
			BufFilePutBack(c, f);

		    /*
		     * get the integer value. It has to be less than 256.
		     */
		    value = atoi(escStr);
		    if (value > 255)
			return -1;

		    /*
		     * transfer the value to the character variable
		     * and zero the index for the general buffer.
		     */
		    c   = value;
		    len = 0;

		     /*
		      * set up for the return information
		      */
		     done   = True;
		     reason = 2;
		     *non_break_char = c;
		  }

		/*
		 * clear the escaped flag, so that the character gets
		 * put in the output buffer.
		 */
	 	escaped = False;
	      }
	    else if (c == '\n')
	      {
		if (type == SdlTypeCdata)
		    lastWasSpace = False;
		else if (lastWasSpace == False)
		    lastWasNl = True;
		continue;
	      }
	    else if (c == '&')
		escaped = True;

	    if (escaped == False && ignore == False)
	      {
	        if (c == '\t')
		    c = ' ';

	        ptr = &c;
	        if (c == '\n')
		  {
		    lastWasSpace = True;
		    reason       = 1;
		    done         = True;
		  }
		/*
		 * as long as we aren't processing a non break character,
		 * add it to the buffer.
		 */
		else if (2 != reason)
		  {
		    if (c != ' ')
		        lastWasSpace = False;

		    if ((lastWasSpace == False || type == SdlTypeLiteral ||
						type == SdlTypeUnlinedLiteral)
				&&
	                _DtHelpCeAddCharToBuf(&ptr, string, &strSize,
							str_max, 32) == -1)
		            return -1;

		    if (type != SdlTypeLiteral &&
				type != SdlTypeUnlinedLiteral && c == ' ')
		        lastWasSpace = True;
		  }
	      }
	  }
	else if (strMB > 1)
	  {
	    if (ignore == False)
	      {
	        if (lastWasNl == True)
	          {
	            if (nl_to_space == True || lastWasMb == False)
	              {
		        ptr = " ";
	                if (_DtHelpCeAddCharToBuf(&ptr, string, &strSize,
							str_max, 32) == -1)
		            return -1;
	              }
	          }

	        ptr = buf;
	        if (_DtHelpCeAddStrToBuf(&ptr,string,&strSize,str_max,len,32)
						== -1)
		    return -1;
	      }
	    lastWasMb    = True;
	    lastWasNl    = False;
	    lastWasSpace = False;
	    escaped      = False;
	    len          = 0;
	  }
	else if (strMB == 0 || len > max_mb || len >= MB_LEN_MAX)
	    return -1;

    } while (done == False);

    *cur_space = lastWasSpace;
    *cur_nl    = lastWasNl;
    *cur_mb    = lastWasMb;
    return reason;
}

/******************************************************************************
 * Function:	int _DtHelpCeGetSdlAttributeCdata (BufFilePtr f, char **string);
 *
 * Parameters:
 *
 *
 * Returns	 0 if successfully retrieved an attribute name.
 *		-1 if errors.
 *
 * errno Values:
 *
 * Purpose:	Parses the CDATA value for an element attribute. Expects the
 *		pointer to be after the equals sign of the attribute.  The
 *		CDATA must be included in quotes (either single or double)
 *		and can have white space (spaces, newlines or tabs) preceding
 *		it.  Any newline encountered within the CDATA value is turned
 *		into a space.  Any quote enclosed in the CDATA must not be the
 *		same type as what is used to delimit the CDATA value.  I.E.
 *		if a single quote is used in the CDATA, the CDATA value must
 *		be enclosed in double quotes.
 *
 * Memory Owned By Caller:	attribute_name
 *
 *****************************************************************************/
int
_DtHelpCeGetSdlAttributeCdata (
    BufFilePtr	  f,
    _DtCvValue	  limit,
    char	**string)
{
    _DtCvValue   ignore      = False;
    _DtCvValue   done        = False;
    _DtCvValue   firstChar   = True;
    int         strSize     = 0;
    int         strMax      = 0;
    char       *myStr       = NULL;
    char        c;
    char        testChar;

    if (string == NULL)
	ignore = True;
    else if (limit == True)
	myStr = *string;

    if (SkipWhiteSpace(f) != 0)
	return -1;

    do {
	c = BufFileGet(f);
	if (c == BUFFILEEOF)
	  {
	    if (myStr != NULL)
		free(myStr);
	    return -1;
	  }

	if (firstChar)
	  {
	    if (c == '\'' || c == '\"')
	      {
	        testChar  = c;
	        firstChar = False;
	      }
	    else
	      {
	        if (myStr != NULL)
		    free(myStr);
	        return -1;
	      }
	  }
	else if (c == testChar)
	    done = True;
	else if (ignore == False)
	  {
	    if (c == '\n')
		c = ' ';

	    if (limit == True && strSize >= DTD_NAMELEN)
	      {
		if (myStr != NULL)
		    free(myStr);
		return -1;
	      }
	    else if (limit == False && strSize >= strMax)
	      {
		strMax += 64;
		if (strSize == 0)
		    myStr = (char *) malloc (sizeof(char) * 65);
		else
		    myStr = (char *) realloc (myStr,(sizeof(char)*(strMax+1)));

		if (myStr == NULL)
		    return -1;
	      }

	    myStr[strSize++] = c;
	  }
    } while (done == False);

    if (ignore == False)
      {
	if (myStr != NULL)
            myStr[strSize] = '\0';
	if (limit == False)
            *string = myStr;
      }

    return 0;
}

/******************************************************************************
 * Function:	int _DtHelpCeSkipCdata (BufFilePtr f, _DtCvValue Flag)
 *
 * Parameters:
 *
 *
 * Returns	 0 if successfully retrieved an attribute name.
 *		-1 if errors.
 *
 * errno Values:
 *
 * Purpose:
 *
 * Memory Owned By Caller:	attribute_name
 *
 *****************************************************************************/
int
_DtHelpCeSkipCdata (
    BufFilePtr	  f,
    _DtCvValue	  flag)
{
    _DtCvValue   done        = False;
    _DtCvValue   singleQuote = False;
    _DtCvValue   doubleQuote = False;
    _DtCvValue   comment     = False;
    _DtCvValue   element     = False;
    _DtCvValue   bracket     = False;
    _DtCvValue   processing  = True;
    char        c;

    do {
	c = BufFileGet(f);
	if (c == BUFFILEEOF)
	    return -1;

	if (processing)
	  {
	    if (c == '\'')
	      {
		singleQuote = True;
		processing  = False;
	      }
	    else if (c == '\"')
	      {
		doubleQuote = True;
		processing  = False;
	      }
	    else if (c == '-')
	      {
		c = BufFileGet(f);
		if (c == BUFFILEEOF)
	    	return -1;
	    
		if (c == '-')
		  {
		    comment    = True;
		    processing = False;
		  }
		else
	    	    BufFilePutBack(c,f);
	      }
	    else if (!element && c == '<')
		  element = True;

	    else if (c == '>')
	      {
		if (element)
		    element = False;
		else
		  {
	    	    BufFilePutBack(c,f);
		    done = True;
		  }
	      }
	    else if (flag && c == '[')
		bracket = True;

	    else if (bracket && c == ']')
	      {
		bracket = False;
		flag    = False;
	      }
	  }
	else
	  {
	    if (singleQuote && c == '\'')
	      {
		singleQuote = False;
		processing  = True;
	      }
	    else if (doubleQuote && c == '\"')
	      {
		doubleQuote = False;
		processing  = True;
	      }
	    else if (comment && c == '-')
	      {
		c = BufFileGet(f);
		if (c == BUFFILEEOF)
		    return -1;
	    
		if (c == '-')
		  {
		    comment     = False;
		    processing  = True;
		  }
		else
		    BufFilePutBack(c,f);
	      }
	  }
    } while (done == False);

    if (flag)
	return -1;

    return 0;
}

/******************************************************************************
 * Function:	int _DtHelpCeGetSdlId (BufFilePtr f, char **id_value)
 *
 * Parameters:
 *
 *
 * Returns	 0 if successfully retrieved an attribute name.
 *		-1 if errors.
 *
 * errno Values:
 *
 * Purpose:
 *
 * Memory Owned By Caller:	id_value
 *
 *****************************************************************************/
int
_DtHelpCeGetSdlId (
    BufFilePtr	 f,
    char	**id_value)
{
    *id_value = NULL;

    if (_DtHelpCeGetSdlAttributeCdata (f, False, id_value) != 0)
	return -1;

    /*
     * check that the attribute name starts correctly
     */
    if (CheckSDLAttributeNameStart(**id_value) != 0)
	return -1;

    return 0;
}

/******************************************************************************
 * Function:	int _DtHelpCeGetSdlNumber (BufFilePtr f, char **number_str)
 *
 * Parameters:
 *
 *
 * Returns	 0 if successfully retrieved an attribute name.
 *		-1 if errors.
 *
 * errno Values:
 *
 * Purpose:
 *
 * Memory Owned By Caller:	number_str
 *
 *****************************************************************************/
int
_DtHelpCeGetSdlNumber (
    BufFilePtr	 f,
    char	*number_str)
{
    if (_DtHelpCeGetSdlAttributeCdata(f, True, &number_str) != 0)
	return -1;

    /*
     * check that the attribute name starts correctly
     */
    if (!IsNumber(*number_str))
	return -1;

    return 0;
}

/******************************************************************************
 * Function:	int _DtHelpCeFindSkipSdlElementEnd (BufFilePtr f)
 *
 * Parameters:
 *
 *
 * Returns	 0 if successfully retrieved an attribute name.
 *		-1 if errors.
 *
 * errno Values:
 *
 * Purpose:
 *
 * Memory Owned By Caller:	attribute_name
 *
 *****************************************************************************/
int
_DtHelpCeFindSkipSdlElementEnd (
    BufFilePtr	 f)
{
    char  c;

    if (SkipWhiteSpace(f) != 0)
	return -1;

    do {
	c = BufFileGet(f);
	if (c == BUFFILEEOF)
	  return -1;

    } while (c != '>');

    return 0;
}

/******************************************************************************
 * Function:    int _DtHelpCeMergeSdlAttribInfo (_DtCanvasStruct canvas,
 *				_DtCvSegment *src_el,
 *				_DtCvSegment *dst_el);
 *
 * Parameters:
 *
 * Returns:     nothing
 *
 * Purpose: Merge the src specified attributes into the dst.
 *	    THIS WILL NOT MERGE THE SSI or RSSI values.
 *
 ******************************************************************************/
void
_DtHelpCeMergeSdlAttribInfo(
    _DtCvSegment		*src_seg,
    _DtCvSegment		*dst_seg,
    _DtHelpFontHints		*dst_fonts,
    void			*el_info,
    unsigned long		*str1_values,
    unsigned long		*str2_values)
{
    unsigned long	 flag;
    const SDLAttribute	*pAttr;
    SdlOption		*dstEnum;
    SdlOption		*srcEnum;
    SDLNumber		*dstNum;
    SDLNumber		*srcNum;
    SDLCdata		*dstStr;
    SDLCdata		*srcStr;
    SDLTossInfo		*tossInfo;
    char                *src;
    char                *dst;

    if (src_seg != NULL)
      {
	tossInfo = _SdlSegPtrToTossInfo(src_seg);
        for (pAttr = _DtHelpCeGetSdlAttributeList();
			pAttr->data_type != SdlAttrDataTypeInvalid; pAttr++)
          {
	    if ((pAttr->struct_type == SdlFontSpecific ||
				pAttr->struct_type == SdlContainerSpecific)
			&&
		SDLIsAttrSet(*tossInfo, pAttr->sdl_attr_define)
			&&
		!(ATTRS_EQUAL(pAttr->data_type, pAttr->sdl_attr_define,
				SDL_ATTR_SSI) ||
		  ATTRS_EQUAL(pAttr->data_type, pAttr->sdl_attr_define,
				SDL_ATTR_RSSI)))
	      {
		if (pAttr->struct_type == SdlFontSpecific)
		  {
		    src = ((char *)(&(_SdlTossInfoPtrFontSpecs(tossInfo))));
		    dst = ((char *) dst_fonts);
		  }
		else /* if (pAttr->struct_type == SdlContainerSpecific) */
		  {
		    src = ((char *)_SdlContainerPtrOfSeg(src_seg));
		    dst = ((char *)_SdlContainerPtrOfSeg(dst_seg));
		  }

	        switch (SDLAttrFlagType(pAttr->sdl_attr_define))
	          {
		    case ENUM_VALUE:
		        dstEnum  = (SdlOption *)
					(((char *)dst) + pAttr->field_ptr);
		        srcEnum  = (SdlOption *)
					(((char *)src) + pAttr->field_ptr);
		        *dstEnum = *srcEnum;
		        break;

		    case NUMBER_VALUE:
		        dstNum  = (SDLNumber *)
					(((char *)dst) + pAttr->field_ptr);
		        srcNum  = (SDLNumber *)
					(((char *)src) + pAttr->field_ptr);
		        *dstNum = *srcNum;
		        break;

		    case STRING1_VALUE:
		    case STRING2_VALUE:
		        dstStr  = (SDLCdata *)
					(((char *)dst) + pAttr->field_ptr);
		        srcStr  = (SDLCdata *)
					(((char *)src) + pAttr->field_ptr);
		        *dstStr = *srcStr;
		        break;

	          }
	      }
	    else if (NULL != el_info &&
			pAttr->struct_type == SdlElementSpecific &&
				SDLIsAttrSet(*tossInfo, pAttr->sdl_attr_define))
	      {
		flag = pAttr->sdl_attr_define & ~(VALUE_MASK);

		dstStr = (SDLCdata *)(((char *)el_info) + pAttr->field_ptr);
		if (SDL_ATTR_COLW == pAttr->sdl_attr_define)
		    *dstStr = _SdlTossInfoPtrColW(tossInfo);
		else if (SDL_ATTR_COLJ == pAttr->sdl_attr_define)
		    *dstStr = _SdlTossInfoPtrColJ(tossInfo);
		else if (SDL_ATTR_ENTER == pAttr->sdl_attr_define)
		    *dstStr = _SdlTossInfoPtrEnter(tossInfo);
		else if (SDL_ATTR_EXIT == pAttr->sdl_attr_define)
		    *dstStr = _SdlTossInfoPtrExit(tossInfo);

		if (STRING1_VALUE == (pAttr->sdl_attr_define & VALUE_MASK))
		   *str1_values = *str1_values | flag;
		else if (STRING2_VALUE == (pAttr->sdl_attr_define & VALUE_MASK))
		   *str2_values = *str2_values | flag;
	      }
          }
      }
}

/******************************************************************************
 * Function:    void _DtHelpCeAddSegToList (_DtCvSegment *seg_ptr,
 *				_DtCvSegment **seg_list,
 *				_DtCvSegment **last_seg)
 *
 * Parameters:
 *
 * Returns:     0 if successful, -1 if errors
 *
 * errno Values:
 *
 * Purpose:     Allocates a memory
 *
 ******************************************************************************/
void
_DtHelpCeAddSegToList(
    _DtCvSegment	*seg_ptr,
    _DtCvSegment **seg_list,
    _DtCvSegment **last_seg)
{
    if (*seg_list == NULL)
        *seg_list = seg_ptr;
    else
        (*last_seg)->next_seg = seg_ptr;

    while (NULL != seg_ptr->next_seg)
	seg_ptr = seg_ptr->next_seg;

    *last_seg = seg_ptr;
}

/******************************************************************************
 * Function:    int _DtHelpCeAllocSegment (
 *					int  malloc_size,
 *					int *alloc_size,
 *					_DtCvSegment **next_seg,
 *					_DtCvSegment **seg_ptr);
 *
 * Parameters:
 *		malloc_size	Specifies the number of blocks to
 *				malloc. The first one is returned in
 *				'seg_ptr' with the rest in 'next_seg',
 *				if 'next_seg' is non-NULL.
 *		alloc_size	Ignored if NULL. If non-NULL, indicates
 *				the number of blocks in 'next_seg' if
 *				'next_seg' is non-NULL and the 'next_seg'
 *				pointer is non-NULL.
 *		next_seg	Specifies the block of memory to take the
 *				next segment from.
 *				Returns the pointer to the next segment.
 *		seg_ptr		Returns a pointer to the segment to use.
 *
 * Returns:     0 if successful, -1 if errors
 *
 * Purpose:     Allocates a memory
 *
 ******************************************************************************/
int
_DtHelpCeAllocSegment(
    int		  malloc_size,
    int		 *alloc_size,
    _DtCvSegment   **next_seg,
    _DtCvSegment   **seg_ptr)
{
    int		 cnt    = 0;
    _DtCvSegment	*newSeg = NULL;

    if (next_seg != NULL)
	newSeg = *next_seg;

    if (alloc_size != NULL && newSeg != NULL)
	cnt = *alloc_size;

    if (cnt < 1)
      {
	newSeg = _DtHelpAllocateSegments(malloc_size);
        if (newSeg == NULL)
            return -1;

        cnt    = --malloc_size;
      }
    else
	cnt--;

    *seg_ptr    = newSeg++;
    if (next_seg != NULL)
      {
	*next_seg = NULL;
	if (cnt > 0)
            *next_seg   = newSeg;
      }
    if (alloc_size != NULL)
        *alloc_size = cnt;

    return 0;
}

/******************************************************************************
 * Function:    int _DtHelpCeSaveString (
 *                              string, multi_len)
 *
 * Parameters:
 *
 * Returns:     0 if successful, -1 if errors
 *
 * errno Values:
 *
 * Purpose:     Looks for the virtual page beginning.
 *
 ******************************************************************************/
int
_DtHelpCeSaveString(
    _DtCvPointer  client_data,
    _DtCvSegment	**seg_list,
    _DtCvSegment	**last_seg,
    _DtCvSegment	**prev_seg,
    char         *string,
    _DtHelpFontHints   *font_specs,
    int           link_index,
    int           multi_len,
    int           flags,
    void	(*load_font)(),
    _SdlFontMode  resolve_font,
    _DtCvValue     newline)
{

    int        len;
    int        size;
    int        numChars = 0;
    int        junk    = 0;
    char       tmpChar;
    char         *ptr     = string;
    _DtCvSegment    *pSeg = NULL;
    _DtCvSegment    *junkSeg  = NULL;
    _DtHelpFontHints   *fontSpec = NULL;
    _DtCvValue addToList = False;
    _DtCvPointer useFont = (_DtCvPointer) -1;

    if (*ptr == '\0')
      {
    	if (newline == True)
	  {
	    if (_DtHelpCeAllocSegment(1, &junk, &junkSeg, &pSeg) != 0)
	        return -1;
	    addToList = True;
	  }
      }
    else
      {
	if (_SdlFontModeResolve == resolve_font)
	    (*load_font)(client_data, _DtHelpFontHintsLang(*font_specs),
				_DtHelpFontHintsCharSet(*font_specs),
				*font_specs, &useFont);

	while (*ptr != '\0')
	  {
	    if (_DtHelpCeAllocSegment(1, &junk, &junkSeg, &pSeg) != 0)
		return -1;

	    if (_SdlFontModeSave == resolve_font)
	      {
	        fontSpec = (_DtHelpFontHints *) malloc (sizeof(_DtHelpFontHints));
	        if (NULL == fontSpec)
	          {
		    free(pSeg);
		    return -1;
	          }

		/*
		 * copy the current fonts over
		 */
		*fontSpec = *font_specs;
		useFont   = (_DtCvPointer) fontSpec;
	      }

	    /*
	     * set the type
	     */
	    pSeg->type = _DtCvSetTypeToString(flags);

	    /*
	     * set the wide char flag if necessary.
	     */
	    len = 1;
	    if (multi_len != 1)
	      {
		len  = mblen (ptr, MB_CUR_MAX);
		size = _DtHelpFmtFindBreak(ptr, len, &numChars);
		if (1 != len)
		    pSeg->type = _DtCvSetTypeToWideChar(pSeg->type);
	      }
	    else
		size = strlen(ptr);
    
	    /*
	     * set the link flag
	     */
	    if (link_index != -1)
	      {
		pSeg->type = _DtCvSetTypeToHyperText(pSeg->type);
		pSeg->link_idx = link_index;
	      }
    
	    tmpChar   = ptr[size];
	    ptr[size] = '\0';
    
	    /*
	     * save the string appropriately.
	     */
	    if (_DtCvIsSegWideChar(pSeg))
	      {
		wchar_t *pwcs;
		size_t   convSize;

		/*
		 * include the terminating byte in the conversion.
		 */
		numChars++;

		/*
		 * malloc the memory
		 */
		pwcs = (wchar_t *) malloc(sizeof(wchar_t) * numChars);
		if (NULL != pwcs)
		  {
		    /* convert */
		    convSize = mbstowcs(pwcs, string, ((size_t) numChars));

		    /* check to see if it converted everything */
		    if (convSize + 1 == numChars)
			_DtCvStringOfStringSeg(pSeg) = (void *) pwcs;
		    else
			free(pwcs);
		  }
	      }
	    else
	        _DtCvStringOfStringSeg(pSeg) = (void *) strdup(string);

	    if (NULL == _DtCvStringOfStringSeg(pSeg))
		return -1;

	    /*
	     * set the font
	     */
	    _DtCvFontOfStringSeg(pSeg)  = useFont;

	    /*
	     * setup the list pointers
	     */
	    if (*prev_seg != NULL)
		(*prev_seg)->next_disp = pSeg;
    
	    *prev_seg = pSeg;
    
	    _DtHelpCeAddSegToList(pSeg, seg_list, last_seg);
    
	    ptr[size] = tmpChar;
	    ptr      += size;
	    string    = ptr;
	  }
      }

    if (newline == True)
      {
	pSeg->type = _DtCvSetTypeToNewLine(pSeg->type);

	if (addToList == True)
	  {
	    if (*prev_seg != NULL)
	        (*prev_seg)->next_disp = pSeg;
	    *prev_seg = pSeg;
	    _DtHelpCeAddSegToList(pSeg, seg_list, last_seg);
	  }
      }

    return 0;
}

/******************************************************************************
 * Function:    _DtCvSegment *_DtHelpCeMatchSemanticStyle (CESDLSegment *toss,
 *						enum SDLClass clan,
 *						char *ssi);
 * Parameters:
 *
 * Returns:     Nothing.
 *
 *****************************************************************************/
_DtCvSegment *
_DtHelpCeMatchSemanticStyle (
    _DtCvSegment	*toss,
    SdlOption		 clan,
    int			 level,
    char		*ssi)
{
    _DtCvSegment		*pSeg;
    SDLTossInfo		*pEl;
    const ClassStyleMatrix *pClassStyle;

    /*
     * initalize the starting style.
     */
    pSeg = toss;
    pClassStyle = ClassToStyle;

    do
      {
        /*
         * figure out which element style this class is associated with
         */
        while (pClassStyle->start != SdlOptionBad && 
		!(pClassStyle->start <= clan && clan <= pClassStyle->end))
	    pClassStyle++;

        if (pClassStyle->start != SdlOptionBad)
	  {
            /*
             * skip those not matching the style this class belongs to.
	     * But don't go past the ones after, because they might match
	     * too.
             */
            while (pSeg != NULL &&
		_SdlSegPtrToTossType(pSeg) != pClassStyle->style &&
		_SdlSegPtrToTossType(pSeg) != pClassStyle[1].style)
	        pSeg = pSeg->next_seg;
    
            /*
             * the class matches the style, look for a specific class, ssi and
             * level match.
             */
            while (pSeg != NULL &&
		_SdlSegPtrToTossType(pSeg) == pClassStyle->style)
              {
	        pEl = _SdlSegPtrToTossInfo(pSeg);
	        if (_SdlTossInfoPtrClan(pEl) == clan &&
	            (_SdlTossInfoPtrRlevel(pEl) == -1 ||
				  _SdlTossInfoPtrRlevel(pEl) == level) &&
	            (_SdlTossInfoPtrSsi(pEl) == NULL ||
		       (ssi != NULL &&
		       _DtHelpCeStrCaseCmp(_SdlTossInfoPtrSsi(pEl), ssi) == 0)))
	            return pSeg;

	        /*
	         * skip to the next element
	         */
	        pSeg = pSeg->next_seg;
              }

	    /*
	     * went through those styles, try more
	     */
	    pClassStyle++;
          }
      } while (pClassStyle->start != SdlOptionBad);

    return NULL;
}

/******************************************************************************
 * Function:    int _DtHelpCeAllocateFontStruct (int num, _DtHelpFontHints *ret_ptr)
 *
 * Parameters:
 *
 * Returns      ptr to the allocated structure.
 *
 * errno Values:
 *
 * Purpose:     Allocate the number of structure specified
 *
 *****************************************************************************/
int
_DtHelpCeAllocateFontStruct (
    int   size,
    _DtHelpFontHints **ret_ptr)
{
    _DtHelpFontHints *font;

    font = (_DtHelpFontHints *) malloc (sizeof (_DtHelpFontHints) * size);
    if (NULL == font)
	return -1;

    *ret_ptr = font;
    return 0;
}

