/*
 * File:	stringio.C $XConsortium: stringio.C /main/5 1996/06/21 17:36:19 ageorge $
 *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company
 * (c) Copyright 1993, 1994 International Business Machines Corp.
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 * (c) Copyright 1993, 1994 Novell, Inc.
 */
#include <codelibs/nl_hack.h>
#include "stringio.h"

int
_StringIO::doit(register int commit)
{
    register wchar_t ch;
    register int cu = curr;
    register char *ccp;
    wchar_t __nlh_char[1];

    for (; cu >= 0; cu--)
    {
	ccp = ptr[cu];
	ch = ccp ? CHARAT(ccp) : '\0';
	if (ch != '\0')
	    break;
    }
    if (commit)
    {
	old_curr = curr;
	if ((curr = cu) >= 0)
	{
	    old_ccp = (const char *)ccp;
	    ADVANCE(ccp);
	    ptr[curr] = ccp;
	}
    }
    return ch;
}
