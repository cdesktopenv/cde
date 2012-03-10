/*  $XConsortium: svc_init.c /main/3 1996/07/09 14:47:28 drk $ */ 

#include <sys/uswitch.h>
#include <errno.h>

/*
 * Activate System V Null pointer treatment.  This allows dereferences
 * of NULL pointers in the CDE executables to work.
 *
 * This procedure is obsolete.  Dereferencing a NULL pointer is
 * an error, and such code should be repaired.
 */

svc_init()
{
    int usw_val;

    usw_val=uswitch(USC_GET,0);
    if ((uswitch(USC_SET, usw_val | USW_NULLP)) == 01) {
	perror("svc_init");
	return(-1);
    }
    return(0);
}
