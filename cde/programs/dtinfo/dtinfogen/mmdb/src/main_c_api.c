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
/* $XConsortium: main_c_api.c /main/5 1996/06/11 17:29:25 cde-hal $ */

#include "oliasdb/Mmdb.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>


void usage(int argc, char** argv)
{
   fprintf(stderr, "Usage:\n");
   fprintf(stderr, "   %s 0 libpath [bookcaseName]\n", argv[0]);
   fprintf(stderr, "   %s 1 libpath bookcaseName locator (show bookcase)\n", argv[0]);
   fprintf(stderr, "   %s 2 libpath bookcaseName locator (show section)\n", argv[0]);
   fprintf(stderr, "   %s 3 libpath bookcaseName mmdboid (show section)\n", argv[0]);
   fprintf(stderr, "   %s 4 libpath bookcaseName mmdboid (show TOC)\n", argv[0]);
   fprintf(stderr, "   %s 5 libpath bookcaseName loc (show LOC)\n", argv[0]);
   /*
   fprintf(stderr, "   %s 6 libpath bookcaseName mmdboid (show LOC)\n", argv[0]);
   */
   fprintf(stderr, "   %s 7 libpath bookcaseName loc (show graphic)\n", argv[0]);
   fprintf(stderr, "   %s 8 libpath bookcaseName mmdboid (show graphic)\n", argv[0]);
   fprintf(stderr, "   %s 9 libpath bookcaseName loc (show stylesheet)\n", argv[0]);
   fprintf(stderr, "   %s 10 libpath bookcaseName mmdboid (show stylesheet)\n", argv[0]);
   fprintf(stderr, "   %s 11 libpath bookcaseName mmdboid (show dlp)\n", argv[0]);
   fprintf(stderr, "   %s 12 libpath bookcaseName TOC_section_mmdboid (show book)\n", argv[0]);
   fprintf(stderr, "   %s 13 libpath bookcaseName book_mmdboid (show book)\n", argv[0]);
   fprintf(stderr, "   %s 14 libpath bookcaseName book_seq_num (show book)\n", argv[0]);
   fprintf(stderr, "   %s 15 info_lib_path info_base_spec base_name number_of_sections max_section_size, min_section_size [true|false](delete test db)\n", argv[0]);
}

extern int test0(int argc, char** argv);
extern int test1(int argc, char** argv);
extern int test2(int argc, char** argv);
extern int test3(int argc, char** argv);
extern int test4(int argc, char** argv);
extern int test_loc1(int argc, char** argv);
extern int test_loc2(int argc, char** argv);
extern int test_graphic1(int argc, char** argv);
extern int test_graphic2(int argc, char** argv);
extern int test_ss1(int argc, char** argv);
extern int test_ss2(int argc, char** argv);
extern int test_dlp(int argc, char** argv);
extern int test_book1(int argc, char** argv);
extern int test_book2(int argc, char** argv);
extern int test_book3(int argc, char** argv);
extern int auto_test_c_api(int argc, char** argv);

int main( int argc, char** argv )
{
   int ok = 0;
   DtMmdbInit();

   if ( argc == 1 || 0==isdigit(argv[1][0]) ) {
      usage(argc, argv);
   } else
      switch ( atoi(argv[1]) ) {
        case 0:
          test0(argc, argv);
          break;
        case 1:
          test1(argc, argv);
          break;
        case 2:
          test2(argc, argv);
          break;
        case 3:
          test3(argc, argv);
          break;
        case 4:
          test4(argc, argv);
          break;
        case 5:
          test_loc1(argc, argv);
          break;
/*
        case 6:
          test_loc2(argc, argv);
          break;
*/
        case 7:
          test_graphic1(argc, argv);
          break;
        case 8:
          test_graphic2(argc, argv);
          break;
        case 9:
          test_ss1(argc, argv);
          break;
        case 10:
          test_ss2(argc, argv);
          break;
        case 11:
          test_dlp(argc, argv);
          break;
        case 12:
          test_book1(argc, argv);
          break;
        case 13:
          test_book2(argc, argv);
          break;
        case 14:
          test_book3(argc, argv);
          break;
        case 15:
          ok = auto_test_c_api(argc, argv);
          break;
        default:
          usage(argc, argv);
      }

   DtMmdbQuit();

   exit(ok);
}

