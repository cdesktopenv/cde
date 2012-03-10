/*
 *   COMPONENT_NAME: austext
 *
 *   FUNCTIONS: none
 *
 *   ORIGINS: 27,157
 *
 *   This module contains IBM CONFIDENTIAL code. -- (IBM
 *   Confidential Restricted when combined with the aggregated
 *   modules for this product)
 *
 *   OBJECT CODE ONLY SOURCE MATERIALS
 *   (C) COPYRIGHT International Business Machines Corp. 1995, 1996
 *   All Rights Reserved
 *   US Government Users Restricted Rights - Use, duplication or
 *   disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 */
/* $XConsortium: oflag.c /main/3 1996/05/07 13:46:59 drk $
 * Author: Mike Russell, October 1995.
 * Global db_oflag replaces hardcoded O_RDWR in calls
 * to open_b() in runtime and open_u() in utilities
 * to permit read-only database opens.
 * Placed in its own module because there are no common
 * modules or headers in both runtime and utility source sets.
 * This module is duplicated exactly in both directories.
 *
 * $Log$
 * Revision 1.1  1995/10/17  19:57:02  miker
 * Initial revision
 *
 */
#include <fcntl.h>

int     db_oflag =     O_RDWR;

