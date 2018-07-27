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
/* fssxeg.h 1.5 - Fujitsu source for CDEnext    96/07/18 13:19:30      */
/* $XConsortium: fssxeg.h /main/4 1996/07/19 20:32:54 cde-fuj $ */
/*
 *  (c) Copyright 1995 FUJITSU LIMITED
 *  This is source code modified by FUJITSU LIMITED under the Joint
 *  Development Agreement for the CDEnext PST.
 *  This is unpublished proprietary source code of FUJITSU LIMITED
 */



#include "udccom.h"	/* 1995.10.09 */
#include "FaLib.h"

#define	ON			1
#define	OFF			0

#define	MAXPTNSIZE	128
#define	MAXPTNBYTE	MAXPTNSIZE*((MAXPTNSIZE+7)/8)

#define ERROR_VAL	fal_utyerror
#define EXISTS_FLAG	fal_utyexists

#define	EDPANE_SIZE	400
#define	EDLIST_MAX	30
