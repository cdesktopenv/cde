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
/* selectxlfd.h 1.1 - Fujitsu source for CDEnext    96/01/06 16:57:23      */
/* $XConsortium: selectxlfd.h /main/3 1996/04/08 16:02:27 cde-fuj $ */
/*
 *  (c) Copyright 1995 FUJITSU LIMITED
 *  This is source code modified by FUJITSU LIMITED under the Joint
 *  Development Agreement for the CDEnext PST.
 *  This is unpublished proprietary source code of FUJITSU LIMITED
 */



#define BUTTONITEMS     4
#define PUSHBUTTONS     2
#define COPYLABELS	2

#define CX		12
#define CY		-4

#define XLFD_COLUMNS	38
#define KEY_COLUMNS	15

/*
 *	put data from resource database
 */
typedef struct _Rsrc {
	char	* xlfd_label ;
	char	* copy_xlfd_label ;
	char	* code_label ;
	char	* style_label ;
	char	* size_label ;
	char	* exec_label ;
	char	* quit_label ;
	char	* ok_label ;
	char	* cancel_label ;
	char	* copy_orgin ;
	char	* copy_target ;
	char	* copy_label ;
	char	* overlay_label ;
} Rsrc ;
/***************< end of selectxlfd.h >***************/
