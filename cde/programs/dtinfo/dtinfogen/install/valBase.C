/*
 * CDE - Common Desktop Environment
 *
 * Copyright (c) 1993-2012, The Open Group. All rights reserved.
 *
 * These libraries and programs are free software; you can
 * redistribute them and/or modify them under the terms of the GNU
 * Lesser General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * These libraries and programs are distributed in the hope that
 * they will be useful, but WITHOUT ANY WARRANTY; without even the
 * implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with these librararies and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
/*
 * $XConsortium: valBase.C /main/5 1996/10/29 21:14:58 cde-hal $
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

#include <iostream>
using namespace std;
#include <assert.h>
#include <string.h>
#include <stdlib.h>

// MMDB header file
#include "oliasdb/mmdb.h"
#include "oliasdb/asciiIn_filters.h"

// DDK header file
#include "olGlobal.h"

// Debuggging macro
int dbgLevel;
#ifdef DEBUG
#define DBG(level) if ( dbgLevel >= level )
#else
#define DBG(level) if (0)
#endif

//------------------------------------------------
int dbgInit()
{
  char *dbgStr;

  dbgStr = getenv("OL_DEBUG");
  return ( dbgStr ? atoi ( dbgStr ) : 0 );
}

//------------------------------------------------
int main(int argc, char **argv)
{
  
  INIT_EXCEPTIONS();

  OLIAS_DB mmdb_handle;
  info_lib *InfoLib;

  char checkVersionStr [ 16 ];
  char installVersionStr [ 16 ];
  char locatorStr[64];
  
  dbgLevel = dbgInit();

  mtry {
    
   if ( argc < 2 ) {
     cerr << "Usage : valBase checkBase installBase\n";
     exit (1);
   }

   char *checkBaseStr = argv[1];
   char *installBaseStr = argv[2];
   
   DBG(10) cerr << "(DEBUG) checkBase = " << checkBaseStr << endl;
   DBG(10) cerr << "(DEBUG) installBase = " << installBaseStr << endl;

   // first construct the info_base ptr for installBase

   InfoLib = mmdb_handle.openInfoLib(getenv("MMDB_PATH"));
   assert ( InfoLib != NULL );

   info_base *installBase = InfoLib->get_info_base ( installBaseStr );
   if ( !installBase ) 
     throw(stringException("NULL infobase ptr for installation infobase"));

   // construct the info_base ptr for checkBase

   info_base *checkBase = InfoLib->get_info_base ( checkBaseStr );
   if ( !checkBase ) 
     throw(stringException("NULL infobase ptr for check infobase"));

   // check the data version number

   mm_version &checkVersion = checkBase->data_version();
   short major_version_check = checkVersion.major_version();
   short minor_version_check = checkVersion.minor_version();

   snprintf (checkVersionStr, sizeof(checkVersionStr),
			      "%d%d", major_version_check,
				      minor_version_check);
   
   int checkVersionNum = atoi ( checkVersionStr );

   DBG(10) cerr << "(DEBUG) checkBaseVersion = " << checkVersionNum << endl;
   
   mm_version &installVersion = installBase->data_version();
   short major_version_install = installVersion.major_version();
   short minor_version_install = installVersion.minor_version();

   snprintf (installVersionStr, sizeof(installVersionStr),
				"%d%d", major_version_install,
					minor_version_install);
   int installVersionNum = atoi ( installVersionStr );

   DBG(10) cerr << "(DEBUG) installVersionNum = " << installVersionNum << endl;

   // Now perform the version checking

   if ( installVersionNum == 10 ) {
     if ( checkVersionNum >= 11 ) {
       cerr << "(ERROR) Data version mismatch\n";
       cerr << "        " << checkBaseStr << " version = "
	    << major_version_check << "." << minor_version_check << endl;
       cerr << "        " << installBaseStr << " version = "
	    << major_version_install << "." << minor_version_install << endl;
       exit ( 1 );
     }
   }
   else if ( checkVersionNum == 10 ) {
     if ( installVersionNum >= 11 ) {
       cerr << "(ERROR) Data version mismatch\n";
       cerr << "        " << checkBaseStr << " version = "
	    << major_version_check << "." << minor_version_check << endl;
       cerr << "        " << installBaseStr << " version = "
	    << major_version_install << "." << minor_version_install << endl;
       exit ( 1 );
     }
   }

   // Now to check locators

   Iterator *it = checkBase->first(LOCATOR_SET_NAME, LOCATOR_CODE );

   int DupLocFound=0;
   
   while ( *it ) {

     locator_smart_ptr x(checkBase, checkBase->get_oid(*it));

     int len = MIN(strlen(x.inside_node_locator_str()), 64 - 1);
     *((char *) memcpy (locatorStr,
			x.inside_node_locator_str(), len) + len) = '\0';

     DBG(10) cerr << "(DEBUG) locatorStr = " << locatorStr
		  << endl;

     // check this locator value with the installation infobase
     locator_smart_ptr loc(installBase, locatorStr);

     if (! strcmp(loc.inside_node_locator_str(), locatorStr))
     {
	 cerr << "(ERROR) Duplicate locator [ " << locatorStr
	      << " ] found in " << installBaseStr << endl;
	 DupLocFound = 1;
     }
    
     checkBase->next(*it);
   }

   // clean up phase
   delete it;
   delete InfoLib;

   if ( DupLocFound ) {
     exit ( 1 );
   }

   exit (0);
 } 

 mcatch ( mmdbException &, e )
 {
   debug(cerr, e );
   abort();
 }
 end_try;

}
  
