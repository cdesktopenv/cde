/* $XConsortium: upcmp8.c /main/3 1995/11/08 11:09:12 rswiston $ */
/*
                   Copyright 1986, 1987, 1988, 1989 Hewlett-Packard Co.
*/

/* Compare uppercase versions of two strings of eight-bit characters */

#include <stdio.h>
#include "basic.h"
#include "common.h"
extern M_CHARTYPE m_ctarray[256] ;

int m_wcupstrcmp(
#if defined(M_PROTO)
  const M_WCHAR *p, const M_WCHAR *q
#endif
  ) ;

int m_wcmbupstrcmp(
#if defined(M_PROTO)
  const M_WCHAR *p, const char *q
#endif
  ) ;

int m_mbmbupstrcmp(
#if defined(M_PROTO)
  const char *p, const char *q
#endif
  ) ;

#if defined(M_PROTO)
int m_wcupstrcmp(const M_WCHAR *p, const M_WCHAR *q )
#else
int m_wcupstrcmp(p, q)
  M_WCHAR *p, *q ;
#endif /* M_PROTO */
  {
    do {
      if (m_ctupper(*p) < m_ctupper(*q)) return(-1) ;
      if (m_ctupper(*p) > m_ctupper(*q)) return(1) ;
      q++ ;
      } while (*p++) ;
    return(0) ;
    }

#if defined(M_PROTO)
int m_wcmbupstrcmp(const M_WCHAR *p, const char *q)
#else
int m_wcmbupstrcmp(p, q)
  M_WCHAR *p;
  char *q ;
#endif
  {
  M_WCHAR *wc;
  int      retval;

  wc = MakeWideCharString(q);
  retval = m_wcupstrcmp(p, wc);
  m_free(wc,"wide character string");
  return retval;
  }

#if defined(M_PROTO)
int m_mbmbupstrcmp(const char *p, const char *q)
#else
int m_mbmbupstrcmp(p, q)
  char *p;
  char *q ;
#endif
  {
  M_WCHAR *w_p, *w_q;
  int      retval;

  w_p = MakeWideCharString(p);
  w_q = MakeWideCharString(q);
  retval = m_wcupstrcmp(w_p, w_q);
  m_free(w_p,"wide character string");
  m_free(w_q,"wide character string");
  return retval;
  }
