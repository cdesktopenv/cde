/* $XConsortium: laccess.h /main/1 1996/04/21 19:23:36 drk $ */
/*
 *  (c) Copyright 1993, 1994 Hewlett-Packard Company
 *  (c) Copyright 1993, 1994 International Business Machines Corp.
 *  (c) Copyright 1993, 1994 Novell, Inc.
 *  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 */

#ifndef _LACCESS_H
#define _LACCESS_H

#include "ansi_c.h"

extern boolean_t _DtCmIsSamePath P((char *str1, char *str2));

extern boolean_t _DtCmIsSameUser P((char *user1, char *user2));

#endif
