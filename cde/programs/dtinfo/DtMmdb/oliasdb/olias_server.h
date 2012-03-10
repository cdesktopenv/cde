/*
 * $XConsortium: olias_server.h /main/3 1996/06/11 17:30:21 cde-hal $
 *
 * Copyright (c) 1992 HAL Computer Systems International, Ltd.
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


#ifndef _olias_server_h
#define _olias_server_h 1

#include "oliasdb/dlp_hd.h"
#include "oliasdb/doc_hd.h"
#include "oliasdb/graphic_hd.h"
#include "oliasdb/locator_hd.h"
#include "oliasdb/node_hd.h"
#include "oliasdb/toc_hd.h"
#include "oliasdb/stylesheet_hd.h"
#include "api/info_lib.h"
#include "api/info_base.h"
#include "api/server.h"

#include "oliasdb/user_base.h"
#include "oliasdb/mark.h"
#include "oliasdb/pref.h"


class olias_server : public server
{

public:

   enum TestSelector { LOC, GRA };
   olias_server(
                Boolean delayed_infolib_init = false,
                char* selected_base_name = 0,
                char* x_info_lib_path = getenv("MMDB_PATH")
               );
   ~olias_server();

  void init_infolib();

  info_base* get_infobase(const char *locator_string, enum TestSelector = LOC);
  info_base** get_infobases(char **locator_strings, int count, enum TestSelector = LOC);

protected:
   dlp      		dlp_template;
   doc      		doc_template;
   graphic  		graphic_template;
   olias_locator 	locator_template;
   olias_node       	node_template;
   toc      		toc_template;
   umark 		mark_template;
   pref 		pref_template;
   stylesheet		stylesheet_template;

};


#endif
