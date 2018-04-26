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
 * License along with these libraries and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
/* $XConsortium: prolatex.c /main/3 1996/06/11 17:38:30 cde-hal $ */

#include <stdio.h> 
#include <string.h> 

#define BUFSIZ 1000

int replace();

main(argc, argv)
int argc;
char* argv[];
{
   char buf[BUFSIZ];

   while ( fgets(buf, BUFSIZ, stdin) != NULL ) {
       buf[strlen(buf)-1] = '\0';
       if ( replace(buf, "\\epsffile{", argv[1]) == 0 )
          continue;
       if ( replace(buf, "\\input{", argv[1]) == 0 )
          continue;
       else {
         fputs(buf, stdout);
         fputs("\n", stdout);
      }
   }
}

int replace(buf, pattern, s)
char* buf;
char* pattern;
char* s;
{
   int loc = strlen(pattern);
   char c = buf[loc];
   buf[loc] = '\0';
   if ( strcmp(buf, pattern) == 0 ) {
     fputs(buf, stdout);
     /*fputs(getenv("PWD"), stdout);*/
     fputs(s, stdout);
     fputs("/", stdout);
     buf[loc] = c;
     fputs(buf + loc, stdout);
     fputs("\n", stdout);
      return 0;
   } else  {
     buf[loc] = c;
     return -1;
   }
}
