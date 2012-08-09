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
/* $XConsortium: ds_common.c /main/3 1995/11/01 12:39:41 rswiston $ */
/*                                                                      *
 *  ds_common.c                                                         *
 *   Contains some common functions used throughout the Desktop         *
 *   Calculator.                                                        *
 *                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company			*
 * (c) Copyright 1993, 1994 International Business Machines Corp.	*
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.			*
 * (c) Copyright 1993, 1994 Novell, Inc. 				*
 */

#include <stdio.h>
#include <stdlib.h>
#include "ds_common.h"

#define  FREE    (void) free
#define  STRCPY  (void) strcpy


void
read_str(str, value)
char **str, *value ;
{
  if (*str != NULL) FREE(*str) ;
  if (value != NULL && strlen(value))
    {
      *str = (char *) malloc((unsigned) (strlen(value) + 1)) ;
      STRCPY(*str, value) ;
    }
  else *str = NULL ;
}


char *
set_bool(value)
int value ;
{
  return((value) ? "true" : "false") ;
}
