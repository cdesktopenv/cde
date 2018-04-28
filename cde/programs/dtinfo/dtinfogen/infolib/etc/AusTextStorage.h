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
 * License along with these libraries and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
/* $XConsortium: AusTextStorage.h /main/3 1996/07/18 16:39:21 drk $ */

#ifndef AUSTEXT_STOR_HDR
#define AUSTEXT_STOR_HDR

class DataRepository;

class AusTextStore {
private:
  FILE *afp;
  char *austext_path;
  static unsigned long f_recordcount;
  
public:

  AusTextStore( const char *BookCasePath, const char *BookCaseName ); 
  /*
   * insert ( "This Book Case Name", 2,
   *           "XmyLcfhalklkoop",
   * 	      "This is the text that the indexing machine will see",
   *	      51 );
   *
   */
  
  void insert( const char *BookShortTitle,
	       const char *BookID,
	       const char *SectionID,
	       const char *SectionTitle,
	       DataRepository *store
    );
  ~AusTextStore();
  
};

#endif

  
