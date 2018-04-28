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
 * $XConsortium: istr_test.c /main/5 1996/10/02 17:03:17 drk $
 * 
 * @(#)istr_test.c	1.8 11 Feb 1994	cde_app_builder/src/libAButil
 * 
 * RESTRICTED CONFIDENTIAL INFORMATION:
 * 
 * The information in this document is subject to special restrictions in a
 * confidential disclosure agreement between HP, IBM, Sun, USL, SCO and
 * Univel.  Do not distribute this document outside HP, IBM, Sun, USL, SCO,
 * or Univel without Sun's specific written approval.  This document and all
 * copies and derivative works thereof must be returned or destroyed at Sun's
 * request.
 * 
 * Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 * 
 */


/*
 * istr_test.c string allocator test case
 * 
 */
#include <stdio.h>
#include <string.h>
#include <ab_private/istr.h>



int
print_array(void)
{
    return 0;
}


int
print_str(ISTRING istr)
{
    printf("'%s' ", util_strsafe(istr_string(istr)));
    return 0;
}


main()
{
    ISTRING             a,
                        b,
                        c,
                        d,
                        e,
                        f,
                        g,
                        h,
                        l,
                        m,
                        n,
                        o,
                        p;
    int                 i,
                        j,
                        k;
    char               *x,
                       *y,
                       *z;
    BOOL                q;

    printf("\n\n\n\n\n");
    x = NULL;
    f = istr_create_alloced(x);
    printf("%p value of NULL\n", f);
    x = strdup("one");
    f = istr_create_alloced(x);
    printf("%p value of one\n", f);
    printf("%s %p *******string after one\n", x, x);
    y = strdup("one");
    printf("%s %p *******string before one --y\n", y, y);
    g = istr_create_alloced(y);
    printf("%p value of one\n", g);
    printf("%p *******string after one --y\n", y);
    z = strdup("two");
    h = istr_create_alloced(z);
    printf("%s %p *******string after two --x\n", x, x);
    printf("%p *******string after two --y\n", y);
    printf("%s %p *******string after two --z\n", z, z);
    printf("%p value of one\n", h);
    print_array();
    x = strdup("create");
    p = istr_create_alloced(x);
    printf("%p value of create\n", p);
    x = strdup("ffffff");
    o = istr_create_alloced(x);
    printf("%p value of ffffff\n", o);
    i = istr_destroy(p);
    printf("%d returned %p value\n", i, p);
    print_array();
    x = strdup("hhhhhh");
    o = istr_create_alloced(x);
    printf("%p value of hhhh\n", o);
    print_array();

    printf("\n\n\n\n\n");
    a = istr_create("first");
    printf("%p value of first\n", a);
    b = istr_create("second");
    printf("%p value of second\n", b);
    c = istr_create("third");
    printf("%p value of third\n", c);
    d = istr_create("fourth");
    printf("%p value of fourth\n", d);
    e = istr_create("first");
    printf("%p value of first\n", e);
    x = strdup("hiiii");
    f = istr_create_alloced(x);
    printf("%p value of hiiii\n", f);
    x = strdup("hiiii");
    g = istr_create_const(x);
    printf("%p value of hiiii\n", g);
    h = istr_create("third");
    printf("%p value of third\n", h);
    l = istr_create("fourth");
    printf("%p value of fourth\n", l);
    m = istr_create_const("fourth");
    printf("%p value of fourth\n", m);
    n = istr_create_const("fifth");
    printf("%p value of fifth\n", n);
    o = istr_create("fifth");
    printf("%p value of fifth\n", o);
    print_array();
    printf("###################\n");
    i = istr_destroy(a);
    printf("%d returned %p value\n", i, a);
    i = istr_destroy(b);
    printf("%d returned %p value\n", i, b);
    i = istr_destroy(c);
    printf("%d returned %p value\n", i, c);
    i = istr_destroy(d);
    printf("%d returned %p value\n", i, d);
    i = istr_destroy(e);
    printf("%d returned %p value\n", i, e);
    i = istr_destroy(h);
    printf("%d returned %p value\n", i, h);
    i = istr_destroy(l);
    printf("%d returned %p value\n", i, l);
    i = istr_destroy(m);
    printf("%d returned %p value\n", i, m);
    i = istr_destroy(p);
    i = istr_destroy(f);
    printf("%d returned %p value\n", i, f);
    i = istr_destroy(g);
    printf("%d returned %p value\n", i, g);
    print_array();

    printf("$$$$$$$$$$$$$$$$$$$\n");
    /* e = istr_dup(5); */
    a = istr_create("fifth");
    printf("%p value of fifth\n", a);
    b = istr_create("sixth");
    printf("%p value of sixth\n", b);
    c = istr_create("sixth");
    printf("%p value of sixth\n", c);
    d = istr_create("seventh");
    printf("%p value of seventh\n", d);
    e = istr_create("eigth");
    printf("%p value of eighth\n", e);
    f = istr_create("nineth");
    printf("%p value of nieth\n", f);
    g = istr_create("tenth");
    printf("%p value of tenth\n", g);
    h = istr_create("eleventh");
    printf("%p value of eleventh\n", h);
    l = istr_create("twelvth");
    printf("%p value of twelvth\n", l);
    m = istr_create("thirteenth");
    printf("%p value of thirteenth\n", m);
    i = istr_destroy(b);
    printf("%d returned %p value\n", i, b);
    i = istr_destroy(b);
    printf("%d returned %p value\n", i, b);
    b = istr_create("fourteenth");
    printf("%p value of fourteenth\n", b);
    p = istr_create("fifthteenth");
    printf("%p value of fifthteenth\n", p);
    print_array();
    printf("$$$$$$$$$$$$$$$$$$$\n");


    e = istr_dup(a);
    printf("%p value dupped ", e);
    print_str(e);
    printf(" string\n");
    e = istr_dup(d);
    printf("%p value dupped ", e);
    print_str(e);
    printf(" string\n");
    /* e = istr_dup(8); */
    printf("%p value dupped ", e);
    print_str(e);
    printf(" string\n");
    /* e = istr_dup(9); */
    printf("%p value dupped ", e);
    print_str(e);
    printf(" string\n");
    /* e = istr_dup(6); */
    printf("%p value dupped ", e);
    print_str(e);
    printf(" string\n");
    print_array();
    /* x = istr_string(9); */
    printf("%d value %s string\n", 9, x);
    x = istr_string(a);
    printf("%p value %s string\n", a, x);
    x = istr_string(b);
    printf("%p value %s string\n", b, x);
    x = istr_string(c);
    printf("%p value %s string\n", c, x);
    x = istr_string(d);
    printf("%p value %s string\n", d, x);
    /* x = istr_string(6); */
    printf("%d value %s string\n", 6, x);
    /* x = istr_string(8); */
    printf("8 value %s string\n", x);
    /* x = istr_string(0); */
    print_array();



    printf("\n\n\n\n\n");
    a = istr_dup_existing("tenth");
    printf("%p value tenth\n", a);
    a = istr_dup_existing("not");
    printf("%p value not\n", a);
    a = istr_dup_existing(NULL);
    printf("%p value NULL\n", a);
    print_array();
    a = (ISTRING)5;
    i = istr_move(c, a);
    printf("%p from %p to %d\n", a, c, i);

    i = istr_len(0);
    printf("%d length %d value\n", i, 4);
    i = istr_len((ISTRING)4);
    printf("%d length %d value\n", i, 4);

    printf("istr_cmp\n");
    i = istr_cmp((ISTRING)2, (ISTRING)2);
    printf("%d value ", i);
    print_str((ISTRING)2);
    print_str((ISTRING)2);
    printf("\n");
    i = istr_cmp((ISTRING)1, (ISTRING)2);
    printf("%d value ", i);
    print_str((ISTRING)1);
    print_str((ISTRING)2);
    printf("\n");
    i = istr_cmp((ISTRING)2, (ISTRING)1);
    printf("%d value ", i);
    print_str((ISTRING)2);
    print_str((ISTRING)1);
    printf("\n");
    i = istr_cmp((ISTRING)2, NULL);
    printf("%d value ", i);
    print_str((ISTRING)2);
    printf("%d\n", NULL);

    printf("istr_equal\n");
    q = istr_equal((ISTRING)2, (ISTRING)2);
    printf("%d value %d %d\n", q, 2, 2);
    q = istr_equal((ISTRING)1, (ISTRING)2);
    printf("%d value %d %d\n", q, 1, 2);
    q = istr_equal((ISTRING)2, (ISTRING)1);
    printf("%d value %d %d\n", q, 2, 1);
    q = istr_equal(2, NULL);
    printf("%d value %d %d\n", q, 2, NULL);

    printf("istr_cmpstr\n");
    i = istr_cmpstr((ISTRING)2, "two");
    printf("%d value %d %d\n", i, 2, 2);
    i = istr_cmpstr((ISTRING)1, "two");
    printf("%d value %d %d\n", i, 1, 2);
    i = istr_cmpstr((ISTRING)2, "one");
    printf("%d value %d %d\n", i, 2, 1);
    i = istr_cmpstr((ISTRING)2, NULL);
    printf("%d value %d %d\n", i, 2, NULL);

    printf("istr_equalstr\n");
    q = istr_equalstr((ISTRING)2, "two");
    printf("%d value %d %d\n", q, 2, 2);
    q = istr_equalstr((ISTRING)1, "two");
    printf("%d value %d %d\n", q, 1, 2);
    q = istr_equalstr((ISTRING)2, "one");
    printf("%d value %d %d\n", q, 2, 1);
    q = istr_equalstr((ISTRING)2, NULL);
    printf("%d value %d %d\n", q, 2, NULL);

    print_array();

}
