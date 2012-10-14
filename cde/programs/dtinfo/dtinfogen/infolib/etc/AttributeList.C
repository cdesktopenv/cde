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
/* $XConsortium: AttributeList.C /main/3 1996/08/21 15:45:57 drk $
 *
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1996 Hewlett-Packard Company.
 * (c) Copyright 1996 International Business Machines Corp.
 * (c) Copyright 1996 Sun Microsystems, Inc.
 * (c) Copyright 1996 Novell, Inc. 
 * (c) Copyright 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 */

 #include <stdio.h>
 #include <string.h>
 #include <stdlib.h>
 #include <iostream>
using namespace std;
 #include <assert.h>

 #include "AttributeRec.h"
 #include "AttributeList.h"

 // Debugging macro
 #ifdef DEBUG
 #define DBG(level) if ( dbgLevel >= level)
 #else
 #define DBG(level) if (0)
 #endif

 static int dbgLevel;

 //---------------------------------------------------------
 // ValList : Constructor

 OL_AttributeList::OL_AttributeList()
 {

   DBG(10) cerr << "(DEBUG) Invoking OL_AttributeList::OL_AttributeList()\n";

   dbgLevel=-1;
   char *dbgStr;

   head = NULL;
   tail = NULL;

   dbgStr = getenv("OL_DEBUG");
   dbgLevel = ( dbgStr ? atoi(dbgStr):0);

 }

 //---------------------------------------------------------
 // OL_AttributeList::OL_AttributeList

 OL_AttributeList::~OL_AttributeList()
 {
   AttributeRec *pt = head;

   while ( pt ) {
     AttributeRec *tmp = pt;
     pt = pt->next;
     delete tmp;
   }
 }

 //---------------------------------------------------------
 // OL_AttributeList:lookup
 AttributeRec *OL_AttributeList::lookup( int attName ) const
 {

   DBG(10) cerr << "(DEBUG) Invoking OL_AttributeList::lookup()\n";
   DBG(10) cerr << "        with attName = " << attName << endl;

   AttributeRec *pt = head;

   while ( pt ) {

     if ( pt->attName == attName ) {
       return ( pt );
     }

     pt = pt->next;
   }

   return ( NULL );

 }

 //---------------------------------------------------------
 // OL_AttributeList::add
 void OL_AttributeList::insert ( AttributeRec *entry )
 {

   if ( !tail ) {
     head = tail = entry;
   }
   else {
     tail->next = entry;
     tail       = entry;
   }

 }

 //---------------------------------------------------------
 const AttributeRec *OL_AttributeList::GetFirstAttr() const
 {
   return( head );
 }

 //---------------------------------------------------------
 const AttributeRec *
 OL_AttributeList::GetNextAttr( const AttributeRec *attRec) const
 {
  assert( attRec != NULL );
  return( attRec->next );
}

  
