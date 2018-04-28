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
/*
 *      $XConsortium: objxmP.h /main/3 1995/11/06 18:44:17 rswiston $
 *
 * @(#)objxmP.h	1.21 15 Feb 1994      cde_app_builder/src/libABobjXm
 *
 *      RESTRICTED CONFIDENTIAL INFORMATION:
 *
 *      The information in this document is subject to special
 *      restrictions in a confidential disclosure agreement between
 *      HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
 *      document outside HP, IBM, Sun, USL, SCO, or Univel without
 *      Sun's specific written approval.  This document and all copies
 *      and derivative works thereof must be returned or destroyed at
 *      Sun's request.
 *
 *      Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 *
 */

/*
 * objxmP.h - Implements private component of objxm module 
 *
 */
#ifndef _OBJXM_P_H_
#define _OBJXM_P_H_

#include <X11/Intrinsic.h>
#include <nl_types.h>
#include <ab_private/obj_notify.h>
#include <ab_private/objxm.h>

/* 
** I18N defines: Which message set are our messages in?  This must be in sync
** with the 'set N' definition in the libABobjXm.msg message source text file.
** 
** And what global variable are we expecting as our opened catalog descriptor?
*/
#define OBJXM_MESSAGE_CATD		Dtb_project_catd
#define OBJXM_MESSAGE_SET		203
extern nl_catd		OBJXM_MESSAGE_CATD;

/*
 * Objxm Enum structure
 */
typedef struct _OBJXM_ENUM{
    unsigned char    xmdef; /* i.e. XmWORK_AREA    */
    ISTRING        strdef;/* i.e. "XmWORK_AREA"  */
} ObjxmEnum;

/*
 * EnumTable structure
 */
typedef struct _OBJXM_ENUM_TABLE {
    ObjxmEnum	*enums;
    int		count;
} ObjxmEnumTable;

/*
 * Function type for calculating Resource Default value
 * based on the associated WidgetClass
 */
typedef XtArgVal (*ObjxmResDefaultFunc) (
   WidgetClass 
);

/*
 * Objxm Resource structure
 */
typedef struct _OBJXM_RES {
    ISTRING	   xmname;   		/* e.g. XmNfoo ("foo") */
    ISTRING        strname;  		/* e.g. "XmNfoo" ISTRING*/
    AB_ARG_TYPE    type;     		/* e.g. AB_ARG_STRING   */
    AB_ARG_CLASS   arg_class;		/* e.g. AB_ARG_CLASS_COLOR */
    ObjxmEnumTable *enum_table; 	/* points to table of Enum values */
    XtArgVal	   default_val;     	/* default value for resource */
    ObjxmResDefaultFunc default_func; 	/* default value calculation function */
} ObjxmRes;

typedef    struct _OBJXM_CREATE_FUNC {
    XmCreateFunc    ftn;    /* i.e. XmCreateFrame  */
    ISTRING        ftnstr; /* i.e. "XmCreateFrame" */
} ObjxmCreateFunc;

/*
 * Structure to store ABObj & XtTranslations for a widget
 */
typedef	   struct _OBJXM_WIDGET_INFO {
    ABObj	   obj;
    XtTranslations translations;
} ObjxmWidgetInfo;


/*
 * Declare module-private functions
 */

		/* Called when an object is being
		 * destroyed
		 */
extern int	objxmP_destroy(
		    ObjEvDestroyInfo	info
		);

                /* Function which returns XmConfigInfo
                 * ptr for an object
                 */
extern XmConfigInfo
                objxmP_get_config_info(
                    ABObj        obj
                );

                /* Create a SubObj directly under the root and 
                 * move the root's children under the new SubObj 
                 */
extern ABObj    objxmP_create_direct_subobj( 
                    ABObj       root, 
                    AB_OBJECT_TYPE type, 
                    STRING      subname
                ); 

		/* Function which re-configures attachments after
 		 * objects have been xmconfigured
 		 */
extern int      objxmP_obj_config_attachments(
                    ABObj       obj
                );

extern int	objxmP_obj_set_attachment_arg(
		    ABObj       obj,
		    OBJXM_CONFIG_TYPE ctype,
		    ABObj       parent_obj,
		    AB_COMPASS_POINT dir
		);

extern int	objxmP_comp_set_type_args(
		    ABObj	obj,
		    OBJXM_CONFIG_TYPE ctype
		);

extern int	objxmP_comp_type_post_instantiate(
		    ABObj	obj,
		    OBJXM_CONFIG_TYPE ctype
		);

extern int	objxmP_merge_arg(
    		    ArgList *p_arglist,
    		    Arg     *arg
		);

extern ArgList	objxmP_merge_arglists(
    		    ArgList args1,
    		    int num_args1,
    		    ArgList args2,
    		    int num_args2
		);

extern Widget				ObjxmP_toplevel;
extern int				ObjxmP_visual_depth;
extern ObjxmFilenameToPixmapFunc	ObjxmP_filename_to_pixmap_cb;
extern ObjxmPostInstantiateFunc		ObjxmP_post_instantiate_cb;

#endif /* _OBJXM_P_H_ */

