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
/* $XConsortium: entout.c /main/3 1995/11/08 11:04:43 rswiston $ */
/* Copyright (c) 1986, 1987, 1988, 1989 Hewlett-Packard Co. */

/* Entout has procedures to write generated entity TRIE */

void entptr(
#if defined(M_PROTO)
  M_ENTITY *data
#endif
  ) ;

char *typetype(
#if defined(M_PROTO)
  int n
#endif
  ) ;

char *typewhere(
#if defined(M_PROTO)
  unsigned char n
#endif
  ) ;

/* Macro names written to output file */
char xdbuild[] = "M_DBUILD" ;
char xdeltdef[] = "M_DELTDEF" ;
char xgeneral[] = "M_GENERAL" ;
char xsystem[] = "M_SYSTEM" ;
char xstarttag[] = "M_STARTTAG" ;
char xendtag[] = "M_ENDTAG" ;
char xmd[] = "M_MD" ;
char xms[] = "M_MS" ;
char xpi[] = "M_PI" ;
char xcdataent[] = "M_CDATAENT" ;
char xsdata[] = "M_SDATA" ;
char xcodepi[] = "M_CODEPI" ;
char xcodesdata[] = "M_CODESDATA" ;
char xerror[] = "ERROR" ;

/* Outputs entity definitions */
#define ENTFILENAME 12
void entout(fname)
  char *fname ;
  {
    char efilename[ENTFILENAME] ;
    int count = 1 ;
    M_ENTITY *ent ;
    M_WCHAR *p ;
    int conindex ;
    int nameindex ;
    LOGICAL start ;

    strncpy(efilename, fname, ENTFILENAME) ;
    strncpy(&efilename[strlen(efilename)], ".h", 2) ;
    m_openchk(&entfile, efilename, "w") ;

    fprintf(entfile, "#include \"entdef.h\"\n") ;
    fprintf(entfile, "#if defined(M_ENTDEF)\n") ;
    fprintf(entfile, "#define M_ENTEXTERN\n") ;
    fprintf(entfile, "#define M_ENTINIT(a) = a\n") ;
    fprintf(entfile, "#else\n") ;
    fprintf(entfile, "#define M_ENTEXTERN extern\n") ;
    fprintf(entfile, "#define M_ENTINIT(a)\n") ;
    fprintf(entfile, "#endif\n\n") ;

    if (m_entcnt) {
      for (ent = firstent, conindex = 0, nameindex = 0 ;
           ent ; ent = ent->next) {
        if (ent->content) conindex += w_strlen(ent->content) + 1 ;
        nameindex += w_strlen(ent->name) + 1 ;
        }
      fprintf(entfile, "M_ENTEXTERN M_WCHAR m_entcon[%d]\n",
        conindex ? conindex : 1) ;
      if (conindex) {
        fputs("#if defined(M_ENTDEF)\n  = {\n", entfile) ;
        start = FALSE ;
        for (ent = firstent ; ent ; ent = ent->next)
          if (ent->content) {
            if (start) fputs(",\n", entfile) ;
            start = TRUE ;
            for (p = ent->content ; *p ; p++)
              fprintf(entfile, "  %d,\n", (int)*p) ;
            fputs("  0", entfile) ;
            }
        fprintf(entfile, "}\n#endif\n") ;
        }
      fputs("  ;\n\n", entfile) ;

      fprintf(entfile, "M_ENTEXTERN M_WCHAR m_entname[%d]\n", nameindex) ;
      fputs("#if defined(M_ENTDEF)\n  = {\n", entfile) ;
      for (ent = firstent ; ent ; ent = ent->next) {
        for (p = ent->name ; *p ; p++)
          fprintf(entfile, "  %d,\n", (int)*p) ;
        if (ent != lastent) fputs("  0,\n", entfile) ;
        else fputs("  0\n", entfile) ;
        }
      fprintf(entfile, "}\n#endif\n  ;\n\n") ;

      }
    else {
      fputs("M_ENTEXTERN M_WCHAR m_entcon[1] ;\n", entfile) ;
      fputs("M_ENTEXTERN M_WCHAR m_entname[1] ;\n", entfile) ;
      }

    fprintf(entfile, "M_ENTEXTERN M_ENTITY m_entities[%d]\n",
      m_entcnt ? m_entcnt : 1) ;
    if (m_entcnt) {
      fprintf(entfile, "#if defined(M_ENTDEF)\n  = {\n") ;
      for (ent = firstent, conindex = 0, nameindex = 0 ;
           ent ; ent = ent->next) {
        fprintf(entfile, "  {%s, %s, ",
          typetype(ent->type),
          typewhere(ent->wheredef)) ;
        if (ent->content) {
          fprintf(entfile, "&m_entcon[%d]", conindex) ;
          conindex += w_strlen(ent->content) + 1 ;
          }
        else fputs(" NULL", entfile) ;
        fprintf(entfile, ", &m_entname[%d]", nameindex) ;
        nameindex += w_strlen(ent->name) + 1 ;
#if defined(BUILDEXTERN)
        fprintf(entfile, ", %d", ent->index) ;
        if (ent != lastent)
          fprintf(entfile, ", &m_entities[%d], 0}", ent->index) ;
        else fputs(", NULL, 0}", entfile) ;
#else
        fprintf(entfile, ", %d}", ent->codeindex) ;
#endif
        if (ent != lastent) fprintf(entfile, ", \n") ;
        else fprintf(entfile, "}\n#endif\n") ;
        }
      }
    fprintf(entfile, "  ;\n\n") ;

    if (m_enttrie->data) countdown(m_enttrie, &count) ;
    fprintf(entfile,
      "M_ENTEXTERN M_TRIE m_enttrie[%d]\n", count) ;
    if (m_enttrie->data) {
      count = 0 ;
      fputs("#if defined(M_ENTDEF)\n  = {\n  {0, NULL, &m_enttrie[1]}", entfile) ;
      m_dumptrie(entfile, m_enttrie->data, "m_enttrie", &count, entptr) ;
      fprintf(entfile, "}\n#endif\n") ;
      }
    fprintf(entfile, "  ;\n\n") ;
#if defined(BUILDEXTERN)
    fprintf(entfile, "M_ENTEXTERN int m_entcnt M_ENTINIT(%d) ;\n", m_entcnt) ;
#endif

#if defined(BUILDEXTERN)
      if (m_entcnt) {
        fputs("M_ENTEXTERN M_ENTITY *firstent M_ENTINIT(&m_entities[0]) ;\n",
          entfile) ;
        fprintf(entfile,
          "M_ENTEXTERN M_ENTITY *lastent M_ENTINIT(&m_entities[%d]) ;\n",
          m_entcnt - 1) ;
        }
      else {
        fputs("M_ENTEXTERN M_ENTITY *firstent M_ENTINIT(NULL) ;\n", entfile) ;
        fputs("M_ENTEXTERN M_ENTITY *lastent M_ENTINIT(NULL) ;\n", entfile) ;
        }
#endif

  } /* End entout() */

/* Entptr is called by m_dumptrie to output the value stored with a
   particular entity in the entity trie */
void entptr(data)
  M_ENTITY *data ;
  {
    fprintf(entfile, "(M_TRIE *) &m_entities[%d]", data->index - 1) ;
    }

/* Typetype returns a string indicating the type of the nth entity.*/
char *typetype(n)
  int n ;
  {
    switch(n) {
      case M_GENERAL: return(xgeneral) ;
      case M_SYSTEM: return(xsystem) ;
      case M_STARTTAG: return(xstarttag) ;
      case M_ENDTAG: return(xendtag) ;
      case M_MD: return(xmd) ;
      case M_MS: return(xms) ;
      case M_PI: return(xpi) ;
      case M_CDATAENT: return(xcdataent) ;
      case M_SDATA: return(xsdata) ;
      case M_CODEPI: return(xcodepi) ;
      case M_CODESDATA: return(xcodesdata) ;
      default: return(xerror) ;
      }
    }

/* Typewhere returns a string indicating where the nth entity was defined.*/
#if defined(M_PROTO)
char *typewhere(unsigned char n)
#else
char *typewhere(n)
  unsigned char n ;
#endif
  {
    switch(n) {
      case M_DBUILD: return(xdbuild) ;
      case M_DELTDEF: return(xdeltdef) ;
      case FALSE: return("0") ;
      }
    return("0");
    }



