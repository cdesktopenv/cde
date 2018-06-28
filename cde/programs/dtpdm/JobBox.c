/*
 * CDE - Common Desktop Environment
 *
 * Copyright (c) 1993-2012, The Open Group. All rights reserved.
 *
 * These libraries and programs are free software; you can
 * redistribute them and/or modify them under the terms of the GNU
 * Lesser General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * These libraries and programs are distributed in the hope that
 * they will be useful, but WITHOUT ANY WARRANTY; without even the
 * implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with these libraries and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
/* $XConsortium: JobBox.c /main/3 1996/08/12 18:41:59 cde-hp $ */
/*
 * dtpdm/JobBox.c
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
#include <Dt/PrintDlgMgrP.h>

#include "JobBox.h"

/*
 * JobBox-specific data - PdmSetupBox.subclass_data
 */
typedef struct
{
    const char* job_options;
    const char* job_name;
    PdmOidNotify notification_profile;

    Boolean job_options_supported;
    Boolean job_name_supported;
    Boolean notification_profile_supported;
    
    Widget job_options_text;
    Widget job_name_text;
    Widget notify_toggle;

} PdmJobBoxData;

/*
 * JobBox-specific fallback resources
 */
static String PdmJobBoxFallbackResources[] =
{
    "*JobSetup.locationId: JobSetup",
    "*JobSetup.marginHeight: 5",
    "*JobSetup.marginWidth: 5",
    "*JobSetup.verticalSpacing: 5",
    "*JobTab.compoundString: Job",
    "*JobTab.labelString: Job"
};

/*
 * static function declarations
 */
static void PdmJobBoxDelete(PdmSetupBox* me);
static void PdmJobBoxCreate(PdmSetupBox* me, Widget parent);
static PdmStatus PdmJobBoxVerifyAttr(PdmSetupBox* me, PdmXp* pdm_xp);
static void PdmJobBoxGetAttr(PdmSetupBox* me, PdmXp* pdm_xp);
static void PdmJobBoxSetAttr(PdmSetupBox* me, PdmXp* pdm_xp);

/*
 * ------------------------------------------------------------------------
 * Name: PdmJobBoxNew
 *
 * Description:
 *
 *     Creates a new PdmJobBox (PdmSetupBox) instance structure.
 *
 * Return value:
 *
 *     The new PdmJobBox instance structure.
 *
 */
PdmSetupBox*
PdmJobBoxNew(void)
{
    PdmSetupBox* me = (PdmSetupBox*)XtCalloc(1, sizeof(PdmSetupBox));

    me->delete_proc = PdmJobBoxDelete;
    me->create_proc = PdmJobBoxCreate;
    me->verify_attr_proc = PdmJobBoxVerifyAttr;
    me->get_attr_proc = PdmJobBoxGetAttr;
    me->set_attr_proc = PdmJobBoxSetAttr;
    me->fallback_resources = PdmJobBoxFallbackResources;
    me->fallback_resources_count = XtNumber(PdmJobBoxFallbackResources);
    me->tab_name = "JobTab";

    me->subclass_data = (XtPointer)XtCalloc(1, sizeof(PdmJobBoxData));

    return me;
}

/*
 * ------------------------------------------------------------------------
 * Name: PdmJobBoxDelete
 *
 * Description:
 *
 *     Frees the passed PdmJobBox (PdmSetupBox) instance structure.
 *
 * Return value:
 *
 *     None.
 *
 */
static void
PdmJobBoxDelete(PdmSetupBox* me)
{
    PdmJobBoxData* data = (PdmJobBoxData*)me->subclass_data;

    XtFree((char*)data);
    XtFree((char*)me);
}

/*
 * ------------------------------------------------------------------------
 * Name: PdmJobBoxCreate
 *
 * Description:
 *
 *     Creates a managed PDM Job options setup box widget.
 *
 * Return value:
 *
 *     The passed PdmJobBox (PdmSetupBox) instance structure.
 *
 */
static void
PdmJobBoxCreate(PdmSetupBox* me, Widget parent)
{
    PdmJobBoxData* data = (PdmJobBoxData*)me->subclass_data;
    me->widget = _DtCreatePDMJobSetup(parent);
    XtManageChild(me->widget);
    /*
     * get the control widget ids
     */
    data->notify_toggle = XtNameToWidget(me->widget, "*SendMail.button_0");
    data->job_name_text = XtNameToWidget(me->widget, "*Banner");
    data->job_options_text = XtNameToWidget(me->widget, "*Options");
    /*
     * initialize the banner field
     */
    if(data->job_name_supported)
    {
	XtVaSetValues(data->job_name_text, XmNvalue, data->job_name, NULL);
    }
    else
    {
	XtSetSensitive(data->job_name_text, False);
	XtSetSensitive(XtNameToWidget(me->widget, "*BannerLabel"), False);
    }
    /*
     * initialize the options field
     */
    if(data->job_options_supported)
    {
	XtVaSetValues(data->job_options_text,
		      XmNvalue, data->job_options,
		      NULL);
    }
    else
    {
	XtSetSensitive(data->job_options_text, False);
	XtSetSensitive(XtNameToWidget(me->widget, "*OptionsLabel"), False);
    }
    /*
     * initialize the send mail toggle
     */
    if(data->notification_profile_supported)
    {
	Boolean set;
	set = PDMOID_NOTIFY_EMAIL == data->notification_profile ? True : False;
	XmToggleButtonGadgetSetState(data->notify_toggle, set, False);
    }
    else
    {
	XtSetSensitive(data->notify_toggle, False);
    }
    
}

/*
 * ------------------------------------------------------------------------
 * Name: PdmJobBoxVerifyAttr
 *
 * Description:
 *
 *
 *
 * Return value:
 *
 *     PDM_SUCCESS if all of the Printer options selected are OK.
 *
 *     PDM_FAILURE if any of the Printer options are invalid.
 *
 */
static PdmStatus
PdmJobBoxVerifyAttr(PdmSetupBox* me, PdmXp* pdm_xp)
{
    return PDM_SUCCESS;
}

/*
 * ------------------------------------------------------------------------
 * Name: PdmJobBoxGetAttr
 *
 * Description:
 *
 * Return value:
 *
 *     None.
 *
 */
static void
PdmJobBoxGetAttr(PdmSetupBox* me, PdmXp* pdm_xp)
{
    PdmJobBoxData* data = (PdmJobBoxData*)me->subclass_data;
    PdmOidList* job_attrs_supported = NULL;
    const char* strval = NULL;
    
    /*
     * job attributes supported
     */
#if 0 && defined(PRINTING_SUPPORTED)
    strval = PdmXpGetStringValue(pdm_xp, XPPrinterAttr,
				 pdmoid_att_job_attributes_supported);
#endif /* PRINTING_SUPPORTED */
    job_attrs_supported = PdmOidListNew(strval);
    /*
     * job options
     */
    if(PdmOidListHasOid(job_attrs_supported,
			pdmoid_att_xp_spooler_command_options))
    {
	data->job_options_supported = True;
#if 0 && defined(PRINTING_SUPPORTED)
	data->job_options =
	    PdmXpGetStringValue(pdm_xp, XPJobAttr,
				pdmoid_att_xp_spooler_command_options);
#endif /* PRINTING_SUPPORTED */
    }
    /*
     * job name (banner)
     */
    if(PdmOidListHasOid(job_attrs_supported, pdmoid_att_job_name))
    {
	data->job_name_supported = True;
#if 0 && defined(PRINTING_SUPPORTED)
	data->job_name =
	    PdmXpGetStringValue(pdm_xp, XPJobAttr, pdmoid_att_job_name);
#endif /* PRINTING_SUPPORTED */
    }
    /*
     * notification profile (send mail)
     */
    if(PdmOidListHasOid(job_attrs_supported, pdmoid_att_notification_profile))
    {
#if 0 && defined(PRINTING_SUPPORTED)
	strval = PdmXpGetStringValue(pdm_xp, XPJobAttr,
				     pdmoid_att_notification_profile);
#endif /* PRINTING_SUPPORTED */
	data->notification_profile = PdmOidNotifyParse(strval);
	if(data->notification_profile != PDMOID_NOTIFY_UNSUPPORTED)
	    data->notification_profile_supported = True;
    }
    /*
     * clean up
     */
    PdmOidListDelete(job_attrs_supported);
}

/*
 * ------------------------------------------------------------------------
 * Name: PdmJobBoxSetAttr
 *
 * Description:
 *
 * Return value:
 *
 *     None.
 *
 */
static void
PdmJobBoxSetAttr(PdmSetupBox* me, PdmXp* pdm_xp)
{
    PdmJobBoxData* data = (PdmJobBoxData*)me->subclass_data;

    if(data->job_options_supported)
    {
	String value;
	XtVaGetValues(data->job_options_text, XmNvalue, &value, NULL);
#if 0 && defined(PRINTING_SUPPORTED)
	PdmXpSetStringValue(pdm_xp, XPJobAttr,
			    pdmoid_att_xp_spooler_command_options, value);
#endif /* PRINTING_SUPPORTED */
	XtFree(value);
    }
    if(data->job_name_supported)
    {
	String value;
	XtVaGetValues(data->job_name_text, XmNvalue, &value, NULL);
#if 0 && defined(PRINTING_SUPPORTED)
	PdmXpSetStringValue(pdm_xp, XPJobAttr, pdmoid_att_job_name, value);
#endif /* PRINTING_SUPPORTED */
	XtFree(value);
    }
    if(data->notification_profile_supported)
    {
	Boolean set;
	
	set = XmToggleButtonGadgetGetState(data->notify_toggle);
#if 0 && defined(PRINTING_SUPPORTED)
	PdmXpSetStringValue(pdm_xp, XPJobAttr,
			    pdmoid_att_notification_profile,
			    PdmOidNotifyString(set
					       ? PDMOID_NOTIFY_EMAIL
					       : PDMOID_NOTIFY_NONE));
#endif /* PRINTING_SUPPORTED */
    }
}



