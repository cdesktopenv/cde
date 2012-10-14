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
/* $XConsortium: dfiles.c /main/3 1996/06/11 17:40:25 cde-hal $ */
#include <stdio.h>
#include <stdlib.h>

main(argc, argv)
int argc ;
char **argv; 
{
  /* first parameter d or h */
  /* second parameter is library name, other params are classnames  */
  int i ;
  char *libname = argv[2] ;
  char buffer[256];

  FILE *hfile, *dfile;

  switch (argv[1][0])
    {
    case 'h':
      {
	sprintf(buffer, "%s.h", libname);

	hfile = fopen(buffer, "w");
	if (!hfile)
	  exit(-1);
  
	for (i = 3 ; i < argc ; i++ )
	  fprintf(hfile, "#ifdef C_%s\n#include <%s/%s.hh>\n#endif\n",
		  argv[i], libname, argv[i]);

	fclose(hfile);
      }
      break;
    case 'd':
      {
	sprintf(buffer, "%s.d", libname);

	dfile = fopen(buffer, "w");
	if (!dfile)
	  exit (-1);

	for (i = argc - 1 ; i > 2; i-- )
	  fprintf(dfile, "#ifdef C_%s\n#include <%s/%s.d>\n#endif\n",
		  argv[i], libname, argv[i]);

	fclose(dfile);
      }
      break;
    default:
      exit (-1);
      break;
    }
  exit(0);
}
