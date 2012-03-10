/* $XConsortium: regerror.c /main/3 1996/06/19 17:13:36 drk $ */
#include <stdio.h>

void
tpt_regerror(s)
char *s;
{
#ifdef ERRAVAIL
	error("tpt_regexp: %s", s);
#else
	fprintf(stderr, "tpt_regexp(3): %s", s);
	exit(1);
#endif
	/* NOTREACHED */
}
