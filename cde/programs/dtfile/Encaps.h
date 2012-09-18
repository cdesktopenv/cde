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
/* $XConsortium: Encaps.h /main/4 1995/11/02 14:36:49 rswiston $ */
/************************************<+>*************************************
 ****************************************************************************
 *
 *   FILE:           Encaps.h
 *
 *   COMPONENT_NAME: Desktop File Manager
 *
 *   DESCRIPTION:    Public include file for the dialog encapsulation functions.
 *
 *   (c) Copyright 1993, 1994, 1995 Hewlett-Packard Company
 *   (c) Copyright 1993, 1994, 1995 International Business Machines Corp.
 *   (c) Copyright 1993, 1994, 1995 Sun Microsystems, Inc.
 *   (c) Copyright 1993, 1994, 1995 Novell, Inc.
 *
 ****************************************************************************
 ************************************<+>*************************************/

#ifndef _Encapsualte_h
#define _Encapsulate_h


/* Timer globals */
extern int initialTimeoutLength;
extern int activeTimeoutLength;
extern int idleTimeoutLength;

/* Dialog positioning offset value */
extern int topPositionOffset;



/*
 *  Common dialog instance data.
 */

typedef struct
{
   Boolean   displayed;
   Position  x;
   Position  y;
   Dimension width;
   Dimension height;
} DialogInstanceData, * DialogInstanceDataPtr;


/*
 *  DialogData contains the dialog type specific data shared between
 *  the dialog and the application.
 */

typedef struct
{
   int     type;
   XtPointer data;
} DialogData;


/*
 *  The following typedefs are the function definitions used by the
 *  Dialog Class structure.
 */

typedef void (*DialogCreateProc)(
    Display * display,
    Widget    parent,
    Widget  * shell,
    XtPointer * dialog
);

typedef void (*DialogInstallChangeProc)(
    XtPointer        dialog,
    XtCallbackProc callback,
    XtPointer        client_data
);

typedef void (*DialogInstallCloseProc)(
    XtPointer        dialog,
    XtCallbackProc callback,
    XtPointer        client_data
);

typedef void (*DialogDestroyProc)(
    XtPointer dialog
);

typedef XtPointer (*DialogGetValuesProc)(
    XtPointer dialog
);

typedef XtPointer (*DialogGetDefaultValuesProc)(
    void
);


typedef XtPointer (*DialogGetResourceValuesProc)(
    XrmDatabase    data_base,
    char        ** name_list
);

typedef void (*DialogSetValuesProc)(
    XtPointer     dialog,
    XtPointer     values
);
typedef void (*DialogWriteResourceValuesProc)(
    DialogData *  values,
    int           fd,
    char       ** name_list
);

typedef void (*DialogFreeValuesProc)(
    XtPointer     values
);

typedef void (*DialogSetFocusProc)(
    XtPointer     dialog,
    XtPointer     values
);


typedef void (*DialogMapWindowProc)(
    Widget    parent,
    XtPointer  dialog
);


/*
 *  Resource write proc and structure used for dialog resources. 
 */

typedef void (*WriteResourceProc)();
/*  XtPointer resource_val;   */

typedef struct 
{
   String  name;
   String  type;
   int     size;
   int     offset;
   XtPointer default_value;
   WriteResourceProc write_resource;   
} DialogResource;


/*
 *  The Dialog Class structure.
 */

typedef struct
{
   DialogResource              * resources;
   int                           resource_count;
   DialogCreateProc              create;
   DialogInstallChangeProc       install_change_callback;
   DialogInstallCloseProc        install_close_callback;
   DialogDestroyProc             destroy;
   DialogGetValuesProc           get_values;
   DialogGetDefaultValuesProc    get_default_values;
   DialogGetResourceValuesProc   get_resource_values;
   DialogSetValuesProc           set_values;
   DialogWriteResourceValuesProc write_resource_values;
   DialogFreeValuesProc          free_values;
   DialogMapWindowProc           map;
   DialogSetFocusProc            set_focus;
} DialogClass;



/*
 *  Procedure types used by the application for functions to be passed
 *  to the encapsulation mechanism.
 */

typedef void (*DialogChangedProc)(
    XtPointer      change_data,
    DialogData * old_data,
    DialogData * new_data,
    XtPointer      call_data
);

typedef void (*DialogClosedProc)(
    XtPointer      close_data,
    DialogData * old_data,
    DialogData * new_data
);



/********    Public Function Declarations    ********/

extern void _DtInitializeEncapsulation( 
                        Display *display,
                        char *name,
                        char *class) ;
extern int _DtInstallDialog( 
                        DialogClass *dialog_class,
                        Boolean cache,
                        Boolean destroyPopups) ;
extern DialogData * _DtGetDialogData( 
                        DialogData *dialog_data) ;
extern DialogData * _DtGetDefaultDialogData( 
                        int dialog_type) ;
extern DialogData * _DtGetResourceDialogData( 
                        int dialog_type,
                        XrmDatabase data_base,
                        char **name_list) ;
extern void _DtShowDialog( 
                        Widget parent,
                        Widget map_parent,
                        XtPointer top_rec,
                        DialogData *dialog_data,
                        DialogChangedProc change_proc,
                        XtPointer change_data,
                        DialogClosedProc close_proc,
                        XtPointer close_data,
                        char *workspaces,
                        Boolean iconify_state,
                        Boolean ignoreCache,
                        char * title,
                        XClassHint * classHints) ;
extern void _DtHideDialog( 
                        DialogData *dialog_data,
                        Boolean call_callbacks) ;
extern Widget _DtGetDialogShell( 
                        DialogData *dialog_data) ;
extern XtPointer _DtGetDialogInstance( 
                        DialogData *dialog_data) ;
extern DialogData * _DtGetInstanceData( 
                        XtPointer instance) ;
extern Boolean _DtIsDialogShowing( 
                        DialogData *dialog_data) ;
extern void _DtWriteDialogData( 
                        DialogData *dialog_data,
                        int fd,
                        char **name_list) ;
extern void _DtFreeDialogData( 
                        DialogData *dialog_data) ;
extern void _DtDialogGetResources( 
                        XrmDatabase database,
                        char **name_list,
                        char *dialog_name,
                        char *base,
                        DialogResource *resources,
                        int resource_count) ;
extern void _DtDialogPutResources( 
                        int fd,
                        char **name_list,
                        char *dialog_name,
                        char *base,
                        DialogResource *resources,
                        int resource_count) ;
extern void _DtIntToString( 
                        int fd,
                        int *value,
                        char *out_buf) ;
extern void _DtShortToString( 
                        int fd,
                        short *value,
                        char *out_buf) ;
extern void _DtPositionToString( 
                        int fd,
                        Position *value,
                        char *out_buf) ;
extern void _DtDimensionToString( 
                        int fd,
                        Dimension *value,
                        char *out_buf) ;
extern void _DtBooleanToString( 
                        int fd,
                        Boolean *value,
                        char *out_buf) ;
extern void _DtXmStringToString( 
                        int fd,
                        XmString *value,
                        char *out_buf) ;
extern void _DtXmStringTableToString( 
                        int fd,
                        XmStringTable *value,
                        char *out_buf) ;
extern void _DtStringToString( 
                        int fd,
                        char **value,
                        char *out_buf) ;
extern void _DtmapCB(
                        Widget w,
                        XtPointer client_data,
                        XtPointer call_data) ;
extern void _DtEncapSetWorkSpaceHints( 
                        Widget shell,
                        char *workspaces) ;
extern void _DtBuildDialog(
                        Widget parent,
                        Widget map_parent,
                        XtPointer top_rec,
                        DialogData *dialog_data,
                        DialogChangedProc change_proc,
                        XtPointer change_data,
                        DialogClosedProc close_proc,
                        XtPointer close_data,
                        char *workspaces,
                        Boolean iconify_state,
                        Boolean ignoreCache,
                        char * title,
                        XClassHint * classHints ) ;
extern void _DtShowBuiltDialog(
                        Widget parent,
                        Widget map_parent,
                        DialogData *dialog_data,
                        char *workspaces,
                        Boolean iconify_state,
                        XClassHint * classHints ) ;

extern void _DtChangeTo(
                        XtPointer client_data,
                        char  *directory) ;

extern void _DtFreeDialog(
                        DialogData *dialog_data);
/********    End Public Function Declarations    ********/

#endif /* _Encapsulate_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
