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
 * License along with these librararies and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
/* $XConsortium: PrinterBox.c /main/10 1996/11/20 12:59:48 cde-hp $ */
/*
 * dtpdm/PrinterBox.c
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
#include <Xm/XmAll.h>
#include <Dt/DtNlUtils.h>

#include "PrinterBox.h"

#include "PdmMsgs.h"

#ifndef CDE_INSTALLATION_TOP
#define CDE_INSTALLATION_TOP "/usr/dt"
#endif

#define ICON_INSTALL_DIR CDE_INSTALLATION_TOP "/appconfig/icons/C/"

#ifndef MAX
# define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif /* ! MAX */

/*
 * PrinterBox-specific data - PdmSetupBox.subclass_data
 */
typedef struct
{
    /*
     * child controls
     */
    struct _OrientCtl* orient_ctl;
    struct _PlexCtl* plex_ctl;
    struct _TrayCtl* tray_ctl;
    struct _SizeCtl* size_ctl;
    /*
     * attribute values
     */
    const char* printer_model;
    char* document_format;

    PdmOidList* document_attrs_supported;

    PdmOidList* orientations_supported;
    PdmOid orientation;

    PdmOidList* plexes_supported;
    PdmOid plex;

    PdmOid default_input_tray;
    PdmOid default_medium;

    PdmOidList* input_trays;
    PdmOidList* tray_sizes;
    PdmOidLinkedList* sizes_supported;

} PdmPrinterBoxData;

/*
 * orientations control
 */
typedef struct _OrientCtl
{
    Widget widget;
    Widget frame_label;
    Widget option_menu;
    Widget icon_label;

    int selected_item;

    const PdmOidList* orientations_supported;
    PdmOidList* dummy_orientations_supported;

    struct _PdmPixmapList* pixmap_list;
    struct _PdmPixmapList* i_pixmap_list;
    
    struct _PlexCtl* plex_ctl;
    
} OrientCtl;

/*
 * plexes control
 */
typedef struct _PlexCtl
{
    Widget widget;
    Widget frame_label;
    Widget option_menu;
    Widget icon_label;
    
    int selected_item;

    const PdmOidList* plexes_supported;
    PdmOidList* dummy_plexes_supported;

    struct _PdmPixmapList** pixmap_list_ptr;
    struct _PdmPixmapList* portrait_pixmap_list;
    struct _PdmPixmapList* landscape_pixmap_list;

    struct _PdmPixmapList** i_pixmap_list_ptr;
    struct _PdmPixmapList* i_portrait_pixmap_list;
    struct _PdmPixmapList* i_landscape_pixmap_list;

} PlexCtl;

/*
 * input trays control
 */
typedef struct _TrayCtl
{
    Widget widget;
    Widget frame_label;
    Widget option_menu;

    int selected_item;

    const PdmOidList* input_trays;
    const PdmOidList* tray_sizes;
    
    struct _SizeCtl* size_ctl;
    
} TrayCtl;

/*
 * page size control
 */
typedef struct _SizeCtlSortRec
{
    const char* str;
    PdmOid oid;
} SizeCtlSortRec, *SizeCtlSortList;

typedef struct _SizeCtl
{
    Widget widget;
    Widget frame_label;
    Widget list_box;
    Widget radio_box;
    Widget ready_radio;
    Widget supported_radio;

    PdmOidList* sizes_ready;
    PdmOidList* sizes_supported; 

    PdmOid selected_size;
    enum {
	SIZECTL_UNSUPPORTED,
	SIZECTL_SIZES_READY,
	SIZECTL_SIZES_SUPPORTED } selected_list;

    Boolean single_size_mode;

    XmStringTable sizes_ready_xmstr;
    XmStringTable sizes_supported_xmstr;
    
} SizeCtl;

/*
 * list of pixmaps
 */
typedef struct _PdmPixmapList
{
    Pixmap* pixmap;
    char** image_name;
    int count;
    Screen* screen;
    Pixel foreground, background;
    Pixmap stipple;
} PdmPixmapList;

/*
 * PrinterBox-specific fallback resources
 */
static String PdmPrinterBoxFallbackResources[] =
{
    "*PrinterSetup*LoadedAllowed.button_0.labelString: Loaded in Printer",
    "*PrinterSetup*LoadedAllowed.button_1.labelString: All Sizes",
    "*PrinterSetup*OrientationLabel.labelString: Page Orientation",
    "*PrinterSetup*PageSizeLabel.labelString: Page Size",
    "*PrinterSetup*PlexLabel.labelString: Printed Sides",
    "*PrinterSetup*DocumentFormatLabel.labelString: Format:",
    "*PrinterSetup*PrinterModelLabel.labelString: Printer Model:",
    "*PrinterSetup*TrayLabel.labelString: Tray",
    "*PrinterSetup.locationId: PrinterSetup",
    "*PrinterSetup*OrientCtlTop.PortraitIconFileName: "
	ICON_INSTALL_DIR "portrait.pm",
    "*PrinterSetup*OrientCtlTop.LandscapeIconFileName: "
	ICON_INSTALL_DIR "landscap.pm",
    "*PrinterSetup*OrientCtlTop.ReversePortraitIconFileName: "
	ICON_INSTALL_DIR "rportrai.pm",
    "*PrinterSetup*OrientCtlTop.ReverseLandscapeIconFileName: "
	ICON_INSTALL_DIR "rlandsca.pm",
    "*PrinterSetup*PlexCtlTop.SimplexPortraitIconFileName: "
	ICON_INSTALL_DIR "psimplex.pm",
    "*PrinterSetup*PlexCtlTop.SimplexLandscapeIconFileName: "
	ICON_INSTALL_DIR "lsimplex.pm",
    "*PrinterSetup*PlexCtlTop.DuplexPortraitIconFileName: "
	ICON_INSTALL_DIR "pduplex.pm",
    "*PrinterSetup*PlexCtlTop.DuplexLandscapeIconFileName: "
	ICON_INSTALL_DIR "lduplex.pm",
    "*PrinterSetup*PlexCtlTop.TumblePortraitIconFileName: "
	ICON_INSTALL_DIR "ptumble.pm",
    "*PrinterSetup*PlexCtlTop.TumbleLandscapeIconFileName: "
	ICON_INSTALL_DIR "ltumble.pm",
    "*PrinterTab.compoundString: Printer",
    "*PrinterTab.labelString: Printer"
};

/*
 * Printer Setup Box static function declarations
 */
static void PdmPrinterBoxDelete(PdmSetupBox* me);
static void PdmPrinterBoxCreate(PdmSetupBox* me, Widget parent);
static PdmStatus PdmPrinterBoxVerifyAttr(PdmSetupBox* me, PdmXp* pdm_xp);
static void PdmPrinterBoxGetAttr(PdmSetupBox* me, PdmXp* pdm_xp);
static void PdmPrinterBoxSetAttr(PdmSetupBox* me, PdmXp* pdm_xp);
static Widget PdmPrinterBoxCreateWindow(PdmSetupBox* me, Widget parent);
static void PdmPrinterBoxResizeCB(Widget w,
				  XtPointer client_data,
				  XtPointer call_data);
/*
 * Orientation Control methods
 */
static OrientCtl* OrientCtlNew(PlexCtl* plex_ctl);
static void OrientCtlDelete(OrientCtl* me);
static void OrientCtlCreate(OrientCtl* me,
			    Widget parent,
			    PdmOid orientation,
			    const PdmOidList* document_attrs_supported,
			    const PdmOidList* orientations_supported);
static void OrientCtlCreateWindow(OrientCtl* me, Widget parent);
#define OrientCtlGetWidget(me) ((me)->widget)
static void OrientCtlCreatePulldown(OrientCtl* me);
static void OrientCtlDisable(OrientCtl* me);
static void OrientCtlUpdateIcon(OrientCtl* me);
static PdmOid OrientCtlGetOrientation(OrientCtl* me);
static void OrientCtlResizeCB(Widget w,
			      XtPointer client_data, XtPointer call_data);
static void OrientCtlSetSelectedItem(OrientCtl* me, int selected_item);
static void OrientCtlSelectCB(Widget w, XtPointer client_d, XtPointer call_d);
static void OrientCtlInitPixmapList(OrientCtl* me);

/*
 * Plex Control methods
 */
static PlexCtl* PlexCtlNew();
static void PlexCtlDelete(PlexCtl* me);
static void PlexCtlCreate(PlexCtl* me,
			  Widget parent,
			  PdmOid plex,
			  const PdmOidList* document_attrs_supported,
			  const PdmOidList* plexes_supported);
static void PlexCtlCreateWindow(PlexCtl* me, Widget parent);
static void PlexCtlDisable(PlexCtl* me);
#define PlexCtlGetWidget(me) ((me)->widget)
static void PlexCtlCreatePulldown(PlexCtl* me);
static void PlexCtlUpdateIcon(PlexCtl* me);
static PdmOid PlexCtlGetPlex(PlexCtl* me);
static void PlexCtlResizeCB(Widget w,
			    XtPointer client_data, XtPointer call_data);
static void PlexCtlSetSelectedItem(PlexCtl* me, int selected_item);
static void PlexCtlSelectCB(Widget w, XtPointer client_d, XtPointer call_d);
static void PlexCtlSetOrientation(PlexCtl* me, PdmOid orientation);
static void PlexCtlInitPixmapLists(PlexCtl* me);

/*
 * Tray Control methods
 */
static TrayCtl* TrayCtlNew(SizeCtl* size_ctl);
static void TrayCtlDelete(TrayCtl* me);
static void TrayCtlCreate(TrayCtl* me,
			  Widget parent,
			  PdmOid default_input_tray,
			  PdmOid default_medium,
			  const PdmOidList* document_attrs_supported,
			  const PdmOidList* input_trays,
			  const PdmOidList* tray_sizes);
static void TrayCtlCreateWindow(TrayCtl* me, Widget parent);
#define TrayCtlGetWidget(me) ((me)->widget)
static PdmOid TrayCtlGetTray(TrayCtl* me);
static void TrayCtlDisable(TrayCtl* me);
static void TrayCtlResizeCB(Widget w,
			    XtPointer client_data, XtPointer call_data);
static void TrayCtlCreatePulldown(TrayCtl* me);
static void TrayCtlSetSelectedItem(TrayCtl* me, int selected_item);
static void TrayCtlSelectCB(Widget w, XtPointer client_d, XtPointer call_d);

/*
 * Size Control methods
 */
static SizeCtl* SizeCtlNew();
static void SizeCtlDelete(SizeCtl* me);
static int SizeCtlStrColl(const void* e1, const void* e2);
static void SizeCtlSetSizeLists(PdmOidList** oids,
				XmStringTable* xmstrs,
				PdmOidLinkedList* oids_ll);
static void SizeCtlCreate(SizeCtl* me,
			  Widget parent,
			  PdmOid default_medium,
			  const PdmOidList* document_attrs_supported,
			  const PdmOidList* tray_sizes,
			  PdmOidLinkedList* sizes_supported);
static void SizeCtlCreateWindow(SizeCtl* me, Widget parent);
#define SizeCtlGetWidget(me) ((me)->widget)
static void SizeCtlUpdate(SizeCtl* me);
static PdmOid SizeCtlGetSize(SizeCtl* me);
static void SizeCtlResizeCB(Widget w,
			    XtPointer client_data, XtPointer call_data);
static void SizeCtlSetSingleSizeMode(SizeCtl* me, PdmOid size);
static void SizeCtlSetMultiSizeMode(SizeCtl* me);
static void SizeCtlReadyChangedCB(Widget w,
				  XtPointer client_data, XtPointer call_data);
static void SizeCtlSupportedChangedCB(Widget w,
				      XtPointer client_data,
				      XtPointer call_data);
static void SizeCtlSelectCB(Widget w,
			    XtPointer client_data,
			    XtPointer call_data);
/*
 * pixmap list functions
 */
static PdmPixmapList* PdmPixmapListNew(int count, Widget w, Boolean stipple);
static void PdmPixmapListDelete(PdmPixmapList* me);
static Pixmap PdmPixmapListGetPixmap(PdmPixmapList* me, int i);
#define PdmPixmapListCount(me) ((me)->count)
static void PdmPixmapListSetImageName(PdmPixmapList* me,
				      const char* name,
				      int i);

/*
 * option menu list creation function
 */
static Widget
CreateOptionMenuPulldown(String pulldown_name,
			 Widget option_menu,
			 XtCallbackProc activate_proc,
			 XtPointer user_data,
			 XmString* item_list,
			 int item_count,
			 int initial_item);
/*
 * XmString / oid utilities
 */
static XmString* CreateXmStringsFromOidList(const PdmOidList* list);
static void FreeXmStringList(XmString* list, int count);

/*
 * utility to help determine maximum icon size
 */
static void
IconMaxDimension(Widget icon_label,
		 PdmPixmapList* pixmap_list, int i,
		 Dimension* max_width, Dimension* max_height);

/*
 * utility to position list box initial selection
 */
static void SetListBoxSelection(Widget list_box, int position);

/*
 * utility to grab icon file names out of the Xrm resource database
 */
static String GetIconFileName(Widget w, String resource_name);

/*
 * ------------------------------------------------------------------------
 * Name: PdmPrinterBoxNew
 *
 * Description:
 *
 *     Creates a new PdmPrinterBox (PdmSetupBox) instance structure.
 *
 * Return value:
 *
 *     The new PdmPrinterBox instance structure.
 *
 */
PdmSetupBox*
PdmPrinterBoxNew()
{
    PdmSetupBox* me = (PdmSetupBox*)XtCalloc(1, sizeof(PdmSetupBox));
    PdmPrinterBoxData* data;
    
    me->delete_proc = PdmPrinterBoxDelete;
    me->create_proc = PdmPrinterBoxCreate;
    me->verify_attr_proc = PdmPrinterBoxVerifyAttr;
    me->get_attr_proc = PdmPrinterBoxGetAttr;
    me->set_attr_proc = PdmPrinterBoxSetAttr;
    me->fallback_resources = PdmPrinterBoxFallbackResources;
    me->fallback_resources_count = XtNumber(PdmPrinterBoxFallbackResources);
    me->tab_name = "PrinterTab";

    me->subclass_data = (XtPointer)XtCalloc(1, sizeof(PdmPrinterBoxData));
    data = (PdmPrinterBoxData*)me->subclass_data;
    
    data->plex_ctl = PlexCtlNew();
    data->orient_ctl = OrientCtlNew(data->plex_ctl);

    data->size_ctl = SizeCtlNew();
    data->tray_ctl = TrayCtlNew(data->size_ctl);

    return me;
}

/*
 * ------------------------------------------------------------------------
 * Name: PdmPrinterBoxDelete
 *
 * Description:
 *
 *     Frees the passed PdmPrinterBox (PdmSetupBox) instance structure.
 *
 * Return value:
 *
 *     None.
 *
 */
static void
PdmPrinterBoxDelete(PdmSetupBox* me)
{
    if(me != (PdmSetupBox*)NULL)
    {
	PdmPrinterBoxData* data = (PdmPrinterBoxData*)me->subclass_data;

	XtFree(data->document_format);
	
	PdmOidListDelete(data->document_attrs_supported);
	PdmOidListDelete(data->orientations_supported);
	PdmOidListDelete(data->plexes_supported);
	PdmOidListDelete(data->input_trays);
	PdmOidListDelete(data->tray_sizes);
	PdmOidLinkedListDelete(data->sizes_supported);
	
	OrientCtlDelete(data->orient_ctl);
	PlexCtlDelete(data->plex_ctl);
	SizeCtlDelete(data->size_ctl);
	TrayCtlDelete(data->tray_ctl);
	
	XtFree((char*)data);
	XtFree((char*)me);
    }
}

/*
 * ------------------------------------------------------------------------
 * Name: PdmPrinterBoxCreate
 *
 * Description:
 *
 *     Creates the PDM Printer options setup box.
 *
 * Return value:
 *
 *     The passed PdmPrinterBox (PdmSetupBox) instance structure.
 *
 */
static void
PdmPrinterBoxCreate(PdmSetupBox* me, Widget parent)
{
    me->widget = PdmPrinterBoxCreateWindow(me, parent);
    XtManageChild(me->widget);
}

/*
 * ------------------------------------------------------------------------
 * Name: PdmPrinterBoxVerifyAttr
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
PdmPrinterBoxVerifyAttr(PdmSetupBox* me, PdmXp* pdm_xp)
{
    return PDM_SUCCESS;
}

/*
 * ------------------------------------------------------------------------
 * Name: PdmPrinterBoxGetAttr
 *
 * Description:
 *
 *     
 *
 * Return value:
 *
 *     None.
 *
 */
static void
PdmPrinterBoxGetAttr(PdmSetupBox* me, PdmXp* pdm_xp)
{
    PdmPrinterBoxData* data = (PdmPrinterBoxData*)me->subclass_data;
    const char* strval;
    PdmOidMediumSS* medium_ss_supported;
    PdmOidTrayMediumList* input_trays_medium;
    /*
     * printer model description
     */
    data->printer_model =
	PdmXpGetStringValue(pdm_xp, XPPrinterAttr, pdmoid_att_printer_model);
    /*
     * document format
     */
    strval = PdmXpGetStringValue(pdm_xp, XPDocAttr,
				 pdmoid_att_document_format);
    data->document_format = PdmOidDocumentFormatParse(strval);
    if((char*)NULL == data->document_format)
    {
	/*
	 * document format not specified, try to obtain
	 * document-formats-supported, and obtain up the default from it
	 */
	strval = PdmXpGetStringValue(pdm_xp, XPPrinterAttr,
				     pdmoid_att_document_formats_supported);
	data->document_format = PdmOidDocumentFormatDefault(strval);
    }
    /*
     * document attributes supported
     */
    strval = PdmXpGetStringValue(pdm_xp, XPPrinterAttr,
				 pdmoid_att_document_attributes_supported);
    data->document_attrs_supported = PdmOidListNew(strval);
    /*
     * content orientation supported
     */
    strval = PdmXpGetStringValue(pdm_xp, XPPrinterAttr,
				 pdmoid_att_content_orientations_supported);
    data->orientations_supported = PdmOidListNew(strval);
    /*
     * orientation
     */
    data->orientation =
	PdmXpGetValue(pdm_xp, XPDocAttr, pdmoid_att_content_orientation);
    /*
     * plexes supported
     */
    strval = PdmXpGetStringValue(pdm_xp, XPPrinterAttr,
				 pdmoid_att_plexes_supported);
    data->plexes_supported = PdmOidListNew(strval);
    /*
     * plex
     */
    data->plex = PdmXpGetValue(pdm_xp, XPDocAttr, pdmoid_att_plex);
    /*
     * default input tray, default medium
     */
    data->default_input_tray =
	PdmXpGetValue(pdm_xp, XPDocAttr, pdmoid_att_default_input_tray);
    data->default_medium =
	PdmXpGetValue(pdm_xp, XPDocAttr, pdmoid_att_default_medium);
    /*
     * medium source sizes supported
     */
    strval = PdmXpGetStringValue(pdm_xp, XPPrinterAttr,
				 pdmoid_att_medium_source_sizes_supported);
    medium_ss_supported = PdmOidMediumSSNew(strval);
    /*
     * input trays medium
     */
    strval = PdmXpGetStringValue(pdm_xp, XPPrinterAttr,
				 pdmoid_att_input_trays_medium);
    input_trays_medium = PdmOidTrayMediumListNew(strval);
    /*
     * build input trays and page sizes ready lists
     */
    PdmOidMediumSSGetTraysSizes(medium_ss_supported, input_trays_medium,
				&data->input_trays, &data->tray_sizes);
    /*
     * build list of supported page sizes
     */
    data->sizes_supported = PdmOidMediumSSGetAllSizes(medium_ss_supported);
    /*
     * clean up
     */
    PdmOidMediumSSDelete(medium_ss_supported);
    PdmOidTrayMediumListDelete(input_trays_medium);
}

/*
 * ------------------------------------------------------------------------
 * Name: PdmPrinterBoxSetAttr
 *
 * Description:
 *
 *     
 *
 * Return value:
 *
 *     None.
 *
 */
static void
PdmPrinterBoxSetAttr(PdmSetupBox* me, PdmXp* pdm_xp)
{
    PdmPrinterBoxData* data = (PdmPrinterBoxData*)me->subclass_data;
    /*
     * orientation
     */
    PdmXpSetValue(pdm_xp, XPDocAttr, pdmoid_att_content_orientation,
		  OrientCtlGetOrientation(data->orient_ctl));
    /*
     * plex
     */
    PdmXpSetValue(pdm_xp, XPDocAttr, pdmoid_att_plex,
		  PlexCtlGetPlex(data->plex_ctl));
    /*
     * page size
     */
    PdmXpSetValue(pdm_xp, XPDocAttr, pdmoid_att_default_medium,
		  SizeCtlGetSize(data->size_ctl));
    /*
     * input tray
     */
    PdmXpSetValue(pdm_xp, XPDocAttr, pdmoid_att_default_input_tray,
		  TrayCtlGetTray(data->tray_ctl));
}

/*
 * ------------------------------------------------------------------------
 * Name: PdmPrinterBoxCreateWindow
 *
 * Description:
 *
 *     Creates the PDM Printer options setup box window.
 *
 * Return value:
 *
 *     Widget ID of the new Printer setup box.
 *
 */
static Widget
PdmPrinterBoxCreateWindow(PdmSetupBox* me, Widget parent)
{
    PdmPrinterBoxData* data = (PdmPrinterBoxData*)me->subclass_data;
    Widget manager;
    Widget row;
    Widget w;
    Widget draw_area;
    XmString label;
    /*
     * create the manager for the Printer Setup Box
     */
    manager = XtVaCreateManagedWidget(
				      "PrinterSetup",
				      xmRowColumnWidgetClass,
				      parent,
				      NULL);
    /*
     * create the printer model row
     */
    if(data->printer_model != (const char*)NULL)
    {
	char* desc;
	char* ptr;

	row = XtVaCreateManagedWidget(
				      "PrinterModelRow",
				      xmRowColumnWidgetClass,
				      manager,
				      XmNorientation, XmHORIZONTAL,
				      NULL);
	/*
	 * create the printer model label
	 */
	w = XtVaCreateManagedWidget("PrinterModelLabel",
				    xmLabelGadgetClass,
				    row,
				    NULL);
	/*
	 * create the printer model using just the 1st line
	 */
	desc = XtNewString(data->printer_model);
	ptr = Dt_strchr(desc, '\n');
	if(ptr != NULL)
	    *ptr = '\0';
	label = XmStringCreateLocalized(desc);
	XtFree(desc);
	w = XtVaCreateManagedWidget("PrinterModel",
				    xmLabelGadgetClass,
				    row,
				    XmNlabelString, label,
				    NULL);
	XmStringFree(label);
    }
    /*
     * create the document format row
     */
    if((char*)NULL != data->document_format)
    {
	/*
	 * create the document format row
	 */
	row = XtVaCreateManagedWidget(
				      "DocumentFormatRow",
				      xmRowColumnWidgetClass,
				      manager,
				      XmNorientation, XmHORIZONTAL,
				      NULL);
	/*
	 * create the document format label
	 */
	w = XtVaCreateManagedWidget("DocumentFormatLabel",
				    xmLabelGadgetClass,
				    row,
				    NULL);
	/*
	 * create the document format
	 */
	label = XmStringCreateLocalized(data->document_format);
	w = XtVaCreateManagedWidget("DocumentFormat",
				    xmLabelGadgetClass,
				    row,
				    XmNlabelString, label,
				    NULL);
	XmStringFree(label);
    }
    /*
     * orientation / plex / tray / sides
     */
    draw_area = XtVaCreateWidget("PrinterSetupControls",
				 xmDrawingAreaWidgetClass,
				 manager,
				 NULL);
    XtAddCallback(draw_area, XmNresizeCallback,
		  PdmPrinterBoxResizeCB, (XtPointer)me);
    /*
     * orientation control
     */
    OrientCtlCreate(data->orient_ctl,
		    draw_area,
		    data->orientation,
		    data->document_attrs_supported,
		    data->orientations_supported);
    /*
     * page size control
     */
    SizeCtlCreate(data->size_ctl,
		  draw_area,
		  data->default_medium,
		  data->document_attrs_supported,
		  data->tray_sizes,
		  data->sizes_supported);
    /*
     * plex control
     */
    PlexCtlCreate(data->plex_ctl,
		  draw_area,
		  data->plex,
		  data->document_attrs_supported,
		  data->plexes_supported);
    /*
     * input tray control
     */
    TrayCtlCreate(data->tray_ctl,
		  draw_area,
		  data->default_input_tray,
		  data->default_medium,
		  data->document_attrs_supported,
		  data->input_trays,
		  data->tray_sizes);

    XtManageChild(draw_area);
    /*
     * return the new Printer Setup Box
     */
    return manager;
}

/*
 * ------------------------------------------------------------------------
 * Name: PdmPrinterBoxResizeCB
 *
 * Description:
 *
 *     
 *
 * Return value:
 *
 *     None.
 *
 */
static void
PdmPrinterBoxResizeCB(Widget w,
		      XtPointer client_data,
		      XtPointer call_data)
{
    PdmSetupBox* me = (PdmSetupBox*)client_data;
    PdmPrinterBoxData* data = (PdmPrinterBoxData*)me->subclass_data;
    typedef struct {
	Position x, y;
	Dimension width, height;
    } PdmRect;
    PdmRect rt, rs, ro, rp, c1, c2, r1, r2;
    /*
     * retrieve child dimensions
     */
    XtVaGetValues(TrayCtlGetWidget(data->tray_ctl),
		  XmNx, &rt.x,
		  XmNy, &rt.y,
		  XmNwidth, &rt.width,
		  XmNheight, &rt.height,
		  NULL);
    XtVaGetValues(SizeCtlGetWidget(data->size_ctl),
		  XmNx, &rs.x,
		  XmNy, &rs.y,
		  XmNwidth, &rs.width,
		  XmNheight, &rs.height,
		  NULL);
    XtVaGetValues(OrientCtlGetWidget(data->orient_ctl),
		  XmNx, &ro.x,
		  XmNy, &ro.y,
		  XmNwidth, &ro.width,
		  XmNheight, &ro.height,
		  NULL);
    XtVaGetValues(PlexCtlGetWidget(data->plex_ctl),
		  XmNx, &rp.x,
		  XmNy, &rp.y,
		  XmNwidth, &rp.width,
		  XmNheight, &rp.height,
		  NULL);
    /*
     * calculate rows and columns dimensions
     */
    r1.x = r2.x = c1.x = 0;
    r1.y = c1.y = c2.y = 0;
    r1.height = MAX(ro.height, rp.height);
    r2.height = MAX(rs.height, rt.height);
    c1.width = MAX(ro.width, rs.width);
    c2.width = MAX(rp.width, rt.width);
    r2.y = r1.height + 5;
    c2.x = c1.width + 3;
    r1.width = r2.width = c2.x + c2.width;
    c1.height = c2.height = r2.y + r2.height;
    /*
     * position the children
     */
    XtVaSetValues(OrientCtlGetWidget(data->orient_ctl),
		  XmNx, c1.x,
		  XmNy, r1.y,
		  XmNwidth, c1.width,
		  XmNheight, r1.height,
		  NULL);
    XtVaSetValues(PlexCtlGetWidget(data->plex_ctl),
		  XmNx, c2.x,
		  XmNy, r1.y,
		  XmNwidth, c2.width,
		  XmNheight, r1.height,
		  NULL);
    XtVaSetValues(SizeCtlGetWidget(data->size_ctl),
		  XmNx, c1.x,
		  XmNy, r2.y,
		  XmNwidth, c1.width,
		  XmNheight, r2.height,
		  NULL);
    XtVaSetValues(TrayCtlGetWidget(data->tray_ctl),
		  XmNx, c2.x,
		  XmNy, r2.y,
		  XmNwidth, c2.width,
		  XmNheight, r2.height,
		  NULL);
    /*
     * resize the drawing area to accomodate the new layout
     */
    XtVaSetValues(w,
		  XmNwidth, r1.width,
		  XmNheight, c1.height,
		  NULL);
}

/*
 * ------------------------------------------------------------------------
 * Name: OrientCtlNew
 *
 * Description:
 *
 *     
 *
 * Return value:
 *
 *     
 *
 */
static OrientCtl*
OrientCtlNew(PlexCtl* plex_ctl)
{
    OrientCtl* me = (OrientCtl*)XtCalloc(1, sizeof(OrientCtl));
    me->plex_ctl = plex_ctl;
    return me;
}

/*
 * ------------------------------------------------------------------------
 * Name: OrientCtlDelete
 *
 * Description:
 *
 *     
 *
 * Return value:
 *
 *     
 *
 */
static void
OrientCtlDelete(OrientCtl* me)
{
    PdmOidListDelete(me->dummy_orientations_supported);
    PdmPixmapListDelete(me->pixmap_list);
    PdmPixmapListDelete(me->i_pixmap_list);
    
    XtFree((char*)me);
}

/*
 * ------------------------------------------------------------------------
 * Name: OrientCtlCreate
 *
 * Description:
 *
 *     
 *
 * Return value:
 *
 *     
 *
 */
static void
OrientCtlCreate(OrientCtl* me,
		Widget parent,
		PdmOid orientation,
		const PdmOidList* document_attrs_supported,
		const PdmOidList* orientations_supported)
{
    Boolean supported;
    /*
     * determine if setting the content orientation is supported
     */
    if(PdmOidListHasOid(document_attrs_supported,
			pdmoid_att_content_orientation)
       &&
       PdmOidListCount(orientations_supported) > 0)
    {
	supported = True;
	me->orientations_supported = orientations_supported;
    }
    else
    {
	supported = False;
	/*
	 * dummy up a list to be presented (insensitive)
	 */
	me->dummy_orientations_supported = 
	  PdmOidListNew(PdmOidString(pdmoid_val_content_orientation_portrait));
	me->orientations_supported = me->dummy_orientations_supported;
	orientation = pdmoid_none;
    }
    /*
     * set the initially selected list item
     */
    OrientCtlSetSelectedItem(me,
			     PdmOidListGetIndex(me->orientations_supported,
						orientation));
    /*
     * create the orientations control window
     */
    OrientCtlCreateWindow(me, parent);
    /*
     * initialize the window components
     */
    OrientCtlCreatePulldown(me);
    OrientCtlInitPixmapList(me);
    OrientCtlUpdateIcon(me);
    /*
     * if content-orientation is not supported, disable the control
     */
    if(!supported)
	OrientCtlDisable(me);
}

/*
 * ------------------------------------------------------------------------
 * Name: OrientCtlCreateWindow
 *
 * Description:
 *
 *     
 *
 * Return value:
 *
 *     
 *
 */
static void
OrientCtlCreateWindow(OrientCtl* me,
		      Widget parent)
{
    Widget control_manager;
    
    /*
     * orientation frame
     */
    me->widget = XtVaCreateManagedWidget("OrientCtlTop",
					 xmFrameWidgetClass,
					 parent,
					 XmNshadowType, XmSHADOW_ETCHED_IN,
					 NULL);
    /*
     * orientation label
     */
    me->frame_label =
	XtVaCreateManagedWidget("OrientationLabel",
				xmLabelWidgetClass,
				me->widget,
				XmNframeChildType, XmFRAME_TITLE_CHILD,
				NULL);
    /*
     * orientation controls manager
     */
    control_manager =
	XtVaCreateManagedWidget("OrientationControls",
				xmDrawingAreaWidgetClass,
				me->widget,
				XmNframeChildType, XmFRAME_WORKAREA_CHILD,
				NULL);
    XtAddCallback(control_manager, XmNresizeCallback, OrientCtlResizeCB, me);
    /*
     * orientations option menu
     */
    me->option_menu =
	XtVaCreateManagedWidget("OrientationOptionMenu",
				xmRowColumnWidgetClass,
				control_manager,
				XmNmarginHeight, 0,
				XmNrowColumnType, XmMENU_OPTION,
				NULL);
    /*
     * orientation icon
     */
    me->icon_label =
	XtVaCreateManagedWidget("OrientationIcon",
				xmLabelWidgetClass,
				control_manager,
				XmNlabelType, XmPIXMAP,
				XmNalignment, XmALIGNMENT_CENTER,
				NULL);
}

/*
 * ------------------------------------------------------------------------
 * Name: OrientCtlDisable
 *
 * Description:
 *
 *     Disable the orientation controls.
 *
 * Return value:
 *
 *     None.
 *
 */
static void
OrientCtlDisable(OrientCtl* me)
{
    XtSetSensitive(me->frame_label, False);
    XtSetSensitive(me->option_menu, False);
    XtSetSensitive(me->icon_label, False);
}

/*
 * ------------------------------------------------------------------------
 * Name: OrientCtlResizeCB
 *
 * Description:
 *
 *     
 *
 * Return value:
 *
 *     None.
 *
 */
static void
OrientCtlResizeCB(Widget w, XtPointer client_data, XtPointer call_data)
{
    OrientCtl* me = (OrientCtl*)client_data;
    Dimension height, width, margin_width, new_width;
    Position menu_x, menu_y, icon_x, icon_y;
    Dimension menu_h, menu_w, icon_h, icon_w;
    /*
     * get current layout info
     */
    XtVaGetValues(w, XmNheight, &height, XmNwidth, &width,
		  XmNmarginWidth, &margin_width,
		  NULL);
    XtVaGetValues(me->option_menu, XmNheight, &menu_h, XmNwidth, &menu_w,
		  NULL);
    XtVaGetValues(me->icon_label, XmNheight, &icon_h, XmNwidth, &icon_w,
		  NULL);
    /*
     * position controls side by side
     */
    menu_x = 0;
    icon_x = menu_x + menu_w;
    /*
     * center vertically
     */
    menu_y = (height - menu_h) / 2;
    icon_y = (height - icon_h) / 2;
    /*
     * set new positions
     */
    XtVaSetValues(me->option_menu, XmNx, menu_x, XmNy, menu_y, NULL);
    XtVaSetValues(me->icon_label, XmNx, icon_x, XmNy, icon_y, NULL);
    /*
     * update drawing area width
     */
    new_width = menu_x + menu_w + icon_w + margin_width;
    if(new_width > width)
	XtVaSetValues(w, XmNwidth, new_width, NULL);
}

/*
 * ------------------------------------------------------------------------
 * Name: OrientCtlCreatePulldown
 *
 * Description:
 *
 *     
 *
 * Return value:
 *
 *     
 *
 */
static void
OrientCtlCreatePulldown(OrientCtl* me)
{
    XmString* list;
    int count;
    /*
     * create a list of XmStrings for the content orientations
     */
    list = CreateXmStringsFromOidList(me->orientations_supported);
    /*
     * create the option menu pulldown
     */
    count = PdmOidListCount(me->orientations_supported);
    CreateOptionMenuPulldown("OrientationOptions",
			     me->option_menu,
			     OrientCtlSelectCB,
			     (XtPointer)me,
			     list, count,
			     me->selected_item);
    /*
     * clean up
     */
    FreeXmStringList(list, count);
}

/*
 * ------------------------------------------------------------------------
 * Name: OrientCtlSelectCB
 *
 * Description:
 *
 *     
 *
 * Return value:
 *
 *     
 *
 */
static void
OrientCtlSelectCB(Widget w, XtPointer client_data, XtPointer call_data)
{
    OrientCtl* me;
    int selected_item = (int)client_data;

    XtVaGetValues(w, XmNuserData, &me, NULL);
    OrientCtlSetSelectedItem(me, selected_item);
    OrientCtlUpdateIcon(me);
}

/*
 * ------------------------------------------------------------------------
 * Name: OrientCtlInitPixmapList
 *
 * Description:
 *
 *     
 *
 * Return value:
 *
 *     
 *
 */
static void
OrientCtlInitPixmapList(OrientCtl* me)
{
    int i;
    const char* image_name;
    int count;
    Dimension max_width = 0;
    Dimension max_height = 0;
    
    /*
     * initialize the list of pixmaps based on the supported orientations
     */
    count = PdmOidListCount(me->orientations_supported);
    me->pixmap_list = PdmPixmapListNew(count, me->icon_label, False);
    me->i_pixmap_list = PdmPixmapListNew(count, me->icon_label, True);
    for(i = 0; i < count; i++)
    {
	switch(PdmOidListGetOid(me->orientations_supported, i))
	{
	case pdmoid_val_content_orientation_portrait:
	    image_name = GetIconFileName(me->widget, "PortraitIconFileName");
	    break;
	case pdmoid_val_content_orientation_landscape:
	    image_name = GetIconFileName(me->widget, "LandscapeIconFileName");
	    break;
	case pdmoid_val_content_orientation_reverse_portrait:
	    image_name =
		GetIconFileName(me->widget, "ReversePortraitIconFileName");
	    break;
	case pdmoid_val_content_orientation_reverse_landscape:
	    image_name =
		GetIconFileName(me->widget, "ReverseLandscapeIconFileName");
	    break;
	default:
	    continue;
	}
	PdmPixmapListSetImageName(me->pixmap_list, image_name, i);
	PdmPixmapListSetImageName(me->i_pixmap_list, image_name, i);
	IconMaxDimension(me->icon_label,
			 me->pixmap_list, i,
			 &max_width, &max_height);
    }
    XtVaSetValues(me->icon_label,
		  XmNwidth, max_width,
		  XmNheight, max_height,
		  XmNrecomputeSize, False,
		  NULL);
}

/*
 * ------------------------------------------------------------------------
 * Name: OrientCtlSetSelectedItem
 *
 * Description:
 *
 *     
 *
 * Return value:
 *
 *     
 *
 */
static void
OrientCtlSetSelectedItem(OrientCtl* me, int selected_item)
{
    if(selected_item == -1)
	me->selected_item = 0;
    else
	me->selected_item = selected_item;

    if(me->plex_ctl != (PlexCtl*)NULL)
	PlexCtlSetOrientation(me->plex_ctl, OrientCtlGetOrientation(me));
}


/*
 * ------------------------------------------------------------------------
 * Name: OrientCtlUpdateIcon
 *
 * Description:
 *
 *     
 *
 * Return value:
 *
 *     
 *
 */
static void
OrientCtlUpdateIcon(OrientCtl* me)
{
    if(me->icon_label != (Widget)NULL)
	XtVaSetValues(me->icon_label,
		      XmNlabelPixmap,
		      PdmPixmapListGetPixmap(me->pixmap_list,
					     me->selected_item),
		      XmNlabelInsensitivePixmap,
		      PdmPixmapListGetPixmap(me->i_pixmap_list,
					     me->selected_item),
		      NULL);
}

/*
 * ------------------------------------------------------------------------
 * Name: OrientCtlGetOrientation
 *
 * Description:
 *
 *     
 *
 * Return value:
 *
 *     
 *
 */
static PdmOid
OrientCtlGetOrientation(OrientCtl* me)
{
    if(me->orientations_supported == me->dummy_orientations_supported)
	return pdmoid_none;
    else
	return PdmOidListGetOid(me->orientations_supported, me->selected_item);
}


/*
 * ------------------------------------------------------------------------
 * Name: PlexCtlNew
 *
 * Description:
 *
 *     
 *
 * Return value:
 *
 *     
 *
 */
static PlexCtl*
PlexCtlNew()
{
    PlexCtl* me = (PlexCtl*)XtCalloc(1, sizeof(PlexCtl));
    me->pixmap_list_ptr = &me->portrait_pixmap_list;
    me->i_pixmap_list_ptr = &me->i_portrait_pixmap_list;
    return me;
}

/*
 * ------------------------------------------------------------------------
 * Name: PlexCtlDelete
 *
 * Description:
 *
 *     
 *
 * Return value:
 *
 *     
 *
 */
static void
PlexCtlDelete(PlexCtl* me)
{
    PdmOidListDelete(me->dummy_plexes_supported);
    PdmPixmapListDelete(me->portrait_pixmap_list);
    PdmPixmapListDelete(me->i_portrait_pixmap_list);
    PdmPixmapListDelete(me->landscape_pixmap_list);
    PdmPixmapListDelete(me->i_landscape_pixmap_list);
    
    XtFree((char*)me);
}

/*
 * ------------------------------------------------------------------------
 * Name: PlexCtlCreate
 *
 * Description:
 *
 *     
 *
 * Return value:
 *
 *     
 *
 */
static void
PlexCtlCreate(PlexCtl* me,
	      Widget parent,
	      PdmOid plex,
	      const PdmOidList* document_attrs_supported,
	      const PdmOidList* plexes_supported)
{
    Boolean supported;
    /*
     * determine if setting the content orientation is supported
     */
    if(PdmOidListHasOid(document_attrs_supported, pdmoid_att_plex)
       &&
       PdmOidListCount(plexes_supported) > 0)
    {
	supported = True;
	me->plexes_supported = plexes_supported;
    }
    else
    {
	supported = False;
	/*
	 * dummy up a list to be presented (insensitive)
	 */
	me->dummy_plexes_supported = 
	  PdmOidListNew(PdmOidString(pdmoid_val_plex_simplex));
	me->plexes_supported = me->dummy_plexes_supported;
	plex = pdmoid_none;
    }

    /*
     * set the initially selected list item
     */
    PlexCtlSetSelectedItem(me, PdmOidListGetIndex(me->plexes_supported, plex));
    /*
     * create the plexes control window
     */
    PlexCtlCreateWindow(me, parent);
    /*
     * initialize the window components
     */
    PlexCtlCreatePulldown(me);
    PlexCtlInitPixmapLists(me);
    PlexCtlUpdateIcon(me);
    /*
     * if plex is not supported, disable the control
     */
    if(!supported)
	PlexCtlDisable(me);
}

/*
 * ------------------------------------------------------------------------
 * Name: PlexCtlCreateWindow
 *
 * Description:
 *
 *     
 *
 * Return value:
 *
 *     
 *
 */
static void
PlexCtlCreateWindow(PlexCtl* me,
		      Widget parent)
{
    Widget control_manager;
    /*
     * plex frame
     */
    me->widget = XtVaCreateManagedWidget("PlexCtlTop",
					 xmFrameWidgetClass,
					 parent,
					 XmNshadowType, XmSHADOW_ETCHED_IN,
					 NULL);
    /*
     * plex label
     */
    me->frame_label =
	XtVaCreateManagedWidget("PlexLabel",
				xmLabelGadgetClass,
				me->widget,
				XmNframeChildType, XmFRAME_TITLE_CHILD,
				NULL);
    /*
     * plex controls manager
     */
    control_manager =
	XtVaCreateManagedWidget(
				"PlexControls",
				xmDrawingAreaWidgetClass,
				me->widget,
				XmNframeChildType, XmFRAME_WORKAREA_CHILD,
				NULL);
    XtAddCallback(control_manager, XmNresizeCallback, PlexCtlResizeCB, me);
    /*
     * plexes option menu
     */
    me->option_menu =
	XtVaCreateManagedWidget("PlexOptionMenu",
				xmRowColumnWidgetClass,
				control_manager,
				XmNrowColumnType, XmMENU_OPTION,
				XmNmarginHeight, 0,
				NULL);
    /*
     * plex icon
     */
    me->icon_label =
	XtVaCreateManagedWidget("PlexIcon",
				xmLabelWidgetClass,
				control_manager,
				XmNlabelType, XmPIXMAP,
				XmNalignment, XmALIGNMENT_CENTER,
				NULL);
}

/*
 * ------------------------------------------------------------------------
 * Name: PlexCtlDisable
 *
 * Description:
 *
 *     Disable the plex controls.
 *
 * Return value:
 *
 *     None.
 *
 */
static void
PlexCtlDisable(PlexCtl* me)
{
    XtSetSensitive(me->frame_label, False);
    XtSetSensitive(me->option_menu, False);
    XtSetSensitive(me->icon_label, False);
}

/*
 * ------------------------------------------------------------------------
 * Name: PlexCtlResizeCB
 *
 * Description:
 *
 *     
 *
 * Return value:
 *
 *     None.
 *
 */
static void
PlexCtlResizeCB(Widget w, XtPointer client_data, XtPointer call_data)
{
    PlexCtl* me = (PlexCtl*)client_data;
    Dimension height, width, margin_width, new_width;
    Position menu_x, menu_y, icon_x, icon_y;
    Dimension menu_h, menu_w, icon_h, icon_w;
    /*
     * get current layout info
     */
    XtVaGetValues(w, XmNheight, &height, XmNwidth, &width,
		  XmNmarginWidth, &margin_width,
		  NULL);
    XtVaGetValues(me->option_menu, XmNheight, &menu_h, XmNwidth, &menu_w,
		  NULL);
    XtVaGetValues(me->icon_label, XmNheight, &icon_h, XmNwidth, &icon_w,
		  NULL);
    /*
     * position controls side by side
     */
    menu_x = 0;
    icon_x = menu_x + menu_w;
    /*
     * center vertically
     */
    menu_y = (height - menu_h) / 2;
    icon_y = (height - icon_h) / 2;
    /*
     * set new positions
     */
    XtVaSetValues(me->option_menu, XmNx, menu_x, XmNy, menu_y, NULL);
    XtVaSetValues(me->icon_label, XmNx, icon_x, XmNy, icon_y, NULL);
    /*
     * update drawing area width
     */
    new_width = menu_x + menu_w + icon_w + margin_width;
    if(new_width > width)
	XtVaSetValues(w, XmNwidth, new_width, NULL);
}

/*
 * ------------------------------------------------------------------------
 * Name: PlexCtlCreatePulldown
 *
 * Description:
 *
 *     
 *
 * Return value:
 *
 *     
 *
 */
static void
PlexCtlCreatePulldown(PlexCtl* me)
{
    XmString* list;
    int count;
    /*
     * build a list of Xm strings for the supported plexes
     */
    list = CreateXmStringsFromOidList(me->plexes_supported);
    /*
     * create the option menu pulldown
     */
    count = PdmOidListCount(me->plexes_supported);
    CreateOptionMenuPulldown("PlexOptions",
			     me->option_menu,
			     PlexCtlSelectCB,
			     (XtPointer)me,
			     list, count,
			     me->selected_item);
    /*
     * clean up
     */
    FreeXmStringList(list, count);
}

/*
 * ------------------------------------------------------------------------
 * Name: PlexCtlSelectCB
 *
 * Description:
 *
 *     
 *
 * Return value:
 *
 *     
 *
 */
static void
PlexCtlSelectCB(Widget w, XtPointer client_data, XtPointer call_data)
{
    PlexCtl* me;
    int selected_item = (int)client_data;

    XtVaGetValues(w, XmNuserData, &me, NULL);
    PlexCtlSetSelectedItem(me, selected_item);
    PlexCtlUpdateIcon(me);
}

/*
 * ------------------------------------------------------------------------
 * Name: PlexCtlSetOrientation
 *
 * Description:
 *
 *     
 *
 * Return value:
 *
 *     
 *
 */
static void
PlexCtlSetOrientation(PlexCtl* me,
		      PdmOid orientation)
{
    switch(orientation)
    {
    case pdmoid_val_content_orientation_landscape:
    case pdmoid_val_content_orientation_reverse_landscape:
	me->pixmap_list_ptr = &me->landscape_pixmap_list;
	me->i_pixmap_list_ptr = &me->i_landscape_pixmap_list;
	break;
    default:
	me->pixmap_list_ptr = &me->portrait_pixmap_list;
	me->i_pixmap_list_ptr = &me->i_portrait_pixmap_list;
	break;
    }
    PlexCtlUpdateIcon(me);
}

/*
 * ------------------------------------------------------------------------
 * Name: PlexCtlInitPixmapLists
 *
 * Description:
 *
 *     
 *
 * Return value:
 *
 *     
 *
 */
static void
PlexCtlInitPixmapLists(PlexCtl* me)
{
    int i;
    const char* portrait_image_name;
    const char* landscape_image_name;
    int count;
    Dimension max_width = 0;
    Dimension max_height = 0;
    
    /*
     * initialize the list of pixmaps based on the list of plexes
     */
    count = PdmOidListCount(me->plexes_supported);
    me->portrait_pixmap_list = PdmPixmapListNew(count, me->icon_label, False);
    me->i_portrait_pixmap_list = PdmPixmapListNew(count, me->icon_label, True);
    me->landscape_pixmap_list = PdmPixmapListNew(count, me->icon_label, False);
    me->i_landscape_pixmap_list = PdmPixmapListNew(count, me->icon_label, True);
    for(i = 0; i < count; i++)
    {
	switch(PdmOidListGetOid(me->plexes_supported, i))
	{
	case pdmoid_val_plex_simplex:
	    portrait_image_name =
		GetIconFileName(me->widget, "SimplexPortraitIconFileName");
	    landscape_image_name =
		GetIconFileName(me->widget, "SimplexLandscapeIconFileName");
	    break;
	case pdmoid_val_plex_duplex:
	    portrait_image_name =
		GetIconFileName(me->widget, "DuplexPortraitIconFileName");
	    landscape_image_name =
		GetIconFileName(me->widget, "DuplexLandscapeIconFileName");
	    break;
	case pdmoid_val_plex_tumble:
	    portrait_image_name =
		GetIconFileName(me->widget, "TumblePortraitIconFileName");
	    landscape_image_name =
		GetIconFileName(me->widget, "TumbleLandscapeIconFileName");
	    break;
	default:
	    continue;
	}
	PdmPixmapListSetImageName(me->portrait_pixmap_list,
				  portrait_image_name, i);
	PdmPixmapListSetImageName(me->i_portrait_pixmap_list,
				  portrait_image_name, i);
	PdmPixmapListSetImageName(me->landscape_pixmap_list,
				  landscape_image_name, i);
	PdmPixmapListSetImageName(me->i_landscape_pixmap_list,
				  landscape_image_name, i);
	IconMaxDimension(me->icon_label,
			 me->portrait_pixmap_list, i,
			 &max_width, &max_height);
	IconMaxDimension(me->icon_label,
			 me->landscape_pixmap_list, i,
			 &max_width, &max_height);
    }
    XtVaSetValues(me->icon_label,
		  XmNwidth, max_width,
		  XmNheight, max_height,
		  XmNrecomputeSize, False,
		  NULL);
}

/*
 * ------------------------------------------------------------------------
 * Name: PlexCtlSetSelectedItem
 *
 * Description:
 *
 *     
 *
 * Return value:
 *
 *     
 *
 */
static void
PlexCtlSetSelectedItem(PlexCtl* me, int selected_item)
{
    if(selected_item == -1)
	me->selected_item = 0;
    else
	me->selected_item = selected_item;
}

/*
 * ------------------------------------------------------------------------
 * Name: PlexCtlUpdateIcon
 *
 * Description:
 *
 *     
 *
 * Return value:
 *
 *     
 *
 */
static void
PlexCtlUpdateIcon(PlexCtl* me)
{
    if(me->icon_label != (Widget)NULL)
	XtVaSetValues(me->icon_label,
		      XmNlabelPixmap,
		      PdmPixmapListGetPixmap(*me->pixmap_list_ptr,
					     me->selected_item),
		      XmNlabelInsensitivePixmap,
		      PdmPixmapListGetPixmap(*me->i_pixmap_list_ptr,
					     me->selected_item),
		      NULL);
}

/*
 * ------------------------------------------------------------------------
 * Name: PlexCtlGetPlex
 *
 * Description:
 *
 *     
 *
 * Return value:
 *
 *     
 *
 */
static PdmOid
PlexCtlGetPlex(PlexCtl* me)
{
    if(me->plexes_supported == me->dummy_plexes_supported)
	return pdmoid_none;
    else
	return PdmOidListGetOid(me->plexes_supported, me->selected_item);
}

/*
 * ------------------------------------------------------------------------
 * Name: TrayCtlNew
 *
 * Description:
 *
 *     
 *
 * Return value:
 *
 *     
 *
 */
static TrayCtl*
TrayCtlNew(SizeCtl* size_ctl)
{
    TrayCtl* me = (TrayCtl*)XtCalloc(1, sizeof(TrayCtl));

    me->size_ctl = size_ctl;

    return me;
}

/*
 * ------------------------------------------------------------------------
 * Name: TrayCtlDelete
 *
 * Description:
 *
 *     
 *
 * Return value:
 *
 *     
 *
 */
static void
TrayCtlDelete(TrayCtl* me)
{
    XtFree((char*)me);
}

/*
 * ------------------------------------------------------------------------
 * Name: TrayCtlCreate
 *
 * Description:
 *
 *     
 *
 * Return value:
 *
 *     
 *
 */
static void
TrayCtlCreate(TrayCtl* me,
	      Widget parent,
	      PdmOid default_input_tray,
	      PdmOid default_medium,
	      const PdmOidList* document_attrs_supported,
	      const PdmOidList* input_trays,
	      const PdmOidList* tray_sizes)
{
    Boolean supported;
    /*
     * save the trays and sizes list pointers
     */
    me->input_trays = input_trays;
    me->tray_sizes = tray_sizes;
    /*
     * determine if setting the input tray is supported
     */
    if(PdmOidListHasOid(document_attrs_supported,
			pdmoid_att_default_input_tray)
       &&
       PdmOidListCount(me->input_trays) > 0)
    {
	supported = True;
    }
    else
	supported = False;
    /*
     * set the initially selected list item
     */
    if(default_medium != pdmoid_none)
    {
	/*
	 * default-medium has precedence; set "Auto-select" (index 0)
	 */
	TrayCtlSetSelectedItem(me, 0);
    }
    else
    {
	int i = PdmOidListGetIndex(me->input_trays, default_input_tray);
	if(!supported || i == -1)
	    /*
	     * default-input-tray unsupported or not found in list; set
	     * "Auto-select" (index 0)
	     */
	    TrayCtlSetSelectedItem(me, 0);
	else
	    /*
	     * Select the list item corresponding to default-input-tray -
	     * ie. add 1 to the input trays list index since the option
	     * menu is always built with "Auto-select" as the first
	     * entry, followed by the input trays list entries.
	     */
	    TrayCtlSetSelectedItem(me, i+1);
    }
    /*
     * create the tray control window
     */
    TrayCtlCreateWindow(me, parent);
    /*
     * initialize the window components
     */
    TrayCtlCreatePulldown(me);
    /*
     * if tray selection is not supported, disable the control
     */
    if(!supported)
	TrayCtlDisable(me);
}

/*
 * ------------------------------------------------------------------------
 * Name: TrayCtlCreateWindow
 *
 * Description:
 *
 *     
 *
 * Return value:
 *
 *     
 *
 */
static void
TrayCtlCreateWindow(TrayCtl* me, Widget parent)
{
    Widget control_manager;
    
    /*
     * tray frame
     */
    me->widget = XtVaCreateManagedWidget("TrayCtlTop",
					 xmFrameWidgetClass,
					 parent,
					 XmNshadowType, XmSHADOW_ETCHED_IN,
					 NULL);
    /*
     * tray label
     */
    me->frame_label
	= XtVaCreateManagedWidget("TrayLabel",
				  xmLabelGadgetClass,
				  me->widget,
				  XmNframeChildType, XmFRAME_TITLE_CHILD,
				  NULL);
    /*
     * tray controls manager
     */
    control_manager =
	XtVaCreateManagedWidget("TrayControls",
				xmDrawingAreaWidgetClass,
				me->widget,
				XmNframeChildType, XmFRAME_WORKAREA_CHILD,
				NULL);
    XtAddCallback(control_manager, XmNresizeCallback, TrayCtlResizeCB, me);
    /*
     * trays option menu
     */
    me->option_menu = 
	XtVaCreateManagedWidget("TrayOptionMenu",
				xmRowColumnWidgetClass,
				control_manager,
				XmNmarginHeight, 0,
				XmNrowColumnType, XmMENU_OPTION,
				NULL);
}

/*
 * ------------------------------------------------------------------------
 * Name: TrayCtlDisable
 *
 * Description:
 *
 *     Disable the input tray controls.
 *
 * Return value:
 *
 *     None.
 *
 */
static void
TrayCtlDisable(TrayCtl* me)
{
    XtSetSensitive(me->frame_label, False);
    XtSetSensitive(me->option_menu, False);
}

/*
 * ------------------------------------------------------------------------
 * Name: TrayCtlResizeCB
 *
 * Description:
 *
 *     
 *
 * Return value:
 *
 *     None.
 *
 */
static void
TrayCtlResizeCB(Widget w, XtPointer client_data, XtPointer call_data)
{
    TrayCtl* me = (TrayCtl*)client_data;
    Dimension height, menu_h;
    Position menu_y;
    /*
     * get current layout info
     */
    XtVaGetValues(w, XmNheight, &height, NULL);
    XtVaGetValues(me->option_menu, XmNheight, &menu_h, NULL);
    /*
     * center vertically
     */
    menu_y = (height - menu_h) / 2;
    /*
     * set new position
     */
    XtVaSetValues(me->option_menu, XmNx, 0, XmNy, menu_y, NULL);
}

/*
 * ------------------------------------------------------------------------
 * Name: TrayCtlGetTray
 *
 * Description:
 *
 *     
 *
 * Return value:
 *
 *     
 *
 */
static PdmOid
TrayCtlGetTray(TrayCtl* me)
{
    if(me->selected_item == 0)
	return pdmoid_none;
    else
	return PdmOidListGetOid(me->input_trays, me->selected_item - 1);
}

/*
 * ------------------------------------------------------------------------
 * Name: TrayCtlCreatePulldown
 *
 * Description:
 *
 *     
 *
 * Return value:
 *
 *     
 *
 */
static void
TrayCtlCreatePulldown(TrayCtl* me)
{
    XmString* xmstr_list;
    int count;
    PdmOid oid;
    const char* str;
    int i;
    /*
     * create a list of XmStrings for the Auto-select item plus the list
     * of specific trays
     */
    count = 1 + PdmOidListCount(me->input_trays);
    xmstr_list = (XmString*)XtCalloc(count, sizeof(XmString));
    /*
     * set the "Auto-select" item as the first item
     */
    i = 0;
    xmstr_list[i++] = XmStringCreateLocalized((char*)PDM_MSG_TRAY_AUTO_SELECT);
    /*
     * add the input trays
     */
    for(; i < count; i++)
    {
	oid = PdmOidListGetOid(me->input_trays, i-1);
	str = DTPDM_GETMESSAGE(PdmOidMsgSet(oid),
			       PdmOidMsgNum(oid),
			       PdmOidDefaultMsg(oid));
	xmstr_list[i] = XmStringCreateLocalized((char*)str);
    }
    /*
     * create the option menu pulldown
     */
    CreateOptionMenuPulldown("TrayOptions",
			     me->option_menu,
			     TrayCtlSelectCB,
			     (XtPointer)me,
			     xmstr_list, count,
			     me->selected_item);
    /*
     * clean up
     */
    FreeXmStringList(xmstr_list, count);
}

/*
 * ------------------------------------------------------------------------
 * Name: TrayCtlSetSelectedItem
 *
 * Description:
 *
 *     
 *
 * Return value:
 *
 *     
 *
 */
static void
TrayCtlSetSelectedItem(TrayCtl* me, int selected_item)
{
    if(selected_item == -1)
	me->selected_item = 0;
    else
	me->selected_item = selected_item;
    /*
     * update the page size control accordingly
     */
    if(me->selected_item == 0)
	SizeCtlSetMultiSizeMode(me->size_ctl);
    else
	SizeCtlSetSingleSizeMode(me->size_ctl,
				 PdmOidListGetOid(me->tray_sizes,
						  me->selected_item - 1));
}

/*
 * ------------------------------------------------------------------------
 * Name: TrayCtlSelectCB
 *
 * Description:
 *
 *     
 *
 * Return value:
 *
 *     
 *
 */
static void
TrayCtlSelectCB(Widget w, XtPointer client_data, XtPointer call_data)
{
    TrayCtl* me;
    int selected_item = (int)client_data;

    XtVaGetValues(w, XmNuserData, &me, NULL);
    TrayCtlSetSelectedItem(me, selected_item);
}


/*
 * ------------------------------------------------------------------------
 * Name: SizeCtlNew
 *
 * Description:
 *
 *     
 *
 * Return value:
 *
 *     
 *
 */
static SizeCtl*
SizeCtlNew()
{
    SizeCtl* me = (SizeCtl*)XtCalloc(1, sizeof(SizeCtl));
    return me;
}

/*
 * ------------------------------------------------------------------------
 * Name: SizeCtlDelete
 *
 * Description:
 *
 *     
 *
 * Return value:
 *
 *     
 *
 */
static void
SizeCtlDelete(SizeCtl* me)
{
    if(me != (SizeCtl*)NULL)
    {
	if(me->sizes_ready_xmstr != (XmString*)NULL)
	    FreeXmStringList(me->sizes_ready_xmstr,
			     PdmOidListCount(me->sizes_ready));
	if(me->sizes_supported_xmstr != (XmString*)NULL)
	    FreeXmStringList(me->sizes_supported_xmstr,
			     PdmOidListCount(me->sizes_supported));
	PdmOidListDelete(me->sizes_ready);
	PdmOidListDelete(me->sizes_supported);
	XtFree((char*)me);
    }
}

/*
 * ------------------------------------------------------------------------
 * Name: SizeCtlStrColl
 *
 * Description:
 *
 *     An adaptor for using strcoll on SizeCtlSortRec's with qsort.
 *
 * Return value:
 *
 *     The return value for strcoll given the strings in the passed
 *     SizeCtlSortRec's.
 *
 */
static int
SizeCtlStrColl(const void* e1, const void* e2)
{
    const SizeCtlSortRec *r1 = e1, *r2 = e2;

    return strcoll(r1->str, r2->str);
}

/*
 * ------------------------------------------------------------------------
 * Name: SizeCtlSetSizeLists
 *
 * Description:
 *
 *     Creates a sorted XmStringTable and PdmOidList using
 *     the oids from the passed PdmOidLinkedList. The newly created lists
 *     are returned via the 'oids' and 'xmstrs' parms.
 *
 * Return value:
 *
 *     None.
 *
 */
static void
SizeCtlSetSizeLists(PdmOidList** oid_list,
		    XmStringTable* xmstrs,
		    PdmOidLinkedList* oid_ll)
{
    int count;

    count = PdmOidLinkedListCount(oid_ll);
    if(count == 0)
    {
	*oid_list = (PdmOidList*)NULL;
	*xmstrs = (XmStringTable)NULL;
    }
    else
    {	
	SizeCtlSortList sort_list;
	int i;
	PdmOid oid;
	/*
	 * sort the passed sizes
	 */
	sort_list = (SizeCtlSortList)XtCalloc(count, sizeof(SizeCtlSortRec));
	for(i = 0, oid = PdmOidLinkedListFirstOid(oid_ll);
	    i < count;
	    i++, oid = PdmOidLinkedListNextOid(oid_ll))
	{
	    sort_list[i].oid = oid;
	    sort_list[i].str = DTPDM_GETMESSAGE(PdmOidMsgSet(oid),
						PdmOidMsgNum(oid),
						PdmOidDefaultMsg(oid));
	}
	qsort(sort_list, count, sizeof(SizeCtlSortRec), SizeCtlStrColl);
	/*
	 * create the return lists of oids and XmStrings
	 */
	*oid_list = (PdmOidList*)XtCalloc(1, sizeof(PdmOidList));
	(*oid_list)->list = (PdmOid*)XtCalloc(count, sizeof(PdmOid));
	(*oid_list)->count = count;
	*xmstrs = (XmStringTable)XtCalloc(count, sizeof(XmString));
	for(i = 0; i < count; i++)
	{
	    ((*oid_list)->list)[i] = sort_list[i].oid;
	    (*xmstrs)[i] = XmStringCreateLocalized((char*)sort_list[i].str);
	}
	XtFree((char*)sort_list);
    }
}

/*
 * ------------------------------------------------------------------------
 * Name: SizeCtlCreate
 *
 * Description:
 *
 *     
 *
 * Return value:
 *
 *     
 *
 */
static void
SizeCtlCreate(SizeCtl* me,
	      Widget parent,
	      PdmOid default_medium,
	      const PdmOidList* document_attrs_supported,
	      const PdmOidList* tray_sizes,
	      PdmOidLinkedList* sizes_supported)
{
    Boolean supported;
    int i;
    PdmOid oid;
    PdmOidLinkedList* sizes_ready_ll;
    /*
     * initialize the selected size
     */
    me->selected_size = default_medium;
    /*
     * build the sizes ready list using unique elements of the tray sizes
     * list
     */
    sizes_ready_ll = PdmOidLinkedListNew();
    for(i = 0; i < PdmOidListCount(tray_sizes); i++)
    {
	oid = PdmOidListGetOid(tray_sizes, i);
	if(!PdmOidLinkedListHasOid(sizes_ready_ll, oid))
	{
	    PdmOidLinkedListAddOid(sizes_ready_ll, oid);
	}
    }
    /*
     * determine if setting the medium is supported
     */
    if(!PdmOidListHasOid(document_attrs_supported,
			 pdmoid_att_default_medium)
       ||
       PdmOidLinkedListCount(sizes_supported) == 0)
    {
	me->selected_list = SIZECTL_UNSUPPORTED;
    }
    else
    {
	/*
	 * default to the ready list
	 */
	me->selected_list = SIZECTL_SIZES_READY;
    }
    /*
     * create the XmString versions of the sizes lists
     */
    SizeCtlSetSizeLists(&me->sizes_ready, &me->sizes_ready_xmstr,
			sizes_ready_ll);
    PdmOidLinkedListDelete(sizes_ready_ll);
    SizeCtlSetSizeLists(&me->sizes_supported, &me->sizes_supported_xmstr,
			sizes_supported);
    /*
     * create the size control window
     */
    SizeCtlCreateWindow(me, parent);
    /*
     * update the control state
     */
    SizeCtlUpdate(me);
}

/*
 * ------------------------------------------------------------------------
 * Name: SizeCtlCreateWindow
 *
 * Description:
 *
 *     
 *
 * Return value:
 *
 *     
 *
 */
static void
SizeCtlCreateWindow(SizeCtl* me, Widget parent)
{
    Arg args[15];
    Cardinal n;
    Widget control_manager;
    Widget hscroll;
    XmString default_xmstr = (XmString)NULL;
    /*
     * page size frame
     */
    me->widget = XtVaCreateManagedWidget("PageSizeCtlTop",
					 xmFrameWidgetClass,
					 parent,
					 XmNshadowType, XmSHADOW_ETCHED_IN,
					 NULL);
    /*
     * page size label
     */
    me->frame_label
	= XtVaCreateManagedWidget("PageSizeLabel",
				  xmLabelGadgetClass,
				  me->widget,
				  XmNframeChildType, XmFRAME_TITLE_CHILD,
				  NULL);

    /*
     * page size controls manager
     */
    control_manager =
	XtVaCreateManagedWidget("PageSizeControls",
				xmDrawingAreaWidgetClass,
				me->widget,
				XmNframeChildType, XmFRAME_WORKAREA_CHILD,
				NULL);
    XtAddCallback(control_manager, XmNresizeCallback, SizeCtlResizeCB, me);
    /*
     * page sizes list box
     */
    n = 0;
    XtSetArg(args[n], XmNvisibleItemCount, 3); n++;
    XtSetArg(args[n], XmNscrollBarDisplayPolicy, XmSTATIC); n++;
    XtSetArg(args[n], XmNhorizontalScrollBar, NULL); n++;
    /*
     * set the supported sizes list in the list box, in order to lock
     * down the maximum dimensions - the list box does not support
     * changes to the list size policy after creation
     */
    if(me->selected_list == SIZECTL_UNSUPPORTED)
    {
	/*
	 * unsupported, use "Default"
	 */
	default_xmstr =
	    XmStringCreateLocalized((char*)PDM_MSG_SIZE_DEFAULT);
	XtSetArg(args[n], XmNitemCount, 1); n++;
	XtSetArg(args[n], XmNitems, &default_xmstr); n++;
    }
    else
    {
	XtSetArg(args[n], XmNitems, me->sizes_supported_xmstr); n++;
	XtSetArg(args[n], XmNitemCount,
		 PdmOidListCount(me->sizes_supported)); n++;
    }
    XtSetArg(args[n], XmNlistSizePolicy, XmCONSTANT); n++;

    me->list_box = XmCreateScrolledList(control_manager, "PageSizes", args, n);
    if((XmString)NULL != default_xmstr) XmStringFree(default_xmstr);
    XtAddCallback(me->list_box, XmNbrowseSelectionCallback,
		  SizeCtlSelectCB, (XtPointer)me);
    /*
     * hide the horizontal scroll bar, since the width of the list has
     * been set to accomodate the widest item.
     */
    hscroll = XtNameToWidget(XtParent(me->list_box), "*HorScrollBar");
    XtVaSetValues(hscroll,
		  XmNmappedWhenManaged, False,
		  XmNx, 0, XmNy, 0, XmNwidth, 1, XmNheight, 1,
		  NULL);
    /*
     * manage the scrolled list
     */
    XtManageChild(me->list_box);
    XtManageChild(XtParent(me->list_box));
    /*
     * loaded / allowed sizes radio buttons
     */
    me->radio_box = XtVaCreateWidget(
				     "LoadedAllowed",
				     xmRowColumnWidgetClass,
				     control_manager,
				     XmNradioBehavior, True,
				     NULL);
    me->ready_radio = XtVaCreateManagedWidget("button_0",
					      xmToggleButtonWidgetClass,
					      me->radio_box,
					      NULL);
    XtAddCallback(me->ready_radio, XmNvalueChangedCallback,
		  SizeCtlReadyChangedCB, (XtPointer)me);
    me->supported_radio = XtVaCreateManagedWidget("button_1",
					      xmToggleButtonWidgetClass,
					      me->radio_box,
					      NULL);
    XtAddCallback(me->supported_radio, XmNvalueChangedCallback,
		  SizeCtlSupportedChangedCB, (XtPointer)me);
    
    XtManageChild(me->radio_box);
}

/*
 * ------------------------------------------------------------------------
 * Name: SizeCtlUpdate
 *
 * Description:
 *
 *     Update the state of the size control
 *
 * Return value:
 *
 *     None.
 *
 */
static void
SizeCtlUpdate(SizeCtl* me)
{
    Boolean frame_label_sensitive = False;
    Boolean list_box_sensitive = False;
    Boolean list_box_traverse = False;
    Boolean ready_radio_sensitive = False;
    Boolean supported_radio_sensitive = False;
    /*
     * clear the current list
     */
    XmListDeleteAllItems(me->list_box);

    if(me->single_size_mode)
    {
	XmString item;
	PdmOid oid = me->selected_size;
	
	item =
	    XmStringCreateLocalized((char*)
				    DTPDM_GETMESSAGE(PdmOidMsgSet(oid),
						     PdmOidMsgNum(oid),
						     PdmOidDefaultMsg(oid)));
	XmListAddItem(me->list_box, item, 0);
	XmListSelectPos(me->list_box, 0, False);
	XmStringFree(item);

	if(me->selected_list != SIZECTL_UNSUPPORTED)
	{
	    frame_label_sensitive = True;
	    list_box_sensitive = True;
	    me->selected_list = SIZECTL_SIZES_READY;
	    XmToggleButtonSetState(me->ready_radio, True, False);
	    XmToggleButtonSetState(me->supported_radio, False, False);
	}
    }
    else
    {
	if(me->selected_list == SIZECTL_UNSUPPORTED)
	{
	    XmString default_item;

	    default_item =
	      XmStringCreateLocalized((char*)PDM_MSG_SIZE_DEFAULT);
	    XmListAddItem(me->list_box, default_item, 0);
	    XmListSelectPos(me->list_box, 0, False);
	    XmStringFree(default_item);
	}
	else
	{
	    XmString* list;
	    int count;
	    int pos;
	    /*
	     * set control sensitivities
	     */
	    frame_label_sensitive = True;
	    supported_radio_sensitive = True;
	    list_box_sensitive = True;
	    list_box_traverse = True;
	    if(PdmOidListCount(me->sizes_ready) > 0)
		ready_radio_sensitive = True;
	    /*
	     * check to see if the selected size is supported
	     */
	    if(!PdmOidListHasOid(me->sizes_supported, me->selected_size))
	    {
		/*
		 * the selected size is not supported
		 */
		if(PdmOidListCount(me->sizes_ready) > 0)
		{
		    /*
		     * use the 1st entry in the ready list
		     */
		    me->selected_size =
			PdmOidListGetOid(me->sizes_ready, 0);
		    me->selected_list = SIZECTL_SIZES_READY;
		}
		else
		{
		    /*
		     * the ready list is empty, use the 1st entry in the
		     * supported list
		     */
		    me->selected_size =
			PdmOidListGetOid(me->sizes_supported, 0);
		    me->selected_list = SIZECTL_SIZES_SUPPORTED;
		}
	    }
	    else if(me->selected_list == SIZECTL_SIZES_READY
		    &&
		    !PdmOidListHasOid(me->sizes_ready, me->selected_size))
	    {
		/*
		 * the selected size is not in the ready list; use the
		 * supported list instead
		 */
		me->selected_list = SIZECTL_SIZES_SUPPORTED;
	    }
	    /*
	     * set the appropriate radio button and update the list
	     */
	    if(me->selected_list == SIZECTL_SIZES_READY)
	    {
		XmToggleButtonSetState(me->ready_radio, True, False);
		XmToggleButtonSetState(me->supported_radio, False, False);

		list = me->sizes_ready_xmstr;
		count = PdmOidListCount(me->sizes_ready);
		pos = 1 + PdmOidListGetIndex(me->sizes_ready,
					     me->selected_size);
	    }
	    else
	    {
		XmToggleButtonSetState(me->supported_radio, True, False);
		XmToggleButtonSetState(me->ready_radio, False, False);

		list = me->sizes_supported_xmstr;
		count = PdmOidListCount(me->sizes_supported);
		pos = 1 + PdmOidListGetIndex(me->sizes_supported,
					     me->selected_size);
	    }
	    XmListAddItemsUnselected(me->list_box, list, count, 0);
	    SetListBoxSelection(me->list_box, pos);
	}
    }
    /*
     * update control sensitivities
     */
    XtSetSensitive(me->frame_label, frame_label_sensitive);
    XtSetSensitive(me->list_box, list_box_sensitive);
    XtVaSetValues(me->list_box, XmNtraversalOn, list_box_traverse, NULL);
    XtSetSensitive(me->ready_radio, ready_radio_sensitive);
    XtSetSensitive(me->supported_radio, supported_radio_sensitive);
}

/*
 * ------------------------------------------------------------------------
 * Name: SizeCtlResizeCB
 *
 * Description:
 *
 *     
 *
 * Return value:
 *
 *     None.
 *
 */
static void
SizeCtlResizeCB(Widget w, XtPointer client_data, XtPointer call_data)
{
    SizeCtl* me = (SizeCtl*)client_data;
    Dimension height, width, margin_width, new_width;
    Position list_x, list_y, radio_x, radio_y;
    Dimension list_h, list_w, radio_h, radio_w;
    Widget scrolled_list = XtParent(me->list_box);
    /*
     * get current layout info
     */
    XtVaGetValues(w, XmNheight, &height, XmNwidth, &width,
		  XmNmarginWidth, &margin_width,
		  NULL);
    XtVaGetValues(scrolled_list, XmNheight, &list_h, XmNwidth, &list_w, NULL);
    XtVaGetValues(me->radio_box, XmNheight, &radio_h, XmNwidth, &radio_w,
		  NULL);
    /*
     * position controls side by side
     */
    list_x = margin_width;
    radio_x = list_x + list_w;
    /*
     * center vertically
     */
    list_y = (height - list_h) / 2;
    radio_y = (height - radio_h) / 2;
    /*
     * set new positions
     */
    XtVaSetValues(scrolled_list, XmNx, list_x, XmNy, list_y, NULL);
    XtVaSetValues(me->radio_box, XmNx, radio_x, XmNy, radio_y, NULL);
    /*
     * update drawing area width and height
     */
    new_width = list_x + list_w + radio_w;
    if(new_width > width)
	XtVaSetValues(w, XmNwidth, new_width, NULL);
}

/*
 * ------------------------------------------------------------------------
 * Name: SizeCtlSetSingleSizeMode
 *
 * Description:
 *
 *     
 *
 * Return value:
 *
 *     
 *
 */
static void
SizeCtlSetSingleSizeMode(SizeCtl* me, PdmOid size)
{
    if(size != pdmoid_none)
    {
	me->single_size_mode = True;
	me->selected_size = size;
	SizeCtlUpdate(me);
    }
}

/*
 * ------------------------------------------------------------------------
 * Name: SizeCtlSetMultiSizeMode
 *
 * Description:
 *
 *     
 *
 * Return value:
 *
 *     
 *
 */
static void
SizeCtlSetMultiSizeMode(SizeCtl* me)
{
    me->single_size_mode = False;
    SizeCtlUpdate(me);
}



/*
 * ------------------------------------------------------------------------
 * Name: SizeCtlGetSize
 *
 * Description:
 *
 *     
 *
 * Return value:
 *
 *     
 *
 */
static PdmOid
SizeCtlGetSize(SizeCtl* me)
{
    if(me->single_size_mode || me->selected_list == SIZECTL_UNSUPPORTED)
	return pdmoid_none;
    else
	return me->selected_size;
}

/*
 * ------------------------------------------------------------------------
 * Name: SizeCtlReadyChangedCB
 *
 * Description:
 *
 *     
 *
 * Return value:
 *
 *     
 *
 */
static void
SizeCtlReadyChangedCB(Widget w,
		      XtPointer client_data,
		      XtPointer call_data)
{
    XmToggleButtonCallbackStruct* cbs =
	(XmToggleButtonCallbackStruct*)call_data;

    if(cbs->set)
    {
	SizeCtl* me = (SizeCtl*)client_data;
	me->selected_list = SIZECTL_SIZES_READY;
	if(!PdmOidListHasOid(me->sizes_ready, me->selected_size))
	    me->selected_size = pdmoid_none;
	SizeCtlUpdate(me);
    }
}

/*
 * ------------------------------------------------------------------------
 * Name: SizeCtlSupportedChangedCB
 *
 * Description:
 *
 *     
 *
 * Return value:
 *
 *     
 *
 */
static void
SizeCtlSupportedChangedCB(Widget w,
			  XtPointer client_data,
			  XtPointer call_data)
{
    XmToggleButtonCallbackStruct* cbs =
	(XmToggleButtonCallbackStruct*)call_data;

    if(cbs->set)
    {
	SizeCtl* me = (SizeCtl*)client_data;
	me->selected_list = SIZECTL_SIZES_SUPPORTED;
	SizeCtlUpdate(me);
    }
}

/*
 * ------------------------------------------------------------------------
 * Name: SizeCtlSelectCB
 *
 * Description:
 *
 *     
 *
 * Return value:
 *
 *     
 *
 */
static void
SizeCtlSelectCB(Widget w,
		XtPointer client_data,
		XtPointer call_data)
{
    XmListCallbackStruct* cbs = (XmListCallbackStruct*)call_data;
    SizeCtl* me = (SizeCtl*)client_data;

    if(!me->single_size_mode)
    {
	switch(me->selected_list)
	{
	case SIZECTL_SIZES_READY:
	    me->selected_size =
		PdmOidListGetOid(me->sizes_ready, cbs->item_position - 1);
	    break;
	case SIZECTL_SIZES_SUPPORTED:
	    me->selected_size =
		PdmOidListGetOid(me->sizes_supported, cbs->item_position - 1);
	    break;
	}
    }
}

/*
 * ------------------------------------------------------------------------
 * Name: CreateOptionMenuPulldown
 *
 * Description:
 *
 *     
 *
 * Return value:
 *
 *     
 *
 */
static Widget
CreateOptionMenuPulldown(String pulldown_name,
			 Widget option_menu,
			 XtCallbackProc activate_proc,
			 XtPointer user_data,
			 XmString* item_list,
			 int item_count,
			 int initial_item)
{
    Widget cascade_button;
    Widget button;
    Widget initial_selected = (Widget)NULL;
    Widget pulldown;
    char button_name[48];
    int i;

    pulldown =
	XmCreatePulldownMenu(XtParent(option_menu), pulldown_name, NULL, 0);

    for(i = 0; i < item_count; i++)
    {
	sprintf(button_name, "button_%d", i);
	button = XtVaCreateManagedWidget(button_name,
					 xmPushButtonGadgetClass,
					 pulldown,
					 XmNuserData, user_data,
					 XmNlabelString, item_list[i],
					 NULL);
	if(activate_proc != (XtCallbackProc)NULL)
	    XtAddCallback(button, XmNactivateCallback,
			  activate_proc, (XtPointer)i);
	if(i == initial_item)
	    initial_selected = button;
    }
    XtVaSetValues(option_menu,
		  XmNsubMenuId, pulldown,
		  XmNmenuHistory, initial_selected,
		  NULL);

    return pulldown;
}

/*
 * ------------------------------------------------------------------------
 * Name: CreateXmStringsFromOidList
 *
 * Description:
 *
 *     
 *
 * Return value:
 *
 *     
 *
 */
static XmString*
CreateXmStringsFromOidList(const PdmOidList* list)
{
    int count = PdmOidListCount(list);
    XmString* xmstr_list = (XmString*)NULL;
    
    if(count > 0)
    {
	int i;
	PdmOid oid;
	const char* str;
	/*
	 * build a list of Xm strings for the passed PdmOidList
	 */
	xmstr_list = (XmString*)XtCalloc(count, sizeof(XmString));
	for(i = 0; i < count; i++)
	{
	    oid = PdmOidListGetOid(list, i);
	    str = DTPDM_GETMESSAGE(PdmOidMsgSet(oid),
				   PdmOidMsgNum(oid),
				   PdmOidDefaultMsg(oid));
	    xmstr_list[i] = XmStringCreateLocalized((char*)str);
	}
    }
    return xmstr_list;
}

/*
 * ------------------------------------------------------------------------
 * Name: FreeXmStringList
 *
 * Description:
 *
 *     
 *
 * Return value:
 *
 *     
 *
 */
static void
FreeXmStringList(XmString* list, int count)
{
    int i;
    
    for(i = 0; i < count; i++)
	XmStringFree(list[i]);
    XtFree((char*)list);
}

/*
 * ------------------------------------------------------------------------
 * Name: IconMaxDimension
 *
 * Description:
 *
 *     
 *
 * Return value:
 *
 *     
 *
 */
static void
IconMaxDimension(Widget icon_label,
		 PdmPixmapList* pixmap_list, int i,
		 Dimension* max_width, Dimension* max_height)
{
    Dimension width, height;
    /*
     * set the pixmap in the label
     */
    XtVaSetValues(icon_label,
		  XmNlabelPixmap, PdmPixmapListGetPixmap(pixmap_list, i),
		  NULL);
    /*
     * get the label dimensions
     */
    XtVaGetValues(icon_label,
		  XmNwidth, &width,
		  XmNheight, &height,
		  NULL);
    /*
     * update the max dimensions
     */
    if(width > *max_width)
	*max_width = width;
    if(height > *max_height)
	*max_height = height;
}

/*
 * ------------------------------------------------------------------------
 * Name: PdmPixmapListNew
 *
 * Description:
 *
 *     
 *
 * Return value:
 *
 *     
 *
 */
static PdmPixmapList*
PdmPixmapListNew(int count,
		 Widget w,
		 Boolean stipple)
{
    PdmPixmapList* me = (PdmPixmapList*)XtCalloc(1, sizeof(PdmPixmapList));

    me->pixmap = (Pixmap*)XtCalloc(count, sizeof(Pixmap));
    me->image_name = (char**)XtCalloc(count, sizeof(char*));
    me->screen = XtScreen(w);
    if(stipple)
    {
	Widget xmscr = XmGetXmScreen(me->screen);
	XtVaGetValues(xmscr, XmNinsensitiveStippleBitmap, &me->stipple, NULL);
    }
    else
    {
	me->stipple = None;
    }
    XtVaGetValues(w,
		  XmNforeground, &me->foreground,
		  XmNbackground, &me->background,
		  NULL);

    return me;
}

/*
 * ------------------------------------------------------------------------
 * Name: PdmPixmapListDelete
 *
 * Description:
 *
 *     
 *
 * Return value:
 *
 *     
 *
 */
static void
PdmPixmapListDelete(PdmPixmapList* me)
{
    if(me != (PdmPixmapList*)NULL)
    {
	int i;
	for(i = 0; i < PdmPixmapListCount(me); i++)
	{
	    if(me->pixmap[i] != (Pixmap)NULL)
	    {
		if(me->stipple == None)
		{
		    if(me->pixmap[i] != None)
			XmDestroyPixmap(me->screen, me->pixmap[i]);
		}
		else
		{
		    if(me->pixmap[i] != None)
		    	XFreePixmap(DisplayOfScreen(me->screen),
				    me->pixmap[i]);
		}
		XtFree((char*)me->image_name[i]);
	    }
	}
	XtFree((char*)me->pixmap);
	XtFree((char*)me->image_name);
	XtFree((char*)me);
    }
}

/*
 * ------------------------------------------------------------------------
 * Name: PdmPixmapSetImageName
 *
 * Description:
 *
 *     
 *
 * Return value:
 *
 *     
 *
 */
static void
PdmPixmapListSetImageName(PdmPixmapList* me,
			  const char* image_name,
			  int i)
{
    me->image_name[i] = XtNewString(image_name);
}

/*
 * ------------------------------------------------------------------------
 * Name: PdmPixmapListGetPixmap
 *
 * Description:
 *
 *     
 *
 * Return value:
 *
 *     
 *
 */
static Pixmap
PdmPixmapListGetPixmap(PdmPixmapList* me, int i)
{
    if(me->pixmap[i] == None)
    {
	me->pixmap[i] = XmGetPixmap(me->screen, me->image_name[i],
				    me->foreground, me->background);
	if(me->pixmap[i] == XmUNSPECIFIED_PIXMAP)
	{
	    me->pixmap[i] == None;
	}
	else if(me->stipple != None)
	{
	    Pixmap xmpixmap = me->pixmap[i];
	    Display* display = DisplayOfScreen(me->screen);
	    Window root;
	    int x, y;
	    unsigned int width, height, border_width, depth;
	    GC gc;
	    XGCValues values;
	    /*
             * make a copy of the pixmap cached by motif
	     */
	    XGetGeometry(display, xmpixmap, &root, &x, &y,
			 &width, &height, &border_width, &depth);
	    me->pixmap[i] =
		XCreatePixmap(display, xmpixmap, width, height, depth);
	    gc = XCreateGC(display, me->pixmap[i], (unsigned long)0, &values);
	    XCopyArea(display, xmpixmap, me->pixmap[i], gc,
		      0, 0, width, height, 0, 0);
	    /*
             * stipple it
	     */
	    XSetFillStyle(display, gc, FillStippled);
	    XSetStipple(display, gc, me->stipple);
	    XFillRectangle(display, me->pixmap[i], gc, 0, 0, width, height);
	    /*
             * clean up
	     */
	    XFreeGC(display, gc);	
	    XmDestroyPixmap(me->screen, xmpixmap);
	}
    }
    return me->pixmap[i];
}

/*
 * ------------------------------------------------------------------------
 * Name: SetListBoxSelection
 *
 * Description:
 *
 *
 * Return value:
 *
 *     None.
 *
 */
static void
SetListBoxSelection(Widget list_box, int position)
{
    int visible_item_count;
    int item_count;
    int middle_offset;
    int first_visible_pos;
    /*
     * Scroll the list, making the item at the indicated position
     * visible in the center of the list box, and make
     * it the initial selection.
     */
    XtVaGetValues(list_box,
		  XmNitemCount, &item_count,
		  XmNvisibleItemCount, &visible_item_count,
		  NULL);

    if(item_count > visible_item_count)
    {
	middle_offset = (visible_item_count+1) / 2;
	if(position > middle_offset)
	    if(position > item_count - middle_offset)
		first_visible_pos = item_count - visible_item_count +1;
	    else
		first_visible_pos = position - middle_offset + 1;
	else
	    first_visible_pos = 1;
	XmListSetPos(list_box, first_visible_pos);
    }

    if(position > 0)
	XmListSelectPos(list_box, position, False);
    else
	XmListDeselectAllItems(list_box);
}

/*
 * ------------------------------------------------------------------------
 * Name: GetIconFileName
 *
 * Description:
 *
 *
 * Return value:
 *
 *     None.
 *
 */
static String
GetIconFileName(Widget w, String resource_name)
{
    String icon_file_name;
    XtResource res_struct;

    res_struct.resource_name = resource_name;
    res_struct.resource_class = resource_name;
    res_struct.resource_type =  XmRString;
    res_struct.resource_size = sizeof(String);
    res_struct.resource_offset = 0;
    res_struct.default_type = XmRImmediate;
    res_struct.default_addr = (XtPointer)NULL;
    XtGetApplicationResources(w, (XtPointer)&icon_file_name,
	 		      &res_struct, 1, (ArgList)NULL, 0);

    return icon_file_name;
}
