/* $XConsortium: hash.h /main/1 1996/04/21 19:23:24 drk $ */
/*
 *  (c) Copyright 1993, 1994 Hewlett-Packard Company
 *  (c) Copyright 1993, 1994 International Business Machines Corp.
 *  (c) Copyright 1993, 1994 Novell, Inc.
 *  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 */

#ifndef _HASH_H
#define _HASH_H

#include "ansi_c.h"

extern void * _DtCmMakeHash P((int size));

extern void ** _DtCmGetHash P((void * tbl, const unsigned char * key));

extern void ** _DtCmFindHash P((void * tbl,const unsigned char * key));

extern void _DtCmDestroyHash P((void * tbl, int (*des_func)(), void * usr_arg));

#endif /* _HASH_H */

