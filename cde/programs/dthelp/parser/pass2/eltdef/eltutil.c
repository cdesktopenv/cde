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
/* $XConsortium: eltutil.c /main/3 1995/11/08 10:45:02 rswiston $ */
/*
              Copyright 1986 Tandem Computers Incorporated.
This product and information is proprietary of Tandem Computers Incorporated.
                   Copyright (c) 1986, 1987, 1988, 1989 Hewlett-Packard Co.
*/

/* Eltutil.c contains procedures for program ELTDEF */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#if defined(MSDOS)
#include <process.h>
#endif
#include "basic.h"
#include "trie.h"

#include "dtdext.h"

#include "eltdef.h"
#include "delim.h"
#include "context.h"

#include "entext.h"
    
#include "version.h"

/* Output indicated action pointer array to if.h */
void actptrout(array, name)
  ACTION **array;
  char *name;
  {
    int i;

    fprintf(ifh, "M_IFEXTERN int %s[%d]\n#if defined(M_IFDEF)\n  = {\n",
      name, m_elcount);
    for (i = 0 ; i < m_elcount ; i++) {
      if (i > 0) fprintf(ifh, ",\n");
      fprintf(ifh, "  %d", array[i] ? array[i]->count : M_NULLVAL);
      }
    fprintf(ifh, "}\n#endif\n  ;\n");
    }


/* Close a code file and write jump table at the end */
#if defined(M_PROTO)
void closeiffile(LOGICAL flag, FILE *file, int count, char *table, char *proto)
#else
void closeiffile(flag, file, count, table, proto)
  LOGICAL flag;
  FILE *file;
  int count;
  char *table;
  char *proto;
#endif
  {
    int i;

    endcode(flag, file);
    if (count) {
      fprintf(file,
        "void (*m_%stable[])(\n#if defined(M_PROTO)\n  %s\n#endif\n  ) = {\n",
        table, proto);
      fprintf(file, "  m_%s1, /* Place holder for 1-based indexing */\n",
                    table);
      for (i = 1 ; i <= count ; i++) {
        fprintf(file, "  m_%s%d", table, i);
        if (i != count) fputs(",\n", file);
        }
      fputs("} ;\n", file);
      }
    else
      fprintf(file,
        "void (*m_%stable[1])(\n#if defined(M_PROTO)\n  %s\n#endif\n  ) ;\n",
        table, proto);
    fclose(file);
    }

/* Called when the C identifier to be #define'd to a parameter value is
   scanned */
void cvalue(M_NOPAR)
  {
    PARVAL *new;

    new = (PARVAL *) m_malloc(sizeof(PARVAL), "parameter value");
    if ((pval = (PARVAL *) m_ntrtrie(name, &parval, (M_TRIE *) new)))
      m_free(new, "parameter value");
    else {
      new->line = 0;
      new->value = NULL;
      new->cname = (M_WCHAR *) m_malloc(w_strlen(name) + 1, "C name");
      w_strcpy(new->cname, name);
      pval = new;
      }
    }


/* Called after last input character is read to place closing punctuation
   at end of output files and close them */
void done(M_NOPAR)
  {
    M_ENTITY *ent;
    ACTION *actp;
    LOGICAL first;
    int i;

    closeiffile(inent, entfile, codeent, "c", nopar);
    closeiffile(intext, tfile, tactions, "t", tproto);
    closeiffile(inpc, pfile, pactions, "p", pproto);
    closeiffile(insc, sfile, sactions, "s", nopar);
    closeiffile(inec, efile, eactions, "e", nopar);
    closeiffile(instc, stfile, stactions, "st", stproto);

    endstring();
    fclose(string);

    /* Write if.h (which was opened for sign-on message) */
    fprintf(ifh, "M_IFEXTERN int m_gss M_IFINIT(%d) ;\n", gss);
    fprintf(ifh, "M_IFEXTERN int m_ges M_IFINIT(%d) ;\n", ges);
    fputs("M_IFEXTERN struct {\n", ifh);
    fputs("  int data ;\n", ifh);
    fputs("  M_ELEMENT element ;\n", ifh);
    fputs("  int son ;\n", ifh);
    fputs("  int next ;\n", ifh);
    if (actlen) {
      fprintf(ifh,
              "  } m_action[%d]\n#if defined(M_IFDEF)\n  = {\n",
              actlen);

      first = TRUE;
      for (actp = firstact ; actp ; actp = actp->nextact) {
        if (first) first = FALSE;
        else fprintf(ifh, ",\n");
        fprintf(ifh, "  %d, %d, %d, %d",
                actp->data, actp->element,
                actp->son ? actp->son->count : M_NULLVAL,
                actp->next ? actp->next->count : M_NULLVAL);
        }
      fprintf(ifh, "}\n#endif\n  ;\n");
      }
    else fputs("  } m_action[1] ;\n", ifh);

    actptrout(starray, "m_starray");
    actptrout(etarray, "m_etarray");
    actptrout(scarray, "m_scarray");
    actptrout(ecarray, "m_ecarray");
    actptrout(tcarray, "m_tcarray");
    actptrout(pcarray, "m_pcarray");
    actptrout(stcarray, "m_stcarray");

    outstring();
    m_openchk(&pvalh, "pval.h", "w");
    fputs("/* Parameter values specified in the interface */\n", pvalh);
    if (parval.data) outpval(parval.data);

    fclose(ifh);

    /* Report any elements and entities left undefined */
    for (i = 0 ; i < m_elcount ; i++)
      if (! processed[i])
        warning1("Warning: No specification for element %s",
          &m_ename[m_element[i].enptr]);

    for (ent = firstent ; ent ; ent = ent->next)
      if (! ent->wheredef)
        warning1("Warning: Entity %s undefined", ent->name);
    entout("entity2");

    exit(errexit);
    }

/* Called when finished reading a section of code from the input file */
#if defined(M_PROTO)
void endcode(LOGICAL flag, FILE *file)
#else
void endcode(flag, file)
  LOGICAL flag;
  FILE *file;
#endif
  {
    if (flag) fprintf(file, "}}\n\n");
    }

/* Called at the end of all initial fields in the interface definition */
void endini(M_NOPAR)
  {
    if (finitext) 
      fputs("    putc(m_textchar, m_outfile);\n", tfile);
    if (finipi) 
      fputs("    fputs(m_pi, m_outfile) ;\n", pfile);
    if (finistc)
      {
      fputs("    {\n", stfile);
      fputs("    char *mb_string;\n", stfile);
      fputs("    mb_string = MakeMByteString(m_string);\n", stfile);
      fputs("    fputs(mb_string, m_outfile) ;\n", stfile);
      fputs("    m_free(mb_string,\"multi-byte string\");;\n", stfile);
      fputs("    }\n", stfile);
      }
    }

/* End the sign-on message */
#include <time.h>
void endsignon(M_NOPAR)
  {
    time_t storetime;
    char *p;
    char *timeofday;
    static char signon1[] = "Interface generated from ";
    static char signon2[] = " on ";
    int i;
    int len;
    FILE *sgfile;

    signonend = TRUE;
    /* Discard a trailing carriage return in the sign-on message */
    if (socr) sochar--;

    time(&storetime);
    timeofday = ctime(&storetime);

    len =
      sochar+strlen(signon1)+strlen(signon2)+strlen(iffile)+strlen(timeofday)
        /* a byte for end of string marker */
        + 1
        /* a byte for a carriage return after any user-supplied message */
        + (sochar ? 1 : 0);
    m_openchk(&sgfile, "signonx.h", "w");
    fprintf(sgfile, "extern char m_signon[%d] ;\n", len);
    fclose(sgfile);
    m_openchk(&sgfile, "signon.h", "w");
    fprintf(sgfile, "char m_signon[%d] = {\n", len);

    for (i = 0 ; i <sochar ; i++) 
      fprintf(sgfile, "  %d,\n", signonmsg[i]);
    if (sochar) fputs("  10,\n", sgfile);
    for (p = signon1 ; *p ; p++)
      fprintf(sgfile, "  %d,\n", *p);
    for (p = iffile ; *p ; p++)
      fprintf(sgfile, "  %d,\n", *p);
    for (p = signon2 ; *p ; p++)
      fprintf(sgfile, "  %d,\n", *p);
    for (p = timeofday ; *p ; p++)
      fprintf(sgfile, "  %d,\n", *p);
    fputs("  0} ;\n", sgfile);
    fclose(sgfile);
    }

/* Closes a start-string or end-string */
void endstring(M_NOPAR)
  {
    if (instring) {
      if (stringstart) stringstart = FALSE;
      else fprintf(string, ",\n");
      stringcnt++;
      instring = FALSE;
      fprintf(string, "  0");
      }
    /* If called after ENDFILE, ensure at least one character in output file*/
    else if (stringstart) fprintf(string, "  0");
    }

/* Set the type of an entity and check if different than declaration in
   BUILD */
void enttype(type)
  int type;
  {
    if ((entity->type == M_PI && type == M_CODEPI) ||
        (entity->type == M_SDATA && type == M_CODESDATA));
    else if (entity->type != M_GENERAL &&
        entity->type != (unsigned char) type)
      warning1("Redefining type of entity %s", name);
    entity->type = (unsigned char) type;
    }

/* Free storage used for a context-specification chain */
void freechain(M_NOPAR)
  {
    CHAIN *chainp, *dchainp;

    for (chainp = firstchain ; chainp ;) {
      dchainp = chainp;
      chainp = chainp->next;
      m_free(dchainp, "chain");
      }
    firstchain = NULL;
    nextchain = &firstchain;
    }     

/* Returns pointer to data field in action node for current chain of
   elements */
int *getaction(array)
  ACTION **array;
  {
    ACTION *start, *node;
    CHAIN *chainp;

    if (! array[openelt - 1]) {
      array[openelt - 1] = getactstruct();
      array[openelt - 1]->element = openelt + 1;
      }
    start = array[openelt - 1];
    for (chainp = firstchain ; chainp ; chainp = chainp->next) {
      if (! start->son) {
        for ( ; chainp ; chainp = chainp->next) {
          start->son = getactstruct();
          start->son->element = chainp->elt;
          start = start->son;
          }
        freechain();
        return(&start->data);
        }
      for (node = start->son ; node ; start = node, node = node->next)
        if (node->element == chainp->elt) break;
      if (! node) {
        start->next = getactstruct();
        start->next->element = chainp->elt;
        start = start->next;
        }
      else start = node;
      }
    if (start->data) m_error("Duplicate specification");
    freechain();
    return(&start->data);
    }

/* Allocate new action structure */
ACTION *getactstruct(M_NOPAR)
  {
    ACTION *new;

    new = (ACTION *) m_malloc(sizeof(ACTION), "action");
    new->count = ++actlen;
    new->data = M_NULLVAL;
    new->son = new->next = new->nextact = NULL;
    *nextact = new;
    nextact = &new->nextact;
    return(new);
    }

/* Program initialization */
void initialize(M_NOPAR)
{
char    **mb_delims;
M_WCHAR **wc_delims;

fprintf(stderr, "MARKUP System - ELTDEF %s\n", M_VERSION);
fprintf(stderr, "Copyright (c) 1986, 1987, 1988, 1989 Hewlett-Packard Co.\n");

m_openchk(&ifh, "if.h", "w");
m_openchk(&globdef, "globdef.h", "w");
m_openchk(&globdec, "globdec.h", "w");
m_openchk(&pfile, "pfile.c", "w");
/* ELTDEF opens too many files for use with CodeView debugger.  If
this option is set, all code segments are written to one file */
if (debug)
    {
    tfile =
    sfile =
    efile =
    stfile =
    entfile =
    pfile;
    }
else
    {
    m_openchk(&tfile, "tfile.c", "w");
    m_openchk(&sfile, "sfile.c", "w");
    m_openchk(&efile, "efile.c", "w");
    m_openchk(&stfile, "stfile.c", "w");
    m_openchk(&entfile, "entfile.c", "w");
    }
m_openchk(&string, "estring.h", "w");
m_openchk(&m_errfile, "error", "w");
m_openchk(&ifile, iffile, "r");

mb_delims = mb_dlmptr;
wc_delims = m_dlmptr;

while (*mb_delims)
    {
    *wc_delims++ = MakeWideCharString(*mb_delims);
    mb_delims++;
    }
*wc_delims = 0;

/* Start array for sign-on message on globals.h.  Also,
make sure there is at least one line in globals.h, because
DOS copy does not copy an empty file */

fputs("/* Global definitions specified by interface designer*/\n",
globdef);
fputs("/* Global declarations specified by interface designer*/\n",
globdec);

fputs("#if defined(M_IFDEF)\n", ifh);
fputs("#define M_IFEXTERN\n", ifh);
fputs("#define M_IFINIT(a) = a\n", ifh);
fputs("#else\n", ifh);
fputs("#define M_IFEXTERN extern\n", ifh);
fputs("#define M_IFINIT(a)\n", ifh);
fputs("#endif\n", ifh);

startcode(pactions, &inpc, pfile, "p", pproto, pformal, pftype);
startcode(tactions, &intext, tfile, "t", tproto, tformal, tftype);
startcode(sactions, &insc, sfile, "s", nopar, nopar, "");
startcode(eactions, &inec, efile, "e", nopar, nopar, "");
startcode(stactions, &instc, stfile, "st", stproto, stformal, stft);
}

/* Output definitions for strings */
void outstring(M_NOPAR)
  {
    int c;

    m_openchk(&string, "estring.h", "r");
    fprintf(ifh, "M_IFEXTERN char m_string[%d]\n",
            stringcnt > 1 ? stringcnt - 1 : 1);
    if (stringcnt - 1) {
      fputs("#if defined(M_IFDEF)\n  = {\n", ifh);
      while ((c = getc(string)) != EOF) putc(c, ifh);
      fputs("}\n#endif\n", ifh);
      }
    fputs("  ;\n", ifh);
    fclose(string);
    }

/* Output #define's for parameter values */
void outpval(p)
M_TRIE *p;
{
M_WCHAR *q;

for ( ; p ; p = p->next)
    if (p->symbol) outpval(p->data);
    else
	{
	char *mb_cname;

	mb_cname = MakeMByteString(((PARVAL *) p->data)->cname);
	fprintf(pvalh,
		"/* line %d \"%s\" */\n",
	        ((PARVAL *) p->data)->line,
		iffile);
	fprintf(pvalh, "#define %s \"", mb_cname);
	m_free(mb_cname,"multi-byte string");
	if ((q = ((PARVAL *) p->data)->value))
	    for ( ; *q ; q++)
		{
		char mbq[32]; /* larger than largest possible mbyte char */
		int  length;

		length = wctomb(mbq, *q);
		if (length == 1)
		    switch (*mbq)
			{
			case '\n':
			    fputs("\\n", pvalh);
			    break;
			case '"':
			    fputs("\\\"", pvalh);
			    break;
			default:
			    putc(*mbq, pvalh);
			    break;
			}
		else
		    fputs(mbq, pvalh);
		}
	fputs("\"\n", pvalh);
	}
}

/* Skip rest of statement after an error */
void skiptoend(M_NOPAR)
  {
    int i;
    static int errlev = 0;
    CVARSTRUCT *cvarp, *dvarp;

    for (cvarp = cvarlist ; cvarp ;) {
      dvarp = cvarp;
      m_free(cvarp->cvarptr, "C variable name");
      cvarp = cvarp->next;
      m_free(dvarp, "C variable");
      }
    cvarlist = NULL;
    freechain();
    if (! errlev++) {
      curcon = ERROR;
      while (TRUE) {
        i = scan();
        if (i == ENDFILE) break;
        else if (i == ELT) {
          if (restart <= RSIGNON) endsignon();
          restart = RELEMENT;
          curcon = NEEDN;
          break;
          }
        else if (i == GDEF && restart < RGLOBDEF) {
          if (restart <= RSIGNON) endsignon();
          restart = RGLOBDEF;
          curcon = GLOBDEF;
          break;
          }
        else if (i == GDEC && restart < RGLOBDEC) {
          if (restart <= RSIGNON) endsignon();
          restart = RGLOBDEC;
          curcon = GLOBDEC;
          break;
          }
        else if (i == SIGNON && restart < RSIGNON) {
          curcon = INSIGNON;
          restart = RSIGNON;
          break;
          }
        else if (i == ENTSTART && restart == RENTITY) {
          curcon = ENTDEC;
          break;
          }
        curcon = ERROR;
        } /* end while */
      } /* end if ! errlev */
    errlev--;
  }

/* Starts processing a code segment from the input file */
void startcode(caseno, flag, file, prefix, proto, formal, formtype)
int caseno;
LOGICAL *flag;
FILE *file;
char *prefix;
char *proto;
char *formal;
char *formtype;
{
CVARSTRUCT *cvarp;

endcode(*flag, file);
*flag = TRUE;
/* protoype */
fprintf(file,
	"void m_%s%d(\n#if defined(M_PROTO)\n  %s\n#endif\n  ) ;\n",
	prefix,
	caseno,
	proto);

/* ANSI defines */
fputs("#if defined(M_PROTO)\n", file);
fprintf(file, "void m_%s%d(%s)\n", prefix, caseno, proto);
fputs("#else\n", file);
fprintf(file,
	"void m_%s%d(%s)\n%s\n#endif\n  {\n", /* balance the "}" */
	prefix,
	caseno,
	formal,
	formtype);

for (cvarp = cvarlist ; cvarp ; cvarp = cvarp->next)
    {
    char *mb_cvarptr;

    mb_cvarptr = MakeMByteString(cvarp->cvarptr);
    fprintf(file, "    M_WCHAR *%s ;\n", mb_cvarptr);
    m_free(mb_cvarptr,"multi-byte string");
    }

for (cvarp = cvarlist ; cvarp ; cvarp = cvarp->next)
    {
    char *mb_cvarptr;

    mb_cvarptr = MakeMByteString(cvarp->cvarptr);
    fprintf(file,
	    "    m_setparam(&%s, %d) ;\n",
	    mb_cvarptr,
	    cvarp->param);
    m_free(mb_cvarptr,"multi-byte string");
    }
fprintf(file, "{\n/* line %d \"%s\" */\n", m_line, iffile); /* balance "}" */
}

/* Begins processing a new element */
void startelement(M_NOPAR)
  {
    CVARSTRUCT *cvarp;
    CVARSTRUCT *discard;

    for (cvarp = cvarlist ; cvarp ; ) {
      discard = cvarp;
      m_free(cvarp->cvarptr, "C variable name");
      cvarp = cvarp->next;
      m_free(discard, "C variable");
      }
    cvarlist = NULL;
    if ((openelt = m_packedlook(m_entree, name))) {
      if (processed[openelt - 1])
        warning1("Warning: Element %s already processed", name);
      processed[openelt - 1] = TRUE;
      }
    else m_err1("Undefined element: %s", name);
    }

/* Stores the name of a C variable read from the input file */
void storecvar(M_NOPAR)
  {
    CVARSTRUCT *new;

    new = (CVARSTRUCT *) m_malloc(sizeof(CVARSTRUCT), "C variable");
    new->cvarptr = (M_WCHAR *) m_malloc(w_strlen(name) + 1,
				     "C variable name");
    w_strcpy(new->cvarptr, name);
    new->next = cvarlist;
    cvarlist = new;
    }

/* Compares the parameter name associated with a C variable in the input
   file with the names of all parameters of the current element.  Stores
   result for later output with code segments */
void storepname(M_NOPAR)
  {
    int i, par;

    for (i = 0, par = m_element[openelt - 1].parptr;
         i < m_element[openelt - 1].parcount;
         i++, par++)
      if (! w_strcmp(&m_pname[m_parameter[par - 1].paramname], name))
	  break;
    if (i >= m_element[openelt - 1].parcount) {
      m_err2("%s: No such parameter for element %s", name,
        &m_ename[m_element[openelt - 1].enptr]);
      return;
      }
    cvarlist->param = i;
    }

/* Called when a possible parameter value to be defined is encountered */
void value(p)
M_WCHAR *p;
{
char buffer[5];

if (m_partype(cvarlist->param + m_element[openelt - 1].parptr, p))
    {
    if (pval->line)
	{
	if (w_strcmp(pval->value, p))
	    {
	    M_WCHAR *w_buffer;

	    snprintf(buffer, 5, "%d", pval->line);
	    w_buffer = MakeWideCharString(buffer);
	    m_err5("Can't #define %s to %s. %s #define'd to %s on line %s",
		   pval->cname,
		   p,
		   pval->cname,
		   pval->value,
		   w_buffer);
	    m_free(w_buffer, "wide character string");
	    }
	}
    else {
      pval->value = (M_WCHAR *) m_malloc(w_strlen(p) + 1, "pval value");
      w_strcpy(pval->value, p);
      pval->line = m_line;
      }
    }
else m_err3("\"%s\" illegal value for parameter %s of %s",
	    p,
	    &m_pname[m_parameter[cvarlist->param +
		       m_element[openelt - 1].parptr - 1].paramname],
	    &m_ename[m_element[openelt-1].enptr]);
}

#include "paramu.c"
