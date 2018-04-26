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

/*
 *	$XConsortium: util_test.c /main/4 1995/11/06 18:54:31 rswiston $
 *
 *	@(#)util_test.c	1.7 14 Feb 1994	
 *
 * 	RESTRICTED CONFIDENTIAL INFORMATION:
 *	
 *	The information in this document is subject to special
 *	restrictions in a confidential disclosure agreement between
 *	HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
 *	document outside HP, IBM, Sun, USL, SCO, or Univel without
 *	Sun's specific written approval.  This document and all copies
 *	and derivative works thereof must be returned or destroyed at
 *	Sun's request.
 *
 *	Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 *
 */

#include "util.h"
#include <sys/types.h>
#include <sys/stat.h>		/* mkdir */
#include <sys/param.h>

static int test_verbosity(void);
static int test_output(void);
static int test_get_dir_name(void);
static int test_cvt_path_to_relative(void);
static int get_it(char *dirname);
static int cvt_it(STRING path, STRING dir);

main (int argc, char *argv[])
{
    util_init(argv[0]);
    test_verbosity();
    test_output();
    /* test_get_dir_name(); */
    /* test_cvt_path_to_relative(); */

    return 0;
}

static int
test_verbosity(void)
{
#define set_lvl(l) ((lvl = (l)), (dlvl = util_max(0, lvl - 2)))
    int	lvl = 0;
    int	dlvl = 0;
    int	lvl_from_get_verbosity = 0;
    int dlvl_from_util_get_debug_level = 0;
    int dlvl_from_debug_level = 0;

    util_printf("***** TESTING VERBOSITY *****\n");
    set_lvl(3);
    printf("setting verbosity to %d\n", lvl);
    util_set_verbosity(lvl);
    lvl_from_get_verbosity = util_get_verbosity();
    printf("util_get_verbosity -> %d\n", lvl_from_get_verbosity);
    if (lvl != lvl_from_get_verbosity)
    {
	printf("ERROR - bad verbosity %s:%d\n", __FILE__, __LINE__);
    }
    dlvl_from_util_get_debug_level = util_get_debug_level();
    printf("util_get_debug_level() -> %d\n", dlvl_from_util_get_debug_level);
    if (dlvl != dlvl_from_util_get_debug_level)
    {
	printf("ERROR - bad debug level %s:%d\n", __FILE__, __LINE__);
    }
    dlvl_from_debug_level = debug_level();
    printf("debug_level() -> %d\n", dlvl_from_debug_level);
    if (dlvl != dlvl_from_debug_level)
    {
	printf("ERROR - bad debug level %s%d\n", __FILE__, __LINE__);
    }

    printf("debugging: %d\n", debugging());
    if (!debugging())
    {
	printf("ERROR - bad debugging %s:%d\n", __FILE__, __LINE__);
    }

    return 0;
}

static int
test_output(void)
{
    util_printf("***** TESTING OUTPUT\n");

    util_puts("string to util_puts\n");
    util_puts_err("string to util_puts_err\n");
    util_dputs(0, "string to util_dputs\n");
    util_printf("string to util_printf at %s:%d\n", __FILE__, __LINE__);
    util_printf_err("string to util_printf_err at %s:%d\n", __FILE__, __LINE__);
    util_dprintf(0, "string to util_dprintf at %s:%d\n", __FILE__, __LINE__);

    return 0;
}

static int
test_cvt_path_to_relative(void)
{
    mkdir("TEST", 0777);
    cvt_it("/local/home/dunn/tmp", "/local/home/dunn");
    cvt_it("/local/home/dunn/tmp", "/net/fuggit/local/home/dunn");
    cvt_it( "/net/fuggit/local/home/dunn", "/local/home/dunn/tmp");
    cvt_it("///usr//dt", "/usr///dt/include");
    cvt_it("/usr///dt/include", "///usr//dt");
    cvt_it("TEST", NULL);
    cvt_it("../libAButil/TEST", NULL);
    cvt_it("/home/marth", "/home/marth");
    rmdir("TEST");
    return 0;
}

static int
cvt_it(STRING path, STRING from)
{
    STRING	s1= NULL;
    STRING	s2= NULL;
    if (path != NULL)
    {
        s1= strdup(path);
    }
    if (from != NULL)
    {
        s2= strdup(from);
    }

    char	buf[MAXPATHLEN]= "";
    int		ret;

    ret= util_cvt_path_to_relative(path, from, buf, MAXPATHLEN);
    if (ret < 0)
    {
	printf("cvt(%s,%s) -> %d\n", 
		util_strsafe(s1), util_strsafe(s2), ret);
    }
    else
    {
	printf("cvt(%s,%s) -> '%s'\n",
		util_strsafe(s1), util_strsafe(s2), util_strsafe(buf));
    }

    util_free(s1);
    util_free(s2);
    return 0;
}


static int
test_get_dir_name(void)
{
    get_it("");
    get_it("abc/xyz");
    get_it("../");
    get_it("x");
    get_it("/");
    get_it("/abc/def/ghi");
    get_it("/abc/");
    get_it("/abc///");
    get_it("///abc");
    get_it("//");
    return 0;
}

int 
get_it(char *path)
{
    char	buf[256];

    printf("'%s' => '%s'\n", 
	path,
	util_get_dir_name_from_path(path, buf, 256));
    return 0;
}

