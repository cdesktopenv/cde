/*
 * $XConsortium: utility.cc /main/4 1996/06/11 17:11:55 cde-hal $
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


#include "api/utility.h"
#include "unique_id.h"

typedef char* charPtr;

int load( info_base* base, char* container_nm )
{
   return _load( base, container_nm, cin );
}

int load( info_base* base, char* container_nm, char* data_path )
{
   fstream in(data_path, ios::in);
   return _load( base, container_nm, in );
}

int load( info_base* base, char** argv, int argc )
{
   int ok = 0;
   char* container_nm, *data_path;
   for ( int i=0; i<argc; i += 2 ) {
      container_nm   = argv[i];
      data_path = argv[i+1];
      ok |= load( base, container_nm, data_path );
   }
   return ok;
}

int _load( info_base* base, char* container_nm, istream& in )
{
   handler* x = (base -> get_obj_dict()).get_handler(container_nm);

   if ( x ) {

      (*(cset_handler*)x) -> asciiIn(in);

      return 0;

   } else
      return -1;
}

