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
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */
/****************************<+>*************************************
 **
 **   File:     EnvControlP.h
 **
 **   RCS:	$TOG: EnvControlP.h /main/6 1998/07/30 12:13:45 mgreess $
 **   Project:  DT Runtime Library
 **
 **   Description: Defines structures, and parameters used
 **                for communication with the environment
 **
 **   (c) Copyright 1990 by Hewlett-Packard Company
 **
 ****************************<+>*************************************/

/********    Public Function Declarations    ********/
#ifdef __cplusplus
extern "C" {
#endif
extern int _DtEnvControl(
                        int mode) ;
extern int _DtWsmSetBackdropSearchPath(
                        Screen *screen,
                        char   *backdropDir,
                        Boolean useMultiColorIcons) ;
/********    End Public Function Declarations    ********/

/**********************************************************************
 * Command parameters to the function, which double as result codes.
 * If the invocation is successful, the same is returned; 
 * else DT_ENV_NO_OP is returned.

DT_ENV_SET
	Sets the DT environment.

DT_ENV_RESTORE_PRE_DT
	Restores the pre-DT application environment

DT_ENV_RESTORE_POST_DT
	Reinstalls the DT environment after a restoring pre-DT
	environment

DT_ENV_NO_OP
	Does nothing

DT_ENV_SET_BIN
	Sets the DT environment PLUS sets the PATH= variable to
	where the DT files live.

 **********************************************************************/
#define DT_ENV_SET			0
#define DT_ENV_RESTORE_PRE_DT		1
#define DT_ENV_RESTORE_POST_DT	2
#define DT_ENV_NO_OP			3
#define DT_ENV_SET_BIN			4

#ifdef __cplusplus
}
#endif
/****************************        eof       **********************/
