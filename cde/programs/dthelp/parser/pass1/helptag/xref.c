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
/* $XConsortium: xref.c /main/3 1995/11/08 10:13:50 rswiston $ */
/*
Copyright 1988, 1989 Hewlett-Packard Co.
*/

/* HP Tag to TeX cross-reference processing */

#include "userinc.h"
#include "globdec.h"

/* Chk for duplicate xref id's, called in TEST, S1, S2, S3, S4, and rsect. */
M_WCHAR *checkid(M_WCHAR *id)
{
struct xref *xref;
char *buffer;
char *mb_id;

if (!id) return(NULL);

m_getline(&xrffile, &xrfline);
if (xrffile == NULL)
    {
    /* set to primary input */
    xrffile = inputname;
    }

xref = (struct xref*) m_lookfortrie(id, &xtree);
if (!xref) return(id);  /* not in trie, so not previously defined */

if (!(xref->wheredef))  /* defined only in previous pass */
    return(id);

/* else, we have a duplicate definition of id */
m_err1("Redefinition of ID '%s'", id);
mb_id = MakeMByteString(id);
buffer = (char *) m_malloc(1 + strlen(firstused) + strlen(mb_id) + 6,
			   "error buffer");
sprintf(buffer, firstused, mb_id, xref->line);
m_errline(buffer);
m_free(buffer, "error buffer");
m_free(mb_id,"multi-byte string");

if (xref->file)
    {
    char *mb_file;

    mb_file = MakeMByteString(xref->file);
    buffer = (char *) m_malloc(1 - 2 + strlen(offile) + strlen(mb_file),
			       "error buffer");
    sprintf(buffer, offile, mb_file);
    m_errline(buffer);
    m_free(buffer, "error buffer");
    m_free(mb_file,"multi-byte string");
    }
m_errline(".\n");
return(NULL);
}

/* Write a single cross-reference macro definition */
static void defxref(FILE *xfile, M_WCHAR *id, struct xref *xref)
{
char *csname;
char *p;
char *buffer;

csname = MakeMByteString(id);
if (xref->defined)
    {
    char *mb_text;

    mb_text = MakeMByteString(xref->textptr);
    fprintf(xfile,
	    "\\definexref %s\\\\endxref\001%s\002\001%s",
	    csname,
	    mb_text,
	    xref->chapstring);
    fprintf(xfile,
	    "\002\001%c\002\001%s\002\001%c\002%%\n",
	    xref->csensitive ? 'Y' : 'N',
	    xref->inchapter ? "\\LOCchap" : "\\LOCapp",
	    xref->xrefable ? 'Y' : 'N');
    m_free(mb_text,"multi-byte string");
    }
else
    {
    fprintf(xfile, "\\undefinedxref %s\\\\endxref\001", csname);
    for (p = csname; *p ; p++)
	outchar(*p, xfile);
    fputs("\002%\n", xfile);
    m_eprefix();  /* correctly delimit error in error file */
    buffer = (char *) m_malloc(1 - 2 + strlen(undef) + w_strlen(id),
			       "error buffer");
    sprintf(buffer, undef, csname);
    m_errline(buffer);
    m_free(buffer, "error buffer");
    if (xref->file)
	{
	char *mb_file;

	mb_file = MakeMByteString(xref->file);
	buffer = (char *) m_malloc(1 - 2 + strlen(infile) +
				   strlen(mb_file),
				   "error buffer");
	sprintf(buffer, infile, mb_file);
	m_errline(buffer);
	m_free(buffer, "error buffer");
	m_free(mb_file,"multi-byte string");
	}
    buffer = (char *) m_malloc(1 - 2 + strlen(online) + 10,
			       "error buffer");
    sprintf(buffer, online, xref->line);
    m_errline(buffer);
    m_free(buffer, "error buffer");
    }
m_free(csname, "csname");
}

/* Write cross-reference file for next time document is processed
   and TeX macro file for post-processing this pass */
void dumpxref(void)
{
int n;
M_WCHAR id[M_NAMELEN + 1];
M_TRIE *node[M_NAMELEN + 1];
M_TRIE *current;
FILE *tex;

strcpy(helpext, ".xrh");
tex = fopen(helpbase, "w");
if (! tex) {
	return;
}
fprintf(tex, "\\gobble\001%s\002%%\n", m_signon);
fputs("% Generated Cross-Reference Macros (for a particular document)\n", tex);
if (! xtree.data) {
	fclose(tex);
	return;
}

n = 0;
current = xtree.data;
while (TRUE)
    {
    id[n] = current->symbol;
    node[n] = current->next;
    if (! id[n])
	{
	/* Process one table entry */
	defxref(tex, id, (struct xref *) current->data);

	/* Look for the next one */
	current = current->next;
	while (! current)
	    {
	    n--;
	    if (n < 0)
		{
		fclose(tex);
		return;
		}
	    current = node[n];
	    }
	}
    else
	{
	current = current->data;
	n++;
	}
    }
}

/* Copies string to end of current cross-reference string */
void idstring(M_WCHAR *string)
{
int length;

length = w_strlen(string);
if (xstrlen + length + 1 > (sizeof(xrefstring) / sizeof(M_WCHAR)))
    {
    if (! iderr)
	{
	m_error(
    "Internal error. Too many characters in cross-reference expansion.");
	iderr = TRUE;
	}
    return;
    }

w_strcpy(&xrefstring[xstrlen],string);
xstrlen += length;
}

/* Initializes cross-reference table reading in file output from previous
   pass through document */
void loadxref(void)
{
M_WCHAR id[M_NAMELEN + 1];
char c, d;
int nextc;
int n;
LOGICAL csensitive;
LOGICAL inchapter;
LOGICAL xrefable;
char chapnum[15];
char string[15];
int braces;
M_WCHAR *p, *pStart;
static M_WCHAR wbs = 0, wca, wcb, wnl, weof;

if (! xrf) return;

if (!wbs)
    {
    char mb_eof;

    mbtowc(&wbs, "\\", 1);
    mbtowc(&wca, "\001", 1); /* ^A */
    mbtowc(&wcb, "\002", 1); /* ^B */
    mbtowc(&wnl, "\n", 1);
    mb_eof = EOF;
    mbtowc(&weof, &mb_eof, 1);
    }

while (TRUE)
    {
    if (fscanf(xrf, "%14s ", string) != 1)
	{
	fclose(xrf);
	return;
	}
    if (! strcmp(string, "\\undefinedxref"))
	{
	for (nextc = mb_getwc(xrf);
	     (nextc != wnl) && (nextc != weof);
	     nextc = mb_getwc(xrf)
	    );
	continue;
	}
    for (n = 0; n <= M_NAMELEN ; n++)
	{
	nextc = mb_getwc(xrf); /* use mb_getwc to read multi-byte chars */
	if (nextc == weof)
	    {
	    m_error("Incomplete cross-reference file.");
	    fclose(xrf);
	    return;
	    }
	if (nextc == wbs) break;
	id[n] = nextc;
	}
    if (n > M_NAMELEN)
	{
	m_error("Error in cross-reference file.");
	fclose(xrf);
	return;
	}
    pStart = p = MakeWideCharString("\\endxref\001");
    for (; *p ; p++)
	{
	nextc = mb_getwc(xrf);
	if (nextc != *p)
	    {
	    m_free(pStart,"wide character string");
	    m_error("Error in cross-reference file.");
	    fclose(xrf);
	    return;
	    }
	}
    m_free(pStart,"wide character string");
    id[n] = M_EOS;
    for (xstrlen = 0, braces = 1;
	 xstrlen < (sizeof(xrefstring) / sizeof(M_WCHAR));
	 xstrlen++)
	{
	xrefstring[xstrlen] = mb_getwc(xrf);
	if (xrefstring[xstrlen] == wca) braces++;
	else if (xrefstring[xstrlen] == wcb) if (! --braces) break;
    }
    if (xstrlen >= (sizeof(xrefstring) / sizeof(M_WCHAR)))
	{
	m_error("Error in cross-reference file");
	fclose(xrf);
	return;
	}

    xrefstring[xstrlen] = M_EOS;
    if ((fscanf(xrf,
		"\001%14[^\002]\002\001%c\002\001%8[^\002]\002\001%c\002%%\n",
		chapnum, &c, string, &d) != 4)                ||
	    (c != 'Y' && c != 'N') ||  (d != 'Y' && d != 'N') || 
	    (strcmp(string, "\\LOCapp") && strcmp(string, "\\LOCchap"))
       )
	{
	m_error("Error in cross-reference file");
	fclose(xrf);
	return;
	}
    csensitive = (LOGICAL) (c == 'Y');
    inchapter = (LOGICAL) (strcmp(string, "\\LOCchap") == FALSE);
    xrefable = (LOGICAL) (d == 'Y');
    setid(id,
          FALSE,
          csensitive,
          inchapter,
          chapnum,
          NULL,
          0,
          xrefable);
    }
/* fclose(xrf);  ** not reachable; left in for future reference */
}


/* Save a cross-reference ID and associated expansion */
void setid(M_WCHAR *id, LOGICAL where, LOGICAL csensitive, LOGICAL inchapter,
	   char *chapter, M_WCHAR *xrffile, int xrfline, LOGICAL xrefable)
{
struct xref *old;
struct xref *xref;
char *buffer;

if (!*id)
    {
    m_err2("Erroneous cross reference of `%s' for id `%s'",
    xrefstring,
    id);
    return;
    }
xref = (struct xref *) m_malloc(sizeof(struct xref), "xref");
if (old = (struct xref *) m_ntrtrie(id, &xtree, (M_TRIE *) xref))
    {
    m_free(xref, "xref");
    xref = old;
    if (xref->wheredef)
	{
	char *mb_id;

	mb_id = MakeMByteString(id);
	m_err1("Redefinition of ID '%s'", id);
	buffer = (char *)
	    m_malloc(1 + strlen(firstused) + w_strlen(id) + 6,
		     "error buffer");
	sprintf(buffer, firstused, mb_id, xref -> line);
	m_errline(buffer);
	m_free(buffer, "error buffer");
	m_free(mb_id,"multi-byte string");
	if (xref->file)
	    {
	    char *mb_file;

	    mb_file = MakeMByteString(xref->file);
	    buffer = (char *)
		m_malloc(1 - 2 + strlen(offile) + strlen(mb_file),
			 "error message");
	    sprintf(buffer, offile, mb_file);
	    m_errline(buffer);
	    m_free(buffer, "error buffer");
	    m_free(mb_file,"multi-byte string");
	    }
	m_errline(".\n");
	return;
	}
    if (xref->defined)
	{
	if (w_strcmp(xrefstring, xref->textptr) != 0) /* ref has changed */
	    have_forward_xrefs = TRUE;         /* so force a second pass */
	m_free(xref->chapstring, "xref chapter string");
	m_free(xref->textptr, "xref text pointer");
	}
    }
else xref->retrieved = FALSE;

xref->defined = TRUE;
xref->wheredef = where;
xref->csensitive = csensitive;
xref->inchapter = inchapter;
xref->chapstring =
    (char *) m_malloc(strlen(chapter) + 1, "xref chapter string");
xref->textptr = (M_WCHAR *) m_malloc(xstrlen + 1, "xref text pointer");
strcpy(xref->chapstring, chapter);
w_strcpy(xref->textptr, xrefstring);
xref->file = xrffile;
xref->line = xrfline;
xref->xrefable = xrefable;
xstrlen = 0;
savid = NULL;
}

/* Generate a cross-reference */
void xrefexpand(M_WCHAR *id)
{
struct xref *xref;
struct xref *old;
#define format "\\<xref %s>"
char *texcode;
char *csname;

/* ID is 0 in case of a parameter error which MARKUP will report */
if (! id) return;

/* Write call to generated macro in output file */
csname = MakeMByteString(id);
texcode = (char *) m_malloc(sizeof(format) + strlen(csname), "texcode");
sprintf(texcode, format, csname);

if (echo)
    {
    mb_echohead(m_stago);
    mb_echohead("XREF ");
    echohead(id);
    mb_echohead(m_tagc);
    }

if (savhd)
    mb_shstring(texcode,
	        &svheadlen,
                (sizeof(savehead) / sizeof(M_WCHAR)),
                savehead,
                "Too many characters in head or caption",
                &hderr);

if (savtc)
    mb_shstring(texcode,
                &svtclen,
                (sizeof(savetabcap) / sizeof(M_WCHAR)),
                savetabcap,
                "Too many characters in table caption",
                &hderr);

if (savid)
    mb_shstring(texcode,
		&xstrlen,
		(sizeof(xrefstring) / sizeof(M_WCHAR)),
		xrefstring,
		"Too many characters in corresponding cross-reference",
		&iderr);

/* Enter id in xref table if it's not already there */
xref = (struct xref *) m_malloc(sizeof(struct xref), "xref");
if (old = (struct xref *) m_ntrtrie(id, &xtree, (M_TRIE *) xref))
    {
    /* non-NULL, we had an old one */
    m_free(xref, "xref");
    xref = old;
    if (! xref->xrefable)
	{
	m_error("You can't cross reference to that type of tag");
	/* but put out the string anyway... */

	/* decrement error counter to treat this as a warning */
	m_errcnt--;
	}
    if (xref->textptr)
	{
	/* we have an expansion */
	char *mb_textptr;

	if (!*xref->textptr)
	    {
	    m_err1("Empty cross reference for id `%s'", id);
	    return;
	    }

	mb_textptr = MakeMByteString(xref->textptr);
	fprintf(outfile,
	        "<LINK RID=\"%s\">%s</LINK>",
		csname,
		mb_textptr);
	m_free(mb_textptr,"multi-byte string");
	}
    else
	{ /* expansion */
	/* dump out a tag string as a placeholder */
	fputs(texcode, outfile);
	/* not really forward, but a null string */
	have_forward_xrefs = TRUE;
	/*     xref->defined = FALSE;
	xref->retrieved = FALSE;
	xref->textptr = NULL;
	xref->wheredef = FALSE;*/
	}
    }
else
    { /* no old xref */
    if (rebuild)
	{
	m_errline(
	    "Unresolved xrefs. Running HelpTag a second time may help.\n");
	rebuild = FALSE;  /* one error message will do */
	}
    have_forward_xrefs = TRUE;
    xref->defined = FALSE;
    xref->retrieved = FALSE;
    xref->textptr = NULL;
    xref->wheredef = FALSE;

    /* This is a forward ref.  We assume it is xrefable */
    /* It will be set correctly if and when the ref is defined */
    xref->xrefable = TRUE;

    fputs(texcode, outfile);
    }

/* Record retrieval status if this is the first reference */
if (! xref->retrieved && ! xref->defined)
    {
    m_getline(&xref->file, &xref->line);
    if (xref->file == NULL)
	{
	/* set to primary input */
	xref->file = inputname;
	}
    xref->retrieved = TRUE;
    }
m_free(texcode, "texcode buffer");
m_free(csname, "csname");
}

