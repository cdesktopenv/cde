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
/* $XConsortium: icon_selection_dialog.h /main/4 1995/11/01 16:14:48 rswiston $ */

/*******************************************************************************
       icon_selection_dialog.h
       This header file is included by icon_selection_dialog.c

*******************************************************************************/

#ifndef _ICON_SELECTION_DIALOG_INCLUDED
#define _ICON_SELECTION_DIALOG_INCLUDED


#include <stdio.h>
#include <Xm/Xm.h>
#include <Xm/MwmUtil.h>
#include <Xm/DialogS.h>
#include <Xm/MenuShell.h>
#include "UxXt.h"

#include <Xm/List.h>
#include <Xm/ScrolledW.h>
#include <Xm/Label.h>
#include <Xm/TextF.h>
#include <Xm/SeparatoG.h>
#include <Xm/PushB.h>
#include <Xm/Form.h>
#include <Xm/Form.h>

/*******************************************************************************
       The definition of the context structure:
       If you create multiple copies of your interface, the context
       structure ensures that your callbacks use the variables for the
       correct copy.

       For each swidget in the interface, each argument to the Interface
       function, and each variable in the Interface Specific section of the
       Declarations Editor, there is an entry in the context structure.
       and a #define.  The #define makes the variable name refer to the
       corresponding entry in the context structure.
*******************************************************************************/

typedef struct
{
        Widget  Uxicon_selection_dialog;
        Widget  Uxbottom_button_form;
        Widget  Uxbottom_button1;
        Widget  Uxbottom_button2;
        Widget  Uxbottom_button3;
        Widget  Uxbottom_button4;
        Widget  UxseparatorGadget1;
        Widget  Uxicon_name_text_field;
        Widget  Uxicon_name_textfield_label;
        Widget  Uxfilter_textfield_label;
        Widget  Uxfilter_text_field;
        Widget  Uxdirectory_list_label;
        Widget  Uxicon_container_label;
        Widget  Uxtop_button_form;
        Widget  Uxtop_button1;
        Widget  Uxtop_button2;
        Widget  Uxtop_button3;
        Widget  Uxdir_scrolled_list;
        Widget  Uxicon_scrolled_win;
        Widget  Uxicon_scrolled_container;
        unsigned char   *Uxcont_title;
        swidget UxUxParent;
        unsigned char   *Uxdialog_title;
        unsigned char   *Uxfilter_field_title;
        int     Uxuse_filter_field;
        unsigned char   *Uxfile_filter;
        unsigned char   *Uxdirectory_title;
        unsigned char   **Uxdirectories_list;
        unsigned char   *Uxcontainer_title;
        int     UxnumberOfTopButtons;
        unsigned char   *Uxtop_button_one_label;
        void    (*Uxtop_button_one_cb)();
        unsigned char   *Uxtop_button_two_label;
        void    (*Uxtop_button_two_cb)();
        unsigned char   *Uxtop_button_three_label;
        void    (*Uxtop_button_three_cb)();
        int     Uxuse_icon_name_field;
        unsigned char   *Uxname_field_title;
        int     UxnumberOfBottomButtons;
        unsigned char   *Uxbottom_button_one_label;
        void    (*Uxbottom_button_one_cb)();
        unsigned char   *Uxbottom_button_two_label;
        void    (*Uxbottom_button_two_cb)();
        unsigned char   *Uxbottom_button_three_label;
        void    (*Uxbottom_button_three_cb)();
        unsigned char   *Uxbottom_button_four_label;
        void    (*Uxbottom_button_four_cb)();
} _UxCicon_selection_dialog;

#ifdef CONTEXT_MACRO_ACCESS
static _UxCicon_selection_dialog *UxIcon_selection_dialogContext;
#define icon_selection_dialog   UxIcon_selection_dialogContext->Uxicon_selection_dialog
#define bottom_button_form      UxIcon_selection_dialogContext->Uxbottom_button_form
#define bottom_button1          UxIcon_selection_dialogContext->Uxbottom_button1
#define bottom_button2          UxIcon_selection_dialogContext->Uxbottom_button2
#define bottom_button3          UxIcon_selection_dialogContext->Uxbottom_button3
#define bottom_button4          UxIcon_selection_dialogContext->Uxbottom_button4
#define separatorGadget1        UxIcon_selection_dialogContext->UxseparatorGadget1
#define icon_name_text_field    UxIcon_selection_dialogContext->Uxicon_name_text_field
#define icon_name_textfield_label UxIcon_selection_dialogContext->Uxicon_name_textfield_label
#define filter_textfield_label  UxIcon_selection_dialogContext->Uxfilter_textfield_label
#define filter_text_field       UxIcon_selection_dialogContext->Uxfilter_text_field
#define directory_list_label    UxIcon_selection_dialogContext->Uxdirectory_list_label
#define icon_container_label    UxIcon_selection_dialogContext->Uxicon_container_label
#define top_button_form         UxIcon_selection_dialogContext->Uxtop_button_form
#define top_button1             UxIcon_selection_dialogContext->Uxtop_button1
#define top_button2             UxIcon_selection_dialogContext->Uxtop_button2
#define top_button3             UxIcon_selection_dialogContext->Uxtop_button3
#define dir_scrolled_list       UxIcon_selection_dialogContext->Uxdir_scrolled_list
#define icon_scrolled_win       UxIcon_selection_dialogContext->Uxicon_scrolled_win
#define icon_scrolled_container      UxIcon_selection_dialogContext->Uxicon_scrolled_container
#define cont_title              UxIcon_selection_dialogContext->Uxcont_title
#define UxParent                UxIcon_selection_dialogContext->UxUxParent
#define dialog_title            UxIcon_selection_dialogContext->Uxdialog_title
#define filter_field_title      UxIcon_selection_dialogContext->Uxfilter_field_title
#define use_filter_field        UxIcon_selection_dialogContext->Uxuse_filter_field
#define file_filter             UxIcon_selection_dialogContext->Uxfile_filter
#define directory_title         UxIcon_selection_dialogContext->Uxdirectory_title
#define directories_list        UxIcon_selection_dialogContext->Uxdirectories_list
#define container_title         UxIcon_selection_dialogContext->Uxcontainer_title
#define numberOfTopButtons      UxIcon_selection_dialogContext->UxnumberOfTopButtons
#define top_button_one_label    UxIcon_selection_dialogContext->Uxtop_button_one_label
#define top_button_one_cb       UxIcon_selection_dialogContext->Uxtop_button_one_cb
#define top_button_two_label    UxIcon_selection_dialogContext->Uxtop_button_two_label
#define top_button_two_cb       UxIcon_selection_dialogContext->Uxtop_button_two_cb
#define top_button_three_label  UxIcon_selection_dialogContext->Uxtop_button_three_label
#define top_button_three_cb     UxIcon_selection_dialogContext->Uxtop_button_three_cb
#define use_icon_name_field  UxIcon_selection_dialogContext->Uxuse_icon_name_field
#define name_field_title        UxIcon_selection_dialogContext->Uxname_field_title
#define numberOfBottomButtons   UxIcon_selection_dialogContext->UxnumberOfBottomButtons
#define bottom_button_one_label UxIcon_selection_dialogContext->Uxbottom_button_one_label
#define bottom_button_one_cb    UxIcon_selection_dialogContext->Uxbottom_button_one_cb
#define bottom_button_two_label UxIcon_selection_dialogContext->Uxbottom_button_two_label
#define bottom_button_two_cb    UxIcon_selection_dialogContext->Uxbottom_button_two_cb
#define bottom_button_three_label UxIcon_selection_dialogContext->Uxbottom_button_three_label
#define bottom_button_three_cb  UxIcon_selection_dialogContext->Uxbottom_button_three_cb
#define bottom_button_four_label UxIcon_selection_dialogContext->Uxbottom_button_four_label
#define bottom_button_four_cb   UxIcon_selection_dialogContext->Uxbottom_button_four_cb

#endif /* CONTEXT_MACRO_ACCESS */

/*******************************************************************************
       Declarations of global functions.
*******************************************************************************/
Widget  create_icon_selection_dialog(swidget,
                                     unsigned char *,
                                     unsigned char *,
                                     int,
                                     unsigned char *,
                                     unsigned char *,
                                     unsigned char **,
                                     unsigned char *,
                                     int,
                                     unsigned char *,
                                     void (*)(),
                                     unsigned char *,
                                     void (*)(),
                                     unsigned char *,
                                     void (*)(),
                                     int,
                                     unsigned char *,
                                     int,
                                     unsigned char *,
                                     void (*)(),
                                     unsigned char *,
                                     void (*)(),
                                     unsigned char *,
                                     void (*)(),
                                     unsigned char *,
                                     void (*)()
                                    );

#endif  /* _ICON_SELECTION_DIALOG_INCLUDED */

/*******************************************************************************
       Platform specifics
*******************************************************************************/
#ifdef SVR4
#define dirent_struct dirent_t
#else
#define dirent_struct dirent
#endif
