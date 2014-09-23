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
/* $XConsortium: scanutil.c /main/3 1995/11/08 10:35:33 rswiston $ */
/* Copyright (c) 1986, 1987, 1988, 1989 Hewlett-Packard Co. */

/* Scanutil.c contains procedures used in the scanners of BUILD and ELTDEF */

extern FILE *ifile;

/* Getachar returns the next character from the input stream */
int getachar(M_NOPAR)
{
int  c;
M_WCHAR wc,wnl;
char mbyte[32]; /* bigger than any possible multibyte char */
int  length, ret;

if (toundo) wc = (M_WCHAR) savechar[--toundo];
else
    {
    length = 0;
    if ((c = getc(ifile)) == EOF) return(EOF);
    while (1)
	{
	mbyte[length++] = c;
	mbyte[length]   = 0;
	if (mblen(mbyte,length) != -1) break; /* hurray! */
	if (length == MB_CUR_MAX)
	    { /* reached max without a hit */
	    m_error("An invalid multi-byte character was found in the input");
	    c = ' ';
	    length = 1;
	    break;
	    }
	if ((c = getc(ifile)) == EOF)
	    { /* huh? */
	    m_error("End-of-file found in within a multi-byte character");
	    return(EOF);
	    }
	}
    ret = mbtowc(&wc,mbyte,length);
    }

ret = mbtowc(&wnl, "\n", 1);
(void) ret;
if (wc == wnl) m_line++;

if (wc == 65535)
    return -1;

return((int) wc);
}

/* Reads the next token stored in a packed trie (as defined by context.dat) */
int gettoken(c, context)
int *c;
int context;
{
int hold[MAXD + 1];
int ucase;
int i, n = 0, current, delim[MAXD + 1];

if (! (current = m_contree[context - 1]))
    {
    *c = getachar();
    return(M_NULLVAL);
    }
current--;
while (TRUE)
    {
    hold[n] = getachar();
    ucase = m_ctupper(hold[n]);
    delim[n] = FALSE;
    for (i = current;
	 (int) m_delimtrie[i].symbol < ucase && m_delimtrie[i].more;
	 i++);
    if ((int) m_delimtrie[i].symbol == ucase)
	{
	current = m_delimtrie[i].index;
	if (! m_delimtrie[current].symbol)
	  delim[n] = m_delimtrie[current].index;
	n++;
	}
    else break;
    }
while (n >= 0)
    {
    if (delim[n])
	{
	/* Found a delimiter. If it ends with a letter, verify
	   that the following character is not a letter, in order
	   to issue error messages in cases such as <!ENTITYrunon ... */
	if (m_cttype(hold[n]) != M_NMSTART) return(delim[n]);
	*c = getachar();
	ungetachar(*c);
	if (*c == EOF || m_cttype(*c) != M_NMSTART) return(delim[n]);
	}
    if (n) ungetachar(hold[n]);
    n--;
    }
*c = *hold;
return(M_NULLVAL);
}

/* Reads a literal (called after the opening quotation mark is read) */
LOGICAL litproc(delim)
int delim;
{
int n, i, number;
M_WCHAR *p, *pStart; /* bigger than wide versions of lit or lita */
int c;

for (i = 0 ; i < M_LITLEN ; i++)
    {
    n = gettoken(&c, LITCON);
    switch (n)
	{
	case LIT:
	case LITA:
	    if (n == delim)
		{
		literal[i] = M_EOS;
		return(TRUE);
		}
	    pStart = p = MakeWideCharString((n == LIT) ? lit : lita);
	    if (w_strlen(p) + i > M_LITLEN)
		{
		m_error("Literal too long");
		return(FALSE);
		}
	    while (*p) literal[i++] = *p++;
	    m_free(pStart,"wide character string");
	    i--;
	    break;
	case CRO:
	    number = 0;
	    while (! (n = gettoken(&c, CHARENT)))
		{
		if (c <= '0' || c >= '9')
		    {
		    ungetachar(c);
		    break;
		    }
		number = 10 * number + c - '0';
		if (number >= M_CHARSETLEN)
		    {
		    m_error("Invalid character code");
		    return(FALSE);
		    }
		}
	  literal[i] = (M_WCHAR) number;
	  break;
	case LITRS:
	case LITRSC:
#if defined(BUILDEXTERN)
	  if (curcon == SREF || curcon == S2REF)
	    literal[i] = RS;
	  else
#endif
	    i--;
	  break;
	case LITRE:
	case LITREC:
	  literal[i] = M_RE;
	  break;
	case LITTAB:
	case LITTABC:
	  literal[i] = M_TAB;
	  break;
	case LITSPACE:
	case LITCSPACE:
	  literal[i] = M_SPACE;
	  break;
	default:
	  if (c == EOF)
	      {
	      m_error("End of File occurred within literal");
	      return(FALSE);
	      }
	  literal[i] = (M_WCHAR) c;
	  break;
	} /* End switch */
    } /* End for i */
n = gettoken(&c, LITCON);
if (n == delim)
    {
    literal[M_LITLEN] = M_EOS;
    return(TRUE);
    }
if (n) undodelim(m_dlmptr[n - 1]) ; 
else ungetachar(c);
m_error("Literal too long");
return(FALSE);
}

/* Returns a context-dependent delimiter string to input stream so
   characters can be reread one at a time in another context */
void undodelim(delim)
M_WCHAR *delim;
{
M_WCHAR *p;

for (p = delim ; *p ; p++);

p--;
while (TRUE)
    {
    ungetachar(*p);
    if (p == delim) return;
    p--;
    }
}

/* Returns a character to the input stream to read again later. Unexplained
   problems occurred using standard ungetc procedure; hence
   explicit declaration of buffer for read-ahead characters */
void ungetachar(c)
int c;
{
M_WCHAR wnl;

if (toundo >= SAVECHAR)
    {
    m_error("Exceeded read-ahead buffer");
    exit(TRUE);
    }
savechar[toundo++] = c;
if (toundo > maxundo) maxundo = toundo;

int ret = mbtowc(&wnl, "\n", 1);
(void) ret;
if (c == wnl) m_line--;
}

