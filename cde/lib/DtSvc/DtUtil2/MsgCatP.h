/* 
 * (c) Copyright 1995 Digital Equipment Corporation.
 * (c) Copyright 1995 Hewlett-Packard Company.
 * (c) Copyright 1995 International Business Machines Corp.
 * (c) Copyright 1995 Sun Microsystems, Inc.
 * (c) Copyright 1995 Novell, Inc. 
 * (c) Copyright 1995 FUJITSU LIMITED.
 * (c) Copyright 1995 Hitachi.
 *
 * MsgCat.h - Public interfaces for the Cached Message Catalog Service
 *
 * $TOG: MsgCatP.h /main/1 1998/04/22 14:19:24 mgreess $
 *
 */

#ifndef _Dt_MsgCat_h
#define _Dt_MsgCat_h

#include <nl_types.h>

#ifdef __cplusplus
extern "C" {
#endif

extern char *_DtCatgetsCached(nl_catd catd, int set, int num, char *dflt);

#ifdef __cplusplus
}
#endif

#endif /* _Dt_MsgCat_h */
