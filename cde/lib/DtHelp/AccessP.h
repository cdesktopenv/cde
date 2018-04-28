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
/* $XConsortium: AccessP.h /main/5 1995/12/18 16:30:01 cde-hp $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        AccessP.h
 **
 **   Project:     Run Time Project File Access
 **
 **  
 **   Description: Private header file for Access.h
 **
 **  (c) Copyright 1987, 1988, 1989, 1990, 1991, 1992 Hewlett-Packard Company
 **
 **  (c) Copyright 1993, 1994 Hewlett-Packard Company
 **  (c) Copyright 1993, 1994 International Business Machines Corp.
 **  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 **  (c) Copyright 1993, 1994 Novell, Inc.
 **
 **
 ****************************************************************************
 ************************************<+>*************************************/
#ifndef _DtAccessP_h
#define _DtAccessP_h

#include <sys/stat.h>
typedef	void*	SdlVolumeHandle;
typedef	void*	CcdfVolumeHandle;

typedef	union _dthelpVolumes {
	SdlVolumeHandle	sdl_vol;
	CcdfVolumeHandle ccdf_vol;
} DtHelpVols;

/*
 * The following structure holds loaded volumes.  The fields of this 
 * structure should not be accessed by any code outside of the volume
 * module.
 */
struct _DtHelpVolumeRec {
    short	sdl_flag;	/* The type of volume */
    char *volFile;		/* The name of the volume file in the */
				/* form it was passed to _DtVolumeOpen. */

    char **keywords;		/* A pointer to a string array */
				/* containing all of the keywords in */
				/* sorted order.  This field is not loaded */
				/* until it is needed. */

    char ***keywordTopics;	/* A pointer to an array of string */
				/* arrays.  Each string array specifies */
				/* the list of topics which contain the */
				/* corresponding keyword.  This field is  */
				/* not loaded until it is needed. */

    DtHelpVols vols;		/* Handles to format specific volume info */
    int openCount;		/* A count of the number of times this */
				/* volume has been opened. */

    time_t check_time;		/* Time this volume was last modified */
    struct _DtHelpVolumeRec *nextVol;
				/* A pointer to the next volume, used to */
				/* chain all of the open volumes together. */
};

typedef struct _DtHelpVolumeRec *_DtHelpVolume;

#endif /* _DtAccessP_h */
