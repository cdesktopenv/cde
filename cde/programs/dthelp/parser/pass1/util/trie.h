/* $XConsortium: trie.h /main/3 1995/11/08 10:37:32 rswiston $ */
/*
                   Copyright 1986, 1987, 1988, 1989 Hewlett-Packard Co.
*/

/* Trie.h contains definitions relating to trees used to store
   element names, delimiter strings, short reference strings, etc. */

/* The data structure described here is not a true trie as described
   in Knuth, Vol. 3.  These functions represent entries by a tree
   structure in which there is one level of subnode for each letter
   in an entry.  In a trie, subnodes are not needed as soon as a
   unique prefix has been encountered. */

typedef struct m_trie M_TRIE ;

typedef struct m_ptrie M_PTRIE ;

struct m_trie {
  M_WCHAR symbol ;
  M_TRIE *next ;
  M_TRIE *data ;
  } ;

struct m_ptrie {
  LOGICAL more ;
  M_WCHAR symbol ;
  int index ;
  } ;


M_TRIE *m_gettrienode(
#if defined(M_PROTO)
  M_NOPAR
#endif
  ) ;

void *m_lookfortrie(
#if defined(M_PROTO)
  const M_WCHAR *p, const M_TRIE *xtrie
#endif
  ) ;

void *m_ntrtrie(
#if defined(M_PROTO)
  M_WCHAR *p, M_TRIE *xtrie, void *dataval
#endif
  ) ;

void *m_resettrie(
#if defined(M_PROTO)
  M_TRIE *xtrie, M_WCHAR *p, void *value
#endif
  ) ;
