/*
 * $XConsortium: atomic_lock.C /main/4 1996/11/01 10:19:09 drk $
 *
 * Copyright (c) 1993 HAL Computer Systems International, Ltd.
 * All rights reserved.  Unpublished -- rights reserved under
 * the Copyright Laws of the United States.  USE OF A COPYRIGHT
 * NOTICE IS PRECAUTIONARY ONLY AND DOES NOT IMPLY PUBLICATION
 * OR DISCLOSURE.
 * 
 * THIS SOFTWARE CONTAINS CONFIDENTIAL INFORMATION AND TRADE
 * SECRETS OF HAL COMPUTER SYSTEMS INTERNATIONAL, LTD.  USE,
 * DISCLOSURE, OR REPRODUCTION IS PROHIBITED WITHOUT THE
 * PRIOR EXPRESS WRITTEN PERMISSION OF HAL COMPUTER SYSTEMS
 * INTERNATIONAL, LTD.
 * 
 *                         RESTRICTED RIGHTS LEGEND
 * Use, duplication, or disclosure by the Government is subject
 * to the restrictions as set forth in subparagraph (c)(l)(ii)
 * of the Rights in Technical Data and Computer Software clause
 * at DFARS 252.227-7013.
 *
 *          HAL COMPUTER SYSTEMS INTERNATIONAL, LTD.
 *                  1315 Dell Avenue
 *                  Campbell, CA  95008
 * 
 */


#include "atomic_lock.h"

#include <X11/Xosdefs.h>
#include <errno.h>
#ifdef X_NOT_STDC_ENV
extern int errno;
#endif

atomic_lock::atomic_lock(const char* p)
{
   strcpy(v_path, p);
}

atomic_lock::~atomic_lock()
{
}

/////////////////////////////////////////
// from Rochkind's Adv. UNIX programming
/////////////////////////////////////////
Boolean atomic_lock::lock()
{
   int fd, tries;

   tries = 0;

   while ( (fd = creat(v_path, 0)) == -1 && errno == EACCES ) {
      if ( ++tries >= MAXTRIES )
         break;
      sleep (NAPTIME);
   }
   if ( fd == -1 || close(fd) == -1 ) {
      perror("lock");
      return false;
   } else
      return true;
}

Boolean atomic_lock::unlock()
{
   if ( unlink(v_path) == -1 ) {
      MESSAGE(cerr, "unlock(): unlink() failed");
      throw(systemException(errno));
   }

   return true;
}

