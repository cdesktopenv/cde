/*
 * $XConsortium: const.h /main/4 1996/06/11 17:36:31 cde-hal $
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



#ifndef _const_h
#define _const_h 1

#define KB 1024
#define PATHSIZ 256

#define NAMESIZ 50

#ifndef LBUFSIZ
#define LBUFSIZ (8*KB)
#endif

#define PAGSIZ LBUFSIZ 

#ifndef CACHE_PAGES 
#define CACHE_PAGES 500
#endif

#ifndef TOTAL_MEM_PAGES
#define TOTAL_MEM_PAGES 4000
#endif

#define LIST_MARK_CHAR '#' // LIST_MARK_CHAR encloses
                           // short_list elements. Used in
                           // short_list::asciiIn(), and
                           // dl_list::asciiIn().

#define CommentChar '#'


#endif

