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
/* $XConsortium: fclndir.c /main/3 1995/11/08 10:32:12 rswiston $ */
/* Copyright (c) 1987-1990 Hewlett-Packard Co.

Fclndir.c compares two files, ignoring C line directives that appear
within them as well as blank lines, and leading white space.  It exits
with an error code of 0 if the files are the same, 1 if they differ, and
2 if the program was called incorrectly.
*/

/* Feb. 11, 1991, CED:  Added v2 fixes to v3 version of fclndir.
   Thus, both versions now copy the source file to the target if
   the files differ.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if defined(MSDOS)
#include <process.h>
#endif
#include "basic.h"
#define SAME 0
/* different defined to be 0 as of 12-1-89, because fclndir now does
the file-copy if the files are different, or if the second file
doesn't exist.  (different used to be 1.)  --ced */
#define DIFFERENT 0
#define ERROR 2

struct data {
  int c ;
  LOGICAL linestart ;
  char *p ;
  char *q ;
  } data1, data2 ;

char linedir[] = "#line" ;
char *p = linedir, *q = linedir + 1 ;

void main(
#if defined(M_PROTO)
  int argc, char **argv
#endif
  ) ;

void copyfile(
#if defined(m_proto)
  char *pfile1, char *pfile2
#endif
  ) ;

int nextchar(
#if defined (M_PROTO)
  FILE *file, struct data *data
#endif
  ) ;

void main(argc, argv)
  int argc ;
  char **argv ;
  {
    FILE *one, *two ;
    int c1, c2 ;

    if (argc != 3) {
      fputs("Usage: fclndir file1 (=source) file2 (=target)\n", stderr) ;
      exit(ERROR) ;
      }
    if (! (one = fopen(argv[1], "r"))) {
      fprintf(stderr, "ERROR: source file unavailable: %s\n", argv[1]) ;
      exit(ERROR) ;
      }
    if (! (two = fopen(argv[2], "r"))) {
      fprintf(stderr, "No target file %s; copying source file...\n", argv[2]) ;
 /* call to copyfile entered by ced, 12-29-89. */
      copyfile(argv[1],argv[2]);
      exit(DIFFERENT) ;
      }
    data1.linestart = data2.linestart = TRUE ;
    data1.p = data2.p = linedir ;
    data1.q = data2.q = linedir + 1 ;
    c1 = nextchar(one, &data1) ;
    c2 = nextchar(two, &data2) ;
    while (c1 != EOF && c2 != EOF) {
      if (c1 != c2) {
        fprintf(stderr, "%s and %s are different; copying source...\n", 
		argv[1], argv[2]) ;
/* call to copyfile entered by ced, 12-1-89. */
       copyfile(argv[1],argv[2]);
       exit(DIFFERENT) ;
        }
      c1 = nextchar(one, &data1) ;
      c2 = nextchar(two, &data2) ;
      }
    if (c1 != c2) {
      fprintf(stderr, "%s and %s are different; copying source...\n", 
	      argv[1], argv[2]) ;
/* call to copyfile entered by ced, 12-1-89. */
       copyfile(argv[1],argv[2]);
       exit(DIFFERENT) ;
      }
    fprintf(stderr, "%s and %s are the same\n", argv[1], argv[2]) ;
    exit(SAME) ;
    }

/* copyfile inserted by ced, 12-1-89. */
void copyfile(pfile1,pfile2)
  char *pfile1, *pfile2; 
{
	int ret;
	char *pcmd;

/* malloc space for the system command: two filenames, plus a command,
   spaces, and the terminating null */
	pcmd = (char *) malloc(strlen(pfile1) + strlen(pfile2) + 8);
#if defined(MSDOS)
	ret = sprintf(pcmd,"copy %s %s",pfile1,pfile2);
#else
	ret = sprintf(pcmd,"cp %s %s",pfile1,pfile2);
#endif
	ret = system(pcmd);
	ret = sprintf(pcmd,"touch %s",pfile2);
	ret = system(pcmd);
}

int nextchar(file, data)
  FILE *file ;
  struct data *data ;
  {
    while (data->linestart) {
      data->linestart = FALSE ;
      for (data->p = linedir ; *data->p; data->p++)
        if ((data->c = getc(file)) != (int) *data->p) break ;
      /* Found a line directive, skip remainder of line */
      if (! *data->p) {
        data->c = getc(file) ;
        while (data->c != '\n' && data->c != EOF)
          data->c = getc(file) ;
        data->linestart = TRUE ;
        continue ;
        }
      /* Check for leading blanks */
      else if (data->p == linedir) {
        while (data->c == ' ' || data->c == '\t') data->c = getc(file) ;
        if (data->c == '\n') {
          data->linestart = TRUE ;
          continue ;
          }
        return(data->c) ;
        }
      /* Line began with a prefix of #line */
      data->q = linedir ;
      }

    if (data->q < data->p) return(*data->q++) ;
    else if (data->q++ != data->p) data->c = getc(file) ;
    if (data->c == '\n') data->linestart = TRUE ;
    return(data->c) ;
    }

