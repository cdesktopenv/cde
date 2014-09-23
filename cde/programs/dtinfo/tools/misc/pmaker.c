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
/* $XConsortium: pmaker.c /main/3 1996/06/11 17:40:31 cde-hal $ */
#include <stdio.h>
#include <stdlib.h>

int main(argc, argv)
int argc ;
char **argv ;
{
  FILE *f ;
  int i;

  f = fopen("Prelude.h", "w");
  if (!f)
    exit(-1);

  fprintf(f, "#include \"config.h\"\n");
  for (i = argc - 1 ; i > 0 ; i--)
    fprintf(f, "#ifdef L_%s\n#include <%s/%s.d>\n#endif\n",
	    argv[i], argv[i], argv[i]);

  for (i = 1 ; i < argc ; i++)
    fprintf(f, "#ifdef L_%s\n#include <%s/%s.h>\n#endif\n",
	    argv[i], argv[i], argv[i]);

  fclose(f);
  exit(0);
}
