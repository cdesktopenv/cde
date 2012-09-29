/*
 *
 * PRIVATE MOTIF FUNCTIONS
 * Declarations extracted from Open Motif header files
 *
 */

#include <Xm/TextStrSoP.h>
#ifdef __cplusplus
extern "C" {
#endif
/* Extracted from BaseClassI.h */
extern void _XmPushWidgetExtData(
                        Widget widget,
                        XmWidgetExtData data,
#if NeedWidePrototypes
                        unsigned int extType) ;
#else
                        unsigned char extType) ;
#endif /* NeedWidePrototypes */
extern void _XmPopWidgetExtData(
                        Widget widget,
                        XmWidgetExtData *dataRtn,
#if NeedWidePrototypes
                        unsigned int extType) ;
#else
                        unsigned char extType) ;
#endif /* NeedWidePrototypes */
extern XmWidgetExtData _XmGetWidgetExtData(
                        Widget widget,
#if NeedWidePrototypes
                        unsigned int extType) ;
#else
                        unsigned char extType) ;
#endif /* NeedWidePrototypes */
/* Extracted from IconFileP.h */
extern void XmeFlushIconFileCache(String	path);
/* Extracted from MenuStateI.h */
extern void _XmSetInDragMode(
                        Widget widget,
#if NeedWidePrototypes
                        int mode) ;
#else
                        Boolean mode) ;
#endif /* NeedWidePrototypes */
/* Extracted from MenuUtilI.h */
extern int _XmGrabPointer(
                        Widget widget,
                        int owner_events,
                        unsigned int event_mask,
                        int pointer_mode,
                        int keyboard_mode,
                        Window confine_to,
                        Cursor cursor,
                        Time time) ;
/* Extracted from RCMenuI.h */
extern void _XmGetActiveTopLevelMenu(
				     Widget wid,
				     Widget *rwid);
/* Extracted from SyntheticI.h */
extern void _XmExtGetValuesHook(
                        Widget w,
                        ArgList args,
                        Cardinal *num_args) ;
extern void _XmGadgetImportSecondaryArgs(
                        Widget w,
                        ArgList args,
                        Cardinal *num_args) ;
/* Extracted from TextFI.h */
extern Boolean _XmTextFieldSetDestination(
                        Widget w,
                        XmTextPosition position,
                        Time set_time) ;
/* Extracted from TextStrSoI.h */
extern char  * _XmStringSourceGetString(XmTextWidget tw,
				        XmTextPosition from,
				        XmTextPosition to,
#if NeedWidePrototypes
				        int want_wchar);
#else
                                        Boolean want_wchar);
#endif /* NeedWidePrototypes */
/* Extracted from TravActI.h */
extern void _XmEnterGadget(
                        Widget wid,
                        XEvent *event,
                        String *params,
                        Cardinal *num_params) ;
extern void _XmLeaveGadget(
                        Widget wid,
                        XEvent *event,
                        String *params,
                        Cardinal *num_params) ;
extern void _XmFocusInGadget(
                        Widget wid,
                        XEvent *event,
                        String *params,
                        Cardinal *num_params) ;
extern void _XmFocusOutGadget(
                        Widget wid,
                        XEvent *event,
                        String *params,
                        Cardinal *num_params) ;
/* Extracted from TraversalI.h */
extern Boolean _XmFocusIsHere(
                        Widget w) ;
extern Boolean _XmShellIsExclusive(
                        Widget wid) ;
/* Extracted from UniqueEvnI.h */
extern Boolean _XmIsEventUnique(XEvent *event) ;
extern void _XmRecordEvent(XEvent *event) ;
/* Extracted from VendorSI.h */
extern void _XmAddGrab(
                        Widget wid,
#if NeedWidePrototypes
                        int exclusive,
                        int spring_loaded) ;
#else
                        Boolean exclusive,
                        Boolean spring_loaded) ;
#endif /* NeedWidePrototypes */
extern void _XmRemoveGrab(
                        Widget wid) ;
/* Extracted from XmI.h */
extern void _XmSocorro(
                        Widget w,
                        XEvent *event,
                        String *params,
                        Cardinal *num_params) ;
extern void _XmClearShadowType(
                        Widget w,
#if NeedWidePrototypes
                        int old_width,
                        int old_height,
                        int old_shadow_thickness,
                        int old_highlight_thickness) ;
#else
                        Dimension old_width,
                        Dimension old_height,
                        Dimension old_shadow_thickness,
                        Dimension old_highlight_thickness) ;
#endif /* NeedWidePrototypes */
/* Extracted from XmStringI.h */
extern XtPointer _XmStringUngenerate (XmString string,
			XmStringTag tag,
			XmTextType tag_type,
			XmTextType output_type);
#ifdef __cplusplus
}
#endif
/*************************************************************
 * END OF EXTRACTED DATA
 *************************************************************/
