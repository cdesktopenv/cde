/* $XConsortium: mamstate.c /main/3 1995/11/01 16:25:23 rswiston $ */
/***************************************************************
*                                                              *
*                      AT&T - PROPRIETARY                      *
*                                                              *
*        THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF        *
*                    AT&T BELL LABORATORIES                    *
*         AND IS NOT TO BE DISCLOSED OR USED EXCEPT IN         *
*            ACCORDANCE WITH APPLICABLE AGREEMENTS             *
*                                                              *
*                Copyright (c) 1995 AT&T Corp.                 *
*              Unpublished & Not for Publication               *
*                     All Rights Reserved                      *
*                                                              *
*       The copyright notice above does not evidence any       *
*      actual or intended publication of such source code      *
*                                                              *
*               This software was created by the               *
*           Advanced Software Technology Department            *
*                    AT&T Bell Laboratories                    *
*                                                              *
*               For further information contact                *
*                    {research,attmail}!dgk                    *
*                                                              *
***************************************************************/
/*
 * AT&T Bell Laboratories
 * make abstract machine file state support
 *
 * mamstate reference [ file ... | <files ]
 *
 * stdout is list of <file,delta> pairs where delta
 * is diff between reference and file times
 * non-existent files are not listed
 */

#if !lint
static char id[] = "\n@(#)mamstate (AT&T Bell Laboratories) 06/26/89\0\n";
#endif

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

main(argc, argv)
int		argc;
register char**	argv;
{
	register char*	s;
	register int	c;
	long		ref;
	struct stat	st;
	char		buf[1024];

	if (!(s = *++argv) || stat(s, &st))
	{
		fprintf(stderr, "Usage: mamstate reference [ file ... | <files ]\n");
		exit(1);
	}
	ref = (long)st.st_mtime;
	if (s = *++argv) do
	{
		if (!stat(s, &st))
			printf("%s %ld\n", s, (long)st.st_mtime - ref);
	} while (s = *++argv);
	else do
	{
		s = buf;
		while ((c = getchar()) != EOF && c != ' ' && c != '\n')
			if (s < buf + sizeof(buf) - 1) *s++ = c;
		if (s > buf)
		{
			*s = 0;
			if (!stat(buf, &st))
				printf("%s %ld\n", buf, (long)st.st_mtime - ref);
		}
	} while (c != EOF);
	exit(0);
}
