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
/* $XConsortium: lock.c /main/5 1996/11/08 02:07:28 cde-fuj $ */
/*
 *  (c) Copyright 1995 FUJITSU LIMITED
 *  This is source code modified by FUJITSU LIMITED under the Joint
 *  Development Agreement for the CDEnext PST.
 *  This is unpublished proprietary source code of FUJITSU LIMITED
 */


#include	<stdio.h>
#include	<errno.h>
#include	<sys/types.h>
#include	<fcntl.h>
#include	<unistd.h>
#include 	"udcutil.h"

/*
 * FileLock( fd )
 * fd:	file descripter
 *
 * lock a file by a writing mode
 *
 * normal end : 0
 * abnormal end : -1
 *
 */

int
FileLock( int fd )
{

	struct flock	flpar;


	flpar.l_type = F_RDLCK;
	flpar.l_start = 0;
	flpar.l_len = 0;
	flpar.l_whence = 0;

	if ( fcntl( fd, F_SETLK, &flpar ) == -1 ) {
		USAGE("Write lock not success \n" );
		return	-1;
	}

	return	0;
}


/*
 * FileUnLock( fd )
 *
 * free a file by a writing mode
 *
 * normal end : 0
 * abnormal end : -1
 */

int
FileUnLock( int fd )
{
	struct flock	flpar;


	flpar.l_type = F_UNLCK;
	flpar.l_start = 0;
	flpar.l_len = 0;
	flpar.l_whence = 0;

	if ( fcntl( fd, F_SETLK, &flpar ) == -1 ){
		USAGE("File unlock not success \n" );
		return	-1;
	}
	return	0;
}

/*
 * isLock( fd )
 *
 * search whether file is locked by a writing mode
 *
 * 1 : file is locked by a writing mode
 * 0 : file isn't locked by a writing mode
 */

int
isLock( int fd )
{
	struct flock	flpar;


	flpar.l_type = F_WRLCK;
	flpar.l_start = 0;
	flpar.l_len = 0;
	flpar.l_whence = 0;

	if ( fcntl( fd, F_GETLK, &flpar ) == -1 ) {
		USAGE("Inquiry of file lock not sucess \n" );
		return	-1;
	}

	if ( flpar.l_type == F_UNLCK ){
		return	0 ;
	} else {
		return	1 ;
	}
}
