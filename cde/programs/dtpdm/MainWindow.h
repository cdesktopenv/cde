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
/* $XConsortium: MainWindow.h /main/4 1996/08/12 18:42:19 cde-hp $ */
/*
 * dtpdm/MainWindow.h
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
#ifndef _pdmMainWin_h
#define _pdmMainWin_h

#include "PdmXp.h"

/*
 * return codes for a PdmBoxVerifyAttrProc
 */
typedef enum { PDM_SUCCESS, PDM_FAILURE } PdmStatus;

/*
 * callback procedure types for setup box children of the PDM main window
 */
typedef struct _PdmSetupBox* (*PdmBoxNewProc) (void);
typedef void (*PdmBoxDeleteProc)(struct _PdmSetupBox* me);
typedef void (*PdmBoxCreateProc)(struct _PdmSetupBox* me, Widget parent);
typedef PdmStatus (*PdmBoxVerifyAttrProc)(struct _PdmSetupBox* me,
					  PdmXp* pdm_xp);
typedef void (*PdmBoxGetAttrProc)(struct _PdmSetupBox* me, PdmXp* pdm_xp);
typedef void (*PdmBoxSetAttrProc)(struct _PdmSetupBox* me, PdmXp* pdm_xp);

/*
 * instance structure for setup box children of the PDM main window
 */
typedef struct _PdmSetupBox
{
    /*
     * define common setup box functions
     */
    PdmBoxDeleteProc delete_proc;
    PdmBoxCreateProc create_proc;
    PdmBoxVerifyAttrProc verify_attr_proc;
    PdmBoxGetAttrProc get_attr_proc;
    PdmBoxSetAttrProc set_attr_proc;
    /*
     * the setup box child's widget ID
     */
    Widget widget;
    /*
     * the setup box provides its own fallback resources and the
     * notebook tab widget instance name
     */
    const String* fallback_resources;
    int fallback_resources_count;
    const char* tab_name;
    /*
     * allow child setup box-specific data
     */
    void* subclass_data;
    
} PdmSetupBox;

/*
 * node structure for the list of setup box children managed by the PDM
 * main window
 */
typedef struct _PdmBoxNode
{
    PdmSetupBox* box;
    Widget tab;
    struct _PdmBoxNode* next;
} PdmBoxNode, *PdmBoxList;

/*
 * PDM main window instance structure
 */
typedef struct _PdmMainWin
{
    /*
     * widget IDs
     */
    Widget widget;
    Widget notebook;
    /*
     * printer information
     */
    char* print_display_spec;
    
    const char* printer_descriptor;
    const char* printer_name;
    /*
     * linked list of setup boxes
     */
    PdmBoxList box_list_head;
    PdmBoxList box_list_tail;
    /*
     * Xp interface
     */
    PdmXp* pdm_xp;
    /*
     * online help
     */
    Widget help_dialog;

} PdmMainWin;

/*
 * public PdmMainWin methods
 */
PdmMainWin* PdmMainWinNew(void);
PdmMainWin* PdmMainWinCreate(
			     PdmMainWin* me,
			     Widget parent,
			     String print_display_spec,
			     String print_context_str);
void PdmMainWinDelete(PdmMainWin* me);
void PdmMainWinAddSetupBox(PdmMainWin* me, PdmSetupBox* box);
String* PdmMainWinMergeFallbacks(PdmMainWin* me,
				 const String* app_fallbacks,
				 int count);

#endif /* _pdmMainWin_h */


