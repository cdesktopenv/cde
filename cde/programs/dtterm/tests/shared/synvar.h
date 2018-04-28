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
/* $XConsortium: synvar.h /main/3 1995/10/31 12:01:44 rswiston $ */

/******************************************************************
 *
 * (c) Copyright Hewlett-Packard Company, 1993.
 *
 ******************************************************************/

#include   <stdio.h>
#include   <X11/Xlib.h>
#include   <synlib/synlib.h>

typedef struct {
    Display       *display;
    int            waitTime;
    SynFocus      *focusMap;
} TestData, *TestDataPtr;


#define WIN_WAIT_TIME       120
#define MULTI_CLICK_DELAY   5

#define IMAGE_DIR           "../image/"
#define EXPECTED_SUFFIX     ".exp"
#define ACTUAL_SUFFIX       ".act"

#define NEED_LEN            256

#define IMAGE_FILE_LEN      100  /* should be strlen(IMAGE_DIR) + 20 (say) */


#define TERM_EMU            "dtterm"
