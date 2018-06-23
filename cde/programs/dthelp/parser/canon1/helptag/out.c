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
/* $XConsortium: out.c /main/4 1995/12/04 11:41:40 rswiston $ */
/*   Copyright (c) 1986, 1987, 1988, 1989 Hewlett-Packard Co. */
/* Functions for HPTag/TeX translator relevant to output of heads and
   data characters */

#include "userinc.h"
#include "globdec.h"


/* Echo part of a head to the screen to indicate how much of the document
   has been processed */
void echohead(M_WCHAR *p)
{
char *mb_p,*mb_string;

mb_string = MakeMByteString(p);

#ifdef _AIX
/* This is a bug fix for AIX. Using putc() for EUC Gaiji characters on aixterm
 * has a problem.
 */
fprintf(stderr, "%s", mb_string);
#else /* _AIX */
for (mb_p = mb_string ; *mb_p ; mb_p++)
    {
    putc(*mb_p, stderr);
    }
#endif /* _AIX */

m_free(mb_string,"multi-byte string");
}

/* call echohead with a literal string */
void mb_echohead(char *p)
{
M_WCHAR *wc;

wc = MakeWideCharString(p);
echohead(wc);
m_free(wc,"wide character string");
}
  

/* Called at end of a head in a chapter, appendix, section, or
   reference section */
void endhead(void)
{
if (savid)
    {
    if (w_strlen(xrefstring) + 1 > (sizeof(xrefstring) / sizeof(M_WCHAR)))
	{
	m_error("ID expansion too long");
	m_exit(TRUE);
	}

    mb_shchar(M_EOS,
	      &xstrlen,
	      (sizeof(xrefstring) / sizeof(M_WCHAR)),
	      xrefstring,
	      idstring,
	      "Too many characters in corresponding cross-reference",
	      &iderr);
    setid(savid,
	  TRUE,
	  TRUE,
	  inchapter,
	  chapstring,
	  xrffile,
	  xrfline,
	  TRUE);
    }

savehead[svheadlen] = M_EOS;
echo = savhd = FALSE;
}

/* Called at the end of SDL <head> elements for <virpage> elements.
 * Saves the location in the output file of the end of the
 * <virpage><head>.  If any <snb> elements have been generated, the
 * location is placed in the snb file and the <snb> elements are
 * emitted there too.
*/
void chksnb(void)
{
fputs("</HEAD>\n", outfile);
snbstart = ftell(outfile);
if (savesnb)
    {
    if (strlen(savesnb) != 0)
	{
	fprintf(snbfp, "%d\n%s", snbstart, savesnb);
	snbstart = 0;
	}
    mb_free(&savesnb);
    }
}


/* Process a character in an environment where spaces must be escaped */
void esoutchar(M_WCHAR textchar)
{
static M_WCHAR wsp = 0;

if (!wsp)
    {
    char space[2];

    space[0] = M_SPACE;
    space[1] = 0;
    mbtowc(&wsp, space, 1);
    }

if (textchar == wsp)
    {
    mb_strcode("&", outfile);
    }
outchar(textchar, outfile);
}

/* Process a character in a verbatim example */
void vexoutchar(M_WCHAR textchar)
{
static M_WCHAR  wla = 0;
       char    *holdSaveExSeg;

if (!wla)
    {
    mbtowc(&wla, "<", 1);
    }

if (textchar == wla)
    {
    holdSaveExSeg = saveexseg;
    saveexseg = NULL;
    mb_strcode("&<", outfile);
    saveexseg = holdSaveExSeg;
    }
else
    {
    multi_cr_flag = FALSE;
    outchar(textchar, outfile);
    }
}

/* Process a character in an example */
void exoutchar(M_WCHAR textchar)
{
char          exbuff[32]; /* arbitrarily large */
int           bufflen;
LOGICAL       holdSavex;
unsigned char index;

bufflen = wctomb(exbuff, textchar);
exbuff[bufflen] = '\0';
if (bufflen == 1)
    {
    index = (unsigned char) exbuff[0];
    if (index == M_RE)
	exLineNum++;
    if (special[index])
	{
	snprintf(exbuff, sizeof(exbuff), "%s", special[index]);
	bufflen = strlen(exbuff);
	}
    }
saveexseg = mb_realloc(saveexseg, svexseglen + bufflen);
strcpy(saveexseg + svexseglen - 1, exbuff);
svexseglen += bufflen;
holdSavex = savex;
savex = FALSE;
outchar(textchar, outfile);
savex = holdSavex;
}

/* Process a character in an image paragraph */
void imoutchar(M_WCHAR textchar)
{
char           imbuff[32]; /* arbitrarily large */
int            bufflen;

bufflen = wctomb(imbuff, textchar);
imbuff[bufflen] = '\0';
if (bufflen == 1)
    {
    unsigned char index;
    index = (unsigned char) imbuff[0];
    if (special[index])
	{
	snprintf(imbuff, sizeof(imbuff), "%s", special[index]);
	bufflen = strlen(imbuff);
	}
    mb_strcode(imbuff, outfile);
    }
else
    outchar(textchar, outfile);
}

/* Write a character to be passed to the index file */
void indexchar(M_WCHAR textchar)
{
if (idxsavlen + 4 > (sizeof(idxsav) / sizeof(M_WCHAR)))
    {
    m_error("Internal error. Exceeded save buffer for index");
    m_exit(TRUE);
    }
idxsav[idxsavlen++] = textchar;
}


/* Processes a data character */
void outchar(M_WCHAR textchar, FILE *outfile)
{
M_WCHAR buffer[2];
static M_WCHAR wnl = 0;
char mb_textchar[32]; /* arbitrarily large */
unsigned char index;
int length;

if (!wnl)
    mbtowc(&wnl, "\n", 1);

if (textchar == wnl)
    {
    if (multi_cr_flag)
	{
	return;
	}
    multi_cr_flag = TRUE;
    }
else
    {
    multi_cr_flag = FALSE;
    }

if (echo)
    {
    buffer[0] = textchar;
    buffer[1] = M_EOS;
    echohead(buffer);
    }

cur_char = textchar;

length = wctomb(mb_textchar, textchar);

index = (unsigned char) mb_textchar[0];
if ((length == 1) && special[index])
    mb_strcode(special[index], outfile);
else
    realoutchar(textchar, outfile);
} /* end procedure outchar() */


/* Called for processing instruction */
void outpi(int enttype, M_WCHAR *pi, M_WCHAR *entname)
{
strcode(pi, outfile);

if (echo)
    {
    if (entname)
	{
	mb_echohead(m_ero);
	echohead(entname);
	mb_echohead(m_refc);
	}
    else
	{
	mb_echohead(m_pio);
	echohead(pi);
	mb_echohead(m_pic);
	}
    }
}


/* Really output a character */
void realoutchar(M_WCHAR textchar, FILE *outfile)
{
char mb_textchar[32]; /* arbitrarily large */
int length, i;

length = wctomb(mb_textchar, textchar);
if (tooutput)
    for (i = 0; i < length; i++)
	putc(mb_textchar[i], outfile);

if (savid)
    shchar(textchar,
	   &xstrlen, 
	   (sizeof(xrefstring) / sizeof(M_WCHAR)),
	   xrefstring,
	   idstring,
	   "Too many characters in corresponding cross-reference",
	   &iderr);

if (savhd)
    shchar(textchar,
	   &svheadlen,
	   (sizeof(savehead) / sizeof(M_WCHAR)),
	   savehead,
	   svhdstring,
	   "Too many characters in head or caption",
	   &hderr);

if (savtc)
    shchar(textchar,
	   &svtclen,
	   (sizeof(savetabcap) / sizeof(M_WCHAR)),
	   savetabcap,
	   svtcstring,
	   "Too many characters in table caption",
	   &hderr);
}

/* Save a character in the array used to store table of contents entries
   when processing a head */
void shchar(M_WCHAR textchar, int *len, int max, M_WCHAR *string, void (*proc)(M_WCHAR *string), char *msg, LOGICAL *errflg)
{
char mb_textchar[32]; /* arbitrarily large */
unsigned char index;
int  length;

length = wctomb(mb_textchar, textchar);

index = (unsigned char) mb_textchar[0];
if ((length == 1) && special[index])
    {
    M_WCHAR *wc_special;

    wc_special = MakeWideCharString(special[index]);
    (*proc)(wc_special);
    m_free(wc_special,"wide character string");
    }
else
    {
    if (*len + 1 + 1 > max)
	{
	if (! *errflg)
	    {
	    m_error(msg);
	    *errflg = TRUE;
	    }
	}
    else
	string[(*len)++] = textchar;
    }
}


void mb_shchar(char textchar,
	       int *len,
	       int max,
	       M_WCHAR *string,
	       void (*proc)(M_WCHAR *string),
	       char *msg, LOGICAL *errflg)
{
M_WCHAR wc_textchar;

mbtowc(&wc_textchar, &textchar, 1);

shchar(wc_textchar, len, max, string, proc, msg, errflg);
}


/* Save a string in the array used to store table of contents entries
   when processing a head */
void shstring(M_WCHAR *addstring, int *len, int max,
              M_WCHAR *storestring, char *msg, LOGICAL *errflg)
{
int addlength;

addlength = w_strlen(addstring);
if (*len + addlength + 1 > max)
    {
    if (! *errflg)
	{
	m_error(msg);
	*errflg = TRUE;
	}
    }
else
    {
    w_strcpy(&storestring[*len], addstring);
    *len += addlength;
    }
}


void mb_shstring(char *addstring, int *len, int max,
                 M_WCHAR *storestring, char *msg, LOGICAL *errflg)
{
M_WCHAR *wc_addstring;

wc_addstring = MakeWideCharString(addstring);
shstring(wc_addstring, len, max, storestring, msg, errflg);
m_free(wc_addstring,"wide character string");
}

/* Writes a string to the output file, and if appropriate saves it */
void strcode(M_WCHAR *string, FILE *outfile)
{
char    exbuff[32]; /* arbitrarily large */
int     bufflen;
M_WCHAR wc;

if (tooutput)
    {
    char *mb_string;

    mb_string = MakeMByteString(string);
    fputs(mb_string, outfile);
    m_free(mb_string,"multi-byte string");
    }

if (savid)
    shstring(string,
	     &xstrlen,
	     (sizeof(xrefstring) / sizeof(M_WCHAR)),
	     xrefstring,
	     "Too many characters in corresponding cross-reference",
	     &iderr);

if (savhd)
    shstring(string,
	     &svheadlen,
	     (sizeof(savehead) / sizeof(M_WCHAR)),
	     savehead,
	     "Too many characters in head or caption",
	     &hderr);

if (savtc)
    shstring(string,
	     &svtclen,
	     (sizeof(savetabcap) / sizeof(M_WCHAR)),
	     savetabcap,
	     "Too many characters in table caption",
	     &hderr);

if (savex)
    {
    while (wc = *string++)
	{
	bufflen = wctomb(exbuff, wc);
	if ((exbuff[0] == M_RE) && (bufflen == 1))
	    exLineNum++;
	saveexseg = mb_realloc(saveexseg, svexseglen + bufflen);
	strncpy(saveexseg + svexseglen - 1, exbuff, bufflen);
	svexseglen += bufflen;
	}
    saveexseg[svexseglen-1] = '\0';
    }
}


void mb_strcode(char *string, FILE *outfile)
{
M_WCHAR *wc;

wc = MakeWideCharString(string);
strcode(wc, outfile);
m_free(wc,"wide character string");
}

/* Copies string to end of buffer where saving head for table of contents */
void svhdstring(M_WCHAR *string)
{
int length;

length = w_strlen(string);
if (svheadlen + length + 1 > (sizeof(savehead) / sizeof(M_WCHAR)))
    {
    if (! hderr)
	{
	m_error("Too many characters in head or caption");
	hderr = TRUE;
	}
    return;
    }
w_strcpy(&savehead[svheadlen], string);
svheadlen += length;
}

/* Copies string to end of buffer where saving table caption */
void svtcstring(M_WCHAR *string)
{
int length;

length = w_strlen(string);
if (svtclen + length + 1 > (sizeof(savetabcap) / sizeof(M_WCHAR)))
    {
    if (! hderr)
	{
	m_error("Too many characters in table caption");
	hderr = TRUE;
	}
    return;
    }
w_strcpy(&savetabcap[svtclen], string);
svtclen += length;
}

/* Process a character in a term */
void termchar(M_WCHAR textchar)
{
int length;
char mb_textchar[32]; /* arbitrarily large */
char *pc;

length = wctomb(mb_textchar, textchar);
if ((length == 1) && (pc = special[(unsigned char) *mb_textchar]))
    {
    length = strlen(pc);
    if (termp - term + length > MAXTERM)
	{
	if (termp - term <= MAXTERM)
	    {
	    M_WCHAR *wc_stago, *wc_tagc;

	    wc_stago = MakeWideCharString(m_stago);
	    wc_tagc  = MakeWideCharString(m_tagc);
	    m_err4("Too many characters in %sTERM%s or %sDTERM%s",
		   wc_stago,
		   wc_tagc,
		   wc_stago,
		   wc_tagc);
	    m_free(wc_stago,"wide character string");
	    m_free(wc_tagc,"wide character string");
	    }
	}
    else
	{
	mbstowcs(termp, pc, length);
	}
    termp += length;
    }
else
    {
    if (termp - term >= MAXTERM)
	{
	if (termp - term == MAXTERM)
	    {
	    M_WCHAR *wc_stago, *wc_tagc;

	    wc_stago = MakeWideCharString(m_stago);
	    wc_tagc  = MakeWideCharString(m_tagc);
	    m_err4("Too many characters in %sTERM%s or %sDTERM%s",
		   wc_stago,
		   wc_tagc,
		   wc_stago,
		   wc_tagc);
	    m_free(wc_stago,"wide character string");
	    m_free(wc_tagc,"wide character string");
	    termp++; /* avoid getting this message again */
	    }
	}
    else
	{
	*termp++ = textchar;
	}
    }
}

/* Process a PI in a term */
void termpi(int m_enttype, M_WCHAR *m_pi, M_WCHAR *m_entname)
{
int length;

length = w_strlen(m_pi);
if (termp - term + length > MAXTERM)
    {
    if ((termp - term) <= MAXTERM)
	{
	M_WCHAR *wc_stago, *wc_tagc;

	wc_stago = MakeWideCharString(m_stago);
	wc_tagc  = MakeWideCharString(m_tagc);
	m_err4("Too many characters in %sTERM%s or %sDTERM%s",
	       wc_stago,
	       wc_tagc,
	       wc_stago,
	       wc_tagc);
	m_free(wc_stago,"wide character string");
	m_free(wc_tagc,"wide character string");
	}
    }
else
    {
    w_strncpy(termp, m_pi, length);
    }
termp += length;

if (echo)
    {
    if (m_entname)
	{
	mb_echohead(m_ero);
	echohead(m_entname);
	mb_echohead(m_refc);
	}
    else
	{
	mb_echohead(m_pio);
	echohead(m_pi);
	mb_echohead(m_pic);
	}
    }
}
