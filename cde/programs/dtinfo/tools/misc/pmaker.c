/* $XConsortium: pmaker.c /main/3 1996/06/11 17:40:31 cde-hal $ */
#include <stdio.h>

main(argc, argv)
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
