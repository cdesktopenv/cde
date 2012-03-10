/* $XConsortium: free.h /main/1 1996/04/21 19:23:19 drk $ */
/*
 *  (c) Copyright 1993, 1994 Hewlett-Packard Company
 *  (c) Copyright 1993, 1994 International Business Machines Corp.
 *  (c) Copyright 1993, 1994 Novell, Inc.
 *  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 */

#ifndef _FREE_H
#define _FREE_H

#include <EUSCompat.h>
#include "ansi_c.h"

extern CSA_calendar_user *_DtCm_alloc_calendar_users(uint num_elem);

extern char **_DtCm_alloc_character_pointers(uint num_elem);

extern CSA_attribute *_DtCm_alloc_attributes(uint num_elem);

extern CSA_entry_handle *_DtCm_alloc_entry_handles(uint num_elem);

extern CSA_reminder_reference *_DtCm_alloc_reminder_references(uint num_elem);

extern void *_DtCm_alloc_entry();

extern CSA_return_code _DtCm_free(void *ptr);

extern void _DtCm_free_character_pointers P((uint num_elem, void *vptr));

#endif

