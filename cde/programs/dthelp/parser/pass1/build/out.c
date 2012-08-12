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
/* $XConsortium: out.c /main/3 1995/11/08 10:02:15 rswiston $ */
/*
              Copyright 1986 Tandem Computers Incorporated.
This product and information is proprietary of Tandem Computers Incorporated.
                   Copyright (c) 1986, 1987, 1988, 1989 Hewlett-Packard Co.
*/

/* Out.c contains the procedures used by program BUILD to output results */

#include <string.h>
#include <stdlib.h>
#if defined(MSDOS)
#include <process.h>
#endif
#include "context.h"
#include "build.h"
#include "delim.h"
#include "entdef.h"

#define boolean(x) x ? "TRUE" : "FALSE"

/* Macro names written to dtd.h */
char many[] = "M_ANY" ;
char mcdata[] = "M_CDATA" ;
char mcdataent[] = "CDATAENT" ;
char mcdata_att[] = "M_CDATT" ;
char mcurrent[] = "M_CURRENT" ;
char mentatt[] = "M_ENTATT" ;
char merror[] = "M_ERROR" ;
char mgeneral[] = "" ;
char midrefs[] = "M_IDRFS" ;
char midref[] = "M_IDRF" ;
char mid[] = "M_ID" ;
char mkeyword[] = "M_KEYWORD" ;
char mnamedef[] = "M_NAMEDEF" ;
char mnamepar[] = "M_NAMEPAR" ;
char mnames[] = "M_NAMES" ;
char mnmtkns[] = "M_NMSTOKEN" ;
char mnmtoken[] = "M_NMTOKEN" ;
char mnone[] = "M_NONE" ;
char mnulldef[] = "M_NULLDEF" ;
char mnumbers[] = "M_NUMS" ;
char mnumber[] = "M_NUMBER" ;
char mnutkns[] = "M_NUSTOKEN" ;
char mnutoken[] = "M_NUTOKEN" ;
char mrcdata[] = "M_RCDATA" ;
char mregexp[] = "M_REGEXP" ;
char mrequired[] = "M_REQUIRED" ;

/* Deftype returns a string indicating the default type of the nth parameter.
   */
char *deftype(n)
  int n ;
  {
    switch (n) {
      case NAME: return(mnamedef) ;
      case CURRENT: return(mcurrent) ;
      case REQUIRED: return(mrequired) ;
      case NULLDEF: return(mnulldef) ;
      default:
        errexit = ERREXIT ;
        return(merror) ;
      }
    }

/* Called after all input is read to generate output */
void done(M_NOPAR)
  {

    if (! eltree.data) {
      warning("Error: no elements specified") ;
      return ;
      }
    eltreeout() ;
    parout() ;
    srefout() ;
    entout("entity") ;
    fsa() ;
    exout() ;
    template() ;
    }

/* Prints data value of an entry in the element name tree */
void dumpentnode(file, value)
  FILE *file ;
  M_TRIE *value ;
  {

    fprintf(file, ", %d", ((ELTSTRUCT *) value)->eltno) ;
    }

/* Prints data value of an entry in the trie of short reference map names,
   reporting any maps that are referenced but not defined */
void dumpmapnode(file, value)
  FILE *file ;
  M_TRIE *value ;
  {
    fprintf(file, ", %d", ((MAP *) value)->map) ;
    if (! ((MAP *) value)->defined)
      warning1("Short reference map \"%s\" referenced but not defined.",
        ((MAP *) value)->mapname) ;
    }


/* Prints data value of an entry in the trie of short reference delimiters */
void dumpsrefnode(file, value)
  FILE *file ;
  M_TRIE *value ;
  {
    fprintf(file, ", %d", ((SREFSTRUCT *) value)->srefcnt) ;
    }


/* Controls printing of element blocks in alphabetical order to the
   template file */
void eltblocks(tempfile)
  FILE *tempfile ;
  {
    int n ;
    M_TRIE *node[M_NAMELEN + 1] ;
    M_TRIE *current ;
    char symbol ;

    n = 0 ;
    current = eltree.data ;
    while (TRUE) {
      symbol = current->symbol ;
      node[n] = current->next ;
      if (! symbol) {
        tempelt((ELTSTRUCT *) current->data, tempfile) ;
        current = current->next ;
        while (! current) {
          n-- ;
          if (n < 0) return ;
          current = node[n] ;
          }
        }
      else {
        current = current->data ;
        n++ ;
        }
      }
    }


/* Writes data about elements */
void eltreeout(M_NOPAR)
  {
    M_WCHAR *p ;
    int enameindex = 0 ;
    ELTSTRUCT *eltp ;
    LOGICAL first ;

    m_openchk(&dtd, "dtd.h", "w") ;

    fputs("#if defined(M_DTDDEF)\n", dtd) ;
    fputs("#define M_DTDEXTERN\n", dtd) ;
    fputs("#define M_DTDINIT(a) = a\n", dtd) ;
    fputs("#else\n", dtd) ;
    fputs("#define M_DTDEXTERN extern\n", dtd) ;
    fputs("#define M_DTDINIT(a)\n", dtd) ;
    fputs("#endif\n\n", dtd) ;

    fputs("#include \"dtddef.h\"\n\n", dtd) ;

    dumpptrie(&eltree, "m_entree", dtd, "M_DTDEXTERN", "M_DTDDEF",
              dumpentnode) ;

    fprintf(dtd, "M_DTDEXTERN M_WCHAR m_ename[%d]\n", enamelen) ;
    fputs("#if defined(M_DTDDEF)\n  = {\n", dtd) ;
    first = TRUE ;
    for (eltp = firstelt ; eltp ; eltp = eltp->next) {
      if (first) first = FALSE ;
      else fputs(",\n", dtd) ;
      for (p = eltp->enptr ; *p ; p++)
        fprintf(dtd, "  %d,\n", *p) ;
      fputs("  0", dtd) ;
      }
    fputs(ndif, dtd) ;


    fprintf(dtd, "M_DTDEXTERN int m_elcount M_DTDINIT(%d) ;\n", ecount) ;
    fprintf(dtd,
      "M_DTDEXTERN M_ELTSTRUCT m_element[%d]\n#if defined(M_DTDDEF)\n  = {\n",
      ecount) ;
    first = TRUE ;
    for (eltp = firstelt ; eltp ; eltp = eltp->next) {
      if (first) first = FALSE ;
      else fputs(",\n", dtd) ;
      if (! eltp->content)
        warning1("No content model for element %s", eltp->enptr) ;
      fprintf(dtd, "  %d, %d, %s, ",
        enameindex, eltp->model ? eltp->model->count : 0,
        typecon(eltp->content)) ;
      fprintf(dtd, "%d, %d, ", eltp->inptr, eltp->exptr) ;
      fprintf(dtd, "%d, %d, %d",
        eltp->parindex, eltp->paramcount, eltp->srefptr) ;
      enameindex += w_strlen(eltp->enptr) + 1 ;
      fprintf(dtd, ", %s, %s, %s",
        boolean(eltp->stmin), boolean(eltp->etmin), boolean(eltp->useoradd)) ;
      }
    fputs(ndif, dtd) ;
    }

/* Enttype returns a string indicating the type of an entity */
char *enttype(n)
  int n ;
  {
    switch(n) {
      case M_GENERAL: return(mgeneral) ;
      case M_SYSTEM: return(entsystem) ;
      case M_STARTTAG: return(entst) ;
      case M_ENDTAG: return(entet) ;
      case M_MD: return(entmd) ;
      case M_PI: return(entpi) ;
      case M_CDATAENT: return(cdata) ;
      case M_SDATA: return(entsdata) ;
      default:
        errexit = ERREXIT ;
        return(merror) ;
      }
    }

/* Outputs exception lists */
void exout(M_NOPAR)
  {
    EXCEPTION *ex ;
    int exindex = 0 ;
    LOGICAL first = TRUE ;

    fputs("M_DTDEXTERN M_EXCEPTION m_exception", dtd) ;
    if (excount) {
      fprintf(dtd,
      "[%d]\n#if defined(M_DTDDEF)\n  = {\n",
        excount) ;
      for (ex = firstex ; ex ; ex = ex->nextptr) {
        if (! first) fputs(",\n", dtd) ;
        first = FALSE ;
        exindex++ ;
        fprintf(dtd, "  %d, %d", ex->element, ex->next ? exindex + 1 : 0) ;
        }
      fputs(ndif, dtd) ;
      }
    SUBONE
    }

/* Outputs FSA definitions */
void fsa(M_NOPAR)
  {
    int arcount = 0 ;
    STATE *pstate ;
    FILE *farc ;
    LOGICAL first ;
    ANDGROUP *pand ;
    ARC *parc ;

    m_openchk(&farc, "arc.h", "w") ;

    fprintf(dtd,
      "M_DTDEXTERN M_STATESTRUCT m_state[%d]\n#if defined(M_DTDDEF)\n = {\n",
      stateused) ;

    for (pstate = firststate ; pstate ; pstate = pstate->next) 
      for (parc = pstate->first ; parc ; parc = parc->next) 
        ++arcount ;
    fprintf(farc,
      "M_DTDEXTERN M_ARCSTRUCT m_arc[%d]\n#if defined(M_DTDDEF)\n  = {\n",
      arcount) ;

    arcount = 0 ;
    first = TRUE ;
    for (pstate = firststate ; pstate ; pstate = pstate->next) {
      if (first) first = FALSE ;
      else fputs(",\n", dtd) ;
      fprintf(dtd, "  %s, %s, %d",
        boolean(pstate->final), boolean(pstate->datacontent), 
        pstate->first ? ++arcount : 0) ;
      for (parc = pstate->first ; parc ; parc = parc->next) {
        if (arcount > 1) fputs(",\n", farc) ;
        fprintf(farc, "  %d, %s, %d, %d, %d, %d",
          parc->label ? parc->label->eltno : 0,
          boolean(parc->optional),
          parc->minim ? parc->id : 0,
          parc->group ? parc->group->count : 0,
          parc->to->count,
          parc->next ? ++arcount : 0
          ) ;
        }
      }
    fputs(ndif, dtd) ;
    fputs(ndif, farc) ;

    fputs("M_DTDEXTERN M_ANDSTRUCT m_andgroup", dtd) ;
    if (andused) {
      fprintf(dtd, "[%d]\n#if defined(M_DTDDEF)\n  = {\n", andused) ;
      first = TRUE ;
      for (pand = firstand ; pand ; pand = pand->nextptr) {
        if (first) first = FALSE ;
        else fputs(",\n", dtd) ;
        fprintf(dtd, "  %d, %d",
                     pand->start->count,
                     pand->next ? pand->next->count : M_NULLVAL) ;
        }
      fputs(ndif, dtd) ;
      }
    SUBONE
    fclose(farc) ;
    }

/* Writes data about parameters */
void parout(M_NOPAR)
{
PARAMETER *paramp ;
LOGICAL first = TRUE ;
int pnameindex = 0 ;
M_WCHAR *p ;
int kw ;
int defindex = 0 ;
PTYPE *ptypep ;

fputs("M_DTDEXTERN M_WCHAR m_keyword", dtd) ;
if (kwlen)
    {
    fprintf(dtd,
    "[%d]\n#if defined(M_DTDDEF)\n  = {\n", /* keep the "}" balanced */
    kwlen) ;
    first = TRUE ;
    for (ptypep = firstptype ; ptypep ; ptypep = ptypep->nextptr)
	{
	if (first) first = FALSE ;
	else fputs(",\n", dtd) ;
	for (p = ptypep->keyword ; *p ; p++)
	    fprintf(dtd, "  %d,\n", *p) ;
	fputs("  0", dtd) ;
	}
    fputs(ndif, dtd) ;
    }
SUBONE

fputs("M_DTDEXTERN M_WCHAR m_defval", dtd) ;
if (deflen)
    {
    fprintf(dtd,
    "[%d]\n#if defined(M_DTDDEF)\n  = {\n", /* keep the "}" balanced */
    deflen) ;
    first = TRUE ;
    for (paramp = firstpar ; paramp ; paramp = paramp->nextptr)
    if (paramp->defstring)
	{
	if (first) first = FALSE ;
	else fputs(",\n", dtd) ;
	for (p = paramp->defstring ; *p ; p++)
	fprintf(dtd, "  %d,\n", *p) ;
	fputs("  0", dtd) ;
	}
    fputs(ndif, dtd) ;
    }
SUBONE

fputs(
"M_DTDEXTERN struct {\n  int keyword, next ;\n  } m_ptype",
dtd) ;
if (ptypelen)
    {
    fprintf(dtd, "[%d]\n#if defined(M_DTDDEF)\n  = {\n", ptypelen) ;
    /* keep the "}" balanced */
    kw = 0 ;
    first = TRUE ;
    for (ptypep = firstptype ; ptypep ; ptypep = ptypep->nextptr)
	{
	if (first) first = FALSE ;
	else fprintf(dtd, ",\n") ;
	fprintf(dtd, "  %d, %d", kw, ptypep->next) ;
	kw += w_strlen(ptypep->keyword) + 1 ;
	}
    fputs(ndif, dtd) ;
    }
SUBONE

fputs("M_DTDEXTERN M_PARAMETER m_parameter", dtd) ;
if (parcount)
    {
    kw = 0 ;
    fprintf(dtd,
            "[%d]\n#if defined(M_DTDDEF)\n  = {\n", /* keep the "}" balanced */
            parcount) ;
    first = TRUE ;
    for (paramp = firstpar ; paramp ; paramp = paramp->nextptr)
	{
	if (first) first = FALSE ;
	else fputs(",\n", dtd) ;
	fprintf(dtd,
		"  %d, %s, %d, %s, ",
		pnameindex,
		partype(paramp->type),
		paramp->kwlist,
		deftype(paramp->deftype)) ;
	pnameindex += w_strlen(paramp->paramname) + 1 ;
	if (paramp->defval)
	    fprintf(dtd, "&m_keyword[%d]", paramp->defval - 1) ;
	else if (paramp->defstring)
	    {
	    fprintf(dtd, "&m_defval[%d]", defindex) ;
	    defindex += w_strlen(paramp->defstring) + 1 ;
	    }
	else 
	fputs("NULL", dtd) ;
	}
    fputs(ndif, dtd) ;
    }
SUBONE

fputs("M_DTDEXTERN M_WCHAR m_pname", dtd) ;
if (pnamelen)
    {
    fprintf(dtd,
    "[%d]\n#if defined(M_DTDDEF)\n  = {\n", /* keep the "}" balanced */
    pnamelen) ;
    first = TRUE ;
    for (paramp = firstpar ; paramp ; paramp = paramp->nextptr)
	{
	if (first) first = FALSE ;
	else fputs(",\n", dtd) ;
	for (p = paramp->paramname ; *p ; p++) fprintf(dtd, "  %d,\n", *p) ;
	fputs("  0", dtd) ;
	}
    fputs(ndif, dtd) ;
    }
SUBONE

fprintf(dtd, "#define M_MAXPAR %d\n\n", maxpar) ;
}

/* Partype returns a string indicating the type of the nth parameter. */
char *partype(n)
  int n ;
  {
    switch(n) {
      case GRPO: return(mkeyword) ;
      case CDATA: return(mcdata_att) ;
      case ID: return(mid) ;
      case IDREF: return(midref) ;
      case IDREFS: return(midrefs) ;
      case NAMETYPE: return(mnamepar) ;
      case NAMES: return(mnames) ;
      case NMTOKEN: return(mnmtoken) ;
      case NMTOKENS: return(mnmtkns) ;
      case NUMBER: return(mnumber) ;
      case NUMBERS: return(mnumbers) ;
      case NUTOKEN: return(mnutoken) ;
      case NUTOKENS: return(mnutkns) ;
      case ENTATT: return(mentatt) ;
      default:
        errexit = ERREXIT ;
        return(merror) ;
      }
    }

/* Write short reference information */
void srefout(M_NOPAR)
  {
    LOGICAL first = TRUE ;
    int *mapbysref ;
    SREFSTRUCT *srefp ;
    SREFDATA *data ;
    int count = 0 ;
    int thisrow ;
    int i, j ;

    fprintf(dtd, "#define M_MAXSR %d\n", maxsr) ;
    fprintf(dtd, "#define M_MAXSEQ %d\n", maxseq) ;
    fprintf(dtd, "#define M_SREFCNT %d\n", sreflen) ;
    dumpptrie(&maptree, "m_maptree", dtd, "M_DTDEXTERN", "M_DTDDEF",
              dumpmapnode) ;
    dumpptrie(&sreftree, "m_sreftree", dtd, "M_DTDEXTERN", "M_DTDDEF",
              dumpsrefnode) ;

    fputs("M_DTDEXTERN int m_map", dtd) ;
    if (mapcnt) {
      mapbysref = (int *) calloc(mapcnt * sreflen, sizeof(int)) ;
      if (! mapbysref) {
        m_error("Unable to allocate workspace to process short references") ;
        exit(TRUE) ;
        }
      for (srefp = firstsref ; srefp ; srefp = srefp->next)
        for (data = srefp->data ; data ; data = data->next)
          if (data->entidx)
            mapbysref[sreflen * (data->map - 1) + srefp->srefcnt - 1] =
              data->entidx ;
      fprintf(dtd, "[%d]\n#if defined(M_DTDDEF)\n  = {\n", mapcnt) ;
      for (i = 0 ; i < mapcnt ; i++) {
        if (i) fputs(",\n", dtd) ;
        thisrow = 0 ;
        for (j = 0 ; j < sreflen ; j++)
          if (mapbysref[sreflen * i + j]) thisrow++ ;
        fprintf(dtd, "  %d", thisrow ? count + 1 : M_NULLVAL) ;
        count += thisrow ;
        }
      fputs(ndif, dtd) ;
      }
    SUBONE

    fputs(
      "M_DTDEXTERN M_SREF m_sref", dtd) ;
    if (count) {
      fprintf(dtd, "[%d]\n#if defined(M_DTDDEF)\n  = {\n", count) ;
      first = TRUE ;
      count = 0 ;
      for (i = 0 ; i < mapcnt ; i++) 
        for (j = 0 ; j < sreflen ; )
          if (mapbysref[sreflen * i + j]) {
            count++ ;
            if (first) first = FALSE ;
            else fputs(",\n", dtd) ;
            fprintf(dtd, "  %d, %d, ", j + 1, mapbysref[sreflen * i + j]) ;
            for (j++ ; j < sreflen ; j++)
              if (mapbysref[sreflen * i + j]) break ;
            fprintf(dtd, "%d", j < sreflen ? count + 1 : 0) ;
            }
          else j++ ;
      fputs(ndif, dtd) ;
      }
    SUBONE
    }

/* Output one element block in a template */
void tempelt(eltp, tempfile)
ELTSTRUCT *eltp ;
FILE *tempfile ;
{
PARAMETER *paramp ;
PTYPE *ptypep ;
M_WCHAR *p ;
char *mb_paramname;
int indent ;
int i ;

fprintf(tempfile, "\n<ELEMENT %s>\n", eltp->enptr) ;
if (eltp->parptr)
    {
    fputs("  /*\n", tempfile) ;
    fputs("  <PARAM>\n", tempfile) ;
    for (paramp = eltp->parptr ; paramp ; paramp = paramp->next)
	{
	fputs("    ", tempfile) ;
	indent = 9 ;
	for (p = paramp->paramname ; *p ; p++, indent += 2)
	    {
	    int length;
	    char mbyte[32]; /* larger than any multibyte character */
	    char *pc, c;

	    length = wctomb(mbyte, *p);
	    if (length < 0)
		m_error("Invalid multibyte character found in an element");
	    else
		{
		pc = mbyte;
		if (length == 1)
		    {
		    c = *pc;
		    putc(isupper(c) ? tolower(c) : c, tempfile) ;
		    }
		else
		    while (--length >= 0) putc(*pc++, tempfile) ;
		}
	    }
	mb_paramname = MakeMByteString(paramp->paramname);
	fprintf(tempfile, " = %s ", mb_paramname) ;
	m_free(mb_paramname,"multi-byte string");
	if (paramp->type == GRPO)
	    {
	    putc('(', tempfile) ;
	    for (ptypep = paramp->ptypep ; ptypep ; )
		{
		char *mb_keyword;

		mb_keyword = MakeMByteString(ptypep->keyword);
		fprintf(tempfile,
			"%s = %s",
		        ptypep->keyword,
			ptypep->keyword) ;
		m_free(mb_keyword,"multi-byte string");
		ptypep = ptypep->next ? ptypep->nextptr : (PTYPE *) NULL ;
		if (ptypep)
		    {
		    fputs(",\n", tempfile) ;
		    for (i = 0 ; i < indent ; i++) putc(' ', tempfile) ;
		    }
		}
	    putc(')', tempfile) ;
	    }
	fputs(" ;\n", tempfile) ;
	}
    fputs("  */\n", tempfile) ;
    }
fputs("  /*<STRING-CODE>*/\n", tempfile) ;
fputs("  <START-CODE>\n", tempfile) ;
fputs("  <START-STRING><\\START-STRING>\n", tempfile) ;
fputs("  /*<TEXT-CODE>*/\n", tempfile) ;
fputs("  /*<PI-CODE>*/\n", tempfile) ;
fputs("  <END-CODE>\n", tempfile) ;
fputs("  <END-STRING><\\END-STRING>\n", tempfile) ;
}

/* Write template */
void template(M_NOPAR)
  {
    FILE *tempfile ;
    M_ENTITY *ent ;
    LOGICAL undefent = FALSE ;

    m_openchk(&tempfile, "template", "w") ;
    for (ent = firstent ; ent ; ent = ent->next) {
      if (! ent->wheredef) {
        if (! undefent) {
          fputs("/*\n", tempfile) ;
          undefent = TRUE ;
          }
        fprintf(tempfile, "<!ENTITY %s %s \"\">\n",
          ent->name,
          enttype(ent->type)
          ) ;
        }
      }
    if (undefent) fputs("*/\n", tempfile) ;
    fputs("<SIGN-ON>\n", tempfile) ;
    fputs("<GLOBAL-DEFINE>\n<GLOBAL-DECLARE>\n\n", tempfile) ;
    eltblocks(tempfile) ;
    fclose(tempfile) ;
    }

/* Typecon returns a string indicating the content type of the nth element.*/
char *typecon(n)
  int n ;
  {
    switch(n) {
      case GRPO: return(mregexp) ;
      case ANY: return(many) ;
      case NONE: return(mnone) ;
      case CDATA: return(mcdata) ;
      case RCDATA: return(mrcdata) ;
      default:
        errexit = ERREXIT ;
        return(merror) ;
      }
    }

#include "entout.c"  
