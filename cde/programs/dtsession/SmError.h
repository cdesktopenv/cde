/* $TOG: SmError.h /main/6 1998/10/26 17:20:54 mgreess $ */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        SmError.h
 **
 **   Project:     HP DT
 **
 **   Description
 **   -----------
 **   Variables and definitions needed for
 **   Error Handling for the session manager
 **
 **
 **  (c) Copyright Hewlett-Packard Company, 1990.  
 **
 **
 **
 ****************************************************************************
 ************************************<+>*************************************/
#ifndef _smerror_h
#define _smerror_h
 
/* 
 *  #include statements 
 */


/* 
 *  #define statements 
 */


/*
 *  Error messages trapped by X window calls - These are never displayed
 */
#define		BAD_ACCESS		"Attempt to access unavailable resource"
#define		BAD_ATOM		"Atom for argument is not a defined atom"
#define		BAD_DRAWABLE	"Drawable argument is invalid"
#define		BAD_MATCH		"Drawable is a invalid match for operation"
#define		BAD_VALUE		"Value in X call is invalid"
#define		BAD_WINDOW		"Window does not exist"
#define		DEFAULT_ERROR	"Non-fatal error from X server"


/* 
 * typedef statements 
 */

/*
 * All NLS error messages
 */
typedef struct _NlsStrings
{
    char	*cantLockErrorString;
    char	*trustedSystemErrorString;
    char	*cantMallocErrorString;
    char	*cantOpenFileString;
    char	*cantForkClientString;
    char	*cantExecClientString;
    char	*cantCreateDirsString;
} NlsStrings;

/*
 *  External variables  
 */
extern NlsStrings	smNLS;


/*  
 *  External Interface  
 */

extern void InitErrorHandler( void ) ;
extern void PrintError( DtSeverity , char *) ;
extern void PrintErrnoError( DtSeverity , char *) ;
#ifndef  NO_MESSAGE_CATALOG
char * GetMessage( int, int, char * );
#endif


#endif /*_smerror_h*/
/* DON'T ADD ANYTHING AFTER THIS #endif */
