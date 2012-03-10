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
