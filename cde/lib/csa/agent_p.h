/* $XConsortium: agent_p.h /main/1 1996/04/21 19:21:26 drk $ */
/*
 *  (c) Copyright 1993, 1994 Hewlett-Packard Company
 *  (c) Copyright 1993, 1994 International Business Machines Corp.
 *  (c) Copyright 1993, 1994 Novell, Inc.
 *  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 */

#ifndef _AGENT_P_H
#define _AGENT_P_H

#include "ansi_c.h"

extern void _DtCm_init_agent P(());
extern void _DtCm_destroy_agent P(());
extern void _DtCm_process_updates P(());

#endif /* _AGENT_P_H */
