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
/* $TOG: help.c /main/4 1998/04/06 13:17:27 mgreess $ */
/*   Copyright (c) 1986, 1987, 1988, 1989 Hewlett-Packard Co. */
/* Miscellaneous Procedures for HP Tag/TeX translator */

#include "userinc.h"
#include "globdec.h"

/* Start an appendix */
void appstart(id, letter)
M_WCHAR *id, *letter;
{
M_WCHAR *p, *q, wnull;
char    *pc;
int      length;
long     xchapter;

rsectseq = FALSE;
if (inchapter)
    {
    chapter = 0;
    inchapter = FALSE;
    }
chapst = FALSE;
chapinc = 0;

/* Calculate the letter of the appendix in sequence */
if (! letter)
    {
    chapter++;
    itoletter(chapter, 'A', chapstring, sizeof(chapstring) - 1);
    }

/* User-specified letter */
else
    {
    for (p = letter, xchapter = 0; *p >= 'A' && *p <= 'Z' ; p++)
	xchapter = 26 * xchapter + *p + 1 - 'A';
    /* Test for overflow.  May occur if user omitted "id=" before
     id and system interpreted what is really an id as the appendix
     letter */
    for (; *p >= '0' && *p <= '9' ; p++)
	chapinc = 10 * chapinc + *p - '0';
    if ((long) (int) xchapter != xchapter)
	{
	m_err1("%s: Too many characters in appendix letter", letter);
	chapter++;
	}
    else chapter = (int) xchapter;
    mbtowc(&wnull, "", 1);
    for (pc = chapstring, q = letter; *q != wnull ; pc += length, q++)
	{
	char mbyte[32]; /* larger than the largest possible multibyte char */
	length = wctomb(mbyte, *q);
	if ((pc + length) >= (&chapstring[sizeof(chapstring) - 1]))
	    {
	    m_err1("%s: Too many characters in appendix letter", letter);
	    break;
	    }
	strcpy(pc, mbyte);
	}
    *p = M_EOS;
    }

fprintf(stderr, "\nAppendix %s. ", chapstring);

open_new_helpfile();

savid = checkid(id);
iderr = FALSE;

figno = 0;
tableno = 0;
footnoteno = 1;
}

void assert_hometopic_exists(void)
{
char *string =
    GetDefaultHeaderString("UntitledElementDefaultHeadingString",
			   M_SDATA,
			   "Untitled");
if (nohometopic)
    {
    char    *pc;

    mb_starthelpnode("_HOMETOPIC", FALSE);
    nohometopic = FALSE;
    pc = NULL;
    if (savedtitle)
	pc = MakeMByteString(savedtitle);
    fprintf(outfile,
	    "%s%s%s",
	    "<TITLE><TYPE serif><WEIGHT bold><SIZE 14>",
	    pc  ? pc : string,
	    "</SIZE></WEIGHT></TYPE></TITLE>\n");
    if (pc)
	m_free(pc, "multi-byte string");
    }
m_free(string, "GetDefaultHeaderString return");
}

/* Determine base name for files (i.e., input name without extension).
   Open output and error files */
void basename(void)
  {
    char *p, *q;
    int n;
    char save;

    m_errfile = NULL;
    if (m_argc < 2) {
      m_error("Specify input file");
      exit(TRUE);
      }
    if (m_argc > 2 &&
        (strchr(m_argv[2], 'f') || strchr(m_argv[2], 'F'))
       ) filelist = TRUE;

    /* Get installation directory */
#if defined(hpux) || defined(_AIX) || defined(sun) || defined(__linux__)
#define CONTRIB "/usr/hphelp/bin/"

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

      if ( *(m_argv[0]) == '/' ) {
	    /* fully qualified path to ourself was specified */
	    if (access(m_argv[0],1) == 0) {
        /* if full path name exists and is executable */
        /* get the dirname */
        for (p = m_argv[0]; *p ; p++) ; /* end of string, (the hard way) */
          /* backup to dirsep */
          for (; ; p--) {
            if (p < m_argv[0]) m_error("Internal Error.");
            if (*p == dirsep) break; 
            }
          p++; /* just past the dirsep */
          save = *p;
          *p = M_EOS;
          install = (char *)m_malloc(strlen(m_argv[0]) + 1, "installation directory");
          strcpy(install, m_argv[0]);
          *p = save;

          none = 0; /* we've got it. */
          }
        else {
          m_error("Internal Error (which).");
          }
        }
      else {
        /* not fully specified, check each component of path for ourself */
        strcpy(patbuf, getenv("PATH"));
        path = patbuf;
        cp = path;

        while(1) {
          cp = strchr(path, ':');
          if (cp == NULL)
            quit++;
	        else
            *cp = '\0';
          sprintf(buf, "%s/%s", path, m_argv[0]);

          if (access(buf, 1) == 0) {
            install = (char*) m_malloc(strlen(path) + 1,
                      "installation directory");
            strcpy(install, path);
            none = 0;
            }
          /* else, not an error if we can't find a particular one. */

          path = ++cp;
          if (quit || !none) {
            break; /* either out of paths, or we found it. */
            }
          }  /* end while */
     }
     
     if (none) {
	  /* can't get it, use default */
	  install = (char*) m_malloc(strlen(CONTRIB) + 1,
				     "installation directory");
	  strcpy(install, CONTRIB);
     }
     /* else -- we've got it */
}

#else
 Get installation directory
#endif

    /* Set default working directory (from input filename) */
    for (p = strchr(m_argv[1], M_EOS); p > m_argv[1] ; p--)
      if (*(p - 1) == dirsep)
        break;
    if (p > m_argv[1]) {
      save = *p;
      *p = M_EOS;
      work = (char *) m_malloc(strlen(m_argv[1]) + 1, "working directory");
      strcpy(work, m_argv[1]);
      indir = (char *) m_malloc(strlen(m_argv[1]) + 1, "input directory");
      strcpy(indir, m_argv[1]);
      *p = save;
      }
    else {
      indir = NULL;
      }

    /* Build base name */
    q = strchr(m_argv[1], M_EOS);
    while (q > m_argv[1] && *q != '.' && *q != dirsep) q--;
    defaultext = (LOGICAL) (*q != '.');
    if (! defaultext) *q = M_EOS;
    nodirbase = (char *)
                m_malloc(strlen(p) + 1, "basename without directory");
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
    if (usingshortnames) {
     /* Build short versions of basename */
     /* set up global helpbase and helpext */
     helpbase = (char *) m_malloc(strlen(work) + BASENAME_LIMIT + PRE_EXTENSION_LIMIT + strlen(".ext") + 1, "help basename");
     strcpy(helpbase, work);
     strncat(helpbase, nodirbase, BASENAME_LIMIT);
     helpext = helpbase + strlen(helpbase);
}
else { /* Build long names */
     /* set up global helpbase and helpext */
     helpbase = (char *) m_malloc(strlen(work) + strlen(nodirbase) + PRE_EXTENSION_LIMIT + strlen(".ext") + 1, "help basename");
     strcpy(helpbase, work);
     strcat(helpbase, nodirbase);
     helpext = helpbase + strlen(helpbase);
}
    /* Open error files */
    if (filelist) {
      m_openchk(&m_errfile, "filelist.err", "w");
      }
    else {
      strcpy(helpext, ".err");
      m_openchk(&m_errfile, helpbase, "w");
      }
    }


/* This procedure starts a CHAPTER OR PART, depending on the character
   array "type," which should only contain "part" or "chapter."  Note
   that PART is supported only for calculator.  */
void chapstart(id, number, type)
M_WCHAR *id, *number;
char type[];          /* values: locase "chapter" or "part" ONLY  */
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
char *partPrefixString =
  GetDefaultHeaderString("PartElementDefaultHeadingString",
			 M_SDATA,
			 "Part");
char *partSuffixString =
  GetDefaultHeaderString("PartSuffixElementDefaultHeadingString",
			 M_SDATA,
			 "");

rsectseq = FALSE;   /* CHAPTERs only; irrelevant for PART */
chapst = FALSE;     /* CHAPTERs only; irrelevant for PART */
chapinc = 0;
if (! number)
    {
    if ( *type == 'c' )
	{
	chapter++;      /* CHAPTER processing */
	m_itoa(chapter, chapstring);
	}
    else
	{
	part++;
	m_itoa(part, chapstring);
	}
    }
else
    {
    mbyte = MakeMByteString(number);
    chapter = atoi(mbyte);
    m_free(mbyte,"multi-byte string");

    for (p = number; *p >= '0' && *p <= '9' ; p++);
    for (; *p >= 'A' && *p <= 'Z' ; p++)
	chapinc = 26 * chapinc + *p + 1 - 'A';

    mbyte = MakeMByteString(number);
    if (strlen(mbyte) + 1 > sizeof(chapstring))
	{
	wc = MakeWideCharString(type);
	m_err2("%s: Too many characters in %s number", number, wc);
	m_free(wc,"wide character string");
	strncpy(chapstring, mbyte, sizeof(chapstring) - 1);
	}
    else
	strcpy(chapstring, mbyte);
    m_free(mbyte,"multi-byte string");

    pc = chapstring;
    while (*pc)
	{
	if ((length = mblen(pc,32)) == 1)
	    *pc = isupper(*pc) ? tolower(*pc) : *pc;
	pc += length;
	}
    }
if (*type == 'c')
    { /* chapter, not part */
	 open_new_helpfile();
    }
savid = checkid(id);
iderr = FALSE;

figno = 0;
tableno = 0;
footnoteno = 1;
fprintf(stderr,
	"\n%s %s%s. ",
        *type=='c' ? chapterPrefixString : partPrefixString,
	chapstring,
        *type=='c' ? chapterSuffixString : partSuffixString
	);
m_free(chapterPrefixString, "GetDefaultHeaderString return");
m_free(partPrefixString, "GetDefaultHeaderString return");
if (*chapterSuffixString)
    m_free(chapterSuffixString, "GetDefaultHeaderString return");
if (*partSuffixString)
    m_free(partSuffixString, "GetDefaultHeaderString return");
}

/*****************************************************************
*****************************************************************
 CDROM routines:  check to see if sdata is supported in cdrom
*/ 
#define STATIC static
/*
 * define arrays specifing laserrom defined entities.
 * these are used to override "texchar.ent" SDATA && PI definitions.
 */
STATIC struct
{
    char *entityName;
    char *entityData;
} DefPIEntity[] =
{
    {"D-", ""},
    {"m-d-", ""},
    /*
     * the last entry in this array should always be
     * '{NULL, NULL}'
     */
    {NULL, NULL}
};

STATIC struct
{
    char *entityName;
    char *entityData;
} DefSDataEntity[] =
{
    {"EMPTY", ""},
    {"MINUS", "\\symbol{4}"},
    {"PM", "+-"},
    {"DIV", "\\symbol{5}"},
    {"TIMES", "\\times "},
    {"LEQ", "\\symbol{12}"},
    {"GEQ", "\\symbol{13}"},
    {"NEQ", "\\symbol{14}"},
    {"COPY", "(c)"},
    {"REG", "(R)"},
    {"TM", "\\special{{PLU}}TM\\special{{PLD}}"},
    {"ELLIPSIS", "\\symbol{18}"},
    {"VELLIPSIS", "\\symbol{7}"},
    {"DEG", "degree"},
    {"SQUOTE", "\\symbol{8}"},
    {"DQUOTE", "\\symbol{9}"},
    {"ENDASH", "\\symbol{10}"},
    {"EMDASH", "\\symbol{21}"},
    {"VBLANK", "_"},
    {"CENTS", "cents"},
    {"STERLING", "sterling"},
    {"PELLIPSIS", "...."},
    {"MSPECCHAR", ""},
    {"SIGSPACE", "\\ "},
    {"HALFSPACE", "\\ "},
    {"MICRO", "\\symbol{24}"},
    {"OHM", "ohm"},
    {"UP", "up arrow"},
    {"DOWN", "down arrow"},
    {"LEFT", "left arrow"},
    {"RIGHT", "right arrow"},
    {"HOME", "home arrow"},
    {"BACK", "[[backspace]]"},
    {"DATE", "\\date "},
    {"TIME", "\\time "},
    {"A.M.", "am"},
    {"P.M.", "pm"},
    {"MINUTES", "\\squote "},
    {"SECONDS", "\\dquote "},
    {"M-SPECCHAR", ""},
    {"alpha", "alpha"},
    {"beta", "beta"},
    {"gamma", "gamma"},
    {"delta", "delta"},
    {"epsilon", "epsilon"},
    {"varepsilon", "varepsilon"},
    {"zeta", "zeta"},
    {"eta", "eta"},
    {"theta", "theta"},
    {"vartheta", "vartheta"},
    {"iota", "iota"},
    {"kappa", "kappa"},
    {"lambda", "lambda"},
    {"mu", "mu"},
    {"nu", "nu"},
    {"xi", "xi"},
    {"pi", "pi"},
    {"varpi", "varpi"},
    {"rho", "rho"},
    {"varrho", "varrho"},
    {"sigma", "sigma"},
    {"varsigma", "varsigma"},
    {"tau", "tau"},
    {"upsilon", "upsilon"},
    {"phi", "phi"},
    {"varphi", "varphi"},
    {"chi", "chi"},
    {"psi", "psi"},
    {"omega", "omega"},
    {"UGamma", "Gamma"},
    {"UDelta", "Delta"},
    {"UTheta", "Theta"},
    {"ULambda", "Lambda"},
    {"UXi", "Xi"},
    {"UPi", "Pi"},
    {"USigma", "Sigma"},
    {"UUpsilon", "Upsilon"},
    {"UPhi", "Phi"},
    {"UPsi", "Psi"},
    {"UOmega", "Omega"},
    {"CA", "\\it{A}"},
    {"CD", "\\it{D}"},
    {"CG", "\\it{G}"},
    {"CJ", "\\it{J}"},
    {"CM", "\\it{M}"},
    {"CP", "\\it{P}"},
    {"CS", "\\it{S}"},
    {"CV", "\\it{V}"},
    {"CY", "\\it{Y}"},
    {"CB", "\\it{B}"},
    {"CE", "\\it{E}"},
    {"CH", "\\it{H}"},
    {"CK", "\\it{K}"},
    {"CN", "\\it{N}"},
    {"CQ", "\\it{Q}"},
    {"CT", "\\it{T}"},
    {"CW", "\\it{W}"},
    {"CZ", "\\it{Z}"},
    {"CC", "\\it{C}"},
    {"CF", "\\it{F}"},
    {"CI", "\\it{I}"},
    {"CL", "\\it{L}"},
    {"CO", "\\it{O}"},
    {"CR", "\\it{R}"},
    {"CU", "\\it{U}"},
    {"CX", "\\it{X}"},
    {"aleph", "aleph"},
    {"hbar", "hbar"},
    {"imath", "\\it{i}"},
    {"jmath", "\\it{j}"},
    {"ell", "\\it{l}"},
    {"wp", "wp"},
    {"Re", "Re"},
    {"Im", "Im"},
    {"partial", "d"},
    {"infty", "infinity"},
    {"S", "section"},
    {"szero", "szero"},
    {"prime", "\\symbol{8}"},
    {"emptyset", "empty set"},
    {"nabla", "nabla"},
    {"surd", "x"},
    {"top", "top"},
    {"bot", "bottom"},
    {"vbar", "||"},
    {"angle", "angle"},
    {"triangle", "triangle"},
    {"backslash", "\\\\"},
    {"P", "paragraph"},
    {"forall", "forall"},
    {"exists", "exists"},
    {"neg", "^"},
    {"flat", "\\it{b}"},
    {"natural", "natural"},
    {"sharp", "\\#"},
    {"clubsuit", "clubs"},
    {"diamondsuit", "diamonds"},
    {"heartsuit", "hearts"},
    {"spadesuit", "spades"},
    {"sum", "sum"},
    {"prod", "prod"},
    {"coprod", "coprod"},
    {"int", "integral"},
    {"oint", "ointegral"},
    {"bigcap", "bigcap"},
    {"bigcup", "bigcup"},
    {"bigsqcup", "bigsqcup"},
    {"bigvee", "bigvee"},
    {"bigwedge", "bigwedge"},
    {"bigodot", "bigodot"},
    {"bigotimes", "bigotimes"},
    {"bigoplus", "bigoplus"},
    {"biguplus", "biguplus"},
    {"mp", "-+"},
    {"setminus", "\\\\"},
    {"cdot", "\\special{{PLU}}.\\special{{PLD}}"},
    {"ast", "*"},
    {"star", "*"},
    {"diamond", "diamond"},
    {"circ", "circ"},
    {"bullet", "bullet"},
    {"cap", "cap"},
    {"cup", "cup"},
    {"uplus", "uplus"},
    {"sqcap", "sqcap"},
    {"sqcup", "sqcup"},
    {"triangleleft", "triangleleft"},
    {"triangleright", "triangleright"},
    {"wr", "wr"},
    {"bigcirc", "bigcirc"},
    {"bigtriangleup", "triangleup"},
    {"bigtriangledown", "triangledown"},
    {"vee", "v"},
    {"wedge", "wedge"},
    {"oplus", "oplus"},
    {"ominus", "ominus"},
    {"otimes", "otimes"},
    {"oslash", "oslash"},
    {"odot", "odot"},
    {"dagger", "*"},
    {"ddagger", "**"},
    {"amalg", "amalg"},
    {"prec", "prec"},
    {"preceq", "preceq"},
    {"ll", "<<"},
    {"subset", "subset"},
    {"subseteq", "subseteq"},
    {"sqsubseteq", "sqsubseteq"},
    {"in", "in"},
    {"vdash", "|-"},
    {"smile", "smile"},
    {"frown", "frown"},
    {"succ", "succ"},
    {"succeq", "succeq"},
    {"gg", ">>"},
    {"supset", "supset"},
    {"supseteq", "supseteq"},
    {"sqsupseteq", "sqsupseteq"},
    {"ni", "notin"},
    {"dashv", "-|"},
    {"mid", "|"},
    {"parallel", "||"},
    {"equiv", "equiv"},
    {"sim", "sim"},
    {"simeq", "simeq"},
    {"asymp", "asymp"},
    {"approx", "approx"},
    {"cong", "cong"},
    {"bowtie", "bowtie"},
    {"propto", "propto"},
    {"models", "|="},
    {"doteq", "doteq"},
    {"perp", "perp"},
    {"not-l", "not <"},
    {"not-leq", "not <="},
    {"not-prec", "not prec"},
    {"not-preceq", "not preceq"},
    {"not-subset", "not subset"},
    {"not-subseteq", "not subset equal"},
    {"not-sqsubseteq", "not sqsubseteq"},
    {"not-eq", "not ="},
    {"not-sim", "not sim"},
    {"not-approx", "not approx"},
    {"not-asymp", "not asymp"},
    {"not-g", "not >"},
    {"not-geq", "not >="},
    {"not-succ", "not succ"},
    {"not-succeq", "not succeq"},
    {"not-supset", "not supset"},
    {"not-supseteq", "not supseteq"},
    {"not-sqsupseteq", "not sqsupseteq"},
    {"not-equiv", "not equiv"},
    {"not-simeq", "not simeq"},
    {"not-cong", "not cong"},
    {"leftarrow", "<-"},
    {"bigleftarrow", "<="},
    {"rightarrow", "->"},
    {"bigrightarrow", "=>"},
    {"leftrightarrow", "<->"},
    {"bigleftrightarrow", "<=>"},
    {"mapsto", "|->"},
    {"hookleftarrow", "hookleftarrow"},
    {"leftharpoonup", "leftharpoonup"},
    {"leftharpoondown", "leftharpoondown"},
    {"rightleftharpoons", "rightleftharpoons"},
    {"uparrow", "uparrow"},
    {"biguparrow", "biguparrow"},
    {"downarrow", "downarrow"},
    {"bigdownarrow", "bigdownarrow"},
    {"updownarrow", "updownarrow"},
    {"bigupdownarrow", "bigupdownarrow"},
    {"longleftarrow", "<--"},
    {"biglongleftarrow", "<=="},
    {"longrightarrow", "-->"},
    {"biglongrightarrow", "==>"},
    {"longleftrightarrow", "<-->"},
    {"biglongleftrightarrow", "<==>"},
    {"longmapsto", "|-->"},
    {"hookrightarrow", "hookrightarrow"},
    {"rightharpoonup", "rightharpoonup"},
    {"rightharpoondown", "rightharpoondown"},
    {"nearrow", "NEarrow"},
    {"searrow", "SEarrow"},
    {"swarrow", "SWarrow"},
    {"nwarrow", "NWarrow"},
    {"half", "1/2"},
    {"third", "1/3"},
    {"quarter", "1/4"},
    {"threequarter", "3/4"},
    {"fifth", "1/5"},
    {"sixth", "1/6"},
    {"eighth", "1/8"},
    {"sixteenth", "1/16"},
    {"space", "\\symbol{20}"},
    {"INSTMAN", "instman"},
    {"HAZVOLT", "hazvolt"},
    {"GROUND", "ground"},
    {"DCVOLT", "dcvolt"},
    {"NEGPULSE", "negpulse"},
    {"POSPULSE", "pospulse"},
    {"SINEWAVE", "sinewave"},
    {"SAWWAVE", "sawwave"},
    {"RAMPWAVE", "rampwave"},
    {"TRIWAVE", "triwave"},
    {"SQWAVE", "sqwave"},
    /*
     * the last entry in this array should always be
     * '{NULL, NULL}'
     */
    {NULL, NULL}
};


/* Verify Tag dimension, else issue error message */
M_WCHAR *checkdimen( val, paramname, elt )
M_WCHAR *val;
M_WCHAR *paramname;
M_WCHAR *elt;
{
/* returns val if a valid TeX dimension, else NULL */
M_WCHAR *valid = NULL;

if ( val == NULL ) return ( NULL );
if ((valid = okdimen(val)) != NULL) return(valid);
if ( valid == NULL )
    {
    M_WCHAR *wc_stago, *wc_tagc;

    wc_stago = MakeWideCharString(m_stago);
    wc_tagc  = MakeWideCharString(m_tagc);
    m_err5 ("[%s] Illegal value of %s for %s%s%s",
	   val,
	   paramname,
	   wc_stago,
	   elt,
	   wc_tagc );
    m_free(wc_stago,"wide character string");
    m_free(wc_tagc,"wide character string");
    }
return ( valid );
}


M_WCHAR *mb_checkdimen( val, paramname, elt )
M_WCHAR *val;
char *paramname;
char *elt;
{
M_WCHAR *wc_paramname, *wc_elt, *retval;

wc_paramname = MakeWideCharString(paramname);
wc_elt       = MakeWideCharString(elt);

retval = checkdimen(val, wc_paramname, wc_elt);

m_free(wc_paramname,"wide character string");
m_free(wc_elt,"wide character string");
}

/* Check through search (like openent()) paths of SYSTEM entity name to
   construct fully qualified or relative path names, if any */
M_WCHAR *checkent(entcontent)
M_WCHAR *entcontent;
{
M_WCHAR *filename;
SEARCH  *searchp;
char    *mb_entc, mb_filen[2048];
int      dir_leng, econt_leng;

mb_entc = MakeMByteString(entcontent);
if (access(mb_entc, 04)==0)
    {  /* entcontent is readable */
    filename = (M_WCHAR *) m_malloc(w_strlen(entcontent) + 1,
				    "checkent figinpath");
    w_strcpy(filename, entcontent);
    m_free(mb_entc,"multi-byte string");
    return(filename);
    }

econt_leng = strlen(mb_entc);
for (searchp = path; searchp ; searchp = searchp->next)
    {
    if (((dir_leng = strlen(searchp->directory)) + econt_leng) >= 2048)
	{
	M_WCHAR *wc;

	wc = MakeWideCharString(searchp->directory);
	m_err2("%s and %s overflow the file name space",
	       wc,
	       entcontent);
	m_free(wc,"wide character string");
	continue;
	}
    strcpy(mb_filen, searchp->directory);
    strcpy(mb_filen + dir_leng, mb_entc);
    if (access(mb_filen, 04)==0)  /* entcontent is readable */
	{
	m_free(mb_entc,"multi-byte string");
	return MakeWideCharString(mb_filen);
	}
    }

m_free(mb_entc,"multi-byte string");
return(NULL);
}


/* Called at end of manual to report terms that occurred in the document
   but not entered in the glossary */
void checkgloss(void)
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
	if ((unsigned) current->data != DEFINETERM)
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

		
/* End Error _DtMessage macro \starterrmsg call, check to see if Error _DtMessage
   head is user specified or default */
void checkmsghead(void)
{
char *string =
    GetDefaultHeaderString("MessagesElementDefaultHeadingString",
		           M_SDATA,
			   "Messages");
if (emsghead == DEFHEAD)
    {  /* head not output yet */
    fprintf(outfile,
	    "%s%s%s",
	    "<TITLE><TYPE serif><WEIGHT bold><SIZE 14>",
	    string,
	    "</TYPE></WEIGHT></SIZE></TITLE>\n");
    emsghead = FALSE;
    }
else if (emsghead == USERHEAD)
    {  /* user specified head */
    emsghead = FALSE;
    }
m_free(string, "GetDefaultHeaderString return");
}  /* end checkmsghead() */

		
/* Verify that val is an acceptable real number value */
M_WCHAR *
checkreal (M_WCHAR *val, M_WCHAR *paramname, LOGICAL msgout, M_WCHAR *elt)
{
/* returns val if a valid real number, else NULL */
double num;
M_WCHAR *valid = NULL;
char     s1[M_NAMELEN+1];
char    *mbyte;

if (val == NULL) return NULL;

mbyte = MakeMByteString(val);

if (sscanf (mbyte, "%lf", &num ) == 1 ) valid = val;

if (sscanf(mbyte, "%lf%s", &num, s1 ) == 2 )
    valid = NULL; /* no characters allowed  */

if ( valid == NULL )
    {
    M_WCHAR *wc_stago, *wc_tagc;

    if (msgout)
	{
	wc_stago = MakeWideCharString(m_stago);
	wc_tagc  = MakeWideCharString(m_tagc);
	m_err5("[%s] Illegal value of %s for %s%s%s",
	       val,
	       paramname,
	       wc_stago,
	       elt,
	       wc_tagc );
	m_free(wc_stago,"wide character string");
	m_free(wc_tagc,"wide character string");
	}
    }

m_free(mbyte,"multi-byte string");
return ( valid );
} /* END checkreal procedure */


/* end a help node */
void endhelpnode(void)
{
}

/* End code for <EX> and <VEX> */
void exvexend (textsize)
M_WCHAR *textsize;
{
int tsize;

tsize = vextextsize(textsize);
if (tsize==NORMAL)
    {
    fputs("</SPACING></TYPE></PARAGRAPH>\n\n", outfile);
    }      
else
    {  /* textsize is either SMALLER or SMALLEST */
    fputs("</SIZE>\n</SPACING></TYPE></PARAGRAPH>\n\n", outfile);
    }
}  /* END exvexend() */


/* Starting stuff for <EX> and <VEX> */
void exvexstart (M_WCHAR *type, M_WCHAR *textsize, char *listinfo)
{
int tsize;

STARTSTUFF
rshnewclear();
parspace();
tsize = vextextsize(textsize);
if (tsize==NORMAL)
    {
    fprintf(outfile,
    "<PARAGRAPH%s firstindent %d leftindent %d nowrap><TYPE serif><SPACING m>",
            listinfo,
            LEFTINDENT,
            LEFTINDENT);
    }      
else
    {  /* textsize is either SMALLER or SMALLEST */
    fprintf(outfile,
     "<PARAGRAPH%s firstindent %d leftindent %d nowrap><TYPE serif><SPACING m>",
            listinfo,
	    LEFTINDENT,
	    LEFTINDENT);
    if (tsize==SMALLER)
	{
	fputs("<SIZE 8>", outfile);
	}
    else
	{
	fputs("<SIZE 6>", outfile);
	}
    }
}  /* END exvexstart() */


/* construct a qualified file name */
int mb_getqualified ( qualname, unqualname )
char *qualname;
char *unqualname;
{
#if defined(hpux) || defined(_AIX) || defined(sun) || defined(__linux__)
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

if (strlen(unqualname) < FNAMELEN)
    strcpy(fn, unqualname );
else
    {
    m_mberr1("Internal Error. File name too long for qualifying: %s",
	     unqualname);
    return (-1);
    }

fnp = fn;

#if defined(hpux) || defined(_AIX) || defined(sun) || defined(__linux__)
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
    if (strlen(gp) >= roomleft)
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
	if ((strlen(p)+1) < roomleft)
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

#if defined(hpux) || defined(_AIX) || defined(sun) || defined(__linux__)
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
char *mb_content;
static M_WCHAR empty = M_EOS;

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
    fprintf(outfile, " graphic %s", mb_content);
    m_free(mb_content,"multi-byte string");
    }

/* and the position now */
if (gposition)
    {
    M_WCHAR *wc_left, *wc_right;

    wc_left = MakeWideCharString(QLEFT);
    if (!m_wcupstrcmp(gposition, wc_left))
	{
	fputs(" gpos left", outfile);
	}
    else
	{
	wc_right = MakeWideCharString(QRIGHT);
	if (!m_wcupstrcmp(gposition, wc_right))
	    {
	    fputs(" gpos right", outfile);
	    }
	else
	    {
	    m_err1("Invalid value for gposition: `%s'", gposition);
	    }
	m_free(wc_right,"wide character string");
	}
    m_free(wc_left,"wide character string");
    }

/* end handling graphic specific code */
/* handle link specific code */
if (!glinktype)
    { /* default to type jump */
     /* do nothing */
    }
else
    { /* type is set, choose which is correct */
    M_WCHAR *wc_stago, *wc_tagc;

    if (!m_wcmbupstrcmp(glinktype, QJUMP))
	{ fputs(" gtypelink 0", outfile); }
    else if (!m_wcmbupstrcmp(glinktype, QJUMPNEWVIEW))
	{ fputs(" gtypelink 1", outfile); }
    else if (!m_wcmbupstrcmp(glinktype, QDEFINITION))
	{ fputs(" gtypelink 2", outfile); }
    else if (!m_wcmbupstrcmp(glinktype, QEXECUTE))
	{ fputs(" gtypelink 3", outfile); }
    else if (!m_wcmbupstrcmp(glinktype, QMAN))
	{ fputs(" gtypelink 4", outfile); }
    else if (!m_wcmbupstrcmp(glinktype, QAPPDEFINED))
	{ fputs(" gtypelink 5", outfile); }
    else
	{ /* parser won't let this case happen */
	wc_stago = MakeWideCharString(m_stago);
	wc_tagc  = MakeWideCharString(m_tagc);
	m_err3(
	  "%sLINK%s (glinktype=``%s'') did not match an allowed value",
	      wc_stago,
	      wc_tagc,
	      glinktype); 
	m_free(wc_stago,"wide character string");
	m_free(wc_tagc,"wide character string");
	}
    }

if (ghyperlink)
    {
    char *mb_ghyperlink;

    mb_ghyperlink = MakeMByteString(ghyperlink);
    fprintf(outfile, " glink \"%s\"", mb_ghyperlink);
    m_free(mb_ghyperlink,"multi-byte string");
    }
else
    {
    if (glinktype || gdescription)
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
	fputs(" glink _undefined", outfile);
	}
    }

if (gdescription)
    {
    fprintf(outfile, " description \"%s\"", gdescription);
    }
/* end of link specific code */
}

/* Process an item in a list */
void item(id)
M_WCHAR *id;
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
int count, metaCount;

if (lastlist->lastlist->type == ORDER)
    fprintf(outfile, "<PARAGRAPH leftindent %d", ORDEREDLISTITEMINDENT);
else
    fprintf(outfile, "<PARAGRAPH leftindent %d", LISTITEMINDENT);

if (lastlist->lastlist->space == TIGHT)
    {
    fputs(" after 0", outfile);
    }
lastlist->lastlist->where = FIRST;

orderString[0] = '\0';
count = ++lastlist->lastlist->count;
if (count > 999) count = 999; /* holy cow!  Big list. */
switch (lastlist->lastlist->order)
    {
    case UROMAN:
	strcpy(orderString, ROMAN100[count / 100]);
	strcat(orderString, ROMAN10[(count / 10) % 10]);
	strcat(orderString, ROMAN0[count % 10]);
	if ((count = strlen(orderString)) < 4)
	    {
	    char tmpString[32];

	    tmpString[0] = '\0';
	    count = 4 - count;
	    while (--count >= 0) strcat(tmpString, "\\ ");
	    strcat(tmpString, orderString);
	    strcpy(orderString, tmpString);
	    }
	break;
    case UALPHA:
	metaCount = 1;
	while ((count -= 26) > 0) metaCount++;
	count = lastlist->lastlist->count;
	if (count > 999) count = 999;
	count -= 1;
	count %= 26;
	while (--metaCount >= 0) strncat(orderString, &ALPHABET[count], 1);
	break;
    case ARABIC:
	if (metaCount = (count / 100))
	    strncat(orderString, &numbers[metaCount], 1);
	if (metaCount || ((count / 10) % 10))
	    strncat(orderString, &numbers[(count / 10) % 10], 1);
	strncat(orderString, &numbers[count % 10], 1);
	break;
    case LROMAN:
	strcpy(orderString, roman100[count / 100]);
	strcat(orderString, roman10[(count / 10) % 10]);
	strcat(orderString, roman0[count % 10]);
	if ((count = strlen(orderString)) < 4)
	    {
	    char tmpString[32];

	    tmpString[0] = '\0';
	    count = 4 - count;
	    while (--count >= 0) strcat(tmpString, "\\ ");
	    strcat(tmpString, orderString);
	    strcpy(orderString, tmpString);
	    }
	break;
    case LALPHA:
	metaCount = 1;
	while ((count -= 26) > 0) metaCount++;
	count = lastlist->lastlist->count;
	if (count > 999) count = 999;
	count -= 1;
	count %= 26;
	while (--metaCount >= 0) strncat(orderString, &alphabet[count], 1);
	break;
    }

if (lastlist->lastlist->type == ORDER)
    {
    fprintf( /* keep the '('s balanced */
	    outfile,
	    "><LABEL><WEIGHT bold>%s%2s%c</WEIGHT></LABEL",
	    (strlen(orderString) > 1) ? "" : "\\",
	    orderString,
	    lastlist->lastlist->punct == DOTPUNCT ? '.' : ')' );
    if (id)
	{
	char buffer[400], *mbyte;
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

	mbyte = MakeMByteString(id);
	fprintf(outfile, " id \"%s\"", id);
	m_free(mbyte,"multi-byte string");
	}
    }
else
    { /* Bullet, Check or Plain list */
    if (id) m_error("Cross-reference ID in non-ORDER list not allowed");
    if (lastlist->lastlist->type == BULLET)
	{
	fprintf(outfile,
		"><LABEL><CHARACTERSET symbol>%c</></LABEL",
	        183 /* BULLET CHAR */ );
	}
    else if (lastlist->lastlist->type == CHECK)
	{
	/* no hollow square in the symbol charset, use hollow diamond */
	fprintf(outfile,
		"><LABEL><CHARACTERSET symbol>%c</></LABEL",
	        224 /* CHECK CHAR */ );
	}
    else /* if ( lastlist->lastlist->type == PLAIN) */
	{ /* above commented out to make this the default for MILSPEC lists */
	/* emit a hard space label to make things look right when TIGHT */
	fprintf(outfile, "><LABEL>\\ </LABEL");
	}
    }
fputs(">\n", outfile);
}


/* Express a sequence number as a letter in an alpha list or appendix */
void itoletter(int n, char start, char *dest, int length)
{
char invert[10];
char *p, *q;

if (n <=0)
    {
    m_error("Internal error: converting non-positive number to letter");
    dest[0] = start;
    dest[1] = M_EOS;
    return;
    }
for (p = invert; n ; p++, n = (n - 1) / 26)
    {
    *p = start + (n - 1) % 26;
    if (p == &invert[length] || p == &invert[sizeof(invert)])
	{
	m_error("Internal error: out of space converting number to letter");
	break;
	}
    }
p--;
q = dest;
while (TRUE)
    {
    *q++ = *p;
    if (p == invert) break;
    p--;
    };
*q = M_EOS;
}

/* Start the <MANUAL> tag processing */
void manustart(language, idxvol, status)
M_WCHAR *language, *idxvol, *status;
{
if (idxvol) w_strcpy(volume, idxvol);
}  /* END manustart procedure */


/* Issue error message and text in document element is not supported in
   a particular option or context */
void notimp(eltname, option)
char *eltname;
char *option;
{
char *p;
M_WCHAR *wc_stago, *wc_eltname, *wc_tagc, *wc_option;

fputs("<PARAGRAPH><WEIGHT bold>", outfile);
for (p = m_stago; *p ; p++)
    {
    outchar(*p, outfile);
    }
fputs(eltname, outfile);
for (p = m_tagc; *p ; p++)
    {
    outchar(*p, outfile);
    }
fprintf(outfile, " not supported in %s.</WEIGHT></PARAGRAPH>\n\n", option);

wc_stago = MakeWideCharString(m_stago);
wc_eltname = MakeWideCharString(eltname);
wc_tagc = MakeWideCharString(m_tagc);
wc_option = MakeWideCharString(option);

m_err4("%s%s%s not suppported in %s",
       wc_stago,
       wc_eltname,
       wc_tagc,
       wc_option);

m_free(wc_stago,"wide character string");
m_free(wc_eltname,"wide character string");
m_free(wc_tagc,"wide character string");
m_free(wc_option,"wide character string");

outfile = nullfile;
}

/* Close current output, construct new output name and open output */
void open_new_helpfile()
{	 
int j, length;
char htfilenostring[15];
M_WCHAR *wc_1, *wc_2;

if (! firstnode)
    {
    fputs("</TOPIC>\n", outfile);
    }
fclose(m_outfile);

m_itoa(htfileno++, htfilenostring);

if ((j = strlen(htfilenostring)) > PRE_EXTENSION_LIMIT)
    {
    helptmpbuf[0] = '0' + PRE_EXTENSION_LIMIT;
    helptmpbuf[1] = '\0';
    wc_1 = MakeWideCharString(htfilenostring);
    wc_2 = MakeWideCharString(helptmpbuf);
    m_err2(
    "Error:  Tail of new output help file, `%s', is longer than %s characters",
	   wc_1,
	   wc_2);
    m_free(wc_1,"wide character string");
    m_free(wc_2,"wide character string");
    m_exit(m_errexit);
    }

switch(j)
    {
    case 0: m_error("Fatal Error: Null tail on new help file.");
	m_exit(m_errexit);
	break;
    case 1:
	*helpext = '0';
	*(helpext + 1) = *htfilenostring;
	*(helpext + 2) = '\0';
	break;
    case 2:
	  *helpext = htfilenostring[0];
	  *(helpext + 1) = htfilenostring[1];
	  *(helpext + 2) = '\0';
	  break;
    default:
	  m_error("Internal error. (helpext)");
	  break;
    }

strcat(helpext, ".ht");
m_openchk(&m_outfile, helpbase, "w");
outfile = m_outfile;
firstnode = TRUE;

/* keep name for logging id's */
m_free(savehelpfilename, "help file name");
length = strlen(helpbase) + 1;
savehelpfilename = (M_WCHAR *) m_malloc(length, "help file name");
mbstowcs(savehelpfilename, helpbase, length);

} /* end procedure open_new_helpfile */


/* Verify that val will be understandable to TeX as a dimension */
M_WCHAR *okdimen ( val )
M_WCHAR *val;
{
/* returns val if a valid TeX dimension, else NULL */
char s1[M_NAMELEN + 1], s2[M_NAMELEN + 1];
float flt;
int i;
M_WCHAR *valid = NULL;
char *mbyte;

if ( val == NULL ) return ( NULL );

mbyte = MakeMByteString(val);

if (sscanf(mbyte, "%f%s%s", &flt, s1, s2) == 2)
    for (i = 0; i < (sizeof(dimarray) / sizeof(char *)); ++i)
	if (! m_mbmbupstrcmp(dimarray[i], s1))
	    {
	    valid = val;
	    break;
	    }
m_free(mbyte,"multi-byte string");
return (valid);
}  /* END procedure okdimen  */


/* Start a rsect */
void rsectstart(id, pagebreak)
M_WCHAR *id, *pagebreak;
{
savid = checkid(id);
iderr = FALSE;
if (rsectseq)
    {
    M_WCHAR *wc_samepage;

    wc_samepage = MakeWideCharString(QSAMEPAGE);
    if (pagebreak &&
	(rsectsame != (LOGICAL) (! m_wcupstrcmp(pagebreak,wc_samepage))))
	{
	m_err1(
	  "%s specification ignored within a sequence of reference sections",
	       pagebreak);
	}
    m_free(wc_samepage,"wide character string");
    if (rsectholdnew)
	{
	rsectholdnew = FALSE;
	}
    }
else
    {
    rsectsame = (LOGICAL) (vrsectpage(pagebreak) == SAMEPAGE);
    rsectseq = TRUE;
    rsectholdnew = FALSE;
    }

if (rsectsame && m_mblevel("S3")) st4 = TRUE;
didabbrev = FALSE;
if (! rsectsame)
    {  /* Finish off NEED BEGIN if necessary */
    if (m_mblevel("S3")) {}
    else if (m_mblevel("S2")) {}
    else if (m_mblevel("S1")) {}
    /* rsect in TEST shares the same level head as S1 */
    else if (m_mblevel("TEST")) {}
    else {}
    }
else if (m_mblevel("S3")) {}
else if (m_mblevel("S2")) {}
else if (m_mblevel("S1")) {}
/* rsect in TEST shares the same level head as S1 */
	    else if (m_mblevel("TEST")) {}
/* two cases for rsect SAMEPAGE in chapters */
else if (chapst) {}
else /* rsect SAMEPAGE is first element in chapter, suppress rule */
    {}
chapst = TRUE;
}  /* END procedure rsectstart  */


/* Called at the end of a chapter, appendix, or section to end an open
   sequence of rsects */
void rseqend(void)
{
if (rsectseq)
    {
    rsectseq = FALSE;
    rshnewclear();
    }
}

/* Rsectholdnew clear -- have determined a <newpage> in a samepage <rsect>
   sequence is not immediately followed by a new <rsect>.  Hence can output
   the \newpage macro call without concern about side effects on the
   page header */
void rshnewclear(void)
{
if (rsectholdnew)
    {
    rsectholdnew = FALSE;
    }
}

/* Follow search path to find a file, returning qualified name */
M_WCHAR *searchforfile(file)
M_WCHAR *file;
{
M_WCHAR *filename;
SEARCH  *searchp;
char    *mb_file, mb_filename[2048];
int     dir_leng,file_leng;

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


/* show how much memory is available */
void showmemavail(void)
{
int amount=0;
char *p;

while (p = (char *) malloc(1024))
    {
    amount += 1;
    if ( amount >= 512 ) break;
    }
m_exit(0);
}

/* Start a new helpnode */
void starthelpnode(M_WCHAR *id, LOGICAL suppress_topic_map)
{
int i;
char *mbyte;
char mb_nodeid[NODEID_LENGTH+1], nodenum[32];

if (firstnode)
    {
    /* don't end nonexistent previous node */
    firstnode = FALSE;
    }
else
    {
    fprintf(outfile, "</TOPIC>\n\n");
    }

if (outfile != m_outfile)
    {
    m_error("Internal warning:  Outfile has been redirected.");
    m_errcnt--;
    }

fflush(m_outfile);
nodeoffset = ftell(m_outfile);

mbyte = MakeMByteString(helpcharset);
fprintf(outfile, "<TOPIC charset %s>\n", mbyte);
m_free(mbyte,"multi-byte string");

if (nodeoffset == -1L)
    {
    m_error("Internal error, node offset.");
    m_exit(m_errexit);
    }

if (id == NULL)
    {
    strcpy(mb_nodeid, "_TOPIC_ID_");
    m_itoa(autonodenumber, nodenum);
    strcpy(nodestring, nodenum);
    autonodenumber++;
    strcat(mb_nodeid, nodenum);
    mbstowcs(nodeid, mb_nodeid, NODEID_LENGTH);
    }
else
    {
    w_strncpy(nodeid, id, NODEID_LENGTH);
    wcstombs(mb_nodeid, id, NODEID_LENGTH);
    }

/*     fprintf(stderr, "Starting Node ID: %s\n", nodeid); */
mbyte = MakeMByteString(savehelpfilename);
fprintf(idtablefp, "%s: %s %ld\n", mb_nodeid, mbyte, nodeoffset);
m_free(mbyte,"multi-byte string");

if (!suppress_topic_map)
    {
    /* handle TOPIC MAP file */

    if (thisnodelevel == lastnodelevel)
	{ /* this node at same level */
	if (!isfirsttopicmap)
	    {
	    fputs("\n", topicmapfp);
	    }
	else
	    {
	    isfirsttopicmap = FALSE;
	    }
	/* indent routine >here< */
	for (i = thisnodelevel; --i >= 0;) { fputs("  ", topicmapfp); }
	fprintf(topicmapfp, "%s", mb_nodeid);
	}
    /* elseif */
    if (thisnodelevel > lastnodelevel)
	{ /* this node is deeper */
	fputs(" {\n", topicmapfp);  /* keep the '}' balanced */
	/* indent routine >here< */
	for (i = thisnodelevel; --i >= 0;) { fputs("  ", topicmapfp); }
	fprintf(topicmapfp, "%s", mb_nodeid);
	}
    /* else */
    if (thisnodelevel < lastnodelevel)
	{ /* this node is higher */
	fputs("\n", topicmapfp);
	while (thisnodelevel != lastnodelevel--)
	    {
	    /* indent routine >here< */
	    for (i = lastnodelevel; --i >= 0;) { fputs("  ", topicmapfp); }
	    /* keep the '{' balanced */
	    fputs("}\n", topicmapfp);
	    }
	for (i = thisnodelevel; --i >= 0;) { fputs("  ", topicmapfp); }
	fprintf(topicmapfp, "%s", mb_nodeid);
	}
    /* get ready for next time */
    lastnodelevel = thisnodelevel;
    }
} /* End starthelpnode(id) */


void mb_starthelpnode(char *id, LOGICAL suppress_topic_map)
{
M_WCHAR *wc;

wc = MakeWideCharString(id);
starthelpnode(wc, suppress_topic_map);
m_free(wc,"wide character string");
}


/* Start a labeled list */
void startlablist(longlabel, width, spacing)
M_WCHAR *longlabel, *width, *spacing;
{
STARTSTUFF;
rshnewclear();
list++;
if (list > MAXLISTLEV)
    m_error("Nesting of <LIST> and <LABLIST> too deep");
fputs("<PARAGRAPH>\n", outfile);
if (list <= MAXLISTLEV)
    {
    if (vspacing(spacing) == TIGHT)
	{
	lablistindented[list] = TRUE;
	}
    else
	{
	lablistindented[list] = FALSE;
	}
    }
}

/* Start a list */
void startlist(type, order, spacing, cont)
M_WCHAR *type, *order, *spacing, *cont;
{
LIST *nextlist;
CONTCHAIN *chain;
CONTCHAIN *xchain;
M_WCHAR *wc;

STARTSTUFF;
rshnewclear();
list++;

/* Set ``paragraph'' for a new list */
if (type && !m_wcmbupstrcmp(type, QORDER))
    fprintf(outfile,
	    "<PARAGRAPH firstindent %d leftindent %d>\n",
	    ORDEREDLISTFIRSTINDENT,
	    ORDEREDLISTFIRSTINDENT);
else
    fprintf(outfile,
	    "<PARAGRAPH firstindent %d leftindent %d>\n",
	    LISTFIRSTINDENT,
	    LISTFIRSTINDENT);

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

if (spacing && ! m_wcmbupstrcmp(spacing, QTIGHT)) lastlist->space = TIGHT;

if (type && order && m_wcmbupstrcmp(type, QORDER))
  m_err2("Incompatible specification for list: %s and %s", type, order);

if (lastlist->type == ORDER)
    {
    if (lastlist->order == ARABIC) {}
    else if (lastlist->order == UROMAN) {}
    else if (lastlist->order == LROMAN) {}
    else if (lastlist->order == UALPHA) {}
    else /* if (lastlist->order == LALPHA) */  {}
    chain = (CONTCHAIN *) m_malloc(sizeof(CONTCHAIN), "list chain");
    chain->next = lastlist->chain;
    lastlist->chain = chain;
    chain->where = ftell(outfile);
    }

if (lastlist->type == PLAIN) {}
else if (lastlist->type == BULLET) {}
else if (lastlist->type == CHECK) {}
lastlist->started = TRUE;

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


/* Test for a comment at the beginning of the input document to indicate
   whether the file is a generated index.  If so, set MARKUP e option. */
void testindexfile(first)
FILE *first;
{
char *p;
int c;

for (p = m_mdo; *p ; p++)
    {
    c = getc(first);
    if (c !=*p) return;
    }

for (p = m_comment; *p ; p++)
    {
    c = getc(first);
    if (c !=*p) return;
    }

for (p = "Index"; *p ; p++)
    {
    c = getc(first);
    if (c !=*p) return;
    }

m_entdupchk = FALSE;
}

/* Open and initialize TeX file */
void texinit(void)
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
M_WCHAR wbs, wsl, wcl;
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
strcpy(helpext, "00"); /* first file starts at 00 */
strcat(helpext, ".ht");
m_openchk(&m_outfile, helpbase, "w");
outfile = m_outfile;
firstnode = TRUE;

savehelpfilename = (M_WCHAR *)m_malloc(strlen(helpbase)+1, "help file name");
mbstowcs(savehelpfilename, helpbase, strlen(helpbase) + 1);

/* doc reg file */

strcpy(helpext, ".hmi");
m_openchk(&docregfp, helpbase, "w");


/* id table file */
strcpy(helpext, ".idt");
m_openchk(&idtablefp, helpbase, "wb");

/* topic map file */
strcpy(helpext, ".tpc");
m_openchk(&topicmapfp, helpbase, "wb");

/* Index file */
strcpy(helpext, ".idx");
m_openchk(&indexfp, helpbase, "wb");

/* Sign-on message passed to TeX and generated files. */
/* EXACT FORMAT AND ORDER OF INITIALIZATION PARAMETERS IN .TEX FILE
IS CHECKED DURING TESTING FOR NEED TO REGENERATE .TEX FILE.  */
texcomplete = ftell(outfile);

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
	mbtowc(&wbs, "\\", 1);
	mbtowc(&wsl, "/", 1);
	mbtowc(&wcl, ":", 1);

	if (w_strchr(content, wbs) ||
	    w_strchr(content, wsl) ||
	    w_strchr(content, wcl) )
	    {
	    m_err2("Avoid directory names in FILE entity %s: %s",
		   name,
		   content);
	    m_errline("(Use the SEARCH option instead)\n");
	    }
	}
    }

#if defined(hpux) || defined(_AIX) || defined(sun) || defined(__linux__)
m_openchk(&nullfile, "/dev/null", "w");
#else
m_openchk(&nullfile, "nullfile", "w");
#endif

idxupdate = ftell(outfile);

/* Include cross-reference file */
strcpy(helpext, ".xrh");
loadxref();
postpreamble = ftell(outfile);
}

/* Issue error message and text in document when starting an unimplemented
   element */
void mb_unimp(eltname)
char *eltname;
{
M_WCHAR *wc_stago, *wc_eltname, *wc_tagc;
#if 0
char *p;

fputs("<PARAGRAPH><WEIGHT bold>", outfile);
for (p = m_stago; *p ; p++)
    {
    outchar(*p, outfile);
    }
fputs(eltname, outfile);
for (p = m_tagc; *p ; p++)
    {
    outchar(*p, outfile);
    }
fprintf(outfile, " not implemented for HelpTag. </WEIGHT></PARAGRAPH>\n\n");
#endif /* 0 */

wc_stago   = MakeWideCharString(m_stago);
wc_eltname = MakeWideCharString(eltname);
wc_tagc    = MakeWideCharString(m_tagc);

m_err3("%s%s%s is not implemented for HelpTag", wc_stago, wc_eltname, wc_tagc);

m_free(wc_stago,"wide character string");
m_free(wc_eltname,"wide character string");
m_free(wc_tagc,"wide character string");
}


void unimp(eltname)
M_WCHAR *eltname;
{
char *mb_eltname;

mb_eltname = MakeMByteString(eltname);
mb_unimp(mb_eltname);
m_free(mb_eltname,"multi-byte string");
}


/* convert user-supplied dimension to scaled points */
long usertosp(s)
char *s;
{
double num = 0.0, num2 = 0.0;
int i;
char temp[8];

temp[0] = M_EOS;
if (sscanf(s,"%lf%s%lf",&num, temp, &num2) == 3)
    { /* get numeric portion */
    M_WCHAR *wc_s, *wc_temp;

    wc_s = MakeWideCharString(s);
    wc_temp = MakeWideCharString(temp);
    m_err2("%s: Ignoring number after %s in Tag dimension", wc_s, wc_temp);
    m_free(wc_s,"wide character string");
    m_free(wc_temp,"wide character string");
    }

i=0;
while (temp[i] != M_EOS)
    {
    if (isupper(temp[i])) temp[i] = tolower(temp[i]);
    i++;
    }

if (! m_mbmbupstrcmp(temp, "IN"))
    num *= PTperIN * SPperPT;
else if (! m_mbmbupstrcmp(temp, "CM"))
    num *= PTperIN / CMperIN * SPperPT;
else if (! m_mbmbupstrcmp(temp,"MM"))
    num *= PTperIN / MMperIN * SPperPT;
else if (! m_mbmbupstrcmp(temp, "PC"))
    num *= PTperPC * SPperPT;
else if (! m_mbmbupstrcmp(temp,"PT"))
    num *= SPperPT;
else
    m_error("Invalid units: pt assumed");

if (num<0.0)
    {
    m_error("No negative dimensions: positive assumed");
    num *= -1.0;
    }
return((long) num );
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
GetDefaultHeaderString(
    char          *elementName,
    unsigned char  desiredType,
    char          *defaultString )
{
unsigned char type,wheredef;
M_WCHAR *content;
M_WCHAR *path;
M_WCHAR	*wc_elementName;
char    *mb_content;
char    *retval;

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


/*
 * Look for an entity by the name of "LanguageElementDefaultLocale".
 * If found, set the current locale to be the content of that entity.
*/
void
SetDefaultLocale()
{
unsigned char type,wheredef;
M_WCHAR *elementName;
M_WCHAR *content;
char    *mb_content;

elementName = MakeWideCharString("LanguageElementDefaultLocale");
if (m_lookent(elementName, &type, &content, &wheredef))
    {
    if (type == M_SDATA)
	{
	mb_content = MakeMByteString(content);
	setlocale(LC_ALL, mb_content);
	m_free(mb_content,"multi-byte string");
	}
    else
	setlocale(LC_ALL, "");
    }
else
    setlocale(LC_ALL, "");
m_free(elementName,"wide character string");

elementName = MakeWideCharString("LanguageElementDefaultCharset");
if (m_lookent(elementName, &type, &content, &wheredef))
    {
    if (type == M_SDATA)
	{
	m_free(helpcharset, "help charset");
	helpcharset = m_malloc(w_strlen(content) + 1, "help charset");
	w_strcpy(helpcharset, content);
	}
    }
m_free(elementName,"wide character string");
}


void paragraph(M_WCHAR *indent,
	       M_WCHAR *id,
	       M_WCHAR *gentity,
	       M_WCHAR *gposition,
	       M_WCHAR *ghyperlink,
	       M_WCHAR *glinktype,
	       M_WCHAR *gdescription,
	       char    *listinfo)
{
if (id)
    {
    mbstowcs(xrefstring, "\\<xref paragraph>", 400);
    xstrlen = w_strlen(xrefstring);
    m_getline(&xrffile, &xrfline);
    if (xrffile == NULL)
	{
	/* set to primary input source */
	xrffile = inputname;
	}
    setid(id, TRUE, FALSE, inchapter, chapstring, xrffile, xrfline, FALSE);

    {
    char *mb_nodeid, *mb_id;

    mb_nodeid = MakeMByteString(nodeid);
    mb_id     = MakeMByteString(id);
    fprintf(idtablefp, "%s> %s\n", mb_nodeid, mb_id);
    m_free(mb_nodeid,"multi-byte string");
    m_free(mb_id,"multi-byte string");
    }

    }
chapst = TRUE;
rshnewclear();
fputs("<PARAGRAPH", outfile);
if (id)
    {
    char *mb_id;

    mb_id = MakeMByteString(id);
    fprintf(outfile, " id %s", mb_id);
    m_free(mb_id,"multi-byte string");
    }

if (st4) st4 = FALSE;

if (indent)
    {
    fprintf(outfile,
	    " firstindent %d leftindent %d",
            LEFTINDENT,
            LEFTINDENT);
    }

handle_link_and_graphic(m_parent(0),
			gentity,
			gposition,
			ghyperlink,
			glinktype,
			gdescription);

fprintf(outfile, "%s>", listinfo);

unleaded = FALSE;
}

void figure(
       M_WCHAR *number,
       M_WCHAR *tonumber,
       M_WCHAR *id,
       M_WCHAR *file,
       M_WCHAR *figpos,
       M_WCHAR *cappos,
       M_WCHAR *oldtype,
       M_WCHAR *xwidth,
       M_WCHAR *xdepth,
       M_WCHAR *xhadjust,
       M_WCHAR *xvadjust,
       M_WCHAR *border,
       M_WCHAR *type,
       M_WCHAR *xmagnify,
       M_WCHAR *video,
       M_WCHAR *strip,
       M_WCHAR *mirror,
       M_WCHAR *margin,
       M_WCHAR *clip,
       M_WCHAR *penwidth,
       M_WCHAR *snap,
       M_WCHAR *autoscale,
       M_WCHAR *plottype,
       M_WCHAR *callout,
       M_WCHAR *textsize,
       M_WCHAR *ghyperlink,
       M_WCHAR *glinktype,
       M_WCHAR *gdescription,
       char    *listinfo)
{
unsigned char etype,wheredef;
char *string = 
    GetDefaultHeaderString("FigureElementDefaultHeadingString",
			   M_SDATA,
			   "Figure");

fprintf(outfile, "<PARAGRAPH%s>\n", listinfo);
rshnewclear() ;
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
    m_err1(
    "Explicit figure number %s inconsistent with NONUMBER option",
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

    {
    char *mb_content;
    static M_WCHAR empty = M_EOS;

    if (!f_content) f_content = &empty;

    mb_content = MakeMByteString(f_content);
    fprintf(outfile, "<FIGURE file %s", mb_content);
    m_free(mb_content,"multi-byte string");
    }

if (id)
    {
    char *mb_id;

    mb_id = MakeMByteString(id);
    fprintf(outfile, " id %s", mb_id);
    m_free(mb_id,"multi-byte string");
    }

/* handle link specific code */
if (!glinktype)
    { /* default to type jump */
    /* do nothing */
    }
else
    { /* type is set, choose which is correct */
    if (!m_wcmbupstrcmp(glinktype, QJUMP))
	{ fputs(" typelink 0", outfile); }
    else if (!m_wcmbupstrcmp(glinktype, QJUMPNEWVIEW))
	{ fputs(" typelink 1", outfile); }
    else if (!m_wcmbupstrcmp(glinktype, QDEFINITION))
	{ fputs(" typelink 2", outfile); }
    else if (!m_wcmbupstrcmp(glinktype, QEXECUTE))
	{ fputs(" typelink 3", outfile); }
    else if (!m_wcmbupstrcmp(glinktype, QMAN))
	{ fputs(" typelink 4", outfile); }
    else if (!m_wcmbupstrcmp(glinktype, QAPPDEFINED))
	{ fputs(" typelink 5", outfile); }
    else /* parser won't let this case happen */
	{
	M_WCHAR *wc_stago, *wc_tagc;

	wc_stago = MakeWideCharString(m_stago);
	wc_tagc  = MakeWideCharString(m_tagc);
	m_err3("%sLINK%s (glinktype=``%s'') did not match an allowed value",
	       wc_stago,
	       wc_tagc,
	       glinktype); 
	m_free(wc_stago,"wide character string");
	m_free(wc_tagc,"wide character string");
	}
    }
if (ghyperlink)
    {
    char *mb_ghyperlink;

    mb_ghyperlink = MakeMByteString(ghyperlink);
    fprintf(outfile, " link \"%s\"", mb_ghyperlink);
    m_free(mb_ghyperlink,"multi-byte string");
    }
else
    {
    if (glinktype || gdescription)
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
	fputs(" glink _undefined", outfile);
	}
    }

if (gdescription)
    {
    char *mb_gdescription;

    mb_gdescription = MakeMByteString(gdescription);
    fprintf(outfile, " description \"%s\"", mb_gdescription);
    m_free(mb_gdescription,"multi-byte string");
    }
/* end of link specific code */

/* if caption position is not specified, have it track the figure position */
if (!cappos)
    cappos = figpos;

if (figpos)
    switch (vcenter(figpos))
	{
	case LEFT:
	    fputs(" left", outfile);
	    break;
	case CENTER:
	    fputs(" center", outfile);
	    break;
	case RIGHT:
	    fputs(" right", outfile);
	    break;
	}
if (cappos)
    switch (vcenter(cappos))
	{
	case LEFT:
	    fputs(" cleft", outfile);
	    break;
	case CENTER:
	    fputs(" ccenter", outfile);
	    break;
	case RIGHT:
	    fputs(" cright", outfile);
	    break;
	}
fputs(" ctop>\n", outfile);

if (ftonumber)
    {
    fprintf(outfile, "%s %d.  ", string, figno);
    }
m_free(string, "default header string return");
}

void endterm( M_WCHAR *base, M_WCHAR *gloss, int linktype)
{
M_WCHAR *p;
M_WCHAR wnl, wsp, wus;

tooutput = TRUE;

mbtowc(&wnl, "\n", 1);
mbtowc(&wsp, " ", 1);
mbtowc(&wus, "_", 1);

/* strip possible newlines out of base form */
if (base)
    {
    for (p = base; *p ; p++)
	{
	if (*p == wnl) *p = wsp;
	}
    }

/* terminate the term string */
if (termp - term > MAXTERM) termp = &term[MAXTERM];
*termp = M_EOS;

/* trim possible last space */
if (termp - term > 1 && *(termp-1) == wsp)
    {
    *(termp-1) = M_EOS;
    }

/* Check if it should be entered into the glossary structure */
if (vgloss(gloss) == GLOSS)
    {
    m_ntrtrie(base ? base : term, &gtree, (M_TRIE *) ENTERTERM);
    fprintf(outfile, "<LINK %d \"", linktype);
    for (p = base ? base : term; *p; p++)
	{
	if (*p == wsp)
	    {
	    outchar(wus, outfile);
	    }
	else
	    {
	    outchar(wc_toupper(*p), outfile);
	    }
	}
    fputs("_DEF\">", outfile);
    }

/* Handle the output */
mb_strcode("<WEIGHT bold>", outfile);

if (m_mblevel("EXAMPLESEG") || m_mblevel("IMAGE"))
    {
    /* Are we in one of these? */
    for(p = term; *p; p++)
	{
	exoutchar(*p);
	}
    }
else
    { /* normal case */
    for(p = term; *p; p++)
	{
	outchar(*p, outfile);
	}
    }

mb_strcode("</WEIGHT>", outfile);

if (vgloss(gloss) == GLOSS)
    {
    mb_strcode("</LINK>", outfile);
    }

if (echo) mb_echohead("++");
}

M_WCHAR wc_toupper(M_WCHAR wc)
{
if ((wc >= 0) && (wc <= 255))
    {
    return _toupper(wc);
    }
return wc;
}
