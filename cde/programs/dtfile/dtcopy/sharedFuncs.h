/* $XConsortium: sharedFuncs.h /main/4 1995/11/02 14:48:35 rswiston $ */
/************************************<+>*************************************
 ****************************************************************************
 *
 *   FILE:           sharedFuncs.h
 *
 *
 *   DESCRIPTION:    Header file used in copydialog.c,errordialog.c,
 *                   main_dtcopy.c,overwrtdialog.c,sharedFuncs.c,utils.c
 *
 *   (c) Copyright 1993, 1994, 1995 Hewlett-Packard Company
 *   (c) Copyright 1993, 1994, 1995 International Business Machines Corp.
 *   (c) Copyright 1993, 1994, 1995 Sun Microsystems, Inc.
 *   (c) Copyright 1993, 1994, 1995 Novell, Inc.
 *
 ****************************************************************************
 ************************************<+>*************************************/

#ifndef SHAREDFUNCS_H
#define SHAREDFUNCS_H

/* Includes */

#include <X11/Intrinsic.h>

/* Macros */

#define TIGHTNESS 20   /* button spacing in Create_Action_Area */

#define MAX_PATH  1024   /* Maximum path length */

/* Types  */

typedef struct {
        char      *label;         /* PushButton's default Label */
        int       msg_set;        /* set from which PushButton's Label is taken */
        int       msg_num;        /* message number of PushButton's Label */
        void      (*callback)();  /* pointer to callback routine */
        XtPointer data;           /* client data for callback routine */
} ActionAreaItem;

typedef struct {
        int            numActions;
        int            defaultAction;  /* index in actionList[], >=0, <numActions */
        ActionAreaItem *actionList;
} ActionAreaDefn;


/* Global Variables */

/* External Routines */

extern int       auto_rename(const String path);
extern void      generate_NewPath(String newPath, String oldPath);
extern void      split_path(const String path, String folder, String object);
extern String    build_path(const String folder, const String object);
extern XImage  * CreateDefaultImage(
                                    Display *display,
                                    char *bits,
                                    unsigned int width,
                                    unsigned int height );
extern void      ImageInitialize(Display *display);
extern Widget    Create_Action_Area(
                                    Widget            parent_widget,
                                    ActionAreaDefn    actions,
                                    Widget          * pushbutton_array);
extern void      CheckDeleteAccess(
                                    XtAppContext app_context,
                                    int delay,
                                    Boolean checkPerms,
                                    Boolean move,
                                    char *source_name);
extern void      TimeoutHandler(
                                    XtPointer client_data,
                                    XtIntervalId *id);



#endif /* SHAREDFUNCS_H */
