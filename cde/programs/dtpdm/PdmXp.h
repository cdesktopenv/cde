/* $XConsortium: PdmXp.h /main/4 1996/08/12 18:43:09 cde-hp $ */
/*
 * dtpdm/PdmXp.h
 */
/*
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1996 Hewlett-Packard Company.
 * (c) Copyright 1996 International Business Machines Corp.
 * (c) Copyright 1996 Sun Microsystems, Inc.
 * (c) Copyright 1996 Novell, Inc. 
 * (c) Copyright 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 */
#ifndef _PdmXp_h
#define _PdmXp_h

#include <X11/extensions/Print.h>

#include "PdmOid.h"

#define PDMXP_POOL_COUNT 4

/*
 * PdmXp instance structure
 */
typedef struct _PdmXp
{
    /*
     * print server connection info
     */
    Display* display;
    XPContext context;
    /*
     * printing attribute pools
     */
    XrmDatabase pool[PDMXP_POOL_COUNT];
    /*
     * qualifier for retrieving attributes
     */
    char* qualifier;
    int qualifier_len;
    
} PdmXp;

/*
 * PdmXp public methods
 */
extern PdmXp* PdmXpNew();
extern Display* PdmXpOpen(PdmXp* me,
			  char* display_spec,
			  char* context_str);
extern void PdmXpClose(PdmXp* me);
extern void PdmXpDelete(PdmXp* me);
extern PdmOid PdmXpGetValue(PdmXp* me,
			    XPAttributes type,
			    PdmOid id_att);
extern const char* PdmXpGetStringValue(PdmXp* me,
				       XPAttributes type,
				       PdmOid id_att);
extern void PdmXpSetValue(PdmXp* me,
			  XPAttributes type,
			  PdmOid id_att,
			  PdmOid id_val);
extern void PdmXpSetStringValue(PdmXp* me,
				XPAttributes type,
				PdmOid id_att,
				const char* str_val);
extern void PdmXpUpdateAttributes(PdmXp* me);


#endif /* _PdmXp_h */
