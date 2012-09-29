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
/*******************************************************************************
**
**  cm_i18n.c
**
**  $TOG: cm_i18n.c /main/9 1998/04/01 17:26:31 mgreess $
**
**  RESTRICTED CONFIDENTIAL INFORMATION:
**
**  The information in this document is subject to special
**  restrictions in a confidential disclosure agreement between
**  HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
**  document outside HP, IBM, Sun, USL, SCO, or Univel without
**  Sun's specific written approval.  This document and all copies
**  and derivative works thereof must be returned or destroyed at
**  Sun's request.
**
**  Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
**
*******************************************************************************/

/*									*
 * (c) Copyright 1993, 1994 Hewlett-Packard Company			*
 * (c) Copyright 1993, 1994 International Business Machines Corp.	*
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.			*
 * (c) Copyright 1993, 1994 Novell, Inc. 				*
 */

#ifndef lint
#endif

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
/*

This include has been left out for the moment because of a 
definition of the symbol "single" in rtable4.h.  This 
should be fixed in the future.

#include <floatingpoint.h>
*/
#include <locale.h>
#include <sys/param.h>       /* for MAXPATHLEN */
#include <Xm/Xm.h>
#include "util.h"
#include "timeops.h"
#include "cm_i18n.h"
#include "deskset.h"
/*
#include "gettext.h"
*/

#include <langinfo.h>
/* Iconv not defined for linux.  Use the EUSCompat stubs instead. */
#if !defined(linux)
#  include <iconv.h>
#endif
#include <EUSCompat.h>

char *fontset1[2]; 
char *fontset2[2];
int use_octal = FALSE;


int 
is_comment(char line[MAX_LINE_LEN])
{
	char ch[2];

	strncpy(ch, line, 1);
	if ( strcmp((char *)ch, COMMENT_SYMBOL) == 0 ) {
		return 1;	
	} else {
		return 0;	
	}
}


int 
match_locale(char *locale, char line[MAX_LINE_LEN])
{
	char loc[MAX_LINE_LEN];	

	if ( !isalpha(line[0]) ) {
		return 0;
	}
	(void) sscanf(line, "%s", loc);
	if ( strcmp(loc, locale) == 0 ) {
		return 1;
	} else {
		return 0;
	}
}

/*
 * The following code was very similar to what was developped for dtmail's
 * conversion mechanism. See libDtMail/RFC/RFCMIME.C ! Have fun !
 */
#define WORKSIZE 1024
static void           *_i18nwork1 = NULL;
static unsigned long  _i18nsize1 = 0;
static int            shouldAlloc1 = ~0;
static int	      isFirstCall = ~0;
static iconv_t        CD = (iconv_t)-1;
static int	      amI_932 = ~0;

#ifdef ICONV_INBUF_CONST
# define ICONV_INBUF_TYPE	const char **
#else
# define ICONV_INBUF_TYPE	char **
#endif

void _converter_( void *from, unsigned long from_len,
			void **to,  unsigned long *to_len )
{
    char          *InBuf;
    size_t        InBytesLeft;
    char          *OutBuf = NULL;
    size_t        OutBytesLeft = 0;
    size_t        _OutBytesLeft = 0;
    size_t        iconv_ret;
    size_t        converted_num = 0;

    *to = NULL;
    *to_len = 0;

    if ( shouldAlloc1 ) {
        /* Obtain work area */
        _i18nwork1 = (size_t *)malloc( WORKSIZE );
        if ( !_i18nwork1 ) {
            _i18nwork1 = NULL;
            return;
        }
        _i18nsize1 = WORKSIZE; 
        shouldAlloc1 = 0;
    }
#ifdef _AIX
    if ( isFirstCall ) {
	if ( ( CD = iconv_open( "IBM-eucJP", "IBM-932" ) ) == (iconv_t)-1 )
	    return; /* no converter */
	amI_932 = !strncasecmp( "IBM-932", nl_langinfo( CODESET ), 7 );
	isFirstCall = 0;
    }
#endif /* _AIX */

    if ( ( !amI_932 ) || ( CD == (iconv_t)-1 ) )
	return;

    InBuf        = (char *)from;
    InBytesLeft  = from_len;
    OutBytesLeft = _i18nsize1;
    OutBuf = (char *)_i18nwork1;

    while( 1 ) {
	/*
	 * InBuf
	 *  v
	 * +----------------------------+
	 * | |                        | |
	 * +----------------------------+
	 *  <-------------------------->
	 *          InBytesLeft
	 *
	 *             |
	 *             | iconv()
	 *             V
	 * (_i18nwork1)
	 * OutBuf
	 *  v
	 * +----------------------------+
	 * | |                        | |
	 * +----------------------------+
	 *  <-------------------------->
	 *          InBytesLeft
	 */

	iconv_ret = iconv( CD, (ICONV_INBUF_TYPE)&InBuf, &InBytesLeft,
                               &OutBuf, &OutBytesLeft );
	if ( iconv_ret == 0 ) {
	    /* iconv done
	     *                             InBuf
	     *                               v
	     * +----------------------------+
	     * |XXXXXXXXXXXXXXXXXXXXXXXXXXXX|
	     * +----------------------------+
	     *                               
	     *                               InBytesLeft=0
	     *
	     * (_i18nwork1)
	     *  |               OutBuf
	     *  V                 v
	     * +----------------------------+
	     * |XXXXXXXXXXXXXXXXX| |      | |
	     * +----------------------------+
	     *  <---------------> <-------->
	     *   converted_num    OutBytesLeft
	     */
	    converted_num = (unsigned long)((char *)OutBuf-(char *)_i18nwork1);
	    *to = (void *)_i18nwork1;
	    *to_len = (unsigned long)converted_num;
	    break;
	} else {
	    if ( errno == E2BIG ) {
		/* Overflow. still data is left.
		 *               InBuf
		 *                 v
		 * +----------------------------+
		 * |XXXXXXXXXXXXXX| |         | |
		 * +----------------------------+
		 *                 <----------->
		 *                  InBytesLeft
		 *
		 * (_i18nwork1)
		 *  |                         OutBuf
		 *  V                          v
		 * +----------------------------+
		 * |XXXXXXXXXXXXXXXXXXXXXXXXXXX |
		 * +----------------------------+
		 *  <-------------------------> 
		 *          converted_num      OutBytesLeft=?
		 */
		void *_p;

		/* Check how many converted already. */

		converted_num =
			(unsigned long)((char *)OutBuf - (char *)_i18nwork1);
		_i18nsize1 += WORKSIZE;
		_p = realloc( _i18nwork1, _i18nsize1 );
		if ( !_p ) {
		    *to = NULL;
		    *to_len = 0;
		    free( _i18nwork1 );
		    _i18nwork1 = NULL;
		    _i18nsize1 = 0;
		    shouldAlloc1 = ~0;
		    break;
		} else {
		    _i18nwork1 = _p;
		    OutBuf = (char *)((char*)_i18nwork1 + converted_num);
		    OutBytesLeft += WORKSIZE;
		}  
	    } else {
		*to = NULL;
		*to_len = 0;
		break;
	    }
	}
    }

    /*
     * NULL terminate
     */
    if ( _i18nsize1 > converted_num ) {
	((char *)_i18nwork1)[converted_num] = '\0';
    } else { /* _i18nsize1 == converted_num */
	void *_p;

	_i18nsize1++;
	_p = realloc( _i18nwork1, _i18nsize1 );

	if ( !_p ) {
	    *to = NULL;
	    *to_len = 0;
	    free( _i18nwork1 );
	    _i18nwork1 = NULL;
	    _i18nsize1 = 0;
	    shouldAlloc1 = ~0;
	} else {
	    _i18nwork1 = _p;
	    ((char *)_i18nwork1)[converted_num] = '\0';
	}
    }
}

/* The following routine is specific to using FMapType 3 composite fonts
 * in postscript.  Kanji, Asian specific?
 */
char *
euc_to_octal(char *srcStr)
{
	int inKanji = FALSE;
	char buf[64];
	static char dstStr[512];
	int i;
	int len = cm_strlen(srcStr);

#ifdef SVR4
	memset(dstStr, 0, sizeof(dstStr));
#else
	bzero(dstStr, sizeof(dstStr));
#endif /* SVR4 */
	for (i = 0; i < len; i++) {
		if (inKanji) {
			if (!isEUC(srcStr[i])) {
				inKanji = FALSE;
				/* NOT NEEDED FOR FMapType 4 (or 5)
				cm_strcat(dstStr, "\\377\\000");
				*/
			}
		}
		else {
			if (isEUC(srcStr[i])) {
				inKanji = TRUE;
				/* NOT NEEDED FOR FMapType 4 (or 5)
				cm_strcat(dstStr, "\\377\\001");
				*/
			}
		}
		if (inKanji) {
			sprintf(buf, "\\%3.3o\\%3.3o", srcStr[i] & 0xff, srcStr[i+1] & 0xff);
			i++;
		}
		else {
			sprintf(buf, "%c", srcStr[i]);
		}
		cm_strcat(dstStr, buf);
	}
	return dstStr;
}


/* This routine should be in libdeskset.
 * This routine uses fconvert() to avoid locale conversion.
 */
/* 310 characters are the minimum needed to accommodate any double-precision
 * value + 1 null terminator.
 */
#define DBL_SIZE  311
/*
 *  Returns a null terminated formatted string.
 *  If error is encountered, such as malloc() failed, then return NULL.
 *  The caller of this function should beware that the return value is
 *  a static buffer declared within this function and the value of it may
 *  change.
 */
char *
cm_printf(double value, int decimal_pt)
{
	int sign = 0;
	int deci_pt = 0;
	int buf_cnt = 0;
	int formatted_cnt = 0;
	int buf_len = 0;
	char *buf = NULL;
	static char *formatted = NULL;

	if ( formatted != NULL ) {
		free(formatted);
		formatted = NULL;
	}
	if ( (value == (double)0) && (decimal_pt == 0) ) {
		formatted = (char *)cm_strdup("0");
		return formatted;
	}
	if ( (buf = (char *)malloc(DBL_SIZE + decimal_pt)) == NULL ) {
		return (char *)NULL;
	}
	if ( (formatted = (char *)calloc(1, DBL_SIZE + decimal_pt)) == NULL ) {
		free(buf);
		return (char *)NULL;
	}
#ifdef SunOS
	fconvert(value, decimal_pt, &deci_pt, &sign, buf);
#elif defined(CSRG_BASED)
	snprintf(buf, decimal_pt, "%f", value);
#else
	/* this version, available on the HP and AIX machine is not reentrant. */

	strcpy(buf, fcvt(value, decimal_pt, &deci_pt, &sign));
#endif
	if ( sign ) {
		strcpy(formatted, "-");
	}
	buf_len = deci_pt + decimal_pt;
	if ( deci_pt ) {
		strncat(formatted, buf, deci_pt);
	} else {    /* zero */
		strcat(formatted, "0");	
	}
	if ( deci_pt == buf_len ) {
		strcat(formatted, "\0");
		free(buf);
		return formatted;
	}
	strcat(formatted, ".");
	for ( formatted_cnt = strlen(formatted), buf_cnt = deci_pt;  buf_cnt < buf_len;  buf_cnt++, formatted_cnt++ ) {
		formatted[formatted_cnt] = buf[buf_cnt];
	}
	formatted[formatted_cnt] = '\0';
	free(buf);
	return formatted;	
}
