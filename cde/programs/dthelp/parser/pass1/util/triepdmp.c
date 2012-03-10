/* $XConsortium: triepdmp.c /main/3 1995/11/08 10:38:13 rswiston $ */
/*
                   Copyright 1986, 1987, 1988, 1989 Hewlett-Packard Co.
*/

#include <stdio.h>
#include "basic.h"
#include "trie.h"

void countdown(
#if defined(M_PROTO)
  M_TRIE *parent, int *count
#endif
  ) ;

void dumpnode(
#if defined(M_PROTO)
  LOGICAL *first, FILE *file, M_TRIE *trieptr, int *count,
    void (*proc)(FILE *file, void *value)
#endif
  ) ;

void dumpptrie(
#if defined(M_PROTO)
  M_TRIE *intname, char *extname, FILE *file, char *externdef, char *defdef,
  void (*proc)(FILE *file, void *value)
#endif
  ) ;

/* Write the declaration of a packed trie to the specified files */
void dumpptrie(intname, extname, file, externdef, defdef, proc)
  M_TRIE *intname ;
  char *extname ;
  FILE *file ;
  char *externdef ;
  char *defdef ;
  void (*proc) (
#if defined (M_PROTO)
    FILE *file,
    void *value
#endif
    ) ;
  {
    LOGICAL first = TRUE ;
    int count = 0 ;
    int treelen = 1 ;

    /* Count nodes in tree */
    countdown(intname, &treelen); 

    if (treelen > 1) {
      fprintf(file, "%s M_PTRIE %s[%d]\n#if defined(%s)\n = {\n",
        externdef, extname, treelen - 1, defdef) ;
      dumpnode(&first, file, intname, &count, proc) ;
      fprintf(file, "}\n#endif\n  ;\n\n") ;
      }
    else {
      fprintf(file, "%s M_PTRIE %s[1] ;\n\n", externdef, extname) ;
      }
    }
