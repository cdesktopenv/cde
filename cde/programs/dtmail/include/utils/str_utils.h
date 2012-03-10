/* $XConsortium: str_utils.h /main/1 1996/04/21 20:00:04 drk $
 *
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1996 Hewlett-Packard Company.
 * (c) Copyright 1996 International Business Machines Corp.
 * (c) Copyright 1993-1996 Sun Microsystems, Inc.
 * (c) Copyright 1996 Novell, Inc. 
 * (c) Copyright 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 */
#ifndef _STR_UTILS_H
#define _STR_UTILS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>

#ifdef NEED_STRCASECMP
int strcasecmp(const char *, const char *);
int strncasecmp(const char *, const char *, size_t);
#endif

#ifdef __cplusplus
}
#endif

#endif /* _STR_UTILS_H */
