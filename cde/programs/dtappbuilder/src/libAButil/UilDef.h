/* $XConsortium: UilDef.h /main/2 1995/07/15 22:31:34 drk $ */
#ifndef _AB_UILDEF_H_
#define _AB_UILDEF_H_
/*
 * UilDef.h - include standard header files, adding symbols for AB
 * 		files.
 */

#ifndef _POSIX_SOURCE
#define _POSIX_SOURCE 1		/* we want to be POSIX-compliant */
#endif

#include <uil/UilDef.h>

/*
 * Make sure we don't redefine types defined in standard X include files
 */
#define _AB_BOOL_DEFINED_
#define _AB_BYTE_DEFINED_

#endif /* _AB_UILDEF_H_ */
