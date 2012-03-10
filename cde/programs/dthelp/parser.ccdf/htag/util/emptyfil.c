/* $XConsortium: emptyfil.c /main/3 1995/11/08 11:38:06 rswiston $ */
/* Copyright (c) 1988, 1989 Hewlett-Packard Co. */
/* Creates a file with nothing in it */
#include <stdio.h>
#if defined(MSDOS)
#include <process.h>
#endif
#include "basic.h"

void main(
#if defined(M_PROTO)
  int argc, char **argv
#endif
  ) ;

void main(argc, argv)
  int argc ;
  char **argv ;
  {
    if (argc != 2) {
      fprintf(stderr, "Usage: emptyfil filename\n") ;
      exit(1) ;
      }
    if (! fopen(argv[1], "w"))
      fprintf(stderr, "Unable to open %s\n", argv[1]) ;
    return 0;
    }
