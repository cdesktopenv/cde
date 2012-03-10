/* static char rcsid[] = 
	"$XConsortium: PanelS.c /main/4 1995/11/01 11:27:29 rswiston $";
*/
/**---------------------------------------------------------------------
***	
***	file:		PanelS.c
***
***	project:	MotifPlus Widgets
***
***	description:	Source code for DtPanelShell class.
***	
***
***			(c) Copyright 1992 by Hewlett-Packard Company.
***
***
***-------------------------------------------------------------------*/


/*-------------------------------------------------------------
**	Include Files
*/

#include <Xm/XmP.h>

#include <X11/Xatom.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>
#include <X11/ShellP.h>
#include <Dt/DtStrDefs.h>

#include "PanelSP.h"


/********    Public Function Declarations    ********/

/********    End Public Function Declarations    ********/

/********    Static Function Declarations    ********/


/********    End Static Function Declarations    ********/



/*-------------------------------------------------------------
**	Resource List
*/
#define R_Offset(field) \
	XtOffset (DtPanelShellWidget, panel.field)

static XtResource panelResources[]=
    {
	{
	XmNresolution,
	XmCDimension, XmRDimension, sizeof (Dimension),
	R_Offset (resolution), XmRImmediate, (XtPointer) 0
	},
	{
	XmNhighResFontList,
	XmCHighResFontList, XmRFontList, sizeof (XmFontList),
	R_Offset (high_res_font_list), XmRString, (XtPointer) "fixed"
	},
	{
	XmNmediumResFontList,
	XmCMediumResFontList, XmRFontList, sizeof (XmFontList),
	R_Offset (medium_res_font_list), XmRString, (XtPointer) "fixed"
	},
	{
	XmNlowResFontList,
	XmCLowResFontList, XmRFontList, sizeof (XmFontList),
	R_Offset (low_res_font_list), XmRString, (XtPointer) "fixed"
	}
    };



/*-------------------------------------------------------------
**	Class Record
*/

static CompositeClassExtensionRec compositeClassExtension =
    {
    /* next_extension	*/	NULL,
    /* record_type	*/	NULLQUARK,
    /* version		*/	XtCompositeExtensionVersion,
    /* record_size	*/	sizeof(CompositeClassExtensionRec),
    /* accepts_objects	*/	TRUE
    };

DtPanelShellClassRec dtPanelShellClassRec =
  {
/*	Core Part
 */
    {
    /* superclass         */    (WidgetClass) &topLevelShellClassRec,
    /* class_name         */    "FrontPanel",
    /* size               */    sizeof(DtPanelShellRec),
    /* Class Initializer  */	NULL,
    /* class_part_initialize*/	NULL,
    /* Class init'ed ?    */	FALSE,
    /* initialize         */    NULL,
    /* initialize_notify  */	NULL,		
    /* realize            */    XtInheritRealize,
    /* actions            */    NULL,
    /* num_actions        */    0,
    /* resources          */    panelResources,
    /* resource_count     */	XtNumber(panelResources),
    /* xrm_class          */    NULLQUARK,
    /* compress_motion    */    FALSE,
    /* compress_exposure  */    XtExposeCompressMaximal,
    /* compress_enterleave*/    FALSE,
    /* visible_interest   */    FALSE,
    /* destroy            */    NULL,
    /* resize             */    XtInheritResize,
    /* expose             */    NULL,
    /* set_values         */    NULL,
    /* set_values_hook    */	NULL,			
    /* set_values_almost  */	XtInheritSetValuesAlmost,
    /* get_values_hook    */	NULL,			
    /* accept_focus       */    NULL,
    /* intrinsics version */	XtVersion,
    /* callback offsets   */    NULL,
    /* tm_table		  */	XtInheritTranslations,
    /* query_geometry	  */	NULL,
    /* display_accelerator*/	NULL,
    /* extension	  */	NULL
    },
/*	Composite Part
 */
    {
    /* geometry_manager   */    XtInheritGeometryManager,
    /* change_managed     */    XtInheritChangeManaged,
    /* insert_child	  */	XtInheritInsertChild,
    /* delete_child	  */	XtInheritDeleteChild,
    /* extension	  */	(XtPointer)&compositeClassExtension
    },
/*	Constraint Part
 */
    {
    /* extension	  */	NULL
    },
    {
    /* extension	  */	NULL
    },
    {
    /* extension	  */	NULL
    },
    {
    /* extension	  */	NULL
    }
  };

WidgetClass dtPanelShellWidgetClass = (WidgetClass) &dtPanelShellClassRec;



/*-------------------------------------------------------------
**	Private Procs
**-------------------------------------------------------------
*/


/*-------------------------------------------------------------
**	Core Procs
**-------------------------------------------------------------
*/

