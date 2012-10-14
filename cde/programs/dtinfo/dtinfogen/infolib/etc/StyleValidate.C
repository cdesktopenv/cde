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
/* $XConsortium: StyleValidate.cc /main/2 1996/07/18 16:18:13 drk $ */
#include <stdio.h>
#include <iostream>
#include <sstream>
using namespace std;
#include <unistd.h>
#include <sys/wait.h>

#include "DataBase.h"
#include "StyleValidate.h"
#include "api/utility.h"

//---------------------------------------------------------------------
int 
validate_stylesheet( const char *buf, int buf_size, enum RENDERER_ENGINE_T t )
{

  char *styleFile = form( "/tmp/style_sheet.%d", getpid() );
  
  FILE *fp = fopen( styleFile, "w" );
  if ( !fp ) {
    throw( PosixError(1, "Unable to open style_sheet\n") );
  }

  fwrite( (char *)buf, buf_size, 1, fp );
  fclose( fp );

  char* renderer = 0;

  switch ( t ) {
    case ONLINE:
     renderer = (char*)"online";
     break;
    case PRINT:
     renderer = (char*)"hardcopy";
     break;
    default:
     throw( PosixError(1, "Unknown renderer engine\n") );
  }

  char *cmd = form("validator %s %s", renderer, styleFile);
  int status = system(cmd);
  int exit_status = WEXITSTATUS(status);

  unlink( styleFile );

  return (exit_status);
  
}

