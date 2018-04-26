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
/* $XConsortium: mailtest.h /main/3 1995/11/06 16:33:59 rswiston $ */
/*
 * File: mailtest.h
 * Contains object data structures and callback declarations

 *
 * This file was generated from mailtest by dtcodegen
 *
 *    ** DO NOT MODIFY BY HAND - ALL MODIFICATIONS WILL BE LOST **
 *
 */
#ifndef _MAILTEST_H_
#define _MAILTEST_H_

#include <stdlib.h>
#include <X11/Intrinsic.h>

/*
 * Structure to store values for Application Resources
 */
typedef struct {
    char	*session_file;

/* vvv Add client code below vvv */
} DtbAppResourceRec;


extern DtbAppResourceRec	dtb_app_resource_rec;

#endif /* _MAILTEST_H_ */

