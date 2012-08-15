/*
 *+SNOTICE
 *
 *
 *	$XConsortium: ImplConfigTable.hh /main/5 1996/07/22 13:49:15 mgreess $
 *
 *	RESTRICTED CONFIDENTIAL INFORMATION:
 *	
 *	The information in this document is subject to special
 *	restrictions in a confidential disclosure agreement bertween
 *	HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
 *	document outside HP, IBM, Sun, USL, SCO, or Univel wihtout
 *	Sun's specific written approval.  This documment and all copies
 *	and derivative works thereof must be returned or destroyed at
 *	Sun's request.
 *
 *	Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 *
 *+ENOTICE
 */

#ifndef _IMPLCONFIGTABLE_HH
#define _IMPLCONFIGTABLE_HH

#include <DtMail/DtMailValues.hh>

struct ImplConfigTable {
    const char	*impl_name;
    const char	*lib_name;
    const char	*meta_entry_point;
};

static const ImplConfigTable initial_impls[] = {
    { "Internet MIME",	NULL,	"RFCMetaFactory" },
#if defined(sun)
    { "Sun Mail Tool",	NULL,	"V3MetaFactory" },
#endif
    { NULL,		NULL,	NULL }
};

#endif
