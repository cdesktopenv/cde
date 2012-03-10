/*
 *  $XConsortium: AppPrintData.cc /main/1 1996/04/22 17:41:23 rcs $
 *
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1996 Hewlett-Packard Company.
 * (c) Copyright 1996 International Business Machines Corp.
 * (c) Copyright 1996 Sun Microsystems, Inc.
 * (c) Copyright 1996 Novell, Inc. 
 * (c) Copyright 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 *
 */

#define C_WindowGeometryPref
#define L_Preferences

#define C_WindowSystem
#define L_Other

#define C_ServiceMgr
#define C_MessageMgr
#define L_Managers

#define C_TOC_Element
#define L_Basic

#define USES_OLIAS_FONT

#include "Prelude.h"

#include "Exceptions.hh"

#include <unistd.h>

//--------- Constructors ----------------------------------

AppPrintData::AppPrintData()
{
    f_print_data = (DtPrintSetupData*)XtCalloc(1, sizeof(DtPrintSetupData));
    f_pform = NULL ;
    f_print_dialog = NULL;
    f_print_shell = NULL;
    f_print_only = FALSE;
    f_outline_element = NULL;
};

//--------- Destructors ----------------------------------


AppPrintData::~AppPrintData()
{
    XtFree((char *)f_print_data);
};
