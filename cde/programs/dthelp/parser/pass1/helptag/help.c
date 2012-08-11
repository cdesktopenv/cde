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
/* $TOG: help.c /main/5 1998/04/06 13:18:57 mgreess $ */
/*   Copyright (c) 1986, 1987, 1988, 1989 Hewlett-Packard Co. */
/* Miscellaneous Procedures for HP Tag/TeX translator */

#include "userinc.h"
#include "globdec.h"

#include "LocaleXlate.h"
#include "XlationSvc.h"

void assert_hometopic_exists(M_NOPAR)
{
static const char hometopic[] = "-HOMETOPIC";
char id[SDLNAMESIZ + sizeof(hometopic)];
char *string =
    GetDefaultHeaderString("UntitledElementDefaultHeadingString",
			   M_SDATA,
			   "Untitled");
if (nohometopic)
    {
    char    *pc;

    sprintf(id, "%s%s", sdlReservedName, hometopic);
    mb_starthelpnode("_HOMETOPIC", id, 1);
    nohometopic = FALSE;
    pc = NULL;
    if (savedtitle)
	pc = MakeMByteString(savedtitle);
    fprintf(outfile,
	    "<HEAD CLASS=\"HEAD\" ABBREV=\"%s\"></HEAD>\n</VIRPAGE>\n",
	    pc ? pc : string);
    if (pc)
	m_free(pc, "multi-byte string");
    }
m_free(string, "GetDefaultHeaderString return");
}

/* Determine base name for files (i.e., input name without extension).
   Open output and error files */
void fbasename(M_NOPAR)
{
char *p, *q;
int n;
char save;
char fileListErr[] = "filelist.err";

m_errfile = NULL;
if (m_argc < 2)
    {
    m_error("Specify input file");
    exit(TRUE);
    }

if ((m_argc > 2) && (strchr(m_argv[2], 'f') || strchr(m_argv[2], 'F')))
    filelist = TRUE;

/* Get installation directory */
#define CONTRIB "/usr/dt/bin/"

/* get our path if we can */
/* A hacked up ``which'', just to find our directory */
/* fills ``install'' with path to ourself */
{
char *path, *cp;
char buf[200];
char patbuf[BUFSIZ];
int quit, none;

quit = 0;
none = 1;

if ( *(m_argv[0]) == '/' )
    {
    /* fully qualified path to ourself was specified */
    if (access(m_argv[0],1) == 0)
	{
	/* if full path name exists and is executable */
	/* get the dirname */
	for (p = m_argv[0]; *p ; p++) ; /* end of string, (the hard way) */
	/* backup to dirsep */
	for (; ; p--)
	    {
	    if (p < m_argv[0]) m_error("Internal Error.");
	    if (*p == dirsep) break; 
	    }
	p++; /* just past the dirsep */
	save = *p;
	*p = M_EOS;
	install = (char *) m_malloc(strlen(m_argv[0]) + 1,
				    "installation directory");
	strcpy(install, m_argv[0]);
	*p = save;

	none = 0; /* we've got it. */
	}
    else
	{
	m_error("Internal Error (which).");
	}
    }
else
    {
    /* not fully specified, check each component of path for ourself */
    strcpy(patbuf, getenv("PATH"));
    path = patbuf;
    cp = path;

    while(1)
	{
	cp = strchr(path, ':');
	if (cp == NULL)
	quit++;
	else
	*cp = '\0';
	sprintf(buf, "%s/%s", path, m_argv[0]);

	if (access(buf, 1) == 0)
	    {
	    install = (char*) m_malloc(strlen(path) + 1,
				       "installation directory");
	    strcpy(install, path);
	    none = 0;
	    }
	/* else, not an error if we can't find a particular one. */

	path = ++cp;
	if (quit || !none)
	    {
	    break; /* either out of paths, or we found it. */
	    }
	}  /* end while */
    }

if (none)
    {
    /* can't get it, use default */
    install = (char *) m_malloc(strlen(CONTRIB) + 1,
				"installation directory");
    strcpy(install, CONTRIB);
    }
/* else -- we've got it */
}

/* Set default working directory (from input filename) */
for (p = strchr(m_argv[1], M_EOS); p > m_argv[1] ; p--)
    if (*(p - 1) == dirsep)
	break;
if (p > m_argv[1])
    {
    save = *p;
    *p = M_EOS;
    work = (char *) m_malloc(strlen(m_argv[1]) + 1, "working directory");
    strcpy(work, m_argv[1]);
    indir = (char *) m_malloc(strlen(m_argv[1]) + 1, "input directory");
    strcpy(indir, m_argv[1]);
    *p = save;
    }
else
    {
    indir = NULL;
    }

/* Build base name */
q = strchr(m_argv[1], M_EOS);
while ((q > m_argv[1]) && (*q != '.') && (*q != dirsep))
    q--;
defaultext = (LOGICAL) (*q != '.');
if (! defaultext) *q = M_EOS;
nodirbase = (char *) m_malloc(strlen(p) + 1,
			      "basename without directory");
strcpy(nodirbase, p);
/* Get working directory option, if specified */
n = strlen(p);
base = (char *) m_malloc(n + strlen(".htg") + 1, "basename");
strcpy(base, p);
base[n] = '.';
baseext = base + n + 1;
if (! defaultext)
    if (*(q + 1))
	*q = '.';

options(TRUE); /* pluck only length changing optins */
if (usingshortnames)
    {
    /* Build short versions of basename */
    /* set up global helpbase and helpext */
    helpbase = (char *) m_malloc(strlen(work)            +
				     BASENAME_LIMIT      +
				     PRE_EXTENSION_LIMIT +
				     strlen(".ext")      +
				     1,
				 "help basename");
    strcpy(helpbase, work);
    strncat(helpbase, nodirbase, BASENAME_LIMIT);
    helpext = helpbase + strlen(helpbase);
    }
else
    { /* Build long names */
    /* set up global helpbase and helpext */
    helpbase = (char *) m_malloc(strlen(work)            +
				     strlen(nodirbase)   +
				     PRE_EXTENSION_LIMIT +
				     strlen(".ext")      +
				     1,
				 "help basename");
    strcpy(helpbase, work);
    strcat(helpbase, nodirbase);
    helpext = helpbase + strlen(helpbase);
    }

/* Open error files */
if (filelist)
    {
    p = mb_malloc(strlen(work)+sizeof(fileListErr));
    strcpy(p, work);
    strcat(p, fileListErr);
    m_openchk(&m_errfile, p, "w");
    mb_free(&p);
    }
else
    {
    strcpy(helpext, ".err");
    m_openchk(&m_errfile, helpbase, "w");
    }
}


/* This procedure starts a CHAPTER */
void chapstart(id)
M_WCHAR *id;
{
M_WCHAR *p, *q, *wc;
int i;
char *mbyte, *pc;
int   length;

char *chapterPrefixString =
  GetDefaultHeaderString("ChapterElementDefaultHeadingString",
			 M_SDATA,
			 "Chapter");
char *chapterSuffixString =
  GetDefaultHeaderString("ChapterSuffixElementDefaultHeadingString",
			 M_SDATA,
			 "");

rsectseq = FALSE;
chapst = FALSE;
chapinc = 0;
chapter++;
m_itoa(chapter, chapstring);

savid = checkid(id);
iderr = FALSE;

figno = 0;
tableno = 0;
footnoteno = 1;
fprintf(stderr,
	"\n%s %s%s. ",
	chapterPrefixString,
	chapstring,
	chapterSuffixString);
m_free(chapterPrefixString, "GetDefaultHeaderString return");
if (*chapterSuffixString)
    m_free(chapterSuffixString, "GetDefaultHeaderString return");
}


/* Called at end of manual to report terms that occurred in the document
   but not entered in the glossary */
void checkgloss(M_NOPAR)
{
int n;
M_WCHAR id[MAXTERM + 1];
M_TRIE *node[MAXTERM + 1];
M_TRIE *current;

if (! gtree.data) return;
n = 0;
current = gtree.data;
while (TRUE)
    {
    id[n] = current->symbol;
    node[n] = current->next;
    if (! id[n])
	{
	if ((int) current->data >= 0)
	    m_err1("No glossary definition for %s", id);
	current = current->next;
	while (! current)
	    {
	    n--;
	    if (n < 0) return;
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

		
/* End Error Message macro \starterrmsg call, check to see if Error Message
   head is user specified or default */
void checkmsghead(M_NOPAR)
{
char *string =
    GetDefaultHeaderString("MessagesElementDefaultHeadingString",
			   M_SDATA,
			   "Messages");
if (emsghead == DEFHEAD)
    {  /* head not output yet */
    fprintf(outfile, "<HEAD CLASS=\"HEAD\">%s</HEAD>\n", string);
    emsghead = FALSE;
    }
else if (emsghead == USERHEAD)
    {  /* user specified head */
    emsghead = FALSE;
    }
m_free(string, "GetDefaultHeaderString return");
}  /* end checkmsghead() */


/* construct a qualified file name */
#if defined(M_PROTO)
static int mb_getqualified(char *qualname, char *unqualname)
#else
static int mb_getqualified(qualname, unqualname)
char *qualname;
char *unqualname;
#endif
{
#if defined(hpux) || defined(_AIX) || defined(sun) || defined(USL) || defined(__uxp__) || defined(__osf__) || defined(linux) || defined(CSRG_BASED)
FILE *f;
#endif
char fn[FNAMELEN];
char tokstr [ 20 ], *gp, *p, *pp, *fnp, curdir[FNAMELEN-1];
int roomleft = FNAMELEN - 1;

if (!unqualname)
    {
    *qualname = M_EOS;
    return( 0 );
    }

if (strlen(unqualname) < (size_t) FNAMELEN)
    strcpy(fn, unqualname );
else
    {
    m_mberr1("Internal Error. File name too long for qualifying: %s",
	     unqualname);
    return (-1);
    }

fnp = fn;

#if defined(hpux) || defined(_AIX) || defined(sun) || defined(USL) || defined(__uxp__) || defined(__osf__) || defined(linux) || defined(CSRG_BASED)
qualname[0] = '\0';
gp = qualname + strlen(qualname);
roomleft = roomleft - strlen(qualname);

/* if path is from root, tack that on, else tack on the current
 directory (for the referenced drive, if MS-DOS) */
/* We assume FNAMELEN is at least three (3), so no range checking here */
if ( *fnp == CSEP )
    {
    strcat(qualname, SSEP);
    roomleft--;
    ++fnp;
    }
else
    {
    f = popen("/bin/pwd", "r");
    fscanf(f, "%s", gp);
    pclose(f);
    if (strlen(gp) >= (size_t) roomleft)
	{
	m_mberr1("Internal error: possible stray pointer in getqualified(): %s",
		 gp);
	return(-1);
	}
    strcat(qualname,SSEP);
    roomleft--;
    }
#else
/* if MS-DOS, force to upper case, then get drive spec */
strupr ( fn );
if ( fn[1] == ':' ) {
strncpy ( qualname, fn, 2 );
fnp += 2;
}
else {
getcwd(qualname, roomleft);
}
qualname[2] = '\0';
gp = qualname + strlen ( qualname );
roomleft = roomleft - strlen ( qualname );
/* if path is from root, tack that on, else tack on the current
 directory (for the referenced drive, if MS-DOS) */
if ( *fnp == CSEP ) {
strcat ( qualname, SSEP );
roomleft--;
++fnp;
}
else {
/* assume current directory always !!! */
*gp = CSEP;
getcwd(curdir, FNAMELEN-1);
if (*curdir != *qualname) {
  m_err1("Relative directory %s for non-current drive, can't qualify",
	  unqualname);
  return (-1);
  }
if (strlen(curdir) > 3) {
  if ((strlen(curdir+3)+1) < roomleft) {  /* "1" for SSEP */
    strcpy( gp+1, curdir+3 );
    strcat ( qualname, SSEP );
    roomleft = roomleft - strlen(curdir+3) - 1;  /* "1" for SSEP */
    }
  else {
    m_err1("Internal error. File name too long for qualifying: %s",
      unqualname);
    return (-1);
    }
  }
}
#endif

strcpy(tokstr, " \r\n\t");
strcat(tokstr, SSEP);
p = NULL;
do  {
    p = strtok(( p == NULL ) ? fnp : NULL, tokstr);
    if ( p == NULL ) break;
    if ( *p == '.' ) /* alias */
	{
	if ( *(p+1) == '.' ) /* parent */
	    {
	    *strrchr(qualname, CSEP) = '\0';
	    pp = strrchr(qualname, CSEP);
	    if (pp == NULL) /* FAIL */
		{
		m_mberr1("Internal error. Failed in qualifying %s", unqualname);
		return ( -1 );
		}
	    else
		{
		*(pp+1) = '\0';
		}
	    }
	}
    else
	{
	if ((strlen(p)+1) < (size_t) roomleft)
	    {  /* "1" for SSEP */
	    strcat(qualname, p);
	    strcat(qualname, SSEP);
	    roomleft = roomleft - strlen(p) - 1;
	    }
	else
	    {
	    m_mberr1("Internal error. File name too long for qualifying: %s",
		     unqualname);
	    return (-1);
	    }
	}
    }
while (1);
*strrchr(qualname, CSEP) = '\0';

#if defined(hpux) || defined(_AIX) || defined(sun) || defined(USL) || defined(__uxp__) || defined(__osf__) || defined(linux) || defined(CSRG_BASED)
#else
strupr ( qualname );
#endif

return ( 0 );
}  /* end mb_getqualified */


int getqualified (qualname, unqualname)
M_WCHAR *qualname;
M_WCHAR *unqualname;
{
int retval;
char mb_qualname[FNAMELEN],
     mb_unqualname[FNAMELEN];

wcstombs(mb_qualname, qualname, FNAMELEN);
wcstombs(mb_unqualname, unqualname, FNAMELEN);

retval = mb_getqualified(mb_qualname, mb_unqualname);

mbstowcs(qualname, mb_qualname, FNAMELEN);
mbstowcs(unqualname, mb_unqualname, FNAMELEN);

return retval;
}


/* handle the common link and graphic code for <p> and <image> */
void
handle_link_and_graphic(parent,
			gentity,
			gposition,
			ghyperlink,
			glinktype,
			gdescription)
M_WCHAR *parent, *gentity, *gposition, *ghyperlink, *glinktype, *gdescription;
{
unsigned char etype, wheredef;
char *mb_content, *ssi, id[32];
static M_WCHAR empty = M_EOS;
char *leftright;

/* handle graphic specific code */
/* initialize some stuff first:
- file is the entity name,
- f_file is the content of the entity, used only if f_content nonNULL
- f_content is f_file with the relative pathname, initialized to NULL,
- f_contqual is fully qualified f_file, assigned ONLY IF
  f_content nonNULL
*/
file_ent = FALSE;
f_content = NULL;
f_contqual[0] = M_EOS;

/* get the position, default to left */
leftright = "LEFT";
if (gposition)
    {
    M_WCHAR *wc_left, *wc_right;

    wc_right = MakeWideCharString(QRIGHT);
    if (!m_wcupstrcmp(gposition, wc_right))
	{
	leftright = "RIGHT";
	}
    else
	{
	wc_left = MakeWideCharString(QLEFT);
	if (m_wcupstrcmp(gposition, wc_left))
	    {
	    m_err1("Invalid value for gposition: `%s'", gposition);
	    }
	m_free(wc_left,"wide character string");
	}
    m_free(wc_right,"wide character string");
    }

/* check ENTITY and determine the figure type  */
if (gentity)
    {
    m_lookent(gentity, &etype, &f_file, &wheredef);
    if (etype != M_SYSTEM)
	{
	M_WCHAR *wc_entsystem, *wc_entkw, *wc_stago, *wc_tagc;

	wc_entsystem = MakeWideCharString(m_entsystem);
	wc_entkw = MakeWideCharString(m_entkw);
	wc_stago = MakeWideCharString(m_stago);
	wc_tagc = MakeWideCharString(m_tagc);
	m_err6(
	    "%s not a %s %s, as required for the ENTITY parameter of %s%s%s",
	       gentity,
	       wc_entsystem,
	       wc_entkw,
	       wc_stago,
	       m_parent(0),
	       wc_tagc);
	m_free(wc_entsystem,"wide character string");
	m_free(wc_entkw,"wide character string");
	m_free(wc_stago,"wide character string");
	m_free(wc_tagc,"wide character string");
	}
    else
	{
	file_ent = TRUE;
	f_content = searchforfile(f_file);
	if (f_content)
	    {
	    if (getqualified(f_contqual, f_content))
		{
		/* unsuccessful qual */
		if (w_strlen(f_content) < FNAMELEN)
		    w_strcpy(f_contqual, f_content);
		else
		    {
		    m_err1("Internal error. File name too long: %s",
			   f_content);
		    m_exit(m_errexit);
		    }
		}
	    }
	else
	    {
	    m_err2("Can't find file %s (declared in entity %s)",
		   f_file,
		   gentity);
	    }
	}
    if (!f_content) f_content = &empty;

    mb_content = MakeMByteString(f_content);
    sprintf(id, "%s%d", sdlReservedName, NextId());
    ssi = MakeMByteString(m_parent(0));
    fprintf(outfile, "<HEAD SSI=\"%s-GRAPHIC-%s\">", ssi, leftright);
    if (ghyperlink)
	HandleLink(ghyperlink, glinktype, gdescription);
    fprintf(outfile, "<SNREF>\n<REFITEM RID=\"%s\" ", id);
    fputs("CLASS=\"FIGURE\"", outfile);
    AddToSNB(id, mb_content);
    m_free(mb_content,"multi-byte string");
    }

/* and finish the position now */
if (gposition)
    {
    fprintf(outfile, " SSI=\"GRPH-%s\"", leftright);
    }

if (gentity)
    {
    fputs("></REFITEM></SNREF>", outfile);
    if (ghyperlink)
	fputs("</LINK>", outfile);
    fputs("</HEAD>", outfile);
    }

if (!ghyperlink && (glinktype || gdescription))
    {
    m_eprefix();
    fprintf(stderr,
	   "Error: %sP%s ghyperlink was undefined.\n",
	   m_stago,
	   m_tagc);
    fprintf(m_errfile,
	   "Error: %sP%s ghyperlink was undefined.\n",
	   m_stago,
	   m_tagc);
    fprintf(stderr,
     "Beware of using a reserved value such as:\n%s, %s, %s, %s, %s, or %s.\n",
	   QJUMP,
	   QJUMPNEWVIEW,
	   QDEFINITION,
	   QEXECUTE,
	   QAPPDEFINED,
	   QMAN);
    fprintf(m_errfile,
     "Beware of using a reserved value such as:\n%s, %s, %s, %s, %s, or %s.\n",
	   QJUMP,
	   QJUMPNEWVIEW,
	   QDEFINITION,
	   QEXECUTE,
	   QAPPDEFINED,
	   QMAN);
    m_errline("Use ``ghyperlink='' if the value contains non-alphabetics");
    m_esuffix();
    }
}

/* Process an item in a list */
#if defined(M_PROTO)
void Item(M_WCHAR *id)
#else
void Item(id)
M_WCHAR *id;
#endif
{
char orderString[32];
static char *ROMAN0[] =
    {"", "I", "II", "III", "IV", "V", "VI", "VII", "VIII", "IX"};
static char *ROMAN10[] =
    {"", "X", "XX", "XXX", "XL", "L", "LX", "LXX", "LXXX", "XC"};
static char *ROMAN100[] =
    {"", "C", "CC", "CCC", "CD", "D", "DC", "DCC", "DCCC", "CM"};
static char ALPHABET[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
static char *roman0[] =
    {"", "i", "ii", "iii", "iv", "v", "vi", "vii", "viii", "ix"};
static char *roman10[] =
    {"", "x", "xx", "xxx", "xl", "l", "lx", "lxx", "lxxx", "xc"};
static char *roman100[] =
    {"", "c", "cc", "ccc", "cd", "d", "dc", "dcc", "dccc", "cm"};
static char alphabet[] = "abcdefghijklmnopqrstuvwxyz";
static char numbers[] = "0123456789";
static char romanString[]  = "ROMAN";
static char alphaString[]  = "ALPHA";
static char arabicString[] = "ARABIC";
int	 count, metaCount;
char	*item_id;
char	 label_id[SDLNAMESIZ+10];
int	 listtype;
char	*type;
char	*loose;
char	*first;
LOGICAL  isBullet, isLoose, isFirst;
int	*pThisBulletId;
char	 ssi[BIGBUF];

if (listitems[list].firstitem)
    {
    listitems[list].firstitem = FALSE;
    }

count = ++lastlist->lastlist->count;
if (count > 999) count = 999; /* holy cow!  Big list. */
isFirst = FALSE;
if (count == 1)
    {
    isFirst = TRUE;
    first = "FIRST-";
    }
else
    first = "";

isLoose = FALSE;
if (lastlist->lastlist->space == TIGHT)
    loose = "TIGHT";
else
    {
    isLoose = TRUE;
    loose = "LOOSE";
    }

isBullet = FALSE;
listtype = lastlist->lastlist->type;
if (listtype == BULLET)
    isBullet = TRUE;

if ((listtype != PLAIN) && (listtype != MILSPEC))
    { /* don't emit label for type==PLAIN and type==MILSPEC */
    if (isBullet)
	{
	if (isLoose)
	    {
	    if (isFirst)
		pThisBulletId = &firstLooseBulletId;
	    else
		{
		looseBulletIdIndex = (- looseBulletIdIndex) + 1;
		pThisBulletId = &looseBulletId[looseBulletIdIndex];
		}
	    }
	else
	    {
	    if (isFirst)
		pThisBulletId = &firstBulletId;
	    else
		{
		bulletIdIndex = (- bulletIdIndex) + 1;
		pThisBulletId = &bulletId[bulletIdIndex];
		}
	    }
	if (*pThisBulletId)
	    { /* use an existing block containing a bullet */
	    strcpy(label_id, sdlReservedName);
	    m_itoa(*pThisBulletId, label_id + SDLNAMESIZ - 1);
	    }
	else
	    {
	    *pThisBulletId = NextId();
	    strcpy(label_id, sdlReservedName);
	    m_itoa(*pThisBulletId, label_id + SDLNAMESIZ - 1);
	    if (needFData)
		{
		fputs("<FDATA>\n", outfile);
		needFData = FALSE;
		}
	    fprintf(outfile,
		    "<BLOCK ID=\"%s\" CLASS=\"ITEM\" TIMING=\"ASYNC\" ",
		    label_id);
	    fprintf(outfile,
		    "SSI=\"%s%s-BULLET\">\n<P><SPC NAME=\"[bull  ]\">",
		    first,
		    loose);
	    fputs("</P>\n</BLOCK>\n", outfile);
	    }
	}
    else
	{
	strcpy(label_id, sdlReservedName);
	m_itoa(NextId(), label_id + SDLNAMESIZ - 1);
	if (needFData)
	    {
	    fputs("<FDATA>\n", outfile);
	    needFData = FALSE;
	    }
	fprintf(outfile, "<BLOCK ID=\"%s\" CLASS=\"ITEM\" SSI=\"", label_id);
	}
    }

item_id = NULL;
if (listtype == ORDER)
    {
    orderString[0] = '\0';
    switch (lastlist->lastlist->order)
	{
	case UROMAN:
	    strcpy(orderString, ROMAN100[count / 100]);
	    strcat(orderString, ROMAN10[(count / 10) % 10]);
	    strcat(orderString, ROMAN0[count % 10]);
	    type = romanString;
	    break;
	case UALPHA:
	    metaCount = 1;
	    while ((count -= 26) > 0) metaCount++;
	    count = lastlist->lastlist->count;
	    if (count > 999) count = 999;
	    count -= 1;
	    count %= 26;
	    while (--metaCount >= 0) strncat(orderString, &ALPHABET[count], 1);
	    type = alphaString;
	    break;
	case ARABIC:
	    if (metaCount = (count / 100))
		strncat(orderString, &numbers[metaCount], 1);
	    if (metaCount || ((count / 10) % 10))
		strncat(orderString, &numbers[(count / 10) % 10], 1);
	    strncat(orderString, &numbers[count % 10], 1);
	    type = arabicString;
	    break;
	case LROMAN:
	    strcpy(orderString, roman100[count / 100]);
	    strcat(orderString, roman10[(count / 10) % 10]);
	    strcat(orderString, roman0[count % 10]);
	    type = romanString;
	    break;
	case LALPHA:
	    metaCount = 1;
	    while ((count -= 26) > 0) metaCount++;
	    count = lastlist->lastlist->count;
	    if (count > 999) count = 999;
	    count -= 1;
	    count %= 26;
	    while (--metaCount >= 0) strncat(orderString, &alphabet[count], 1);
	    type = alphaString;
	    break;
	}
    fprintf(outfile,
	    "%s%s-%s\">\n<P>%s%c",
	    first,
	    loose,
	    type,
	    orderString, 
	    lastlist->lastlist->punct == DOTPUNCT ? '.' : ')' );
    if (id)
	{
	char buffer[400];
	M_WCHAR *wc;

	sprintf(buffer, "Item %s", orderString);
	wc = MakeWideCharString(buffer);
	w_strcpy(xrefstring, wc);
	m_free(wc,"wide character string");

	xstrlen = w_strlen(xrefstring);
	m_getline(&xrffile, &xrfline);
	if (xrffile == NULL)
	    {
	    /* set to primary input */
	    xrffile = inputname;
	    }

	setid(id,
	     TRUE,
	     FALSE,
	     inchapter,
	     chapstring,
	     xrffile,
	     xrfline, TRUE);

	item_id = MakeMByteString(id);
	}
    }
else
    { /* Bullet, Check or Plain list */
    if (id) m_error("Cross-reference ID in non-ORDER list not allowed");
    /* already handled Bullet */
    if (listtype == CHECK)
	{
	fprintf(outfile,
		"%s%s-CHECK\">\n<P><SPC NAME=\"[check ]\">",
		first,
		loose);
	}
    /* don't emit label for listtype==PLAIN and listtype==MILSPEC */
    }

if ((listtype != PLAIN) && (listtype != MILSPEC) && (listtype != BULLET))
    fputs("</P>\n</BLOCK>\n", outfile);

sprintf(ssi, "%s%s", first, loose);
if ((listtype == PLAIN) || (listtype == MILSPEC))
    PushForm("ITEM", ssi, item_id);
else
    PushForm2("ITEM", ssi, label_id, item_id);

if (item_id)
    m_free(item_id, "multi-byte string");
}


/* Start a rsect */
void rsectstart(id)
M_WCHAR *id;
{
savid = checkid(id);
iderr = FALSE;
rsectseq = TRUE;
chapst = TRUE;
}  /* END procedure rsectstart  */


/* Called at the end of a chapter, appendix, or section to end an open
   sequence of rsects */
void rseqend(M_NOPAR)
{
if (rsectseq)
    --thisnodelevel;
rsectseq = FALSE;
}


/* Follow search path to find a file, returning qualified name */
M_WCHAR *searchforfile(file)
M_WCHAR *file;
{
M_WCHAR *filename;
SEARCH	*searchp;
char	*mb_file, mb_filename[2048];
int	dir_leng,file_leng;

mb_file = MakeMByteString(file);
if (! access(mb_file, READABLE))
    {
    filename = (M_WCHAR *)
	m_malloc(w_strlen(file) + 1, "figure/icon filename");
    w_strcpy(filename, file);
    m_free(mb_file,"multi-byte string");
    return(filename);
    }

file_leng = strlen(mb_file);
for (searchp = path; searchp ; searchp = searchp->next)
    {
    if (((dir_leng = strlen(searchp->directory)) + file_leng) >= 2048)
	{
	M_WCHAR *wc;

	wc = MakeWideCharString(searchp->directory);
	m_err2("%s and %s overflow the file name space", wc, file);
	m_free(wc,"wide character string");
	continue;
	}
    strcpy(mb_filename, searchp->directory);
    strcpy(mb_filename+dir_leng, mb_file);
    if (! access(mb_filename, READABLE))
	{
	m_free(mb_file,"multi-byte string");
	return(MakeWideCharString(mb_filename));
	}
    }

m_free(mb_file,"multi-byte string");
return(NULL);
}


/* Start a new helpnode */
#if defined(M_PROTO)
void starthelpnode(M_WCHAR *ssi,
		   M_WCHAR *id,
		   int	    level)
#else
void starthelpnode(ssi, id, level)
M_WCHAR *ssi;
M_WCHAR *id;
int	 level;
#endif
{
int i;
char *mbyte, *mb_ssi;
char mb_nodeid[NODEID_LENGTH+1], nodenum[32];

if (outfile != m_outfile)
    {
    m_error("Internal warning:  Outfile has been redirected.");
    m_errcnt--;
    }

fflush(m_outfile);

mb_ssi = MakeMByteString(ssi);

/* we can't use the bullet block across virpage boundaries */
bulletId[0]        = 0;
bulletId[1]        = 0;
looseBulletId[0]   = 0;
looseBulletId[1]   = 0;
firstBulletId      = 0;
firstLooseBulletId = 0;

/* might as well always start with id 0 (we flip first) */
bulletIdIndex      = 1;
looseBulletIdIndex = 1;

if ((id == NULL) || (*id == 0))
    {
    strcpy(mb_nodeid, sdlReservedName);
    m_itoa(NextId(), nodenum);
    strcpy(mb_nodeid + SDLNAMESIZ - 1, nodenum);
    mbstowcs(nodeid, mb_nodeid, NODEID_LENGTH);
    }
else
    {
    w_strncpy(nodeid, id, NODEID_LENGTH);
    wcstombs(mb_nodeid, id, NODEID_LENGTH);
    }

fprintf(outfile,
"<VIRPAGE ID=\"%s\" LEVEL=\"%d\" LANGUAGE=\"%s\" CHARSET=\"%s\" DOC-ID=\"%s\" SSI=\"%s\">\n",
	mb_nodeid,
	level,
	GetLanguage(),
	GetCharset(),
	docId,
	mb_ssi);
m_free(mb_ssi,"multi-byte string");
snbstart = ftell(outfile);
} /* End starthelpnode(id) */


#if defined(M_PROTO)
void mb_starthelpnode(char   *ssi,
		      char   *id,
		      int     level)
#else
void mb_starthelpnode(ssi, id, level)
char   *ssi;
char   *id;
int	level;
#endif
{
M_WCHAR *wc_ssi, *wc_id;

wc_ssi = NULL;
if (ssi)
    wc_ssi = MakeWideCharString(ssi);
wc_id = NULL;
if (id)
    wc_id  = MakeWideCharString(id);
starthelpnode(wc_ssi, wc_id, level);
if (wc_ssi)
    m_free(wc_ssi,"wide character string");
if (wc_id)
    m_free(wc_id,"wide character string");
}


/* Start a labeled list */
void StartLabList(spacing, longlabel)
M_WCHAR *spacing;
M_WCHAR *longlabel;
{
       char *mb_spacing;
static char  def_spacing[] = "LOOSE";
       char  ssi[32];

if (list >= MAXLISTLEV)
    m_error("Nesting of <LIST> and <LABLIST> too deep");

if (spacing)
    mb_spacing = MakeMByteString(spacing);
else
    mb_spacing = def_spacing;

sprintf(ssi, "LABELED-%s", mb_spacing);
PushForm("LIST", ssi, NULL);

if (mb_spacing != def_spacing)
    mb_free(&mb_spacing);

list++;

listitems[list].firstitem = TRUE;
listitems[list].longlabel = vlonglabel(longlabel);

if (list <= MAXLISTLEV)
    {
    if (vspacing(spacing) == TIGHT)
	{
	lablisttight[list] = TRUE;
	}
    else
	{
	lablisttight[list] = FALSE;
	}
    }
}


/* Start a list */
#if defined(M_PROTO)
void StartList(M_WCHAR *type,
	       M_WCHAR *order,
	       M_WCHAR *spacing,
	       M_WCHAR *cont)
#else
void StartList(type, order, spacing, cont)
M_WCHAR *type, *order, *spacing, *cont;
#endif
{
LIST *nextlist;
CONTCHAIN *chain;
CONTCHAIN *xchain;
M_WCHAR *wc;
char *mb_spacing;
static char def_spacing[]  = "LOOSE";
char *list_type;
char  ssi[BIGBUF];

if (spacing)
    {
    mb_spacing = MakeMByteString(spacing);
    }
else
    {
    mb_spacing = def_spacing;
    }

list++;

if (list > MAXLISTLEV)
    m_error("Nesting of <LIST> and <LABLIST> too deep");

if (! lastlist->started && cont)
    {
    m_error("No previous list to continue");
    cont = NULL;
    }

if (cont)
    {
    if (order && (vordertype(order) != lastlist->order))
	{
	m_error("Unable to continue a list and change the numbering scheme");
	cont = NULL;
	}

    if (type && (vtype(type) != lastlist->type))
	{
	m_error("Unable to continue a list and change its type");
	cont = NULL;
	}
    }

if (! cont)
    { /* clear old list? */
    for (chain = lastlist->chain; chain ; chain = xchain)
	{
	xchain = chain->next;
	m_free(chain, "list chain");
	}
    lastlist->chain = NULL;
    lastlist->count = 0;
    }

/* If outermost list, initialize according to current defaults */
if (! cont && lastlist == &outlist)
    {
    outlist.type       = vtype(NULL);
    outlist.order      = vordertype(NULL);
    outlist.count      = 0;
    outlist.space      = vspacing(NULL);
    outlist.punct      = DOTPUNCT;
    }

if (type) lastlist->type = vtype(type);

if (order)
    {
    lastlist->type = ORDER;
    lastlist->order = vordertype(order);
    }

switch (lastlist->type)
    {
    case MILSPEC:
    case PLAIN:
	list_type = "PLAIN";
	break;
    case CHECK:
	list_type = "CHECK";
	break;
    case ORDER:
	list_type = "ORDER";
	break;
    case BULLET:
    default:
	list_type = "BULLET";
    }
sprintf(ssi, "%s-%s", list_type, mb_spacing);

PushForm("LIST", ssi, NULL);

if (mb_spacing != def_spacing)
    m_free(mb_spacing, "multi-byte string");


lastlist->space = LOOSE;
if (spacing && ! m_wcmbupstrcmp(spacing, QTIGHT)) lastlist->space = TIGHT;

if (type && order && m_wcmbupstrcmp(type, QORDER))
  m_err2("Incompatible specification for list: %s and %s", type, order);

if (lastlist->type == ORDER)
    {
    chain = (CONTCHAIN *) m_malloc(sizeof(CONTCHAIN), "list chain");
    chain->next = lastlist->chain;
    lastlist->chain = chain;
    chain->where = ftell(outfile);
    }

lastlist->started         = TRUE;
listitems[list].firstitem = TRUE;

/* Prepare for sublist */
nextlist = (LIST *) m_malloc(sizeof(LIST), "list structure");
nextlist->lastlist = lastlist;
nextlist->type = lastlist->type;
nextlist->punct = lastlist->punct;
if (lastlist->type == ORDER)
    {
    nextlist->order = lastlist->order + 1;
    if (nextlist->order > LROMAN)
	{
	nextlist->order = ARABIC;
	nextlist->punct = PARENPUNCT;
	}
    }
else nextlist->order = lastlist->order;
nextlist->count = 0;
nextlist->space = lastlist->space;
nextlist->started = FALSE;
nextlist->where = FIRST;
nextlist->chain = NULL;
lastlist = nextlist;
}


void EndList()
{
LIST *curlist ;
CONTCHAIN *chain, *xchain ;
char *ncols;

curlist = lastlist->lastlist ;

if ((curlist->type == PLAIN) || (curlist->type == MILSPEC))
    PopForm();
else
    PopForm2();

list-- ;

for (chain = lastlist->chain ; chain ; chain = xchain)
    {
    xchain = chain->next ;
    m_free(chain, "list chain") ;
    }
m_free(lastlist, "list structure") ;
lastlist = curlist ;
for (chain = lastlist->chain ; chain ; chain = chain->next)
    {
    if (lastlist->count > 999)
	{
	m_error("Unable to support more than 999 items in an ordered list") ;
	lastlist->count = 999 ;
	}
    }
if (lastlist->type == BULLET) bulcount-- ;
}


/* Open and initialize TeX file */
void texinit(M_NOPAR)
{
LOGICAL init = TRUE;
unsigned char type;
M_WCHAR *content;
unsigned char wheredef;
M_WCHAR *name;
M_WCHAR *qfile;
LOGICAL icon;
SEARCH *searchp;
char *p;
int ic;
int i;
M_WCHAR wsl;
M_WCHAR *wc;

/* Check .XRF file */
strcpy(helpext, ".xrh");
xrf = fopen(helpbase, "r");
if (! xrf) rebuild = TRUE;
else
    {
    fscanf(xrf, "\\gobble\001");
    for (p = m_signon; *p ; p++)
	{
	ic = getc(xrf);
	if (ic != (int) *p)
	    {
	    m_errline(
	"Output files from different version of Tag, regenerating. . .\n");
	    fclose(xrf);
	    xrf = NULL;
	    rebuild = TRUE;
	    break;
	    }
	}
    }
if (xrf)
    while ((ic = getc(xrf)) != EOF)
	if (ic == '\n') break;
if (xrf && ic !=EOF)
    while ((ic = getc(xrf)) != EOF)
	if (ic == '\n') break;

rebuild = TRUE;

/* Open output files */
strcpy(helpext, ".sdl");
m_openchk(&m_outfile, helpbase, "w");
outfile = m_outfile;

savehelpfilename = (M_WCHAR *)m_malloc(strlen(helpbase)+1, "help file name");
mbstowcs(savehelpfilename, helpbase, strlen(helpbase) + 1);

/* index file */
strcpy(helpext, ".idx");
m_openchk(&indexfp, helpbase, "wb");

/* system notation block file */
strcpy(helpext, ".snb");
m_openchk(&snbfp, helpbase, "wb");

while (name = m_cyclent(init, &type, &content, &wheredef))
    {
    init = FALSE;
    qfile = NULL;
    icon = FALSE;

    if (type == M_SYSTEM)
	qfile = searchforfile(content);

    if (qfile) m_free(qfile, "figure filename");

    if (type == M_SYSTEM)
	{
	mbtowc(&wsl, "/", 1);

	if (w_strchr(content, wsl))
	    {
	    m_err2("Avoid directory names in FILE entity %s: %s",
		   name,
		   content);
	    m_errline("(Use the SEARCH option instead)\n");
	    }
	}
    }

/* Include cross-reference file */
strcpy(helpext, ".xrh");
loadxref();
postpreamble = ftell(outfile);
}


/* Lookup localized header string entity as defined (by default) in
   locallang.ent.  If the the header string was not found, or it was
   not of type "desiredType", return the default.

   If the entity is of type file (M_SYSTEM) then if the content is not
   empty search the path for the file.  If the file is found, return
   its name else return an empty string.

   If this routine returns anything other than the empty string (""),
   the string returned must be m_free'd.
*/
char *
#if defined(M_PROTO)
GetDefaultHeaderString(
    char          *elementName,
    unsigned char  desiredType,
    char          *defaultString )
#else
GetDefaultHeaderString(elementName, desiredType, defaultString)
char	      *elementName;
unsigned char  desiredType;
char	      *defaultString;
#endif
{
unsigned char type,wheredef;
M_WCHAR *content;
M_WCHAR *path;
M_WCHAR *wc_elementName;
char	*mb_content;
char	*retval;

wc_elementName = MakeWideCharString(elementName);
if (m_lookent(wc_elementName, &type, &content, &wheredef))
    {
    if (type == desiredType)
	{
	if (type == M_SDATA)
	    {
	    m_free(wc_elementName,"wide character string");
	    mb_content = MakeMByteString(content);
	    if (!*mb_content)
		{
		m_free(mb_content,"multi-byte string");
		return "";
		}
	    else
		return mb_content;
	    }
	if (*content)
	    {
	    path = searchforfile(content);
	    if (!path)
		{
		m_err2("Can't find file %s (declared in entity %s)",
		       content,
		       wc_elementName);
		m_free(wc_elementName,"wide character string");
		return("");
		}
	    else
		{
		m_free(wc_elementName,"wide character string");
		return MakeMByteString(path);
		}
	    }
	m_free(wc_elementName,"wide character string");
	return "";
	}
    }

m_free(wc_elementName,"wide character string");
if (*defaultString)
    {
    retval = m_malloc(strlen(defaultString) + 1,
		      "GetDefaultHeaderString return");
    return strcpy(retval, defaultString);
    }

return "";
}


/* A function that takes a language/charset pair and:
 *     if they are standard, leave them unchanged but get local
 *                           versions and setlocale(3) using those
 *     if they are local, setlocale(3) with them and replace them with
 *			  standard versions.
*/
static void
#if defined(M_PROTO)
SetStdLocale(char *pLang, char *pCharset)
#else
SetStdLocale(pLang, pCharset)
char *pLang;
char *pCharset;
#endif
{
static const char *cString   = "C";
static const char *isoString = "ISO-8859-1";
_DtXlateDb  myDb = NULL;
char        myPlatform[_DtPLATFORM_MAX_LEN+1];
char        myLocale[256]; /* arbitrarily large */
char       *locale;
char       *lang;
char       *charset;
int         execVer;
int         compVer;
int         isStd;

strcpy(myLocale, pLang);
if (*pCharset)
    {
    strcat(myLocale, ".");
    strcat(myLocale, pCharset);
    }

if ((_DtLcxOpenAllDbs(&myDb) != 0) ||
    (_DtXlateGetXlateEnv(myDb,myPlatform,&execVer,&compVer) != 0))
    {
    fprintf(stderr,
            "Warning: could not open locale translation database.\n");
    if (m_errfile != stderr)
	fprintf(m_errfile,
		"Warning: could not open locale translation database.\n");
    strcpy(pLang, cString);
    strcpy(pCharset, isoString);
    if (myDb != 0)
	_DtLcxCloseDb(&myDb);
    return;
    }

isStd = !_DtLcxXlateOpToStd(myDb,
		            "CDE",
		            0,
		            DtLCX_OPER_STD,
		            myLocale,
		            NULL,
		            NULL,
		            NULL,
		            NULL);

if (isStd)
    { /* already standard - get local versions and set locale */
    if (_DtLcxXlateStdToOp(myDb,
		           myPlatform,
		           compVer,
		           DtLCX_OPER_SETLOCALE,
		           myLocale,
		           NULL,
		           NULL,
		           NULL,
		           &locale))
	{
	fprintf(stderr,
		"Warning: could not translate CDE locale to local\n");
	if (m_errfile != stderr)
	    fprintf(m_errfile,
		    "Warning: could not translate CDE locale to local\n");
	strcpy(pLang, cString);
	strcpy(pCharset, isoString);
	_DtLcxCloseDb(&myDb);
	return;
	}
    else
	{
	setlocale(LC_CTYPE, locale);
	mb_free(&locale);
	}
    }
else
    { /* already local - set locale and get standard versions */
    if (_DtLcxXlateOpToStd(myDb,
		           myPlatform,
		           compVer,
		           DtLCX_OPER_SETLOCALE,
		           myLocale,
		           NULL,
		           &lang,
		           &charset,
		           NULL))
	{
	fprintf(stderr,
		"Warning: could not translate local locale to CDE\n");
	if (m_errfile != stderr)
	    fprintf(m_errfile,
		    "Warning: could not translate local locale to CDE\n");
	strcpy(pLang, cString);
	strcpy(pCharset, isoString);
	_DtLcxCloseDb(&myDb);
	return;
	}
    setlocale(LC_CTYPE, myLocale);

    if (*lang)
	{
	strcpy(pLang, lang);
	mb_free(&lang);
	}
    else
	strcpy(pLang, cString);

    if (*charset)
	{
	strcpy(pCharset, charset);
	mb_free(&charset);
	}
    else
	strcpy(pCharset, isoString);
    }

_DtLcxCloseDb(&myDb);
}


/* A function that takes the return value from a call to setlocale()
 * and extracts the langterr.charset data from it in a vendor neutral
 * fashion.
*/
static char *
#if defined(M_PROTO)
GetStdLocale()
#else
GetStdLocale()
#endif
{
static char buffer[256];
static char *cString   = "C";
_DtXlateDb  myDb = NULL;
char        myPlatform[_DtPLATFORM_MAX_LEN+1];
char       *opLocale;
char       *stdLocale;
int         execVer;
int         compVer;

if ((_DtLcxOpenAllDbs(&myDb) == 0) &&
    (_DtXlateGetXlateEnv(myDb,myPlatform,&execVer,&compVer) != 0))
    {
    fprintf(stderr,
            "Warning: could not open locale translation database.\n");
    if (m_errfile != stderr)
	fprintf(m_errfile,
		"Warning: could not open locale translation database.\n");
    return cString;
    }

if (_DtLcxXlateOpToStd(myDb,
		       myPlatform,
		       compVer,
		       DtLCX_OPER_SETLOCALE,
		       setlocale(LC_ALL, NULL),
		       &stdLocale,
		       NULL,
		       NULL,
		       NULL))
    {
    fprintf(stderr,
	    "Warning: could not translate local locale to CDE\n");
    if (m_errfile != stderr)
	fprintf(m_errfile,
		"Warning: could not translate local locale to CDE\n");
    _DtLcxCloseDb(&myDb);
    return cString;
    }

if (_DtLcxXlateStdToOp(myDb,
		       myPlatform,
		       compVer,
		       DtLCX_OPER_SETLOCALE,
		       stdLocale,
		       NULL,
		       NULL,
		       NULL,
		       &opLocale))
    {
    fprintf(stderr,
	    "Warning: could not translate CDE locale to local\n");
    if (m_errfile != stderr)
	fprintf(m_errfile,
		"Warning: could not translate CDE locale to local\n");
    mb_free(&stdLocale);
    _DtLcxCloseDb(&myDb);
    return cString;
    }

_DtLcxCloseDb(&myDb);

strcpy(buffer, opLocale);
mb_free(&stdLocale);
mb_free(&opLocale);
return buffer;
}


/*
 * Look for a entities by the name of "LanguageElementDefaultLocale".
 * and "LanguageElementDefaultCharset".  If not found, get the user's
 * locale.  If LanguageElementDefaultCharset was set, use that in
 * place of the charset of the local (if any).  Call SetStdLocale()
 * to insure the language and charset are in the normalized form.
 * SetStdLocale() will also set the current locale to the local
 * versions of the language and charset.
*/
void
SetDefaultLocale()
{
unsigned char type,wheredef;
M_WCHAR *elementName;
M_WCHAR *content;
char	*locale;
char    *charset;
char    *dotPtr;
char    *tmpStr;
char     stdLang[256];    /* arbitrarily large */
char     stdCharset[256]; /* arbitrarily large */

locale        = NULL;
charset       = NULL;
dotPtr        = NULL;
stdLang[0]    = 0;
stdCharset[0] = 0;

elementName = MakeWideCharString("LanguageElementDefaultLocale");
if (m_lookent(elementName, &type, &content, &wheredef))
    {
    if (type == M_SDATA)
	{
	locale = MakeMByteString(content);
	}
    }
m_free(elementName,"wide character string");

if (!locale)
    {
    tmpStr = GetStdLocale();
    locale = mb_malloc(strlen(tmpStr)+1);
    strcpy(locale, tmpStr);
    }

dotPtr = strchr(locale, '.');

if (helpcharset)
    {
    charset = MakeMByteString(helpcharset);
    m_free(helpcharset, "help charset");
    helpcharset = NULL;
    }
else
    {
    elementName = MakeWideCharString("LanguageElementDefaultCharset");
    if (m_lookent(elementName, &type, &content, &wheredef))
	{
	if (type == M_SDATA)
	    {
	    charset = MakeMByteString(content);
	    }
	}
    m_free(elementName,"wide character string");
    }

if (!charset)
    {
    if (dotPtr)
	charset = dotPtr + 1;
    }
if (dotPtr)
    *dotPtr = NULL;

strcpy(stdLang, locale);
if (charset)
    strcpy(stdCharset, charset);
SetStdLocale(stdLang, stdCharset);

if (*stdCharset)
    helpcharset = MakeWideCharString(stdCharset);
helplang = MakeWideCharString(stdLang);

mb_free(&locale);
if (charset && (charset != (dotPtr+1)))
    mb_free(&charset);
}


#if defined(M_PROTO)
void paragraph(M_WCHAR *indent,
	       M_WCHAR *id,
	       M_WCHAR *gentity,
	       M_WCHAR *gposition,
	       M_WCHAR *ghyperlink,
	       M_WCHAR *glinktype,
	       M_WCHAR *gdescription)
#else
void paragraph(indent,
	       id,
	       gentity,
	       gposition,
	       ghyperlink,
	       glinktype,
	       gdescription)
M_WCHAR *indent;
M_WCHAR *id;
M_WCHAR *gentity;
M_WCHAR *gposition;
M_WCHAR *ghyperlink;
M_WCHAR *glinktype;
M_WCHAR *gdescription;
#endif
{
char *firstString, *indentString;

if (id)
    {
    savid = checkid(id);
    }
chapst = TRUE;
inSdlP = TRUE;
if (!inBlock)
    StartBlock(NULL, NULL, NULL);
fputs("<P", outfile);
if (id)
    {
    char *mb_id;

    mb_id = MakeMByteString(id);
    fprintf(outfile, " ID=\"%s\"", mb_id);
    m_free(mb_id,"multi-byte string");
    }

firstString = "";
if (firstPInBlock)
    {
    firstString = "1";
    firstPInBlock = FALSE;
    }

indentString = "";
if (indent)
    indentString = "-INDENT";

fprintf(outfile, " SSI=\"P%s%s\">", firstString, indentString);

handle_link_and_graphic(m_parent(0),
			gentity,
			gposition,
			ghyperlink,
			glinktype,
			gdescription);

}

#if defined(M_PROTO)
void figure(
       M_WCHAR *number,
       M_WCHAR *tonumber,
       M_WCHAR *id,
       M_WCHAR *file,
       M_WCHAR *figpos,
       M_WCHAR *cappos,
       M_WCHAR *ghyperlink,
       M_WCHAR *glinktype,
       M_WCHAR *gdescription)
#else
void figure(
       number,
       tonumber,
       id,
       file,
       figpos,
       cappos,
       ghyperlink,
       glinktype,
       gdescription)
M_WCHAR *number;
M_WCHAR *tonumber;
M_WCHAR *id;
M_WCHAR *file;
M_WCHAR *figpos;
M_WCHAR *cappos;
M_WCHAR *border;
M_WCHAR *ghyperlink;
M_WCHAR *glinktype;
M_WCHAR *gdescription;
#endif
{
char snb_id[32];
unsigned char etype,wheredef;
char *string = 
    GetDefaultHeaderString("FigureElementDefaultHeadingString",
			   M_SDATA,
			   "Figure");

if (needFData)
    {
    fputs("<FDATA>\n", outfile);
    needFData = FALSE;
    }
if (!inBlock)
    StartBlock(NULL, NULL, NULL);

inSdlP = TRUE;
fputs("<P SSI=\"FIGURE\">\n", outfile);

if (ghyperlink)
    {
    HandleLink(ghyperlink, glinktype, gdescription);
    }
else if (glinktype || gdescription)
    {
    m_eprefix();
    fprintf(stderr,
	    "Error: %sP%s ghyperlink was undefined.\n",
	    m_stago,
	    m_tagc);
    fprintf(m_errfile,
	    "Error: %sP%s ghyperlink was undefined.\n",
	    m_stago,
	    m_tagc);
    fprintf(stderr,
    "Beware of using a reserved value such as:\n%s, %s, %s, %s, %s, or %s.\n",
	    QJUMP,
	    QJUMPNEWVIEW,
	    QDEFINITION,
	    QEXECUTE,
	    QAPPDEFINED,
	    QMAN);
    fprintf(m_errfile,
    "Beware of using a reserved value such as:\n%s, %s, %s, %s, %s, or %s.\n",
	    QJUMP,
	    QJUMPNEWVIEW,
	    QDEFINITION,
	    QEXECUTE,
	    QAPPDEFINED,
	    QMAN);
    m_errline("Use ``ghyperlink='' if the value contains non-alphabetics");
    m_esuffix();
    }
/* end of link specific code */

figcaption = FALSE;
ftonumber = (!tonumber || (vnumber(tonumber) == NUMBER));
if (id && ! ftonumber)
    {
    m_error("Figures with ID's must be numbered");
    ftonumber = NUMBER;
    }
if (ftonumber) figno++;

if (number)
    {
    char *mb_number;

    mb_number = MakeMByteString(number);
    figno = atoi(mb_number);
    m_free(mb_number,"multi-byte string");
    if (! ftonumber)
    m_err1("Explicit figure number %s inconsistent with NONUMBER option",
	   number);
    }
svheadlen = 0;
*savehead = M_EOS;
if (id)
    {
    char mb_xrefstring[400];

    sprintf(mb_xrefstring, "%s %d", string, figno);
    mbstowcs(xrefstring, mb_xrefstring, 400);
    xstrlen = w_strlen(xrefstring);
    m_getline(&xrffile, &xrfline);
    if (xrffile == NULL)
	{
	/* set to primary input source */
	xrffile = inputname;
	}
    setid(id, TRUE, FALSE, inchapter, chapstring, xrffile, xrfline, TRUE);
    }

/* initialize some stuff first:
- file is the entity name,
- f_file is the content of the entity,
used only if f_content nonNULL
- f_content is f_file with the relative pathname, initialized to NULL,
- f_contqual is fully qualified f_file, assigned ONLY IF
f_content nonNULL
*/
file_ent = FALSE;
f_content = NULL;
f_contqual[0] = M_EOS;

/* check ENTITY and determine the figure type  */
if (file)
    {
    m_lookent(file, &etype, &f_file, &wheredef);
    if (etype != M_SYSTEM)
	{
	M_WCHAR *wc_stago, *wc_tagc;
	M_WCHAR *wc_entsystem, *wc_entkw;

	wc_stago = MakeWideCharString(m_stago);
	wc_tagc  = MakeWideCharString(m_tagc);
	wc_entsystem  = MakeWideCharString(m_entsystem);
	wc_entkw  = MakeWideCharString(m_entkw);
	m_err6("%s not a %s %s, as required for the ENTITY parameter of %s%s%s",
	       file,
	       wc_entsystem,
	       wc_entkw,
	       wc_stago,
	       m_parent(0),
	       wc_tagc);
	m_free(wc_stago,"wide character string");
	m_free(wc_tagc,"wide character string");
	m_free(wc_entsystem,"wide character string");
	m_free(wc_entkw,"wide character string");
	}
    else
	{
	file_ent = TRUE;
	f_content = searchforfile(f_file);
	if (f_content)
	    {
	    if (getqualified(f_contqual, f_content))
		{
		/* unsuccessful qual */
		if (w_strlen(f_content) < FNAMELEN)
		    w_strcpy(f_contqual, f_content);
		else
		    {
		    m_err1("Internal error. File name too long: %s",
			   f_content);
		    m_exit(m_errexit);
		    }
		}
	    }
	else
	    {
	    m_err2("Can't find file %s (declared in entity %s)",
		   f_file,
		   file);
	    }
	}
    }

sprintf(snb_id, "%s%d", sdlReservedName, NextId());

{
static M_WCHAR empty = M_EOS;
char *mb_content;

if (!f_content) f_content = &empty;

mb_content = MakeMByteString(f_content);
AddToSNB(snb_id, mb_content);
m_free(mb_content,"multi-byte string");
}

fputs("<SNREF", outfile);

if (id)
    {
    char *mb_id;

    mb_id = MakeMByteString(id);
    fprintf(outfile, " ID=\"%s\"", mb_id);
    m_free(mb_id,"multi-byte string");
    }
fprintf(outfile, ">\n<REFITEM RID=\"%s\" CLASS=\"FIGURE\" SSI=\"FIG", snb_id);


/* if caption position is not specified, have it track the figure position */
if (!cappos)
    cappos = figpos;

if (figpos)
    switch (vcenter(figpos))
	{
	case LEFT:
	    fputs("-LEFT", outfile);
	    break;
	case CENTER:
	    fputs("-CENTER", outfile);
	    break;
	case RIGHT:
	    fputs("-RIGHT", outfile);
	    break;
	}
fputs("\">\n", outfile);

capposition = "";
if (cappos)
    switch (vcenter(cappos))
	{
	case LEFT:
	    capposition = "-LEFT";
	    break;
	case CENTER:
	    capposition = "-CENTER";
	    break;
	case RIGHT:
	    capposition = "-RIGHT";
	    break;
	}

if (ftonumber)
    {
    fprintf(outfile,
	    "<HEAD TYPE=\"LINED\" CLASS=\"CAPTION\" SSI=\"NUMBER%s\">%s %d.",
	    capposition,
	    string,
	    figno);
    }
m_free(string, "default header string return");
}

#if defined(M_PROTO)
void endterm(M_WCHAR *base, M_WCHAR *gloss, char *linktype)
#else
void endterm( base, gloss, linktype)
M_WCHAR *base;
M_WCHAR *gloss;
char	*linktype;
#endif
{
M_WCHAR *p;
M_WCHAR  dterm[MAXTERM+1];
M_WCHAR  wnl, wsp;
char     buffer[BIGBUF];
int	 idn;

if (!(m_mblevel("HEAD")       ||
      m_mblevel("EXAMPLESEG") ||
      m_mblevel("ANNOTATION")
     )
   )
    {
    /* Don't re-enable output yet if in a <HEAD>, <EX> or <ANNOTATION> */
    tooutput = TRUE;
    }

mbtowc(&wnl, "\n", 1);
mbtowc(&wsp, " ", 1);

/* terminate the term string */
if (termp - term > MAXTERM) termp = &term[MAXTERM];
*termp = M_EOS;

/* make a copy - will transform to match definition transformations */
w_strcpy(dterm, term);
termp = dterm + (termp - term);

/* Check if it should be entered into the glossary structure */
if (vgloss(gloss) == GLOSS)
    {
    /* strip possible newlines out of base form */
    if (base)
	{
	for (p = base; *p ; p++)
	    {
	    if ((p - base) >= MAXTERM)
		{
		if ((p - base) == MAXTERM)
		    {
		    M_WCHAR *wc_stago, *wc_tagc;

		    wc_stago = MakeWideCharString(m_stago);
		    wc_tagc  = MakeWideCharString(m_tagc);
		    m_err2("Too many characters in BASEFORM for %sTERM%s",
			   wc_stago,
			   wc_tagc);
		    m_free(wc_stago,"wide character string");
		    m_free(wc_tagc,"wide character string");

		    *p = M_EOS;
		    }
		}
	    else
		{
		if (*p == wnl) *p = wsp;
		}
	    }
	}

    /* strip possible newlines out of dterm */
    for (p = dterm; *p ; p++)
	{
	if (*p == wnl) *p = wsp;
	}

    /* trim possible last space */
    if (termp - dterm > 1 && *(termp-1) == wsp)
	{
	*(termp-1) = M_EOS;
	}

    idn = (int) m_lookfortrie(base ? base : dterm, &gtree);
    if (idn < 0)
	idn = -idn;
    if (!idn)
	{
	idn = NextId();
	m_ntrtrie(base ? base : dterm, &gtree, (void *) idn);
	}
    }

/* Handle the output */
if (vgloss(gloss) == GLOSS)
    {
    sprintf(buffer,
	    "<LINK WINDOW=\"%s\" RID=\"%s%d\">",
	    linktype,
	    sdlReservedName,
	    idn);
    mb_strcode(buffer, outfile);
    }

mb_strcode("<KEY CLASS=\"TERM\">", outfile);

#if 0
if (m_mblevel("EXAMPLESEG"))
    {
    /* Are we in one of these?  May need to number a line. */
    for(p = term; *p; p++)
	{
	exoutchar(*p);
	}
    }
else
#endif
    multi_cr_flag = FALSE;
    strcode(term, outfile);

mb_strcode("</KEY>", outfile);

if (vgloss(gloss) == GLOSS)
    {
    mb_strcode("</LINK>", outfile);
    }

if (echo) mb_echohead("++");
}

#if defined(M_PROTO)
M_WCHAR wc_toupper(M_WCHAR wc)
#else
M_WCHAR wc_toupper(wc)
M_WCHAR wc;
#endif
{
if ((wc >= 0) && (wc <= 255))
    {
    return _toupper(wc);
    }
return wc;
}

#if defined(M_PROTO)
M_WCHAR *wc_stringtoupper(M_WCHAR *wcp)
#else
M_WCHAR *wc_stringtoupper(wcp)
M_WCHAR *wcp;
#endif
{
M_WCHAR *newstring, *nsp;

nsp = newstring =
    m_malloc(w_strlen(wcp) + 1, "wide character upper case string");

while (*wcp)
    {
    *nsp = wc_toupper(*wcp);
    nsp++;
    wcp++;
    }
*nsp = 0;

return(newstring);
}

int NextId()
{
static id = 0;

return ++id;
}

char *GetLanguage()
{
static char *pLang = NULL;

if (!pLang)
    {
    pLang = MakeMByteString(helplang);
    }

return pLang;
}

char *GetCharset()
{
static char *pCharset = NULL;

if (!pCharset)
    {
    pCharset = MakeMByteString(helpcharset);
    }

return pCharset;
}


#if defined(M_PROTO)
void HandleLink(M_WCHAR *hyperlink, M_WCHAR *type, M_WCHAR *description)
#else
void HandleLink(hyperlink, type, description)
M_WCHAR *hyperlink;
M_WCHAR *type;
M_WCHAR *description;
#endif
{
char *mb_hyperlink, mb_undefined[64];
char buffer[BIGBUF];
static M_WCHAR wsp = 0;

if (!wsp)
    {
    mbtowc(&wsp, " ", 1);
    }

strcpy(mb_undefined, sdlReservedName);
strcpy(mb_undefined + SDLNAMESIZ - 1, "-UNDEFINED");

mb_strcode("<LINK ", outfile);
global_linktype = 0; /* default to type jump */
if (type)
    {
    /* type is set, choose which is correct */
    if (m_wcmbupstrcmp(type, QJUMP))
	{ /* not type jump */
	if (!m_wcmbupstrcmp(type, QJUMPNEWVIEW))
	    { mb_strcode("WINDOW=\"NEW\" ", outfile); global_linktype = 1;}
	else if (!m_wcmbupstrcmp(type, QDEFINITION))
	    { mb_strcode("WINDOW=\"POPUP\" ", outfile); global_linktype = 2;}
	else if (!m_wcmbupstrcmp(type, QEXECUTE))
	    { global_linktype = 3;}
	else if (!m_wcmbupstrcmp(type, QMAN))
	    { mb_strcode("WINDOW=\"POPUP\" ", outfile); global_linktype = 4;}
	else if (!m_wcmbupstrcmp(type, QAPPDEFINED))
	    { global_linktype = 5;}
	}
    }
mb_strcode("RID=\"", outfile);
if (hyperlink)
    {
    mb_hyperlink = MakeMByteString(hyperlink);
    if (*mb_hyperlink == '_') /* must be metainfo */
	{
	strcpy(buffer, sdlReservedName);
	buffer[SDLNAMESIZ-1] = '-';
	strcpy(buffer+SDLNAMESIZ, mb_hyperlink+1);
	mb_free(&mb_hyperlink);
	}
    }
else
    {
    m_eprefix();
    fprintf(stderr,
	    "Error: %sLINK%s hyperlink was undefined.\n",
	    m_stago,
	    m_tagc);
    fprintf(m_errfile,
	    "Error: %sLINK%s hyperlink was undefined.\n",
	    m_stago,
	    m_tagc);
    fprintf(stderr,
    "Beware of using a reserved value such as:\n%s, %s, %s, %s, %s, or %s.\n",
	    QJUMP,
	    QJUMPNEWVIEW,
	    QDEFINITION,
	    QEXECUTE,
	    QAPPDEFINED,
	    QMAN);
    fprintf(m_errfile,
    "Beware of using a reserved value such as:\n%s, %s, %s, %s, %s, or %s.\n",
	    QJUMP,
	    QJUMPNEWVIEW,
	    QDEFINITION,
	    QEXECUTE,
	    QAPPDEFINED,
	    QMAN);
    m_errline("Use ``hyperlink='' if the value contains non-alphabetics");
    m_esuffix();
    mb_hyperlink = mb_undefined;
    }

if ((global_linktype <= 2) && hyperlink && (!w_strchr(hyperlink, wsp)))
    {
    if (mb_hyperlink)
	sprintf(buffer, "%s", mb_hyperlink);
    }
else
    {
    char tmpsnb[BIGBUF];
    int  snblen;

    int nextId = NextId();
    sprintf(buffer, "%s%d", sdlReservedName, nextId);
    if (!savesnb && snbstart)
	{
	fprintf(snbfp, "%d\n", snbstart);
	snbstart = 0;
	}
    switch (global_linktype)
	{
	case 0: /* jump */
	case 1: /* jump new */
	case 2: /* definition */
	    sprintf(tmpsnb,
		    "<CROSSDOC ID=\"%s%d\" XID",
		    sdlReservedName,
		    nextId);
	    break;
	case 3: /* execute */
	    sprintf(tmpsnb,
		    "<SYS-CMD ID=\"%s%d\" COMMAND",
		    sdlReservedName,
		    nextId);
	    break;
	case 4: /* man */
	    sprintf(tmpsnb,
		    "<MAN-PAGE ID=\"%s%d\" XID",
		    sdlReservedName,
		    nextId);
	    break;
	case 5: /* app defined */
	    sprintf(tmpsnb,
		    "<CALLBACK ID=\"%s%d\" DATA",
		    sdlReservedName,
		    nextId);
	    break;
	}
    if (savesnb)
	{
	snblen = strlen(savesnb);
	savesnb = mb_realloc(savesnb, snblen + strlen(tmpsnb) + 1);
	strcpy(savesnb + snblen, tmpsnb);
	sprintf(tmpsnb, "=\"%s\">\n", mb_hyperlink);
	snblen = strlen(savesnb);
	savesnb = mb_realloc(savesnb, snblen + strlen(tmpsnb) + 1);
	strcpy(savesnb + snblen, tmpsnb);
	}
    else
	{
	fputs(tmpsnb, snbfp);
	fprintf(snbfp, "=\"%s\">\n", mb_hyperlink);
	}
    }
mb_strcode(buffer, outfile);
mb_strcode("\"", outfile);
if (mb_hyperlink && (mb_hyperlink != mb_undefined))
    m_free(mb_hyperlink,"multi-byte string");
if (description)
    {
    char *mb_description;

    mb_description = MakeMByteString(description);
    sprintf(buffer, " DESCRIPT=\"%s\"", mb_description);
    mb_strcode(buffer, outfile);
    m_free(mb_description,"multi-byte string");
    }
mb_strcode(">", outfile);
}


#if defined(M_PROTO)
char *mb_realloc(char *ptr, long size)
#else
char *mb_realloc(ptr, size)
char *ptr;
long  size;
#endif
{
void   *vptr;
size_t	ssize;

ssize = (size_t) size;
vptr = realloc((void *) ptr, ssize);

#if DEBUG_MB_MALLOC
if (ptr != vptr)
    {
    fprintf(stdout, "realloc 0x%p 0x%p\n", ptr, vptr);
    fflush(stdout);
    }
#endif

return (char *) vptr;
}


#if defined(M_PROTO)
char *mb_malloc(long size)
#else
char *mb_malloc(size)
long  size;
#endif
{
char   *cptr;
size_t	ssize;

ssize = (size_t) size;
cptr = (char *) malloc(ssize);

#if DEBUG_MB_MALLOC
fprintf(stdout, "malloc 0x%p\n", cptr);
fflush(stdout);
#endif

*cptr = '\0';
return cptr;
}


#if defined(M_PROTO)
void mb_free(char **pptr)
#else
void  *mb_free(pptr)
char **pptr;
#endif
{

#if DEBUG_MB_MALLOC
fprintf(stdout, "free 0x%p\n", *pptr);
fflush(stdout);
#endif

free((void *) *pptr);
*pptr = NULL;
}

#if defined(M_PROTO)
static void AddToRowVec(int *length, char **rowvec, char *id)
#else
static void AddToRowVec(length, rowvec, id)
int   *length;
char **rowvec;
char  *id;
#endif
{
char tempRowVec[BIGBUF];
int  rowVecLen;

sprintf(tempRowVec, "<FROWVEC CELLS=\"%s\">\n", id);
rowVecLen = strlen(tempRowVec);
*rowvec = mb_realloc(*rowvec, *length + rowVecLen);
strcpy(*rowvec + *length - 1, tempRowVec);
*length += rowVecLen;
}


#if defined(M_PROTO)
void Add2ToRowVec(int *length, char **rowvec, char *id1, char *id2)
#else
void Add2ToRowVec(length, rowvec, id1, id2)
int   *length;
char **rowvec;
char  *id1, *id2;
#endif
{
char tempRowVec[BIGBUF];
int  rowVecLen;

sprintf(tempRowVec, "<FROWVEC CELLS=\"%s %s\">\n", id1, id2);
rowVecLen = strlen(tempRowVec);
*rowvec = mb_realloc(*rowvec, *length + rowVecLen);
strcpy(*rowvec + *length - 1, tempRowVec);
*length += rowVecLen;
}


#if defined(M_PROTO)
void StartEx(M_WCHAR *notes, M_WCHAR *lines, M_WCHAR *textsize)
#else
void StartEx(*notes, *lines, *textsize)
M_WCHAR *notes;
M_WCHAR *lines;
M_WCHAR *textsize;
#endif
{
exTextSize = vextextsize(textsize);
stackex = vstack(notes);

StartBlock(NULL, "EX", NULL);

tonumexlines = (LOGICAL) (vnumber(lines) == NUMBER);
firstAnnot   = TRUE;
exLineNum    = 1;
oldExLineNum = 1;
saveex       = mb_malloc(1);
svexlen      = 1;
}


#if defined(M_PROTO)
void EndEx()
#else
void EndEx()
#endif
{
char *annotation;
int   annotLen;
char *ssi;
char  buffer[BIGBUF];
int   length;

if (saveexseg)
    {
    if (exTextSize == SMALLEST)
	ssi = "SMLST";
    else if (exTextSize == SMALLER)
	ssi = "SMLR";
    else
	ssi = "NML";
    length = sprintf(buffer, "<P TYPE=\"LITERAL\" SSI=\"EX-%s\">", ssi);

    if (tonumexlines)
	{
	length += sprintf(buffer + length,
			  "<HEAD TYPE=\"LINED\" SSI=\"EX-NUM\">");
	while (oldExLineNum <= exLineNum)
	    {
	    length += sprintf(buffer + length, "%2d:\n", oldExLineNum);
	    oldExLineNum++;
	    }
	length += sprintf(buffer + length, "</HEAD>");
	}

    if (svheadlen)
	{
	annotation = MakeMByteString(savehead);
	annotLen = strlen(annotation);
	}
    else
	{
	annotation = "";
	annotLen   = 0;
	}
    saveex = mb_realloc(saveex,
			svexlen + length + (svexseglen - 1) + annotLen + 5);

    strcpy(saveex + svexlen - 1, buffer);
    svexlen += length;

    if (svheadlen)
	{
	strcpy(saveex + svexlen - 1, annotation);
	svexlen += annotLen;
	}

    strcpy(saveex + svexlen - 1, saveexseg);
    svexlen += svexseglen - 1;

    strcpy(saveex + svexlen - 1, "</P>\n");
    svexlen += 5;
    mb_free(&saveexseg);

    if (svheadlen)
	m_free(annotation, "multi-byte string");
    }
}


#if defined(M_PROTO)
void StartNCW(char *which)
#else
void StartNCW(which)
char *which;
#endif
{
notehead = FALSE;
PushForm(NULL, which, NULL);
}


#if defined(M_PROTO)
void StartBlock(char *pclass, char *ssi, char *id)
#else
void StartBlock(pclass, ssi, id)
char *pclass, *ssi, *id;
#endif
{
char  localId[32];
char *realId;

if (needFData)
    {
    fputs("<FDATA>\n", outfile);
    needFData = FALSE;
    }

if (inBlock)
    fputs("</BLOCK>\n", outfile);

realId = id;
if (formStackBase && (formStackTop >= formStackBase))
    { /* there is a <form> in progress */
    if (!id)
	{
	sprintf(localId, "%s%d", sdlReservedName, NextId());
	realId = localId;
	}
    AddToRowVec(&(formStackTop->vecLen), &(formStackTop->rowVec), realId);
    }

fputs("<BLOCK", outfile);
if (realId)
    fprintf(outfile, " ID=\"%s\"", realId);
if (pclass)
    fprintf(outfile, " CLASS=\"%s\"", pclass);
if (ssi)
    fprintf(outfile, " SSI=\"%s\"", ssi);
fputs(">\n", outfile);

inBlock = TRUE;
firstPInBlock = TRUE;
}


#if defined(M_PROTO)
void StartNCWtext(char *which, char *iconFile, char *headingString)
#else
void StartNCWtext(which, iconFile, headingString)
char *which;
char *iconFile;
char *headingString;
#endif
{
char *icon, *heading;

/* Write default head if no user-specified head was encountered */
if (! notehead)
    {
    heading = GetDefaultHeaderString(headingString, M_SDATA, which);
    fprintf(outfile, "<HEAD SSI=\"NCW\">%s", heading);
    fputs("</HEAD>\n", outfile);
    notehead = TRUE;
    m_free(heading, "default header string return");
    }

icon = GetDefaultHeaderString(iconFile, M_SYSTEM, "");
if (*icon)
    {
    char id[32];

    sprintf(id, "%s%d", sdlReservedName, NextId());
    fprintf(outfile,
	    "<HEAD SSI=\"NCW-ICON\"><SNREF>\n<REFITEM RID=\"%s\" ",
	    id);
    fputs("CLASS=\"ICON\" SSI=\"NCW-ICON\">", outfile);
    fputs("</REFITEM>\n</SNREF></HEAD>\n", outfile);
    AddToSNB(id, icon);
    m_free(icon, "icon name");
    }
}


#if defined(M_PROTO)
void AddToSNB(char *id, char *xid)
#else
void AddToSNB(id, xid)
char *id;
char *xid;
#endif
{
char tmpsnb[BIGBUF];
int  snblen;

if (savesnb)
    {
    sprintf(tmpsnb, "<GRAPHIC ID=\"%s\" XID=\"%s\">\n", id, xid);
    snblen = strlen(savesnb);
    savesnb = mb_realloc(savesnb, snblen + strlen(tmpsnb) + 1);
    strcpy(savesnb + snblen, tmpsnb);
    }
else
    {
    if (snbstart)
	{
	fprintf(snbfp, "%d\n", snbstart);
	snbstart = 0;
	}
    fprintf(snbfp, "<GRAPHIC ID=\"%s\" XID=\"%s\">\n", id, xid);
    }
}


#if defined(M_PROTO)
void IncludeToss()
#else
void IncludeToss()
#endif
{
char	 pathbuf[BIGBUF];
char	*try = pathbuf;
int	 tryleng = sizeof(pathbuf);
int	 pathleng;
int	 fileleng;
SEARCH	*thispath;
char	*mb_inputname;
M_WCHAR *wc_try, *wc_outputname;
int	 tossfile;
char	 filebuf[BIGBUF];
size_t	 bytesread;
char   **tossline = toss;

/* the code below assume the extensions are .htg, .tss and .sdl */
/* or at least that all extensions are 3 character plus a dot   */

fileleng = w_strlen(inputname);
mb_inputname = mb_malloc(fileleng + 1);
wcstombs(mb_inputname, inputname, fileleng);
strcpy(mb_inputname + fileleng - 4, ".tss");

thispath = path;
tossfile = -1;
while (thispath)
    {
    pathleng = strlen(thispath->directory);
    if ((pathleng + fileleng) >= tryleng)
	{
	tryleng = pathleng + fileleng + 1;
	if (try == pathbuf)
	    try = mb_malloc(tryleng);
	else
	    try = mb_realloc(try, tryleng);
	}
    strcpy(try, thispath->directory);
    strcpy(try + pathleng, mb_inputname);
    tossfile = open(try, O_RDONLY);
    if (tossfile >= 0) break;
    thispath = thispath->next;
    }

if (tossfile >= 0)
    {
    fputs("<TOSS>\n", outfile);
    while ((bytesread = read(tossfile, filebuf, sizeof(filebuf))) > 0)
	if (fwrite(filebuf,
		   sizeof(*filebuf),
		   bytesread,
		   outfile) != bytesread) break;
    if (bytesread != 0)
	{
	strcpy(mb_inputname + fileleng - 4, ".sdl");
	wc_outputname = MakeWideCharString(mb_inputname);
	wc_try = MakeWideCharString(try);
	m_err2("error copying %s to output (%s) as the <toss> element",
	       wc_try,
	       wc_outputname);
	m_free(wc_try, "wide character toss input name");
	m_free(wc_outputname, "wide character output name");
	}
    close(tossfile);
    fputs("</TOSS>\n", outfile);
    }
else
    {
    while (*tossline)
	{
	fputs(*tossline++, outfile);
	fputs("\n", outfile);
	}
    }

if (try != pathbuf) mb_free(&try);
}

/* Below is a modified version of m_cyclent() that returns a pointer
 * to the entity content rather than its value.  Returning a pointer
 * to the entity's content field allows it to be modified.
*/
/* Cyclent.c contains procedure m_cyclent(), callable by interface
   designers, to cycle through all defined entities, returning information
   about them */
#if defined(M_PROTO)
M_WCHAR *CycleEnt(LOGICAL init,
		  unsigned char *type,
		  M_WCHAR ***content,
		  unsigned char *wheredef)
#else
M_WCHAR *m_cyclent(init, type, content, wheredef)
  LOGICAL init ;
  unsigned char *type ;
  M_WCHAR ***content ;
  unsigned char *wheredef ;
#endif
{
static M_TRIE *current ;
static M_TRIE *ancestor[M_NAMELEN + 1] ;
static length = 0 ;
static M_WCHAR name[M_NAMELEN + 1] ;

if (init)
    {
    current = m_enttrie->data ;
    length = 0 ;
    }

if (length < 0) return(NULL) ;

while (current->symbol)
    {
    ancestor[length] = current ;
    name[length++] = current->symbol ;
    current = current->data ;
    }
name[length] = M_EOS ;

*type	  =  ((M_ENTITY *)  current->data)->type ;
*content  = &(((M_ENTITY *) current->data)->content) ;
*wheredef =  ((M_ENTITY *)  current->data)->wheredef ;

while (length >= 0)
    {
    if (current->next)
	{
	current = current->next ;
	break ;
	}
    length-- ;
    if (length < 0) break ;
    current = ancestor[length] ;
    }

return(name) ;
}

/* A routine to examine all defined entities looking for ones of type
 * M_SDATA.  When found, if the entity's content is of the form
 * [......] (six characters surrounded by square brackets), its
 * content is modified to be <SPC NAME="[......]"> so that it may be
 * emitted into the SDL output.
*/
void ModifyEntities()
{
unsigned char type;
unsigned char wheredef;
M_WCHAR **content;
M_WCHAR  *name;
char	 *mb_content;
M_WCHAR  *newContent;
static char mb_newContent[] = "<SPC NAME=\"[123456]\">";

name = CycleEnt(TRUE, &type, &content, &wheredef);
if (!name) return;

mb_newContent[21] = '\0';

do  {
    if ((type == M_SDATA) && *content)
	{
	mb_content = MakeMByteString(*content);
	if ((strlen(mb_content) == 8) &&
	    (mb_content[0] == '[')    &&
	    (mb_content[7] == ']'))
	    {
	    strncpy(mb_newContent+11, mb_content, 8);
	    if (wheredef == M_DPARSER)
		m_free(*content, "old SDATA entity content");
	    *content = MakeWideCharString(mb_newContent);
	    }
	m_free(mb_content, "multi-byte SDATA entity content");
	}
    }
while (name = CycleEnt(FALSE, &type, &content, &wheredef));
}


#if defined(M_PROTO)
void PushForm(char *class, char *ssi, char *id)
#else
void PushForm(class, ssi, id)
     char *class;
     char *ssi;
     char *id;
#endif
{
char  localId[SDLNAMESIZ+10];
char *realId;
int   stackSize;

if (needFData)
    {
    fputs("<FDATA>\n", outfile);
    needFData = FALSE;
    }

if (inBlock)
    {
    fputs("</BLOCK>\n", outfile);
    inBlock = FALSE;
    }

realId = id;
if (formStackBase && (formStackTop >= formStackBase))
    { /* there is a <form> in progress */
    if (!id)
	{
	sprintf(localId, "%s%d", sdlReservedName, NextId());
	realId = localId;
	}
    AddToRowVec(&(formStackTop->vecLen), &(formStackTop->rowVec), realId);
    }

if (formStackTop == formStackMax)
    {
    if (!formStackBase)
	{
	formStackBase = (FORMINFO *) malloc(10 * sizeof(FORMINFO));
	formStackTop  = formStackBase;
	formStackMax  = formStackBase + 9;
	}
    else
	{
	stackSize = formStackMax - formStackBase + 1;
	formStackBase =
	    realloc(formStackBase, (stackSize + 10) * sizeof(FORMINFO));
	formStackTop = formStackBase + stackSize;
	formStackMax = formStackBase + (stackSize + 9);
	}
    }
else
    formStackTop++;

formStackTop->rowVec = mb_malloc(1);
formStackTop->vecLen = 1;

fputs("<FORM", outfile);
if (realId)
    fprintf(outfile, " ID=\"%s\"", realId);
if (class)
    fprintf(outfile, " CLASS=\"%s\"", class);
if (ssi)
    fprintf(outfile, " SSI=\"%s\"", ssi);
fputs(">\n", outfile);
needFData = TRUE;
}


#if defined(M_PROTO)
void PushForm2(char *class, char *ssi, char *id1, char *id2)
#else
void PushForm2(class, ssi, id1, id2)
     char *class;
     char *ssi;
     char *id1;
     char *id2;
#endif
{
char id[32];
int  stackSize;
int  formId;

if (needFData)
    {
    fputs("<FDATA>\n", outfile);
    needFData = FALSE;
    }

if (inBlock)
    {
    fputs("</BLOCK>\n", outfile);
    inBlock = FALSE;
    }

if (!id2)
    {
    sprintf(id, "%s%d", sdlReservedName, formId = NextId());
    id2 = id;
    }

if (formStackBase && (formStackTop >= formStackBase))
    { /* there is a <form> in progress */
    Add2ToRowVec(&(formStackTop->vecLen),
		 &(formStackTop->rowVec),
		 id1,
		 id2);
    }

if (formStackTop == formStackMax)
    {
    if (!formStackBase)
	{
	formStackBase = (FORMINFO *) malloc(10 * sizeof(FORMINFO));
	formStackTop  = formStackBase;
	formStackMax  = formStackBase + 9;
	}
    else
	{
	stackSize = formStackMax - formStackBase + 1;
	formStackBase =
	    realloc(formStackBase, (stackSize + 10) * sizeof(FORMINFO));
	formStackTop = formStackBase + stackSize;
	formStackMax = formStackBase + (stackSize + 9);
	}
    }
else
    formStackTop++;

formStackTop->rowVec = mb_malloc(1);
formStackTop->vecLen = 1;

fprintf(outfile, "<FORM ID=\"%s\"", id2);
if (class)
    fprintf(outfile, " CLASS=\"%s\"", class);
if (ssi)
    fprintf(outfile, " SSI=\"%s\"", ssi);
fputs(">\n", outfile);
needFData = TRUE;
}

void PopForm()
{
if (inBlock)
    {
    fputs("</BLOCK>\n", outfile);
    inBlock = FALSE;
    }

fprintf(outfile,
	"</FDATA>\n<FSTYLE>\n%s</FSTYLE>\n</FORM>\n",
	formStackTop->rowVec);

mb_free(&(formStackTop->rowVec));
--formStackTop;
}

void PopForm2()
{
if (inBlock)
    {
    fputs("</BLOCK>\n", outfile);
    inBlock = FALSE;
    }

fprintf(outfile,
	"</FDATA>\n<FSTYLE NCOLS=\"2\">\n%s</FSTYLE>\n</FORM>\n",
	formStackTop->rowVec);

mb_free(&(formStackTop->rowVec));
--formStackTop;
}

/* look to see if there's an open form with no data; if so, add a data
 * block and close the form.  This situation will happen when a form
 * is pushed but the source doesn't go to text either because the text
 * is explicitly optional or due to the fact that text can be null.
*/
void PopFormMaybe()
{
if ((formStackTop >= formStackBase) && (formStackTop->vecLen == 1))
    {
    StartBlock(NULL, NULL, NULL);
    PopForm();
    }
}

void EmitSavedAnchors()
{
char buffer[BIGBUF];

if (parTextId)
    {
    sprintf(buffer,
	    "<ANCHOR ID=\"%s%d\">",
	    sdlReservedName,
	    parTextId);
    mb_strcode(buffer, outfile);
    parTextId = 0;
    }
}

void CloseVirpage()
{
if (parTextId)
    {
    fputs("<BLOCK>\n<P>", outfile);
    EmitSavedAnchors();
    fputs("</P>\n</BLOCK>", outfile);
    }
fputs("</VIRPAGE>\n", outfile);
}
