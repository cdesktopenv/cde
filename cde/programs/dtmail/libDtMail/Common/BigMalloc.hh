/*
 *+SNOTICE
 *
 *
 *	$XConsortium: BigMalloc.hh /main/4 1996/04/21 19:47:07 drk $
 *
 *	RESTRICTED CONFIDENTIAL INFORMATION:
 *	
 *	The information in this document is subject to special
 *	restrictions in a confidential disclosure agreement bertween
 *	HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
 *	document outside HP, IBM, Sun, USL, SCO, or Univel wihtout
 *	Sun's specific written approval.  This documment and all copies
 *	and derivative works thereof must be returned or destroyed at
 *	Sun's request.
 *
 *	Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 *
 *+ENOTICE
 */

#ifndef _BIGMALLOC_H
#define _BIGMALLOC_H

#include <DtMail/DtLanguages.hh>

class BigMalloc : public DtCPlusPlusAllocator {
  public:
    BigMalloc(unsigned int size);
    ~BigMalloc(void);

    void Realloc(unsigned int new_size);

    operator char*(void);

  private:
    void *		_buffer;
    size_t	_size;
    int			_fd;
};

#endif
