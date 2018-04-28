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
/* $XConsortium: AppSpecific.h /main/4 1996/05/09 03:39:24 drk $ */
/*
 * constant definitions
 */
#define APP_CLASS "Dtprint"

/*
 * Application-specific "object"
 */
typedef struct _AppObject
{
    Widget main_window ;
    Widget widget;
    String file_name;
    char * file_buffer ;
    
} AppObject;

/*
 * public AppObject functions
 */
extern AppObject* AppObject_new(
				Widget parent,
				String file_name);
extern void AppObject_customizePrintSetupBox(
					     AppObject* me,
					     Widget print_dialog);
