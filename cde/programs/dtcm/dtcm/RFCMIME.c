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
static char sccsid[] = "$TOG: RFCMIME.c /main/11 1999/06/30 12:08:55 mgreess $";
/*
 *   COMPONENT_NAME: desktop
 *
 *   FUNCTIONS: CvtStr
 *		DtXlateOpToStdLocale
 *		DtXlateStdToOpLocale
 *		_converter_
 *		base64size
 *		crlf
 *		getCharSet
 *		getEncodingType
 *		mbisspace
 *		md5PlainText
 *		rfc1522cpy
 *		targetTagName
 *		writeBase64
 *		writeContentHeaders
 *		writeQPrint
 *
 *   ORIGINS: 119
 *
 *   OBJECT CODE ONLY SOURCE MATERIALS
 */

#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <md5.h>
#define _ILS_MACROS
#include <ctype.h>
#include <assert.h>
#include <RFCMIME.h>

#include <iconv.h>
#include <EUSCompat.h>
#include <locale.h>
#include <LocaleXlate.h>

#ifdef ICONV_INBUF_CONST
# define ICONV_INBUF_TYPE	const char **
#else
# define ICONV_INBUF_TYPE	char **
#endif

#define WORKSIZE 1024*10	
/*
 * The following escape sequence is defined as "To ASCII".
 * But is it correct regardless of ISO-2022-XX ???
 */
#define ToASCII_NUM 3
static char ToASCII[ToASCII_NUM] = { 0x1b, 0x28, 0x42 };

/*
 * _i18nwork1[] is used to convert the passed string with CD iconv.
 * in _converter_().
 *
 */
static void           *_i18nwork1 = NULL;
static unsigned long  _i18nsize1 = 0;
static int            shouldAlloc1 = ~0;

/*
 * _i18nwork2[] is used to convert the passed string with CD iconv.
 * in CvtStr().
 *
 */
static void           *_i18nwork2 = NULL;
static unsigned long  _i18nsize2 = 0;
static int            shouldAlloc2 = ~0;

/*
 * Forward declarations
 */
extern void writeBase64(char * buf, const char * bp, const unsigned long len);

static const char *DfltStdCharset = "us-ascii";
static const char *DfltStdLang = "C";

static void crlf(char *buf)
{
	strcat(buf,"\n");
}

/******************************************************************************
 * Function:    int DtXlateOpToStdLocale (char *operation, char *opLocale,
 *                         char **ret_stdLocale, char **ret_stdLang, char **ret_stdSet)
 *
 * Parameters:
 *              operation       Operation associated with the locale value
 *              opLocale	An operation-specific locale string
 *              ret_locale      Returns the std locale
 *				Caller must free this string.
 *		ret_stdLang        Returns the std language & territory string.
 *				Caller must free this string.
 *              ret_stdSet         Returns the std code set string.
 *				Caller must free this string.
 *
 * Return Value:
 *
 * Purpose:  Gets the standard locale given an operation and its locale
 *
 *****************************************************************************/
void
DtXlateOpToStdLocale (
     char       *operation,
     char       *opLocale,
     char       **ret_stdLocale,
     char       **ret_stdLang,
     char       **ret_stdSet)
{
     _DtXlateDb MyDb = NULL;
     char MyPlatform[_DtPLATFORM_MAX_LEN + 1];
     int	ExecVer;
     int	CompVer;	

     if (_DtLcxOpenAllDbs(&MyDb) == 0 &&
         _DtXlateGetXlateEnv(MyDb,MyPlatform,&ExecVer,&CompVer) != 0)
     {
          _DtLcxCloseDb(&MyDb);
          MyDb = NULL;
     }


    if (MyDb != NULL)	
	{
	(void) _DtLcxXlateOpToStd(MyDb, MyPlatform, ExecVer,
				operation,opLocale,
				ret_stdLocale, ret_stdLang, ret_stdSet, NULL);
	}

    /* if failed, give default values */
    if (ret_stdLocale != NULL && *ret_stdLocale == NULL)
    {
        *ret_stdLocale = (char *)malloc(strlen(DfltStdLang)+strlen(DfltStdCharset)+3);
	sprintf(*ret_stdLocale,"%s.%s",DfltStdLang,DfltStdCharset);
    }
    if (ret_stdLang != NULL && *ret_stdLang == NULL)
	*ret_stdLang = (char *)strdup(DfltStdLang);
    if (ret_stdSet != NULL && *ret_stdSet == NULL)
	*ret_stdSet = (char *)strdup(DfltStdCharset);
}

/******************************************************************************
 * Function:    int DtXlateStdToOpLocale ( char *operation, char *stdLocale,
 *					char *stdLang, char *stdCodeSet,
 *                                      char *dflt_opLocale, char **ret_opLocale)
 *
 * Parameters:
 *    operation		operation whose locale value will be retrieved
 *    stdLocale		standard locale value
 *    stdLang		standard Lang/Territory Value
 *    stdCodeSet	standard CodeSet Value
 *    dflt_opLocale	operation-specific locale-value
 *			This is the default value used in error case
 *    ret_opLocale	operation-specific locale-value placed here
 *			Caller must free this string.
 *
 * Return Value:
 *
 * Purpose: Gets an operation-specific locale string given the standard string
 *
 *****************************************************************************/
void
DtXlateStdToOpLocale (
     char       *operation,
     char       *stdLocale,
     char       *stdLang,
     char       *stdCodeSet,
     char       *dflt_opLocale,
     char       **ret_opLocale)
{
     _DtXlateDb MyDb = NULL;
     char MyPlatform[_DtPLATFORM_MAX_LEN + 1];
     int        ExecVer;
     int        CompVer;

     if (_DtLcxOpenAllDbs(&MyDb) == 0 &&
         _DtXlateGetXlateEnv(MyDb,MyPlatform,&ExecVer,&CompVer) != 0)
     {
          _DtLcxCloseDb(&MyDb);
          MyDb = NULL;
     }


    if (MyDb != NULL)
    {
	(void) _DtLcxXlateStdToOp(MyDb, MyPlatform, ExecVer,
			operation, stdLocale, stdLang, stdCodeSet, NULL,
			ret_opLocale);
    }

    /* if translation fails, use a default value */
    if (ret_opLocale && *ret_opLocale == NULL)
    {
       if (dflt_opLocale) *ret_opLocale = (char *)strdup(dflt_opLocale);
       else if (stdLocale) *ret_opLocale = (char *)strdup(stdLocale);
    }
}
char *
targetTagName(void)
{
   char *ret_locale = NULL;
   char *ret_lang = NULL;
   char *ret_codeset = NULL;
   char *ret_target = NULL;

   DtXlateOpToStdLocale(DtLCX_OPER_SETLOCALE,
          setlocale(LC_CTYPE, NULL),
          &ret_locale,
          &ret_lang,
          &ret_codeset);
   DtXlateStdToOpLocale(DtLCX_OPER_INTERCHANGE_CODESET,
          NULL,
          NULL,
          ret_codeset,
          NULL,
          &ret_target);
   DtXlateStdToOpLocale(DtLCX_OPER_MIME,
          NULL,
          NULL,
          ret_target,
          NULL,
          &ret_codeset);

   free(ret_locale);
   free(ret_target);
   free(ret_lang);

   return ret_codeset;
}

void
getCharSet(char * charset)
{
        char *mimeCS = NULL;

     mimeCS = targetTagName();

    if (mimeCS) {
       strcpy(charset, mimeCS);
       free(mimeCS);
    } else {
           strcpy(charset, "us-ascii");   /* default MIME codeset */
    }
}


void
md5PlainText(const char * bp, const unsigned long len, unsigned char * digest)
{
    /* We need to compute the md5 signature based on a message that has
    // the CRLF line terminator. Most of our buffers don't so we will need
    // to scan the body and do some magic. The approach will be to sum
    // one line at a time. If the buffer doesn't have CRLF we will do that
    // independently.
    */

    MD5_CTX context;
    unsigned char * local_crlf = (unsigned char *)"\r\n";
    const char * last = bp;
    const char * cur;

    MD5Init(&context);
    for (cur = bp; cur < (bp + len); cur++) {
	if (*cur == '\n') {
	    if (cur == bp || *(cur - 1) == '\r') {
		MD5Update(&context, (unsigned char *)last,
			  cur - last + 1);
	    }
	    else {
		MD5Update(&context, (unsigned char *)last,
			  cur - last);
		MD5Update(&context, local_crlf, 2);
	    }
	    last = cur + 1;
	}
    }

    if (bp[len - 1] != '\n') {
	/* Need to sum the trailing fraction with a CRLF. */
	MD5Update(&context, (unsigned char *)last,
		  cur - last);
	MD5Update(&context, local_crlf, 2);
    }

    MD5Final(digest, &context);
}


static void _converter_( iconv_t CD,
			void *from, unsigned long from_len,
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

    InBuf        = (char *)from;
    InBytesLeft  = from_len;
    OutBytesLeft = _i18nsize1;
    OutBuf = (char *)_i18nwork1;

    /*
     * Need to place iconv state to the initial one by
     * setting inbuf to NULL of iconv().
     */
    iconv( CD, (ICONV_INBUF_TYPE)NULL, 0, NULL, 0 );
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

		/* Check how many converted already.
		*/
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
}


int
CvtStr( char *charSet, void *from, unsigned long from_len,
                 void **to, unsigned long *to_len, Direction dir )
{
    char	*ret_locale = NULL;
    char	*ret_lang = NULL;
    char	*ret_codeset = NULL;
    char	*from_codeset = NULL;
    char	*to_codeset = NULL;
    char	*CuStdCodeSet = NULL;
    char	*InterChCodeSet = NULL;
    char	*StdCodeSet = NULL;
    iconv_t	CD;
    int		isASCII=~0;
    int		isStopASCII = ~0;
    unsigned long converted_num = 0;

/* Get CuStdCodeSet */
    DtXlateOpToStdLocale( DtLCX_OPER_SETLOCALE,
                          setlocale( LC_CTYPE, NULL ),
                          &ret_locale,
                          &ret_lang,
                          &CuStdCodeSet );

/*
 * If charSet is NULL, it means the passed string's charset in *from is
 * unknown by dtmail. In this case, this converter assumes that 
 * when dir = CURRENT_TO_INTERNET,
 *     *from's encoding is the current locale's one.
 * when dir = INTERNET_TO_CURRENT,
 *     *from's encoding is the current locale's Internet Message's one.
 *
 * Example.
 *   dtmail is running under ja_JP locale.
 *    dir : CURRENT_TO_INTERNET
 *         *from = IBM-eucJP
 *         *to   = ISO-2022-JP
 *    dir : INTERNET_TO_CURRENT
 *         *from = ISO-2022-JP
 *         *to   = IBM-eucJP
 */
/*
 * ISO-2022-JP can be converted to either EUC-JP or IBM-932 practically.
 * But the current AIX.lcx says 
 *       StdCodeSet      InterchangeCodeset
 *         EUC-JP   <-->   ISO-2022-JP
 *         IBM-932  --->   ISO-2022-JP
 *         HP-SJIS  --->   ISO-2022-JP
 *         HP-KANA8 --->   ISO-2022-JP
 * therefore DtXlateOpToStdLocale() can convert ISO-2022-JP to EUC-JP only.
 * To fix this, we hard-code'ed this situation with the CDE Standard Name
 *
 * ???? Is it correct ???
 */
    if ( dir == INTERNET_TO_CURRENT ) {
	/*
	 * As for from_codeset
	 */
	if ( ( charSet == NULL ) || ( *charSet == '\0' ) ) {
	    /* Convert CuStdCodeSet to StdInterChangeCodeSet */
	    DtXlateStdToOpLocale( DtLCX_OPER_INTERCHANGE_CODESET,
				NULL,
				NULL,
				CuStdCodeSet,
				NULL,
				&InterChCodeSet );
	} else {
	    /* Convert charSet to StdInterChangeCodeSet */
	    free(ret_locale);
	    free(ret_lang);
	    free(ret_codeset);
	    ret_locale = ret_lang = ret_codeset = NULL;
	    DtXlateOpToStdLocale( DtLCX_OPER_MIME,
				charSet,
				&ret_locale,
				&ret_lang,
				&InterChCodeSet );
	}

	/* Convert StdInterChangeCodeSet to OpIVONC3 codeset */
	DtXlateStdToOpLocale( DtLCX_OPER_ICONV3,
				NULL,
				NULL,
				InterChCodeSet,
				NULL,
				&from_codeset );
	/*
	 * As for to_codeset
	 */
	if ( ( charSet == NULL ) || ( *charSet == '\0' ) ) {
	    /* Convert CuStdCodeSet to OpIVONC3 codeset */
	    DtXlateStdToOpLocale( DtLCX_OPER_ICONV3,
				NULL,
				NULL,
				CuStdCodeSet,
				NULL,
				&to_codeset );
	} else {
#ifdef _AIX
	    if ( (!strncasecmp(InterChCodeSet,"ISO-2022-JP",11) &&
		  !strncasecmp(CuStdCodeSet,"IBM-932",7)        ) ||
		 (!strncasecmp(InterChCodeSet,"ISO-2022-JP",11) &&
		  !strncasecmp(CuStdCodeSet,"EUC-JP",6)         )    ) {
		ret_codeset = CuStdCodeSet;
	    } else 
#endif /* _AIX */
	    {
		/* Convert InterChCodeSet to StdCodeSet */
		free(ret_locale);
		free(ret_lang);
		free(ret_codeset);
		ret_locale = ret_lang = ret_codeset = NULL;
		DtXlateOpToStdLocale( DtLCX_OPER_INTERCHANGE_CODESET,
				InterChCodeSet,
				&ret_locale,
				&ret_lang,
				&ret_codeset );
	    }
	    DtXlateStdToOpLocale( DtLCX_OPER_ICONV3,
				NULL,
				NULL,
				ret_codeset,
				NULL,
				&to_codeset );
	}
    } else { /* dir == CURRENT_TO_INTERNET */
	/*
	 * As for from_codeset
	 */
	if ( ( charSet == NULL ) || ( *charSet == '\0' ) ) {
	    /* Convert CuStdCodeSet to OpICONV3 codeset */
	    DtXlateStdToOpLocale( DtLCX_OPER_ICONV3,
				NULL,
				NULL,
				CuStdCodeSet,
				NULL,
				&from_codeset );
	} else {
	    /* Convert charSet to StdInterChangeCodeSet */
	    free(ret_locale);
	    free(ret_lang);
	    free(ret_codeset);
	    ret_locale = ret_lang = ret_codeset = NULL;
	    DtXlateOpToStdLocale( DtLCX_OPER_MIME,
				charSet,
				&ret_locale,
				&ret_lang,
				&ret_codeset );
	    /* Convert StdInterChangeCodeSet to OpIVONC3 codeset */
	    DtXlateStdToOpLocale( DtLCX_OPER_ICONV3,
				NULL,
				NULL,
				ret_codeset,
				NULL,
				&from_codeset );
	}
	/*
	 * As for to_codeset
	 */
	if ( ( charSet == NULL ) || ( *charSet == '\0' ) ) {
	    /* Convert CuStdCodeSet to StdInterChangeCodeSet */
	    DtXlateStdToOpLocale( DtLCX_OPER_INTERCHANGE_CODESET,
				NULL,
				NULL,
				CuStdCodeSet,
				NULL,
				&InterChCodeSet );
	} else {
	    /* Convert charSet to StdInterChangeCodeSet */
	    free(ret_locale);
	    free(ret_lang);
	    free(ret_codeset);
	    ret_locale = ret_lang = ret_codeset = NULL;
	    DtXlateOpToStdLocale( DtLCX_OPER_MIME,
				charSet,
				&ret_locale,
				&ret_lang,
				&InterChCodeSet );
	}

	/* Convert StdInterChangeCodeSet to OpIVONC3 codeset */
	DtXlateStdToOpLocale( DtLCX_OPER_ICONV3,
				NULL,
				NULL,
				InterChCodeSet,
				NULL,
				&to_codeset );
    }

    free(ret_locale);
    free(ret_lang);
    free(ret_codeset);

    *to = NULL;
    *to_len = 0;

    if ( shouldAlloc2 ) {
        /* Obtain work area */
        _i18nwork2 = (size_t *)malloc( WORKSIZE );
        if ( !_i18nwork2 ) {
            _i18nwork2 = NULL;
            return( isASCII );
        }
        _i18nsize2 = WORKSIZE; 
        shouldAlloc2 = 0;
    }

    if (NULL == to_codeset || NULL == from_codeset)
      return( isASCII );

    if ( ( CD = iconv_open( to_codeset, from_codeset ) ) != (iconv_t)-1 ) {
	/*
	 * According to several RFCs( 822, 1468, 1557, ..... )
	 * the escape sequence to switch to ASCII is needed just before
	 * '\n'. IBM-eucJP/IBM-932 <--> fold 7 does while the other doesn't.
	 * Therefore CvtStr() does take care of this here.
	 */
	if ( dir == INTERNET_TO_CURRENT ) {
	    _converter_( CD, from, from_len, to, to_len );
	} else {
	    void *new_from = from;
	    unsigned long new_from_len = from_len;
	    unsigned long _passed = 0;
	    size_t clen;
	    void *_tmp = NULL;
	    unsigned long _tmp_len = 0;

	    while ( _passed < from_len ) {
		/*
		 * Find \n or \0
		 */
		for ( ; _passed < from_len; _passed += clen ) {
		    clen = mblen(&(((char *)from)[_passed]), MB_CUR_MAX);
		    if ( clen < 0 )
			break;

		    if ( ( clen > 1 ) || !isascii( ((char*)from)[_passed] ) ){
			/* Here, maybe MB or non-ASCII */
			isASCII = 0;
			isStopASCII = 0;
		    } else {
			if ( ( ((char*)from)[_passed] != '\n' ) &&
			     ( ((char*)from)[_passed] != '\0' )    ) {
			    isStopASCII = ~0;
			}
		    }

		    if ( ((char*)from)[_passed] == '\n' ||
		         ((char*)from)[_passed] == '\0'    )
			break;
		}
                new_from_len = &(((char *)from)[_passed])-(char *)new_from;
                if ( ( _passed < from_len ) && ( clen == 1 ) &&
                     ( ((char*)from)[_passed] == '\n' ) ) {
                    new_from_len++;
                }

		/*
		 * new_from                 from[_passed]
		 *  V                        V
		 * +------------------------+--+------.................+
		 * | |                      |\n| |                     |
		 * +------------------------+--+------.................+
		 *  <-------------------------> $
		 *     new_from_len             next new_from
		 *  <------------------------------------------------->
		 *  $                 from_len
		 * from 
		 */

		/*
		 *      ********** DO 1 LINE CONVERSION **********
		 */
		_tmp = NULL; _tmp_len = 0;
		_converter_( CD, new_from, new_from_len, &_tmp, &_tmp_len );

		if ( ( _tmp == NULL ) && ( _tmp_len == 0 ) ) {
		    /* Conversion fail */
		    *to = NULL;
		    *to_len = 0;
		    break;
		}

		/*
		 * _i18nwork2                 _tmp
		 *  V                           V
		 * +-----------------------+   +-------------+  
		 * |XXXXXXXX               | + |             |
		 * +-----------------------+   +-------------+  
		 *  <------>
		 *  converted_num
		 *  <--------------------->     <----------->
		 *   _18nsize2                    _tmp_len
		 */
		/* Append _tmp to target */
		if ( converted_num + _tmp_len > _i18nsize2 ) {
		    /* Need much memory..... */
		    void *_i18n = NULL;

		    _i18nsize2 += WORKSIZE;
		    _i18n = realloc( _i18nwork2, _i18nsize2  ); 
		    if ( !_i18n ) {
			*to = NULL;
			*to_len = 0;
			_i18nwork2 = NULL;
			_i18nsize2 = 0;
			shouldAlloc2 = ~0;
			break;
		    } else {
			_i18nwork2 = _i18n;
		    }
		}

		/*
		 * _i18nwork2  _tmp
		 *  V          v
		 * +---------------------------+  
		 * |XXXXXXXXXXX(COPIED)XX      |
		 * +---------------------------+  
		 *  <---------><-------->
		 *  (old)conv.  _tmp_len
		 *  <------------------->
		 *  (new)converted_num
		 *  <--------------------------> 
		 *        _i18nsize2
		 */
		strncpy( (char *)_i18nwork2 + converted_num,
				(char *)_tmp, _tmp_len );
		converted_num += _tmp_len;

		*to = (void *)_i18nwork2;
		*to_len = converted_num;
		new_from =  &(((char *)from)[_passed]) + 1;
		_passed++;

		/*
		 * According to RFC1468, if the line is ended with non-ASCII
		 * char, but not not switch to ASCII before the end of line,
		 * we must switch to ASCII just before the end of the line.
		 *
		 * _i18nwork2                     ToASCII
		 *  V                           ??? V
		 * +---------------------------+   +------+
		 * |XXXXXXXXXXXXXXXXXXXXX      | + |1b2842|
		 * +---------------------------+   +------+
		 *  <------------------->           <---->
		 *  converted_num                   ToASCII_NUM
		 *  <------------------------->
		 *        _i18nsize2
		 *                ========
		 *                1b 28 42 ??
		 *
		 */
		if ( !isStopASCII ) {
		    if ( (((char *)_i18nwork2)[converted_num-1] == '\n') ||
		         (((char *)_i18nwork2)[converted_num-1] == '\0')    ) {

			char _p = ((char *)_i18nwork2)[converted_num-1];

			if (!((converted_num >=3+1 ) &&
	       		 !memcmp((void *)((char *)_i18nwork2+converted_num-3-1),
				ToASCII, ToASCII_NUM ))                      ){
			    /* if not ToASCII there, then */
			    /* Append ToASCII */

			    if ( converted_num + ToASCII_NUM > _i18nsize2 ) {
				/* Need much memory..... */
				void *_i18n = NULL;

				_i18nsize2 += WORKSIZE;
				_i18n=realloc(_i18nwork2,_i18nsize2); 
				if ( !_i18n ) {
				    *to = NULL;
				    *to_len = 0;
				    shouldAlloc2 = ~0;
				    break;
				} else {
				    _i18nwork2 = _i18n;
				}
			    }
			    strncpy( (char *)_i18nwork2+converted_num-1,
					ToASCII, ToASCII_NUM );
			    converted_num += ToASCII_NUM;
			    ((char *)_i18nwork2)[converted_num-1] = _p;
			    *to = _i18nwork2;
			    *to_len = converted_num;
			    isStopASCII = ~0;
			}
		    } else {
			if (!((converted_num >=3 ) &&
	       		 !memcmp((void *)((char *)_i18nwork2+converted_num-3),
				ToASCII, ToASCII_NUM ))                      ){
			    /*
			    // if not ToASCII there, then
			    // Append ToASCII
			    */
			    if ( converted_num + ToASCII_NUM > _i18nsize2 ) {
				void *_i18n = NULL;

				_i18nsize2 += WORKSIZE;
				_i18n=realloc(_i18nwork2,_i18nsize2); 
				if ( !_i18n ) {
				    *to = NULL;
				    *to_len = 0;
				    shouldAlloc2 = ~0;
				    break;
				} else {
				    _i18nwork2 = _i18n;
				}
			    }
			    strncpy( (char *)_i18nwork2+converted_num,
					ToASCII, ToASCII_NUM );
			    converted_num += ToASCII_NUM;
			    *to = _i18nwork2;
			    *to_len = converted_num;
			    isStopASCII = ~0;
			}
		    }
		}
	    }
	    /*
	     * Again........
	     */
	    if( ( *to != NULL ) && ( *to_len != 0 ) ) {
		if ( !isStopASCII ) {
		    if ( (((char *)_i18nwork2)[converted_num-1] == '\n') ||
		         (((char *)_i18nwork2)[converted_num-1] == '\0')    ) {

			char _p = ((char *)_i18nwork2)[converted_num-1];

			if (!((converted_num >=3+1 ) &&
	       		 !memcmp((void *)((char *)_i18nwork2+converted_num-3-1),
				ToASCII, ToASCII_NUM ))                      ){
			    /* if not ToASCII there, then
			    // Append ToASCII
			    */

			    if ( converted_num + ToASCII_NUM > _i18nsize2 ) {
				void *_i18n = NULL;

				_i18nsize2 += WORKSIZE;
				_i18n=realloc(_i18nwork2,_i18nsize2); 
				if ( !_i18n ) {
				    *to = NULL;
				    *to_len = 0;
				    shouldAlloc2 = ~0;
				} else {
				    _i18nwork2 = _i18n;
				}
			    }
			    strncpy( (char *)_i18nwork2+converted_num-1,
					ToASCII, ToASCII_NUM );
			    converted_num += ToASCII_NUM;
			    ((char *)_i18nwork2)[converted_num-1] = _p;
			    *to = _i18nwork2;
			    *to_len = converted_num;
			    isStopASCII = ~0;
			}
		    } else {
			if (!((converted_num >=3 ) &&
	       		 !memcmp((void *)((char *)_i18nwork2+converted_num-3),
				ToASCII, ToASCII_NUM ))                      ){
			    /* if not ToASCII there, then
			    // Append ToASCII
			    */

			    if ( converted_num + ToASCII_NUM > _i18nsize2 ) {
				void *_i18n = NULL;

				_i18nsize2 += WORKSIZE;
				_i18n=realloc(_i18nwork2,_i18nsize2); 
				if ( !_i18n ) {
				    *to = NULL;
				    *to_len = 0;
				    shouldAlloc2 = ~0;
				} else {
				    _i18nwork2 = _i18n;
				}
			    }
			    strncpy( (char *)_i18nwork2+converted_num,
					ToASCII, ToASCII_NUM );
			    converted_num += ToASCII_NUM;
			    *to = _i18nwork2;
			    *to_len = converted_num;
			    isStopASCII = ~0;
			}
		    }
		}
	    }
	    /*
	     * If InterChCodeSet is either ISO-2022-JP/ISO-2022-KR/ISO-2022-TW
	     * ISO-2022-CN, strip MSB here since iconv'ed UDC has MSB set to 1
	     */
	    if ( !strncasecmp( InterChCodeSet, "ISO-2022-JP", 11 ) ||
		 !strncasecmp( InterChCodeSet, "ISO-2022-KR", 11 ) ||
		 !strncasecmp( InterChCodeSet, "ISO-2022-TW", 11 ) ||
		 !strncasecmp( InterChCodeSet, "ISO-2022-CN", 11 )   ) {
		int _i;

		for ( _i = 0; _i < *to_len; _i++ ) {
			((unsigned char *)(*to))[_i] &= (unsigned char)0x7f;
		}
	    }
	}
        iconv_close( CD );
    }
    return( isASCII );
}
unsigned int
base64size(const unsigned long len)
{
    int b_len = len + (len / 3);
    b_len += (b_len / 72 * 2) + 4;

    return(b_len);
}

getEncodingType(const char * body,
                         const unsigned int len,
                         boolean_t strict_mime)
{
    /*
    // Our goal here is to produce the most readable, safe encoding.
    // We have a couple of parameters that will guide our
    // choices:
    //
    // 1) RFC822 allows lines to be a minimum of 1000 characters,
    //    but MIME encourages mailers to keep lines to <76 characters
    //    and use quoted-printable if necessary to achieve this.
    //
    // 2) The base64 encoding will grow the body size by 33%, and
    //    also render it unreadable by humans. We don't want to use
    //    it unless really necessary.
    //
    // Given the above 2 rules, we want to scan the body part and
    // select an encoding. The 3 choices will be decided by:
    //
    // 1) If the text is 7 bit clean, and all lines are <76 chars,
    //    then no encoding will be applied.
    //
    //
    // 2) If the text is not 7 bit clean, or there are lines >76 chars,
    //    and the quoted-printable size is less than the base64 size,
    //    then quoted-printable will be done.
    //
    // 3) If 1 & 2 are not true, then base64 will be applied.
    //
    // If "strict_mime" is false we will only encode if the message
    // is not 7 bit clean.
    */


    int base64_growth = base64size(len) - len;
    int qprint_growth = 0;
    boolean_t eight_bit = B_FALSE;
    boolean_t base64 = B_FALSE;
    boolean_t encode = B_FALSE;
    Encoding	enc;

    const char * last_nl = body;
    const char * cur;

    if (body == NULL || len == 0) {
        return(MIME_7BIT);
    }
    if (strncmp(body, "From ", 5) == 0) {
        qprint_growth += 2;
    }

    for (cur = body; cur < (body + len); cur++) {
        if (*cur != (*cur & 0x7f)) {
            eight_bit = B_TRUE;
            encode = B_TRUE;
            qprint_growth += 2;
        } else if (*cur == '=' || *cur == 0) {
	    /*
            // These characters don't force encoding, but will be
            // encoded if we end up encoding.
            qprint_growth += 2;
	    */
        }

        if (*cur == '\n') {
            if ((cur - last_nl) > 76) {
                encode = B_TRUE;
                qprint_growth += 2;
            }


            if ((cur != body && (*(cur - 1) == ' ' || *(cur - 1) == '\t'))) {
                encode = B_TRUE;
                qprint_growth += 2;
            }

            if ((cur + 6) < (body + len) &&
                                        strncmp((cur + 1), "From ", 5) == 0) {
                encode = B_TRUE;
                qprint_growth += 2;
            }

            last_nl = cur + 1;
        }

        if (encode && (qprint_growth > base64_growth)) {
            base64 = B_TRUE;
            break;
        }
    }

    /* Deal with buffers that don't end with a new line. */

    if ((cur - last_nl) > 76) {
        encode = B_TRUE;
        qprint_growth += 2;
    }

    enc = MIME_7BIT;

    if (!strict_mime && !eight_bit) {
        /* If strict_mime is off we only encode if we have 8 bit data */
        enc = MIME_7BIT;
    } else if (encode) {
        /* strict_mime is TRUE and we have reason to encode. */
        if (base64) {
                enc = MIME_BASE64;
        } else {
                enc = MIME_QPRINT;
        }
    }

    return(enc);
}

void
writeContentHeaders(char * hdr_buf,
                             const char * type,
                             const Encoding enc,
                             const char * digest,
			     int isAllASCII)
{
    char default_charset[64];

    strcat(hdr_buf,"Content-Type: ");
    strcat(hdr_buf,type);

    if (isAllASCII)
	strcpy(default_charset,"US-ASCII");
    else
        getCharSet(default_charset);

    strcat(hdr_buf,"; charset=");

    strcat(hdr_buf,default_charset);

    crlf(hdr_buf);

    strcat(hdr_buf,"Content-Transfer-Encoding: ");

    switch (enc) {
      case MIME_7BIT:
        strcat(hdr_buf,"7bit\n");
        break;

      case MIME_8BIT:
      default: /* Assume the worst. */
        strcat(hdr_buf,"8bit\n");
        break;
      case MIME_QPRINT:
        strcat(hdr_buf,"quoted-printable\n");
        break;

      case MIME_BASE64:
        strcat(hdr_buf,"base64\n");
        break;
    }

    strcat(hdr_buf,"Content-MD5: ");
    writeBase64(hdr_buf, digest, 16);
}
/*
//
// Base64 Alphabet (65-character subset of US-ASCII as per RFC1521)
//
*/

static const char base64_chars[] =
{'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N',
 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a',
 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '0',
 '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/'
};

void
writeBase64(char * buf, const char * bp, const unsigned long len)
{

    /* The length has to be a multiple of 3. We will need to pad
    // any extra. Let's just work on the main body and save the
    // fractional stuff for the end.
    */
    unsigned long main_len = len - (len % 3);
    const unsigned char * ubp = (const unsigned char *)bp;

    char line[80];

    unsigned int enc_char;

    int lf = 0;

    int block;

    if (bp == NULL || len == 0) {
	crlf(buf);
	return;
    }

    for (block = 0; block < main_len; block += 3) {
	enc_char = (ubp[block] >> 2) & 0x3f;
	line[lf++] = base64_chars[enc_char];

	enc_char = ((ubp[block] & 0x3) << 4) | ((ubp[block+1] >> 4) & 0xf);
	line[lf++] = base64_chars[enc_char];

	enc_char = ((ubp[block + 1] & 0xf) << 2) | ((ubp[block + 2] >> 6) & 0x3);
	line[lf++] = base64_chars[enc_char];

	enc_char = ubp[block + 2] & 0x3f;
	line[lf++] = base64_chars[enc_char];

	if (lf == 72) {
	    strncat(buf,line,lf);
	    crlf(buf);
	    lf = 0;
	}
    }

    if (lf > 0) {
	strncat(buf, line,lf);
    }

    if (((lf + 4) % 72) == 0) {
	crlf(buf);
    }

    switch(len % 3) {
      case 1:
	enc_char = (ubp[block] >> 2) & 0x3f ;
	strncat(buf, &base64_chars[enc_char], 1);

	enc_char = ((ubp[block] & 0x3) << 4);
	strncat(buf, &base64_chars[enc_char], 1);

	strncat(buf,"==", 2);
	break;

      case 2:
	enc_char = (ubp[block] >> 2) & 0x3f;
	strncat(buf,&base64_chars[enc_char], 1);

	enc_char = ((ubp[block] & 0x3) << 4) | ((ubp[block+1] >> 4) & 0xf);
	strncat(buf,&base64_chars[enc_char], 1);

	enc_char = ((ubp[block + 1] & 0xf) << 2);
	strncat(buf,&base64_chars[enc_char], 1);

	strncat(buf,"=", 1);
    }

/*    crlf(buf); */
}
void
writeQPrint(char *buf, const char * bp, const unsigned long bp_len,
			int is_Special )
{
    int last_nl = 0;
    int off = 0;
    char line_buf[80];
    const char * start;
    const char * cur;
    const char * white;
    const char * nw;
    int line_len;
    const char *cp_w;
    char prev;
    char tmpbuf[20];

    if (bp == NULL || bp_len == 0) {
	crlf(buf);
	return;
    }


    /*
    // A line buffer for improving formatting performance. Note that
    // QP requires all lines to be < 72 characters plus CRLF. So, a
    // fixed size 80 character buffer is safe.
    */

    /* There are probably more elegant ways to deal with a message that
    // begins with "From ", but we will simply due it this more simplistic
    // way.
    */
    if (strncmp(bp, "From ", 5) == 0) {
	memcpy(&line_buf[off], "=46", 3);
	start = bp + 1;
	off += 3;
    }
    else {
	start = bp;
    }

    /* This loop will apply the encodings, following the rules identified
    // in RFC1521 (though not necessarily in the order presented.
    */
    for (cur = start; cur < (bp + bp_len); cur++) {

	/* Rule #5: Part 1! We will try to break at white space
	// if possible, but it may not be possible. In any case,
	// we want to force the lines to be less than 76 characters.
	*/
	if (off > 72) {
	    line_buf[off++] = '=';
	    strncat(buf,line_buf, off);
	    crlf(buf);
	    last_nl = 0;
	    off = 0;
	}

	/* Rule #1: Any octet, except those indicating a line break
	// according to the newline convention mabe represented by
	// an = followed by a two digit hexadecimal representation
	// of the octet's value. We will represent any non-7bit
	// data this way, but let the rest slide. We do wrap "="
	// just to be safe.
	*/
	if (*cur != (*cur & 0x7f) || *cur == '=') {
	    char tmp[20];
	    sprintf(tmp, "=%02X", (int)(unsigned char)*cur);
	    memcpy(&line_buf[off], tmp, 3);
	    off += 3;
	    continue;
	}
	if ( is_Special ){
	/*
	 * Under ISO-2022-XX codeset, several escape sequence may be in
	 * From, Subject field. To pass them, writeQPrint() also accept
	 * such kind of character.
	 */
	    if ( *cur == (char)0x1b ) {
		/* Only 0x1b ????? */
		char tmp[3];
		sprintf(tmp, "=%02X", (int)(unsigned char)*cur);
		memcpy(&line_buf[off], tmp, 3);
		off += 3;
		continue;
	    }
	}

	/* Rule #2: Octets with decimal values of 33 through 60
	// inclusive and 62 through 126, inclusive, MAY be represented
	// as the ASCII characters which correspond to those octets.
	*/
	if ((*cur >= 33 && *cur <= 60) ||
	    (*cur >= 62 && *cur <= 126)) {
	    line_buf[off++] = *cur;
	    continue;
	}

	/* Rule #5: The q-p encoding REQUIRES that encoded lines be
	// no more than 76 characters long. If longer, an equal sign
	// as the last character n the line indicates a soft line break.
	//
	// This is tricky if you want to leave it reasonably readable
	// (why else do this?). We only want to break on white space.
	// At each white gap, we need to count forward to the next
	// white gap and see if we exceed the 76 character limit.
	// We will cheat a few characters to allow us some room
	// for arithmetic.
	*/
	if (*cur == ' ' || *cur == '\t') {
	    /* Find the end of this clump of white space.
	    */
	    for (nw = cur;
		 nw < (bp + bp_len) && *nw && *nw != '\n'; nw++) {
		if (!isspace(*nw)) {
		    break;
		}
	    }

	    /* Find the end of the next non-white region.
	    */
	    for (white = nw;
		 white < (bp + bp_len) && *white && !isspace(*white);
		 white++) {
		continue;
	    }

	    line_len = (off - last_nl) + (white - cur);
	    if (line_len > 72) {
		/* Need a soft line break. Lets put it after the
		// current clump of white space. We will break
		// at 72 characters, even if we arent at the end
		// of the white space. This prevents buffer overruns.
		*/
		for (cp_w = cur; cp_w < nw; cp_w++) {
		    line_buf[off++] = *cp_w;
		    if (off > 72) {
			line_buf[off++] = '=';
			strncat(buf,line_buf, off);
			crlf(buf);
			off = 0;
			last_nl = 0;
		    }
		}

		/* There is an edge case that we may have written the last
		// white space character in the for loop above. This will
		// prevent us from spitting an extra continuation line.
		*/
		if (off) {
		    line_buf[off++] = '=';
		    strncat(buf,line_buf, off);
		    crlf(buf);
		    last_nl = 0;
		    off = 0;
		}

		/* If we created a "From " at the front we need to wrap
		// it to protect from parsers.
		*/
		if ((nw + 5) < (bp + bp_len) && strncmp(nw, "From ", 5) == 0) {
		    memcpy(&line_buf[off], "=46", 3);
		    off += 3;
		    cur = nw + 1;
		}
		else {
		    cur = nw - 1;
		}
	    }
	    else {
		line_buf[off++] = *cur;
	    }

	    continue;
	}

	/* Rule 3: Octets with values of 9 and 32 MAY be represented
	// as ASCII TAB and SPACE but MUST NOT be represented at the
	// end of an encoded line. We solve this be encoding the last
	// white space before a new line (except a new line) using
	// Rule #1.
	*/
	if (*cur == '\n') {
	    if (cur == start) {
		crlf(buf);
	    }
	    else {
		last_nl = off + 1;
		
		prev = *(cur - 1);
		if ((prev == ' ' || prev == '\t') && prev != '\n') {
		    off = off ? off - 1 : off;
		    
		    sprintf(tmpbuf, "=%02X", *(cur - 1));
		    memcpy(&line_buf[off], tmpbuf, 3);
		    off += 3;
		}

		strncat(buf,line_buf, off);
		last_nl = 0;
		off = 0;

		if (*(cur - 1) == '\r') {
		    strncat(buf,cur, 1);
		}
/*
		else {
		    crlf(buf);
		}
*/
	    }
	    /* We need to munge a line that starts with "From " to it
	    // protect from parsers. The simplest way is to encode the
	    // "F" using rule #1.
	    */
	    if ((cur + 5) < (bp + bp_len) && strncmp((cur + 1), "From ", 5) == 0) {
		memcpy(&line_buf[off], "=46", 3);
		off += 3;
		cur += 1;
	    }
	    continue;
	}
    }

    if (off > 0) {
	strncat(buf,line_buf, off);
    }
/*
    if (*(cur - 1) != '\n') {
	crlf(buf);
    }
*/
}


static int
mbisspace(int c)
{
    return((c & 0x7f) == c && isspace(c));
}

void
rfc1522cpy(char * buf, const char * value)
{
    const char * cur;
    const char * scan_c;
    boolean_t eight_bit = B_FALSE;
    char charset[64];
    char *ret_locale = NULL;
    char *ret_lang = NULL;
    char *ret_codeset = NULL;
    char  tmp[1024];
    char *NewBuf = NULL;
    unsigned long _len = 0;
    /*
    // We are going to encode 8 bit data, one word at a time. This may
    // not be the best possible algorithm, but it will get the correct
    // information in the header.
    */
    for (cur = value; *cur; cur++) {
	if (mbisspace(*cur)) {
	    strncat(buf,cur, 1);
	    continue;
	}

	for (scan_c = cur; *scan_c && !mbisspace(*scan_c); scan_c++) {
	    if (*scan_c != (*scan_c & 0x7f)) {
		eight_bit = B_TRUE;
	    }
	}

	if (eight_bit == B_FALSE) {
	    /* Simple! Copy the chars to the output. */
	    strncat(buf,cur,scan_c - cur);
	    cur = scan_c - 1;
	}
	else {

	    getCharSet( charset );
	    /* Convert default_charset to InterchangeCodeset name. */
	    DtXlateOpToStdLocale( DtLCX_OPER_MIME,
				charset,
				&ret_locale,
				&ret_lang,
				&ret_codeset );

	    /* We have a word here. It has 8 bit data, so we will put
	    // it out as RFC1522 chunk.
	    */

	    if ( !strncasecmp( ret_codeset, "ISO-2022-KR", 11 ) ) {
	    /*
	     * According to RFC1557, in the Header Field, we don't use
	     * ISO-2022-KR encoding char.
	     */
		char *_tmp1_;
		char *_tmp2_;
		free(ret_locale);
		free(ret_lang);
		ret_locale = ret_lang = _tmp1_ = NULL;
		DtXlateOpToStdLocale( DtLCX_OPER_INTERCHANGE_CODESET,
					ret_codeset,
					&ret_locale,
					&ret_lang,
					&_tmp1_ );
		_tmp2_ = NULL;
		DtXlateStdToOpLocale( DtLCX_OPER_MIME,
				NULL,
				NULL,
				_tmp1_,
				NULL,
				&_tmp2_ );
		strncat(buf,"=?", 2);
		strncat(buf,_tmp2_, strlen(_tmp2_));
		strncat(buf,"?q?", 3);

		free(_tmp1_);
		free(_tmp2_);
	    } else if ( !strncasecmp( ret_codeset, "ISO-2022-JP", 11 ) ) {
	    /*
	     * According to RFC1468, in the Header Field, we should use
	     * B-encoding
	     */
		strncat(buf,"=?", 2);
		strncat(buf,charset, strlen(charset));
		strncat(buf,"?b?", 3);
	    } else {
		strncat(buf,"=?", 2);
		strncat(buf,charset, strlen(charset));
		strncat(buf,"?q?", 3);
	    }

	    /*
	     * According to RFC1557, in the Header Field, we don't use
	     * ISO-2022-KR encoding char. Also in us-ascci, we don't have to
	     * call converter.
	     */
	    memset(tmp, '\0', 1024);
	    if (!( !strncasecmp( ret_codeset, "ISO-2022-KR", 11 ) ||
		   !strncasecmp( charset, "us-ascii", 8 ) )          )
		(void)CvtStr( (char *)NULL, (void *)cur, scan_c - cur,
				(void **)&NewBuf, &_len, CURRENT_TO_INTERNET );
	    if ( ( NewBuf != NULL ) && ( _len != 0 ) ) {
		/*
		 * if ret_codeset == ISO-2022-KR, we don't come here.
		 */
		/*
		 * According to RFC1468, we should use B-encoding.
		 */
		if ( !strncasecmp( ret_codeset, "ISO-2022-JP", 11 ) ) {
		    writeBase64( tmp, NewBuf, _len );
		} else {
		    writeQPrint( tmp, NewBuf, _len, 
			   (!strncasecmp( ret_codeset, "ISO-2022-TW", 11 ) ||
			    !strncasecmp( ret_codeset, "ISO-2022-CN", 11 )   ));
		}
	    } else
		writeQPrint( tmp, cur, scan_c - cur, 0 );

	    strncat(buf,tmp,strlen(tmp));
	    strcat(buf,"?=");
	    cur = scan_c - 1;
	}
    }

    free(ret_codeset);
    free(ret_locale);
    free(ret_lang);

    crlf(buf);
}
