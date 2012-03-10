/* $XConsortium: sparse.h /main/3 1995/11/08 11:42:50 rswiston $ */
/* Copyright (c) 1986, 1987, 1988, 1989 Hewlett-Packard Co. */

/* Sparse.h contains definitions which allow selection of the sparse
   matrix output option of CONTEXT */

#if defined(sparse)
#define m_newcon(i, j) m_sprscon(i, j)

int m_sprscon(
#if defined(M_PROTO)
  int i, int j
#endif
  ) ;

#else
#define m_newcon(i, j) m_nextcon[i][j]
#endif


