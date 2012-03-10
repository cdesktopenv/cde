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
#if NeedFunctionPrototypes
FileLock( int fd )
#else
FileLock( fd )
int 	fd;		/* a file descripter */
#endif
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
#if NeedFunctionPrototypes
FileUnLock( int fd )
#else
FileUnLock( fd )
int 	fd;	/* a file descripter */
#endif
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
#if NeedFunctionPrototypes
isLock( int fd )
#else
isLock( fd )
int 	fd;	/* file descripter */
#endif
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
